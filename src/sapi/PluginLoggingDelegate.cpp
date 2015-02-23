#include "PluginLoggingDelegate.h"

namespace sapi {

PluginLoggingDelegate *PluginLoggingDelegate::s_instance = 0;
PluginLoggingDelegate *PluginLoggingDelegate::instance()
{
    if (!s_instance) {
        s_instance = new PluginLoggingDelegate();
    }
    return s_instance;
}

PluginLoggingDelegate::PluginLoggingDelegate() :
    m_device(0)
{
}

QDebug PluginLoggingDelegate::debug()
{
    QDebug dbg = m_device ? QDebug(m_device) : QDebug(QtDebugMsg);
    return dbg;
}

void PluginLoggingDelegate::setDevice(QIODevice *device)
{
    m_device = device;
}

}
