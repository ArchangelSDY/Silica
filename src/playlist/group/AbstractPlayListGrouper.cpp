#include "AbstractPlayListGrouper.h"

QStringList AbstractPlayListGrouper::groupNames() const
{
    return m_groupNames;
}

QString AbstractPlayListGrouper::groupNameOf(Image *image) const
{
    auto it = m_imageGroups.find(image);
    if (it != m_imageGroups.end()) {
        int idx = *it;
        if (idx >= 0 && idx < m_groupNames.count()) {
            return m_groupNames[idx];
        }
    }
    return QStringLiteral("");
}
