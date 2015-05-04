#ifndef TASKPROGRESS_H
#define TASKPROGRESS_H

#include <QDateTime>
#include <QObject>

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
    void setMaximum(int max);
    void setMinimum(int min);
    void setValue(int val);
    void reset();

private:
    QString m_key;
    int m_maximum;
    int m_minimum;
    int m_value;
    QDateTime m_startTime;
    bool m_isRunning;
};

#endif // TASKPROGRESS_H
