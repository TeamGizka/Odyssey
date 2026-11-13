#include "../math3d/math3d.h"
#include <new>
#include <stdio.h>
#include <string.h>
#include "aurora.h"

static inline void AurGLPoolLists()
{
    List<unsigned long>* pPools = new List<unsigned long>(0);
    List<int> lstLocks(0);
    List<char*> lstBackup(0);
    pPools->Remove(0);
}
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

inline void PushMatrix()
{
    glPushMatrix();
}

inline void PopMatrix()
{
    glPopMatrix();
}

inline void LoadMatrix(const float* pMatrix)
{
    glLoadMatrixf(pMatrix);
}

inline void Viewport(int nX, int nY, int nWidth, int nHeight)
{
    glViewport(nX, nY, nWidth, nHeight);
}

#include "aurimage.h"
#include "aurtexman.h"
#include "aurrendergl.h"
#include "aurmodel.h"
#include "swmain.h"
#include "aurglext.h"

extern float aquadraticterm;

enum
{
    GL_MODELVIEW_PROJECTION_NV = 0x8629,
    GL_IDENTITY_NV = 0x862A,
    GL_INVERSE_NV = 0x862B,
    GL_TRANSPOSE_NV = 0x862C,
    GL_INVERSE_TRANSPOSE_NV = 0x862D
};

extern "C" AurGlMultiTexCoord2fProc glMultiTexCoord2fARB;

extern int usebumpdecaltexture;
int AurVertexProgramsAvailable();
extern "C" AurWglGetExtensionsStringProc wglGetExtensionsStringARB;
#define GL_CON_7_ATI 0x8948
#define GL_BLUE_BIT_ATI 0x4
#define GL_CND_ATI 0x896A
#define GL_TEXTURE_3D 0x806F
extern bool g_ForceSight;
extern int g_Letterbox_top;
extern int g_Letterbox_bottom;
extern bool g_bLetterBoxMode;
typedef void (WINAPI* PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (WINAPI* PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
extern "C"
{
    extern PFNGLTEXIMAGE3DPROC glTexImage3D;
    extern PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
}
float largest_supported_anisotropy = 0.0f;
int GLRender::cm_nAnisotropy = 0;
bool GLRender::cm_bPBuffersAllocated = 0;
unsigned int GLRender::cm_nOverbrightenRed = 0;
unsigned int GLRender::cm_nOverbrightenGreen = 0;
unsigned int GLRender::cm_nOverbrightenBlue = 0;

bool GLRender::cm_bVSync = true;
float GLRender::m_ForceSightColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float GLRender::m_PerInstanceIllumination[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
unsigned int GLRender::cm_nRedTexture = 0;
unsigned int GLRender::cm_nGreenTexture = 0;
unsigned int GLRender::cm_nBlueTexture = 0;
unsigned int GLRender::cm_nTargetTexture = 0;
bool GLRender::m_bSpeedBlurEnabled = 0;
bool GLRender::m_bSaturationEnabled = 0;
bool GLRender::m_bScanNoiseEnabled = 0;
bool GLRender::m_bFilmNoiseEnabled = 0;
CAurTexture* GLRender::m_pDreamDistortTexture = 0;
bool GLRender::m_bDreamDistortEnabled = 0;
bool GLRender::m_bDreamDistortLetterbox = 0;
CAurTexture* GLRender::m_pHologramFilterTexture = 0;
short GLRender::m_FuryLevel = 0;
CAurTexture* GLRender::m_pFuryTexture = 0;
CAurTexture* GLRender::m_pDirtyTexture[3];
float GLRender::m_fSaturation = 0;
float GLRender::m_fAccumulationRatio = 0.98f;
int g_nAliasingMode = 0;

const unsigned char* wglExtensions = 0;
int g_nOldAliasingMode = -1;
unsigned long GLRender::cm_nScreenWidthPow2 = 0;
unsigned long GLRender::cm_nScreenHeightPow2 = 0;
unsigned int GLRender::cm_uiCurrentStage = 0;
int GLRender::cm_bEnablePausePlane = 0;
unsigned int GLRender::cm_tidAttenuationTexture = 0;
static RequestPoolManager* manRef = 0;
unsigned int enableTranslationTable[] =
{
    GL_BLEND, GL_ALPHA_TEST, GL_FOG, GL_CULL_FACE, GL_TEXTURE_2D,
    GL_LIGHTING, GL_TEXTURE_GEN_S, GL_TEXTURE_GEN_T, GL_TEXTURE_GEN_R,
    GL_VERTEX_PROGRAM_ARB, GL_STENCIL_TEST, GL_DEPTH_TEST,
    GL_TEXTURE_CUBE_MAP_ARB, GL_NORMALIZE, GL_PN_TRIANGLES_ATI,
    GL_FOG_START, GL_FOG_END, GL_FOG_DENSITY, GL_FOG_COLOR, GL_TEXTURE_3D
};
int multmatrix = 1;
GLubyte rasters[95][13] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36},
    {0x00, 0x00, 0x00, 0x66, 0x66, 0xff, 0x66, 0x66, 0xff, 0x66, 0x66, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x7e, 0xff, 0x1b, 0x1f, 0x7e, 0xf8, 0xd8, 0xff, 0x7e, 0x18},
    {0x00, 0x00, 0x0e, 0x1b, 0xdb, 0x6e, 0x30, 0x18, 0x0c, 0x76, 0xdb, 0xd8, 0x70},
    {0x00, 0x00, 0x7f, 0xc6, 0xcf, 0xd8, 0x70, 0x70, 0xd8, 0xcc, 0xcc, 0x6c, 0x38},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1c, 0x0c, 0x0e},
    {0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c},
    {0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x30},
    {0x00, 0x00, 0x00, 0x00, 0x99, 0x5a, 0x3c, 0xff, 0x3c, 0x5a, 0x99, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18, 0x00, 0x00},
    {0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x03, 0x03},
    {0x00, 0x00, 0x3c, 0x66, 0xc3, 0xe3, 0xf3, 0xdb, 0xcf, 0xc7, 0xc3, 0x66, 0x3c},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x38, 0x18},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0xe7, 0x7e},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0x07, 0x03, 0x03, 0xe7, 0x7e},
    {0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xff, 0xcc, 0x6c, 0x3c, 0x1c, 0x0c},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x03, 0x03, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x03, 0x7f, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e},
    {0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x1c, 0x1c, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06},
    {0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60},
    {0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x18, 0x0c, 0x06, 0x03, 0xc3, 0xc3, 0x7e},
    {0x00, 0x00, 0x3f, 0x60, 0xcf, 0xdb, 0xd3, 0xdd, 0xc3, 0x7e, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18},
    {0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e},
    {0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3},
    {0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff},
    {0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c},
    {0x00, 0x03, 0x03, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60},
    {0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18},
    {0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x30, 0x70},
    {0x00, 0x00, 0x7f, 0xc3, 0xc3, 0x7f, 0x03, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0x7e, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x03, 0x03, 0x03, 0x03, 0x03},
    {0x00, 0x00, 0x7f, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x33, 0x1e},
    {0x7e, 0xc3, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00},
    {0x38, 0x6c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x0c, 0x00},
    {0x00, 0x00, 0xc6, 0xcc, 0xf8, 0xf0, 0xd8, 0xcc, 0xc6, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78},
    {0x00, 0x00, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xfe, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xfc, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00},
    {0xc0, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0x00, 0x00, 0x00, 0x00},
    {0x03, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xfe, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xfe, 0x03, 0x03, 0x7e, 0xc0, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x1c, 0x36, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x00},
    {0x00, 0x00, 0x7e, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xdb, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00},
    {0xc0, 0x60, 0x60, 0x30, 0x18, 0x3c, 0x66, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xff, 0x60, 0x30, 0x18, 0x0c, 0x06, 0xff, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x0f, 0x18, 0x18, 0x18, 0x38, 0xf0, 0x38, 0x18, 0x18, 0x18, 0x0f},
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
    {0x00, 0x00, 0xf0, 0x18, 0x18, 0x18, 0x1c, 0x0f, 0x1c, 0x18, 0x18, 0x18, 0xf0},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x8f, 0xf1, 0x60, 0x00, 0x00, 0x00},
};
int usebumpdecaltexture = 1;
int totalvertexpool = 0;
int totalvertexpool4k = 0;

int totalindexpool = 0;
int totalindexpool4 = 0;

int junk = 0;

int visualizeregular = 1;
int visualizepass1 = 0;
int visualizepass2 = 1;
int visualizepass3 = 1;
int visualizepass4 = 1;
int visualizepass5 = 1;
int visualizepass6 = 1;
int visualizepass7 = 1;
int doframebuffer = 1;
unsigned int GLRender::cm_nAccumulationBlur = 0;
PBUFFERARB* GLRender::cm_hAccumulationPBuffer = 0;
HDC GLRender::cm_hAccumulationHDC = 0;
HGLRC GLRender::cm_hAccumulationHGLRC = 0;

bool GLRender::cm_bFirstFrameBufferFrame = true;
CAurTexture* GLRender::cm_pFilmNoiseTexture = 0;
unsigned int GLRender::cm_pHeatDistortionVertexProg = 0;
CAurTexture* GLRender::cm_pForceDistortionTexture = 0;
int usetwosidedshadows = 0;
int noclipshadows = 0;
int dontprotectagainstclip = 1;

char pPerPixelFlareShader[] =
    "!!ARBfp1.0TEMP temp;\n"
    "TEMP red;\n"
    "TEMP green;\n"
    "TEMP blue;\n"
    "TEMP final;\n"
    "PARAM c0 = { 0.0, 0.0, 0.0, 1.0 };\n"
    "TEX temp, fragment.texcoord[0], texture[0], RECT;\n"
    "TEX red, temp.r, texture[1], 1D;\n"
    "TEX green, temp.g, texture[2], 1D;\n"
    "TEX blue, temp.b, texture[3], 1D;\n"
    "ADD_SAT final, red, green;\n"
    "ADD_SAT final, final, blue;\n"
    "MOV result.color, final;\n"
    "END";
unsigned int GLRender::cm_nPerPixelFlareShader = 0;
PBUFFERARB* GLRender::cm_hSoftShadowPBuffer = 0;
HDC GLRender::cm_hSoftShadowHDC = 0;
HGLRC GLRender::cm_hSoftShadowHGLRC = 0;
unsigned int GLRender::cm_nSoftShadowRenderTexture = 0;
unsigned int GLRender::cm_nBufferRegionTexture = 0;
bool GLRender::cm_bSetUpImageSpaceShadows = true;
PBUFFERARB* GLRender::cm_hSoftShadowTargetPBuffer = 0;
HDC GLRender::cm_hSoftShadowTargetHDC = 0;
HGLRC GLRender::cm_hSoftShadowTargetHGLRC = 0;
unsigned int GLRender::cm_nSoftShadowTargetRenderTexture = 0;
unsigned int GLRender::cm_nPerPixelFlareShaderATI = 0;
unsigned int GLRender::cm_nDecalShaderATI = 0;

unsigned int GLRender::cm_n4DecalShaderATI = 0;
unsigned int GLRender::cm_nSaturationShaderATI = 0;
unsigned int GLRender::cm_nAccumulationBlurShaderATI = 0;
unsigned int GLRender::cm_nFilmNoiseShaderATI = 0;
unsigned int GLRender::cm_nForceDistortionShaderATI = 0;
unsigned int GLRender::cm_nSoftShadowShaderATI = 0;
unsigned int GLRender::cm_nOverbrightenShaderATI = 0;
unsigned int GLRender::cm_nDreamDistortShaderATI = 0;
float aquadraticterm = 0.5f;

float quadricshift = 0.05f;
bool GLRender::cm_bSetUpFrameBuffer = true;

float multiplier = 0.35f;
float cutoff = 0.05f;
float pixeloffset = 2.0f;
float softshadowdarkeningfactor = 0.4f;
int displaylayer = 5;

int viewconvolution = 0;
int speedtestconvolution = 0;
int baselayer = 0;
float basesize = 64.0f;
float basesize256 = 256.0f;
float basesize512 = 512.0f;
float basesize64 = 64.0f;
float basesize128 = 128.0f;

int renderregular = 1;
float g_fGLCoreUnused1 = 0.35f;
float distortionalpha = 0.85f;

bool CheckExtension(char* sExtension, unsigned char const* sExtensions);
extern unsigned int GL_STENCIL_TWO_SIDED_BIT;
extern unsigned int EXT_BGRA_BIT;
extern unsigned int EXT_TEXTURE_ENV_COMBINE_BIT;
extern unsigned int NV_REGISTER_COMBINERS_BIT;
extern unsigned int NV_REGISTER_COMBINERS2_BIT;
extern unsigned int EXT_TEXTURE_CUBE_MAP_BIT;
extern unsigned int EXT_FOG_COORD_BIT;
extern unsigned int EXT_SECONDARY_COLOR_BIT;
extern unsigned int NV_VERTEX_PROGRAMS_BIT;
extern unsigned int EXT_TEXTURE_FILTER_ANISOTROPIC_BIT;
extern unsigned int NV_PIXEL_SHADERS_BIT;
extern unsigned int NV_VERTEX_ARRAY_RANGE_BIT;
extern unsigned int NV_VERTEX_ARRAY_RANGE2_BIT;
extern unsigned int NV_FENCE_BIT;
extern unsigned int ARB_TEXTURE_COMPRESSION_BIT;
extern unsigned int EXT_TEXTURE_COMPRESSION_S3TC_BIT;
extern unsigned int ATIX_PN_TRIANGLES_BIT;
extern unsigned int EXT_COMPILED_VERTEX_ARRAY_BIT;
extern unsigned int WGL_ARB_RENDER_TO_TEXTURE_BIT;
extern unsigned int EXT_SGIS_MIPMAP_BIT;
extern unsigned int ATI_FRAGMENT_SHADER_BIT;
extern unsigned int GL_ARB_VERTEX_PROGRAM_BIT;
extern unsigned int GL_ARB_DEPTH_TEXTURE_BIT;
extern unsigned int GL_ARB_VERTEX_BUFFER_OBJECT_BIT;
extern unsigned int WGL_NV_RENDER_TO_TEXTURE_RECTANGLE_BIT;
extern unsigned int WGL_ARB_BUFFER_REGION_BIT;
extern unsigned int EXT_SWAP_CONTROL_BIT;
void InitializeSystemPoolManager();
void InitializeVertexBufferObjectPoolManager();

enum { GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = 0x84ff };

extern ImposterManager manRefImposter;

unsigned int GLRender::cm_nGLCurrentTID[6];
HDC GLRender::m_hDC;
HGLRC GLRender::m_hGLRC;

extern int baselayer;
extern float basesize;
extern int displaylayer;

extern int baselayer;
extern float basesize;
extern int displaylayer;

enum
{
    GL_LIGHT_MODEL_COLOR_CONTROL_EXT = 0x81F8,
    GL_SINGLE_COLOR_EXT = 0x81F9,
    GL_SEPARATE_SPECULAR_COLOR_EXT = 0x81FA
};

inline ImposterManager::ImposterManager() : m_pImposters(NULL), m_nImposters(2)
{
}
// PC 0x00421C40 GOG 0x00421C50
inline int GLRender::BindTexture(Aurora::AuroraEnableEnums nTarget,
                                 unsigned long nTexture)
{
    glBindTexture(enableTranslationTable[nTarget], nTexture);
    glTexParameteri(enableTranslationTable[nTarget], 0x84FE,
                    GLRender::cm_nAnisotropy);
    GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = nTexture;
    return GLRender::cm_uiCurrentStage;
}
// PC 0x004015A0 GOG 0x004015A0
void AurInitVertexArrayRange(int nBytes)
{
}
// PC 0x004015A0 GOG 0x004015A0
void RequestPoolManagerSys::BackUpPoolManager()
{
}
// PC 0x004015A0 GOG 0x004015A0
void RequestPoolManagerSys::RestorePoolManager()
{
}

inline void SetMatrixMode(unsigned int nMode)
{
    glMatrixMode(nMode);
}

inline void LoadIdentity()
{
    glLoadIdentity();
}

inline void SetBlendFunc(unsigned int nSource, unsigned int nDestination)
{
    glBlendFunc(nSource, nDestination);
}

inline void DrawIndexedPrimitive(unsigned int nMode, int nCount, const void* pIndices)
{
    glDrawElements(nMode, nCount, GL_UNSIGNED_SHORT, pIndices);
}

inline void MultMatrix(const float* pMatrix)
{
    glMultMatrixf(pMatrix);
}

inline void EnableClientState(unsigned int nArray)
{
    glEnableClientState(nArray);
}

inline void DisableClientState(unsigned int nArray)
{
    glDisableClientState(nArray);
}

inline void AurPushAttrib(unsigned int nMask)
{
    glPushAttrib(nMask);
}

inline void AurPopAttrib()
{
    glPopAttrib();
}

inline void SetStencilOp(unsigned int nFail, unsigned int nDepthFail, unsigned int nPass)
{
    glStencilOp(nFail, nDepthFail, nPass);
}

inline void SetFrontFace(unsigned int nMode)
{
    glFrontFace(nMode);
}

inline void SetStencilMask(unsigned int nMask)
{
    glStencilMask(nMask);
}

void (*GLRender::pfuncSetPixelShaderAmbientColor)(Vector& vColor, float fAlpha);
void (*GLRender::pfuncSetBumpLightColor)(Vector& vColor, float fAlpha);
void (*GLRender::pfuncEnablePixelShaders)(int nShader, CAurTexture* pTexture0,
                                         CAurTexture* pTexture1,
                                         CAurTexture* pTexture2,
                                         CAurTexture* pTexture3);
unsigned short GLRender::m_UsedVertices[GLRender::MAX_SHADOW_VERTICES];

int GLRender::GetAvailableTextureMemory()
{
    unsigned char data[256 * 256 * 4];
    unsigned int textures[1000];
    unsigned int nTexture;
    int nResident;
    int nCount = 0;
    int i;

    cm_nGLCurrentTID[cm_uiCurrentStage] = 0;
    if (!junk)
    {
        for (i = 0; i < 1000; i++)
        {
            data[0] = (unsigned char)i;
            glGenTextures(1, &nTexture);
            glBindTexture(GL_TEXTURE_2D, nTexture);
            textures[i] = nTexture;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, data);
        }
    }
    junk = 1;
    for (i = 0; i < 1000; i++)
    {
        nResident = -1;
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_RESIDENT, &nResident);
        if (nResident)
        {
            nCount++;
        }
    }
    return nCount * 256 * 256 * 4;
}

inline void Translate(float fX, float fY, float fZ)
{
    glTranslatef(fX, fY, fZ);
}

inline void Rotate(float fAngle, float fX, float fY, float fZ)
{
    glRotatef(fAngle, fX, fY, fZ);
}

inline void SetDepthMask(unsigned char bWrite)
{
    glDepthMask(bWrite);
}

int GLRender::SetSpecularMode(int bSeparateSpecular)
{
    if (bSeparateSpecular)
    {
        if (!CheckExtension("GL_EXT_separate_specular_color",
                            glGetString(GL_EXTENSIONS)))
        {
            return 0;
        }
        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT,
                      GL_SEPARATE_SPECULAR_COLOR_EXT);
        return 1;
    }
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SINGLE_COLOR_EXT);
    return 1;
}

extern unsigned int g_rgbaPauseOverlayColor;

inline void SetStencilFunc(unsigned int nFunc, int nRef, unsigned int nMask)
{
    glStencilFunc(nFunc, nRef, nMask);
}

inline void DrawPrimitive(unsigned int nMode, int nFirst, int nCount)
{
    glDrawArrays(nMode, nFirst, nCount);
}

void BeginImposterRender(unsigned long nRecord)
{
    ImposterManager::Record* pRecord = &manRefImposter.m_pImposters[nRecord];
    if (!wglMakeCurrent(pRecord->m_hBufferDC, pRecord->m_hContext))
        gluErrorString(glGetError());

    glViewport(0, 0, 256, 256);
    glClearColor(0.5f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.01, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 6.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    gluErrorString(glGetError());
}

void EndImposterRender(unsigned long nRecord)
{
    gluErrorString(glGetError());
    GLRender::SetActiveTexture(0);
    ImposterManager::Record* pRecord = &manRefImposter.m_pImposters[nRecord];
    glBindTexture(GL_TEXTURE_2D, pRecord->m_nTexture);
    GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = 0;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFinish();
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 128, 128, 0);
    gluErrorString(glGetError());
    wglMakeCurrent(GLRender::m_hDC, GLRender::m_hGLRC);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, pRecord->m_nTexture);
    GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = 0;
    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(4.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(4.0f, 4.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0.0f, 4.0f, 0.0f);
    glEnd();
}

void GLRender::RenderBox(Vector& vMinimum, Vector& vMaximum)
{
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(vMinimum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);

    glVertex3f(vMinimum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);

    glVertex3f(vMinimum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);

    glVertex3f(vMaximum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);

    glVertex3f(vMaximum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);

    glVertex3f(vMaximum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
    glEnd();
}

void GLRender::RenderPausePlane()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBegin(GL_QUADS);
    glColor4ubv((GLubyte*)&g_rgbaPauseOverlayColor);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glEnd();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

static Vector* AllocateVectorArray(int nCount)
{
    return new Vector[nCount];
}

unsigned int ones32popcount(unsigned int x)
{
    x -= ((x >> 1) & 0x55555555);
    x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
    x = (((x >> 4) + x) & 0x0F0F0F0F);
    x += (x >> 8);
    x += (x >> 16);
    return x & 0x3F;
}

void GLRender::BloomFilterFromTexture(HDC hDC, HGLRC hContext, float fWidth,
                                      float fHeight, unsigned int nTexture,
                                      float fSourceWidth, float fSourceHeight)
{
    float filter[25] = {
        0.0037f, 0.0147f, 0.0256f, 0.0147f, 0.0037f,
        0.0147f, 0.0586f, 0.0952f, 0.0586f, 0.0147f,
        0.0256f, 0.0952f, 0.1502f, 0.0952f, 0.0256f,
        0.0147f, 0.0586f, 0.0952f, 0.0586f, 0.0147f,
        0.0037f, 0.0147f, 0.0256f, 0.0147f, 0.0037f
    };
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_PER_STAGE_CONSTANTS_NV);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_SOFT_SHADOW, 0, 0, 0, 0);
    int nRow = 0;
    int nColumn = 0;
    glEnable(GL_BLEND);
    for (int nSample = 0; nSample < 25; nSample += 4)
    {
        for (int nStage = 0; nStage < 4; ++nStage)
        {
            float color[4];
            color[3] = color[2] = color[1] = color[0] =
                filter[nSample + nStage] * 8.6f;
            if (nStage == 0)
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR0_NV, color);
            else if (nStage == 1)
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, color);
            else if (nStage == 2)
                glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR0_NV, color);
            else
                glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, color);
        }
        glBegin(GL_QUADS);
        for (int i = 0; i < 4; ++i)
        {
            glColor4f(1.0f, 1.0f, 1.0f, multiplier);
            glActiveTextureARB(GL_TEXTURE0_ARB);
            float t = (i == 0 || i == 3) ? (nRow + fSourceHeight - 1.5f) / fSourceHeight : 0.0f + (nRow - 1.5f) / fSourceHeight;
            float s = (i == 0 || i == 1) ? 0.0f + (nColumn - 1.5f) / fSourceWidth : (nColumn + fSourceWidth - 1.5f) / fSourceWidth;
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
            int nTextureRow = (nColumn + 1 > 4) ? nRow + 1 : nRow;
            int nTextureColumn = (nColumn + 1) % 5;
            glActiveTextureARB(GL_TEXTURE1_ARB);
            t = (i == 0 || i == 3) ? (nTextureColumn + fSourceHeight - 1.5f) / fSourceHeight : 0.0f + (nTextureColumn - 1.5f) / fSourceHeight;
            s = (i == 0 || i == 1) ? 0.0f + (nTextureRow - 1.5f) / fSourceWidth : (nTextureRow + fSourceWidth - 1.5f) / fSourceWidth;
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, s, t);
            nTextureRow = (nColumn + 2 > 4) ? nRow + 1 : nRow;
            nTextureColumn = (nColumn + 2) % 5;
            glActiveTextureARB(GL_TEXTURE2_ARB);
            t = (i == 0 || i == 3) ? (nTextureColumn + fSourceHeight - 1.5f) / fSourceHeight : 0.0f + (nTextureColumn - 1.5f) / fSourceHeight;
            s = (i == 0 || i == 1) ? 0.0f + (nTextureRow - 1.5f) / fSourceWidth : (nTextureRow + fSourceWidth - 1.5f) / fSourceWidth;
            glMultiTexCoord2fARB(GL_TEXTURE2_ARB, s, t);
            nTextureRow = (nColumn + 3 > 4) ? nRow + 1 : nRow;
            nTextureColumn = (nColumn + 3) % 5;
            glActiveTextureARB(GL_TEXTURE3_ARB);
            t = (i == 0 || i == 3) ? (nTextureColumn + fSourceHeight - 1.5f) / fSourceHeight : 0.0f + (nTextureColumn - 1.5f) / fSourceHeight;
            s = (i == 0 || i == 1) ? 0.0f + (nTextureRow - 1.5f) / fSourceWidth : (nTextureRow + fSourceWidth - 1.5f) / fSourceWidth;
            glMultiTexCoord2fARB(GL_TEXTURE3_ARB, s, t);
            float y = (i == 0 || i == 3) ? fHeight : 0.0f;
            float x = (i == 0 || i == 1) ? 0.0f : fWidth;
            glVertex3f(x, y, 0.0f);
        }
        glEnd();
        if (nColumn + 4 > 4)
            ++nRow;
        nColumn = (nColumn + 4) % 5;
    }
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glFlush();
}

void GLRender::SetUpSoftShadowsARB()
{
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nBufferRegionTexture);
    glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, g_nScreenWidth, g_nScreenHeight);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
// PC 0x00448F80 GOG 0x00431EB0
template <> inline List<unsigned long>::~List()
{
    if (m_pList != 0)
    {
        delete[] m_pList;
    }
}

inline RequestPoolManager::RequestPoolManager() : m_lstFreePools(0), m_lstPoolLocks(0)
{
}

void GLRender::RenderObjectsVTC(Aurora::AuroraPrimitiveTypes nPrimitive,
                                unsigned long nPool, int nVertices)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, manRef->GetPool(nPool));
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(RenderVertexVTC), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(RenderVertexVTC), (const GLvoid*)12);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RenderVertexVTC), (const GLvoid*)16);
        glDrawArrays(primTypeTranslationTable[nPrimitive], 0, nVertices);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glDisableClientState(GL_COLOR_ARRAY);
    }
    else
    {
        glBegin(primTypeTranslationTable[nPrimitive]);
        RenderVertexVTC* pVertices =
            (RenderVertexVTC*)manRef->GetPool(nPool);
        for (int i = 0; i < nVertices; ++i)
        {
            glColor4ubv(pVertices[i].color);
            glTexCoord2f(pVertices[i].u, pVertices[i].v);
            glVertex3fv(pVertices[i].position);
        }
        glEnd();
    }
}

void SpitLetters(char* szString)
{
    if (initialized == 0)
    {
        AurInitFont();
    }

    AurPushAttrib(GL_LIST_BIT);
    glListBase(fontOffset);
    glCallLists(strlen(szString), GL_UNSIGNED_BYTE, szString);
    AurPopAttrib();
}
// PC 0x00447510 GOG 0x00447660
void InitializeSystemPoolManager()
{
    if (manRef == 0)
    {
        manRef = new RequestPoolManagerSys;
    }
}
// PC 0x00447580 GOG 0x004476D0
void InitializeVertexBufferObjectPoolManager()
{
    if (manRef == 0)
    {
        manRef = new RequestPoolManagerVBO;
    }
}
// PC 0x00425350 GOG 0x00425460
void RequestPoolManagerSys::ResizePool(unsigned int nPool, unsigned int nSize)
{
    totalvertexpool += nSize - m_pPoolSize[nPool];
    totalvertexpool4k += ((nSize + 4095) & 0xFFFFF000) -
                                ((m_pPoolSize[nPool] + 4095) & 0xFFFFF000);
    char* pOldPool = m_ppPools[nPool];
    char* pNewPool = new char[nSize];
    memcpy(pNewPool, pOldPool, m_pPoolSize[nPool]);
    delete[] pOldPool;
    m_pPoolSize[nPool] = nSize;
    m_ppPools[nPool] = pNewPool;
}
// PC 0x00425400 GOG 0x00425510
int RequestPoolManagerSys::GetPoolSize(unsigned int nPool)
{
    return m_pPoolSize[nPool];
}
// PC 0x00425410 GOG 0x00425520
int RequestPoolManagerSys::GetPool(unsigned int nPool)
{
    if (nPool == (unsigned int)-1)
    {
        return 0;
    }

    return (int)m_ppPools[nPool];
}
// PC 0x00425410 GOG 0x00425520
int RequestPoolManagerSys::GetIndexPool(unsigned int nPool)
{
    if (nPool == (unsigned int)-1)
    {
        return 0;
    }

    return (int)m_ppPools[nPool];
}
// PC 0x00434BB0 GOG 0x00434D00
void RequestPoolManagerVBO::BackUpPoolManager()
{
    int nSize;
    for (int nPool = 0; nPool < m_lstPools.m_nCount; nPool++)
    {
        if (m_lstPools[nPool] == 0)
        {
            m_lstPoolBackup.Add(0);
            m_lstPoolBackupSize.Add(0);
        }
        else
        {
            glBindBufferARB(m_lstPoolTypes[nPool], m_lstPools[nPool]);
            glGetBufferParameterivARB(m_lstPoolTypes[nPool], GL_BUFFER_SIZE_ARB,
                                      &nSize);
            m_lstPoolBackupSize.Add(nSize);
            m_lstPoolBackup.Add(new char[nSize]);
            void* pData = glMapBufferARB(m_lstPoolTypes[nPool], GL_READ_ONLY_ARB);
            memcpy(m_lstPoolBackup[nPool], pData, m_lstPoolBackupSize[nPool]);
            glUnmapBufferARB(m_lstPoolTypes[nPool]);

            glDeleteBuffersARB(1, (unsigned int*)&m_lstPools[nPool]);
            m_lstPools[nPool] = 0;
        }
    }
}
// PC 0x00434D60 GOG 0x00434EB0
void RequestPoolManagerVBO::RestorePoolManager()
{
    for (int nPool = 0; nPool < m_lstPoolBackup.m_nCount; nPool++)
    {
        if (m_lstPoolBackup[nPool] != 0)
        {
            unsigned int nBuffer;
            glGenBuffersARB(1, &nBuffer);
            m_lstPools[nPool] = nBuffer;
            glBindBufferARB(m_lstPoolTypes[nPool], nBuffer);
            glBufferDataARB(m_lstPoolTypes[nPool], m_lstPoolBackupSize[nPool],
                            m_lstPoolBackup[nPool], GL_DYNAMIC_DRAW_ARB);
            delete[] m_lstPoolBackup[nPool];
        }
    }

    m_lstPoolBackup.m_nCount = 0;
    m_lstPoolBackupSize.m_nCount = 0;
    m_lstPoolBackup.SetSize(0);
    m_lstPoolBackupSize.SetSize(0);
}
// PC 0x006049D0 GOG 0x006049B0
void RequestPoolManagerSys::RefreshPool(int nPool, int nUsage)
{
}
// PC 0x00425500 GOG 0x00425610
Vector Quaternion::axis() const
{
    Vector vAxis(m_fX, m_fY, m_fZ);
    double dAngle = acos(m_fW) * 2.0;
    if (AbsoluteValue(dAngle) < 1e-7)
    {
        return Vector(1.0f, 0.0f, 0.0f);
    }
    float fScale = (float)(1.0 / sin(dAngle * 0.5));
    return Vector(vAxis.m_fX * fScale, vAxis.m_fY * fScale, vAxis.m_fZ * fScale);
}
// PC 0x00425590 GOG 0x004256A0
void DisableTextureShaders()
{
    glDisable(GL_TEXTURE_SHADER_NV);
}
// PC 0x004255A0 GOG 0x004256B0
inline void GLRender::DisableCombiners(CAurTexture* pTexture)
{
    glDisable(GL_REGISTER_COMBINERS_NV);
    GLRender::cm_uiCurrentStage = 1;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(GLRender::cm_uiCurrentStage + 33984);
    }
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_2D);
    envmapon = 0;
    GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = 0;
    tex1on = 0;
    GLRender::cm_uiCurrentStage = 0;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(GLRender::cm_uiCurrentStage + 33984);
    }
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
}

const char* GLStats(char* sBuffer)
{
    sprintf(sBuffer, "faces[%d]", countfaces);
    return sBuffer;
}
// PC 0x00425650 GOG 0x00425760
inline void GLRender::SetCombinerConstant0(Vector& vColor, float fAlpha)
{
    glCombinerParameterfvNV(0x852A, Homogenize(vColor, fAlpha));
}
// PC 0x00425670 GOG 0x00425780
inline void GLRender::SetCombinerConstant1(Vector& vColor, float fAlpha)
{
    glCombinerParameterfvNV(0x852B, Homogenize(vColor, fAlpha));
}
// PC 0x00425690 GOG 0x004257A0
void SetShaderConstant(unsigned int nConstant, float* pValues, int nCount)
{
    if ((existingextensions & ATI_FRAGMENT_SHADER_BIT) == ATI_FRAGMENT_SHADER_BIT)
    {
        if (nConstant == 0)
            glSetFragmentShaderConstantATI(GL_CON_0_ATI, pValues);
        else if (nConstant == 1)
            glSetFragmentShaderConstantATI(GL_CON_1_ATI, pValues);
    }
    else
    {
        if (nConstant == 0)
            glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, pValues);
        else if (nConstant == 1)
            glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, pValues);
    }
}

void GLRender::EnableCombiners(int nShader, CAurTexture* pTexture0,
                               CAurTexture* pTexture1, CAurTexture* pTexture2,
                               CAurTexture* pTexture3)
{
    switch (nShader)
    {
        case MODEL_SHADER_DISABLE:
            GLRender::DisableCombiners(pTexture0);
            glEnable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLRender::SetActiveTexture(1);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLRender::SetActiveTexture(2);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLRender::SetActiveTexture(3);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLRender::SetActiveTexture(0);
            GLRender::SetCurrentStageTID(0, 0);
            GLRender::SetCurrentStageTID(1, 0);
            GLRender::SetCurrentStageTID(2, 0);
            GLRender::SetCurrentStageTID(3, 0);
            return;

        case MODEL_SHADER_PRIMARY_COLOR:
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_TEXTURE_FOG:
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_FOG);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_ATTENUATED_NORMAL:
            glEnable(GL_REGISTER_COMBINERS_NV);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture3->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, GLRender::cm_tidAttenuationTexture);
            GLRender::SetCurrentStageTID(1, 0);
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture2->Bind();
            GLRender::SetActiveTexture(0);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE3_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE1_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_FOG:
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_ENVIRONMENT_FOG:
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            pTexture1->Bind();
            if (pTexture1->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_EMLM:
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(2);
            pTexture2->Bind();
            if (pTexture2->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_NV1X_ENVIRONMENT:
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            pTexture1->Bind();
            if ( pTexture1->IsCubeMap() )
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_MODULATE:
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_EMLM_DISABLE:
            GLRender::SetActiveTexture(1);
            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            glDisable(GL_TEXTURE_2D);
            GLRender::SetCurrentTID(0);
            GLRender::SetActiveTexture(2);
            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            glDisable(GL_TEXTURE_2D);
            GLRender::SetCurrentTID(0);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_TEXTURE_GEN_R);
            GLRender::DisableCombiners(pTexture0);
            GLRender::SetActiveTexture(0);
            return;

        case MODEL_SHADER_LIGHTMAP_ONLY:
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            tex1on = 1;
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_NV1X_LIGHTMAP:
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            tex1on = 1;
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_SPECULAR:
        case MODEL_SHADER_SPECULAR_SQUARED:
        case MODEL_SHADER_SPECULAR_FOG:
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(1);
            pTexture3->Bind();
            GLRender::SetActiveTexture(0);
            pTexture1->Bind();
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_SIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_TRUE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_BLUE);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_E_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_F_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_E_TIMES_F_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            if ( nShader == MODEL_SHADER_SPECULAR_SQUARED )
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_E_TIMES_F_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            else
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_DIFFUSE_BUMP:
        case MODEL_SHADER_DIFFUSE_BUMP_FOG:
        case MODEL_SHADER_DIFFUSE_BUMP_FOG_ALPHA:
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_BLUE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_BLUE);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_SCALE_BY_FOUR_NV, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            if ( nShader == MODEL_SHADER_DIFFUSE_BUMP_FOG_ALPHA )
            {
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            }
            else
            {
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            }
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            if (nShader == MODEL_SHADER_DIFFUSE_BUMP || nShader == MODEL_SHADER_DIFFUSE_BUMP_FOG_ALPHA)
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            else
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_BUMP:
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_2D);
            pTexture3->Bind();
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 7);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE2_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_TRUE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ONE, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_TRUE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_DISCARD_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ONE, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_DISCARD_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            if ( usebumpdecaltexture != 0 )
                glCombinerInputNV(GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            else
                glCombinerInputNV(GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER5_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER5_NV, GL_RGB, GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ONE, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER6_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER6_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_BUMP_DISABLE:
            GLRender::SetActiveTexture(1);
            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            glDisable(GL_TEXTURE_2D);
            GLRender::SetCurrentTID(0);
            GLRender::SetActiveTexture(2);
            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            glDisable(GL_TEXTURE_2D);
            GLRender::SetCurrentTID(0);
            GLRender::SetActiveTexture(3);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            GLRender::SetCurrentTID(0);
            GLRender::DisableCombiners(pTexture0);
            glEnable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            return;

        case MODEL_SHADER_NORMAL:
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture3->Bind();
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE3_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_TRUE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ONE, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_NV1X_DISABLE:
            if (AurNV2xLevelShadersAvailable())
            {
                GLRender::SetActiveTexture(3);
                glDisable(GL_TEXTURE_3D);
                glBindTexture(GL_TEXTURE_3D, 0);
                GLRender::SetCurrentTID(0);
                glDisable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(2);
                glDisable(GL_TEXTURE_3D);
                glBindTexture(GL_TEXTURE_3D, 0);
                GLRender::SetCurrentTID(0);
                glDisable(GL_TEXTURE_2D);
                if (envmapon != 0)
                {
                    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                }
            }
            GLRender::SetActiveTexture(1);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_3D);
            glBindTexture(GL_TEXTURE_3D, 0);
            GLRender::SetCurrentTID(0);
            if (envmapon != 0)
            {
                glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                glDisable(GL_TEXTURE_GEN_S);
                glDisable(GL_TEXTURE_GEN_T);
                glDisable(GL_TEXTURE_GEN_R);
                envmapon = 0;
            }
            GLRender::SetActiveTexture(0);
            GLRender::DisableCombiners(pTexture0);
            return;

        case MODEL_SHADER_BUMP_LIGHTMAP:
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_2D);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_2D);
            pTexture3->Bind();
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_NORMAL_LIGHTMAP:
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_2D);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_2D);
            pTexture3->Bind();
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_SECONDARY_COLOR_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_BUMP_ENVIRONMENT:
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_2D);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_2D);
            pTexture3->Bind();
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE1_NV, GL_SECONDARY_COLOR_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE1_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_BUMP_ENVIRONMENT_LIGHTMAP:
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_2D);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_2D);
            pTexture3->Bind();
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE1_NV, GL_SECONDARY_COLOR_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_BLUE);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE1_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_SPARE1_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_ENVIRONMENT_ONLY:
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE1_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_TEXTURE_SHADER_DISABLE:
            DisableTextureShaders();
            break;

        case MODEL_SHADER_BUMP_MODULATE:
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            break;

        case MODEL_SHADER_TEXTURE_SHADER_BUMP:
            GLRender::MakeLookUpTexture(5.0f);
            glEnable(GL_TEXTURE_SHADER_NV);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
            GLRender::SetActiveTexture(1);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            GLRender::SetActiveTexture(2);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture3->Bind();
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            break;

        case MODEL_SHADER_TEXTURE_SHADER_LOOKUP:
            GLRender::MakeLookUpTexture(5.0f);
            glEnable(GL_TEXTURE_SHADER_NV);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
            GLRender::SetActiveTexture(1);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            GLRender::SetActiveTexture(2);
            glBindTexture(GL_TEXTURE_2D, lookup);
            GLRender::SetCurrentStageTID(2, 0);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_TEXTURE_2D_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            GLRender::SetActiveTexture(3);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, 0);
            break;

        case MODEL_SHADER_TEXTURE_COLOR_FOG:
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_FOG);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_NOISE_SUBTRACT:
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_NOISE_ADD:
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_NOISE_MODULATE:
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_NOISE_EM_SUBTRACT:
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_NOISE_EM_ADD:
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_NOISE_EM_MODULATE:
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_NOISE_LM_SUBTRACT:
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_2D);
                pTexture1->Bind();
                tex1on = 1;
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_NOISE_LM_ADD:
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_2D);
                pTexture1->Bind();
                tex1on = 1;
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_NOISE_LM_MODULATE:
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_2D);
                pTexture1->Bind();
                tex1on = 1;
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_NOISE_EMLM_SUBTRACT:
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(2);
            pTexture2->Bind();
            if (pTexture2->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_3D);
            pTexture3->Bind();
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_SIGNED_NEGATE_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_NOISE_EMLM_ADD:
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(2);
            pTexture2->Bind();
            if (pTexture2->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_3D);
            pTexture3->Bind();
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_NOISE_EMLM_MODULATE:
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(2);
            pTexture2->Bind();
            if (pTexture2->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_3D);
            pTexture3->Bind();
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_FGLOW:
        {
            glActiveTextureARB(GL_TEXTURE0_ARB);
            glEnable(GL_TEXTURE_2D);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, 0.8f };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_FGLOW_LM:
        {
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            tex1on = 1;
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, 0.8f };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_FGLOW_EM:
        {
            envmapon = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            pTexture1->Bind();
            if (pTexture1->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, 0.8f };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_FGLOW_EMLM:
        {
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(2);
            pTexture2->Bind();
            if (pTexture2->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, 0.8f };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_FGLOW_BUMPY_SHINY:
        {
            GLRender::MakeLookUpTexture(5.0f);
            glEnable(GL_TEXTURE_SHADER_NV);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
            GLRender::SetActiveTexture(1);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            GLRender::SetActiveTexture(2);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture3->Bind();
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, 0.8f };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_FGLOW_BUMPMAP:
        {
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_2D);
            pTexture3->Bind();
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, 0.8f };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_DESAT:
        {
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_DESAT_LM:
        {
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            tex1on = 1;
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_DESAT_EM:
        {
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            pTexture1->Bind();
            if (pTexture1->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_DESAT_EMLM:
        {
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(2);
            pTexture2->Bind();
            if (pTexture2->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_DESAT_BUMPY_SHINY:
        {
            GLRender::MakeLookUpTexture(5.0f);
            glEnable(GL_TEXTURE_SHADER_NV);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
            GLRender::SetActiveTexture(1);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            GLRender::SetActiveTexture(2);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture3->Bind();
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_DESAT_BUMPMAP:
        {
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_2D);
            pTexture3->Bind();
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, 0.8f };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_SUB:
        {
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                SetShaderConstant(0, fDesaturate, 1);
                SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_SIGNED_NEGATE_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_SUB_EM:
        {
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                SetShaderConstant(0, fDesaturate, 1);
                SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_SUB_LM:
        {
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_2D);
                pTexture1->Bind();
                tex1on = 1;
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                SetShaderConstant(0, fDesaturate, 1);
                SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_SUB_EMLM:
        {
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(2);
            pTexture2->Bind();
            if (pTexture2->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_3D);
            pTexture3->Bind();
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_SIGNED_NEGATE_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_ADD:
        {
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                SetShaderConstant(0, fDesaturate, 1);
                SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_ADD_EM:
        {
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                SetShaderConstant(0, fDesaturate, 1);
                SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_ADD_LM:
        {
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_2D);
                pTexture1->Bind();
                tex1on = 1;
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                SetShaderConstant(0, fDesaturate, 1);
                SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_ADD_EMLM:
        {
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(2);
            pTexture2->Bind();
            if (pTexture2->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_3D);
            pTexture3->Bind();
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_MOD:
        {
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                SetShaderConstant(0, fDesaturate, 1);
                SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_MOD_EM:
        {
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                SetShaderConstant(0, fDesaturate, 1);
                SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_MOD_LM:
        {
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_2D);
                pTexture1->Bind();
                tex1on = 1;
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                SetShaderConstant(0, fDesaturate, 1);
                SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_DESAT_NOISE_MOD_EMLM:
        {
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(2);
            pTexture2->Bind();
            if (pTexture2->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_3D);
            pTexture3->Bind();
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            SetShaderConstant(0, fDesaturate, 1);
            SetShaderConstant(1, GLRender::m_ForceSightColor, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_NOISE_DISABLE:
            glEnable(GL_LIGHTING);
            glDisable(GL_TEXTURE_SHADER_NV);
            GLRender::SetActiveTexture(1);
            glDisable(GL_TEXTURE_2D);
            GLRender::SetActiveTexture(0);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            GLRender::SetActiveTexture(0);
            DisableTextureShaders();
            GLRender::SetActiveTexture(1);
            DisableTextureShaders();
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_3D);
            glBindTexture(GL_TEXTURE_3D, 0);
            if (envmapon != 0)
            {
                glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                glDisable(GL_TEXTURE_GEN_S);
                glDisable(GL_TEXTURE_GEN_T);
                glDisable(GL_TEXTURE_GEN_R);
            }
            GLRender::SetActiveTexture(2);
            DisableTextureShaders();
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_3D);
            glBindTexture(GL_TEXTURE_3D, 0);
            if (envmapon != 0)
            {
                glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                glDisable(GL_TEXTURE_GEN_S);
                glDisable(GL_TEXTURE_GEN_T);
                glDisable(GL_TEXTURE_GEN_R);
                envmapon = 0;
            }
            GLRender::SetActiveTexture(3);
            DisableTextureShaders();
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_3D);
            glBindTexture(GL_TEXTURE_3D, 0);
            GLRender::SetActiveTexture(0);
            GLRender::SetCurrentStageTID(0, 0);
            GLRender::SetCurrentStageTID(1, 0);
            GLRender::SetCurrentStageTID(2, 0);
            GLRender::SetCurrentStageTID(3, 0);
            GLRender::DisableCombiners(pTexture0);
            return;

        case MODEL_SHADER_HOLO:
        {
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            GLRender::m_pHologramFilterTexture->Bind();
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, 0.75f };
            SetShaderConstant(0, fDesaturate, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_HOLO_DESAT:
        {
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            GLRender::m_pHologramFilterTexture->Bind();
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, 0.75f };
            SetShaderConstant(0, fDesaturate, 1);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_HOLO_DISABLE:
            GLRender::DisableCombiners(pTexture0);
            DisableTextureShaders();
            GLRender::SetActiveTexture(1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
            GLRender::SetActiveTexture(0);
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            GLRender::SetCurrentStageTID(0, 0);
            GLRender::SetCurrentStageTID(1, 0);
            return;

        case MODEL_SHADER_ILLUM:
            glDisable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
            glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_ILLUM_ENV:
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            pTexture1->Bind();
            if (pTexture1->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
            glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;

        case MODEL_SHADER_ILLUM_NOISE_SUBTRACT:
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_ADD:
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_MODULATE:
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_EM_SUBTRACT:
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(3);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_EM_ADD:
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_EM_MODULATE:
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_LM_SUBTRACT:
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_LM_ADD:
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_LM_MODULATE:
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_EMLM_SUBTRACT:
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture2->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture3->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_EMLM_ADD:
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture2->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture3->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_NOISE_EMLM_MODULATE:
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture2->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture3->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glDisable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            }
            break;

        case MODEL_SHADER_ILLUM_DESAT:
        case MODEL_SHADER_ILLUM_DESAT_LM:
        {
            glEnable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
            glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
            glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
            glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_EM:
        case MODEL_SHADER_ILLUM_DESAT_EMLM:
        {
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            pTexture1->Bind();
            if (pTexture1->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            glEnable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
            glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
            glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
            glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_BUMPY_SHINY:
        {
            GLRender::MakeLookUpTexture(5.0f);
            glEnable(GL_TEXTURE_SHADER_NV);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
            GLRender::SetActiveTexture(1);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            GLRender::SetActiveTexture(2);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture3->Bind();
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
            glEnable(GL_PER_STAGE_CONSTANTS_NV);
            glEnable(GL_REGISTER_COMBINERS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
            glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
            glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
            glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
            glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_BUMPMAP:
        {
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_2D);
            pTexture3->Bind();
            glEnable(GL_PER_STAGE_CONSTANTS_NV);
            glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
            float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, 0.8f };
            glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
            glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
            glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
            glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
            glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
            glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_SUB:
        {
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_SIGNED_NEGATE_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_SUB_LM:
        {
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_SIGNED_NEGATE_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_SUB_EM:
        {
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_SUB_EMLM:
        {
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture2->Bind();
                if (pTexture2->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture3->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_SIGNED_NEGATE_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_ADD:
        {
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_ADD_LM:
        {
            if (pTexture2 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_ADD_EM:
        {
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_ADD_EMLM:
        {
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture2->Bind();
                if (pTexture2->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture3->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_MOD:
        {
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture1->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_MOD_LM:
        {
            if (pTexture1 != 0)
            {
                GLRender::SetActiveTexture(0);
                glEnable(GL_TEXTURE_2D);
                GLRender::SetActiveTexture(1);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_MOD_EM:
        {
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture1->Bind();
                if (pTexture1->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture2->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        case MODEL_SHADER_ILLUM_DESAT_NOISE_MOD_EMLM:
        {
            if (pTexture2 != 0)
            {
                envmapon = 1;
                tex1on = 1;
                GLRender::SetActiveTexture(0);
                pTexture0->Bind();
                GLRender::SetActiveTexture(1);
                pTexture2->Bind();
                if (pTexture2->IsCubeMap())
                {
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    glEnable(GL_TEXTURE_GEN_R);
                }
                else
                {
                    glEnable(GL_TEXTURE_2D);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                }
                GLRender::SetActiveTexture(2);
                glEnable(GL_TEXTURE_3D);
                pTexture3->Bind();
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R_EXT, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_PER_STAGE_CONSTANTS_NV);
                glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
                float fDesaturate[4] = { 0.3f, 0.59f, 0.11f, g_DesatureFactor };
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_PerInstanceIllumination);
                glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR0_NV, fDesaturate);
                glCombinerStageParameterfvNV(GL_COMBINER4_NV, GL_CONSTANT_COLOR1_NV, GLRender::m_ForceSightColor);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_EXPAND_NORMAL_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_RGB);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_TRUE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glCombinerInputNV(GL_COMBINER4_NV, GL_RGB, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
                glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
                glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
                glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
            }
            break;
        }

        default:
            break;

    }
    glEnable(GL_REGISTER_COMBINERS_NV);
}
// PC 0x00425730 GOG 0x00425840
void GLRender::RenderObjectsVTCQuadUP(RenderVertexVTC* pVertices, int nVertices)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(RenderVertexVTC), pVertices->color);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RenderVertexVTC), &pVertices->u);
        glVertexPointer(3, GL_FLOAT, sizeof(RenderVertexVTC), pVertices);
        glDrawArrays(GL_QUADS, 0, nVertices);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    }
    else
    {
        glBegin(GL_QUADS);
        for (int i = 0; i < nVertices; ++i)
        {
            glColor4ubv(pVertices[i].color);
            glTexCoord2f(pVertices[i].u, pVertices[i].v);
            glVertex3fv(pVertices[i].position);
        }
        glEnd();
    }
}
// PC 0x00425820 GOG 0x00425930
void GLRender::RenderObjectsVTQuadUP(RenderVertexVT* pVertices, int nVertices)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RenderVertexVT), &pVertices->u);
        glVertexPointer(3, GL_FLOAT, sizeof(RenderVertexVT), pVertices);
        glDrawArrays(GL_QUADS, 0, nVertices);
    }
    else
    {
        glBegin(GL_QUADS);
        for (int i = 0; i < nVertices; ++i)
        {
            glTexCoord2f(pVertices[i].u, pVertices[i].v);
            glVertex3fv(pVertices[i].position);
        }
        glEnd();
    }
}
// PC 0x004258E0 GOG 0x004259F0
void GLRender::RenderObjectsVCUP(Aurora::AuroraPrimitiveTypes nPrimitive,
                                RenderVertexVC* pVertices, int nVertices)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        EnableClientState(GL_VERTEX_ARRAY);
        EnableClientState(GL_COLOR_ARRAY);
        DisableClientState(GL_TEXTURE_COORD_ARRAY);
        DisableClientState(GL_NORMAL_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(RenderVertexVC), pVertices->color);
        glVertexPointer(3, GL_FLOAT, sizeof(RenderVertexVC), pVertices);
        glDrawArrays(primTypeTranslationTable[nPrimitive], 0, nVertices);
    }
    else
    {
        glBegin(primTypeTranslationTable[nPrimitive]);
        for (int i = 0; i < nVertices; ++i)
        {
            glColor4ubv(pVertices[i].color);
            glVertex3fv(pVertices[i].position);
        }
        glEnd();
    }
}
// PC 0x004259B0 GOG 0x00425AC0
void GLRender::MakeLookUpTexture(float fGamma)
{
    if (!glIsTexture(lookup))
    {
        unsigned char* pPixels = new unsigned char[256 * 256 * 2];
        unsigned char* pPixel = pPixels;
        for (int i = 0; i < 256; ++i)
        {
            unsigned char nGamma =
                (unsigned char)(pow(i / 255.0, fGamma) * 255.99);
            for (int j = 0; j < 256; ++j)
            {
                float fValue = j / 255.0f;
                *pPixel++ = (unsigned char)(fValue * 255.99001f);
                *pPixel++ = nGamma;
            }
        }
        glGenTextures(1, &lookup);
        glBindTexture(GL_TEXTURE_2D, lookup);
        cm_nGLCurrentTID[cm_uiCurrentStage] = 0;
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_LUMINANCE_ALPHA, 256, 256,
                          GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pPixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        delete[] pPixels;
    }
}
// PC 0x00425B10 GOG 0x00425C20
unsigned long GLRender::RequestPool(Aurora::AuroraBufferRequestType nType,
                           unsigned long nSize, unsigned long nUsage,
                           unsigned long nPoolFlags, unsigned long* pnPool)
{
    return *pnPool = manRef->RequestPoolGeneric(nType, nSize, nUsage);
}
// PC 0x00425B40 GOG 0x00425C50
void* GLRender::LockPool(unsigned long nPool, unsigned long nOffset,
                         unsigned long nSize, unsigned long nCount)
{
    return manRef->LockPool(nPool, nCount);
}
// PC 0x00425B60 GOG 0x00425C70
void GLRender::UnlockPool(unsigned long nPool)
{
    manRef->UnlockPool(nPool);
}
// PC 0x00425B80 GOG 0x00425C90
void GLRender::DeletePool(unsigned long nPool)
{
    if (nPool != (unsigned int)-1)
    {
        manRef->DeletePool(nPool);
    }
}
// PC 0x00425BA0 GOG 0x00425CB0
void GLRender::ResizePool(unsigned long nPool, unsigned long nSize)
{
    manRef->ResizePool(nPool, nSize);
}
// PC 0x00425BC0 GOG 0x00425CD0
void GLRender::CreateCubeTexture(CAurTexture* pTexture, bool bMipMaps,
                                 int nIndex)
{
    static int s1[6] = {1, 0, 2, 3, 4, 5};
    static int numrotations[6] = {3, 1, 0, 2, 2, 0};

    for (int i = 0; i < 6; i++)
    {
        int nWidth = pTexture->GetWidth();
        int nHeight = pTexture->GetHeight();
        int nBPP = pTexture->GetBPP();
        int nFaceHeight = nHeight / 6;
        int nTarget = 0x8515 + s1[i];
        unsigned char* pFace =
            pTexture->GetImage() + nWidth * i * nFaceHeight * nBPP;
        int nSubWidth = nWidth / pTexture->GetNumX();
        int nSubHeight = nFaceHeight / pTexture->GetNumY();
        int nX = nIndex % pTexture->GetNumX() * nSubWidth;
        int nY = nIndex / pTexture->GetNumX() * nSubHeight;
        unsigned char* pSubImage;
        if (pTexture->GetNumX() == 1 && pTexture->GetNumY() == 1)
        {
            pSubImage = pFace;
        }
        else
        {
            pSubImage = ImageGetSubImage(pFace, nWidth, nFaceHeight, nBPP, nX, nY,
                                         nSubWidth, nSubHeight);
        }
        unsigned char* pRotated =
            ImageQuarterRotated(pSubImage, nSubWidth, nSubHeight,
                                pTexture->GetBPP(), numrotations[i]);
        int nComponents = pTexture->GetBPP();
        int nFormat;
        if (nComponents == 4)
        {
            nFormat = GL_RGBA;
        }
        else if (nComponents == 3)
        {
            nFormat = GL_RGB;
        }
        else if (nComponents == 2)
        {
            nFormat = GL_LUMINANCE_ALPHA;
        }
        else
        {
            nFormat = GL_LUMINANCE;
        }
        if (pTexture->GetMipMap())
        {
            gluBuild2DMipmaps(nTarget, nComponents, nSubWidth, nSubHeight,
                              nFormat, GL_UNSIGNED_BYTE, pRotated);
        }
        else if (bMipMaps)
        {
            glTexSubImage2D(nTarget, 0, 0, 0, nSubWidth, nSubHeight, nFormat,
                            GL_UNSIGNED_BYTE, pRotated);
        }
        else
        {
            glTexImage2D(nTarget, 0, nComponents, nSubWidth, nSubHeight, 0,
                         nFormat, GL_UNSIGNED_BYTE, pRotated);
        }
        if (pRotated != pSubImage)
        {
            delete pRotated;
        }
        if (pSubImage != pFace)
        {
            delete pSubImage;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R_EXT, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER,
                    pTexture->GetFilter() ? GL_LINEAR : GL_NEAREST);
    if (pTexture->GetMipMap())
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER,
                        pTexture->GetFilter() ? GL_LINEAR_MIPMAP_LINEAR
                                              : GL_NEAREST_MIPMAP_NEAREST);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER,
                        pTexture->GetFilter() ? GL_LINEAR : GL_NEAREST);
    }
}
// PC 0x00425E60 GOG 0x00425F70
void GLRender::CreateProcessedCubeTexture(CAurTexture* pTexture, bool bSubImage,
                                          int nIndex)
{
    int nWidth = pTexture->GetWidth();
    int nHeight = pTexture->GetHeight() / 6;
    int nBPP = pTexture->GetBPP();
    int bMipMap = pTexture->GetMipMap();
    unsigned char* pImage = pTexture->GetImage();
    int nTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
    for (int nFace = 0; nFace < 6; nFace++, nTarget++)
    {
        if (pTexture->GetCompressedSize() > 0)
        {
            int nLevel = 0;
            int nBlockSize = nBPP == 4 ? 16 : 8;
            glCompressedTexImage2DARB(nTarget, 0,
                textureFormatTranslationTable[pTexture->GetCompressionFormat()],
                nWidth, nHeight, 0, pTexture->GetCompressedSize(), pImage);
            if (bMipMap)
            {
                int nSize = pTexture->GetCompressedSize();
                int nMipWidth = nWidth >> 1;
                int nMipHeight = nHeight >> 1;
                while (nMipWidth || nMipHeight)
                {
                    if (nMipWidth == 0)
                        nMipWidth = 1;
                    if (nMipHeight == 0)
                        nMipHeight = 1;
                    nLevel++;
                    pImage += nSize;
                    nSize = ((nMipWidth + 3) / 4) * ((nMipHeight + 3) / 4) * nBlockSize;
                    glCompressedTexImage2DARB(nTarget, nLevel,
                        textureFormatTranslationTable[pTexture->GetCompressionFormat()],
                        nMipWidth, nMipHeight, 0, nSize, pImage);
                    nMipWidth >>= 1;
                    nMipHeight >>= 1;
                }
                pImage += nSize;
            }
        }
        else
        {
            int nFormat = nBPP == 4 ? GL_RGBA : GL_RGB;
            if (bMipMap)
            {
                int nLevel = 0;
                if (bSubImage)
                    glTexSubImage2D(nTarget, 0, 0, 0, nWidth, nHeight, nFormat,
                                    GL_UNSIGNED_BYTE, pImage);
                else
                    glTexImage2D(nTarget, 0, nBPP, nWidth, nHeight, 0, nFormat,
                                 GL_UNSIGNED_BYTE, pImage);
                int nSize = nWidth * nHeight * nBPP;
                int nMipWidth = nWidth >> 1;
                int nMipHeight = nHeight >> 1;
                for (int nMip = pTexture->GetNumMipMaps() - 1; nMip > 0; nMip--)
                {
                    if (nMipWidth == 0)
                        nMipWidth = 1;
                    if (nMipHeight == 0)
                        nMipHeight = 1;
                    nLevel++;
                    pImage += nSize;
                    nSize = nMipWidth * nMipHeight * nBPP;
                    if (bSubImage)
                        glTexSubImage2D(nTarget, nLevel, 0, 0, nMipWidth, nMipHeight,
                                        nFormat, GL_UNSIGNED_BYTE, pImage);
                    else
                        glTexImage2D(nTarget, nLevel, nBPP, nMipWidth, nMipHeight, 0,
                                     nFormat, GL_UNSIGNED_BYTE, pImage);
                    nMipWidth >>= 1;
                    nMipHeight >>= 1;
                }
                pImage += nSize;
            }
            else if (bSubImage)
                glTexSubImage2D(nTarget, 0, 0, 0, nWidth, nHeight, nFormat,
                                GL_UNSIGNED_BYTE, pImage);
            else
                glTexImage2D(nTarget, 0, nBPP, nWidth, nHeight, 0, nFormat,
                             GL_UNSIGNED_BYTE, pImage);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R_EXT, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER,
                     pTexture->GetFilter() ? GL_LINEAR : GL_NEAREST);
    if (pTexture->GetMipMap())
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER,
                         pTexture->GetFilter() ? GL_LINEAR_MIPMAP_LINEAR
                                               : GL_NEAREST_MIPMAP_NEAREST);
    else
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER,
                         pTexture->GetFilter() ? GL_LINEAR : GL_NEAREST);
}
// PC 0x004261B0 GOG 0x004262C0
void AurBuild2DMipMaps(int nSrcWidth, int nSrcHeight, int nBPP,
                       unsigned char* pImage, bool bSubImage)
{
    int nWidth = nSrcWidth;
    int nHeight = nSrcHeight;
    unsigned char* pMipImage;
    if (nWidth != nHeight)
    {
        int nSize = nWidth > nHeight ? nWidth : nHeight;
        pMipImage = ImageScale(pImage, nWidth, nHeight, nBPP, nSize, nSize);
        nHeight = nWidth = nSize;
    }
    else
    {
        pMipImage = new unsigned char[nWidth * nHeight * nBPP];
        memcpy(pMipImage, pImage, nWidth * nHeight * nBPP);
    }

    int nLevel = 0;
    int nType = nBPP == 2 ? GL_UNSIGNED_SHORT_4_4_4_4 : GL_UNSIGNED_BYTE;
    int nFormat;
    if (nBPP == 4)
        nFormat = GL_RGBA;
    else if (nBPP == 3)
        nFormat = GL_RGB;
    else if (nBPP == 2)
        nFormat = GL_RGBA;
    else
        nFormat = GL_LUMINANCE;

    if (!bSubImage)
        glTexImage2D(GL_TEXTURE_2D, nLevel, nBPP, nWidth, nHeight, 0,
                     nFormat, nType, pMipImage);
    else
        glTexSubImage2D(GL_TEXTURE_2D, nLevel, 0, 0, nWidth, nHeight,
                        nFormat, nType, pMipImage);

    while (nWidth * nHeight > 1)
    {
        unsigned char* pNextImage = new unsigned char[(nWidth / 2) * (nHeight / 2) * nBPP];
        for (int nY = 0; nY < nHeight / 2; nY++)
        {
            for (int nX = 0; nX < nWidth / 2; nX++)
            {
                if (nBPP == 2)
                {
                    int nTopLeft = pMipImage[(nY * nWidth + nX) * 4];
                    int nTopRight = pMipImage[(nY * nWidth + nX) * 4 + 2];
                    int nBottomLeft = pMipImage[((nY * 2 + 1) * nWidth + nX * 2) * 2];
                    int nBottomRight = pMipImage[((nY * 2 + 1) * nWidth + nX * 2) * 2 + 2];
                    pNextImage[(nY * (nWidth / 2) + nX) * 2] =
                        (((nTopLeft & AUR_MIPMAP_COMPONENT_MASK) +
                          (nTopRight & AUR_MIPMAP_COMPONENT_MASK) +
                          (nBottomLeft & AUR_MIPMAP_COMPONENT_MASK) +
                          (nBottomRight & AUR_MIPMAP_COMPONENT_MASK)) / 4) |
                        ((((nTopLeft >> 4) + (nTopRight >> 4) +
                           (nBottomLeft >> 4) + (nBottomRight >> 4)) / 4) << 4);
                    nTopLeft = pMipImage[(nY * nWidth + nX) * 4 + 1];
                    nTopRight = pMipImage[(nY * nWidth + nX) * 4 + 3];
                    nBottomLeft = pMipImage[((nY * 2 + 1) * nWidth + nX * 2) * 2 + 1];
                    nBottomRight = pMipImage[((nY * 2 + 1) * nWidth + nX * 2) * 2 + 3];
                    pNextImage[(nY * (nWidth / 2) + nX) * 2 + 1] =
                        (((nTopLeft & AUR_MIPMAP_COMPONENT_MASK) +
                          (nTopRight & AUR_MIPMAP_COMPONENT_MASK) +
                          (nBottomLeft & AUR_MIPMAP_COMPONENT_MASK) +
                          (nBottomRight & AUR_MIPMAP_COMPONENT_MASK)) / 4) |
                        ((((nTopLeft >> 4) + (nTopRight >> 4) +
                           (nBottomLeft >> 4) + (nBottomRight >> 4)) / 4) << 4);
                }
                else
                {
                    for (int nComponent = 0; nComponent < nBPP; nComponent++)
                    {
                        pNextImage[(nY * (nWidth / 2) + nX) * nBPP + nComponent] =
                            (pMipImage[(nY * nWidth + nX) * 2 * nBPP + nComponent] +
                             pMipImage[((nY * 2 + 1) * nWidth + nX * 2) * nBPP + nComponent] +
                             pMipImage[((nY * 2 + 1) * nWidth + nX * 2 + 1) * nBPP + nComponent] +
                             pMipImage[((nY * nWidth + nX) * 2 + 1) * nBPP + nComponent]) / 4;
                    }
                }
            }
        }
        delete pMipImage;
        pMipImage = pNextImage;
        nWidth >>= 1;
        nHeight >>= 1;
        nLevel++;
        if (!bSubImage)
            glTexImage2D(GL_TEXTURE_2D, nLevel, nBPP, nWidth, nHeight, 0,
                         nFormat, nType, pMipImage);
        else
            glTexSubImage2D(GL_TEXTURE_2D, nLevel, 0, 0, nWidth, nHeight,
                            nFormat, nType, pMipImage);
    }
    delete pMipImage;
}
// PC 0x00426570 GOG 0x00426680
void GLRender::CreateTexture(CAurTexture* pTexture, bool bSubImage, int nIndex)
{
    int nHeight = pTexture->GetHeight();
    int nBPP = pTexture->GetBPP();
    unsigned char* pImage = pTexture->GetImage();
    int nNumX = pTexture->GetNumX();
    int nNumY = pTexture->GetNumY();
    int nWidth = pTexture->GetWidth();
    int nSubWidth = nWidth / nNumX;
    int nSubHeight = nHeight / nNumY;
    int nX = nIndex % nNumX * nSubWidth;
    int nY = nIndex / nNumX * nSubHeight;
    unsigned char* pSubImage;
    if (nNumX == 1 && nNumY == 1)
        pSubImage = pImage;
    else
        pSubImage = ImageGetSubImage(pImage, nWidth, nHeight, nBPP, nX, nY,
                                    nSubWidth, nSubHeight);

    if (pTexture->IsBumpMap())
    {
        if (AurBumpMapAvailable())
            BuildTextureNormalMap(pSubImage, pTexture->GetBumpMapScaling(),
                                  nSubWidth, nSubHeight, nBPP, 0,
                                  pTexture->IsNormalMap());
        if (pSubImage != pImage)
            delete pSubImage;
        return;
    }

    int bMipMap = pTexture->GetMipMap();
    if (pTexture->GetCompressedSize() > 0)
    {
        int nLevel = 0;
        int nBlockSize = nBPP == 4 ? 16 : 8;
        unsigned char* pMipImage = pTexture->GetImage();
        glCompressedTexImage2DARB(GL_TEXTURE_2D, 0,
            textureFormatTranslationTable[pTexture->GetCompressionFormat()],
            nWidth, nHeight, 0, pTexture->GetCompressedSize(), pTexture->GetImage());
        if (pTexture->GetMipMap())
        {
            int nSize = pTexture->GetCompressedSize();
            int nMipWidth = nWidth >> 1;
            int nMipHeight = nHeight >> 1;
            while (nMipWidth || nMipHeight)
            {
                if (nMipWidth == 0)
                    nMipWidth = 1;
                if (nMipHeight == 0)
                    nMipHeight = 1;
                nLevel++;
                pMipImage += nSize;
                nSize = ((nMipWidth + 3) / 4) * ((nMipHeight + 3) / 4) * nBlockSize;
                glCompressedTexImage2DARB(GL_TEXTURE_2D, nLevel,
                    textureFormatTranslationTable[pTexture->GetCompressionFormat()],
                    nMipWidth, nMipHeight, 0, nSize, pMipImage);
                nMipWidth >>= 1;
                nMipHeight >>= 1;
            }
        }
    }
    else
    {
        int nFormat;
        if (nBPP == 4)
            nFormat = GL_RGBA;
        else if (nBPP == 3)
            nFormat = GL_RGB;
        else
            nFormat = nBPP == 2 ? GL_RGBA : GL_LUMINANCE;
        int nType;
        int nComponents;
        if (nBPP == 2)
        {
            nType = GL_UNSIGNED_SHORT_4_4_4_4;
            nComponents = 4;
        }
        else
        {
            nType = GL_UNSIGNED_BYTE;
            nComponents = nBPP;
        }
        if (bMipMap)
        {
            if (AurSGIMipmapAvailable())
            {
                glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
                glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nSubWidth, nSubHeight,
                             0, nFormat, nType, pSubImage);
            }
            else if (!bSubImage && nBPP != 2)
                gluBuild2DMipmaps(GL_TEXTURE_2D, nComponents, nSubWidth, nSubHeight,
                                  nFormat, nType, pSubImage);
            else
                AurBuild2DMipMaps(nSubWidth, nSubHeight, nBPP, pSubImage, bSubImage);
        }
        else if (bSubImage)
        {
            if (pTexture->GetDepth())
                glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nSubWidth, nSubHeight,
                                pTexture->GetDepth(), nFormat, GL_UNSIGNED_BYTE, pSubImage);
            else
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nSubWidth, nSubHeight,
                                nFormat, GL_UNSIGNED_BYTE, pSubImage);
        }
        else
        {
            if (pTexture->GetDepth())
                glTexImage3D(GL_TEXTURE_3D, 0, nComponents, nSubWidth, nSubHeight,
                             pTexture->GetDepth(), 0, nFormat, nType, pSubImage);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nSubWidth, nSubHeight,
                             0, nFormat, nType, pSubImage);
        }
    }

    int nClamp = pTexture->GetClamp();
    int bFilter = pTexture->GetFilter();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     nClamp & TEXTURE_CLAMP_S ? GL_CLAMP : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     nClamp & TEXTURE_CLAMP_T ? GL_CLAMP : GL_REPEAT);
    int nMagFilter = bFilter ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nMagFilter);
    int nMinFilter;
    if (bMipMap)
        nMinFilter = bFilter ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
    else
        nMinFilter = nMagFilter;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, nMinFilter);
    if (pSubImage != pImage)
        delete pSubImage;
}
// PC 0x004269B0 GOG 0x00426AC0
int GetCompressedContactSheetOffset(int nIndex, int nWidth, int nBytesPerPixel)
{
    int nMipWidth = nWidth;
    int nBlockSize = nBytesPerPixel == 4 ? 16 : 8;
    int nSize = nBlockSize * ((nWidth + 3) / 4) * ((nWidth + 3) / 4);

    while (nMipWidth > 1)
    {
        nMipWidth >>= 1;
        nSize += nBlockSize * ((nMipWidth + 3) / 4) * ((nMipWidth + 3) / 4);
    }

    return nIndex * nSize;
}
// PC 0x00426A10 GOG 0x00426B20
void GLRender::CreateProcessedTexture(CAurTexture* pTexture, bool bSubImage,
                                      int nIndex)
{
    int nContactSheetIndex = nIndex;
    int nHeight = pTexture->GetHeight();
    int nBPP = pTexture->GetBPP();
    unsigned char* pImage = pTexture->GetImage();
    int nNumX = pTexture->GetNumX();
    int nNumY = pTexture->GetNumY();
    int nWidth = pTexture->GetWidth();
    int nSubWidth = nWidth / nNumX;
    int nSubHeight = nHeight / nNumY;
    unsigned char* pSubImage;
    if (nNumX == 1 && nNumY == 1)
        pSubImage = pImage;
    else
        pSubImage = pImage + GetCompressedContactSheetOffset(nContactSheetIndex, nSubWidth, nBPP);

    int bMipMap = pTexture->GetMipMap();
    if (pTexture->IsBumpMap() && nBPP == 1)
    {
        if (AurBumpMapAvailable())
            BuildTextureNormalMap(pSubImage, pTexture->GetBumpMapScaling(),
                                  nSubWidth, nSubHeight, nBPP, 0,
                                  pTexture->IsNormalMap());
        if (pSubImage != pImage)
            delete pSubImage;
        return;
    }

    if (pTexture->GetCompressedSize() > 0)
    {
        int nLevel = 0;
        unsigned char* pMipImage = pSubImage;
        int nBlockSize = nBPP == 4 ? 16 : 8;
        int nSize = ((nSubWidth + 3) / 4) * ((nSubHeight + 3) / 4) * nBlockSize;
        glCompressedTexImage2DARB(GL_TEXTURE_2D, 0,
            textureFormatTranslationTable[pTexture->GetCompressionFormat()],
            nSubWidth, nSubHeight, 0, nSize, pMipImage);
        if (pTexture->GetMipMap())
        {
            int nMipWidth = nSubWidth >> 1;
            int nMipHeight = nSubHeight >> 1;
            while (nMipWidth || nMipHeight)
            {
                if (nMipWidth == 0)
                    nMipWidth = 1;
                if (nMipHeight == 0)
                    nMipHeight = 1;
                nLevel++;
                pMipImage += nSize;
                nSize = ((nMipWidth + 3) / 4) * ((nMipHeight + 3) / 4) * nBlockSize;
                glCompressedTexImage2DARB(GL_TEXTURE_2D, nLevel,
                    textureFormatTranslationTable[pTexture->GetCompressionFormat()],
                    nMipWidth, nMipHeight, 0, nSize, pMipImage);
                nMipWidth >>= 1;
                nMipHeight >>= 1;
            }
        }
    }
    else
    {
        int nFormat;
        if (nBPP == 4)
            nFormat = GL_RGBA;
        else if (nBPP == 3)
            nFormat = GL_RGB;
        else
            nFormat = nBPP == 2 ? GL_RGBA : GL_LUMINANCE;
        int nType;
        int nComponents;
        if (nBPP == 2)
        {
            nType = GL_UNSIGNED_SHORT_4_4_4_4;
            nComponents = 4;
        }
        else
        {
            nType = GL_UNSIGNED_BYTE;
            nComponents = nBPP;
        }
        if (bMipMap)
        {
            int nLevel = 0;
            unsigned char* pMipImage = pTexture->GetImage();
            if (bSubImage)
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nSubWidth, nSubHeight,
                                nFormat, GL_UNSIGNED_BYTE, pMipImage);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nSubWidth, nSubHeight,
                             0, nFormat, nType, pMipImage);
            if (pTexture->GetMipMap())
            {
                int nSize = nSubWidth * nSubHeight * nBPP;
                int nMipWidth = nSubWidth >> 1;
                int nMipHeight = nSubHeight >> 1;
                for (int nMip = pTexture->GetNumMipMaps() - 1; nMip > 0; nMip--)
                {
                    if (nMipWidth == 0)
                        nMipWidth = 1;
                    if (nMipHeight == 0)
                        nMipHeight = 1;
                    nLevel++;
                    pMipImage += nSize;
                    nSize = nMipWidth * nMipHeight * nBPP;
                    if (bSubImage)
                        glTexSubImage2D(GL_TEXTURE_2D, nLevel, 0, 0, nMipWidth, nMipHeight,
                                        nFormat, nType, pMipImage);
                    else
                        glTexImage2D(GL_TEXTURE_2D, nLevel, nComponents, nMipWidth, nMipHeight,
                                     0, nFormat, nType, pMipImage);
                    nMipWidth >>= 1;
                    nMipHeight >>= 1;
                }
            }
        }
        else if (bSubImage)
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nSubWidth, nSubHeight,
                            nFormat, GL_UNSIGNED_BYTE, pSubImage);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nSubWidth, nSubHeight,
                         0, nFormat, nType, pSubImage);
    }

    if (!pTexture->IsBumpMap())
    {
        int nClamp = pTexture->GetClamp();
        int bFilter = pTexture->GetFilter();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                         nClamp & TEXTURE_CLAMP_S ? GL_CLAMP : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                         nClamp & TEXTURE_CLAMP_T ? GL_CLAMP : GL_REPEAT);
        int nMagFilter = bFilter ? GL_LINEAR : GL_NEAREST;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nMagFilter);
        int nMinFilter;
        if (bMipMap)
            nMinFilter = bFilter ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
        else
            nMinFilter = nMagFilter;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, nMinFilter);
    }
}
// PC 0x00426E90 GOG 0x00426FA0
void GLRender::SetLightmapTexCoordToStage0(unsigned long nStride, int nFlags,
                                         unsigned long nPool, int nVertexCount,
                                         MdlNodeTriMesh* pMesh,
                                         unsigned long nOffset)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        int nPoolBuffer = manRef->GetPool(nPool);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, nPoolBuffer);
        ActivateTextureStage(0);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord1Offset + nOffset));
        ActivateTextureStage(1);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord0Offset + nOffset));
        ActivateTextureStage(0);
    }
    else
    {
        unsigned long nBuffer = manRef->GetPool(nPool) + nOffset;
        ActivateTextureStage(0);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord1Offset + nBuffer));
        ActivateTextureStage(1);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord0Offset + nBuffer));
        ActivateTextureStage(0);
    }
}
// PC 0x00426FE0 GOG 0x004270F0
void GLRender::SetDecalTexCoordToStage0(unsigned long nStride, int nFlags,
                                      unsigned long nPool, int nVertexCount,
                                      MdlNodeTriMesh* pMesh,
                                      unsigned long nOffset)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        int nPoolBuffer = manRef->GetPool(nPool);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, nPoolBuffer);
        ActivateTextureStage(0);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord0Offset + nOffset));
        ActivateTextureStage(1);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord1Offset + nOffset));
        ActivateTextureStage(0);
    }
    else
    {
        unsigned long nBuffer = manRef->GetPool(nPool) + nOffset;
        ActivateTextureStage(0);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord0Offset + nBuffer));
        ActivateTextureStage(1);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord1Offset + nBuffer));
        ActivateTextureStage(0);
    }
}
// PC 0x00427130 GOG 0x00427240
void GLRender::SetInterleavedBuffer(unsigned long nStride, int nFlags,
                                   unsigned long nPool, int nVertexCount,
                                   MdlNodeTriMesh* pMesh, unsigned long nOffset)
{
    unsigned long nBuffer;
    if (AurVertexBufferObjectARBAvailable())
    {
        int nPoolBuffer = manRef->GetPool(nPool);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, nPoolBuffer);
        nBuffer = nOffset;
    }
    else
    {
        nBuffer = manRef->GetPool(nPool) + nOffset;
    }

    if (nFlags & MdlNodeTriMesh::VERTEX_ARRAY_POSITION)
        glVertexPointer(3, GL_FLOAT, pMesh->m_nVertexStride,
                        (const GLvoid*)(pMesh->m_nPositionOffset + nBuffer));
    if (nFlags & MdlNodeTriMesh::VERTEX_ARRAY_NORMAL)
        glNormalPointer(GL_FLOAT, pMesh->m_nVertexStride,
                        (const GLvoid*)(pMesh->m_nNormalOffset + nBuffer));
    if (nFlags & MdlNodeTriMesh::VERTEX_ARRAY_COLOR)
        glColorPointer(4, GL_UNSIGNED_BYTE, pMesh->m_nVertexStride,
                       (const GLvoid*)(pMesh->m_nColorOffset + nBuffer));
    if (nFlags & MdlNodeTriMesh::VERTEX_ARRAY_TEXCOORD0)
    {
        ActivateTextureStage(0);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord0Offset + nBuffer));
    }
    if (nFlags & MdlNodeTriMesh::VERTEX_ARRAY_TEXCOORD1)
    {
        ActivateTextureStage(1);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord1Offset + nBuffer));
    }
    if (nFlags & MdlNodeTriMesh::VERTEX_ARRAY_TEXCOORD2)
    {
        ActivateTextureStage(2);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord2Offset + nBuffer));
    }
    if (nFlags & MdlNodeTriMesh::VERTEX_ARRAY_TEXCOORD3)
    {
        ActivateTextureStage(3);
        glTexCoordPointer(2, GL_FLOAT, pMesh->m_nVertexStride,
                          (const GLvoid*)(pMesh->m_nTexCoord3Offset + nBuffer));
    }
    ActivateTextureStage(0);

    if (AurVertexProgramsAvailable())
    {
        if (nFlags & MdlNodeTriMesh::VERTEX_ARRAY_TANGENT_BASIS)
        {
            {
                const GLvoid* pBasis = (const GLvoid*)(pMesh->m_TangentBases + nBuffer);
                int nVertexStride = pMesh->m_nVertexStride;
                if (usearbvertexprograms)
                    glVertexAttribPointerARB(VERTEX_ATTRIBUTE_TANGENT_BASIS0, 3,
                        GL_FLOAT, GL_FALSE, nVertexStride, pBasis);
                else if (usevertexprograms && AurVertexProgramsAvailable())
                    glVertexAttribPointerNV(VERTEX_ATTRIBUTE_TANGENT_BASIS0, 3,
                        GL_FLOAT, nVertexStride, pBasis);
            }
            {
                const GLvoid* pBasis = (const GLvoid*)(pMesh->m_TangentBases + nBuffer + 12);
                int nVertexStride = pMesh->m_nVertexStride;
                if (usearbvertexprograms)
                    glVertexAttribPointerARB(VERTEX_ATTRIBUTE_TANGENT_BASIS1, 3,
                        GL_FLOAT, GL_FALSE, nVertexStride, pBasis);
                else if (usevertexprograms && AurVertexProgramsAvailable())
                    glVertexAttribPointerNV(VERTEX_ATTRIBUTE_TANGENT_BASIS1, 3,
                        GL_FLOAT, nVertexStride, pBasis);
            }
            {
                const GLvoid* pBasis = (const GLvoid*)(pMesh->m_TangentBases + nBuffer + 24);
                int nVertexStride = pMesh->m_nVertexStride;
                if (usearbvertexprograms)
                    glVertexAttribPointerARB(VERTEX_ATTRIBUTE_TANGENT_BASIS2, 3,
                        GL_FLOAT, GL_FALSE, nVertexStride, pBasis);
                else if (usevertexprograms && AurVertexProgramsAvailable())
                    glVertexAttribPointerNV(VERTEX_ATTRIBUTE_TANGENT_BASIS2, 3,
                        GL_FLOAT, nVertexStride, pBasis);
            }
            glEnableVertexAttribArrayARB(VERTEX_ATTRIBUTE_TANGENT_BASIS0);
            glEnableVertexAttribArrayARB(VERTEX_ATTRIBUTE_TANGENT_BASIS1);
            glEnableVertexAttribArrayARB(VERTEX_ATTRIBUTE_TANGENT_BASIS2);
        }
        else
        {
            glDisableVertexAttribArrayARB(VERTEX_ATTRIBUTE_TANGENT_BASIS0);
            glDisableVertexAttribArrayARB(VERTEX_ATTRIBUTE_TANGENT_BASIS1);
            glDisableVertexAttribArrayARB(VERTEX_ATTRIBUTE_TANGENT_BASIS2);
        }

        if (pMesh->AsMdlNodeSkin())
        {
            {
                const GLvoid* pWeights = (const GLvoid*)(pMesh->AsMdlNodeSkin()->m_nSkinWeightOffset + nBuffer);
                int nVertexStride = pMesh->m_nVertexStride;
                if (usearbvertexprograms)
                    glVertexAttribPointerARB(VERTEX_ATTRIBUTE_SKIN_WEIGHT, 4,
                        GL_FLOAT, GL_FALSE, nVertexStride, pWeights);
                else if (usevertexprograms && AurVertexProgramsAvailable())
                    glVertexAttribPointerNV(VERTEX_ATTRIBUTE_SKIN_WEIGHT, 4,
                        GL_FLOAT, nVertexStride, pWeights);
            }
            {
                const GLvoid* pIndices = (const GLvoid*)(pMesh->AsMdlNodeSkin()->m_nBoneIndexOffset + nBuffer);
                int nVertexStride = pMesh->m_nVertexStride;
                if (usearbvertexprograms)
                    glVertexAttribPointerARB(VERTEX_ATTRIBUTE_BONE_INDEX, 4,
                        GL_FLOAT, GL_FALSE, nVertexStride, pIndices);
                else if (usevertexprograms && AurVertexProgramsAvailable())
                    glVertexAttribPointerNV(VERTEX_ATTRIBUTE_BONE_INDEX, 4,
                        GL_FLOAT, nVertexStride, pIndices);
            }
            glEnableVertexAttribArrayARB(VERTEX_ATTRIBUTE_SKIN_WEIGHT);
            glEnableVertexAttribArrayARB(VERTEX_ATTRIBUTE_BONE_INDEX);
        }
        else
        {
            glDisableVertexAttribArrayARB(VERTEX_ATTRIBUTE_SKIN_WEIGHT);
            glDisableVertexAttribArrayARB(VERTEX_ATTRIBUTE_BONE_INDEX);
        }
    }
}
// PC 0x00427510 GOG 0x00427620
void GLRender::SetVertexBuffer(unsigned long nStride, unsigned long nOffset,
                               unsigned long nPool, int nUsage)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        int nBuffer = manRef->GetPool(nPool);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, nBuffer);
        glVertexPointer(3, GL_FLOAT, nStride, (const GLvoid*)nOffset);
    }
    else
    {
        int nBuffer = manRef->GetPool(nPool);
        glVertexPointer(3, GL_FLOAT, nStride,
                        (const GLvoid*)(nOffset + nBuffer));
    }
}
// PC 0x00427580 GOG 0x00427690
void GLRender::SetColorBuffer(unsigned long nStride, unsigned long nOffset,
                              unsigned long nPool, int nUsage)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        int nBuffer = manRef->GetPool(nPool);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, nBuffer);
        glColorPointer(4, GL_UNSIGNED_BYTE, nStride, (const GLvoid*)nOffset);
    }
    else
    {
        int nBuffer = manRef->GetPool(nPool);
        glColorPointer(4, GL_UNSIGNED_BYTE, nStride,
                       (const GLvoid*)(nOffset + nBuffer));
    }
}
// PC 0x004275F0 GOG 0x00427700
void GLRender::SetNormalBuffer(unsigned int nStride, unsigned int nOffset,
                               unsigned int nPool, int nUsage)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        int nBuffer = manRef->GetPool(nPool);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, nBuffer);
        glNormalPointer(GL_FLOAT, nStride, (const GLvoid*)nOffset);
    }
    else
    {
        int nBuffer = manRef->GetPool(nPool);
        glNormalPointer(GL_FLOAT, nStride, (const GLvoid*)(nOffset + nBuffer));
    }
}
// PC 0x00427650 GOG 0x00427760
void GLRender::SetTexCoordBuffer(unsigned long nStride, unsigned long nOffset,
                                 unsigned long nPool, int nUsage)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        int nBuffer = manRef->GetPool(nPool);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, nBuffer);
        glTexCoordPointer(2, GL_FLOAT, nStride, (const GLvoid*)nOffset);
    }
    else
    {
        int nBuffer = manRef->GetPool(nPool);
        glTexCoordPointer(2, GL_FLOAT, nStride,
                          (const GLvoid*)(nOffset + nBuffer));
    }
}
// PC 0x004276C0 GOG 0x004277D0
void GLRender::DrawElements(Aurora::AuroraPrimitiveTypes nPrimitive,
                            unsigned long nCount, unsigned long nOffset,
                            unsigned long nPool)
{
    int nIndexPool = manRef->GetIndexPool(nPool);
    glDrawElements(primTypeTranslationTable[nPrimitive], nCount,
                   GL_UNSIGNED_SHORT,
                   (const GLvoid*)(nIndexPool + 2 * nOffset));
}
// PC 0x00427700 GOG 0x00427810
void ImposterManager::AllocateImposter(unsigned long nRecord, int nVertices,
                                      float* pVertices)
{
    HDC hOldDC = GLRender::m_hDC;
    HGLRC hOldContext = GLRender::m_hGLRC;
    Record* pRecord = &m_pImposters[nRecord];
    pRecord->m_bAllocated = true;
    pRecord->m_nVertices = nVertices;
    pRecord->m_pVertices = new float[nVertices * 3];
    memcpy(pRecord->m_pVertices, pVertices, nVertices * 3 * sizeof(float));

    unsigned int nTexture;
    glGenTextures(1, &nTexture);
    pRecord->m_nTexture = nTexture;
    glBindTexture(GL_TEXTURE_2D, nTexture);
    GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = 0;

    float fFloatAttributes[1];
    int nPixelFormat;
    unsigned int nNumFormats;
    int nPbufferAttributes[3];
    int nPixelFormatAttributes[5] = {WGL_DRAW_TO_PBUFFER_ARB, 1,
                                    WGL_SUPPORT_OPENGL_ARB, 1, 0};
    fFloatAttributes[0] = 0.0f;
    wglChoosePixelFormatARB(hOldDC, nPixelFormatAttributes, fFloatAttributes, 1,
                            &nPixelFormat, &nNumFormats);
    nPbufferAttributes[0] = WGL_PBUFFER_LARGEST_ARB;
    nPbufferAttributes[1] = 1;
    nPbufferAttributes[2] = 0;
    PBUFFERARB* pBuffer = wglCreatePBufferARB(hOldDC, nPixelFormat, 256, 256,
                                            nPbufferAttributes);
    HDC hBufferDC = wglGetPBufferDCARB(pBuffer);
    HGLRC hContext = wglCreateContext(hBufferDC);
    wglShareLists(hOldContext, hContext);
    wglMakeCurrent(hBufferDC, hContext);
    pRecord->m_hContext = hContext;
    pRecord->m_hBufferDC = hBufferDC;
    pRecord->m_pBuffer = pBuffer;
    wglMakeCurrent(hOldDC, hOldContext);
}
// PC 0x00427840 GOG 0x00427950
int GLRender::InitRenderSystem()
{
    GLRender::m_hGLRC = wglGetCurrentContext();
    GLRender::m_hDC = wglGetCurrentDC();
    memset(GLRender::cm_nGLCurrentTID, 0, sizeof(GLRender::cm_nGLCurrentTID));
    GLRender::cm_uiCurrentStage = 0;
    return 0;
}
// PC 0x00427870 GOG 0x00427980
void GLRender::Shutdown(int bReleaseRenderer)
{
    if (manRef)
        manRef->BackUpPoolManager();

    if (cm_hSoftShadowHDC)
    {
        if (AurATISoftShadowsAvailable())
        {
            wglMakeContextCurrentARB(cm_hSoftShadowHDC, cm_hSoftShadowHDC,
                                    cm_hSoftShadowHGLRC);
            glDeleteTextures(1, &cm_nSoftShadowRenderTexture);
            wglMakeContextCurrentARB(m_hDC, m_hDC, m_hGLRC);
            wglReleasePBufferDCARB(cm_hSoftShadowPBuffer, cm_hSoftShadowHDC);
            wglDestroyPBufferARB(cm_hSoftShadowPBuffer);
            cm_hSoftShadowHDC = 0;
            cm_hSoftShadowHGLRC = 0;
            cm_hSoftShadowPBuffer = 0;
            cm_nSoftShadowRenderTexture = 0;

            wglMakeContextCurrentARB(cm_hSoftShadowTargetHDC,
                                    cm_hSoftShadowTargetHDC,
                                    cm_hSoftShadowTargetHGLRC);
            glDeleteTextures(1, &cm_nSoftShadowTargetRenderTexture);
            wglMakeContextCurrentARB(m_hDC, m_hDC, m_hGLRC);
            wglReleasePBufferDCARB(cm_hSoftShadowTargetPBuffer,
                                  cm_hSoftShadowTargetHDC);
            wglDestroyPBufferARB(cm_hSoftShadowTargetPBuffer);
            cm_hSoftShadowTargetHDC = 0;
            cm_hSoftShadowTargetHGLRC = 0;
            cm_hSoftShadowTargetPBuffer = 0;
            cm_nSoftShadowTargetRenderTexture = 0;
            glDeleteTextures(1, &cm_nBufferRegionTexture);
            cm_nBufferRegionTexture = 0;
        }
        else
        {
            wglMakeCurrent(cm_hSoftShadowHDC, cm_hSoftShadowHGLRC);
            glDeleteTextures(1, &cm_nSoftShadowRenderTexture);
            wglMakeCurrent(m_hDC, m_hGLRC);
            wglDeleteContext(cm_hSoftShadowHGLRC);
            wglReleasePBufferDCARB(cm_hSoftShadowPBuffer, cm_hSoftShadowHDC);
            wglDestroyPBufferARB(cm_hSoftShadowPBuffer);
            cm_hSoftShadowHDC = 0;
            cm_hSoftShadowHGLRC = 0;
            cm_hSoftShadowPBuffer = 0;
            cm_nSoftShadowRenderTexture = 0;

            wglMakeCurrent(cm_hSoftShadowTargetHDC, cm_hSoftShadowTargetHGLRC);
            glDeleteTextures(1, &cm_nSoftShadowTargetRenderTexture);
            wglMakeCurrent(m_hDC, m_hGLRC);
            wglDeleteContext(cm_hSoftShadowTargetHGLRC);
            wglReleasePBufferDCARB(cm_hSoftShadowTargetPBuffer,
                                  cm_hSoftShadowTargetHDC);
            wglDestroyPBufferARB(cm_hSoftShadowTargetPBuffer);
            cm_hSoftShadowTargetHDC = 0;
            cm_hSoftShadowTargetHGLRC = 0;
            cm_hSoftShadowTargetPBuffer = 0;
            cm_nSoftShadowTargetRenderTexture = 0;
            glDeleteTextures(1, &cm_nBufferRegionTexture);
            cm_nBufferRegionTexture = 0;
        }
    }

    if (cm_nTargetTexture)
    {
        glDeleteTextures(1, &cm_nTargetTexture);
        cm_nTargetTexture = 0;
    }
    if (cm_nRedTexture)
    {
        glDeleteTextures(1, &cm_nRedTexture);
        cm_nRedTexture = 0;
    }
    if (cm_nGreenTexture)
    {
        glDeleteTextures(1, &cm_nGreenTexture);
        cm_nGreenTexture = 0;
    }
    if (cm_nBlueTexture)
    {
        glDeleteTextures(1, &cm_nBlueTexture);
        cm_nBlueTexture = 0;
    }
    if (cm_nOverbrightenRed)
    {
        glDeleteTextures(1, &cm_nOverbrightenRed);
        cm_nOverbrightenRed = 0;
    }
    if (cm_nOverbrightenGreen)
    {
        glDeleteTextures(1, &cm_nOverbrightenGreen);
        cm_nOverbrightenGreen = 0;
    }
    if (cm_nOverbrightenBlue)
    {
        glDeleteTextures(1, &cm_nOverbrightenBlue);
        cm_nOverbrightenBlue = 0;
    }
    if (cm_nPerPixelFlareShader)
    {
        glDeleteProgramsARB(1, &cm_nPerPixelFlareShader);
        cm_nPerPixelFlareShader = 0;
    }

    if (cm_bPBuffersAllocated == true)
    {
        if (AurATIFrameBufferModificationsAvailable())
        {
            for (int i = 0; i < CONVOLUTION_BUFFER_COUNT; ++i)
            {
                wglMakeContextCurrentARB(cm_hConvolutionHDC[i],
                                        cm_hConvolutionHDC[i],
                                        cm_hConvolutionHGLRC[i]);
                glDeleteTextures(1, &cm_nConvolutionRenderTexture[i]);
                wglMakeContextCurrentARB(m_hDC, m_hDC, m_hGLRC);
                wglReleasePBufferDCARB(cm_hConvolutionPBuffer[i],
                                      cm_hConvolutionHDC[i]);
                wglDestroyPBufferARB(cm_hConvolutionPBuffer[i]);
                cm_hConvolutionHDC[i] = 0;
                cm_hConvolutionHGLRC[i] = 0;
                cm_hConvolutionPBuffer[i] = 0;
                cm_nConvolutionRenderTexture[i] = 0;
            }
            wglMakeContextCurrentARB(cm_hAccumulationHDC,
                                    cm_hAccumulationHDC,
                                    cm_hAccumulationHGLRC);
            glDeleteTextures(1, &cm_nAccumulationBlur);
            wglMakeContextCurrentARB(m_hDC, m_hDC, m_hGLRC);
            wglReleasePBufferDCARB(cm_hAccumulationPBuffer,
                                  cm_hAccumulationHDC);
            wglDestroyPBufferARB(cm_hAccumulationPBuffer);
            cm_hAccumulationHDC = 0;
            cm_hAccumulationHGLRC = 0;
            cm_hAccumulationPBuffer = 0;
            cm_nAccumulationBlur = 0;
        }
        else
        {
            for (int i = 0; i < CONVOLUTION_BUFFER_COUNT; ++i)
            {
                wglMakeCurrent(cm_hConvolutionHDC[i], cm_hConvolutionHGLRC[i]);
                glDeleteTextures(1, &cm_nConvolutionRenderTexture[i]);
                wglMakeCurrent(m_hDC, m_hGLRC);
                wglDeleteContext(cm_hConvolutionHGLRC[i]);
                wglReleasePBufferDCARB(cm_hConvolutionPBuffer[i],
                                      cm_hConvolutionHDC[i]);
                wglDestroyPBufferARB(cm_hConvolutionPBuffer[i]);
                cm_hConvolutionHDC[i] = 0;
                cm_hConvolutionHGLRC[i] = 0;
                cm_hConvolutionPBuffer[i] = 0;
                cm_nConvolutionRenderTexture[i] = 0;
            }
            if (cm_hAccumulationPBuffer)
            {
                wglMakeCurrent(cm_hAccumulationHDC, cm_hAccumulationHGLRC);
                glDeleteTextures(1, &cm_nAccumulationBlur);
                wglMakeCurrent(m_hDC, m_hGLRC);
                wglDeleteContext(cm_hAccumulationHGLRC);
                wglReleasePBufferDCARB(cm_hAccumulationPBuffer,
                                      cm_hAccumulationHDC);
                wglDestroyPBufferARB(cm_hAccumulationPBuffer);
                cm_hAccumulationHDC = 0;
                cm_hAccumulationHGLRC = 0;
                cm_hAccumulationPBuffer = 0;
                cm_nAccumulationBlur = 0;
            }
        }
        cm_bPBuffersAllocated = false;
    }
    if (cm_pHeatDistortionVertexProg)
    {
        glDeleteProgramsARB(1, &cm_pHeatDistortionVertexProg);
        cm_pHeatDistortionVertexProg = 0;
    }
    if (cm_nPerPixelFlareShaderATI)
    {
        glDeleteFragmentShaderATI(cm_nPerPixelFlareShaderATI);
        cm_nPerPixelFlareShaderATI = 0;
        glDeleteFragmentShaderATI(cm_nDecalShaderATI);
        cm_nDecalShaderATI = 0;
        glDeleteFragmentShaderATI(cm_n4DecalShaderATI);
        cm_n4DecalShaderATI = 0;
        glDeleteFragmentShaderATI(cm_nSaturationShaderATI);
        cm_nSaturationShaderATI = 0;
        glDeleteFragmentShaderATI(cm_nAccumulationBlurShaderATI);
        cm_nAccumulationBlurShaderATI = 0;
        glDeleteFragmentShaderATI(cm_nFilmNoiseShaderATI);
        cm_nFilmNoiseShaderATI = 0;
        glDeleteFragmentShaderATI(cm_nForceDistortionShaderATI);
        cm_nForceDistortionShaderATI = 0;
        glDeleteFragmentShaderATI(cm_nOverbrightenShaderATI);
        cm_nOverbrightenShaderATI = 0;
        glDeleteFragmentShaderATI(cm_nDreamDistortShaderATI);
        cm_nDreamDistortShaderATI = 0;
    }
    ReleaseDC(g_hRenderWnd, m_hDC);
    if (bReleaseRenderer)
    {
        delete manRef;
        manRef = 0;
    }
}
// PC 0x00427E20 GOG 0x00427F30
int GLRender::GetFogArraysInfoRef()
{
    int nPool = manRef->RequestPoolGeneric(0, 8, 2);
    int* pFogArrays = (int*)manRef->LockPool(nPool, 1);
    RequestPoolManager* pPoolManager = manRef;
    pFogArrays[0] = 0;
    pFogArrays[1] = 0;
    pPoolManager->UnlockPool(nPool);
    return nPool;
}
// PC 0x00427E60 GOG 0x00427F70
void GLRender::SetViewTransform(Vector& vPosition, Quaternion& qOrientation)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    if (multmatrix != 0)
    {
        Vector vOrigin;
        float fMatrix[16];
        MakeMatrix4x4(fMatrix, vOrigin, -qOrientation);
        MultMatrix(fMatrix);
    }
    else
    {
        Vector vAxis = qOrientation.axis();
        float fAngle = (float)(acos(qOrientation.m_fW) * 2.0) * -RAD_TO_DEG;
        Rotate(fAngle, vAxis.m_fX, vAxis.m_fY, vAxis.m_fZ);
    }
    Translate(-vPosition.m_fX, -vPosition.m_fY, -vPosition.m_fZ);
}
// PC 0x00427F50 GOG 0x00428060
static void CalculateCubeMapNormal(int nFace, int nSize, int nX, int nY,
                                   float* pNormal)
{
    float s = 2.0f * ((nX + 0.5f) / nSize) - 1.0f;
    float t = 2.0f * ((nY + 0.5f) / nSize) - 1.0f;
    switch (nFace)
    {
    case 0: pNormal[0] = 1.0f; pNormal[1] = -t; pNormal[2] = -s; break;
    case 1: pNormal[0] = -1.0f; pNormal[1] = -t; pNormal[2] = s; break;
    case 2: pNormal[0] = s; pNormal[1] = 1.0f; pNormal[2] = t; break;
    case 3: pNormal[0] = s; pNormal[1] = -1.0f; pNormal[2] = -t; break;
    case 4: pNormal[0] = s; pNormal[1] = -t; pNormal[2] = 1.0f; break;
    case 5: pNormal[0] = -s; pNormal[1] = -t; pNormal[2] = -1.0f; break;
    }
    float fScale = 1.0f / (float)sqrt(pNormal[0] * pNormal[0] +
                             pNormal[1] * pNormal[1] +
                             pNormal[2] * pNormal[2]);
    pNormal[0] *= fScale;
    pNormal[1] *= fScale;
    pNormal[2] *= fScale;
}
// PC 0x00428070 GOG 0x00428180
void GLRender::MakeNormalizationCubeMap(int nSize)
{

    static int mapping[6] = {2, 3, 4, 5, 6, 7};
    unsigned char* pPixels = new unsigned char[nSize * nSize * 3];
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    for (int nFace = 0; nFace < 6; ++nFace)
    {
        for (int y = 0; y < nSize; ++y)
        {
            for (int x = 0; x < nSize; ++x)
            {
                float normal[3];
                CalculateCubeMapNormal(nFace, nSize, x, y, normal);
                pPixels[3 * (y * nSize + x) + 0] = (unsigned char)(normal[0] * 127.0f + 128.0f);
                pPixels[3 * (y * nSize + x) + 1] = (unsigned char)(normal[1] * 127.0f + 128.0f);
                pPixels[3 * (y * nSize + x) + 2] = (unsigned char)(normal[2] * 127.0f + 128.0f);
            }
        }
        gluBuild2DMipmaps(texParamTargetTranslationTable[mapping[nFace]],
                          3, nSize, nSize, GL_RGB, GL_UNSIGNED_BYTE, pPixels);
    }
    delete[] pPixels;
}
// PC 0x004281D0 GOG 0x004282E0
void GLRender::RenderSquares(unsigned int nPool)
{
    int nVBOAvailable = AurVertexBufferObjectARBAvailable();
    RequestPoolManager* pPoolManager = manRef;
    if (nVBOAvailable != 0)
    {
        int nBuffer = pPoolManager->GetPool(nPool);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, nBuffer);
        glVertexPointer(3, GL_FLOAT, 16, 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, 16, (const GLvoid*)12);
        DrawPrimitive(GL_LINE_LOOP, 0, 4);
    }
    else
    {
        int nVertexData = pPoolManager->GetPool(nPool);
        glBegin(GL_LINE_LOOP);
        glColor4ubv((const GLubyte*)(nVertexData + 12));
        for (int i = 0; i < 4; ++i)
        {
            glVertex3fv((const GLfloat*)nVertexData);
            nVertexData += 16;
        }
        glEnd();
    }
}
// PC 0x00428270 GOG 0x00428380
void GLRender::ConvertHeightFieldAndLoadNormalMapTexture(
    unsigned char* pHeightField, int nWidth, int nHeight, int nRowWidth,
    int nRowCount, float fScale, int bUpdate, bool bNormalMap)
{
    Normal* pNormalMap;
    if (bNormalMap)
    {
        pNormalMap = new Normal[nWidth * nHeight];
        memcpy(pNormalMap, pHeightField, nWidth * nHeight * 4);
    }
    else
    {
        pNormalMap = convertHeightFieldToNormalMap(
            pHeightField, nWidth, nHeight, nRowWidth, nRowCount, fScale);
    }

    int nLevel = 0;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0,
                 GL_BGRA_EXT, GL_UNSIGNED_BYTE, pNormalMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, cm_nAnisotropy);

    while (nWidth > 1 || nHeight > 1)
    {
        ++nLevel;
        int nNewWidth = nWidth >> 1;
        int nNewHeight = nHeight >> 1;
        if (nNewWidth == 0)
            nNewWidth = 1;
        if (nNewHeight == 0)
            nNewHeight = 1;
        pNormalMap = downSampleNormalMap(pNormalMap, nWidth, nHeight,
                                          nNewWidth, nNewHeight);
        if (bUpdate)
        {
            glTexSubImage2D(GL_TEXTURE_2D, nLevel, 0, 0, nNewWidth,
                            nNewHeight, GL_RGBA, GL_UNSIGNED_BYTE, pNormalMap);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, nLevel, GL_RGBA8, nNewWidth,
                         nNewHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE,
                         pNormalMap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                             cm_nAnisotropy);
        }
        nWidth = nNewWidth;
        nHeight = nNewHeight;
    }
    delete[] pNormalMap;
}
// PC 0x00428400 GOG 0x00428510
void GLRender::DrawLightmappedGrass(void* pVertices, int nVertexStride,
                                    void* pIndices, int nStride, int nFirst,
                                    int nQuads)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glVertexPointer(3, GL_FLOAT, nVertexStride, pVertices);
        glNormalPointer(GL_FLOAT, nStride, (float*)pVertices + nQuads * 12);
        glTexCoordPointer(2, GL_FLOAT, nStride, (float*)pVertices + nQuads * 12 + 3);
        ActivateTextureStage(1);
        glTexCoordPointer(2, GL_FLOAT, nStride, (float*)pVertices + nQuads * 12 + 5);
        ActivateTextureStage(0);
        glDrawArrays(GL_QUADS, nFirst, nQuads * 4);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    }
    else
    {
        glVertexPointer(3, GL_FLOAT, nVertexStride, pVertices);
        glNormalPointer(GL_FLOAT, nStride, (float*)pVertices + nQuads * 12);
        glTexCoordPointer(2, GL_FLOAT, nStride, (float*)pVertices + nQuads * 12 + 3);
        ActivateTextureStage(1);
        glTexCoordPointer(2, GL_FLOAT, nStride, (float*)pVertices + nQuads * 12 + 5);
        ActivateTextureStage(0);
        glDrawArrays(GL_QUADS, nFirst, nQuads * 4);
    }
}
// PC 0x00428590 GOG 0x004286A0
void GLRender::InitAttenuationTexture()
{
    unsigned char pixels[64 * 64 * 4];

    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            float fX = (float)(x - 32) * 0.125f;
            float fY = (float)(y - 32) * 0.125f;
            float fValue = 1.0f / ((fX * fX + fY * fY) * 4.5f + 1.0f);
            if (x == 0 || y == 0 || x == 63 || y == 63)
            {
                fValue = 0.0f;
            }
            unsigned char nValue = (unsigned char)(fValue * 255.0f);
            pixels[(y * 64 + x) * 4 + 0] = nValue;
            pixels[(y * 64 + x) * 4 + 1] = nValue;
            pixels[(y * 64 + x) * 4 + 2] = nValue;
            pixels[(y * 64 + x) * 4 + 3] = 0xFF;
        }
    }

    glGenTextures(1, &cm_tidAttenuationTexture);
    BindTexture(ENABLE_TEXTURE_2D, cm_tidAttenuationTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 64, 64, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, cm_nAnisotropy);
}
// PC 0x00428720 GOG 0x00428830
int GLRender::SetCurrentStageTID(unsigned long nStage, unsigned int nTID)
{
    GLRender::cm_nGLCurrentTID[nStage] = nTID;
    return nTID;
}
// PC 0x00428730 GOG 0x00428840
PBUFFERARB* GLRender::CreatePBuffer(int nWidth, int nHeight)
{
    HDC hDC = m_hDC;
    float fFloatAttributes;
    int nPixelFormat;
    unsigned int nNumFormats;
    int nPbufferAttributes[6];
    int nPixelFormatAttributes[17] = {8208, 1, 8237, 1, 8305, 1,
                                      8213, 8, 8215, 8, 8217, 8,
                                      8219, 8, 8209, 0, 0};

    fFloatAttributes = 0.0f;
    wglChoosePixelFormatARB(hDC, nPixelFormatAttributes, &fFloatAttributes, 1,
                            &nPixelFormat, &nNumFormats);

    nPbufferAttributes[0] = 8306;
    nPbufferAttributes[1] = 8310;
    nPbufferAttributes[2] = 8307;
    nPbufferAttributes[3] = 8314;
    nPbufferAttributes[4] = 0;
    nPbufferAttributes[5] = 0;
    return wglCreatePBufferARB(hDC, nPixelFormat, nWidth, nHeight,
                               nPbufferAttributes);
}
// PC 0x00428820 GOG 0x00428930
void GLRender::SetDirtyTexture(int nIndex, CAurTexture* pTexture)
{
    m_pDirtyTexture[nIndex] = pTexture;
}
// PC 0x00428830 GOG 0x00428940
void GLRender::SetFilmNoiseTexture(CAurTexture* pTexture)
{
    GLRender::cm_pFilmNoiseTexture = pTexture;
}
// PC 0x00428840 GOG 0x00428950
void GLRender::RenderShadowArray(int nVertices, int nEdges, int nPool)
{
    static unsigned short* indexToken = new unsigned short[AUR_SHADOW_INDEX_COUNT];
    unsigned short* pIndex = indexToken;
    if (nEdges == 0)
        return;

    unsigned short* indices = pIndex;
    int nNear = nVertices * 2;
    int nFar = nNear + 1;
    for (int i = 0; i < nEdges; ++i)
    {
        int nFirst = m_UsedVertices[PartTriMesh::m_VertexIndices[i * 2]] * 2 - 2;
        int nSecond = m_UsedVertices[PartTriMesh::m_VertexIndices[i * 2 + 1]] * 2 - 2;
        if (i % 2 == 0)
        {
            *pIndex++ = nNear;
            *pIndex++ = nFirst;
            *pIndex++ = nSecond;
            *pIndex++ = nFirst + 1;
            *pIndex++ = nSecond + 1;
            *pIndex++ = nFar;
        }
        else
        {
            *pIndex++ = nFar;
            *pIndex++ = nSecond + 1;
            *pIndex++ = nFirst + 1;
            *pIndex++ = nSecond;
            *pIndex++ = nFirst;
            *pIndex++ = nNear;
        }
    }

    if (AurVertexBufferObjectARBAvailable())
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glVertexPointer(3, GL_FLOAT, 12, cm_pUserShadowData);

    if (usetwosidedshadows && AurTwoSidedStencilAvailable())
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
        glActiveStencilFaceEXT(GL_BACK);
        SetStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP_EXT);
        SetStencilMask(-1);
        SetStencilFunc(GL_ALWAYS, 0, -1);
        glActiveStencilFaceEXT(GL_FRONT);
        SetStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP_EXT);
        SetStencilMask(-1);
        if (AurVertexBufferObjectARBAvailable())
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        DrawIndexedPrimitive(GL_TRIANGLE_STRIP, nEdges * 6, indices);
        glEnable(GL_CULL_FACE);
        SetFrontFace(GL_CCW);
        glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
    }
    else
    {
        glFrontFace(GL_CW);
        glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
        if (AurVertexBufferObjectARBAvailable())
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        glDrawElements(GL_TRIANGLE_STRIP, nEdges * 6, GL_UNSIGNED_SHORT, indices);
        glFrontFace(GL_CCW);
        glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
        if (AurVertexBufferObjectARBAvailable())
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        glDrawElements(GL_TRIANGLE_STRIP, nEdges * 6, GL_UNSIGNED_SHORT, indices);
    }
    countfaces += nEdges * 8;
}
// PC 0x00428B20 GOG 0x00428C30
char* AllocateShadowMemory(int nSize)
{
    static char* data = new char[nSize];
    static int c_size = nSize;

    if (nSize > maxshadowvar)
    {
        maxshadowvar = nSize;
    }

    if (nSize > c_size)
    {
        delete[] data;
        data = new char[nSize];
        c_size = nSize;
    }

    return data;
}
// PC 0x00428BD0 GOG 0x00428CE0
int GLRender::BuildShadowArray(const Vector& vLight, const float& fRadius,
                               const Plane& plane, const Vector& vNearCap,
                               const Vector& vFarCap, int nVertices,
                               RenderVertexV* pVertices)
{
    static Vector light2temp;
    static Vector scratch[2];
    int nUsed = 0;
    for (int i = 0; i < nVertices; ++i)
    {
        if (m_UsedVertices[i] == SHADOW_VERTEX_USED)
            m_UsedVertices[i] = ++nUsed;
    }
    int nSize = nUsed * sizeof(scratch);
    cm_pUserShadowData = (float*)AllocateShadowMemory(nSize + sizeof(scratch));
    char* pShadowData = (char*)cm_pUserShadowData;
    for (int iVertex = 0; iVertex < nVertices; ++iVertex)
    {
        int nOffset = m_UsedVertices[iVertex] * 6 - 6;
        if (m_UsedVertices[iVertex] > 0)
        {
            scratch[0] = pVertices[iVertex].position;
            light2temp = normalize(scratch[0] - vLight);
            scratch[1] = vLight + light2temp * fRadius;
            if (!noclipshadows)
            {
                ClipProjection(scratch[0], vLight, plane);
                ClipProjection(scratch[1], vLight, plane);
            }
            if (dontprotectagainstclip &&
                (scratch[0] - scratch[1]) * light2temp > 0.0f)
                scratch[1] = scratch[0];
            memcpy(pShadowData + nOffset * sizeof(float), scratch, sizeof(scratch));
        }
    }
    char* pCap = pShadowData + nSize;
    *(Vector*)pCap = vNearCap;
    pCap += sizeof(Vector);
    *(Vector*)pCap = vFarCap;
    return nUsed;
}
// PC 0x00428EA0 GOG 0x00428FB0
void GLRender::InitializeImageSpaceSoftShadows()
{
    if (AurSoftShadowsAvailable() && cm_bSetUpImageSpaceShadows && enablesoftshadows)
    {
        if (cm_nTargetTexture == 0)
        {
            glGenTextures(1, &cm_nTargetTexture);
            glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
            glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8, 0, 0,
                             g_nScreenWidth, g_nScreenHeight, 0);
            glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
        }
        glGenTextures(1, &cm_nBufferRegionTexture);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nBufferRegionTexture);
        glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8, 0, 0,
                         g_nScreenWidth, g_nScreenHeight, 0);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        cm_hSoftShadowPBuffer = CreatePBuffer(512, 512);
        cm_hSoftShadowHDC = wglGetPBufferDCARB(cm_hSoftShadowPBuffer);
        cm_hSoftShadowHGLRC = wglCreateContext(cm_hSoftShadowHDC);
        wglShareLists(m_hGLRC, cm_hSoftShadowHGLRC);
        wglMakeCurrent(cm_hSoftShadowHDC, cm_hSoftShadowHGLRC);
        glGenTextures(1, &cm_nSoftShadowRenderTexture);
        glBindTexture(GL_TEXTURE_2D, cm_nSoftShadowRenderTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        cm_hSoftShadowTargetPBuffer = CreatePBuffer(512, 512);
        cm_hSoftShadowTargetHDC = wglGetPBufferDCARB(cm_hSoftShadowTargetPBuffer);
        cm_hSoftShadowTargetHGLRC = wglCreateContext(cm_hSoftShadowTargetHDC);
        wglShareLists(m_hGLRC, cm_hSoftShadowTargetHGLRC);
        wglMakeCurrent(cm_hSoftShadowTargetHDC, cm_hSoftShadowTargetHGLRC);
        glGenTextures(1, &cm_nSoftShadowTargetRenderTexture);
        glBindTexture(GL_TEXTURE_2D, cm_nSoftShadowTargetRenderTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        wglMakeCurrent(m_hDC, m_hGLRC);
    }
    else if (AurATISoftShadowsAvailable() && cm_bSetUpImageSpaceShadows && enablesoftshadows)
    {

        void FindSmallestPowerOf2WidthHeight(unsigned int, unsigned int, unsigned long&, unsigned long&);
        FindSmallestPowerOf2WidthHeight(g_nScreenWidth, g_nScreenHeight, cm_nScreenWidthPow2, cm_nScreenHeightPow2);
        if (cm_nTargetTexture == 0)
        {
            glGenTextures(1, &cm_nTargetTexture);
            glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, cm_nScreenWidthPow2, cm_nScreenHeightPow2, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glGenTextures(1, &cm_nBufferRegionTexture);
        glBindTexture(GL_TEXTURE_2D, cm_nBufferRegionTexture);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, cm_nScreenWidthPow2, cm_nScreenHeightPow2, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        cm_hSoftShadowPBuffer = CreatePBuffer(512, 512);
        cm_hSoftShadowHDC = wglGetPBufferDCARB(cm_hSoftShadowPBuffer);
        cm_hSoftShadowHGLRC = m_hGLRC;
        wglMakeContextCurrentARB(cm_hSoftShadowHDC, cm_hSoftShadowHDC, m_hGLRC);
        glGenTextures(1, &cm_nSoftShadowRenderTexture);
        glBindTexture(GL_TEXTURE_2D, cm_nSoftShadowRenderTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        wglMakeContextCurrentARB(m_hDC, m_hDC, m_hGLRC);
        cm_hSoftShadowTargetPBuffer = CreatePBuffer(512, 512);
        cm_hSoftShadowTargetHDC = wglGetPBufferDCARB(cm_hSoftShadowTargetPBuffer);
        cm_hSoftShadowTargetHGLRC = m_hGLRC;
        wglMakeContextCurrentARB(cm_hSoftShadowTargetHDC, cm_hSoftShadowTargetHDC, m_hGLRC);
        glGenTextures(1, &cm_nSoftShadowTargetRenderTexture);
        glBindTexture(GL_TEXTURE_2D, cm_nSoftShadowTargetRenderTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        wglMakeContextCurrentARB(m_hDC, m_hDC, m_hGLRC);
        glEnable(GL_FRAGMENT_SHADER_ATI);
        cm_nSoftShadowShaderATI = glGenFragmentShadersATI(1);
        glBindFragmentShaderATI(cm_nSoftShadowShaderATI);
        glBeginFragmentShaderATI();
        glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
        glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
        glSampleMapATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STR_ATI);
        glSampleMapATI(GL_REG_3_ATI, GL_TEXTURE3_ARB, GL_SWIZZLE_STR_ATI);
        glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_4_ATI, 0, 0,
                              GL_REG_0_ATI, 0, 0, GL_CON_0_ATI, 0, 0);
        glAlphaFragmentOp2ATI(GL_MUL_ATI, GL_REG_4_ATI, 0,
                              GL_REG_0_ATI, 0, 0, GL_CON_0_ATI, 0, 0);
        glColorFragmentOp3ATI(GL_MAD_ATI, GL_REG_4_ATI, 0, 0,
                              GL_REG_1_ATI, 0, 0, GL_CON_1_ATI, 0, 0,
                              GL_REG_4_ATI, 0, 0);
        glAlphaFragmentOp3ATI(GL_MAD_ATI, GL_REG_4_ATI, 0,
                              GL_REG_1_ATI, 0, 0, GL_CON_1_ATI, 0, 0,
                              GL_REG_4_ATI, 0, 0);
        glColorFragmentOp3ATI(GL_MAD_ATI, GL_REG_1_ATI, 0, 0,
                              GL_REG_2_ATI, 0, 0, GL_CON_2_ATI, 0, 0,
                              GL_REG_4_ATI, 0, 0);
        glAlphaFragmentOp3ATI(GL_MAD_ATI, GL_REG_1_ATI, 0,
                              GL_REG_2_ATI, 0, 0, GL_CON_2_ATI, 0, 0,
                              GL_REG_4_ATI, 0, 0);
        glColorFragmentOp3ATI(GL_MAD_ATI, GL_REG_0_ATI, 0, 0,
                              GL_REG_3_ATI, 0, 0, GL_CON_3_ATI, 0, 0,
                              GL_REG_1_ATI, 0, 0);
        glAlphaFragmentOp3ATI(GL_MAD_ATI, GL_REG_0_ATI, 0,
                              GL_REG_3_ATI, 0, 0, GL_CON_3_ATI, 0, 0,
                              GL_REG_1_ATI, 0, 0);
        glEndFragmentShaderATI();
        glDisable(GL_FRAGMENT_SHADER_ATI);
    }
}
// PC 0x004295C0 GOG 0x004296D0
unsigned int log2integer(unsigned int x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return ones32popcount(x >> 1);
}
// PC 0x00429630 GOG 0x00429740
void FindSmallestPowerOf2WidthHeight(unsigned int nWidth, unsigned int nHeight,
                                    unsigned long& nWidthPow2,
                                    unsigned long& nHeightPow2)
{
    nWidthPow2 = 1 << (log2integer(nWidth) + 1);
    nHeightPow2 = 1 << (log2integer(nHeight) + 1);
}
// PC 0x00429670 GOG 0x00429780
void GLRender::InitializeFrameBufferModificationFragmentShadersATI()
{
    glEnable(GL_FRAGMENT_SHADER_ATI);
    cm_nOverbrightenShaderATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(cm_nOverbrightenShaderATI);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_1_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_0_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_2_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_1_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_3_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_2_ATI, 0, 0);
    glPassTexCoordATI(GL_REG_0_ATI, GL_REG_0_ATI, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_REG_1_ATI, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_2_ATI, GL_REG_2_ATI, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_3_ATI, GL_REG_3_ATI, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, 0, 0, GL_REG_1_ATI, 0, 0, GL_REG_2_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_REG_3_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_PRIMARY_COLOR_ARB, 0, 0);
    glEndFragmentShaderATI();
    cm_nForceDistortionShaderATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(cm_nForceDistortionShaderATI);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_4_ATI, GL_RED_BIT_ATI, 0, GL_REG_0_ATI, 0, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI, GL_REG_1_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_4_ATI, GL_GREEN_BIT_ATI, 0, GL_REG_0_ATI, 0, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI, GL_REG_2_ATI, 0, 0);
    glPassTexCoordATI(GL_REG_0_ATI, GL_REG_0_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_1_ATI, GL_REG_1_ATI, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_2_ATI, GL_REG_4_ATI, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, 0, GL_REG_2_ATI, 0, 0);
    glEndFragmentShaderATI();
    cm_nFilmNoiseShaderATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(cm_nFilmNoiseShaderATI);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_REG_1_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, 0, 0, GL_COMP_BIT_ATI);
    glEndFragmentShaderATI();
    cm_nAccumulationBlurShaderATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(cm_nAccumulationBlurShaderATI);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_2_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_0_ATI, 0, 0);
    glColorFragmentOp3ATI(GL_MAD_ATI, GL_REG_0_ATI, 0, 0, GL_REG_1_ATI, 0, 0, GL_CON_1_ATI, 0, 0, GL_REG_2_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_CON_0_ATI, 0, 0);
    glEndFragmentShaderATI();
    cm_nSaturationShaderATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(cm_nSaturationShaderATI);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_1_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_0_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_2_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_1_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_1_ATI, 0, 0, GL_REG_1_ATI, 0, 0, GL_CON_3_ATI, 0, 0);
    glColorFragmentOp3ATI(GL_MAD_ATI, GL_REG_1_ATI, 0, 0, GL_REG_2_ATI, 0, 0, GL_CON_4_ATI, 0, 0, GL_REG_1_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_2_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_2_ATI, 0, 0);
    glColorFragmentOp3ATI(GL_MAD_ATI, GL_REG_0_ATI, 0, 0, GL_REG_2_ATI, 0, 0, GL_CON_5_ATI, 0, 0, GL_REG_1_ATI, 0, 0);
    glEndFragmentShaderATI();
    cm_nPerPixelFlareShaderATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(cm_nPerPixelFlareShaderATI);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_1_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_0_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_2_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_1_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_3_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_2_ATI, 0, 0);
    glPassTexCoordATI(GL_REG_0_ATI, GL_REG_0_ATI, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_REG_1_ATI, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_2_ATI, GL_REG_2_ATI, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_3_ATI, GL_REG_3_ATI, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, 0, 0, GL_REG_1_ATI, 0, 0, GL_REG_2_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_REG_3_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, 0, 0, GL_COMP_BIT_ATI);
    glEndFragmentShaderATI();
    cm_nDecalShaderATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(cm_nDecalShaderATI);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, 0, GL_REG_0_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, 0, 0, GL_COMP_BIT_ATI);
    glEndFragmentShaderATI();
    cm_n4DecalShaderATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(cm_n4DecalShaderATI);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_3_ATI, GL_TEXTURE3_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_CON_0_ATI, 0, 0);
    glColorFragmentOp3ATI(GL_MAD_ATI, GL_REG_1_ATI, 0, 0, GL_REG_1_ATI, 0, 0, GL_CON_1_ATI, 0, 0, GL_REG_0_ATI, 0, 0);
    glColorFragmentOp3ATI(GL_MAD_ATI, GL_REG_2_ATI, 0, 0, GL_REG_2_ATI, 0, 0, GL_CON_2_ATI, 0, 0, GL_REG_1_ATI, 0, 0);
    glColorFragmentOp3ATI(GL_MAD_ATI, GL_REG_0_ATI, 0, 0, GL_REG_3_ATI, 0, 0, GL_CON_3_ATI, 0, 0, GL_REG_2_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_PRIMARY_COLOR_ARB, GL_ALPHA, 0);
    glEndFragmentShaderATI();
    cm_nDreamDistortShaderATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(cm_nDreamDistortShaderATI);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_4_ATI, GL_RED_BIT_ATI, 0, GL_REG_0_ATI, 0, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI, GL_REG_1_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_4_ATI, GL_GREEN_BIT_ATI, 0, GL_REG_0_ATI, 0, GL_BIAS_BIT_ATI | GL_2X_BIT_ATI, GL_REG_2_ATI, 0, 0);
    glSampleMapATI(GL_REG_2_ATI, GL_REG_4_ATI, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_3_ATI, GL_TEXTURE3_ARB, GL_SWIZZLE_STR_ATI);
    glAlphaFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, 0, GL_REG_3_ATI, GL_ALPHA, 0, GL_CON_7_ATI, GL_ALPHA, 0);
    glAlphaFragmentOp3ATI(GL_CND_ATI, GL_REG_1_ATI, 0, GL_ONE, GL_ALPHA, 0, GL_ZERO, 0, 0, GL_REG_0_ATI, GL_ALPHA, 0);
    glColorFragmentOp3ATI(GL_MAD_ATI, GL_REG_0_ATI, GL_RED_BIT_ATI | GL_GREEN_BIT_ATI | GL_BLUE_BIT_ATI, 0, GL_REG_2_ATI, 0, 0, GL_REG_1_ATI, GL_ALPHA, 0, GL_REG_3_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_REG_3_ATI, GL_ALPHA, 0);
    glEndFragmentShaderATI();
    glDisable(GL_FRAGMENT_SHADER_ATI);
}
// PC 0x00429FF0 GOG 0x0042A100
void GLRender::InitializeFrameBufferModifications()
{
    if (doframebuffer && cm_bSetUpFrameBuffer)
    {
        if (AurFrameBufferModificationsAvailable())
        {
            if (cm_nTargetTexture == 0)
            {
                glGenTextures(1, &cm_nTargetTexture);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8, 0, 0,
                                 g_nScreenWidth, g_nScreenHeight, 0);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
            }
            unsigned int nRed[FRAMEBUFFER_LOOKUP_SIZE];
            unsigned int nGreen[FRAMEBUFFER_LOOKUP_SIZE];
            unsigned int nBlue[FRAMEBUFFER_LOOKUP_SIZE];
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                if (i < FRAMEBUFFER_RED_THRESHOLD)
                    nRed[i] = 0;
                else
                {
                    float fLookup = (i - FRAMEBUFFER_RED_THRESHOLD) / (float)(FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_RED_THRESHOLD - 1);
                    nRed[i] = (unsigned int)(fLookup * AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN);
                }
            }
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                if (i < FRAMEBUFFER_GREEN_THRESHOLD)
                    nGreen[i] = 0;
                else
                {
                    float fLookup = (i - FRAMEBUFFER_GREEN_THRESHOLD) / (float)(FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_GREEN_THRESHOLD - 1);
                    nGreen[i] = (unsigned int)(fLookup * AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN) << FRAMEBUFFER_GREEN_SHIFT;
                }
            }
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                if (i < FRAMEBUFFER_BLUE_THRESHOLD)
                    nBlue[i] = 0;
                else
                {
                    float fLookup = (i - FRAMEBUFFER_BLUE_THRESHOLD) / (float)(FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_BLUE_THRESHOLD - 1);
                    nBlue[i] = (unsigned int)(fLookup * AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN) << FRAMEBUFFER_BLUE_SHIFT;
                }
            }
            glGenTextures(1, &cm_nRedTexture);
            glBindTexture(GL_TEXTURE_2D, cm_nRedTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, FRAMEBUFFER_LOOKUP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nRed);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenTextures(1, &cm_nGreenTexture);
            glBindTexture(GL_TEXTURE_2D, cm_nGreenTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FRAMEBUFFER_LOOKUP_SIZE, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nGreen);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenTextures(1, &cm_nBlueTexture);
            glBindTexture(GL_TEXTURE_2D, cm_nBlueTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, FRAMEBUFFER_LOOKUP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nBlue);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            memset(cm_nConvolutionRenderTexture, 0, sizeof(cm_nConvolutionRenderTexture));
            memset(cm_hConvolutionHGLRC, 0, sizeof(cm_hConvolutionHGLRC));
            memset(cm_hConvolutionHDC, 0, sizeof(cm_hConvolutionHDC));
            memset(cm_hConvolutionPBuffer, 0, sizeof(cm_hConvolutionPBuffer));
            cm_bPBuffersAllocated = true;

            cm_hConvolutionPBuffer[0] = CreatePBuffer(64, 64);
            cm_hConvolutionHDC[0] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[0]);
            cm_hConvolutionHGLRC[0] = wglCreateContext(cm_hConvolutionHDC[0]);
            wglShareLists(m_hGLRC, cm_hConvolutionHGLRC[0]);
            wglMakeCurrent(cm_hConvolutionHDC[0], cm_hConvolutionHGLRC[0]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[0]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hConvolutionPBuffer[1] = CreatePBuffer(128, 128);
            cm_hConvolutionHDC[1] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[1]);
            cm_hConvolutionHGLRC[1] = wglCreateContext(cm_hConvolutionHDC[1]);
            wglShareLists(m_hGLRC, cm_hConvolutionHGLRC[1]);
            wglMakeCurrent(cm_hConvolutionHDC[1], cm_hConvolutionHGLRC[1]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[1]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[1]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hConvolutionPBuffer[2] = CreatePBuffer(256, 256);
            cm_hConvolutionHDC[2] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[2]);
            cm_hConvolutionHGLRC[2] = wglCreateContext(cm_hConvolutionHDC[2]);
            wglShareLists(m_hGLRC, cm_hConvolutionHGLRC[2]);
            wglMakeCurrent(cm_hConvolutionHDC[2], cm_hConvolutionHGLRC[2]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[2]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[2]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hConvolutionPBuffer[3] = CreatePBuffer(512, 512);
            cm_hConvolutionHDC[3] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[3]);
            cm_hConvolutionHGLRC[3] = wglCreateContext(cm_hConvolutionHDC[3]);
            wglShareLists(m_hGLRC, cm_hConvolutionHGLRC[3]);
            wglMakeCurrent(cm_hConvolutionHDC[3], cm_hConvolutionHGLRC[3]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[3]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[3]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hConvolutionPBuffer[4] = CreatePBuffer(256, 256);
            cm_hConvolutionHDC[4] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[4]);
            cm_hConvolutionHGLRC[4] = wglCreateContext(cm_hConvolutionHDC[4]);
            wglShareLists(m_hGLRC, cm_hConvolutionHGLRC[4]);
            wglMakeCurrent(cm_hConvolutionHDC[4], cm_hConvolutionHGLRC[4]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[4]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[4]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hConvolutionPBuffer[5] = CreatePBuffer(256, 256);
            cm_hConvolutionHDC[5] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[5]);
            cm_hConvolutionHGLRC[5] = wglCreateContext(cm_hConvolutionHDC[5]);
            wglShareLists(m_hGLRC, cm_hConvolutionHGLRC[5]);
            wglMakeCurrent(cm_hConvolutionHDC[5], cm_hConvolutionHGLRC[5]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[5]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[5]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hAccumulationHDC = 0;
            cm_hAccumulationHGLRC = 0;
            cm_hAccumulationPBuffer = 0;
            cm_nAccumulationBlur = 0;
            cm_bFirstFrameBufferFrame = true;
            cm_hAccumulationPBuffer = CreatePBuffer(128, 128);
            cm_hAccumulationHDC = wglGetPBufferDCARB(cm_hAccumulationPBuffer);
            cm_hAccumulationHGLRC = wglCreateContext(cm_hAccumulationHDC);
            wglShareLists(m_hGLRC, cm_hAccumulationHGLRC);
            wglMakeCurrent(cm_hAccumulationHDC, cm_hAccumulationHGLRC);
            glGenTextures(1, &cm_nAccumulationBlur);
            glBindTexture(GL_TEXTURE_2D, cm_nAccumulationBlur);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            static char szForceDistortProg[] =
                "!!ARBvp1.0\n"
                "TEMP vReg0;\n"
                "TEMP vReg1;\n"
                "TEMP vReg2;\n"
                "TEMP vReg4, vReg3;\n"
                "DP4   result.position.x, state.matrix.mvp.row[0], vertex.position;\n"
                "DP4   result.position.y, state.matrix.mvp.row[1], vertex.position;\n"
                "DP4   result.position.z, state.matrix.mvp.row[2], vertex.position;\n"
                "DP4   result.position.w, state.matrix.mvp.row[3], vertex.position;\n"
                "MOV result.texcoord[0], vertex.texcoord[0];\n"
                "MOV vReg1, vertex.texcoord[1];\n"
                "MOV vReg2, vertex.texcoord[2];\n"
                "MOV vReg3.x, vReg1.x;\n"
                "MOV vReg3.y, program.env[15].x;\n"
                "MOV vReg3.z, vReg2.x;\n"
                "MOV vReg4.x, program.env[15].x;\n"
                "MOV vReg4.y, vReg1.y;\n"
                "MOV vReg4.z, vReg2.y;\n"
                "MOV result.texcoord[1].xyz, vReg3;\n"
                "MOV result.texcoord[2].xyz, vReg4;\n"
                "MOV result.color.primary, program.env[15].y;\n"
                "END";
            glEnable(GL_VERTEX_PROGRAM_ARB);
            glGenProgramsARB(1, &cm_pHeatDistortionVertexProg);
            glBindProgramARB(GL_VERTEX_PROGRAM_ARB, cm_pHeatDistortionVertexProg);
            glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
                               strlen(szForceDistortProg), szForceDistortProg);
            glBindProgramARB(GL_VERTEX_PROGRAM_ARB, 0);
            glDisable(GL_VERTEX_PROGRAM_ARB);
            AurGetVertexProgramARBError(szForceDistortProg);

            unsigned int nOverbrightenRed[FRAMEBUFFER_LOOKUP_SIZE];
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                float fIntensity = i / AUR_FRAMEBUFFER_CHANNEL_MAX;
                if (fIntensity < AUR_FRAMEBUFFER_OVERBRIGHTEN_THRESHOLD)
                    fIntensity = aquadraticterm * fIntensity * fIntensity * AUR_FRAMEBUFFER_QUADRATIC_GAIN;
                else
                    fIntensity = (float)sqrt((fIntensity - AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) *
                                      AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) + AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE;
                nOverbrightenRed[i] = (unsigned int)(fIntensity * AUR_FRAMEBUFFER_CHANNEL_MAX);
            }
            unsigned int nOverbrightenGreen[FRAMEBUFFER_LOOKUP_SIZE];
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                float fIntensity = i / AUR_FRAMEBUFFER_CHANNEL_MAX;
                if (fIntensity < AUR_FRAMEBUFFER_OVERBRIGHTEN_THRESHOLD)
                {
                    fIntensity *= fIntensity;
                    fIntensity = fIntensity * aquadraticterm * AUR_FRAMEBUFFER_QUADRATIC_GAIN;
                }
                else
                    fIntensity = (float)sqrt((fIntensity - AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) *
                                      AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) + AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE;
                nOverbrightenGreen[i] = (unsigned int)(fIntensity * AUR_FRAMEBUFFER_CHANNEL_MAX) << FRAMEBUFFER_GREEN_SHIFT;
            }
            unsigned int nOverbrightenBlue[FRAMEBUFFER_LOOKUP_SIZE];
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                float fIntensity = i / AUR_FRAMEBUFFER_CHANNEL_MAX;
                if (fIntensity < AUR_FRAMEBUFFER_OVERBRIGHTEN_THRESHOLD)
                {
                    fIntensity *= fIntensity;
                    fIntensity = fIntensity * aquadraticterm * AUR_FRAMEBUFFER_QUADRATIC_GAIN;
                }
                else
                    fIntensity = (float)sqrt((fIntensity - AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) *
                                      AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) + AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE;
                nOverbrightenBlue[i] = (unsigned int)(fIntensity * AUR_FRAMEBUFFER_CHANNEL_MAX) << FRAMEBUFFER_BLUE_SHIFT;
            }
            glGenTextures(1, &cm_nOverbrightenRed);
            glBindTexture(GL_TEXTURE_2D, cm_nOverbrightenRed);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, FRAMEBUFFER_LOOKUP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nOverbrightenRed);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenTextures(1, &cm_nOverbrightenGreen);
            glBindTexture(GL_TEXTURE_2D, cm_nOverbrightenGreen);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FRAMEBUFFER_LOOKUP_SIZE, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nOverbrightenGreen);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenTextures(1, &cm_nOverbrightenBlue);
            glBindTexture(GL_TEXTURE_2D, cm_nOverbrightenBlue);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, FRAMEBUFFER_LOOKUP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nOverbrightenBlue);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        if (AurATIFrameBufferModificationsAvailable())
        {
            InitializeFrameBufferModificationFragmentShadersATI();
            FindSmallestPowerOf2WidthHeight(g_nScreenWidth, g_nScreenHeight,
                                             cm_nScreenWidthPow2, cm_nScreenHeightPow2);
            if (cm_nTargetTexture == 0)
            {
                glGenTextures(1, &cm_nTargetTexture);
                glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
                glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0,
                                 cm_nScreenWidthPow2, cm_nScreenHeightPow2, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            unsigned int nRed[FRAMEBUFFER_LOOKUP_SIZE];
            unsigned int nGreen[FRAMEBUFFER_LOOKUP_SIZE];
            unsigned int nBlue[FRAMEBUFFER_LOOKUP_SIZE];
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                if (i < FRAMEBUFFER_RED_THRESHOLD)
                    nRed[i] = 0;
                else
                {
                    float fLookup = (i - FRAMEBUFFER_RED_THRESHOLD) / (float)(FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_RED_THRESHOLD - 1);
                    nRed[i] = (unsigned int)(fLookup * AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN);
                }
            }
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                if (i < FRAMEBUFFER_GREEN_THRESHOLD)
                    nGreen[i] = 0;
                else
                {
                    float fLookup = (i - FRAMEBUFFER_GREEN_THRESHOLD) / (float)(FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_GREEN_THRESHOLD - 1);
                    nGreen[i] = (unsigned int)(fLookup * AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN) << FRAMEBUFFER_GREEN_SHIFT;
                }
            }
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                if (i < FRAMEBUFFER_BLUE_THRESHOLD)
                    nBlue[i] = 0;
                else
                {
                    float fLookup = (i - FRAMEBUFFER_BLUE_THRESHOLD) / (float)(FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_BLUE_THRESHOLD - 1);
                    nBlue[i] = (unsigned int)(fLookup * AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN) << FRAMEBUFFER_BLUE_SHIFT;
                }
            }
            glGenTextures(1, &cm_nRedTexture);
            glBindTexture(GL_TEXTURE_1D, cm_nRedTexture);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, FRAMEBUFFER_LOOKUP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nRed);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenTextures(1, &cm_nGreenTexture);
            glBindTexture(GL_TEXTURE_1D, cm_nGreenTexture);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, FRAMEBUFFER_LOOKUP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nGreen);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenTextures(1, &cm_nBlueTexture);
            glBindTexture(GL_TEXTURE_1D, cm_nBlueTexture);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, FRAMEBUFFER_LOOKUP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nBlue);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            HDC hOriginalDC = m_hDC;
            HGLRC hOriginalContext = m_hGLRC;
            memset(cm_nConvolutionRenderTexture, 0, sizeof(cm_nConvolutionRenderTexture));
            memset(cm_hConvolutionHGLRC, 0, sizeof(cm_hConvolutionHGLRC));
            memset(cm_hConvolutionHDC, 0, sizeof(cm_hConvolutionHDC));
            memset(cm_hConvolutionPBuffer, 0, sizeof(cm_hConvolutionPBuffer));
            cm_bPBuffersAllocated = true;

            cm_hConvolutionPBuffer[0] = CreatePBuffer(64, 64);
            cm_hConvolutionHDC[0] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[0]);
            cm_hConvolutionHGLRC[0] = m_hGLRC;
            wglMakeCurrent(cm_hConvolutionHDC[0], cm_hConvolutionHGLRC[0]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[0]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hConvolutionPBuffer[1] = CreatePBuffer(128, 128);
            cm_hConvolutionHDC[1] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[1]);
            cm_hConvolutionHGLRC[1] = m_hGLRC;
            wglMakeCurrent(cm_hConvolutionHDC[1], cm_hConvolutionHGLRC[1]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[1]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[1]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hConvolutionPBuffer[2] = CreatePBuffer(256, 256);
            cm_hConvolutionHDC[2] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[2]);
            cm_hConvolutionHGLRC[2] = m_hGLRC;
            wglMakeCurrent(cm_hConvolutionHDC[2], cm_hConvolutionHGLRC[2]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[2]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[2]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hConvolutionPBuffer[3] = CreatePBuffer(512, 512);
            cm_hConvolutionHDC[3] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[3]);
            cm_hConvolutionHGLRC[3] = m_hGLRC;
            wglMakeCurrent(cm_hConvolutionHDC[3], cm_hConvolutionHGLRC[3]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[3]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[3]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hConvolutionPBuffer[4] = CreatePBuffer(256, 256);
            cm_hConvolutionHDC[4] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[4]);
            cm_hConvolutionHGLRC[4] = m_hGLRC;
            wglMakeCurrent(cm_hConvolutionHDC[4], cm_hConvolutionHGLRC[4]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[4]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[4]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hConvolutionPBuffer[5] = CreatePBuffer(256, 256);
            cm_hConvolutionHDC[5] = wglGetPBufferDCARB(cm_hConvolutionPBuffer[5]);
            cm_hConvolutionHGLRC[5] = m_hGLRC;
            wglMakeCurrent(cm_hConvolutionHDC[5], cm_hConvolutionHGLRC[5]);
            glGenTextures(1, &cm_nConvolutionRenderTexture[5]);
            glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[5]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(m_hDC, m_hGLRC);

            cm_hAccumulationHDC = 0;
            cm_hAccumulationHGLRC = 0;
            cm_hAccumulationPBuffer = 0;
            cm_nAccumulationBlur = 0;
            cm_bFirstFrameBufferFrame = true;
            cm_hAccumulationPBuffer = CreatePBuffer(128, 128);
            cm_hAccumulationHDC = wglGetPBufferDCARB(cm_hAccumulationPBuffer);
            cm_hAccumulationHGLRC = m_hGLRC;
            wglMakeCurrent(cm_hAccumulationHDC, cm_hAccumulationHGLRC);
            glGenTextures(1, &cm_nAccumulationBlur);
            glBindTexture(GL_TEXTURE_2D, cm_nAccumulationBlur);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            wglMakeCurrent(hOriginalDC, hOriginalContext);

            static char szForceDistortProg[] =
                "!!ARBvp1.0\n"
                "TEMP vReg0;\n"
                "TEMP vReg1;\n"
                "TEMP vReg2;\n"
                "TEMP vReg4, vReg3;\n"
                "DP4   result.position.x, state.matrix.mvp.row[0], vertex.position;\n"
                "DP4   result.position.y, state.matrix.mvp.row[1], vertex.position;\n"
                "DP4   result.position.z, state.matrix.mvp.row[2], vertex.position;\n"
                "DP4   result.position.w, state.matrix.mvp.row[3], vertex.position;\n"
                "MOV result.texcoord[0], vertex.texcoord[0];\n"
                "MOV vReg1, vertex.texcoord[1];\n"
                "MOV vReg2, vertex.texcoord[2];\n"
                "MOV vReg3.x, vReg1.x;\n"
                "MOV vReg3.y, program.env[15].x;\n"
                "MOV vReg3.z, vReg2.x;\n"
                "MOV vReg4.x, program.env[15].x;\n"
                "MOV vReg4.y, vReg1.y;\n"
                "MOV vReg4.z, vReg2.y;\n"
                "MOV result.texcoord[1].xyz, vReg3;\n"
                "MOV result.texcoord[2].xyz, vReg4;\n"
                "MOV result.color.primary, program.env[15].y;\n"
                "END";
            glEnable(GL_VERTEX_PROGRAM_ARB);
            glGenProgramsARB(1, &cm_pHeatDistortionVertexProg);
            glBindProgramARB(GL_VERTEX_PROGRAM_ARB, cm_pHeatDistortionVertexProg);
            glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
                               strlen(szForceDistortProg), szForceDistortProg);
            glBindProgramARB(GL_VERTEX_PROGRAM_ARB, 0);
            glDisable(GL_VERTEX_PROGRAM_ARB);
            AurGetVertexProgramARBError(szForceDistortProg);

            unsigned int nOverbrightenRed[FRAMEBUFFER_LOOKUP_SIZE];
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                float fIntensity = i / AUR_FRAMEBUFFER_CHANNEL_MAX;
                if (fIntensity < AUR_FRAMEBUFFER_OVERBRIGHTEN_THRESHOLD)
                {
                    fIntensity *= fIntensity;
                    fIntensity = fIntensity * aquadraticterm * AUR_FRAMEBUFFER_QUADRATIC_GAIN;
                }
                else
                    fIntensity = (float)sqrt((fIntensity - AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) *
                                      AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) + AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE;
                nOverbrightenRed[i] = (unsigned int)(fIntensity * AUR_FRAMEBUFFER_CHANNEL_MAX);
            }
            unsigned int nOverbrightenGreen[FRAMEBUFFER_LOOKUP_SIZE];
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                float fIntensity = i / AUR_FRAMEBUFFER_CHANNEL_MAX;
                if (fIntensity < AUR_FRAMEBUFFER_OVERBRIGHTEN_THRESHOLD)
                {
                    fIntensity *= fIntensity;
                    fIntensity = fIntensity * aquadraticterm * AUR_FRAMEBUFFER_QUADRATIC_GAIN;
                }
                else
                    fIntensity = (float)sqrt((fIntensity - AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) *
                                      AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) + AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE;
                nOverbrightenGreen[i] = (unsigned int)(fIntensity * AUR_FRAMEBUFFER_CHANNEL_MAX) << FRAMEBUFFER_GREEN_SHIFT;
            }
            unsigned int nOverbrightenBlue[FRAMEBUFFER_LOOKUP_SIZE];
            for (int i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; ++i)
            {
                float fIntensity = i / AUR_FRAMEBUFFER_CHANNEL_MAX;
                if (fIntensity < AUR_FRAMEBUFFER_OVERBRIGHTEN_THRESHOLD)
                {
                    fIntensity *= fIntensity;
                    fIntensity = fIntensity * aquadraticterm * AUR_FRAMEBUFFER_QUADRATIC_GAIN;
                }
                else
                    fIntensity = (float)sqrt((fIntensity - AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) *
                                      AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE) + AUR_FRAMEBUFFER_OVERBRIGHTEN_KNEE;
                nOverbrightenBlue[i] = (unsigned int)(fIntensity * AUR_FRAMEBUFFER_CHANNEL_MAX) << FRAMEBUFFER_BLUE_SHIFT;
            }
            glGenTextures(1, &cm_nOverbrightenRed);
            glBindTexture(GL_TEXTURE_1D, cm_nOverbrightenRed);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, FRAMEBUFFER_LOOKUP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nOverbrightenRed);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenTextures(1, &cm_nOverbrightenGreen);
            glBindTexture(GL_TEXTURE_1D, cm_nOverbrightenGreen);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, FRAMEBUFFER_LOOKUP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nOverbrightenGreen);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenTextures(1, &cm_nOverbrightenBlue);
            glBindTexture(GL_TEXTURE_1D, cm_nOverbrightenBlue);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, FRAMEBUFFER_LOOKUP_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nOverbrightenBlue);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }
}
// PC 0x0042B750 GOG 0x0042B860
void GLRender::FrameBufferModificationPixelShadersATI(unsigned long nMode,
    unsigned long nTexture0, unsigned long nTexture1,
    unsigned long nTexture2, unsigned long nTexture3)
{
    switch (nMode)
    {
    case FRAMEBUFFER_SHADER_SATURATION:
    {
        static float c4define[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
        static float c5define[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
        static float c6define[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glSetFragmentShaderConstantATI(GL_CON_3_ATI, c4define);
        glSetFragmentShaderConstantATI(GL_CON_4_ATI, c5define);
        glSetFragmentShaderConstantATI(GL_CON_5_ATI, c6define);
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(cm_nSaturationShaderATI);
        break;
    }
    case FRAMEBUFFER_SHADER_CONVOLUTION:
    {
        static float c0define[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
        static float c1define[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
        static float c2define[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nTexture0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, nTexture1);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, nTexture2);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, nTexture3);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(cm_nPerPixelFlareShaderATI);
        glSetFragmentShaderConstantATI(GL_CON_0_ATI, c0define);
        glSetFragmentShaderConstantATI(GL_CON_1_ATI, c1define);
        glSetFragmentShaderConstantATI(GL_CON_2_ATI, c2define);
        break;
    }
    case FRAMEBUFFER_SHADER_COPY:
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(cm_nDecalShaderATI);
        break;
    case FRAMEBUFFER_SHADER_FOUR_DECALS:
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(cm_n4DecalShaderATI);
        break;
    case FRAMEBUFFER_SHADER_DISABLE:
        glDisable(GL_FRAGMENT_SHADER_ATI);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_1D);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
        glBindTexture(GL_TEXTURE_1D, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_1D);
        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
        glBindTexture(GL_TEXTURE_1D, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_1D);
        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_1D, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        break;
    case FRAMEBUFFER_SHADER_FILM_NOISE:
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(cm_nFilmNoiseShaderATI);
        break;
    case FRAMEBUFFER_SHADER_ACCUMULATION_BLUR:
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(cm_nAccumulationBlurShaderATI);
        break;
    case FRAMEBUFFER_SHADER_FORCE_DISTORTION:
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(cm_nForceDistortionShaderATI);
        break;
    case FRAMEBUFFER_SHADER_SOFT_SHADOW:
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(cm_nSoftShadowShaderATI);
        break;
    case FRAMEBUFFER_SHADER_OVERBRIGHTEN:
    {
        static float c0define[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
        static float c1define[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
        static float c2define[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nTexture0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, nTexture1);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, nTexture2);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, nTexture3);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(cm_nOverbrightenShaderATI);
        glSetFragmentShaderConstantATI(GL_CON_0_ATI, c0define);
        glSetFragmentShaderConstantATI(GL_CON_1_ATI, c1define);
        glSetFragmentShaderConstantATI(GL_CON_2_ATI, c2define);
        break;
    }
    case FRAMEBUFFER_SHADER_DREAM_DISTORTION:
    {
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_FRAGMENT_SHADER_ATI);
        float c7define[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
        glSetFragmentShaderConstantATI(GL_CON_7_ATI, c7define);
        glBindFragmentShaderATI(cm_nDreamDistortShaderATI);
        break;
    }
    }
}
// PC 0x0042BDD0 GOG 0x0042BEE0
void GLRender::FrameBufferModificationPixelShaders(unsigned long nMode,
    unsigned long nTexture0, unsigned long nTexture1,
    unsigned long nTexture2, unsigned long nTexture3)
{
    switch (nMode)
    {
    case FRAMEBUFFER_SHADER_SATURATION:
    {
        static float c4define[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
        static float c5define[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
        static float c6define[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glEnable(GL_REGISTER_COMBINERS_NV);
        glEnable(GL_PER_STAGE_CONSTANTS_NV);
        glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 4);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_SPARE1_NV, GL_DISCARD_NV, 0, 0, GL_TRUE, GL_TRUE, GL_FALSE);
        glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR0_NV, c4define);
        glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR1_NV, c5define);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_SPARE1_NV, GL_DISCARD_NV, GL_DISCARD_NV, 0, 0, GL_TRUE, GL_FALSE, GL_FALSE);
        glCombinerStageParameterfvNV(GL_COMBINER3_NV, GL_CONSTANT_COLOR0_NV, c6define);
        glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER3_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER3_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glFinalCombinerInputNV(GL_VARIABLE_A_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_G_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        break;
    }
    case FRAMEBUFFER_SHADER_CONVOLUTION:
    {
        static float c0define[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
        static float c1define[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
        static float c2define[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_RECTANGLE_NV);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, nTexture0);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_RECTANGLE_NV);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nTexture1);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_AR_TEXTURE_2D_NV);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nTexture2);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_GB_TEXTURE_2D_NV);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nTexture3);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_GB_TEXTURE_2D_NV);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_REGISTER_COMBINERS_NV);
        glEnable(GL_PER_STAGE_CONSTANTS_NV);
        glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
        glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR0_NV, c0define);
        glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, c1define);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR0_NV, c2define);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glFinalCombinerInputNV(GL_VARIABLE_A_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_G_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        break;
    }
    case FRAMEBUFFER_SHADER_COPY:
    {
        glEnable(GL_REGISTER_COMBINERS_NV);
        glEnable(GL_PER_STAGE_CONSTANTS_NV);
        glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glFinalCombinerInputNV(GL_VARIABLE_A_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_G_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        break;
    }
    case FRAMEBUFFER_SHADER_FOUR_DECALS:
    {
        glEnable(GL_REGISTER_COMBINERS_NV);
        glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glFinalCombinerInputNV(GL_VARIABLE_A_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_PRIMARY_COLOR_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        break;
    }
    case FRAMEBUFFER_SHADER_DISABLE:
    {
        glDisable(GL_REGISTER_COMBINERS_NV);
        glDisable(GL_PER_STAGE_CONSTANTS_NV);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_RECTANGLE_NV);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
        glDisable(GL_TEXTURE_SHADER_NV);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_SHADER_NV);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_SHADER_NV);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_SHADER_NV);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        break;
    }
    case FRAMEBUFFER_SHADER_FILM_NOISE:
    {
        glEnable(GL_REGISTER_COMBINERS_NV);
        glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
        glEnable(GL_PER_STAGE_CONSTANTS_NV);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glFinalCombinerInputNV(GL_VARIABLE_A_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_G_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        break;
    }
    case FRAMEBUFFER_SHADER_ACCUMULATION_BLUR:
    {
        glEnable(GL_REGISTER_COMBINERS_NV);
        glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 2);
        glEnable(GL_PER_STAGE_CONSTANTS_NV);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glFinalCombinerInputNV(GL_VARIABLE_A_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        break;
    }
    case FRAMEBUFFER_SHADER_FORCE_DISTORTION:
    {
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_RECTANGLE_NV);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
        glEnable(GL_REGISTER_COMBINERS_NV);
        glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
        glEnable(GL_PER_STAGE_CONSTANTS_NV);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glFinalCombinerInputNV(GL_VARIABLE_A_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_G_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        break;
    }
    case FRAMEBUFFER_SHADER_SOFT_SHADOW:
    {
        glEnable(GL_REGISTER_COMBINERS_NV);
        glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_SPARE1_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glFinalCombinerInputNV(GL_VARIABLE_A_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        break;
    }
    case FRAMEBUFFER_SHADER_OVERBRIGHTEN:
    {
        static float c0define[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
        static float c1define[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
        static float c2define[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_RECTANGLE_NV);
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, nTexture0);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_RECTANGLE_NV);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nTexture1);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_AR_TEXTURE_2D_NV);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nTexture2);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_GB_TEXTURE_2D_NV);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nTexture3);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_GB_TEXTURE_2D_NV);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glEnable(GL_REGISTER_COMBINERS_NV);
        glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 3);
        glEnable(GL_PER_STAGE_CONSTANTS_NV);
        glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR0_NV, c0define);
        glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR0_NV, c1define);
        glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, c2define);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_C_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER2_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER2_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glFinalCombinerInputNV(GL_VARIABLE_A_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_PRIMARY_COLOR_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        break;
    }
    case FRAMEBUFFER_SHADER_DREAM_DISTORTION:
    {
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_NV);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0_ARB);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DOT_PRODUCT_TEXTURE_2D_NV);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, GL_EXPAND_NORMAL_NV);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glEnable(GL_TEXTURE_SHADER_NV);
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
        glEnable(GL_REGISTER_COMBINERS_NV);
        glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
        glEnable(GL_PER_STAGE_CONSTANTS_NV);
        float c0define[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
        glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR0_NV, c0define);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE2_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_C_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_INVERT_NV, GL_RGB);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, 0, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_C_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_SPARE0_NV, GL_DISCARD_NV, GL_DISCARD_NV, 0, 0, GL_FALSE, GL_FALSE, GL_FALSE);
        glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        glFinalCombinerInputNV(GL_VARIABLE_B_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_FOG, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_D_NV, 0, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
        glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SPARE0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
        break;
    }
    }
}
// PC 0x0042DA50 GOG 0x0042DB60
void GLRender::MipmapFilterATI(HDC hDC, HGLRC hContext, float fWidth,
                              float fHeight, PBUFFERARB* pBuffer,
                              unsigned long nTexture, float fSourceSize)
{
    wglMakeContextCurrentARB(hDC, hDC, hContext);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, fWidth, 0.0, fHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, (int)fWidth, (int)fHeight);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_COPY, 0, 0, 0, 0);
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? 1.0f : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
        float y = (i == 0 || i == 3) ? fHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : fWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glFlush();
}
// PC 0x0042DCB0 GOG 0x0042DDC0
void GLRender::MipmapFilter(HDC hDC, HGLRC hContext, float fWidth,
                            float fHeight, PBUFFERARB* pBuffer,
                            unsigned long nTexture, float fSourceSize)
{
    wglMakeCurrent(hDC, hContext);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, fWidth, 0.0, fHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, (int)fWidth, (int)fHeight);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_COPY, 0, 0, 0, 0);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? 1.0f : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? fHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : fWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glFlush();
}
// PC 0x0042DEF0 GOG 0x0042E000
void GLRender::BloomFilterATI(HDC hDC, HGLRC hContext, float fWidth,
                           float fHeight, PBUFFERARB* pBuffer,
                           unsigned long nTexture, float fSourceSize,
                           int bSoftShadows)
{
    static float filter[28] = {
        0.12f, 0.25f, 0.29f, 0.25f, 0.12f,
        0.25f, 0.49f, 0.58f, 0.49f, 0.25f,
        0.29f, 0.58f, 0.67f, 0.58f, 0.29f,
        0.25f, 0.49f, 0.58f, 0.49f, 0.25f,
        0.12f, 0.25f, 0.29f, 0.25f, 0.12f,
        0.0f, 0.0f, 0.0f
    };
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    wglMakeContextCurrentARB(hDC, hDC, hContext);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, fWidth, 0.0, fHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, (int)fWidth, (int)fHeight);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    if (bSoftShadows)
    {
        glEnable(GL_FRAGMENT_SHADER_ATI);
        glBindFragmentShaderATI(cm_nSoftShadowShaderATI);
    }
    else
        FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_FOUR_DECALS, 0, 0, 0, 0);
    int nRow = 0;
    int nColumn = 0;
    glEnable(GL_BLEND);
    float fInverseSize = 1.0f / fSourceSize;
    for (int nSample = 0; nSample < 25; nSample += 4)
    {
        for (int nStage = 0; nStage < 4; ++nStage)
        {
            float color[4];
            if (bSoftShadows)
                color[3] = color[2] = color[1] = color[0] =
                    softshadowdarkeningfactor * filter[nSample + nStage];
            else
            {
                color[0] = filter[nSample + nStage];
                color[1] = filter[nSample + nStage];
                color[2] = filter[nSample + nStage];
                color[3] = filter[nSample + nStage];
            }
            if (nStage == 0)
                glSetFragmentShaderConstantATI(GL_CON_0_ATI, color);
            else if (nStage == 1)
                glSetFragmentShaderConstantATI(GL_CON_1_ATI, color);
            else if (nStage == 2)
            {
                color[0] = color[1] = color[2] = color[3] = 0.0f;
                glSetFragmentShaderConstantATI(GL_CON_2_ATI, color);
            }
            else
                glSetFragmentShaderConstantATI(GL_CON_3_ATI, color);
        }
        glBegin(GL_QUADS);
        for (int i = 0; i < 4; ++i)
        {
            glColor4f(1.0f, 1.0f, 1.0f, multiplier);
            glActiveTextureARB(GL_TEXTURE0_ARB);
            float tBase = (i == 0 || i == 3) ? (nRow + fSourceSize - pixeloffset) * fInverseSize : (nRow - pixeloffset) * fInverseSize;
            float s = (i == 0 || i == 1) ? (float)(0.0 + (nColumn - pixeloffset) * fInverseSize) : (nColumn + fSourceSize - pixeloffset) * fInverseSize;
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, tBase);
            int nTextureRow = (nColumn + 1 > 4) ? nRow + 1 : nRow;
            int nTextureColumn = (nColumn + 1) % 5;
            glActiveTextureARB(GL_TEXTURE1_ARB);
            float t = (i == 0 || i == 3) ? (nTextureColumn + fSourceSize - pixeloffset) * fInverseSize : (nTextureColumn - pixeloffset) * fInverseSize;
            s = (i == 0 || i == 1) ? (nTextureRow - pixeloffset) * fInverseSize : (nTextureRow + fSourceSize - pixeloffset) * fInverseSize;
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, s, t);
            nTextureRow = (nColumn + 2 > 4) ? nRow + 1 : nRow;
            nTextureColumn = (nColumn + 2) % 5;
            glActiveTextureARB(GL_TEXTURE2_ARB);
            t = (i == 0 || i == 3) ? (nTextureColumn + fSourceSize - pixeloffset) * fInverseSize : (nTextureColumn - pixeloffset) * fInverseSize;
            s = (i == 0 || i == 1) ? (nTextureRow - pixeloffset) * fInverseSize : (nTextureRow + fSourceSize - pixeloffset) * fInverseSize;
            glMultiTexCoord2fARB(GL_TEXTURE2_ARB, s, t);
            nTextureRow = (nColumn + 3 > 4) ? nRow + 1 : nRow;
            nTextureColumn = (nColumn + 3) % 5;
            glActiveTextureARB(GL_TEXTURE3_ARB);
            t = (i == 0 || i == 3) ? (nTextureColumn + fSourceSize - pixeloffset) * fInverseSize : (nTextureColumn - pixeloffset) * fInverseSize;
            s = (i == 0 || i == 1) ? (nTextureRow - pixeloffset) * fInverseSize : (nTextureRow + fSourceSize - pixeloffset) * fInverseSize;
            glMultiTexCoord2fARB(GL_TEXTURE3_ARB, s, t);
            glActiveTextureARB(GL_TEXTURE0_ARB);
            float y = (i == 0 || i == 3) ? fHeight : 0.0f;
            float x = (i == 0 || i == 1) ? 0.0f : fWidth;
            glVertex3f(x, y, 0.0f);
        }
        glEnd();
        if (nColumn + 4 > 4)
            ++nRow;
        nColumn = (nColumn + 4) % 5;
    }
    glActiveTextureARB(GL_TEXTURE0_ARB);
    wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);
    wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glDisable(GL_TEXTURE_2D);
    wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glFlush();
}
// PC 0x0042E680 GOG 0x0042E790
void GLRender::BloomFilter(HDC hDC, HGLRC hContext, float fWidth,
                           float fHeight, PBUFFERARB* pBuffer,
                           unsigned long nTexture, float fSourceSize,
                           int bSoftShadows)
{
    static float filter[28] = {
        0.12f, 0.25f, 0.29f, 0.25f, 0.12f,
        0.25f, 0.49f, 0.58f, 0.49f, 0.25f,
        0.29f, 0.58f, 0.67f, 0.58f, 0.29f,
        0.25f, 0.49f, 0.58f, 0.49f, 0.25f,
        0.12f, 0.25f, 0.29f, 0.25f, 0.12f,
        0.0f, 0.0f, 0.0f
    };
    wglMakeCurrent(hDC, hContext);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, fWidth, 0.0, fHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, (int)fWidth, (int)fHeight);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_PER_STAGE_CONSTANTS_NV);
    if (bSoftShadows)
        FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_SOFT_SHADOW, 0, 0, 0, 0);
    else
        FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_FOUR_DECALS, 0, 0, 0, 0);
    int nRow = 0;
    int nColumn = 0;
    glEnable(GL_BLEND);
    float fInverseSize = 1.0f / fSourceSize;
    for (int nSample = 0; nSample < 25; nSample += 4)
    {
        for (int nStage = 0; nStage < 4; ++nStage)
        {
            float color[4];
            if (bSoftShadows)
                color[3] = color[2] = color[1] = color[0] =
                    filter[nSample + nStage] * softshadowdarkeningfactor;
            else
            {
                color[0] = filter[nSample + nStage];
                color[1] = filter[nSample + nStage];
                color[2] = filter[nSample + nStage];
                color[3] = filter[nSample + nStage];
            }
            if (nStage == 0)
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR0_NV, color);
            else if (nStage == 1)
                glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV, color);
            else if (nStage == 2)
                glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR0_NV, color);
            else
                glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV, color);
        }
        glBegin(GL_QUADS);
        for (int i = 0; i < 4; ++i)
        {
            glColor4f(1.0f, 1.0f, 1.0f, multiplier);
            glActiveTextureARB(GL_TEXTURE0_ARB);
            float tBase = (i == 0 || i == 3) ? (nRow + fSourceSize - pixeloffset) * fInverseSize : (nRow - pixeloffset) * fInverseSize;
            float s = (i == 0 || i == 1) ? (float)(0.0 + (nColumn - pixeloffset) * fInverseSize) : (nColumn + fSourceSize - pixeloffset) * fInverseSize;
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, tBase);
            int nTextureRow = (nColumn + 1 > 4) ? nRow + 1 : nRow;
            int nTextureColumn = (nColumn + 1) % 5;
            glActiveTextureARB(GL_TEXTURE1_ARB);
            float t = (i == 0 || i == 3) ? (nTextureColumn + fSourceSize - pixeloffset) * fInverseSize : (nTextureColumn - pixeloffset) * fInverseSize;
            s = (i == 0 || i == 1) ? (nTextureRow - pixeloffset) * fInverseSize : (nTextureRow + fSourceSize - pixeloffset) * fInverseSize;
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, s, t);
            nTextureRow = (nColumn + 2 > 4) ? nRow + 1 : nRow;
            nTextureColumn = (nColumn + 2) % 5;
            glActiveTextureARB(GL_TEXTURE2_ARB);
            t = (i == 0 || i == 3) ? (nTextureColumn + fSourceSize - pixeloffset) * fInverseSize : (nTextureColumn - pixeloffset) * fInverseSize;
            s = (i == 0 || i == 1) ? (nTextureRow - pixeloffset) * fInverseSize : (nTextureRow + fSourceSize - pixeloffset) * fInverseSize;
            glMultiTexCoord2fARB(GL_TEXTURE2_ARB, s, t);
            nTextureRow = (nColumn + 3 > 4) ? nRow + 1 : nRow;
            nTextureColumn = (nColumn + 3) % 5;
            glActiveTextureARB(GL_TEXTURE3_ARB);
            t = (i == 0 || i == 3) ? (nTextureColumn + fSourceSize - pixeloffset) * fInverseSize : (nTextureColumn - pixeloffset) * fInverseSize;
            s = (i == 0 || i == 1) ? (nTextureRow - pixeloffset) * fInverseSize : (nTextureRow + fSourceSize - pixeloffset) * fInverseSize;
            glMultiTexCoord2fARB(GL_TEXTURE3_ARB, s, t);
            float y = (i == 0 || i == 3) ? fHeight : 0.0f;
            float x = (i == 0 || i == 1) ? 0.0f : fWidth;
            glVertex3f(x, y, 0.0f);
        }
        glEnd();
        if (nColumn + 4 > 4)
            ++nRow;
        nColumn = (nColumn + 4) % 5;
    }
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glFlush();
}
// PC 0x0042EDE0 GOG 0x0042EEF0
void GLRender::RenderOverlayConvolutionATI(unsigned long nTexture)
{
    if (!renderregular)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(enableTranslationTable[ENABLE_BLEND]);
    HDC hOldDC = m_hDC;
    HGLRC hOldContext = m_hGLRC;
    wglMakeContextCurrentARB(cm_hConvolutionHDC[3], cm_hConvolutionHDC[3],
                             cm_hConvolutionHGLRC[3]);
    glDrawBuffer(GL_FRONT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 512.0, 0.0, 512.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, 512, 512);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(enableTranslationTable[ENABLE_LIGHTING]);
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_CONVOLUTION,
                                           nTexture, cm_nRedTexture,
                                           cm_nGreenTexture, cm_nBlueTexture);
    float fMaxS = (float)g_nScreenWidth / (float)cm_nScreenWidthPow2;
    float fMaxT = (float)g_nScreenHeight / (float)cm_nScreenHeightPow2;
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? fMaxT : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : fMaxS;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
        float y = (i == 0 || i == 3) ? 512.0f : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : 512.0f;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glFlush();
    wglMakeContextCurrentARB(hOldDC, hOldDC, hOldContext);
    MipmapFilterATI(cm_hConvolutionHDC[2], cm_hConvolutionHGLRC[2],
                    256.0f, 256.0f, cm_hConvolutionPBuffer[3],
                    cm_nConvolutionRenderTexture[3], 512.0f);
    MipmapFilterATI(cm_hConvolutionHDC[1], cm_hConvolutionHGLRC[1],
                    128.0f, 128.0f, cm_hConvolutionPBuffer[2],
                    cm_nConvolutionRenderTexture[2], 256.0f);
    MipmapFilterATI(cm_hConvolutionHDC[0], cm_hConvolutionHGLRC[0],
                    64.0f, 64.0f, cm_hConvolutionPBuffer[1],
                    cm_nConvolutionRenderTexture[1], 128.0f);
    BloomFilterATI(cm_hConvolutionHDC[5], cm_hConvolutionHGLRC[5],
                   256.0f, 256.0f, cm_hConvolutionPBuffer[baselayer],
                   cm_nConvolutionRenderTexture[baselayer], basesize, 0);
    wglMakeContextCurrentARB(hOldDC, hOldDC, hOldContext);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, g_nScreenWidth, g_nScreenHeight);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[displaylayer]);
    wglBindTexImageARB(cm_hConvolutionPBuffer[displaylayer], WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_COPY, 0, 0, 0, 0);
    glDrawBuffer(GL_BACK);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? 1.0f : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(cm_hConvolutionPBuffer[displaylayer], WGL_FRONT_LEFT_ARB);
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
}
// PC 0x0042F360 GOG 0x0042F470
void GLRender::RenderOverlayConvolution(unsigned long nTexture)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(enableTranslationTable[ENABLE_BLEND]);
    wglMakeCurrent(cm_hConvolutionHDC[3], cm_hConvolutionHGLRC[3]);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 512.0, 0.0, 512.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, 512, 512);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(enableTranslationTable[ENABLE_LIGHTING]);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_CONVOLUTION,
        nTexture, cm_nRedTexture, cm_nGreenTexture, cm_nBlueTexture);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? 512.0f : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : 512.0f;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glFlush();
    MipmapFilter(cm_hConvolutionHDC[2], cm_hConvolutionHGLRC[2],
        256.0f, 256.0f, cm_hConvolutionPBuffer[3],
        cm_nConvolutionRenderTexture[3], 512.0f);
    MipmapFilter(cm_hConvolutionHDC[1], cm_hConvolutionHGLRC[1],
        128.0f, 128.0f, cm_hConvolutionPBuffer[2],
        cm_nConvolutionRenderTexture[2], 256.0f);
    MipmapFilter(cm_hConvolutionHDC[0], cm_hConvolutionHGLRC[0],
        64.0f, 64.0f, cm_hConvolutionPBuffer[1],
        cm_nConvolutionRenderTexture[1], 128.0f);
    BloomFilter(cm_hConvolutionHDC[5], cm_hConvolutionHGLRC[5],
        256.0f, 256.0f, cm_hConvolutionPBuffer[baselayer],
        cm_nConvolutionRenderTexture[baselayer], basesize, 0);
    wglMakeCurrent(m_hDC, m_hGLRC);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, g_nScreenWidth, g_nScreenHeight);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nConvolutionRenderTexture[displaylayer]);
    wglBindTexImageARB(cm_hConvolutionPBuffer[displaylayer], WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_COPY, 0, 0, 0, 0);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? 1.0f : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(cm_hConvolutionPBuffer[displaylayer], WGL_FRONT_LEFT_ARB);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
}
// PC 0x0042F840 GOG 0x0042F950
void GLRender::SetUpSoftShadowsATI()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nBufferRegionTexture);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, cm_nScreenWidthPow2, cm_nScreenHeightPow2);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
// PC 0x0042F8C0 GOG 0x0042F9D0
void GLRender::SetUpSoftShadows()
{
    if (AurATISoftShadowsAvailable())
    {
        GLRender::SetUpSoftShadowsATI();
        return;
    }
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, GLRender::cm_nBufferRegionTexture);
    glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, g_nScreenWidth, g_nScreenHeight);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
    glFlush();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
// PC 0x0042F950 GOG 0x0042FA60
void GLRender::FinishSoftShadowsATI()
{
    glDisable(enableTranslationTable[ENABLE_DEPTH_TEST]);
    glEnable(enableTranslationTable[ENABLE_BLEND]);
    glDisable(GL_STENCIL_TEST);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                       cm_nScreenWidthPow2, cm_nScreenHeightPow2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glFlush();
    glClear(GL_COLOR_BUFFER_BIT);
    wglMakeContextCurrentARB(cm_hSoftShadowHDC, cm_hSoftShadowHDC,
                            cm_hSoftShadowHGLRC);
    glDrawBuffer(GL_FRONT);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glViewport(0, 0, 512, 512);
    float fMaxS = (float)g_nScreenWidth / (float)cm_nScreenWidthPow2;
    float fMaxT = (float)g_nScreenHeight / (float)cm_nScreenHeightPow2;
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? fMaxT : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : fMaxS;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    wglMakeContextCurrentARB(m_hDC, m_hDC, m_hGLRC);
    glDrawBuffer(GL_BACK);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nBufferRegionTexture);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, g_nScreenWidth, g_nScreenHeight);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? fMaxT : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : fMaxS;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT);
    wglMakeContextCurrentARB(cm_hSoftShadowTargetHDC, cm_hSoftShadowTargetHDC,
                            cm_hSoftShadowTargetHGLRC);
    glDrawBuffer(GL_FRONT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, 512, 512);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    BloomFilterATI(cm_hSoftShadowTargetHDC, cm_hSoftShadowTargetHGLRC, 512.0f, 512.0f,
                   cm_hSoftShadowPBuffer, cm_nSoftShadowRenderTexture,
                   512.0f, 1);
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glFlush();
    wglMakeContextCurrentARB(m_hDC, m_hDC, m_hGLRC);
    glDrawBuffer(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, g_nScreenWidth, g_nScreenHeight);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nSoftShadowTargetRenderTexture);
    wglBindTexImageARB(cm_hSoftShadowTargetPBuffer, WGL_FRONT_LEFT_ARB);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? 1.0f : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    wglReleaseTexImageARB(cm_hSoftShadowTargetPBuffer, WGL_FRONT_LEFT_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(enableTranslationTable[ENABLE_DEPTH_TEST]);
    glDisable(GL_STENCIL_TEST);
    SetDepthMask(GL_TRUE);
    glStencilMask(STENCIL_WRITE_MASK);
    glDisable(GL_FRAGMENT_SHADER_ATI);
}
// PC 0x004300A0 GOG 0x004301B0
void GLRender::FinishSoftShadows()
{
    if (AurATISoftShadowsAvailable())
    {
        FinishSoftShadowsATI();
        return;
    }
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
    glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                       g_nScreenWidth, g_nScreenHeight);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
    glFlush();
    wglMakeCurrent(cm_hSoftShadowTargetHDC, cm_hSoftShadowTargetHGLRC);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, 512, 512);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    wglMakeCurrent(m_hDC, m_hGLRC);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nBufferRegionTexture);
    glDisable(GL_LIGHTING);
    glEnable(enableTranslationTable[ENABLE_BLEND]);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_STENCIL_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, g_nScreenWidth, g_nScreenHeight);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
    glDepthMask(GL_FALSE);
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT);
    wglMakeCurrent(cm_hSoftShadowHDC, cm_hSoftShadowHGLRC);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glViewport(0, 0, 512, 512);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    BloomFilter(cm_hSoftShadowHDC, cm_hSoftShadowHGLRC, 512.0f, 512.0f,
                cm_hSoftShadowTargetPBuffer, cm_nSoftShadowTargetRenderTexture,
                512.0f, 1);
    wglMakeCurrent(m_hDC, m_hGLRC);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nSoftShadowRenderTexture);
    wglBindTexImageARB(cm_hSoftShadowPBuffer, WGL_FRONT_LEFT_ARB);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? 1.0f : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    wglReleaseTexImageARB(cm_hSoftShadowPBuffer, WGL_FRONT_LEFT_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(enableTranslationTable[ENABLE_DEPTH_TEST]);
    glDisable(GL_STENCIL_TEST);
    SetDepthMask(GL_TRUE);
    glStencilMask(STENCIL_WRITE_MASK);
}
// PC 0x00430740 GOG 0x00430850
int GLRender::InitializeAntiAliasing()
{

    static char szClassName[] = "Test GL Window";
    HGLRC hContext;
    WNDCLASSA windowClass;
    PIXELFORMATDESCRIPTOR pixelFormat;
    OSVERSIONINFOA version;

    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    windowClass.lpfnWndProc = DefWindowProcA;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = g_hInst;
    windowClass.hIcon = 0;
    windowClass.hCursor = 0;
    windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    windowClass.lpszMenuName = 0;
    windowClass.lpszClassName = szClassName;
    RegisterClassA(&windowClass);

    HWND hWindow = CreateWindowExA(0, szClassName, "Exo Base Window",
                                   WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0,
                                   CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, 0);

    memset(&pixelFormat, 0, sizeof(pixelFormat));
    pixelFormat.nSize = sizeof(pixelFormat);
    pixelFormat.nVersion = 1;
    pixelFormat.dwFlags =
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixelFormat.iPixelType = PFD_TYPE_RGBA;
    pixelFormat.cDepthBits = 24;
    pixelFormat.cColorBits = 32;
    pixelFormat.iLayerType = PFD_MAIN_PLANE;
    pixelFormat.cStencilBits = 8;

    HDC hDC = GetDC(hWindow);
    int nPixelFormat = ChoosePixelFormat(hDC, &pixelFormat);
    if (!SetPixelFormat(hDC, nPixelFormat, 0))
    {
        return ReleaseDC(hWindow, hDC);
    }

    DescribePixelFormat(hDC, nPixelFormat, sizeof(pixelFormat), &pixelFormat);
    hContext = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hContext);

    wglGetExtensionsStringARB =
        (AurWglGetExtensionsStringProc)wglGetProcAddress(
            "wglGetExtensionsStringARB");
    wglChoosePixelFormatARB = (AurWglChoosePixelFormatProc)wglGetProcAddress(
        "wglChoosePixelFormatARB");

    const unsigned char* pExtensions = glGetString(GL_EXTENSIONS);
    int bATIFragmentShader = 0;
    if (CheckExtension("GL_ATI_fragment_shader", pExtensions))
    {
        bATIFragmentShader = 1;
    }
    if (wglGetExtensionsStringARB != 0)
    {
        const unsigned char* pWglExtensions = wglGetExtensionsStringARB(hDC);
        wglExtensions = pWglExtensions;
        if (pWglExtensions != 0 &&
            CheckExtension("WGL_ARB_multisample", pWglExtensions))
        {
            if (bATIFragmentShader == 1)
            {
                memset(&version, 0, sizeof(version));
                version.dwOSVersionInfoSize = sizeof(version);
                if (GetVersionExA(&version) && version.dwMajorVersion > 4)
                {
                    existingextensions |= WGL_ARB_MULTISAMPLE_BIT;
                }
            }
            else
            {
                existingextensions |= WGL_ARB_MULTISAMPLE_BIT;
            }
        }
    }

    wglDeleteContext(hContext);
    ReleaseDC(hWindow, hDC);
    return DestroyWindow(hWindow);
}
// PC 0x00430940 GOG 0x00430A50
void GLRender::EnableDreamDistortion(bool bLetterbox)
{
    m_bDreamDistortEnabled = true;
    m_bDreamDistortLetterbox = bLetterbox;
}
// PC 0x00430960 GOG 0x00430A70
void GLRender::DisableDreamDistortion()
{
    m_bDreamDistortEnabled = false;
}
// PC 0x00430970 GOG 0x00430A80
void GLRender::EnableFury(short nLevel)
{
    if (nLevel > 3)
        nLevel = 3;
    else if (nLevel < 0)
        nLevel = 0;
    if (nLevel != 0)
    {
        if (m_FuryLevel != nLevel)
        {
            if (m_pFuryTexture != 0)
            {
                SetCurrentStageTID(3, 0);
                AurTextureRelease(m_pFuryTexture);
            }
            switch (nLevel)
            {
                case 2:
                    m_pFuryTexture = AurTextureGetReference("fx_Fury02", 0);
                    break;
                case 3:
                    m_pFuryTexture = AurTextureGetReference("fx_Fury03", 0);
                    break;
                default:
                    m_pFuryTexture = AurTextureGetReference("fx_Fury01", 0);
                    break;
            }
            m_FuryLevel = nLevel;
        }
    }
    else
    {
        if (m_pFuryTexture != 0)
        {
            SetActiveTexture(3);
            glDisable(enableTranslationTable[ENABLE_TEXTURE_2D]);
            SetActiveTexture(0);
            AurTextureRelease(m_pFuryTexture);
            m_pFuryTexture = 0;
        }
        m_FuryLevel = 0;
    }
}
// PC 0x00430A60 GOG 0x00430B70
void GLRender::RenderDreamDistortion(unsigned long nTexture)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(enableTranslationTable[ENABLE_ALPHA_TEST]);
    glEnable(enableTranslationTable[ENABLE_BLEND]);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    cm_pForceDistortionTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    cm_uiCurrentStage = 1;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nAccumulationBlur);
    wglBindTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    m_pDreamDistortTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DREAM_DISTORTION,
                                        0, 0, 0, 0);
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, distortionalpha);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, (i == 0 || i == 1) ? 0.0f : 1.0f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glMultiTexCoord3fARB(GL_TEXTURE1_ARB, 0.15625f, 0.0f,
                             (i == 0 || i == 1) ? 0.0f : 1.0f);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glMultiTexCoord3fARB(GL_TEXTURE2_ARB, 0.0f, 0.15625f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glMultiTexCoord2fARB(GL_TEXTURE3_ARB, (i == 0 || i == 1) ? 0.0f : 1.0f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        if (g_bLetterBoxMode)
        {
            glVertex3f((i == 0 || i == 1) ? -25.0f : (float)g_nScreenWidth + 25.0f,
                       (i == 0 || i == 3) ? (float)g_Letterbox_bottom + 25.0f
                                          : (float)g_Letterbox_top - 25.0f,
                       0.0f);
        }
        else
        {
            float y = (i == 0 || i == 3) ? (float)g_nScreenHeight + 25.0f : -25.0f;
            float x = (i == 0 || i == 1) ? -25.0f : (float)g_nScreenWidth + 25.0f;
            glVertex3f(x, y, 0.0f);
        }
    }
    glEnd();
    glFlush();
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
}
// PC 0x00430EB0 GOG 0x00430FC0
void GLRender::RenderDreamDistortionATI(unsigned long nTexture)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0, (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, g_nScreenWidth, g_nScreenHeight);
    glDisable(GL_DEPTH_TEST);
    glEnable(enableTranslationTable[ENABLE_ALPHA_TEST]);
    glEnable(enableTranslationTable[ENABLE_BLEND]);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    cm_pForceDistortionTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    cm_uiCurrentStage = 1;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nAccumulationBlur);
    wglBindTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    m_pDreamDistortTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_FRAGMENT_SHADER_ATI);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_FRAGMENT_SHADER_ATI);
    float c7define[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
    glSetFragmentShaderConstantATI(GL_CON_7_ATI, c7define);
    glBindFragmentShaderATI(cm_nDreamDistortShaderATI);
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, distortionalpha);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, (i == 0 || i == 1) ? 0.0f : 1.0f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glMultiTexCoord3fARB(GL_TEXTURE1_ARB, 0.15625f, 0.0f,
                             (i == 0 || i == 1) ? 0.0f : 1.0f);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glMultiTexCoord3fARB(GL_TEXTURE2_ARB, 0.0f, 0.15625f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glMultiTexCoord2fARB(GL_TEXTURE3_ARB, (i == 0 || i == 1) ? 0.0f : 1.0f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        if (g_bLetterBoxMode)
        {
            glVertex3f((i == 0 || i == 1) ? -25.0f : (float)g_nScreenWidth + 25.0f,
                       (i == 0 || i == 3) ? (float)g_Letterbox_bottom + 25.0f
                                          : (float)g_Letterbox_top - 25.0f,
                       0.0f);
        }
        else
        {
            float y = (i == 0 || i == 3) ? (float)g_nScreenHeight + 25.0f : -25.0f;
            float x = (i == 0 || i == 1) ? -25.0f : (float)g_nScreenWidth + 25.0f;
            glVertex3f(x, y, 0.0f);
        }
    }
    glEnd();
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    wglReleaseTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_FRAGMENT_SHADER_ATI);
    glFlush();
}
// PC 0x00431400 GOG 0x00431510
void GLRender::RenderFury(unsigned long nTexture)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(enableTranslationTable[ENABLE_ALPHA_TEST]);
    glEnable(enableTranslationTable[ENABLE_BLEND]);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    cm_pForceDistortionTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    cm_uiCurrentStage = 1;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nAccumulationBlur);
    wglBindTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    m_pFuryTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DREAM_DISTORTION,
                                        0, 0, 0, 0);
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, distortionalpha);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, (i == 0 || i == 1) ? 0.0f : 1.0f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glMultiTexCoord3fARB(GL_TEXTURE1_ARB, 0.15625f, 0.0f,
                             (i == 0 || i == 1) ? 0.0f : 1.0f);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glMultiTexCoord3fARB(GL_TEXTURE2_ARB, 0.0f, 0.15625f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glMultiTexCoord2fARB(GL_TEXTURE3_ARB, (i == 0 || i == 1) ? 0.0f : 1.0f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        if (g_bLetterBoxMode)
        {
            glVertex3f((i == 0 || i == 1) ? -25.0f : (float)g_nScreenWidth + 25.0f,
                       (i == 0 || i == 3) ? (float)g_Letterbox_bottom + 25.0f
                                          : (float)g_Letterbox_top - 25.0f,
                       0.0f);
        }
        else
        {
            float y = (i == 0 || i == 3) ? (float)g_nScreenHeight + 25.0f : -25.0f;
            float x = (i == 0 || i == 1) ? -25.0f : (float)g_nScreenWidth + 25.0f;
            glVertex3f(x, y, 0.0f);
        }
    }
    glEnd();
    glFlush();
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
}
// PC 0x00431850 GOG 0x00431960
void GLRender::RenderFuryATI(unsigned long nTexture)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0, (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, g_nScreenWidth, g_nScreenHeight);
    glDisable(GL_DEPTH_TEST);
    glEnable(enableTranslationTable[ENABLE_ALPHA_TEST]);
    glEnable(enableTranslationTable[ENABLE_BLEND]);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    cm_pForceDistortionTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    cm_uiCurrentStage = 1;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nAccumulationBlur);
    wglBindTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    m_pFuryTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_FRAGMENT_SHADER_ATI);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_FRAGMENT_SHADER_ATI);
    float c7define[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
    glSetFragmentShaderConstantATI(GL_CON_7_ATI, c7define);
    glBindFragmentShaderATI(cm_nDreamDistortShaderATI);
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, distortionalpha);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, (i == 0 || i == 1) ? 0.0f : 1.0f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glMultiTexCoord3fARB(GL_TEXTURE1_ARB, 0.15625f, 0.0f,
                             (i == 0 || i == 1) ? 0.0f : 1.0f);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glMultiTexCoord3fARB(GL_TEXTURE2_ARB, 0.0f, 0.15625f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        glActiveTextureARB(GL_TEXTURE3_ARB);
        glMultiTexCoord2fARB(GL_TEXTURE3_ARB, (i == 0 || i == 1) ? 0.0f : 1.0f,
                             (i == 0 || i == 3) ? 1.0f : 0.0f);
        if (g_bLetterBoxMode)
        {
            glVertex3f((i == 0 || i == 1) ? -25.0f : (float)g_nScreenWidth + 25.0f,
                       (i == 0 || i == 3) ? (float)g_Letterbox_bottom + 25.0f
                                          : (float)g_Letterbox_top - 25.0f,
                       0.0f);
        }
        else
        {
            float y = (i == 0 || i == 3) ? (float)g_nScreenHeight + 25.0f : -25.0f;
            float x = (i == 0 || i == 1) ? -25.0f : (float)g_nScreenWidth + 25.0f;
            glVertex3f(x, y, 0.0f);
        }
    }
    glEnd();
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    wglReleaseTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_FRAGMENT_SHADER_ATI);
    glFlush();
}
// PC 0x00431DA0 GOG 0x00431EF0
inline RequestPoolManager::~RequestPoolManager()
{
}
// PC 0x00434E20 GOG 0x00434F70
RequestPoolManagerVBO::RequestPoolManagerVBO()
{
    m_nPoolCount = 100;
    for (unsigned int nPool = 0; nPool < m_nPoolCount; nPool++)
    {
        m_lstFreePools.Add(nPool);
        m_lstPoolUsage.Add(0);
        m_lstPoolLocks.Add(0);
        m_lstPools.Add(0);
        m_lstPoolTypes.Add(GL_ARRAY_BUFFER_ARB);
    }
}
// PC 0x004350B0 GOG 0x00435200
RequestPoolManagerVBO::~RequestPoolManagerVBO()
{
    for (unsigned int nPool = 0; nPool < m_nPoolCount; nPool++)
    {
        if (m_lstPools[nPool] != 0)
        {
            glDeleteBuffersARB(1, (unsigned int*)&m_lstPools[nPool]);
            m_lstPools[nPool] = 0;
        }
    }

    for (int nBackup = 0; nBackup < m_lstPoolBackup.m_nCount; nBackup++)
    {
        delete[] m_lstPoolBackup[nBackup];
    }

    m_lstPoolBackup.SetSize(0);
    m_lstPoolBackupSize.SetSize(0);
    m_lstPoolTypes.SetSize(0);
}
// PC 0x00435200 GOG 0x00435350
int RequestPoolManagerVBO::RequestPoolGeneric(int nType, int nSize, int nUsage)
{
    if (nSize == 0)
    {
        return INVALID_POOL;
    }
    static unsigned long totalAllocs = 0;
    totalAllocs += nSize;

    if (m_lstFreePools.m_nCount != 0)
    {
        int nPool = m_lstFreePools[m_lstFreePools.m_nCount - 1];
        m_lstFreePools.DelIndex(m_lstFreePools.m_nCount - 1);
        m_lstPoolLocks[nPool] = 0;
        m_lstPoolUsage[nPool] = nUsage;
        m_lstPools[nPool] = 0;
        glGenBuffersARB(1, (unsigned int*)&m_lstPools[nPool]);
        if (nType == Aurora::POOL_VERTEX)
        {
            m_lstPoolTypes[nPool] = GL_ARRAY_BUFFER_ARB;
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_lstPools[nPool]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, nSize, 0, GL_DYNAMIC_DRAW_ARB);
            return nPool;
        }
        else
        {
            m_lstPoolTypes[nPool] = GL_ELEMENT_ARRAY_BUFFER_ARB;
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_lstPools[nPool]);
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, nSize, 0, GL_DYNAMIC_DRAW_ARB);
            return nPool;
        }
    }

    m_lstPoolLocks.Add(0);
    m_lstPoolUsage.Add(0);
    m_lstPools.Add(0);
    m_lstPoolTypes.Add(GL_ARRAY_BUFFER_ARB);

    for (unsigned int nPool = m_nPoolCount + 1;
         nPool < (unsigned int)(2 * m_nPoolCount); ++nPool)
    {
        m_lstFreePools.Add(nPool);
        m_lstPoolLocks.Add(0);
        m_lstPoolUsage.Add(0);
        m_lstPools.Add(0);
        m_lstPoolTypes.Add(GL_ARRAY_BUFFER_ARB);
    }

    glGenBuffersARB(1, (unsigned int*)&m_lstPools[m_nPoolCount]);
    if (nType == Aurora::POOL_VERTEX)
    {
        m_lstPoolTypes[m_nPoolCount] = GL_ARRAY_BUFFER_ARB;
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_lstPools[m_nPoolCount]);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, nSize, 0, GL_DYNAMIC_DRAW_ARB);
    }
    else
    {
        m_lstPoolTypes[m_nPoolCount] = GL_ELEMENT_ARRAY_BUFFER_ARB;
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_lstPools[m_nPoolCount]);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, nSize, 0, GL_DYNAMIC_DRAW_ARB);
    }
    int nResult = m_nPoolCount;
    m_nPoolCount *= 2;
    return nResult;
}
// PC 0x00435560 GOG 0x004356B0
void RequestPoolManagerVBO::DeletePool(unsigned int nPool)
{
    if (nPool == INVALID_POOL)
    {
        return;
    }

    m_lstFreePools.Add(nPool);
    m_lstPoolLocks[nPool] = 0;
    unsigned int nBuffer = m_lstPools[nPool];
    glDeleteBuffersARB(1, &nBuffer);
    m_lstPools[nPool] = 0;
}
// PC 0x004355D0 GOG 0x00435720
RequestPoolManagerSys::RequestPoolManagerSys()
{
    m_nPoolCount = 100;
    m_ppPools = new char*[m_nPoolCount];
    m_pPoolSize = new unsigned long[m_nPoolCount];
    memset(m_pPoolSize, 0, 4 * m_nPoolCount);
    memset(m_ppPools, 0, 4 * m_nPoolCount);
    for (int nPool = 0; nPool < 100; nPool++)
    {
        m_lstFreePools.Add(nPool);
        m_lstPoolLocks.Add(0);
    }
}
// PC 0x00435780 GOG 0x004358D0
int RequestPoolManagerSys::RequestPoolGeneric(int nType, int nSize, int nUsage)
{
    totalvertexpool += nSize;
    totalvertexpool4k += (nSize + 4095) & 0xFFFFF000;
    if (nSize == 0)
    {
        return -1;
    }
    static unsigned long totalAllocs = 0;
    totalAllocs += nSize;

    if (m_lstFreePools.m_nCount != 0)
    {
        char* pPool = new char[nSize];
        int nPool = m_lstFreePools[m_lstFreePools.m_nCount - 1];
        m_lstFreePools.DelIndex(m_lstFreePools.m_nCount - 1);
        m_ppPools[nPool] = pPool;
        m_pPoolSize[nPool] = nSize;
        return nPool;
    }

    m_lstPoolLocks.Add(0);

    for (unsigned int nPool = m_nPoolCount + 1;
         nPool < (unsigned int)(2 * m_nPoolCount); ++nPool)
    {
        m_lstFreePools.Add(nPool);
        m_lstPoolLocks.Add(0);
    }

    char** ppPools = new char*[2 * m_nPoolCount];
    unsigned long* pPoolSizes = new unsigned long[2 * m_nPoolCount];
    memset(ppPools, 0, 8 * m_nPoolCount);
    memcpy(ppPools, m_ppPools, 4 * m_nPoolCount);
    delete[] m_ppPools;
    memset(pPoolSizes, 0, 8 * m_nPoolCount);
    memcpy(pPoolSizes, m_pPoolSize, 4 * m_nPoolCount);
    delete[] m_pPoolSize;
    m_ppPools = ppPools;
    m_pPoolSize = pPoolSizes;
    m_ppPools[m_nPoolCount] = new char[nSize];
    m_pPoolSize[m_nPoolCount] = nSize;
    int nResult = m_nPoolCount;
    m_nPoolCount *= 2;
    return nResult;
}
// PC 0x00435980 GOG 0x00435AD0
void RequestPoolManagerSys::DeletePool(unsigned int nPool)
{
    if (nPool == INVALID_POOL)
    {
        return;
    }

    m_lstFreePools.Add(nPool);
    totalvertexpool -= m_pPoolSize[nPool];
    totalvertexpool4k -=
        (m_pPoolSize[nPool] + POOL_PAGE_ROUNDING) & POOL_PAGE_MASK;
    m_lstPoolLocks[nPool] = 0;
    if (m_ppPools[nPool] != 0)
    {
        delete[] m_ppPools[nPool];
    }
    m_ppPools[nPool] = 0;
    m_pPoolSize[nPool] = 0;
}
// PC 0x00446BA0 GOG 0x00446CF0
void GLRender::AddSquareToRender(unsigned long nPool)
{
    if (nPool != (unsigned long)-1)
    {
        cm_listSquaresToRender.Add(nPool);
    }
}
// PC 0x00446BF0 GOG 0x00446D40
void GLRender::RemoveSquareToRender(unsigned long nPool)
{
    if (nPool != (unsigned long)-1)
    {
        if (cm_listSquaresToRender.Contains(nPool))
        {
            cm_listSquaresToRender.Remove(nPool);
        }
    }
}
// PC 0x00447610 GOG 0x00447760
void GLRender::InitExtensions()
{
    wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
        wglGetProcAddress("wglGetExtensionsStringARB");
    const unsigned char* pExtensions = glGetString(GL_EXTENSIONS);
    const unsigned char* pWglExtensions = 0;
    if (wglGetExtensionsStringARB != 0)
        pWglExtensions = wglGetExtensionsStringARB(wglGetCurrentDC());

    if (CheckExtension("GL_EXT_stencil_two_side", pExtensions))
        existingextensions |= GL_STENCIL_TWO_SIDED_BIT;
    if (CheckExtension("GL_EXT_bgra", pExtensions))
        existingextensions |= EXT_BGRA_BIT;
    if (CheckExtension("GL_ARB_multitexture", pExtensions))
        existingextensions |= ARB_MULTITEXTURE_BIT;
    if (CheckExtension("GL_EXT_texture_env_combine", pExtensions))
        existingextensions |= EXT_TEXTURE_ENV_COMBINE_BIT;
    if (CheckExtension("GL_NV_register_combiners", pExtensions))
        existingextensions |= NV_REGISTER_COMBINERS_BIT;
    if (CheckExtension("GL_NV_register_combiners2", pExtensions))
        existingextensions |= NV_REGISTER_COMBINERS2_BIT;
    if (CheckExtension("GL_EXT_texture_cube_map", pExtensions))
        existingextensions |= EXT_TEXTURE_CUBE_MAP_BIT;
    if (CheckExtension("GL_EXT_fog_coord", pExtensions))
        existingextensions |= EXT_FOG_COORD_BIT;
    if (CheckExtension("GL_EXT_secondary_color", pExtensions))
        existingextensions |= EXT_SECONDARY_COLOR_BIT;
    if (CheckExtension("GL_NV_vertex_program", pExtensions))
        existingextensions |= NV_VERTEX_PROGRAMS_BIT;
    if (CheckExtension("GL_EXT_texture_filter_anisotropic", pExtensions))
        existingextensions |= EXT_TEXTURE_FILTER_ANISOTROPIC_BIT;
    if (CheckExtension("GL_NV_texture_shader", pExtensions))
        existingextensions |= NV_PIXEL_SHADERS_BIT;
    if (CheckExtension("GL_NV_vertex_array_range", pExtensions))
        existingextensions |= NV_VERTEX_ARRAY_RANGE_BIT;
    if (CheckExtension("GL_NV_vertex_array_range2", pExtensions))
        existingextensions |= NV_VERTEX_ARRAY_RANGE2_BIT;
    if (CheckExtension("GL_NV_fence", pExtensions))
        existingextensions |= NV_FENCE_BIT;
    if (CheckExtension("GL_ARB_texture_compression", pExtensions))
        existingextensions |= ARB_TEXTURE_COMPRESSION_BIT;
    if (CheckExtension("GL_EXT_texture_compression_s3tc", pExtensions))
        existingextensions |= EXT_TEXTURE_COMPRESSION_S3TC_BIT;
    if (CheckExtension("GL_ATI_pn_triangles", pExtensions))
        existingextensions |= ATIX_PN_TRIANGLES_BIT;
    if (CheckExtension("GL_EXT_compiled_vertex_array", pExtensions))
        existingextensions |= EXT_COMPILED_VERTEX_ARRAY_BIT;
    if (pWglExtensions != 0 && CheckExtension("WGL_ARB_render_texture", pWglExtensions))
        existingextensions |= WGL_ARB_RENDER_TO_TEXTURE_BIT;
    if (CheckExtension("GL_SGIS_generate_mipmap", pExtensions))
        existingextensions |= EXT_SGIS_MIPMAP_BIT;
    if (CheckExtension("GL_ATI_fragment_shader", pExtensions))
        existingextensions |= ATI_FRAGMENT_SHADER_BIT;
    if (CheckExtension("GL_ARB_vertex_program", pExtensions))
        existingextensions |= GL_ARB_VERTEX_PROGRAM_BIT;
    if (CheckExtension("GL_ARB_depth_texture", pExtensions))
        existingextensions |= GL_ARB_DEPTH_TEXTURE_BIT;
    if (CheckExtension("GL_ARB_vertex_buffer_object", pExtensions))
        existingextensions |= GL_ARB_VERTEX_BUFFER_OBJECT_BIT;
    if (pWglExtensions != 0 && CheckExtension("WGL_NV_render_texture_rectangle", pWglExtensions))
        existingextensions |= WGL_NV_RENDER_TO_TEXTURE_RECTANGLE_BIT;
    if (pWglExtensions != 0 && CheckExtension("WGL_ARB_buffer_region", pWglExtensions))
        existingextensions |= WGL_ARB_BUFFER_REGION_BIT;
    if (pWglExtensions != 0 && CheckExtension("WGL_EXT_swap_control", pWglExtensions))
        existingextensions |= EXT_SWAP_CONTROL_BIT;

    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)
        wglGetProcAddress("glMultiTexCoord2fARB");
    glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)
        wglGetProcAddress("glMultiTexCoord2iARB");
    glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)
        wglGetProcAddress("glMultiTexCoord3fARB");
    glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)
        wglGetProcAddress("glMultiTexCoord2fvARB");
    glMultiTexCoord3fvARB = (PFNGLMULTITEXCOORD3FVARBPROC)
        wglGetProcAddress("glMultiTexCoord3fvARB");
    glMultiTexCoord4fvARB = (PFNGLMULTITEXCOORD4FVARBPROC)
        wglGetProcAddress("glMultiTexCoord4fvARB");
    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
        wglGetProcAddress("glActiveTextureARB");
    glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)
        wglGetProcAddress("glClientActiveTextureARB");
    glTexImage3D = (PFNGLTEXIMAGE3DPROC)
        wglGetProcAddress("glTexImage3D");
    glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)
        wglGetProcAddress("glTexSubImage3D");
    glBindBufferARB = (PFNGLBINDBUFFERARBPROC)
        wglGetProcAddress("glBindBufferARB");
    glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)
        wglGetProcAddress("glDeleteBuffersARB");
    glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)
        wglGetProcAddress("glGenBuffersARB");
    glIsBufferARB = (PFNGLISBUFFERARBPROC)
        wglGetProcAddress("glIsBufferARB");
    glBufferDataARB = (PFNGLBUFFERDATAARBPROC)
        wglGetProcAddress("glBufferDataARB");
    glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)
        wglGetProcAddress("glBufferSubDataARB");
    glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)
        wglGetProcAddress("glGetBufferSubDataARB");
    glMapBufferARB = (PFNGLMAPBUFFERARBPROC)
        wglGetProcAddress("glMapBufferARB");
    glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)
        wglGetProcAddress("glUnmapBufferARB");
    glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)
        wglGetProcAddress("glGetBufferParameterivARB");
    glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)
        wglGetProcAddress("glGetBufferPointervARB");
    glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SARBPROC)
        wglGetProcAddress("glVertexAttrib1sARB");
    glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC)
        wglGetProcAddress("glVertexAttrib1fARB");
    glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DARBPROC)
        wglGetProcAddress("glVertexAttrib1dARB");
    glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SARBPROC)
        wglGetProcAddress("glVertexAttrib2sARB");
    glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC)
        wglGetProcAddress("glVertexAttrib2fARB");
    glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DARBPROC)
        wglGetProcAddress("glVertexAttrib2dARB");
    glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SARBPROC)
        wglGetProcAddress("glVertexAttrib3sARB");
    glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC)
        wglGetProcAddress("glVertexAttrib3fARB");
    glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DARBPROC)
        wglGetProcAddress("glVertexAttrib3dARB");
    glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SARBPROC)
        wglGetProcAddress("glVertexAttrib4sARB");
    glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC)
        wglGetProcAddress("glVertexAttrib4fARB");
    glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DARBPROC)
        wglGetProcAddress("glVertexAttrib4dARB");
    glVertexAttrib4NubARB = (PFNGLVERTEXATTRIB4NUBARBPROC)
        wglGetProcAddress("glVertexAttrib4NubARB");
    glVertexAttrib1svARB = (PFNGLVERTEXATTRIB1SVARBPROC)
        wglGetProcAddress("glVertexAttrib1svARB");
    glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)
        wglGetProcAddress("glVertexAttrib1fvARB");
    glVertexAttrib1dvARB = (PFNGLVERTEXATTRIB1DVARBPROC)
        wglGetProcAddress("glVertexAttrib1dvARB");
    glVertexAttrib2svARB = (PFNGLVERTEXATTRIB2SVARBPROC)
        wglGetProcAddress("glVertexAttrib2svARB");
    glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVARBPROC)
        wglGetProcAddress("glVertexAttrib2fvARB");
    glVertexAttrib2dvARB = (PFNGLVERTEXATTRIB2DVARBPROC)
        wglGetProcAddress("glVertexAttrib2dvARB");
    glVertexAttrib3svARB = (PFNGLVERTEXATTRIB3SVARBPROC)
        wglGetProcAddress("glVertexAttrib3svARB");
    glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVARBPROC)
        wglGetProcAddress("glVertexAttrib3fvARB");
    glVertexAttrib3dvARB = (PFNGLVERTEXATTRIB3DVARBPROC)
        wglGetProcAddress("glVertexAttrib3dvARB");
    glVertexAttrib4bvARB = (PFNGLVERTEXATTRIB4BVARBPROC)
        wglGetProcAddress("glVertexAttrib4bvARB");
    glVertexAttrib4svARB = (PFNGLVERTEXATTRIB4SVARBPROC)
        wglGetProcAddress("glVertexAttrib4svARB");
    glVertexAttrib4ivARB = (PFNGLVERTEXATTRIB4IVARBPROC)
        wglGetProcAddress("glVertexAttrib4ivARB");
    glVertexAttrib4ubvARB = (PFNGLVERTEXATTRIB4UBVARBPROC)
        wglGetProcAddress("glVertexAttrib4ubvARB");
    glVertexAttrib4usvARB = (PFNGLVERTEXATTRIB4USVARBPROC)
        wglGetProcAddress("glVertexAttrib4usvARB");
    glVertexAttrib4uivARB = (PFNGLVERTEXATTRIB4UIVARBPROC)
        wglGetProcAddress("glVertexAttrib4uivARB");
    glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC)
        wglGetProcAddress("glVertexAttrib4fvARB");
    glVertexAttrib4dvARB = (PFNGLVERTEXATTRIB4DVARBPROC)
        wglGetProcAddress("glVertexAttrib4dvARB");
    glVertexAttrib4NbvARB = (PFNGLVERTEXATTRIB4NBVARBPROC)
        wglGetProcAddress("glVertexAttrib4NbvARB");
    glVertexAttrib4NsvARB = (PFNGLVERTEXATTRIB4NSVARBPROC)
        wglGetProcAddress("glVertexAttrib4NsvARB");
    glVertexAttrib4NivARB = (PFNGLVERTEXATTRIB4NIVARBPROC)
        wglGetProcAddress("glVertexAttrib4NivARB");
    glVertexAttrib4NubvARB = (PFNGLVERTEXATTRIB4NUBVARBPROC)
        wglGetProcAddress("glVertexAttrib4NubvARB");
    glVertexAttrib4NusvARB = (PFNGLVERTEXATTRIB4NUSVARBPROC)
        wglGetProcAddress("glVertexAttrib4NusvARB");
    glVertexAttrib4NuivARB = (PFNGLVERTEXATTRIB4NUIVARBPROC)
        wglGetProcAddress("glVertexAttrib4NuivARB");
    glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)
        wglGetProcAddress("glVertexAttribPointerARB");
    glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)
        wglGetProcAddress("glEnableVertexAttribArrayARB");
    glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)
        wglGetProcAddress("glDisableVertexAttribArrayARB");
    glProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)
        wglGetProcAddress("glProgramStringARB");
    glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)
        wglGetProcAddress("glBindProgramARB");
    glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC)
        wglGetProcAddress("glDeleteProgramsARB");
    glGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)
        wglGetProcAddress("glGenProgramsARB");
    glProgramEnvParameter4dARB = (PFNGLPROGRAMENVPARAMETER4DARBPROC)
        wglGetProcAddress("glProgramEnvParameter4dARB");
    glProgramEnvParameter4dvARB = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)
        wglGetProcAddress("glProgramEnvParameter4dvARB");
    glProgramEnvParameter4fARB = (PFNGLPROGRAMENVPARAMETER4FARBPROC)
        wglGetProcAddress("glProgramEnvParameter4fARB");
    glProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)
        wglGetProcAddress("glProgramEnvParameter4fvARB");
    glProgramLocalParameter4dARB = (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)
        wglGetProcAddress("glProgramLocalParameter4dARB");
    glProgramLocalParameter4dvARB = (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)
        wglGetProcAddress("glProgramLocalParameter4dvARB");
    glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)
        wglGetProcAddress("glProgramLocalParameter4fARB");
    glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)
        wglGetProcAddress("glProgramLocalParameter4fvARB");
    glGetProgramEnvParameterdvARB = (PFNGLGETPROGRAMENVPARAMETERDVARBPROC)
        wglGetProcAddress("glGetProgramEnvParameterdvARB");
    glGetProgramEnvParameterfvARB = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC)
        wglGetProcAddress("glGetProgramEnvParameterfvARB");
    glGetProgramLocalParameterdvARB = (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC)
        wglGetProcAddress("glGetProgramLocalParameterdvARB");
    glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)
        wglGetProcAddress("glGetProgramLocalParameterfvARB");
    glGetProgramivARB = (PFNGLGETPROGRAMIVARBPROC)
        wglGetProcAddress("glGetProgramivARB");
    glGetProgramStringARB = (PFNGLGETPROGRAMSTRINGARBPROC)
        wglGetProcAddress("glGetProgramStringARB");
    glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVARBPROC)
        wglGetProcAddress("glGetVertexAttribdvARB");
    glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVARBPROC)
        wglGetProcAddress("glGetVertexAttribfvARB");
    glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVARBPROC)
        wglGetProcAddress("glGetVertexAttribivARB");
    glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)
        wglGetProcAddress("glGetVertexAttribPointervARB");
    glIsProgramARB = (PFNGLISPROGRAMARBPROC)
        wglGetProcAddress("glIsProgramARB");
    glCombinerParameterfvNV = (PFNGLCOMBINERPARAMETERFVNVPROC)
        wglGetProcAddress("glCombinerParameterfvNV");
    glCombinerParameterivNV = (PFNGLCOMBINERPARAMETERIVNVPROC)
        wglGetProcAddress("glCombinerParameterivNV");
    glCombinerParameterfNV = (PFNGLCOMBINERPARAMETERFNVPROC)
        wglGetProcAddress("glCombinerParameterfNV");
    glCombinerParameteriNV = (PFNGLCOMBINERPARAMETERINVPROC)
        wglGetProcAddress("glCombinerParameteriNV");
    glCombinerInputNV = (PFNGLCOMBINERINPUTNVPROC)
        wglGetProcAddress("glCombinerInputNV");
    glCombinerOutputNV = (PFNGLCOMBINEROUTPUTNVPROC)
        wglGetProcAddress("glCombinerOutputNV");
    glFinalCombinerInputNV = (PFNGLFINALCOMBINERINPUTNVPROC)
        wglGetProcAddress("glFinalCombinerInputNV");
    glGetCombinerInputParameterfvNV = (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)
        wglGetProcAddress("glGetCombinerInputParameterfvNV");
    glGetCombinerInputParameterivNV = (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)
        wglGetProcAddress("glGetCombinerInputParameterivNV");
    glGetCombinerOutputParameterfvNV = (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)
        wglGetProcAddress("glGetCombinerOutputParameterfvNV");
    glGetCombinerOutputParameterivNV = (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)
        wglGetProcAddress("glGetCombinerOutputParameterivNV");
    glGetFinalCombinerInputParameterfvNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)
        wglGetProcAddress("glGetFinalCombinerInputParameterfvNV");
    glGetFinalCombinerInputParameterivNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)
        wglGetProcAddress("glGetFinalCombinerInputParameterivNV");
    glCombinerStageParameterfvNV = (PFNGLCOMBINERSTAGEPARAMETERFVNVPROC)
        wglGetProcAddress("glCombinerStageParameterfvNV");
    glFogCoordPointerEXT = (PFNGLFOGCOORDPOINTEREXTPROC)
        wglGetProcAddress("glFogCoordPointerEXT");
    glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC)
        wglGetProcAddress("glFogCoordfEXT");
    glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC)
        wglGetProcAddress("glLockArraysEXT");
    glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC)
        wglGetProcAddress("glUnlockArraysEXT");
    glSecondaryColor3fvEXT = (PFNGLSECONDARYCOLOR3FVEXTPROC)
        wglGetProcAddress("glSecondaryColor3fvEXT");
    glGenProgramsNV = (PFNGLGENPROGRAMSNVPROC)
        wglGetProcAddress("glGenProgramsNV");
    glDeleteProgramsNV = (PFNGLDELETEPROGRAMSNVPROC)
        wglGetProcAddress("glDeleteProgramsNV");
    glLoadProgramNV = (PFNGLLOADPROGRAMNVPROC)
        wglGetProcAddress("glLoadProgramNV");
    glBindProgramNV = (PFNGLBINDPROGRAMNVPROC)
        wglGetProcAddress("glBindProgramNV");
    glTrackMatrixNV = (PFNGLTRACKMATRIXNVPROC)
        wglGetProcAddress("glTrackMatrixNV");
    glProgramParameter4fNV = (PFNGLPROGRAMPARAMETER4FNVPROC)
        wglGetProcAddress("glProgramParameter4fNV");
    glExecuteProgramNV = (PFNGLEXECUTEPROGRAMNVPROC)
        wglGetProcAddress("glExecuteProgramNV");
    glGetProgramParameterfvNV = (PFNGLGETPROGRAMPARAMETERFVNVPROC)
        wglGetProcAddress("glGetProgramParameterfvNV");
    glVertexAttribPointerNV = (PFNGLVERTEXATTRIBPOINTERNVPROC)
        wglGetProcAddress("glVertexAttribPointerNV");
    glIsProgramNV = (PFNGLISPROGRAMNVPROC)
        wglGetProcAddress("glIsProgramNV");
    glVertexAttrib1fvNV = (PFNGLVERTEXATTRIB1FVNVPROC)
        wglGetProcAddress("glVertexAttrib1fvNV");
    glVertexAttrib2fvNV = (PFNGLVERTEXATTRIB2FVNVPROC)
        wglGetProcAddress("glVertexAttrib2fvNV");
    glVertexAttrib3fvNV = (PFNGLVERTEXATTRIB3FVNVPROC)
        wglGetProcAddress("glVertexAttrib3fvNV");
    glVertexAttrib4fvNV = (PFNGLVERTEXATTRIB4FVNVPROC)
        wglGetProcAddress("glVertexAttrib4fvNV");
    wglAllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC)
        wglGetProcAddress("wglAllocateMemoryNV");
    wglFreeMemoryNV = (PFNWGLFREEMEMORYNVPROC)
        wglGetProcAddress("wglFreeMemoryNV");
    glVertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC)
        wglGetProcAddress("glVertexArrayRangeNV");
    glDeleteFencesNV = (PFNGLDELETEFENCESNVPROC)
        wglGetProcAddress("glDeleteFencesNV");
    glGenFencesNV = (PFNGLGENFENCESNVPROC)
        wglGetProcAddress("glGenFencesNV");
    glIsFenceNV = (PFNGLISFENCENVPROC)
        wglGetProcAddress("glIsFenceNV");
    glTestFenceNV = (PFNGLTESTFENCENVPROC)
        wglGetProcAddress("glTestFenceNV");
    glGetFenceivNV = (PFNGLGETFENCEIVNVPROC)
        wglGetProcAddress("glGetFenceivNV");
    glFinishFenceNV = (PFNGLFINISHFENCENVPROC)
        wglGetProcAddress("glFinishFenceNV");
    glSetFenceNV = (PFNGLSETFENCENVPROC)
        wglGetProcAddress("glSetFenceNV");
    glGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)
        wglGetProcAddress("glGetCompressedTexImageARB");
    glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)
        wglGetProcAddress("glCompressedTexImage2DARB");
    glPNTrianglesiATI = (PFNGLPNTRIANGLESIATIPROC)
        wglGetProcAddress("glPNTrianglesiATI");
    glPNTrianglesfATI = (PFNGLPNTRIANGLESFATIPROC)
        wglGetProcAddress("glPNTrianglesfATI");
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)
        wglGetProcAddress("wglChoosePixelFormatARB");
    wglCreatePBufferARB = (PFNWGLCREATEPBUFFERARBPROC)
        wglGetProcAddress("wglCreatePbufferARB");
    wglGetPBufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)
        wglGetProcAddress("wglGetPbufferDCARB");
    wglReleasePBufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)
        wglGetProcAddress("wglReleasePbufferDCARB");
    wglDestroyPBufferARB = (PFNWGLDESTROYPBUFFERARBPROC)
        wglGetProcAddress("wglDestroyPbufferARB");
    wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
        wglGetProcAddress("wglGetPixelFormatAttribivARB");
    wglQueryPBufferARB = (PFNWGLQUERYPBUFFERARBPROC)
        wglGetProcAddress("wglQueryPbufferARB");
    wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)
        wglGetProcAddress("wglMakeContextCurrentARB");
    wglBindTexImageARB = (PFNWGLBINDTEXIMAGEARBPROC)
        wglGetProcAddress("wglBindTexImageARB");
    wglReleaseTexImageARB = (PFNWGLRELEASETEXIMAGEARBPROC)
        wglGetProcAddress("wglReleaseTexImageARB");
    glGenFragmentShadersATI = (PFNGLGENFRAGMENTSHADERSATIPROC)
        wglGetProcAddress("glGenFragmentShadersATI");
    glBindFragmentShaderATI = (PFNGLBINDFRAGMENTSHADERATIPROC)
        wglGetProcAddress("glBindFragmentShaderATI");
    glDeleteFragmentShaderATI = (PFNGLDELETEFRAGMENTSHADERATIPROC)
        wglGetProcAddress("glDeleteFragmentShaderATI");
    glBeginFragmentShaderATI = (PFNGLBEGINFRAGMENTSHADERATIPROC)
        wglGetProcAddress("glBeginFragmentShaderATI");
    glEndFragmentShaderATI = (PFNGLENDFRAGMENTSHADERATIPROC)
        wglGetProcAddress("glEndFragmentShaderATI");
    glPassTexCoordATI = (PFNGLPASSTEXCOORDATIPROC)
        wglGetProcAddress("glPassTexCoordATI");
    glSampleMapATI = (PFNGLSAMPLEMAPATIPROC)
        wglGetProcAddress("glSampleMapATI");
    glColorFragmentOp1ATI = (PFNGLCOLORFRAGMENTOP1ATIPROC)
        wglGetProcAddress("glColorFragmentOp1ATI");
    glColorFragmentOp2ATI = (PFNGLCOLORFRAGMENTOP2ATIPROC)
        wglGetProcAddress("glColorFragmentOp2ATI");
    glColorFragmentOp3ATI = (PFNGLCOLORFRAGMENTOP3ATIPROC)
        wglGetProcAddress("glColorFragmentOp3ATI");
    glAlphaFragmentOp1ATI = (PFNGLALPHAFRAGMENTOP1ATIPROC)
        wglGetProcAddress("glAlphaFragmentOp1ATI");
    glAlphaFragmentOp2ATI = (PFNGLALPHAFRAGMENTOP2ATIPROC)
        wglGetProcAddress("glAlphaFragmentOp2ATI");
    glAlphaFragmentOp3ATI = (PFNGLALPHAFRAGMENTOP3ATIPROC)
        wglGetProcAddress("glAlphaFragmentOp3ATI");
    glSetFragmentShaderConstantATI = (PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)
        wglGetProcAddress("glSetFragmentShaderConstantATI");
    glBeginVertexShaderEXT = (PFNGLBEGINVERTEXSHADEREXTPROC)
        wglGetProcAddress("glBeginVertexShaderEXT");
    glEndVertexShaderEXT = (PFNGLENDVERTEXSHADEREXTPROC)
        wglGetProcAddress("glEndVertexShaderEXT");
    glBindVertexShaderEXT = (PFNGLBINDVERTEXSHADEREXTPROC)
        wglGetProcAddress("glBindVertexShaderEXT");
    glGenVertexShadersEXT = (PFNGLGENVERTEXSHADERSEXTPROC)
        wglGetProcAddress("glGenVertexShadersEXT");
    glDeleteVertexShadersEXT = (PFNGLDELETEVERTEXSHADERSEXTPROC)
        wglGetProcAddress("glDeleteVertexShadersEXT");
    glShaderOp1EXT = (PFNGLSHADEROP1EXTPROC)
        wglGetProcAddress("glShaderOp1EXT");
    glShaderOp2EXT = (PFNGLSHADEROP2EXTPROC)
        wglGetProcAddress("glShaderOp2EXT");
    glShaderOp3EXT = (PFNGLSHADEROP3EXTPROC)
        wglGetProcAddress("glShaderOp3EXT");
    glSwizzleEXT = (PFNGLSWIZZLEEXTPROC)
        wglGetProcAddress("glSwizzleEXT");
    glWriteMaskEXT = (PFNGLWRITEMASKEXTPROC)
        wglGetProcAddress("glWriteMaskEXT");
    glInsertComponentEXT = (PFNGLINSERTCOMPONENTEXTPROC)
        wglGetProcAddress("glInsertComponentEXT");
    glExtractComponentEXT = (PFNGLEXTRACTCOMPONENTEXTPROC)
        wglGetProcAddress("glExtractComponentEXT");
    glGenSymbolsEXT = (PFNGLGENSYMBOLSEXTPROC)
        wglGetProcAddress("glGenSymbolsEXT");
    glSetInvariantEXT = (PFNGLSETINVARIANTEXTPROC)
        wglGetProcAddress("glSetInvariantEXT");
    glSetLocalConstantEXT = (PFNGLSETLOCALCONSTANTEXTPROC)
        wglGetProcAddress("glSetLocalConstantEXT");
    glVariantbvEXT = (PFNGLVARIANTBVEXTPROC)
        wglGetProcAddress("glVariantbvEXT");
    glVariantsvEXT = (PFNGLVARIANTSVEXTPROC)
        wglGetProcAddress("glVariantsvEXT");
    glVariantivEXT = (PFNGLVARIANTIVEXTPROC)
        wglGetProcAddress("glVariantivEXT");
    glVariantfvEXT = (PFNGLVARIANTFVEXTPROC)
        wglGetProcAddress("glVariantfvEXT");
    glVariantdvEXT = (PFNGLVARIANTDVEXTPROC)
        wglGetProcAddress("glVariantdvEXT");
    glVariantubEXT = (PFNGLVARIANTUBEXTPROC)
        wglGetProcAddress("glVariantubEXT");
    glVariantusEXT = (PFNGLVARIANTUSEXTPROC)
        wglGetProcAddress("glVariantusEXT");
    glVariantuiEXT = (PFNGLVARIANTUIEXTPROC)
        wglGetProcAddress("glVariantuiEXT");
    glVariantPointerEXT = (PFNGLVARIANTPOINTEREXTPROC)
        wglGetProcAddress("glVariantPointerEXT");
    glEnableVariantClientStateEXT = (PFNGLENABLEVARIANTCLIENTSTATEEXTPROC)
        wglGetProcAddress("glEnableVariantClientStateEXT");
    glDisableVariantClientStateEXT = (PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC)
        wglGetProcAddress("glDisableVariantClientStateEXT");
    glBindLightParameterEXT = (PFNGLBINDLIGHTPARAMETEREXTPROC)
        wglGetProcAddress("glBindLightParameterEXT");
    glBindMaterialParameterEXT = (PFNGLBINDMATERIALPARAMETEREXTPROC)
        wglGetProcAddress("glBindMaterialParameterEXT");
    glBindTexGenParameterEXT = (PFNGLBINDTEXGENPARAMETEREXTPROC)
        wglGetProcAddress("glBindTexGenParameterEXT");
    glBindTextureUnitParameterEXT = (PFNGLBINDTEXTUREUNITPARAMETEREXTPROC)
        wglGetProcAddress("glBindTextureUnitParameterEXT");
    glBindParameterEXT = (PFNGLBINDPARAMETEREXTPROC)
        wglGetProcAddress("glBindParameterEXT");
    glIsVariantEnabledEXT = (PFNGLISVARIANTENABLEDEXTPROC)
        wglGetProcAddress("glIsVariantEnabledEXT");
    glGetVariantBooleanvEXT = (PFNGLGETVARIANTBOOLEANVEXTPROC)
        wglGetProcAddress("glGetVariantBooleanvEXT");
    glGetVariantIntegervEXT = (PFNGLGETVARIANTINTEGERVEXTPROC)
        wglGetProcAddress("glGetVariantIntegervEXT");
    glGetVariantFloatvEXT = (PFNGLGETVARIANTFLOATVEXTPROC)
        wglGetProcAddress("glGetVariantFloatvEXT");
    glGetVariantPointervEXT = (PFNGLGETVARIANTPOINTERVEXTPROC)
        wglGetProcAddress("glGetVariantPointervEXT");
    glGetInvariantBooleanvEXT = (PFNGLGETINVARIANTBOOLEANVEXTPROC)
        wglGetProcAddress("glGetInvariantBooleanvEXT");
    glGetInvariantIntegervEXT = (PFNGLGETINVARIANTINTEGERVEXTPROC)
        wglGetProcAddress("glGetInvariantIntegervEXT");
    glGetInvariantFloatvEXT = (PFNGLGETINVARIANTFLOATVEXTPROC)
        wglGetProcAddress("glGetInvariantFloatvEXT");
    glGetLocalConstantBooleanvEXT = (PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC)
        wglGetProcAddress("glGetLocalConstantBooleanvEXT");
    glGetLocalConstantIntegervEXT = (PFNGLGETLOCALCONSTANTINTEGERVEXTPROC)
        wglGetProcAddress("glGetLocalConstantIntegervEXT");
    glGetLocalConstantFloatvEXT = (PFNGLGETLOCALCONSTANTFLOATVEXTPROC)
        wglGetProcAddress("glGetLocalConstantFloatvEXT");
    glActiveStencilFaceEXT = (PFNGLACTIVESTENCILFACEEXTPROC)
        wglGetProcAddress("glActiveStencilFaceEXT");
    wglCreateBufferRegionARB = (PFNWGLCREATEBUFFERREGIONARBPROC)
        wglGetProcAddress("wglCreateBufferRegionARB");
    wglDeleteBufferRegionARB = (PFNWGLDELETEBUFFERREGIONARBPROC)
        wglGetProcAddress("wglDeleteBufferRegionARB");
    wglSaveBufferRegionARB = (PFNWGLSAVEBUFFERREGIONARBPROC)
        wglGetProcAddress("wglSaveBufferRegionARB");
    wglRestoreBufferRegionARB = (PFNWGLRESTOREBUFFERREGIONARBPROC)
        wglGetProcAddress("wglRestoreBufferRegionARB");
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)
        wglGetProcAddress("wglSwapIntervalEXT");
    wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)
        wglGetProcAddress("wglGetSwapIntervalEXT");

    if ((existingextensions & NV_VERTEX_PROGRAMS_BIT) != 0)
    {
        const char* sRenderer = (const char*)glGetString(GL_RENDERER);
        if (strstr(sRenderer, "GeForce3") != 0)
            g_bVertexBufferObjectsWriteOnly = false;
    }
    if (AurAnisotropicFilterAvailable())
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
    InitAttenuationTexture();
    int bATIFragmentShaders;
    if ((existingextensions & ATI_FRAGMENT_SHADER_BIT) == ATI_FRAGMENT_SHADER_BIT)
    {
        pfuncEnablePixelShaders = EnableFragmentShaders;
        pfuncSetPixelShaderAmbientColor = SetFragmentShaderConstant0;
        pfuncSetBumpLightColor = SetFragmentShaderConstant1;
        bATIFragmentShaders = 1;
    }
    else
    {
        pfuncEnablePixelShaders = EnableCombiners;
        pfuncSetPixelShaderAmbientColor = SetCombinerConstant0;
        pfuncSetBumpLightColor = SetCombinerConstant1;
        bATIFragmentShaders = 0;
    }
    if (bATIFragmentShaders)
    {
        szBumpyShinyVPARB = new char[strlen(szBumpyShinySetupVPARB) +
            strlen(szBumpyShinyVPARB_ATI) + 1];
        szBumpyShinySkinVPARB = new char[strlen(szBumpyShinySkinSetupVPARB) +
            strlen(szBumpyShinyVPARB_ATI) + 1];
        strcpy(szBumpyShinyVPARB, szBumpyShinySetupVPARB);
        strcat(szBumpyShinyVPARB, szBumpyShinyVPARB_ATI);
        strcpy(szBumpyShinySkinVPARB, szBumpyShinySkinSetupVPARB);
        strcat(szBumpyShinySkinVPARB, szBumpyShinyVPARB_ATI);
        InitializeATIFragmentShaders();
    }
    else
    {
        szBumpyShinyVPARB = new char[strlen(szBumpyShinySetupVPARB) +
            strlen(szBumpyShinyVPARB_NV) + 1];
        szBumpyShinySkinVPARB = new char[strlen(szBumpyShinySkinSetupVPARB) +
            strlen(szBumpyShinyVPARB_NV) + 1];
        strcpy(szBumpyShinyVPARB, szBumpyShinySetupVPARB);
        strcat(szBumpyShinyVPARB, szBumpyShinyVPARB_NV);
        strcpy(szBumpyShinySkinVPARB, szBumpyShinySkinSetupVPARB);
        strcat(szBumpyShinySkinVPARB, szBumpyShinyVPARB_NV);
    }
    if (usearbvertexprograms)
        enableTranslationTable[ENABLE_VERTEX_PROGRAM] = GL_VERTEX_PROGRAM_ARB;
    if (AurVertexBufferObjectARBAvailable())
        InitializeVertexBufferObjectPoolManager();
    else
        InitializeSystemPoolManager();
    manRef->RestorePoolManager();
}
// PC 0x004486C0 GOG 0x00448810
void GLRender::FrameBufferModifications(unsigned long nFlags)
{
    static bool c_bPreviousFrameWasSpeed = false;
    glDisable(enableTranslationTable[ENABLE_FOG]);
    if (doframebuffer && cm_bPBuffersAllocated && cm_bSetUpFrameBuffer)
    {
        if (!AurFrameBufferModificationsAvailable())
        {
            if (AurATIFrameBufferModificationsAvailable())
                FrameBufferModificationsATI(nFlags);
        }
        else
        {
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(0.0, (double)g_nScreenWidth, 0.0,
                    (double)g_nScreenHeight, -1.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            glDisable(enableTranslationTable[ENABLE_ALPHA_TEST]);
            glDisable(enableTranslationTable[ENABLE_DEPTH_TEST]);
            glDisable(enableTranslationTable[ENABLE_LIGHTING]);
            glDepthMask(GL_FALSE);
            glDisable(enableTranslationTable[ENABLE_BLEND]);
            if (visualizepass6 && m_bSpeedBlurEnabled &&
                (nFlags & FRAMEBUFFER_SPEED_BLUR) && !g_ForceSight &&
                !cm_bFirstFrameBufferFrame && c_bPreviousFrameWasSpeed == true)
            {
                glEnable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                                   g_nScreenWidth, g_nScreenHeight);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
                glDisable(GL_TEXTURE_RECTANGLE_NV);
                glFlush();
                RenderAccumulationBlur(cm_nTargetTexture);
            }
            if (visualizepass6 && m_bSpeedBlurEnabled &&
                (nFlags & FRAMEBUFFER_SPEED_BLUR) && !g_ForceSight)
            {
                glActiveTextureARB(GL_TEXTURE0_ARB);
                glEnable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                                   g_nScreenWidth, g_nScreenHeight);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
                glDisable(GL_TEXTURE_RECTANGLE_NV);
                glFlush();
                SetActiveTexture(0);
                RenderFrameToTexture(cm_nTargetTexture);
                c_bPreviousFrameWasSpeed = true;
            }
            else
                c_bPreviousFrameWasSpeed = false;
            if (visualizepass1 && (nFlags & FRAMEBUFFER_OVERBRIGHTEN))
            {
                glEnable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                                   g_nScreenWidth, g_nScreenHeight);
                glDisable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
                glFlush();
                RenderOverlayOverbrighten(cm_nTargetTexture);
            }
            if (visualizepass2 && (nFlags & FRAMEBUFFER_CONVOLUTION))
            {
                glEnable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                                   g_nScreenWidth, g_nScreenHeight);
                glDisable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
                glFlush();
                RenderOverlayConvolution(cm_nTargetTexture);
            }
            if (visualizepass5 && (nFlags & FRAMEBUFFER_FORCE_DISTORTION))
            {
                glActiveTextureARB(GL_TEXTURE0_ARB);
                glEnable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                                   g_nScreenWidth, g_nScreenHeight);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
                glDisable(GL_TEXTURE_RECTANGLE_NV);
                RenderForceDistortion(cm_nTargetTexture);
            }
            if (visualizepass3 && m_bScanNoiseEnabled &&
                (nFlags & FRAMEBUFFER_NOISE) && cm_pFilmNoiseTexture &&
                !cm_pFilmNoiseTexture->IsNull())
            {
                glEnable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                                   g_nScreenWidth, g_nScreenHeight);
                glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                RenderOverlayFilmNoise(cm_nTargetTexture, true);
            }
            if (visualizepass7 && m_bFilmNoiseEnabled &&
                (nFlags & FRAMEBUFFER_NOISE) && cm_pFilmNoiseTexture &&
                !cm_pFilmNoiseTexture->IsNull())
            {
                glEnable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                                   g_nScreenWidth, g_nScreenHeight);
                glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                RenderOverlayFilmNoise(cm_nTargetTexture, false);
            }
            if (m_bDreamDistortEnabled)
            {
                glActiveTextureARB(GL_TEXTURE0_ARB);
                glEnable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                                   g_nScreenWidth, g_nScreenHeight);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
                glDisable(GL_TEXTURE_RECTANGLE_NV);
                glFlush();
                SetActiveTexture(0);
                RenderFrameToTexture(cm_nTargetTexture);
                RenderDreamDistortion(cm_nTargetTexture);
            }
            if (m_FuryLevel != 0)
            {
                glActiveTextureARB(GL_TEXTURE0_ARB);
                glEnable(GL_TEXTURE_RECTANGLE_NV);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                                   g_nScreenWidth, g_nScreenHeight);
                glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
                glDisable(GL_TEXTURE_RECTANGLE_NV);
                glFlush();
                SetActiveTexture(0);
                RenderFrameToTexture(cm_nTargetTexture);
                RenderFury(cm_nTargetTexture);
            }
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(enableTranslationTable[ENABLE_ALPHA_TEST]);
            glEnable(enableTranslationTable[ENABLE_DEPTH_TEST]);
            glEnable(enableTranslationTable[ENABLE_LIGHTING]);
            glDepthMask(GL_TRUE);
            cm_bFirstFrameBufferFrame = false;
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
        }
    }
}
// PC 0x00446C30 GOG 0x00446D80
void GLRender::FrameBufferModificationsATI(unsigned long nFlags)
{
    static bool c_bPreviousFrameWasSpeed = false;
    glDisable(enableTranslationTable[ENABLE_ALPHA_TEST]);
    glDisable(enableTranslationTable[ENABLE_DEPTH_TEST]);
    glDisable(enableTranslationTable[ENABLE_LIGHTING]);
    glDepthMask(GL_FALSE);
    glDisable(enableTranslationTable[ENABLE_BLEND]);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    if (visualizepass6 && m_bSpeedBlurEnabled &&
        (nFlags & FRAMEBUFFER_SPEED_BLUR) && !g_ForceSight &&
        !cm_bFirstFrameBufferFrame && c_bPreviousFrameWasSpeed == true)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                           cm_nScreenWidthPow2, cm_nScreenHeightPow2);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFlush();
        RenderAccumulationBlurATI(cm_nTargetTexture);
    }
    if (visualizepass6 && m_bSpeedBlurEnabled &&
        (nFlags & FRAMEBUFFER_SPEED_BLUR) && !g_ForceSight)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                           cm_nScreenWidthPow2, cm_nScreenHeightPow2);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFlush();
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        SetActiveTexture(0);
        RenderFrameToTextureATI(cm_nTargetTexture);
        c_bPreviousFrameWasSpeed = true;
    }
    else
        c_bPreviousFrameWasSpeed = false;
    if (visualizepass1 && (nFlags & FRAMEBUFFER_OVERBRIGHTEN))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                           cm_nScreenWidthPow2, cm_nScreenHeightPow2);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFlush();
        RenderOverlayOverbrightenATI(cm_nTargetTexture);
    }
    if (visualizepass2 && (nFlags & FRAMEBUFFER_CONVOLUTION))
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                           cm_nScreenWidthPow2, cm_nScreenHeightPow2);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFlush();
        RenderOverlayConvolutionATI(cm_nTargetTexture);
    }
    if (visualizepass5 && (nFlags & FRAMEBUFFER_FORCE_DISTORTION))
    {
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                           cm_nScreenWidthPow2, cm_nScreenHeightPow2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glFlush();
        RenderForceDistortionATI(cm_nTargetTexture);
    }
    if (visualizepass3 && m_bScanNoiseEnabled &&
        (nFlags & FRAMEBUFFER_NOISE) && cm_pFilmNoiseTexture &&
        !cm_pFilmNoiseTexture->IsNull())
    {
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                           cm_nScreenWidthPow2, cm_nScreenHeightPow2);
        glFlush();
        RenderOverlayFilmNoiseATI(cm_nTargetTexture, true);
    }
    if (visualizepass7 && m_bFilmNoiseEnabled &&
        (nFlags & FRAMEBUFFER_NOISE) && cm_pFilmNoiseTexture &&
        !cm_pFilmNoiseTexture->IsNull())
    {
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                           cm_nScreenWidthPow2, cm_nScreenHeightPow2);
        glFlush();
        RenderOverlayFilmNoiseATI(cm_nTargetTexture, false);
    }
    if (m_bDreamDistortEnabled)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                           cm_nScreenWidthPow2, cm_nScreenHeightPow2);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFlush();
        SetActiveTexture(0);
        RenderFrameToTextureATI(cm_nTargetTexture);
        RenderDreamDistortionATI(cm_nTargetTexture);
    }
    if (m_FuryLevel != 0)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
                           cm_nScreenWidthPow2, cm_nScreenHeightPow2);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFlush();
        SetActiveTexture(0);
        RenderFrameToTextureATI(cm_nTargetTexture);
        RenderFuryATI(cm_nTargetTexture);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(enableTranslationTable[ENABLE_ALPHA_TEST]);
    glEnable(enableTranslationTable[ENABLE_DEPTH_TEST]);
    glEnable(enableTranslationTable[ENABLE_LIGHTING]);
    glDepthMask(GL_TRUE);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    cm_bFirstFrameBufferFrame = false;
}
// PC 0x004471F0 GOG 0x00447340
void GLRender::FrameBufferModificationsATI2(unsigned long nFlags)
{
    if (visualizepass4 && m_bSaturationEnabled &&
        (nFlags & FRAMEBUFFER_SATURATION))
    {
        glDisable(enableTranslationTable[ENABLE_DEPTH_TEST]);
        glDisable(enableTranslationTable[ENABLE_LIGHTING]);
        glDepthMask(GL_FALSE);
        glEnable(enableTranslationTable[ENABLE_BLEND]);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0, (double)g_nScreenWidth, 0.0,
                (double)g_nScreenHeight, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        if (visualizepass4 && m_bSaturationEnabled)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, cm_nScreenWidthPow2, cm_nScreenHeightPow2);
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            RenderSaturationAdjustmentATI(cm_nTargetTexture);
        }
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(enableTranslationTable[ENABLE_DEPTH_TEST]);
        glEnable(enableTranslationTable[ENABLE_ALPHA_TEST]);
        glEnable(enableTranslationTable[ENABLE_LIGHTING]);
        glDepthMask(GL_TRUE);
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
}
// PC 0x00447370 GOG 0x004474C0
void GLRender::FrameBufferModifications2(unsigned long nFlags)
{
    if (doframebuffer && cm_bPBuffersAllocated && cm_bSetUpFrameBuffer)
    {
        if (!AurFrameBufferModificationsAvailable())
        {
            if (AurATIFrameBufferModificationsAvailable())
            {
                FrameBufferModificationsATI2(nFlags);
            }
        }
        else
        {
            if (visualizepass4 && m_bSaturationEnabled &&
                (nFlags & FRAMEBUFFER_SATURATION))
            {
                glDisable(enableTranslationTable[ENABLE_DEPTH_TEST]);
                glDisable(enableTranslationTable[ENABLE_LIGHTING]);
                glDepthMask(GL_FALSE);
                glEnable(enableTranslationTable[ENABLE_BLEND]);
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                glOrtho(0.0, (double)g_nScreenWidth, 0.0,
                        (double)g_nScreenHeight, -1.0, 1.0);
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();
                if (visualizepass4 && m_bSaturationEnabled)
                {
                    glEnable(GL_TEXTURE_RECTANGLE_NV);
                    glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
                    glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,
                                       g_nScreenWidth, g_nScreenHeight);
                    RenderSaturationAdjustment(cm_nTargetTexture);
                }
                SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                glEnable(enableTranslationTable[ENABLE_DEPTH_TEST]);
                glEnable(enableTranslationTable[ENABLE_ALPHA_TEST]);
                glEnable(enableTranslationTable[ENABLE_LIGHTING]);
                glDepthMask(GL_TRUE);
                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
            }
        }
    }
}
// PC 0x00431DF0 GOG 0x00431F40
void* RequestPoolManagerVBO::LockPool(int nPool, int nUsage)
{
    if (nPool == INVALID_POOL)
    {
        return 0;
    }
    if (m_lstPoolLocks.m_pList[nPool] != 0)
    {
        return 0;
    }

    glBindBufferARB(m_lstPoolTypes.m_pList[nPool], m_lstPools.m_pList[nPool]);
    m_lstPoolUsage.m_pList[nPool] = nUsage;
    m_lstPoolLocks.m_pList[nPool] = 1;

    return glMapBufferARB(m_lstPoolTypes.m_pList[nPool],
                          (nUsage & POOL_LOCK_READ_ONLY) ? GL_READ_ONLY_ARB :
                          (nUsage & POOL_LOCK_READ_WRITE) ? GL_READ_WRITE_ARB :
                          g_bVertexBufferObjectsWriteOnly ? GL_WRITE_ONLY_ARB :
                                                          GL_READ_WRITE_ARB);
}
// PC 0x00431E90 GOG 0x00431FE0
void RequestPoolManagerVBO::UnlockPool(unsigned int nPool)
{
    if (nPool != (unsigned int)-1 && m_lstPoolLocks.m_pList[nPool] != 0)
    {
        m_lstPoolLocks.m_pList[nPool] = 0;
        glBindBufferARB(m_lstPoolTypes.m_pList[nPool], m_lstPools.m_pList[nPool]);
        glUnmapBufferARB(m_lstPoolTypes.m_pList[nPool]);
    }
}
// PC 0x00431EE0 GOG 0x00432030
int RequestPoolManagerVBO::GetPool(unsigned int nPool)
{
    if (nPool == (unsigned int)-1)
    {
        return 0;
    }

    return m_lstPools.m_pList[nPool];
}
// PC 0x00431F00 GOG 0x00432050
int RequestPoolManagerVBO::GetIndexPool(unsigned int nPool)
{
    if (nPool != (unsigned int)-1)
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_lstPools.m_pList[nPool]);
    }

    return 0;
}
// PC 0x00431F20 GOG 0x00432070
int RequestPoolManagerVBO::GetPoolSize(unsigned int nPool)
{
    int nIndex = nPool;
    if (nIndex == -1)
    {
        return 0;
    }

    glBindBufferARB(m_lstPoolTypes.m_pList[nIndex], m_lstPools.m_pList[nIndex]);
    int nSize;
    glGetBufferParameterivARB(m_lstPoolTypes.m_pList[nIndex], GL_BUFFER_SIZE_ARB, &nSize);
    return nSize;
}
// PC 0x00431F70 GOG 0x004320C0
void RequestPoolManagerVBO::ResizePool(unsigned int nPool, unsigned int nSize)
{
    if (nPool == INVALID_POOL)
    {
        return;
    }

    glBindBufferARB(m_lstPoolTypes.m_pList[nPool], m_lstPools.m_pList[nPool]);
    int nOldSize;
    glGetBufferParameterivARB(m_lstPoolTypes.m_pList[nPool], GL_BUFFER_SIZE_ARB,
                              &nOldSize);
    char* pBackup = new char[nOldSize];
    void* pData = glMapBufferARB(m_lstPoolTypes.m_pList[nPool], GL_READ_ONLY_ARB);
    memcpy(pBackup, pData, nOldSize);
    glUnmapBufferARB(m_lstPoolTypes.m_pList[nPool]);

    unsigned int nBuffer = m_lstPools.m_pList[nPool];
    glDeleteBuffersARB(1, &nBuffer);
    m_lstPools.m_pList[nPool] = 0;
    glGenBuffersARB(1, (unsigned int*)&m_lstPools.m_pList[nPool]);
    glBindBufferARB(m_lstPoolTypes.m_pList[nPool], m_lstPools.m_pList[nPool]);
    glBufferDataARB(m_lstPoolTypes.m_pList[nPool], nSize, 0, GL_DYNAMIC_DRAW_ARB);
    glBufferSubDataARB(m_lstPoolTypes.m_pList[nPool], 0, nOldSize, pBackup);
    delete[] pBackup;
}
// PC 0x00432090 GOG 0x004321E0
RequestPoolManagerSys::~RequestPoolManagerSys()
{
    for (unsigned int nPool = 0; nPool < m_nPoolCount; nPool++)
    {
        delete[] m_ppPools[nPool];
    }
    delete[] m_ppPools;
    delete[] m_pPoolSize;
}
// PC 0x00432100 GOG 0x00432250
void* RequestPoolManagerSys::LockPool(int nPool, int nUsage)
{
    if (nPool == -1)
    {
        return 0;
    }

    int nLock = m_lstPoolLocks.m_pList[nPool];
    int* pLock = &m_lstPoolLocks.m_pList[nPool];
    if (nLock != 0)
    {
        return 0;
    }

    *pLock = 1;
    return m_ppPools[nPool];
}
// PC 0x00432130 GOG 0x00432280
void RequestPoolManagerSys::UnlockPool(unsigned int nPool)
{
    if (nPool != (unsigned int)-1)
    {
    int* pLock = &m_lstPoolLocks.m_pList[nPool];
        if (*pLock != 0)
        {
            *pLock = 0;
        }
    }
}
// PC 0x00432150 GOG 0x004322A0
void GLRender::DrawTriangles(List<MaxFace>& lFaces, RenderVertexV* pVertices,
                              RenderVertexC* pColors)
{
    glBegin(GL_TRIANGLES);

    Vector vFirst, vSecond, vThird;
    for (int i = 0; i < lFaces.m_nCount; ++i)
    {
        glColor4ubv(pColors[lFaces.m_pList[i].m_nSurfaceType].color);
        const Vector& vFirstPosition = pVertices[lFaces.m_pList[i].m_anVertexIndices[0]].position;
        vFirst = vFirstPosition;
        const Vector& vSecondPosition = pVertices[lFaces.m_pList[i].m_anVertexIndices[1]].position;
        vSecond = vSecondPosition;
        const Vector& vThirdPosition = pVertices[lFaces.m_pList[i].m_anVertexIndices[2]].position;
        vThird = vThirdPosition;
        glNormal3fv((const GLfloat*)&NormalOf(vFirst, vSecond, vThird));
        glVertex3fv((const GLfloat*)&vFirst);
        glVertex3fv((const GLfloat*)&vSecond);
        glVertex3fv((const GLfloat*)&vThird);
    }

    glEnd();
}
// PC 0x004322B0 GOG 0x00432400
void GLRender::DrawSphereBound(float fRadius, Vector& vCenter,
                               const Vector& vColor, float fScale,
                               bool bBillboard)
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    if (bBillboard)
    {
        float fMatrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, fMatrix);
        fMatrix[12] = vCenter.m_fX;
        fMatrix[13] = vCenter.m_fY;
        fMatrix[14] = vCenter.m_fZ;
        float fXY = fMatrix[1];
        fMatrix[1] = fMatrix[4];
        fMatrix[4] = fXY;
        float fXZ = fMatrix[2];
        fMatrix[2] = fMatrix[8];
        fMatrix[8] = fXZ;
        float fYZ = fMatrix[6];
        fMatrix[6] = fMatrix[9];
        fMatrix[9] = fYZ;
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glMultMatrixf(fMatrix);
    }
    glColor3f(vColor.m_fX, vColor.m_fY, vColor.m_fZ);
    fRadius *= fScale;
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.0f, fRadius, 0.0f);
    float fDiagonal = fRadius * AUR_SPHERE_BOUND_DIAGONAL_FACTOR;
    glVertex3f(fDiagonal, fDiagonal, 0.0f);
    glVertex3f(fRadius, 0.0f, 0.0f);
    float fNegativeDiagonal = fRadius * -AUR_SPHERE_BOUND_DIAGONAL_FACTOR;
    glVertex3f(fDiagonal, fNegativeDiagonal, 0.0f);
    fRadius = -fRadius;
    glVertex3f(0.0f, fRadius, 0.0f);
    glVertex3f(fNegativeDiagonal, fNegativeDiagonal, 0.0f);
    glVertex3f(fRadius, 0.0f, 0.0f);
    glVertex3f(fNegativeDiagonal, fDiagonal, 0.0f);
    glEnd();
    if (bBillboard)
        glPopMatrix();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}
// PC 0x00432410 GOG 0x00432560
void GLRender::RenderObjectsVTCQuad(unsigned long nPool, int nVertices)
{
    if (AurVertexBufferObjectARBAvailable())
    {
        int nBuffer = manRef->GetPool(nPool);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, nBuffer);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(RenderVertexVTC), (const GLvoid*)12);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RenderVertexVTC), (const GLvoid*)16);
        glVertexPointer(3, GL_FLOAT, sizeof(RenderVertexVTC), 0);
        glDrawArrays(GL_QUADS, 0, nVertices);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    }
    else
    {
        glBegin(GL_QUADS);
        RenderVertexVTC* pVertices =
            (RenderVertexVTC*)manRef->GetPool(nPool);
        for (int i = 0; i < nVertices; ++i)
        {
            glColor4ubv(pVertices[i].color);
            glTexCoord2f(pVertices[i].u, pVertices[i].v);
            glVertex3fv(pVertices[i].position);
        }
        glEnd();
    }
}
// PC 0x00432510 GOG 0x00432660
int ImposterManager::AllocateImposterRecord(int nVertices, float* pVertices)
{
    int nRecord;
    for (nRecord = 0; nRecord < m_nImposters; ++nRecord)
    {
        if (!m_pImposters[nRecord].m_bAllocated)
        {
            AllocateImposter(nRecord, nVertices, pVertices);
            return nRecord;
        }
    }

    Record* pOldImposters = m_pImposters;
    m_pImposters = new Record[m_nImposters * 2];
    memset(m_pImposters, 0, m_nImposters * 2);
    memcpy(m_pImposters, pOldImposters, m_nImposters * sizeof(Record));
    delete[] pOldImposters;
    AllocateImposter(nRecord, nVertices, pVertices);
    m_nImposters *= 2;
    return m_nImposters >> 1;
}
// PC 0x004325E0 GOG 0x00432730
int GLRender::SetUpImposters(int nType, float* pfValues)
{
    return manRefImposter.AllocateImposterRecord(nType, pfValues);
}
// PC 0x00432600 GOG 0x00432750
void GLRender::RenderDrawableSquares()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    for (int i = 0; i < cm_listSquaresToRender.m_nCount; ++i)
    {
        RenderSquares(cm_listSquaresToRender.m_pList[i]);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}
// PC 0x00432660 GOG 0x004327B0
void GLRender::RenderOverlayFilmNoiseATI(unsigned long nTexture, bool bScanLines)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glBlendFunc(GL_ONE, GL_ZERO);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLRender::cm_uiCurrentStage = 1;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(GLRender::cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    cm_pFilmNoiseTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glEnable(GL_FRAGMENT_SHADER_ATI);
    glBindFragmentShaderATI(cm_nFilmNoiseShaderATI);
    float fMaxS = (float)g_nScreenWidth / (float)cm_nScreenWidthPow2;
    float fMaxT = (float)g_nScreenHeight / (float)cm_nScreenHeightPow2;
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        float t = (i == 0 || i == 3) ? fMaxT : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : fMaxS;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        float fNoiseT = (i == 0 || i == 3) ? 0.0f : 7.5f;
        float fNoiseS = (i == 0 || i == 1) ? (bScanLines ? 0.0f : 7.5f) : 0.0f;
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, fNoiseS, fNoiseT);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    GLRender::cm_uiCurrentStage = 0;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(GLRender::cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
// PC 0x004329B0 GOG 0x00432B00
void GLRender::RenderOverlayFilmNoise(unsigned long nTexture, bool bScanLines)
{
    glBlendFunc(GL_ONE, GL_ZERO);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, nTexture);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLRender::cm_uiCurrentStage = 1;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(GLRender::cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    cm_pFilmNoiseTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_FILM_NOISE, 0, 0, 0,
                                        0);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        float t = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        float fNoiseT = (i == 0 || i == 3) ? 0.0f : 7.5f;
        float fNoiseS = (i == 0 || i == 1) ? (bScanLines ? 0.0f : 7.5f) : 0.0f;
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, fNoiseS, fNoiseT);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    GLRender::cm_uiCurrentStage = 0;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(GLRender::cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glDisable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
    glFlush();
}
// PC 0x00432C50 GOG 0x00432DA0
void GLRender::RenderForceDistortionATI(unsigned long nTexture)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0, (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, g_nScreenWidth, g_nScreenHeight);
    glEnable(GL_VERTEX_PROGRAM_ARB);
    glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, DISTORTION_ENV_PARAMETER,
                               0.0f, 1.0f, 0.0f, 0.0f);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, cm_pHeatDistortionVertexProg);
    cm_pForceDistortionTexture->GetWidth();
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    cm_uiCurrentStage = 0;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    cm_pForceDistortionTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_FRAGMENT_SHADER_ATI);
    glBindFragmentShaderATI(cm_nForceDistortionShaderATI);
    glEnable(enableTranslationTable[ENABLE_STENCIL_TEST]);
    glStencilFunc(GL_LESS, 0, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    float fMaxS = (float)g_nScreenWidth / (float)cm_nScreenWidthPow2;
    float fMaxT = (float)g_nScreenHeight / (float)cm_nScreenHeightPow2;
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        float t = (i == 0 || i == 3) ? 0.0f : 1.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, fMaxS / 10.0f, fMaxT / 10.0f);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        float fScreenT = (i == 0 || i == 3) ? fMaxT : 0.0f;
        float fScreenS = (i == 0 || i == 1) ? 0.0f : fMaxS;
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB, fScreenS, fScreenT);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight + 25.0f : -25.0f;
        float x = (i == 0 || i == 1) ? -25.0f : (float)g_nScreenWidth + 25.0f;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glDisable(enableTranslationTable[ENABLE_STENCIL_TEST]);
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glDisable(GL_VERTEX_PROGRAM_ARB);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, 0);
    cm_uiCurrentStage = 0;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glFlush();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
// PC 0x004330C0 GOG 0x00433210
void GLRender::RenderForceDistortion(unsigned long nTexture)
{
    glEnable(GL_VERTEX_PROGRAM_ARB);
    glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, DISTORTION_ENV_PARAMETER,
                               0.0f, 1.0f, 0.0f, 0.0f);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, cm_pHeatDistortionVertexProg);
    cm_pForceDistortionTexture->GetWidth();
    glBlendFunc(GL_ONE, GL_ZERO);
    cm_uiCurrentStage = 0;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    cm_pForceDistortionTexture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    cm_uiCurrentStage = 1;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    cm_uiCurrentStage = 2;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, nTexture);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_FORCE_DISTORTION,
                                        0, 0, 0, 0);
    glEnable(enableTranslationTable[ENABLE_STENCIL_TEST]);
    glStencilFunc(GL_LESS, 0, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        float t = (i == 0 || i == 3) ? 0.0f : 1.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, (float)g_nScreenWidth / 10.0f,
                             (float)g_nScreenHeight / 10.0f);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        float fScreenT = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float fScreenS = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB, fScreenS, fScreenT);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight + 25.0f : -25.0f;
        float x = (i == 0 || i == 1) ? -25.0f : (float)g_nScreenWidth;
        glVertex3f(x + 25.0f, y, 0.0f);
    }
    glEnd();
    glDisable(enableTranslationTable[ENABLE_STENCIL_TEST]);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glDisable(GL_VERTEX_PROGRAM_ARB);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, 0);
    cm_uiCurrentStage = 0;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glFlush();
}
// PC 0x00433470 GOG 0x004335C0
void GLRender::RenderFrameToTextureATI(unsigned long nTexture)
{
    HDC hOldDC = m_hDC;
    HGLRC hOldContext = m_hGLRC;
    wglMakeContextCurrentARB(cm_hAccumulationHDC, cm_hAccumulationHDC,
                             cm_hAccumulationHGLRC);
    glDrawBuffer(GL_FRONT);
    glViewport(0, 0, 128, 128);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 128.0, 0.0, 128.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_COPY, 0, 0, 0, 0);
    glDisable(GL_DEPTH_TEST);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    glBlendFunc(GL_ONE, GL_ZERO);
    float fMaxS = (float)g_nScreenWidth / (float)cm_nScreenWidthPow2;
    float fMaxT = (float)g_nScreenHeight / (float)cm_nScreenHeightPow2;
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? fMaxT : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : fMaxS;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? 128.0f : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : 128.0f;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glDisable(GL_FRAGMENT_SHADER_ATI);
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glFlush();
    wglMakeContextCurrentARB(hOldDC, hOldDC, hOldContext);
    glDrawBuffer(GL_BACK);
}
// PC 0x00433690 GOG 0x004337E0
void GLRender::RenderFrameToTexture(unsigned long nTexture)
{
    wglMakeCurrent(cm_hAccumulationHDC, cm_hAccumulationHGLRC);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 128.0, 0.0, 128.0, -1.0, 1.0);
    glViewport(0, 0, 128, 128);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, nTexture);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_COPY, 0, 0, 0, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? 128.0f : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : 128.0f;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glFlush();
    wglMakeCurrent(m_hDC, m_hGLRC);
}
// PC 0x00433860 GOG 0x004339B0
void GLRender::RenderAccumulationBlurATI(unsigned long nTexture)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, g_nScreenWidth, 0.0, g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, g_nScreenWidth, g_nScreenHeight);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ZERO);
    Vector vAccumulation(m_fAccumulationRatio, m_fAccumulationRatio,
                         m_fAccumulationRatio);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nAccumulationBlur);
    wglBindTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nTargetTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_FRAGMENT_SHADER_ATI);
    glSetFragmentShaderConstantATI(GL_CON_0_ATI,
                                   Homogenize(vAccumulation, 1.0f));
    glSetFragmentShaderConstantATI(GL_CON_1_ATI,
        Homogenize(Vector(1.0f, 1.0f, 1.0f) - vAccumulation, 0.0f));
    glEnable(GL_FRAGMENT_SHADER_ATI);
    glBindFragmentShaderATI(cm_nAccumulationBlurShaderATI);
    glDisable(GL_LIGHTING);
    float fMaxS = (float)g_nScreenWidth / (float)cm_nScreenWidthPow2;
    float fMaxT = (float)g_nScreenHeight / (float)cm_nScreenHeightPow2;
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        float t = (i == 0 || i == 3) ? 1.0f : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        float fScreenT = (i == 0 || i == 3) ? fMaxT : 0.0f;
        float fScreenS = (i == 0 || i == 1) ? 0.0f : fMaxS;
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, fScreenS, fScreenT);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    glActiveTextureARB(GL_TEXTURE0_ARB);
    wglReleaseTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    GLRender::cm_uiCurrentStage = 0;
    if (AurMultiTextureAvailable())
    {
        glActiveTextureARB(GLRender::cm_uiCurrentStage + GL_TEXTURE0_ARB);
    }
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_FRAGMENT_SHADER_ATI);
    glFlush();
}
// PC 0x00433C90 GOG 0x00433DE0
void GLRender::RenderAccumulationBlur(unsigned long nTexture)
{
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ZERO);
    Vector vAccumulation(m_fAccumulationRatio, m_fAccumulationRatio,
                         m_fAccumulationRatio);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cm_nAccumulationBlur);
    wglBindTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, cm_nTargetTexture);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_ACCUMULATION_BLUR,
                                        0, 0, 0, 0);
    glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR0_NV,
                                 Homogenize(vAccumulation, 1.0f));
    glCombinerStageParameterfvNV(GL_COMBINER1_NV, GL_CONSTANT_COLOR1_NV,
        Homogenize(Vector(1.0f, 1.0f, 1.0f) - vAccumulation, 0.0f));
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        float t = (i == 0 || i == 3) ? 1.0f : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
        glActiveTextureARB(GL_TEXTURE1_ARB);
        float fScreenT = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float fScreenS = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, fScreenS, fScreenT);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, 0);
    wglReleaseTexImageARB(cm_hAccumulationPBuffer, WGL_FRONT_LEFT_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
    glActiveTextureARB(GL_TEXTURE0_ARB);
}
// PC 0x00433FE0 GOG 0x00434130
void GLRender::RenderOverlayOverbrighten(unsigned long nTexture)
{
    glBlendFunc(GL_ONE, GL_ZERO);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_OVERBRIGHTEN,
                                        nTexture, cm_nOverbrightenRed,
                                        cm_nOverbrightenGreen,
                                        cm_nOverbrightenBlue);
    glBegin(GL_QUADS);
    for (unsigned int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex4f(x, y, 0.0f, 1.0f);
    }
    glEnd();
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
}
// PC 0x00434130 GOG 0x00434280
void GLRender::RenderOverlayOverbrightenATI(unsigned long nTexture)
{
    glBlendFunc(GL_ONE, GL_ZERO);
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_OVERBRIGHTEN,
                                           nTexture, cm_nOverbrightenRed,
                                           cm_nOverbrightenGreen,
                                           cm_nOverbrightenBlue);
    float fMaxS = (float)g_nScreenWidth / (float)cm_nScreenWidthPow2;
    float fMaxT = (float)g_nScreenHeight / (float)cm_nScreenHeightPow2;
    glBegin(GL_QUADS);
    for (unsigned int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? fMaxT : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : fMaxS;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex4f(x, y, 0.0f, 1.0f);
    }
    glEnd();
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}
// PC 0x004342C0 GOG 0x00434410
void GLRender::RenderSaturationAdjustmentATI(unsigned long nTexture)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)g_nScreenWidth, 0.0,
            (double)g_nScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, nTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_SATURATION,
                                        0, 0, 0, 0);
    float fDesaturation = 1.0f - m_fSaturation;
    Vector vRed;
    Vector vGreen;
    Vector vBlue;
    vRed.m_fX = m_fSaturation + 0.3086f * fDesaturation;
    vGreen.m_fX = 0.3086f * fDesaturation;
    vBlue.m_fX = 0.3086f * fDesaturation;
    vRed.m_fY = 0.6094f * fDesaturation;
    vGreen.m_fY = m_fSaturation + 0.6094f * fDesaturation;
    vBlue.m_fY = 0.6094f * fDesaturation;
    vRed.m_fZ = 0.0820f * fDesaturation;
    vGreen.m_fZ = 0.0820f * fDesaturation;
    vBlue.m_fZ = 0.0820f * fDesaturation + m_fSaturation;
    vRed = vRed * m_vSaturationMultiplier.m_fX;
    vGreen = vGreen * m_vSaturationMultiplier.m_fY;
    vBlue = vBlue * m_vSaturationMultiplier.m_fZ;
    if (vRed.m_fX > 1.0f) vRed.m_fX = 1.0f;
    if (vRed.m_fY > 1.0f) vRed.m_fY = 1.0f;
    if (vRed.m_fZ > 1.0f) vRed.m_fZ = 1.0f;
    if (vGreen.m_fX > 1.0f) vGreen.m_fX = 1.0f;
    if (vGreen.m_fY > 1.0f) vGreen.m_fY = 1.0f;
    if (vGreen.m_fZ > 1.0f) vGreen.m_fZ = 1.0f;
    if (vBlue.m_fX > 1.0f) vBlue.m_fX = 1.0f;
    if (vBlue.m_fY > 1.0f) vBlue.m_fY = 1.0f;
    if (vBlue.m_fX > 1.0f) vBlue.m_fZ = 1.0f;
    glSetFragmentShaderConstantATI(GL_CON_0_ATI,
                                 Homogenize(vRed, 0.0f));
    glSetFragmentShaderConstantATI(GL_CON_1_ATI,
                                 Homogenize(vGreen, 0.0f));
    glSetFragmentShaderConstantATI(GL_CON_2_ATI,
                                 Homogenize(vBlue, 0.0f));
    glBegin(GL_QUADS);
    float fMaxS = (float)g_nScreenWidth / (float)cm_nScreenWidthPow2;
    float fMaxT = (float)g_nScreenHeight / (float)cm_nScreenHeightPow2;
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? fMaxT : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : fMaxS;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    FrameBufferModificationPixelShadersATI(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
// PC 0x00434780 GOG 0x004348D0
void GLRender::RenderSaturationAdjustment(unsigned long nTexture)
{
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, nTexture);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_SATURATION,
                                        0, 0, 0, 0);
    float fDesaturation = 1.0f - m_fSaturation;
    Vector vRed;
    Vector vGreen;
    Vector vBlue;
    vRed.m_fX = m_fSaturation + 0.3086f * fDesaturation;
    vGreen.m_fX = 0.3086f * fDesaturation;
    vBlue.m_fX = 0.3086f * fDesaturation;
    vRed.m_fY = 0.6094f * fDesaturation;
    vGreen.m_fY = m_fSaturation + 0.6094f * fDesaturation;
    vBlue.m_fY = 0.6094f * fDesaturation;
    vRed.m_fZ = 0.0820f * fDesaturation;
    vGreen.m_fZ = 0.0820f * fDesaturation;
    vBlue.m_fZ = 0.0820f * fDesaturation + m_fSaturation;
    vRed = vRed * m_vSaturationMultiplier.m_fX;
    vGreen = vGreen * m_vSaturationMultiplier.m_fY;
    vBlue = vBlue * m_vSaturationMultiplier.m_fZ;
    if (vRed.m_fX > 1.0f) vRed.m_fX = 1.0f;
    if (vRed.m_fY > 1.0f) vRed.m_fY = 1.0f;
    if (vRed.m_fZ > 1.0f) vRed.m_fZ = 1.0f;
    if (vGreen.m_fX > 1.0f) vGreen.m_fX = 1.0f;
    if (vGreen.m_fY > 1.0f) vGreen.m_fY = 1.0f;
    if (vGreen.m_fZ > 1.0f) vGreen.m_fZ = 1.0f;
    if (vBlue.m_fX > 1.0f) vBlue.m_fX = 1.0f;
    if (vBlue.m_fY > 1.0f) vBlue.m_fY = 1.0f;
    if (vBlue.m_fX > 1.0f) vBlue.m_fZ = 1.0f;
    glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR0_NV,
                                 Homogenize(vRed, 0.0f));
    glCombinerStageParameterfvNV(GL_COMBINER0_NV, GL_CONSTANT_COLOR1_NV,
                                 Homogenize(vGreen, 0.0f));
    glCombinerStageParameterfvNV(GL_COMBINER2_NV, GL_CONSTANT_COLOR0_NV,
                                 Homogenize(vBlue, 0.0f));
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float t = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glTexCoord2f(s, t);
        float y = (i == 0 || i == 3) ? (float)g_nScreenHeight : 0.0f;
        float x = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
    FrameBufferModificationPixelShaders(FRAMEBUFFER_SHADER_DISABLE, 0, 0, 0, 0);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
}
// PC 0x006049D0 GOG 0x006049B0
void RequestPoolManagerVBO::RefreshPool(int nPool, int nUsage)
{
}

GLuint fontOffset;

const unsigned int nvMatrixTranslationTable[] =
{
    GL_MODELVIEW_PROJECTION_NV, GL_MODELVIEW, GL_PROJECTION
};

const unsigned int nvMatrixTransformTranslationTable[] =
{
    GL_IDENTITY_NV, GL_TRANSPOSE_NV, GL_INVERSE_NV, GL_INVERSE_TRANSPOSE_NV
};

const float combinationModeTranslationTable[] = { GL_REPLACE, GL_MODULATE };

const unsigned int texParamTargetTranslationTable[] =
{
    GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
};

const unsigned int fogModeTranslationTable[] = { GL_LINEAR, GL_EXP, GL_EXP2 };

const unsigned int queryTranslationTable[] = { GL_FOG, GL_MAX_GENERAL_COMBINERS_NV };

const unsigned int blendTranslationTable[] =
{
    GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO,
    GL_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA
};

const unsigned int transformTranslationTable[] =
{
    GL_PROJECTION, GL_MODELVIEW, GL_MODELVIEW, GL_TEXTURE
};

const unsigned int cmpTranslationTable[] =
{
    GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL,
    GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS
};

const unsigned int cullTranslationTable[] = { GL_CW, GL_CCW };

const unsigned int stencilOpTranslationTable[] =
{
    GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_DECR, GL_INVERT
};

const unsigned int fillTranslationTable[] = { GL_POINT, GL_LINE, GL_FILL };

const unsigned int facingTranslationTable[] = { GL_FRONT, GL_BACK, GL_FRONT_AND_BACK };

const unsigned int materialTranslationTable[] =
{
    GL_SHININESS, GL_AMBIENT, GL_DIFFUSE, GL_EMISSION, GL_SPECULAR
};

const unsigned int colorTrackingTranslationTable[] =
{
    GL_EMISSION, GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_AMBIENT_AND_DIFFUSE
};

const unsigned int lightTranslationTable[] =
{
    GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_POSITION, GL_QUADRATIC_ATTENUATION
};

const unsigned int primTypeTranslationTable[] =
{
    GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, GL_POLYGON, GL_LINE_LOOP
};

const unsigned int texParamTranslationTable[] =
{
    GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER
};

const unsigned int textureFormatTranslationTable[] =
{
    GL_RGBA, GL_RGB, GL_LUMINANCE_ALPHA, GL_LUMINANCE, GL_BGRA_EXT,
    GL_COMPRESSED_LUMINANCE_ARB, GL_COMPRESSED_LUMINANCE_ALPHA_ARB,
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
    GL_TEXTURE_COMPRESSED_ARB, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB
};

const unsigned int clientStateTranslationTable[] =
{
    GL_TEXTURE_COORD_ARRAY, GL_NORMAL_ARRAY, GL_COLOR_ARRAY, GL_VERTEX_ARRAY,
    GL_VERTEX_ATTRIB_ARRAY1_NV, GL_VERTEX_ATTRIB_ARRAY3_NV,
    GL_VERTEX_ATTRIB_ARRAY4_NV, GL_VERTEX_ATTRIB_ARRAY5_NV,
    GL_VERTEX_ATTRIB_ARRAY12_NV, GL_VERTEX_ATTRIB_ARRAY13_NV,
    GL_VERTEX_ATTRIB_ARRAY14_NV, GL_VERTEX_ATTRIB_ARRAY15_NV,
    GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV
};

float* GLRender::cm_pUserShadowData;

unsigned int GLRender::cm_nConvolutionRenderTexture[CONVOLUTION_BUFFER_COUNT];
HDC GLRender::cm_hConvolutionHDC[CONVOLUTION_BUFFER_COUNT];
HGLRC GLRender::cm_hConvolutionHGLRC[CONVOLUTION_BUFFER_COUNT];
PBUFFERARB* GLRender::cm_hConvolutionPBuffer[CONVOLUTION_BUFFER_COUNT];

Vector GLRender::m_vSaturationMultiplier(1.0f, 1.0f, 1.0f);
List<unsigned long> GLRender::cm_listSquaresToRender;

ImposterManager manRefImposter;
