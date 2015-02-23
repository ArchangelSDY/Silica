#include "LoadingIndicatorDelegate.h"

#include "ui/LoadingIndicator.h"

namespace sapi {

LoadingIndicatorDelegate::LoadingIndicatorDelegate() :
    m_indicator(0)
{
}

void LoadingIndicatorDelegate::setIndicator(::LoadingIndicator *indicator)
{
    m_indicator = indicator;
}

void LoadingIndicatorDelegate::start()
{
    if (!m_indicator) {
        return;
    }
    m_indicator->start();
}

void LoadingIndicatorDelegate::stop()
{
    if (!m_indicator) {
        return;
    }
    m_indicator->stop();
}

void LoadingIndicatorDelegate::reportProgress(int min, int max, int cur)
{
    Q_UNUSED(min);
    Q_UNUSED(max);
    Q_UNUSED(cur);
}

static sapi::LoadingIndicatorDelegate *g_pluginLoadingIndicator =
    new sapi::LoadingIndicatorDelegate();

void initPluginLoadingIndicator(::LoadingIndicator *indicator)
{
    g_pluginLoadingIndicator->setIndicator(indicator);
}

sapi::LoadingIndicator *loadingIndicator()
{
    return g_pluginLoadingIndicator;
}

}
