#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QList>
#include <QString>

class GlobalConfig
{
public:
    static GlobalConfig *instance();

    QList<QString> zipDirs() const { return m_zipDirs; }
private:
    GlobalConfig();

    void load();

    static GlobalConfig *m_instance;
    QList<QString> m_zipDirs;
};

#endif // GLOBALCONFIG_H
