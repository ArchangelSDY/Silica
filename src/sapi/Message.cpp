#include "Message.h"

#include <QAtomicInt>
#include <QHash>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>

namespace sapi {
    
    namespace message {

        static QAtomicInt g_msgId;
        static QHash<QString, QVector<Listener> > g_listeners;
        static QMutex g_mutex;

        void sendMessage(const QString &topic, const QByteArray &payload)
        {
            int msgId = g_msgId.fetchAndAddOrdered(1);
            const QVector<Listener> &topicListeners = g_listeners[topic];
            for (const Listener &listener : topicListeners) {
                listener(msgId, payload);
            }
        }

        void addMessageListener(const QString &topic, Listener listener)
        {
            QMutexLocker locker(&g_mutex);
            g_listeners[topic] << listener;
        }

    }

}
