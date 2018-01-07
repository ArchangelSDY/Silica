#pragma once

#include <QIODevice>
#include <QVariantHash>

#include "sapi_global.h"

namespace sapi {

class SAPI_EXPORT IImageSource
{
public:
    virtual ~IImageSource();

    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool exists() = 0;
    virtual QVariantHash readMetadata();
    virtual bool readFrames(QList<QImage> &frames, QList<int> &durations) = 0;
    virtual QIODevice *device() = 0;

    virtual QString name() const = 0;
    virtual QByteArray hash() const = 0;
    virtual QUrl url() const = 0;
};

}
