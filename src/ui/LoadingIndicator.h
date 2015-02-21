#ifndef LOADINGINDICATOR_H
#define LOADINGINDICATOR_H

#include <QWidget>

class QPropertyAnimation;

class LoadingIndicator : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
public:
    explicit LoadingIndicator(const QSize &size, QWidget *parent = 0);

    qreal angle() const;
    void setAngle(qreal angle);

signals:
    void angleChanged(qreal angle);

public slots:
    void start();
    void stop();

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QWidget *m_parent;
    QSize m_size;
    qreal m_angle;
    int m_loadCount;

    QPropertyAnimation *m_aniSpin;
};

#endif // LOADINGINDICATOR_H
