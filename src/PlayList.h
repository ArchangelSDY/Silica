#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QHash>
#include <QUrl>

#include "AbstractPlayListFilter.h"
#include "Image.h"
#include "PlayListRecord.h"

class PlayList : public QObject
{
    Q_OBJECT
public:
    PlayList();
    PlayList(const QList<QUrl> &imageUrls);
    PlayList(const PlayList &playList);
    ~PlayList();

    PlayListRecord *record() { return m_record; }
    void setRecord(PlayListRecord *record) { m_record = record; }

    void addPath(const QString &);
    void addPath(const QUrl &);

    void sortByName();
    void sortByAspectRatio();
    void sortByGroup();

    int groupForImage(Image * const image);
    void groupByThumbHist();

    void setFilter(AbstractPlayListFilter *filter);

    // Delegate to inner QList
    typedef QList<QSharedPointer<Image> >::const_iterator const_iterator;
    inline const_iterator begin() const
    {
        return m_filteredImages.begin();
    }
    inline const_iterator end() const
    {
        return m_filteredImages.end();
    }

    inline void append(const QSharedPointer<Image> &image)
    {
        ImageList l;
        l << image;
        append(l);
    }
    inline void append(const ImageList &images)
    {
        int start = count();
        m_allImages.append(images);
        m_filteredImages.append(m_filter->filtered(images));
        emit itemsAppended(start);
    }
    inline void append(PlayList *playList, bool watching = false)
    {
        int start = count();
        m_allImages.append(playList->m_allImages);
        m_filteredImages.append(playList->m_filteredImages);
        emit itemsAppended(start);

        if (watching) {
            connect(playList, SIGNAL(itemsAppended(int)),
                    this, SLOT(watchedPlayListAppended(int)));
        }
    }

    inline QList<QSharedPointer<Image> > &operator<<(
            const QSharedPointer<Image> &image)
    {
        ImageList l;
        l << image;
        return (*this << l);
    }
    inline QList<QSharedPointer<Image> > &operator<<(
            const ImageList &images)
    {
        int start = count();
        m_allImages << images;
        QList<QSharedPointer<Image> > &ret =
            m_filteredImages << m_filter->filtered(images);
        emit itemsAppended(start);
        return ret;
    }

    inline void clear()
    {
        m_allImages.clear();
        m_filteredImages.clear();
        emit itemsChanged();
    }

    inline QSharedPointer<Image> &operator[](int i)
    {
        return m_filteredImages[i];
    }
    inline const QSharedPointer<Image> &operator[](int i) const
    {
        return m_filteredImages[i];
    }
    inline const QSharedPointer<Image> &at(int i) const
    {
        return m_filteredImages.at(i);
    }

    inline int count() const
    {
        return m_filteredImages.count();
    }
    inline int size() const
    {
        return m_filteredImages.size();
    }
    inline bool isEmpty() const
    {
        return m_filteredImages.isEmpty();
    }
    inline ImageList toImageList()
    {
        return m_filteredImages;
    }

    bool removeOne(const ImagePtr &val);
    void removeAt(int index);

signals:
    void itemsChanged();
    void itemsAppended(int start);

private slots:
    void watchedPlayListAppended(int start);

private:
    bool groupLessThan(const ImagePtr &left, const ImagePtr &right);

    ImageList m_allImages;
    ImageList m_filteredImages;
    PlayListRecord *m_record;
    AbstractPlayListFilter *m_filter;   // Should never be 0

    QHash<Image *, int> m_imageGroups;
};

#endif // PLAYLIST_H
