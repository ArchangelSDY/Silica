#include "playlist/LocalPlayListEntity.h"

#include "db/LocalDatabase.h"
#include "../GlobalConfig.h"
#include "../PlayList.h"

LocalPlayListEntity::LocalPlayListEntity(LocalPlayListProvider *provider,
        int id, const QString &name, int count, const QString &coverPath) :
    PlayListEntity(provider) ,
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
    return option == PlayListEntityOption::AddImageUrls
        || option == PlayListEntityOption::RemoveImageUrls;
}

QImage LocalPlayListEntity::loadCoverImage()
{
    QString coverFullPath = GlobalConfig::instance()->thumbnailPath() + "/" + m_coverPath;
    return QImage(coverFullPath);
}

QList<QUrl> LocalPlayListEntity::loadImageUrls()
{
    return LocalDatabase::instance()->queryLocalPlayListEntityImageUrls(m_id);
}

void LocalPlayListEntity::setCoverImagePath(const QString &path)
{
    m_coverPath = path;
}

void LocalPlayListEntity::addImageUrls(const QList<QUrl> &imageUrls)
{
    LocalDatabase::instance()->insertLocalPlayListEntityImageUrls(m_id, imageUrls);
    m_count += imageUrls.size();
    m_provider->updateEntity(this);
}

void LocalPlayListEntity::removeImageUrls(const QList<QUrl> &imageUrls)
{
    LocalDatabase::instance()->removeLocalPlayListEntityImageUrls(m_id, imageUrls);
    m_count -= imageUrls.size();
    m_provider->updateEntity(this);
}
