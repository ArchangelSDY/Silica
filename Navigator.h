#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QObject>

#include "Image.h"
#include "PlayList.h"

class Navigator : public QObject
{
    Q_OBJECT
public:
    explicit Navigator(QObject *parent = 0);

    void setPlayList(const PlayList&);

    void goIndex(int index);
    void goPrev();
    void goNext();

    Image* currentImage() const { return m_currentImage; }

signals:
    void paint(Image *image);

public slots:
    void imageLoaded();

private:
    Image* loadIndex(int index, bool shouldPaint);
    void preload();
    void reset();

    int m_currentIndex;
    Image* m_currentImage;
    bool m_reverseNavigation;
    QCache<QUrl, Image> m_cachedImages;
    PlayList m_playlist;
};

#endif // NAVIGATOR_H
