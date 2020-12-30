#ifndef PLAYLISTPROVIDERMANAGER_H
#define PLAYLISTPROVIDERMANAGER_H

#include <QHash>

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
    QHash<int, PlayListProvider *> m_providers;
};

#endif // PLAYLISTPROVIDERMANAGER_H
