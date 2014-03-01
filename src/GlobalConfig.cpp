#include <QApplication>
#include <QDebug>
#include <QSettings>

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
    qDebug() << "Config: " << settings.fileName();

    // Zip dirs
    int size = settings.beginReadArray("ZipDirs");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        m_zipDirs << settings.value("DIR").toString();

        qDebug() << "ZipDir: " << settings.value("DIR").toString();
    }
    settings.endArray();

    // Wallpaper dir
    m_wallpaperDir = settings.value("WALLPAPER_DIR").toString();
    qDebug() << "WallpaperDir: " << m_wallpaperDir;

    // Migration config path
    m_migrationConfigPath = qApp->applicationDirPath() + "/migration.json";

    // FIXME: Load gallery item size
    m_galleryItemSize = QSize(200, 200);
}
