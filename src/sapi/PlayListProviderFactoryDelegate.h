#pragma once

#include <QJsonObject>
#include <QScopedPointer>
#include <QString>

#include "playlist/PlayListProviderFactory.h"

namespace sapi {

class IPlayListProviderPlugin;

class PlayListProviderFactoryDelegate : public PlayListProviderFactory
{
public:
    PlayListProviderFactoryDelegate(IPlayListProviderPlugin *plugin,
                                    const QJsonObject &meta);

    // PlayListProviderFactory interface
    PlayListProvider *create();

private:
    QScopedPointer<IPlayListProviderPlugin> m_plugin;
    QJsonObject m_meta;
};

}
