#ifndef GALLERYVIEW_H
#define GALLERYVIEW_H

#include <QGraphicsView>

#include "Navigator.h"
#include "PlayList.h"

class GalleryView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GalleryView(QWidget *parent = 0);
    ~GalleryView();

    void setNavigator(Navigator *navigator);

signals:
    void transitToView();

public slots:
    void playListChange(PlayList playList);
    void playListAppend(PlayList appended);
    void sortByName();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    void clear();
    void layout();

    Navigator *m_navigator;
    PlayList *m_playList;
    QGraphicsScene *m_scene;
};

#endif // GALLERYVIEW_H
