#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>

namespace Ui {
class NotificationWidget;
}

class NotificationWidget : public QWidget
{
    Q_OBJECT
public:
    enum StickyMode {
        StickyTopLeft,
        StickyTopRight,
        StickyBottomLeft,
        StickyBottomRight,
        StickyCenter,
        NoSticky,
    };

    explicit NotificationWidget(QWidget *parent = 0);
    ~NotificationWidget();

    void setStickyMode(StickyMode mode);
    void setMessage(const QString &message);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    static const int STICKY_MARGIN;

    void moveToStickyPosition();

    Ui::NotificationWidget *ui;
    StickyMode m_stickyMode;
};

#endif // NOTIFICATIONWIDGET_H
