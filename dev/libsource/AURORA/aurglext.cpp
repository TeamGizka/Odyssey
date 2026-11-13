#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <string.h>
#include "aurora.h"
#include "aurrendergl.h"
#include "aurglext.h"

typedef void (WINAPI* PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (WINAPI* PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);

extern "C"
{
    PFNGLTEXIMAGE3DPROC glTexImage3D;
    PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
    PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB;
    PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;
    PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB;
    PFNGLMULTITEXCOORD3FVARBPROC glMultiTexCoord3fvARB;
    PFNGLMULTITEXCOORD4FVARBPROC glMultiTexCoord4fvARB;
    PFNGLISBUFFERARBPROC glIsBufferARB;
    PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB;
    PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB;
    PFNGLVERTEXATTRIB1SARBPROC glVertexAttrib1sARB;
    PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB;
    PFNGLVERTEXATTRIB1DARBPROC glVertexAttrib1dARB;
    PFNGLVERTEXATTRIB2SARBPROC glVertexAttrib2sARB;
    PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB;
    PFNGLVERTEXATTRIB2DARBPROC glVertexAttrib2dARB;
    PFNGLVERTEXATTRIB3SARBPROC glVertexAttrib3sARB;
    PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB;
    PFNGLVERTEXATTRIB3DARBPROC glVertexAttrib3dARB;
    PFNGLVERTEXATTRIB4SARBPROC glVertexAttrib4sARB;
    PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB;
    PFNGLVERTEXATTRIB4DARBPROC glVertexAttrib4dARB;
    PFNGLVERTEXATTRIB4NUBARBPROC glVertexAttrib4NubARB;
    PFNGLVERTEXATTRIB1SVARBPROC glVertexAttrib1svARB;
    PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB;
    PFNGLVERTEXATTRIB1DVARBPROC glVertexAttrib1dvARB;
    PFNGLVERTEXATTRIB2SVARBPROC glVertexAttrib2svARB;
    PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB;
    PFNGLVERTEXATTRIB2DVARBPROC glVertexAttrib2dvARB;
    PFNGLVERTEXATTRIB3SVARBPROC glVertexAttrib3svARB;
    PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB;
    PFNGLVERTEXATTRIB3DVARBPROC glVertexAttrib3dvARB;
    PFNGLVERTEXATTRIB4BVARBPROC glVertexAttrib4bvARB;
    PFNGLVERTEXATTRIB4SVARBPROC glVertexAttrib4svARB;
    PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB;
    PFNGLVERTEXATTRIB4UBVARBPROC glVertexAttrib4ubvARB;
    PFNGLVERTEXATTRIB4USVARBPROC glVertexAttrib4usvARB;
    PFNGLVERTEXATTRIB4UIVARBPROC glVertexAttrib4uivARB;
    PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB;
    PFNGLVERTEXATTRIB4DVARBPROC glVertexAttrib4dvARB;
    PFNGLVERTEXATTRIB4NBVARBPROC glVertexAttrib4NbvARB;
    PFNGLVERTEXATTRIB4NSVARBPROC glVertexAttrib4NsvARB;
    PFNGLVERTEXATTRIB4NIVARBPROC glVertexAttrib4NivARB;
    PFNGLVERTEXATTRIB4NUBVARBPROC glVertexAttrib4NubvARB;
    PFNGLVERTEXATTRIB4NUSVARBPROC glVertexAttrib4NusvARB;
    PFNGLVERTEXATTRIB4NUIVARBPROC glVertexAttrib4NuivARB;
    PFNGLPROGRAMENVPARAMETER4DARBPROC glProgramEnvParameter4dARB;
    PFNGLPROGRAMENVPARAMETER4DVARBPROC glProgramEnvParameter4dvARB;
    PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;
    PFNGLPROGRAMLOCALPARAMETER4DARBPROC glProgramLocalParameter4dARB;
    PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glProgramLocalParameter4dvARB;
    PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
    PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB;
    PFNGLGETPROGRAMENVPARAMETERDVARBPROC glGetProgramEnvParameterdvARB;
    PFNGLGETPROGRAMENVPARAMETERFVARBPROC glGetProgramEnvParameterfvARB;
    PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdvARB;
    PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfvARB;
    PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;
    PFNGLGETPROGRAMSTRINGARBPROC glGetProgramStringARB;
    PFNGLGETVERTEXATTRIBDVARBPROC glGetVertexAttribdvARB;
    PFNGLGETVERTEXATTRIBFVARBPROC glGetVertexAttribfvARB;
    PFNGLGETVERTEXATTRIBIVARBPROC glGetVertexAttribivARB;
    PFNGLGETVERTEXATTRIBPOINTERVARBPROC glGetVertexAttribPointervARB;
    PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV;
    PFNGLCOMBINERPARAMETERFNVPROC glCombinerParameterfNV;
    PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC glGetCombinerInputParameterfvNV;
    PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC glGetCombinerInputParameterivNV;
    PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV;
    PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV;
    PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
    PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;
    PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT;
    PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;
    PFNGLLOCKARRAYSEXTPROC glLockArraysEXT;
    PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;
    PFNGLSECONDARYCOLOR3FVEXTPROC glSecondaryColor3fvEXT;
    PFNGLEXECUTEPROGRAMNVPROC glExecuteProgramNV;
    PFNGLGETPROGRAMPARAMETERFVNVPROC glGetProgramParameterfvNV;
    PFNGLVERTEXATTRIB1FVNVPROC glVertexAttrib1fvNV;
    PFNGLVERTEXATTRIB2FVNVPROC glVertexAttrib2fvNV;
    PFNGLVERTEXATTRIB3FVNVPROC glVertexAttrib3fvNV;
    PFNGLVERTEXATTRIB4FVNVPROC glVertexAttrib4fvNV;
    PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
    PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;
    PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;
    PFNGLDELETEFENCESNVPROC glDeleteFencesNV;
    PFNGLGENFENCESNVPROC glGenFencesNV;
    PFNGLISFENCENVPROC glIsFenceNV;
    PFNGLTESTFENCENVPROC glTestFenceNV;
    PFNGLGETFENCEIVNVPROC glGetFenceivNV;
    PFNGLFINISHFENCENVPROC glFinishFenceNV;
    PFNGLSETFENCENVPROC glSetFenceNV;
    PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glGetCompressedTexImageARB;
    PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI;
    PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI;
    PFNWGLQUERYPBUFFERARBPROC wglQueryPBufferARB;
    PFNGLBEGINVERTEXSHADEREXTPROC glBeginVertexShaderEXT;
    PFNGLENDVERTEXSHADEREXTPROC glEndVertexShaderEXT;
    PFNGLBINDVERTEXSHADEREXTPROC glBindVertexShaderEXT;
    PFNGLGENVERTEXSHADERSEXTPROC glGenVertexShadersEXT;
    PFNGLDELETEVERTEXSHADERSEXTPROC glDeleteVertexShadersEXT;
    PFNGLSHADEROP1EXTPROC glShaderOp1EXT;
    PFNGLSHADEROP2EXTPROC glShaderOp2EXT;
    PFNGLSHADEROP3EXTPROC glShaderOp3EXT;
    PFNGLSWIZZLEEXTPROC glSwizzleEXT;
    PFNGLWRITEMASKEXTPROC glWriteMaskEXT;
    PFNGLINSERTCOMPONENTEXTPROC glInsertComponentEXT;
    PFNGLEXTRACTCOMPONENTEXTPROC glExtractComponentEXT;
    PFNGLGENSYMBOLSEXTPROC glGenSymbolsEXT;
    PFNGLSETINVARIANTEXTPROC glSetInvariantEXT;
    PFNGLSETLOCALCONSTANTEXTPROC glSetLocalConstantEXT;
    PFNGLVARIANTBVEXTPROC glVariantbvEXT;
    PFNGLVARIANTSVEXTPROC glVariantsvEXT;
    PFNGLVARIANTIVEXTPROC glVariantivEXT;
    PFNGLVARIANTFVEXTPROC glVariantfvEXT;
    PFNGLVARIANTDVEXTPROC glVariantdvEXT;
    PFNGLVARIANTUBEXTPROC glVariantubEXT;
    PFNGLVARIANTUSEXTPROC glVariantusEXT;
    PFNGLVARIANTUIEXTPROC glVariantuiEXT;
    PFNGLVARIANTPOINTEREXTPROC glVariantPointerEXT;
    PFNGLENABLEVARIANTCLIENTSTATEEXTPROC glEnableVariantClientStateEXT;
    PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC glDisableVariantClientStateEXT;
    PFNGLBINDLIGHTPARAMETEREXTPROC glBindLightParameterEXT;
    PFNGLBINDMATERIALPARAMETEREXTPROC glBindMaterialParameterEXT;
    PFNGLBINDTEXGENPARAMETEREXTPROC glBindTexGenParameterEXT;
    PFNGLBINDTEXTUREUNITPARAMETEREXTPROC glBindTextureUnitParameterEXT;
    PFNGLBINDPARAMETEREXTPROC glBindParameterEXT;
    PFNGLISVARIANTENABLEDEXTPROC glIsVariantEnabledEXT;
    PFNGLGETVARIANTBOOLEANVEXTPROC glGetVariantBooleanvEXT;
    PFNGLGETVARIANTINTEGERVEXTPROC glGetVariantIntegervEXT;
    PFNGLGETVARIANTFLOATVEXTPROC glGetVariantFloatvEXT;
    PFNGLGETVARIANTPOINTERVEXTPROC glGetVariantPointervEXT;
    PFNGLGETINVARIANTBOOLEANVEXTPROC glGetInvariantBooleanvEXT;
    PFNGLGETINVARIANTINTEGERVEXTPROC glGetInvariantIntegervEXT;
    PFNGLGETINVARIANTFLOATVEXTPROC glGetInvariantFloatvEXT;
    PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC glGetLocalConstantBooleanvEXT;
    PFNGLGETLOCALCONSTANTINTEGERVEXTPROC glGetLocalConstantIntegervEXT;
    PFNGLGETLOCALCONSTANTFLOATVEXTPROC glGetLocalConstantFloatvEXT;
    PFNWGLCREATEBUFFERREGIONARBPROC wglCreateBufferRegionARB;
    PFNWGLDELETEBUFFERREGIONARBPROC wglDeleteBufferRegionARB;
    PFNWGLSAVEBUFFERREGIONARBPROC wglSaveBufferRegionARB;
    PFNWGLRESTOREBUFFERREGIONARBPROC wglRestoreBufferRegionARB;
    PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;
    AurGlActiveTextureProc glActiveTextureARB;
    AurGlBindProgramProc glBindProgramARB;
    AurGlBindProgramProc glBindProgramNV;
    AurGlBindFragmentShaderProc glBindFragmentShaderATI;
    unsigned int(WINAPI* glGenFragmentShadersATI)(unsigned int);
    void(WINAPI* glGenProgramsARB)(int, unsigned int*);
    void(WINAPI* glDeleteProgramsARB)(int, const unsigned int*);
    AurGlActiveStencilFaceProc glActiveStencilFaceEXT;
    void(WINAPI* glBeginFragmentShaderATI)();
    void(WINAPI* glEndFragmentShaderATI)();
    void(WINAPI* glDeleteFragmentShaderATI)(unsigned int);
    void(WINAPI* glPassTexCoordATI)(unsigned int, unsigned int, unsigned int);
    void(WINAPI* glSampleMapATI)(unsigned int, unsigned int, unsigned int);
    AurGlSetFragmentShaderConstantProc glSetFragmentShaderConstantATI;
    AurGlActiveTextureProc glClientActiveTextureARB;
    AurGlMultiTexCoord2fProc glMultiTexCoord2fARB;
    AurGlProgramEnvParameter4fProc glProgramEnvParameter4fARB;
    AurGlProgramEnvParameter4fProc glProgramParameter4fNV;
    void(WINAPI* glProgramStringARB)(unsigned int, unsigned int, int,
                                     const void*);
    void(WINAPI* glGenProgramsNV)(int, unsigned int*);
    void(WINAPI* glDeleteProgramsNV)(int, const unsigned int*);
    unsigned char(WINAPI* glIsProgramARB)(unsigned int);
    unsigned char(WINAPI* glIsProgramNV)(unsigned int);
    void(WINAPI* glLoadProgramNV)(unsigned int, unsigned int, int,
                                  const unsigned char*);
    void(WINAPI* glTrackMatrixNV)(unsigned int, unsigned int, unsigned int,
                                  unsigned int);
    void(WINAPI* glVertexAttribPointerARB)(unsigned int, int, unsigned int,
                                           unsigned char, int, const void*);
    void(WINAPI* glVertexAttribPointerNV)(unsigned int, int, unsigned int,
                                          int, const void*);
    void(WINAPI* glEnableVertexAttribArrayARB)(unsigned int);
    void(WINAPI* glDisableVertexAttribArrayARB)(unsigned int);
    AurGlCombinerParameterfvProc glCombinerParameterfvNV;
    AurGlCombinerStageParameterfvProc glCombinerStageParameterfvNV;
    void(WINAPI* glCombinerParameteriNV)(unsigned int, int);
    void(WINAPI* glCombinerInputNV)(unsigned int, unsigned int, unsigned int,
                                    unsigned int, unsigned int, unsigned int);
    void(WINAPI* glCombinerOutputNV)(unsigned int, unsigned int, unsigned int,
                                     unsigned int, unsigned int, unsigned int,
                                     unsigned int, unsigned char, unsigned char,
                                     unsigned char);
    void(WINAPI* glFinalCombinerInputNV)(unsigned int, unsigned int,
                                         unsigned int, unsigned int);
    AurGlBindBufferProc glBindBufferARB;
    AurGlMapBufferProc glMapBufferARB;
    AurGlUnmapBufferProc glUnmapBufferARB;
    AurGlGetBufferParameterivProc glGetBufferParameterivARB;
    AurGlDeleteBuffersProc glDeleteBuffersARB;
    AurGlGenBuffersProc glGenBuffersARB;
    AurGlBufferDataProc glBufferDataARB;
    AurGlBufferSubDataProc glBufferSubDataARB;
    AurGlCompressedTexImage2DProc glCompressedTexImage2DARB;
    void(WINAPI* glColorFragmentOp1ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
    void(WINAPI* glAlphaFragmentOp1ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int);
    void(WINAPI* glColorFragmentOp2ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int);
    void(WINAPI* glAlphaFragmentOp2ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int);
    void(WINAPI* glColorFragmentOp3ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int);
    void(WINAPI* glAlphaFragmentOp3ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
    AurWglChoosePixelFormatProc wglChoosePixelFormatARB;
    AurWglGetPixelFormatAttribivProc wglGetPixelFormatAttribivARB;
    AurWglCreatePbufferProc wglCreatePBufferARB;
    AurWglGetPbufferDCProc wglGetPBufferDCARB;
    AurWglTexImageProc wglBindTexImageARB;
    AurWglTexImageProc wglReleaseTexImageARB;
    AurWglMakeContextCurrentProc wglMakeContextCurrentARB;
    int(WINAPI* wglReleasePBufferDCARB)(PBUFFERARB*, HDC);
    int(WINAPI* wglDestroyPBufferARB)(PBUFFERARB*);
    AurWglGetExtensionsStringProc wglGetExtensionsStringARB;
    AurWglSwapIntervalProc wglSwapIntervalEXT;

    PROC glUnusedExtension59;
    PROC glUnusedExtension132;
    PROC glUnusedExtension62;
}

int g_nGLExtUnused1 = 0;

int existingextensions = 0;

unsigned int EXT_BGRA_BIT = (1u << 0);
unsigned int ARB_MULTITEXTURE_BIT = (1u << 1);
unsigned int EXT_TEXTURE_ENV_COMBINE_BIT = (1u << 2);
unsigned int NV_REGISTER_COMBINERS_BIT = (1u << 3);
unsigned int EXT_TEXTURE_CUBE_MAP_BIT = (1u << 4);
unsigned int EXT_FOG_COORD_BIT = (1u << 5);
unsigned int EXT_SECONDARY_COLOR_BIT = (1u << 6);
unsigned int NV_VERTEX_PROGRAMS_BIT = (1u << 7);
unsigned int EXT_TEXTURE_FILTER_ANISOTROPIC_BIT = (1u << 8);

unsigned int EXT_IMAGING_SUBSET = (1u << 9);
unsigned int NV_PIXEL_SHADERS_BIT = (1u << 10);
unsigned int NV_VERTEX_ARRAY_RANGE_BIT = (1u << 11);
unsigned int NV_VERTEX_ARRAY_RANGE2_BIT = (1u << 12);
unsigned int NV_FENCE_BIT = (1u << 13);
unsigned int ARB_TEXTURE_COMPRESSION_BIT = (1u << 14);
unsigned int EXT_TEXTURE_COMPRESSION_S3TC_BIT = (1u << 15);
unsigned int ATIX_PN_TRIANGLES_BIT = (1u << 16);
unsigned int EXT_COMPILED_VERTEX_ARRAY_BIT = (1u << 17);
unsigned int WGL_ARB_RENDER_TO_TEXTURE_BIT = (1u << 18);
unsigned int EXT_SGIS_MIPMAP_BIT = (1u << 19);
unsigned int ATI_FRAGMENT_SHADER_BIT = (1u << 20);
unsigned int GL_ARB_VERTEX_PROGRAM_BIT = (1u << 21);
unsigned int GL_ARB_VERTEX_BUFFER_OBJECT_BIT = (1u << 22);
unsigned int NV_REGISTER_COMBINERS2_BIT = (1u << 23);
unsigned int GL_STENCIL_TWO_SIDED_BIT = (1u << 24);
unsigned int WGL_NV_RENDER_TO_TEXTURE_RECTANGLE_BIT = (1u << 25);

unsigned int WGL_ARB_BUFFER_REGION_BIT = (1u << 26);
unsigned int WGL_ARB_MULTISAMPLE_BIT = (1u << 27);
unsigned int EXT_SWAP_CONTROL_BIT = (1u << 28);
unsigned int GL_ARB_DEPTH_TEXTURE_BIT = (1u << 29);

unsigned int GL_ARB_FRAGMENT_PROGRAM_BIT = (1u << 6);
int useatifragmentshaders = 1;
int renderlevel = 2;
int g_nUnlockATISoftShadowsForHotFix = 0;

int g_nGLExtUnused2 = 0;

unsigned int lookup = 0;
int g_nGLExtUnused3 = 0;

int AurATIFrameBufferModificationsAvailable();
int AurATIFragmentShaderAvailable();
// PC 0x00475310 GOG 0x004754A0
int AurATIFragmentShaderAvailable()
{
    unsigned int nBits = ATI_FRAGMENT_SHADER_BIT;
    int returnval;

    if ((nBits & existingextensions) == nBits)
    {
        returnval = 1;
    }
    else
    {
        returnval = 0;
    }

    return returnval & useatifragmentshaders;
}
// PC 0x00475330 GOG 0x004754C0
GLboolean AuroraUseARBShaders()
{
    return (EXT_SWAP_CONTROL_BIT & existingextensions) == EXT_SWAP_CONTROL_BIT;
}
// PC 0x00475350 GOG 0x004754E0
GLboolean AurBumpMapCardValidate()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        returnval = AurATIFragmentShaderAvailable() == 0;
    }

    return returnval == 1;
}
// PC 0x00475390 GOG 0x00475520
GLboolean AurNonRadeon8500Validate()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        if (AurATIFragmentShaderAvailable() && !(GL_ARB_DEPTH_TEXTURE_BIT & existingextensions))
        {
            returnval = 0;
        }
        else
        {
            returnval = 1;
        }
    }

    return returnval == 1;
}
// PC 0x004753F0 GOG 0x00475580
GLboolean AurATIValidate()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        returnval = AurATIFragmentShaderAvailable() != 0;
    }

    return returnval == 1;
}
// PC 0x00475430 GOG 0x004755C0
int AurBumpyShinyAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        unsigned int nBits = EXT_BGRA_BIT |
                             ARB_MULTITEXTURE_BIT | NV_REGISTER_COMBINERS_BIT |
                             EXT_TEXTURE_CUBE_MAP_BIT |
                             NV_PIXEL_SHADERS_BIT | NV_VERTEX_PROGRAMS_BIT;

        returnval = (nBits & existingextensions) == nBits;

        if (returnval == 0)
        {
            unsigned int nATIBits =
                EXT_BGRA_BIT | ARB_MULTITEXTURE_BIT |
                EXT_TEXTURE_CUBE_MAP_BIT | ATI_FRAGMENT_SHADER_BIT |
                GL_ARB_VERTEX_PROGRAM_BIT;

            returnval = usearbvertexprograms & useatifragmentshaders &
                        ((nATIBits & existingextensions) == nATIBits);
        }
    }

    return returnval;
}
// PC 0x004754C0 GOG 0x00475650
int AurMultiTextureAvailable()
{
    static int returnval = -1;
    int nAvailable = returnval;

    if (returnval == -1)
    {
        nAvailable =
            (ARB_MULTITEXTURE_BIT & existingextensions) == ARB_MULTITEXTURE_BIT;
        returnval = nAvailable;
    }

    return nAvailable;
}
// PC 0x004754F0 GOG 0x00475680
int AurVertexProgramsNVAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        returnval = (NV_VERTEX_PROGRAMS_BIT & existingextensions) ==
                    NV_VERTEX_PROGRAMS_BIT;
    }

    return returnval;
}

int AurVertexProgramsARBAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        returnval = (GL_ARB_VERTEX_PROGRAM_BIT & existingextensions) ==
                    GL_ARB_VERTEX_PROGRAM_BIT;
    }

    return returnval;
}
// PC 0x00475520 GOG 0x004756B0
int AurVertexProgramsAvailable()
{
    int nAvailable = AurVertexProgramsARBAvailable();

    int nUse = usearbvertexprograms;

    return (nUse & nAvailable) != 0 && nUse != 0;
}
// PC 0x00475560 GOG 0x004756F0
int AurSoftShadowsAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        unsigned int nBits =
            WGL_NV_RENDER_TO_TEXTURE_RECTANGLE_BIT | NV_REGISTER_COMBINERS2_BIT;

        if ((nBits & existingextensions) == nBits)
        {
            returnval = 1;
        }
        else
        {
            returnval = 0;
        }
    }

    return returnval;
}
// PC 0x00475590 GOG 0x00475720
int AurATISoftShadowsAvailable()
{
    static int returnval = -1;

    if (g_nUnlockATISoftShadowsForHotFix == 0)
    {
        return 0;
    }

    if (returnval == -1)
    {
        unsigned int nBits = ATI_FRAGMENT_SHADER_BIT | WGL_ARB_RENDER_TO_TEXTURE_BIT;

        if ((nBits & existingextensions) == nBits)
        {
            returnval = 1;
        }
        else
        {
            returnval = 0;
        }
    }

    return returnval;
}
// PC 0x004755D0 GOG 0x00475760
int AurFrameBufferModificationsAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        unsigned int nBits = WGL_ARB_RENDER_TO_TEXTURE_BIT |
                             NV_REGISTER_COMBINERS2_BIT |
                             GL_ARB_VERTEX_PROGRAM_BIT;

        if ((nBits & existingextensions) == nBits)
        {
            returnval = 1;
        }
        else
        {
            returnval = 0;
        }
    }

    return returnval;
}
// PC 0x00475610 GOG 0x004757A0
int AurATIFrameBufferModificationsAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        unsigned int nBits = WGL_ARB_RENDER_TO_TEXTURE_BIT |
                             ATI_FRAGMENT_SHADER_BIT |
                             GL_ARB_VERTEX_PROGRAM_BIT;

        returnval = (nBits & existingextensions) == nBits;

        if (!AurNonRadeon8500Validate())
        {
            returnval = 0;
        }
    }

    return returnval;
}
// PC 0x00475660 GOG 0x004757F0
int AurCubeMapAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        unsigned int nBits = EXT_TEXTURE_CUBE_MAP_BIT | ARB_MULTITEXTURE_BIT;

        returnval = (nBits & existingextensions) == nBits;
    }

    return returnval;
}
// PC 0x00475690 GOG 0x00475820
int AurAnisotropicFilterAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        returnval = (EXT_TEXTURE_FILTER_ANISOTROPIC_BIT & existingextensions) ==
                    EXT_TEXTURE_FILTER_ANISOTROPIC_BIT;
    }

    return returnval;
}
// PC 0x004756C0 GOG 0x00475850
int AurSGIMipmapAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        unsigned int nBits = EXT_SGIS_MIPMAP_BIT;

        if (existingextensions & ATI_FRAGMENT_SHADER_BIT)
        {
            returnval = 0;
        }
        else
        {
            returnval = (nBits & existingextensions) == nBits;
        }
    }

    return returnval;
}
// PC 0x00475700 GOG 0x00475890
int AurTextureCompressionAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        unsigned int nBits =
            ARB_TEXTURE_COMPRESSION_BIT | EXT_TEXTURE_COMPRESSION_S3TC_BIT;

        returnval = (nBits & existingextensions) == nBits;
    }

    return returnval;
}
// PC 0x00475730 GOG 0x004758C0
bool CheckExtension(char* sExtension, unsigned char const* sExtensions)
{
    unsigned char const* pExtension = sExtensions;
    unsigned int nExtensionLength = strlen(sExtension);
    unsigned char const* pEnd = sExtensions + strlen((char const*)sExtensions);

    while (pExtension < pEnd)
    {
        int nTokenLength = strcspn((char const*)pExtension, " ");

        if (nExtensionLength == nTokenLength &&
            strncmp(sExtension, (char const*)pExtension, nTokenLength) == 0)
        {
            return true;
        }

        pExtension += nTokenLength + 1;
    }

    return false;
}
// PC 0x004757A0 GOG 0x00475930
int AurTwoSidedStencilAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        returnval = (GL_STENCIL_TWO_SIDED_BIT & existingextensions) != 0;
    }

    return returnval;
}
// PC 0x004757D0 GOG 0x00475960
bool AurCheckSoftShadowsAvailable()
{
    bool bResult = true;

    if (AurSoftShadowsAvailable() == 0)
    {
        if (AurATISoftShadowsAvailable() == 0)
        {
            bResult = false;
        }
    }

    return bResult;
}
// PC 0x00475860 GOG 0x004759F0
bool AurCheckFrameBufferModificationsAvailable()
{
    if (AurFrameBufferModificationsAvailable() ||
        AurATIFrameBufferModificationsAvailable())
    {
        return true;
    }

    return false;
}
// PC 0x004758C0 GOG 0x00475A50
int AurBumpMapAvailable()
{
    unsigned int nBits = EXT_BGRA_BIT | ARB_MULTITEXTURE_BIT |
                         NV_REGISTER_COMBINERS_BIT | EXT_TEXTURE_CUBE_MAP_BIT |
                         GL_ARB_VERTEX_PROGRAM_BIT;

    int returnval = (nBits & existingextensions) == nBits;

    if (returnval == 0)
    {
        if (AurVertexProgramsARBAvailable() & usearbvertexprograms)
        {
            unsigned int nATIBits = ATI_FRAGMENT_SHADER_BIT;

            if (((nATIBits & existingextensions) == nATIBits) &
                useatifragmentshaders)
            {
                return 1;
            }
        }

        return 0;
    }

    return returnval;
}
// PC 0x00475940 GOG 0x00475AD0
bool AurNV1xLevelShadersAvailable()
{
    if (renderlevel < 1)
    {
        return false;
    }

    if (AurATIFragmentShaderAvailable() && renderlevel == 1)
    {
        return false;
    }

    return AurBumpMapAvailable() ? true : false;
}
// PC 0x00475980 GOG 0x00475B10
bool AurNV2xLevelShadersAvailable()
{
    if (renderlevel < 2)
    {
        return false;
    }

    return AurBumpyShinyAvailable() ? true : false;
}
// PC 0x004759A0 GOG 0x00475B30
int AurOnePassEnvMapAvailable()
{
    static int returnval = -1;

    if (returnval == -1)
    {
        if (AurNV1xLevelShadersAvailable())
        {
            if (!AurATIFragmentShaderAvailable())
            {
                glGetIntegerv(queryTranslationTable[1], &returnval);
                returnval = returnval >= 2;
            }
            else
            {
                returnval = 1;
            }
        }
        else
        {
            returnval = 0;
        }
    }

    return returnval;
}
// PC 0x00475A40 GOG 0x00475BD0
int AurVertexBufferObjectARBAvailable()
{
    static int returnval = -1;

    if (g_bDisableVertexBufferObjects == true)
    {
        return 0;
    }

    if (returnval == -1)
    {
        returnval = (GL_ARB_VERTEX_BUFFER_OBJECT_BIT & existingextensions) ==
                    GL_ARB_VERTEX_BUFFER_OBJECT_BIT;

        if (!AurNV2xLevelShadersAvailable())
        {
            returnval = 0;
        }

        if (!AurNonRadeon8500Validate())
        {
            returnval = 0;
        }
    }

    return returnval;
}
// PC 0x00475AB0 GOG 0x00475C40
int AurGetAvailableStencilBits()
{
    static int stencilbits = -1;

    if (stencilbits == -1)
    {
        glGetIntegerv(GL_STENCIL_BITS, &stencilbits);
    }

    return stencilbits;
}
// PC 0x00475AD0 GOG 0x00475C60
int AurGetStencilClearValue()
{
    static int stencilclear = -1;
    int nStencilBits = AurGetAvailableStencilBits();

    if (nStencilBits == 0)
    {
        return 0;
    }

    if (stencilclear == -1)
    {
        stencilclear = (1 << (nStencilBits - 1)) - 1;
    }

    return stencilclear;
}
