#ifndef ABSTRACTGALLERYVIEWRENDERER_H
#define ABSTRACTGALLERYVIEWRENDERER_H

class QGraphicsScene;

class GalleryItem;

class AbstractGalleryViewRenderer
{
public:
    AbstractGalleryViewRenderer(QGraphicsScene *scene) :
        m_scene(scene)
    {
    }

    virtual ~AbstractGalleryViewRenderer() {}

    virtual void layout(QList<GalleryItem *> &items,
                        const QStringList &itemGroups,
                        const QRect &viewGeometry) = 0;

protected:
    QGraphicsScene *m_scene;
};

#endif // ABSTRACTGALLERYVIEWRENDERER_H
