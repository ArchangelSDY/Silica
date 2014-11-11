#ifndef RARIMAGESOURCEFACTORY_H
#define RARIMAGESOURCEFACTORY_H

#include "ImageSourceFactory.h"

class RARImageSourceFactory : public ImageSourceFactory
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

#endif // RARIMAGESOURCEFACTORY_H
