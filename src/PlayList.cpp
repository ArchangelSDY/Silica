#include "ImageSourceManager.h"
#include "PlayList.h"

#include "DoNothingFilter.h"

PlayList::PlayList() :
    m_record(0) ,
    m_filter(new DoNothingFilter())
{
}

PlayList::PlayList(const QList<QUrl> &imageUrls) :
    m_record(0) ,
    m_filter(new DoNothingFilter())
{
    foreach(const QUrl &imageUrl, imageUrls) {
        addPath(imageUrl);
    }
}

PlayList::~PlayList()
{
    delete m_filter;
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

void PlayList::watchedPlayListAppended(int start)
{
    int oldCount = count();
    PlayList *watched = static_cast<PlayList *>(sender());
    for (int i = start; i < watched->count(); ++i) {
        const ImagePtr &image = watched->at(i);

        ImageList l;
        l << image;

        m_allImages << l;
        m_filteredImages << m_filter->filtered(l);
    }
    emit itemsAppended(oldCount);
}

static bool imageNameLessThan(const QSharedPointer<Image> &left,
                              const QSharedPointer<Image> &right)
{
    return left->name() < right->name();
}

void PlayList::sortByName()
{
    qSort(m_filteredImages.begin(), m_filteredImages.end(), imageNameLessThan);
    emit itemsChanged();
}

static bool imageAspectRatioLessThan(const QSharedPointer<Image> &left,
                                     const QSharedPointer<Image> &right)
{
    return left->aspectRatio() < right->aspectRatio();
}

void PlayList::sortByAspectRatio()
{
    qSort(m_filteredImages.begin(), m_filteredImages.end(), imageAspectRatioLessThan);
    emit itemsChanged();
}

void PlayList::setFilter(AbstractPlayListFilter *filter)
{
    m_filter = filter ? filter : new DoNothingFilter();
    m_filteredImages = m_filter->filtered(m_allImages);
}
