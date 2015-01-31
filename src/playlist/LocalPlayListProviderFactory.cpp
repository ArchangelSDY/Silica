#include "LocalPlayListProviderFactory.h"

#include "playlist/LocalPlayListProvider.h"

LocalPlayListProviderFactory::LocalPlayListProviderFactory()
{
}

LocalPlayListProviderFactory::~LocalPlayListProviderFactory()
{
}

PlayListProvider *LocalPlayListProviderFactory::create()
{
    return new LocalPlayListProvider();
}
