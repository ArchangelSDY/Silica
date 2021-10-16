#include <QPainter>
#include <QPropertyAnimation>

#include "NotificationWidget.h"
#include "ui_NotificationWidget.h"

const int NotificationWidget::STICKY_MARGIN = 15;

NotificationWidget::NotificationWidget(QWidget *relativeWidget) :
    QWidget(nullptr),
    ui(new Ui::NotificationWidget) ,
    m_relativeWidget(relativeWidget) ,
    m_stickyMode(NotificationWidget::StickyCenter) ,
    m_transparency(255)
{
    ui->setupUi(this);
    setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor("#58B6BF"));
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
    animation->setEasingCurve(QEasingCurve::InCubic);
    animation->setStartValue(255);
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
        pal.setColor(QPalette::Window, QColor("#71C341"));
        break;
    case ThemeWarning:
        pal.setColor(QPalette::Window, QColor("#F83"));
        break;
    case ThemeInfo:
        pal.setColor(QPalette::Window, QColor("#58B6BF"));
        break;
    case ThemeMessage:
        pal.setColor(QPalette::Window, QColor("#05B2D2"));
        break;
    default:
        pal.setColor(QPalette::Window, QColor("#58B6BF"));
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
    QColor bgColor = palette().color(QPalette::Window);
    bgColor.setAlpha(m_transparency);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(rect(), 5, 5);
}

void NotificationWidget::moveToStickyPosition()
{
    if (m_stickyMode == NoSticky || m_relativeWidget == nullptr) {
        return;
    }

    const QRect &parentRect = m_relativeWidget->rect();
    QPoint targetPos;

    switch (m_stickyMode) {
    case StickyTopLeft:
        targetPos = m_relativeWidget->mapToGlobal(parentRect.topLeft())
            + QPoint(STICKY_MARGIN, STICKY_MARGIN);
        break;
    case StickyTopRight:
        targetPos = m_relativeWidget->mapToGlobal(parentRect.topRight())
            + QPoint(- STICKY_MARGIN - rect().width(), STICKY_MARGIN);
        break;
    case StickyBottomLeft:
        targetPos = m_relativeWidget->mapToGlobal(parentRect.bottomLeft())
            + QPoint(STICKY_MARGIN, - STICKY_MARGIN - rect().height());
        break;
    case StickyBottomRight:
        targetPos = m_relativeWidget->mapToGlobal(parentRect.bottomRight())
            - QPoint(STICKY_MARGIN + rect().width(),
                     STICKY_MARGIN + rect().height());
        break;
    case StickyCenter:
        targetPos = m_relativeWidget->mapToGlobal(parentRect.center()) - QPoint(rect().width() / 2,
                                                  rect().height() / 2);
        break;
    default:
        return;
    }

    move(targetPos);
}
