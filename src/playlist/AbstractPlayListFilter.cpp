#include "image/ImageRank.h"

#include "AbstractPlayListFilter.h"
#include "DoNothingFilter.h"

AbstractPlayListFilter::AbstractPlayListFilter(
        AbstractPlayListFilter *parentFilter) :
    m_parentFilter(parentFilter)
{
}

AbstractPlayListFilter::~AbstractPlayListFilter()
{
}

AbstractPlayListFilter *AbstractPlayListFilter::defaultFilter()
{
    return new DoNothingFilter();
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
