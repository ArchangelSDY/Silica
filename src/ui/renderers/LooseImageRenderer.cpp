#include "GlobalConfig.h"
#include "LooseImageRenderer.h"

const int LooseImageRenderer::PADDING = 10;
const int LooseImageRenderer::BORDER = 5;

LooseImageRenderer::LooseImageRenderer() :
    AbstractGalleryItemRenderer()
{
}

void LooseImageRenderer::layout()
{
    if (!m_image) {
        return;
    }

    const QSize &itemSize = GlobalConfig::instance()->galleryItemSize();
    QSize sizeWithoutPadding(
        itemSize.width() - 2 * LooseImageRenderer::PADDING,
        itemSize.height() - 2 * LooseImageRenderer::PADDING
    );
    m_imageRect.setSize(
        m_image->size().scaled(sizeWithoutPadding, Qt::KeepAspectRatio));

    m_imageRect.moveTo(
        (itemSize.width() - m_imageRect.width()) / 2,
        (itemSize.height() - m_imageRect.height()) / 2
    );

    m_borderRect.setSize(m_imageRect.size() +
        QSize(2 * LooseImageRenderer::BORDER, 2 * LooseImageRenderer::BORDER));
    m_borderRect.moveTo(m_imageRect.topLeft() -
        QPoint(LooseImageRenderer::BORDER, LooseImageRenderer::BORDER));

    m_boundingRect = QRect(QPoint(0, 0), itemSize);
}

void LooseImageRenderer::paint(QPainter *painter)
{
    if (!m_image) {
        return;
    }

    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // Border
    painter->setBrush(Qt::white);
    painter->setPen(Qt::white);
    painter->drawRoundedRect(m_borderRect,
        LooseImageRenderer::BORDER, LooseImageRenderer::BORDER);

    // Image
    painter->drawImage(m_imageRect, *m_image);
}
