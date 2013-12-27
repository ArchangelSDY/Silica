#include "Navigator.h"

Navigator::Navigator(QObject *parent) :
    QObject(parent)
{
}

void Navigator::setPlaylist(QList<QUrl> playlist)
{
    m_playlist = playlist;
}

void Navigator::goIndex(int index)
{
    if (index < 0 || index >= m_playlist.count()) {
        return;
    }

    QUrl url = m_playlist[index];
    Image *image = m_cachedImages.object(url);
    if (image != 0
        && image->status() == Image::LoadComplete) {
        emit paint(image->data());
    } else {
        image = new Image(url);
        m_cachedImages.insert(url, image);
        connect(image, SIGNAL(loaded()), this, SLOT(imageLoaded()));
        image->load();
    }

    m_currentIndex = index;
    m_currentImage = image;
}

void Navigator::goPrev()
{
    if (m_currentIndex > 0) {
        goIndex(m_currentIndex - 1);
    }
}

void Navigator::goNext()
{
    if (m_currentIndex < m_playlist.count() - 1) {
        goIndex(m_currentIndex + 1);
    }
}

void Navigator::imageLoaded()
{
    Image *loadedImage = static_cast<Image*>(QObject::sender());
    if (loadedImage && !loadedImage->data().isNull()) {
        if (loadedImage->url() == m_currentImage->url()) {
            emit paint(loadedImage->data());
        }
    }
}
