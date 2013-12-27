#include <QtConcurrent/QtConcurrent>

#include "Image.h"

static QImage loadImageAtBackground(QString path)
{
    QImageReader reader(path);
    return reader.read();
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

    QString path = m_url.toLocalFile();
    m_readerFuture = QtConcurrent::run(loadImageAtBackground, path);
    m_readerWatcher.setFuture(m_readerFuture);
}

void Image::readerFinished()
{
    m_status = Image::LoadComplete;
    m_image = m_readerFuture.result();
    emit loaded();
}
