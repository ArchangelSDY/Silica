#pragma once

#include <QScopedPointer>

#include "sapi/ISharerPlugin.h"
#include "share/Sharer.h"

namespace sapi {

class SharerPluginDelegate : public Sharer
{
    Q_OBJECT
public:
    SharerPluginDelegate(ISharerPlugin *sharer);

    bool share(QSharedPointer<Image> image) override;

private:
    QScopedPointer<ISharerPlugin> m_sharer;
};

}
