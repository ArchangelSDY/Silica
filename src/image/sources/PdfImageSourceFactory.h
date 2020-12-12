#pragma once

#include "image/ImageSourceFactory.h"

class PdfImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT
public:
    PdfImageSourceFactory(ImageSourceManager *mgr);

    QString name() const;
    QString fileNamePattern() const;
    QString urlScheme() const;
    ImageSource *createSingle(const QUrl &url);
    ImageSource *createSingle(const QString &path);
    QList<ImageSource *> createMultiple(const QUrl &url);
    QList<ImageSource *> createMultiple(const QString &path);
};
