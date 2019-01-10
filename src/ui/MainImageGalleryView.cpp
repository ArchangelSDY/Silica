#include "MainImageGalleryView.h"

#include <QKeyEvent>

#include "models/BasketModel.h"
#include "ImageGalleryItem.h"

MainImageGalleryView::MainImageGalleryView(QWidget *parent) :
    ImageGalleryView(parent) ,
    m_basket(nullptr)
{
}

void MainImageGalleryView::setBasketModel(BasketModel *basket)
{
    m_basket = basket;
}

void MainImageGalleryView::keyPressEvent(QKeyEvent *event)
{
    // Press 'Shift + B' to add images to basket
    if (event->modifiers() & Qt::ShiftModifier && event->key() == Qt::Key_B) {
        addToBasket();
    } else {
        ImageGalleryView::keyPressEvent(event);
    }
}

QMenu *MainImageGalleryView::createContextMenu()
{
    QMenu *menu = ImageGalleryView::createContextMenu();

    // Basket
    if (!selectedGalleryItems().isEmpty()) {
        menu->addSeparator();
        menu->addAction(tr("Add to Basket"), this, SLOT(addToBasket()));
    }

    return menu;
}

void MainImageGalleryView::addToBasket()
{
    QList<GalleryItem *> selectedItems = selectedGalleryItems();
    for (int i = 0; i < selectedItems.count(); ++i) {
        ImageGalleryItem *item =
            static_cast<ImageGalleryItem *>(selectedItems[i]);

        ImagePtr image = item->image();
        m_basket->add(image);
    }
}
