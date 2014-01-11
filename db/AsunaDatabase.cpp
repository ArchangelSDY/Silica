#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "AsunaDatabase.h"

static const char *ASUNA_BASE = "http://asuna.archangelsdy.com";
static const int PAGE_LIMIT = 50;

// TODO: Read this from config
static const char *LOCAL_ZIPS_DIR = "G:/ACG/Pic/Moe-Zip/";

AsunaDatabase::AsunaDatabase()
{
}

void AsunaDatabase::queryByTag(const QString &tag, int page)
{
    QString urlString;
    QTextStream urlBuilder(&urlString);
    urlBuilder << ASUNA_BASE << "/api/images/by-tag/" << tag
               << "/?page=" << page;

    QUrl url(urlString);
    m_net.get(QNetworkRequest(url));
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
        imageUrlBuilder << "zip:///" << LOCAL_ZIPS_DIR << zipPackage
            << "#" << fileName;

        playList << QUrl(imageUrl);
    }

    emit gotPlayList(playList);

    // Check for next page
    if (images.count() == PAGE_LIMIT) {
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

            m_net.get(QNetworkRequest(url));
        }
    }

}
