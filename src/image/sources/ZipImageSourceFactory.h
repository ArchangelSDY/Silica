#ifndef ZIPIMAGESOURCEFACTORY_H
#define ZIPIMAGESOURCEFACTORY_H

#include "image/ImageSourceFactory.h"

class ZipImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT
public:
    ZipImageSourceFactory(ImageSourceManager *mgr);

    QString name() const;
    QStringList fileNameSuffixes() const;
    QString urlScheme() const;
    ImageSource *createSingle(const QUrl &url);
    ImageSource *createSingle(const QString &path);
    QList<ImageSource *> createMultiple(const QUrl &url);
    QList<ImageSource *> createMultiple(const QString &path);
};

#endif // ZIPIMAGESOURCEFACTORY_H
