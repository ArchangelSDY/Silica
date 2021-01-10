#pragma once

#include "PlayListProviderView.h"

class FileSystemPlayListProvider;

class FileSystemPlayListProviderView : public PlayListProviderView
{
public:
    FileSystemPlayListProviderView(FileSystemPlayListProvider *provider);

    virtual void setupToolBar(QToolBar *toolBar) override;

private:
    FileSystemPlayListProvider *m_provider;
};
