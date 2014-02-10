#ifndef SEVENZIMAGESOURCE_H
#define SEVENZIMAGESOURCE_H

#include <QBuffer>

#include <FrequencyCache.h>
#include "ImageSource.h"

class Qt7zPackage;

class SevenzImageSource : public ImageSource
{
public:
    SevenzImageSource(QString packagePath, QString imageName);

    virtual bool open();

    virtual bool copy(const QString &destPath);
private:
    static FrequencyCache<QString, QSharedPointer<Qt7zPackage> > m_packageCache;

    QString m_packagePath;
};

#endif // SEVENZIMAGESOURCE_H
