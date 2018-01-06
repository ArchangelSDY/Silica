#pragma once

#include "image/Image.h"

class AbstractPlayListSorter
{
public:
    virtual ~AbstractPlayListSorter() {}

    virtual void sort(ImageList::iterator begin, ImageList::iterator end) = 0;
};
