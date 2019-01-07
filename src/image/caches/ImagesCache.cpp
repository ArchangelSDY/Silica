#include <QMap>

#include "ImagesCache.h"
#include "Navigator.h"
#include "NormalImagesCacheStrategy.h"

ImagesCache::ImagesCache(int capacity, QObject *parent) :
    QObject(parent) ,
    m_capacity(capacity) ,
    m_strategy(new NormalImagesCacheStrategy(this))
{
}

ImagesCache::~ImagesCache()
{
    clear();
    delete m_strategy;
}

void ImagesCache::insert(int index, QSharedPointer<ImageData> image)
{
    if (m_capacity > 0) {
        trim(index);
        m_images.insert(index, image);
    }
}

bool ImagesCache::contains(int index) const
{
    return m_images.contains(index);
}

void ImagesCache::clear()
{
    m_images.clear();
}

void ImagesCache::trim(int index)
{
    if (m_images.count() < m_capacity) {
        return;
    }

    int toRemoveIndex = m_strategy->nextIndexToRemove(index);
    m_images.remove(toRemoveIndex);
}

void ImagesCache::setStrategy(AbstractImagesCacheStrategy *strategy)
{
    delete m_strategy;
    m_strategy = strategy;
}
