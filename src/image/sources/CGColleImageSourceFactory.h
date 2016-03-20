#ifndef CGCOLLEIMAGESOURCEFACTORY_H
#define CGCOLLEIMAGESOURCEFACTORY_H

#include "image/ImageSourceFactory.h"

class CGColleImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT
public:
    CGColleImageSourceFactory(ImageSourceManager *mgr);

    QString name() const;
    QString fileNamePattern() const;
    QString urlScheme() const;
    ImageSource *createSingle(const QUrl &url);
    ImageSource *createSingle(const QString &path);
    QList<ImageSource *> createMultiple(const QUrl &url);
    QList<ImageSource *> createMultiple(const QString &path);

};

#endif // CGCOLLEIMAGESOURCEFACTORY_H