#include "ImageRank.h"
#include "NotEqualRankFilter.h"

NotEqualRankFilter::NotEqualRankFilter(int rank,
                                       AbstractPlayListFilter *parentFilter) :
    AbstractPlayListFilter(parentFilter) ,
    m_rank(rank)
{
}

bool NotEqualRankFilter::filter(const ImagePtr &image)
{
    int rank = image->rank()->value();
    return rank != m_rank;
}
