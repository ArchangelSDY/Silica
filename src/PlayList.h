#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QHash>
#include <QUrl>

#include "image/Image.h"
#include "playList/group/AbstractPlayListGrouper.h"
#include "playList/sort/AbstractPlayListSorter.h"

class AbstractPlayListFilter;
class PlayListRecord;

class PlayList : public QObject
{
    Q_OBJECT
public:
    PlayList();
    PlayList(const QList<QUrl> &imageUrls);
    PlayList(const QStringList &imagePaths);
    ~PlayList();

    PlayListRecord *record() { return m_record; }
    void setRecord(PlayListRecord *record) { m_record = record; }

    ImagePtr addSinglePath(const QString &);
    ImagePtr addSinglePath(const QUrl &);
    void addMultiplePath(const QString &);
    void addMultiplePath(const QUrl &);

    void sortBy(AbstractPlayListSorter *sorter);

    QString groupNameOf(Image *image) const;
    void groupBy(AbstractPlayListGrouper *grouper);

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

    void append(const QSharedPointer<Image> &image);
    void append(const ImageList &images);
    void append(QSharedPointer<PlayList> playList);

    QList<QSharedPointer<Image> > &operator<<(
            const QSharedPointer<Image> &image);
    QList<QSharedPointer<Image> > &operator<<(
            const ImageList &images);

    void clear();

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

private:
    PlayList(const PlayList &playList);

    ImageList m_allImages;
    ImageList m_filteredImages;
    PlayListRecord *m_record;
    QScopedPointer<AbstractPlayListFilter> m_filter;   // Should never be 0
    QScopedPointer<AbstractPlayListGrouper> m_grouper;
};

#endif // PLAYLIST_H
