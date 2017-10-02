#pragma once

#include <QString>

class CGColleReader
{
public:
    class ImageReader
    {
    public:
        virtual ~ImageReader() {}

        virtual QByteArray read() = 0;
    };

    virtual ~CGColleReader() {}

    virtual bool open() = 0;
    virtual void close() = 0;

    virtual QStringList imageNames() const = 0;
    virtual ImageReader *createReader(const QString &imageName) = 0;

    static CGColleReader *create(const QString &path);
};
