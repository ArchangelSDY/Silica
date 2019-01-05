#pragma once

#include <QVector>

#include "ImageEffect.h"

class Image;

class ImageEffectManager
{
public:
    ImageEffectManager();
    ~ImageEffectManager();

    QImage process(QSharedPointer<Image> image, const QImage &frame);

private:
    QVector<ImageEffect *> m_effects;
};
