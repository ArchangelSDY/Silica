#include "PlayListRecord.h"

#include "db/LocalDatabase.h"
#include "playlist/PlayListProvider.h"
#include "playlist/PlayListProviderManager.h"
#include "../PlayList.h"

PlayListRecord::PlayListRecord() :
    QObject(0) ,
    m_id(PlayListRecord::EMPTY_ID) ,
    m_count(PlayListRecord::EMPTY_COUNT) ,
    m_coverIndex(PlayListRecord::EMPTY_COVER_INDEX) ,
    m_type(PlayListRecord::UNKNOWN_TYPE) ,
    m_provider(0) ,
    m_playList(0)
{
}

PlayListRecord::~PlayListRecord()
{
    if (m_provider) {
        m_provider->deleteLater();
    }
}

int PlayListRecord::coverIndex()
{
    if (m_coverIndex == PlayListRecord::EMPTY_COVER_INDEX) {
        QSharedPointer<PlayList> pl = playList();
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

QSharedPointer<PlayList> PlayListRecord::playList()
{
    if (!m_playList && m_provider) {
        m_playList.reset(new PlayList());
        m_playList->setRecord(this);

        QObject::connect(
            m_provider, SIGNAL(gotItems(QList<QUrl>,QList<QVariantHash>)),
            this, SLOT(gotItems(QList<QUrl>,QList<QVariantHash>)),
            static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection));
        QObject::connect(
            m_provider, SIGNAL(itemsCountChanged(int)),
            this, SLOT(providerItemsCountChanged(int)),
            static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection));

        m_provider->request(m_name, providerExtra());
    }

    return m_playList;
}

void PlayListRecord::reload()
{
    if (m_playList) {
        m_playList->clear();
        m_provider->request(m_name, providerExtra());
    }
}

QVariantHash PlayListRecord::providerExtra() const
{
    QVariantHash extra;
    extra.insert("id", m_id);
    return extra;
}

bool PlayListRecord::save()
{
    bool ok = false;
    if (m_id == PlayListRecord::EMPTY_ID) {
        ok = LocalDatabase::instance()->insertPlayListRecord(this);

        // Notify provider
        if (m_playList && m_provider && !m_provider->isImagesReadOnly()) {
            m_provider->onPlayListRecordCreated(*this, m_playList->toImageList());
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
    m_playList.reset();
}

bool PlayListRecord::removeImage(ImagePtr image)
{
    if (!m_provider || m_provider->isImagesReadOnly()){
        return false;
    }

    ImageList images;
    images << image;

    bool ret = m_provider->removeImages(*this, images);

    // Remove cached playlist. It will be re-generated in the future
    flushPlayList();

    return ret;
}

bool PlayListRecord::insertImages(const ImageList &images)
{
    if (!m_provider || m_provider->isImagesReadOnly()){
        return false;
    }

    // PlayListRecord should be saved already.
    if (!isSaved()) {
        return false;
    }

    bool ret = m_provider->insertImages(*this, images);

    // Remove cached playlist. It will be re-generated in the future
    flushPlayList();

    return ret;
}

void PlayListRecord::continueProvide()
{
    if (m_provider && m_provider->canContinueProvide()) {
        QVariantHash extra = providerExtra();
        extra.insert("continue", true);
        m_provider->request(m_name, extra);
    }
}

void PlayListRecord::gotItems(const QList<QUrl> &imageUrls,
                              const QList<QVariantHash> &extraInfos)
{
    QSharedPointer<PlayList> pl = QSharedPointer<PlayList>::create();
    for (int i = 0; i < imageUrls.count(); ++i) {
        ImagePtr image = pl->addSinglePath(imageUrls[i]);
        if (!image.isNull() && extraInfos.count() > i) {
            image->extraInfo() = extraInfos[i];
        }
    }
    m_playList->append(pl);
}

void PlayListRecord::providerItemsCountChanged(int count)
{
    m_count = count;
    save();
}

QList<PlayListRecord *> PlayListRecord::all()
{
    return LocalDatabase::instance()->queryPlayListRecords();
}


PlayListRecordBuilder::PlayListRecordBuilder() :
    m_record(new PlayListRecord()) ,
    m_isObtained(false)
{
}

PlayListRecordBuilder::~PlayListRecordBuilder()
{
    if (!m_isObtained) {
        delete m_record;
    }
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
    m_record->m_provider = PlayListProviderManager::instance()->create(type);
    return *this;
}

/*
PlayListRecordBuilder &PlayListRecordBuilder::setProvider(PlayListProvider *provider)
{
    m_record->m_provider = provider;
    return *this;
}
*/

PlayListRecordBuilder &PlayListRecordBuilder::setPlayList(QSharedPointer<PlayList> playlist)
{
    m_record->m_playList = playlist;
    return *this;
}

PlayListRecord *PlayListRecordBuilder::obtain()
{
    if (m_record->m_type != PlayListRecord::UNKNOWN_TYPE
            && m_record->m_provider == 0) {
        qWarning() << QString("PlayListRecordBuilder: Cannot find PlayListProvider for type = %1")
                      .arg(m_record->m_type);
        return 0;
    }

    m_isObtained = true;
    return m_record;
}
