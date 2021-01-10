#pragma once

#include "playlist/PlayListProvider.h"

class FileSystemPlayListEntity;

class FileSystemPlayListProvider : public PlayListProvider
{
    Q_OBJECT
public:
    static const int TYPE = 1;

    FileSystemPlayListProvider(QObject *parent = nullptr);
    virtual ~FileSystemPlayListProvider() override;
    virtual int type() const override;
    virtual QString name() const override;
    virtual bool supportsOption(PlayListProviderOption option) const override;
    virtual QList<PlayListEntity *> loadEntities() override;
    virtual PlayListEntityTriggerResult triggerEntity(PlayListEntity *entity) override;
    virtual PlayListEntity *createEntity(const QString &name) override;
    virtual void insertEntity(PlayListEntity *entity) override;
    virtual void updateEntity(PlayListEntity *entity) override;
    virtual void removeEntity(PlayListEntity *entity) override;

    QString rootPath() const;
    void setRootPath(const QString &path);

signals:
    void rootPathChanged(const QString &rootPath);

private:
    QString m_rootPath;
};

