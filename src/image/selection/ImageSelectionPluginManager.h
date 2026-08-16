#pragma once

#include <QImage>
#include <QList>
#include <QRect>
#include <QStringList>
#include <QThread>

namespace sapi {
class IImageSelectionPlugin;
}

class QObject;

class ImageSelectionPluginManager
{
public:
    static ImageSelectionPluginManager *instance();

    ~ImageSelectionPluginManager();

    QStringList pluginNames() const;
    int activePluginIndex() const;
    void setActivePluginIndex(int index);
    void handleSelection(QImage sourceImage, const QRect &selectionRect);

private:
    ImageSelectionPluginManager();

    static ImageSelectionPluginManager *s_instance;

    QList<sapi::IImageSelectionPlugin *> m_plugins;
    QStringList m_pluginNames;
    int m_activePluginIndex;
    QThread m_workerThread;
    QObject *m_workerContext;
};
