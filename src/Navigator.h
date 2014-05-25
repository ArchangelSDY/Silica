#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QObject>

#include "Image.h"
#include "ImagesCache.h"
#include "PlayList.h"

class AbstractNavigationPlayer;

class Navigator : public QObject
{
    Q_OBJECT
public:
    enum Direction {
        NormalDirection = 0,
        ReverseDirection = 1,
    };

    static const int FAST_AUTO_NAVIGATION_INTERVAL = 200;
    static const int MEDIUM_AUTO_NAVIGATION_INTERVAL = 500;
    static const int SLOW_AUTO_NAVIGATION_INTERVAL = 1000;

    explicit Navigator(QObject *parent = 0);
    ~Navigator();

    PlayList *playList() { return m_playList; }

    void goPrev();
    void goNext();
    void goFirst();
    void goLast();
    void goIndexUntilSuccess(int index, int delta);

    Image* currentImage() const { return m_currentImage; }
    int currentIndex() { return m_currentIndex; }

    bool isAutoNavigating() const { return m_autoNavigationTimer.isActive(); }
    int autoNavigationInterval() const { return m_autoNavigationTimer.interval(); }
    void startAutoNavigation(Direction direction = NormalDirection);
    void stopAutoNavigation();

    bool isLooping() const { return m_isLooping; }

    void setPlayer(AbstractNavigationPlayer *player);
    AbstractNavigationPlayer *player() { return m_player; }

signals:
    void paint(Image *image);
    void paintThumbnail(Image *image);
    void playListChange();
    void playListAppend(int start);
    void navigationChange(int index);
    void fitInView(QRectF rect);

public slots:
    void setPlayList(PlayList *, bool takeOwnership = false);
    void reloadPlayList();

    bool goIndex(int index);

    void imageLoaded();
    void thumbnailLoaded();

    void setAutoNavigationInterval(int msec);
    void setLoop(bool loop);

private slots:
    void playListAppended(int start);

    void goAutoNavigation();

private:
    Image* loadIndex(int index, bool shouldPaint);

    void preload();
    void reset();

    int m_currentIndex;
    Image* m_currentImage;
    bool m_reverseNavigation;
    bool m_isLooping;
    ImagesCache m_cachedImages;
    PlayList *m_playList;
    bool m_ownPlayList;
    QTimer m_autoNavigationTimer;

    AbstractNavigationPlayer *m_player;
};

#endif // NAVIGATOR_H
