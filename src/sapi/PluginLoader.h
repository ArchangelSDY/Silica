#pragma once

#include <functional>

#include <QDir>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QPluginLoader>

namespace sapi {

template<class T>
using PluginLoadCallback = std::function<void(T *, const QJsonObject &meta)>;

template<class T>
void loadPlugins(const QString &dir, PluginLoadCallback<T> cb)
{
    // Load plugins
    foreach(const QString &libPath, qApp->libraryPaths()) {
        QDir libDir(libPath);
        if (!libDir.cd(dir)) {
            continue;
        }

        foreach(const QString &filename, libDir.entryList(QDir::Files)) {
            QPluginLoader loader(libDir.absoluteFilePath(filename));

            // Check meta
            QJsonObject meta = loader.metaData();
            QJsonObject customMeta = meta["MetaData"].toObject();

            QObject *instance = loader.instance();
            if (instance) {
                T *plugin = qobject_cast<T *>(instance);
                if (plugin) {
                    cb(plugin, customMeta);
                }
            } else {
                qWarning() << "[Plugin]" << loader.fileName()
                    << loader.errorString();
            }
        }
    }
}

}