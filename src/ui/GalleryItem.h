#ifndef GALLERYITEM_H
#define GALLERYITEM_H

#include <QColor>
#include <QFutureWatcher>
#include <QGraphicsItem>

#include "ui/renderers/AbstractGalleryItemRenderer.h"
#include "ui/renderers/AbstractRendererFactory.h"

class GalleryItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit GalleryItem(AbstractRendererFactory *rendererFactory,
                         QGraphicsItem *parent = 0);
    virtual ~GalleryItem();

    static const int KEY_IS_NAME_FILTERED = 1;
    virtual QString name() const;

    virtual void load() = 0;
    void setRendererFactory(AbstractRendererFactory *factory);
    AbstractGalleryItemRenderer *renderer() const;

    // Have to implement our own visibility system as QGraphicsScene will always
    // ignore hidden items when selecting by rect
    void show();
    void hide();

    /**
     * @brief Call by GalleryView during layout to check if thumbnail is ready.
     * @return True if this item has thumbnail loaded and ready to show.
     */
    virtual bool isReadyToShow();

    /**
     * @brief Make this item selected after shown.
     */
    void scheduleSelectedAfterShown();

    void setIsInsideViewportPreload(bool isInside);

signals:
    void requestLayout();
    void readyToShow();
    void mouseDoubleClicked();

protected:
    static const QColor SELECTED_COLOR;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget);
    void setRenderer(AbstractGalleryItemRenderer *renderer);
    virtual void createRenderer() = 0;
    void setThumbnail(QSharedPointer<QImage> thumbnail);

    // TODO: should not be accessed by derived classes directly
    AbstractRendererFactory *m_rendererFactory;
    QScopedPointer<AbstractGalleryItemRenderer> m_renderer;
    bool m_selectedAfterShownScheduled;

private slots:
    void onThumbnailScaled();

private:
    void setScaledThumbnail(QSharedPointer<QImage>);
    void resetThumbnail();
    void onVisibilityChanged(bool isVisible);

    QSharedPointer<QImage> m_thumbnailScaled;
    QFutureWatcher<QSharedPointer<QImage> > m_thumbnailScaleWatcher;
    QSize m_thumbnailSize;
    bool m_isVisible;
    bool m_isReadyToShow;
    bool m_isInsideViewportPreload;
};

#endif // GALLERYITEM_H
