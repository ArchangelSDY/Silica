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
    void enterItem();

protected slots:
    void setLooseRenderer();
    void setCompactRenderer();

    void enableGrouping();
    void disableGrouping();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void keyPressEvent(QKeyEvent *event);

    virtual void clear();
    virtual void layout();

    void setRendererFactory(AbstractRendererFactory *factory);

    virtual QString groupForItem(QGraphicsItem *) { return QString(); }
    virtual void sortByGroup(QList<QGraphicsItem *> *) {}

    QGraphicsScene *m_scene;
    AbstractRendererFactory *m_rendererFactory;
    bool m_enableGrouping;
};

#endif // GALLERYVIEW_H
