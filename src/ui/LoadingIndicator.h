#ifndef LOADINGINDICATOR_H
#define LOADINGINDICATOR_H

#include <QList>
#include <QTimer>
#include <QWidget>

#ifdef ENABLE_OPENGL
#include <QOpenGLWidget>
#endif

#include "sapi/LoadingIndicatorDelegate.h"

class QPropertyAnimation;
class TaskProgress;

class LoadingIndicator
#ifdef ENABLE_OPENGL
    : public QOpenGLWidget
#else
    : public QWidget
#endif
    , public sapi::LoadingIndicatorDelegate::Indicator
{
    Q_OBJECT
public:
    explicit LoadingIndicator(const QSize &size, QWidget *parent = 0);

public slots:
    virtual void addTaskProgress(const TaskProgress &progress);
    virtual void removeTaskProgress(const TaskProgress &progress);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QWidget *m_parent;
    QSize m_size;
    int m_infiniteSpinAngle;
    int m_loadCount;

    QTimer m_timer;

    QList<const TaskProgress *> m_entries;
};

#endif // LOADINGINDICATOR_H
