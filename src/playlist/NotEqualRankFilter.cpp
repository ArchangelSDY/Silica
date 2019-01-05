#include "NotEqualRankFilter.h"

#include "image/ImageRank.h"

NotEqualRankFilter::NotEqualRankFilter(int rank,
                                       AbstractPlayListFilter *parentFilter) :
    AbstractPlayListFilter(parentFilter) ,
    m_rank(rank)
{
}

bool NotEqualRankFilter::filter(const ImagePtr &image)
{
    ImageRank rank(image.data());
    return rank.value() != m_rank;
}
