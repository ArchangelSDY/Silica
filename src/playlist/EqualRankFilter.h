#ifndef EQUALRANKFILTER_H
#define EQUALRANKFILTER_H

#include "AbstractPlayListFilter.h"

class EqualRankFilter : public AbstractPlayListFilter
{
public:
    EqualRankFilter(int rank, AbstractPlayListFilter *parentFilter = 0);

    bool filter(const ImagePtr &image);

private:
    int m_rank;
};

#endif // EQUALRANKFILTER_H
