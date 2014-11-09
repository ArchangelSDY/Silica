#ifndef LOCALDATABASE_H
#define LOCALDATABASE_H

#include <QList>
#include <QObject>

#include "Image.h"
#include "ImageHotspot.h"
#include "PlayListRecord.h"

class ImageRank;

class LocalDatabase : public QObject
{
    Q_OBJECT
public:
    virtual bool migrate() = 0;

    virtual QList<PlayListRecord *> queryPlayListRecords() = 0;
    virtual QStringList queryImageUrlsForLocalPlayListRecord(int playListId) = 0;
    virtual bool insertPlayListRecord(PlayListRecord *playListRecord) = 0;
    virtual bool removePlayListRecord(PlayListRecord *playListRecord) = 0;
    virtual bool updatePlayListRecord(PlayListRecord *playListRecord) = 0;
    virtual bool insertImagesForPlayListRecord(
        PlayListRecord *playListRecord, const QList<ImagePtr> &images) = 0;
    virtual bool removeImageFromPlayListRecord(
        PlayListRecord *playListRecord, ImagePtr image) = 0;

    virtual int queryImagesCount() = 0;
    virtual bool insertImage(Image *image) = 0;
    virtual Image *queryImageByHashStr(const QString &hashStr) = 0;
    virtual bool updateImageSize(Image *image) = 0;
    virtual QSize queryImageSize(Image *image) = 0;

    virtual bool insertImageHotspot(ImageHotspot *hotspot) = 0;
    virtual bool removeImageHotspot(ImageHotspot *hotspot) = 0;
    virtual QList<ImageHotspot *> queryImageHotspots(Image *image) = 0;

    virtual int queryImageRankValue(Image *image) = 0;
    virtual bool updateImageRank(Image *image, int rank) = 0;

    static LocalDatabase *instance();

protected:
    LocalDatabase(QObject *parent = 0);

    static LocalDatabase *m_instance;
};

#endif
