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

PluginLoggingDelegate::~PluginLoggingDelegate()
{
    delete m_dbg;
}

QDebug *PluginLoggingDelegate::debug()
{
    return m_dbg;
}

void PluginLoggingDelegate::setDevice(QIODevice *device)
{
    m_device = device;
    delete m_dbg;
    m_dbg = new QDebug(m_device);
    setLoggingDebugImpl(debug());
}

}
