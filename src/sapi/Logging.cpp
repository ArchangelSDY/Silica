#include "Logging.h"
#include "LoggingPrivate.h"

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
    g_pluginDebug = debug;
}

}
