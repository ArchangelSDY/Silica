#include "Navigator.h"

#include "image/caches/ImagesCache.h"
#include "navigation/AbstractNavigationPlayer.h"
#include "navigation/NormalNavigationPlayer.h"
#include "playlist/PlayListRecord.h"


const int Navigator::MAX_PRELOAD = 5;
const int Navigator::FAST_AUTO_NAVIGATION_INTERVAL = 200;
const int Navigator::MEDIUM_AUTO_NAVIGATION_INTERVAL = 500;
const int Navigator::SLOW_AUTO_NAVIGATION_INTERVAL = 1000;

Navigator::Navigator(QSharedPointer<ImagesCache> imagesCache, QObject *parent) :
    QObject(parent) ,
    m_currentIndex(-1) ,
    m_currentUuid(QUuid()) ,
    m_reverseNavigation(false) ,
    m_isLooping(true) ,
    m_cachedImages(imagesCache) ,
    m_playList(0) ,
    m_player(nullptr) ,
    m_basket(new PlayList())
{
    m_autoNavigationTimer.setInterval(Navigator::MEDIUM_AUTO_NAVIGATION_INTERVAL);
    connect(&m_autoNavigationTimer, SIGNAL(timeout()),
            this, SLOT(goAutoNavigation()));
}

Navigator::~Navigator()
{
}

void Navigator::reset()
{
    m_cachedImages->clear();
    m_currentImage = 0;
    m_currentIndex = -1;
    m_currentUuid = QUuid();
    m_reverseNavigation = false;
    m_playList.reset();
}

void Navigator::preload()
{
    for (int i = 1; i <= MAX_PRELOAD; ++i) {
        int delta = m_reverseNavigation ? (-i) : i;
        // Load but not paint
        loadIndex(m_currentIndex + delta, false);
    }
}

void Navigator::setPlayList(QSharedPointer<PlayList> playList)
{
    if (!playList) {
        return;
    }

    reset();

    m_playList = playList;
    connect(m_playList.data(), SIGNAL(itemsChanged()), this, SLOT(reloadPlayList()));
    connect(m_playList.data(), SIGNAL(itemsAppended(int)),
            this, SLOT(playListAppended(int)));
    emit playListChange(m_playList);

    goIndex(0);

    m_player->reset();
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
    emit playListChange(m_playList);

    // Cached index may be incorrect after filtering, clear cache.
    m_cachedImages->clear();

    // Try to find new index of current image and reload it.
    goUuid(m_currentUuid, true);
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

    // Double check here in case playlist has no images, eg.
    if (index < 0 || index >= m_playList->count()) {
        return 0;
    }

    Image *image = m_cachedImages->at(index);

    if (!image) {
        image = (*m_playList)[index].data();
        m_cachedImages->insert(index, image);
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

bool Navigator::goIndex(int index, bool forceReloadCurrent)
{
    if (m_playList.isNull() || index < 0 || index >= m_playList->count()) {
        return false;
    }

    if (!forceReloadCurrent && index == m_currentIndex) {
        return true;
    }

    // Load and paint
    Image *image = loadIndex(index, true);
    if (!image || image->status() == Image::LoadError) {
        return false;
    }

    m_currentIndex = index;
    m_currentUuid = image->uuid();
    m_currentImage = image;

    emit paint(image);
    emit navigationChange(index);

    if (image->status() != Image::LoadComplete) {
        image->loadThumbnail();
    }

    preload();
    checkContinueProvide();

    return true;
}

bool Navigator::goUuid(const QUuid &uuid, bool forceReloadCurrent)
{
    if (uuid.isNull()) {
        return false;
    }

    int index = 0;
    for (; index < m_playList->count(); ++index) {
        if (m_playList->at(index)->uuid() == uuid) {
            break;
        }
    }

    if (index < m_playList->count()) {
        return goIndex(index, forceReloadCurrent);
    } else {
        return goIndex(0, forceReloadCurrent);
    }
}

void Navigator::goIndexUntilSuccess(int index, int delta)
{
    int failedCount = 0;
    while (failedCount < m_playList->count()) {
        // Normalize index in loop mode
        if (index < 0 || index >= m_playList->count()) {
            if (m_isLooping) {
                index = (index + m_playList->count()) % m_playList->count();
            } else {
                return;
            }
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

void Navigator::goPrevGroup()
{
    if (!m_playList || m_playList->count() == 0) {
        return;
    }

    int cmpGroup = m_playList->groupForImage(m_currentImage);
    int initIndex = m_currentIndex;
    int index = m_currentIndex;
    bool isInPrevGroup = false;

    do {
        --index;
        if (index < 0 || index >= m_playList->count()) {
            index = (index + m_playList->count()) % m_playList->count();
        }

        const ImagePtr &image = m_playList->at(index);
        int group = m_playList->groupForImage(image.data());
        if (group != cmpGroup) {
            if (!isInPrevGroup) {
                // Now at the end of prev group. Try to find the head.
                isInPrevGroup = true;
                cmpGroup = group;
            } else {
                break;
            }
        }
    } while (index != initIndex);

    // Now at the end of prev prev group. Next is the head of prev group.
    ++index;
    if (index < 0 || index >= m_playList->count()) {
        index = (index + m_playList->count()) % m_playList->count();
    }

    goIndex(index);
}

void Navigator::goNextGroup()
{
    if (!m_playList || m_playList->count() == 0) {
        return;
    }

    int initGroup = m_playList->groupForImage(m_currentImage);
    int initIndex = m_currentIndex;
    int index = m_currentIndex;

    do {
        ++index;
        if (index < 0 || index >= m_playList->count()) {
            index = (index + m_playList->count()) % m_playList->count();
        }

        const ImagePtr &image = m_playList->at(index);
        int group = m_playList->groupForImage(image.data());
        if (group != initGroup) {
            break;
        }
    } while (index != initIndex);

    goIndex(index);
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

ImagePtr Navigator::currentImagePtr()
{
    if (m_playList &&
        m_currentIndex >= 0 && m_currentIndex < m_playList->count()) {
        return m_playList->at(m_currentIndex);
    } else {
        return ImagePtr();
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
    if (m_player) {
        m_player->onLeave();
    }

    m_player = player;

    m_player->onEnter();
}

void Navigator::checkContinueProvide()
{
    if (m_playList->count() - m_currentIndex <= MAX_PRELOAD
            && m_playList->record()) {
        m_playList->record()->continueProvide();
    }
}
