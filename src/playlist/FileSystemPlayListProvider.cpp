#include "FileSystemPlayListProvider.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QList>
#include <QUrl>
#include <QtConcurrent>

#include "image/ImageSourceManager.h"
#include "playlist/FileSystemPlayListEntity.h"

FileSystemPlayListProvider::FileSystemPlayListProvider(QObject *parent) :
    PlayListProvider(parent)
{
}

FileSystemPlayListProvider::~FileSystemPlayListProvider()
{
}

int FileSystemPlayListProvider::type() const
{
    return FileSystemPlayListProvider::TYPE;
}

QString FileSystemPlayListProvider::name() const
{
    return QObject::tr("FileSystem");
}

bool FileSystemPlayListProvider::supportsOption(PlayListProviderOption option) const
{
    return false;
    // TODO: Support renaming and removing
    // return option == PlayListProviderOption::UpdateEntity
    //     || option == PlayListProviderOption::RemoveEntity;
}

QList<PlayListEntity *> FileSystemPlayListProvider::loadEntities()
{
    QList<PlayListEntity *> entities;

    QDirIterator iter(
        m_rootPath, ImageSourceManager::instance()->nameFilters(),
        QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    while (iter.hasNext()) {
        entities << new FileSystemPlayListEntity(this, iter.next());
    }

    return entities;
}

PlayListEntityTriggerResult FileSystemPlayListProvider::triggerEntity(PlayListEntity *entity)
{
    FileSystemPlayListEntity *fsEntity = static_cast<FileSystemPlayListEntity *>(entity);
    if (fsEntity->fileInfo().isDir()) {
        m_rootPath = fsEntity->fileInfo().absoluteFilePath();
        emit entitiesChanged();
        return PlayListEntityTriggerResult::None;
    } else {
        return PlayListEntityTriggerResult::LoadPlayList;
    }
}

PlayListEntity *FileSystemPlayListProvider::createEntity(const QString &name)
{
    Q_UNREACHABLE();
    return nullptr;
}

void FileSystemPlayListProvider::insertEntity(PlayListEntity *entity)
{
    Q_UNREACHABLE();
}

void FileSystemPlayListProvider::updateEntity(PlayListEntity *entity)
{
}

void FileSystemPlayListProvider::removeEntity(PlayListEntity *entity)
{
}
