#ifndef DONOTHINGFILTER_H
#define DONOTHINGFILTER_H

#include "AbstractPlayListFilter.h"

class DoNothingFilter : public AbstractPlayListFilter
{
public:
    DoNothingFilter(AbstractPlayListFilter *parentFilter = 0);

    bool filter(const ImagePtr &image);
};

#endif // DONOTHINGFILTER_H
