#include <QByteArray>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QTextStream>

#include "deps/QtRAR/src/qtrar.h"
#include "deps/QtRAR/src/qtrarfile.h"
#include "deps/QtRAR/src/qtrarfileinfo.h"

#include "RARImageSource.h"

RARImageSource::RARImageSource(ImageSourceFactory *factory,
                               const QString &arcPath,
                               const QString &imageName,
                               const QString &password) :
    ImageSource(factory) ,
    m_arcPath(arcPath) ,
    m_password(password)
{
    m_name = imageName;

    // Compute hash
    QString hashStr;
    QTextStream(&hashStr) <<  "rar#" << m_arcPath << "#" << m_name;
    m_hash = QCryptographicHash::hash(
        QByteArray(hashStr.toUtf8()), QCryptographicHash::Sha1).toHex();

    // Compute url
    m_url = QUrl::fromLocalFile(m_arcPath);
    m_url.setScheme("rar");
    m_url.setFragment(m_name);
}

bool RARImageSource::open()
{
    QtRARFile *file = new QtRARFile(m_arcPath, m_name);
    bool ok = file->open(QIODevice::ReadOnly);

    if (m_password.isEmpty()) {
        QtRARFileInfo info;
        file->fileInfo(&info);
        if (info.isEncrypted()) {
            ImageSource::requestPassword(m_arcPath, m_password);
        }
    }

    if (!m_password.isEmpty()) {
        if (file->isOpen()) {
            file->close();
        }

        ok = file->open(QIODevice::ReadOnly, m_password);

        if (ok) {
            ImageSource::passwordAccepted(m_arcPath, m_password);
        } else {
            // Though an open failure may not always due to wrong password, remove saved password aggressively
            // won't harm too much
            ImageSource::passwordRejected(m_arcPath);
        }
    }

    m_device.reset(file);

    return ok;
}

bool RARImageSource::exists()
{
    return QFileInfo::exists(m_arcPath);
}

bool RARImageSource::copy(const QString &destPath)
{
    return false;
}
