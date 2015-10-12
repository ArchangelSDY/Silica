#include "sapi/LoadingIndicator.h"
#include "sapi/LoadingIndicatorPrivate.h"

#include <QtGlobal>

namespace sapi {

// Dummy loading indicator used before a meaningful one set
class DummyLoadingIndicator : public LoadingIndicator
{
public:
    DummyLoadingIndicator() {}

    virtual void start() override {}
    virtual void stop() override {}
    virtual void reportProgress(int min, int max, int cur) override {}
};


static LoadingIndicator *g_pluginLoadingIndicator = new DummyLoadingIndicator();

LoadingIndicator *loadingIndicator()
{
    return g_pluginLoadingIndicator;
}

void setLoadingIndicatorImpl(LoadingIndicator *loadingIndicator)
{
    if (g_pluginLoadingIndicator) {
        delete g_pluginLoadingIndicator;
    }

    g_pluginLoadingIndicator = loadingIndicator;
}

}

