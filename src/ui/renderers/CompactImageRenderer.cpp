#include "CompactImageRenderer.h"
#include "GlobalConfig.h"
#include "Image.h"

CompactImageRenderer::CompactImageRenderer(
        AbstractGalleryItemRenderer *parentRenderer,
        const QRect &boundingRect) :
    AbstractGalleryItemRenderer(parentRenderer) ,
    m_border(1) ,
    m_borderColor(Qt::transparent) ,
    m_padding(0) ,
    m_paddingColor(Qt::transparent) ,
    m_translateX(0) ,
    m_translateY(0)
{
    m_boundingRect = boundingRect;
}

Qt::AspectRatioMode CompactImageRenderer::aspectRatioMode() const
{
    return Qt::KeepAspectRatioByExpanding;
}

void CompactImageRenderer::layout(const QRect &viewGeometry)
{
    AbstractGalleryItemRenderer::layout(viewGeometry);

    QSize sourcePaintSize = m_boundingRect.size().scaled(
        m_imageSize, Qt::KeepAspectRatio);
    m_imageSourceRect.setSize(sourcePaintSize);
    m_imageSourceRect.moveTo(
        (m_imageSize.width() - sourcePaintSize.width()) / 2,
        (m_imageSize.height() - sourcePaintSize.height()) / 2
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
