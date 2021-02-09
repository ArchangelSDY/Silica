#include "WaterfallImageRenderer.h"

const int WaterfallImageRenderer::PADDING = 10;
const int WaterfallImageRenderer::BORDER = 5;

WaterfallImageRenderer::WaterfallImageRenderer(int itemWidth) :
    AbstractGalleryItemRenderer() ,
    m_itemWidth(itemWidth)
{
}

Qt::AspectRatioMode WaterfallImageRenderer::aspectRatioMode() const
{
    return Qt::KeepAspectRatioByExpanding;
}

void WaterfallImageRenderer::layout()
{
    if (m_imageSize.isEmpty()) {
        return;
    }

    qreal imageWidth = (qreal)m_itemWidth * columnsSpan() - 2 * (qreal)WaterfallImageRenderer::PADDING;
    qreal factor = imageWidth / m_imageSize.width();
    qreal imageHeight = m_imageSize.height() * factor;
    m_imageRect.setSize(QSize(imageWidth, imageHeight));

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

int WaterfallImageRenderer::columnsSpan() const
{
    return m_imageSize.width() >= m_imageSize.height() * 1.5 ? 2 : 1;
}
