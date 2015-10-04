#ifndef SAPI_PLUGINLOGGINGDELEGATE_H
#define SAPI_PLUGINLOGGINGDELEGATE_H

#include <QDebug>

class QIODevice;

namespace sapi {

class PluginLoggingDelegate
{
public:
    static PluginLoggingDelegate *instance();

    ~PluginLoggingDelegate();

    QDebug *debug();
    void setDevice(QIODevice* device);

private:
    static PluginLoggingDelegate *s_instance;

    PluginLoggingDelegate();
    QDebug *m_dbg;
    QIODevice *m_device;
};

}

#endif // SAPI_PLUGINLOGGINGDELEGATE_H
