#ifndef LOCALDATABASE_H
#define LOCALDATABASE_H

#include <QList>
#include <QObject>

#include "image/Image.h"
#include "image/ImageHotspot.h"
#include "playlist/PlayListRecord.h"
#include "playlist/PlayListProviderFactory.h"

class ImageRank;

class LocalDatabase : public QObject
{
    Q_OBJECT
public:
    virtual bool migrate() = 0;

    virtual QList<PlayListRecord *> queryPlayListRecords() = 0;
    virtual QList<QUrl> queryImageUrlsForLocalPlayListRecord(int playListId) = 0;
    virtual bool insertPlayListRecord(PlayListRecord *playListRecord) = 0;
    virtual bool removePlayListRecord(PlayListRecord *playListRecord) = 0;
    virtual bool updatePlayListRecord(PlayListRecord *playListRecord) = 0;
    virtual bool insertImagesForLocalPlayListProvider(
        const PlayListRecord &record, const ImageList &images) = 0;
    virtual bool removeImagesForLocalPlayListProvider(
        const PlayListRecord &record, const ImageList &images) = 0;

    virtual int queryImagesCount() = 0;
    virtual bool insertImage(Image *image) = 0;
    virtual Image *queryImageByHashStr(const QString &hashStr) = 0;
    virtual bool updateImageUrl(const QUrl &oldUrl,
                                const QUrl &newUrl) = 0;

    virtual bool insertImageHotspot(ImageHotspot *hotspot) = 0;
    virtual bool removeImageHotspot(ImageHotspot *hotspot) = 0;
    virtual QList<ImageHotspot *> queryImageHotspots(Image *image) = 0;

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
