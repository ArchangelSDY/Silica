#include <qglobal.h>

#ifdef Q_OS_UNIX
#include <sys/types.h>
#include <time.h>
#include <utime.h>
#endif

#include <quazipfile.h>

#include "GlobalConfig.h"
#include "LocalDatabase.h"
#include "Image.h"
#include "ImageRank.h"
#include "ImageSourceManager.h"

static const int THUMBNAIL_MIN_HEIGHT = 480;
static const int THUMBNAIL_SCALE_RATIO = 8;

// ---------- Load Image Task ----------
class LoadImageTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadImageTask(QSharedPointer<ImageSource> imageSource) :
        QRunnable() ,
        m_imageSource(imageSource) {}

    void run();

signals:
    void loaded(QList<QImage *> image, QList<int> durations);

private:
    QSharedPointer<ImageSource> m_imageSource;
};

void LoadImageTask::run()
{
    QList<QImage *> images;
    QList<int> durations;
    if (!m_imageSource.isNull() && m_imageSource->open()) {
        QImageReader reader(m_imageSource->device());

        forever {
            QImage image = reader.read();
            if (!image.isNull()) {
                images << new QImage(image);
                durations << reader.nextImageDelay();
            } else {
                break;
            }
        }

        m_imageSource->close();
    }

    if (images.isEmpty()) {
        images << new QImage();
        durations << 0;
    }

    emit loaded(images, durations);
}

// ---------- Load Thumbnail Task ----------
class LoadThumbnailTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadThumbnailTask(QString thumbnailPath, bool makeImmediately) :
        QRunnable() ,
        m_thumbnailPath(thumbnailPath) ,
        m_makeImmediately(makeImmediately) {}

    void run();

signals:
    void loaded(QImage *thumbnail, bool makeImmediately);

private:
    QString m_thumbnailPath;
    bool m_makeImmediately;
};

void LoadThumbnailTask::run()
{
    QFile file(m_thumbnailPath);
    if (file.exists()) {
        // Set last access/modified time
#ifdef Q_OS_UNIX
        time_t now = time(0);
        struct utimbuf buf;
        buf.actime = now;
        buf.modtime = now;
        utime(m_thumbnailPath.toUtf8().data(), &buf);
#endif
        QImage *image = new QImage(m_thumbnailPath);
        if (!image->isNull()) {
            emit loaded(image, m_makeImmediately);
        } else {
            // Broken thumbnail, delete it
            delete image;
            file.remove();
            emit loaded(0, m_makeImmediately);
        }
    } else {
        emit loaded(0, m_makeImmediately);
    }
}

// ---------- Make Thumbnail Task ----------
class MakeThumbnailTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    MakeThumbnailTask(QImage *image, const QString &path) :
        QRunnable() ,
        m_image(image) ,
        m_path(path) {}

    ~MakeThumbnailTask()
    {
        delete m_image;
    }

    void run();

signals:
    void thumbnailMade(QImage *thumbnail);

private:
      QImage *m_image;
      QString m_path;
};

void MakeThumbnailTask::run()
{
    if (!m_image) {
        emit thumbnailMade(0);
        return;
    }

    int height = qMax<int>(
        THUMBNAIL_MIN_HEIGHT, m_image->height() / THUMBNAIL_SCALE_RATIO);

    QImage *thumbnail = new QImage(m_image->scaledToHeight(height,
        Qt::SmoothTransformation));
    thumbnail->save(m_path, "JPG");
    emit thumbnailMade(thumbnail);
}


// ---------- Image ----------
const QSize Image::UNKNOWN_SIZE = QSize(-1, -1);

Image::Image(QUrl url, QObject *parent) :
    QObject(parent) ,
    m_status(Image::NotLoad) ,
    m_imageSource(ImageSourceManager::instance()->createSingle(url)) ,
    m_thumbnail(new QImage()) ,
    m_loadRequestsCount(0) ,
    m_isLoadingImage(false) ,
    m_isLoadingThumbnail(false) ,
    m_isMakingThumbnail(false) ,
    m_hotspotsLoaded(false) ,
    m_rank(new ImageRank(this, this)) ,
    m_size(Image::UNKNOWN_SIZE) ,
    m_isAnimation(false)
{
    resetFrames();
    computeThumbnailPath();
    loadMetaFromDatabase();
}

Image::Image(ImageSource *imageSource, QObject *parent) :
    QObject(parent) ,
    m_status(Image::NotLoad) ,
    m_imageSource(imageSource) ,
    m_thumbnail(new QImage()) ,
    m_loadRequestsCount(0) ,
    m_isLoadingImage(false) ,
    m_isLoadingThumbnail(false) ,
    m_isMakingThumbnail(false) ,
    m_hotspotsLoaded(false) ,
    m_rank(new ImageRank(this, this)) ,
    m_size(Image::UNKNOWN_SIZE) ,
    m_isAnimation(false)
{
    resetFrames();
    computeThumbnailPath();
    loadMetaFromDatabase();
}

Image::~Image()
{
    destroyFrames();

    m_imageSource.clear();

    if (m_thumbnail) {
        delete m_thumbnail;
    }

    while (!m_hotspots.isEmpty()) {
        delete m_hotspots.takeFirst();
    }
}

void Image::load(int priority)
{
    m_loadRequestsCount ++;
    m_status = Image::Loading;

    if (m_isLoadingImage) {
        return;
    }

    m_isLoadingImage = true;

    LoadImageTask *loadImageTask = new LoadImageTask(m_imageSource);
    connect(loadImageTask, SIGNAL(loaded(QList<QImage*>, QList<int>)),
            this, SLOT(imageReaderFinished(QList<QImage*>, QList<int>)));
    QThreadPool::globalInstance()->reserveThread();
    QThreadPool::globalInstance()->start(loadImageTask, priority);
}

void Image::scheduleUnload()
{
    unloadIfNeeded();
}

void Image::unloadIfNeeded()
{
    if (m_loadRequestsCount == 0) {
        // If error, keep this state to prevent further try
        if (m_status != Image::LoadError) {
            m_status = Image::NotLoad;
        }

        resetFrames();
    } else {
        m_loadRequestsCount --;
    }
}

void Image::destroyFrames()
{
    foreach (QImage *image, m_frames) {
        delete image;
    }
    m_frames.clear();
    m_durations.clear();
}

void Image::resetFrames(QImage *defaultFrame, int defaultDuration)
{
    destroyFrames();
    m_frames << defaultFrame;
    m_durations << defaultDuration;
}

void Image::imageReaderFinished(QList<QImage *> images, QList<int> durations)
{
    Q_ASSERT(images.count() > 0);
    Q_ASSERT(durations.count() > 0);

    QThreadPool::globalInstance()->releaseThread();
    m_isLoadingImage = false;

    destroyFrames();
    m_frames = images;
    m_durations = durations;

    if (m_thumbnail->isNull()) {
        makeThumbnail();
    }

    m_isAnimation = (m_frames.count() > 1);

    if (!defaultFrame()->isNull()) {
        bool shouldWriteSizeIntoDatabase = (m_size == Image::UNKNOWN_SIZE);

        m_size = defaultFrame()->size();

        // Write image size into database
        if (shouldWriteSizeIntoDatabase) {
            LocalDatabase::instance()->updateImageSize(this);
        }

        m_status = Image::LoadComplete;
    } else {
        m_status = Image::LoadError;

        // Show a warning image
        resetFrames(new QImage(":/res/warning.png"), 0);
    }

    emit loaded();

    unloadIfNeeded();
}

void Image::thumbnailReaderFinished(QImage *thumbnail, bool makeImmediately)
{
    QThreadPool::globalInstance()->releaseThread();
    m_isLoadingThumbnail = false;

    if (thumbnail) {
        delete m_thumbnail;
        m_thumbnail = thumbnail;

        emit thumbnailLoaded();
    } else if (makeImmediately) {
        load(LowestPriority);   // Thumbnail making should be low priority
        scheduleUnload();   // Release memory after thumbnail generated
    }
}

void Image::loadThumbnail(bool makeImmediately)
{
    if (!m_thumbnail->isNull()) {
        emit thumbnailLoaded();
        return;
    }

    if (m_isLoadingThumbnail) {
        return;
    }

    m_isLoadingThumbnail = true;

    QString thumbnailFullPath = GlobalConfig::instance()->thumbnailPath() +
        QDir::separator() + m_thumbnailPath;
    LoadThumbnailTask *loadThumbnailTask =
        new LoadThumbnailTask(thumbnailFullPath, makeImmediately);
    connect(loadThumbnailTask, SIGNAL(loaded(QImage *, bool)),
            this, SLOT(thumbnailReaderFinished(QImage *, bool)));
    QThreadPool::globalInstance()->reserveThread();
    // Thumbnail loading should be low priority
    QThreadPool::globalInstance()->start(loadThumbnailTask, LowPriority);
}

void Image::makeThumbnail()
{
    Q_ASSERT(defaultFrame());

    if (defaultFrame()->isNull() || m_isMakingThumbnail) {
        return;
    }

    m_isMakingThumbnail = true;

    QString thumbnailFullPath = GlobalConfig::instance()->thumbnailPath() +
        QDir::separator() + m_thumbnailPath;
    MakeThumbnailTask *makeThumbnailTask =
        new MakeThumbnailTask(new QImage(*defaultFrame()), thumbnailFullPath);
    connect(makeThumbnailTask, SIGNAL(thumbnailMade(QImage*)),
            this, SLOT(thumbnailMade(QImage*)));
    QThreadPool::globalInstance()->reserveThread();
    QThreadPool::globalInstance()->start(makeThumbnailTask);
}

void Image::thumbnailMade(QImage *thumbnail)
{
    QThreadPool::globalInstance()->releaseThread();
    if (thumbnail) {
        delete m_thumbnail;
        m_thumbnail = thumbnail;

        LocalDatabase::instance()->insertImage(this);

        emit thumbnailLoaded();
    }

    m_isMakingThumbnail = false;
}

void Image::computeThumbnailPath()
{
    if (!m_imageSource.isNull()) {
        QByteArray hash = m_imageSource->hash();

        QString sub = hash.left(2);
        QString name = hash.mid(2);

        QStringList pathParts = QStringList() << sub << name;
        m_thumbnailPath = pathParts.join(QDir::separator());

        QDir dir;
        dir.mkpath(GlobalConfig::instance()->thumbnailPath() +
                   QDir::separator() + sub);
    }
}

void Image::loadMetaFromDatabase()
{
    m_size = LocalDatabase::instance()->queryImageSize(this);
}

QString Image::name() const
{
    return m_imageSource.isNull() ? QString() : m_imageSource->name();
}

qreal Image::aspectRatio() const
{
    Q_ASSERT(defaultFrame());
    Q_ASSERT(m_thumbnail);

    if (!defaultFrame()->isNull()) {
        int width = defaultFrame()->width();
        int height = defaultFrame()->height();

        return height != 0 ? qreal(width) / qreal(height) : 0;
    } else if (!m_thumbnail->isNull()) {
        int width = m_thumbnail->width();
        int height = m_thumbnail->height();

        return height != 0 ? qreal(width) / qreal(height) : 0;
    }

    return 0;
}

bool Image::copy(const QString &destPath)
{
    if (!m_imageSource.isNull()) {
        return m_imageSource->copy(destPath);
    } else {
        return false;
    }
}

void Image::loadHotspots(bool forceReload)
{
    if (!m_hotspotsLoaded || forceReload) {
        while (!m_hotspots.isEmpty()) {
            delete m_hotspots.takeFirst();
        }

        m_hotspots = LocalDatabase::instance()->queryImageHotspots(this);
        m_hotspotsLoaded = true;
    }
    emit hotpotsLoaded();
}

bool Image::isAnimation() const
{
    return m_isAnimation;
}

QList<int> Image::durations() const
{
    return m_durations;
}

QList<QImage *> Image::frames() const
{
    return m_frames;
}

int Image::frameCount() const
{
    return m_frames.count();
}

#include "Image.moc"
