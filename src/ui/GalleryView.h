#ifndef GALLERYVIEW_H
#define GALLERYVIEW_H

#include <QGraphicsView>

#include "AbstractRendererFactory.h"
#include "Navigator.h"
#include "PlayList.h"

class QLineEdit;

class GalleryItem;

class GalleryView : public QGraphicsView
{
    Q_OBJECT
    friend class AbstractGalleryViewRenderer;
public:
    explicit GalleryView(QWidget *parent = 0);
    ~GalleryView();

    QList<GalleryItem *> galleryItems() const;
    QList<GalleryItem *> selectedGalleryItems() const;

signals:
    void mouseDoubleClicked();
    void keyEnterPressed();
    void loadStart();
    void loadEnd();

public slots:
    virtual void scheduleLayout();
    void setNameFilter(const QString &nameFilter);

protected slots:
    void setLooseRenderer();
    void setCompactRenderer();
    void setWaterfallRenderer();

    void enableGrouping();
    void disableGrouping();

    void itemReadyToShow();

    void enterSearch();
    void leaveSearch();

private slots:
    virtual void layout();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

    virtual void clear();
    virtual void incrItemsToLoad(int count);

    void addItem(GalleryItem *item);
    AbstractRendererFactory *rendererFactory();
    void setRendererFactory(AbstractRendererFactory *factory);

    virtual QString groupForItem(GalleryItem *) { return QString(); }
    /**
     * @brief Sort GalleryItem by group.
     *
     * Note that this method MUST NOT be used to sort the model (for example,
     * PlayList) because model sorting will often trigger re-creation of
     * GalleryItems, which may cause invalid pointers during layout.
     */
    virtual void sortItemByGroup(QList<GalleryItem *> *) {}

    virtual void markItemIsFiltered(GalleryItem *item);

    QGraphicsScene *m_scene;
    QLineEdit *m_searchBox;
    bool m_enableGrouping;
    bool m_layoutNeeded;
    QTimer m_layoutTimer;
    int m_loadingItemsCount;
    QString m_nameFilter;

private:
    static const int LAYOUT_INTERVAL = 10;

    AbstractRendererFactory *m_rendererFactory;
    QList<QGraphicsItem *> m_itemGroupTitles;
};

#endif // GALLERYVIEW_H
