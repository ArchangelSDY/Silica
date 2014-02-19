#ifndef LOCALDATABASE_H
#define LOCALDATABASE_H

#include <QList>
#include <QObject>

#include "PlayListRecord.h"

class LocalDatabase : public QObject
{
    Q_OBJECT
public:
    virtual QList<PlayListRecord> queryPlayListRecords() = 0;
    virtual QStringList queryImageUrlsForPlayList(const QString &name) = 0;
    virtual bool insertPlayListRecord(PlayListRecord *playListRecord) = 0;

    static LocalDatabase *instance() { return m_instance; }

protected:
    LocalDatabase(QObject *parent = 0);

    static LocalDatabase *m_instance;
};

#endif
