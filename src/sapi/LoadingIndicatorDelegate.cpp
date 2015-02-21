#include "LoadingIndicatorDelegate.h"

#include "ui/LoadingIndicator.h"

namespace sapi {

LoadingIndicatorDelegate::LoadingIndicatorDelegate(::LoadingIndicator *indicator) :
    m_indicator(indicator)
{
}

void LoadingIndicatorDelegate::start()
{
    m_indicator->start();
}

void LoadingIndicatorDelegate::stop()
{
    m_indicator->stop();
}

void LoadingIndicatorDelegate::reportProgress(int min, int max, int cur)
{
    Q_UNUSED(min);
    Q_UNUSED(max);
    Q_UNUSED(cur);
}

static sapi::LoadingIndicator *g_pluginLoadingIndicator = 0;

void initPluginLoadingIndicator(::LoadingIndicator *indicator)
{
    if (!g_pluginLoadingIndicator) {
        g_pluginLoadingIndicator = new LoadingIndicatorDelegate(indicator);
    } else {
        qWarning("Plugin loading indicator has already been initialized!");
    }
}

sapi::LoadingIndicator *loadingIndicator()
{
    return g_pluginLoadingIndicator;
}

}
