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

    QAction *actExport = menu->addAction(
        tr("Export To Navigator"), this, SLOT(exportToNavigator()));
    if (!m_playList || m_playList->count() == 0) {
        actExport->setDisabled(true);
    }

    QAction *actAppend = menu->addAction(
        tr("Append To Navigator"), this, SLOT(appendToNavigator()));
    if (!m_playList || m_playList->count() == 0) {
        actAppend->setDisabled(true);
    }

    return menu;
}

void BasketView::exportToNavigator()
{
    PlayList *dupPl = new PlayList(*m_playList);
    Navigator::instance()->setPlayList(dupPl, true);
    m_playList->clear();
}

void BasketView::appendToNavigator()
{
    PlayList *navPl = Navigator::instance()->playList();
    if (navPl && m_playList) {
        navPl->append(m_playList);

        // Sync with record if any
        PlayListRecord *record = navPl->record();
        if (record) {
            record->insertImages(m_playList->toImageList());
        }
    }
    m_playList->clear();
}
