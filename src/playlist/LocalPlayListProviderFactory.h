#ifndef LOCALPLAYLISTPROVIDERFACTORY_H
#define LOCALPLAYLISTPROVIDERFACTORY_H

#include "playlist/PlayListProviderFactory.h"

class LocalPlayListProviderFactory : public PlayListProviderFactory
{
public:
    LocalPlayListProviderFactory();
    ~LocalPlayListProviderFactory();

    PlayListProvider *create();
};

#endif // LOCALPLAYLISTPROVIDERFACTORY_H
