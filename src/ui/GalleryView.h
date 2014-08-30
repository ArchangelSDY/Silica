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

public slots:
    virtual void scheduleLayout();

protected slots:
    void setLooseRenderer();
    void setCompactRenderer();
    void setWaterfallRenderer();

    void enableGrouping();
    void disableGrouping();

private slots:
    virtual void layout();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void keyPressEvent(QKeyEvent *event);

    virtual void clear();

    void setRendererFactory(AbstractRendererFactory *factory);

    virtual QString groupForItem(QGraphicsItem *) { return QString(); }
    virtual void sortByGroup(QList<QGraphicsItem *> *) {}

    QGraphicsScene *m_scene;
    AbstractRendererFactory *m_rendererFactory;
    bool m_enableGrouping;
    bool m_layoutNeeded;
    QTimer m_layoutTimer;

private:
    static const int LAYOUT_INTERVAL = 10;
};

#endif // GALLERYVIEW_H
