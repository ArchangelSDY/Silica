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

PluginLoggingDelegate::PluginLoggingDelegate()
{
    m_buf.open(QIODevice::ReadWrite);
}

QDebug PluginLoggingDelegate::debug()
{
    return QDebug(&m_buf);
}

const QBuffer &PluginLoggingDelegate::content() const
{
    return m_buf;
}

void PluginLoggingDelegate::clear()
{
    m_buf.buffer().clear();
}

}
