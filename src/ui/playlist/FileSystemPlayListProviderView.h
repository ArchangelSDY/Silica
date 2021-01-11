#pragma once

#include "PlayListProviderView.h"

class FileSystemPlayListProvider;

class FileSystemPlayListProviderView : public PlayListProviderView
{
public:
    FileSystemPlayListProviderView(FileSystemPlayListProvider *provider);

    virtual void setUpToolBar(QToolBar *toolBar) override;
    virtual void tearDownToolBar(QToolBar *toolBar) override;

private:
    FileSystemPlayListProvider *m_provider;
    QObjectList m_toolBarObjects;
};
