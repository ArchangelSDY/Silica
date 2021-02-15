#pragma once

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
    QScopedPointer<QDebug> m_dbg;
    QIODevice *m_device;
};

}
