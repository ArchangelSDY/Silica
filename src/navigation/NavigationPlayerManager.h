#pragma once

#include <QHash>
#include <QList>

#include "AbstractNavigationPlayer.h"

class QWidget;
class Navigator;

class NavigationPlayerManager
{
public:
    class Factory
    {
    public:
        virtual QString className() const = 0;
        virtual QString name() const = 0;
        virtual AbstractNavigationPlayer *create(Navigator *navigator, QWidget *view) = 0;
    };

    ~NavigationPlayerManager();

    // AbstractNavigationPlayer *get(int idx);
    // QList<AbstractNavigationPlayer *> all() const;

    QStringList names() const;
    AbstractNavigationPlayer *create(int idx, Navigator *navigator, QWidget *view);
    AbstractNavigationPlayer *create(const QString &className, Navigator *navigator, QWidget *view);

    static NavigationPlayerManager *instance();

private:
    NavigationPlayerManager();

    void registerFactory(Factory *factory);

    static NavigationPlayerManager *s_instance;
    QStringList m_names;
    QList<Factory *> m_playerFactories;
    QHash<QString, Factory *> m_playerFactoriesByClassName;
};