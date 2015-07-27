#ifndef PLAYLISTRECORD_H
#define PLAYLISTRECORD_H

#include <QObject>
#include <QString>

#include "image/Image.h"

class PlayList;
class PlayListProvider;

class PlayListRecord : public QObject
{
    Q_OBJECT
    friend class PlayListRecordBuilder;
public:
    ~PlayListRecord();

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    int count() const { return m_count; }
    int type() const;
    QString typeName() const;

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString coverPath() const { return m_coverPath; }
    void setCoverPath(const QString &coverPath) { m_coverPath = coverPath; }

    int coverIndex();

    bool isSaved() const { return m_id != PlayListRecord::EMPTY_ID; }

    PlayList *playList();
    void reload();

    QVariantHash providerExtra() const;
    bool save();
    bool remove();
    bool removeImage(ImagePtr image);
    bool insertImages(const ImageList &images);

    void continueProvide();

    static QList<PlayListRecord *> all();
    static const int UNKNOWN_TYPE = -1;

signals:
    void saved();

private slots:
    void gotItems(const QList<QUrl> &imageUrls,
                  const QList<QVariantHash> &extraInfos);
    void providerItemsCountChanged(int count);

private:
    static const int EMPTY_ID = -1;
    static const int EMPTY_COVER_INDEX = -1;
    static const int EMPTY_COUNT = -1;

    PlayListRecord();
    virtual void flushPlayList();

    int m_id;
    QString m_name;
    QString m_coverPath;
    int m_count;
    int m_coverIndex;
    int m_type;
    PlayListProvider *m_provider;
    PlayList *m_playList;
    bool m_ownPlayList;
};


class PlayListRecordBuilder
{
public:
    PlayListRecordBuilder();
    ~PlayListRecordBuilder();

    PlayListRecordBuilder &setId(int id);
    PlayListRecordBuilder &setName(const QString &name);
    PlayListRecordBuilder &setCoverPath(const QString &coverPath);
    PlayListRecordBuilder &setCount(int count);
    PlayListRecordBuilder &setType(int type);
    PlayListRecordBuilder &setPlayList(PlayList *playlist);

    // PlayListRecordBuilder &setProvider(PlayListProvider *provider);

    PlayListRecord *obtain();

private:
    PlayListRecord *m_record;
    bool m_isObtained;
};

#endif // PLAYLISTRECORD_H
