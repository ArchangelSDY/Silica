#include <QtConcurrent/QtConcurrent>

#include "Image.h"

static QImage loadImageAtBackground(QImageReader *reader)
{
    return reader->read();
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
    m_reader.setFileName(path);

    m_readerFuture = QtConcurrent::run(
        loadImageAtBackground, &m_reader);
    m_readerWatcher.setFuture(m_readerFuture);
}

void Image::readerFinished()
{
    m_status = Image::LoadComplete;
    m_image = m_readerFuture.result();
    emit loaded();
}
