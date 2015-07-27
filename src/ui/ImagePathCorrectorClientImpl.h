#ifndef IMAGEPATHCORRECTORCLIENTIMPL_H
#define IMAGEPATHCORRECTORCLIENTIMPL_H

#include "logger/listeners/ImagePathCorrector.h"

class ImagePathCorrectorClientImpl : public ImagePathCorrector::PromptClient
{
public:
    ImagePathCorrectorClientImpl(QWidget *parent = 0);

    void prompt(QList<ImagePathCorrector::PathPatch> &patches) override;

private:
    class UIDelegate;

    QWidget *m_parent;
};

#endif // IMAGEPATHCORRECTORCLIENTIMPL_H
