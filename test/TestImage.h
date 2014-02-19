#ifndef TESTIMAGE_H
#define TESTIMAGE_H

#include <QObject>

class TestImage : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void loadThumbnail();
    void loadThumbnail_data();
};

#endif // TESTIMAGE_H
