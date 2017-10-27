#pragma once

#include <QImage>

class Image;

class ImageEffect
{
public:
    virtual ~ImageEffect() {}

    virtual void process(Image *image, QImage &frame) = 0;

};
