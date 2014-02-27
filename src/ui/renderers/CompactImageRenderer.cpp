#include "CompactImageRenderer.h"
#include "GlobalConfig.h"

const int CompactImageRenderer::BORDER = 1;

CompactImageRenderer::CompactImageRenderer() :
    AbstractGalleryItemRenderer()
{
}

void CompactImageRenderer::layout()
{
    QSize margins(2 * CompactImageRenderer::BORDER, 2 * CompactImageRenderer::BORDER);
    m_imageRect.setSize(GlobalConfig::instance()->galleryItemSize() - margins);
    m_imageRect.moveTo(BORDER, BORDER);
    const QSize &imageSize = m_image->size();

    QSize sourcePaintSize = m_imageRect.size().scaled(
        imageSize, Qt::KeepAspectRatio);
    m_imageSourceRect.setSize(sourcePaintSize);
    m_imageSourceRect.moveTo(
        (imageSize.width() - sourcePaintSize.width()) / 2,
        (imageSize.height() - sourcePaintSize.height()) / 2
    );

    m_boundingRect = m_imageRect;
}

void CompactImageRenderer::paint(QPainter *painter)
{
    if (!m_image) {
        return;
    }

    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter->drawImage(m_imageRect, *m_image, m_imageSourceRect);
}
