#pragma once

#include <QString>

class PlayListEntityData
{
public:
    PlayListEntityData() {}
    PlayListEntityData(int t, int i, const QString &n, int c, const QString &cp) :
        type(t), id(i), name(n), count(c), coverPath(cp) {}

    int type = 0;
    int id = 0;
    QString name;
    int count = 0;
    QString coverPath;

    bool isValid() const { return id != 0; }
};
