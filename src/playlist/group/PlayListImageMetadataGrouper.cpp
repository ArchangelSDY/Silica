#include "PlayListImageMetadataGrouper.h"

#include <algorithm>

PlayListImageMetadataGrouper::PlayListImageMetadataGrouper(const QString &key) :
    m_key(key)
{
}

QString PlayListImageMetadataGrouper::groupNameOf(Image *image) const
{
    return image->metadata()[m_key].toString();
}

void PlayListImageMetadataGrouper::group(ImageList::iterator begin, ImageList::iterator end)
{
    const QString &key = m_key;
    std::sort(begin, end, [key](const ImagePtr &left, const ImagePtr &right) -> bool {
        QString leftValue = left->metadata()[key].toString();
        QString rightValue = right->metadata()[key].toString();
        return leftValue < rightValue;
    });

    QSet<QString> values;
    for (auto it = begin; it != end; it++) {
        values << (*it)->metadata()[m_key].toString();
    }
    m_groupNames = values.toList();
}
