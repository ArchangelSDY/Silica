#include "BasketView.h"

#include <QtConcurrent>

#include "models/BasketModel.h"
#include "ui/ImageGalleryItem.h"

BasketView::BasketView(QWidget *parent) :
    ImageGalleryView(parent) ,
    m_basket(nullptr)
{
    QPalette pal;
    pal.setBrush(QPalette::WindowText, QColor("#71929E"));
    pal.setBrush(QPalette::Window, QColor("#3C414C"));
    m_scene->setPalette(pal);
    m_scene->setBackgroundBrush(pal.window());
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

    QAction *actExport = menu->addAction(tr("Export To Navigator"), [this]() {
        emit this->commit(CommitOption::Replace);
    });
    if (m_playList->count() == 0) {
        actExport->setDisabled(true);
    }

    QAction *actAppend = menu->addAction(tr("Append To Navigator"), [this]() {
        emit this->commit(CommitOption::Append);
    });
    if (m_playList->count() == 0) {
        actAppend->setDisabled(true);
    }

    return menu;
}
