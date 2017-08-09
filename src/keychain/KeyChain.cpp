#include "KeyChain.h"

bool KeyChain::write(const QString &key, const QByteArray &data, const QString &comment)
{
    return false;
}

QByteArray KeyChain::read(const QString &key)
{
    return QByteArray();
}

QString KeyChain::errorMessage() const
{
    return QString();
}