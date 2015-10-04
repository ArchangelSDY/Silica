#ifndef TASKPROGRESS_H
#define TASKPROGRESS_H

#include <QDateTime>
#include <QObject>
#include <QTimer>

class TaskProgress : public QObject
{
    Q_OBJECT
public:
    explicit TaskProgress(QObject *parent = 0);

    QString key() const;
    void setKey(const QString &key);

    bool isRunning() const;

    int maximum() const;
    int minimum() const;
    int value() const;

signals:
    void changed();

public slots:
    void start();
    void stop();
    void reset();

    void setMaximum(int max);
    void setMinimum(int min);
    void setValue(int val);

    void setEstimateEnabled(bool enabled);
    void setEstimateInterval(int interval);

private slots:
    void estimateUpdate();

private:
    QString m_key;
    int m_maximum;
    int m_minimum;
    int m_value;
    bool m_isRunning;

    QDateTime m_startTime;
    qint64 m_lastTimeConsumption;
    bool m_enableEstimate;
    QTimer m_estimateTimer;
};

#endif // TASKPROGRESS_H
