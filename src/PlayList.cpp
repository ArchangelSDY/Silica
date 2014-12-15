#include <QRegExp>
#include <functional>
#include <vector>
#include <opencv2/core/core.hpp>

#include "image/Image.h"
#include "image/ImageHistogram.h"
#include "image/ImageSourceManager.h"
#include "playlist/DoNothingFilter.h"
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
        addPath(imageUrl);
    }
}

PlayList::PlayList(const PlayList &playList) :
    m_allImages(playList.m_allImages) ,
    m_filteredImages(playList.m_filteredImages) ,
    m_record(playList.m_record) ,
    m_filter(playList.m_filter)
{
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
    static QRegExp numberRegex("(\\d+)");

    if (left->name().split(numberRegex) == right->name().split(numberRegex)) {
        // Names are similar except number parts

        // Extract number parts from left
        numberRegex.indexIn(left->name());
        QStringList leftNumberStrs = numberRegex.capturedTexts();
        QList<int> leftNumbers;
        foreach (const QString &str, leftNumberStrs) {
            leftNumbers << str.toInt();
        }

        // Extract number parts from right
        numberRegex.indexIn(right->name());
        QStringList rightNumberStrs = numberRegex.capturedTexts();
        QList<int> rightNumbers;
        foreach (const QString &str, rightNumberStrs) {
            rightNumbers << str.toInt();
        }

        int len = qMin(leftNumbers.length(), rightNumbers.length());
        if (len > 0) {
            // Compare using first different number
            for (int i = 0; i < len; ++i) {
                int delta = rightNumbers[i] - leftNumbers[i];
                if (delta != 0) {
                    return delta > 0;
                }
            }
        }
    }

    // Fallback to string comparison
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
    qSort(m_filteredImages.begin(), m_filteredImages.end(),
          imageAspectRatioLessThan);
    emit itemsChanged();
}

static bool isHistSame(ImageHistogram* const &left,
                        ImageHistogram* const &right)
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
        return imageNameLessThan(left, right);
    }
}

void PlayList::sortByGroup()
{
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

void PlayList::groupByThumbHist()
{
    std::vector<ImageHistogram *> hists;

    ImageList::const_iterator it = m_allImages.begin();
    while (it != m_allImages.end()) {
        hists.push_back((*it)->thumbHist());
        ++it;
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
