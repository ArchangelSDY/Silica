#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>

namespace Ui {
class NotificationWidget;
}

class NotificationWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int transparency READ transparency WRITE setTransparency NOTIFY transparencyChanged)
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

    /**
     * @brief Show notification once with some animation.
     * @param duration Animation duration.
     * @param autoDelete Delete self after finished if true.
     */
    void showOnce(int duration = 2000, bool autoDelete = true);

    void setStickyMode(StickyMode mode);
    void setMessage(const QString &message);

    int transparency() const { return m_transparency; }
    void setTransparency(int transparency);

signals:
    void transparencyChanged(int transparency);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    static const int STICKY_MARGIN;

    void moveToStickyPosition();

    Ui::NotificationWidget *ui;
    StickyMode m_stickyMode;
    int m_transparency;
};

#endif // NOTIFICATIONWIDGET_H
