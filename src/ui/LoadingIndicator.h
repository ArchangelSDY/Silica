#ifndef LOADINGINDICATOR_H
#define LOADINGINDICATOR_H

#include <QTimer>
#include <QWidget>

#ifdef ENABLE_OPENGL
#include <QOpenGLWidget>
#endif

class QPropertyAnimation;

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
    void start();
    void stop();

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QWidget *m_parent;
    QSize m_size;
    int m_angle;
    int m_loadCount;

    QTimer m_timer;
};

#endif // LOADINGINDICATOR_H
