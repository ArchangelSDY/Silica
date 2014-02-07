#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QUrl>

#include "Image.h"

class PlayList : public QList<QSharedPointer<Image> >
{
public:
    PlayList() {}
    PlayList(const QList<QUrl> &imageUrls);

    void addPath(const QString &);
    void addPath(const QUrl &);

    void sortByName();
};

#endif // PLAYLIST_H
