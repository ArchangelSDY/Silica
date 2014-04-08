#include "ImageSourceManager.h"
#include "PlayList.h"

PlayList::PlayList() :
    m_record(0) ,
    m_eventEmitter(new PlayListEventEmitter())
{
}

PlayList::PlayList(const QList<QUrl> &imageUrls) :
    m_record(0) ,
    m_eventEmitter(new PlayListEventEmitter())
{
    foreach(const QUrl &imageUrl, imageUrls) {
        addPath(imageUrl);
    }
}

PlayList::~PlayList()
{
    delete m_eventEmitter;
}

void PlayList::addPath(const QString &path)
{
    QList<ImageSource *> imageSources =
        ImageSourceManager::instance()->createMultiple(path);
    foreach (ImageSource *imageSource, imageSources) {
        *this << QSharedPointer<Image>(new Image(imageSource));
    }
}

void PlayList::addPath(const QUrl &url)
{
    if (url.isEmpty()) {
        return;
    }

    QList<ImageSource *> imageSources =
        ImageSourceManager::instance()->createMultiple(url);
    foreach (ImageSource *imageSource, imageSources) {
        *this << QSharedPointer<Image>(new Image(imageSource));
    }
}

static bool imageNameLessThan(const QSharedPointer<Image> &left,
                              const QSharedPointer<Image> &right)
{
    return left->name() < right->name();
}

void PlayList::sortByName()
{
    qSort(this->begin(), this->end(), imageNameLessThan);
}

static bool imageAspectRatioLessThan(const QSharedPointer<Image> &left,
                                     const QSharedPointer<Image> &right)
{
    return left->aspectRatio() < right->aspectRatio();
}

void PlayList::sortByAspectRatio()
{
    qSort(this->begin(), this->end(), imageAspectRatioLessThan);
}
