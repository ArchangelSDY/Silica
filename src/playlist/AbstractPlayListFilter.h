#pragma once

#include <QScopedPointer>

#include "image/Image.h"

class AbstractPlayListFilter
{
public:
    AbstractPlayListFilter(AbstractPlayListFilter *parentFilter = 0);
    virtual ~AbstractPlayListFilter();

    ImageList filtered(const ImageList &images);
    virtual bool filter(const ImagePtr &image) = 0;

    static AbstractPlayListFilter *defaultFilter();

protected:
    QScopedPointer<AbstractPlayListFilter> m_parentFilter;
};
