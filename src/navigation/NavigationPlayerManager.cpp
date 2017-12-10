#include "NavigationPlayerManager.h"

#include <QMetaClassInfo>

#include "sapi/INavigationPlayerPlugin.h"
#include "sapi/NavigationPlayerDelegate.h"
#include "sapi/PluginLoader.h"
#include "CascadeClassifierNavigationPlayer.h"
#include "ExpandingNavigationPlayer.h"
#include "FixedRegionNavigationPlayer.h"
#include "HotspotsNavigationPlayer.h"
#include "MangaNavigationPlayer.h"
#include "NormalNavigationPlayer.h"

template<class T>
class NavigationPlayerFactory : public NavigationPlayerManager::Factory
{
public:
    NavigationPlayerFactory() :
        m_className(T::staticMetaObject.className()) ,
        m_name(T::staticMetaObject.classInfo(0).value())
    {
    }

    virtual QString className() const override
    {
        return m_className;
    }

    virtual QString name() const override
    {
        return m_name;
    }

    virtual AbstractNavigationPlayer *create(Navigator *navigator, QWidget *view) override
    {
        return new T(navigator, view);
    }

private:
    QString m_className;
    QString m_name;
};


NavigationPlayerManager *NavigationPlayerManager::s_instance = nullptr;

NavigationPlayerManager *NavigationPlayerManager::instance()
{
    if (!s_instance) {
        s_instance = new NavigationPlayerManager();
    }
    return s_instance;
}

NavigationPlayerManager::NavigationPlayerManager()
{
    registerFactory(new NavigationPlayerFactory<NormalNavigationPlayer>());
    registerFactory(new NavigationPlayerFactory<HotspotsNavigationPlayer>());
    registerFactory(new NavigationPlayerFactory<ExpandingNavigationPlayer>());
    registerFactory(new NavigationPlayerFactory<FixedRegionNavigationPlayer>());
    registerFactory(new NavigationPlayerFactory<CascadeClassifierNavigationPlayer>());
    registerFactory(new NavigationPlayerFactory<MangaNavigationPlayer>());

    // Register plugins
    // sapi::PluginLoadCallback<sapi::INavigationPlayerPlugin> callback = [this, navigator, view](sapi::INavigationPlayerPlugin *plugin, const QJsonObject &meta) {
    //     sapi::NavigationPlayerDelegate *player = new sapi::NavigationPlayerDelegate(plugin, navigator, view);
    //     this->registerPlayer(player);
    // };

    // sapi::loadPlugins("navigationplayers", callback);
}

NavigationPlayerManager::~NavigationPlayerManager()
{
    for (Factory *factory : m_playerFactories) {
        delete factory;
    }
}

AbstractNavigationPlayer *NavigationPlayerManager::create(const QString &className, Navigator *navigator, QWidget *view)
{
    Factory *factory = m_playerFactoriesByClassName[className];
    if (factory) {
        return factory->create(navigator, view);
    } else {
        return nullptr;
    }
}

AbstractNavigationPlayer *NavigationPlayerManager::create(int idx, Navigator *navigator, QWidget *view)
{
    if (idx >= 0 && idx < m_playerFactories.count()) {
        return m_playerFactories[idx]->create(navigator, view);
    } else {
        return nullptr;
    }
}

QStringList NavigationPlayerManager::names() const
{
    return m_names;
}

void NavigationPlayerManager::registerFactory(Factory *factory)
{
    m_playerFactories << factory;
    m_playerFactoriesByClassName[factory->className()] = factory;
    m_names << factory->name();
}
