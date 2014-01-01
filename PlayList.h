#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QUrl>

class PlayList : public QList<QUrl>
{
public:
    void addPath(const QString &);
    void addPath(const QUrl &);
};

#endif // PLAYLIST_H
