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
    explicit Navigator(QObject *parent = 0);

    const PlayList &playList() const { return m_playlist; }

    void goPrev();
    void goNext();
    void goFirst();
    void goLast();

    Image* currentImage() const { return m_currentImage; }
    int currentIndex() { return m_currentIndex; }


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

private:
    Image* loadIndex(int index, bool shouldPaint);
    void preload();
    void reset();

    int m_currentIndex;
    Image* m_currentImage;
    bool m_reverseNavigation;
    ImagesCache m_cachedImages;
    PlayList m_playlist;
};

#endif // NAVIGATOR_H
