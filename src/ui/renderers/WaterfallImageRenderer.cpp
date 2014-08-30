#include "GlobalConfig.h"
#include "WaterfallImageRenderer.h"

const int WaterfallImageRenderer::PADDING = 10;
const int WaterfallImageRenderer::BORDER = 5;

WaterfallImageRenderer::WaterfallImageRenderer() :
    AbstractGalleryItemRenderer()
{
}

void WaterfallImageRenderer::layout()
{
    if (!m_image) {
        return;
    }

    const QSize &itemSize = GlobalConfig::instance()->galleryItemSize();
    QSize sizeWithoutPadding(
        itemSize.width() - 2 * WaterfallImageRenderer::PADDING,
        INT_MAX
    );
    m_imageRect.setSize(
        m_image->size().scaled(sizeWithoutPadding, Qt::KeepAspectRatio));

    m_imageRect.moveTo(
        WaterfallImageRenderer::PADDING,
        WaterfallImageRenderer::PADDING
    );

    m_borderRect.setSize(m_imageRect.size() +
        QSize(2 * WaterfallImageRenderer::BORDER,
              2 * WaterfallImageRenderer::BORDER));
    m_borderRect.moveTo(m_imageRect.topLeft() -
        QPoint(WaterfallImageRenderer::BORDER, WaterfallImageRenderer::BORDER));

    QRect itemRect(0, 0,
        m_imageRect.width() + 2 * WaterfallImageRenderer::PADDING,
        m_imageRect.height() + 2 * WaterfallImageRenderer::PADDING
    );
    m_boundingRect = itemRect;
}

void WaterfallImageRenderer::paint(QPainter *painter)
{
    if (!m_image) {
        return;
    }

    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // Border
    painter->setBrush(Qt::white);
    painter->setPen(Qt::white);
    painter->drawRect(m_borderRect);

    // Image
    painter->drawImage(m_imageRect, *m_image);
}
