#include <QInputDialog>

#include "ImageSourceManagerClientImpl.h"

ImageSourceManagerClientImpl::ImageSourceManagerClientImpl(QWidget *parent) :
    m_parent(parent)
{
}

bool ImageSourceManagerClientImpl::requestPassword(QByteArray &password)
{
    bool ok;
    QString passStr = QInputDialog::getText(m_parent,
        QObject::tr("Password"), QObject::tr("Password"), QLineEdit::Normal,
        QString(), &ok);
    if (ok) {
        passStr.toUtf8().swap(password);
        return true;
    } else {
        return false;
    }
}
