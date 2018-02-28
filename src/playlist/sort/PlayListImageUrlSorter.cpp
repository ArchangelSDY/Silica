#include "PlayListImageUrlSorter.h"

#include "image/ImageSource.h"

void PlayListImageUrlSorter::sort(ImageList::iterator begin, ImageList::iterator end)
{
    qSort(begin, end, [](const ImagePtr &left, const ImagePtr &right) -> bool {
        QUrl leftUrl = left->source() ? left->source()->url() : QUrl();
        QUrl rightUrl = right->source() ? right->source()->url() : QUrl();
        return leftUrl.toString() < rightUrl.toString();
    });
}

