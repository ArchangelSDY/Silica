#ifndef TESTZIPIMAGESOURCE_H
#define TESTZIPIMAGESOURCE_H

#include <QObject>

#include "../src/ZipImageSource.h"

class TestZipImageSource : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void open();

private:
    ZipImageSource *m_zipImageSource;
};

#endif // TESTZIPIMAGESOURCE_H
