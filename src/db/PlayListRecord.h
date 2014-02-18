#ifndef PLAYLISTRECORD_H
#define PLAYLISTRECORD_H

#include "PlayList.h"

class PlayListRecord
{
public:
    PlayListRecord(const QString &name, const QString &coverPath,
        PlayList *playList = 0);

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString coverPath() const { return m_coverPath; }
    void setCoverPath(const QString &coverPath) { m_coverPath = coverPath; }

    PlayList *playList();

    static QList<PlayListRecord> fromLocalDatabase();
    bool saveToLocalDatabase();

private:
    QString m_name;
    QString m_coverPath;
    PlayList *m_playList;
};

#endif // PLAYLISTRECORD_H
