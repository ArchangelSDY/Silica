#ifndef SHARERPLUGINDELEGATE_H
#define SHARERPLUGINDELEGATE_H

#include "sapi/ISharer.h"
#include "share/Sharer.h"

namespace sapi {

class SharerPluginDelegate : public Sharer
{
    Q_OBJECT
public:
    SharerPluginDelegate(ISharer *sharer);
    ~SharerPluginDelegate();

    bool share(const Image *image) override;

private:
    ISharer *m_sharer;
};

}

#endif // SHARERPLUGINDELEGATE_H
