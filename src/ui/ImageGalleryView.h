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

    void setNavigator(Navigator *navigator);

public slots:
    void setPlayList(QSharedPointer<PlayList> playList);

    void playListChange(QSharedPointer<PlayList>);
    void playListAppend(int start);

protected slots:
    void sortByName();
    void sortByUrl();
    void sortByAspectRatio();
    void sortBySize();

    void setAsCover();
    void addToBasket();
    void removeSelected();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual QMenu *createContextMenu();

    void groupBy(AbstractPlayListGrouper *grouper);

    Navigator *m_navigator;
    QSharedPointer<PlayList> m_playList;
    RankFilterMenuManager *m_rankFilterMenuManager;

    virtual QString groupForItem(GalleryItem *item) override;
};

#endif // IMAGEGALLERYVIEW_H
