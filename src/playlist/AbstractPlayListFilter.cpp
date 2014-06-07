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
    while (it != filtered.end()) {
        if (!filter(*it)) {
            it = filtered.erase(it);
        } else {
            ++it;
        }
    }

    return filtered;
}
