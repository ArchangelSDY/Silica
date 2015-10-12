#ifndef LOADINGINDICATORPRIVATE_H
#define LOADINGINDICATORPRIVATE_H

#include "sapi_global.h"
#include "LoadingIndicator.h"

namespace sapi {

// Set plugin loading indicator implemention.
// This function is NOT thread safe.
void SAPI_EXPORT setLoadingIndicatorImpl(LoadingIndicator *loadingIndicator);

}

#endif // LOADINGINDICATORPRIVATE_H
