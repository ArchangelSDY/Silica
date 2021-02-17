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
#include "image/ImageSource.h"
#include "image/ImageSourceManager.h"
#include "logger/Logger.h"
#include "util/ConcurrentHash.h"
#include "GlobalConfig.h"

static const int THUMBNAIL_MIN_EDGE = 480;

// ---------- Load Image Task ----------

static QSharedPointer<ImageData> doLoadImageSync(QSharedPointer<ImageSource> imageSource)
{
    QList<QImage> frames;
    QList<int> durations;
    QVariantHash metadata;
    if (!imageSource.isNull() && imageSource->open()) {
        // Read metadata
        metadata = imageSource->readMetadata();

        // Read frames
        QList<QImage> rawFrames;
        if (imageSource->readFrames(rawFrames, durations)) {
            for (const QImage &frame : rawFrames) {
                frames << frame.convertToFormat(QImage::Format_ARGB32_Premultiplied);
            }
        }

        imageSource->close();
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
    image->metadata = metadata;

    return image;
}

class LoadImageTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadImageTask(QWeakPointer<bool> liveness, QSharedPointer<ImageSource> imageSource) :
        QRunnable() ,
        m_liveness(liveness) ,
        m_imageSource(imageSource) {}

    void run();

signals:
    void loaded(QSharedPointer<ImageData> image);

private:
    QWeakPointer<bool> m_liveness;
    QSharedPointer<ImageSource> m_imageSource;
};

void LoadImageTask::run()
{
    if (m_liveness.toStrongRef()) {
        auto ret = doLoadImageSync(m_imageSource);
        emit loaded(ret);
    }
}

// ---------- Load Thumbnail Task ----------

static bool isThumbnailOutDated(QSharedPointer<QImage> image)
{
    if (image->isNull()) {
        return true;
    }

    if (image->width() < THUMBNAIL_MIN_EDGE || image->height() < THUMBNAIL_MIN_EDGE) {
        return true;
    }

    return false;
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
        if (!isThumbnailOutDated(image)) {
            return image;
        } else {
            // Broken/outdated thumbnail, delete it
            image.clear();
            file.remove();
            return QSharedPointer<QImage>();
        }
    } else {
        return QSharedPointer<QImage>();
    }
}

class LoadThumbnailTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadThumbnailTask(QWeakPointer<bool> liveness, const QString &thumbnailPath) :
        QRunnable(),
        m_liveness(liveness) ,
        m_thumbnailPath(thumbnailPath)
    {
    }

    void run();

signals:
    void loaded(QSharedPointer<QImage> thumbnail);

private:
    QWeakPointer<bool> m_liveness;
    QString m_thumbnailPath;
};

void LoadThumbnailTask::run()
{
    if (m_liveness.toStrongRef()) {
        QSharedPointer<QImage> thumbnail = doLoadThumbnailSync(m_thumbnailPath);
        emit loaded(thumbnail);
    }
}

// ---------- Make Thumbnail Task ----------

static QSharedPointer<QImage> doMakeThumbnailSync(const QString &path, QSharedPointer<QImage> image)
{
    Q_ASSERT(!image.isNull());

    // Create thumbnail dir
    QFileInfo pathInfo(path);
    QString dirPath = pathInfo.absolutePath();
    QDir dir;
    dir.mkpath(dirPath);

    QSharedPointer<QImage> thumbnail = QSharedPointer<QImage>::create(
        std::move(image->scaled(THUMBNAIL_MIN_EDGE, THUMBNAIL_MIN_EDGE, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
    thumbnail->save(path, "PNG");

    return thumbnail;
}

class MakeThumbnailTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    MakeThumbnailTask(QWeakPointer<bool> liveness, QSharedPointer<QImage> image, const QString &path) :
        QRunnable() ,
        m_liveness(liveness) ,
        m_image(image) ,
        m_path(path) {}

    void run();

signals:
    void thumbnailMade(QSharedPointer<QImage> thumbnail);

private:
    QWeakPointer<bool> m_liveness;
    QSharedPointer<QImage> m_image;
    QString m_path;
};

void MakeThumbnailTask::run()
{
    if (m_liveness.toStrongRef()) {
        auto thumbnail = doMakeThumbnailSync(m_path, m_image);
        emit thumbnailMade(thumbnail);
    }
}

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
    m_liveness(QSharedPointer<bool>::create(true)) ,
    m_imageSource(ImageSourceManager::instance()->createSingle(path)) ,
    m_isLoadingImage(false) ,
    m_isLoadingThumbnail(false) ,
    m_isMakingThumbnail(false) ,
    m_isError(false) ,
    m_needMakeThumbnail(false)
{
    computeThumbnailPath();

    connect(this, &Image::loaded,
            this, &Image::onLoad);
    connect(this, &Image::thumbnailLoadFailed,
            this, &Image::onThumbnailLoadFailed);
}

Image::Image(const QUrl &url, QObject *parent) :
    QObject(parent) ,
    m_uuid(QUuid::createUuid()) ,
    m_liveness(QSharedPointer<bool>::create(true)) ,
    m_imageSource(ImageSourceManager::instance()->createSingle(url)) ,
    m_isLoadingImage(false) ,
    m_isLoadingThumbnail(false) ,
    m_isMakingThumbnail(false) ,
    m_isError(false) ,
    m_needMakeThumbnail(false)
{
    computeThumbnailPath();

    connect(this, &Image::loaded,
            this, &Image::onLoad);
    connect(this, &Image::thumbnailLoadFailed,
            this, &Image::onThumbnailLoadFailed);
}

Image::Image(QSharedPointer<ImageSource> imageSource, QObject *parent) :
    QObject(parent) ,
    m_uuid(QUuid::createUuid()) ,
    m_liveness(QSharedPointer<bool>::create(true)) ,
    m_imageSource(imageSource) ,
    m_isLoadingImage(false) ,
    m_isLoadingThumbnail(false) ,
    m_isMakingThumbnail(false) ,
    m_isError(false) ,
    m_needMakeThumbnail(false)
{
    computeThumbnailPath();

    connect(this, &Image::loaded,
            this, &Image::onLoad);
    connect(this, &Image::thumbnailLoadFailed,
            this, &Image::onThumbnailLoadFailed);
}

Image::~Image()
{
    m_imageSource.clear();
}

void Image::load(int priority, bool forceReload)
{
    if (!forceReload) {
        QSharedPointer<ImageData> image = m_data.toStrongRef();
        if (image) {
            emit loaded(image);
            return;
        }
    }

    if (m_isLoadingImage) {
        return;
    }

    m_isLoadingImage = true;

    LoadImageTask *loadImageTask = new LoadImageTask(m_liveness.toWeakRef(), m_imageSource);
    connect(loadImageTask,
            &LoadImageTask::loaded,
            this,
            &Image::imageReaderFinished);
    threadPool()->start(loadImageTask, priority);
}

void Image::imageReaderFinished(QSharedPointer<ImageData> image)
{
    Q_ASSERT(image->frames.count() > 0);
    Q_ASSERT(image->durations.count() > 0);

    resetMetadata(image->metadata);
    m_data = image;
    m_isLoadingImage = false;

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

void Image::thumbnailReaderFinished(QSharedPointer<QImage> thumbnail)
{
    m_isLoadingThumbnail = false;

    if (!thumbnail.isNull() && !thumbnail->isNull()) {
        m_thumbnail = thumbnail;
        m_thumbnailSize = thumbnail->size();
        m_needMakeThumbnail = false;
        emit thumbnailLoaded(thumbnail);
    } else {
        makeThumbnail();
    }
}

void Image::loadThumbnail()
{
    QSharedPointer<QImage> thumbnail = m_thumbnail.toStrongRef();
    if (thumbnail) {
        m_needMakeThumbnail = false;
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

    LoadThumbnailTask *loadThumbnailTask = new LoadThumbnailTask(m_liveness.toWeakRef(), m_thumbnailPath);
    connect(loadThumbnailTask, &LoadThumbnailTask::loaded, this, &Image::thumbnailReaderFinished);
    // Thumbnail loading should be low priority
    threadPool()->start(loadThumbnailTask, LowPriority);
}

QSharedPointer<QImage> Image::loadThumbnailSync()
{
    auto thumbnail = doLoadThumbnailSync(m_thumbnailPath);
    if (thumbnail) {
        return thumbnail;
    }

    // Thumbnail not found, try to load and make
    auto imageData = doLoadImageSync(m_imageSource);
    return doMakeThumbnailSync(m_thumbnailPath, QSharedPointer<QImage>::create(std::move(imageData->defaultFrame())));
}

void Image::makeThumbnail()
{
    m_needMakeThumbnail = true;
    load(LowestPriority);   // Thumbnail making should be low priority
}

void Image::makeThumbnail(QSharedPointer<ImageData> image)
{
    QImage frame = image->defaultFrame();
    if (frame.isNull()) {
        emit thumbnailLoadFailed();
        return;
    }

    if (m_isMakingThumbnail) {
        return;
    }

    m_isMakingThumbnail = true;

    MakeThumbnailTask *makeThumbnailTask =
        new MakeThumbnailTask(m_liveness.toWeakRef(), QSharedPointer<QImage>::create(std::move(frame)), m_thumbnailPath);
    connect(makeThumbnailTask, &MakeThumbnailTask::thumbnailMade,
            this, &Image::thumbnailMade);
    threadPool()->start(makeThumbnailTask);
}

void Image::thumbnailMade(QSharedPointer<QImage> thumbnail)
{
    if (!thumbnail.isNull() && !thumbnail->isNull()) {
        m_thumbnail = thumbnail;
        m_thumbnailSize = thumbnail->size();
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
        m_thumbnailPath = GlobalConfig::instance()->thumbnailPath() + "/" + pathParts.join("/");
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
    QSharedPointer<ImageData> image = m_data.toStrongRef();
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

void Image::onLoad(QSharedPointer<ImageData> image)
{
    if (m_needMakeThumbnail) {
        makeThumbnail(image);
    }

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
