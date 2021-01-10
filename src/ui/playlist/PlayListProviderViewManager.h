#pragma once

#include <QMap>

class PlayListProvider;
class PlayListProviderView;

class PlayListProviderViewManager
{
public:
    PlayListProviderViewManager();
    ~PlayListProviderViewManager();

    PlayListProviderView *get(int type) const;

private:
    QMap<int, PlayListProviderView *> m_views;
};
