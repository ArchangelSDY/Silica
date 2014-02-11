#ifndef SEVENZIMAGESOURCEFACTORY_H
#define SEVENZIMAGESOURCEFACTORY_H

#include "ImageSourceFactory.h"

class SevenzImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT
public:
    QString name() const;
    QString fileNamePattern() const;
    QString urlScheme() const;
    ImageSource *createSingle(const QUrl &url);
    ImageSource *createSingle(const QString &path);
    QList<ImageSource *> createMultiple(const QUrl &url);
    QList<ImageSource *> createMultiple(const QString &path);
};

#endif // SEVENZIMAGESOURCEFACTORY_H
