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
    // TODO: Support renaming
    return option == PlayListProviderOption::RemoveEntity;
}

QList<PlayListEntity *> FileSystemPlayListProvider::loadEntities()
{
    QList<PlayListEntity *> entities;

    QDirIterator iter(
        m_rootPath,
        QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    while (iter.hasNext()) {
        iter.next();
        if (iter.fileInfo().isFile() && !ImageSourceManager::instance()->isValidNameSuffix(iter.fileInfo().suffix())) {
            continue;
        }
        entities << new FileSystemPlayListEntity(this, iter.fileInfo());
    }

    return entities;
}

PlayListEntityTriggerResult FileSystemPlayListProvider::triggerEntity(PlayListEntity *entity)
{
    FileSystemPlayListEntity *fsEntity = static_cast<FileSystemPlayListEntity *>(entity);
    if (fsEntity->fileInfo().isDir()) {
        auto rootPath = fsEntity->fileInfo().absoluteFilePath();
        setRootPath(rootPath);
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
    FileSystemPlayListEntity *fsEntity = static_cast<FileSystemPlayListEntity *>(entity);
    QFileInfo fileInfo = fsEntity->fileInfo();
    QFile::moveToTrash(fileInfo.absoluteFilePath());
    emit entitiesChanged();
}

QString FileSystemPlayListProvider::rootPath() const
{
    return m_rootPath;
}

void FileSystemPlayListProvider::setRootPath(const QString &path)
{
    if (m_rootPath != path) {
        m_rootPath = path;
        emit rootPathChanged(m_rootPath);
        emit entitiesChanged();
    }
}
