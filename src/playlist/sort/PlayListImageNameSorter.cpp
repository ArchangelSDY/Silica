#include "PlayListImageNameSorter.h"

#include "Utils.h"

void PlayListImageNameSorter::sort(ImageList::iterator begin, ImageList::iterator end)
{
    std::sort(begin, end, [](const auto &left, const auto &right) -> bool {
        return ImageNameLessThan(left->name(), right->name());
    });
}
