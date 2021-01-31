#include "playlist/LocalPlayListEntity.h"

#include "db/LocalDatabase.h"
#include "../GlobalConfig.h"
#include "../PlayList.h"

LocalPlayListEntity::LocalPlayListEntity(LocalPlayListProvider *provider, LocalDatabase *db,
        int id, const QString &name, int count, const QString &coverPath) :
    PlayListEntity(provider) ,
    m_db(db) ,
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
    return option == PlayListEntityOption::Count
        || option == PlayListEntityOption::AddImageUrls
        || option == PlayListEntityOption::RemoveImageUrls;
}

QImage LocalPlayListEntity::loadCoverImage()
{
    QString coverFullPath = GlobalConfig::instance()->thumbnailPath() + "/" + m_coverPath;
    return QImage(coverFullPath);
}

QList<QUrl> LocalPlayListEntity::loadImageUrls()
{
    return m_db->queryLocalPlayListEntityImageUrls(m_id);
}

void LocalPlayListEntity::setName(const QString &name)
{
    m_name = name;
}

void LocalPlayListEntity::setCoverImagePath(const QString &path)
{
    QString dirPrefix = GlobalConfig::instance()->thumbnailPath() + "/";
    if (path.startsWith(dirPrefix)) {
        m_coverPath = path.mid(dirPrefix.count());
    } else {
        m_coverPath = path;
    }
}

void LocalPlayListEntity::addImageUrls(const QList<QUrl> &imageUrls)
{
    m_db->insertLocalPlayListEntityImageUrls(m_id, imageUrls);
    m_count += imageUrls.size();
    m_provider->updateEntity(this);
}

void LocalPlayListEntity::removeImageUrls(const QList<QUrl> &imageUrls)
{
    m_db->removeLocalPlayListEntityImageUrls(m_id, imageUrls);
    m_count -= imageUrls.size();
    m_provider->updateEntity(this);
}

int LocalPlayListEntity::id() const
{
    return m_id;
}

QString LocalPlayListEntity::coverImagePath() const
{
    return m_coverPath;
}
