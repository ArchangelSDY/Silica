#pragma once

#include <QObject>

#include "share/Sharer.h"

class SaveToSharer : public Sharer
{
    Q_OBJECT
public:
    ~SaveToSharer() {}
    virtual bool share(const QList<QSharedPointer<Image>> &images) override;
};
