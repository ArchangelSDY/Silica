#ifndef LOCALPLAYLISTPROVIDERFACTORY_H
#define LOCALPLAYLISTPROVIDERFACTORY_H

#include "playlist/PlayListProviderFactory.h"

class LocalPlayListProviderFactory : public PlayListProviderFactory
{
public:
    static const int TYPE = 0;

    LocalPlayListProviderFactory();
    ~LocalPlayListProviderFactory();

    PlayListProvider *create();
};

#endif // LOCALPLAYLISTPROVIDERFACTORY_H
