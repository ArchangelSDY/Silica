#include "LoopImagesCacheStrategy.h"
#include "Navigator.h"

LoopImagesCacheStrategy::LoopImagesCacheStrategy(
        ImagesCache *cache, Navigator *navigator) :
    AbstractImagesCacheStrategy(cache) ,
    m_navigator(navigator)
{
}

int LoopImagesCacheStrategy::nextIndexToRemove(int insertedIndex)
{
    int total = m_navigator->playList()->count();
    int longestKey = 0, longestDistance = 0;
    const auto &images = AbstractImagesCacheStrategy::cachedImages();
    foreach (const int &key, images.keys()) {
        int delta = qMax(key, insertedIndex) - qMin(key, insertedIndex);

        // Handle looping
        if (delta > total / 2) {
            delta = total - delta;
        }

        if (delta > longestDistance) {
            longestKey = key;
            longestDistance = delta;
        }
    }

    return longestKey;
}
