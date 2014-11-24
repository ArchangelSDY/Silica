#include "ImageSourceFactory.h"
#include "ImageSourceManager.h"

ImageSourceFactory::ImageSourceFactory(ImageSourceManager *mgr,
                                       QObject *parent) :
    QObject(parent) ,
    m_mgr(mgr)
{
}

bool ImageSourceFactory::requestPassword(QByteArray &password)
{
    if (m_mgr->m_client) {
        return m_mgr->m_client->requestPassword(password);
    } else {
        return false;
    }
}
