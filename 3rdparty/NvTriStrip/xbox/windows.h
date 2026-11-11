/* Xbox build of the unmodified NVIDIA sources (not game code): NvTriStripObjects.h includes <windows.h>, which the XDK
   does not ship; its Xbox equivalent is <xtl.h>. Searched by the NvTriStrip target only, in the xbox configuration.
   BioWare's own tree selects its system header through dev/include/platform.h (docs/PLATFORMS.md). */
#pragma once
#include <xtl.h>
