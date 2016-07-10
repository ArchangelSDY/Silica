#ifndef IMAGEGALLERYVIEW_H
#define IMAGEGALLERYVIEW_H

#include "AbstractRendererFactory.h"
#include "GalleryView.h"
#include "RankFilterMenuManager.h"

class ImageGalleryView : public GalleryView
{
    Q_OBJECT
public:
    explicit ImageGalleryView(QWidget *parent = 0);
    ~ImageGalleryView();

public slots:
    void setPlayList(QSharedPointer<PlayList> playList);

    void playListChange(QSharedPointer<PlayList>);
    void playListAppend(int start);

protected slots:
    void sortByName();
    void sortByAspectRatio();
    void sortBySize();

    void setAsCover();
    void addToBasket();
    void removeSelected();

    void groupByThumbHist();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

    virtual QMenu *createContextMenu();

    QSharedPointer<PlayList> m_playList;
    RankFilterMenuManager *m_rankFilterMenuManager;

    enum GroupMode {
        GroupByThumbHist,
    };
    GroupMode m_groupMode;
    virtual QString groupForItem(GalleryItem *);
};

#endif // IMAGEGALLERYVIEW_H
