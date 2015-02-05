#ifndef IPLAYLISTPROVIDERPLUGIN_H
#define IPLAYLISTPROVIDERPLUGIN_H

#include <QObject>

#include "sapi/sapi_global.h"
#include "sapi/IPlayListProvider.h"

namespace sapi {

class SAPI_EXPORT IPlayListProviderPlugin
{
public:
    virtual ~IPlayListProviderPlugin();

    virtual IPlayListProvider *create() = 0;
    virtual void destroy(IPlayListProvider *provider) = 0;
};

}

Q_DECLARE_INTERFACE(sapi::IPlayListProviderPlugin, "com.archangelsdy.silica.plugins.playlistprovider")

#endif // IPLAYLISTPROVIDERPLUGIN_H
