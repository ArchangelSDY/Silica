#ifndef LOCALDATABASE_H
#define LOCALDATABASE_H

#include <QList>
#include <QObject>

#include "Image.h"
#include "PlayListRecord.h"

class LocalDatabase : public QObject
{
    Q_OBJECT
public:
    virtual QList<PlayListRecord *> queryPlayListRecords() = 0;
    virtual QStringList queryImageUrlsForPlayList(int playListId) = 0;
    virtual bool insertPlayListRecord(PlayListRecord *playListRecord) = 0;
    virtual bool removePlayListRecord(PlayListRecord *playListRecord) = 0;
    virtual bool updatePlayListRecord(PlayListRecord *playListRecord) = 0;

    virtual int queryImagesCount() = 0;
    virtual bool insertImage(Image *image) = 0;
    virtual Image *queryImageByHashStr(const QString &hashStr) = 0;

    static LocalDatabase *instance();

protected:
    LocalDatabase(QObject *parent = 0);

    static LocalDatabase *m_instance;
};

#endif
