#pragma once

#include <functional>

#include <QByteArray>
#include <QString>

#include "sapi_global.h"

namespace sapi {

    namespace message {

        void SAPI_EXPORT sendMessage(const QString &topic, const QByteArray &payload);

        using Listener = std::function<void(int, const QByteArray &)>;
        void SAPI_EXPORT addMessageListener(const QString &topic, Listener listener);

    } // namespace message

} // namespace sapi
