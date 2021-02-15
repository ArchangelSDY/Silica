#pragma once

#include <QHash>
#include <QUrl>

#include "image/Image.h"
#include "playList/group/AbstractPlayListGrouper.h"
#include "playList/sort/AbstractPlayListSorter.h"

class AbstractPlayListFilter;

class PlayList : public QObject
{
    Q_OBJECT
public:
    PlayList();
    PlayList(const QList<QUrl> &imageUrls);
    PlayList(const QStringList &imagePaths);
    ~PlayList();

    ImagePtr addSinglePath(const QString &);
    ImagePtr addSinglePath(const QUrl &);
    void addMultiplePath(const QString &);
    void addMultiplePath(const QUrl &);

    void sortBy(AbstractPlayListSorter *sorter);

    QString groupNameOf(const ImagePtr &mage) const;
    void groupBy(AbstractPlayListGrouper *grouper);

    void setFilter(AbstractPlayListFilter *filter);

    // Delegate to inner QList
    typedef QList<ImagePtr>::const_iterator const_iterator;
    inline const_iterator begin() const
    {
        return m_filteredImages.begin();
    }
    inline const_iterator end() const
    {
        return m_filteredImages.end();
    }

    void append(const ImagePtr &image);
    void append(const ImageList &images);
    void append(QSharedPointer<PlayList> playList);
    void replace(int index, const ImagePtr &image);

    QList<ImagePtr> &operator<<(const ImagePtr &image);
    QList<ImagePtr> &operator<<(const ImageList &images);

    void clear();

    inline const ImagePtr &operator[](int i) const
    {
        return m_filteredImages[i];
    }
    inline const ImagePtr &at(int i) const
    {
        return m_filteredImages.at(i);
    }
    inline ImageList mid(int pos, int length = -1) const
    {
        return m_filteredImages.mid(pos, length);
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
    QList<QUrl> toImageUrls() const;

    bool removeOne(const ImagePtr &val);
    void removeAt(int index);

signals:
    void itemChanged(int index);
    void itemsChanged();
    void itemsAppended(int start);

private:
    PlayList(const PlayList &playList);

    ImageList m_allImages;
    ImageList m_filteredImages;
    QScopedPointer<AbstractPlayListFilter> m_filter;   // Should never be 0
    QScopedPointer<AbstractPlayListGrouper> m_grouper;
};
