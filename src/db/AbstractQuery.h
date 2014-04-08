#ifndef ABSTRACTQUERY_H
#define ABSTRACTQUERY_H

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QObject>
#include <QSettings>

class AbstractQuery : public QObject
{
    Q_OBJECT
public:
    explicit AbstractQuery(QObject *parent = 0);
    ~AbstractQuery();

    void start();

signals:
    void gotContent();

protected slots:
    virtual void reqFinish(QNetworkReply *);

protected:
    virtual void parseReply(QNetworkReply *) = 0;
    void queryNextPage();

    QSettings m_settings;
    QNetworkAccessManager m_manager;
    QNetworkDiskCache m_cache;
    QNetworkRequest m_request;
    int m_page;
};

#endif // ABSTRACTQUERY_H
