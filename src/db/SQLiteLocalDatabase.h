#ifndef SQLITELOCALDATABASE_H
#define SQLITELOCALDATABASE_H

#include <QtSql>

#include "Image.h"
#include "LocalDatabase.h"

class SQLiteLocalDatabase : public LocalDatabase
{
    Q_OBJECT
public:
    SQLiteLocalDatabase();

    QList<PlayListRecord *> queryPlayListRecords();
    QStringList queryImageUrlsForLocalPlayListRecord(int playListId);
    bool insertPlayListRecord(PlayListRecord *playListRecord);
    bool removePlayListRecord(PlayListRecord *playListRecord);
    bool updatePlayListRecord(PlayListRecord *playListRecord);

    int queryImagesCount();
    bool insertImage(Image *image);
    Image *queryImageByHashStr(const QString &hashStr);

    bool insertImageHotspot(ImageHotspot *hotspot);
    bool removeImageHotspot(ImageHotspot *hotspot);
    QList<ImageHotspot *> queryImageHotspots(Image *image);

private:
    QSqlDatabase m_db;
};

#endif // SQLITELOCALDATABASE_H
