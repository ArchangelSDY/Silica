#include "Image.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRunnable>
#include <QThreadPool>

#ifdef Q_OS_UNIX
#include <sys/types.h>
#include <time.h>
#include <utime.h>
#endif

#include "deps/quazip/quazip/quazip.h"
#include "db/LocalDatabase.h"
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
    void loaded(QVariantHash metadata, QSharedPointer<ImageData> image);

private:
    QSharedPointer<ImageSource> m_imageSource;
};

void LoadImageTask::run()
{
    QList<QImage> frames;
    QList<int> durations;
    QVariantHash metadata;
    if (!m_imageSource.isNull() && m_imageSource->open()) {
        // Read metadata
        metadata = m_imageSource->readMetadata();

        // Read frames
        QList<QImage> rawFrames;
        if (m_imageSource->readFrames(rawFrames, durations)) {
            for (const QImage &frame : rawFrames) {
                frames << frame.convertToFormat(QImage::Format_ARGB32_Premultiplied);
            }
        }

        m_imageSource->close();
    } else {
        qDebug() << "Unable to open image source";
    }

    Q_ASSERT_X(frames.count() == durations.count(), "LoadImageTask::run()", "Images count should equal to durations count");

    if (frames.empty()) {
        frames << QImage();
        durations << 0;
    }

    QSharedPointer<ImageData> image = QSharedPointer<ImageData>::create();
    image->frames = frames;
    image->durations = durations;
    emit loaded(metadata, image);
}

// ---------- Load Thumbnail Task ----------

static QSharedPointer<QImage> doLoadThumbnailSync(const QString &thumbnailFullPath);

class LoadThumbnailTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadThumbnailTask(const QString &thumbnailPath, bool makeImmediately) :
        QRunnable() ,
        m_thumbnailPath(thumbnailPath) ,
        m_makeImmediately(makeImmediately) {}

    void run();

signals:
    void loaded(QSharedPointer<QImage> thumbnail, bool makeImmediately);

private:
    QString m_thumbnailPath;
    bool m_makeImmediately;
};

void LoadThumbnailTask::run()
{
    QSharedPointer<QImage> thumbnail = doLoadThumbnailSync(m_thumbnailPath);
    emit loaded(thumbnail, m_makeImmediately);
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

    // Create thumbnail dir
    QFileInfo pathInfo(m_path);
    QString dirPath = pathInfo.absolutePath();
    QDir dir;
    dir.mkpath(dirPath);

    int height = qMax<int>(
        THUMBNAIL_MIN_HEIGHT, m_image->height() / THUMBNAIL_SCALE_RATIO);

    QSharedPointer<QImage> thumbnail = QSharedPointer<QImage>(
        new QImage(m_image->scaledToHeight(height, Qt::SmoothTransformation)));
    thumbnail->save(m_path, "JPG");
    emit thumbnailMade(thumbnail);
}


// ---------- Image ----------
const QSize Image::UNKNOWN_SIZE = QSize(-1, -1);


// ---------- Thread Pool ----------
static QThreadPool *threadPool()
{
    static QThreadPool *s_threadPool = nullptr;
    if (!s_threadPool) {
        s_threadPool = new QThreadPool();
        s_threadPool->setMaxThreadCount(QThread::idealThreadCount() - 1);
    }
    return s_threadPool;
}

Image::Image(const QString &path, QObject *parent) :
    QObject(parent) ,
    m_uuid(QUuid::createUuid()) ,
    m_imageSource(ImageSourceManager::instance()->createSingle(path)) ,
    m_isLoadingImage(false) ,
    m_isLoadingThumbnail(false) ,
    m_isMakingThumbnail(false) ,
    m_isError(false) ,
    m_hotspotsLoaded(false) ,
    m_rank(new ImageRank(this, this)) ,
    m_size(Image::UNKNOWN_SIZE)
{
    s_liveImages.insert(m_uuid, true);

    computeThumbnailPath();
}

Image::Image(const QUrl &url, QObject *parent) :
    QObject(parent) ,
    m_uuid(QUuid::createUuid()) ,
    m_imageSource(ImageSourceManager::instance()->createSingle(url)) ,
    m_isLoadingImage(false) ,
    m_isLoadingThumbnail(false) ,
    m_isMakingThumbnail(false) ,
    m_isError(false) ,
    m_hotspotsLoaded(false) ,
    m_rank(new ImageRank(this, this)) ,
    m_size(Image::UNKNOWN_SIZE)
{
    s_liveImages.insert(m_uuid, true);

    computeThumbnailPath();

    connect(this, &Image::loaded,
            this, &Image::onLoad);
    connect(this, &Image::thumbnailLoadFailed,
            this, &Image::onThumbnailLoadFailed);
}

Image::Image(QSharedPointer<ImageSource> imageSource, QObject *parent) :
    QObject(parent) ,
    m_uuid(QUuid::createUuid()) ,
    m_imageSource(imageSource) ,
    m_isLoadingImage(false) ,
    m_isLoadingThumbnail(false) ,
    m_isMakingThumbnail(false) ,
    m_isError(false) ,
    m_hotspotsLoaded(false) ,
    m_rank(new ImageRank(this, this)) ,
    m_size(Image::UNKNOWN_SIZE)
{
    s_liveImages.insert(m_uuid, true);

    computeThumbnailPath();

    connect(this, &Image::loaded,
            this, &Image::onLoad);
    connect(this, &Image::thumbnailLoadFailed,
            this, &Image::onThumbnailLoadFailed);
}

Image::~Image()
{
    s_liveImages.remove(m_uuid);

    m_imageSource.clear();

    while (!m_hotspots.isEmpty()) {
        delete m_hotspots.takeFirst();
    }
}

void Image::load(int priority, bool forceReload)
{
    if (!forceReload) {
        QSharedPointer<ImageData> image = m_image.toStrongRef();
        if (image) {
            emit loaded(image);
            return;
        }
    }

    if (m_isLoadingImage) {
        return;
    }

    m_isLoadingImage = true;

    LoadImageTask *loadImageTask = new LoadImageTask(m_imageSource);
    connect(loadImageTask,
            &LoadImageTask::loaded,
            this,
            &Image::imageReaderFinished);
    threadPool()->start(
        new LiveImageRunnable(m_uuid, loadImageTask),
        priority);
}

void Image::imageReaderFinished(QVariantHash metadata, QSharedPointer<ImageData> image)
{
    Q_ASSERT(image->frames.count() > 0);
    Q_ASSERT(image->durations.count() > 0);

    m_isLoadingImage = false;

    resetMetadata(metadata);

    m_image = image;

    // TODO: Make thumbnail if thumbnail load failed
    if (!m_thumbnail) {
        makeThumbnail();
    }

    QImage &defaultFrame = image->frames.first();
    if (!defaultFrame.isNull()) {
        m_size = defaultFrame.size();
        m_isError = false;
    } else {
        // Show a warning image
        defaultFrame = QImage(":/res/warning.png");
        m_isError = true;
    }

    emit loaded(image);
}

void Image::thumbnailReaderFinished(QSharedPointer<QImage> thumbnail,
                                    bool makeImmediately)
{
    m_isLoadingThumbnail = false;

    if (!thumbnail.isNull() && !thumbnail->isNull()) {
        m_thumbnail = thumbnail;
        m_thumbnailSize = thumbnail->size();
        emit thumbnailLoaded(thumbnail);
    } else if (makeImmediately) {
        load(LowestPriority);   // Thumbnail making should be low priority
    } else {
        emit thumbnailLoadFailed();
    }
}

void Image::loadThumbnail(bool makeImmediately)
{
    QSharedPointer<QImage> thumbnail = m_thumbnail.toStrongRef();
    if (thumbnail) {
        emit thumbnailLoaded(thumbnail);
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
        "/" + m_thumbnailPath;
    LoadThumbnailTask *loadThumbnailTask =
        new LoadThumbnailTask(thumbnailFullPath, makeImmediately);
    connect(loadThumbnailTask, &LoadThumbnailTask::loaded, this, &Image::thumbnailReaderFinished);
    // Thumbnail loading should be low priority
    threadPool()->start(
        new LiveImageRunnable(m_uuid, loadThumbnailTask),
        LowPriority);
}

QSharedPointer<QImage> Image::loadThumbnailSync()
{
    QString thumbnailFullPath = GlobalConfig::instance()->thumbnailPath() +
        "/" + m_thumbnailPath;
    return doLoadThumbnailSync(thumbnailFullPath);
}

static QSharedPointer<QImage> doLoadThumbnailSync(const QString &thumbnailFullPath)
{
    QFile file(thumbnailFullPath);
    if (file.exists()) {
        // Set last access/modified time
#ifdef Q_OS_UNIX
        time_t now = time(0);
        struct utimbuf buf;
        buf.actime = now;
        buf.modtime = now;
        utime(thumbnailFullPath.toUtf8().data(), &buf);
#endif
        QSharedPointer<QImage> image =
            QSharedPointer<QImage>::create(thumbnailFullPath);
        if (!image->isNull()) {
            return image;
        } else {
            // Broken thumbnail, delete it
            image.clear();
            file.remove();
            return QSharedPointer<QImage>();
        }
    } else {
        return QSharedPointer<QImage>();
    }
}

void Image::makeThumbnail()
{
    QImage frame = defaultFrame();
    if (frame.isNull()) {
        emit thumbnailLoadFailed();
        return;
    }

    if (m_isMakingThumbnail) {
        return;
    }

    m_isMakingThumbnail = true;

    QString thumbnailFullPath = GlobalConfig::instance()->thumbnailPath() +
        "/" + m_thumbnailPath;
    MakeThumbnailTask *makeThumbnailTask =
        new MakeThumbnailTask(new QImage(frame), thumbnailFullPath);
    connect(makeThumbnailTask, SIGNAL(thumbnailMade(QSharedPointer<QImage>)),
            this, SLOT(thumbnailMade(QSharedPointer<QImage>)));
    threadPool()->start(
        new LiveImageRunnable(m_uuid, makeThumbnailTask));
}

void Image::thumbnailMade(QSharedPointer<QImage> thumbnail)
{
    if (!thumbnail.isNull() && !thumbnail->isNull()) {
        m_thumbnail = thumbnail;
        m_thumbnailSize = thumbnail->size();

        LocalDatabase::instance()->insertImage(this);

        emit thumbnailLoaded(thumbnail);
    } else {
        emit thumbnailLoadFailed();
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
        m_thumbnailPath = pathParts.join("/");
    }
}

void Image::resetMetadata(const QVariantHash &metadata)
{
    // We cannot use QHash::unite() because image could be loaded multiple times
    for (auto it = metadata.begin(); it != metadata.end(); it++) {
        m_metadata.insert(it.key(), it.value());
    }
}

void Image::loadMetadata()
{
    if (m_imageSource) {
        resetMetadata(m_imageSource->readMetadata());
    }
}

QString Image::name() const
{
    return m_imageSource.isNull() ? QString() : m_imageSource->name();
}

QImage Image::defaultFrame() const
{
    QSharedPointer<ImageData> image = m_image.toStrongRef();
    if (!image) {
        return QImage();
    }

    return image->defaultFrame();
}

qreal Image::aspectRatio() const
{
    if (!m_size.isNull()) {
        int width = m_size.width();
        int height = m_size.height();

        return height != 0 ? qreal(width) / qreal(height) : 0;
    } else if (!m_thumbnailSize.isNull()) {
        int width = m_thumbnailSize.width();
        int height = m_thumbnailSize.height();

        return height != 0 ? qreal(width) / qreal(height) : 0;
    }

    return 0;
}

bool Image::isLoading() const
{
    return m_isLoadingImage;
}

bool Image::isError() const
{
    return m_isError;
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

void Image::onLoad(QSharedPointer<ImageData> image)
{
    if (image->defaultFrame().isNull()) {
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
