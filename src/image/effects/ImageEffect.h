#pragma once

#include <QImage>
#include <QSharedPointer>

class Image;

class ImageEffect
{
public:
    virtual ~ImageEffect() {}

    virtual void process(QSharedPointer<Image> image, QImage &frame) = 0;

};
