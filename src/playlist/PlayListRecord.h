#ifndef PLAYLISTRECORD_H
#define PLAYLISTRECORD_H

#include <QObject>
#include <QString>

#include "image/Image.h"
#include "PlayList.h"

class PlayList;

class PlayListRecord : public QObject
{
    Q_OBJECT
public:
    PlayListRecord(const QString &name, const QString &coverPath,
        PlayList *playList = new PlayList(), QObject *parent = 0);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    int count() const { return m_count; }
    void setCount(int count);

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString coverPath() const { return m_coverPath; }
    void setCoverPath(const QString &coverPath) { m_coverPath = coverPath; }

    int type() const;
    void setType(int type);

    bool isSaved() const { return m_id != PlayListRecord::EMPTY_ID; }

    int coverIndex();

    PlayList *playList();
    bool save();
    bool remove();
    bool removeImage(ImagePtr image);
    bool insertImages(const ImageList &images);

    static QList<PlayListRecord *> all();
    static const int UNKNOWN_TYPE = -1;

signals:
    void saved();

private slots:
    void gotItems(const QList<QUrl> &imageUrls,
                  const QList<QVariantHash> &extraInfos);

private:
    static const int EMPTY_ID = -1;
    static const int EMPTY_COVER_INDEX = -1;
    static const int EMPTY_COUNT = -1;

    virtual void flushPlayList();

    int m_id;
    QString m_name;
    QString m_coverPath;
    int m_count;
    int m_coverIndex;
    int m_type;
    PlayList *m_playList;
    bool m_ownPlayList;
};

#endif // PLAYLISTRECORD_H
