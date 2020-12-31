#ifndef IMAGEGALLERYVIEW_H
#define IMAGEGALLERYVIEW_H

#include <QScopedPointer>

#include "AbstractRendererFactory.h"
#include "GalleryView.h"
#include "RankFilterMenuManager.h"

class PlayListEntity;

class ImageGalleryView : public GalleryView
{
    Q_OBJECT
public:
    explicit ImageGalleryView(QWidget *parent = 0);
    ~ImageGalleryView();

public slots:
    void setPlayList(QSharedPointer<PlayList> playList);
    void setPlayListEntity(PlayListEntity *playListEntity);
    void reload();

    void playListItemChange(int index);
    //void playListChange(QSharedPointer<PlayList>);
    void playListAppend(int start);

protected slots:
    void sortByName();
    void sortByUrl();
    void sortByAspectRatio();
    void sortBySize();

    void setAsCover();
    void removeSelected();
    void refreshSelected();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual QMenu *createContextMenu();

    void groupBy(AbstractPlayListGrouper *grouper);

    QSharedPointer<PlayList> m_playList;
    PlayListEntity *m_playListEntity;
    QScopedPointer<RankFilterMenuManager> m_rankFilterMenuManager;

    virtual QString groupForItem(GalleryItem *item) override;
};

#endif // IMAGEGALLERYVIEW_H
