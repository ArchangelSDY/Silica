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
    bool migrate();

    QList<PlayListRecord *> queryPlayListRecords();
    QStringList queryImageUrlsForLocalPlayListRecord(int playListId);
    bool insertPlayListRecord(PlayListRecord *playListRecord);
    bool removePlayListRecord(PlayListRecord *playListRecord);
    bool updatePlayListRecord(PlayListRecord *playListRecord);
    bool insertImagesForPlayListRecord(
        PlayListRecord *playListRecord, const QList<ImagePtr> &images);
    bool removeImageFromPlayListRecord(
        PlayListRecord *playListRecord, ImagePtr image);

    int queryImagesCount();
    bool insertImage(Image *image);
    Image *queryImageByHashStr(const QString &hashStr);
    bool updateImageSize(Image *image);
    QSize queryImageSize(Image *image);

    bool insertImageHotspot(ImageHotspot *hotspot);
    bool removeImageHotspot(ImageHotspot *hotspot);
    QList<ImageHotspot *> queryImageHotspots(Image *image);

    int queryImageRankValue(Image *image);
    bool updateImageRank(Image *image, int rank);

private:
    QSqlDatabase m_db;
};

#endif // SQLITELOCALDATABASE_H
