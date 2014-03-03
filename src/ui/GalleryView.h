#ifndef GALLERYVIEW_H
#define GALLERYVIEW_H

#include <QGraphicsView>

#include "AbstractRendererFactory.h"
#include "Navigator.h"
#include "PlayList.h"

class GalleryView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GalleryView(QWidget *parent = 0);
    ~GalleryView();

signals:
    void mouseDoubleClicked();

protected slots:
    void setLooseRenderer();
    void setCompactRenderer();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);

    virtual void clear();
    virtual void layout();

    void setRendererFactory(AbstractRendererFactory *factory);

    QGraphicsScene *m_scene;
    AbstractRendererFactory *m_rendererFactory;
};

#endif // GALLERYVIEW_H
