#ifndef IMAGESCACHE_H
#define IMAGESCACHE_H

#include <QObject>

#include "AbstractImagesCacheStrategy.h"
#include "Image.h"

class Navigator;

class ImagesCache : public QObject
{
    Q_OBJECT

    friend class AbstractImagesCacheStrategy;

public:
    explicit ImagesCache(int capacity, QObject *parent = 0);
    ~ImagesCache();

    void insert(int index, ImagePtr image);
    ImagePtr at(int index);
    void clear();

    void setStrategy(AbstractImagesCacheStrategy *strategy);

private:
    void trim(int index);

    int m_capacity;
    QMap<int, ImagePtr> m_images;
    AbstractImagesCacheStrategy *m_strategy;
};

#endif // IMAGESCACHE_H
