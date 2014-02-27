#include "GlobalConfig.h"
#include "LooseImageRenderer.h"

const int LooseImageRenderer::PADDING = 10;
const int LooseImageRenderer::BORDER = 5;

LooseImageRenderer::LooseImageRenderer(Image *image) :
    AbstractGalleryItemRenderer() ,
    m_image(image)
{
}

void LooseImageRenderer::layout()
{
    const QImage &image = m_image->thumbnail();
    const QSize &itemSize = GlobalConfig::instance()->galleryItemSize();
    QSize sizeWithoutPadding(
        itemSize.width() - 2 * LooseImageRenderer::PADDING,
        itemSize.height() - 2 * LooseImageRenderer::PADDING
    );
    m_imageRect.setSize(
        image.size().scaled(sizeWithoutPadding, Qt::KeepAspectRatio));

    m_imageRect.moveTo(
        (itemSize.width() - m_imageRect.width()) / 2,
        (itemSize.height() - m_imageRect.height()) / 2
    );

    m_borderRect.setSize(m_imageRect.size() +
        QSize(2 * LooseImageRenderer::BORDER, 2 * LooseImageRenderer::BORDER));
    m_borderRect.moveTo(m_imageRect.topLeft() -
        QPoint(LooseImageRenderer::BORDER, LooseImageRenderer::BORDER));
}

void LooseImageRenderer::paint(QPainter *painter)
{
    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // Border
    painter->setBrush(Qt::white);
    painter->setPen(Qt::white);
    painter->drawRoundedRect(m_borderRect,
        LooseImageRenderer::BORDER, LooseImageRenderer::BORDER);

    // Image
    painter->drawImage(m_imageRect, m_image->thumbnail());
}
