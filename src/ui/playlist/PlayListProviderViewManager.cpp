#include "PlayListProviderViewManager.h"

#include "playlist/PlayListProviderManager.h"
#include "playlist/FileSystemPlayListProvider.h"
#include "ui/playlist/FileSystemPlayListProviderView.h"

PlayListProviderViewManager::PlayListProviderViewManager()
{
    auto fsProvider = static_cast<FileSystemPlayListProvider *>(PlayListProviderManager::instance()->get(FileSystemPlayListProvider::TYPE));
    m_views[fsProvider->type()] = new FileSystemPlayListProviderView(fsProvider);
}

PlayListProviderViewManager::~PlayListProviderViewManager()
{
    qDeleteAll(m_views);
}

PlayListProviderView *PlayListProviderViewManager::get(int type) const
{
    return m_views[type];
}
