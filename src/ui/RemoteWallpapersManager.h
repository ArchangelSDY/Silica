#ifndef REMOTEWALLPAPERSMANAGER_H
#define REMOTEWALLPAPERSMANAGER_H

#include <QObject>

#include "Image.h"

class NotificationWidget;

class RemoteWallpapersManager : public QObject
{
    Q_OBJECT
public:
    explicit RemoteWallpapersManager(QWidget *parentWidget);

    void addImageToWallpapers(ImagePtr image);

private slots:
    void showResult();
    void showSuccess();
    void showError();

private:
    static const char *WALLPAPERS_ALBUM;

    QWidget *m_parentWidget;
};

#endif // REMOTEWALLPAPERSMANAGER_H
