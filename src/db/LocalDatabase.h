#ifndef LOCALDATABASE_H
#define LOCALDATABASE_H

#include <QFuture>
#include <QList>
#include <QObject>

#include "db/PlayListEntityData.h"
#include "image/Image.h"
#include "playlist/PlayListProviderFactory.h"

class ImageRank;

class LocalDatabase : public QObject
{
    Q_OBJECT
public:
    virtual bool migrate() = 0;

    virtual QList<PlayListEntityData> queryPlayListEntities(int type) = 0;
    virtual PlayListEntityData queryPlayListEntity(int playListId) = 0;
    virtual bool insertPlayListEntity(PlayListEntityData &data) = 0;
    virtual bool removePlayListEntity(int playListId) = 0;
    virtual bool updatePlayListEntity(const PlayListEntityData &data) = 0;

    virtual QList<QUrl> queryLocalPlayListEntityImageUrls(int playListId) = 0;
    virtual bool insertLocalPlayListEntityImageUrls(
        int playListId, const QList<QUrl> &imageUrls) = 0;
    virtual bool removeLocalPlayListEntityImageUrls(
        int playListId, const QList<QUrl> &imageUrls) = 0;

    virtual int queryImagesCount() = 0;
    virtual bool insertImage(Image *image) = 0;
    virtual bool insertImages(const ImageList &images) = 0;
    virtual Image *queryImageByHashStr(const QString &hashStr) = 0;
    virtual bool updateImageUrl(const QUrl &oldUrl,
                                const QUrl &newUrl) = 0;

    virtual int queryImageRankValue(Image *image) = 0;
    virtual bool updateImageRank(Image *image, int rank) = 0;

    virtual int insertPluginPlayListProviderType(const QString &name) = 0;
    virtual int queryPluginPlayListProviderType(const QString &name) = 0;

    virtual bool saveTaskProgressTimeConsumption(const QString &key,
                                    qint64 timeConsumption) = 0;
    virtual qint64 queryTaskProgressTimeConsumption(const QString &key) = 0;

    static LocalDatabase *instance();

protected:
    LocalDatabase(QObject *parent = 0);

    static LocalDatabase *m_instance;
};

#endif
