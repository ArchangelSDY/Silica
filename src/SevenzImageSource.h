#ifndef SEVENZIMAGESOURCE_H
#define SEVENZIMAGESOURCE_H

#include <QBuffer>

#include "ImageSource.h"

class SevenzImageSource : public ImageSource
{
public:
    SevenzImageSource(QString packagePath, QString imageName);

    virtual bool open();

    virtual bool copy(const QString &destPath);
private:
    QString m_packagePath;
};

#endif // SEVENZIMAGESOURCE_H
