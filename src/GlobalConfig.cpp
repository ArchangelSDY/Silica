#include <QApplication>
#include <QDebug>
#include <QFile>
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
    QSettings::setDefaultFormat(QSettings::IniFormat);
#ifdef Q_OS_OSX
    QString baseConfigDir = qApp->applicationDirPath() + "/../Resources";
    if (!QFile::exists(baseConfigDir)) {
        baseConfigDir = qApp->applicationDirPath();
    }
#else
    QString baseConfigDir = qApp->applicationDirPath();
#endif

    // Init config path
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       baseConfigDir + "/config");
    QSettings settings;
    qDebug() << "Config: " << settings.fileName();
    if (!QFile::exists(settings.fileName())) {
        qDebug() << "Config does not exist. Init with a default one.";
        QFile::copy(settings.fileName() + ".default", settings.fileName());
    }

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

    // Local database path
    m_localDatabasePath = baseConfigDir + "/local.db";

    // Migration config path
    m_migrationConfigPath = ":/assets/migration.json";

    // Thumbnail path
    m_thumbnailPath = baseConfigDir + "/thumbnails";

    // FIXME: Load gallery item size
    m_galleryItemSize = QSize(200, 200);

#ifdef ENABLE_OPENGL
    qDebug() << "OpenGL Enabled";
#else
    qDebug() << "OpenGL Disabled";
#endif
}
