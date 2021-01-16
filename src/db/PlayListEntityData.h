#pragma once

#include <QString>

class PlayListEntityData
{
public:
    int type = 0;
    int id = 0;
    QString name;
    int count = 0;
    QString coverPath;

    bool isValid() const { return id != 0; }
};
