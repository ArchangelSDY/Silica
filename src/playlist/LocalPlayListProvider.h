#ifndef LOCALPLAYLISTPROVIDER_H
#define LOCALPLAYLISTPROVIDER_H

#include "playlist/PlayListProvider.h"

class LocalPlayListProvider : public PlayListProvider
{
public:
    LocalPlayListProvider(QObject *parent = 0);
    ~LocalPlayListProvider();

    void request(const QString &name, const QVariantHash &extra);
};

#endif // LOCALPLAYLISTPROVIDER_H
