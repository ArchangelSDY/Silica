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

