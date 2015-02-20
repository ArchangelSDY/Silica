#ifndef PLAYLISTPROVIDER_H
#define PLAYLISTPROVIDER_H

#include <QObject>
#include <QVariantHash>

#include "image/Image.h"

class PlayListProviderFactory;
class PlayListRecord;

class PlayListProvider : public QObject
{
    Q_OBJECT
public:
    explicit PlayListProvider(QObject *parent = 0) : QObject(parent) {}

    virtual ~PlayListProvider() {}

    virtual QString typeName() const = 0;
    virtual bool canContinueProvide() const = 0;

    virtual void request(const QString &name,
                         const QVariantHash &extra = QVariantHash()) = 0;

    virtual bool isImagesReadOnly() const { return true; }
    virtual bool insertImages(const PlayListRecord &record,
                              const ImageList &images) { return false; }
    virtual bool removeImages(const PlayListRecord &record,
                              const ImageList &images) { return false; }

    virtual void onPlayListRecordCreated(const PlayListRecord &record,
                                       const ImageList &images) {}

signals:
    void gotItems(const QList<QUrl> &urls,
                  const QList<QVariantHash> &extraInfos);
    void itemsCountChanged(int count);
};

#endif // PLAYLISTPROVIDER_H
