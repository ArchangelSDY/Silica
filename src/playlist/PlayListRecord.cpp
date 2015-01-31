#include "PlayListRecord.h"

#include "db/LocalDatabase.h"
#include "PlayList.h"

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
    m_type(PlayListRecord::UNKNOWN_TYPE) ,
    m_playList(playList)
{
    // If playList is given, then we do not own this playList and will not try
    // to delete it.
    m_ownPlayList = (playList == 0);
}

void PlayListRecord::setCount(int count)
{
    // FIXME
    // RemotePlayList does not have a fixed count
    // if (m_type == PlayListRecord::RemotePlayList) {
    //     return;
    // }

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

int PlayListRecord::type() const
{
    return m_type;
}

void PlayListRecord::setType(int type)
{
    m_type = type;
}

PlayList *PlayListRecord::playList()
{
    return m_playList;
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
    if (m_playList && m_ownPlayList) {
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

void PlayListRecord::gotItems(const QList<QUrl> &imageUrls,
                              const QList<QVariantHash> &extraInfos)
{
    for (int i = 0; i < imageUrls.count(); ++i) {
        ImagePtr image = m_playList->addSinglePath(imageUrls[i]);
        if (!image.isNull()) {
            image->extraInfo() = extraInfos[i];
        }
    }
}

QList<PlayListRecord *> PlayListRecord::all()
{
    return LocalDatabase::instance()->queryPlayListRecords();
}
