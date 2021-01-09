#pragma once

#include <QMap>

class PlayListProvider;
class PlayListProviderFactory;

class PlayListProviderManager
{
public:
    ~PlayListProviderManager();

    PlayListProvider *get(int type);
    QList<PlayListProvider *> all() const;

    static PlayListProviderManager *instance();

private:
    PlayListProviderManager();
    void registerProvider(int type, PlayListProvider *provider);

    static PlayListProviderManager *s_instance;
    QMap<int, PlayListProvider *> m_providers;
};
