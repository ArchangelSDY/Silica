#ifndef IMAGEGALLERYVIEW_H
#define IMAGEGALLERYVIEW_H

#include "GalleryView.h"

class ImageGalleryView : public GalleryView
{
    Q_OBJECT
public:
    explicit ImageGalleryView(QWidget *parent = 0);

    void setNavigator(Navigator *navigator);

signals:
    void transitToView();

public slots:
    void playListChange(PlayList playList);
    void playListAppend(PlayList appended);

    void sortByName();
    void sortByAspectRatio();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    Navigator *m_navigator;
    PlayList *m_playList;
};

#endif // IMAGEGALLERYVIEW_H
