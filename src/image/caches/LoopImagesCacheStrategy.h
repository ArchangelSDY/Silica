#ifndef LOOPIMAGESCACHESTRATEGY_H
#define LOOPIMAGESCACHESTRATEGY_H

#include "AbstractImagesCacheStrategy.h"

class Navigator;

class LoopImagesCacheStrategy : public AbstractImagesCacheStrategy
{
public:
    LoopImagesCacheStrategy(ImagesCache *cache, Navigator *navigator);

    int nextIndexToRemove(int insertedIndex);

private:
    Navigator *m_navigator;
};

#endif // LOOPIMAGESCACHESTRATEGY_H
