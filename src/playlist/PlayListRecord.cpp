#include "PlayListRecord.h"

#include "db/LocalDatabase.h"
#include "playlist/PlayListProvider.h"
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
    m_provider(0) ,
    m_playList(playList)
{
    // If playList is given, then we do not own this playList and will not try
    // to delete it.
    m_ownPlayList = (playList == 0);
}

PlayListRecord::~PlayListRecord()
{
    if (m_ownPlayList && m_playList) {
        m_playList->deleteLater();
    }
    if (m_provider) {
        m_provider->deleteLater();
    }
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

void PlayListRecord::setPlayListProvider(PlayListProvider *provider)
{
    m_provider = provider;
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
    if (!m_playList && m_provider) {
        m_playList = new PlayList();

        QObject::connect(
            m_provider, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)),
            this, SLOT(gotItems(QList<QUrl>,QList<QVariantHash>)));

        QVariantHash extra;
        extra.insert("id", m_id);
        m_provider->request(m_name, extra);
    }

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
    PlayList pl;
    for (int i = 0; i < imageUrls.count(); ++i) {
        ImagePtr image = pl.addSinglePath(imageUrls[i]);
        if (!image.isNull() && extraInfos.count() > i) {
            image->extraInfo() = extraInfos[i];
        }
    }
    m_playList->append(&pl);
}

QList<PlayListRecord *> PlayListRecord::all()
{
    return LocalDatabase::instance()->queryPlayListRecords();
}
