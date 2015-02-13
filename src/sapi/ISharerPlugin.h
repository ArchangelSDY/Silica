#ifndef ISHARER_H
#define ISHARER_H

#include <QObject>

#include "sapi/ImageResource.h"
#include "sapi/sapi_global.h"

namespace sapi {

class SAPI_EXPORT ISharerPlugin : public QObject
{
    Q_OBJECT
public:
    virtual ~ISharerPlugin();
    virtual bool share(const sapi::ImageResource &image) = 0;

signals:
    void finished(bool success);
};

}

Q_DECLARE_INTERFACE(sapi::ISharerPlugin, "com.archangelsdy.silica.plugins.sharer")

#endif // ISHARER_H
