#ifndef NORMALIMAGESCACHESTRATEGY_H
#define NORMALIMAGESCACHESTRATEGY_H

#include "AbstractImagesCacheStrategy.h"

class NormalImagesCacheStrategy : public AbstractImagesCacheStrategy
{
public:
    NormalImagesCacheStrategy(ImagesCache *cache);

    int nextIndexToRemove(int insertedIndex);
};

#endif // NORMALIMAGESCACHESTRATEGY_H
