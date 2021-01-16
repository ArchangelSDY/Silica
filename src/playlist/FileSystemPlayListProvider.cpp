#include "FileSystemPlayListProvider.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QList>
#include <QSet>
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

    QSet<QString> validNameSuffixes = QSet<QString>::fromList(ImageSourceManager::instance()->nameSuffixes());
    QDirIterator iter(
        m_rootPath,
        QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    while (iter.hasNext()) {
        QString path = iter.next();
        if (iter.fileInfo().isFile() && !validNameSuffixes.contains(iter.fileInfo().suffix())) {
            continue;
        }
        entities << new FileSystemPlayListEntity(this, path);
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
