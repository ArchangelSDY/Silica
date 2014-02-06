#ifndef TESTZIPIMAGESOURCE_H
#define TESTZIPIMAGESOURCE_H

#include <QObject>

class TestImageSource : public QObject
{
    Q_OBJECT
private slots:
    void open();
    void open_data();
//    void close();
//    void name();
//    void hash();
//    void copy();
};

#endif // TESTZIPIMAGESOURCE_H
