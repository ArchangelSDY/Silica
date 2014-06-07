#ifndef NOTEQUALFILTER_H
#define NOTEQUALFILTER_H

#include "AbstractPlayListFilter.h"

class NotEqualRankFilter : public AbstractPlayListFilter
{
public:
    NotEqualRankFilter(int rank, AbstractPlayListFilter *parentFilter = 0);

    bool filter(const ImagePtr &image);

private:
    int m_rank;
};

#endif // NOTEQUALFILTER_H
