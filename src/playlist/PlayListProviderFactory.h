#ifndef PLAYLISTPROVIDERFACTORY
#define PLAYLISTPROVIDERFACTORY

class PlayListProvider;

class PlayListProviderFactory
{
public:
    virtual ~PlayListProviderFactory() {}

    virtual PlayListProvider *create() = 0;
};

#endif // PLAYLISTPROVIDERFACTORY

