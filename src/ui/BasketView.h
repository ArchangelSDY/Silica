#pragma once

#include "ImageGalleryView.h"

class BasketModel;

class BasketView : public ImageGalleryView
{
    Q_OBJECT
public:
    explicit BasketView(QWidget *parent = 0);

    void setBasketModel(BasketModel *basket);

protected slots:
    virtual void exportToNavigator();
    virtual void appendToNavigator();

protected:
    virtual QMenu *createContextMenu();

private:
    BasketModel *m_basket;
};
