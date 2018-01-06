#include "PlayListImageSizeSorter.h"

#include "Utils.h"

void PlayListImageSizeSorter::sort(ImageList::iterator begin, ImageList::iterator end)
{
    qSort(begin, end, [](const ImagePtr &left, const ImagePtr &right) -> bool {
        const QSize leftSize = left->size();
        const QSize rightSize = right->size();

        if (leftSize.width() != rightSize.width()) {
            return leftSize.width() < rightSize.width();
        }

        if (leftSize.height() != rightSize.height()) {
            return leftSize.height() < rightSize.height();
        }

        return ImageNameLessThan(left->name(), right->name());
    });
}
