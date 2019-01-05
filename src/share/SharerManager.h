#pragma once

#include <QList>
#include <QSharedPointer>

class Image;
class Sharer;

class SharerManager
{
public:
    static SharerManager *instance();

    ~SharerManager();

    QStringList sharerNames() const;
    bool share(int index, QSharedPointer<Image> image);

private:
    SharerManager();
    void registerSharer(const QString &name, Sharer *sharer);

    static SharerManager *s_instance;
    QList<Sharer *> m_sharers;
    QStringList m_sharerNames;
};
