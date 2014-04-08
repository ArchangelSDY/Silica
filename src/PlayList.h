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
        m_images.append(image);
    }
    inline void append(const QList<QSharedPointer<Image> > &images)
    {
        m_images.append(images);
    }
    inline void append(PlayList *playList)
    {
        m_images.append(playList->m_images);
    }

    inline QList<QSharedPointer<Image> > &operator<<(
            const QSharedPointer<Image> &image)
    {
        return m_images << image;
    }
    inline QList<QSharedPointer<Image> > &operator<<(
            const QList<QSharedPointer<Image> > &images)
    {
        return m_images << images;
    }

    inline void clear()
    {
        m_images.clear();
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

private:
    QList<QSharedPointer<Image> > m_images;
    PlayListRecord *m_record;
};

#endif // PLAYLIST_H
