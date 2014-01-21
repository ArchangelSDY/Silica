#ifndef IMAGESOURCE_H
#define IMAGESOURCE_H

#include <QIODevice>
#include <QSharedPointer>

class ImageSource
{
public:
    enum SourceFormat {
        NoneImage,
        LocalImage,
        LocalZip,
    };

    explicit ImageSource(QUrl url);
    explicit ImageSource(QString path);

    bool open();
    void close();

    QIODevice *device() { return m_device.data(); }
    const QString name() const { return m_name; }
    const QByteArray hash() const { return m_hash; }

    inline bool operator ==(const ImageSource &other)
    {
        return hash() == other.hash();
    }

private:
    void initLocalImage(QString path);
    QString searchRealPath(QString path);
    void computeHash();

    SourceFormat m_sourceFormat;
    QString m_imagePath;
    QString m_zipPath;
    QString m_name;
    QByteArray m_hash;
    QSharedPointer<QIODevice> m_device;
};

#endif // IMAGESOURCE_H
