#ifndef SAPI_LOADINGINDICATOR
#define SAPI_LOADINGINDICATOR

namespace sapi {

class LoadingIndicator
{
public:
    virtual ~LoadingIndicator() {}
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void reportProgress(int min, int max, int cur) = 0;
};

LoadingIndicator *loadingIndicator();

}

#endif // SAPI_LOADINGINDICATOR

