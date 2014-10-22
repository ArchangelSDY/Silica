#ifndef TESTMULTIPAGEREPLYITERATOR_H
#define TESTMULTIPAGEREPLYITERATOR_H

#include <QNetworkAccessManager>
#include <QObject>

#include "deps/QtMockWebServer/src/QtMockWebServer.h"

class TestMultiPageReplyIterator : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void gotPage();
    void gotPage_data();

private:
    QtMockWebServer *m_mockServer;
    QNetworkAccessManager m_mgr;
};

#endif // TESTMULTIPAGEREPLYITERATOR_H
