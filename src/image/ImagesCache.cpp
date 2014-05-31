#include <QMap>

#include "ImagesCache.h"
#include "Navigator.h"

ImagesCache::ImagesCache(int capacity, Navigator *navigator, QObject *parent) :
    QObject(parent) ,
    m_capacity(capacity) ,
    m_navigator(navigator)
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

    int toRemoveIndex;

    if (!m_navigator->isLooping()) {
        int min = m_images.firstKey();
        int max = m_images.lastKey();
        int minDistance = qAbs(index - min);
        int maxDistance = qAbs(index - max);
        toRemoveIndex = (minDistance > maxDistance) ? min : max;
    } else {
        int total = m_navigator->playList()->count();
        int longestKey = 0, longestDistance = 0;
        foreach (const int &key, m_images.keys()) {
            int delta = qMax(key, index) - qMin(key, index);

            // Handle looping
            if (delta > total / 2) {
                delta = total - delta;
            }

            if (delta > longestDistance) {
                longestKey = key;
                longestDistance = delta;
            }
        }
        toRemoveIndex = longestKey;
    }

    Image *toRemove = m_images.take(toRemoveIndex);

    if (toRemove) {
        toRemove->scheduleUnload();
    }
}
