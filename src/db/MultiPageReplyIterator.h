#ifndef MULTIPAGEREPLYITERATOR_H
#define MULTIPAGEREPLYITERATOR_H

#include <QNetworkReply>
#include <QObject>

class MultiPageReplyIterator : public QObject
{
    Q_OBJECT
public:
    explicit MultiPageReplyIterator(QNetworkReply *reply, QObject *parent = 0);

signals:
    void gotPage(QNetworkReply *reply);

private slots:
    void replyFinished();

private:
    void setReply(QNetworkReply *reply);

    QNetworkReply *m_reply;
};

#endif // MULTIPAGEREPLYITERATOR_H
