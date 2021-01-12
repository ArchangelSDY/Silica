#ifndef LOCALIMAGESOURCEFACTORY_H
#define LOCALIMAGESOURCEFACTORY_H

#include "image/ImageSource.h"
#include "image/ImageSourceFactory.h"

class LocalImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT
public:
    LocalImageSourceFactory(ImageSourceManager *mgr);

    QString name() const;
    QStringList fileNameSuffixes() const;
    QString urlScheme() const;
    ImageSource *createSingle(const QUrl &url);
    ImageSource *createSingle(const QString &path);
    QList<ImageSource *> createMultiple(const QUrl &url);
    QList<ImageSource *> createMultiple(const QString &path);
};

#endif // LOCALIMAGESOURCEFACTORY_H
