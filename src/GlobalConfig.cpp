#include <QApplication>
#include <QDebug>
#include <QFile>
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
    QCoreApplication::setOrganizationName("Asuna");
    QCoreApplication::setApplicationName("Silica");

    QSettings::setDefaultFormat(QSettings::IniFormat);
#ifdef Q_OS_OSX
    QString baseDir = qApp->applicationDirPath() + "/../Resources";
    if (!QFile::exists(baseDir)) {
        baseDir = qApp->applicationDirPath();
    }
#else
    QString baseDir = qApp->applicationDirPath();
#endif

    // Init config path
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       baseDir + "/config");
    QSettings settings;
    qDebug() << "Config: " << settings.fileName();
    if (!QFile::exists(settings.fileName())) {
        qDebug() << "Config does not exist. Init with a default one.";
        QFile::copy(settings.fileName() + ".default", settings.fileName());
    }

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
    m_localDatabasePath = baseDir + "/local.db";

    // Migration config path
    m_migrationConfigPath = ":/assets/migration.json";

    // Thumbnail path
    m_thumbnailPath = baseDir + "/thumbnails";

    // Network cache path
    m_netCachePath = baseDir + "/netcache";

    // Plugins path
    m_pluginsPath = baseDir + "/plugins";

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
    QCoreApplication::addLibraryPath(baseDir + "/plugins");
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
