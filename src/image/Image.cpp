#include "Image.h"

#include <QtGlobal>
#include <QUuid>

#ifdef Q_OS_UNIX
#include <sys/types.h>
#include <time.h>
#include <utime.h>
#endif

#include "deps/quazip/quazip/quazip.h"
#include "db/LocalDatabase.h"
#include "image/ImageHistogram.h"
#include "image/ImageHotspot.h"
#include "image/ImageRank.h"
#include "image/ImageSource.h"
#include "image/ImageSourceManager.h"
#include "logger/Logger.h"
#include "util/ConcurrentHash.h"
#include "GlobalConfig.h"

static const int THUMBNAIL_MIN_HEIGHT = 480;
static const int THUMBNAIL_SCALE_RATIO = 8;

static ConcurrentHash<QUuid, bool> s_liveImages;

class LiveImageRunnable : public QRunnable
{
public:
    LiveImageRunnable(const QUuid &uuid, QRunnable *wrapped) :
        m_uuid(uuid) ,
        m_wrapped(wrapped)
    {
    }

    ~LiveImageRunnable()
    {
        if (m_wrapped) {
            delete m_wrapped;
        }
    }

    void run()
    {
        if (m_wrapped && s_liveImages.contains(m_uuid)) {
            m_wrapped->run();
        }
    }

private:
    QUuid m_uuid;
    QRunnable *m_wrapped;
};

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
    void loaded(QList<QSharedPointer<QImage> > images, QList<int> durations);

private:
    QSharedPointer<ImageSource> m_imageSource;
};

void LoadImageTask::run()
{
    QList<QSharedPointer<QImage> > images;
    QList<int> durations;
    if (!m_imageSource.isNull() && m_imageSource->open()) {
        QImageReader reader(m_imageSource->device());

        forever {
            QImage image = reader.read();

            // If fail to read first frame, try to decide format from content
            if (image.isNull() && images.isEmpty()) {
                m_imageSource->device()->reset();
                reader.setDevice(m_imageSource->device());
                reader.setDecideFormatFromContent(true);
                image = reader.read();
            }

            if (!image.isNull()) {
                images << QSharedPointer<QImage>::create(image);
                durations << reader.nextImageDelay();
            } else {
                qWarning() << "Failed to read image due to error:"
                           << reader.errorString();
                break;
            }
        }

        m_imageSource->close();
    }

    if (images.isEmpty()) {
        images << QSharedPointer<QImage>::create();
        durations << 0;
    }

    emit loaded(images, durations);
}

// ---------- Load Thumbnail Task ----------
class LoadThumbnailTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadThumbnailTask(QString thumbnailPath, const QSize &size, bool makeImmediately) :
        QRunnable() ,
        m_thumbnailPath(thumbnailPath) ,
        m_size(size) ,
        m_makeImmediately(makeImmediately) {}

    void run();

signals:
    void loaded(QSharedPointer<QImage> thumbnail, bool makeImmediately);

private:
    QString m_thumbnailPath;
    QSize m_size;
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
        QSharedPointer<QImage> image =
            QSharedPointer<QImage>::create(m_thumbnailPath);
        if (!image->isNull()) {
            if (m_size.isNull()) {
                emit loaded(image, m_makeImmediately);
            } else {
                QSharedPointer<QImage> scaledImage = QSharedPointer<QImage>::create(
                    image->scaled(m_size.width(), m_size.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                emit loaded(scaledImage, m_makeImmediately);
            }
        } else {
            // Broken thumbnail, delete it
            image.clear();
            file.remove();
            emit loaded(QSharedPointer<QImage>(), m_makeImmediately);
        }
    } else {
        emit loaded(QSharedPointer<QImage>(), m_makeImmediately);
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
    void thumbnailMade(QSharedPointer<QImage> thumbnail);

private:
      QImage *m_image;
      QString m_path;
};

void MakeThumbnailTask::run()
{
    if (!m_image) {
        emit thumbnailMade(QSharedPointer<QImage>());
        return;
    }

    int height = qMax<int>(
        THUMBNAIL_MIN_HEIGHT, m_image->height() / THUMBNAIL_SCALE_RATIO);

    QSharedPointer<QImage> thumbnail = QSharedPointer<QImage>(
        new QImage(m_image->scaledToHeight(height, Qt::SmoothTransformation)));
    thumbnail->save(m_path, "JPG");
    emit thumbnailMade(thumbnail);
}


// ---------- Image ----------
const QSize Image::UNKNOWN_SIZE = QSize(-1, -1);

QThreadPool *Image::s_threadPool = 0;
QThreadPool *Image::threadPool()
{
    if (!s_threadPool) {
        s_threadPool = new QThreadPool();
    }
    return s_threadPool;
}

Image::Image(const QString &path, QObject *parent) :
    QObject(parent) ,
    m_uuid(QUuid::createUuid()) ,
    m_status(Image::NotLoad) ,
    m_imageSource(ImageSourceManager::instance()->createSingle(path)) ,
    m_thumbnail(new QImage()) ,
    m_loadRequestsCount(0) ,
    m_isLoadingImage(false) ,
    m_isLoadingThumbnail(false) ,
    m_isMakingThumbnail(false) ,
    m_hotspotsLoaded(false) ,
    m_rank(new ImageRank(this, this)) ,
    m_size(Image::UNKNOWN_SIZE) ,
    m_isAnimation(false) ,
    m_thumbHist(0)
{
    s_liveImages.insert(m_uuid, true);

    resetFrames();
    computeThumbnailPath();
    loadMetaFromDatabase();

    connect(this, SIGNAL(thumbnailLoaded()),
            this, SLOT(initThumbHist()));
}

Image::Image(const QUrl &url, QObject *parent) :
    QObject(parent) ,
    m_uuid(QUuid::createUuid()) ,
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
    m_isAnimation(false) ,
    m_thumbHist(0)
{
    s_liveImages.insert(m_uuid, true);

    resetFrames();
    computeThumbnailPath();
    loadMetaFromDatabase();

    connect(this, SIGNAL(thumbnailLoaded()),
            this, SLOT(initThumbHist()));
    connect(this, SIGNAL(loaded()),
            this, SLOT(onLoad()));
    connect(this, SIGNAL(thumbnailLoadFailed()),
            this, SLOT(onThumbnailLoadFailed()));
}

Image::Image(ImageSource *imageSource, QObject *parent) :
    QObject(parent) ,
    m_uuid(QUuid::createUuid()) ,
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
    m_isAnimation(false) ,
    m_thumbHist(0)
{
    s_liveImages.insert(m_uuid, true);

    resetFrames();
    computeThumbnailPath();
    loadMetaFromDatabase();

    connect(this, SIGNAL(thumbnailLoaded()),
            this, SLOT(initThumbHist()));
    connect(this, SIGNAL(loaded()),
            this, SLOT(onLoad()));
    connect(this, SIGNAL(thumbnailLoadFailed()),
            this, SLOT(onThumbnailLoadFailed()));
}

Image::~Image()
{
    s_liveImages.remove(m_uuid);

    destroyFrames();

    m_imageSource.clear();

    if (m_thumbHist) {
        delete m_thumbHist;
    }
    if (m_thumbnail) {
        delete m_thumbnail;
    }

    while (!m_hotspots.isEmpty()) {
        delete m_hotspots.takeFirst();
    }
}

void Image::scheduleUnload()
{
    // if (m_status == Image::LoadComplete) {

    --m_loadRequestsCount;
    checkUnload();

    // }
}

void Image::checkUnload()
{
    if (m_loadRequestsCount == 0) {
        // If error, keep this state to prevent further try
        if (m_status != Image::LoadError) {
            m_status = Image::NotLoad;
        }

        resetFrames();
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

void Image::load(int priority)
{
    ++m_loadRequestsCount;
    m_status = Image::Loading;

    if (m_isLoadingImage) {
        return;
    }

    m_isLoadingImage = true;

    LoadImageTask *loadImageTask = new LoadImageTask(m_imageSource);
    connect(loadImageTask,
            SIGNAL(loaded(QList<QSharedPointer<QImage> >, QList<int>)),
            this,
            SLOT(imageReaderFinished(QList<QSharedPointer<QImage> >, QList<int>)));
    Image::threadPool()->start(
        new LiveImageRunnable(m_uuid, loadImageTask),
        priority);
}

void Image::imageReaderFinished(QList<QSharedPointer<QImage> > images,
                                QList<int> durations)
{
    Q_ASSERT(images.count() > 0);
    Q_ASSERT(durations.count() > 0);

    m_isLoadingImage = false;

    destroyFrames();
    foreach (const QSharedPointer<QImage> &image, images) {
        m_frames.append(new QImage(*image));
    }
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

    // If unload scheduled during loading, there will be no further chance to unload,
    // leading to a memory leak. So we check here right after loaded to see if we
    // are no longer requested.
    checkUnload();
}

void Image::thumbnailReaderFinished(QSharedPointer<QImage> thumbnail,
                                    bool makeImmediately)
{
    m_isLoadingThumbnail = false;

    if (!thumbnail.isNull() && !thumbnail->isNull()) {
        delete m_thumbnail;
        m_thumbnail = new QImage(*thumbnail);

        emit thumbnailLoaded();
    } else if (makeImmediately) {
        load(LowestPriority);   // Thumbnail making should be low priority
        scheduleUnload();       // Release memory after thumbnail generated
    } else {
        emit thumbnailLoadFailed();
    }
}

void Image::loadThumbnail(const QSize &size, bool makeImmediately)
{
    if (!m_thumbnail->isNull()) {
        emit thumbnailLoaded();
        return;
    }

    if (m_isLoadingThumbnail) {
        return;
    }

    // Do not load thumbnail for image source that doesn't exist
    if (!m_imageSource->exists()) {
        emit thumbnailLoadFailed();
        return;
    }

    m_isLoadingThumbnail = true;

    QString thumbnailFullPath = GlobalConfig::instance()->thumbnailPath() +
        QDir::separator() + m_thumbnailPath;
    LoadThumbnailTask *loadThumbnailTask =
        new LoadThumbnailTask(thumbnailFullPath, size, makeImmediately);
    connect(loadThumbnailTask, SIGNAL(loaded(QSharedPointer<QImage>, bool)),
            this, SLOT(thumbnailReaderFinished(QSharedPointer<QImage>, bool)));
    // Thumbnail loading should be low priority
    Image::threadPool()->start(
        new LiveImageRunnable(m_uuid, loadThumbnailTask),
        LowPriority);
}

void Image::makeThumbnail()
{
    Q_ASSERT(defaultFrame());

    if (defaultFrame()->isNull()) {
        emit thumbnailLoadFailed();
        return;
    }

    if (m_isMakingThumbnail) {
        return;
    }

    m_isMakingThumbnail = true;

    QString thumbnailFullPath = GlobalConfig::instance()->thumbnailPath() +
        QDir::separator() + m_thumbnailPath;
    MakeThumbnailTask *makeThumbnailTask =
        new MakeThumbnailTask(new QImage(*defaultFrame()), thumbnailFullPath);
    connect(makeThumbnailTask, SIGNAL(thumbnailMade(QSharedPointer<QImage>)),
            this, SLOT(thumbnailMade(QSharedPointer<QImage>)));
    Image::threadPool()->start(
        new LiveImageRunnable(m_uuid, makeThumbnailTask));
}

void Image::thumbnailMade(QSharedPointer<QImage> thumbnail)
{
    if (!thumbnail.isNull() && !thumbnail->isNull()) {
        delete m_thumbnail;
        m_thumbnail = new QImage(*thumbnail);

        LocalDatabase::instance()->insertImage(this);

        emit thumbnailLoaded();
    } else if (m_thumbnail->isNull()) {
        emit thumbnailLoadFailed();
    }

    m_isMakingThumbnail = false;

    checkUnload();
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

ImageHistogram *Image::thumbHist() const
{
    return m_thumbHist;
}

void Image::initThumbHist()
{
    if (!m_thumbnail || m_thumbnail->isNull()) {
        return;
    }

    // No need to create again if there's already one
    if (!m_thumbHist) {
        m_thumbHist = new ImageHistogram(*m_thumbnail);
    }
}

void Image::onLoad()
{
    if (m_status == Image::LoadError) {
        Logger::instance()->log(Logger::IMAGE_LOAD_ERROR)
            .describe("ImageHashStr", m_imageSource->hashStr())
            .describe("ImageURL", m_imageSource->url())
            .save();
    }
}

void Image::onThumbnailLoadFailed()
{
    Logger::instance()->log(Logger::IMAGE_THUMBNAIL_LOAD_ERROR)
        .describe("ImageHashStr", m_imageSource->hashStr())
        .describe("ImageURL", m_imageSource->url())
        .save();
}

#include "Image.moc"
