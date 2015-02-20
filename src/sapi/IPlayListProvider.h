#ifndef IPLAYLISTPROVIDER_H
#define IPLAYLISTPROVIDER_H

#include <QUrl>
#include <QVariant>

#include "sapi_global.h"

namespace sapi {

class SAPI_EXPORT IPlayListProvider : public QObject
{
    Q_OBJECT
public:
    virtual ~IPlayListProvider();

    virtual void request(const QString &name,
                         const QVariantHash &extras) = 0;

signals:
    void gotItems(const QList<QUrl> &urls,
                  const QList<QVariantHash> &extraInfos);
    void itemsCountChanged(int count);
};

}

#endif // IPLAYLISTPROVIDER_H
