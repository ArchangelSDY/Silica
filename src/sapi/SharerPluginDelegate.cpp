#include "SharerPluginDelegate.h"

#include "sapi/ImageDelegate.h"

namespace sapi {

SharerPluginDelegate::SharerPluginDelegate(ISharerPlugin *sharer) :
    m_sharer(sharer)
{
    connect(m_sharer, SIGNAL(finished(bool)),
            this, SIGNAL(finished(bool)));
}

SharerPluginDelegate::~SharerPluginDelegate()
{
    disconnect(m_sharer, SIGNAL(finished(bool)),
               this, SIGNAL(finished(bool)));
    delete m_sharer;
}

bool SharerPluginDelegate::share(QSharedPointer<Image> image)
{
    ImageDelegate imageDelegate(image);
    return m_sharer->share(imageDelegate);
}

}
