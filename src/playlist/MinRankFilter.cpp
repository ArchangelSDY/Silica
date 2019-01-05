#include "MinRankFilter.h"

#include "image/ImageRank.h"

MinRankFilter::MinRankFilter(int minRank,
                             AbstractPlayListFilter *parentFilter) :
    AbstractPlayListFilter(parentFilter) ,
    m_minRank(minRank)
{
}

bool MinRankFilter::filter(const ImagePtr &image)
{
    ImageRank rank(image.data());
    return rank.value() >= m_minRank;
}
