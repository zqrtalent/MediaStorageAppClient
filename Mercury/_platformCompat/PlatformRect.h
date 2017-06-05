#ifdef __APPLE__
#include "PlatformRect_MacOSX.h"
#elif _WINDOWS
#include "PlatformRect_Win.h"
#else
#include "PlatformRect_UNIX.h"
#endif
