#include "PlayListImageNameSorter.h"

#include "Utils.h"

void PlayListImageNameSorter::sort(ImageList::iterator begin, ImageList::iterator end)
{
    qSort(begin, end, [](const ImagePtr &left, const ImagePtr &right) -> bool {
        return ImageNameLessThan(left->name(), right->name());
    });
}
