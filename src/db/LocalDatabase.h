#ifndef LOCALDATABASE_H
#define LOCALDATABASE_H

#include <QList>
#include <QObject>

#include "image/Image.h"
#include "image/ImageHotspot.h"
#include "playlist/PlayListRecordInfo.h"
#include "playList/PlayListRecord.h"
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
        const PlayListRecordInfo &plrInfo, const QList<ImageInfo> &imgInfos) = 0;
    virtual bool removeImagesForLocalPlayListProvider(
        const PlayListRecordInfo &plrInfo, const QList<ImageInfo> &imgInfos) = 0;

    virtual int queryImagesCount() = 0;
    virtual bool insertImage(const ImageInfo &info) = 0;
    virtual Image *queryImageByHashStr(const QString &hashStr) = 0;
    virtual bool updateImageSize(Image *image) = 0;
    virtual QSize queryImageSize(Image *image) = 0;

    virtual bool insertImageHotspot(ImageHotspot *hotspot) = 0;
    virtual bool removeImageHotspot(ImageHotspot *hotspot) = 0;
    virtual QList<ImageHotspot *> queryImageHotspots(Image *image) = 0;

    virtual int queryImageRankValue(Image *image) = 0;
    virtual bool updateImageRank(Image *image, int rank) = 0;

    virtual int insertPluginPlayListProviderType(const QString &name) = 0;
    virtual int queryPluginPlayListProviderType(const QString &name) = 0;

    static LocalDatabase *instance();

protected:
    LocalDatabase(QObject *parent = 0);

    static LocalDatabase *m_instance;
};

#endif
