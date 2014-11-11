#ifndef RARIMAGESOURCE_H
#define RARIMAGESOURCE_H

#include "ImageSource.h"

class RARImageSource : public ImageSource
{
public:
    RARImageSource(const QString &arcPath, const QString &imageName);

    virtual bool open();
    virtual bool copy(const QString &destPath);

private:
    QString m_arcPath;
};

#endif // RARIMAGESOURCE_H
