#ifndef DATABASE_H
#define DATABASE_H

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QObject>

#include "../PlayList.h"

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);

    virtual void queryByTag(const QString &tag, int page = 1) = 0;

protected:
    QNetworkAccessManager m_net;
    QNetworkDiskCache m_cache;

signals:
    void gotPlayList(PlayList playList);

public slots:
    virtual void reqFinish(QNetworkReply *) = 0;
};

#endif // DATABASE_H
