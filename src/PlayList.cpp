#include "ImageSourceManager.h"
#include "PlayList.h"

PlayList::PlayList() :
    m_record(0)
{
}

PlayList::PlayList(const QList<QUrl> &imageUrls) :
    m_record(0)
{
    foreach(const QUrl &imageUrl, imageUrls) {
        addPath(imageUrl);
    }
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
    // qSort(this->begin(), this->end(), imageNameLessThan);
    qSort(m_images.begin(), m_images.end(), imageNameLessThan);
    // TODO: emit signal itemChanged
}

static bool imageAspectRatioLessThan(const QSharedPointer<Image> &left,
                                     const QSharedPointer<Image> &right)
{
    return left->aspectRatio() < right->aspectRatio();
}

void PlayList::sortByAspectRatio()
{
    // qSort(this->begin(), this->end(), imageAspectRatioLessThan);
    qSort(m_images.begin(), m_images.end(), imageAspectRatioLessThan);
    // TODO: emit signal itesChanged
}
