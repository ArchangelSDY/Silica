#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QNetworkReply>

#include "ImagesQuery.h"
#include "PlayList.h"

ImagesQuery::ImagesQuery(QString tag, QObject *parent) :
    AbstractQuery(parent)
{
    QString urlString;
    QTextStream urlBuilder(&urlString);
    urlBuilder << m_settings.value("ASUNA_BASE").toString()
               << "/api/images/by-tag/" << tag
               << "/";
    QUrl url(urlString);
    m_request.setUrl(url);
}

void ImagesQuery::parseReply(QNetworkReply *reply)
{
    QString content = QString::fromUtf8(reply->readAll());
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    QJsonArray images = doc.array();
    for (QJsonArray::const_iterator i = images.constBegin();
         i != images.constEnd(); ++i) {
        QJsonObject image = (*i).toObject();
        QJsonObject imageFields = image["fields"].toObject();
        QString fileName = imageFields["name"].toString();
        QString zipPackage = imageFields["zip_package"].toString();

        QString imageUrl;
        QTextStream imageUrlBuilder(&imageUrl);
        imageUrlBuilder << "zip:///"
                        << zipPackage << "#" << fileName;

        m_queuedImages.enqueue(QSharedPointer<Image>(new Image(imageUrl)));
    }

    emit gotContent();

    if (images.count() == m_settings.value("PAGE_LIMIT").toInt()) {
        queryNextPage();
    }
}

PlayList *ImagesQuery::playList()
{
    PlayList *pl = new PlayList();
    while (!m_queuedImages.isEmpty()) {
        pl->append(m_queuedImages.dequeue());
    }
    return pl;
}
