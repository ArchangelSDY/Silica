#include <QtConcurrent/QtConcurrent>
#include <quazipfile.h>

#include "Image.h"

static const QString THUMBNAIL_FOLDER = "thumbnails";
static const int THUMBNAIL_MIN_HEIGHT = 480;
static const int THUMBNAIL_SCALE_RATIO = 8;

static QImage loadImageAtBackground(QSharedPointer<ImageSource> imageSource)
{
    if (!imageSource.isNull() && imageSource->open()) {
        QImageReader reader(imageSource->device());
        QImage image = reader.read();

        imageSource->close();
        return image;
    } else {
        return QImage();
    }
}

Image::Image(QUrl url, QObject *parent) :
    QObject(parent) ,
    m_status(Image::NotLoad) ,
    m_imageSource(ImageSource::create(url))
{
    computeThumbnailPath();

    connect(&m_readerWatcher, SIGNAL(finished()), this, SLOT(readerFinished()));
}

Image::~Image()
{
    m_imageSource.clear();
}

void Image::load()
{
    if (m_status != Image::NotLoad) {
        return;
    }

    m_status = Image::Loading;

    m_readerFuture = QtConcurrent::run(loadImageAtBackground, m_imageSource);
    m_readerWatcher.setFuture(m_readerFuture);
}

void Image::readerFinished()
{
    m_image = m_readerFuture.result();

    if (!m_image.isNull()) {
        m_status = Image::LoadComplete;
    } else {
        m_status = Image::LoadError;

        // Show a warning image
        m_image = QImage(":/res/warning.png");
    }

    emit loaded();

    if (m_thumbnail.isNull() && m_status == Image::LoadComplete) {
        makeThumbnail();
    }
}

void Image::loadThumbnail()
{
    QFile file(m_thumbnailPath);

    if (file.exists()) {
        m_thumbnail = QImage(m_thumbnailPath);

        if (!m_thumbnail.isNull()) {
            emit thumbnailLoaded();
        }
    }
}

void Image::makeThumbnail()
{
    int height = qMax<int>(
        THUMBNAIL_MIN_HEIGHT, m_image.height() / THUMBNAIL_SCALE_RATIO);

    m_thumbnail = m_image.scaledToHeight(height);
    m_thumbnail.save(m_thumbnailPath, "JPG");
    emit thumbnailLoaded();
}

void Image::computeThumbnailPath()
{
    if (!m_imageSource.isNull()) {
        QByteArray hash = m_imageSource->hash();

        QString sub = hash.left(2);
        QString name = hash.mid(2);

        QStringList pathParts;
        pathParts << QCoreApplication::applicationDirPath()
                  << THUMBNAIL_FOLDER << sub << name;
        m_thumbnailPath = pathParts.join(QDir::separator());

        QDir dir;
        dir.mkpath(QCoreApplication::applicationDirPath() + QDir::separator() +
                   THUMBNAIL_FOLDER + QDir::separator() + sub);
    }
}


QString Image::name() const
{
    return m_imageSource.isNull() ? QString() : m_imageSource->name();
}

bool Image::copy(const QString &destPath)
{
    if (!m_imageSource.isNull()) {
        m_imageSource->copy(destPath);
    } else {
        return false;
    }
}
