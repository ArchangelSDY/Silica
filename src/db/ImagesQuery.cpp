#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QNetworkReply>

#include "ImagesQuery.h"
#include "PlayList.h"
#include "RemoteDatabase.h"

ImagesQuery::ImagesQuery(QString tag, QObject *parent) :
    AbstractQuery(parent)
{
    m_request.setUrl(RemoteDatabase::instance()->getUrlToQueryByTag(tag));
}

void ImagesQuery::parseReply(QNetworkReply *reply)
{
    QString content = QString::fromUtf8(reply->readAll());
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    QJsonArray images = doc.array();
    for (QJsonArray::const_iterator i = images.constBegin();
         i != images.constEnd(); ++i) {
        QJsonObject imageInfo = (*i).toObject();
        QJsonObject imageFields = imageInfo["fields"].toObject();
        QString fileName = imageFields["name"].toString();
        QString zipPackage = imageFields["zip_package"].toString();

        QString imageUrl;
        QTextStream imageUrlBuilder(&imageUrl);
        imageUrlBuilder << "zip:///"
                        << zipPackage << "#" << fileName;

        ImagePtr image(new Image(imageUrl));
        m_queuedImages.enqueue(image);
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
