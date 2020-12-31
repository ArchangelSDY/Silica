#include "playlist/LocalPlayListEntity.h"

#include "db/LocalDatabase.h"
#include "../GlobalConfig.h"
#include "../PlayList.h"

LocalPlayListEntity::LocalPlayListEntity(LocalPlayListProvider *provider,
        int type, int id, const QString &name, int count, const QString &coverPath) :
    PlayListEntity(provider) ,
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

QList<QUrl> LocalPlayListEntity::loadImageUrls()
{
    return LocalDatabase::instance()->queryImageUrlsForLocalPlayListEntity(m_id);
}

void LocalPlayListEntity::setCoverImagePath(const QString &path)
{
    m_coverPath = path;
}
