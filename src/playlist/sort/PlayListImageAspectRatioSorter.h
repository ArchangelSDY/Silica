#pragma once

#include "AbstractPlayListSorter.h"

class PlayListImageAspectRatioSorter : public AbstractPlayListSorter
{
public:
    virtual void sort(ImageList::iterator begin, ImageList::iterator end) override;
};

