#pragma once

#include <QByteArray>
#include <QString>

class KeyChain
{
public:
    bool write(const QString &key, const QByteArray &data, const QString &comment);
    QByteArray read(const QString &key);
    QString errorMessage() const;
};
