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
    void setPlayList(PlayList *playList);

    void playListChange(PlayList *playList = 0);
    void playListAppend(int start);

protected slots:
    void sortByName();
    void sortByAspectRatio();

    void setAsCover();
    void addToBasket();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

    virtual QMenu *createContextMenu();

    PlayList *m_playList;
    RankFilterMenuManager *m_rankFilterMenuManager;
};

#endif // IMAGEGALLERYVIEW_H
