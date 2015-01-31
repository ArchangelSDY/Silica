#ifndef PLAYLISTPROVIDERMANAGER_H
#define PLAYLISTPROVIDERMANAGER_H

#include <QHash>

class PlayListProviderFactory;
class PlayListRecord;

class PlayListProviderManager
{
public:
    ~PlayListProviderManager();

    PlayListRecord *create(int type, int id, const QString &name,
                           const QString &coverPath);

    static PlayListProviderManager *instance();

private:
    PlayListProviderManager();
    void registerPluginProvider(const QString &name,
                                PlayListProviderFactory *factory);
    void registerProvider(int typeId, PlayListProviderFactory *factory);

    static PlayListProviderManager *s_instance;
    QHash<int, PlayListProviderFactory *> m_providers;
};

#endif // PLAYLISTPROVIDERMANAGER_H
