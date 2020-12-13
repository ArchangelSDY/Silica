#pragma once

#include <QSharedPointer>

#include "image/ImageSource.h"

class QPdfDocument;

class PdfImageSource : public ImageSource
{
public:
    PdfImageSource(ImageSourceFactory* factory,
                   const QString &arcPath, QSharedPointer<QPdfDocument> doc, int page);

    virtual bool open() override;
    virtual void close() override;
    virtual bool exists() override;
    virtual bool readFrames(QList<QImage> &images, QList<int> &durations) override;
    virtual bool copy(const QString &destPath) override;

private:
    QString m_arcPath;
    QSharedPointer<QPdfDocument> m_doc;
    int m_page;
};
