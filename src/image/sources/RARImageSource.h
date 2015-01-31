#ifndef RARIMAGESOURCE_H
#define RARIMAGESOURCE_H

#include "image/ImageSource.h"

class RARImageSource : public ImageSource
{
public:
    RARImageSource(ImageSourceFactory *factory,
                   const QString &arcPath, const QString &imageName,
                   const QByteArray &password = QByteArray());

    virtual bool open();
    virtual bool copy(const QString &destPath);

private:
    QString m_arcPath;
    QByteArray m_password;
};

#endif // RARIMAGESOURCE_H
