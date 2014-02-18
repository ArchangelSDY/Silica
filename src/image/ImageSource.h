#ifndef IMAGESOURCE_H
#define IMAGESOURCE_H

#include <QIODevice>
#include <QSharedPointer>
#include <QUrl>

class ImageSource
{
public:
    virtual ~ImageSource();

    virtual bool open() = 0;
    virtual void close();

    virtual bool copy(const QString &destPath) = 0;

    QIODevice *device() { return m_device.data(); }
    const QString name() const { return m_name; }
    const QByteArray hash() const { return m_hash; }
    const QUrl &url() const { return m_url; }

    inline bool operator ==(const ImageSource &other)
    {
        return hash() == other.hash();
    }

protected:
    ImageSource() {}

    QString searchRealPath(QString path);

    QString m_name;
    QByteArray m_hash;
    QUrl m_url;
    QSharedPointer<QIODevice> m_device;
};

#endif // IMAGESOURCE_H
