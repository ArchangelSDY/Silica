#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "sapi_global.h"

namespace sapi {

class IImageSource;

class SAPI_EXPORT IImageSourceFactory : public QObject
{
    Q_OBJECT
public:
    virtual ~IImageSourceFactory();

    virtual QString name() const = 0;
    virtual QString fileNamePattern() const = 0;
    virtual QString urlScheme() const = 0;
    virtual IImageSource *createSingle(const QUrl &url) = 0;
    virtual IImageSource *createSingle(const QString &path) = 0;
    virtual QList<IImageSource *> createMultiple(const QUrl &url) = 0;
    virtual QList<IImageSource *> createMultiple(const QString &path) = 0;
};

}
