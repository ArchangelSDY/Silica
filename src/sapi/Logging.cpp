#include "Logging.h"
#include "LoggingPrivate.h"

#include <QtGlobal>

namespace sapi {

static QDebug *g_pluginDebug = 0;

QDebug debug()
{
    if (g_pluginDebug) {
        return *g_pluginDebug;
    } else {
        return QDebug(QtMsgType::QtDebugMsg);
    }
}

void setLoggingDebugImpl(QDebug *debug)
{
    g_pluginDebug = debug;
}

}
