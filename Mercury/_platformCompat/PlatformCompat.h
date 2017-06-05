//
//  PlatformCompat.h
//  TestApp
//
//  Created by Zaqro Butskhrikidze on 7/30/14.
//  Copyright (c) 2014 Zaqro Butskhrikidze. All rights reserved.
//

#ifdef __APPLE__
#include "PlatformCompat_MacOSX.h"
#elif _WINDOWS
#include "PlatformCompat_Win.h"
#else
#include "PlatformCompat_UNIX.h"
#endif
