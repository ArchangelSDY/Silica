#ifndef ITEMGROUPTITLE_H
#define ITEMGROUPTITLE_H

#include <QGraphicsTextItem>

class ItemGroupTitle : public QGraphicsTextItem
{
public:
    ItemGroupTitle(const QString &title, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    void setMinSize(const QSizeF &minSize);

private:
    QSizeF m_minSize;
};

#endif // ITEMGROUPTITLE_H
