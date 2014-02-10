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
    const QSize &galleryItemSize() const { return m_galleryItemSize; }
private:
    GlobalConfig();

    void load();

    static GlobalConfig *m_instance;
    QList<QString> m_zipDirs;
    QString m_wallpaperDir;
    QSize m_galleryItemSize;
};

#endif // GLOBALCONFIG_H