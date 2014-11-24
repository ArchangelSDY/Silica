#ifndef IMAGESOURCEMANAGERCLIENTIMPL_H
#define IMAGESOURCEMANAGERCLIENTIMPL_H

#include "image/ImageSourceManager.h"

class ImageSourceManagerClientImpl : public ImageSourceManagerClient
{
public:
    ImageSourceManagerClientImpl(QWidget *parent);

    virtual bool requestPassword(QByteArray &password);

private:
    QWidget *m_parent;
};

#endif // IMAGESOURCEMANAGERCLIENTIMPL_H
