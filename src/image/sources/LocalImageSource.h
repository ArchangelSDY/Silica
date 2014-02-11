#ifndef LOCALIMAGESOURCE_H
#define LOCALIMAGESOURCE_H

#include "ImageSource.h"

class LocalImageSource : public ImageSource
{
public:
    LocalImageSource(QString path);

    virtual bool open();

    virtual bool copy(const QString &destPath);
private:
    QString m_path;
};

#endif // LOCALIMAGESOURCE_H
