#include "NormalImagesCacheStrategy.h"

NormalImagesCacheStrategy::NormalImagesCacheStrategy(ImagesCache *cache) :
    AbstractImagesCacheStrategy(cache)
{
}

int NormalImagesCacheStrategy::nextIndexToRemove(int insertedIndex)
{
    const QMap<int, QSharedPointer<ImageData>> &images =
        AbstractImagesCacheStrategy::cachedImages();

    int min = images.firstKey();
    int max = images.lastKey();
    int minDistance = qAbs(insertedIndex - min);
    int maxDistance = qAbs(insertedIndex - max);
    int toRemoveIndex = (minDistance > maxDistance) ? min : max;

    return toRemoveIndex;
}
