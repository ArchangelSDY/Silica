#include "KeyChain.h"

bool KeyChain::write(const QString &key, const QByteArray &data, const QString &comment)
{
    return false;
}

QByteArray KeyChain::read(const QString &key)
{
    return QByteArray();
}

bool KeyChain::remove(const QString &key)
{
    return false;
}

QString KeyChain::errorMessage() const
{
    return QString();
}