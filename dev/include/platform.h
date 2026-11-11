#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#if defined(_XBOX)
#include <xtl.h>
#elif defined(__linux__)

#include "linuxspecific.h"
#else
#error "platform.h: unsupported platform"
#endif

#endif
