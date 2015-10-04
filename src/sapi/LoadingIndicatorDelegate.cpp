#include "LoadingIndicatorDelegate.h"

#include "ui/LoadingIndicator.h"

namespace sapi {

LoadingIndicatorDelegate::LoadingIndicatorDelegate() :
    m_indicator(0)
{
    m_commonProgress.setEstimateEnabled(false);
}

void LoadingIndicatorDelegate::setIndicator(::LoadingIndicator *indicator)
{
    if (m_indicator) {
        m_indicator->removeTaskProgress(m_commonProgress);
    }

    m_indicator = indicator;

    if (m_indicator) {
        m_indicator->addTaskProgress(m_commonProgress);
    }
}

void LoadingIndicatorDelegate::start()
{
    m_commonProgress.reset();
    m_commonProgress.setMinimum(0);
    m_commonProgress.setMaximum(0);
    m_commonProgress.start();
}

void LoadingIndicatorDelegate::stop()
{
    m_commonProgress.stop();
}

void LoadingIndicatorDelegate::reportProgress(int min, int max, int cur)
{
    m_commonProgress.setMinimum(min);
    m_commonProgress.setMaximum(max);
    m_commonProgress.setValue(cur);
}

void setLoadingIndicatorImpl(LoadingIndicator *indicator);

void initPluginLoadingIndicator(::LoadingIndicator *indicator)
{
    sapi::LoadingIndicatorDelegate *delegate = new sapi::LoadingIndicatorDelegate();
    delegate->setIndicator(indicator);
    setLoadingIndicatorImpl(delegate);
}

}
