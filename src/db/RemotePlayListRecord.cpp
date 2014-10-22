#include "PlayList.h"
#include "RemoteDatabase.h"
#include "RemotePlayListRecord.h"

RemotePlayListRecord::RemotePlayListRecord(const QString &name,
                                           const QString &coverPath,
                                           QObject *parent) :
    PlayListRecord(name, coverPath, 0, parent) ,
    m_replyIter(0)
{
    m_type = PlayListRecord::RemotePlayList;
}

RemotePlayListRecord::~RemotePlayListRecord()
{
    if (m_replyIter) {
        delete m_replyIter;
    }
}

PlayList *RemotePlayListRecord::playList()
{
    if (!m_playList) {
        m_playList = new PlayList();
        m_playList->setRecord(this);

        // Start a query
        if (!m_replyIter) {
            m_replyIter = RemoteDatabase::instance()->queryImagesByTag(m_name);
            connect(m_replyIter, SIGNAL(gotPage(QNetworkReply*)),
                    this, SLOT(gotReplyPage(QNetworkReply*)));
        }
    }

    return m_playList;
}

void RemotePlayListRecord::gotReplyPage(QNetworkReply *reply)
{
    Q_ASSERT(m_playList);

    PlayList toBeAppended;
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
        image->extraInfo().insert("pk", imageInfo["pk"].toInt());
        toBeAppended.append(image);
    }

    m_playList->append(&toBeAppended);
    reply->deleteLater();
}
