#include "AbstractPlayListFilter.h"

AbstractPlayListFilter::AbstractPlayListFilter(
        AbstractPlayListFilter *parentFilter) :
    m_parentFilter(parentFilter)
{
}

ImageList AbstractPlayListFilter::filtered(const ImageList &images)
{
    ImageList filtered =
        m_parentFilter ? m_parentFilter->filtered(images) : images;

    ImageList::iterator it = filtered.begin();
    for (; it != filtered.end(); ++it) {
        if (!filter(*it)) {
            filtered.erase(it);
        }
    }

    return filtered;
}
