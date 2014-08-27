#ifndef PLAYLISTRECORD_H
#define PLAYLISTRECORD_H

#include <QObject>
#include <QString>

#include "Image.h"

class PlayList;

class PlayListRecord : public QObject
{
    Q_OBJECT
public:
    PlayListRecord(const QString &name, const QString &coverPath,
        PlayList *playList = 0, QObject *parent = 0);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    int count() const { return m_count; }
    void setCount(int count);

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString coverPath() const { return m_coverPath; }
    void setCoverPath(const QString &coverPath) { m_coverPath = coverPath; }

    enum PlayListType {
        LocalPlayList,
        RemotePlayList,
    };

    PlayListType type() const { return m_type; }
    bool isSaved() const { return m_id != PlayListRecord::EMPTY_ID; }

    int coverIndex();

    virtual PlayList *playList() = 0;
    virtual bool save();
    virtual bool remove();
    virtual bool removeImage(Image *);
    virtual bool insertImages(const ImageList &images);

    static QList<PlayListRecord *> all();
    static PlayListRecord *create(PlayListType type,
                                  const QString &name,
                                  const QString &coverPath);

signals:
    void saved();

protected:
    static const int EMPTY_ID = -1;
    static const int EMPTY_COVER_INDEX = -1;
    static const int EMPTY_COUNT = -1;

    virtual void flushPlayList();

    int m_id;
    QString m_name;
    QString m_coverPath;
    PlayListType m_type;
    int m_count;
    int m_coverIndex;
    PlayList *m_playList;
};

#endif // PLAYLISTRECORD_H
