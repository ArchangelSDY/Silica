#ifndef IMAGESOURCEMANAGER_H
#define IMAGESOURCEMANAGER_H

#include <QObject>

#include "ImageSource.h"
#include "ImageSourceFactory.h"

class ImageSourceManagerClient;

class ImageSourceManager : public QObject
{
    friend class ImageSourceFactory;

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
    QStringList dirIterFilters() const;

    void clearCache();

    /**
     * @brief Set a client to handle callbacks. Will take ownership of it.
     * @param client
     */
    void setClient(ImageSourceManagerClient *client);

private:
    explicit ImageSourceManager(QObject *parent = 0);
    void registerFactory(ImageSourceFactory *factory);

    static ImageSourceManager *m_instance;
    QHash<QString, ImageSourceFactory *> m_factories;
    ImageSourceManagerClient *m_client;
};


class ImageSourceManagerClient
{
public:
    virtual ~ImageSourceManagerClient() {}

    virtual bool requestPassword(QByteArray &password) = 0;
};

#endif // IMAGESOURCEMANAGER_H
