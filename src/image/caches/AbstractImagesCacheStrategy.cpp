#include "AbstractImagesCacheStrategy.h"
#include "ImagesCache.h"

AbstractImagesCacheStrategy::AbstractImagesCacheStrategy(ImagesCache *cache) :
    m_cache(cache)
{
}

const QMap<int, QSharedPointer<ImageData>> &AbstractImagesCacheStrategy::cachedImages() const
{
    return m_cache->m_images;
}
