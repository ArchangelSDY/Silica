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
    void paint(QImage image);

public slots:
    void imageLoaded();

private:
    int m_currentIndex;
    Image* m_currentImage;
    QCache<QUrl, Image> m_cachedImages;
    PlayList m_playlist;
};

#endif // NAVIGATOR_H
