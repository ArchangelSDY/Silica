#ifndef IMAGEDELEGATE_H
#define IMAGEDELEGATE_H

#include <QSharedPointer>

#include "sapi/ImageResource.h"

class Image;

namespace sapi {

class ImageDelegate : public sapi::ImageResource
{
public:
    ImageDelegate(QSharedPointer<Image> image);

    // Image interface
    QString name() const;
    QSize size() const;
    QString hash() const;
    QUrl url() const;
    const QVariantHash &extraInfo() const;
    bool isAnimation() const;
    int frameCount() const;
    QList<int> durations() const;
    QList<const QImage *> frames() const;
    QImage thumbnail() const;
    QByteArray readRaw() const;

private:
    QSharedPointer<Image> m_image;
};

}

#endif // IMAGEDELEGATE_H
