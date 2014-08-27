#include "LocalDatabase.h"
#include "LocalPlayListRecord.h"
#include "PlayList.h"
#include "PlayListRecord.h"
#include "RemotePlayListRecord.h"

PlayListRecord::PlayListRecord(const QString &name,
                               const QString &coverPath,
                               PlayList *playList,
                               QObject *parent) :
    QObject(parent) ,
    m_id(PlayListRecord::EMPTY_ID) ,
    m_name(name) ,
    m_coverPath(coverPath) ,
    m_count(PlayListRecord::EMPTY_COUNT) ,
    m_coverIndex(PlayListRecord::EMPTY_COVER_INDEX) ,
    m_playList(playList)
{
}

void PlayListRecord::setCount(int count)
{
    // RemotePlayList does not have a fixed count
    if (m_type == PlayListRecord::RemotePlayList) {
        return;
    }

    m_count = count;
}

int PlayListRecord::coverIndex()
{
    if (m_coverIndex == PlayListRecord::EMPTY_COVER_INDEX) {
        PlayList *pl = playList();
        for (int i = 0; i < pl->count(); ++i) {
            if (pl->at(i)->thumbnailPath() == m_coverPath) {
                m_coverIndex = i;
                break;
            }
        }
    }

    return m_coverIndex;
}

bool PlayListRecord::save()
{
    bool ok = false;
    if (m_id == PlayListRecord::EMPTY_ID) {
        ok = LocalDatabase::instance()->insertPlayListRecord(this);
    } else {
        ok = LocalDatabase::instance()->updatePlayListRecord(this);
    }

    emit saved();
    return ok;
}

bool PlayListRecord::remove()
{
    return LocalDatabase::instance()->removePlayListRecord(this);
}

void PlayListRecord::flushPlayList()
{
    if (m_playList) {
        delete m_playList;
        m_playList = 0;
    }
}

bool PlayListRecord::removeImage(ImagePtr image)
{
    bool ret = LocalDatabase::instance()->removeImageFromPlayListRecord(
        this, image);

    // Remove cached playlist. It will be re-generated in the future
    flushPlayList();

    return ret;
}

bool PlayListRecord::insertImages(const ImageList &images)
{
    bool ret = LocalDatabase::instance()->insertImagesForPlayListRecord(
        this, images);

    // Remove cached playlist. It will be re-generated in the future
    flushPlayList();

    return ret;
}

QList<PlayListRecord *> PlayListRecord::all()
{
    return LocalDatabase::instance()->queryPlayListRecords();
}

PlayListRecord *PlayListRecord::create(PlayListType type, const QString &name,
                                       const QString &coverPath)
{
    if (type == PlayListRecord::LocalPlayList) {
        return new LocalPlayListRecord(name, coverPath);
    } else if (type == PlayListRecord::RemotePlayList) {
        return new RemotePlayListRecord(name, coverPath);
    } else {
        return 0;
    }
}
