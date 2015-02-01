#ifndef SQLITELOCALDATABASE_H
#define SQLITELOCALDATABASE_H

#include <QtSql>

#include "db/LocalDatabase.h"
#include "image/Image.h"

class SQLiteLocalDatabase : public LocalDatabase
{
    Q_OBJECT
public:
    SQLiteLocalDatabase();
    bool migrate();

    QList<PlayListRecord *> queryPlayListRecords();
    QList<QUrl> queryImageUrlsForLocalPlayListRecord(int playListId);
    bool insertPlayListRecord(PlayListRecord *playListRecord);
    bool removePlayListRecord(PlayListRecord *playListRecord);
    bool updatePlayListRecord(PlayListRecord *playListRecord);
    bool insertImagesForLocalPlayListProvider(
        const PlayListRecordInfo &plrInfo, const QList<ImageInfo> &imgInfos);
    bool removeImagesForLocalPlayListProvider(
        const PlayListRecordInfo &plrInfo, const QList<ImageInfo> &imgInfos);

    int queryImagesCount();
    bool insertImage(const ImageInfo &imageInfo);
    Image *queryImageByHashStr(const QString &hashStr);
    bool updateImageSize(Image *image);
    QSize queryImageSize(Image *image);

    bool insertImageHotspot(ImageHotspot *hotspot);
    bool removeImageHotspot(ImageHotspot *hotspot);
    QList<ImageHotspot *> queryImageHotspots(Image *image);

    int queryImageRankValue(Image *image);
    bool updateImageRank(Image *image, int rank);

    int insertPluginPlayListProviderType(const QString &name);
    int queryPluginPlayListProviderType(const QString &name);

private:
    QSqlDatabase m_db;

    static const int PLUGIN_PLAYLIST_PROVIDER_TYPE_OFFSET = 100;
};

#endif // SQLITELOCALDATABASE_H
