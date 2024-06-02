#include "SharerPluginDelegate.h"

#include "sapi/ImageDelegate.h"

namespace sapi {

SharerPluginDelegate::SharerPluginDelegate(ISharerPlugin *sharer) :
    m_sharer(sharer)
{
    connect(m_sharer.data(), SIGNAL(finished(bool)),
            this, SIGNAL(finished(bool)));
}

bool SharerPluginDelegate::share(const QList<QSharedPointer<Image>> &images)
{
    QList<QSharedPointer<ImageResource>> delegates;
    for (auto image : images) {
        delegates << QSharedPointer<ImageDelegate>::create(image);
    }
    return m_sharer->share(delegates);
}

}
