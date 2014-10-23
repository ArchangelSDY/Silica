#ifndef MULTISIGNALSPY_H
#define MULTISIGNALSPY_H

#include <QObject>

class MultiSignalSpy : public QObject
{
    Q_OBJECT
public:
    MultiSignalSpy(const QObject *sender, const char *signal);

    bool wait(int count, int timeout=5000);

private:
    const QObject *m_sender;
    const char *m_signal;
};

#endif // MULTISIGNALSPY_H
