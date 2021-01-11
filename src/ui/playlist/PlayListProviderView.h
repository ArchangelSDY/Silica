#pragma once

#include <QToolBar>

class PlayListProviderView
{
public:
    virtual ~PlayListProviderView() {}
    virtual void setUpToolBar(QToolBar *toolBar) = 0;
    virtual void tearDownToolBar(QToolBar *toolBar) = 0;
};
