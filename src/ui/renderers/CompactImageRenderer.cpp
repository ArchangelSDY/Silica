#include "CompactImageRenderer.h"
#include "GlobalConfig.h"
#include "Image.h"

CompactImageRenderer::CompactImageRenderer(
        AbstractGalleryItemRenderer *parentRenderer,
        QImage *image, const QRect &boundingRect) :
    AbstractGalleryItemRenderer(parentRenderer) ,
    m_border(1) ,
    m_borderColor(Qt::transparent) ,
    m_padding(0) ,
    m_paddingColor(Qt::transparent) ,
    m_translateX(0) ,
    m_translateY(0)
{
    m_image = image;
    m_boundingRect = boundingRect;
}

void CompactImageRenderer::layout()
{
    AbstractGalleryItemRenderer::layout();

    if (!m_image) {
        return;
    }

    const QSize &imageSize = m_image->size();
    QSize sourcePaintSize = m_boundingRect.size().scaled(
        imageSize, Qt::KeepAspectRatio);
    m_imageSourceRect.setSize(sourcePaintSize);
    m_imageSourceRect.moveTo(
        (imageSize.width() - sourcePaintSize.width()) / 2,
        (imageSize.height() - sourcePaintSize.height()) / 2
    );

    m_imageRect = m_boundingRect
        .adjusted(m_padding + m_border, m_padding + m_border,
                  - m_padding - m_border, -m_padding - m_border)
        .translated(m_translateX, m_translateY);
}

void CompactImageRenderer::paint(QPainter *painter)
{
    if (m_parentRenderer) {
        m_parentRenderer->paint(painter);
    }

    if (!m_image || m_image->isNull()) {
        return;
    }

    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QRect paddingBg = m_boundingRect.marginsRemoved(
        QMargins(m_border, m_border, m_border, m_border));
    painter->fillRect(paddingBg, m_paddingColor);

    painter->drawImage(m_imageRect, *m_image, m_imageSourceRect);
}

AbstractGalleryItemRenderer *CompactImageRenderer::setBorder(
        int border, const QColor &color)
{
    m_border = border;
    m_borderColor = color;
    return this;
}

AbstractGalleryItemRenderer *CompactImageRenderer::setPadding(
        int padding, const QColor &color)
{
    m_padding = padding;
    m_paddingColor = color;
    return this;
}

AbstractGalleryItemRenderer *CompactImageRenderer::translate(int dx, int dy)
{
    m_translateX = dx;
    m_translateY = dy;
    return this;
}
