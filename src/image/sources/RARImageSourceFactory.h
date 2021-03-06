#ifndef RARIMAGESOURCEFACTORY_H
#define RARIMAGESOURCEFACTORY_H

#include "image/ImageSourceFactory.h"

class RARImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT
public:
    RARImageSourceFactory(ImageSourceManager *mgr);

    QString name() const;
    QStringList fileNameSuffixes() const;
    QString urlScheme() const;
    ImageSource *createSingle(const QUrl &url);
    ImageSource *createSingle(const QUrl &url, const QString &password);
    ImageSource *createSingle(const QString &path);
    QList<ImageSource *> createMultiple(const QUrl &url);
    QList<ImageSource *> createMultiple(const QString &path);
};

#endif // RARIMAGESOURCEFACTORY_H
