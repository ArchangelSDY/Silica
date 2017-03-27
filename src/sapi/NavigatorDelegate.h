#pragma once

#include "sapi/NavigatorResource.h"

class Navigator;

namespace sapi {

class NavigatorDelegate : public sapi::NavigatorResource
{
public:
    NavigatorDelegate(Navigator *navigator);

    virtual int currentIndex() const override;
    virtual QSharedPointer<sapi::ImageResource> currentImage() override;

    virtual void goIndexUntilSuccess(int index, int delta) override;
    virtual void focusOnRect(const QRectF &rect) override;

private:
    Navigator *m_navigator;
};

}
