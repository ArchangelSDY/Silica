#ifndef MAINIMAGEGALLERYVIEW_H
#define MAINIMAGEGALLERYVIEW_H

#include "ImageGalleryView.h"

class BasketModel;

class MainImageGalleryView : public ImageGalleryView
{
    Q_OBJECT
public:
    explicit MainImageGalleryView(QWidget *parent = 0);

    void setBasketModel(BasketModel *basket);

protected:
    virtual void keyPressEvent(QKeyEvent *event);

    virtual QMenu *createContextMenu();

private slots:
    void addToBasket();

private:
    BasketModel *m_basket;
};

#endif // MAINIMAGEGALLERYVIEW_H
