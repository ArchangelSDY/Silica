#include "Navigator.h"

static const int MAX_PRELOAD = 0;

// Cache both backward/forward preloaded images and the current one
static const int MAX_CACHE = 2 * MAX_PRELOAD + 1;

Navigator::Navigator(QObject *parent) :
    QObject(parent) ,
    m_currentIndex(-1) ,
    m_reverseNavigation(false) ,
    m_cachedImages(MAX_CACHE)
{
}

void Navigator::reset()
{
    m_cachedImages.clear();
    m_currentImage = 0;
    m_currentIndex = -1;
    m_reverseNavigation = false;
    m_playlist.clear();
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
    reset();

    m_playlist = playList;
    emit playListChange(&m_playlist);

    goIndex(0);
}

void Navigator::appendPlayList(const PlayList &playList)
{
    // If empty, load the first one
    bool shouldGoFirst = (m_playlist.count() == 0);

    m_playlist.append(playList);
    emit playListAppend(&PlayList(playList));

    if (shouldGoFirst) {
        goIndex(0);
    }
}

void Navigator::clearPlayList()
{
    reset();

    emit playListChange(&m_playlist);
}

Image* Navigator::loadIndex(int index, bool shouldPaint)
{
    if (index < 0 || index >= m_playlist.count()) {
        return 0;
    }

    QUrl url = m_playlist[index];
    Image *image = m_cachedImages.at(index);

    if (!image) {
        image = new Image(url);
        m_cachedImages.insert(index, image);
    }

    if (shouldPaint) {
        connect(image, SIGNAL(thumbnailLoaded()),
                this, SLOT(thumbnailLoaded()));
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
    if (index < 0 || index >= m_playlist.count() || index == m_currentIndex) {
        return;
    }

    // Load and paint
    Image *image = loadIndex(index, true);
    if (!image) {
        return;
    }

    m_currentIndex = index;
    m_currentImage = image;

    emit paint(image);
    emit navigationChange(index);

    if (image->status() != Image::LoadComplete) {
        image->loadThumbnail();
    }

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
    if (loadedImage) {
        if (loadedImage->url() == m_currentImage->url()) {
            emit paint(loadedImage);
        }
    }
}

void Navigator::thumbnailLoaded()
{
    Image *image = static_cast<Image*>(QObject::sender());
    if (image &&
        image->status() != Image::LoadComplete &&
        !image->thumbnail().isNull()) {

        if (image->url() == m_currentImage->url()) {
            emit paintThumbnail(image);
        }
    }
}
