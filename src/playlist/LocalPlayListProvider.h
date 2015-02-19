#ifndef LOCALPLAYLISTPROVIDER_H
#define LOCALPLAYLISTPROVIDER_H

#include "playlist/PlayListProvider.h"

class LocalPlayListProvider : public PlayListProvider
{
public:
    LocalPlayListProvider(QObject *parent = 0);
    ~LocalPlayListProvider();

    QString typeName() const;
    bool canContinueProvide() const;

    void request(const QString &name, const QVariantHash &extra);

    bool isImagesReadOnly() const;
    bool insertImages(const PlayListRecord &record, const ImageList &images);
    bool removeImages(const PlayListRecord &record, const ImageList &images);
    void onPlayListRecordCreated(const PlayListRecord &record,
                                 const ImageList &images);
};

#endif // LOCALPLAYLISTPROVIDER_H
