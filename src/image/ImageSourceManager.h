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

    ImageSource *create(const QUrl &url);
    ImageSource *create(const QString &path);

private:
    explicit ImageSourceManager(QObject *parent = 0);
    void registerFactory(ImageSourceFactory *factory);

    static ImageSourceManager *m_instance;
    QHash<QString, ImageSourceFactory *> m_factories;
};

#endif // IMAGESOURCEMANAGER_H
