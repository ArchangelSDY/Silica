#include "PluginLoggingDelegate.h"

#include "LoggingPrivate.h"

namespace sapi {

PluginLoggingDelegate *PluginLoggingDelegate::s_instance =
    new PluginLoggingDelegate();
PluginLoggingDelegate *PluginLoggingDelegate::instance()
{
    return s_instance;
}

PluginLoggingDelegate::PluginLoggingDelegate() :
    m_device(0) ,
    m_dbg(new QDebug(QtDebugMsg))
{
    setLoggingDebugImpl(debug());
}

QDebug *PluginLoggingDelegate::debug()
{
    return m_dbg.data();
}

void PluginLoggingDelegate::setDevice(QIODevice *device)
{
    m_device = device;
    m_dbg.reset(new QDebug(m_device));
    setLoggingDebugImpl(debug());
}

}
