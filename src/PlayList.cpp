#include <QtConcurrent>
#include <QEventLoop>
#include <functional>
#include <vector>
#include <opencv2/core/core.hpp>

#include "image/Image.h"
#include "image/ImageHistogram.h"
#include "image/ImageSource.h"
#include "image/ImageSourceManager.h"
#include "image/ImageThumbnailBlockLoader.h"
#include "playlist/AbstractPlayListFilter.h"
#include "playlist/DoNothingFilter.h"
#include "playlist/PlayListRecord.h"
#include "playlist/sort/Utils.h"
#include "PlayList.h"

PlayList::PlayList() :
    m_record(0) ,
    m_filter(AbstractPlayListFilter::defaultFilter())
{
}

PlayList::PlayList(const QList<QUrl> &imageUrls) :
    m_record(0) ,
    m_filter(AbstractPlayListFilter::defaultFilter())
{
    foreach(const QUrl &imageUrl, imageUrls) {
        addMultiplePath(imageUrl);
    }
}

PlayList::PlayList(const QStringList &imagePaths) :
    m_record(0) ,
    m_filter(AbstractPlayListFilter::defaultFilter())
{
    foreach(const QString &imagePath, imagePaths) {
        addMultiplePath(imagePath);
    }
}

PlayList::~PlayList()
{
    delete m_filter;
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

QList<QSharedPointer<Image> > &PlayList::operator<<(
        const QSharedPointer<Image> &image)
{
    ImageList l;
    l << image;
    return (*this << l);
}

QList<QSharedPointer<Image> > &PlayList::operator<<(
        const ImageList &images)
{
    int start = count();
    m_allImages << images;
    QList<QSharedPointer<Image> > &ret =
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

static bool isHistSame(QSharedPointer<ImageHistogram> left,
                       QSharedPointer<ImageHistogram> right)
{
    if (!left || !right) {
        return false;
    }

    double distance = left->compare(*right);
    return distance >= 0.95;
}

bool PlayList::groupLessThan(const QSharedPointer<Image> &left,
                                 const QSharedPointer<Image> &right)
{
    int leftGroup = this->m_imageGroups.value(left.data());
    int rightGroup = this->m_imageGroups.value(right.data());

    if (leftGroup != rightGroup) {
        return leftGroup < rightGroup;
    } else {
        return ImageNameLessThan(left->name(), right->name());
    }
}

void PlayList::sortByGroup()
{
    // Make groups if needed
    if (m_imageGroups.count() != count()) {
        groupByThumbHist();
    }

    auto lessThanFunc = std::bind(&PlayList::groupLessThan, this,
                                  std::placeholders::_1,
                                  std::placeholders::_2);
    qSort(m_filteredImages.begin(), m_filteredImages.end(), lessThanFunc);

    emit itemsChanged();
}

int PlayList::groupForImage(Image * const image)
{
    return m_imageGroups.value(image, -1);
}

static QSharedPointer<ImageHistogram> computeImageHistogram(ImagePtr image)
{
    ImageThumbnailBlockLoader loader(image);
    loader.loadAndWait();

    QSharedPointer<QImage> thumbnail = loader.thumbnail();

    return QSharedPointer<ImageHistogram>::create(*thumbnail);
}

void PlayList::groupByThumbHist()
{
    std::vector<QSharedPointer<ImageHistogram> > hists;

    // Load thumbnail and compute thumbnail histograms
    QFuture<QSharedPointer<ImageHistogram> > f = QtConcurrent::mapped(m_allImages, computeImageHistogram);
    f.waitForFinished();
    for (QSharedPointer<ImageHistogram> hist : f) {
        hists.push_back(hist);
    }

    Q_ASSERT(hists.size() == m_allImages.size());

    std::vector<int> labels;
    cv::partition(hists, labels, isHistSame);

    Q_ASSERT(labels.size() == m_allImages.size());

    m_imageGroups.clear();
    for (int i = 0; i < m_allImages.size(); ++i) {
        m_imageGroups.insert(m_allImages[i].data(), labels[i]);
    }
}

void PlayList::setFilter(AbstractPlayListFilter *filter)
{
    if (m_filter) {
        delete m_filter;
    }

    m_filter = filter ? filter : new DoNothingFilter();
    m_filteredImages = m_filter->filtered(m_allImages);

    emit itemsChanged();
}
