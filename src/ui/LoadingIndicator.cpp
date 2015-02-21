#include <QAbstractAnimation>
#include <QConicalGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#ifdef ENABLE_OPENGL
#include <QSurfaceFormat>
#endif

#include "LoadingIndicator.h"

LoadingIndicator::LoadingIndicator(const QSize &size, QWidget *parent) :
#ifdef ENABLE_OPENGL
    QOpenGLWidget(parent) ,
#else
    QWidget(parent) ,
#endif
    m_parent(parent) ,
    m_size(size) ,
    m_loadCount(0)
{
    hide();
    setMinimumSize(size);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

#ifdef ENABLE_OPENGL
    QSurfaceFormat format;
    format.setSamples(8);
    setFormat(format);
#endif

    m_timer.setInterval(50);
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
}

void LoadingIndicator::start()
{
    ++m_loadCount;
    if (m_loadCount > 0 && !m_timer.isActive()) {
        m_timer.start();
        show();
    }

//    QPropertyAnimation *aniScale =
//        new QPropertyAnimation(this, "minimumSize", this);
//    aniScale->setDuration(200);
//    aniScale->setStartValue(QSize(0, 0));
//    aniScale->setEndValue(m_size);
//    aniScale->setLoopCount(1);
//    aniScale->setEasingCurve(QEasingCurve(QEasingCurve::OutCubic));
//    aniScale->start(QAbstractAnimation::DeleteWhenStopped);
}

void LoadingIndicator::stop()
{
    if (m_loadCount > 0) {
        --m_loadCount;
    }

    if (m_loadCount == 0 && m_timer.isActive()) {
        m_timer.stop();
        hide();
    }

//    QPropertyAnimation *aniScale =
//        new QPropertyAnimation(this, "minimumSize", this);
//    aniScale->setDuration(200);
//    aniScale->setStartValue(minimumSize());
//    aniScale->setEndValue(QSize(0, 0));
//    aniScale->setLoopCount(1);
//    aniScale->setEasingCurve(QEasingCurve(QEasingCurve::InCubic));
//    aniScale->start(QAbstractAnimation::DeleteWhenStopped);
//    connect(aniScale, SIGNAL(finished()), this, SLOT(hide()));
}

void LoadingIndicator::paintEvent(QPaintEvent *)
{
    m_angle = (m_angle - 36) % 360;

    QPainter painter(this);
    painter.setRenderHints(
        QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

#ifdef ENABLE_OPENGL
    painter.fillRect(rect(), QColor("#3C414C"));
#endif

    qreal circleWidth = width();
    qreal circleHeight = height();
    qreal circleBorder = 2;

    QConicalGradient circleGradient(circleWidth / 2, circleHeight / 2, m_angle);
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
