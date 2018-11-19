#ifndef ABSTRACTIMAGESCACHESTRATEGY_H
#define ABSTRACTIMAGESCACHESTRATEGY_H

#include <QMap>

#include "image/Image.h"

class Image;
class ImagesCache;

class AbstractImagesCacheStrategy
{
public:
    AbstractImagesCacheStrategy(ImagesCache *cache);
    virtual ~AbstractImagesCacheStrategy() {}

    virtual int nextIndexToRemove(int insertedIndex) = 0;

protected:
    const QMap<int, ImagePtr> &cachedImages() const;

    ImagesCache *m_cache;
};

#endif // ABSTRACTIMAGESCACHESTRATEGY_H
