#ifndef IMAGEGALLERYVIEW_H
#define IMAGEGALLERYVIEW_H

#include "AbstractRendererFactory.h"
#include "GalleryView.h"

class ImageGalleryView : public GalleryView
{
    Q_OBJECT
public:
    explicit ImageGalleryView(QWidget *parent = 0);
    ~ImageGalleryView();

    void setNavigator(Navigator *navigator);

public slots:
    void playListChange(PlayList playList);
    void playListAppend(PlayList appended);

private slots:
    void sortByName();
    void sortByAspectRatio();

    void setLooseRenderer();
    void setCompactRenderer();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    void setRendererFactory(AbstractRendererFactory *factory);

    Navigator *m_navigator;
    PlayList *m_playList;
    AbstractRendererFactory *m_rendererFactory;
};

#endif // IMAGEGALLERYVIEW_H
