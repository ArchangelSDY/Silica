#include "playlist/LocalPlayListEntity.h"

#include "db/LocalDatabase.h"
#include "../GlobalConfig.h"
#include "../PlayList.h"

LocalPlayListEntity::LocalPlayListEntity(int type, int id, const QString &name, int count, const QString &coverPath) :
    m_type(type) ,
    m_id(id) ,
    m_name(name) ,
    m_count(count) ,
    m_coverPath(coverPath)
{
}

LocalPlayListEntity::~LocalPlayListEntity()
{
}

int LocalPlayListEntity::count() const
{
    return m_count;
}

QString LocalPlayListEntity::name() const
{
    return m_name;
}

bool LocalPlayListEntity::supportsOption(PlayListEntityOption option) const
{
    return false;
}

QImage LocalPlayListEntity::loadCoverImage()
{
    QString coverFullPath = GlobalConfig::instance()->thumbnailPath() + "/" + m_coverPath;
    return QImage(coverFullPath);
}

#include <QDebug>

PlayList *LocalPlayListEntity::createPlayList()
{
    QList<QUrl> imageUrls =
        LocalDatabase::instance()->queryImageUrlsForLocalPlayListEntity(m_id);
    for (const auto &url : imageUrls) {
        qDebug() << url;
    }
    return new PlayList(imageUrls);
}
