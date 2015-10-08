#include "sapi/LoadingIndicator.h"
#include "sapi/LoadingIndicatorPrivate.h"

#include <QtGlobal>

namespace sapi {

static LoadingIndicator *g_pluginLoadingIndicator = 0;

LoadingIndicator *loadingIndicator()
{
    return g_pluginLoadingIndicator;
}

void setLoadingIndicatorImpl(LoadingIndicator *loadingIndicator)
{
    Q_ASSERT(!g_pluginLoadingIndicator);
    g_pluginLoadingIndicator = loadingIndicator;
}

}

