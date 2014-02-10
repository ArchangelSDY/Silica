#ifndef IMAGESCACHE_H
#define IMAGESCACHE_H

#include <QObject>

#include "Image.h"

class ImagesCache : public QObject
{
    Q_OBJECT
public:
    explicit ImagesCache(int capacity, QObject *parent = 0);

    void insert(int index, Image *image);
    Image *at(int index);
    void clear();

private:
    void trim(int index);

    int m_capacity;
    QMap<int, Image *> m_images;
};

#endif // IMAGESCACHE_H