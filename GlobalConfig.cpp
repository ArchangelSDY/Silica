#include <QSettings>

#include <QDebug>

#include "GlobalConfig.h"

GlobalConfig* GlobalConfig::m_instance = 0;

GlobalConfig::GlobalConfig()
{
    load();
}

GlobalConfig *GlobalConfig::instance()
{
    if (!m_instance) {
        m_instance = new GlobalConfig();
    }

    return m_instance;
}

void GlobalConfig::load()
{
    QSettings settings;
    qDebug() << "Config: " << settings.fileName() << "\n";

    int size = settings.beginReadArray("ZipDirs");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        m_zipDirs << settings.value("DIR").toString();

        qDebug() << "ZipDir: " << settings.value("DIR").toString() << "\n";
    }
    settings.endArray();
}
