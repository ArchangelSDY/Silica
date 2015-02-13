#ifndef ISHARER_H
#define ISHARER_H

#include <QObject>

#include "sapi/ImageResource.h"

namespace sapi {

class ISharer : public QObject
{
    Q_OBJECT
public:
    virtual ~ISharer();
    virtual bool share(const sapi::ImageResource &image) = 0;

signals:
    void finished(bool success);
};

}

#endif // ISHARER_H
