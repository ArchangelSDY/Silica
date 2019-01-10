#include "BasketView.h"

#include <QtConcurrent>

#include "playlist/PlayListRecord.h"
#include "ui/ImageGalleryItem.h"

BasketView::BasketView(QWidget *parent) :
    ImageGalleryView(parent)
{
    QPalette pal;
    pal.setBrush(QPalette::Foreground, QColor("#71929E"));
    pal.setBrush(QPalette::Background, QColor("#3C414C"));
    m_scene->setPalette(pal);
    m_scene->setBackgroundBrush(pal.background());
    m_view->setLineWidth(0);
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
    QSharedPointer<PlayList> dupPl = QSharedPointer<PlayList>::create();
    dupPl->append(m_playList);
    // TODO: Propagate to Main Window?
    m_navigator->setPlayList(dupPl);
    m_playList->clear();
}

void BasketView::appendToNavigator()
{
    QSharedPointer<PlayList> navPl = m_navigator->playList();
    if (navPl && m_playList) {
        navPl->append(m_playList);

        // Sync with record if any
        PlayListRecord *record = navPl->record();
        if (record) {
            ImageList images = m_playList->toImageList();
            QtConcurrent::run([record, images]() {
                record->insertImages(images);
            });
        }
    }
    m_playList->clear();
}
