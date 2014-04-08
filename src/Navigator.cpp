#include "ImageSourceManager.h"
#include "Navigator.h"

static const int MAX_PRELOAD = 5;

// Cache both backward/forward preloaded images and the current one
static const int MAX_CACHE = 2 * MAX_PRELOAD + 1;

Navigator::Navigator(QObject *parent) :
    QObject(parent) ,
    m_currentIndex(-1) ,
    m_reverseNavigation(false) ,
    m_isLooping(true) ,
    m_cachedImages(MAX_CACHE) ,
    m_playList(0)
{
    m_autoNavigationTimer.setInterval(Navigator::MEDIUM_AUTO_NAVIGATION_INTERVAL);
    connect(&m_autoNavigationTimer, SIGNAL(timeout()),
            this, SLOT(goFastForward()));
}

Navigator::~Navigator()
{
    if (m_playList) {
        delete m_playList;
    }
}

void Navigator::reset()
{
    m_cachedImages.clear();
    m_currentImage = 0;
    m_currentIndex = -1;
    m_reverseNavigation = false;
    if (m_playList) {
        m_playList->clear();
        delete m_playList;
        m_playList = 0;
    }

    // Notify to clear image source cache
    ImageSourceManager::instance()->clearCache();
}

void Navigator::preload()
{
    for (int i = 1; i <= MAX_PRELOAD; ++i) {
        int delta = m_reverseNavigation ? (-i) : i;
        // Load but not paint
        loadIndex(m_currentIndex + delta, false);
    }
}

void Navigator::setPlayList(PlayList *playList)
{
    reset();

    m_playList = playList;
    connect(m_playList, SIGNAL(itemsChanged()), this, SLOT(reloadPlayList()));
    emit playListChange(m_playList);

    goIndex(0);
}

void Navigator::appendPlayList(PlayList *playList)
{
    // If empty, load the first one
    bool shouldGoFirst = (m_playList->count() == 0);

    m_playList->append(playList);
    emit playListAppend(playList);

    if (shouldGoFirst) {
        goIndex(0);
    }

    // Appended playlist is no longer needed
    // FIXME
    // delete playList;
}

void Navigator::clearPlayList()
{
    reset();

    emit playListChange(m_playList);
}

void Navigator::reloadPlayList()
{
    emit playListChange(m_playList);
}

Image* Navigator::loadIndex(int index, bool shouldPaint)
{
    if (index < 0 || index >= m_playList->count()) {
        if (!m_isLooping) {
            return 0;
        } else {
            index = (index + m_playList->count()) % m_playList->count();
        }
    }

    Image *image = m_cachedImages.at(index);

    if (!image) {
        image = (*m_playList)[index].data();
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
    if (index < 0 || index >= m_playList->count() || index == m_currentIndex) {
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
    } else if (m_isLooping) {
        m_reverseNavigation = true;
        goIndex(m_playList->count() - 1);
    }
}

void Navigator::goNext()
{
    if (m_currentIndex < m_playList->count() - 1) {
        m_reverseNavigation = false;
        goIndex(m_currentIndex + 1);
    } else if (m_isLooping) {
        m_reverseNavigation = false;
        goIndex(0);
    }
}

void Navigator::goFirst()
{
    m_reverseNavigation = false;
    goIndex(0);
}

void Navigator::goLast()
{
    m_reverseNavigation = true;
    goIndex(m_playList->count() - 1);
}

void Navigator::imageLoaded()
{
    Image *loadedImage = static_cast<Image*>(QObject::sender());
    if (loadedImage) {
        if (loadedImage == m_currentImage) {
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

        if (image == m_currentImage) {
            emit paintThumbnail(image);
        }
    }
}

void Navigator::startAutoNavigation(Direction direction)
{
    m_reverseNavigation = (direction == ReverseDirection);
    m_autoNavigationTimer.start();
}

void Navigator::stopAutoNavigation()
{
    m_autoNavigationTimer.stop();
}

void Navigator::setAutoNavigationInterval(int msec)
{
    m_autoNavigationTimer.setInterval(msec);
}

void Navigator::goFastForward()
{
    if (!m_reverseNavigation) {
        if (m_currentIndex < m_playList->count() - 1 || m_isLooping) {
            goNext();
        } else {
            stopAutoNavigation();
        }
    } else {
        if (m_currentIndex > 0 || m_isLooping) {
            goPrev();
        } else {
            stopAutoNavigation();
        }
    }
}

void Navigator::setLoop(bool shouldLoop)
{
    m_isLooping = shouldLoop;
}
