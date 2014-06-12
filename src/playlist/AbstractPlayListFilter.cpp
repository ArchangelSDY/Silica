#include "AbstractPlayListFilter.h"
#include "ImageRank.h"
#include "NotEqualRankFilter.h"

AbstractPlayListFilter::AbstractPlayListFilter(
        AbstractPlayListFilter *parentFilter) :
    m_parentFilter(parentFilter)
{
}

AbstractPlayListFilter::~AbstractPlayListFilter()
{
    if (m_parentFilter) {
        delete m_parentFilter;
    }
}

AbstractPlayListFilter *AbstractPlayListFilter::defaultFilter()
{
    AbstractPlayListFilter *filter = 0;
    for (int i = ImageRank::MIN_VALUE; i < ImageRank::MIN_SHOWN_VALUE; ++i) {
        filter = new NotEqualRankFilter(i, filter);
    }
    return filter;
}

ImageList AbstractPlayListFilter::filtered(const ImageList &images)
{
    ImageList filtered =
        m_parentFilter ? m_parentFilter->filtered(images) : images;

    ImageList::iterator it = filtered.begin();
    ImageList::iterator end = filtered.end();
    while (it != end) {
        if (!filter(*it)) {
            --end;
            *it = qMove(*end);
            filtered.erase(end);
        } else {
            ++it;
        }
    }

    return filtered;
}
