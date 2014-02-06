#ifndef TESTZIPIMAGESOURCE_H
#define TESTZIPIMAGESOURCE_H

#include <QObject>

class TestImageSource : public QObject
{
    Q_OBJECT
private slots:
    void openAndClose();
    void openAndClose_data();
    void properties();
    void properties_data();
    void copy();
    void copy_data();
};

#endif // TESTZIPIMAGESOURCE_H
