#include <windows.h>
#include <GL/gl.h>
#include <aurlist.h>
#include "../math3d/math3d.h"

class PartLight;
inline void AurPchListPartLightPtr_Allocate() { (void)&List<PartLight*>::allocate; }
#include "aurmodel.h"
#define WIN32_LEAN_AND_MEAN
#include "aurprimitive.h"
#include "aurrendergl.h"
#include <math.h>

#include <aurora.h>
#include "auremitter.h"
#include "aurmaterial.h"
#include "aurlightmanager.h"
#include "aurlist.h"
#include "aurmodel.h"
#include "aurpointer.h"
#include "aurrendergl.h"
#include "aurscenemanager.h"
#include "aurtexcontrol.h"

static inline void AurmodelrenderTrace1();
#define AURMODELRENDER_TRACE() AurmodelrenderTrace1()
#define AUR_FORCE_GLOW_PULSE_STEP 0.024639942f

#ifndef _XBOX
#pragma inline_recursion(on)
#endif

int rendermaxdepth = 999999;

int enablemat = 1;
int cullparts = 1;
int counttrans = 0;
int countgobs = 0;
int countfaces = 0;
int countparts = 0;

int countshadowfaces = 0;
int countcullpart = 0;
#ifdef _XBOX
int usearbvertexprograms = 0;
#else
int usearbvertexprograms = 1;
#endif
int enabletransform = 1;
int bumpspecular = 1;
int bumpdiffuse = 1;
int enablebumpmap = 1;
int bumpother = 1;

int bumptexture = 1;
int enablelightmap = 1;
int cullnonrenderable = 1;
float cullonangle = 0.011f;

int renderhull = 0;
int drawbbox = 0;
int drawhull = 0;
int culltransparent = 0;

int cullonlocalarea = 0;
int enablerender = 1;
int usedrawelements = 1;
int enablelighting = 1;
int enablerenderlightmapped = 1;
int dosecondhalf = 1;
int enablenoshaderlightmaps = 1;
int useonepasssolution = 1;
float specularintensity = 1.0f;

int bmspecularexponent = 1;

int enableonepassenvmap = 1;
int environmentmapdest = 0;

int enableold = 0;
int useenvmapalphaondecal = 0;
int enableenvmap = 1;
int enablerenderenv = 1;
int renderemlm = 1;
float diffuseintensity = 1.0f;
float modulate = 1.0f;
int enabledecal = 1;
float shinyalpha = 1.0f;
int enablebsenvmap = 1;
int enablerenderembm = 1;
int modeembm = 1;
int embmphong = 1;
int testembm = 1;

int enablerenderembmlight = 0;
int testlightmap = 0;
int overridestuff = 0;

VertexProgram* g_pBumpyShinyPass1 = 0;
VertexProgram* g_pBumpyShinyPass2 = 0;
VertexProgram* g_pBumpyShinyPass3 = 0;
VertexProgram* g_pBumpyShinyPass4 = 0;
VertexProgram* g_pBumpyShinyPass5 = 0;
int enablebumpyshiny = 1;

int enablerenderbumpmap = 1;

int showrancoraxes = 0;

int dophong = 0;
int phongreset = 0;
int flushconstants = 0;
int enablevis = 0;
float polygonoffset = 1.0f;
float polygonoffset2 = 1.0f;
float phongweaken = 1.0f;
int nobumpdiff = 1;
int phongrender = 1;
float enableoffset = -0.01f;
int rendertrimesh = 1;
float shadowdepth = 0.1f;
float beamdepth = 20.0f;
int enablecompiledarrays = 1;
VertexProgram* g_pBumpedOutVP = NULL;

int shadowcount = 0;
int shadowvertcount = 0;
int shadowstripcount = 0;
int stenciloff = 1;
float projectionalpha = 0.2f;

static float walkmeshcolors[32][3] =
{
    {0.8f, 0.8f, 0.8f},
    {0.580392f, 0.486275f, 0.313726f},
    {0.8f, 0.8f, 0.0f},
    {0.243137f, 0.588235f, 0.125490f},
    {0.4f, 0.4f, 0.4f},
    {0.596078f, 0.349020f, 0.184314f},
    {0.0f, 0.745098f, 0.694118f},
    {0.8f, 0.0f, 0.0f},
    {0.996078f, 0.8f, 0.745098f},
    {0.509804f, 0.0f, 0.501961f},
    {0.623529f, 0.725490f, 0.725490f},
    {0.501961f, 0.996078f, 0.8f},
    {0.537255f, 0.615686f, 0.0f},
    {0.286275f, 0.113725f, 0.0f},
    {0.278431f, 0.356863f, 0.137255f},
    {0.8f, 0.388235f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0941177f, 0.8f}
};
int enableplaceholders = 0;

int renderpieces = 1;
int rendersegments = 1;
int renderfrontpiece = 1;
int enablelightsaberbluradd = 0;
int renderextrapieces = 0;
int lightsaberrenderfronthalf = 1;
float lightsabersegmentinterval = 0.001f;
int lightsabertotalsegments = 0;
float lightsaberfudgefactor = 0.5f;
int lightsaberwireframe = 0;
int lightsaberlighting = 0;
int lightsaberrender = 1;
int renderdangly = 1;

float partalpha = 0.5f;
int shadowmodeone = 1;
int shadowmodetwo = 1;
float cullProjectionSize = 0.25f;

int showlights = 0;
int ignoremat = 0;

int doallmaterials = 0;
int alwaysmat = 0;
float epsilon = 0.0f;
int maxshadowvar = 0;
int useshadowmethod = 0;

int doskinshadows = 0;
int rendergobbbox = 0;
int rendergobspheres = 0;

int LOD = 0;
int cullwithLOD = 0;
float mindooralpha = 0.25f;
int enablerendercharacter = 1;
int g_bFadeDoors = 0;

int Gob::cm_nLastGobIndex = 0;

int enablegobrender = 1;

static float testpath[12] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                             1.0f, 2.0f, 0.0f, 2.0f, 0.0f, 0.0f};
extern int rint(float fValue);
extern int stenciloff;
extern int enablelighting;
Vector g_vZero(0.0f, 0.0f, 0.0f);
Vector g_vOne(1.0f, 1.0f, 1.0f);
unsigned long g_SceneRender_CurrentTime_ms;
Vector projectioncolor(1.0f, 1.0f, 1.0f);
static Vector currentcolor(1.0f, 1.0f, 1.0f);
class Scene;
extern int renderemlm;
extern int enablerenderenv;
extern int enableenvmap;
bool AurTextureEnvMapDisabled();
bool AurNV1xLevelShadersAvailable();
GLboolean AurBumpMapCardValidate();

extern int usebumpdiffuse;
extern int usebumpspecular;

extern int bumpspecular;

extern int enablerenderbumpmap;
extern int useonepasssolution;
extern int bumpdiffuse;
extern int bumpother;
extern int bumptexture;

extern int g_bBumpDiffuseColor;
extern int g_bBumpSpecularColor;
extern float diffuseintensity;

extern float specularintensity;

extern int rendertrimesh;
extern float lightsabersegmentlife;
static RenderVertexC* GetWalkMeshColors();
static RenderVertexC* GetInverseWalkMeshColors();
#ifdef _XBOX

void Transform(const Vector& vPosition, const Quaternion& qOrientation,
               int nForce);

#define AurCubeMapAvailable() 1
#define AurOnePassEnvMapAvailable() 1
#define AurMultiTextureAvailable() 1
#define AurVertexProgramsAvailable() 1
#define AurATIValidate() 1

#define SetBlendMode(nSource, nDestination) GLRender::SetBlendFunc(nSource, nDestination)
#endif

class VertexPrimitiveFlat;

void RenderFlat(VertexPrimitiveFlat* pPrimitive);
void RenderEMLM(VertexPrimitiveFlat* pPrimitive);
#ifndef _XBOX
void RenderEnvironmentMapped(VertexPrimitiveFlat* pPrimitive, bool, bool);
#endif
void RenderEMBM(VertexPrimitiveFlat* pPrimitive);
void RenderLightMappedBumpSpecular(VertexPrimitiveFlat* pPrimitive);
void RenderLightMappedBumpDiffuse(VertexPrimitiveFlat* pPrimitive);
void RenderNormalMapped(VertexPrimitiveFlat* pPrimitive);
void RenderBumpMapped(VertexPrimitiveFlat* pPrimitive);
void RenderNV1xEMLM(VertexPrimitiveFlat* pPrimitive);
void RenderLightMappedNoShaders(VertexPrimitiveFlat* pPrimitive);
int AurBumpyShinyAvailable();

int IsTriMeshRenderable(PartTriMesh* pPart);

extern int countdisplaycreates;

class ShadowInfo
{
public:

    enum { MAX_FACES = 2048 };
    float m_fDistance;

    short m_anAdjacentFaces[3];
    short m_anVertexIndices[3];
};
// PC 0x00485E00 GOG 0x00485EC0
int countreset()
{
    countbinds = 0;
    countgobs = 0;
    countfaces = 0;
    countparts = 0;
    countshadowfaces = 0;
    counttrans = 0;
    countcullpart = 0;
    countdisplaycreates = 0;
    return 0;
}
// PC 0x00485E30 GOG 0x00485EF0
void PartTriMesh::RotateTexture(float fAngle)
{
    m_fTextureAngle = fAngle;
}
// PC 0x00485E40 GOG 0x00485F00
#ifdef _XBOX
inline
#endif
int IsTriMeshRenderable(PartTriMesh* pPart)
{
    MdlNodeTriMesh* pNode = (MdlNodeTriMesh*)pPart->m_pNode;
    if (pPart->m_fAlpha <= 0.001f)
        return 0;
    if (cullnonrenderable && !pNode->m_bRender)
        return 0;
    if ((culltransparent && pNode->m_nTransparencyHint != 0) ||
        pNode->m_nVertexCount == 0)
        return 0;
    else
        return 1;
}

#ifndef _XBOX
inline void GLRender::Scale(float fX, float fY, float fZ)  // PC 0x0046E300 GOG 0x0046E460
{
    glScalef(fX, fY, fZ);
}
#endif

#include "aurcamera.h"
#include "aurscene.h"

#ifdef _XBOX

void GLRender::DrawAxisAlignedFrame(const Vector& vMinimum,
                                    const Vector& vMaximum,
                                    const Vector& vColor)
{
    RenderBox(vMinimum, vMaximum, vColor);
}
#else
inline void GLRender::DrawAxisAlignedFrame(const Vector& vMinimum,  // PC 0x00485F10 GOG 0x00485FD0
                                    const Vector& vMaximum,
                                    const Vector& vColor)
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor3f(vColor.m_fX, vColor.m_fY, vColor.m_fZ);

    glBegin(GL_LINE_LOOP);
    glVertex3f(vMinimum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
    GLRender_EndImmediatePrimitive();

    glBegin(GL_LINE_LOOP);
    glVertex3f(vMinimum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);
    GLRender_EndImmediatePrimitive();

    glBegin(GL_LINES);
    glVertex3f(vMinimum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMaximum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
    glVertex3f(vMinimum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);
    GLRender_EndImmediatePrimitive();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}
// PC 0x00486060 GOG 0x00486120
inline void GLRender::Transform(const Vector& vPosition,
                         const Quaternion& qOrientation, int nForce)
{
    static GLfloat m[16];
    GLfloat fAngle;

    if (multmatrix != 0 || nForce != 0)
    {
        ++counttrans;
        if (multmatrix > 0)
        {
            MakeMatrix4x4(m, vPosition, qOrientation);
            glMultMatrixf(m);
        }
        else
        {
            static Vector p;
            static Quaternion q;

            p = vPosition;
            glTranslatef(p.m_fX, p.m_fY,
                         p.m_fZ);
            q = qOrientation;
            Vector vAxis = q.axis();
            double fHalfAngle = acos(q.m_fW);
            fAngle = (float)(fHalfAngle + fHalfAngle) * RAD_TO_DEG;
            glRotatef(fAngle, vAxis.m_fX, vAxis.m_fY, vAxis.m_fZ);
        }
    }
}
#endif
// PC 0x004861A0 GOG 0x00486260
int PartTriMesh::WithinRange(Vector vPosition, float fRange)
{
    Vector vDifference;

    vDifference.m_fX = m_vPosition.m_fX - vPosition.m_fX;
    vDifference.m_fY = m_vPosition.m_fY - vPosition.m_fY;
    vDifference.m_fZ = m_vPosition.m_fZ - vPosition.m_fZ;
    float fDistance = magnitude(vDifference);
    fRange += 2.0f;
    return fDistance < fRange;
}
// PC 0x00486200 GOG 0x004862C0
static RenderVertexC* GetWalkMeshColors()
{
#ifdef _XBOX

    unsigned long nPool;
    GLRender::RequestPool(Aurora::POOL_VERTEX, 32 * sizeof(unsigned long), 0, 0, &nPool);
    unsigned long* colorarray = (unsigned long*)GLRender::LockPool(nPool, 0, 0, 1);
#else
    static unsigned long* colorarray = new unsigned long[32];
#endif

    for (int i = 0; i < 32; i++)
    {
        int nRed;
        int nGreen;
        int nBlue;
        roundToInt(nRed, walkmeshcolors[i][0] * 255.0f);
        roundToInt(nGreen, walkmeshcolors[i][1] * 255.0f);
        roundToInt(nBlue, walkmeshcolors[i][2] * 255.0f);
#ifdef _XBOX
        colorarray[i] = nBlue | ((nGreen | ((nRed | (0xFFFFFF << 8)) << 8)) << 8);
#else
        colorarray[i] = nRed | ((nGreen | ((nBlue | (0xFF << 8)) << 8)) << 8);
#endif
    }

#ifdef _XBOX
    GLRender::UnlockPool(nPool);
    return (RenderVertexC*)nPool;
#else
    return (RenderVertexC*)colorarray;
#endif
}
// PC 0x00486300 GOG 0x004863C0
static RenderVertexC* GetInverseWalkMeshColors()
{
#ifdef _XBOX

    unsigned long nPool;
    GLRender::RequestPool(Aurora::POOL_VERTEX, 32 * sizeof(unsigned long), 0, 0, &nPool);
    unsigned long* colorarray = (unsigned long*)GLRender::LockPool(nPool, 0, 0, 1);
#else
    static unsigned long* colorarray = new unsigned long[32];
#endif

    for (int i = 0; i < 32; i++)
    {
        int nRed;
        int nGreen;
        int nBlue;
        roundToInt(nRed, (1.0f - walkmeshcolors[i][0]) * 255.0f);
        roundToInt(nGreen, (1.0f - walkmeshcolors[i][1]) * 255.0f);
        roundToInt(nBlue, (1.0f - walkmeshcolors[i][2]) * 255.0f);
#ifdef _XBOX
        colorarray[i] = nBlue | ((nGreen | ((nRed | (0xFFFFFF << 8)) << 8)) << 8);
#else
        colorarray[i] = nRed | ((nGreen | ((nBlue | (0xFF << 8)) << 8)) << 8);
#endif
    }

#ifdef _XBOX
    GLRender::UnlockPool(nPool);
    return (RenderVertexC*)nPool;
#else
    return (RenderVertexC*)colorarray;
#endif
}
// PC 0x00486410 GOG 0x004864D0
bool StaticPartNeedsMat(PartTriMesh* pPart, MdlNodeTriMesh* pNode,
                        bool& bHasTileData, bool& bUnknown)
{
    if (pPart->m_bColors == 0)
    {
        if (pPart->m_pTileData != 0)
        {
            bHasTileData = pPart->m_pTileData->m_bSuppliesMaterial != 0;
        }

        if (bHasTileData)
        {
            bUnknown = true;
            return true;
        }
    }

    if (equal(pNode->m_vDiffuse, g_vOne) == 0 ||
        pPart->m_fAlpha != 1.0f ||
        equal(pPart->m_vSelfIllumColor, g_vZero) == 0 ||
        pPart->m_pMaterial->m_bDecal != 0)
    {
        return true;
    }

    return false;
}
// PC 0x00489B10 GOG 0x00489BE0
void PartAnimMesh::Draw(bool bRenderChildren)
{
    VertexPrimitiveFlat* pPrimitive;

    if (m_pAurObject == 0)
    {
        GLRender_PushCurrentMatrix();
#ifdef _XBOX
        Transform(m_vPosition, m_qOrientation, 0);
#else
        if (enabletransform != 0)
        {
            GLRender::Transform(m_vPosition, m_qOrientation, 0);
        }
#endif
    }

    m_pVertexPrimitive->SetupVertexArraysStandard();
    pPrimitive =
        (VertexPrimitiveFlat*)m_pVertexPrimitive->AsVertexPrimitiveFlat();
    RenderFlat(pPrimitive);

    if (m_pAurObject == 0)
    {
        GLRender_PopCurrentMatrix();
    }
}

void Transform(const Vector& vPosition, const Quaternion& qOrientation,
               int nForce);
// PC 0x00489B70 GOG 0x00489C40
void PartAABB::Draw(bool bRenderChildren)
{
    if (renderaabb != 0)
    {
        static RenderVertexC* walkMeshColorToken = GetWalkMeshColors();
        static RenderVertexC* inverseWalkMeshColorToken = GetInverseWalkMeshColors();

        MdlNodeAABB* pNode = (MdlNodeAABB*)m_pNode;
        GLRender_Disable(ENABLE_TEXTURE_2D);
        GLRender_Disable(ENABLE_LIGHTING);
        GLRender_SetColor4f(0.0f, 0.0f, 0.0f, 0.0f);
        if (pNode != 0)
        {
            if (m_pAurObject == 0)
            {
                GLRender_PushCurrentMatrix();
                Transform(m_vPosition, m_qOrientation, 0);
            }

            countfaces += pNode->m_lFaces.m_nCount;
#ifdef _XBOX

            GLRender::DrawTriangles(pNode->m_lFaces.m_nCount, pNode->m_lFaces,
                pNode->m_pModel->AsModel()->m_nVertexPool, pNode->m_nVertexStride, pNode->m_nPositionOffset,
                pNode->m_nVertexCount, (unsigned long)walkMeshColorToken, pNode->m_nVertexDataOffset);
            GLRender_SetFrontFace(CULL_CLOCKWISE);
            GLRender::DrawTriangles(pNode->m_lFaces.m_nCount, pNode->m_lFaces,
                pNode->m_pModel->AsModel()->m_nVertexPool, pNode->m_nVertexStride, pNode->m_nPositionOffset,
                pNode->m_nVertexCount, (unsigned long)inverseWalkMeshColorToken, pNode->m_nVertexDataOffset);
            GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
            GLRender_SetPolygonRenderMode(FACING_FRONT, FILL_LINE);
            GLRender::DrawTriangles(pNode->m_lFaces.m_nCount, pNode->m_lFaces,
                pNode->m_pModel->AsModel()->m_nVertexPool, pNode->m_nVertexStride, pNode->m_nPositionOffset,
                pNode->m_nVertexCount, (unsigned long)inverseWalkMeshColorToken, pNode->m_nVertexDataOffset);
            GLRender_SetFrontFace(CULL_CLOCKWISE);
            GLRender::DrawTriangles(pNode->m_lFaces.m_nCount, pNode->m_lFaces,
                pNode->m_pModel->AsModel()->m_nVertexPool, pNode->m_nVertexStride, pNode->m_nPositionOffset,
                pNode->m_nVertexCount, (unsigned long)walkMeshColorToken, pNode->m_nVertexDataOffset);
            GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
            GLRender_SetPolygonRenderMode(FACING_FRONT, FILL_SOLID);
#else
            GLRender::DrawTriangles(pNode->m_lFaces,
                (RenderVertexV*)pNode->m_pVertexPositions, walkMeshColorToken);
            GLRender_SetFrontFace(CULL_CLOCKWISE);
            GLRender::DrawTriangles(pNode->m_lFaces,
                (RenderVertexV*)pNode->m_pVertexPositions, inverseWalkMeshColorToken);
            GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
            GLRender_SetPolygonRenderMode(FACING_FRONT,
                          FILL_LINE);
            GLRender::DrawTriangles(pNode->m_lFaces,
                (RenderVertexV*)pNode->m_pVertexPositions, inverseWalkMeshColorToken);
            GLRender_SetFrontFace(CULL_CLOCKWISE);
            GLRender::DrawTriangles(pNode->m_lFaces,
                (RenderVertexV*)pNode->m_pVertexPositions, walkMeshColorToken);
            GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
            GLRender_SetPolygonRenderMode(FACING_FRONT,
                          FILL_SOLID);
#endif

            if (m_pAurObject == 0)
            {
                GLRender_PopCurrentMatrix();
            }
            GLRender_Enable(ENABLE_TEXTURE_2D);
            GLRender_Enable(ENABLE_LIGHTING);
        }
    }
}

#ifdef _XBOX
static inline void SetVertexData4f(Aurora::AuroraStreamMode nStream, float fX, float fY, float fZ, float fW);
#else
static void SetVertexData4f(Aurora::AuroraStreamMode nStream,
                            float fX, float fY, float fZ, float fW);
#endif
void PartLightsaber::Draw(bool bRenderChildren)  // PC 0x00489E70 GOG 0x00489F40
{
    if (lightsaberrender != 0)
    {
        MdlNodeLightsaber* pNode = m_pNode->AsMdlNodeLightsaber();
        Vector vVertex;
        Vector vWorldPosition;
        Quaternion qWorldOrientation;
        GetWorld(vWorldPosition, qWorldOrientation);

        GLRender_Disable(ENABLE_CULL_FACE);
        if (enablelightsaberbluradd != 0)
        {
            GLRender_SetBlendFunc(BLEND_ONE,
                        BLEND_ONE);
            GLRender_SetDepthMask(GL_FALSE);
            GLRender_SetStencilMask(0);
        }
        if (lightsaberlighting == 0)
            GLRender_Disable(ENABLE_LIGHTING);
        GLRender_SetColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        GLRender_SetPolygonRenderMode(FACING_FRONT_AND_BACK,
                      lightsaberwireframe ? FILL_LINE : FILL_SOLID);

        Vector* pPositions = pNode->m_pVertexPositions;
        float* pTextureCoords = pNode->m_pTextureCoordinates;
        Vector* pNormals = pNode->m_pVertexNormals;
        int i;
        if (renderpieces != 0 && m_nSegments < 2)
        {
            int nHalf = MdlNodeLightsaber::NUM_PIECE_VERTS / 2;
            GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_QUAD_STRIP);
            for (i = 0; i < nHalf; ++i)
            {
                GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * i], pTextureCoords[2 * i + 1]);
                SetVertexData4f(Aurora::STREAM_NORMAL, pNormals[i].m_fX, pNormals[i].m_fY, pNormals[i].m_fZ, 1.0f);
                SetVertexData4f(Aurora::STREAM_POSITION, pPositions[i].m_fX, pPositions[i].m_fY, pPositions[i].m_fZ, 1.0f);
                GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * (i + nHalf)],
                             pTextureCoords[2 * (i + nHalf) + 1]);
                SetVertexData4f(Aurora::STREAM_NORMAL, pNormals[i + nHalf].m_fX, pNormals[i + nHalf].m_fY, pNormals[i + nHalf].m_fZ, 1.0f);
                SetVertexData4f(Aurora::STREAM_POSITION, pPositions[i + nHalf].m_fX, pPositions[i + nHalf].m_fY, pPositions[i + nHalf].m_fZ, 1.0f);
            }
            GLRender_EndImmediatePrimitive();
            GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_QUAD_STRIP);
            for (i = MdlNodeLightsaber::NUM_PIECE_VERTS +
                     MdlNodeLightsaber::NUM_FRONT_SEGMENTS *
                     MdlNodeLightsaber::NUM_SEGMENT_VERTS;
                 i < MdlNodeLightsaber::NUM_PIECE_VERTS +
                     MdlNodeLightsaber::NUM_FRONT_SEGMENTS *
                     MdlNodeLightsaber::NUM_SEGMENT_VERTS + nHalf;
                 ++i)
            {
                GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * (i + nHalf)],
                             pTextureCoords[2 * (i + nHalf) + 1]);
                SetVertexData4f(Aurora::STREAM_NORMAL, pNormals[i + nHalf].m_fX, pNormals[i + nHalf].m_fY, pNormals[i + nHalf].m_fZ, 1.0f);
                SetVertexData4f(Aurora::STREAM_POSITION, pPositions[i + nHalf].m_fX, pPositions[i + nHalf].m_fY, pPositions[i + nHalf].m_fZ, 1.0f);
                GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * i], pTextureCoords[2 * i + 1]);
                SetVertexData4f(Aurora::STREAM_NORMAL, pNormals[i].m_fX, pNormals[i].m_fY, pNormals[i].m_fZ, 1.0f);
                SetVertexData4f(Aurora::STREAM_POSITION, pPositions[i].m_fX, pPositions[i].m_fY, pPositions[i].m_fZ, 1.0f);
            }
            GLRender_EndImmediatePrimitive();
            GLRender_SetPolygonRenderMode(FACING_FRONT_AND_BACK,
                          FILL_SOLID);
            GLRender_Enable(ENABLE_CULL_FACE);
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                        BLEND_ONE_MINUS_SRC_ALPHA);
            GLRender_Enable(ENABLE_LIGHTING);
            GLRender_SetDepthMask(GL_TRUE);
            GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
            return;
        }

        lightsabertotalsegments = 0;
        float fTime = 0.0f;
        Vector avTrail[4];
        int nHalf = MdlNodeLightsaber::NUM_PIECE_VERTS / 2;
        Vector vBlade = pPositions[nHalf] - pPositions[0];
        avTrail[0] = vBlade;
        avTrail[1] = avTrail[2] = avTrail[3] = avTrail[0];

        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
            ;
        float fLength = magnitude(avTrail[0]);
        float fTrailLength;
        short nCurrent = m_nNextSegment - 1;
        if (nCurrent < 0)
            nCurrent += MdlNodeLightsaber::NUM_BACK_SEGMENTS;
        short nPrevious = m_nNextSegment - 2;
        if (nPrevious < 0)
            nPrevious += MdlNodeLightsaber::NUM_BACK_SEGMENTS;
        Quaternion qCurrent;
        Quaternion qPrevious;
        Quaternion qOlder;
        Quaternion qNewer;
        Quaternion qNext;
        Vector vPrevious;
        int nSegment;
        Vector vCurrent = m_avSegmentPosition[nCurrent];
        qCurrent = m_aqSegmentOrientation[nCurrent];
        int nEnd = MdlNodeLightsaber::NUM_PIECE_VERTS +
                   MdlNodeLightsaber::NUM_FRONT_SEGMENTS *
                   MdlNodeLightsaber::NUM_SEGMENT_VERTS;
        for (nSegment = 1; nSegment < m_nSegments; ++nSegment)
        {
            float fFraction = lightsabersegmentinterval /
                (m_afSegmentAge[nPrevious] - m_afSegmentAge[nCurrent]);
            while (fFraction < 1.0f)
            {
                fTime += lightsabersegmentinterval;
                if (fTime > lightsabersegmentlife)
                    break;
                vPrevious = vCurrent;
                qPrevious = qCurrent;
                vCurrent = Interpolate(m_avSegmentPosition[nCurrent],
                                       m_avSegmentPosition[nPrevious], fFraction);
                qCurrent = slerp(m_aqSegmentOrientation[nCurrent],
                                 m_aqSegmentOrientation[nPrevious], fFraction);
                ++lightsabertotalsegments;
                GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_QUAD_STRIP);
                for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
                {
                    GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * (nEnd + i)],
                                 pTextureCoords[2 * (nEnd + i) + 1]);
                    vVertex = Vector(pPositions[nEnd + i].m_fX, pPositions[nEnd + i].m_fY, pPositions[nEnd + i].m_fZ);
                    vVertex = -qWorldOrientation *
                        (vCurrent + qCurrent * vVertex - vWorldPosition);
                    SetVertexData4f(Aurora::STREAM_POSITION, vVertex.m_fX, vVertex.m_fY, vVertex.m_fZ, 1.0f);
                    avTrail[i] = vVertex;
                    GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * (nEnd + i)],
                                 pTextureCoords[2 * (nEnd + i) + 1]);
                    vVertex = Vector(pPositions[nEnd + i].m_fX, pPositions[nEnd + i].m_fY, pPositions[nEnd + i].m_fZ);
                    vVertex = -qWorldOrientation *
                        (vPrevious + qPrevious * vVertex - vWorldPosition);
                    SetVertexData4f(Aurora::STREAM_POSITION, vVertex.m_fX, vVertex.m_fY, vVertex.m_fZ, 1.0f);
                    avTrail[i] = avTrail[i] - vVertex;
                }
                GLRender_EndImmediatePrimitive();
                fFraction += lightsabersegmentinterval /
                    (m_afSegmentAge[nPrevious] - m_afSegmentAge[nCurrent]);
            }
            nCurrent = nPrevious;
            if (--nPrevious < 0)
                nPrevious += MdlNodeLightsaber::NUM_BACK_SEGMENTS;
        }
        for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
        {
            fTrailLength = magnitude(avTrail[i]);
            if (fTrailLength != 0.0f)
            {
                if (lightsaberfudgefactor * fLength > fTrailLength)
                    avTrail[i] = vBlade;
                else
                    avTrail[i] = avTrail[i] / fTrailLength * fLength;
            }
        }
        GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_QUAD_STRIP);
        for (i = 0; i < MdlNodeLightsaber::NUM_PIECE_VERTS / 2; ++i)
        {
            GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * (nEnd + nHalf + i)],
                         pTextureCoords[2 * (nEnd + nHalf + i) + 1]);
            vVertex = Vector(pPositions[nEnd + i].m_fX, pPositions[nEnd + i].m_fY, pPositions[nEnd + i].m_fZ);
            vVertex = avTrail[i] + -qWorldOrientation *
                (vCurrent + qCurrent * vVertex - vWorldPosition);
            SetVertexData4f(Aurora::STREAM_POSITION, vVertex.m_fX, vVertex.m_fY, vVertex.m_fZ, 1.0f);
            GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * (nEnd + i)],
                         pTextureCoords[2 * (nEnd + i) + 1]);
            vVertex = Vector(pPositions[nEnd + i].m_fX, pPositions[nEnd + i].m_fY, pPositions[nEnd + i].m_fZ);
            vVertex = -qWorldOrientation *
                (vCurrent + qCurrent * vVertex - vWorldPosition);
            SetVertexData4f(Aurora::STREAM_POSITION, vVertex.m_fX, vVertex.m_fY, vVertex.m_fZ, 1.0f);
        }
        GLRender_EndImmediatePrimitive();

        if (lightsaberrenderfronthalf != 0)
        {
            fTime = 0.0f;
            nCurrent = m_nNextSegment - 1;
            if (nCurrent < 0)
                nCurrent += MdlNodeLightsaber::NUM_BACK_SEGMENTS;
            nPrevious = m_nNextSegment - 2;
            if (nPrevious < 0)
                nPrevious += MdlNodeLightsaber::NUM_BACK_SEGMENTS;
            Vector vNewer;
            vNewer = m_avSegmentPosition[nCurrent];
            qNewer = m_aqSegmentOrientation[nCurrent];
            Vector vOlder;
            vOlder = m_avSegmentPosition[nPrevious];
            qOlder = m_aqSegmentOrientation[nPrevious];
            vCurrent = vNewer;
            qCurrent = qNewer;
            for (nSegment = 1; nSegment < m_nSegments; ++nSegment)
            {
                Vector vNext = Interpolate(vOlder, vNewer, 2.0f);
                vVertex = Vector(pPositions[0].m_fX, pPositions[0].m_fY, pPositions[0].m_fZ);
                Vector vOlderStart;
                vOlderStart = -qWorldOrientation *
                    (vOlder + qOlder * vVertex - vWorldPosition);
                vVertex = Vector(pPositions[3].m_fX, pPositions[3].m_fY, pPositions[3].m_fZ);
                vVertex = -qWorldOrientation *
                    (vOlder + qOlder * vVertex - vWorldPosition);
                Vector vOlderBlade;
                vOlderBlade = vVertex - vOlderStart;
                vVertex = Vector(pPositions[0].m_fX, pPositions[0].m_fY, pPositions[0].m_fZ);
                Vector vNewerStart;
                vNewerStart = -qWorldOrientation *
                    (vNewer + qNewer * vVertex - vWorldPosition);
                vVertex = Vector(pPositions[3].m_fX, pPositions[3].m_fY, pPositions[3].m_fZ);
                vVertex = -qWorldOrientation *
                    (vNewer + qNewer * vVertex - vWorldPosition);
                Vector vNewerBlade;
                vNewerBlade = vVertex - vNewerStart;
                qNext = RotationArc(vOlderBlade, vNewerBlade);
                qNext = qNewer * qNext;
                float fFraction = (float)(lightsabersegmentinterval /
                    (m_afSegmentAge[nPrevious] - m_afSegmentAge[nCurrent]));
                while (fFraction < 1.0f)
                {
                    fTime += lightsabersegmentinterval;
                    if (fTime > lightsabersegmentlife)
                        break;
                    vPrevious = vCurrent;
                    qPrevious = qCurrent;
                    vCurrent = Interpolate(vNewer, vNext, fFraction);
                    qCurrent = slerp(qNewer, qNext, fFraction);
                    ++lightsabertotalsegments;
                    GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_QUAD_STRIP);
                    for (i = 0; i < MdlNodeLightsaber::NUM_SEGMENT_VERTS; ++i)
                    {
                        GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * i], pTextureCoords[2 * i + 1]);
                        vVertex = Vector(pPositions[i].m_fX, pPositions[i].m_fY, pPositions[i].m_fZ);
                        vVertex = -qWorldOrientation *
                            (vPrevious + qPrevious * vVertex - vWorldPosition);
                        SetVertexData4f(Aurora::STREAM_POSITION, vVertex.m_fX, vVertex.m_fY, vVertex.m_fZ, 1.0f);
                        avTrail[i] = vVertex;
                        GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * i], pTextureCoords[2 * i + 1]);
                        vVertex = Vector(pPositions[i].m_fX, pPositions[i].m_fY, pPositions[i].m_fZ);
                        vVertex = -qWorldOrientation *
                            (vCurrent + qCurrent * vVertex - vWorldPosition);
                        SetVertexData4f(Aurora::STREAM_POSITION, vVertex.m_fX, vVertex.m_fY, vVertex.m_fZ, 1.0f);
                        avTrail[i] = avTrail[i] - vVertex;
                    }
                    GLRender_EndImmediatePrimitive();
                    fFraction += lightsabersegmentinterval /
                        (m_afSegmentAge[nPrevious] - m_afSegmentAge[nCurrent]);
                }
                vOlder = vNewer;
                qOlder = qNewer;
                vNewer = vNext;
                qNewer = qNext;
                nCurrent = nPrevious;
                if (--nPrevious < 0)
                    nPrevious += MdlNodeLightsaber::NUM_BACK_SEGMENTS;
            }
            GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_QUAD_STRIP);
            for (i = 0; i < MdlNodeLightsaber::NUM_PIECE_VERTS / 2; ++i)
            {
                fTrailLength = magnitude(avTrail[i]);
                if (fTrailLength != 0.0f)
                {
                    if (lightsaberfudgefactor * fLength > fTrailLength)
                        avTrail[i] = -vBlade;
                    else
                        avTrail[i] = avTrail[i] / fTrailLength * -fLength;
                }
                GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * i], pTextureCoords[2 * i + 1]);
                vVertex = Vector(pPositions[i].m_fX, pPositions[i].m_fY, pPositions[i].m_fZ);
                vVertex = -qWorldOrientation *
                    (vCurrent + qCurrent * vVertex - vWorldPosition);
                SetVertexData4f(Aurora::STREAM_POSITION, vVertex.m_fX, vVertex.m_fY, vVertex.m_fZ, 1.0f);
                GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pTextureCoords[2 * (nHalf + i)],
                             pTextureCoords[2 * (nHalf + i) + 1]);
                vVertex = Vector(pPositions[i].m_fX, pPositions[i].m_fY, pPositions[i].m_fZ);
                vVertex = avTrail[i] + -qWorldOrientation *
                    (vCurrent + qCurrent * vVertex - vWorldPosition);
                SetVertexData4f(Aurora::STREAM_POSITION, vVertex.m_fX, vVertex.m_fY, vVertex.m_fZ, 1.0f);
            }
            GLRender_EndImmediatePrimitive();
        }
        GLRender_SetPolygonRenderMode(FACING_FRONT_AND_BACK,
                      FILL_SOLID);
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                    BLEND_ONE_MINUS_SRC_ALPHA);
        GLRender_Enable(ENABLE_LIGHTING);
        GLRender_Enable(ENABLE_CULL_FACE);
        GLRender_SetDepthMask(GL_TRUE);
        GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
    }
}

static void SetVertexData4f(Aurora::AuroraStreamMode nStream,
                            float fX, float fY, float fZ, float fW)
{
#ifdef _XBOX
    GLRender::SetVertexData4f(nStream, fX, fY, fZ, fW);
#else
    switch (nStream)
    {
    case Aurora::STREAM_POSITION:
        glVertex4f(fX, fY, fZ, fW);
        break;
    case Aurora::STREAM_COLOR:
        GLRender_SetColor4f(fX, fY, fZ, fW);
        break;
    case Aurora::STREAM_NORMAL:
        glNormal3f(fX, fY, fZ);
        break;
    }
#endif
}
// PC 0x0048E200 GOG 0x0048E2D0
void PartLight::Mat()
{
    AURMODELRENDER_TRACE();
    float fRed = m_vColor.m_fX;
    float fGreen = m_vColor.m_fY;
    float fBlue = m_vColor.m_fZ;

    GLRender_SetColor4f(fRed, fGreen, fBlue, 1.0f);
    currentcolor = m_vColor;
}

void DynamicPartCalculateMaterial(Gob* pGob, Vector& vColor,
                                  Vector& vIllumination, float& fAlpha);

void PartTriMesh::Mat()
{
    AURMODELRENDER_TRACE();
    MdlNodeTriMesh* pNode = (MdlNodeTriMesh*)m_pNode;
    static Vector vLastDiffuse(-1.0f, -1.0f, -1.0f);
    static Vector vLastAmbient(-1.0f, -1.0f, -1.0f);
    static Vector vLastIllumination(-1.0f, -1.0f, -1.0f);
    static float fLastAlpha = -1.0f;
    static bool bLastNeedsMat = true;
    static bool bLastFog = true;
    bool bHasTileData = false;
    bool bTileMaterial = false;
    bool bNeedsMat;
    float fAlpha = m_fAlpha;

    if (currentscene && currentscene->m_bFog)
    {
        if (m_bFog)
        {
            GLRender_Enable(ENABLE_FOG);
            bLastFog = true;
        }
        if (!m_bFog)
        {
            GLRender_Disable(ENABLE_FOG);
            bLastFog = false;
        }
    }

    if (m_pAurObject)
    {
        if (!m_pAurObject->m_bSingleTexture)
        {
            m_pMaterial->BindTexture0();
            m_pMaterial->SetBlendingMode(0);
        }
        bNeedsMat = true;
    }
    else
    {
        bNeedsMat = StaticPartNeedsMat(this, pNode, bHasTileData, bTileMaterial);
    }
    if (ignoremat)
        return;

    static Vector vDiffuse;
    static Vector vAmbient;
    static Vector vIllumination;
#ifdef _XBOX
    if (!bNeedsMat)
    {
        if (!bLastNeedsMat)
            return;
#else
    GLRender_SetColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    if (!bNeedsMat)
    {
        if (!bLastNeedsMat && !alwaysmat)
            return;
#endif
        vDiffuse.m_fX = 1.0f;
        vDiffuse.m_fY = 1.0f;
        vDiffuse.m_fZ = 1.0f;
        vAmbient.m_fX = 1.0f;
        vAmbient.m_fY = 1.0f;
        vAmbient.m_fZ = 1.0f;
        vIllumination.m_fX = 0.0f;
        vIllumination.m_fY = 0.0f;
        vIllumination.m_fZ = 0.0f;
    }
    else
    {
        vDiffuse = pNode->m_vDiffuse;
        vAmbient = pNode->m_vAmbient;
        vIllumination = m_pMaterial->m_bDecal ? g_vOne : m_vSelfIllumColor;
        if (m_pAurObject)
            DynamicPartCalculateMaterial(m_pAurObject, vDiffuse, vIllumination,
                                         fAlpha);
    }

#ifdef _XBOX
    if ((bLastNeedsMat || bNeedsMat) &&
        (!equal(vLastDiffuse, vDiffuse) || !equal(vLastAmbient, vAmbient) ||
         !equal(vLastIllumination, vIllumination) || fLastAlpha != fAlpha ||
         m_bMaterialHasChanged))
#else
    if (((bLastNeedsMat || bNeedsMat) &&
         (!equal(vLastDiffuse, vDiffuse) || !equal(vLastAmbient, vAmbient) ||
          !equal(vLastIllumination, vIllumination) || fLastAlpha != fAlpha ||
          m_bMaterialHasChanged)) || alwaysmat)
#endif
    {
        if ((usevertexprograms || enablebumpyshiny) && AurVertexProgramsAvailable())
        {
            Vector vColor = vIllumination;
            if (currentscene && m_pAurObject)
                vColor = vColor + cmul(currentscene->m_vAmbient, vAmbient);
            float fRed = vColor.m_fX;
            float fGreen = vColor.m_fY;
            float fBlue = vColor.m_fZ;
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_MATERIAL_ILLUMINATION,
                fRed, fGreen, fBlue, 1.0f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_MATERIAL_DIFFUSE,
                vDiffuse.m_fX, vDiffuse.m_fY, vDiffuse.m_fZ, fAlpha);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_MATERIAL_AMBIENT,
                vAmbient.m_fX, vAmbient.m_fY, vAmbient.m_fZ, 1.0f);
        }
        float* pAmbient = Homogenize(vAmbient, 1.0f);
        GLRender_SetMaterialv(FACING_FRONT, MATERIAL_AMBIENT, pAmbient);
        float* pDiffuse = Homogenize(vDiffuse, fAlpha);
        GLRender_SetMaterialv(FACING_FRONT, MATERIAL_DIFFUSE, pDiffuse);
        float* pEmission = Homogenize(vIllumination, 1.0f);
        GLRender_SetMaterialv(FACING_FRONT, MATERIAL_EMISSION, pEmission);
#ifdef _XBOX
        if (m_pAurObject)
        {
            m_pMaterial->SetMaterialColor(vDiffuse, fAlpha);
        }
#else
        if (m_pAurObject)
        {
            m_pMaterial->SetMaterialColor(vDiffuse, fAlpha);
            GLRender_SetColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else
        {
            GLRender_SetColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        }
#endif
        bLastNeedsMat = bNeedsMat;
        vLastDiffuse = vDiffuse;
        vLastAmbient = vAmbient;
        vLastIllumination = vIllumination;
        fLastAlpha = fAlpha;
        m_bMaterialHasChanged = false;
    }
}

void DynamicPartCalculateMaterial(Gob* pGob, Vector& vColor,
                                  Vector& vIllumination, float& fAlpha)
{
    if (pGob->m_bColorShifting)
    {
        fAlpha *= pGob->m_fAlpha;
        vColor = cmul(vColor, pGob->m_vColor);
    }

    if ((bool)pGob->m_pModel->m_Classification.m_bDoor)
    {
        fAlpha *= pGob->m_fOcclusionAlpha;
    }

    if (pGob->m_bIllumination)
    {
        vIllumination = vIllumination + pGob->m_vIllumination;
    }
}

#ifndef _XBOX
void BindVertexProgram(unsigned int nProgram)
{
    if (usearbvertexprograms != 0)
    {
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB, nProgram);
    }
    else
    {
        glBindProgramNV(GL_VERTEX_PROGRAM_NV, nProgram);
    }
}
#endif

int ClipProjectionCaps(Vector& vStart, Vector& vEnd, const Plane& plane)
{
    if (vEnd.m_fX * plane.m_vNormal.m_fX + vEnd.m_fY * plane.m_vNormal.m_fY +
            vEnd.m_fZ * plane.m_vNormal.m_fZ + plane.m_fDistance >
        0.0f)
    {
        if (plane.m_fDistance > 0.0f)
        {
            return 0;
        }

        vEnd = planelineintersection(plane.m_vNormal, plane.m_fDistance, vStart,
                                     vEnd);
    }

    if (vStart.m_fX * plane.m_vNormal.m_fX +
            vStart.m_fY * plane.m_vNormal.m_fY +
            vStart.m_fZ * plane.m_vNormal.m_fZ + plane.m_fDistance >
        0.0f)
    {
        vStart = planelineintersection(plane.m_vNormal, plane.m_fDistance,
                                       vStart, vEnd);
    }

    return 1;
}

void ClipProjection(Vector& vPosition, const Vector& vStart, const Plane& plane)
{
    if (vPosition.m_fX * plane.m_vNormal.m_fX +
            vPosition.m_fY * plane.m_vNormal.m_fY +
            vPosition.m_fZ * plane.m_vNormal.m_fZ + plane.m_fDistance >
        0.0f)
    {
        vPosition = planelineintersection(plane.m_vNormal, plane.m_fDistance,
                                          vStart, vPosition);
    }
}

// PC 0x004866C0 GOG 0x00486780
void SetBumpLightColor(Vector& vColor)
{
    GLRender_SetBumpLightColor(vColor, 1.0f);
}

// PC 0x004866E0 GOG 0x004867A0
void SetPerInstanceIllumination(float* pColor)
{
    memcpy(GLRender::m_PerInstanceIllumination, pColor, sizeof(GLRender::m_PerInstanceIllumination));
}

void Transform(const Vector& vPosition, const Quaternion& qOrientation,
               int nForce)
{
    if (enabletransform != 0)
    {
        GLRender::Transform(vPosition, qOrientation, nForce);
    }
}

void PartTriMesh::TransformTextureMatrix()
{
    GLRender::TransformTextureMatrix(m_fTextureAngle);
}

void PartTriMesh::RestoreTextureMatrix()
{
    GLRender::RestoreTextureMatrix(m_fTextureAngle);
}

void PartTriMesh::TransformModelviewMatrix()
{
    if (m_pAurObject == 0)
    {
        GLRender_PushCurrentMatrix();
#ifdef _XBOX
        Transform(m_vPosition, m_qOrientation, 0);
#else
        if (enabletransform != 0)
        {
            GLRender::Transform(m_vPosition, m_qOrientation, 0);
        }
#endif
    }
}

void PartTriMesh::RestoreModelviewMatrix()
{
    if (m_pAurObject == 0)
    {
        GLRender_PopCurrentMatrix();
    }
}

void DoEnvMapTransform(CAurTexture* pTexture, unsigned long nStage)
{
    GLfloat m[16];

#ifdef _XBOX

    if (pTexture->IsCubeMap())
    {
        GLRender::SetActiveTexture(nStage);
        GLRender::SetMatrixMode(3);
        GLRender::PushCurrentMatrix();
        if (CurrentCamera != 0)
        {
            MakeMatrix4x4(m, g_vZero, CurrentCamera->m_qOrientation);
        }
        else
        {
            Quaternion qOrientation;
            MakeMatrix4x4(m, g_vZero, qOrientation);
        }

        GLRender::MultMatrix(m);
        GLRender::ApplyMatrix(3);
        GLRender::SetMatrixMode(2);
        GLRender::SetActiveTexture(0);
    }
#else
    if (AurCubeMapAvailable() != 0 && pTexture->IsCubeMap())
    {
        GLRender::cm_uiCurrentStage = nStage;
        if (AurMultiTextureAvailable() != 0)
        {
            glActiveTextureARB(GLRender::cm_uiCurrentStage + 33984);
        }

        glMatrixMode(transformTranslationTable[3]);
        GLRender_PushCurrentMatrix();
        if (CurrentCamera != 0)
        {
            MakeMatrix4x4(m, g_vZero, CurrentCamera->m_qOrientation);
        }
        else
        {
            Quaternion qOrientation;
            MakeMatrix4x4(m, g_vZero, qOrientation);
        }

        glMultMatrixf(m);
        glMatrixMode(transformTranslationTable[2]);
        GLRender::cm_uiCurrentStage = 0;
        if (AurMultiTextureAvailable() != 0)
        {
            glActiveTextureARB(GLRender::cm_uiCurrentStage + 33984);
        }
    }
#endif
}

void UndoEnvMapTransform(CAurTexture* pTexture, unsigned long nStage)
{
#ifdef _XBOX

    if (pTexture->IsCubeMap())
    {
        GLRender::SetActiveTexture(nStage);
        GLRender::SetMatrixMode(3);
        GLRender::PopCurrentMatrix();
        GLRender::ApplyMatrix(3);
        GLRender::SetMatrixMode(2);
        GLRender::SetActiveTexture(0);
    }
#else
    if (AurCubeMapAvailable() != 0 && pTexture->IsCubeMap())
    {
        GLRender::cm_uiCurrentStage = nStage;
        if (AurMultiTextureAvailable() != 0)
        {
            glActiveTextureARB(GLRender::cm_uiCurrentStage + 33984);
        }

        glMatrixMode(transformTranslationTable[3]);
        GLRender_PopCurrentMatrix();
        glMatrixMode(transformTranslationTable[2]);

        GLRender::cm_uiCurrentStage = 0;
        if (AurMultiTextureAvailable() != 0)
        {
            glActiveTextureARB(GLRender::cm_uiCurrentStage + 33984);
        }
    }
#endif
}

// PC 0x00486960 GOG 0x00486A20
void SetupForceSightShader(Gob* pGob, int nForceShader, int nShader,
                           CAurTexture* pTexture0, CAurTexture* pTexture1,
                           CAurTexture* pTexture2, CAurTexture* pTexture3,
                           bool bKeepBlend)
{
    bool bNonForceBlending = false;
    bool bIllumination = false;
    float afColor[4];
    if (pGob)
    {
        bool bForceGlow = pGob->IsForceGlowEnabled();
        bNonForceBlending = pGob->IsNonForceBlending();
        pGob->GetForcePoints();
        bIllumination = pGob->IsIlluminationEnable();
        if (bForceGlow)
        {
            GLRender_SetBlendFunc(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);
            unsigned char nPhase = (unsigned char)(g_SceneRender_CurrentTime_ms * 0.125f);
            float fPulse = (float)sin(nPhase * AUR_FORCE_GLOW_PULSE_STEP) * 0.25f + 0.75f;
            {
                float fRed;
                float fGreen;
                float fBlue;
                float fAlpha;
                pGob->GetForceGlowColor(fRed, fGreen, fBlue, fAlpha);
                afColor[0] = fRed;
                afColor[1] = fGreen;
                afColor[2] = fBlue;
                afColor[3] = fAlpha * fPulse;
            }
            memcpy(GLRender::m_ForceSightColor, afColor, sizeof(afColor));
            GLRender_EnablePixelShaders(nForceShader, pTexture0, pTexture1, pTexture2, pTexture3);
            return;
        }
        if (bNonForceBlending && !bKeepBlend)
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    }
    afColor[0] = 0.0f;
    afColor[1] = 0.0f;
    afColor[2] = 0.0f;
    afColor[3] = 0.75f;
    if (!bNonForceBlending)
        afColor[3] = 1.0f;
    memcpy(GLRender::m_ForceSightColor, afColor, sizeof(afColor));
    if (bIllumination)
    {
        int nIlluminationShader = nShader + (MODEL_SHADER_ILLUM_DESAT - MODEL_SHADER_DESAT);
        if (nIlluminationShader >= MODEL_SHADER_ILLUM_DESAT_NOISE_MOD_EMLM + 1)
            nIlluminationShader = MODEL_SHADER_ILLUM_DESAT;
        afColor[0] = 0.0f;
        afColor[1] = 0.0f;
        afColor[2] = 0.0f;
        afColor[3] = 1.0f;
        pGob->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
        memcpy(GLRender::m_PerInstanceIllumination, afColor, sizeof(afColor));
        GLRender_EnablePixelShaders(nIlluminationShader, pTexture0, pTexture1, pTexture2, pTexture3);
    }
    else
    {
        GLRender_EnablePixelShaders(nShader, pTexture0, pTexture1, pTexture2, pTexture3);
    }
}

void RenderFlat(VertexPrimitiveFlat* pPrimitive)
{
    if (enablerender == 0)
    {
        return;
    }

#ifndef _XBOX
    if (enablelighting == 0)
    {
        glDisable(GL_LIGHTING);
        glDisableClientState(GL_COLOR_ARRAY);
        GLRender_SetColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
#endif

    ++countparts;
    MdlNodeTriMesh* pNode = pPrimitive->m_pNode;
    int nBatches = pNode->m_lstIndexCounts.m_nCount;
#ifdef _XBOX
    if (pNode->m_nTextureCount < 1)
#else
    if (pNode->m_nTexCoord0Offset < 0)
#endif
    {
        GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
    }

    if (nBatches != 0)
    {
        for (int nBatch = 0; nBatch < nBatches; ++nBatch)
        {
            int nIndices = pNode->m_lstIndexCounts.m_pList[nBatch];
            GLRender::DrawElements((Aurora::AuroraPrimitiveTypes)pNode->m_nPrimitiveType, nIndices, 0,
                                   pNode->m_lstIndexPools.m_pList[nBatch]);
            countfaces += nIndices / 3;
        }
    }

    if (pNode->m_nPrimitiveType == Aurora::PRIMITIVE_TRIANGLE_STRIP)
    {
        if (pNode->m_nAuxIndexCount > 0)
        {
            GLRender::DrawElements(Aurora::PRIMITIVE_TRIANGLES,
                                   pNode->m_nAuxIndexCount,
                                   pNode->m_nVertexDataOffset,
                                   pNode->m_nAuxIndexPool);
        }
    }

#ifdef _XBOX
    if (pNode->m_nTextureCount < 1)
#else
    if (pNode->m_nTexCoord0Offset < 0)
#endif
    {
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    }
#ifndef _XBOX
    if (enablelighting == 0)
    {
        glEnable(GL_LIGHTING);
        glEnableClientState(GL_COLOR_ARRAY);
    }
#endif
}

void RenderLightMapped(VertexPrimitiveFlat* pPrimitive)
{
#ifndef _XBOX
    if (enablerenderlightmapped == 0)
    {
        return;
    }
#endif

    if (currentscene == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }

    PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
    if (((CAurTexture*)pPart->m_pMaterial->m_pTexture0.m_pObject)->IsNull() !=
        0)
    {
        RenderFlat(pPrimitive);
        return;
    }

#ifndef _XBOX
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
#endif
    GLRender::ActivateTextureStage(1);
    GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    GLRender::ActivateTextureStage(0);
    if (g_ForceSight)
    {
        SetupForceSightShader(pPart->m_pAurObject, MODEL_SHADER_FGLOW_LM, MODEL_SHADER_DESAT_LM,
                              pPart->m_pMaterial->m_pTexture0.m_pObject,
                              pPart->m_pMaterial->m_pTexture1.m_pObject, 0, 0, false);
        RenderFlat(pPrimitive);
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
    }
    else
    {
        GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_LIGHTMAP, pPart->m_pMaterial->m_pTexture0.m_pObject,
                                   pPart->m_pMaterial->m_pTexture1.m_pObject, 0, 0);
        RenderFlat(pPrimitive);
        GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_DISABLE, 0, 0, 0, 0);
    }
    GLRender::ActivateTextureStage(1);
    GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
    GLRender::ActivateTextureStage(0);
#ifdef _XBOX

    GLRender::Disable(ENABLE_VERTEX_PROGRAM);
#endif
}

#ifndef _XBOX

void RenderLightMappedNoShaders(VertexPrimitiveFlat* pPrimitive)
{
    if (enablenoshaderlightmaps == 0)
    {
        return;
    }

    PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
    MdlNodeTriMesh* pNode = pPart->m_pNode->AsMdlNodeTriMesh();
    unsigned long nOffset;
    int nCount;
    unsigned long nPool;
    if (((CAurTexture*)pPart->m_pMaterial->m_pTexture0.m_pObject)->IsNull() ||
        ((CAurTexture*)pPart->m_pMaterial->m_pTexture1.m_pObject)->IsNull() ||
        currentscene == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }

    if (AurMultiTextureAvailable() && !pPart->m_pMaterial->HasEnvMap())
    {
        GLRender_SetColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        GLRender_Disable(ENABLE_LIGHTING);
        SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        GLRender::SetActiveTexture(0);
        GLRender_Enable(ENABLE_TEXTURE_2D);
        pPart->m_pMaterial->BindTexture1();
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        GLRender::SetActiveTexture(1);
        GLRender_Enable(ENABLE_TEXTURE_2D);
        pPart->m_pMaterial->BindTexture0();
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        GLRender::SetActiveTexture(0);
        GLRender::ActivateTextureStage(1);
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
        nOffset = pNode->m_nVertexDataOffset;
        nCount = pNode->m_nVertexCount + 1;
        nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
        GLRender::SetLightmapTexCoordToStage0(pNode->m_nVertexStride,
            pNode->m_nVertexArrayFlags, nPool, nCount, pNode, nOffset);
        RenderFlat(pPrimitive);
        nOffset = pNode->m_nVertexDataOffset;
        nCount = pNode->m_nVertexCount + 1;
        nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
        GLRender::SetDecalTexCoordToStage0(pNode->m_nVertexStride,
            pNode->m_nVertexArrayFlags, nPool, nCount, pNode, nOffset);
        GLRender::ActivateTextureStage(1);
        GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender::ActivateTextureStage(0);
        GLRender::SetActiveTexture(1);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        GLRender_Disable(ENABLE_TEXTURE_2D);
        GLRender::SetActiveTexture(0);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        pPart->m_pMaterial->BindTexture0();
        SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        GLRender_SetColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        GLRender_Enable(ENABLE_LIGHTING);
        return;
    }

    GLRender_SetColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    GLRender_Disable(ENABLE_LIGHTING);
    pPart->m_pMaterial->BindTexture0();
    SetBlendMode(BLEND_ONE, BLEND_ZERO);
    RenderFlat(pPrimitive);
    pPart->m_pMaterial->BindTexture1();
    SetBlendMode(BLEND_DST_COLOR, BLEND_ZERO);
    nOffset = pNode->m_nVertexDataOffset;
    nCount = pNode->m_nVertexCount + 1;
    nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
    GLRender::SetLightmapTexCoordToStage0(pNode->m_nVertexStride,
        pNode->m_nVertexArrayFlags, nPool, nCount, pNode, nOffset);
    RenderFlat(pPrimitive);
    nOffset = pNode->m_nVertexDataOffset;
    nCount = pNode->m_nVertexCount + 1;
    nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
    GLRender::SetDecalTexCoordToStage0(pNode->m_nVertexStride,
        pNode->m_nVertexArrayFlags, nPool, nCount, pNode, nOffset);
    if (pPart->m_pMaterial->HasEnvMap() && AurCubeMapAvailable())
    {
        GLRender_Disable(ENABLE_ALPHA_TEST);
        pPart->m_pMaterial->BindTexture0();
        SetBlendMode(BLEND_ZERO, BLEND_ONE);
        RenderFlat(pPrimitive);
        pPart->m_pMaterial->BindEnvMap();
        SetBlendMode(BLEND_ONE_MINUS_DST_ALPHA, BLEND_ONE);
        DoEnvMapTransform(
            (CAurTexture*)pPart->m_pMaterial->m_pEnvMap.m_pObject, 0);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
        GLRender_Enable(ENABLE_TEXTURE_GEN_S);
        GLRender_Enable(ENABLE_TEXTURE_GEN_T);
        GLRender_Enable(ENABLE_TEXTURE_GEN_R);
        RenderFlat(pPrimitive);
        UndoEnvMapTransform(
            (CAurTexture*)pPart->m_pMaterial->m_pEnvMap.m_pObject, 0);
        GLRender_Disable(ENABLE_TEXTURE_GEN_S);
        GLRender_Disable(ENABLE_TEXTURE_GEN_T);
        GLRender_Disable(ENABLE_TEXTURE_GEN_R);
        GLRender_Enable(ENABLE_ALPHA_TEST);
    }
    GLRender_SetColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    GLRender_Enable(ENABLE_LIGHTING);
    SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
}
#endif

#ifndef _XBOX
// PC 0x00487280 GOG 0x00487340
void RenderNoiseMapped(VertexPrimitiveFlat* pPrimitive, int nTexture, int nCoordSpace)
{
    PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
    Gob* pGob = pPart->m_pAurObject;
    Material* pMaterial = pPart->m_pMaterial;
    DirtyTextureController* pDirty =
        (DirtyTextureController*)GLRender::m_pDirtyTexture[nTexture]->GetReferencedTexture()->GetController();
    int nBlendMode;
    if (pDirty)
        nBlendMode = pDirty->m_nBlendMode;
    else
        nBlendMode = 0;
    if ((pGob && pGob->m_pEnvironmentMap) || pMaterial->HasEnvMap())
    {
        if (pPart->m_pNode->AsMdlNodeTriMesh()->m_bLightMapped && enablelightmap)
        {
            static VertexProgram* world_DirtyVP = new VertexProgram(
                usearbvertexprograms ? szcStandardDirtyEMLMVPARB : cStandardDirtyEMLMVP,
                cStandardVP2Declaration);
            static VertexProgram* local_DirtyVP = new VertexProgram(
                usearbvertexprograms ? szszcStandardDirtyLSEMLMVPARB : cStandardDirtyLSEMLMVP,
                cStandardVP2Declaration);
            VertexProgram* pDirtyVP = nCoordSpace == 1 ? world_DirtyVP : local_DirtyVP;
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
            pDirtyVP->Enable(pPart, true);
            GLRender::ActivateTextureStage(1);
            GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender::ActivateTextureStage(0);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_DIRTY_SELECT, 1.0f, 0.0f, 1.0f, 0.0f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_DIRTY_CONSTANTS, 0.0f, 2.0f, 3.0f, 1.0f);
            if (g_ForceSight)
            {
                SetupForceSightShader(pGob, MODEL_SHADER_DESAT_NOISE_SUB_EMLM + nBlendMode,
                                      MODEL_SHADER_DESAT_NOISE_SUB_EMLM + nBlendMode,
                                      pPart->m_pMaterial->m_pTexture0.m_pObject,
                                      pPart->m_pMaterial->m_pTexture1.m_pObject,
                                      pPart->m_pMaterial->m_pEnvMap.m_pObject,
                                      GLRender::m_pDirtyTexture[nTexture], false);
            }
            else if (pGob && pGob->IsIlluminationEnable())
            {
                float afColor[4];
                afColor[0] = 0.0f;
                afColor[1] = 0.0f;
                afColor[2] = 0.0f;
                afColor[3] = 1.0f;
                pGob->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
                SetPerInstanceIllumination(afColor);
                GLRender_EnablePixelShaders(MODEL_SHADER_ILLUM_NOISE_EMLM_SUBTRACT + nBlendMode,
                                            pPart->m_pMaterial->m_pTexture0.m_pObject,
                                            pPart->m_pMaterial->m_pTexture1.m_pObject,
                                            pPart->m_pMaterial->m_pEnvMap.m_pObject,
                                            GLRender::m_pDirtyTexture[nTexture]);
            }
            else
            {
                GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_EMLM_SUBTRACT + nBlendMode,
                                            pPart->m_pMaterial->m_pTexture0.m_pObject,
                                            pPart->m_pMaterial->m_pTexture1.m_pObject,
                                            pPart->m_pMaterial->m_pEnvMap.m_pObject,
                                            GLRender::m_pDirtyTexture[nTexture]);
            }
        }
        else
        {
            static VertexProgram* world_DirtyVP = new VertexProgram(
                usearbvertexprograms ? szcStandardDirtyEMVPARB : cStandardDirtyEMVP,
                cStandardVP2Declaration);
            static VertexProgram* local_DirtyVP = new VertexProgram(
                usearbvertexprograms ? szcStandardDirtyLSEMVPARB : cStandardDirtyLSEMVP,
                cStandardVP2Declaration);
            VertexProgram* pDirtyVP = nCoordSpace == 1 ? world_DirtyVP : local_DirtyVP;
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
            pDirtyVP->Enable(pPart, true);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_DIRTY_SELECT, 1.0f, 0.0f, 1.0f, 0.0f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_DIRTY_CONSTANTS, 0.0f, 2.0f, 3.0f, 1.0f);
            if (pGob && pGob->m_pEnvironmentMap.m_pObject)
            {
                if (g_ForceSight)
                {
                    SetupForceSightShader(pGob, MODEL_SHADER_DESAT_NOISE_SUB_EM + nBlendMode,
                                          MODEL_SHADER_DESAT_NOISE_SUB_EM + nBlendMode,
                                          pPart->m_pMaterial->m_pTexture0.m_pObject,
                                          pGob->m_pEnvironmentMap.m_pObject,
                                          GLRender::m_pDirtyTexture[nTexture], 0, false);
                }
                else if (pGob->IsIlluminationEnable())
                {
                    float afColor[4];
                    afColor[0] = 0.0f;
                    afColor[1] = 0.0f;
                    afColor[2] = 0.0f;
                    afColor[3] = 1.0f;
                    pGob->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
                    SetPerInstanceIllumination(afColor);
                    GLRender_EnablePixelShaders(MODEL_SHADER_ILLUM_NOISE_EM_SUBTRACT + nBlendMode,
                                                pPart->m_pMaterial->m_pTexture0.m_pObject,
                                                pGob->m_pEnvironmentMap.m_pObject,
                                                GLRender::m_pDirtyTexture[nTexture], 0);
                }
                else
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_EM_SUBTRACT + nBlendMode,
                                                pPart->m_pMaterial->m_pTexture0.m_pObject,
                                                pGob->m_pEnvironmentMap.m_pObject,
                                                GLRender::m_pDirtyTexture[nTexture], 0);
                }
            }
            else if (g_ForceSight)
            {
                SetupForceSightShader(pGob, MODEL_SHADER_DESAT_NOISE_SUB_EM + nBlendMode,
                                      MODEL_SHADER_DESAT_NOISE_SUB_EM + nBlendMode,
                                      pPart->m_pMaterial->m_pTexture0.m_pObject,
                                      pPart->m_pMaterial->m_pEnvMap.m_pObject,
                                      GLRender::m_pDirtyTexture[nTexture], 0, false);
            }
            else if (pGob && pGob->IsIlluminationEnable())
            {
                float afColor[4];
                afColor[0] = 0.0f;
                afColor[1] = 0.0f;
                afColor[2] = 0.0f;
                afColor[3] = 1.0f;
                pGob->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
                SetPerInstanceIllumination(afColor);
                GLRender_EnablePixelShaders(MODEL_SHADER_ILLUM_NOISE_EM_SUBTRACT + nBlendMode,
                                            pPart->m_pMaterial->m_pTexture0.m_pObject,
                                            pPart->m_pMaterial->m_pEnvMap.m_pObject,
                                            GLRender::m_pDirtyTexture[nTexture], 0);
            }
            else
            {
                GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_EM_SUBTRACT + nBlendMode,
                                            pPart->m_pMaterial->m_pTexture0.m_pObject,
                                            pPart->m_pMaterial->m_pEnvMap.m_pObject,
                                            GLRender::m_pDirtyTexture[nTexture], 0);
            }
        }
    }
    else if (pPart->m_pNode->AsMdlNodeTriMesh()->m_bLightMapped && enablelightmap)
    {
        static VertexProgram* world_DirtyVP = new VertexProgram(
            usearbvertexprograms ? szcStandardDirtyLMVPARB : cStandardDirtyLMVP,
            cStandardVP2Declaration);
        static VertexProgram* local_DirtyVP = new VertexProgram(
            usearbvertexprograms ? szcStandardDirtyLSLMVPARB : cStandardDirtyLSLMVP,
            cStandardVP2Declaration);
        VertexProgram* pDirtyVP = nCoordSpace == 1 ? world_DirtyVP : local_DirtyVP;
        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        pDirtyVP->Enable(pPart, true);
        GLRender::ActivateTextureStage(1);
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender::ActivateTextureStage(0);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_DIRTY_SELECT, 1.0f, 0.0f, 1.0f, 0.0f);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_DIRTY_CONSTANTS, 0.0f, 2.0f, 3.0f, 1.0f);
        if (g_ForceSight)
        {
            SetupForceSightShader(pGob, MODEL_SHADER_DESAT_NOISE_SUB_LM + nBlendMode,
                                  MODEL_SHADER_DESAT_NOISE_SUB_LM + nBlendMode,
                                  pPart->m_pMaterial->m_pTexture0.m_pObject,
                                  pPart->m_pMaterial->m_pTexture1.m_pObject,
                                  GLRender::m_pDirtyTexture[nTexture], 0, false);
        }
        else if (pGob && pGob->IsIlluminationEnable())
        {
            float afColor[4];
            afColor[0] = 0.0f;
            afColor[1] = 0.0f;
            afColor[2] = 0.0f;
            afColor[3] = 1.0f;
            pGob->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
            SetPerInstanceIllumination(afColor);
            GLRender_EnablePixelShaders(MODEL_SHADER_ILLUM_NOISE_LM_SUBTRACT + nBlendMode,
                                        pPart->m_pMaterial->m_pTexture0.m_pObject,
                                        pPart->m_pMaterial->m_pTexture1.m_pObject,
                                        GLRender::m_pDirtyTexture[nTexture], 0);
        }
        else
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_LM_SUBTRACT + nBlendMode,
                                        pPart->m_pMaterial->m_pTexture0.m_pObject,
                                        pPart->m_pMaterial->m_pTexture1.m_pObject,
                                        GLRender::m_pDirtyTexture[nTexture], 0);
        }
    }
    else
    {
        static VertexProgram* world_DirtyVP = new VertexProgram(
            usearbvertexprograms ? szcStandardDirtyVPARB : cStandardDirtyVP,
            cStandardVPDeclaration);
        static VertexProgram* local_DirtyVP = new VertexProgram(
            usearbvertexprograms ? szcStandardDirtyLSVPARB : cStandardDirtyLSVP,
            cStandardVPDeclaration);
        VertexProgram* pDirtyVP = nCoordSpace == 1 ? world_DirtyVP : local_DirtyVP;
        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        pDirtyVP->Enable(pPart, true);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_DIRTY_SELECT, 1.0f, 0.0f, 1.0f, 0.0f);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_DIRTY_CONSTANTS, 0.0f, 2.0f, 3.0f, 1.0f);
        if (g_ForceSight)
        {
            SetupForceSightShader(pGob, MODEL_SHADER_DESAT_NOISE_SUB + nBlendMode,
                                  MODEL_SHADER_DESAT_NOISE_SUB + nBlendMode,
                                  pPart->m_pMaterial->m_pTexture0.m_pObject,
                                  GLRender::m_pDirtyTexture[nTexture], 0, 0, false);
        }
        else if (pGob && pGob->IsIlluminationEnable())
        {
            float afColor[4];
            afColor[0] = 0.0f;
            afColor[1] = 0.0f;
            afColor[2] = 0.0f;
            afColor[3] = 1.0f;
            pGob->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
            SetPerInstanceIllumination(afColor);
            GLRender_EnablePixelShaders(MODEL_SHADER_ILLUM_NOISE_SUBTRACT + nBlendMode,
                                        pPart->m_pMaterial->m_pTexture0.m_pObject,
                                        GLRender::m_pDirtyTexture[nTexture], 0, 0);
        }
        else
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_SUBTRACT + nBlendMode,
                                        pPart->m_pMaterial->m_pTexture0.m_pObject,
                                        GLRender::m_pDirtyTexture[nTexture], 0, 0);
        }
    }
    AurSetVertexProgramParameter4f(VertexProgram::PARAM_DIRTY_OCTAVES, 0.5f, 0.25f, 0.125f, 0.0675f);
    RenderFlat(pPrimitive);
    GLRender_Disable(ENABLE_VERTEX_PROGRAM);
    GLRender::ActivateTextureStage(1);
    GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
    GLRender::ActivateTextureStage(0);
    if (g_ForceSight)
        GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
    else
        GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_DISABLE, 0, 0, 0, 0);
}
#endif

#ifdef _XBOX

static inline void XbBindTexture(Aurora::AuroraEnableEnums nTarget, unsigned long nTexture)
{
    if (nTexture != 0)
    {
        GLRender::BindTexture(nTarget, nTexture);
        return;
    }
    g_pCurrentTextureState = NULL;
    g_pCurrentTexture = NULL;
    g_nCurrentTexture = 0;
    GLRender::cm_pCurrentTexture[GLRender::cm_uiCurrentStage] = NULL;
    D3DDevice_SetTexture(GLRender::cm_uiCurrentStage, NULL);
    GLRender::SetTextureStageFlag(GLRender::cm_uiCurrentStage, 0);
}

static inline void XbSetTextureCoordGenerationState(Aurora::AuroraTexCoordMappingModes nMode)
{
    if (nMode == Aurora::TEXCOORD_MAPPING_REFLECTION)
    {
        g_pD3DDevice->SetTextureStageState(GLRender::cm_uiCurrentStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
        g_pD3DDevice->SetTextureStageState(GLRender::cm_uiCurrentStage, D3DTSS_TEXCOORDINDEX,
            GLRender::cm_uiCurrentStage | D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
    }
    else if (nMode == Aurora::TEXCOORD_MAPPING_SPHERE)
    {
        g_pD3DDevice->SetTextureStageState(GLRender::cm_uiCurrentStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
        g_pD3DDevice->SetTextureStageState(GLRender::cm_uiCurrentStage, D3DTSS_TEXCOORDINDEX,
            GLRender::cm_uiCurrentStage | D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
    }
    GLRender::Enable(ENABLE_TEXTURE_GEN_S);
    GLRender::Enable(ENABLE_TEXTURE_GEN_T);
    if (nMode == Aurora::TEXCOORD_MAPPING_REFLECTION)
    {
        GLRender::Enable(ENABLE_TEXTURE_GEN_R);
    }
}

static inline void XbSetPixelShaderAmbientColor(Vector& vColor, float fAlpha)
{
    float fW = fAlpha;
    g_pD3DDevice->SetPixelShaderConstant(0, Homogenize(vColor, fW), 1);
}

void RenderEnvironmentMapped(VertexPrimitiveFlat* pPrimitive, bool bFog)
#else
void RenderEnvironmentMapped(VertexPrimitiveFlat* pPrimitive, bool,
                             bool bFog)
#endif
{
    if (enablerenderenv == 0 || enablerender == 0)
    {
        return;
    }

    PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
    Gob* pGob = pPart->m_pAurObject;
    if (enableenvmap == 0 || AurTextureEnvMapDisabled())
    {
        int nFormat = ((CAurTexture*)pPart->m_pMaterial->m_pTexture0.m_pObject)->GetCompressionFormat();
        if (nFormat != Aurora::TEXTURE_FORMAT_DXT1 &&
            nFormat != Aurora::TEXTURE_FORMAT_DXT5)
        {
            RenderFlat(pPrimitive);
            return;
        }
    }

    float fAlpha = pPart->m_fAlpha;
    CAurTexture* pEnvMap = 0;
    if (pGob)
    {
        pEnvMap = (CAurTexture*)pGob->m_pEnvironmentMap.m_pObject;
        if (pGob->m_bColorShifting)
        {
            fAlpha *= pGob->m_fAlpha;
        }
    }
    if (!pEnvMap)
    {
        pEnvMap = (CAurTexture*)pPart->m_pMaterial->m_pEnvMap.m_pObject;
    }

    if (enableonepassenvmap != 0 && AurOnePassEnvMapAvailable())
    {
        GLRender_Disable(ENABLE_ALPHA_TEST);
        if (bFog && AurATIValidate())
        {
            if (g_ForceSight)
            {
                SetupForceSightShader(pGob, MODEL_SHADER_FGLOW_EM, MODEL_SHADER_DESAT_EM,
                                      pPart->m_pMaterial->m_pTexture0.m_pObject, pEnvMap, 0, 0, false);
            }
            else if (pGob && pGob->IsIlluminationEnable())
            {
                float afColor[4];
                afColor[0] = 0.0f;
                afColor[1] = 0.0f;
                afColor[2] = 0.0f;
                afColor[3] = 1.0f;
                pGob->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
                SetPerInstanceIllumination(afColor);
                GLRender_EnablePixelShaders(MODEL_SHADER_ILLUM_ENV,
                    pPart->m_pMaterial->m_pTexture0.m_pObject, pEnvMap, 0, 0);
            }
            else
            {
                GLRender_EnablePixelShaders(MODEL_SHADER_ATI_ENVIRONMENT_FOG,
                    pPart->m_pMaterial->m_pTexture0.m_pObject, pEnvMap, 0, 0);
            }
        }
        else
        {
            if (g_ForceSight)
            {
                SetupForceSightShader(pGob, MODEL_SHADER_FGLOW_EM, MODEL_SHADER_DESAT_EM,
                                      pPart->m_pMaterial->m_pTexture0.m_pObject, pEnvMap, 0, 0, false);
            }
            else if (pGob && pGob->IsIlluminationEnable())
            {
                float afColor[4];
                afColor[0] = 0.0f;
                afColor[1] = 0.0f;
                afColor[2] = 0.0f;
                afColor[3] = 1.0f;
                pGob->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
                SetPerInstanceIllumination(afColor);
                GLRender_EnablePixelShaders(MODEL_SHADER_ILLUM_ENV,
                    pPart->m_pMaterial->m_pTexture0.m_pObject, pEnvMap, 0, 0);
            }
            else
            {
                GLRender_EnablePixelShaders(MODEL_SHADER_ENVIRONMENT,
                    pPart->m_pMaterial->m_pTexture0.m_pObject, pEnvMap, 0, 0);
            }
        }
        DoEnvMapTransform(pEnvMap, 1);
        Vector vColor(1.0f, 1.0f, 1.0f);
#ifdef _XBOX
        XbSetPixelShaderAmbientColor(vColor, fAlpha);
#else
        if (!g_ForceSight)
            GLRender_SetPixelShaderAmbientColor(vColor, fAlpha);
#endif
        RenderFlat(pPrimitive);
        if (g_ForceSight)
            GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
        else
            GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_DISABLE, 0, 0, 0, 0);
        UndoEnvMapTransform(pEnvMap, 1);
        GLRender::SetActiveTexture(1);
        if (AurCubeMapAvailable())
        {
            GLRender_Disable(ENABLE_TEXTURE_CUBE_MAP);
        }
        GLRender::SetActiveTexture(0);
        GLRender_Enable(ENABLE_ALPHA_TEST);
        return;
    }

    GLRender_Disable(ENABLE_ALPHA_TEST);
    if (environmentmapdest != 0)
    {
        GLRender_SetBlendFunc(BLEND_ONE,
                    BLEND_ZERO);
        RenderFlat(pPrimitive);
        GLRender_SetBlendFunc(BLEND_ONE_MINUS_DST_ALPHA,
                    BLEND_ONE);
        DoEnvMapTransform(pEnvMap, 0);
        pEnvMap->Bind();
        if (pEnvMap->IsCubeMap())
        {
#ifdef _XBOX
            XbSetTextureCoordGenerationState(Aurora::TEXCOORD_MAPPING_REFLECTION);
#else
            GLRender::SetTextureCoordGenerationState(
                Aurora::TEXCOORD_MAPPING_REFLECTION);
#endif
        }
        else
        {
#ifdef _XBOX
            XbSetTextureCoordGenerationState(Aurora::TEXCOORD_MAPPING_SPHERE);
#else
            GLRender::SetTextureCoordGenerationState(
                Aurora::TEXCOORD_MAPPING_SPHERE);
#endif
        }
        UndoEnvMapTransform(pEnvMap, 0);
        RenderFlat(pPrimitive);
        if (pPart->m_pAurObject == 0)
        {
            pPart->m_pMaterial->BindTexture0();
        }
    }
    else
    {
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                    BLEND_ONE_MINUS_SRC_ALPHA);
        DoEnvMapTransform(pEnvMap, 0);
        pEnvMap->Bind();
        if (pEnvMap->IsCubeMap())
        {
#ifdef _XBOX
            XbSetTextureCoordGenerationState(Aurora::TEXCOORD_MAPPING_REFLECTION);
#else
            GLRender::SetTextureCoordGenerationState(
                Aurora::TEXCOORD_MAPPING_REFLECTION);
#endif
        }
        else
        {
#ifdef _XBOX
            XbSetTextureCoordGenerationState(Aurora::TEXCOORD_MAPPING_SPHERE);
#else
            GLRender::SetTextureCoordGenerationState(
                Aurora::TEXCOORD_MAPPING_SPHERE);
#endif
        }
        RenderFlat(pPrimitive);
        UndoEnvMapTransform(pEnvMap, 0);
        GLRender_Disable(ENABLE_TEXTURE_GEN_S);
        GLRender_Disable(ENABLE_TEXTURE_GEN_T);
        GLRender_Disable(ENABLE_TEXTURE_GEN_R);
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                    BLEND_ONE_MINUS_SRC_ALPHA);
        pPart->m_pMaterial->BindTexture0();
        RenderFlat(pPrimitive);
    }

#ifdef _XBOX
    XbBindTexture(ENABLE_TEXTURE_CUBE_MAP, 0);
#else
    if (AurCubeMapAvailable())
    {
        GLRender::BindTexture(ENABLE_TEXTURE_CUBE_MAP, 0);
    }
#endif
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                BLEND_ONE_MINUS_SRC_ALPHA);
    GLRender_Enable(ENABLE_ALPHA_TEST);
    GLRender::SetActiveTexture(1);
    GLRender_Disable(ENABLE_TEXTURE_GEN_S);
    GLRender_Disable(ENABLE_TEXTURE_GEN_T);
    GLRender_Disable(ENABLE_TEXTURE_GEN_R);
    if (AurCubeMapAvailable())
    {
        GLRender_Disable(ENABLE_TEXTURE_CUBE_MAP);
    }
    GLRender::SetActiveTexture(0);
}

void RenderEMLM(VertexPrimitiveFlat* pPrimitive)
{
    if (renderemlm == 0 || enablerenderenv == 0 || enablerender == 0 ||
        !AurNV2xLevelShadersAvailable())
    {
        return;
    }

    PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
    Gob* pGob = pPart->m_pAurObject;
    if (enableenvmap == 0 || AurTextureEnvMapDisabled())
    {
        int nFormat = ((CAurTexture*)pPart->m_pMaterial->m_pTexture0.m_pObject)->GetCompressionFormat();
        if (nFormat != Aurora::TEXTURE_FORMAT_DXT1 &&
            nFormat != Aurora::TEXTURE_FORMAT_DXT5)
        {
            RenderFlat(pPrimitive);
            return;
        }
    }

    float fAlpha = pPart->m_fAlpha;
    CAurTexture* pEnvMap = 0;
    if (pGob)
    {
        pEnvMap = (CAurTexture*)pGob->m_pEnvironmentMap.m_pObject;
        if (pGob->m_bColorShifting)
        {
            fAlpha *= pGob->m_fAlpha;
        }
    }
    if (!pEnvMap)
    {
        pEnvMap = (CAurTexture*)pPart->m_pMaterial->m_pEnvMap.m_pObject;
    }

#ifdef _XBOX
    if (enableonepassenvmap == 0)
    {
        return;
    }
#endif
    GLRender_Disable(ENABLE_ALPHA_TEST);
    GLRender::ActivateTextureStage(1);
    GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    GLRender::ActivateTextureStage(0);
    if (g_ForceSight)
        SetupForceSightShader(pGob, MODEL_SHADER_FGLOW_EMLM, MODEL_SHADER_DESAT_EMLM,
                              pPart->m_pMaterial->m_pTexture0.m_pObject,
                              pPart->m_pMaterial->m_pTexture1.m_pObject, pEnvMap, 0, false);
    else
        GLRender_EnablePixelShaders(MODEL_SHADER_EMLM,
            pPart->m_pMaterial->m_pTexture0.m_pObject,
            pPart->m_pMaterial->m_pTexture1.m_pObject, pEnvMap, 0);
    DoEnvMapTransform(pEnvMap, 2);
    Vector vColor(1.0f, 1.0f, 1.0f);
    if (!g_ForceSight)
        GLRender_SetPixelShaderAmbientColor(vColor, fAlpha);
    RenderFlat(pPrimitive);
    UndoEnvMapTransform(pEnvMap, 2);
    if (g_ForceSight)
        GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
    else
        GLRender_EnablePixelShaders(MODEL_SHADER_EMLM_DISABLE, 0, 0, 0, 0);
    GLRender::ActivateTextureStage(1);
    GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
    GLRender::ActivateTextureStage(0);
    GLRender_Enable(ENABLE_ALPHA_TEST);
#ifdef _XBOX
    GLRender::SetActiveTexture(2);
#else
    GLRender::SetActiveTexture(1);
#endif
    GLRender_Disable(ENABLE_TEXTURE_CUBE_MAP);
    GLRender::SetActiveTexture(0);
}

#ifndef _XBOX
void RenderNV1xEMLM(VertexPrimitiveFlat* pPrimitive)
{
    if (!AurNV1xLevelShadersAvailable())
        return;

    PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
    if (pPart->m_pAurObject)
        return;

    if (currentscene &&
        !((CAurTexture*)pPart->m_pMaterial->m_pTexture0.m_pObject)->IsNull() &&
        !((CAurTexture*)pPart->m_pMaterial->m_pTexture1.m_pObject)->IsNull() &&
        !((CAurTexture*)pPart->m_pMaterial->m_pEnvMap.m_pObject)->IsNull())
    {
        GLRender_Disable(ENABLE_ALPHA_TEST);
        GLRender_Disable(ENABLE_VERTEX_PROGRAM);
        SetBlendMode(BLEND_ONE, BLEND_ZERO);
        GLRender::ActivateTextureStage(1);
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender::ActivateTextureStage(0);
        GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_LIGHTMAP,
            pPart->m_pMaterial->m_pTexture0.m_pObject,
            pPart->m_pMaterial->m_pTexture1.m_pObject, 0, 0);
        RenderFlat(pPrimitive);
        GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_DISABLE, 0, 0, 0, 0);
        GLRender::ActivateTextureStage(1);
        GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender::ActivateTextureStage(0);
        SetBlendMode(BLEND_ONE, BLEND_ONE);
        GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_ENVIRONMENT,
            pPart->m_pMaterial->m_pTexture0.m_pObject,
            pPart->m_pMaterial->m_pEnvMap.m_pObject, 0, 0);
        DoEnvMapTransform((CAurTexture*)pPart->m_pMaterial->m_pEnvMap.m_pObject, 1);
        Vector vColor(1.0f, 1.0f, 1.0f);
        GLRender_SetPixelShaderAmbientColor(vColor, 1.0f);
        RenderFlat(pPrimitive);
        GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_DISABLE, 0, 0, 0, 0);
        GLRender_Disable(ENABLE_TEXTURE_GEN_S);
        GLRender_Disable(ENABLE_TEXTURE_GEN_T);
        GLRender_Disable(ENABLE_TEXTURE_GEN_R);
        UndoEnvMapTransform((CAurTexture*)pPart->m_pMaterial->m_pEnvMap.m_pObject, 1);
        GLRender_Enable(ENABLE_ALPHA_TEST);
        SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        RenderFlat(pPrimitive);
    }
}
#endif

void RenderFlatNonTextured(VertexPrimitiveFlat* pPrimitive)
{
    if (enablerender != 0)
    {
        GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender_Disable(ENABLE_TEXTURE_2D);
        RenderFlat(pPrimitive);
        GLRender_Enable(ENABLE_TEXTURE_2D);
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    }
}

void RenderFlatDecal(VertexPrimitiveFlat* pPrimitive)
{
    if (enablerender == 0)
    {
        return;
    }

    if (currentscene == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }

    pPrimitive->m_pPart->m_pMaterial->BindTexture0();
    GLRender_EnablePixelShaders(
        1, pPrimitive->m_pPart->m_pMaterial->m_pTexture0.m_pObject, 0, 0, 0);
#ifndef _XBOX
    if (AurMultiTextureAvailable() != 0)
    {
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
    }
#endif
    GLRender::cm_uiCurrentStage = 0;
    GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    GLRender_EnableClientState(CLIENT_STATE_VERTEX);
    GLRender_DisableClientState(CLIENT_STATE_NORMAL);
    GLRender_DisableClientState(CLIENT_STATE_COLOR);
    RenderFlat(pPrimitive);
    GLRender_EnableClientState(CLIENT_STATE_NORMAL);
    GLRender_EnableClientState(CLIENT_STATE_COLOR);
}

void RenderEMBM(VertexPrimitiveFlat* pPrimitive)  // PC 0x00488800 GOG 0x004888C0
{
    if (!enablerenderembm)
        return;
    if (!currentscene)
    {
        RenderFlat(pPrimitive);
        return;
    }
    PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
    MdlNodeTriMesh* pNode = (MdlNodeTriMesh*)pPart->m_pNode;
    Gob* pGob = pPart->m_pAurObject;
    GLRender_Enable(ENABLE_BLEND);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    bool bLightMapped = false;
    if (pPrimitive->m_pNode->m_bLightMapped && enablelightmap)
    {
        CAurTexture* pLightMap = (CAurTexture*)pPart->m_pMaterial->m_pTexture1.m_pObject;
        bLightMapped = true;
#ifndef _XBOX
        if (pLightMap && AurTextureGetTID(pLightMap) == 0)
            bLightMapped = false;
#endif
    }
    bool bFog;
    if (currentscene->m_bFog)
    {
        Model* pModel = pNode->m_pModel->AsModel();
        if (pModel->m_bModelOptionEnabled)
            bFog = true;
        else
            bFog = false;
    }
    else
        bFog = false;
    if (bFog)
        GLRender_Disable(ENABLE_FOG);
    GLRender_SetBlendFunc(BLEND_ONE,
                BLEND_ZERO);
    float fWaterAlpha = pPart->m_pMaterial->m_fWaterAlpha;
    if (fWaterAlpha == 1.0f)
    {
        if (((CAurTexture*)pPart->m_pMaterial->m_pTexture0.m_pObject)->IsNull() ||
            !bLightMapped)
        {
            if (enabledecal)
            {
                g_pBumpyShinyPass3->Enable(pPart, true);
                GLRender_Enable(ENABLE_VERTEX_PROGRAM);
                if (g_ForceSight)
                {
                    SetupForceSightShader(pGob, MODEL_SHADER_FGLOW, MODEL_SHADER_DESAT, 0, 0, 0, 0, false);
                }
                else
                {
                    if (pGob && pGob->IsIlluminationEnable() && AurNV1xLevelShadersAvailable())
                    {
                        float afColor[4];
                        afColor[0] = 0.0f;
                        afColor[1] = 0.0f;
                        afColor[2] = 0.0f;
                        afColor[3] = 1.0f;
                        pGob->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
                        SetPerInstanceIllumination(afColor);
                        GLRender_EnablePixelShaders(MODEL_SHADER_ILLUM, 0, 0, 0, 0);
                    }
                    else
                    {
                        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
                    }
                    pPart->m_pMaterial->BindTexture0();
                }
                RenderFlat(pPrimitive);
                if (g_ForceSight)
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
                    SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                    GLRender_SetDepthMask(1);
                }
                else if (pGob && pGob->IsIlluminationEnable())
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
                }
            }
        }
        else if (enabledecal)
        {
            GLRender::ActivateTextureStage(1);
            GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender::ActivateTextureStage(0);
            if (g_ForceSight)
                SetupForceSightShader(pGob, MODEL_SHADER_FGLOW_LM, MODEL_SHADER_DESAT_LM,
                                      pPart->m_pMaterial->m_pTexture0.m_pObject,
                                      pPart->m_pMaterial->m_pTexture1.m_pObject, 0, 0, false);
            else
                GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_LIGHTMAP,
                    pPart->m_pMaterial->m_pTexture0.m_pObject,
                    pPart->m_pMaterial->m_pTexture1.m_pObject, 0, 0);
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
            g_pBumpyShinyPass1->Enable(pPart, true);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMPY_SHINY_PASS,
                1.0f, 1.0f, 1.0f, 0.0f);
            RenderFlat(pPrimitive);
            GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_DISABLE, 0, 0, 0, 0);
            GLRender::ActivateTextureStage(1);
            GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender::ActivateTextureStage(0);
        }
    }
    GLRender_Disable(ENABLE_ALPHA_TEST);
#ifndef _XBOX
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
#endif
    GLRender_Enable(ENABLE_VERTEX_PROGRAM);
    if (enablebsenvmap)
    {
#ifdef _XBOX
        GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS0);
        GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS1);
        GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS2);
#else
        if (usearbvertexprograms)
        {
            glEnableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS0);
            glEnableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS1);
            glEnableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS2);
        }
        else
        {
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS0);
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS1);
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS2);
        }
#endif
        GLRender_DisableClientState(CLIENT_STATE_NORMAL);
        if (bLightMapped)
            g_pBumpyShinyPass5->Enable(pPart, false);
        else
            g_pBumpyShinyPass4->Enable(pPart, false);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMPY_SHINY_PASS,
            1.0f, 0.0f, 1.0f, 0.0f);
        if (g_ForceSight)
        {
            SetupForceSightShader(pGob, MODEL_SHADER_FGLOW_BUMPY_SHINY, MODEL_SHADER_DESAT_BUMPY_SHINY,
                                  pPart->m_pMaterial->m_pBumpMap.m_pObject, 0, 0,
                                  pPart->m_pMaterial->m_pEnvMap.m_pObject, true);
        }
        else
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_BUMPY_SHINY,
                pPart->m_pMaterial->m_pBumpMap.m_pObject, 0, 0,
                pPart->m_pMaterial->m_pEnvMap.m_pObject);
            GLRender_EnablePixelShaders(MODEL_SHADER_BUMPY_SHINY_ENABLE, 0, 0, 0, 0);
        }
        if (pPart->m_pMaterial->m_fWaterAlpha < 1.0f)
        {
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                        BLEND_ONE_MINUS_SRC_ALPHA);
            if (!g_ForceSight)
            {
                float fBlendAlpha = pPart->m_pMaterial->m_fWaterAlpha;
                Vector vWhite(1.0f, 1.0f, 1.0f);
                GLRender_SetPixelShaderAmbientColor(vWhite, fBlendAlpha);
            }
        }
        else
            GLRender_SetBlendFunc(BLEND_ONE_MINUS_DST_ALPHA,
                        BLEND_ONE);
        if (overridestuff)
            GLRender_SetBlendFunc(BLEND_ONE,
                        BLEND_ZERO);
        RenderFlat(pPrimitive);
        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
        GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_RESET, 0, 0, 0, 0);
        GLRender_EnablePixelShaders(MODEL_SHADER_BUMPY_SHINY_DISABLE, 0, 0, 0, 0);
#ifdef _XBOX
        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
#else
        if (usearbvertexprograms)
        {
            glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS0);
            glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS1);
            glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS2);
        }
        else
        {
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
        }
#endif
        GLRender_EnableClientState(CLIENT_STATE_NORMAL);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMPY_SHINY_PASS,
            1.0f, 1.0f, 1.0f, 0.0f);
    }
    if (bFog)
    {
        GLRender_Enable(ENABLE_FOG);
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                    BLEND_ONE_MINUS_SRC_ALPHA);
        GLRender_EnablePixelShaders(MODEL_SHADER_FOG,
            pPart->m_pMaterial->m_pTexture0.m_pObject, 0, 0, 0);
        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        g_pBumpyShinyPass2->Enable(pPart, false);
        RenderFlat(pPrimitive);
    }
#ifndef _XBOX
    glPopAttrib();
#endif
    AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMPY_SHINY_PASS,
        1.0f, 1.0f, 1.0f, 0.0f);
    GLRender_Disable(ENABLE_VERTEX_PROGRAM);
    GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
    GLRender_Enable(ENABLE_ALPHA_TEST);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                BLEND_ONE_MINUS_SRC_ALPHA);
}

// PC 0x00488E30 GOG 0x00488EE0
void RenderHologramEffect(int nRenderPath, VertexPrimitiveFlat* pPrimitive)
{
    PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
    GLRender_Enable(ENABLE_VERTEX_PROGRAM);
    VertexProgram* pHoloVP = 0;
    switch (nRenderPath)
    {
    case PartTriMesh::RENDER_EMBM:
    {
        static VertexProgram* pHoloBumpyShinyRegularVP =
            new VertexProgram(cHoloVPARB, cBumpyShinyRegularVPDeclaration);
        pHoloVP = pHoloBumpyShinyRegularVP;
        break;
    }
    case PartTriMesh::RENDER_LIGHTMAPPED_BUMP_SPECULAR:
    {
        static VertexProgram* pHoloBumpMapSpecularVP =
            new VertexProgram(cHoloVPARB, cBumpMapSpecularVPDeclaration);
        pHoloVP = pHoloBumpMapSpecularVP;
        break;
    }
    case PartTriMesh::RENDER_LIGHTMAPPED_BUMP_DIFFUSE:
    {
        static VertexProgram* pHoloBumpMapDiffuseVP =
            new VertexProgram(cHoloVPARB, cBumpMapDiffuseVPDeclaration);
        pHoloVP = pHoloBumpMapDiffuseVP;
        break;
    }
    case PartTriMesh::RENDER_NORMALMAPPED:
    {
        static VertexProgram* pHoloNormalmapVP =
            new VertexProgram(cHoloVPARB, szNormalmapVPARBDeclaration);
        pHoloVP = pHoloNormalmapVP;
        break;
    }
    case PartTriMesh::RENDER_BUMPMAPPED:
    case PartTriMesh::RENDER_BUMPMAP_FALLBACK:
    {
        static VertexProgram* pHoloBumpMapOnePassVP =
            new VertexProgram(cHoloVPARB, cBumpMapOnePassVPDeclaration);
        pHoloVP = pHoloBumpMapOnePassVP;
        break;
    }
    case PartTriMesh::RENDER_FLAT:
    case PartTriMesh::RENDER_FLAT_VP:
    case PartTriMesh::RENDER_ENVIRONMENTMAPPED:
    case PartTriMesh::RENDER_ENVIRONMENTMAPPED_VP:
    {
        static VertexProgram* pHoloStandardVP =
            new VertexProgram(cHoloVPARB, cStandardVPDeclaration);
        pHoloVP = pHoloStandardVP;
        break;
    }
    }
    if (pHoloVP)
    {
        pHoloVP->Enable(pPart, false);
        if (g_ForceSight)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DESAT, 0, 0, 0, 0);
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE);
        }
        else
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_HOLO, 0, 0, 0, 0);
            GLRender_SetBlendFunc(BLEND_ONE, BLEND_SRC_ALPHA);
        }
        RenderFlat(pPrimitive);
        GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DISABLE, 0, 0, 0, 0);
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    }
    GLRender_Disable(ENABLE_VERTEX_PROGRAM);
}

#ifndef _XBOX
int PartTriMesh::GetRenderPath()
{
    MdlNodeTriMesh* pNode = m_pNode->AsMdlNodeTriMesh();
    unsigned char bHasBumpMap = m_pMaterial->HasBumpMap();
    unsigned char bHasNormalMap = m_pMaterial->HasNormalMap();
    unsigned char bHasBumpyShinyMap = m_pMaterial->HasBumpyShinyMap();
    unsigned char bHasTangentBases = pNode->HasTangentBases();
    unsigned char bHasEnvMap = m_pMaterial->HasEnvMap();
    float fAlpha = m_pMaterial->m_fWaterAlpha;
    char bLightMapped = pNode->m_bLightMapped;
    bool bTransparent = fAlpha < 1.0f;

    if (m_pMaterial->m_pTexture1.m_pObject != 0 &&
        AurTextureGetTID((CAurTexture*)m_pMaterial->m_pTexture1.m_pObject) == 0)
    {
        bLightMapped = 0;
    }
    if (!bHasEnvMap && m_pAurObject != 0 && m_pAurObject->m_pEnvironmentMap.m_pObject != 0)
    {
        bHasEnvMap = 1;
    }

    int nPath = RENDER_FLAT;
    if (!AurNonRadeon8500Validate())
    {
        nPath = RENDER_FLAT_VP;
    }

    if (bHasBumpyShinyMap && bHasBumpMap && bHasTangentBases && enablebumpyshiny)
    {
        if (AurNV2xLevelShadersAvailable())
        {
            nPath = RENDER_EMBM;
        }
        else if (bLightMapped)
        {
            if (AurNV2xLevelShadersAvailable())
            {
                if (AurNonRadeon8500Validate())
                {
                    nPath = RENDER_EMLM;
                }
                else
                {
                    nPath = RENDER_EMLM_VP;
                }
            }
            else if (bTransparent)
            {
                nPath = RENDER_ENVIRONMENTMAPPED;
            }
            else
            {
                if (AurNV1xLevelShadersAvailable())
                {
                    nPath = RENDER_NV1X_EMLM;
                }
                else
                {
                    nPath = RENDER_LIGHTMAPPED_NO_SHADERS;
                }
            }
        }
        else
        {
            nPath = RENDER_ENVIRONMENTMAPPED;
        }
    }
    else if (pNode->m_bDirtEnabled &&
             (AurFrameBufferModificationsAvailable() || AurATIFrameBufferModificationsAvailable()))
    {
        nPath = RENDER_NOISEMAPPED;
    }
    else if (bLightMapped && bHasBumpMap && bHasTangentBases &&
             AurNV2xLevelShadersAvailable() && enablebumpmap && enablelightmap &&
             !usearbvertexprograms)
    {
        if (m_pMaterial->m_nRenderType != 0)
        {
            nPath = RENDER_LIGHTMAPPED_BUMP_SPECULAR;
        }
        else
        {
            nPath = RENDER_LIGHTMAPPED_BUMP_DIFFUSE;
        }
    }
    else if (bHasBumpMap && bHasTangentBases && enablebumpmap &&
             AurNV2xLevelShadersAvailable())
    {
        if (AurBumpMapCardValidate())
        {
            nPath = RENDER_BUMPMAPPED;
        }
        else
        {
            nPath = RENDER_BUMPMAP_FALLBACK;
        }
    }
    else if (bHasNormalMap && !bHasTangentBases && enablebumpmap &&
             AurNV2xLevelShadersAvailable())
    {
        nPath = RENDER_NORMALMAPPED;
    }
    else if (bLightMapped && bHasEnvMap && enablelightmap)
    {
        if (AurNV2xLevelShadersAvailable())
        {
            if (AurNonRadeon8500Validate())
            {
                nPath = RENDER_EMLM;
            }
            else
            {
                nPath = RENDER_EMLM_VP;
            }
        }
        else
        {
            if (AurNV1xLevelShadersAvailable())
            {
                nPath = RENDER_NV1X_EMLM;
            }
            else
            {
                nPath = RENDER_LIGHTMAPPED_NO_SHADERS;
            }
        }
    }
    else if (bHasEnvMap)
    {
        nPath = RENDER_ENVIRONMENTMAPPED;
    }
    else if (bLightMapped && enablelightmap)
    {
        if (AurNV2xLevelShadersAvailable())
        {
            if (AurNonRadeon8500Validate())
            {
                nPath = RENDER_LIGHTMAPPED;
            }
            else
            {
                nPath = RENDER_LIGHTMAPPED_VP;
            }
        }
        else
        {
            nPath = RENDER_LIGHTMAPPED_NO_SHADERS;
        }
    }
    else if (bHasBumpMap && bHasTangentBases && enablebumpmap &&
             !AurNV2xLevelShadersAvailable())
    {
        nPath = RENDER_BUMPMAP_FALLBACK;
    }

    if (nPath == RENDER_ENVIRONMENTMAPPED && !AurNonRadeon8500Validate())
    {
        nPath = RENDER_ENVIRONMENTMAPPED_VP;
    }
    return nPath;
}

void RenderFlatVP(VertexPrimitive* pPrimitive, PartTriMesh* pPart)  // PC 0x00489490 GOG 0x00489540
{
    GLRender_Enable(ENABLE_VERTEX_PROGRAM);
    static VertexProgram* standardvp =
        new VertexProgram(usearbvertexprograms ? szStandardVPARB
                                               : cStandardVP,
                          cStandardVPDeclaration);
    standardvp->Enable((Part*)pPart, true);
    Gob* pGob = pPrimitive->m_pPart->m_pAurObject;
    if (g_ForceSight)
    {
        SetupForceSightShader(pGob, MODEL_SHADER_FGLOW, MODEL_SHADER_DESAT, 0, 0, 0, 0, false);
        RenderFlat(pPrimitive->AsVertexPrimitiveFlat());
        GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        GLRender_SetDepthMask(1);
    }
    else
    {
        if (pGob && pGob->IsIlluminationEnable() && AurNV1xLevelShadersAvailable())
        {
            float afColor[4];
            afColor[0] = 0.0f;
            afColor[1] = 0.0f;
            afColor[2] = 0.0f;
            afColor[3] = 1.0f;
            pGob->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
            SetPerInstanceIllumination(afColor);
            GLRender_EnablePixelShaders(MODEL_SHADER_ILLUM, 0, 0, 0, 0);
        }
        RenderFlat(pPrimitive->AsVertexPrimitiveFlat());
        if (pGob && pGob->IsIlluminationEnable())
            GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
    }
    GLRender_Disable(ENABLE_VERTEX_PROGRAM);
}

void RenderLightmappedVP(VertexPrimitive* pPrimitive, PartTriMesh* pPart)
{
    GLRender_Enable(ENABLE_VERTEX_PROGRAM);
    static VertexProgram* standardvp =
        new VertexProgram(usearbvertexprograms ? szStandardVPARB
                                               : cStandardVP,
                          cStandardVPDeclaration);
    standardvp->Enable((Part*)pPart, true);
    RenderLightMapped(pPrimitive->AsVertexPrimitiveFlat());
    GLRender_Disable(ENABLE_VERTEX_PROGRAM);
}

void RenderEnvironmentMappedVP(VertexPrimitive* pPrimitive, PartTriMesh* pPart)
{
    GLRender_Enable(ENABLE_VERTEX_PROGRAM);

    static VertexProgram* standardvp =
        new VertexProgram(usearbvertexprograms ? szStandardCubeVPARB
                                               : cStandardVP,
                          cStandardVPDeclaration);

    standardvp->Enable((Part*)pPart, true);

    if (usearbvertexprograms != 0)
    {
        glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 15,
                                      0.0f, 0.0f, 1.0f, 0.0f);
    }
    else
    {
        glProgramParameter4fNV(GL_VERTEX_PROGRAM_ARB, 15,
                                  0.0f, 0.0f, 1.0f, 0.0f);
    }

    if (usearbvertexprograms != 0)
    {
        glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 16,
                                      0.0f, 2.0f, 3.0f, 1.0f);
    }
    else
    {
        glProgramParameter4fNV(GL_VERTEX_PROGRAM_ARB, 16,
                                  0.0f, 2.0f, 3.0f, 1.0f);
    }

    RenderEnvironmentMapped(pPrimitive->AsVertexPrimitiveFlat(), false, false);
    GLRender_Disable(ENABLE_VERTEX_PROGRAM);
}

void RenderEMLMVP(VertexPrimitive* pPrimitive, PartTriMesh* pPart)
{
    GLRender_Enable(ENABLE_VERTEX_PROGRAM);

    static VertexProgram* standardvp =
        new VertexProgram(usearbvertexprograms ? szStandardLightmapCubeVPARB
                                               : cStandardVP,
                          cStandardVPDeclaration);

    standardvp->Enable((Part*)pPart, true);

    if (usearbvertexprograms != 0)
    {
        glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 15, 0.0f, 0.0f, 1.0f, 0.0f);
    }
    else
    {
        glProgramParameter4fNV(GL_VERTEX_PROGRAM_NV, 15, 0.0f, 0.0f, 1.0f, 0.0f);
    }

    if (usearbvertexprograms != 0)
    {
        glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 16, 0.0f, 2.0f, 3.0f, 1.0f);
    }
    else
    {
        glProgramParameter4fNV(GL_VERTEX_PROGRAM_NV, 16, 0.0f, 2.0f, 3.0f, 1.0f);
    }

    RenderEMLM(pPrimitive->AsVertexPrimitiveFlat());
    GLRender_Disable(ENABLE_VERTEX_PROGRAM);
}

// PC 0x004899A0 GOG 0x00489A70
void RenderBumpmapFallback(VertexPrimitive* pPrimitive)
{
    if (g_ForceSight)
    {
        PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
        SetupForceSightShader(pPart->m_pAurObject, MODEL_SHADER_FGLOW, MODEL_SHADER_DESAT, 0, 0, 0, 0, false);
        RenderFlat(pPrimitive->AsVertexPrimitiveFlat());
        GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        GLRender_SetDepthMask(1);
    }
    else
    {
        GLRender_Disable(ENABLE_ALPHA_TEST);
        GLRender_SetBlendFunc(BLEND_ONE, BLEND_ZERO);
        RenderFlat(pPrimitive->AsVertexPrimitiveFlat());
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        GLRender_Enable(ENABLE_ALPHA_TEST);
    }
}
#endif

static inline void PushMatrix();

static inline void PopMatrix();

void Gob::PartDraw(Part* pPart, bool bRenderLast)
{
    if (pPart->m_bIncludeChildBounds)
    {
        Vector vSavedColor = currentcolor;
        PushMatrix();
#ifdef _XBOX
        Transform(pPart->m_vPosition, pPart->m_qOrientation, 0);
#else
        if (enabletransform)
            GLRender::Transform(pPart->m_vPosition, pPart->m_qOrientation, 0);
#endif
        float fScale = pPart->m_fScale;
        GLRender::Scale(fScale, fScale, fScale);
        PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
        if (!pTriMesh || !pPart->m_pNode ||
            ((MdlNodeTriMesh*)pPart->m_pNode)->m_bRender)
        {
            pPart->Mat();
            pPart->Draw(bRenderLast);
        }
        if (rendermaxdepth)
        {
            --rendermaxdepth;
            for (int nChild = 0; nChild < pPart->m_lstChildren.m_nCount; ++nChild)
                PartDraw(pPart->m_lstChildren[nChild], bRenderLast);
            ++rendermaxdepth;
        }
        PopMatrix();
        currentcolor = vSavedColor;
    }
}

void ProcessFaces(const Vector& vPosition, ShadowInfo* pShadowInfo,
                  MdlNodeTriMesh* pNode)
{
    for (int nFace = 0; nFace < pNode->m_lFaces.m_nCount; nFace++)
    {
        MaxFace& face = pNode->m_lFaces[nFace];
        pShadowInfo[nFace].m_fDistance =
            face.m_vNormal.m_fX * vPosition.m_fX +
            face.m_vNormal.m_fY * vPosition.m_fY +
            face.m_vNormal.m_fZ * vPosition.m_fZ +
            face.m_fDistance;

        *(Vector*)pShadowInfo[nFace].m_anAdjacentFaces =
            *(Vector*)face.m_anAdjacentFaces;
    }
}

static inline void PushMatrix()
{
    GLRender_PushCurrentMatrix();
}

static inline void PopMatrix()
{
    GLRender_PopCurrentMatrix();
}

void PartTriMesh::Project(const Vector& vPoint, float fDistance,
                          const Plane& plane, CAurShadowData* pShadowData)
{
    if (m_pNode->AsMdlNodeLightsaber())
        return;

    MdlNodeTriMesh* pNode = m_pVertexPrimitive->m_pNode;
    static Vector vp2v0;
    static Vector vp2v1;
    static Vector invVp;
    static Vector cap;
    static Vector sink;

    if (!pNode->m_bShadow && !pNode->m_bBeaming)
        return;

    if (useshadowmethod != SHADOW_METHOD_VERTEX_PROGRAM ||
        !usevertexprograms || !systemmemoryshadows)
    {
        int nPool = pNode->m_bBeaming;
        if (pShadowData)
            pShadowData->m_nEdges = 0;

        invVp = normalize(-vPoint);
        cap = invVp * epsilon;
        sink = invVp * fDistance + vPoint;
        if (!nPool && !ClipProjectionCaps(cap, sink, plane))
            return;

        if (!systemmemoryshadows)
        {
            static ShadowInfo processedFaces[ShadowInfo::MAX_FACES];
            unsigned short* pEdge = m_VertexIndices;
            memset(GLRender::m_UsedVertices, 0,
                   pNode->m_nVertexCount * sizeof(unsigned short));
            ProcessFaces(vPoint, processedFaces, pNode);
            for (int nFace = 0; nFace < pNode->m_lFaces.m_nCount; ++nFace)
            {
                ShadowInfo& face = processedFaces[nFace];
                if (!(face.m_fDistance < 0.0f))
                {
                    for (int nEdge = 0; nEdge < 3; ++nEdge)
                    {
                        if (face.m_anAdjacentFaces[nEdge] == MaxFace::NO_ADJACENT_FACE ||
                            !(processedFaces[face.m_anAdjacentFaces[nEdge]].m_fDistance *
                                face.m_fDistance > 0.0f))
                        {
                            int nFirst = face.m_anVertexIndices[nEdge];
                            int nSecond = face.m_anVertexIndices[(nEdge + 1) % 3];
                            GLRender::m_UsedVertices[nFirst] = GLRender::SHADOW_VERTEX_USED;
                            GLRender::m_UsedVertices[nSecond] = GLRender::SHADOW_VERTEX_USED;
                            *pEdge++ = nFirst;
                            *pEdge++ = nSecond;
                        }
                    }
                }
            }
            int nEdges = (pEdge - m_VertexIndices) / 2;
            if (nEdges > 0)
            {
#ifdef _XBOX

                int nVertices = GLRender::BuildShadowArray(vPoint, fDistance,
                    plane, cap, sink, pNode->m_nVertexCount, pNode->m_pModel->AsModel()->m_nVertexPool,
                    pNode->m_nVertexDataOffset, pNode->m_nVertexStride);
#else
                RenderVertexV* pVertices;
                if (AsPartDanglyMesh())
                    pVertices = (RenderVertexV*)AsPartDanglyMesh()->m_pVertexPositions;
                else
                    pVertices = (RenderVertexV*)pNode->m_pVertexPositions;
                int nVertices = GLRender::BuildShadowArray(vPoint, fDistance,
                    plane, cap, sink, pNode->m_nVertexCount, pVertices);
#endif
                GLRender::RenderShadowArray(nVertices, nEdges, nPool);
            }
            if (pShadowData)
                pShadowData->m_nEdges = nEdges;
            return;
        }
    }

    MdlNodeTriMesh* pMesh = m_pNode->AsMdlNodeTriMesh();
    unsigned long nOffset = pMesh->m_nVertexDataOffset;
    int nVertices = pMesh->m_nVertexCount + 1;
    unsigned long nPool = pMesh->m_pModel->AsModel()->m_nVertexPool;
    GLRender::SetInterleavedBuffer(pMesh->m_nVertexStride,
        pMesh->m_nVertexArrayFlags, nPool, nVertices, pMesh, nOffset);
    GLRender_SetFrontFace(CULL_CLOCKWISE);
    GLRender_SetStencilOp(STENCIL_KEEP,
                STENCIL_INCREMENT,
                STENCIL_KEEP);
    m_pVertexPrimitive->AsVertexPrimitiveFlat();
    GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
    GLRender_SetStencilOp(STENCIL_KEEP,
                STENCIL_DECREMENT,
                STENCIL_KEEP);
    m_pVertexPrimitive->AsVertexPrimitiveFlat();
}

void Gob::PartVolumeProject(Part* pPart, Vector vPoint, float fDistance,
                            Plane plane)
{
    if (pPart->m_bIncludeChildBounds)
    {
        GLRender_PushCurrentMatrix();
        if (pPart == m_pRootPart)
        {
#ifdef _XBOX
            Transform(m_vPosition, m_qOrientation, 1);
#else
            if (enabletransform)
                GLRender::Transform(m_vPosition, m_qOrientation, 1);
#endif
            if (!systemmemoryshadows)
            {
                vPoint = -m_qOrientation * (vPoint - m_vPosition);
                plane.Transform(m_vPosition, m_qOrientation);
            }
            if (m_fScale != 1.0f)
                GLRender::Scale(m_fScale, m_fScale, m_fScale);
        }
        Plane localPlane;
        if (!systemmemoryshadows)
        {
            vPoint = -pPart->m_qOrientation * (vPoint - pPart->m_vPosition);
            localPlane = Plane(plane.m_vNormal, plane.m_fDistance);
            localPlane.Transform(pPart->m_vPosition, pPart->m_qOrientation);
        }
#ifdef _XBOX
        Transform(pPart->m_vPosition, pPart->m_qOrientation, 0);
#else
        if (enabletransform)
            GLRender::Transform(pPart->m_vPosition, pPart->m_qOrientation, 0);
#endif
        ++counttrans;
        if (pPart->m_fScale != 1.0f)
            GLRender::Scale(pPart->m_fScale, pPart->m_fScale, pPart->m_fScale);
        if (systemmemoryshadows || !vPoint.IsZero())
        {
            int bBeaming = FALSE;
            PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
            MdlNodeTriMesh* pNode = (MdlNodeTriMesh*)pPart->m_pNode;
            if (pTriMesh)
                bBeaming = pNode->m_bBeaming;
            if (bBeaming)
                SetBeaming();
            pPart->Project(vPoint, fDistance, localPlane, 0);
        }
        for (int nChild = 0; nChild < pPart->m_lstChildren.m_nCount; ++nChild)
            PartVolumeProject(pPart->m_lstChildren[nChild], vPoint,
                              fDistance, localPlane);
        GLRender_PopCurrentMatrix();
    }
}

inline Vector Material::GetBumpMapSpecularColor()
{
    CAurTexture* pBumpMap = (CAurTexture*)m_pBumpMap.m_pObject;
    return pBumpMap->GetSpecularColor();
}

void SetBeaming()
{
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE);
    GLRender_SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_SetColorv(Homogenize(projectioncolor, projectionalpha));

    if (stenciloff != 0)
    {
        GLRender_SetStencilMask(0);
    }
}

void Part::Draw(bool bRenderChildren)
{
    GLfloat fColor[3];

    if (enableplaceholders != 0)
    {
        fColor[0] = 1.0f;
        fColor[1] = 0.0f;
        fColor[2] = 0.0f;
        GLRender::DrawSphereBound(0.1f, m_vPosition, *(Vector*)fColor, 1.0f,
                                  true);
    }
}

inline void Gob::ProxyPartDraw(Part* pPart, bool bRenderLast)
{
    if (m_pTileData && m_pTileData->m_fFadeAlpha < 1.0f)
    {
        GLRender_Enable(ENABLE_FOG);
        float fAlphaSquared = m_pTileData->m_fFadeAlpha * m_pTileData->m_fFadeAlpha;
        Vector vFogColor = m_pScene->m_vFogColor * fAlphaSquared;
        GLRender_SetFogModev(FOG_COLOR, vFogColor);
        GLRender_SetFogModef(FOG_DENSITY, m_pScene->m_fFogDensity + m_pScene->m_fFogDensity);
        GLRender_SetFogModef(FOG_START, m_pScene->m_fFogNear * m_pTileData->m_fFadeAlpha);
        GLRender_SetFogModef(FOG_END, m_pScene->m_fFogFar * m_pTileData->m_fFadeAlpha + 0.1f);
        PartDraw(m_pRootPart, bRenderLast);
        if (!m_pScene->m_bFog)
        {
            GLRender_Disable(ENABLE_FOG);
        }
        float fDensity = m_pScene->m_fFogDensity;
        GLRender_SetFogModef(FOG_DENSITY, fDensity);
        float fNear = m_pScene->m_fFogNear;
        GLRender_SetFogModef(FOG_START, fNear);
        float fFar = m_pScene->m_fFogFar;
        GLRender_SetFogModef(FOG_END, fFar);
        GLRender_SetFogModev(FOG_COLOR, m_pScene->m_vFogColor);
    }
    else
    {
        PartDraw(m_pRootPart, bRenderLast);
    }
}

void Gob::RenderBlur()
{
    GLRender_PushCurrentMatrix();
#ifdef _XBOX
    Transform(m_vPosition, m_qOrientation, 1);
#else
    if (enabletransform != 0)
    {
        GLRender::Transform(m_vPosition, m_qOrientation, 1);
    }
#endif

    if (m_fScale != 1.0f)
    {
        GLRender::Scale(m_fScale, m_fScale, m_fScale);
    }

    ProxyPartDraw(m_pRootPart, true);
    m_vPreviousPosition = m_vPosition;
    m_qPreviousOrientation = m_qOrientation;
    GLRender_PopCurrentMatrix();
}

#ifdef _XBOX

char* setseparatespecular(char* sArguments)
{
    if (atoi(sArguments))
    {
        g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    }
    else
    {
        g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    }
    return "Success";
}
#endif

void RenderLightMappedBumpDiffuse(VertexPrimitiveFlat* pPrimitive)
{
    if (currentscene == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }

    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
    if (lstLights.m_nCount == 0)
    {
        RenderLightMapped(pPrimitive);
        return;
    }
    if (AurPixelShadersAvailable() != 0)
    {
        GLRender_Disable(ENABLE_FOG);
        PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
        MdlNodeTriMesh* pNode = 0;
        if (pPart != 0)
        {
            pNode = (MdlNodeTriMesh*)pPart->m_pNode;
        }
        Vector vDiffuse;

        static VertexProgram* bumpmapvp = new VertexProgram(
            cBumpMapDiffuseVP,
            cBumpMapDiffuseVPDeclaration);
        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        if (pNode != 0)
        {
            vDiffuse = pNode->m_vDiffuse;
        }
        int bAlphaTest = GLRender_IsEnabled(ENABLE_ALPHA_TEST);
        GLRender_Disable(ENABLE_ALPHA_TEST);
        GLRender::ActivateTextureStage(1);
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS0);
        GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS1);
        GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS2);
        GLRender_EnablePixelShaders(MODEL_SHADER_LIGHTMAPPED_BUMP_DIFFUSE,
            pPart->m_pMaterial->m_pBumpMap,
            currentscene->m_pNormalizationCubeMap,
            pPart->m_pMaterial->m_pTexture1,
            pPart->m_pMaterial->m_pTexture0);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_FOG,
            currentscene->m_fFogNear, currentscene->m_fFogFar,
            currentscene->m_bFog ? 1.0f : 0.0f, 0.5f);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_LIGHT_VECTOR,
            1.0f, -1.0f, 0.0f, 0.0f);

        Quaternion qOrientation;
        Vector vPosition;
        pPart->GetWorld(vPosition, qOrientation);
        Vector vEye = CurrentCamera->m_vPosition - vPosition;
        vEye = -qOrientation * vEye;
        for (int nLight = 0; nLight < lstLights.m_nCount; ++nLight)
        {
            currentscene->m_pLightManager->DisableBumpMapLights();
            bumpmapvp->Enable(pPart, true);
            Vector vLight = lstLights[nLight]->m_vWorldPosition;
            vLight = -qOrientation * (vLight - vPosition);
            Vector& vColor = lstLights[nLight]->m_vColor;
            Vector vLightColor = cmul(vColor, vDiffuse * diffuseintensity);
            GLRender_SetBumpLightColor(vLightColor, 1.0f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_LIGHT_POSITION,
                vLight.m_fX, vLight.m_fY, vLight.m_fZ, 1.0f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_EYE_POSITION,
                vEye.m_fX, vEye.m_fY, vEye.m_fZ, 1.0f);
            float fRadius = lstLights[nLight]->m_fRadius;
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_ATTENUATION,
                1.0f, 0.0f, AUR_LIGHT_ATTENUATION_SCALE / (2.0f * fRadius * fRadius),
                1.0f);
            RenderFlat(pPrimitive);
            if (bAlphaTest != 0)
            {
                GLRender_Enable(ENABLE_ALPHA_TEST);
            }
            currentscene->m_pLightManager->EnableBumpMapLights();
        }
        GLRender::ActivateTextureStage(1);
        GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender::ActivateTextureStage(0);
        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
        currentscene->DoFog();
        GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_DISABLE, 0, 0, 0, 0);
        GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
        GLRender_Disable(ENABLE_VERTEX_PROGRAM);
        ((CAurTexture*)pPart->m_pMaterial->m_pTexture0.m_pObject)->Bind();
    }
}

void RenderLightMappedBumpSpecular(VertexPrimitiveFlat* pPrimitive)
{
    if (currentscene == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
    if (lstLights.m_nCount == 0)
    {
        RenderLightMapped(pPrimitive);
        return;
    }
    int bAlphaTestEnabled = GLRender_IsEnabled(ENABLE_ALPHA_TEST);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    if (AurPixelShadersAvailable())
    {
        PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
        GLRender_Enable(ENABLE_FOG);
        MdlNodeTriMesh* pNode = 0;
        if (pPart != 0)
            pNode = (MdlNodeTriMesh*)pPart->m_pNode;
        Vector vSpecular;
        Vector vDiffuse;

        static VertexProgram* bumpmapvp = new VertexProgram(
            cBumpMapSpecularVP,
            cBumpMapSpecularVPDeclaration);
        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        if (pNode != 0)
        {
            vSpecular = pPart->m_pMaterial->GetBumpMapSpecularColor();
            vDiffuse = pNode->m_vDiffuse;
            float fDiffuse = magnitude(vDiffuse);
            float fSpecular = bumpspecular && pPart->m_pMaterial->BumpMapIsSpecular()
                ? magnitude(vSpecular) : 0.0f;
            vDiffuse = (vDiffuse * fDiffuse) / (fDiffuse + fSpecular);
        }
        int bAlphaTest = GLRender_IsEnabled(ENABLE_ALPHA_TEST);
        GLRender_Disable(ENABLE_ALPHA_TEST);
        GLRender::ActivateTextureStage(1);
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS0);
        GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS1);
        GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS2);
        if (bmspecularexponent == 1)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR1,
                pPart->m_pMaterial->m_pBumpMap, currentscene->m_pNormalizationCubeMap,
                pPart->m_pMaterial->m_pTexture1, pPart->m_pMaterial->m_pTexture0);
        }
        else if (bmspecularexponent == 2)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR2,
                pPart->m_pMaterial->m_pBumpMap, currentscene->m_pNormalizationCubeMap,
                pPart->m_pMaterial->m_pTexture1, pPart->m_pMaterial->m_pTexture0);
        }
        else if (bmspecularexponent == 4)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR4,
                pPart->m_pMaterial->m_pBumpMap, currentscene->m_pNormalizationCubeMap,
                pPart->m_pMaterial->m_pTexture1, pPart->m_pMaterial->m_pTexture0);
        }
        else if (bmspecularexponent == 8)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_LIGHTMAPPED_BUMP_SPECULAR8,
                pPart->m_pMaterial->m_pBumpMap, currentscene->m_pNormalizationCubeMap,
                pPart->m_pMaterial->m_pTexture1, pPart->m_pMaterial->m_pTexture0);
        }
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_FOG,
            currentscene->m_fFogNear, currentscene->m_fFogFar,
            currentscene->m_bFog ? 1.0f : 0.0f, 0.5f);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_LIGHT_VECTOR,
            1.0f, -1.0f, 0.0f, 0.0f);
        Quaternion qOrientation;
        Vector vPosition;
        pPart->GetWorld(vPosition, qOrientation);
        Vector vEye = -qOrientation * (CurrentCamera->m_vPosition - vPosition);
        for (int nLight = 0; nLight < lstLights.m_nCount; ++nLight)
        {
            bumpmapvp->Enable(pPart, true);
            Vector vLight = lstLights[nLight]->m_vWorldPosition;
            vLight = -qOrientation * (vLight - vPosition);
            Vector vLightColor = cmul(lstLights[nLight]->m_vColor,
                (vSpecular * specularintensity) *
                ((CAurTexture*)pPart->m_pMaterial->m_pBumpMap.m_pObject)->GetSpecularBumpIntensity());
            GLRender_SetPixelShaderAmbientColor(vLightColor, 1.0f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_LIGHT_POSITION,
                vLight.m_fX, vLight.m_fY, vLight.m_fZ, 1.0f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_EYE_POSITION,
                vEye.m_fX, vEye.m_fY, vEye.m_fZ, 1.0f);
            float fRadius = lstLights[nLight]->m_fRadius;
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_ATTENUATION,
                1.0f, 0.0f, AUR_LIGHT_ATTENUATION_SCALE / (fRadius * fRadius + fRadius * fRadius), 1.0f);
            RenderFlat(pPrimitive);
            if (bAlphaTest != 0)
                GLRender_Enable(ENABLE_ALPHA_TEST);
        }
        GLRender_Disable(ENABLE_FOG);
        GLRender_DisableClientState(CLIENT_STATE_LIGHT_VECTOR);
        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
        GLRender::ActivateTextureStage(1);
        GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender::ActivateTextureStage(0);
        currentscene->DoFog();
        GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_DISABLE, 0, 0, 0, 0);
        GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
        GLRender_Disable(ENABLE_VERTEX_PROGRAM);
    }
    if (bAlphaTestEnabled != 0)
        GLRender_Enable(ENABLE_ALPHA_TEST);
}

void RenderDiffuseBumpMappedPass(VertexPrimitiveFlat* pPrimitive)
{
    if (enablerender == 0)
    {
        return;
    }
    if (currentscene == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }

    Part* pPart = pPrimitive->m_pPart;
    PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
    MdlNodeTriMesh* pNode = 0;
    if (pTriMesh != 0)
    {
        pNode = (MdlNodeTriMesh*)pPart->m_pNode;
    }
    Vector vDiffuse;
    Vector vSpecular;
    if (pNode != 0)
    {
        vSpecular = pPart->m_pMaterial->GetBumpMapSpecularColor();
        vDiffuse = pNode->m_vDiffuse;
        float fDiffuse = magnitude(vDiffuse);
        float fSpecular = bumpspecular && pPart->m_pMaterial->BumpMapIsSpecular()
            ? magnitude(vSpecular) : 0.0f;
        vDiffuse = (vDiffuse * fDiffuse) / (fDiffuse + fSpecular);
    }

    static VertexProgram* bumpvp = new VertexProgram(
        cBumpMapVP, cBumpMapVPDeclaration);
    bumpvp->Enable(pPart, false);
    GLRender_Enable(ENABLE_VERTEX_PROGRAM);
    AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_CONSTANTS,
                                   1.0f, -1.0f, 0.0f, 0.0f);
    GLRender_Enable(ENABLE_FOG);
    GLRender_SetFogMode(FOG_MODE_LINEAR);
    GLRender_SetFogModef(FOG_START, 0.0f);
    GLRender_SetFogModef(FOG_END, 1.0f);
    Vector vFogColor;
    GLRender_SetFogModev(FOG_COLOR, vFogColor);

    List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
    Vector& vAmbient = currentscene->m_vAmbient;
    GLRender_SetPixelShaderAmbientColor(vAmbient, 1.0f);
#ifndef _XBOX
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
#endif
    GLRender_EnableClientState(CLIENT_STATE_LIGHT_VECTOR);
    GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS0);
    GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS1);
    GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS2);
    GLRender_EnableClientState(CLIENT_STATE_BUMP_FOG);
    GLRender_DisableClientState(CLIENT_STATE_NORMAL);
    GLRender_DisableClientState(CLIENT_STATE_COLOR);
    GLRender_EnablePixelShaders(MODEL_SHADER_DIFFUSE_BUMP_FOG_ALPHA,
        pPart->m_pMaterial->m_pBumpMap,
        currentscene->m_pNormalizationCubeMap, 0, 0);

    for (int nLight = 0; nLight < lstLights.m_nCount; ++nLight)
    {
        Vector vLight = lstLights[nLight]->m_vWorldPosition;
        Vector& vColor = lstLights[nLight]->m_vColor;
        float fRadius = lstLights[nLight]->m_fRadius;
        vLight = -pPart->m_qOrientation * (vLight - pPart->m_vPosition);
        pPrimitive->UpdateLightVectors(vLight);
        pPrimitive->UpdateFogValues(vLight, fRadius);
        Vector vLightColor = cmul(vColor, vDiffuse * diffuseintensity);
        GLRender_SetBumpLightColor(vLightColor, 1.0f);
        GLRender_SetBlendFunc(BLEND_ONE, nLight == 0 ? BLEND_ZERO : BLEND_ONE);
        RenderFlat(pPrimitive);
    }

    GLRender_DisableClientState(CLIENT_STATE_LIGHT_VECTOR);
    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
    GLRender_DisableClientState(CLIENT_STATE_BUMP_FOG);
    GLRender_EnableClientState(CLIENT_STATE_NORMAL);
    GLRender_EnableClientState(CLIENT_STATE_COLOR);
#ifndef _XBOX
    glPopClientAttrib();
#endif
    currentscene->DoFog();
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
    GLRender_Disable(ENABLE_VERTEX_PROGRAM);
}

void RenderSpecularBumpMappedPass(VertexPrimitiveFlat* pPrimitive)
{
    if (enablerender == 0)
    {
        return;
    }
    if (currentscene == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }

    PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
    MdlNodeTriMesh* pNode = 0;
    if (pPart != 0)
    {
        pNode = (MdlNodeTriMesh*)pPart->m_pNode;
    }
    Vector vSpecular;
    Vector vDiffuse;
    vSpecular = pPart->m_pMaterial->GetBumpMapSpecularColor();
    if (pNode != 0)
    {
        vDiffuse = pNode->m_vDiffuse;
        float fDiffuse = magnitude(vDiffuse);
        float fSpecular = magnitude(vSpecular);
        vSpecular = (vDiffuse * fSpecular) / (fDiffuse + fSpecular);
    }

    static VertexProgram* bumpvp = new VertexProgram(
        cBumpMapVP, cBumpMapVPDeclaration);
    bumpvp->Enable(pPart, false);
    GLRender_Enable(ENABLE_VERTEX_PROGRAM);
    AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_CONSTANTS,
                                   1.0f, -1.0f, 0.0f, 0.0f);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                BLEND_ONE);
    GLRender_Enable(ENABLE_FOG);
    GLRender_SetFogMode(FOG_MODE_LINEAR);
    GLRender_SetFogModef(FOG_START, 0.0f);
    GLRender_SetFogModef(FOG_END, 1.0f);
    Vector vFogColor;
    GLRender_SetFogModev(FOG_COLOR, vFogColor);

    List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
#ifndef _XBOX
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
#endif
    GLRender_EnableClientState(CLIENT_STATE_LIGHT_VECTOR);
    GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS0);
    GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS1);
    GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS2);
    GLRender_EnableClientState(CLIENT_STATE_BUMP_FOG);
    GLRender_DisableClientState(CLIENT_STATE_NORMAL);
    GLRender_DisableClientState(CLIENT_STATE_COLOR);
    GLRender_EnablePixelShaders(MODEL_SHADER_SPECULAR_FOG,
        pPart->m_pMaterial->m_pTexture0,
        pPart->m_pMaterial->m_pBumpMap, 0,
        currentscene->m_pNormalizationCubeMap);

    for (int nLight = 0; nLight < lstLights.m_nCount; ++nLight)
    {
        Vector vLight = lstLights[nLight]->m_vWorldPosition;
        Vector vEye = CurrentCamera->m_vPosition;
        float fRadius = lstLights[nLight]->m_fRadius;
        vLight = -pPart->m_qOrientation * (vLight - pPart->m_vPosition);
        vEye = -pPart->m_qOrientation * (vEye - pPart->m_vPosition);
        pPrimitive->UpdateHalfAngleVectors(vLight, vEye);
        pPrimitive->UpdateFogValues(vLight, fRadius);
        Vector vLightColor = cmul(lstLights[nLight]->m_vColor,
            (vSpecular * specularintensity) * pPart->m_pMaterial->GetBumpMapIntensity());
        GLRender_SetBumpLightColor(vLightColor, 1.0f);
        RenderFlat(pPrimitive);
    }

    GLRender_DisableClientState(CLIENT_STATE_LIGHT_VECTOR);
    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
    GLRender_DisableClientState(CLIENT_STATE_BUMP_FOG);
    GLRender_EnableClientState(CLIENT_STATE_NORMAL);
    GLRender_EnableClientState(CLIENT_STATE_COLOR);
#ifndef _XBOX
    glPopClientAttrib();
#endif
    currentscene->DoFog();
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                BLEND_ONE_MINUS_SRC_ALPHA);
    GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
    GLRender_Disable(ENABLE_VERTEX_PROGRAM);
}

enum MotionBlurredVPRegister
{
    MOTION_BLURRED_VP_REVERSE = 13,
    MOTION_BLURRED_VP_CONSTANTS = 17,
    MOTION_BLURRED_VP_DIRECTION = 50,
    MOTION_BLURRED_VP_LIMITS = 51,
    MOTION_BLURRED_VP_SIDE = 52,
    MOTION_BLURRED_VP_REFERENCE = 53
};
#ifndef _XBOX
extern unsigned long* cMotionBlurredVPDeclaration;
#endif
#ifdef _XBOX
void RenderMotionBlurred(VertexPrimitiveFlat* pPrimitive)
{
#ifdef _XBOX

    if (currentscene == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }
#endif
    Part* pPart = pPrimitive->m_pPart;
    Vector vDisplacement = pPart->m_pAurObject->m_vPosition -
        pPart->m_pAurObject->m_vPreviousPosition;
    float fBlurLength = magnitude(vDisplacement);
    if (fBlurLength > 0.0f)
    {
#ifdef _XBOX
        bool bAlphaTest = GLRender_IsEnabled(ENABLE_ALPHA_TEST);
        GLRender_Disable(ENABLE_ALPHA_TEST);

        static VertexProgram* motionblurredvp = new VertexProgram(
            cMotionBlurredVP, cMotionBlurredVPDeclaration);

        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        GLRender_SetDepthMask(false);
#else
        pPrimitive->SetupVertexArraysStandard();
        GLRender_SetDepthMask(GL_FALSE);
        GLRender_Enable(ENABLE_VERTEX_PROGRAM);

        static VertexProgram* motionblurredvp = new VertexProgram(
            cMotionBlurredVP, cMotionBlurredVPDeclaration);
#endif
        Quaternion qOrientation;
        Vector vPosition;
        pPart->GetWorld(vPosition, qOrientation);
        Vector vCamera = -qOrientation * (CurrentCamera->m_vPosition - vPosition);
#ifdef _XBOX
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMP_FOG,
            currentscene->m_fFogNear, currentscene->m_fFogFar,
            currentscene->m_bFog ? 1.0f : 0.0f, SKIN_BLUR_ALPHA);
        AurSetVertexProgramParameter4f(MOTION_BLURRED_VP_CONSTANTS,
            0.0f, 2.0f, SKIN_BLUR_ALPHA, 1.0f);
#endif
        Vector vSide = cross(vDisplacement,
            pPart->m_pAurObject->m_qOrientation.zdir());
        vDisplacement = -qOrientation * vDisplacement;
        vSide = -qOrientation * vSide;
        fBlurLength = fBlurLength / (2.0f * DeltaT);
        Vector vOrigin;
        vOrigin = -qOrientation * (pPart->m_pAurObject->m_vPosition - vPosition);

#ifndef _XBOX
        AurSetVertexProgramParameter4f(MOTION_BLURRED_VP_CONSTANTS,
            0.0f, 2.0f, SKIN_BLUR_ALPHA, 1.0f);
#endif
        AurSetVertexProgramParameter4f(MOTION_BLURRED_VP_REFERENCE,
            vOrigin.m_fX, vOrigin.m_fY, vOrigin.m_fZ, 1.0f);
        vDisplacement = normalize(vDisplacement);
        AurSetVertexProgramParameter4f(MOTION_BLURRED_VP_DIRECTION,
            vDisplacement.m_fX, vDisplacement.m_fY, vDisplacement.m_fZ, 1.0f);
        AurSetVertexProgramParameter4f(MOTION_BLURRED_VP_LIMITS,
            fBlurLength, pPart->m_pAurObject->m_fMaxBlurLength,
            SKIN_BLUR_NORMAL_OFFSET, SKIN_BLUR_LENGTH_SCALE);
        AurSetVertexProgramParameter4f(MOTION_BLURRED_VP_SIDE,
            vSide.m_fX, vSide.m_fY, vSide.m_fZ, SKIN_BLUR_SIDE_SCALE);
        AurSetVertexProgramParameter4f(MOTION_BLURRED_VP_REVERSE,
            0.0f, 0.0f, 0.0f, 0.0f);

        Vector vCameraToOrigin = vOrigin - vCamera;
        Vector vProjection = vDisplacement * (vCameraToOrigin * vDisplacement);
        if (vProjection.m_fX * vProjection.m_fX +
            vProjection.m_fY * vProjection.m_fY +
            vProjection.m_fZ * vProjection.m_fZ >
            (vCameraToOrigin * vCameraToOrigin) * SKIN_BLUR_CULL_COSINE_SQUARED)
        {
            if (magnitude(vCameraToOrigin + vDisplacement) >
                magnitude(vCameraToOrigin - vDisplacement))
            {
                AurSetVertexProgramParameter4f(MOTION_BLURRED_VP_REVERSE,
                    1.0f, 0.0f, 0.0f, 0.0f);
                GLRender_SetFrontFace(CULL_CLOCKWISE);
            }
        }
#ifdef _XBOX
        GLRender_EnablePixelShaders(MODEL_SHADER_SKIN_BLUR,
            pPart->m_pMaterial->m_pTexture0.m_pObject, 0, 0, 0);
        motionblurredvp->Enable(pPart, true);
        RenderFlat(pPrimitive);
        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
        if (bAlphaTest)
        {
            GLRender_Enable(ENABLE_ALPHA_TEST);
        }
        GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
    }
#else
        motionblurredvp->Enable(pPart, true);
        RenderFlat(pPrimitive);
    }
    GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
#endif
    GLRender_Disable(ENABLE_VERTEX_PROGRAM);
}
#endif
// PC 0x00492190 GOG 0x00492260
void RenderBumpMapped(VertexPrimitiveFlat* pPrimitive)
{
    if (currentscene == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }

    GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                BLEND_ONE_MINUS_SRC_ALPHA);
    if (enablerenderbumpmap == 0 || enablerender == 0)
    {
        return;
    }

    List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
    if (lstLights.m_nCount == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }

    int bAlphaTestEnabled = GLRender_IsEnabled(ENABLE_ALPHA_TEST);
    GLRender_Disable(ENABLE_ALPHA_TEST);
#ifdef _XBOX
    if (useonepasssolution)
#else
    if (AurPixelShadersAvailable() && useonepasssolution)
#endif
    {
#ifdef _XBOX
        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
#else
        GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
#endif
#ifndef _XBOX
        if (usearbvertexprograms)
        {
            glDisable(GL_VERTEX_PROGRAM_ARB);
        }
        else
#endif
        {
            GLRender_Disable(ENABLE_VERTEX_PROGRAM);
        }

        PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
        MdlNodeTriMesh* pNode = 0;
        if (pPart != 0)
        {
            pNode = (MdlNodeTriMesh*)pPart->m_pNode;
        }
        Vector vSpecular;
        Vector vDiffuse;

        static VertexProgram* bumpmapvp = new VertexProgram(
            usearbvertexprograms ?
            szBumpmapVPARB
            : cBumpMapOnePassVP,
            cBumpMapOnePassVPDeclaration);
#ifndef _XBOX
        if (usearbvertexprograms)
        {
            glEnable(GL_VERTEX_PROGRAM_ARB);
        }
        else
#endif
        {
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        }
        if (pNode != 0)
        {
            vSpecular = pPart->m_pMaterial->GetBumpMapSpecularColor();
            vDiffuse = pNode->m_vDiffuse;
        }
#ifndef _XBOX
        if (usearbvertexprograms)
        {
            glEnableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS0);
            glEnableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS1);
            glEnableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS2);
        }
        else
#endif
        {
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS0);
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS1);
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS2);
        }
        if (!g_ForceSight)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_BUMP,
                pPart->m_pMaterial->m_pBumpMap,
                currentscene->m_pNormalizationCubeMap,
                currentscene->m_pNormalizationCubeMap,
                pPart->m_pMaterial->m_pTexture0);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_FOG,
                currentscene->m_fFogNear, currentscene->m_fFogFar,
                currentscene->m_bFog ? 1.0f : 0.0f, 0.5f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_CONSTANTS,
                1.0f, -1.0f, 0.0f, 0.0f);

            Quaternion qOrientation;
            Vector vPosition;
            pPart->GetWorld(vPosition, qOrientation);
            Vector vEye = -qOrientation * (CurrentCamera->m_vPosition - vPosition);
            for (int nLight = 0; nLight < lstLights.m_nCount; ++nLight)
            {
                currentscene->m_pLightManager->DisableBumpMapLights();
                bumpmapvp->Enable(pPart, true);
                Vector vLight = lstLights[nLight]->m_vWorldPosition;
                vLight = -qOrientation * (vLight - vPosition);
                Vector& vColor = lstLights[nLight]->m_vColor;
                Vector vLightColor = cmul(vColor,
                    (vDiffuse * diffuseintensity) *
                    ((CAurTexture*)pPart->m_pMaterial->m_pBumpMap.m_pObject)->GetDiffuseBumpIntensity());
                if (usebumpdiffuse)
                {
                    SetBumpLightColor(vLightColor);
                }
                else
                {
                    SetBumpLightColor(Vector(1.0f, 0.0f, 0.0f));
                }
                vLightColor = cmul(lstLights[nLight]->m_vColor,
                    (vSpecular * specularintensity) *
                    ((CAurTexture*)pPart->m_pMaterial->m_pBumpMap.m_pObject)->GetSpecularBumpIntensity());
                if (usebumpspecular)
                {
                    GLRender_SetPixelShaderAmbientColor(vLightColor, 1.0f);
                }
                else
                {
                    GLRender_SetPixelShaderAmbientColor(Vector(0.0f, 1.0f, 0.0f), 1.0f);
                }
                AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_LIGHT_POSITION,
                    vLight.m_fX, vLight.m_fY, vLight.m_fZ, 1.0f);
                AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_EYE_POSITION,
                    vEye.m_fX, vEye.m_fY, vEye.m_fZ, 1.0f);
                AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_ATTENUATION,
                    1.0f, 0.0f, AUR_LIGHT_ATTENUATION_SCALE / (lstLights[nLight]->m_fRadius * lstLights[nLight]->m_fRadius +
                                                    lstLights[nLight]->m_fRadius * lstLights[nLight]->m_fRadius), 1.0f);
                RenderFlat(pPrimitive);
                if (bAlphaTestEnabled)
                {
                    GLRender_Enable(ENABLE_ALPHA_TEST);
                }
                currentscene->m_pLightManager->EnableBumpMapLights();
            }
        }
        else
        {
            bumpmapvp->Enable(pPart, true);
            SetupForceSightShader(pPrimitive->m_pPart->m_pAurObject, MODEL_SHADER_FGLOW_BUMPMAP,
                                  MODEL_SHADER_DESAT_BUMPMAP, pPart->m_pMaterial->m_pBumpMap,
                                  currentscene->m_pNormalizationCubeMap,
                                  currentscene->m_pNormalizationCubeMap,
                                  pPart->m_pMaterial->m_pTexture0, false);
            RenderFlat(pPrimitive);
            GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
            SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        }

#ifndef _XBOX
        if (usearbvertexprograms)
        {
            glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS0);
            glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS1);
            glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS2);
        }
        else
#endif
        {
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
        }
        currentscene->DoFog();
#ifdef _XBOX
        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
#else
        GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
#endif
#ifndef _XBOX
        if (usearbvertexprograms)
        {
            glDisable(GL_VERTEX_PROGRAM_ARB);
        }
        else
#endif
        {
            GLRender_Disable(ENABLE_VERTEX_PROGRAM);
        }
    }
    else
    {
        GLRender_Disable(ENABLE_FOG);
        if (bumpdiffuse)
        {
            RenderDiffuseBumpMappedPass(pPrimitive);
        }

        static VertexProgram* standardvp = new VertexProgram(
            cStandardVP, cStandardVPDeclaration);
        if (bumpother)
        {
            currentscene->m_pLightManager->DisableBumpMapLights();
            standardvp->Enable(pPrimitive->m_pPart, true);
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
            SetBlendMode(BLEND_ONE, BLEND_ONE);
            RenderFlatNonTextured(pPrimitive);
            GLRender_Disable(ENABLE_VERTEX_PROGRAM);
            currentscene->m_pLightManager->EnableBumpMapLights();
        }
        if (bumptexture)
        {
            standardvp->Enable(pPrimitive->m_pPart, true);
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
            SetBlendMode(BLEND_DST_COLOR, BLEND_ZERO);
            RenderFlatDecal(pPrimitive);
            GLRender_Disable(ENABLE_VERTEX_PROGRAM);
        }
        if (bumpspecular)
        {
            RenderSpecularBumpMappedPass(pPrimitive);
        }
        currentscene->DoFog();
        int bFog;
#ifndef _XBOX
        GLRender_QueryIntegerv(fogModeTranslationTable[FOG_QUERY_ENABLED], &bFog);
#else

        GLRender_QueryIntegerv(queryTranslationTable[0], &bFog);
#endif
        if (bFog)
        {

            static VertexProgram* fogvp = new VertexProgram(
                cFogOutputVP, cFogOutputVPDeclaration);
            fogvp->Enable(pPrimitive->m_pPart, false);
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_FOG_RANGE,
                currentscene->m_fFogNear, currentscene->m_fFogFar,
                (float)currentscene->m_nFogMode, 1.0f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_FOG_COLOR,
                currentscene->m_vFogColor.m_fX, currentscene->m_vFogColor.m_fY,
                currentscene->m_vFogColor.m_fZ, 1.0f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_FOG_LIMITS,
                1.0f, 0.0f, 0.0f, 0.0f);
            SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            RenderFlatNonTextured(pPrimitive);
            GLRender_Disable(ENABLE_VERTEX_PROGRAM);
        }
        if (bAlphaTestEnabled)
        {
            GLRender_Enable(ENABLE_ALPHA_TEST);
        }
        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE,
            pPrimitive->m_pPart->m_pMaterial->m_pTexture0, 0, 0, 0);
    }
}

#ifndef _XBOX
void RenderNormalMapped(VertexPrimitiveFlat* pPrimitive)  // PC 0x00492F00 GOG 0x00492FD0
{
    if (currentscene == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }

    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    if (enablerenderbumpmap == 0 || enablerender == 0)
    {
        return;
    }

    List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
    if (lstLights.m_nCount == 0)
    {
        RenderFlat(pPrimitive);
        return;
    }

    int bAlphaTestEnabled = GLRender_IsEnabled(ENABLE_ALPHA_TEST);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    if (AurPixelShadersAvailable() && useonepasssolution)
    {
        PartLight** pLights = lstLights.m_pList;
        GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_RESET, 0, 0, 0, 0);
        if (usearbvertexprograms)
        {
            glDisable(GL_VERTEX_PROGRAM_ARB);
        }
        else
        {
            GLRender_Disable(ENABLE_VERTEX_PROGRAM);
        }
        PartTriMesh* pPart = pPrimitive->m_pPart->AsPartTriMesh();
        MdlNodeTriMesh* pNode = 0;
        if (pPart)
        {
            pNode = (MdlNodeTriMesh*)pPart->m_pNode;
        }
        Vector vSpecular;
        Vector vDiffuse;
        static VertexProgram* bumpmapvp =
            new VertexProgram(szNormalmapVPARB, szNormalmapVPARBDeclaration);
        if (usearbvertexprograms)
        {
            glEnable(GL_VERTEX_PROGRAM_ARB);
        }
        else
        {
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        }
        if (pNode)
        {
            vSpecular = pPart->m_pMaterial->GetBumpMapSpecularColor();
            vDiffuse = pNode->m_vDiffuse;
        }
        GLRender_EnablePixelShaders(MODEL_SHADER_NORMAL_MAP,
            pPart->m_pMaterial->m_pTexture0,
            pPart->m_pMaterial->m_pBumpMap,
            currentscene->m_pNormalizationCubeMap,
            currentscene->m_pNormalizationCubeMap);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_FOG,
            currentscene->m_fFogNear, currentscene->m_fFogFar,
            currentscene->m_bFog ? 1.0f : 0.0f, 0.5f);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_CONSTANTS,
            1.0f, -1.0f, 0.0f, 0.0f);

        Quaternion qWorld;
        Vector vWorld;
        pPart->GetWorld(vWorld, qWorld);
        Vector vEye = -qWorld * (CurrentCamera->m_vPosition - vWorld);
        for (int nLight = 0; nLight < lstLights.m_nCount; ++nLight)
        {
            currentscene->m_pLightManager->DisableBumpMapLights();
            bumpmapvp->Enable(pPart, true);
            Vector vLightPosition = pLights[nLight]->m_vWorldPosition;
            vLightPosition = -qWorld * (vLightPosition - vWorld);
            Vector& vLightColor = pLights[nLight]->m_vColor;
            Vector vColor = cmul(vLightColor,
                vDiffuse * diffuseintensity *
                ((CAurTexture*)pPart->m_pMaterial->m_pBumpMap.m_pObject)->GetDiffuseBumpIntensity());
            if (usebumpdiffuse)
            {
                GLRender_SetBumpLightColor(vColor, 1.0f);
            }
            else
            {
                Vector vRed(1.0f, 0.0f, 0.0f);
                GLRender_SetBumpLightColor(vRed, 1.0f);
            }
            vColor = cmul(lstLights[nLight]->m_vColor,
                vSpecular * specularintensity *
                ((CAurTexture*)pPart->m_pMaterial->m_pBumpMap.m_pObject)->GetSpecularBumpIntensity());
            if (usebumpspecular)
            {
                GLRender_SetPixelShaderAmbientColor(vColor, 1.0f);
            }
            else
            {
                Vector vGreen(0.0f, 1.0f, 0.0f);
                GLRender_SetPixelShaderAmbientColor(vGreen, 1.0f);
            }
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_LIGHT_POSITION,
                vLightPosition.m_fX, vLightPosition.m_fY, vLightPosition.m_fZ, 1.0f);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_EYE_POSITION,
                vEye.m_fX, vEye.m_fY, vEye.m_fZ, 1.0f);
            float fRadius = pLights[nLight]->m_fRadius;
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_ATTENUATION,
                1.0f, 0.0f, AUR_LIGHT_ATTENUATION_SCALE /
                (fRadius * fRadius + fRadius * fRadius), 1.0f);
            GLRender::ActivateTextureStage(1);
            GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender::ActivateTextureStage(0);
            RenderFlat(pPrimitive);
            GLRender::ActivateTextureStage(1);
            GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender::ActivateTextureStage(0);
            if (bAlphaTestEnabled)
            {
                GLRender_Enable(ENABLE_ALPHA_TEST);
            }
            currentscene->m_pLightManager->EnableBumpMapLights();
        }
        currentscene->DoFog();
        GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_RESET, 0, 0, 0, 0);
        if (usearbvertexprograms)
        {
            glDisable(GL_VERTEX_PROGRAM_ARB);
        }
        else
        {
            GLRender_Disable(ENABLE_VERTEX_PROGRAM);
        }
        if (showrancoraxes)
        {
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
            glLineWidth(5.0f);
            glBegin(GL_LINES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(5.0f, 0.0f, 0.0f);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 5.0f, 0.0f);
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 5.0f);
            GLRender_EndImmediatePrimitive();
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);
        }
    }
}
#endif

#ifdef _XBOX
void RenderMotionBlurred(VertexPrimitiveFlat* pPrimitive);

void PartTriMesh::Draw(bool bMotionBlurred)
{
    if (rendertrimesh == 0 && AsPartDanglyMesh() == 0 && m_pAurObject != 0)
    {
        return;
    }
    if (IsTriMeshRenderable(this) == 0)
    {
        return;
    }

    MdlNodeTriMesh* pNode = m_pNode->AsMdlNodeTriMesh();
    unsigned char bHasBumpMap = m_pMaterial->HasBumpMap();
    unsigned char bHasBumpyShinyMap = m_pMaterial->HasBumpyShinyMap();
    unsigned char bHasTangentBases = pNode->HasTangentBases();

    TransformModelviewMatrix();
    TransformTextureMatrix();
    m_pVertexPrimitive->SetupVertexArraysStandard();

    if (!bMotionBlurred)
    {
    if (bHasBumpyShinyMap && bHasBumpMap && bHasTangentBases && enablebumpyshiny)
    {
        RenderEMBM(m_pVertexPrimitive->AsVertexPrimitiveFlat());
    }
    else if (pNode->m_bLightMapped && enablelightmap && bHasBumpMap && bHasTangentBases && enablebumpmap)
    {
        if (m_pMaterial->m_nRenderType != 0)
        {
            RenderLightMappedBumpSpecular(m_pVertexPrimitive->AsVertexPrimitiveFlat());
        }
        else
        {
            RenderLightMappedBumpDiffuse(m_pVertexPrimitive->AsVertexPrimitiveFlat());
        }
    }
    else if (bHasBumpMap && bHasTangentBases && enablebumpmap)
    {
        RenderBumpMapped(m_pVertexPrimitive->AsVertexPrimitiveFlat());
    }
    else if (pNode->m_bLightMapped && enablelightmap &&
             ((m_pAurObject != 0 && m_pAurObject->m_pEnvironmentMap.m_pObject != 0) || m_pMaterial->HasEnvMap()))
    {
        RenderEMLM(m_pVertexPrimitive->AsVertexPrimitiveFlat());
    }
    else if ((m_pAurObject != 0 && m_pAurObject->m_pEnvironmentMap.m_pObject != 0) || m_pMaterial->HasEnvMap())
    {
        RenderEnvironmentMapped(m_pVertexPrimitive->AsVertexPrimitiveFlat(), false);
    }
    else if (pNode->m_bLightMapped && enablelightmap)
    {
        RenderLightMapped(m_pVertexPrimitive->AsVertexPrimitiveFlat());
    }
    else
    {
        RenderFlat(m_pVertexPrimitive->AsVertexPrimitiveFlat());
    }
    }
    else
    {
        RenderMotionBlurred(m_pVertexPrimitive->AsVertexPrimitiveFlat());
    }

    if (m_pAurObject != 0 && m_pAurObject->m_fBumpedOutShellHeight != 0.0f && currentscene != 0)
    {
        countparts++;
        pNode = m_pVertexPrimitive->AsVertexPrimitiveFlat()->m_pNode;
        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMPED_OUT_SHELL,
            m_pAurObject->m_fBumpedOutShellHeight, 0.0f, 0.0f, 1.0f);
#ifdef _XBOX
        static VertexProgram* bumpedoutvp = new VertexProgram(cBumpedOutVP, cBumpedOutVPDeclaration);
#else
        static VertexProgram* bumpedoutvp = new VertexProgram(cBumpedOutVP, cStandardVPDeclaration);
#endif
        bumpedoutvp->Enable(this, false);
        ((CAurTexture*)m_pMaterial->m_pTexture3.m_pObject)->Bind();
        if (m_pMaterial->m_lstBlendSrc.m_nCount > 1)
        {
            m_pMaterial->SetBlendingMode(1);
        }

        for (int nArray = 0; nArray < pNode->m_lstIndexCounts.m_nCount; nArray++)
        {
            int nCount = pNode->m_lstIndexCounts[nArray];
            GLRender::DrawElements((Aurora::AuroraPrimitiveTypes)pNode->m_nPrimitiveType, nCount, 0,
                                   pNode->m_lstIndexPools[nArray]);
            countfaces += nCount / 3;
        }
        if (pNode->m_nPrimitiveType == GL_TRIANGLES && pNode->m_nAuxIndexCount > 0)
        {
            GLRender::DrawElements((Aurora::AuroraPrimitiveTypes)GL_LINE_STRIP, pNode->m_nAuxIndexCount,
                pNode->m_nVertexDataOffset, pNode->m_nAuxIndexPool);
        }
        GLRender_Disable(ENABLE_VERTEX_PROGRAM);
    }

    RestoreTextureMatrix();
    RestoreModelviewMatrix();
}
#else
void PartTriMesh::Draw(bool bRenderChildren)  // PC 0x00493A30 GOG 0x00493B00
{
    if (rendertrimesh == 0 && AsPartDanglyMesh() == 0 && m_pAurObject != 0)
    {
        return;
    }
    if (IsTriMeshRenderable(this) == 0)
    {
        return;
    }

    MdlNodeTriMesh* pNode = m_pNode->AsMdlNodeTriMesh();
    m_pMaterial->HasBumpMap();
    m_pMaterial->HasBumpyShinyMap();
    pNode->HasTangentBases();

    TransformModelviewMatrix();
    TransformTextureMatrix();
    m_pVertexPrimitive->SetupVertexArraysStandard();

    int nRenderPath = GetRenderPath();
    if (m_bHologram && AurNV2xLevelShadersAvailable())
    {
        if (!pNode->m_bHideInHolograms)
            RenderHologramEffect(nRenderPath, m_pVertexPrimitive->AsVertexPrimitiveFlat());
    }
    else
    {
        switch (nRenderPath)
        {
        case RENDER_EMBM:
            RenderEMBM(m_pVertexPrimitive->AsVertexPrimitiveFlat());
            break;
        case RENDER_LIGHTMAPPED_BUMP_SPECULAR:
            RenderLightMappedBumpSpecular(m_pVertexPrimitive->AsVertexPrimitiveFlat());
            break;
        case RENDER_LIGHTMAPPED_BUMP_DIFFUSE:
            RenderLightMappedBumpDiffuse(m_pVertexPrimitive->AsVertexPrimitiveFlat());
            break;
        case RENDER_NORMALMAPPED:
            RenderNormalMapped(m_pVertexPrimitive->AsVertexPrimitiveFlat());
            break;
        case RENDER_BUMPMAPPED:
            RenderBumpMapped(m_pVertexPrimitive->AsVertexPrimitiveFlat());
            break;
        case RENDER_BUMPMAP_FALLBACK:
            RenderBumpmapFallback(m_pVertexPrimitive);
            break;
        case RENDER_EMLM:
            RenderEMLM(m_pVertexPrimitive->AsVertexPrimitiveFlat());
            break;
        case RENDER_NV1X_EMLM:
            RenderNV1xEMLM(m_pVertexPrimitive->AsVertexPrimitiveFlat());
            break;
        case RENDER_ENVIRONMENTMAPPED:
            RenderEnvironmentMapped(m_pVertexPrimitive->AsVertexPrimitiveFlat(), false, false);
            break;
        case RENDER_LIGHTMAPPED_NO_SHADERS:
            RenderLightMappedNoShaders(m_pVertexPrimitive->AsVertexPrimitiveFlat());
            break;
        case RENDER_LIGHTMAPPED:
            RenderLightMapped(m_pVertexPrimitive->AsVertexPrimitiveFlat());
            break;
        case RENDER_EMLM_VP:
            RenderEMLMVP(m_pVertexPrimitive, this);
            break;
        case RENDER_LIGHTMAPPED_VP:
            RenderLightmappedVP(m_pVertexPrimitive, this);
            break;
        case RENDER_FLAT_VP:
            RenderFlatVP(m_pVertexPrimitive, this);
            break;
        case RENDER_ENVIRONMENTMAPPED_VP:
            RenderEnvironmentMappedVP(m_pVertexPrimitive, this);
            break;
        case RENDER_NOISEMAPPED:
            RenderNoiseMapped(m_pVertexPrimitive->AsVertexPrimitiveFlat(),
                              pNode->m_nDirtTexture - 1, pNode->m_nDirtCoordSpace);
            break;
        case RENDER_FLAT:
            if (g_ForceSight)
            {
                SetupForceSightShader(m_pAurObject, MODEL_SHADER_FGLOW, MODEL_SHADER_DESAT, 0, 0, 0, 0, false);
                RenderFlat(m_pVertexPrimitive->AsVertexPrimitiveFlat());
                GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
                SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                GLRender_SetDepthMask(1);
            }
            else
            {
                if (m_pAurObject && m_pAurObject->IsIlluminationEnable() && AurNV1xLevelShadersAvailable())
                {
                    float afColor[4];
                    afColor[0] = 0.0f;
                    afColor[1] = 0.0f;
                    afColor[2] = 0.0f;
                    afColor[3] = 1.0f;
                    m_pAurObject->GetIlluminationColor(afColor[0], afColor[1], afColor[2]);
                    SetPerInstanceIllumination(afColor);
                    GLRender_EnablePixelShaders(MODEL_SHADER_ILLUM, 0, 0, 0, 0);
                }
                RenderFlat(m_pVertexPrimitive->AsVertexPrimitiveFlat());
                if (m_pAurObject && m_pAurObject->IsIlluminationEnable())
                    GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
            }
            break;
        }
    }

    if (AurBumpyShinyAvailable() && m_pAurObject != 0 &&
        m_pAurObject->m_fBumpedOutShellHeight != 0.0f && currentscene != 0)
    {
        countparts++;
        pNode = m_pVertexPrimitive->AsVertexPrimitiveFlat()->m_pNode;
        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        AurSetVertexProgramParameter4f(VertexProgram::PARAM_BUMPED_OUT_SHELL,
            m_pAurObject->m_fBumpedOutShellHeight, 0.0f, 0.0f, 1.0f);
        g_pBumpedOutVP->Enable(this, false);
        ((CAurTexture*)m_pMaterial->m_pTexture3.m_pObject)->Bind();
        if (m_pMaterial->m_lstBlendSrc.m_nCount > 1)
        {
            m_pMaterial->SetBlendingMode(1);
        }
        if (g_ForceSight)
            SetupForceSightShader(m_pAurObject, MODEL_SHADER_FGLOW, MODEL_SHADER_DESAT, 0, 0, 0, 0, false);

        for (int nArray = 0; nArray < pNode->m_lstIndexCounts.m_nCount; nArray++)
        {
            int nCount = pNode->m_lstIndexCounts[nArray];
            GLRender::DrawElements((Aurora::AuroraPrimitiveTypes)pNode->m_nPrimitiveType, nCount, 0,
                                   pNode->m_lstIndexPools[nArray]);
            countfaces += nCount / 3;
        }
        if (pNode->m_nPrimitiveType == GL_TRIANGLES && pNode->m_nAuxIndexCount > 0)
        {
            GLRender::DrawElements((Aurora::AuroraPrimitiveTypes)GL_LINE_STRIP, pNode->m_nAuxIndexCount,
                pNode->m_nVertexDataOffset, pNode->m_nAuxIndexPool);
        }
        if (g_ForceSight)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            GLRender_SetDepthMask(1);
        }
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        GLRender_SetDepthMask(1);
        GLRender_Disable(ENABLE_VERTEX_PROGRAM);
    }

    RestoreTextureMatrix();
    RestoreModelviewMatrix();
}
#endif
// PC 0x00493F70 GOG 0x00494030
void PartDanglyMesh::Draw(bool bRenderChildren)
{
    if (renderdangly != 0)
    {
        m_pNode->AsMdlNodeDanglyMesh();
        PartTriMesh::Draw(bRenderChildren);
    }
}

void PartSkin::Project(const Vector& vPosition, float fDistance,
                       const Plane& plane, CAurShadowData* pShadowData)
{
    if (!doskinshadows)
        return;

    MdlNodeTriMesh* pMeshNode = m_pVertexPrimitive->m_pNode;
    int bBeaming = pMeshNode->m_bBeaming;
    if (pShadowData)
        pShadowData->m_nEdges = 0;

    static Vector vp2v0;
    static Vector vp2v1;
    static Vector invVp;
    static Vector cap;
    static Vector sink;
    invVp = normalize(-vPosition);
    cap = invVp * epsilon;
    sink = invVp * fDistance + vPosition;
    if (!bBeaming && !ClipProjectionCaps(cap, sink, plane))
        return;

    if (!useshadowmethod || !usevertexprograms || !AurVertexProgramsAvailable())
        return;

    GLRender_EnableClientState(CLIENT_STATE_SKIN_WEIGHT);
    GLRender_EnableClientState(CLIENT_STATE_BONE_INDEX);
    PartSkin* pSkin = m_pVertexPrimitive->m_pPart->AsPartSkin();
    MdlNodeSkin* pNode = (MdlNodeSkin*)pSkin->m_pNode;
    pSkin->AsPartTriMesh();
    m_pVertexPrimitive->SetupVertexArraysStandard();
    Matrix mRotation;
    static List<Quaternion> qbones(0);
    static List<Vector> tbones(0);
    qbones.m_nCount = 0;
    tbones.m_nCount = 0;
    AurSetVertexProgramParameter4f(SKIN_VP_CONSTANTS, 0.0f, 2.0f, 3.0f, 1.0f);

    Part* pPart = pSkin;
    Vector vTranslation = -pPart->m_qOrientation * -pPart->m_vPosition;
    Quaternion qRotation = -pPart->m_qOrientation;
    while (pPart->m_pParent != 0)
    {
        pPart = pPart->m_pParent;
        vTranslation = vTranslation + qRotation *
            (-pPart->m_qOrientation * -pPart->m_vPosition);
        qRotation = qRotation * -pPart->m_qOrientation;
    }
    GatherBones(pPart, tbones, qbones, vTranslation, qRotation);
    for (int i = 0; i < pNode->m_nBoneMap; i++)
    {
        int nBone = (int)pNode->m_pfBoneMap[i];
        if (nBone != MdlNodeSkin::UNMAPPED_BONE)
        {
            tbones[i] = tbones[i] + qbones[i] * pNode->m_lstTBoneRefInv[i];
            qbones[i] = qbones[i] * pNode->m_lstQBoneRefInv[i];
            mRotation = qbones[i].getmatrix();
            Vector& vBoneTranslation = tbones[i];
            int nRegister = SKIN_VP_BONES + 3 * nBone;
            AurSetVertexProgramParameter4f(nRegister,
                mRotation.m_vFirst.m_fX, mRotation.m_vSecond.m_fX,
                mRotation.m_vThird.m_fX, vBoneTranslation.m_fX);
            AurSetVertexProgramParameter4f(++nRegister,
                mRotation.m_vFirst.m_fY, mRotation.m_vSecond.m_fY,
                mRotation.m_vThird.m_fY, vBoneTranslation.m_fY);
            AurSetVertexProgramParameter4f(++nRegister,
                mRotation.m_vFirst.m_fZ, mRotation.m_vSecond.m_fZ,
                mRotation.m_vThird.m_fZ, vBoneTranslation.m_fZ);
        }
    }
    GLRender_SetFrontFace(CULL_CLOCKWISE);
    GLRender_SetStencilOp(STENCIL_KEEP,
        STENCIL_INCREMENT,
        STENCIL_KEEP);
    m_pVertexPrimitive->AsVertexPrimitiveSkinned();
    GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
    GLRender_SetStencilOp(STENCIL_KEEP,
        STENCIL_DECREMENT,
        STENCIL_KEEP);
    m_pVertexPrimitive->AsVertexPrimitiveSkinned();
}
// PC 0x00494C50 GOG 0x00494D10
void Gob::Render(bool bRenderLast)
{
    if (!enablegobrender)
        return;

    if (!m_pRootPart)
    {
        m_vPreviousPosition = m_vPosition;
        m_qPreviousOrientation = m_qOrientation;
        return;
    }
    if (!enablerendercharacter && m_pModel &&
            bool(m_pModel->m_Classification.m_bCharacter))
    {
        m_vPreviousPosition = m_vPosition;
        m_qPreviousOrientation = m_qOrientation;
        return;
    }
    if (!m_bHasGeometry && !enableplaceholders)
    {
        m_vPreviousPosition = m_vPosition;
        m_qPreviousOrientation = m_qOrientation;
        return;
    }
    if (g_bFadeDoors && bool(m_pModel->m_Classification.m_bDoor) && currentscene)
    {
        Vector vCamera = CurrentCamera->m_vPosition;
        Vector vFocus = currentscene->m_vSceneFocus;
        CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
        hitInfo.m_bResult1 = 0;
        if (HitCheck(vCamera, vFocus, &vFocus, &hitInfo))
            m_fOcclusionAlpha -= DeltaT / fadeToZeroTime;
        else
            m_fOcclusionAlpha += DeltaT / fadeToZeroTime;
        m_fOcclusionAlpha = max(mindooralpha, min(1.0f, m_fOcclusionAlpha));
    }

    DoEvent("GetPersonalRadius", &m_fPersonalSpaceRadius);
    DoEvent("GetCreatureRadius", &m_fPersonalSpaceRadius2);
    DoEvent("GetPath", &m_PathInfo);
    countgobs++;
    m_bRendered = 1;
    GLRender_PushCurrentMatrix();
#ifdef _XBOX
    Transform(m_vPosition, m_qOrientation, 1);
#else
    if (enabletransform)
        GLRender::Transform(m_vPosition, m_qOrientation, 1);
#endif
    if (m_fScale != 1.0f)
        GLRender::Scale(m_fScale, m_fScale, m_fScale);
    if (m_bSingleTexture)
    {
        m_pSingleTexture->BindTexture0();
        m_pSingleTexture->SetBlendingMode(0);
    }
    ProxyPartDraw(m_pRootPart, false);
    if (m_bMotionBlurred)
        blurbucket.Add(this);
    else
    {
        m_vPreviousPosition = m_vPosition;
        m_qPreviousOrientation = m_qOrientation;
    }
    if (rendergobspheres == 1)
    {
        float fRadius = m_fMinimumBoundingSphereRadius;
        Vector vColor(1.0f, 1.0f, 0.0f);
        GLRender::DrawSphereBound(fRadius,
                                 m_vMinimumBoundingSphere, vColor, 1.0f, true);
    }
    if (rendergobbbox || m_bRenderGobBBox)
        GLRender::DrawAxisAlignedFrame(m_vBoundingBoxMinimum,
                                      m_vBoundingBoxMaximum, m_vRenderGobBBox);
    GLRender_PopCurrentMatrix();
    if (bRenderLast)
        DoEvent("propagaterender", 0);
}
// PC 0x004015A0 GOG 0x004015A0
void Part::Mat()
{
}
// PC 0x00693D50 GOG 0x00693AC0
void Part::Project(const Vector& vPoint, float fDistance, const Plane& cPlane,
                   CAurShadowData* pShadowData)
{
}

static inline void AurmodelrenderTrace1()
{
}

#ifdef _XBOX

void Gob::RenderVisibilityBounds()
{
    GLRender_Disable(ENABLE_CULL_FACE);
    GLRender::PushCurrentMatrix();
    Transform(m_vPosition, m_qOrientation, 1);
    if (m_fScale != 1.0f)
    {
        GLRender::Scale(m_fScale, m_fScale, m_fScale);
    }
    Vector vMaximum = m_vBoundingBoxMaximum;
    if (m_vBoundingBoxMaximum.m_fZ <= 0.1f)

    {
        vMaximum.m_fZ = 0.1f;
    }
    GLRender::DrawAxisAlignedFrame(m_vBoundingBoxMinimum, vMaximum, m_vRenderGobBBox);
    GLRender::PopCurrentMatrix();
    GLRender_Enable(ENABLE_CULL_FACE);
}
#endif

#ifdef _XBOX
Gob* FindGob(char* sName);

char* setspecularexponent(char* sArguments)
{
    int nExponent = atoi(sArguments);
    if (nExponent != 1 && nExponent != 2 && nExponent != 4 && nExponent != 8)
    {
        return "Exponent must be one of 1,2,4,8";
    }
    bmspecularexponent = nExponent;
    return "Okey dokey.";
}

char* turnoffshadows(char* sArguments)
{
    static char sResult[256];
    char sName[64];

    sName[0] = 0;
    if (sscanf(sArguments, "%s", sName) != 1)
    {
        return "Usage: turnoffshadowing <gobname>";
    }
    Gob* pGob = FindGob(sName);
    if (pGob == NULL)
    {
        sprintf(sResult, "couldn't find object %s", sName);
        return sResult;
    }
    pGob->TurnOffShadows();
    return "Success, shadowing turned off for specified gob.";
}

char* turnonshadows(char* sArguments)
{
    static char sResult[256];
    char sName[64];

    sName[0] = 0;
    if (sscanf(sArguments, "%s", sName) != 1)
    {
        return "Usage: turnonshadowing <gobname>";
    }
    Gob* pGob = FindGob(sName);
    if (pGob == NULL)
    {
        sprintf(sResult, "couldn't find object %s", sName);
        return sResult;
    }
    pGob->TurnOnShadows();
    return "Success, shadowing turned on for specified gob.";
}

char* setpersonalradius(char* sArguments)
{
    char sName[64];
    float fRadius = 0.0f;

    sscanf(sArguments, "%s %f", sName, &fRadius);
    Gob* pGob = FindGob(sName);
    if (pGob == NULL)
    {
        return "no object by that name";
    }
    pGob->m_fPersonalSpaceRadius = fRadius;
    return "done";
}

char* setcreatureradius(char* sArguments)
{
    char sName[64];
    float fRadius = 0.0f;

    sscanf(sArguments, "%s %f", sName, &fRadius);
    Gob* pGob = FindGob(sName);
    if (pGob == NULL)
    {
        return "no object by that name";
    }
    pGob->m_fPersonalSpaceRadius2 = fRadius;
    return "done";
}

char* setpath(char* sArguments)
{
    char sName[64];

    sscanf(sArguments, "%s", sName);
    Gob* pGob = FindGob(sName);
    if (pGob == NULL)
    {
        return "no object by that name";
    }
    pGob->m_PathInfo.m_nCoordinates = 12;
    pGob->m_PathInfo.m_pfCoordinates = testpath;
    return "done";
}

static void SetPartAlpha(Part* pPart, float fAlpha)
{
    if (pPart->AsPartTriMesh())
    {
        pPart->AsPartTriMesh()->m_fAlpha = fAlpha;
    }
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
        SetPartAlpha(pPart->m_lstChildren[i], fAlpha);
    }
}

char* setalpha(char* sArguments)
{
    char sName[64];
    float fAlpha = 1.0f;

    sscanf(sArguments, "%s %f", sName, &fAlpha);
    Gob* pGob = FindGob(sName);
    if (pGob == NULL)
    {
        return "no object by that name";
    }
    SetPartAlpha(pGob->m_pRootPart, fAlpha);
    return "done";
}

char* setenvironmentmap(char* sArguments)
{
    static char sResult[256];
    char sTexture[64];
    char sName[64];

    sTexture[0] = 0;
    sName[0] = 0;
    sscanf(sArguments, "%s %s", sName, sTexture);
    Gob* pGob = FindGob(sName);
    if (pGob == NULL)
    {
        sprintf(sResult, "couldn't find object %s", sName);
        return sResult;
    }
    if (stricmp("NULL", sTexture) == 0 || stricmp("0", sTexture) == 0)
    {
        pGob->m_pEnvironmentMap = NULL;
        sprintf(sResult, "Setting object %s environmentmap to 0", sName);
        return sResult;
    }
    if (!pGob->SetEnvironmentMap(sTexture))
    {
        return "Bad texture!  Environment map not changed.";
    }
    sprintf(sResult, "bitmap %s  %s  texid=%d", sArguments, pGob->m_pEnvironmentMap.m_pObject ? "found" : "not found",
            pGob->m_pEnvironmentMap.m_pObject);
    return sResult;
}
#endif
