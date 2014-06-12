#ifndef ABSTRACTPLAYLISTFILTER_H
#define ABSTRACTPLAYLISTFILTER_H

#include "Image.h"

class AbstractPlayListFilter
{
public:
    AbstractPlayListFilter(AbstractPlayListFilter *parentFilter = 0);
    virtual ~AbstractPlayListFilter();

    ImageList filtered(const ImageList &images);
    virtual bool filter(const ImagePtr &image) = 0;

    static AbstractPlayListFilter *defaultFilter();

protected:
    AbstractPlayListFilter *m_parentFilter;
};

#endif // ABSTRACTPLAYLISTFILTER_H
