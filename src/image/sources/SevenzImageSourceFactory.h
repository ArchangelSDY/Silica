#ifndef SEVENZIMAGESOURCEFACTORY_H
#define SEVENZIMAGESOURCEFACTORY_H

#include <QMutex>

#include "image/ImageSourceFactory.h"
#include "util/FrequencyCache.h"

class Qt7zPackage;

class SevenzImageSourceFactory : public ImageSourceFactory
{
    Q_OBJECT

    friend class SevenzImageSource;
public:
    SevenzImageSourceFactory(ImageSourceManager *mgr);

    QString name() const;
    QStringList fileNameSuffixes() const;
    QString urlScheme() const;
    ImageSource *createSingle(const QUrl &url);
    ImageSource *createSingle(const QUrl &url, const QString &password);
    ImageSource *createSingle(const QString &path);
    QList<ImageSource *> createMultiple(const QUrl &url);
    QList<ImageSource *> createMultiple(const QString &path);

    void clearCache();

private:
    class Client;
    static FrequencyCache<QString, QSharedPointer<Qt7zPackage> > m_packageCache;
    static QMutex m_mutex;
};

#endif // SEVENZIMAGESOURCEFACTORY_H
