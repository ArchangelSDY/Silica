#include "BasketView.h"
#include "ImageGalleryItem.h"

BasketView::BasketView(QWidget *parent) :
    ImageGalleryView(parent)
{
    QPalette pal;
    pal.setBrush(QPalette::Foreground, QColor("#71929E"));
    pal.setBrush(QPalette::Background, QColor("#3C414C"));
    m_scene->setPalette(pal);
    m_scene->setBackgroundBrush(pal.background());
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
