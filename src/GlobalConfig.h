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

    QList<QString> searchDirs() const { return m_searchDirs; }
    const QString &wallpaperDir() const { return m_wallpaperDir; }
    const QString &localDatabasePath() const { return m_localDatabasePath; }
    const QString &migrationConfigPath() const { return m_migrationConfigPath; }
    const QString &thumbnailPath() const { return m_thumbnailPath; }
    const QString &fileSystemThumbnailPath() const { return m_fileSystemThumbnailPath; }
    const QSize &galleryItemSize() const { return m_galleryItemSize; }
    const QString &crashDumpPath() const { return m_crashDumpPath; }
    const QString &crashUploadHost() const { return m_crashUploadHost; }
    QString crashHandlerPipe() const;

    const char *buildRevision() const;
    const char *buildTimestamp() const;

private:
    GlobalConfig();

    void load();

    static GlobalConfig *m_instance;
    QList<QString> m_searchDirs;
    QString m_wallpaperDir;
    QString m_localDatabasePath;
    QString m_migrationConfigPath;
    QString m_thumbnailPath;
    QString m_fileSystemThumbnailPath;
    QSize m_galleryItemSize;
    QString m_crashDumpPath;
    QString m_crashUploadHost;
};

#endif // GLOBALCONFIG_H
