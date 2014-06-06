#ifndef RANKABOVEFILTER_H
#define RANKABOVEFILTER_H

#include "AbstractPlayListFilter.h"

class MinRankFilter : public AbstractPlayListFilter
{
public:
    MinRankFilter(int minRank, AbstractPlayListFilter *parentFilter = 0);

    bool filter(const ImagePtr &image);

private:
    int m_minRank;
};

#endif // RANKABOVEFILTER_H
