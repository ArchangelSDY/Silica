#pragma once

#include <QFileInfo>

#include "playlist/FileSystemPlayListProvider.h"
#include "playlist/PlayListEntity.h"

class FileSystemPlayListEntity : public PlayListEntity
{
public:
    FileSystemPlayListEntity(FileSystemPlayListProvider *provider, const QFileInfo &fileInfo);
    virtual ~FileSystemPlayListEntity() override;
    virtual int count() const override;
    virtual QString name() const override;
    virtual bool supportsOption(PlayListEntityOption option) const override;
    virtual QImage loadCoverImage() override;
    virtual PlayListEntityLoadContext* createLoadContext() override;
    virtual QList<QUrl> loadImageUrls(PlayListEntityLoadContext *ctx) override;
    virtual void setName(const QString &name) override;
    virtual void setCoverImagePath(const QString &path) override;
    virtual void addImageUrls(const QList<QUrl> &imageUrls) override;
    virtual void removeImageUrls(const QList<QUrl> &imageUrls) override;

    QFileInfo fileInfo() const;

private:
    friend void FileSystemPlayListProvider::insertEntity(PlayListEntity *);
    friend void FileSystemPlayListProvider::updateEntity(PlayListEntity *);
    friend void FileSystemPlayListProvider::removeEntity(PlayListEntity *);

    QFileInfo m_fileInfo;
};

