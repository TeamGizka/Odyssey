#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include "aurrendergl.h"

#ifndef _XBOX
int envmapon = 0;
int tex1on = 0;

bool AurNV1xLevelShadersAvailable();
int AurATIFragmentShaderAvailable();
// PC 0x00485D70 GOG 0x00485E30
int AurIsAAliasingAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        returnval =
            (WGL_ARB_MULTISAMPLE_BIT & existingextensions) == WGL_ARB_MULTISAMPLE_BIT;
    }

    return returnval;
}

#endif

#ifndef _XBOX
int AurPixelShadersAvailable()  // PC 0x00485DA0 GOG 0x00485E60
{
    static int returnval = -1;

    if (returnval == -1)
    {
        if (AurNV1xLevelShadersAvailable() &&
            (AurATIFragmentShaderAvailable() ||
             (glGetIntegerv(queryTranslationTable[1], &returnval),
              returnval >= 8)))
        {
            returnval = 1;
            return 1;
        }
        returnval = 0;
        returnval = AurATIFragmentShaderAvailable();
    }

    return returnval;
}
#else

int AurPixelShadersAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        D3DCAPS8 caps;
        g_pD3DDevice->GetDeviceCaps(&caps);
        returnval = caps.PixelShaderVersion >= 1;
    }

    return returnval;
}
#endif
