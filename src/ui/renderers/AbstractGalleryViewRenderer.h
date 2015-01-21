#ifndef ABSTRACTGALLERYVIEWRENDERER_H
#define ABSTRACTGALLERYVIEWRENDERER_H

class QGraphicsItem;
class QGraphicsScene;

class GalleryItem;
class GalleryView;

class AbstractGalleryViewRenderer
{
public:
    AbstractGalleryViewRenderer(GalleryView *galleryView);
    virtual ~AbstractGalleryViewRenderer();

    virtual void layout(QList<GalleryItem *> &items,
                        const QStringList &itemGroups,
                        const QRect &viewGeometry) = 0;

protected:
    QGraphicsScene *scene() const;
    QList<QGraphicsItem *> &itemGroupTitles() const;

private:
    GalleryView *m_galleryView;
};

#endif // ABSTRACTGALLERYVIEWRENDERER_H
