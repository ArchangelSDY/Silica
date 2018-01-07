#ifndef IMAGESOURCE_H
#define IMAGESOURCE_H

#include <QIODevice>
#include <QScopedPointer>
#include <QUrl>

#include "ImageSourceFactory.h"

class ImageSource
{
public:
    virtual ~ImageSource();

    virtual bool open() = 0;
    virtual void close();
    virtual bool exists() = 0;

    virtual QVariantHash readMetadata();
    virtual bool readFrames(QList<QImage> &images, QList<int> &durations);
    virtual bool copy(const QString &destPath) = 0;

    QIODevice *device() { return m_device.data(); }
    const QString name() const { return m_name; }
    const QByteArray hash() const { return m_hash; }
    QString hashStr() const { return QString::fromUtf8(m_hash.toHex()); }
    const QUrl &url() const { return m_url; }

    inline bool operator ==(const ImageSource &other)
    {
        return hash() == other.hash();
    }

protected:
    ImageSource(ImageSourceFactory *factory);

    QString findRealPath(QString path);
    bool requestPassword(const QString &archivePath, QString &password);
    void passwordAccepted(const QString &archivePath, const QString &password);
    void passwordRejected(const QString &archivePath);

    ImageSourceFactory *m_factory;
    QString m_name;
    QByteArray m_hash;
    QUrl m_url;
    QScopedPointer<QIODevice> m_device;
};

#endif // IMAGESOURCE_H
