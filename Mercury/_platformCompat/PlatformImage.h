#ifdef _SKIA
#include "PlatformImage_Skia.h"
#else
#ifdef __APPLE__
#include "PlatformImage_MacOSX.h"
#elif _WINDOWS
#include "PlatformImage_Win.h"
#else
#include "PlatformImage_UNIX.h"
#endif
#endif

