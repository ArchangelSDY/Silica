#ifndef SAPI_GLOBAL_H
#define SAPI_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(SAPI_LIBRARY)
#define SAPI_EXPORT Q_DECL_EXPORT
#else
#define SAPI_EXPORT Q_DECL_IMPORT
#endif

#endif // SAPI_GLOBAL_H

