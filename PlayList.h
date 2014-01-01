#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QUrl>

class PlayList : public QList<QUrl>
{
public:
    PlayList() {}
    PlayList(const QList<QUrl> &imageUrls);

    void addPath(const QString &);
    void addPath(const QUrl &);
};

#endif // PLAYLIST_H
