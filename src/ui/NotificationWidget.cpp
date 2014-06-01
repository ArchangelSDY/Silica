#include <QPainter>

#include "NotificationWidget.h"
#include "ui_NotificationWidget.h"

const int NotificationWidget::STICKY_MARGIN = 15;

NotificationWidget::NotificationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotificationWidget) ,
    m_stickyMode(NotificationWidget::StickyCenter)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

NotificationWidget::~NotificationWidget()
{
    delete ui;
}

void NotificationWidget::setStickyMode(StickyMode mode)
{
    m_stickyMode = mode;
}

void NotificationWidget::setMessage(const QString &message)
{
    ui->lblMsg->setText(message);
    adjustSize();
}

void NotificationWidget::paintEvent(QPaintEvent *)
{
    moveToStickyPosition();

    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    // painter.setBrush(QColor("#F8C014"));     // Orange color
    painter.setBrush(QColor("#58B6BF"));
    painter.drawRoundedRect(rect(), 5, 5);
}

void NotificationWidget::moveToStickyPosition()
{
    if (m_stickyMode == NoSticky || parentWidget() == 0) {
        return;
    }

    const QRect &parentRect = parentWidget()->rect();
    QPoint targetPos;

    switch (m_stickyMode) {
    case StickyTopLeft:
        targetPos = parentRect.topLeft()
            + QPoint(STICKY_MARGIN, STICKY_MARGIN);
        break;
    case StickyTopRight:
        targetPos = parentRect.topRight()
            + QPoint(- STICKY_MARGIN - rect().width(), STICKY_MARGIN);
        break;
    case StickyBottomLeft:
        targetPos = parentRect.bottomLeft()
            + QPoint(STICKY_MARGIN, - STICKY_MARGIN - rect().height());
        break;
    case StickyBottomRight:
        targetPos = parentRect.bottomRight()
            - QPoint(STICKY_MARGIN + rect().width(),
                     STICKY_MARGIN + rect().height());
        break;
    case StickyCenter:
        targetPos = parentRect.center() - QPoint(rect().width() / 2,
                                                 rect().height() / 2);
        break;
    default:
        targetPos = pos();
    }

    move(targetPos);
}
