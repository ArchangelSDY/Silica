#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QUrl>

#include "Image.h"
#include "PlayListRecord.h"

class PlayList : public QList<QSharedPointer<Image> >
{
public:
    PlayList();
    PlayList(const QList<QUrl> &imageUrls);

    PlayListRecord *record() { return m_record; }
    void setRecord(PlayListRecord *record) { m_record = record; }

    void addPath(const QString &);
    void addPath(const QUrl &);

    void sortByName();
    void sortByAspectRatio();

private:
    PlayListRecord *m_record;
};

#endif // PLAYLIST_H
