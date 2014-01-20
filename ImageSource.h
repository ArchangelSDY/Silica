#ifndef IMAGESOURCE_H
#define IMAGESOURCE_H

#include <QIODevice>
#include <QSharedPointer>

class ImageSource
{
public:
    explicit ImageSource(QUrl url);

    bool open();
    void close();

    QIODevice *device() { return m_device.data(); }
    const QString name() const;
    const QByteArray hash() const;

    inline bool operator ==(const ImageSource &other)
    {
        return m_url == other.m_url;
    }

private:
    QUrl m_url;
    QSharedPointer<QIODevice> m_device;
};

#endif // IMAGESOURCE_H
