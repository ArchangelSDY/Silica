#include "FileSystemPlayListProviderView.h"

#include <QFileInfo>

#include "playlist/FileSystemPlayListProvider.h"

FileSystemPlayListProviderView::FileSystemPlayListProviderView(FileSystemPlayListProvider *provider) :
    m_provider(provider)
{
}

void FileSystemPlayListProviderView::setupToolBar(QToolBar *toolBar)
{
    auto provider = m_provider;
    auto actUp = toolBar->addAction(QObject::tr("Up"), [provider]() {
        QFileInfo rootPathInfo(provider->rootPath());
        provider->setRootPath(rootPathInfo.absolutePath());
    });
    QObject::connect(provider, &FileSystemPlayListProvider::rootPathChanged, actUp, [actUp](const QString &rootPath) {
        QFileInfo rootPathInfo(rootPath);
        actUp->setEnabled(!rootPathInfo.isRoot());
    });
}
