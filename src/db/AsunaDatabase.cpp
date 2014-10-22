#include <QTextStream>

#include "AsunaDatabase.h"

AsunaDatabase::AsunaDatabase()
{
    QSettings settings;
    m_baseUrl = settings.value("ASUNA_BASE").toString();
}

AsunaDatabase::AsunaDatabase(const QUrl &baseUrl) :
    m_baseUrl(baseUrl.toString())
{
}

MultiPageReplyIterator *AsunaDatabase::queryImagesByTag(const QString &tag)
{
    QString urlStr = QString("%1/api/images/by-tag/%2/?page=1")
            .arg(m_baseUrl)
            .arg(tag);
    QUrl url(urlStr);

    return getMultiPage(url);
}

QNetworkReply *AsunaDatabase::addImageToAlbum(ImagePtr image,
                                              const QString &album)
{
    if (image.isNull()) {
        return 0;
    }

    QVariant pkField = image->extraInfo().value("pk");
    if (pkField.isNull()) {
        return 0;
    }

    int pk = pkField.toInt();

    QJsonObject payloadObj;
    payloadObj.insert("pk", QJsonValue(pk));
    QJsonDocument payloadDoc(payloadObj);

    QString urlStr = QString("%1/api/album/%2/")
            .arg(m_baseUrl)
            .arg(album);
    QUrl url(urlStr);
    QNetworkRequest req(url);
    req.setRawHeader("Content-Type", "application/json");

    return m_manager.put(req, payloadDoc.toJson(QJsonDocument::Compact));
}
