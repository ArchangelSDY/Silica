#include <QtConcurrent/QtConcurrent>
#include <quazipfile.h>

#include "Image.h"

static const QString THUMBNAIL_FOLDER = "thumbnails";
static const int THUMBNAIL_MIN_HEIGHT = 480;
static const int THUMBNAIL_SCALE_RATIO = 8;

static QImage loadImageAtBackground(QUrl url)
{
    if (url.scheme() == "file") {
        QString imagePath = url.toLocalFile();

        QImageReader reader(imagePath);
        return reader.read();
    } else if (url.scheme() == "zip") {
        QString imageName = url.fragment();

        QUrl zipUrl = url;
        zipUrl.setScheme("file");
        zipUrl.setFragment("");
        QString zipPath = zipUrl.toLocalFile();

        QuaZipFile imageFile(zipPath, imageName);
        bool success = imageFile.open(QuaZipFile::ReadOnly);

        if (success) {
            QImageReader reader(&imageFile);
            QImage image = reader.read();

            imageFile.close();
            return image;
        }
    }

    return QImage();
}

Image::Image(QUrl url, QObject *parent) :
    QObject(parent) ,
    m_status(Image::NotLoad) ,
    m_url(url)
{
    computeThumbnailPath();

    connect(&m_readerWatcher, SIGNAL(finished()), this, SLOT(readerFinished()));
}

void Image::load()
{
    if (m_status != Image::NotLoad || m_url.isEmpty()) {
        return;
    }

    m_status = Image::Loading;

    m_readerFuture = QtConcurrent::run(loadImageAtBackground, m_url);
    m_readerWatcher.setFuture(m_readerFuture);
}

void Image::readerFinished()
{
    m_status = Image::LoadComplete;
    m_image = m_readerFuture.result();
    emit loaded();

    if (m_thumbnail.isNull()) {
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
    QByteArray hash = QCryptographicHash::hash(
        QByteArray(m_url.url().toUtf8()), QCryptographicHash::Sha1).toHex();
    QString sub = hash.left(2);
    QString name =hash.mid(2);

    QStringList pathParts;
    pathParts << QDir::currentPath() << THUMBNAIL_FOLDER << sub << name;
    m_thumbnailPath = pathParts.join(QDir::separator());

    QDir dir;
    dir.mkpath(THUMBNAIL_FOLDER + QDir::separator() + sub);
}


QString Image::name() const
{
    if (m_url.scheme() == "zip") {
        return m_url.fragment();
    } else {
        QString path = m_url.toLocalFile();
        QFileInfo file(path);
        return file.fileName();
    }
}
