#ifndef SHARER_H
#define SHARER_H

#include <QObject>

class Image;

class Sharer : public QObject
{
    Q_OBJECT
public:
    virtual ~Sharer() {}
    virtual bool share(const QList<QSharedPointer<Image>> &images) = 0;

signals:
    void finished(bool success);
};

#endif // SHARER_H
