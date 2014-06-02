#ifndef TESTIMAGERANK_H
#define TESTIMAGERANK_H

#include <QObject>

class TestImageRank : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();
    void saveAndLoad();
    void saveAndLoad_data();
};

#endif // TESTIMAGERANK_H
