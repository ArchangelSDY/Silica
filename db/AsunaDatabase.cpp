#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>

#include "AsunaDatabase.h"

static const char *ASUNA_BASE = "http://asuna.archangelsdy.com";

// TODO: Read this from config
static const char *LOCAL_ZIPS_DIR = "G:/ACG/Pic/Moe-Zip/";

AsunaDatabase::AsunaDatabase()
{
}

void AsunaDatabase::queryByTag(const QString &tag)
{
    QString urlString;
    QTextStream urlBuilder(&urlString);
    urlBuilder << ASUNA_BASE << "/api/images/by-tag/" << tag << "/";

    QUrl url(urlString);
    m_net.get(QNetworkRequest(url));
}

void AsunaDatabase::reqFinish(QNetworkReply * reply)
{
    reply->deleteLater();

    if (reply->error() == QNetworkReply::NoError) {
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
    }
}
