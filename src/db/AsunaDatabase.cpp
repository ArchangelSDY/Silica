#include <QTextStream>

#include "AsunaDatabase.h"

MultiPageReplyIterator *AsunaDatabase::queryImagesByTag(const QString &tag)
{
    QString urlStr = QString("%1/api/images/by-tag/%2/?page=1")
            .arg(m_settings.value("ASUNA_BASE").toString())
            .arg(tag);
    QUrl url(urlStr);

    return getMultiPage(url);
}
