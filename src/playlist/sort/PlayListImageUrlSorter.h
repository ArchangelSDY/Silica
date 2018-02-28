#pragma once

#include "AbstractPlayListSorter.h"

class PlayListImageUrlSorter : public AbstractPlayListSorter
{
public:
    virtual void sort(ImageList::iterator begin, ImageList::iterator end) override;
};
