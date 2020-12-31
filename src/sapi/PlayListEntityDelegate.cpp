#include "sapi/PlayListEntityDelegate.h"
#include <QHash>

#include "sapi/PlayListProviderDelegate.h"
#include "../GlobalConfig.h"

namespace sapi {

PlayListEntityDelegate::PlayListEntityDelegate(sapi::PlayListProviderDelegate *provider,
        const QString &name, int count, const QString &coverPath, bool canContinueProvide) :
    PlayListEntity(provider) ,
    m_name(name) ,
    m_count(count) ,
    m_coverPath(coverPath) ,
    m_canContinueProvide(canContinueProvide)
{
}

PlayListEntityDelegate::~PlayListEntityDelegate()
{
}

int PlayListEntityDelegate::count() const
{
    return m_count;
}

QString PlayListEntityDelegate::name() const
{
    return m_name;
}

bool PlayListEntityDelegate::supportsOption(PlayListEntityOption option) const
{
    switch (option)
    {
    case PlayListEntityOption::Continuation:
        return m_canContinueProvide;
    default:
        return false;
    }
}

QImage PlayListEntityDelegate::loadCoverImage()
{
    QString coverFullPath = GlobalConfig::instance()->thumbnailPath() + "/" + m_coverPath;
    return QImage(coverFullPath);
}

QList<QUrl> PlayListEntityDelegate::loadImageUrls()
{
    // TODO: Need implementation
    return {};
    //QVariantHash extras;
    //auto imageUrls = m_provider->request(m_name, extras);
    //return new PlayList(imageUrls);
}

void PlayListEntityDelegate::setCoverImagePath(const QString &path)
{
    // TODO: Need implementation
}

void PlayListEntityDelegate::removeImageUrls(const QList<QUrl> &imageUrls)
{
    // TODO: Need implementation
}

}
