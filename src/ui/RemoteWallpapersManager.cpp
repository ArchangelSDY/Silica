#include "NotificationWidget.h"
#include "RemoteDatabase.h"
#include "RemoteWallpapersManager.h"

const char *RemoteWallpapersManager::WALLPAPERS_ALBUM = "wallpapers";

RemoteWallpapersManager::RemoteWallpapersManager(QWidget *parentWidget) :
    QObject(parentWidget) ,
    m_parentWidget(parentWidget)
{
}

void RemoteWallpapersManager::addImageToWallpapers(ImagePtr image)
{
    QNetworkReply *reply =
        RemoteDatabase::instance()->addImageToAlbum(image, WALLPAPERS_ALBUM);
    if (reply) {
        connect(reply, SIGNAL(finished()), this, SLOT(showResult()));
    } else {
        showError();
    }
}

void RemoteWallpapersManager::showResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        showSuccess();
    } else {
        showError();
    }
    reply->deleteLater();
}

void RemoteWallpapersManager::showSuccess()
{
    NotificationWidget *w = new NotificationWidget(m_parentWidget);

    QPalette pal = w->palette();
    pal.setColor(QPalette::Background, QColor("#71C341"));
    w->setPalette(pal);

    w->setMessage(tr("Added as Wallpapers!"));

    w->showOnce();
}

void RemoteWallpapersManager::showError()
{
    NotificationWidget *w = new NotificationWidget(m_parentWidget);

    QPalette pal = w->palette();
    pal.setColor(QPalette::Background, QColor("#F83"));
    w->setPalette(pal);

    w->setMessage("Oops! Fail to Add as Wallpapers!");

    w->showOnce();
}
