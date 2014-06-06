#include "ImageRank.h"
#include "MinRankFilter.h"

MinRankFilter::MinRankFilter(int minRank,
                             AbstractPlayListFilter *parentFilter) :
    AbstractPlayListFilter(parentFilter) ,
    m_minRank(minRank)
{
}

bool MinRankFilter::filter(const ImagePtr &image)
{
    int rank = image->rank()->value();
    return rank >= m_minRank;
}
