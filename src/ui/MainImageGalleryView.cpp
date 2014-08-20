#include "MainImageGalleryView.h"

MainImageGalleryView::MainImageGalleryView(QWidget *parent) :
    ImageGalleryView(parent)
{
}

QMenu *MainImageGalleryView::createContextMenu()
{
    QMenu *menu = ImageGalleryView::createContextMenu();

    // Basket
    if (!scene()->selectedItems().isEmpty()) {
        menu->addSeparator();
        menu->addAction(tr("Add to Basket"), this, SLOT(addToBasket()));
    }

    return menu;
}
