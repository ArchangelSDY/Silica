#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "AsunaDatabase.h"

AsunaDatabase::AsunaDatabase()
{
}

void AsunaDatabase::queryByTag(const QString &tag, int page)
{
    QString urlString;
    QTextStream urlBuilder(&urlString);
    urlBuilder << m_settings.value("ASUNA_BASE").toString()
               << "/api/images/by-tag/" << tag
               << "/?page=" << page;

    QUrl url(urlString);
    QNetworkRequest request(url);
    m_net.get(request);
}

void AsunaDatabase::reqFinish(QNetworkReply * reply)
{
    reply->deleteLater();

    if (reply->error() == QNetworkReply::NoError) {
        parseJsonResponse(reply);
    }
}

void AsunaDatabase::parseJsonResponse(QNetworkReply *reply)
{
    QString content = QString::fromUtf8(reply->readAll());
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    QJsonArray images = doc.array();
    PlayList playList;
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

        playList << QSharedPointer<Image>(new Image(imageUrl));
    }

    emit gotPlayList(playList);

    // Check for next page
    if (images.count() == m_settings.value("PAGE_LIMIT").toInt()) {
        queryNextPage(reply->url());
    }
}

void AsunaDatabase::queryNextPage(QUrl url)
{
    QRegularExpression re("page=(?<page>\\d+)");
    QRegularExpressionMatch match = re.match(url.query());
    if (match.hasMatch()) {
        QString pageStr = match.captured("page");
        bool validPage;
        int page = pageStr.toInt(&validPage);
        if (validPage) {
            int nextPage = page + 1;
            QString nextPageQuery;
            QTextStream(&nextPageQuery) << "page=" << nextPage;
            QString nextQuery = url.query().replace(re, nextPageQuery);
            url.setQuery(nextQuery);

            QNetworkRequest request(url);
            request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                                 QNetworkRequest::PreferCache);
            m_net.get(request);
        }
    }

}
