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

    virtual bool open() override;
    virtual void close() override;
    virtual bool exists() override;
    virtual bool readFrames(QList<QImage> &frames, QList<int> &durations) override;
    virtual bool copy(const QString &destPath) override;

private:
    QScopedPointer<IImageSource> m_source;
};

}