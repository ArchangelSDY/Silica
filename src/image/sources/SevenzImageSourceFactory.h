#ifndef SEVENZIMAGESOURCEFACTORY_H
#define SEVENZIMAGESOURCEFACTORY_H

#include <QMutex>

#include "FrequencyCache.h"
#include "ImageSourceFactory.h"

class Qt7zPackage;

class SevenzImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT

    friend class SevenzImageSource;
public:
    SevenzImageSourceFactory(ImageSourceManager *mgr);

    QString name() const;
    QString fileNamePattern() const;
    QString urlScheme() const;
    ImageSource *createSingle(const QUrl &url);
    ImageSource *createSingle(const QString &path);
    QList<ImageSource *> createMultiple(const QUrl &url);
    QList<ImageSource *> createMultiple(const QString &path);

    void clearCache();

private:
    static FrequencyCache<QString, QSharedPointer<Qt7zPackage> > m_packageCache;
    static QMutex m_mutex;
};

#endif // SEVENZIMAGESOURCEFACTORY_H
