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

bool LocalPlayListProvider::canContinueProvide() const
{
    return false;
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

bool LocalPlayListProvider::insertImages(const PlayListRecord &record,
                                         const ImageList &images)
{
    return LocalDatabase::instance()->insertImagesForLocalPlayListProvider(
        record, images);
}

bool LocalPlayListProvider::removeImages(const PlayListRecord &record,
                                         const ImageList &images)
{
    return LocalDatabase::instance()->removeImagesForLocalPlayListProvider(
        record, images);
}

void LocalPlayListProvider::onPlayListRecordCreated(
        const PlayListRecord &record, const ImageList &images)
{
    LocalDatabase::instance()->insertImagesForLocalPlayListProvider(
        record, images);
}
