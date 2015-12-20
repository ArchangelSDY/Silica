#include <QApplication>
#include <QEventLoop>
#include <QInputDialog>

#include "ImageSourceManagerClientImpl.h"

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
    m_parent(parent)
{
}

bool ImageSourceManagerClientImpl::requestPassword(QString &password)
{
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


#include "ImageSourceManagerClientImpl.moc"
