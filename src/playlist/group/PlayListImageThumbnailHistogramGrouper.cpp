#include "PlayListImageThumbnailHistogramGrouper.h"

#include <algorithm>

#include <QtConcurrent>
#include <QEventLoop>
#include <opencv2/core/core.hpp>

#include "image/ImageHistogram.h"
#include "image/ImageThumbnailBlockLoader.h"
#include "playlist/sort/Utils.h"

static QSharedPointer<ImageHistogram> computeImageHistogram(ImagePtr image)
{
    ImageThumbnailBlockLoader loader(image);
    loader.loadAndWait();

    QSharedPointer<QImage> thumbnail = loader.thumbnail();

    return QSharedPointer<ImageHistogram>::create(*thumbnail);
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

void PlayListImageThumbnailHistogramGrouper::group(ImageList::iterator begin, ImageList::iterator end)
{
    std::vector<QSharedPointer<ImageHistogram> > hists;

    // Load thumbnail and compute thumbnail histograms
    QFuture<QSharedPointer<ImageHistogram> > f = QtConcurrent::mapped(begin, end, computeImageHistogram);
    f.waitForFinished();
    for (QSharedPointer<ImageHistogram> hist : f) {
        hists.push_back(hist);
    }

    // Partition
    std::vector<int> labels;
    int groupCount = cv::partition(hists, labels, isHistSame);

    m_groupNames.clear();
    for (int i = 0; i < groupCount; i++) {
        m_groupNames << QStringLiteral("Group %1").arg(i);
    }

    for (auto it = begin; it != end; it++) {
        m_imageGroups[it->data()] = labels[it - begin];
    }

    // Sort by group
    std::sort(begin, end, [this](const ImagePtr &left, const ImagePtr &right) -> bool {
        int leftLabel = this->m_imageGroups[left.data()];
        int rightLabel = this->m_imageGroups[right.data()];
        if (leftLabel != rightLabel) {
            return leftLabel < rightLabel;
        } else {
            return ImageNameLessThan(left->name(), right->name()); 
        }
    });
}
