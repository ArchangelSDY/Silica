#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QObject>
#include <QSharedPointer>
#include <QUuid>

#include "Image.h"
#include "PlayList.h"

class AbstractNavigationPlayer;
class ImagesCache;

class Navigator : public QObject
{
    Q_OBJECT
public:
    enum Direction {
        NormalDirection = 0,
        ReverseDirection = 1,
    };

    static const int MAX_PRELOAD;
    static const int FAST_AUTO_NAVIGATION_INTERVAL;
    static const int MEDIUM_AUTO_NAVIGATION_INTERVAL;
    static const int SLOW_AUTO_NAVIGATION_INTERVAL;

    explicit Navigator(QSharedPointer<ImagesCache> imagesCache, QObject *parent = 0);
    ~Navigator();

    QSharedPointer<PlayList> playList() { return m_playList; }

    void goPrev();
    void goNext();
    void goFirst();
    void goLast();
    void goIndexUntilSuccess(int index, int delta);
    void goPrevGroup();
    void goNextGroup();

    Image *currentImage() const;
    ImagePtr currentImagePtr() const;
    int currentIndex() { return m_currentIndex; }

    bool isAutoNavigating() const { return m_autoNavigationTimer.isActive(); }
    int autoNavigationInterval() const { return m_autoNavigationTimer.interval(); }
    void startAutoNavigation(Direction direction = NormalDirection);
    void stopAutoNavigation();

    bool isLooping() const { return m_isLooping; }

    void setPlayer(AbstractNavigationPlayer *player);
    AbstractNavigationPlayer *player() { return m_player.data(); }

    QSharedPointer<PlayList> basket() { return m_basket; }

signals:
    void paint(Image *image);
    void paintThumbnail(Image *image);
    void playListChange(QSharedPointer<PlayList>);
    void playListAppend(int start);
    void navigationChange(int index);
    void focusOnRect(QRectF rect);

public slots:
    // TODO(sdy): separate `setOwnedPlayList()` out
    void setPlayList(QSharedPointer<PlayList> playlist);
    void reloadPlayList();

    /**
     * @brief Go to specific image of `index`.
     * @param index Image index.
     * @param forceReload If true, ignore all quick paths and do a full reload even if index not changed.
     * @return true if loaded successfully.
     */
    bool goIndex(int index, bool forceReload = false);

    /**
     * @brief Go to specific image of `uuid`.
     * @param uuid Image uuid.
     * @param forceReload If true, ignore all quick paths and do a full reload even if index not changed.
     * @return true if loaded successfully.
     */
    bool goUuid(const QUuid &uuid, bool forceReload = false);

    void imageLoaded();
    void thumbnailLoaded(QSharedPointer<QImage> thumbnail);

    void setAutoNavigationInterval(int msec);
    void setLoop(bool loop);

private slots:
    void playListAppended(int start);

    void goAutoNavigation();

private:
    ImagePtr loadIndex(int index, bool shouldPaint, bool forceReload = false);

    void preload();
    void reset();

    void checkContinueProvide();

    int m_currentIndex;
    QUuid m_currentUuid;
    ImagePtr m_currentImage;
    bool m_reverseNavigation;
    bool m_isLooping;
    QSharedPointer<ImagesCache> m_cachedImages;
    QSharedPointer<PlayList> m_playList;
    QTimer m_autoNavigationTimer;

    QScopedPointer<AbstractNavigationPlayer> m_player;

    QSharedPointer<PlayList> m_basket;
};

#endif // NAVIGATOR_H
