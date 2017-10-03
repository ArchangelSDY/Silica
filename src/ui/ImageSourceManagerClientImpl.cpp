#include <QApplication>
#include <QDebug>
#include <QEventLoop>
#include <QInputDialog>

#include "Definitions.h"
#include "ImageSourceManagerClientImpl.h"
#include "keychain/KeyChain.h"

class PasswordInputer : public QObject
{
    Q_OBJECT
public:
    PasswordInputer(QWidget *parent) : m_parent(parent) {}

    QString password() const { return m_password; }
    bool isAccepted() const { return m_isAccepted; }

signals:
    void finished();

protected:
    virtual bool event(QEvent *event)
    {
        if (event->type() == QEvent::User) {
            QString password = QInputDialog::getText(m_parent,
                tr("Password"), tr("Password"),
                QLineEdit::Normal, QString(), &m_isAccepted);
            if (m_isAccepted) {
                m_password = password;
            }
            emit finished();
            return true;
        } else {
            return false;
        }
    }

private:
    QWidget *m_parent;
    QString m_password;
    bool m_isAccepted;
};

ImageSourceManagerClientImpl::ImageSourceManagerClientImpl(QWidget *parent) :
    m_parent(parent) ,
    m_keyChain(new KeyChain())
{
}

bool ImageSourceManagerClientImpl::requestPassword(const QString &archivePath, QString &password)
{
    QByteArray savedPassword = m_keyChain->read(credentialKey(archivePath));
    if (!savedPassword.isEmpty()) {
        QString savedPasswordStr = QString::fromUtf8(savedPassword);
        password.swap(savedPasswordStr);
        return true;
    }

    PasswordInputer inputer(m_parent);
    inputer.moveToThread(QApplication::instance()->thread());

    QEventLoop waiter;
    QObject::connect(&inputer, SIGNAL(finished()), &waiter, SLOT(quit()));

    QCoreApplication::postEvent(&inputer, new QEvent(QEvent::User));

    waiter.exec();

    if (inputer.isAccepted()) {
        QString passStr = inputer.password();
        passStr.swap(password);
        return true;
    } else {
        return false;
    }
}

void ImageSourceManagerClientImpl::passwordAccepted(const QString &archivePath, const QString &password)
{
    QString key = credentialKey(archivePath);
    if (!m_keyChain->write(key, password.toUtf8(), QString("%1: Archive Password").arg(g_BUILD_ENV))) {
        qWarning() << m_keyChain->errorMessage();
    }
}

void ImageSourceManagerClientImpl::passwordRejected(const QString &archivePath)
{
    QString key = credentialKey(archivePath);
    m_keyChain->remove(key);
}

QString ImageSourceManagerClientImpl::credentialKey(const QString &archivePath)
{
    return QString("%1:archive=%2").arg(g_BUILD_ENV).arg(archivePath);
}

#include "ImageSourceManagerClientImpl.moc"
