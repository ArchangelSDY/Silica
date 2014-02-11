#ifndef IMAGESOURCEFACTORY_H
#define IMAGESOURCEFACTORY_H

#include <QObject>

#include "ImageSource.h"

class ImageSourceFactory : public QObject
{
    Q_OBJECT
public:
    explicit ImageSourceFactory(QObject *parent = 0);

    virtual QString name() const = 0;
    virtual QString fileNamePattern() const = 0;
    virtual QString urlScheme() const = 0;
    virtual ImageSource *createSingle(const QUrl &url) = 0;
    virtual ImageSource *createSingle(const QString &path) = 0;
    virtual QList<ImageSource *> createMultiple(const QUrl &url) = 0;
    virtual QList<ImageSource *> createMultiple(const QString &path) = 0;
};

#endif // IMAGESOURCEFACTORY_H
