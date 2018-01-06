#pragma once

#include "AbstractPlayListSorter.h"

class PlayListImageSizeSorter : public AbstractPlayListSorter
{
public:
    virtual void sort(ImageList::iterator begin, ImageList::iterator end) override;
};
