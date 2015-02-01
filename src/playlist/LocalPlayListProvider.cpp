#include "LocalPlayListProvider.h"

#include "db/LocalDatabase.h"

LocalPlayListProvider::LocalPlayListProvider(QObject *parent) :
    PlayListProvider(parent)
{
}

LocalPlayListProvider::~LocalPlayListProvider()
{
}

QString LocalPlayListProvider::typeName() const
{
    return QObject::tr("Local");
}

void LocalPlayListProvider::request(const QString &name,
                                    const QVariantHash &extra)
{
    int id = extra.value("id").toInt();
    QList<QUrl> imageUrls =
        LocalDatabase::instance()->queryImageUrlsForLocalPlayListRecord(id);
    QList<QVariantHash> extraInfos;

    emit gotItems(imageUrls, extraInfos);
}

bool LocalPlayListProvider::isImagesReadOnly() const
{
    return false;
}

bool LocalPlayListProvider::insertImages(const PlayListRecordInfo &plrInfo,
                                         const QList<ImageInfo> &imgInfos)
{
    return LocalDatabase::instance()->insertImagesForLocalPlayListProvider(
        plrInfo, imgInfos);
}

bool LocalPlayListProvider::removeImages(const PlayListRecordInfo &plrInfo,
                                         const QList<ImageInfo> &imgInfos)
{
    return LocalDatabase::instance()->removeImagesForLocalPlayListProvider(
        plrInfo, imgInfos);
}

void LocalPlayListProvider::onPlayListRecordCreated(
        const PlayListRecordInfo &plrInfo, const QList<ImageInfo> &imgInfos)
{
    LocalDatabase::instance()->insertImagesForLocalPlayListProvider(
        plrInfo, imgInfos);
}
