#ifndef SAPI_PLUGINLOGGINGDELEGATE_H
#define SAPI_PLUGINLOGGINGDELEGATE_H

#include <QBuffer>
#include <QDebug>

namespace sapi {

class PluginLoggingDelegate
{
public:
    static PluginLoggingDelegate *instance();

    QDebug debug();
    const QBuffer &content() const;
    void clear();

private:
    static PluginLoggingDelegate *s_instance;

    PluginLoggingDelegate();
    QBuffer m_buf;
};

}

#endif // SAPI_PLUGINLOGGINGDELEGATE_H
