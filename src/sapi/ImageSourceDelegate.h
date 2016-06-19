#pragma once

#include <QScopedPointer>

#include "../image/ImageSource.h"

namespace sapi {

class IImageSource;
class ImageSourceFactoryDelegate;

class ImageSourceDelegate : public ImageSource
{
public:
    ImageSourceDelegate(ImageSourceFactory *factory, IImageSource *source);
    virtual ~ImageSourceDelegate();

    virtual bool open();
    virtual void close();
    virtual bool exists();
    virtual bool copy(const QString &destPath);

private:
    QScopedPointer<IImageSource> m_source;
};

}