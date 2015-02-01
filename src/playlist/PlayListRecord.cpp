#include "PlayListRecord.h"

#include "db/LocalDatabase.h"
#include "playlist/PlayListProvider.h"
#include "PlayList.h"

PlayListRecord::PlayListRecord() :
    QObject(0) ,
    m_id(PlayListRecord::EMPTY_ID) ,
    m_count(PlayListRecord::EMPTY_COUNT) ,
    m_coverIndex(PlayListRecord::EMPTY_COVER_INDEX) ,
    m_type(PlayListRecord::UNKNOWN_TYPE) ,
    m_provider(0) ,
    m_playList(0) ,
    m_ownPlayList(true) // PlayList not given and will be created by our self
{
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

QString PlayListRecord::typeName() const
{
    return m_provider ? m_provider->typeName() : QString();
}

PlayListRecordInfo PlayListRecord::info() const
{
    PlayListRecordInfo info;
    info.id = id();
    return info;
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

        // Notify provider
        if (m_playList && m_provider && !m_provider->isImagesReadOnly()) {
            QList<ImageInfo> imageInfos;
            imageInfos.reserve(m_playList->count());
            for (int i = 0; i < m_playList->count(); ++i) {
                imageInfos << m_playList->at(i)->info();
            }
            m_provider->onPlayListRecordCreated(info(), imageInfos);
        }
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
    if (!m_provider && m_provider->isImagesReadOnly()){
        return false;
    }

    QList<ImageInfo> imgInfos;
    imgInfos.append(image->info());

    bool ret = m_provider->removeImages(info(), imgInfos);

    // Remove cached playlist. It will be re-generated in the future
    flushPlayList();

    return ret;
}

bool PlayListRecord::insertImages(const ImageList &images)
{
    if (!m_provider && m_provider->isImagesReadOnly()){
        return false;
    }

    // PlayListRecord should be saved already.
    if (!isSaved()) {
        return false;
    }

    QList<ImageInfo> imgInfos;
    imgInfos.reserve(images.count());
    foreach (const ImagePtr &image, images) {
        imgInfos.append(image->info());
    }

    bool ret = m_provider->insertImages(info(), imgInfos);

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


PlayListRecordBuilder::PlayListRecordBuilder() :
    m_record(new PlayListRecord())
{
}

PlayListRecordBuilder &PlayListRecordBuilder::setId(int id)
{
    m_record->m_id = id;
    return *this;
}

PlayListRecordBuilder &PlayListRecordBuilder::setName(const QString &name)
{
    m_record->m_name = name;
    return *this;
}

PlayListRecordBuilder &PlayListRecordBuilder::setCoverPath(const QString &coverPath)
{
    m_record->m_coverPath = coverPath;
    return *this;
}

PlayListRecordBuilder &PlayListRecordBuilder::setCount(int count)
{
    m_record->m_count = count;
    return *this;
}

PlayListRecordBuilder &PlayListRecordBuilder::setType(int type)
{
    m_record->m_type = type;
    return *this;
}

PlayListRecordBuilder &PlayListRecordBuilder::setProvider(PlayListProvider *provider)
{
    m_record->m_provider = provider;
    return *this;
}

PlayListRecordBuilder &PlayListRecordBuilder::setPlayList(PlayList *playlist)
{
    m_record->m_playList = playlist;
    m_record->m_ownPlayList = false;
    return *this;
}

PlayListRecord *PlayListRecordBuilder::obtain()
{
    return m_record;
}
