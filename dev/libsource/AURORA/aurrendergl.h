#ifndef AURRENDERGL_H
#define AURRENDERGL_H

enum
{
    GL_TEXTURE0_ARB = 0x84C0,
    GL_TEXTURE1_ARB = 0x84C1,
    GL_TEXTURE2_ARB = 0x84C2
};

extern int usearbvertexprograms;
extern int usevertexprograms;

int AurMultiTextureAvailable();
int AurBumpMapAvailable();
int AurVertexProgramsAvailable();
int AurPixelShadersAvailable();

enum { GL_MULTISAMPLE_ARB = 0x809d };

extern "C" void(__stdcall* glClientActiveTextureARB)(unsigned int nTexture);
extern "C" void(__stdcall* glVertexAttribPointerARB)(unsigned int nIndex,
                                                 int nSize, unsigned int nType,
                                                 unsigned char bNormalized,
                                                 int nStride,
                                                 const void* pPointer);
extern "C" void(__stdcall* glVertexAttribPointerNV)(unsigned int nIndex, int nSize,
                                                unsigned int nType,
                                                int nStride,
                                                const void* pPointer);

#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif
#include <GL/gl.h>
#include <aurlist.h>

#define AUR_SPHERE_BOUND_DIAGONAL_FACTOR 0.717f

#define AUR_FRAMEBUFFER_CHANNEL_MAX 255.0f
#define AUR_FRAMEBUFFER_LOOKUP_GAIN 0.7f

#define AUR_FRAMEBUFFER_OVERBRIGHTEN_THRESHOLD 0.5
#define AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE 0.5f
#define AUR_FRAMEBUFFER_QUADRATIC_GAIN 4.0f
#include "../libsource/math3d/math3d.h"

typedef void(WINAPI* AurGlActiveTextureStageProc)(unsigned int);
extern "C" AurGlActiveTextureStageProc glActiveTextureARB;
extern "C" AurGlActiveTextureStageProc glEnableVertexAttribArrayARB;
extern "C" AurGlActiveTextureStageProc glDisableVertexAttribArrayARB;
extern "C" AurGlActiveTextureStageProc glEnableVertexAttribArrayARB;
extern int usearbvertexprograms;
int AurMultiTextureAvailable();

class Vector;
class Quaternion;
class CAurTexture;
#ifdef _XBOX

enum AuroraModelShader
{

    MODEL_SHADER_BUMP_RESET = 11,
    MODEL_SHADER_EMLM = 19,
    MODEL_SHADER_EMLM_DISABLE = 33,

    MODEL_SHADER_NV1X_LIGHTMAP = 22,

    MODEL_SHADER_NV1X_DISABLE = 28,

    MODEL_SHADER_DISABLE = 0,
    MODEL_SHADER_TEXTURE_FOG = 1,
    MODEL_SHADER_FOG = 2,

    MODEL_SHADER_ATTENUATED_NORMAL = 3,
    MODEL_SHADER_DIFFUSE_BUMP = 4,
    MODEL_SHADER_BUMP = 5,

    MODEL_SHADER_BUMP_LIGHTMAP = 6,
    MODEL_SHADER_NORMAL_LIGHTMAP = 7,
    MODEL_SHADER_BUMP_ENVIRONMENT = 8,
    MODEL_SHADER_BUMP_ENVIRONMENT_LIGHTMAP = 9,
    MODEL_SHADER_BUMP_DISABLE = 11,
    MODEL_SHADER_DIFFUSE_BUMP_FOG = 12,
    MODEL_SHADER_DIFFUSE_BUMP_FOG_ALPHA = 13,
    MODEL_SHADER_SPECULAR = 14,
    MODEL_SHADER_SPECULAR_SQUARED = 15,
    MODEL_SHADER_SPECULAR_FOG = 16,
    MODEL_SHADER_ENVIRONMENT_FOG = 17,

    MODEL_SHADER_ENVIRONMENT_ONLY = 20,
    MODEL_SHADER_BUMP_MODULATE = 21,
    MODEL_SHADER_LIGHTMAP_ONLY = 23,
    MODEL_SHADER_TEXTURE_SHADER_DISABLE = 24,
    MODEL_SHADER_TEXTURE_SHADER_BUMP = 25,
    MODEL_SHADER_TEXTURE_SHADER_LOOKUP = 26,
    MODEL_SHADER_PRIMARY_COLOR = 27,
    MODEL_SHADER_TEXTURE_COLOR_FOG = 30,

    MODEL_SHADER_SKIN_BLUR = 30,

    MODEL_SHADER_ENVIRONMENT = 17,
    MODEL_SHADER_ATI_ENVIRONMENT_FOG = 18,

    MODEL_SHADER_BUMPY_SHINY_ENABLE = 20,
    MODEL_SHADER_BUMPY_SHINY_DISABLE = 24,
    MODEL_SHADER_BUMPY_SHINY = 25,

    MODEL_SHADER_BUMP_DIFFUSE = 13,

    MODEL_SHADER_BUMP_SPECULAR = 16,

    MODEL_SHADER_BUMP_MAPPED = 5,

    MODEL_SHADER_LIGHTMAPPED_BUMP_DIFFUSE = 6,

    MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR1 = 7,
    MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR2 = 8,
    MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR4 = 9,
    MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR8 = 10
};
#else

enum AuroraModelShader
{

    MODEL_SHADER_NORMAL_MAP = 7,
    MODEL_SHADER_BUMP_RESET = 13,
    MODEL_SHADER_EMLM = 22,
    MODEL_SHADER_EMLM_DISABLE = 37,

    MODEL_SHADER_NV1X_LIGHTMAP = 26,
    MODEL_SHADER_NV1X_ENVIRONMENT = 21,
    MODEL_SHADER_NV1X_DISABLE = 32,

    MODEL_SHADER_DISABLE = 0,
    MODEL_SHADER_TEXTURE_FOG = 1,
    MODEL_SHADER_FOG = 2,

    MODEL_SHADER_STANDARD_FOG = 3,
    MODEL_SHADER_ATTENUATED_NORMAL = 4,
    MODEL_SHADER_DIFFUSE_BUMP = 5,
    MODEL_SHADER_BUMP = 6,
    MODEL_SHADER_NORMAL = 7,
    MODEL_SHADER_BUMP_LIGHTMAP = 8,
    MODEL_SHADER_NORMAL_LIGHTMAP = 9,
    MODEL_SHADER_BUMP_ENVIRONMENT = 10,
    MODEL_SHADER_BUMP_ENVIRONMENT_LIGHTMAP = 11,
    MODEL_SHADER_BUMP_DISABLE = 13,
    MODEL_SHADER_DIFFUSE_BUMP_FOG = 14,
    MODEL_SHADER_DIFFUSE_BUMP_FOG_ALPHA = 15,
    MODEL_SHADER_SPECULAR = 16,
    MODEL_SHADER_SPECULAR_SQUARED = 17,
    MODEL_SHADER_SPECULAR_FOG = 18,
    MODEL_SHADER_ENVIRONMENT_FOG = 19,
    MODEL_SHADER_MODULATE = 23,
    MODEL_SHADER_ENVIRONMENT_ONLY = 24,
    MODEL_SHADER_BUMP_MODULATE = 25,
    MODEL_SHADER_LIGHTMAP_ONLY = 27,
    MODEL_SHADER_TEXTURE_SHADER_DISABLE = 28,
    MODEL_SHADER_TEXTURE_SHADER_BUMP = 29,
    MODEL_SHADER_TEXTURE_SHADER_LOOKUP = 30,
    MODEL_SHADER_PRIMARY_COLOR = 31,
    MODEL_SHADER_TEXTURE_COLOR_FOG = 34,

    MODEL_SHADER_SKIN_BLUR = 34,

    MODEL_SHADER_ENVIRONMENT = 19,
    MODEL_SHADER_ATI_ENVIRONMENT_FOG = 20,

    MODEL_SHADER_BUMPY_SHINY_ENABLE = 24,
    MODEL_SHADER_BUMPY_SHINY_DISABLE = 28,
    MODEL_SHADER_BUMPY_SHINY = 29,

    MODEL_SHADER_BUMP_DIFFUSE = 15,

    MODEL_SHADER_BUMP_SPECULAR = 18,

    MODEL_SHADER_BUMP_MAPPED = 6,

    MODEL_SHADER_LIGHTMAPPED_BUMP_DIFFUSE = 8,

    MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR1 = 9,
    MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR2 = 10,
    MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR4 = 11,
    MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR8 = 12,

    MODEL_SHADER_NOISE_SUBTRACT = 38,
    MODEL_SHADER_NOISE_ADD = 39,
    MODEL_SHADER_NOISE_MODULATE = 40,
    MODEL_SHADER_NOISE_EM_SUBTRACT = 41,
    MODEL_SHADER_NOISE_EM_ADD = 42,
    MODEL_SHADER_NOISE_EM_MODULATE = 43,
    MODEL_SHADER_NOISE_LM_SUBTRACT = 44,
    MODEL_SHADER_NOISE_LM_ADD = 45,
    MODEL_SHADER_NOISE_LM_MODULATE = 46,
    MODEL_SHADER_NOISE_EMLM_SUBTRACT = 47,
    MODEL_SHADER_NOISE_EMLM_ADD = 48,
    MODEL_SHADER_NOISE_EMLM_MODULATE = 49,
    MODEL_SHADER_FGLOW = 50,
    MODEL_SHADER_FGLOW_LM = 51,
    MODEL_SHADER_FGLOW_EM = 52,
    MODEL_SHADER_FGLOW_EMLM = 53,
    MODEL_SHADER_FGLOW_BUMPMAP = 54,
    MODEL_SHADER_FGLOW_BUMPY_SHINY = 55,
    MODEL_SHADER_DESAT = 56,
    MODEL_SHADER_DESAT_LM = 57,
    MODEL_SHADER_DESAT_EM = 58,
    MODEL_SHADER_DESAT_EMLM = 59,
    MODEL_SHADER_DESAT_BUMPMAP = 60,
    MODEL_SHADER_DESAT_BUMPY_SHINY = 61,
    MODEL_SHADER_DESAT_NOISE_SUB = 62,
    MODEL_SHADER_DESAT_NOISE_ADD = 63,
    MODEL_SHADER_DESAT_NOISE_MOD = 64,
    MODEL_SHADER_DESAT_NOISE_SUB_EM = 65,
    MODEL_SHADER_DESAT_NOISE_ADD_EM = 66,
    MODEL_SHADER_DESAT_NOISE_MOD_EM = 67,
    MODEL_SHADER_DESAT_NOISE_SUB_LM = 68,
    MODEL_SHADER_DESAT_NOISE_ADD_LM = 69,
    MODEL_SHADER_DESAT_NOISE_MOD_LM = 70,
    MODEL_SHADER_DESAT_NOISE_SUB_EMLM = 71,
    MODEL_SHADER_DESAT_NOISE_ADD_EMLM = 72,
    MODEL_SHADER_DESAT_NOISE_MOD_EMLM = 73,
    MODEL_SHADER_NOISE_DISABLE = 74,
    MODEL_SHADER_HOLO = 75,
    MODEL_SHADER_HOLO_DESAT = 76,
    MODEL_SHADER_HOLO_DISABLE = 77,
    MODEL_SHADER_ILLUM = 78,
    MODEL_SHADER_ILLUM_ENV = 79,
    MODEL_SHADER_ILLUM_NOISE_SUBTRACT = 80,
    MODEL_SHADER_ILLUM_NOISE_ADD = 81,
    MODEL_SHADER_ILLUM_NOISE_MODULATE = 82,
    MODEL_SHADER_ILLUM_NOISE_EM_SUBTRACT = 83,
    MODEL_SHADER_ILLUM_NOISE_EM_ADD = 84,
    MODEL_SHADER_ILLUM_NOISE_EM_MODULATE = 85,
    MODEL_SHADER_ILLUM_NOISE_LM_SUBTRACT = 86,
    MODEL_SHADER_ILLUM_NOISE_LM_ADD = 87,
    MODEL_SHADER_ILLUM_NOISE_LM_MODULATE = 88,
    MODEL_SHADER_ILLUM_NOISE_EMLM_SUBTRACT = 89,
    MODEL_SHADER_ILLUM_NOISE_EMLM_ADD = 90,
    MODEL_SHADER_ILLUM_NOISE_EMLM_MODULATE = 91,
    MODEL_SHADER_ILLUM_DESAT = 92,
    MODEL_SHADER_ILLUM_DESAT_LM = 93,
    MODEL_SHADER_ILLUM_DESAT_EM = 94,
    MODEL_SHADER_ILLUM_DESAT_EMLM = 95,
    MODEL_SHADER_ILLUM_DESAT_BUMPMAP = 96,
    MODEL_SHADER_ILLUM_DESAT_BUMPY_SHINY = 97,
    MODEL_SHADER_ILLUM_DESAT_NOISE_SUB = 98,
    MODEL_SHADER_ILLUM_DESAT_NOISE_ADD = 99,
    MODEL_SHADER_ILLUM_DESAT_NOISE_MOD = 100,
    MODEL_SHADER_ILLUM_DESAT_NOISE_SUB_EM = 101,
    MODEL_SHADER_ILLUM_DESAT_NOISE_ADD_EM = 102,
    MODEL_SHADER_ILLUM_DESAT_NOISE_MOD_EM = 103,
    MODEL_SHADER_ILLUM_DESAT_NOISE_SUB_LM = 104,
    MODEL_SHADER_ILLUM_DESAT_NOISE_ADD_LM = 105,
    MODEL_SHADER_ILLUM_DESAT_NOISE_MOD_LM = 106,
    MODEL_SHADER_ILLUM_DESAT_NOISE_SUB_EMLM = 107,
    MODEL_SHADER_ILLUM_DESAT_NOISE_ADD_EMLM = 108,
    MODEL_SHADER_ILLUM_DESAT_NOISE_MOD_EMLM = 109
};
#endif
class MdlNodeTriMesh;
class MaxFace;

struct HPBUFFERARB__;
typedef HPBUFFERARB__ PBUFFERARB;
template <class T> class List;

struct RenderVertexV
{
    Vector position;
};

struct RenderVertexN
{
    Vector normal;
};
struct RenderVertexC
{
    unsigned char color[4];
};

struct RenderVertexVT
{
    float position[3];
    float u;
    float v;
};

struct RenderVertexVTC
{
    float position[3];
    unsigned char color[4];
    float u;
    float v;
};

namespace Aurora
{

enum AuroraBufferRequestType { POOL_VERTEX = 0, POOL_INDEX = 1 };

enum AuroraStreamMode
{
    STREAM_POSITION = 0,
    STREAM_TEXCOORD = 1,
    STREAM_COLOR = 2,
    STREAM_NORMAL = 3
};

enum AuroraPrimitiveTypes
{
    PRIMITIVE_POINTS = 0,
    PRIMITIVE_LINES = 1,
    PRIMITIVE_LINE_STRIP = 2,
    PRIMITIVE_TRIANGLES = 3,
    PRIMITIVE_TRIANGLE_STRIP = 4,
    PRIMITIVE_TRIANGLE_FAN = 5,
    PRIMITIVE_QUADS = 6,
    PRIMITIVE_QUAD_STRIP = 7,
    PRIMITIVE_POLYGON = 8,
    PRIMITIVE_LINE_LOOP = 9
};

enum AuroraTexCoordMappingModes
{
    TEXCOORD_MAPPING_SPHERE = 0,
    TEXCOORD_MAPPING_REFLECTION = 1
};
}

struct RenderVertexVC
{
    float position[3];
    unsigned char color[4];
};

typedef int(WINAPI* AurWglSwapIntervalProc)(int nInterval);
typedef int(WINAPI* AurWglChoosePixelFormatProc)(
    HDC hDC, const int* pnAttributes, const float* pfAttributes,
    unsigned int nMaxFormats, int* pnPixelFormat, unsigned int* pnNumFormats);

typedef int(WINAPI* AurWglGetPixelFormatAttribivProc)(
    HDC hDC, int nPixelFormat, int nLayerPlane, unsigned int nAttributes,
    const int* pnAttributes, int* pnValues);

enum
{
    WGL_DRAW_TO_WINDOW_ARB = 0x2001,
    WGL_ACCELERATION_ARB = 0x2003,
    WGL_DOUBLE_BUFFER_ARB = 0x2011,
    WGL_COLOR_BITS_ARB = 0x2014,
    WGL_ALPHA_BITS_ARB = 0x201b,
    WGL_DEPTH_BITS_ARB = 0x2022,
    WGL_STENCIL_BITS_ARB = 0x2023,
    WGL_FULL_ACCELERATION_ARB = 0x2027,
    WGL_SUPPORT_OPENGL_ARB = 0x2010,
    WGL_DRAW_TO_PBUFFER_ARB = 0x202d,
    WGL_PBUFFER_LARGEST_ARB = 0x2033,
    WGL_FRONT_LEFT_ARB = 0x2083,
    WGL_SAMPLE_BUFFERS_ARB = 0x2041,
    WGL_SAMPLES_ARB = 0x2042
};
typedef PBUFFERARB*(WINAPI* AurWglCreatePbufferProc)(HDC hDC, int nPixelFormat,
                                                     int nWidth, int nHeight,
                                                     const int* pnAttributes);
typedef HDC(WINAPI* AurWglGetPbufferDCProc)(PBUFFERARB* pBuffer);
typedef BOOL(WINAPI* AurWglTexImageProc)(PBUFFERARB* pBuffer, int nBuffer);
typedef BOOL(WINAPI* AurWglMakeContextCurrentProc)(HDC hDrawDC, HDC hReadDC,
                                                 HGLRC hContext);
typedef void(WINAPI* AurGlCombinerParameterfvProc)(unsigned int nPName,
                                                   float const* pfParams);
typedef void(WINAPI* AurGlCombinerStageParameterfvProc)(unsigned int nStage,
    unsigned int nPName, const float* pfParams);
extern "C" AurGlCombinerStageParameterfvProc glCombinerStageParameterfvNV;
typedef void(WINAPI* AurGlBindBufferProc)(unsigned int nTarget,
                                          unsigned int nBuffer);
typedef void*(WINAPI* AurGlMapBufferProc)(unsigned int nTarget,
                                          unsigned int nAccess);
typedef int(WINAPI* AurGlUnmapBufferProc)(unsigned int nTarget);
typedef void(WINAPI* AurGlGetBufferParameterivProc)(unsigned int nTarget,
                                                    unsigned int nPName,
                                                    int* pnParams);
typedef void(WINAPI* AurGlDeleteBuffersProc)(int nCount,
                                           const unsigned int* pnBuffers);
typedef void(WINAPI* AurGlGenBuffersProc)(int nCount, unsigned int* pnBuffers);
typedef void(WINAPI* AurGlBufferDataProc)(unsigned int nTarget, int nSize,
                                        const void* pData, unsigned int nUsage);
typedef void(WINAPI* AurGlBufferSubDataProc)(unsigned int nTarget, int nOffset,
                                           int nSize, const void* pData);

extern "C" AurWglSwapIntervalProc wglSwapIntervalEXT;
extern "C" AurWglChoosePixelFormatProc wglChoosePixelFormatARB;
extern "C" AurWglGetPixelFormatAttribivProc wglGetPixelFormatAttribivARB;
extern "C" AurWglGetPbufferDCProc wglGetPBufferDCARB;
extern "C" AurWglTexImageProc wglBindTexImageARB;
extern "C" AurGlCombinerParameterfvProc glCombinerParameterfvNV;
extern "C" AurGlBindBufferProc glBindBufferARB;
extern "C" AurGlMapBufferProc glMapBufferARB;
extern "C" AurGlUnmapBufferProc glUnmapBufferARB;
extern "C" AurGlGetBufferParameterivProc glGetBufferParameterivARB;
extern "C" AurGlDeleteBuffersProc glDeleteBuffersARB;
extern "C" AurGlGenBuffersProc glGenBuffersARB;
extern "C" AurGlBufferDataProc glBufferDataARB;
extern "C" AurGlBufferSubDataProc glBufferSubDataARB;

typedef void(WINAPI* AurGlCompressedTexImage2DProc)(
    unsigned int nTarget, int nLevel, unsigned int nInternalFormat,
    int nWidth, int nHeight, int nBorder, int nImageSize, const void* pData);

#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_ELEMENT_ARRAY_BUFFER_ARB 0x8893
#define GL_READ_ONLY_ARB 0x88B8
#define GL_WRITE_ONLY_ARB 0x88B9
#define GL_READ_WRITE_ARB 0x88BA
#define GL_BUFFER_SIZE_ARB 0x8764
#define GL_DYNAMIC_DRAW_ARB 0x88E8

#define GL_TEXTURE0_ARB 0x84C0
#define GL_TEXTURE1_ARB 0x84C1
#define GL_TEXTURE2_ARB 0x84C2
#define GL_TEXTURE3_ARB 0x84C3
#define GL_TEXTURE4_ARB 0x84C4
#define GL_VERTEX_PROGRAM_ARB 0x8620
#define GL_VERTEX_PROGRAM_NV 0x8620
#define GL_REFLECTION_MAP_ARB 0x8512

void AurSetVertexProgramParameter4f(unsigned int nIndex,
                                   float x, float y, float z, float w);
typedef void(WINAPI* AurGlProgramEnvParameter4fProc)(unsigned int nTarget,
    unsigned int nIndex, float x, float y, float z, float w);
extern "C" AurGlProgramEnvParameter4fProc glProgramEnvParameter4fARB;
extern "C" AurGlProgramEnvParameter4fProc glProgramParameter4fNV;
#define GL_FRAGMENT_SHADER_ATI 0x8920
#define GL_CON_0_ATI 0x8941
#define GL_CON_1_ATI 0x8942

#define GL_CON_2_ATI 0x8943
#define GL_CON_3_ATI 0x8944
#define GL_CON_4_ATI 0x8945
#define GL_CON_5_ATI 0x8946
#define GL_MOV_ATI 0x8961
#define GL_ADD_ATI 0x8963
#define GL_DOT3_ATI 0x8966
#define GL_RED_BIT_ATI 0x1
#define GL_GREEN_BIT_ATI 0x2
#define GL_2X_BIT_ATI 0x1
#define GL_COMP_BIT_ATI 0x2
#define GL_BIAS_BIT_ATI 0x8
#define GL_PRIMARY_COLOR_ARB 0x8577
#define GL_PROGRAM_FORMAT_ASCII_ARB 0x8875
#define GL_REG_0_ATI 0x8921
#define GL_REG_1_ATI 0x8922
#define GL_REG_2_ATI 0x8923
#define GL_REG_3_ATI 0x8924
#define GL_REG_4_ATI 0x8925
#define GL_REG_5_ATI 0x8926
#define GL_MUL_ATI 0x8964
#define GL_MAD_ATI 0x8968
#define GL_SWIZZLE_STR_ATI 0x8976
#define GL_SECONDARY_INTERPOLATOR_ATI 0x896D

extern "C" unsigned int(WINAPI* glGenFragmentShadersATI)(unsigned int);
extern "C" void(WINAPI* glEndFragmentShaderATI)();
extern "C" void(WINAPI* glPassTexCoordATI)(unsigned int, unsigned int, unsigned int);
extern "C" void(WINAPI* glColorFragmentOp1ATI)(unsigned int, unsigned int,
    unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
extern "C" void(WINAPI* glGenProgramsARB)(int, unsigned int*);
extern "C" void(WINAPI* glDeleteFragmentShaderATI)(unsigned int);
extern "C" int(WINAPI* wglReleasePBufferDCARB)(PBUFFERARB*, HDC);
extern "C" int(WINAPI* wglDestroyPBufferARB)(PBUFFERARB*);
extern "C" void(WINAPI* glProgramStringARB)(unsigned int, unsigned int, int, const void*);
extern "C" void(WINAPI* glSampleMapATI)(unsigned int, unsigned int, unsigned int);
extern "C" void(WINAPI* glColorFragmentOp2ATI)(unsigned int, unsigned int,
    unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
    unsigned int, unsigned int, unsigned int);
extern "C" void(WINAPI* glAlphaFragmentOp2ATI)(unsigned int, unsigned int,
    unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
    unsigned int, unsigned int);
extern "C" void(WINAPI* glColorFragmentOp3ATI)(unsigned int, unsigned int,
    unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
    unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
    unsigned int);
extern "C" void(WINAPI* glAlphaFragmentOp3ATI)(unsigned int, unsigned int,
    unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
    unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
#define GL_TEXTURE_RECTANGLE_NV 0x84F5

#define GL_CONSTANT_COLOR0_NV 0x852A
#define GL_CONSTANT_COLOR1_NV 0x852B
#define GL_COMBINER0_NV 0x8550
#define GL_COMBINER1_NV 0x8551
#define GL_COMBINER2_NV 0x8552
#define GL_PER_STAGE_CONSTANTS_NV 0x8535

#define GL_REGISTER_COMBINERS_NV 0x8522
#define GL_VARIABLE_A_NV 0x8523
#define GL_VARIABLE_B_NV 0x8524
#define GL_VARIABLE_C_NV 0x8525
#define GL_VARIABLE_D_NV 0x8526
#define GL_VARIABLE_G_NV 0x8529
#define GL_SPARE0_NV 0x852E
#define GL_SPARE1_NV 0x852F
#define GL_DISCARD_NV 0x8530
#define GL_UNSIGNED_IDENTITY_NV 0x8536
#define GL_UNSIGNED_INVERT_NV 0x8537
#define GL_EXPAND_NORMAL_NV 0x8538
#define GL_NUM_GENERAL_COMBINERS_NV 0x854E
#define GL_COMBINER3_NV 0x8553
#define GL_VARIABLE_E_NV 0x8527
#define GL_VARIABLE_F_NV 0x8528
#define GL_PRIMARY_COLOR_NV 0x852C
#define GL_SECONDARY_COLOR_NV 0x852D
#define GL_E_TIMES_F_NV 0x8531
#define GL_SIGNED_IDENTITY_NV 0x853C
#define GL_SIGNED_NEGATE_NV 0x853D
#define GL_SCALE_BY_FOUR_NV 0x853F
#define GL_COMBINER4_NV 0x8554
#define GL_COMBINER5_NV 0x8555
#define GL_COMBINER6_NV 0x8556
#define GL_DOT_PRODUCT_TEXTURE_2D_NV 0x86EE
#define GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV 0x86F2
#define GL_REFLECTION_MAP_ARB 0x8512
#define GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV 0x864E
#define GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV 0x86D9
#define GL_TEXTURE_SHADER_NV 0x86DE
#define GL_SHADER_OPERATION_NV 0x86DF
#define GL_PREVIOUS_TEXTURE_INPUT_NV 0x86E4
#define GL_DEPENDENT_AR_TEXTURE_2D_NV 0x86E9
#define GL_DEPENDENT_GB_TEXTURE_2D_NV 0x86EA
#define GL_DOT_PRODUCT_NV 0x86EC
extern "C" void(WINAPI* glCombinerParameteriNV)(unsigned int, int);
extern "C" void(WINAPI* glCombinerInputNV)(unsigned int, unsigned int,
    unsigned int, unsigned int, unsigned int, unsigned int);
extern "C" void(WINAPI* glCombinerOutputNV)(unsigned int, unsigned int,
    unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
    unsigned char, unsigned char, unsigned char);
extern "C" void(WINAPI* glFinalCombinerInputNV)(unsigned int, unsigned int,
    unsigned int, unsigned int);

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE

#define GL_BGRA_EXT 0x80E1

#define GL_REFLECTION_MAP_ARB 0x8512
#define GL_TEXTURE_CUBE_MAP_ARB 0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x851A

#define GL_COMPRESSED_LUMINANCE_ARB 0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB 0x84EB
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#define GL_TEXTURE_COMPRESSED_ARB 0x86A1
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB 0x86A0
#define GL_VERTEX_ATTRIB_ARRAY1_NV 0x8651
#define GL_VERTEX_ATTRIB_ARRAY3_NV 0x8653
#define GL_VERTEX_ATTRIB_ARRAY4_NV 0x8654
#define GL_VERTEX_ATTRIB_ARRAY5_NV 0x8655
#define GL_VERTEX_ATTRIB_ARRAY12_NV 0x865C
#define GL_VERTEX_ATTRIB_ARRAY13_NV 0x865D
#define GL_VERTEX_ATTRIB_ARRAY14_NV 0x865E
#define GL_VERTEX_ATTRIB_ARRAY15_NV 0x865F
#define GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV 0x8533
#define GL_PN_TRIANGLES_ATI 0x87F0
#define GL_MAX_GENERAL_COMBINERS_NV 0x854D
#define GL_TEXTURE_WRAP_R_EXT 0x8072
#define GL_CLAMP_TO_EDGE_EXT 0x812F

#define AUR_MIPMAP_COMPONENT_MASK 0x0F

#define AUR_PACKED_COLOR_OPAQUE_ALPHA 0xFF000000

#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033

#define GL_GENERATE_MIPMAP_SGIS 0x8191

void AurBuild2DMipMaps(int nWidth, int nHeight, int nBPP,
                       unsigned char* pImage, bool bSubImage);
#define GL_STENCIL_TEST_TWO_SIDE_EXT 0x8910
#define GL_INCR_WRAP_EXT 0x8507
#define GL_DECR_WRAP_EXT 0x8508
typedef void(WINAPI* AurGlActiveStencilFaceProc)(unsigned int nFace);
extern "C" AurGlActiveStencilFaceProc glActiveStencilFaceEXT;
int AurTwoSidedStencilAvailable();

extern int usetwosidedshadows;
extern int countfaces;

namespace Aurora
{
enum AuroraEnableEnums
{
    ENABLE_BLEND = 0,
    ENABLE_ALPHA_TEST = 1,

    ENABLE_FOG = 2,
    ENABLE_CULL_FACE = 3,
    ENABLE_TEXTURE_2D = 4,
    ENABLE_LIGHTING = 5,

    ENABLE_TEXTURE_GEN_S = 6,
    ENABLE_TEXTURE_GEN_T = 7,
    ENABLE_TEXTURE_GEN_R = 8,
    ENABLE_VERTEX_PROGRAM = 9,

    ENABLE_STENCIL_TEST = 10,
    ENABLE_DEPTH_TEST = 11,
    ENABLE_TEXTURE_CUBE_MAP = 12,

    ENABLE_NORMALIZE = 13,

    FOG_START = 15,
    FOG_END = 16,
    FOG_DENSITY = 17,
    FOG_COLOR = 18
};
}

using Aurora::ENABLE_BLEND;
using Aurora::ENABLE_ALPHA_TEST;
using Aurora::ENABLE_LIGHTING;
using Aurora::ENABLE_FOG;
using Aurora::ENABLE_CULL_FACE;
using Aurora::ENABLE_TEXTURE_2D;
using Aurora::ENABLE_DEPTH_TEST;
using Aurora::ENABLE_TEXTURE_GEN_S;
using Aurora::ENABLE_TEXTURE_GEN_T;
using Aurora::ENABLE_TEXTURE_GEN_R;
using Aurora::ENABLE_VERTEX_PROGRAM;
using Aurora::ENABLE_STENCIL_TEST;
using Aurora::ENABLE_TEXTURE_CUBE_MAP;
using Aurora::ENABLE_NORMALIZE;

using Aurora::FOG_START;
using Aurora::FOG_END;
using Aurora::FOG_DENSITY;
using Aurora::FOG_COLOR;

enum
{
    CLIENT_STATE_TEXTURE_COORD = 0,
    CLIENT_STATE_NORMAL = 1,
    CLIENT_STATE_COLOR = 2,
    CLIENT_STATE_VERTEX = 3,

    CLIENT_STATE_SKIN_WEIGHT = 4,
    CLIENT_STATE_BONE_INDEX = 6,

    CLIENT_STATE_LIGHT_VECTOR = 7,
    CLIENT_STATE_TANGENT_BASIS0 = 8,
    CLIENT_STATE_TANGENT_BASIS1 = 9,
    CLIENT_STATE_TANGENT_BASIS2 = 10,
    CLIENT_STATE_BUMP_FOG = 11

};

enum
{
    TEX_PARAM_TARGET_2D = 0,
    TEX_PARAM_MAG_FILTER = 2,
    TEX_PARAM_MIN_FILTER = 3
};
extern const unsigned int texParamTargetTranslationTable[];

extern const unsigned int nvMatrixTranslationTable[];
extern const unsigned int nvMatrixTransformTranslationTable[];
extern const float combinationModeTranslationTable[];
extern const unsigned int texParamTranslationTable[];
#ifndef _XBOX
extern unsigned int enableTranslationTable[];
#else

extern const unsigned int enableTranslationTable[];
#endif
extern const unsigned int fogModeTranslationTable[];
extern const unsigned int queryTranslationTable[];

enum
{
    FOG_MODE_LINEAR = 0,
    FOG_MODE_EXP = 1,
    FOG_MODE_EXP2 = 2
};
extern const unsigned int clientStateTranslationTable[];

enum { FOG_QUERY_ENABLED = 3 };

enum
{
    CMP_LEQUAL = 3,
    CMP_GREATER = 4,

    CMP_ALWAYS = 7
};
extern const unsigned int cmpTranslationTable[];

enum { STENCIL_KEEP = 0, STENCIL_REPLACE = 2, STENCIL_INCREMENT = 3, STENCIL_DECREMENT = 4 };
extern const unsigned int stencilOpTranslationTable[];
extern const unsigned int transformTranslationTable[];
extern const unsigned int cullTranslationTable[];
extern const unsigned int fillTranslationTable[];
extern const unsigned int facingTranslationTable[];

enum { TRANSFORM_PROJECTION = 1, TRANSFORM_MODELVIEW = 2 };

enum
{
    MATERIAL_AMBIENT = 1,
    MATERIAL_DIFFUSE = 2,
    MATERIAL_EMISSION = 3
};
void SetMaterialParameter(int nFace, int nMaterial, float* pfParams);
extern const unsigned int materialTranslationTable[];
extern const unsigned int colorTrackingTranslationTable[];

enum
{
    CULL_CLOCKWISE = 0,
    CULL_COUNTERCLOCKWISE = 1,
    FILL_LINE = 1,
    FILL_SOLID = 2,
    FACING_FRONT = 0,

    FACING_FRONT_AND_BACK = 2
};
extern const unsigned int blendTranslationTable[];

namespace Aurora
{
enum AuroraBlendModes
{
    BLEND_SRC_ALPHA = 0,
    BLEND_ONE_MINUS_SRC_ALPHA = 1,

    BLEND_ONE = 2,
    BLEND_ZERO = 3,

    BLEND_DST_COLOR = 5,
    BLEND_ONE_MINUS_DST_ALPHA = 6

};
}
using Aurora::BLEND_SRC_ALPHA;
using Aurora::BLEND_ONE_MINUS_SRC_ALPHA;
using Aurora::BLEND_ONE;
using Aurora::BLEND_ZERO;
using Aurora::BLEND_DST_COLOR;
using Aurora::BLEND_ONE_MINUS_DST_ALPHA;
void SetBlendMode(int nSource, int nDestination);
extern const unsigned int primTypeTranslationTable[];
extern const unsigned int textureFormatTranslationTable[];
extern const unsigned int lightTranslationTable[];

extern int visualizepass4;

extern int visualizepass1;
extern int visualizepass2;
extern int visualizepass3;
extern int visualizepass5;
extern int visualizepass6;
extern int visualizepass7;
extern int doframebuffer;

enum LightParameter
{
    LIGHT_AMBIENT = 0,
    LIGHT_DIFFUSE = 1,
    LIGHT_SPECULAR = 2,
    LIGHT_POSITION = 3,
    LIGHT_QUADRATIC_ATTENUATION = 4
};

#define AUR_SHADOW_INDEX_COUNT 6144

class VertexPrimitiveFlat;
class VertexPrimitiveSkinned;
class MdlNodeSkin;
class Matrix;
#ifndef _XBOX
class GLRender
{

    friend class ImposterManager;
    friend class CAurTextureBasic;
    friend class CAurTriangleBin;
    friend void EndImposterRender(unsigned long nRecord);
    friend void RenderBumpMapped(VertexPrimitiveFlat* pPrimitive);
    friend void RenderDiffuseBumpMappedPass(VertexPrimitiveFlat* pPrimitive);
    friend void RenderEMBM(VertexPrimitiveFlat* pPrimitive);
    friend void RenderEMLM(VertexPrimitiveFlat* pPrimitive);
    friend void RenderEnvironmentMapped(VertexPrimitiveFlat* pPrimitive, bool, bool);
    friend void RenderFlatDecal(VertexPrimitiveFlat* pPrimitive);
    friend void RenderLightMapped(VertexPrimitiveFlat* pPrimitive);
    friend void RenderLightMappedBumpDiffuse(VertexPrimitiveFlat* pPrimitive);
    friend void RenderLightMappedBumpSpecular(VertexPrimitiveFlat* pPrimitive);
    friend void RenderNV1xEMLM(VertexPrimitiveFlat* pPrimitive);
    friend void RenderNormalMapped(VertexPrimitiveFlat* pPrimitive);
    friend void RenderSpecularBumpMappedPass(VertexPrimitiveFlat* pPrimitive);
    friend void ExecuteSkinTransforms(MdlNodeSkin* pNode, List<Vector>& tbones,
                                      List<Matrix>& rbones,
                                      VertexPrimitiveSkinned* pPrimitive,
                                      bool bBumpedOut);
    friend void RenderSkinnedVP(VertexPrimitiveSkinned* pPrimitive);
    friend void RenderSkinnedVPBlur(VertexPrimitiveSkinned* pPrimitive);
public:

    static void SetFogModef(Aurora::AuroraEnableEnums nMode, float fValue)
    {
        glFogf(enableTranslationTable[nMode], fValue);
    }

    static void SetFogModev(Aurora::AuroraEnableEnums nMode, Vector& vValue)
    {
        glFogfv(enableTranslationTable[nMode], &vValue.m_fX);
    }

    static void Clear(unsigned long nFlags);

    static void SetVertexData2f(Aurora::AuroraStreamMode nStream,
                                float fX, float fY)
    {
        switch (nStream)
        {
        case Aurora::STREAM_POSITION:
            glVertex2f(fX, fY);
            break;
        case Aurora::STREAM_TEXCOORD:
            glTexCoord2f(fX, fY);
            break;
        }
    }

    enum { MAX_SHADOW_VERTICES = 4096 };
    static unsigned short m_UsedVertices[];
    static float* cm_pUserShadowData;

    enum { SHADOW_VERTEX_USED = 1 };
    static int BuildShadowArray(const Vector& vLight, const float& fRadius,
                                const Plane& plane, const Vector& vNearCap,
                                const Vector& vFarCap, int nVertices,
                                RenderVertexV* pVertices);
#ifdef _XBOX

    static int BuildShadowArray(const Vector& vLight, const float& fRadius,
                                const Plane& plane, const Vector& vNearCap,
                                const Vector& vFarCap, int nVertices,
                                unsigned long nVertexPool, unsigned long nVertexDataOffset,
                                int nVertexStride);
#endif
    static void RenderShadowArray(int nVertices, int nEdges, int nPool);
    static void InitExtensions();
    static int InitializeAntiAliasing();
    static void Shutdown(int bReleaseRenderer);
    static void DrawFromVerts(int nMode, List<Vector>& lstVertices);
    static void DrawTriangles(List<MaxFace>& lFaces, RenderVertexV* pVertices,
                              RenderVertexC* pColors);
#ifdef _XBOX

    static void DrawTriangles(int nFaces, List<MaxFace>& lFaces, unsigned long nVertexPool,
                              int nVertexStride, int nPositionOffset, short nVertices,
                              RenderVertexC* pColors, unsigned long nVertexDataOffset);
#endif
    static void RenderObjectsVTQuadUP(RenderVertexVT* pVertices, int nVertices);
    static void RenderObjectsVCUP(Aurora::AuroraPrimitiveTypes nPrimitive,
                                  RenderVertexVC* pVertices, int nVertices);
    static void RenderObjectsVTCQuadUP(RenderVertexVTC* pVertices, int nVertices);
    static void RenderObjectsVTCQuad(unsigned long nPool, int nVertices);
    static void RenderObjectsVTC(Aurora::AuroraPrimitiveTypes nPrimitive,
                                 unsigned long nPool, int nVertices);

    static void DrawSphereBound(float fRadius, Vector& vCenter,
                                const Vector& vColor, float fScale,
                                bool bBillboard);
    static void InitializeATIFragmentShaders();
    static void EnableFragmentShaders(int nShader, CAurTexture* pTexture0,
        CAurTexture* pTexture1, CAurTexture* pTexture2, CAurTexture* pTexture3);
    static void DrawElementsInShadow(unsigned int nProgram,
                                     unsigned long nLength,
                                     const char* sProgram,
                                     unsigned long* pnProgram);

    static void DrawAxisAlignedFrame(const Vector& vMinimum,
                                     const Vector& vMaximum,
                                     const Vector& vColor);

    static void TransformTextureMatrix(float fAngle)
    {
        if (fAngle != 0.0f)
        {
            glMatrixMode(GL_TEXTURE);
            glPushMatrix();
            glLoadIdentity();
            glRotatef(fAngle, 0.0f, 0.0f, 1.0f);
            glMatrixMode(GL_MODELVIEW);
        }
    }
    static void RestoreTextureMatrix(float fAngle)
    {
        if (fAngle != 0.0f)
        {
            glMatrixMode(GL_TEXTURE);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
        }
    }
    static void Transform(const Vector& vPosition,
                          const Quaternion& qOrientation, int nForce);

    static void Scale(float fX, float fY, float fZ);
    static void SetViewPort(double fFieldOfView, double fAspect,
                            double fNearClip, double fFarClip);
    static void SetViewPort(int nX, int nY, int nWidth, int nHeight);

    static int cm_bEnablePausePlane;
    static int cm_nAnisotropy;
    static unsigned int cm_uiCurrentStage;
private:
    static unsigned int cm_nGLCurrentTID[6];
public:
    static void SetActiveTexture(unsigned long nStage);
    static void SetCurrentTID(unsigned int nTID);
    static void SetTextureCoordGenerationState(
        Aurora::AuroraTexCoordMappingModes nMode);
    static int SetCurrentStageTID(unsigned long nStage, unsigned int nTID);
    static bool cm_bVSync;
    static float m_ForceSightColor[4];
    static float m_PerInstanceIllumination[4];
    static bool m_bSpeedBlurEnabled;
    static bool m_bSaturationEnabled;
    static bool m_bScanNoiseEnabled;

    static bool m_bFilmNoiseEnabled;
    static bool cm_bFirstFrameBufferFrame;
    static float m_fSaturation;

    static Vector m_vSaturationMultiplier;
    static float m_fAccumulationRatio;
    static HDC m_hDC;
    static HGLRC m_hGLRC;
    static HDC cm_hAccumulationHDC;

    static unsigned int cm_nAccumulationBlur;

    static PBUFFERARB* cm_hAccumulationPBuffer;

    static HGLRC cm_hAccumulationHGLRC;
    static CAurTexture* cm_pFilmNoiseTexture;
    static CAurTexture* cm_pForceDistortionTexture;
    static unsigned int cm_pHeatDistortionVertexProg;
    static unsigned int cm_nBufferRegionTexture;
    static unsigned int cm_tidAttenuationTexture;
    static int BindTexture(Aurora::AuroraEnableEnums nTarget,
                           unsigned long nTexture);
    static unsigned long RequestPool(Aurora::AuroraBufferRequestType nType,
                            unsigned long nSize, unsigned long nUsage,
                            unsigned long nPoolFlags, unsigned long* pnPool);
    static void* LockPool(unsigned long nPool, unsigned long nOffset,
                          unsigned long nSize, unsigned long nCount);
    static void UnlockPool(unsigned long nPool);
    static void DeletePool(unsigned long nPool);
    static void ResizePool(unsigned long nPool, unsigned long nSize);
    static void SetCombinerConstant0(Vector& vColor, float fAlpha);
    static void SetCombinerConstant1(Vector& vColor, float fAlpha);
    static void SetFragmentShaderConstant0(Vector& vColor, float fAlpha);
    static void SetFragmentShaderConstant1(Vector& vColor, float fAlpha);
    static void (*pfuncSetPixelShaderAmbientColor)(Vector& vColor, float fAlpha);
    static void (*pfuncSetBumpLightColor)(Vector& vColor, float fAlpha);
    static void (*pfuncEnablePixelShaders)(int nShader, CAurTexture* pTexture0,
                                          CAurTexture* pTexture1,
                                          CAurTexture* pTexture2,
                                          CAurTexture* pTexture3);
    static void DisableCombiners(CAurTexture* pTexture);
    static void EnableCombiners(int nShader, CAurTexture* pTexture0,
                               CAurTexture* pTexture1, CAurTexture* pTexture2,
                               CAurTexture* pTexture3);
    static PBUFFERARB* CreatePBuffer(int nWidth, int nHeight);

    static void MipmapFilter(HDC hDC, HGLRC hContext, float fWidth,
                             float fHeight, PBUFFERARB* pBuffer,
                             unsigned long nTexture, float fSourceSize);

    static void MipmapFilterATI(HDC hDC, HGLRC hContext, float fWidth,
                                float fHeight, PBUFFERARB* pBuffer,
                                unsigned long nTexture, float fSourceSize);
    static void InitAttenuationTexture();

    static void RenderOverlayFilmNoise(unsigned long nTexture, bool bScanLines);

    static void RenderOverlayFilmNoiseATI(unsigned long nTexture, bool bScanLines);

    static unsigned int cm_nFilmNoiseShaderATI;
    static void RenderOverlayOverbrighten(unsigned long nTexture);

    static void RenderForceDistortion(unsigned long nTexture);

    static void RenderForceDistortionATI(unsigned long nTexture);

    static unsigned int cm_nForceDistortionShaderATI;

    enum { DISTORTION_ENV_PARAMETER = 15 };

    static void RenderOverlayOverbrightenATI(unsigned long nTexture);
    static unsigned int cm_nOverbrightenRed;
    static unsigned int cm_nOverbrightenGreen;
    static unsigned int cm_nOverbrightenBlue;
    static void FrameBufferModificationPixelShaders(unsigned long nMode,
                                                    unsigned long nTexture0,
                                                    unsigned long nTexture1,
                                                    unsigned long nTexture2,
                                                    unsigned long nTexture3);

    enum
    {

        FRAMEBUFFER_SHADER_CONVOLUTION = 1,
        FRAMEBUFFER_SHADER_DISABLE = 2,
        FRAMEBUFFER_SHADER_COPY = 4,

        FRAMEBUFFER_SHADER_FOUR_DECALS = 8,
        FRAMEBUFFER_SHADER_SOFT_SHADOW = 0x256,

        FRAMEBUFFER_SHADER_SATURATION = 0x16,

        FRAMEBUFFER_SHADER_OVERBRIGHTEN = 0x512,

        FRAMEBUFFER_SHADER_FILM_NOISE = 0x32,

        FRAMEBUFFER_SHADER_ACCUMULATION_BLUR = 0x128,

        FRAMEBUFFER_SHADER_FORCE_DISTORTION = 0x64,
        FRAMEBUFFER_SHADER_DREAM_DISTORTION = 0x1024
    };
    static void FrameBufferModificationPixelShadersATI(unsigned long nMode,
                                                       unsigned long nTexture0,
                                                       unsigned long nTexture1,
                                                       unsigned long nTexture2,
                                                       unsigned long nTexture3);
    static void RenderFrameToTextureATI(unsigned long nTexture);

    static void RenderFrameToTexture(unsigned long nTexture);

    static void RenderAccumulationBlur(unsigned long nTexture);

    static void RenderAccumulationBlurATI(unsigned long nTexture);

    static unsigned int cm_nAccumulationBlurShaderATI;
    static void FrameBufferModificationsATI2(unsigned long nFlags);
    static void FrameBufferModifications2(unsigned long nFlags);

    static void FrameBufferModifications(unsigned long nFlags);
    static void FrameBufferModificationsATI(unsigned long nFlags);
    static void RenderOverlayConvolution(unsigned long nTexture);

    static void RenderOverlayConvolutionATI(unsigned long nTexture);

    static void BloomFilter(HDC hDC, HGLRC hContext, float fWidth,
                            float fHeight, PBUFFERARB* pBuffer,
                            unsigned long nTexture, float fSourceSize,
                            int bSoftShadows);

    static void BloomFilterATI(HDC hDC, HGLRC hContext, float fWidth,
                               float fHeight, PBUFFERARB* pBuffer,
                               unsigned long nTexture, float fSourceSize,
                               int bSoftShadows);

    static void BloomFilterFromTexture(HDC hDC, HGLRC hContext, float fWidth,
                                       float fHeight, unsigned int nTexture,
                                       float fSourceWidth, float fSourceHeight);
    static void RenderSaturationAdjustment(unsigned long nTexture);
    static bool cm_bPBuffersAllocated;
    static bool cm_bSetUpFrameBuffer;
    static void RenderSaturationAdjustmentATI(unsigned long nTexture);
    static unsigned int cm_nTargetTexture;

    static void InitializeFrameBufferModifications();
    static void InitializeFrameBufferModificationFragmentShadersATI();

    enum { CONVOLUTION_BUFFER_COUNT = 6 };
private:
    static unsigned int cm_nConvolutionRenderTexture[CONVOLUTION_BUFFER_COUNT];
    static HDC cm_hConvolutionHDC[CONVOLUTION_BUFFER_COUNT];
    static HGLRC cm_hConvolutionHGLRC[CONVOLUTION_BUFFER_COUNT];
    static PBUFFERARB* cm_hConvolutionPBuffer[CONVOLUTION_BUFFER_COUNT];
public:
    static unsigned int cm_nRedTexture;
    static unsigned int cm_nGreenTexture;
    static unsigned int cm_nBlueTexture;
    static unsigned int cm_nOverbrightenShaderATI;
    static unsigned int cm_nDreamDistortShaderATI;
    static unsigned int cm_nSaturationShaderATI;
    static unsigned int cm_nPerPixelFlareShaderATI;
    static unsigned int cm_nDecalShaderATI;
    static unsigned int cm_n4DecalShaderATI;

    enum
    {
        FRAMEBUFFER_LOOKUP_SIZE = 256,
        FRAMEBUFFER_RED_THRESHOLD = 234,
        FRAMEBUFFER_GREEN_THRESHOLD = 216,
        FRAMEBUFFER_BLUE_THRESHOLD = 216,
        FRAMEBUFFER_GREEN_SHIFT = 8,
        FRAMEBUFFER_BLUE_SHIFT = 16
    };

    enum
    {
        FRAMEBUFFER_OVERBRIGHTEN = 1,
        FRAMEBUFFER_CONVOLUTION = 2,
        FRAMEBUFFER_NOISE = 4,
        FRAMEBUFFER_SATURATION = 8,
        FRAMEBUFFER_SPEED_BLUR = 16,
        FRAMEBUFFER_FORCE_DISTORTION = 32
    };
    static void MakeLookUpTexture(float fGamma);

    static int GetAvailableTextureMemory();

    static int SetSpecularMode(int bSeparateSpecular);
    static void SetVertexBuffer(unsigned long nStride, unsigned long nOffset,
                                unsigned long nPool, int nUsage);

    static void SetInterleavedBuffer(unsigned long nStride, int nFlags,
                                    unsigned long nPool, int nVertexCount,
                                    MdlNodeTriMesh* pMesh, unsigned long nOffset);

    enum VertexAttributeIndex
    {
        VERTEX_ATTRIBUTE_SKIN_WEIGHT = 1,
        VERTEX_ATTRIBUTE_BONE_INDEX = 4,
        VERTEX_ATTRIBUTE_TANGENT_BASIS0 = 12,
        VERTEX_ATTRIBUTE_TANGENT_BASIS1 = 13,
        VERTEX_ATTRIBUTE_TANGENT_BASIS2 = 14
    };
    static void SetColorBuffer(unsigned long nStride, unsigned long nOffset,
                               unsigned long nPool, int nUsage);
    static void SetNormalBuffer(unsigned int nStride, unsigned int nOffset,
                                unsigned int nPool, int nUsage);
    static void SetTexCoordBuffer(unsigned long nStride, unsigned long nOffset,
                                  unsigned long nPool, int nUsage);

    static void SetLightmapTexCoordToStage0(unsigned long nStride, int nFlags,
                                            unsigned long nPool,
                                            int nVertexCount,
                                            MdlNodeTriMesh* pMesh,
                                            unsigned long nOffset);
    static void SetDecalTexCoordToStage0(unsigned long nStride, int nFlags,
                                         unsigned long nPool, int nVertexCount,
                                         MdlNodeTriMesh* pMesh,
                                         unsigned long nOffset);
    static void DrawElements(Aurora::AuroraPrimitiveTypes nPrimitive,
                             unsigned long nCount, unsigned long nOffset,
                             unsigned long nPool);
    static void RenderSquares(unsigned int nPool);
    static void RenderDrawableSquares();

    static void RenderBox(Vector& vMinimum, Vector& vMaximum);
    static void ActivateTextureStage(unsigned long nStage)  // PC 0x00425700
    {
        if (AurMultiTextureAvailable())
        {
            glClientActiveTextureARB(nStage + 33984);
        }
        GLRender::cm_uiCurrentStage = nStage;
    }

    static void DrawLightmappedGrass(void* pVertices, int nVertexStride,
                                     void* pIndices, int nStride, int nFirst,
                                     int nQuads);

    static void RenderPausePlane();
    static List<unsigned long> cm_listSquaresToRender;
    static void AddSquareToRender(unsigned long nPool);
    static void RemoveSquareToRender(unsigned long nPool);
    static void MakeNormalizationCubeMap(int nSize);
    static void CreateCubeTexture(CAurTexture* pTexture, bool bMipMaps,
                                  int nIndex);
    static void CreateProcessedCubeTexture(CAurTexture* pTexture, bool bMipMaps,
                                           int nIndex);
    static void CreateTexture(CAurTexture* pTexture, bool bMipMaps, int nIndex);
    static void CreateProcessedTexture(CAurTexture* pTexture, bool bMipMaps,
                                       int nIndex);
    static void ConvertHeightFieldAndLoadNormalMapTexture(
        unsigned char* pHeightField, int nWidth, int nHeight, int nRowWidth,
        int nRowCount, float fScale, int bUpdate, bool bNormalMap);
    static int SetUpImposters(int nType, float* pfValues);
    static void SetFilmNoiseTexture(CAurTexture* pTexture);
    static void SetDirtyTexture(int nIndex, CAurTexture* pTexture);
    static void EnableDreamDistortion(bool bLetterbox);
    static void DisableDreamDistortion();
    static void EnableFury(short nLevel);
    static void RenderDreamDistortion(unsigned long nTexture);
    static void RenderDreamDistortionATI(unsigned long nTexture);
    static void RenderFury(unsigned long nTexture);
    static void RenderFuryATI(unsigned long nTexture);
    static CAurTexture* m_pDreamDistortTexture;
    static bool m_bDreamDistortEnabled;
    static bool m_bDreamDistortLetterbox;
    static CAurTexture* m_pHologramFilterTexture;
    static short m_FuryLevel;
    static CAurTexture* m_pFuryTexture;
    static CAurTexture* m_pDirtyTexture[3];
    static int InitRenderSystem();
    static int GetFogArraysInfoRef();
    static void SetViewTransform(Vector& vPosition, Quaternion& qOrientation);
    static void SetUpSoftShadowsARB();
    static void SetUpSoftShadowsATI();
    static void InitializeImageSpaceSoftShadows();

    static PBUFFERARB* cm_hSoftShadowPBuffer;
    static PBUFFERARB* cm_hSoftShadowTargetPBuffer;
    static unsigned int cm_nSoftShadowRenderTexture;
    static unsigned int cm_nSoftShadowTargetRenderTexture;
    static unsigned int cm_nSoftShadowShaderATI;

    static unsigned int cm_nPerPixelFlareShader;

    static unsigned long cm_nScreenWidthPow2;
    static unsigned long cm_nScreenHeightPow2;

    static HDC cm_hSoftShadowHDC;
    static HDC cm_hSoftShadowTargetHDC;
    static HGLRC cm_hSoftShadowHGLRC;
    static HGLRC cm_hSoftShadowTargetHGLRC;
    static void SetUpSoftShadows();
    static void FinishSoftShadows();
    static void FinishSoftShadowsATI();

    static bool cm_bSetUpImageSpaceShadows;

    enum { STENCIL_WRITE_MASK = 0xFF, SHADOW_STENCIL_MASK = 0xF };

    enum { DISTORTION_STENCIL_MASK = 1 };
    static void SetUpTextFormat();
    static void PrintStringAtScreenPos(char* szString, float fX, float fY);

    static void PopAttrib()
    {
        glPopAttrib();
    }
    static void BeginImmediatePrimitive(Aurora::AuroraPrimitiveTypes nType)
    {
        glBegin(primTypeTranslationTable[nType]);
    }
    static void EndImmediatePrimitive()
    {
        glEnd();
    }

};

inline void GLRender::Clear(unsigned long nFlags)
{
    glClear(((nFlags & 1) ? GL_COLOR_BUFFER_BIT : 0) |
            ((nFlags & 2) ? GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT : 0) |
            ((nFlags & 4) ? GL_STENCIL_BUFFER_BIT : 0));
}
// PC 0x0046E320
inline void GLRender::SetViewPort(int nX, int nY, int nWidth, int nHeight)
{
    glViewport(nX, nY, nWidth, nHeight);
}
// PC 0x00421610
inline void GLRender::SetActiveTexture(unsigned long nStage)
{
    cm_uiCurrentStage = nStage;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
}

inline void GLRender::SetTextureCoordGenerationState(
    Aurora::AuroraTexCoordMappingModes nMode)
{
    int nGLMode;
    switch (nMode)
    {
    case Aurora::TEXCOORD_MAPPING_SPHERE:
        nGLMode = GL_SPHERE_MAP;
        break;
    case Aurora::TEXCOORD_MAPPING_REFLECTION:
        nGLMode = GL_REFLECTION_MAP_ARB;
        break;
    default:
        return;
    }
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, nGLMode);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, nGLMode);
    if (nMode == Aurora::TEXCOORD_MAPPING_REFLECTION)
    {
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, nGLMode);
    }
    glEnable(enableTranslationTable[ENABLE_TEXTURE_GEN_S]);
    glEnable(enableTranslationTable[ENABLE_TEXTURE_GEN_T]);
    if (nMode == Aurora::TEXCOORD_MAPPING_REFLECTION)
    {
        glEnable(enableTranslationTable[ENABLE_TEXTURE_GEN_R]);
    }
}

#define GLRender_Enable(nCap) glEnable(enableTranslationTable[nCap])
#define GLRender_Disable(nCap) glDisable(enableTranslationTable[nCap])
#define GLRender_IsEnabled(nCap) glIsEnabled(enableTranslationTable[nCap])
#define GLRender_SetBlendFunc(nSource, nDestination) \
    glBlendFunc(blendTranslationTable[nSource], blendTranslationTable[nDestination])
#define GLRender_EnableClientState(nArray) glEnableClientState(clientStateTranslationTable[nArray])
#define GLRender_DisableClientState(nArray) glDisableClientState(clientStateTranslationTable[nArray])
#define GLRender_PushCurrentMatrix() glPushMatrix()
#define GLRender_PopCurrentMatrix() glPopMatrix()
#define GLRender_EnablePixelShaders GLRender::pfuncEnablePixelShaders
#define GLRender_SetPixelShaderAmbientColor GLRender::pfuncSetPixelShaderAmbientColor
#define GLRender_SetBumpLightColor GLRender::pfuncSetBumpLightColor
#define GLRender_SetDepthMask glDepthMask
#define GLRender_SetFrontFace(nWinding) glFrontFace(cullTranslationTable[nWinding])
#define GLRender_QueryIntegerv glGetIntegerv
#define GLRender_BeginImmediatePrimitive(nType) glBegin(primTypeTranslationTable[nType])
#define GLRender_EndImmediatePrimitive() glEnd()

#define GLRender_SetMatrixMode(nMode) glMatrixMode(transformTranslationTable[nMode])
#define GLRender_LoadMatrixIdentity() glLoadIdentity()
#define GLRender_SetPolygonRenderMode(nFace, nFill) glPolygonMode(facingTranslationTable[nFace], fillTranslationTable[nFill])
#define GLRender_GetViewPort(pViewport) glGetIntegerv(GL_VIEWPORT, pViewport)
#define GLRender_SetPerspectiveTransform gluPerspective
#define GLRender_SetOrthoTransform glOrtho
#define GLRender_SetViewPort glViewport

#define GLRender_DeleteTextures glDeleteTextures
#define GLRender_GenTextures glGenTextures
#define GLRender_SetClearColor glClearColor
#define GLRender_Rotate glRotatef

#define GLRender_SetFogModef(nMode, fValue) glFogf(enableTranslationTable[nMode], fValue)

#define GLRender_SetColor4f glColor4f
#define GLRender_SetAlphaFunc(nFunc, fReference) glAlphaFunc(cmpTranslationTable[nFunc], fReference)
#define GLRender_SetStencilOp(nFail, nZFail, nPass)     glStencilOp(stencilOpTranslationTable[nFail], stencilOpTranslationTable[nZFail], stencilOpTranslationTable[nPass])
#define GLRender_SetStencilMask glStencilMask
#define GLRender_SetStencilFunc(nFunc, nReference, nMask) glStencilFunc(cmpTranslationTable[nFunc], nReference, nMask)
#define GLRender_SetColorMask glColorMask
#define GLRender_ClearStencilValue glClearStencil

#define GLRender_Clear(nFlags)     glClear((((nFlags) & 1) ? GL_COLOR_BUFFER_BIT : 0) | (((nFlags) & 2) ? GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT : 0) |             (((nFlags) & 4) ? GL_STENCIL_BUFFER_BIT : 0))
#define GLRender_EnableColorMaterial() glEnable(GL_COLOR_MATERIAL)
#define GLRender_DisableColorMaterial() glDisable(GL_COLOR_MATERIAL)
#define GLRender_SetColorMaterial(nFace, nMode)     glColorMaterial(facingTranslationTable[nFace], colorTrackingTranslationTable[nMode])
#define GLRender_SetMaterialv(nFace, nMaterial, pfParams)     glMaterialfv(facingTranslationTable[nFace], materialTranslationTable[nMaterial], pfParams)
#define GLRender_SetLightModel(pfColor) glLightModelfv(GL_LIGHT_MODEL_AMBIENT, pfColor)
#define GLRender_PushAttrib glPushAttrib
#define GLRender_PopAttrib glPopAttrib

#define GLRender_SetLightProperties(nLight, nParameter, pfParams)     glLightfv(GL_LIGHT0 + (nLight), lightTranslationTable[nParameter], pfParams)
#define GLRender_EnableLight(nLight) glEnable(GL_LIGHT0 + (nLight))
#define GLRender_DisableLight(nLight) glDisable(GL_LIGHT0 + (nLight))
#define GLRender_SetFogMode(nFogMode) glFogi(GL_FOG_MODE, fogModeTranslationTable[nFogMode])
#define GLRender_SetFogModev(nMode, vValue) glFogfv(enableTranslationTable[nMode], &(vValue).m_fX)
#define GLRender_SetDepthFunc(nFunc) glDepthFunc(cmpTranslationTable[nFunc])
#define GLRender_SetColorv glColor4fv

#define GLRender_SetVertexProgramParameter4f(nIndex, fX, fY, fZ, fW)     if (usearbvertexprograms)         glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, nIndex, fX, fY, fZ, fW);     else         glProgramParameter4fNV(GL_VERTEX_PROGRAM_ARB, nIndex, fX, fY, fZ, fW)
#define GLRender_Translate glTranslatef
#define GLRender_MultMatrix glMultMatrixf

#else

#include "aurrenderdx.h"
#endif

#ifndef _XBOX
inline void AurSetVertexProgramParameter4f(unsigned int nIndex,  // PC 0x00463BF0
                                          float x, float y, float z, float w)
{
    if (usearbvertexprograms != 0)
    {
        glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, nIndex, x, y, z, w);
    }
    else
    {
        glProgramParameter4fNV(GL_VERTEX_PROGRAM_ARB, nIndex, x, y, z, w);
    }
}
#endif

void SpitLetters(char* szString);
void AurOrtho(double dLeft, double dRight, double dBottom, double dTop,
              double dNear, double dFar);
int AurCubeMapAvailable();
unsigned char AurATIValidate();
unsigned char AurNonRadeon8500Validate();
int AurOnePassEnvMapAvailable();
int AurTextureCompressionAvailable();
int AurBumpMapAvailable();
int AurSGIMipmapAvailable();
int AurMultiTextureAvailable();
int AurFrameBufferModificationsAvailable();
int AurATIFrameBufferModificationsAvailable();
int AurVertexBufferObjectARBAvailable();
int AurATISoftShadowsAvailable();
int AurSoftShadowsAvailable();
int AurGetAvailableStencilBits();
bool AurNV2xLevelShadersAvailable();
extern int systemmemoryshadows;

extern int useshadowmethod;
extern float epsilon;
extern int noclipshadows;
extern int dontprotectagainstclip;
void ClipProjection(Vector& vPosition, const Vector& vStart, const Plane& plane);

enum { SHADOW_METHOD_VERTEX_PROGRAM = 3 };

class ImposterManager
{
public:
    ImposterManager();

    struct Record
    {

        Record() : m_bAllocated(false) {}

        bool m_bAllocated;
        unsigned int m_nTexture;
        HGLRC m_hContext;
        PBUFFERARB* m_pBuffer;
        HDC m_hBufferDC;
        float* m_pVertices;
        int m_nVertices;
    };

    Record* m_pImposters;
    int m_nImposters;
    void AllocateImposter(unsigned long, int, float*);
    int AllocateImposterRecord(int, float*);
};

class RequestPoolManager
{
public:

    enum PoolUsage
    {
        POOL_STATIC = 2,

        POOL_COLOR = 0x40,

        POOL_FLARE_USAGE = 0x42
    };

    enum PoolFormat { POOL_FORMAT_VTC = 0x1011 };
#ifdef _XBOX

    enum { POOL_FORMAT_VC = 0x11 };
#endif

    enum
    {
        INVALID_POOL = -1
    };

    enum
    {
        POOL_LOCK_WRITE = 1,
        POOL_LOCK_READ_ONLY = 8,
        POOL_LOCK_READ_WRITE = 16
    };

    enum
    {
        POOL_PAGE_ROUNDING = 4095,
        POOL_PAGE_MASK = 0xFFFFF000
    };

#ifdef _XBOX

    enum
    {
        POOL_VERTEX_FLAG = 0x80000000,
        POOL_INDEX_MASK = 0x7FFFFFFF,
        POOL_INDEX_ROUNDING = 3,
        POOL_INDEX_ROUND_MASK = 0xFFFFFFFC,
        POOL_INITIAL_COUNT = 50
    };

    enum
    {
        POOL_STATE_FREE = 0,
        POOL_STATE_ADOPTED = 1,
        POOL_STATE_CREATED = 2
    };
    unsigned int m_nVertexPools;
    unsigned int m_nIndexPools;
    D3DVertexBuffer** m_ppVertexPools;
    D3DIndexBuffer** m_ppIndexPools;
    unsigned long* m_pVertexPoolSize;
    unsigned long* m_pIndexPoolSize;
    List<unsigned long> m_lstFreeVertexPools;
    List<unsigned long> m_lstFreeIndexPools;
    unsigned char* m_pVertexPoolState;
    unsigned char* m_pIndexPoolState;
    RequestPoolManager();
    ~RequestPoolManager();
    static RequestPoolManager* GetInstance();
    unsigned long RequestVertexPool(unsigned long nSize, unsigned long nUsage, unsigned long nFVF,
                                    unsigned long* pnPool, D3DDevice* pDevice);
    unsigned long RequestIndexPool(unsigned long nSize, unsigned long nUsage, D3DFORMAT nFormat,
                                   unsigned long* pnPool, D3DDevice* pDevice);
    unsigned long AddVertexPool(D3DVertexBuffer* pBuffer, unsigned long nSize);
    unsigned long AddIndexPool(D3DIndexBuffer* pBuffer, unsigned long nSize);
    void* LockPool(unsigned long nPool, unsigned long nOffset, unsigned long nSize,
                   unsigned long nFlags);
    void UnlockPool(unsigned long nPool);
    D3DResource* GetPool(unsigned long nPool);
    void DeletePool(unsigned long nPool);
    void ResizePool(unsigned long nPool, unsigned long nSize, D3DDevice* pDevice);
    void ReleasePools();
#else
    List<unsigned long> m_lstFreePools;
    List<int> m_lstPoolLocks;

    RequestPoolManager();
    virtual ~RequestPoolManager();
    virtual int RequestPoolGeneric(int nType, int nSize, int nUsage) = 0;
    virtual void* LockPool(int nPool, int nCount) = 0;
    virtual void UnlockPool(unsigned int nPool) = 0;
    virtual int GetPool(unsigned int nPool) = 0;
    virtual int GetIndexPool(unsigned int nPool) = 0;
    virtual void DeletePool(unsigned int nPool) = 0;
    virtual void ResizePool(unsigned int nPool, unsigned int nSize) = 0;

    virtual int GetPoolSize(unsigned int nPool) = 0;
    virtual void RefreshPool(int nPool, int nUsage) = 0;
    virtual void BackUpPoolManager() = 0;
    virtual void RestorePoolManager() = 0;
#endif
};

#ifndef _XBOX
class RequestPoolManagerSys : public RequestPoolManager
{
public:
    int m_nPoolCount;
    char** m_ppPools;
    unsigned long* m_pPoolSize;
    virtual ~RequestPoolManagerSys();
    virtual int RequestPoolGeneric(int, int, int);
    virtual void* LockPool(int, int);
    virtual void UnlockPool(unsigned int);
    virtual int GetPool(unsigned int);
    virtual int GetIndexPool(unsigned int nPool);
    virtual void DeletePool(unsigned int);
    virtual void ResizePool(unsigned int, unsigned int);
    virtual int GetPoolSize(unsigned int);
    virtual void RefreshPool(int, int);
    virtual void BackUpPoolManager();
    virtual void RestorePoolManager();
    RequestPoolManagerSys();
};

class RequestPoolManagerVBO : public RequestPoolManager
{
public:
    int m_nPoolCount;

    List<unsigned long> m_lstPoolUsage;
    List<unsigned long> m_lstPools;
    List<char*> m_lstPoolBackup;
    List<unsigned long> m_lstPoolBackupSize;
    List<unsigned long> m_lstPoolTypes;
    virtual ~RequestPoolManagerVBO();
    virtual int RequestPoolGeneric(int, int, int);
    virtual void* LockPool(int, int);
    virtual void UnlockPool(unsigned int);
    virtual int GetPool(unsigned int);
    virtual int GetIndexPool(unsigned int);
    virtual void DeletePool(unsigned int);
    virtual void ResizePool(unsigned int, unsigned int);
    virtual int GetPoolSize(unsigned int);
    virtual void RefreshPool(int, int);
    virtual void BackUpPoolManager();
    virtual void RestorePoolManager();
    RequestPoolManagerVBO();
};
#endif

#include <aurlist.h>

class Part;
class VertexProgram;
#include <aurlist.h>

void AurVertexProgramMakeCurrent();
void TrackMatrices();
void AurGetVertexProgramARBError(char* sError);

void BindVertexProgram(unsigned int nProgram);

class VertexProgram
{
public:

    enum
    {
        VERTEX_PROGRAM_BUFFER_SIZE = 8192,
        LIGHT_PROGRAM_BUFFER_SIZE = 4096
    };

    enum { PARAM_BUMPED_OUT_SHELL = 12,

           PARAM_GRASS_DIFFUSE = 94, PARAM_GRASS_AMBIENT = 95 };

    enum { PARAM_BUMPY_SHINY_PASS = 15 };

    enum { PARAM_BUMP_LIGHT_VECTOR = 17 };

    enum
    {
        PARAM_BUMP_LIGHT_POSITION = 12,
        PARAM_BUMP_EYE_POSITION = 13,
        PARAM_BUMP_ATTENUATION = 14,
        PARAM_BUMP_FOG = 16
    };

    enum
    {
        PARAM_MATERIAL_ILLUMINATION = 93,
        PARAM_MATERIAL_DIFFUSE = 94,
        PARAM_MATERIAL_AMBIENT = 95
    };

    enum NormalMapParameter
    {
        PARAM_NORMAL_MAP_LIGHT_POSITION = 12,
        PARAM_NORMAL_MAP_EYE_POSITION = 13,
        PARAM_NORMAL_MAP_ATTENUATION = 14,
        PARAM_NORMAL_MAP_FOG = 16,
        PARAM_NORMAL_MAP_CONSTANTS = 17
    };

    enum DirtyParameter
    {
        PARAM_DIRTY_SELECT = 15,
        PARAM_DIRTY_CONSTANTS = 16,
        PARAM_DIRTY_OCTAVES = 28
    };

    enum FogParameter
    {
        PARAM_FOG_RANGE = 20,
        PARAM_FOG_COLOR = 21,
        PARAM_FOG_LIMITS = 22
    };

    VertexProgram(char* sProgram, unsigned long* pnProgram);
    ~VertexProgram();

    void Enable(Part* pPart, bool bEnableLights);

    void ShutDown();
    void Load(char* sProgram, unsigned int* pnProgram);

#ifndef _XBOX
    unsigned char m_aUnmapped0[4];
#endif
    char* m_sProgram;
    char m_sVertexProgram[VERTEX_PROGRAM_BUFFER_SIZE];
    unsigned long* m_pProgram;
    unsigned int m_nGLProgram;
    int m_nAllowedLights;
    int m_nLastUpdateFrame;
    int m_bLoaded;
};

class VertexProgramManager
{
public:
    static int ResetVertexPrograms();
    static void AddVertexProgram(VertexProgram* pProgram);

    static List<VertexProgram*> cm_listExistentVertexPrograms;
};

extern VertexProgram* g_pBumpedOutVP;

extern float position_scale_factor;
extern int initialized;
extern unsigned int fontOffset;
extern unsigned char rasters[95][13];
extern "C" void(WINAPI* glBindProgramARB)(unsigned int nTarget, unsigned int nProgram);
extern "C" void(WINAPI* glBindFragmentShaderATI)(unsigned int nShader);
extern "C" void(WINAPI* glSetFragmentShaderConstantATI)(unsigned int nConstant, const float* pfValue);
extern int multmatrix;
extern "C" void(APIENTRY* glBindProgramNV)(unsigned int nTarget, unsigned int nProgram);
extern float largest_supported_anisotropy;

extern char szBumpmapVPARB[];
extern char szNormalmapVPARB[];
extern char szBumpyShinySetupVPARB[];
extern char szStandardVPARB[];
extern char szStandardCubeVPARB[];
extern char szStandardLightmapCubeVPARB[];
extern char szStandardFogVPARB[];
extern char szBumpyShinySkinSetupVPARB[];
extern char szSkinVPARB[];
extern char szSkinFogVPARB[];
extern char szSkinCubeMapVPARB[];
extern char szSkinBumpedOutVPARB[];
extern char szSkinBumpyVPARB[];
extern char szNewSkinBumpyVPARB[];
extern char szBumpedOutVPARB[];
extern char szSkinBumpyWSVPARB[];
extern char szcStandardDirtyVPARB[];
extern char szcStandardDirtyLMVPARB[];
extern char szcStandardDirtyEMVPARB[];
extern char szcStandardDirtyEMLMVPARB[];
extern char szcStandardDirtyLSVPARB[];
extern char szcStandardDirtyLSLMVPARB[];
extern char szcStandardDirtyLSEMVPARB[];
extern char szszcStandardDirtyLSEMLMVPARB[];
extern char cHoloSkinVPARB[];
extern char cHoloVPARB[];
extern char cBumpyShinyVP[];
extern char cBumpyShinySkinVP[];
extern char cBumpMapOnePassVP[];
extern char cBumpMapSpecularVP[];
extern char cBumpMapDiffuseVP[];
extern char cBumpMapVP[];
extern char cStandardVP[];
extern char cStandardDirtyVP[];
extern char cStandardDirtyLMVP[];
extern char cStandardDirtyEMVP[];
extern char cStandardDirtyEMLMVP[];
extern char cStandardDirtyLSVP[];
extern char cStandardDirtyLSLMVP[];
extern char cStandardDirtyLSEMVP[];
extern char cStandardDirtyLSEMLMVP[];
extern char cFogTestVP[];
extern char cLightProjectionVP[];
extern char cFogOutputVP[];
extern char cSkinVP[];
extern char cSkinBumpedOutVP[];
extern char cSkin3BoneVP[];
extern char cSkinFogOutputVP[];
extern char cSkinBumpyVP[];
extern char cNewSkinBumpyVP[];
extern char cSkinSphereMapNoTextureVP[];
extern char cSkinSphereMapVP[];
extern char cSkinCubeMapVP[];
extern char cMotionBlurredVP[];
extern char cSkinMotionBlurVP[];
extern char cBlurVP[];
extern char cBumpedOutVP[];
extern char cPhongVP[];
extern char cShadowTriMeshVariantVP[];
extern char cSkinShadowVariantVP[];

extern char szBumpyShinyVPARB_ATI[];
extern char szBumpyShinyVPARB_NV[];
extern char* szBumpyShinyVPARB;
extern char* szBumpyShinySkinVPARB;
extern unsigned long* szNormalmapVPARBDeclaration;
extern int g_nOldAliasingMode;
extern int g_nAliasingMode;
extern "C" void(APIENTRY* glTrackMatrixNV)(unsigned int nTarget, unsigned int nAddress, unsigned int nMatrix, unsigned int nTransform);
extern "C" void(APIENTRY* glLoadProgramNV)(unsigned int nTarget, unsigned int nID, int nLength, const unsigned char* sProgram);
extern "C" void(APIENTRY* glDeleteProgramsNV)(int nCount, const unsigned int* pnPrograms);
extern "C" void(APIENTRY* glGenProgramsNV)(int nCount, unsigned int* pnPrograms);
extern "C" unsigned char(APIENTRY* glIsProgramARB)(unsigned int nProgram);
extern "C" unsigned char(APIENTRY* glIsProgramNV)(unsigned int nProgram);
extern unsigned int ARB_MULTITEXTURE_BIT;
extern int existingextensions;
extern unsigned int WGL_ARB_MULTISAMPLE_BIT;
extern int g_nUnlockATISoftShadowsForHotFix;
int AurAnisotropicFilterAvailable();
unsigned char AuroraUseARBShaders();
extern int renderdangly;
extern int maxshadowvar;
extern int cullwithLOD;
extern float mindooralpha;
void SetBeaming();
extern int enablerender;
extern int animationfootprint;
extern int envmapon;
extern int tex1on;
extern float g_DesatureFactor;
extern unsigned long g_SceneRender_CurrentTime_ms;
extern bool g_ForceSight;
int AurIsAAliasingAvailable();
extern void (*AurRenderCallback)();
extern int g_nModelsRead;
extern int enablegrass;
extern int enableshadows;
extern int enablesoftshadows;
extern int renderaabb;
extern float fadeToZeroTime;
#ifndef _XBOX
extern unsigned long* cStandardVPDeclaration;
#endif

#ifndef _XBOX
extern unsigned long* cBumpyShinySkinVPDeclaration;
extern unsigned long* cSkin3BoneVPDeclaration;
extern unsigned long* cSkinBumpyVPDeclaration;
extern unsigned long* cSkinMotionBlurVPDeclaration;
extern unsigned long* cBumpMapOnePassVPDeclaration;
extern unsigned long* cBumpMapSpecularVPDeclaration;
extern unsigned long* cBumpMapDiffuseVPDeclaration;
extern unsigned long* cBumpMapVPDeclaration;
extern unsigned long* cFogOutputVPDeclaration;
extern unsigned long* cSkinVPDeclaration;
extern unsigned long* cSkinBumpedOutVPDeclaration;
extern unsigned long* cSkinSphereMapVPDeclaration;
extern unsigned long* cSkinCubeMapVPDeclaration;
#endif
bool AurNV1xLevelShadersAvailable();
int AurVertexProgramsNVAvailable();
#ifndef _XBOX
extern unsigned long* cStandardVP2Declaration;
extern unsigned long* cBumpyShinyRegularVPDeclaration;
extern unsigned long* cBumpyShinyLightMappedVPDeclaration;
#endif
#ifdef _XBOX
extern unsigned long cBumpyShinyLightMappedVPDeclaration[];
extern unsigned long cBumpyShinyRegularVPDeclaration[];
extern unsigned long cBumpMapVPDeclaration[];
extern unsigned long cStandardVPDeclaration[];
extern unsigned long cStandardVP2Declaration[];
extern unsigned long cEmitterVPDeclaration[];
extern unsigned long cMotionBlurredVPDeclaration[];
extern unsigned long cBumpedOutVPDeclaration[];
extern unsigned long cFogTestVPDeclaration[];
extern unsigned long cLightProjectionVPDeclaration[];
extern unsigned long cFogOutputVPDeclaration[];
extern unsigned long cBumpyShinySkinVPDeclaration[];
extern unsigned long cSkinVPDeclaration[];
extern unsigned long cSkinBumpedOutVPDeclaration[];
extern unsigned long cSkinMotionBlurVPDeclaration[];
extern unsigned long cSkin3BoneVPDeclaration[];
extern unsigned long cSkinBumpyVPDeclaration[];
extern unsigned long cSkinSphereMapVPDeclaration[];
extern unsigned long cSkinCubeMapVPDeclaration[];
extern unsigned long cBumpMapOnePassVPDeclaration[];
extern unsigned long cBumpMapDiffuseVPDeclaration[];
extern unsigned long cBumpMapSpecularVPDeclaration[];
extern unsigned long cSkinFogOutputVPDeclaration[];
extern unsigned long cBlurVPDeclaration[];
extern unsigned long cShadowTriMeshVariantVPDeclaration[];
extern unsigned long cSkinShadowVPDeclaration[];
extern char cEmitterVP[];
extern char cShadowTriMeshVP[];
#endif

extern VertexProgram* g_pBumpyShinyPass1;
extern VertexProgram* g_pBumpyShinyPass2;
extern VertexProgram* g_pBumpyShinyPass3;
extern VertexProgram* g_pBumpyShinyPass4;
extern VertexProgram* g_pBumpyShinyPass5;
extern int enablerenderembm;
extern int enabledecal;
extern int enablebsenvmap;
extern int overridestuff;
#endif
