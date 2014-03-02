#ifndef PLAYLISTRECORD_H
#define PLAYLISTRECORD_H

#include <QObject>
#include <QString>

class PlayList;

class PlayListRecord : public QObject
{
    Q_OBJECT
public:
    PlayListRecord(const QString &name, const QString &coverPath,
        PlayList *playList = 0, QObject *parent = 0);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString coverPath() const { return m_coverPath; }
    void setCoverPath(const QString &coverPath) { m_coverPath = coverPath; }

    PlayList *playList();
    int coverIndex();

    static QList<PlayListRecord *> all();
    bool save();
    bool remove();

signals:
    void saved();

private:
    static const int EMPTY_ID = -1;
    static const int EMPTY_COVER_INDEX = -1;

    int m_id;
    QString m_name;
    QString m_coverPath;
    int m_coverIndex;
    PlayList *m_playList;
};

#endif // PLAYLISTRECORD_H
