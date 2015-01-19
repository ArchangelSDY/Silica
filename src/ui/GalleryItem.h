#ifndef GALLERYITEM_H
#define GALLERYITEM_H

#include <QColor>
#include <QGraphicsItem>

#include "AbstractGalleryItemRenderer.h"
#include "AbstractRendererFactory.h"

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

    /**
     * @brief Call by GalleryView during layout to check if thumbnail is ready.
     * @return True if this item has thumbnail loaded and ready to show.
     */
    virtual bool isReadyToShow();

    /**
     * @brief Make this item selected after shown.
     */
    void scheduleSelectedAfterShown();

signals:
    void readyToShow();

protected:
    static const QColor SELECTED_COLOR;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget);
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);
    void setRenderer(AbstractGalleryItemRenderer *renderer);
    virtual void createRenderer() = 0;
    void setThumbnail(QImage *thumbnail);

    // TODO: should not be accessed by derived classes directly
    AbstractRendererFactory *m_rendererFactory;
    AbstractGalleryItemRenderer *m_renderer;
    bool m_isReadyToShow;
    bool m_selectedAfterShownScheduled;

private:
    QImage *m_thumbnail;
};

#endif // GALLERYITEM_H
