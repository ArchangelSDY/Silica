#include "CompactImageRenderer.h"
#include "GlobalConfig.h"
#include "Image.h"

const int CompactImageRenderer::BORDER = 1;

CompactImageRenderer::CompactImageRenderer(
        AbstractGalleryItemRenderer *parentRenderer,
        QImage *image, const QRect &imageRect) :
    AbstractGalleryItemRenderer(parentRenderer) ,
    m_imageRect(imageRect)
{
    m_image = image;
}

void CompactImageRenderer::layout()
{
    if (!m_image) {
        return;
    }

    AbstractGalleryItemRenderer::layout();

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
    if (m_parentRenderer) {
        m_parentRenderer->paint(painter);
    }

    if (!m_image) {
        return;
    }

    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter->drawImage(m_imageRect, *m_image, m_imageSourceRect);
}
