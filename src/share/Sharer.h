#ifndef SHARER_H
#define SHARER_H

#include <QObject>

class Image;

class Sharer : public QObject
{
    Q_OBJECT
public:
    virtual bool share(QSharedPointer<Image> image) = 0;

signals:
    void finished(bool success);
};

#endif // SHARER_H
