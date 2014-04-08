#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QUrl>

#include "Image.h"
#include "PlayListRecord.h"

class PlayListEventEmitter : public QObject
{
    Q_OBJECT
signals:
    void itemsAppended();
};

class PlayList : public QList<QSharedPointer<Image> >
{
public:
    PlayList();
    PlayList(const QList<QUrl> &imageUrls);
    ~PlayList();

    PlayListRecord *record() { return m_record; }
    void setRecord(PlayListRecord *record) { m_record = record; }

    void addPath(const QString &);
    void addPath(const QUrl &);

    void sortByName();
    void sortByAspectRatio();

    PlayListEventEmitter *eventEmitter() { return m_eventEmitter; }

private:
    PlayListRecord *m_record;
    PlayListEventEmitter *m_eventEmitter;
};

#endif // PLAYLIST_H
