#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QList>
#include <QSize>
#include <QString>

class GlobalConfig
{
public:
    static GlobalConfig *instance();

    QList<QString> zipDirs() const { return m_zipDirs; }
    const QString &wallpaperDir() const { return m_wallpaperDir; }
    const QString &localDatabasePath() const { return m_localDatabasePath; }
    const QString &migrationConfigPath() const { return m_migrationConfigPath; }
    const QString &thumbnailPath() const { return m_thumbnailPath; }
    const QString &netCachePath() const { return m_netCachePath; }
    const QSize &galleryItemSize() const { return m_galleryItemSize; }
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
    QSize m_galleryItemSize;
};

#endif // GLOBALCONFIG_H
