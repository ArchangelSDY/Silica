#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QObject>

#include "Image.h"
#include "ImagesCache.h"
#include "PlayList.h"

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

    PlayList *playList() { return &m_playlist; }

    void goPrev();
    void goNext();
    void goFirst();
    void goLast();

    Image* currentImage() const { return m_currentImage; }
    int currentIndex() { return m_currentIndex; }

    bool isAutoNavigating() const { return m_autoNavigationTimer.isActive(); }
    int autoNavigationInterval() const { return m_autoNavigationTimer.interval(); }
    void startAutoNavigation(Direction direction = NormalDirection);
    void stopAutoNavigation();

signals:
    void paint(Image *image);
    void paintThumbnail(Image *image);
    void playListChange(PlayList playList);
    void playListAppend(PlayList appended);
    void navigationChange(int index);

public slots:
    void setPlayList(const PlayList &);
    void appendPlayList(const PlayList &);
    void clearPlayList();

    void goIndex(int index);

    void imageLoaded();
    void thumbnailLoaded();

    void setAutoNavigationInterval(int msec);

private slots:
    void goFastForward();

private:
    Image* loadIndex(int index, bool shouldPaint);
    void preload();
    void reset();

    int m_currentIndex;
    Image* m_currentImage;
    bool m_reverseNavigation;
    ImagesCache m_cachedImages;
    PlayList m_playlist;
    QTimer m_autoNavigationTimer;
};

#endif // NAVIGATOR_H
