#pragma once

#include <QTimer>

class GamepadAxisScroller : public QObject
{
    Q_OBJECT
public:
    static const int SCROLL_INTERVAL;
    static const int MAX_SCROLL_PIXELS_PER_INTERVAL;

    GamepadAxisScroller();

signals:
    void scroll(int dx, int dy);

public slots:
    void setAxisXValue(double x);
    void setAxisYValue(double y);

private slots:
    void timeout();

private:
    void checkTimer();

    QTimer m_timer;
    double m_axisXValue;
    double m_axisYValue;
};
