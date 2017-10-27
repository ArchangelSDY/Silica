#pragma once

#include <QSharedPointer>

#include "sapi/ImageResource.h"
#include "sapi/sapi_global.h"

namespace sapi {

class SAPI_EXPORT NavigatorResource
{
public:
    virtual ~NavigatorResource();

    virtual int currentIndex() const = 0;
    virtual QSharedPointer<sapi::ImageResource> currentImage() = 0;

    virtual void goIndexUntilSuccess(int index, int delta) = 0;
    virtual void focusOnRect(const QRectF &rect) = 0;
    virtual void repaint() = 0;
};

}