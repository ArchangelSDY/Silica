#ifndef RARIMAGESOURCE_H
#define RARIMAGESOURCE_H

#include "image/ImageSource.h"

class RARImageSource : public ImageSource
{
public:
    RARImageSource(ImageSourceFactory *factory,
                   const QString &arcPath, const QString &imageName,
                   const QString &password = QString());

    virtual bool open();
    virtual bool exists();
    virtual bool copy(const QString &destPath);

private:
    QString m_arcPath;
    QString m_password;
};

#endif // RARIMAGESOURCE_H
