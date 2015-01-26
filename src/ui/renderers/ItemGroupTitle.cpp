#include <QMarginsF>
#include <QPainter>

#include "ItemGroupTitle.h"

ItemGroupTitle::ItemGroupTitle(const QString &title, QGraphicsItem *parent) :
    QGraphicsTextItem(title, parent)
{
    setDefaultTextColor(Qt::white);
}

QRectF ItemGroupTitle::boundingRect() const
{
    QRectF textRect = QGraphicsTextItem::boundingRect();
    textRect.setWidth(qMax(textRect.width(), m_minSize.width()));
    textRect.setHeight(qMax(textRect.height(), m_minSize.height()));
    return textRect;
}

void ItemGroupTitle::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF bgRect = boundingRect();
    qreal bgMargin = bgRect.height() * 0.2;
    bgRect = bgRect.marginsRemoved(QMarginsF(0, bgMargin, 0, bgMargin));
    painter->fillRect(bgRect, QColor("#AA0099EE"));

    QRectF textRect = bgRect;
    qreal textMargin = textRect.height() * 0.2;
    textRect = textRect.marginsRemoved(QMarginsF(textMargin, 0, 0, 0));
    painter->setPen(defaultTextColor());
    painter->setFont(font());
    painter->drawText(textRect, Qt::AlignVCenter, toPlainText());
}

void ItemGroupTitle::setMinSize(const QSizeF &minSize)
{
    m_minSize = minSize;
}

