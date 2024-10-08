#include "GlobalConfig.h"
#include "LooseImageRenderer.h"

const int LooseImageRenderer::PADDING = 10;
const int LooseImageRenderer::BORDER = 5;

LooseImageRenderer::LooseImageRenderer() :
    AbstractGalleryItemRenderer()
{
}

Qt::AspectRatioMode LooseImageRenderer::aspectRatioMode() const
{
    return Qt::KeepAspectRatio;
}

void LooseImageRenderer::layout(const QRect &viewGeometry)
{
    const QSize &itemSize = GlobalConfig::instance()->galleryItemSize();
    QSize sizeWithoutPadding(
        itemSize.width() - 2 * LooseImageRenderer::PADDING,
        itemSize.height() - 2 * LooseImageRenderer::PADDING
    );
    m_imageRect.setSize(
        m_imageSize.scaled(sizeWithoutPadding, Qt::KeepAspectRatio));

    m_imageRect.moveTo(
        (itemSize.width() - m_imageRect.width()) / 2,
        (itemSize.height() - m_imageRect.height()) / 2
    );

    m_borderRect.setSize(m_imageRect.size() +
        QSize(2 * LooseImageRenderer::BORDER, 2 * LooseImageRenderer::BORDER));
    m_borderRect.moveTo(m_imageRect.topLeft() -
        QPoint(LooseImageRenderer::BORDER, LooseImageRenderer::BORDER));

    m_boundingRect = m_borderRect;
}

void LooseImageRenderer::paint(QPainter *painter)
{
    if (!m_image) {
        return;
    }

    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // Border
    if (!m_image->isNull()) {
        painter->setBrush(Qt::white);
        painter->setPen(Qt::white);
    } else {
        painter->setBrush(QColor("#8a92a2"));
        painter->setPen(QColor("#8a92a2"));
    }
    painter->drawRoundedRect(m_borderRect,
        LooseImageRenderer::BORDER, LooseImageRenderer::BORDER);

    // Image
    painter->drawImage(m_imageRect, *m_image);
}
