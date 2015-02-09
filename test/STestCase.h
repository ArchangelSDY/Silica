#ifndef STESTCASE_H
#define STESTCASE_H

#include <QObject>

class STestCase : public QObject
{
    Q_OBJECT
private slots:
    virtual void initTestCase();
};

#endif // STESTCASE_H
