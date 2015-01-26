#include <GalleryView.h>

#include "AbstractGalleryViewRenderer.h"

AbstractGalleryViewRenderer::AbstractGalleryViewRenderer(
        GalleryView *galleryView) :
    m_galleryView(galleryView)
{
}

AbstractGalleryViewRenderer::~AbstractGalleryViewRenderer()
{
}

QGraphicsScene *AbstractGalleryViewRenderer::scene() const
{
    return m_galleryView->scene();
}

QList<QGraphicsItem *> &AbstractGalleryViewRenderer::itemGroupTitles() const
{
    return m_galleryView->m_itemGroupTitles;
}
