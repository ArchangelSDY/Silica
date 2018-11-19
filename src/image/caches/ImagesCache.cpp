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

void ImagesCache::insert(int index, ImagePtr image)
{
    trim(index);
    m_images.insert(index, image);
}

ImagePtr ImagesCache::at(int index)
{
    if (m_images.contains(index)) {
        return m_images[index];
    } else {
        return 0;
    }
}

void ImagesCache::clear()
{
    for (QMap<int, ImagePtr>::iterator i = m_images.begin();
         i != m_images.end(); ++i) {
        ImagePtr image = i.value();
        if (image) {
            image->scheduleUnload();
        }
    }

    m_images.clear();
}

void ImagesCache::trim(int index)
{
    if (m_images.count() < m_capacity) {
        return;
    }

    int toRemoveIndex = m_strategy->nextIndexToRemove(index);
    ImagePtr toRemove = m_images.take(toRemoveIndex);

    if (toRemove) {
        toRemove->scheduleUnload();
    }
}

void ImagesCache::setStrategy(AbstractImagesCacheStrategy *strategy)
{
    delete m_strategy;
    m_strategy = strategy;
}
