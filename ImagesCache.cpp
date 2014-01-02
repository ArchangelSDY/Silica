#include <QMap>

#include "ImagesCache.h"

ImagesCache::ImagesCache(int capacity, QObject *parent) :
    QObject(parent) ,
    m_capacity(capacity)
{
}

void ImagesCache::insert(int index, Image *image)
{
    trim(index);
    m_images.insert(index, image);
}

Image *ImagesCache::at(int index)
{
    if (m_images.contains(index)) {
        return m_images[index];
    } else {
        return 0;
    }
}

void ImagesCache::clear()
{
    for (QMap<int, Image *>::iterator i = m_images.begin();
         i != m_images.end(); ++i) {
        Image *image = i.value();
        if (image) {
            delete image;
        }
    }

    m_images.clear();
}

void ImagesCache::trim(int index)
{
    if (m_images.count() < m_capacity) {
        return;
    }

    int min = m_images.lastKey();
    int minDelta = qAbs(index - min);
    int max = m_images.firstKey();
    int maxDelta = qAbs(index - max);

    Image *toRemove = 0;
    if (minDelta > maxDelta) {
        toRemove = m_images.take(min);
    } else {
        toRemove = m_images.take(max);
    }

    if (toRemove) {
        delete toRemove;
    }
}
