#ifndef IMAGERESOURCE_H
#define IMAGERESOURCE_H

#include <QByteArray>
#include <QImage>
#include <QList>
#include <QSize>
#include <QString>
#include <QUrl>
#include <QVariant>

namespace sapi {

class ImageResource
{
public:
    virtual QString name() const = 0;
    virtual QSize size() const = 0;
    virtual QString hash() const = 0;
    virtual QUrl url() const = 0;
    virtual const QVariantHash &extraInfo() const = 0;

    virtual bool isAnimation() const = 0;
    virtual int frameCount() const = 0;
    virtual QList<int> durations() const = 0;
    virtual QList<const QImage *> frames() const = 0;
    virtual QImage thumbnail() const = 0;

    virtual QByteArray readRaw() const = 0;
};

}

#endif // IMAGERESOURCE_H

