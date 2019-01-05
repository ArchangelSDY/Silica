#include "EqualRankFilter.h"

#include "image/ImageRank.h"

EqualRankFilter::EqualRankFilter(int rank,
                                 AbstractPlayListFilter *parentFilter) :
    AbstractPlayListFilter(parentFilter) ,
    m_rank(rank)
{
}

bool EqualRankFilter::filter(const ImagePtr &image)
{
    ImageRank rank(image.data());
    return rank.value() == m_rank;
}
