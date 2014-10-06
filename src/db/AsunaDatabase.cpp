#include <QTextStream>

#include "AsunaDatabase.h"

QUrl AsunaDatabase::getUrlToQueryByTag(const QString &tag)
{
    QString urlString;
    QTextStream urlBuilder(&urlString);
    urlBuilder << m_settings.value("ASUNA_BASE").toString()
               << "/api/images/by-tag/" << tag
               << "/";
    QUrl url(urlString);
    return url;
}
