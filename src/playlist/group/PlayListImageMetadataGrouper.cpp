#include "PlayListImageMetadataGrouper.h"

#include <algorithm>

PlayListImageMetadataGrouper::PlayListImageMetadataGrouper(const QString &key) :
    m_key(key)
{
}

void PlayListImageMetadataGrouper::group(ImageList::iterator begin, ImageList::iterator end)
{
    for (auto it = begin; it != end; it++) {
        (*it)->loadMetadata();
    }

    const QString &key = m_key;
    std::sort(begin, end, [key](const ImagePtr &left, const ImagePtr &right) -> bool {
        QString leftValue = left->metadata()[key].toString();
        QString rightValue = right->metadata()[key].toString();
        return leftValue < rightValue;
    });

    m_groupNames << "";
    for (auto it = begin; it != end; it++) {
        QString groupName = (*it)->metadata()[m_key].toString();
        if (groupName != m_groupNames.last()) {
            m_groupNames << groupName;
        }
        m_imageGroups[it->data()] = m_groupNames.count() - 1;
    }
}
