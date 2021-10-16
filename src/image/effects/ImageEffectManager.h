#pragma once

#include <QList>
#include <QScopedPointer>

#include "ImageEffect.h"

class Image;

class ImageEffectManager
{
public:
    ImageEffectManager();
    ~ImageEffectManager();

    QImage process(QSharedPointer<Image> image, const QImage &frame);

private:
    QList<ImageEffect *> m_effects;
};
