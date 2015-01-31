#ifndef PLAYLISTPROVIDER_H
#define PLAYLISTPROVIDER_H

#include <QObject>
#include <QVariantHash>

class PlayListProvider : public QObject
{
    Q_OBJECT
public:
    explicit PlayListProvider(QObject *parent = 0) : QObject(parent) {}
    virtual ~PlayListProvider() {}

    virtual void request(const QString &name,
                         const QVariantHash &extra = QVariantHash()) = 0;

signals:
    void gotItems(const QList<QUrl> &urls,
                  const QList<QVariantHash> &extraInfos);
};

Q_DECLARE_INTERFACE(PlayListProvider, "com.archangelsdy.silica.playlistprovider")

#endif // PLAYLISTPROVIDER_H
