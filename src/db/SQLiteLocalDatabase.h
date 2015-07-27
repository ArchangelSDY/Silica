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
        const PlayListRecord &record, const ImageList &images);
    bool removeImagesForLocalPlayListProvider(
        const PlayListRecord &record, const ImageList &images);

    int queryImagesCount();
    bool insertImage(Image *image);
    Image *queryImageByHashStr(const QString &hashStr);
    bool updateImageSize(Image *image);
    QSize queryImageSize(Image *image);
    bool updateImageUrlByHashStr(const QString &hashStr,
                                 const QUrl &newUrl);
    bool updateImageUrl(const QUrl &oldUrl, const QUrl &newUrl);

    bool insertImageHotspot(ImageHotspot *hotspot);
    bool removeImageHotspot(ImageHotspot *hotspot);
    QList<ImageHotspot *> queryImageHotspots(Image *image);

    int queryImageRankValue(Image *image);
    bool updateImageRank(Image *image, int rank);

    int insertPluginPlayListProviderType(const QString &name);
    int queryPluginPlayListProviderType(const QString &name);

    bool saveTaskProgressTimeConsumption(const QString &key,
                                         qint64 timeConsumption);
    qint64 queryTaskProgressTimeConsumption(const QString &key);

private:
    QSqlDatabase m_db;

    static const int PLUGIN_PLAYLIST_PROVIDER_TYPE_OFFSET = 100;
};

#endif // SQLITELOCALDATABASE_H
