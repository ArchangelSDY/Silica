#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QUrl>

#include "Image.h"
#include "PlayListRecord.h"

class PlayList : public QObject
{
    Q_OBJECT
public:
    PlayList();
    PlayList(const QList<QUrl> &imageUrls);

    PlayListRecord *record() { return m_record; }
    void setRecord(PlayListRecord *record) { m_record = record; }

    void addPath(const QString &);
    void addPath(const QUrl &);

    void sortByName();
    void sortByAspectRatio();

    // Delegate to inner QList
    typedef QList<QSharedPointer<Image> >::const_iterator const_iterator;
    inline const_iterator begin() const
    {
        return m_images.begin();
    }
    inline const_iterator end() const
    {
        return m_images.end();
    }

    inline void append(const QSharedPointer<Image> &image)
    {
        int start = count();
        m_images.append(image);
        emit itemsAppended(start);
    }
    inline void append(const QList<QSharedPointer<Image> > &images)
    {
        int start = count();
        m_images.append(images);
        emit itemsAppended(start);
    }
    inline void append(PlayList *playList, bool watching = false)
    {
        int start = count();
        m_images.append(playList->m_images);
        emit itemsAppended(start);

        if (watching) {
            connect(playList, SIGNAL(itemsAppended(int)),
                    this, SLOT(watchedPlayListAppended(int)));
        }
    }

    inline QList<QSharedPointer<Image> > &operator<<(
            const QSharedPointer<Image> &image)
    {
        int start = count();
        QList<QSharedPointer<Image> > &ret =  m_images << image;
        emit itemsAppended(start);
        return ret;
    }
    inline QList<QSharedPointer<Image> > &operator<<(
            const QList<QSharedPointer<Image> > &images)
    {
        int start = count();
        QList<QSharedPointer<Image> > &ret = m_images << images;
        emit itemsAppended(start);
        return ret;
    }

    inline void clear()
    {
        m_images.clear();
        emit itemsChanged();
    }

    inline QSharedPointer<Image> &operator[](int i)
    {
        return m_images[i];
    }
    inline const QSharedPointer<Image> &operator[](int i) const
    {
        return m_images[i];
    }
    inline const QSharedPointer<Image> &at(int i) const
    {
        return m_images.at(i);
    }

    inline int count() const
    {
        return m_images.count();
    }
    inline int size() const
    {
        return m_images.size();
    }
    inline bool isEmpty() const
    {
        return m_images.isEmpty();
    }

signals:
    void itemsChanged();
    void itemsAppended(int start);

private slots:
    void watchedPlayListAppended(int start);

private:
    QList<QSharedPointer<Image> > m_images;
    PlayListRecord *m_record;
};

#endif // PLAYLIST_H
