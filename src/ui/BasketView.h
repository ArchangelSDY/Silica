#ifndef BASKETVIEW_H
#define BASKETVIEW_H

#include "ImageGalleryView.h"

class BasketView : public ImageGalleryView
{
    Q_OBJECT
public:
    explicit BasketView(QWidget *parent = 0);

protected slots:
    virtual void exportToNavigator();
    virtual void appendToNavigator();

protected:
    virtual QMenu *createContextMenu();

};

#endif // BASKETVIEW_H
