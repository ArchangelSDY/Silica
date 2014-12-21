#include <QConicalGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QDebug>

#include "LoadingIndicator.h"

LoadingIndicator::LoadingIndicator(QWidget *parent) :
    QWidget(parent) ,
    m_parent(parent) ,
    m_aniSpin(new QPropertyAnimation(this, "angle", this))
{
    hide();
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    connect(this, SIGNAL(angleChanged(qreal)),
            this, SLOT(update()));

    m_aniSpin->setDuration(500);
    m_aniSpin->setStartValue(360);
    m_aniSpin->setEndValue(0);
    m_aniSpin->setLoopCount(-1);
}

void LoadingIndicator::start()
{
    QPropertyAnimation *aniScale =
        new QPropertyAnimation(this, "minimumSize", this);
    aniScale->setDuration(200);
    aniScale->setStartValue(QSize(0, 0));
    aniScale->setEndValue(QSize(25, 25));
    aniScale->setLoopCount(1);
    aniScale->setEasingCurve(QEasingCurve(QEasingCurve::OutCubic));
    aniScale->start(QAbstractAnimation::DeleteWhenStopped);

    m_aniSpin->start();
    show();
}

void LoadingIndicator::stop()
{
    QPropertyAnimation *aniScale =
        new QPropertyAnimation(this, "minimumSize", this);
    aniScale->setDuration(200);
    aniScale->setStartValue(minimumSize());
    aniScale->setEndValue(QSize(0, 0));
    aniScale->setLoopCount(1);
    aniScale->setEasingCurve(QEasingCurve(QEasingCurve::InCubic));
    aniScale->start(QAbstractAnimation::DeleteWhenStopped);
    connect(aniScale, SIGNAL(finished()), this, SLOT(hide()));

    m_aniSpin->stop();
}

qreal LoadingIndicator::angle() const
{
    return m_angle;
}

void LoadingIndicator::setAngle(qreal angle)
{
    m_angle = angle;
    emit angleChanged(angle);
}

void LoadingIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHints(
        QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    qreal circleWidth = width();
    qreal circleHeight = height();
    qreal circleBorder = 2;

    QConicalGradient circleGradient(circleWidth / 2, circleHeight / 2, angle());
    circleGradient.setColorAt(0, QColor("#07A9CB"));
    circleGradient.setColorAt(0.5, QColor("#23B3AD"));
    circleGradient.setColorAt(0.75, QColor("#40C07E"));
    circleGradient.setColorAt(1, QColor("#00ACE3D7"));
    circleGradient.setColorAt(1, Qt::transparent);
    QBrush circleBrush(circleGradient);
    painter.setBrush(circleBrush);
    painter.setPen(Qt::NoPen);

    QPainterPath outerPath;
    outerPath.addEllipse(0, 0, circleWidth, circleHeight);
    QPainterPath innerPath;
    innerPath.addEllipse(circleBorder, circleBorder,
                         circleWidth - 2 * circleBorder,
                         circleHeight - 2 * circleBorder);
    QPainterPath circlePath = outerPath - innerPath;

    painter.drawPath(circlePath);
}
