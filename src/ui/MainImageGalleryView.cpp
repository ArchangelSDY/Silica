#include "MainImageGalleryView.h"

#include <QKeyEvent>

MainImageGalleryView::MainImageGalleryView(QWidget *parent) :
    ImageGalleryView(parent)
{
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
