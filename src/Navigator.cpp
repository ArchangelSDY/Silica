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
    m_currentImage(nullptr) ,
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
        int index = m_currentIndex + delta;
        ImagePtr image = m_playList->at(resolveIndex(index));
        if (!image) {
            continue;
        }

        connect(image.data(), &Image::loaded, this, [index, cachedImages = m_cachedImages](QSharedPointer<ImageData> imageData) {
            cachedImages->insert(index, imageData);
        }, Qt::UniqueConnection);

        image->load(Image::LoadPriority::NormalPriority);
    }
}

void Navigator::setPlayList(QSharedPointer<PlayList> playList)
{
    if (!playList || m_playList == playList) {
        return;
    }

    reset();

    m_playList = playList;
    connect(m_playList.data(), &PlayList::itemChanged, this, &Navigator::playListItemChange);
    connect(m_playList.data(), &PlayList::itemsChanged, this, &Navigator::reloadPlayList);
    connect(m_playList.data(), &PlayList::itemsAppended, this, &Navigator::playListAppended);
    emit playListChange(m_playList);

    goIndex(0);

    if (m_player) {
        m_player->reset();
    }
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

int Navigator::resolveIndex(int index)
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

    return index;
}

bool Navigator::goIndex(int index, bool forceReload)
{
    if (m_playList.isNull() || index < 0 || index >= m_playList->count()) {
        return false;
    }

    if (!forceReload && index == m_currentIndex) {
        return true;
    }

    ImagePtr image = m_playList->at(resolveIndex(index));
    if (!image) {
        return false;
    }

    m_currentIndex = index;
    m_currentUuid = image->uuid();
    m_currentImage = image;

    emit navigationChange(index);
    
    connect(image.data(), &Image::thumbnailLoaded, this, &Navigator::thumbnailLoaded,
            static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::QueuedConnection));
    connect(image.data(), &Image::loaded, this, &Navigator::imageLoaded,
            static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::QueuedConnection));

    image->loadThumbnail();
    image->load(Image::LoadPriority::NormalPriority, forceReload);

    preload();
    checkContinueProvide();

    return true;
}

bool Navigator::goUuid(const QUuid &uuid, bool forceReload)
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
        return goIndex(index, forceReload);
    } else {
        return goIndex(0, forceReload);
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

    QString cmpGroup = m_playList->groupNameOf(currentImage());
    int initIndex = m_currentIndex;
    int index = m_currentIndex;
    bool isInPrevGroup = false;

    do {
        --index;
        if (index < 0 || index >= m_playList->count()) {
            index = (index + m_playList->count()) % m_playList->count();
        }

        const ImagePtr &image = m_playList->at(index);
        QString group = m_playList->groupNameOf(image.data());
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

    QString initGroup = m_playList->groupNameOf(currentImage());
    int initIndex = m_currentIndex;
    int index = m_currentIndex;

    do {
        ++index;
        if (index < 0 || index >= m_playList->count()) {
            index = (index + m_playList->count()) % m_playList->count();
        }

        const ImagePtr &image = m_playList->at(index);
        QString group = m_playList->groupNameOf(image.data());
        if (group != initGroup) {
            break;
        }
    } while (index != initIndex);

    goIndex(index);
}

void Navigator::repaint()
{
    ImagePtr image = m_currentImage;
    if (!image) {
        return;
    }

    QSharedPointer<ImageData> imageData = image->image().toStrongRef();
    if (!imageData) {
        return;
    }

    emit paint(imageData);
}

void Navigator::imageLoaded(QSharedPointer<ImageData> imageData)
{
    Image *loadedImage = static_cast<Image*>(QObject::sender());
    if (loadedImage) {
        if (loadedImage == m_currentImage) {
            m_cachedImages->insert(m_currentIndex, imageData);
            emit paint(imageData);
        }
    }
}

void Navigator::thumbnailLoaded(QSharedPointer<QImage> thumbnail)
{
    Image *image = static_cast<Image*>(QObject::sender());
    if (image && image == m_currentImage && !thumbnail->isNull()) {
        emit paintThumbnail(thumbnail);
    }
}

Image *Navigator::currentImage() const
{
    return currentImagePtr().data();
}

ImagePtr Navigator::currentImagePtr() const
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

    m_player.reset(player);

    m_player->onEnter();
}

void Navigator::checkContinueProvide()
{
    if (m_playList->count() - m_currentIndex <= MAX_PRELOAD
            && m_playList->record()) {
        m_playList->record()->continueProvide();
    }
}
