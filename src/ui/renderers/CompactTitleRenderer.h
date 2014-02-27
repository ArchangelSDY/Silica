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

    QString m_title;
    QRect m_titleRect;
};

#endif // COMPACTTITLERENDERER_H
