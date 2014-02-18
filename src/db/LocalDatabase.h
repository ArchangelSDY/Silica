#ifndef LOCALDATABASE_H
#define LOCALDATABASE_H

#include <QList>
#include <QObject>

#include "PlayListRecord.h"

class LocalDatabase : public QObject
{
    Q_OBJECT
public:
    virtual QList<PlayListRecord> playListRecords() = 0;
    virtual void savePlayListRecord(const PlayListRecord &playListRecord) = 0;

    static LocalDatabase *instance() { return m_instance; }

protected:
    LocalDatabase(QObject *parent = 0);

    static LocalDatabase *m_instance;
};

#endif
