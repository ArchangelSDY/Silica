#ifndef COMPACTTITLERENDERER_H
#define COMPACTTITLERENDERER_H

#include "AbstractGalleryItemRenderer.h"

class CompactTitleRenderer : public AbstractGalleryItemRenderer
{
public:
    CompactTitleRenderer(const QString &title,
                         AbstractGalleryItemRenderer *parentRenderer);

    virtual void layout();
    virtual void paint(QPainter *painter);

private:
    static const int TITLE_HEIGHT;
    static const int TITLE_LEFT_PADDING;

    QString m_title;
    QRect m_titleRect;
    QRect m_textRect;
};

#endif // COMPACTTITLERENDERER_H
