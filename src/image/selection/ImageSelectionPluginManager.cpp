#include "ImageSelectionPluginManager.h"

#include <QJsonObject>
#include <QMetaObject>
#include <QObject>

#include <utility>

#include "sapi/IImageSelectionPlugin.h"
#include "sapi/PluginLoader.h"

ImageSelectionPluginManager *ImageSelectionPluginManager::s_instance = nullptr;

ImageSelectionPluginManager *ImageSelectionPluginManager::instance()
{
    if (!s_instance) {
        s_instance = new ImageSelectionPluginManager();
    }
    return s_instance;
}

ImageSelectionPluginManager::ImageSelectionPluginManager() :
    m_activePluginIndex(-1) ,
    m_workerContext(new QObject())
{
    m_workerContext->moveToThread(&m_workerThread);
    QObject::connect(&m_workerThread, &QThread::finished, m_workerContext, &QObject::deleteLater);
    m_workerThread.start();

    sapi::PluginLoadCallback<sapi::IImageSelectionPlugin> callback = [this](sapi::IImageSelectionPlugin *plugin, const QJsonObject &meta) {
        QString name = meta["displayName"].toString();
        if (name.isEmpty()) {
            name = meta["name"].toString();
        }
        m_plugins << plugin;
        m_pluginNames << name;
    };
    sapi::loadPlugins("imageselections", callback);
}

ImageSelectionPluginManager::~ImageSelectionPluginManager()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

QStringList ImageSelectionPluginManager::pluginNames() const
{
    return m_pluginNames;
}

int ImageSelectionPluginManager::activePluginIndex() const
{
    return m_activePluginIndex;
}

void ImageSelectionPluginManager::setActivePluginIndex(int index)
{
    if (index == -1 || (index >= 0 && index < m_plugins.count())) {
        m_activePluginIndex = index;
    }
}

void ImageSelectionPluginManager::handleSelection(QImage sourceImage, const QRect &selectionRect)
{
    if (m_activePluginIndex < 0 || m_activePluginIndex >= m_plugins.count() || selectionRect.isEmpty()) {
        return;
    }

    sapi::IImageSelectionPlugin *plugin = m_plugins[m_activePluginIndex];
    QMetaObject::invokeMethod(m_workerContext, [plugin, sourceImage = std::move(sourceImage), selectionRect]() mutable {
        QImage selection = sourceImage.copy(selectionRect);
        if (!selection.isNull()) {
            plugin->handleSelection(std::move(selection));
        }
    }, Qt::QueuedConnection);
}
