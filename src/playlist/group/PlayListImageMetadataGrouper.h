#pragma once

#include "AbstractPlayListGrouper.h"

class PlayListImageMetadataGrouper : public AbstractPlayListGrouper
{
public:
    PlayListImageMetadataGrouper(const QString &key);

    virtual void group(ImageList::iterator begin, ImageList::iterator end) override;

private:
    QString m_key;
};
