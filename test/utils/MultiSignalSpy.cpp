#include <QSignalSpy>
#include <QTime>

#include "MultiSignalSpy.h"

MultiSignalSpy::MultiSignalSpy(const QObject *sender, const char *signal) :
    m_sender(sender) ,
    m_signal(signal)
{
}

bool MultiSignalSpy::wait(int targetCount, int timeout)
{
    QTime time;
    time.start();

    int curCount = 0;
    while (curCount < targetCount && time.elapsed() < timeout) {
        QSignalSpy spy(m_sender, m_signal);
        if (spy.wait(50)) {
            curCount++;
        }
    }

    return curCount >= targetCount;
}
