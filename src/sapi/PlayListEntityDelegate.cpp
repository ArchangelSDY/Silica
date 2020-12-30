#include "sapi/PlayListEntityDelegate.h"
#include <QHash>

#include "sapi/IPlayListProvider.h"
#include "../GlobalConfig.h"

namespace sapi {

PlayListEntityDelegate::PlayListEntityDelegate(IPlayListProvider *provider, const QString &name, int count, const QString &coverPath, bool canContinueProvide) :
    m_provider(provider) ,
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

PlayList *PlayListEntityDelegate::createPlayList()
{
    // TODO: Need implementation
    return nullptr;
    //QVariantHash extras;
    //auto imageUrls = m_provider->request(m_name, extras);
    //return new PlayList(imageUrls);
}

}
