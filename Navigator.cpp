#include "Navigator.h"

static const int MAX_PRELOAD = 5;

Navigator::Navigator(QObject *parent) :
    QObject(parent) ,
    m_reverseNavigation(false)
{
    m_cachedImages.setMaxCost(10);
}

void Navigator::preload()
{
    for (int i = 1; i <= MAX_PRELOAD; ++i) {
        int delta = m_reverseNavigation ? (-i) : i;
        // Load but not paint
        loadIndex(m_currentIndex + delta, false);
    }
}

void Navigator::setPlayList(const PlayList &playList)
{
    m_playlist = playList;
    goIndex(0);
}

Image* Navigator::loadIndex(int index, bool shouldPaint)
{
    if (index < 0 || index >= m_playlist.count()) {
        return 0;
    }

    QUrl url = m_playlist[index];
    Image *image = m_cachedImages.object(url);

    if (!image) {
        image = new Image(url);
        m_cachedImages.insert(url, image);
    }

    if (shouldPaint) {
        connect(image, SIGNAL(loaded()), this, SLOT(imageLoaded()),
                Qt::UniqueConnection);
    }

    if (image->status() == Image::NotLoad) {
        image->load();
    }

    return image;
}

void Navigator::goIndex(int index)
{
    if (index < 0 || index >= m_playlist.count()) {
        return;
    }

    // Load and paint
    Image *image = loadIndex(index, true);
    if (!image) {
        return;
    }

    if (image->status() == Image::LoadComplete) {
        emit paint(image->data());
    } else {
        emit paint(QImage());
    }

    m_currentIndex = index;
    m_currentImage = image;

    preload();
}

void Navigator::goPrev()
{
    if (m_currentIndex > 0) {
        m_reverseNavigation = true;
        goIndex(m_currentIndex - 1);
    }
}

void Navigator::goNext()
{
    if (m_currentIndex < m_playlist.count() - 1) {
        m_reverseNavigation = false;
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
