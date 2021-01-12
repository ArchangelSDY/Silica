#pragma once

#include <QSharedPointer>

#include "image/ImageSourceFactory.h"

class QPdfDocument;

class PdfImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT
public:
    PdfImageSourceFactory(ImageSourceManager *mgr);

    QString name() const;
    QStringList fileNameSuffixes() const;
    QString urlScheme() const;
    ImageSource *createSingle(const QUrl &url);
    ImageSource *createSingle(const QString &path);
    QList<ImageSource *> createMultiple(const QUrl &url);
    QList<ImageSource *> createMultiple(const QString &path);

private:
    ImageSource *createSingle(const QString &path, int page);
};
