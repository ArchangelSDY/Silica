#ifndef SQLITELOCALDATABASE_H
#define SQLITELOCALDATABASE_H

#include <QScopedPointer>
#include <QtSql>

#include "db/LocalDatabase.h"
#include "image/Image.h"

class SQLiteLocalDatabaseBackgroundWorker;

class SQLiteLocalDatabase : public LocalDatabase
{
    Q_OBJECT
public:
    SQLiteLocalDatabase();
    bool migrate();

    virtual QList<PlayListEntityData> queryPlayListEntities(int type) override;
    virtual PlayListEntityData queryPlayListEntity(int playListId) override;
    virtual bool insertPlayListEntity(PlayListEntityData &data) override;
    virtual bool removePlayListEntity(int playListId) override;
    virtual bool updatePlayListEntity(const PlayListEntityData &data) override;

    virtual QList<QUrl> queryLocalPlayListEntityImageUrls(int playListId) override;
    virtual bool insertLocalPlayListEntityImageUrls(
        int playListId, const QList<QUrl> &imageUrls) override;
    virtual bool removeLocalPlayListEntityImageUrls(
        int playListId, const QList<QUrl> &imageUrls) override;

    int queryImagesCount();
    bool insertImage(Image *image);
    bool insertImages(const ImageList &images);
    Image *queryImageByHashStr(const QString &hashStr);
    bool updateImageUrl(const QUrl &oldUrl, const QUrl &newUrl);

    int queryImageRankValue(Image *image);
    bool updateImageRank(Image *image, int rank);

    int insertPluginPlayListProviderType(const QString &name);
    int queryPluginPlayListProviderType(const QString &name);

    bool saveTaskProgressTimeConsumption(const QString &key,
                                         qint64 timeConsumption);
    qint64 queryTaskProgressTimeConsumption(const QString &key);

private:
    static const int PLUGIN_PLAYLIST_PROVIDER_TYPE_OFFSET = 100;

    QSqlDatabase m_db;
};

#endif // SQLITELOCALDATABASE_H
