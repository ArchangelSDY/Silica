#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QList>
#include <QSize>
#include <QString>

class GlobalConfig
{
public:
    static GlobalConfig *create();
    static GlobalConfig *instance();

    QList<QString> zipDirs() const { return m_zipDirs; }
    const QString &wallpaperDir() const { return m_wallpaperDir; }
    const QString &localDatabasePath() const { return m_localDatabasePath; }
    const QString &migrationConfigPath() const { return m_migrationConfigPath; }
    const QString &thumbnailPath() const { return m_thumbnailPath; }
    const QString &netCachePath() const { return m_netCachePath; }
    const QString &pluginsPath() const { return m_pluginsPath; }
    const QSize &galleryItemSize() const { return m_galleryItemSize; }

    const char *buildRevision() const;
    const char *buildTimestamp() const;

private:
    GlobalConfig();

    void load();

    static GlobalConfig *m_instance;
    QList<QString> m_zipDirs;
    QString m_wallpaperDir;
    QString m_localDatabasePath;
    QString m_migrationConfigPath;
    QString m_thumbnailPath;
    QString m_netCachePath;
    QString m_pluginsPath;
    QSize m_galleryItemSize;
};

#endif // GLOBALCONFIG_H
