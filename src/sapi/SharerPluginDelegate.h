#ifndef SHARERPLUGINDELEGATE_H
#define SHARERPLUGINDELEGATE_H

#include "sapi/ISharerPlugin.h"
#include "share/Sharer.h"

namespace sapi {

class SharerPluginDelegate : public Sharer
{
    Q_OBJECT
public:
    SharerPluginDelegate(ISharerPlugin *sharer);
    ~SharerPluginDelegate();

    bool share(QSharedPointer<Image> image) override;

private:
    ISharerPlugin *m_sharer;
};

}

#endif // SHARERPLUGINDELEGATE_H
