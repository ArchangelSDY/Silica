#include "BasketView.h"
#include "ImageGalleryItem.h"

BasketView::BasketView(QWidget *parent) :
    ImageGalleryView(parent)
{
}

QMenu *BasketView::createContextMenu()
{
    QMenu *menu = ImageGalleryView::createContextMenu();

    menu->addAction(tr("Export To Navigator"), this, SLOT(exportToNavigator()));

    return menu;
}

void BasketView::exportToNavigator()
{
    PlayList *dupPl = new PlayList(*m_playList);
    Navigator::instance()->setPlayList(dupPl, true);
    m_playList->clear();
}
