#include "LoadingIndicator.h"

#include <QAbstractAnimation>
#include <QConicalGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#ifdef ENABLE_OPENGL
#include <QSurfaceFormat>
#endif

#include "ui/models/TaskProgress.h"

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
    m_timer.start();
}

void LoadingIndicator::addTaskProgress(const TaskProgress &progress)
{
    m_entries.append(&progress);
    connect(&progress, SIGNAL(changed()),
            this, SLOT(update()));
    update();
}

void LoadingIndicator::removeTaskProgress(const TaskProgress &progress)
{
    m_entries.removeAll(&progress);
    disconnect(&progress, SIGNAL(valueChanged(int)),
               this, SLOT(update()));
    update();
}

void LoadingIndicator::paintEvent(QPaintEvent *)
{
    bool isInfinite = false;
    double minProgress;
    int validCnt = 0;
    foreach (const TaskProgress *progress, m_entries) {
        if (progress->isRunning()) {
            if (progress->minimum() == progress->maximum()) {
                isInfinite = true;
                break;
            }

            validCnt++;
            double curProgress = ((double)(progress->value() - progress->minimum())) / (progress->maximum() - progress->minimum());
            if (validCnt == 1) {
                minProgress = curProgress;
            } else if (curProgress < minProgress) {
                minProgress = curProgress;
            }
        }
    }

    QPainter painter(this);
    painter.setRenderHints(
        QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    #ifdef ENABLE_OPENGL
        painter.fillRect(rect(), QColor("#3C414C"));
    #endif

    if (validCnt == 0) {
        m_timer.stop();
        return;
    } else {
        m_timer.start();
    }

    qreal circleWidth = width();
    qreal circleHeight = height();
    qreal circleBorder = 2;

    if (!isInfinite) {
        // Draw arc
        QPen circlePen(QColor("#07A9CB"), circleBorder);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(circlePen);

        int startAngle = 90 * 16;
        int spanAngle = - minProgress * 360 * 16;
        painter.drawArc(circleBorder, circleBorder,
                        circleWidth - 2 * circleBorder, circleHeight - 2 * circleBorder,
                        startAngle, spanAngle);
    } else {
        // Draw infinite spin circle
        m_infiniteSpinAngle = (m_infiniteSpinAngle - 36) % 360;

        QConicalGradient circleGradient(circleWidth / 2, circleHeight / 2, m_infiniteSpinAngle);
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
}
