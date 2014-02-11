#ifndef LOCALIMAGESOURCEFACTORY_H
#define LOCALIMAGESOURCEFACTORY_H

#include "ImageSource.h"
#include "ImageSourceFactory.h"

class LocalImageSourceFactory : public ImageSourceFactory
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

#endif // LOCALIMAGESOURCEFACTORY_H
