#ifndef STESTCASE_H
#define STESTCASE_H

#include <QObject>

class STestCase : public QObject
{
    Q_OBJECT
protected slots:
    virtual void initTestCase();
    virtual void cleanupTestCase();
};

#endif // STESTCASE_H
