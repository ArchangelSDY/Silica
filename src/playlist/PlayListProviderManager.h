#ifndef PLAYLISTPROVIDERMANAGER_H
#define PLAYLISTPROVIDERMANAGER_H

#include <QHash>

class PlayListProvider;
class PlayListProviderFactory;
class PlayListRecord;

class PlayListProviderManager
{
public:
    ~PlayListProviderManager();

    PlayListProvider *create(int type);
    QList<int> registeredTypes() const;

    void registerProvider(int typeId, PlayListProviderFactory *factory);

    static PlayListProviderManager *instance();

private:
    PlayListProviderManager();
    void registerPluginProvider(const QString &name,
                                PlayListProviderFactory *factory);

    static PlayListProviderManager *s_instance;
    QHash<int, PlayListProviderFactory *> m_providers;
};

#endif // PLAYLISTPROVIDERMANAGER_H
