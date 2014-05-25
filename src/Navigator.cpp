#include "AbstractNavigationPlayer.h"
#include "ImageSourceManager.h"
#include "Navigator.h"
#include "NormalNavigationPlayer.h"

static const int MAX_PRELOAD = 5;

// Cache both backward/forward preloaded images and the current one
static const int MAX_CACHE = 2 * MAX_PRELOAD + 1;

Navigator::Navigator(QObject *parent) :
    QObject(parent) ,
    m_currentIndex(-1) ,
    m_reverseNavigation(false) ,
    m_isLooping(true) ,
    m_cachedImages(MAX_CACHE) ,
    m_playList(0) ,
    m_ownPlayList(false) ,
    m_player(new NormalNavigationPlayer(this))
{
    m_autoNavigationTimer.setInterval(Navigator::MEDIUM_AUTO_NAVIGATION_INTERVAL);
    connect(&m_autoNavigationTimer, SIGNAL(timeout()),
            this, SLOT(goAutoNavigation()));
}

Navigator::~Navigator()
{
    if (m_playList) {
        delete m_playList;
    }
    delete m_player;
}

void Navigator::reset()
{
    m_cachedImages.clear();
    m_currentImage = 0;
    m_currentIndex = -1;
    m_reverseNavigation = false;
    if (m_playList) {
        if (m_ownPlayList) {
            delete m_playList;
        }
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

void Navigator::setPlayList(PlayList *playList, bool takeOwnership)
{
    if (!playList) {
        return;
    }

    reset();

    m_playList = playList;
    m_ownPlayList = takeOwnership;
    connect(m_playList, SIGNAL(itemsChanged()), this, SLOT(reloadPlayList()));
    connect(m_playList, SIGNAL(itemsAppended(int)),
            this, SLOT(playListAppended(int)));
    emit playListChange();

    goIndex(0);
}

void Navigator::playListAppended(int start)
{
    // If empty, load the first one
    bool shouldGoFirst = (m_playList->count() == 0);

    emit playListAppend(start);

    if (shouldGoFirst) {
        goIndex(0);
    }
}

void Navigator::reloadPlayList()
{
    emit playListChange();
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

bool Navigator::goIndex(int index)
{
    if (index < 0 || index >= m_playList->count() || index == m_currentIndex) {
        return false;
    }

    // Load and paint
    Image *image = loadIndex(index, true);
    if (!image || image->status() == Image::LoadError) {
        return false;
    }

    m_currentIndex = index;
    m_currentImage = image;

    emit paint(image);
    emit navigationChange(index);

    if (image->status() != Image::LoadComplete) {
        image->loadThumbnail();
    }

    preload();

    return true;
}

void Navigator::goIndexUntilSuccess(int index, int delta)
{
    int failedCount = 0;
    while (failedCount < m_playList->count()) {
        // Normalize index
        if (index < 0 || index >= m_playList->count()) {
            index = (index + m_playList->count()) % m_playList->count();
        }

        bool success = goIndex(index);
        if (success) {
            m_reverseNavigation = (delta < 0);
            return;
        } else {
            failedCount ++;
            index += delta;
        }
    }
}

void Navigator::goPrev()
{
    m_player->goPrev();
}

void Navigator::goNext()
{
    m_player->goNext();
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

void Navigator::goAutoNavigation()
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

void Navigator::setPlayer(AbstractNavigationPlayer *player)
{
    delete m_player;
    m_player = player;
}
