#pragma once

#include <QList>

class Image;
class Sharer;

class SharerManager
{
public:
    static SharerManager *instance();

    ~SharerManager();

    QStringList sharerNames() const;
    bool share(int index, const Image *image);

private:
    SharerManager();
    void registerSharer(const QString &name, Sharer *sharer);

    static SharerManager *s_instance;
    QList<Sharer *> m_sharers;
    QStringList m_sharerNames;
};
