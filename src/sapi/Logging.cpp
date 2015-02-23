#include "Logging.h"

#include "sapi/PluginLoggingDelegate.h"

namespace sapi {

QDebug debug()
{
    return PluginLoggingDelegate::instance()->debug();
}

}
