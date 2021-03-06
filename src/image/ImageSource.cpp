#include "ImageSource.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QImageReader>
#include <QSettings>
#include <QTextStream>
#include <QUrl>

#include "deps/quazip/quazip/quazipfile.h"

ImageSource::ImageSource(ImageSourceFactory *factory) :
    m_factory(factory)
{
}

ImageSource::~ImageSource()
{
}

bool ImageSource::requestPassword(const QString &archivePath, QString &password)
{
    return m_factory->requestPassword(archivePath, password);
}

void ImageSource::passwordAccepted(const QString &archivePath, const QString &password)
{
    m_factory->passwordAccepted(archivePath, password);
}

void ImageSource::passwordRejected(const QString &archivePath)
{
    m_factory->passwordRejected(archivePath);
}

void ImageSource::close()
{
    if (!m_device.isNull()) {
        m_device->close();
        m_device.reset();
    }
}

QVariantHash ImageSource::readMetadata()
{
    return QVariantHash();
}

bool ImageSource::readFrames(QList<QImage> &frames, QList<int> &durations)
{
    QImageReader reader;
    reader.setDevice(device());
    reader.setAutoTransform(true);

    forever {
        QImage frame = reader.read();

        // If fail to read first frame, try to decide format from content
        if (frame.isNull() && frames.isEmpty()) {
            device()->reset();
            reader.setDevice(device());
            reader.setDecideFormatFromContent(true);
            frame = reader.read();
        }

        if (!frame.isNull()) {
            frames << frame;
            durations << reader.nextImageDelay();
        } else {
            break;
        }

        if (!reader.supportsAnimation()) {
            break;
        }
    }

    if (frames.isEmpty()) {
        qWarning() << "Failed to read image" << url().toString() << "due to error:"
            << reader.errorString();
        return false;
    }

    return true;
}
