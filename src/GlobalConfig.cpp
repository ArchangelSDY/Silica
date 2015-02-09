#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QSettings>
#include <QSharedPointer>

#include "Definitions.h"
#include "GlobalConfig.h"

GlobalConfig* GlobalConfig::m_instance = 0;

GlobalConfig::GlobalConfig()
{
    load();
}

GlobalConfig *GlobalConfig::create()
{
    if (!m_instance) {
        m_instance = new GlobalConfig();
    } else {
        qWarning("GlobalConfig: already created!");
    }

    return m_instance;
}

GlobalConfig *GlobalConfig::instance()
{
    Q_ASSERT_X(m_instance, "GlobalConfig", "Instance not created!");
    return m_instance;
}

void GlobalConfig::load()
{
    QCoreApplication::setOrganizationName(g_BUILD_ENV);
    QCoreApplication::setApplicationName("Silica");

    QSettings::setDefaultFormat(QSettings::IniFormat);

    // Init config path
    // TODO(sdy): Create default config
    QSettings settings;
    qDebug() << "Config: " << settings.fileName();

    QFileInfo settingFileInfo(settings.fileName());
    QDir settingDir = settingFileInfo.dir();
    QDir d;
    d.mkpath(settingDir.absolutePath());
    QString baseDirPath = settingDir.absolutePath();


    // Search dirs
    int size = settings.beginReadArray("SearchDirs");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        m_searchDirs << settings.value("DIR").toString();

        qDebug() << "Search Dirs: " << settings.value("DIR").toString();
    }
    settings.endArray();

    // Wallpaper dir
    m_wallpaperDir = settings.value("WALLPAPER_DIR").toString();
    qDebug() << "WallpaperDir: " << m_wallpaperDir;

    // Local database path
    m_localDatabasePath = baseDirPath + "/local.db";
    qDebug() << m_localDatabasePath;

    // Migration config path
    m_migrationConfigPath = ":/assets/migration.json";

    // Thumbnail path
    // TODO(sdy): Make this configurable
    m_thumbnailPath = baseDirPath + "/thumbnails";

    // Network cache path
    m_netCachePath = baseDirPath + "/netcache";

    // FIXME: Load gallery item size
    m_galleryItemSize = QSize(200, 200);

#ifdef ENABLE_OPENGL
    qDebug() << "OpenGL Enabled";
#else
    qDebug() << "OpenGL Disabled";
#endif

    // Register meta types
    qRegisterMetaType<QList<QImage *> >("QList<QImage*>");
    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaType<QSharedPointer<QImage> >("QSharedPointer<QImage>");
    qRegisterMetaType<QList<QSharedPointer<QImage> > >("QList<QSharedPointer<QImage> >");

    // Set plugin path
    QCoreApplication::addLibraryPath(qApp->applicationDirPath() + "/plugins");
#ifdef Q_OS_OSX
    QCoreApplication::addLibraryPath(qApp->applicationDirPath() + "/../Resources/plugins");
#endif
    QCoreApplication::addLibraryPath(baseDirPath + "/plugins");

}

const char *GlobalConfig::buildRevision() const
{
#ifdef BUILD_GIT_SHA1
    return g_BUILD_GIT_SHA1;
#else
    return "";
#endif
}

const char *GlobalConfig::buildTimestamp() const
{
#ifdef BUILD_TIMESTAMP
    return g_BUILD_TIMESTAMP;
#else
    return "";
#endif
}
