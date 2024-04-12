#pragma once

#include <QObject>

#include "share/Sharer.h"

class SaveToSharer : public Sharer
{
    Q_OBJECT
public:
    ~SaveToSharer() {}
    virtual bool share(QSharedPointer<Image> image) override;
};
