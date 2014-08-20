#ifndef MAINIMAGEGALLERYVIEW_H
#define MAINIMAGEGALLERYVIEW_H

#include "ImageGalleryView.h"

class MainImageGalleryView : public ImageGalleryView
{
    Q_OBJECT
public:
    explicit MainImageGalleryView(QWidget *parent = 0);

protected:
    virtual QMenu *createContextMenu();

};

#endif // MAINIMAGEGALLERYVIEW_H
