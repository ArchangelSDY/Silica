#include <QtConcurrent/QtConcurrent>
#include <quazipfile.h>

#include "Image.h"

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
    connect(&m_readerWatcher, SIGNAL(finished()), this, SLOT(readerFinished()));
}

void Image::load()
{
    if (m_status != Image::NotLoad) {
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
}
