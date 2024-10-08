#ifndef ISHARERPLUGIN_H
#define ISHARERPLUGIN_H

#include <QSharedPointer>
#include <QObject>

#include "sapi/ImageResource.h"
#include "sapi/sapi_global.h"

namespace sapi {

class SAPI_EXPORT ISharerPlugin : public QObject
{
    Q_OBJECT
public:
    virtual ~ISharerPlugin();
    virtual bool share(const QList<QSharedPointer<sapi::ImageResource>> &images) = 0;

signals:
    void finished(bool success);
};

}

Q_DECLARE_INTERFACE(sapi::ISharerPlugin, "com.archangelsdy.silica.plugins.sharer")

#endif // ISHARERPLUGIN_H
