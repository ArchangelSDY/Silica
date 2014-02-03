#ifndef GALLERYVIEW_H
#define GALLERYVIEW_H

#include <QGraphicsView>

#include "PlayList.h"

class GalleryView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GalleryView(QWidget *parent = 0);
    ~GalleryView();

public slots:
    void playListChange(PlayList playList);
    void playListAppend(PlayList appended);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);

private:
    void clear();
    void layout();

    PlayList *m_playList;
    QGraphicsScene *m_scene;
};

#endif // GALLERYVIEW_H
