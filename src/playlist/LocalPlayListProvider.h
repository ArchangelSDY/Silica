#ifndef LOCALPLAYLISTPROVIDER_H
#define LOCALPLAYLISTPROVIDER_H

#include "playlist/PlayListProvider.h"

class LocalPlayListProvider : public PlayListProvider
{
public:
    LocalPlayListProvider(QObject *parent = 0);
    ~LocalPlayListProvider();

    QString typeName() const;

    void request(const QString &name, const QVariantHash &extra);

    bool isImagesReadOnly() const;
    bool insertImages(const PlayListRecordInfo &plrInfo,
                      const QList<ImageInfo> &imgInfos);
    bool removeImages(const PlayListRecordInfo &plrInfo,
                      const QList<ImageInfo> &imgInfos);

    void onPlayListRecordCreated(const PlayListRecordInfo &plrInfo,
                                 const QList<ImageInfo> &imgInfos);
};

#endif // LOCALPLAYLISTPROVIDER_H
