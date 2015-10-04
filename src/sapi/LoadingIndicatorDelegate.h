#ifndef SAPI_LOADINGINDICATORDELEGATE_H
#define SAPI_LOADINGINDICATORDELEGATE_H

#include "sapi/LoadingIndicator.h"
#include "ui/models/TaskProgress.h"

class LoadingIndicator;

namespace sapi {

class LoadingIndicatorDelegate : public sapi::LoadingIndicator
{
public:
    class Indicator {
    public:
        virtual void addTaskProgress(const TaskProgress &progress) = 0;
        virtual void removeTaskProgress(const TaskProgress &progress) = 0;
    };

    LoadingIndicatorDelegate();

    void setIndicator(Indicator *indicator);

    // LoadingIndicator interface
    void start() override;
    void stop() override;
    void reportProgress(int min, int max, int cur) override;

private:
    Indicator *m_indicator;
    TaskProgress m_commonProgress;
};

void initPluginLoadingIndicator(LoadingIndicatorDelegate::Indicator *indicator);

}

#endif // SAPI_LOADINGINDICATORDELEGATE_H
