#ifndef IMAGESOURCEMANAGER_H
#define IMAGESOURCEMANAGER_H

#include <QObject>

#include "ImageSource.h"
#include "ImageSourceFactory.h"

class ImageSourceManager : public QObject
{
    Q_OBJECT
public:
    ~ImageSourceManager();

    static ImageSourceManager *instance();

    ImageSource *createSingle(const QUrl &url);
    ImageSource *createSingle(const QString &path);
    QList<ImageSource *> createMultiple(const QUrl &url);
    QList<ImageSource *> createMultiple(const QString &path);

    QStringList urlPatterns() const;
    QString fileDialogFilters() const;

    void clearCache();

private:
    explicit ImageSourceManager(QObject *parent = 0);
    void registerFactory(ImageSourceFactory *factory);

    static ImageSourceManager *m_instance;
    QHash<QString, ImageSourceFactory *> m_factories;
};

#endif // IMAGESOURCEMANAGER_H
