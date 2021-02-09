#ifndef GALLERYVIEW_H
#define GALLERYVIEW_H

#include <QGraphicsView>
#include <QSet>

#include "ui/models/TaskProgress.h"
#include "ui/renderers/AbstractRendererFactory.h"
#include "Navigator.h"
#include "PlayList.h"

class QLineEdit;

class GalleryItem;

class GalleryView : public QWidget
{
    Q_OBJECT
    friend class AbstractGalleryViewRenderer;
public:
    explicit GalleryView(QWidget *parent = 0);
    virtual ~GalleryView() {}

    QGraphicsScene *scene() const;
    const QList<GalleryItem *> &galleryItems() const;
    QList<GalleryItem *> selectedGalleryItems() const;

    const TaskProgress &loadProgress() const;
    const TaskProgress &groupingProgress() const;

signals:
    void mouseDoubleClicked();
    void keyEnterPressed();
    void loadStart();
    void loadEnd();

public slots:
    virtual void scheduleLayout();
    void setNameFilter(const QString &nameFilter);

protected slots:
    void itemMouseDoubleClicked();

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
    virtual void keyPressEvent(QKeyEvent *event);

    virtual void clear();
    virtual void incrItemsToLoad(int count);

    void addItem(GalleryItem *item);
    void replaceItem(int index, GalleryItem *item);
    AbstractRendererFactory *rendererFactory();
    void setRendererFactory(AbstractRendererFactory *factory);

    virtual QString groupForItem(GalleryItem *item) { return QString(); }
    /**
     * @brief Sort GalleryItem by group.
     *
     * Note that this method MUST NOT be used to sort the model (for example,
     * PlayList) because model sorting will often trigger re-creation of
     * GalleryItems, which may cause invalid pointers during layout.
     */
    virtual void sortItemByGroup(QList<GalleryItem *> *) {}

    virtual void markItemIsFiltered(GalleryItem *item);

    void markItemsInsideViewportPreload();

    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    QLineEdit *m_searchBox;
    bool m_enableGrouping;
    QTimer m_layoutTimer;
    QString m_nameFilter;

    TaskProgress m_loadProgress;
    TaskProgress m_groupingProgress;

private:
    static const int LAYOUT_INTERVAL;
    class GraphicsView;
    friend class GraphicsView;

    void setupItem(GalleryItem *item);

    QList<GalleryItem *> m_galleryItems;
    QScopedPointer<AbstractRendererFactory> m_rendererFactory;
    QList<QGraphicsItem *> m_itemGroupTitles;
    QSet<QGraphicsItem *> m_viewportPreloadItems;
};

#endif // GALLERYVIEW_H
