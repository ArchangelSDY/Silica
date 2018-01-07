#pragma once

#include "AbstractPlayListGrouper.h"

class PlayListImageThumbnailHistogramGrouper : public AbstractPlayListGrouper
{
public:
    virtual void group(ImageList::iterator begin, ImageList::iterator end) override;
};

