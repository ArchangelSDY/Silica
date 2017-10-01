#pragma once

#include <QString>

class CGColleReader
{
public:
    virtual ~CGColleReader() {}

    virtual bool open() = 0;
    virtual void close() = 0;

    virtual QByteArray read(const QString &imageName) = 0;
    virtual QStringList imageNames() const = 0;

    static CGColleReader *create(const QString &path);
};
