#include "Logging.h"

#include <QtGlobal>

namespace sapi {

static QDebug *g_pluginDebug = 0;

QDebug debug()
{
    Q_ASSERT(g_pluginDebug);
    return *g_pluginDebug;
}

void setLoggingDebugImpl(QDebug *debug)
{
    Q_ASSERT(!g_pluginDebug);
    g_pluginDebug = debug;
}

}
