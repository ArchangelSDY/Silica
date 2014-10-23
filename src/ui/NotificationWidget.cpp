#include <QPainter>
#include <QPropertyAnimation>

#include "NotificationWidget.h"
#include "ui_NotificationWidget.h"

const int NotificationWidget::STICKY_MARGIN = 15;

NotificationWidget::NotificationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotificationWidget) ,
    m_stickyMode(NotificationWidget::StickyCenter) ,
    m_transparency(255)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor("#58B6BF"));
    pal.setColor(QPalette::Text, QColor("#FFFFFF"));
    setPalette(pal);

    connect(this, SIGNAL(transparencyChanged(int)),
            this, SLOT(update()));
}

NotificationWidget::~NotificationWidget()
{
    delete ui;
}

void NotificationWidget::setTransparency(int transparency)
{
    m_transparency = transparency;

    // Set message transparency
    QColor msgColor = palette().color(QPalette::Text);
    msgColor.setAlpha(m_transparency);
    ui->lblMsg->setStyleSheet(
        QString("Color: %1").arg(msgColor.name(QColor::HexArgb)));

    emit transparencyChanged(m_transparency);
}

void NotificationWidget::showOnce(int duration, bool autoDelete)
{
    show();

    QPropertyAnimation *animation =
        new QPropertyAnimation(this, "transparency");
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::OutInCubic);
    animation->setStartValue(0);
    animation->setKeyValueAt(0.5, 255);
    animation->setEndValue(0);

    animation->start(QAbstractAnimation::DeleteWhenStopped);

    if (autoDelete) {
        connect(animation, SIGNAL(finished()), this, SLOT(deleteLater()));
    }
}

void NotificationWidget::setTheme(Theme theme) {
    QPalette pal = palette();

    switch (theme) {
    case ThemeSuccess:
        pal.setColor(QPalette::Background, QColor("#71C341"));
        break;
    case ThemeWarning:
        pal.setColor(QPalette::Background, QColor("#F83"));
        break;
    case ThemeInfo:
        pal.setColor(QPalette::Background, QColor("#58B6BF"));
        break;
    case ThemeMessage:
        pal.setColor(QPalette::Background, QColor("#05B2D2"));
        break;
    default:
        pal.setColor(QPalette::Background, QColor("#58B6BF"));
        break;
    }

    setPalette(pal);
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

    // Paint background
    painter.setPen(Qt::NoPen);
    QColor bgColor = palette().color(QPalette::Background);
    bgColor.setAlpha(m_transparency);
    painter.setBrush(bgColor);
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

    if (targetPos != pos()) {
        move(targetPos);
    }
}
