#pragma once

#include "ImageGalleryView.h"

class BasketModel;

class BasketView : public ImageGalleryView
{
    Q_OBJECT
public:
    enum class CommitOption
    {
        Replace,
        Append,
    };

    explicit BasketView(QWidget *parent = 0);

    void setBasketModel(BasketModel *basket);

signals:
    void commit(CommitOption option);

protected:
    virtual QMenu *createContextMenu();

private:
    BasketModel *m_basket;
};
