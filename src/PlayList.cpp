#include "image/Image.h"
#include "image/ImageHistogram.h"
#include "image/ImageSource.h"
#include "image/ImageSourceManager.h"
#include "playlist/AbstractPlayListFilter.h"
#include "playlist/DoNothingFilter.h"
//#include "playlist/PlayListRecord.h"
#include "playlist/sort/Utils.h"
#include "PlayList.h"

PlayList::PlayList() :
    //m_record(0) ,
    m_filter(AbstractPlayListFilter::defaultFilter())
{
}

PlayList::PlayList(const QList<QUrl> &imageUrls) :
    //m_record(0) ,
    m_filter(AbstractPlayListFilter::defaultFilter())
{
    foreach(const QUrl &imageUrl, imageUrls) {
        addMultiplePath(imageUrl);
    }
}

PlayList::PlayList(const QStringList &imagePaths) :
    //m_record(0) ,
    m_filter(AbstractPlayListFilter::defaultFilter())
{
    foreach(const QString &imagePath, imagePaths) {
        addMultiplePath(imagePath);
    }
}

PlayList::~PlayList()
{
}

void PlayList::append(const QSharedPointer<Image> &image)
{
    ImageList l;
    l << image;
    append(l);
}

void PlayList::append(const ImageList &images)
{
    int start = count();
    m_allImages.append(images);
    m_filteredImages.append(m_filter->filtered(images));
    emit itemsAppended(start);
}

void PlayList::append(QSharedPointer<PlayList> playList)
{
    int start = count();
    m_allImages.append(playList->m_allImages);
    m_filteredImages.append(playList->m_filteredImages);
    emit itemsAppended(start);
}

void PlayList::replace(int index, const ImagePtr &image)
{
    ImagePtr oldImage = m_filteredImages[index];
    int allIndex = m_allImages.indexOf(oldImage);
    m_allImages.replace(allIndex, image);
    m_filteredImages.replace(index, image);
    emit itemChanged(index);
}

QList<ImagePtr> &PlayList::operator<<(const ImagePtr &image)
{
    ImageList l;
    l << image;
    return (*this << l);
}

QList<ImagePtr> &PlayList::operator<<(const ImageList &images)
{
    int start = count();
    m_allImages << images;
    QList<ImagePtr> &ret =
        m_filteredImages << m_filter->filtered(images);
    emit itemsAppended(start);
    return ret;
}

void PlayList::clear()
{
    m_allImages.clear();
    m_filteredImages.clear();
    emit itemsChanged();
}

ImagePtr PlayList::addSinglePath(const QString &path)
{
    QSharedPointer<ImageSource> imageSource(ImageSourceManager::instance()->createSingle(path));
    if (imageSource) {
        ImagePtr image(new Image(imageSource));
        *this << image;
        return image;
    } else {
        return ImagePtr();
    }
}

ImagePtr PlayList::addSinglePath(const QUrl &url)
{
    if (url.isEmpty()) {
        return ImagePtr();
    }

    QSharedPointer<ImageSource> imageSource(ImageSourceManager::instance()->createSingle(url));
    if (imageSource) {
        ImagePtr image(new Image(imageSource));
        *this << image;
        return image;
    } else {
        return ImagePtr();
    }
}

void PlayList::addMultiplePath(const QString &path)
{
    QList<ImageSource *> imageSources =
        ImageSourceManager::instance()->createMultiple(path);
    foreach (ImageSource *imageSource, imageSources) {
        *this << QSharedPointer<Image>(new Image(QSharedPointer<ImageSource>(imageSource)));
    }
}

void PlayList::addMultiplePath(const QUrl &url)
{
    if (url.isEmpty()) {
        return;
    }

    QList<ImageSource *> imageSources =
        ImageSourceManager::instance()->createMultiple(url);
    foreach (ImageSource *imageSource, imageSources) {
        *this << QSharedPointer<Image>(new Image(QSharedPointer<ImageSource>(imageSource)));
    }
}

bool PlayList::removeOne(const ImagePtr &val)
{
    bool ret = m_allImages.removeOne(val);
    m_filteredImages.removeOne(val);

    emit itemsChanged();
    return ret;
}

void PlayList::removeAt(int index)
{
    ImagePtr removed = m_allImages.takeAt(index);
    m_filteredImages.removeOne(removed);

    emit itemsChanged();
}

void PlayList::sortBy(AbstractPlayListSorter *sorter)
{
    sorter->sort(m_filteredImages.begin(), m_filteredImages.end());
    emit itemsChanged();
}

QString PlayList::groupNameOf(const ImagePtr &image) const
{
    if (m_grouper) {
        return m_grouper->groupNameOf(image.data());
    } else {
        return QStringLiteral("");
    }
}

void PlayList::groupBy(AbstractPlayListGrouper *grouper)
{
    m_grouper.reset(grouper);
    m_grouper->group(m_allImages.begin(), m_allImages.end());
    m_filteredImages = m_filter->filtered(m_allImages);
    emit itemsChanged();
}

void PlayList::setFilter(AbstractPlayListFilter *filter)
{
    m_filter.reset(filter ? filter : new DoNothingFilter());
    m_filteredImages = m_filter->filtered(m_allImages);

    emit itemsChanged();
}
