#ifndef DATABASE_H
#define DATABASE_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

#include "../PlayList.h"

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);

    virtual void queryByTag(const QString &tag) = 0;

protected:
    QNetworkAccessManager m_net;

signals:
    void gotPlayList(PlayList playList);

public slots:
    virtual void reqFinish(QNetworkReply *) = 0;
};

#endif // DATABASE_H
