#pragma once

#include <QStringList>

#include "image/Image.h"

class AbstractPlayListGrouper
{
public:
    ~AbstractPlayListGrouper() {}

    QStringList groupNames() const;
    virtual QString groupNameOf(Image *image) const;

    virtual void group(ImageList::iterator begin, ImageList::iterator end) = 0;

protected:
    QStringList m_groupNames;
    QHash<Image *, int> m_imageGroups;
};
