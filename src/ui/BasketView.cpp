#include "BasketView.h"

#include <QtConcurrent>

#include "models/BasketModel.h"
#include "ui/ImageGalleryItem.h"

BasketView::BasketView(QWidget *parent) :
    ImageGalleryView(parent) ,
    m_basket(nullptr)
{
    QPalette pal;
    pal.setBrush(QPalette::Foreground, QColor("#71929E"));
    pal.setBrush(QPalette::Background, QColor("#3C414C"));
    m_scene->setPalette(pal);
    m_scene->setBackgroundBrush(pal.background());
    m_view->setLineWidth(0);
}

void BasketView::setBasketModel(BasketModel *basket)
{
    Q_ASSERT_X(m_basket == nullptr, "BasketView::setBasketModel", "Basket model should only been set once");

    m_basket = basket;
    setPlayList(m_basket->playList());
    connect(m_playList.data(), SIGNAL(itemsAppended(int)),
            this, SLOT(playListAppend(int)));
    connect(m_playList.data(), &PlayList::itemsChanged, this, &BasketView::reload);
}

QMenu *BasketView::createContextMenu()
{
    QMenu *menu = ImageGalleryView::createContextMenu();

    QAction *actExport = menu->addAction(
        tr("Export To Navigator"), this, SLOT(exportToNavigator()));
    if (m_playList->count() == 0) {
        actExport->setDisabled(true);
    }

    QAction *actAppend = menu->addAction(
        tr("Append To Navigator"), this, SLOT(appendToNavigator()));
    if (m_playList->count() == 0) {
        actAppend->setDisabled(true);
    }

    return menu;
}

void BasketView::exportToNavigator()
{
    // TODO
    // QSharedPointer<PlayList> dupPl = QSharedPointer<PlayList>::create();
    // dupPl->append(m_basket->playList());
    // // TODO: Propagate to Main Window?
    // m_navigator->setPlayList(dupPl);
    // m_basket->clear();
}

void BasketView::appendToNavigator()
{
    // QSharedPointer<PlayList> navPl = m_navigator->playList();
    // QSharedPointer<PlayList> basketPl = m_basket->playList();
    // if (navPl) {
    //     navPl->append(basketPl);

    //     // TODO

    //     // // Sync with record if any
    //     // PlayListRecord *record = navPl->record();
    //     // if (record) {
    //     //     ImageList images = basketPl->toImageList();
    //     //     QtConcurrent::run([record, images]() {
    //     //         record->insertImages(images);
    //     //     });
    //     // }
    // }
    // m_basket->clear();
}
