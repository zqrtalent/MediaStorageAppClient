#ifdef __APPLE__
#include "PlatformCriticalSection_MacOSX.h"
#elif _WINDOWS
#include "PlatformCriticalSection_Win.h"
#else
#include "PlatformCriticalSection_UNIX.h"
#endif
