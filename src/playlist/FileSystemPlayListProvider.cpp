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
    qDeleteAll(m_entities.begin(), m_entities.end());
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

QList<PlayListEntity*> FileSystemPlayListProvider::entities() const
{
    QList<PlayListEntity*> ret;
    ret.reserve(m_entities.count());
    for (auto entity : m_entities) {
        ret.push_back(entity);
    }
    return ret;
}

void FileSystemPlayListProvider::loadEntities()
{
    qDeleteAll(m_entities.begin(), m_entities.end());
    m_entities.clear();

    QDirIterator iter(
        m_rootPath, ImageSourceManager::instance()->nameFilters(),
        QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    while (iter.hasNext()) {
        m_entities << new FileSystemPlayListEntity(this, iter.next());
    }

    emit entitiesChanged();
}

void FileSystemPlayListProvider::triggerEntity(PlayListEntity *entity)
{
    FileSystemPlayListEntity *fsEntity = static_cast<FileSystemPlayListEntity *>(entity);
    if (fsEntity->fileInfo().isDir()) {
        m_rootPath = fsEntity->fileInfo().absoluteFilePath();
        QtConcurrent::run([this]() {
            this->loadEntities();
        });
    } else {
        emit playListTriggered(entity);
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
