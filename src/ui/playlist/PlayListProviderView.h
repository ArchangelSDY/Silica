#pragma once

#include <QToolBar>

class PlayListProviderView
{
public:
    virtual ~PlayListProviderView() {}
    virtual void setupToolBar(QToolBar *toolBar) = 0;
};
