#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QSettings>
#include <QSharedPointer>
#include <QTextCodec>
#include <QUrl>
#include <QVariantHash>

#include "Definitions.h"
#include "GlobalConfig.h"
#include "PlayList.h"

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
    qDebug() << "Config:" << settings.fileName();

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
    qDebug() << "WallpaperDir:" << m_wallpaperDir;

    // Local database path
    m_localDatabasePath = baseDirPath + "/local.db";
    qDebug() << "Database:" << m_localDatabasePath;

    // Migration config path
    m_migrationConfigPath = ":/assets/migration.json";

    // Thumbnail path
    QString defaultThumbnailPath = baseDirPath + "/thumbnails";
    m_thumbnailPath = settings.value("ThumbnailDir", defaultThumbnailPath)
                              .toString();
    qDebug() << "ThumbnailDir:" << m_thumbnailPath;

    // Crash dump path
    m_crashDumpPath = baseDirPath + "/crash_dump";
    qDebug() << "Crash dump path:" << m_crashDumpPath;
    d.mkpath(m_crashDumpPath);

    // Crash upload host
    m_crashUploadHost = settings.value("CrashUploadHost").toString();
    qDebug() << "Crash upload host:" << m_crashUploadHost;

    // Set default codec
    // This affects codec for zip files
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

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
    qRegisterMetaType<QList<QUrl> >("QList<QUrl>");
    qRegisterMetaType<QList<QVariantHash> >("QList<QVariantHash>");
    qRegisterMetaType<QSharedPointer<PlayList> >("QSharedPointer<PlayList>");

    // Set plugin path
    QCoreApplication::addLibraryPath(qApp->applicationDirPath() + "/plugins");
    QCoreApplication::addLibraryPath(qApp->applicationDirPath() + "/../lib/plugins");
#ifdef Q_OS_OSX
    QCoreApplication::addLibraryPath(qApp->applicationDirPath() + "/../Resources/lib/plugins");
#endif
    QCoreApplication::addLibraryPath(baseDirPath + "/plugins");

    // For high dpi icons
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
}

QString GlobalConfig::crashHandlerPipe() const
{
    return QString("\\\\.\\pipe\\%1-Crash").arg(g_BUILD_ENV);
}

const char *GlobalConfig::buildRevision() const
{
    return g_BUILD_GIT_SHA1;
}

const char *GlobalConfig::buildTimestamp() const
{
    return g_BUILD_TIMESTAMP;
}