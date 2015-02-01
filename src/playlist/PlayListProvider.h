#ifndef PLAYLISTPROVIDER_H
#define PLAYLISTPROVIDER_H

#include <QObject>
#include <QVariantHash>

#include "image/ImageInfo.h"
#include "playlist/PlayListRecordInfo.h"

class PlayListProviderFactory;

class PlayListProvider : public QObject
{
    Q_OBJECT
public:
    explicit PlayListProvider(QObject *parent = 0) : QObject(parent) {}

    virtual ~PlayListProvider() {}

    virtual QString typeName() const = 0;

    virtual void request(const QString &name,
                         const QVariantHash &extra = QVariantHash()) = 0;

    virtual bool isImagesReadOnly() const { return true; }
    virtual bool insertImages(const PlayListRecordInfo &plrInfo,
                              const QList<ImageInfo> &imgInfos) { return false; }
    virtual bool removeImages(const PlayListRecordInfo &plrInfo,
                              const QList<ImageInfo> &imgInfos) { return false; }

    virtual void onPlayListRecordCreated(const PlayListRecordInfo &plrInfo,
                                       const QList<ImageInfo> &imgInfos) {}

signals:
    void gotItems(const QList<QUrl> &urls,
                  const QList<QVariantHash> &extraInfos);
};

#endif // PLAYLISTPROVIDER_H
