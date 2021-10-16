#include "PlayListImageAspectRatioSorter.h"

void PlayListImageAspectRatioSorter::sort(ImageList::iterator begin, ImageList::iterator end)
{
    std::sort(begin, end, [](const ImagePtr &left, const ImagePtr &right) -> bool {
        return left->aspectRatio() < right->aspectRatio();
    });
}
