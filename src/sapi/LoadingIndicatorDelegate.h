#ifndef SAPI_LOADINGINDICATORDELEGATE_H
#define SAPI_LOADINGINDICATORDELEGATE_H

#include "sapi/LoadingIndicator.h"
#include "ui/TaskProgress.h"

class LoadingIndicator;

namespace sapi {

class LoadingIndicatorDelegate : public sapi::LoadingIndicator
{
public:
    LoadingIndicatorDelegate();

    void setIndicator(::LoadingIndicator *indicator);

    // LoadingIndicator interface
    void start() override;
    void stop() override;
    void reportProgress(int min, int max, int cur) override;

private:
    ::LoadingIndicator *m_indicator;
    TaskProgress m_commonProgress;
};

void initPluginLoadingIndicator(::LoadingIndicator *indicator);

}

#endif // SAPI_LOADINGINDICATORDELEGATE_H
