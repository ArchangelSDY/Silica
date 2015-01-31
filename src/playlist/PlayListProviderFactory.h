#ifndef PLAYLISTPROVIDERFACTORY
#define PLAYLISTPROVIDERFACTORY

#include <QtPlugin>

class PlayListProvider;

class PlayListProviderFactory
{
public:
    virtual ~PlayListProviderFactory() {}

    virtual PlayListProvider *create() = 0;
};

Q_DECLARE_INTERFACE(PlayListProviderFactory, "com.archangelsdy.silica.playlistproviderfactory")

#endif // PLAYLISTPROVIDERFACTORY

