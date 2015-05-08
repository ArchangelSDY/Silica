#ifndef LOADINGINDICATOR_H
#define LOADINGINDICATOR_H

#include <QList>
#include <QTimer>
#include <QWidget>

#ifdef ENABLE_OPENGL
#include <QOpenGLWidget>
#endif

class QPropertyAnimation;

#include "ui/TaskProgress.h"

#ifdef ENABLE_OPENGL
class LoadingIndicator : public QOpenGLWidget
#else
class LoadingIndicator : public QWidget
#endif
{
    Q_OBJECT
public:
    explicit LoadingIndicator(const QSize &size, QWidget *parent = 0);

public slots:
    void addTaskProgress(const TaskProgress &progress);
    void removeTaskProgress(const TaskProgress &progress);

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
