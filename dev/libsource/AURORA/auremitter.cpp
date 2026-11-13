#define WIN32_LEAN_AND_MEAN
#include "auremitter.h"
#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "aurrendergl.h"
#include <stdio.h>
#include "aurwindmanager.h"

template void List<Vector>::DelIndex(int);

template <class T> inline void AurEmitterCopy(T* p) { T copy(*p); }
inline void AurEmitterCopyParticles(Particle* pParticle, ChunkyParticle* pChunky) { AurEmitterCopy(pParticle); AurEmitterCopy(pChunky); }

#include "exoresfile.h"

#include "aurcamera.h"
#include "aurmodel.h"
#include "aurmaterial.h"
#include "aurparse.h"

static inline void AuremitterTrace4();
static inline void AuremitterTrace3();
static inline void AuremitterTrace2();
static inline void AuremitterTrace1();

#pragma inline_recursion(on)
#pragma inline_depth(10)

static int NORMAL = 1;
static int LocalZ = 2;
static int WorldZ = 3;
static int Linked = 4;
static int Aligned = 5;
static int LIGHTNING = 6;
static int OTHER = 7;
static int AlignedToParticleDir = 8;
static int MotionBlurred = 9;
CAurCamera* g_pWeatherCamera = 0;
int totalparticles = 0;
int totalemitters = 0;
int particlecount = 0;
unsigned int PartEmitter::particlePoolToken = 0x11111111;
float emitterfadeheight = 5.0f;
int auremitterrenderlightning = 1;

int xsize = 0;
int ysize = 0;
FILE* pAurLogFile = 0;
int emrendernew = 0;

int auremitterrenderbezier = 0;
int auremitterrendertangents = 0;
int auremitterrenderbranchbezier = 0;
float birth = 0;
float randVal = 0.3f;
int EmitterBounceType = 1;
int renderemitterspheres = 0;
int useoldlightninginterp = 0;
// PC 0x004B82A0 GOG 0x004B83B0
void Detonate(CAurObject* pObject, char* sEvent, void* pData)
{
    static_cast<PartEmitter*>(pData)->m_bDetonated = 1;
}
// PC 0x004B82B0 GOG 0x004B83C0
unsigned char MdlNodeEmitter::IsAffectedByWind()
{
    unsigned char bAffectedByWind = m_EmitterFlags.m_bAffectedByWind;
    return bAffectedByWind;
}

unsigned char MdlNodeEmitter::IsRandom()
{
#ifdef _XBOX
    unsigned char bRandom = m_EmitterFlags.m_bRandom;
    return bRandom;
#else
    return (unsigned char)m_EmitterFlags.m_bRandom;
#endif
}
// PC 0x004B82C0 GOG 0x004B83D0
void MdlNodeEmitter::EnablePointToPoint()
{
    m_EmitterFlags.m_bPointToPoint = 1;
}
// PC 0x004B82D0 GOG 0x004B83E0
void MdlNodeEmitter::EnablePointToPointSel()
{
    m_EmitterFlags.m_bPointToPointSel = 1;
}
// PC 0x004B82E0 GOG 0x004B83F0
void MdlNodeEmitter::EnableAffectedByWind()
{
    m_EmitterFlags.m_bAffectedByWind = 1;
}
// PC 0x004B82F0 GOG 0x004B8400
void MdlNodeEmitter::EnableTinted()
{
    m_EmitterFlags.m_bTinted = 1;
}
// PC 0x004B8300 GOG 0x004B8410
void MdlNodeEmitter::EnableBouncing()
{
    m_EmitterFlags.m_bBouncing = 1;
}
// PC 0x004B8310 GOG 0x004B8420
void MdlNodeEmitter::EnableRandom()
{
    m_EmitterFlags.m_bRandom = 1;
}
// PC 0x004B8320 GOG 0x004B8430
void MdlNodeEmitter::EnableRandomOrder()
{
    m_EmitterFlags.m_bRandomOrder = 1;
}
// PC 0x004B8330 GOG 0x004B8440
void MdlNodeEmitter::EnableInheriting()
{
    m_EmitterFlags.m_bInheriting = 1;
}
// PC 0x004B8340 GOG 0x004B8450
void MdlNodeEmitter::EnableInheritingVelocity()
{
    m_EmitterFlags.m_bInheritingVelocity = 1;
}
// PC 0x004B8350 GOG 0x004B8460
void MdlNodeEmitter::EnableInheritingLocal()
{
    m_EmitterFlags.m_bInheritingLocal = 1;
}
// PC 0x004B8360 GOG 0x004B8470
void MdlNodeEmitter::EnableSplat()
{
    m_EmitterFlags.m_bSplat = 1;
}
// PC 0x004B8370 GOG 0x004B8480
void MdlNodeEmitter::EnableInheritingPart()
{
    m_EmitterFlags.m_bInheritingPart = 1;
}
// PC 0x004B8380 GOG 0x004B8490
void MdlNodeEmitter::EnableDepthTexture()
{
    m_EmitterFlags.m_bDepthTexture = 1;
}
// PC 0x004B8390 GOG 0x004B84A0
Particle::~Particle()
{
    --totalparticles;
}
// PC 0x004B83A0 GOG 0x004B84B0
void Particle::randomPosition(int nWidth, int nHeight)
{
    int nX;
    int nY;

    if (nWidth > 0)
    {
        nX = rand() * rand() % (int)(nWidth * 0.5f);
    }
    else
    {
        nX = 0;
    }

    nWidth = nX;

    if (nHeight > 0)
    {
        nY = rand() * rand() % (int)(nHeight * 0.5f);
    }
    else
    {
        nY = 0;
    }

    nHeight = nY;

    if (rand() % 2 > 0)
    {
        nWidth = -nX;
    }

    if (rand() % 2 > 0)
    {
        nHeight = -nY;
    }

    PartEmitter* pEmitter = m_pEmitter;
    double dY = nHeight * 0.0001f;
    Vector vOffsetY = pEmitter->m_vYAxis * dY;
    double dX = nWidth * 0.0001f;
    Vector vOffsetX = pEmitter->m_vXAxis * dX;

    m_vPosition = vOffsetX + pEmitter->m_vOrigin + vOffsetY;
}
// PC 0x004B8B90 GOG 0x004B8CA0
void Particle::inheritVelocity(float fTime)
{
    Quaternion& qOrientation = m_pEmitter->m_qWorldOrientation;
    Vector vForward = qOrientation.zdir();
    Vector vTurn = m_pEmitter->m_vDirection - vForward;
    Vector vOffset = m_pEmitter->m_vOrigin - m_vPosition;
    float fScale;

    if (fTime > 0.0f)
    {
        fScale = 1.0f / fTime;
    }
    else
    {
        fScale = 0.01f;
    }

    float fDistance = sqrt(vOffset * vOffset);
    Vector vSlide = vTurn * fDistance * fScale;
    Vector vStep = (m_pEmitter->m_vOrigin - m_pEmitter->m_vLastOrigin) * fScale;

    m_vVelocity = vStep + vSlide;
    m_fSpeed = sqrt(m_vVelocity.m_fX * m_vVelocity.m_fX +
                    m_vVelocity.m_fY * m_vVelocity.m_fY +
                    m_vVelocity.m_fZ * m_vVelocity.m_fZ);

    if (m_fSpeed != 0.0f)
    {
        m_vVelocity = Vector(m_vVelocity.m_fX / m_fSpeed,
                             m_vVelocity.m_fY / m_fSpeed,
                             m_vVelocity.m_fZ / m_fSpeed);
    }
    else
    {
        m_vVelocity = Vector(0.0f, 0.0f, 0.0f);
    }
}

#ifdef _XBOX
inline float PartEmitter::GetRadius()
#else
float PartEmitter::GetRadius()
#endif
{
    return m_fRadius;
}
// PC 0x004B8510 GOG 0x004B8620
ChunkyParticle::~ChunkyParticle()
{
    if (m_pGob != 0)
    {
        delete m_pGob;
        m_pGob = 0;
    }
}
// PC 0x004B8570 GOG 0x004B8680
float PartEmitter::GetTileAlpha()
{
    float fAlpha = 1.0f;
    TileData* pTile = 0;

    if (m_pTileData != 0)
    {
        pTile = m_pTileData;
    }
    else if (m_pAurObject != 0 && ((Gob*)m_pAurObject)->m_pTileData != 0)
    {
        pTile = ((Gob*)m_pAurObject)->m_pTileData;
    }

    if (pTile != 0)
    {
        if (m_vOrigin.m_fZ - pTile->m_vPosition.m_fZ > emitterfadeheight)
        {
            fAlpha = pTile->m_fHeightFadeAlpha;
        }
    }

    return fAlpha;
}
// PC 0x004B85C0 GOG 0x004B86D0
void PartEmitter::Animate(float fTimeStep)
{
    Part::Animate(fTimeStep);
}
// PC 0x004B85D0 GOG 0x004B86E0
void PartEmitter::chkRandom(Particle* pParticle)
{
    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);

    if (pNode->IsRandom())
    {
        int nRange = (int)(m_fFrameEnd - m_fFrameStart) + 1;

        if (nRange < 0)
        {
            nRange = -nRange;
        }

        if (nRange != 0)
        {
#ifdef _XBOX
            pParticle->m_nFrame =
                (int)((float)(rand() % nRange) + m_fFrameStart);
#else
            pParticle->m_nFrame =
                (int)((double)(rand() % nRange) + m_fFrameStart);
#endif
        }
    }
}
// PC 0x004B8630 GOG 0x004B8740
int PartEmitter::GetNeighborFrame(int nFrame)
{
    if (m_fFrameStart != m_fFrameEnd)
    {
        if (m_fFrameEnd < m_fFrameStart)
        {
            int nNext = nFrame + 1;
            if (nNext <= m_fFrameStart)
                return nNext;
        }
        else
        {
            int nPrev = nFrame - 1;
            if (nPrev >= m_fFrameStart)
                return nPrev;
        }
    }
    return -1;
}
// PC 0x004B8680 GOG 0x004B8790
static RenderVertexVTC* ResizeEmitterPool(RenderVertexVTC* pVertices,
                                   unsigned long nCurrentSize,
                                   unsigned long nNewSize)
{
    RenderVertexVTC* pNewVertices =
        static_cast<RenderVertexVTC*>(operator new(24 * nNewSize));
    memcpy(pNewVertices, pVertices, 24 * nCurrentSize);
    operator delete(pVertices);
    return pNewVertices;
}

void PartEmitter::moveParticle(Particle* pParticle, float fTimeStep,
                               int bInheriting)
{
    if (pParticle->m_bReserved6C)
        return;

    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);
    Vector vPosition;
    if (bInheriting)
    {
        vPosition = (m_qOrientation * -m_qWorldOrientation) *
                    (pParticle->m_vPosition - m_vLastOrigin);
        vPosition += m_vOrigin;
    }
    else
    {
        vPosition = pParticle->m_vPosition;
    }

    pParticle->m_vPosition = vPosition + pParticle->m_vDrift * fTimeStep;
    pParticle->m_vDrift.m_fZ -= fTimeStep * m_fMass * (-EMITTER_GRAVITY);

    m_pHitInfo->ClearOutput();
    if (pNode->m_EmitterFlags.m_bBouncing)
    {
        Vector vStart;
        if (currentscene->HitCheck(vStart, pParticle->m_vPosition,
                                   &pParticle->m_vPosition, m_pHitInfo))
        {
            pParticle->m_vDrift = (m_pHitInfo->m_vOutputNormal * -2.0f *
                (m_pHitInfo->m_vOutputNormal * pParticle->m_vDrift) +
                pParticle->m_vDrift) * m_fBounceCoefficient;
        }
    }
}

#define EMITTER_DEADSPACE_DEBUG 0
// PC 0x004B86F0 GOG 0x004B8800
void Particle::randomDirection(int nWidth, int nHeight)
{
    float fSpread = m_pEmitter->m_fSpread * 0.5f;
    Vector vCameraDirection = normalize(
        m_vPosition - CurrentCamera->Gob::m_vPosition);
    Quaternion qNewOrientation;

    int nAngle = rand() % (1 - (int)(fSpread * -100.0f));
    m_qOrientation = Quaternion(m_pEmitter->m_vXAxis, nAngle * 0.01f) * m_qOrientation;
    int nRotation = rand() % 628;
    m_qOrientation = Quaternion(m_pEmitter->m_vDirection, nRotation * 0.01f) * m_qOrientation;

    if (EMITTER_DEADSPACE_ENABLED &&
        static_cast<MdlNodeEmitter*>(m_pEmitter->m_pNode)->m_fDeadSpace != 0.0f)
    {
        float fFacing = m_qOrientation.ydir() * vCameraDirection;
        qNewOrientation = m_qOrientation;

        while ((fFacing < 0.0f ? -fFacing : fFacing) <
               cos(1.57079632 - static_cast<MdlNodeEmitter*>(m_pEmitter->m_pNode)->m_fDeadSpace))
        {
            float fAngle;
            if (EMITTER_DEADSPACE_DEBUG)
            {
                Vector vDelta = vCameraDirection - vCameraDirection;
                Vector vYDir = m_qOrientation.ydir();
                qNewOrientation.m_fW = 0.0f;
                qNewOrientation.m_fZ = 0.0f;
            }
            if (fFacing < 0.0f)
            {
                fAngle = static_cast<MdlNodeEmitter*>(m_pEmitter->m_pNode)->m_fDeadSpace + HALF_PI - fFacing;
            }
            else
            {
                fAngle = fFacing - HALF_PI + static_cast<MdlNodeEmitter*>(m_pEmitter->m_pNode)->m_fDeadSpace;
            }

            qNewOrientation = Quaternion(cross(vCameraDirection, m_qOrientation.ydir()), fAngle) *
                m_qOrientation;
            fFacing = vCameraDirection * qNewOrientation.ydir();

            if (fFacing < 0.0f)
            {
                fFacing = -fFacing;
            }
        }
        m_qOrientation = qNewOrientation;
    }
}
// PC 0x004B8E00 GOG 0x004B8F10
void PartEmitter::CalculateRadius()
{
    if (m_pAurObject && m_pAurObject->m_pModel &&
        (m_pAurObject->m_pModel->m_Classification.m_bEmitterBounds))
    {
        m_fRadius = EMITTER_EFFECT_RADIUS;
        return;
    }

    Vector vPosition;
    Quaternion qOrientation;
    GetWorld(vPosition, qOrientation);

    if (m_pTarget.m_pObject)
    {
        m_fRadius = magnitude(
            static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition() - vPosition);
        return;
    }

    float fAcceleration = m_fMass * EMITTER_GRAVITY;
    if (fAcceleration < 0.0f)
    {
        fAcceleration = -fAcceleration;
    }
    m_fGravityAcceleration = fAcceleration;

    float fWindSpeed = 0.0f;
    if (m_pWindManager.m_pObject)
    {
        fWindSpeed = magnitude(m_pWindManager.m_pObject->GetBaseWind());
    }
    float fTravelRadius = (fWindSpeed + m_fVelocity + m_fVelocityRandom) * m_fLifeTime + 0.5f * m_fGravityAcceleration * m_fLifeTime * m_fLifeTime;
    m_fRadius = fTravelRadius;

    float fHalfX = max(m_fXSizeStart, m_fXSizeEnd);
    fHalfX = max(fHalfX, m_fXSizeMid) * 0.5f;
    float fHalfY = max(m_fYSizeStart, m_fYSizeEnd);
    fHalfY = max(fHalfY, m_fYSizeMid) * 0.5f;
    if (fHalfY <= 0.0f)
    {
        fHalfY = fHalfX;
    }
    float fEmitterY = m_fYSize / 100.0f;
    float fEmitterX = m_fXSize / 100.0f;
    m_fRadius = ((float)sqrt(fHalfX * fHalfX + fHalfY * fHalfY) +
                 (float)sqrt(fEmitterY * fEmitterY + fEmitterX * fEmitterX) +
                 fTravelRadius) * EMITTER_RADIUS_PADDING;

    for (int i = 0; i < m_lstActive.m_nCount; ++i)
    {
        ChunkyParticle* pParticle = m_lstActive.m_pList[i]->AsChunkyParticle();
        if (!pParticle)
        {
            break;
        }
        Gob* pGob = pParticle->m_pGob;
        if (pGob)
        {
            float fRadius = magnitude(pGob->m_vPosition - vPosition) +
                            pGob->m_fMinimumBoundingSphereRadius;
            m_fRadius = fRadius < m_fRadius ? m_fRadius : fRadius;
        }
    }
    birth = m_fBirthRate;
}
// PC 0x004B90B0 GOG 0x004B91C0
void PartEmitter::updateParticleFrame(float fTimeStep)
{
    bool bRandom = false;
    bool bRandomOrder = false;
    int nAdvance;
    int nRange;
    roundToInt(nRange, m_fFrameEnd - m_fFrameStart);

    if (nRange < 0)
    {
        nRange = -nRange;
    }

    nRange++;

    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);
    if (pNode->IsRandomOrder())
    {
        bRandomOrder = true;
    }

    if (pNode->IsRandom())
    {
        bRandom = true;
    }

    for (int i = 0; i < m_lstActive.m_nCount; i++)
    {
        Particle* pParticle = m_lstActive.m_pList[i];
        float fElapsed;

        if (bRandom)
        {
            fElapsed = pParticle->m_fAge - pParticle->m_fFrameStartAge;
        }
        else
        {
            fElapsed = pParticle->m_fAge - pParticle->m_fFrameAge;
        }

        if (fTimeStep != 0.0f && fElapsed >= fTimeStep)
        {
            int nFirst;
            int nLast;
            roundToInt(nFirst, m_fFrameStart);
            roundToInt(nLast, m_fFrameEnd);

            if (m_fFrameStart != m_fFrameEnd)
            {
                if (bRandomOrder)
                {
                    pParticle->m_nFrame = rand() % nRange;
                }
                else if (m_fFrameEnd < m_fFrameStart)
                {
                    roundToInt(nAdvance, (float)-floor(fElapsed / fTimeStep));
                    pParticle->m_nFrame += nAdvance;

                    if (bRandom)
                    {
                        pParticle->m_fFrameAge += nAdvance * fTimeStep;

                        while (pParticle->m_nFrame < m_fFrameEnd)
                        {
                            pParticle->m_nFrame += nFirst - nLast + 1;
                        }
                    }
                    else
                    {
                        if (pNode->m_nLoop && pParticle->m_nFrame < nFirst)
                        {
                            pParticle->m_nFrame = nLast;
                        }
                        pParticle->m_nFrame = max(nFirst, pParticle->m_nFrame);
                    }
                }
                else
                {
                    roundToInt(nAdvance, (float)floor(fElapsed / fTimeStep));
                    pParticle->m_nFrame += nAdvance;

                    if (bRandom)
                    {
                        pParticle->m_fFrameAge += nAdvance * fTimeStep;

                        while (pParticle->m_nFrame > nLast)
                        {
                            pParticle->m_nFrame += nFirst - nLast - 1;
                        }
                    }
                    else
                    {
                        if (pNode->m_nLoop && pParticle->m_nFrame > nLast)
                        {
                            pParticle->m_nFrame = nFirst;
                        }
                        pParticle->m_nFrame = min(nLast, pParticle->m_nFrame);
                    }
                }

                if (!bRandom)
                {
                    pParticle->m_fFrameAge = pParticle->m_nFrame * fTimeStep;
                }

                if (m_fLifeTime >= 0.0f)
                {
                    pParticle->m_fFrameStartAge = pParticle->m_fAge;
                }
                else
                {
                    pParticle->m_fFrameStartAge = 0;
                    pParticle->m_fAge = 0;
                }
            }
        }
    }
}
// PC 0x004B9390 GOG 0x004B94A0
void PartEmitter::RecursiveFractal(int nIndex, int nStep, int nCount,
                                  Vector vDirection, Vector vOffset, float fRange)
{
    float fRandom = (rand() % (int)(fRange * 100.0f)) * 0.01f;
    vOffset.m_fX += fRandom * m_fLightningRadius;
    LightningEmitter* pLightning = AsLightningEmitter();
    if (pLightning)
        pLightning->m_lstReserved270.m_pList[nIndex] = vOffset.m_fX * vDirection;
    else
        m_lstActive.m_pList[nIndex]->m_vPosition += vOffset.m_fX * vDirection;

    if (nStep != 0)
    {
        int nHalfStep = (int)(nStep * 0.5);
        RecursiveFractal(nIndex - nHalfStep, nHalfStep, nCount, vDirection, vOffset, fRange * 0.5f);
        RecursiveFractal(nIndex + nHalfStep, nHalfStep, nCount, vDirection, vOffset, fRange * 0.5f);
    }
}
#ifdef _XBOX
#define GetNextFrame(pEmitter, nFrame) (pEmitter)->GetNextFrame(nFrame)
#else
static int GetNextFrame(PartEmitter* pEmitter, int nFrame);
#endif
// PC 0x004B9BD0 GOG 0x004B9CE0
void PartEmitter::RenderBoundingSphere()
{
    Vector vPosition;
    Quaternion qOrientation;

    GetWorld(vPosition, qOrientation);

    Vector vColor(1.0f, 0.5f, 1.0f);

    GLRender::DrawSphereBound(GetRadius(), vPosition, vColor, 1.0f, true);
}
// PC 0x004B9C60 GOG 0x004B9D70
void PartEmitter::StartEmitterLayer()
{
    GLRender_SetDepthMask(0);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_Enable(ENABLE_BLEND);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    GLRender_EnableClientState(CLIENT_STATE_VERTEX);
    GLRender_EnableClientState(CLIENT_STATE_COLOR);
    GLRender_EnableClientState(
        CLIENT_STATE_TEXTURE_COORD);
    GLRender_DisableClientState(CLIENT_STATE_NORMAL);
}

#ifndef _XBOX
static int GetNextFrame(PartEmitter* pEmitter, int nFrame)
{
    if (pEmitter->m_fFrameStart != pEmitter->m_fFrameEnd)
    {
        if (pEmitter->m_fFrameEnd < pEmitter->m_fFrameStart)
        {
            int nPrev = nFrame - 1;
            if (nPrev >= pEmitter->m_fFrameEnd)
                return nPrev;
        }
        else
        {
            int nNext = nFrame + 1;
            if (nNext <= pEmitter->m_fFrameEnd)
                return nNext;
        }
    }
    return -1;
}
#endif
// PC 0x004B9CC0 GOG 0x004B9DD0
void PartEmitter::StopEmitterLayer()
{
    GLRender_SetDepthMask(1);
    GLRender_Enable(ENABLE_LIGHTING);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    GLRender_EnableClientState(CLIENT_STATE_NORMAL);
}
#ifndef _XBOX
void PartEmitter::RenderStandard()  // PC 0x004B9D00 GOG 0x004B9E10
{
    static unsigned long poolSize = 1000;
    static RenderVertexVTC* particlePool =
        static_cast<RenderVertexVTC*>(operator new(1000 * sizeof(RenderVertexVTC)));

    if (m_pAurObject && m_pAurObject->m_pTileData &&
        m_pAurObject->m_pTileData->m_fFadeAlpha < 1.0f)
        return;

    Vector vUp;
    Vector vDirection;
    Vector vRight;
    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);
#ifdef _XBOX
    unsigned char bSplat = pNode->IsSplat();
#else
    bool bSplat = pNode->m_EmitterFlags.m_bSplat != 0;
#endif
    Quaternion qRotation;
    Quaternion qCamera;
    float fFrameTime = m_fFramesPerSecond;
    int bFog = 0;
    float fScale = 1.0f;
    int nPass = 0;
    int nPasses = 1;
    if (pNode->m_bFrameBlending)
        nPasses = 5;

    if (renderemitterspheres)
        RenderBoundingSphere();
    if (m_lstActive.m_nCount == 0)
        return;
    if (4 * m_lstActive.m_nCount > poolSize)
    {
        unsigned long nOldSize = poolSize;
        poolSize = 8 * m_lstActive.m_nCount;
        particlePool = ResizeEmitterPool(particlePool, nOldSize, poolSize);
    }
    if (m_pAurObject && m_pAurObject->m_fScale != 1.0f)
        fScale = m_pAurObject->m_fScale;
    if (fFrameTime > 0.0f)
        fFrameTime = 1.0f / fFrameTime;
    else
        fFrameTime = 0.0f;

    m_pEmitterMaterial->BindTexture0();
    m_pEmitterMaterial->SetBlendingMode(0);
    vUp.m_fX = vUp.m_fY = 0.0f;
    vUp.m_fZ = 1.0f;
    if (m_nReserved114 == NORMAL)
    {
        vDirection = CurrentCamera->m_qOrientation.zdir();
        qCamera = CurrentCamera->m_qOrientation;
    }
    else if (m_nReserved114 == WorldZ)
    {
        vDirection = vUp;
        qCamera = Quaternion(vUp, 0.0f);
    }
    else if (m_nReserved114 == LocalZ)
    {
        vDirection = m_qOrientation.zdir();
        qCamera = m_qOrientation;
    }
    else
    {
        vDirection = CurrentCamera->m_qOrientation.zdir();
        qCamera = CurrentCamera->m_qOrientation;
        if (qCamera.ydir() * vUp < 0.0f)
            vUp.m_fZ = -1.0f;
    }

    float fInverseLife = m_fLifeTime > 0.0f ? 1.0f / m_fLifeTime : 0.01f;
    int nXGrid = pNode->m_nXGrid ? pNode->m_nXGrid : 1;
    int nYGrid = pNode->m_nYGrid ? pNode->m_nYGrid : 1;

    GLRender_SetDepthMask(GL_FALSE);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_Enable(ENABLE_BLEND);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    if (static_cast<MdlNodeEmitter*>(m_pNode)->m_nTwoSidedTex)
    {
        GLRender_SetPolygonRenderMode(FACING_FRONT_AND_BACK,
                      FILL_SOLID);
        GLRender_Disable(ENABLE_CULL_FACE);
    }
    if (m_nBlendMode == BLEND_NORMAL)
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                    BLEND_ONE_MINUS_SRC_ALPHA);
    else if (m_nBlendMode == BLEND_PUNCH_THROUGH)
    {
        GLRender_Disable(ENABLE_BLEND);
        GLRender_Enable(ENABLE_ALPHA_TEST);
    }
    else
    {
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE);
        bFog = GLRender_IsEnabled(ENABLE_FOG);
        GLRender_Disable(ENABLE_FOG);
    }
    updateParticleFrame(fFrameTime);
    particlecount += m_lstActive.m_nCount;
    float fUScale = 1.0f / nXGrid;
    float fVScale = 1.0f / nYGrid;
    GLRender_PushCurrentMatrix();
    float fTileAlpha = GetTileAlpha();
    float fAlpha = 0.0f;
    float fFrameAlpha = 0.0f;
#ifdef _XBOX
    float fObjectAlpha = GetObjectAlpha();
#else
    float fObjectAlpha = 1.0f;
    if (m_pAurObject)
        fObjectAlpha = m_pAurObject->m_fAlpha;
#endif
    int nNextFrame;
    int nPreviousFrame;
    for (; nPass < nPasses; nPass++)
    {
        int i = m_lstActive.m_nCount;
        while (i--)
        {
            Particle* pParticle = m_lstActive[m_lstActive.m_nCount - i - 1];
            Vector vColor;
            float fXSize;
            float fYSize;
            float fAge;
            float fT;
            if (m_fLifeTime >= 0.0f)
            {
                fAge = fInverseLife * pParticle->m_fAge;
                if (m_fPercentStart == EMITTER_UNSET_COLOR_KEY_TIME)
                {

                    fT = fAge;
                    vColor = Interpolate(m_vColorStart, m_vColorEnd, fT);
                    fAlpha = (1.0f - fT) * m_fAlphaStart + fT * m_fAlphaEnd;
                    fXSize = ((1.0f - fT) * m_fXSizeStart + fT * m_fXSizeEnd) * 0.5f;
                    if (m_fYSizeStart != 0.0f || m_fYSizeEnd != 0.0f)
                        fYSize = (fT * m_fYSizeEnd + (1.0f - fT) * m_fYSizeStart) * 0.5f;
                    else
                        fYSize = fXSize;
                }
                else if (fAge >= m_fPercentEnd)
                {
                    vColor = m_vColorEnd;
                    fAlpha = m_fAlphaEnd;
                    fXSize = m_fXSizeEnd * 0.5f;
                    fYSize = m_fYSizeEnd != 0.0f ? m_fYSizeEnd * 0.5f : fXSize;
                }

                else if (fAge >= m_fPercentMid)
                {
                    fT = (fAge - m_fPercentMid) / (m_fPercentEnd - m_fPercentMid);
                    vColor = Interpolate(m_vColorMid, m_vColorEnd, fT);
                    fAlpha = (1.0f - fT) * m_fAlphaMid + fT * m_fAlphaEnd;
                    fXSize = ((1.0f - fT) * m_fXSizeMid + fT * m_fXSizeEnd) * 0.5f;
                    if (m_fYSizeMid == 0.0f && m_fYSizeEnd == 0.0f)
                        fYSize = fXSize;
                    else
                        fYSize = (fT * m_fYSizeEnd + (1.0f - fT) * m_fYSizeMid) * 0.5f;
                }
                else if (fAge >= m_fPercentStart)
                {
                    fT = (fAge - m_fPercentStart) / (m_fPercentMid - m_fPercentStart);
                    vColor = Interpolate(m_vColorStart, m_vColorMid, fT);
                    fAlpha = (1.0f - fT) * m_fAlphaStart + fT * m_fAlphaMid;
                    fXSize = ((1.0f - fT) * m_fXSizeStart + fT * m_fXSizeMid) * 0.5f;
                    if (m_fYSizeStart == 0.0f && m_fYSizeMid == 0.0f)
                        fYSize = fXSize;
                    else
                        fYSize = (fT * m_fYSizeMid + (1.0f - fT) * m_fYSizeStart) * 0.5f;
                }
                else
                {
                    vColor = m_vColorStart;
                    fAlpha = m_fAlphaStart;
                    fXSize = m_fXSizeStart * 0.5f;
                    if (m_fYSizeStart != 0.0f)
                        fYSize = m_fYSizeStart * 0.5f;
                    else
                        fYSize = fXSize;
                }
                if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
                else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
                if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
                else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
                if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
                else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
                if (fAlpha < 0.0f) fAlpha = 0.0f;
                else if (fAlpha > 1.0f) fAlpha = 1.0f;
            }
            else
            {
                vColor = m_vColorStart;
                fAlpha = m_fAlphaStart;
                if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
                else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
                if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
                else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
                if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
                else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
                if (fAlpha < 0.0f) fAlpha = 0.0f;
                else if (fAlpha > 1.0f) fAlpha = 1.0f;
                fXSize = m_fXSizeStart * 0.5f;
                if (m_fYSizeStart == 0.0f && m_fYSizeMid == 0.0f && m_fYSizeEnd == 0.0f)
                    fYSize = fXSize;
                else
                    fYSize = m_fYSizeStart * 0.5f;
            }

            if (bSplat)
            {
                if (pParticle->m_bReserved6C)
                {
                    fXSize = m_fXSizeEnd;
                    if (m_fYSizeEnd != 0.0f)
                        fYSize = m_fYSizeEnd;
                    else
                        fYSize = fXSize;
                }
                else
                {
                    fXSize = m_fXSizeStart;
                    if (m_fYSizeStart != 0.0f)
                        fYSize = m_fYSizeStart;
                    else
                        fYSize = fXSize;
                }
            }
            if (pParticle->m_bReserved6C)
            {
                qRotation = Quaternion(pParticle->m_vHeading, pParticle->m_fRotation);
                qRotation = qRotation * pParticle->m_qOrientation;
            }
            else
            {
                qRotation = Quaternion(vDirection, pParticle->m_fRotation);
                qRotation = qRotation * qCamera;
            }
            if (fScale != 1.0f)
            {
                fXSize *= fScale;
                fYSize *= fScale;
            }

            fFrameAlpha = 0.0f;
            int nColumn = 0;
            int nRow = 0;
            if (nPass == 1)
            {
                nNextFrame = GetNextFrame(this, pParticle->m_nFrame);
                if (nNextFrame >= 0)
                {
                    fFrameAlpha = (pParticle->m_fAge - pParticle->m_fFrameAge) /
                                  fFrameTime * 0.5f;
                    nRow = (int)(nNextFrame * fUScale);
                    nColumn = nNextFrame - nXGrid * nRow;
                }
            }
            else if (nPass == 2)
            {
                nPreviousFrame = GetNeighborFrame(pParticle->m_nFrame);
                if (nPreviousFrame >= 0)
                {
                    fAge = (pParticle->m_fAge - pParticle->m_fFrameAge) / fFrameTime;
                    fFrameAlpha = 1.0f - fAge * 0.5f;
                    nRow = (int)(nPreviousFrame * fUScale);
                    nColumn = nPreviousFrame - nXGrid * nRow;
                }
            }
            else if (nPass == 3)
            {
                nNextFrame = GetNextFrame(this, nNextFrame);
                if (nNextFrame >= 0)
                {
                    fFrameAlpha = (pParticle->m_fAge - pParticle->m_fFrameAge) /
                                  fFrameTime * 0.25f;
                    nRow = (int)(nNextFrame * fUScale);
                    nColumn = nNextFrame - nXGrid * nRow;
                }
            }
            else if (nPass == 4)
            {
                nPreviousFrame = GetNeighborFrame(nPreviousFrame);
                if (nPreviousFrame >= 0)
                {
                    fAge = (pParticle->m_fAge - pParticle->m_fFrameAge) / fFrameTime;
                    fFrameAlpha = (1.0f - fAge) * 0.25f;
                    nRow = (int)(nPreviousFrame * fUScale);
                    nColumn = nPreviousFrame - nXGrid * nRow;
                }
            }
            else
            {
                nRow = (int)(pParticle->m_nFrame * fUScale);
                nColumn = pParticle->m_nFrame - nXGrid * nRow;
                if (pNode->m_bFrameBlending)
                    fFrameAlpha = 0.5f;
                else
                    fFrameAlpha = 1.0f;
            }
            Vector vVertical;
            if (m_nReserved114 == AlignedToParticleDir)
            {
                vRight = pParticle->m_qOrientation.xdir();
                vVertical = pParticle->m_qOrientation.zdir();
            }
            else
            {
                vRight = qRotation.xdir();
                if (m_nReserved114 == Aligned)
                    vVertical = vUp;
                else if (bSplat)
                    vVertical = cross(pParticle->m_vHeading, vRight);
                else
                    vVertical = cross(vDirection, vRight);
            }
            vRight = vRight * fXSize;
            vVertical = vVertical * fYSize;
            int nRed, nGreen, nBlue, nAlpha;
            roundToInt(nRed, vColor.m_fX * 255.0f);
            roundToInt(nGreen, vColor.m_fY * 255.0f);
            roundToInt(nBlue, vColor.m_fZ * 255.0f);
            roundToInt(nAlpha, fObjectAlpha * fFrameAlpha * fAlpha * fTileAlpha * 255.0f);
            unsigned long nColor = nRed | ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8);

            RenderVertexVTC* pVertices = particlePool + 4 * i;
            for (int j = 0; j < 4; j++)
                *reinterpret_cast<unsigned long*>(pVertices[j].color) = nColor;

            Vector vLeftCorner = pParticle->m_vPosition - vRight - vVertical;
            pVertices[0].position[0] = vLeftCorner.m_fX;
            particlePool[4 * i + 0].position[1] = vLeftCorner.m_fY;
            particlePool[4 * i + 0].position[2] = vLeftCorner.m_fZ;
            pVertices[0].u = nColumn * fUScale;
            pVertices[0].v = 1.0f - (nRow + 1) * fVScale;
            Vector vRightCorner = pParticle->m_vPosition + vRight - vVertical;
            pVertices[1].position[0] = vRightCorner.m_fX;
            particlePool[4 * i + 1].position[1] = vRightCorner.m_fY;
            particlePool[4 * i + 1].position[2] = vRightCorner.m_fZ;
            pVertices[1].u = (nColumn + 1) * fUScale;
            pVertices[1].v = 1.0f - (nRow + 1) * fVScale;
            vRightCorner = pParticle->m_vPosition + vRight + vVertical;
            pVertices[2].position[0] = vRightCorner.m_fX;
            particlePool[4 * i + 2].position[1] = vRightCorner.m_fY;
            particlePool[4 * i + 2].position[2] = vRightCorner.m_fZ;
            pVertices[2].u = (nColumn + 1) * fUScale;
            pVertices[2].v = 1.0f - nRow * fVScale;
            vLeftCorner = pParticle->m_vPosition - vRight + vVertical;
            pVertices[3].position[0] = vLeftCorner.m_fX;
            particlePool[4 * i + 3].position[1] = vLeftCorner.m_fY;
            particlePool[4 * i + 3].position[2] = vLeftCorner.m_fZ;
            pVertices[3].u = nColumn * fUScale;
            pVertices[3].v = 1.0f - nRow * fVScale;
        }
        if (nPass > 0)
        {
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE);
            bFog = GLRender_IsEnabled(ENABLE_FOG);
            GLRender_Disable(ENABLE_FOG);
        }
        countfaces += m_lstActive.m_nCount;
        GLRender::RenderObjectsVTCQuadUP(particlePool, 4 * m_lstActive.m_nCount);
    }
    GLRender_PopCurrentMatrix();
    GLRender_SetDepthMask(GL_TRUE);
    GLRender_Enable(ENABLE_LIGHTING);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                BLEND_ONE_MINUS_SRC_ALPHA);
    if (bFog) GLRender_Enable(ENABLE_FOG);
}
#endif
void PartEmitter::RenderNothing()  // PC 0x004BB510 GOG 0x004BB620
{
    if (m_pAurObject != 0 &&
        static_cast<Gob*>(m_pAurObject)->m_pTileData != 0 &&
        static_cast<Gob*>(m_pAurObject)->m_pTileData->m_fFadeAlpha < 1.0f)
    {
        return;
    }

    if (renderemitterspheres)
    {
        RenderBoundingSphere();
    }

    for (int i = 0; i < m_lstActive.m_nCount; i++)
    {
        ChunkyParticle* pParticle =
            static_cast<ChunkyParticle*>(m_lstActive[i]);

        if (pParticle->m_pGob != 0)
        {
            pParticle->m_pGob->SetPosition(pParticle->m_vPosition);

            if (m_fParticleRotation != 0.0f && !pParticle->m_bReserved6C)
            {
                pParticle->m_pGob->SetOrientation(
                    Quaternion(pParticle->m_vHeading, pParticle->m_fRotation));
            }
        }
    }
}

#define EMITTER_MOTIONBLUR_DEBUG 0
#ifndef _XBOX
void PartEmitter::RenderMotionBlurred()  // PC 0x004BB630 GOG 0x004BB740
{

    static unsigned long poolSize = 1000;
    static RenderVertexVTC* particlePool =
        static_cast<RenderVertexVTC*>(operator new(1000 * sizeof(RenderVertexVTC)));

    if (m_pAurObject && m_pAurObject->m_pTileData &&
        m_pAurObject->m_pTileData->m_fFadeAlpha < 1.0f)
        return;

    Vector vCameraDirection;
    Quaternion qRotation;
    Quaternion qCamera;
    Vector vCameraX = CurrentCamera->m_qOrientation.xdir();
    m_pNode = m_pNode;
    m_fFramesPerSecond = m_fFramesPerSecond;
    Vector vCameraY = CurrentCamera->m_qOrientation.ydir();
    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);
    int bFog = 0;
    float fScale = 1.0f;
    float fStart = 0.0f;
    float fFrameTime = m_fFramesPerSecond;

    if (m_lstActive.m_nCount == 0)
        return;
    if (renderemitterspheres)
        RenderBoundingSphere();

    if (4 * m_lstActive.m_nCount > poolSize)
    {
        unsigned long nOldSize = poolSize;
        poolSize = 8 * m_lstActive.m_nCount;
        particlePool = ResizeEmitterPool(particlePool, nOldSize, poolSize);
    }

    if (m_pAurObject && m_pAurObject->m_fScale != 1.0f)
        fScale = m_pAurObject->m_fScale;
    if (fFrameTime > 0.0f)
        fFrameTime = 1.0f / fFrameTime;
    else
        fFrameTime = 0.0f;
    m_pEmitterMaterial->BindTexture0();
    m_pEmitterMaterial->SetBlendingMode(0);
    if (EMITTER_MOTIONBLUR_DEBUG)
    {
        vCameraDirection.m_fX = 0.0f;
        vCameraDirection.m_fY = 0.0f;
        vCameraDirection.m_fZ = 0.0f;
    }
    Quaternion& qOrientation = CurrentCamera->m_qOrientation;
    qOrientation.m_fZ = qOrientation.m_fZ;
    vCameraDirection = qOrientation.zdir();
    qCamera = CurrentCamera->m_qOrientation;

    float fReciprocalLife;
    if (m_pReserved1F4 == LIGHTNING)
        fReciprocalLife = 1.0f / m_lstActive.m_nCount;
    else if (m_fLifeTime > 0.0f)
        fReciprocalLife = 1.0f / m_fLifeTime;
    else
        fReciprocalLife = 0.01f;

    int nXGrid = pNode->m_nXGrid;
    if (nXGrid == 0)
        nXGrid = 1;
    int nYGrid = pNode->m_nYGrid;
    if (nYGrid == 0)
        nYGrid = 1;

    GLRender_SetDepthMask(0);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_Enable(ENABLE_BLEND);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    GLRender_Disable(ENABLE_CULL_FACE);
    if (m_nBlendMode == BLEND_NORMAL)
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    else if (m_nBlendMode == BLEND_PUNCH_THROUGH)
    {
        GLRender_Disable(ENABLE_BLEND);
        GLRender_Enable(ENABLE_ALPHA_TEST);
    }
    else
    {
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE);
        bFog = GLRender_IsEnabled(ENABLE_FOG);
        GLRender_Disable(ENABLE_FOG);
    }

    updateParticleFrame(fFrameTime);
    particlecount += m_lstActive.m_nCount;
    float fU = 1.0f / nXGrid;
    float fV = 1.0f / nYGrid;
    float fTileAlpha = GetTileAlpha();
    float fAlpha = 0.0f;
    float fTailAlpha = 0.0f;
#ifdef _XBOX
    float fObjectAlpha = GetObjectAlpha();
#else
    float fObjectAlpha = 1.0f;
    if (m_pAurObject)
        fObjectAlpha = m_pAurObject->m_fAlpha;
#endif

    int i = m_lstActive.m_nCount;
    while (i != 0)
    {
        --i;
        Particle* pParticle = m_lstActive[i];
        Vector vColor;
        float fWidth;
        float fHeight;
        if (m_fLifeTime >= 0.0f)
        {
            if (m_pReserved1F4 == LIGHTNING)
                fFrameTime = i * fReciprocalLife;
            else
                fFrameTime = fReciprocalLife * pParticle->m_fAge;

            if (m_fPercentStart == EMITTER_UNSET_COLOR_KEY_TIME)
            {
                vColor = Interpolate(m_vColorStart, m_vColorEnd, fFrameTime);
                fStart = 1.0f - fFrameTime;
                fAlpha = fStart * m_fAlphaStart;
                fAlpha += fFrameTime * m_fAlphaEnd;
                fWidth = fStart * m_fXSizeStart;
                fWidth += fFrameTime * m_fXSizeEnd;
                fWidth *= 0.5f;
                if (m_fYSizeStart == 0.0f && m_fYSizeEnd == 0.0f)
                    fHeight = fWidth;
                else
                {
                    fHeight = fFrameTime * m_fYSizeEnd;
                    fHeight += fStart * m_fYSizeStart;
                    fHeight *= 0.5f;
                }
            }
            else if (fFrameTime >= m_fPercentEnd)
            {
                vColor = m_vColorEnd;
                fAlpha = m_fAlphaEnd;
                fWidth = m_fXSizeEnd;
                fHeight = m_fYSizeEnd != 0.0f ? m_fYSizeEnd : fWidth;
            }
            else if (fFrameTime >= m_fPercentMid)
            {
                fFrameTime = (fFrameTime - m_fPercentMid) / (m_fPercentEnd - m_fPercentMid);
                vColor = Interpolate(m_vColorMid, m_vColorEnd, fFrameTime);
                float fStart = 1.0f - fFrameTime;
                fAlpha = fStart * m_fAlphaMid;
                fAlpha += fFrameTime * m_fAlphaEnd;
                fWidth = fStart * m_fXSizeMid;
                fWidth += fFrameTime * m_fXSizeEnd;
                fWidth *= 0.5f;
                if (m_fYSizeMid == 0.0f && m_fYSizeEnd == 0.0f)
                    fHeight = fWidth;
                else
                {
                    fHeight = fFrameTime * m_fYSizeEnd;
                    fHeight += fStart * m_fYSizeMid;
                    fHeight *= 0.5f;
                }
            }
            else if (fFrameTime >= m_fPercentStart)
            {
                fFrameTime = (fFrameTime - m_fPercentStart) / (m_fPercentMid - m_fPercentStart);
                vColor = Interpolate(m_vColorStart, m_vColorMid, fFrameTime);
                float fStart = 1.0f - fFrameTime;
                fAlpha = fStart * m_fAlphaStart;
                fAlpha += fFrameTime * m_fAlphaMid;
                fWidth = fStart * m_fXSizeStart;
                fWidth += fFrameTime * m_fXSizeMid;
                fWidth *= 0.5f;
                if (m_fYSizeStart != 0.0f || m_fYSizeMid != 0.0f)
                {
                    fHeight = fFrameTime * m_fYSizeMid;
                    fHeight += fStart * m_fYSizeStart;
                    fHeight *= 0.5f;
                }
                else
                    fHeight = fWidth;
            }
            else
            {
                vColor = m_vColorStart;
                fAlpha = m_fAlphaStart;
                fWidth = m_fXSizeStart;
                if (m_fYSizeStart != 0.0f)
                    fHeight = m_fYSizeStart;
                else
                    fHeight = fWidth;
            }
            if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
            else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
            if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
            else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
            if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
            else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
            if (fAlpha < 0.0f) fAlpha = 0.0f;
            else if (fAlpha > 1.0f) fAlpha = 1.0f;
        }
        else
        {
            vColor = m_vColorStart;
            fAlpha = m_fAlphaStart;
            if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
            else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
            if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
            else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
            if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
            else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
            if (fAlpha < 0.0f) fAlpha = 0.0f;
            else if (fAlpha > 1.0f) fAlpha = 1.0f;
            fWidth = m_fXSizeStart * 0.5f;

            if (m_fYSizeStart == 0.0f && m_fYSizeEnd == 0.0f)
                fHeight = fWidth;
            else
                fHeight = m_fYSizeStart * 0.5f;
        }

        Vector vImaginary = normalize(vCameraDirection) *
            sineLUT(pParticle->m_fRotation * 0.5f);
        qRotation = Quaternion(cosineLUT(pParticle->m_fRotation * 0.5f),
            vImaginary.m_fX, vImaginary.m_fY, vImaginary.m_fZ);
        qRotation = qRotation * qCamera;
        int nRow = (int)(pParticle->m_nFrame * fU);
        int nColumn = pParticle->m_nFrame - nXGrid * nRow;
        Vector vOrigin = pParticle->m_vOrigin;
        Vector vTravel = pParticle->m_vPosition - vOrigin;
        float fTravelX = vCameraX * vTravel;
        float fTravelY = vCameraY * vTravel;
        float fLength = sqrt(fTravelY * fTravelY + fTravelX * fTravelX);
        Vector vRight;
        Vector vUp;
        if (fLength != 1.0f && fLength > 0.00001)
        {
            float fDirectionX = (fTravelX * (1.0f / fLength));
            float fDirectionY = ((1.0f / fLength) * fTravelY);
            vRight = vCameraX * fDirectionY - vCameraY * fDirectionX;
            vUp = vCameraX * fDirectionX + vCameraY * fDirectionY;
        }
        else
        {
            vRight = vCameraX;
            vUp = vCameraY;
            if (fLength == 0.0f)
                fLength = 1.0f;
        }
        fTailAlpha = fWidth / fLength * 2.0f;
        if (!(fTailAlpha < 1.0f))
            fTailAlpha = 1.0f;
        if (fScale != 1.0f)
        {
            fWidth *= fScale;
            fHeight *= fScale;
        }
        Vector vWidth;
        Vector vHeight;
        vWidth = vRight * (fWidth * 2.0f);
        vHeight = vUp * (fHeight * 0.5f);
        int nBlue = (int)(vColor.m_fZ * 255.0f);
        int nGreen = (int)(vColor.m_fY * 255.0f);
        int nRed = (int)(vColor.m_fX * 255.0f);
        unsigned long nColor = nRed | ((nGreen | ((nBlue |
            ((unsigned long)((fObjectAlpha * fAlpha) * fTileAlpha * 255.0f) << 8)) << 8)) << 8);
        int nVertex = i * 4;
        *reinterpret_cast<unsigned long*>(particlePool[nVertex + 0].color) = nColor;
        *reinterpret_cast<unsigned long*>(particlePool[nVertex + 1].color) = nColor;
        nColor = nRed | ((nGreen | ((nBlue |
            ((unsigned long)((fObjectAlpha * fTailAlpha * fAlpha * fTileAlpha) * 255.0f) << 8)) << 8)) << 8);
        *reinterpret_cast<unsigned long*>(particlePool[nVertex + 2].color) = nColor;
        *reinterpret_cast<unsigned long*>(particlePool[nVertex + 3].color) = nColor;

        Vector vVertexPosition = pParticle->m_vPosition + vWidth + vHeight;
        particlePool[i * 4 + 0].position[0] = vVertexPosition.m_fX;
        particlePool[i * 4 + 0].position[1] = vVertexPosition.m_fY;
        particlePool[i * 4 + 0].position[2] = vVertexPosition.m_fZ;
        particlePool[i * 4 + 0].u = (nColumn + 1) * fU;
        particlePool[i * 4 + 0].v = 1.0f - nRow * fV;
        vVertexPosition = pParticle->m_vPosition - vWidth + vHeight;
        particlePool[i * 4 + 1].position[0] = vVertexPosition.m_fX;
        particlePool[i * 4 + 1].position[1] = vVertexPosition.m_fY;
        particlePool[i * 4 + 1].position[2] = vVertexPosition.m_fZ;
        particlePool[i * 4 + 1].u = nColumn * fU;
        particlePool[i * 4 + 1].v = 1.0f - nRow * fV;
        vVertexPosition = vOrigin - vWidth - vHeight;
        particlePool[i * 4 + 2].position[0] = vVertexPosition.m_fX;
        particlePool[i * 4 + 2].position[1] = vVertexPosition.m_fY;
        particlePool[i * 4 + 2].position[2] = vVertexPosition.m_fZ;
        particlePool[i * 4 + 2].u = nColumn * fU;
        particlePool[i * 4 + 2].v = 1.0f - (nRow + 1) * fV;
        vVertexPosition = vOrigin + vWidth - vHeight;
        particlePool[i * 4 + 3].position[0] = vVertexPosition.m_fX;
        particlePool[i * 4 + 3].position[1] = vVertexPosition.m_fY;
        particlePool[i * 4 + 3].position[2] = vVertexPosition.m_fZ;
        particlePool[i * 4 + 3].u = (nColumn + 1) * fU;
        particlePool[i * 4 + 3].v = 1.0f - (nRow + 1) * fV;
    }

    countfaces += m_lstActive.m_nCount;
    GLRender::RenderObjectsVTCQuadUP(particlePool, 4 * m_lstActive.m_nCount);
    GLRender_SetDepthMask(1);
    GLRender_Enable(ENABLE_LIGHTING);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    GLRender_Enable(ENABLE_CULL_FACE);
    if (bFog)
        GLRender_Enable(ENABLE_FOG);
    if (EMITTER_MOTIONBLUR_DEBUG)
    {
        qCamera.m_fW = 0.0f;
        qCamera.m_fX = 0.0f;
        qCamera.m_fY = 0.0f;
        qCamera.m_fZ = 0.0f;
    }
}
#endif
void LightningEmitter::UpdateBranches()  // PC 0x004BC730 GOG 0x004BC840
{
    MdlNodeEmitter* pNode = m_pNode->AsMdlNodeEmitter();
    if (pNode->m_nNumBranches != 0)
    {
        m_nBranches = rand() % (pNode->m_nNumBranches + 1);
        for (int i = 0; i < m_nBranches; i++)
        {
            int nRandom = rand() % 80;
            float fScale = nRandom / 100.0f + 0.1f;
            m_lstBranches[i]->m_fXSizeStart = fScale * m_fXSizeStart;
            m_lstBranches[i]->m_fXSizeMid = fScale * m_fXSizeMid;
            m_lstBranches[i]->m_fXSizeEnd = fScale * m_fXSizeEnd;
            m_lstBranches[i]->m_fYSizeStart = fScale * m_fYSizeStart;
            m_lstBranches[i]->m_fYSizeMid = fScale * m_fYSizeMid;
            m_lstBranches[i]->m_fYSizeEnd = fScale * m_fYSizeEnd;
            int nIndex = (int)(m_lstActive.m_nCount * fScale);
            m_lstBranchIndices[i] = nIndex;
            m_lstBranches[i]->m_vOrigin = m_lstBranches[i]->m_lstReserved210[0] =
                m_lstActive[m_lstBranchIndices[i]]->m_vPosition;
            Vector vTangent;
            vTangent = YawPitchRoll(-(rand() % 45), 0.0f, rand() % 360) *
                (m_vReserved27C * m_fTangentLength);
            m_lstBranches[i]->m_lstReserved234[0] = vTangent;
            if (fScale > 0.5f)
            {
                m_lstBranches[i]->m_pTarget->SetPosition(m_pTarget->GetPosition());
                m_lstBranches[i]->m_lstReserved210[1] = m_pTarget->GetPosition();
                m_lstBranches[i]->m_fTargetSize = m_fTargetSize;
                m_lstBranches[i]->m_fBirthRate = magnitude(
                    m_pTarget->GetPosition() -
                    m_lstActive[m_lstBranchIndices[i]]->m_vPosition) *
                    m_fLightningSubDivisions + 2.0f;
                m_lstBranches[i]->m_lstReserved234[1] =
                    m_lstReserved234[m_lstReserved234.m_nCount - 1];
            }
            else
            {
                Particle* pFirst = m_lstActive[0];
                Particle* pLast = m_lstActive[m_lstActive.m_nCount - 1];
                Vector vDirection = pLast->m_vPosition - pFirst->m_vPosition;
                float fLength = magnitude(vDirection) * 0.5f;
                float fSpread = (rand() % 75) / 100.0f + 0.25f;
                float fDistance = fSpread * fLength;
                if (fDistance != 0.0f)
                {
                    Vector vOffset = YawPitchRoll(0.0f, rand() % 360, 0.0f) *
                        (m_vReserved27C * fDistance);
                    Vector vPosition = vOffset +
                        (vDirection * ((rand() % 25) / 100.0f + 0.25f) +
                         m_lstBranches[i]->m_lstReserved210[0]);
                    m_lstBranches[i]->m_pTarget->SetPosition(vPosition);
                    m_lstBranches[i]->m_fBirthRate = magnitude(
                        vPosition - m_lstActive[m_lstBranchIndices[i]]->m_vPosition) *
                        m_fLightningSubDivisions + 2.0f;
                    m_lstBranches[i]->m_lstReserved234[1] =
                        m_lstBranches[i]->m_lstReserved234[0];
                }
                else
                {
                    m_lstBranches[i]->m_fBirthRate = 0.0f;
                    m_lstBranches[i]->m_pTarget->SetPosition(
                        m_lstActive[m_lstBranchIndices[i]]->m_vPosition);
                }
                m_lstBranches[i]->m_fTargetSize = 0.0f;
                m_lstBranches[i]->m_lstReserved210[1] =
                    m_lstBranches[i]->m_pTarget->GetPosition();

                Vector vBranchDirection;
                vBranchDirection = normalize(
                    m_lstBranches[i]->m_lstReserved210[1] -
                    m_lstBranches[i]->m_lstReserved210[1]);
                m_lstBranches[i]->m_vReserved27C = Vector(
                    -vBranchDirection.m_fY, vBranchDirection.m_fZ, -vBranchDirection.m_fX);
            }
            m_lstBranches[i]->m_vReserved1F8 =
                m_lstBranches[i]->m_pTarget->GetPosition() - m_lstBranches[i]->m_vOrigin;
            m_lstBranches[i]->m_vReserved204 = m_lstBranches[i]->m_vOrigin;
        }
    }
}
// PC 0x004BD310 GOG 0x004BD420
void ChunkyParticle::initialize(float fTime, float fScale)
{
    Particle::initialize(fTime, 1.0f);

    if (m_pGob != 0)
    {
        m_pGob->AttachToScene(currentscene);
        m_pGob->SetObjectScale(fScale, 1);
        m_pGob->PlayAnimation("impact", 1.0f, 1, 0.0f);
    }
}
// PC 0x004BD360 GOG 0x004BD470
void PartEmitter::moveParticleStandard(float fTimeStep)
{
    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);
    Vector vPosition;
    int bInheriting = pNode->IsInheriting();
    Vector vWind;
#ifdef _XBOX
    unsigned char bBouncing = pNode->IsBouncing();
#else
    bool bBouncing = pNode->m_EmitterFlags.m_bBouncing;
#endif

    for (int i = 0; i < m_lstActive.m_nCount; i++)
    {
        Particle* pParticle = m_lstActive.m_pList[i];
        if (pParticle->m_bReserved6C)
            continue;

        Vector vOldPosition = pParticle->m_vPosition;
        if (bInheriting)
        {
            vPosition = (m_qOrientation * -m_qWorldOrientation) *
                        (pParticle->m_vPosition - m_vLastOrigin);
            pParticle->m_vDrift = (m_qOrientation * -m_qWorldOrientation) *
                                  pParticle->m_vDrift;
            vPosition += m_vOrigin;
        }
        else
        {
            vPosition = Vector(pParticle->m_vPosition);
        }

        if (pNode->IsInheritingLocal() &&
            !pNode->IsInheriting())
        {
            if (m_pAurObject)
            {
                Vector vOffset = m_pAurObject->m_vPosition - m_vReserved158;
                vPosition += vOffset;
                pParticle->m_vOrigin += vOffset;
            }
        }
        if (pNode->IsInheritingPart())
        {
            Vector vOffset = m_vOrigin - m_vReserved158;
            vPosition += vOffset;
            pParticle->m_vOrigin += vOffset;
        }

        pParticle->m_vPosition = pParticle->m_vVelocity * pParticle->m_fSpeed * fTimeStep +
                                (vPosition + pParticle->m_vDrift * fTimeStep);
        if (pNode->IsAffectedByWind())
        {
            if (m_lstActive.m_nCount < EMITTER_WIND_SAMPLE_THRESHOLD ||
                i % EMITTER_WIND_SAMPLE_STRIDE == 0)
                vWind = currentscene->GetWindVector(vPosition, fTimeStep);
            pParticle->m_vPosition += vWind;
        }

        pParticle->m_vDrift.m_fZ -= fTimeStep * m_fMass * (-EMITTER_GRAVITY);
        pParticle->m_fSpeed *= 1.0f - fTimeStep;
        if (pParticle->m_fSpeed < 0.0f)
            pParticle->m_fSpeed *= -1.0f;

        if (bBouncing)
        {
            m_pHitInfo->ClearOutput();
            vPosition = pParticle->m_vPosition;
            if (pNode->m_sChunkName[0])
            {
                m_pHitInfo->m_lstHits.SetSize(0);
                Gob* pGob = static_cast<ChunkyParticle*>(pParticle)->m_pGob;
                if (pGob)
                    m_pHitInfo->m_lstHits.Add(pGob);
            }
            if (currentscene->HitCheck(vOldPosition, pParticle->m_vPosition,
                                       &pParticle->m_vPosition, m_pHitInfo))
            {
                float fSpeed;
                if (EmitterBounceType == EMITTER_BOUNCE_DAMPED)
                {
                    if (m_fBounceCoefficient == 0.0f)
                    {
                        pParticle->m_vDrift = Vector(0.0f, 0.0f, 0.0f);
                        fSpeed = 0.0f;
                    }
                    else
                    {
                        float fBounceScale = EMITTER_BOUNCE_REFERENCE_STEP / fTimeStep;
                        if (fBounceScale > 1.0f)
                            fBounceScale = 1.0f;
                        else if (fBounceScale < EMITTER_BOUNCE_MIN_SCALE)
                            fBounceScale = EMITTER_BOUNCE_MIN_SCALE;

                        pParticle->m_vDrift = (m_pHitInfo->m_vOutputNormal * -2.0f *
                            (m_pHitInfo->m_vOutputNormal * pParticle->m_vDrift) +
                            pParticle->m_vDrift) * EMITTER_BOUNCE_SPEED_DAMPING;
                        float fZ = fBounceScale *
                            (pParticle->m_vDrift.m_fZ * m_fBounceCoefficient);
                        pParticle->m_vDrift.m_fZ = fZ;
                        fSpeed = pParticle->m_vDrift.m_fX * pParticle->m_vDrift.m_fX +
                            pParticle->m_vDrift.m_fY * pParticle->m_vDrift.m_fY +
                            pParticle->m_vDrift.m_fZ * pParticle->m_vDrift.m_fZ;
                    }
                }
                else if (EmitterBounceType == EMITTER_BOUNCE_ELASTIC)
                {
                    pParticle->m_vDrift = (m_pHitInfo->m_vOutputNormal * -2.0f *
                        (m_pHitInfo->m_vOutputNormal * pParticle->m_vDrift) +
                        pParticle->m_vDrift) * m_fBounceCoefficient;
                    fSpeed = magnitude(pParticle->m_vDrift);
                }

                if (EmitterBounceType == EMITTER_BOUNCE_DAMPED ?
                    fSpeed < EMITTER_BOUNCE_REST_SPEED_SQUARED :
                    acos(AbsoluteValue(m_pHitInfo->m_vOutputNormal.m_fX)) > EMITTER_BOUNCE_REST_ANGLE &&
                    acos(AbsoluteValue(m_pHitInfo->m_vOutputNormal.m_fY)) > EMITTER_BOUNCE_REST_ANGLE &&
                    fTimeStep * EMITTER_BOUNCE_REST_SPEED_PER_SECOND > fSpeed)
                {
                    pParticle->m_bReserved6C = 1;
                    pParticle->m_qOrientation = RotationArc(Vector(0.0f, 0.0f, 1.0f),
                                                           m_pHitInfo->m_vOutputNormal);
                    pParticle->m_vPosition += m_pHitInfo->m_vOutputNormal * EMITTER_BOUNCE_SURFACE_OFFSET;
                    pParticle->m_vHeading = m_pHitInfo->m_vOutputNormal;
                    if (pNode->m_sChunkName[0])
                        pParticle->m_vPosition += m_fXSizeStart * m_pHitInfo->m_vOutputNormal;
                }
            }
        }

        if (m_bWeather && g_pWeatherCamera)
        {
            Vector vCamera = g_pWeatherCamera->GetPosition();
            static float halfWidth = 5.0f;
            static float halfHeight = 5.0f;
            static float width = halfWidth + halfWidth;
            static float height = halfHeight + halfHeight;
            Vector vOffset = pParticle->m_vPosition - vCamera;
            vOffset.m_fX += halfWidth;
            vOffset.m_fY += halfHeight;
            if (vOffset.m_fX >= 0.0f)
                vOffset.m_fX = (float)fmod(vOffset.m_fX, width) - halfWidth;
            else
                vOffset.m_fX = (float)fmod(vOffset.m_fX, width) + halfWidth;
            if (vOffset.m_fY >= 0.0f)
                vOffset.m_fY = (float)fmod(vOffset.m_fY, height) - halfHeight;
            else
                vOffset.m_fY = (float)fmod(vOffset.m_fY, height) + halfHeight;
            pParticle->m_vPosition = vCamera + vOffset;
        }
    }

    if (m_pAurObject && pNode->IsInheritingLocal())
    {
        m_vReserved158 = m_pAurObject->m_vPosition;
        m_qReserved194 = m_pAurObject->m_qOrientation;
    }
    else if (pNode->IsInheritingPart())
    {
        m_vReserved158 = m_vOrigin;
        m_qReserved194 = m_qOrientation;
    }
}
// PC 0x004BE2E0 GOG 0x004BE3F0
void PartEmitter::moveParticleP2PBezier(float fTimeStep)
{
    Vector vPosition;
    Vector vTarget;
    Vector vNewPosition;
    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);
    int bInheriting = pNode->IsInheriting();

    vTarget = m_pTarget->GetPosition();
    if (m_fTargetSize != 0.0f)
    {
        Vector vRandom;
        int nRandom = rand();
        if (nRandom & 1)
            vRandom.m_fX = nRandom % (int)(m_fTargetSize * 50.0f) / 100.0f;
        else
            vRandom.m_fX = -(nRandom % (int)(m_fTargetSize * 50.0f)) / 100.0f;
        nRandom = rand();
        if (nRandom & 1)
            vRandom.m_fY = nRandom % (int)(m_fTargetSize * 50.0f) / 100.0f;
        else
            vRandom.m_fY = -(nRandom % (int)(m_fTargetSize * 50.0f)) / 100.0f;
        nRandom = rand();
        if (nRandom & 1)
            vRandom.m_fZ = nRandom % (int)(m_fTargetSize * 50.0f) / 100.0f;
        else
            vRandom.m_fZ = -(nRandom % (int)(m_fTargetSize * 50.0f)) / 100.0f;
        vTarget += vRandom;
    }
#ifdef _XBOX
    unsigned char bBouncing = pNode->IsBouncing();
#else
    bool bBouncing = pNode->m_EmitterFlags.m_bBouncing != 0;
#endif
    for (int i = 0; i < m_lstActive.m_nCount; i++)
    {
        Particle* pParticle = m_lstActive.m_pList[i];
        Vector vOldPosition = pParticle->m_vPosition;
        if (bInheriting)
        {
            vPosition = (m_qOrientation * -m_qWorldOrientation) *
                        (pParticle->m_vPosition - m_vLastOrigin);
            pParticle->m_vDrift = (m_qOrientation * -m_qWorldOrientation) *
                                   pParticle->m_vDrift;
            vPosition += m_vOrigin;
        }
        else
        {
            vPosition = pParticle->m_vPosition;
        }
        vNewPosition = vPosition + fTimeStep * pParticle->m_vDrift;
        if (pNode->IsAffectedByWind())
        {
            vNewPosition += currentscene->GetWindVector(vPosition, fTimeStep);
        }
        float fTime = pParticle->m_fAge / m_fLifeTime;
        if (fTime > 1.0f)
            fTime = 1.0f;
        float fStart = (1.0f - fTime) * ((1.0f - fTime) * (1.0f - fTime));
        float fControl1 = (1.0f - fTime) * (1.0f - fTime) * fTime * 3.0f;
        float fControl2 = (1.0f - fTime) * fTime * fTime * 3.0f;
        float fEnd = fTime * fTime * fTime;
        Vector vBezier = fStart * m_vOrigin +
            fControl1 * (m_vOrigin + m_fPointToPointBezier2 * m_vDirection) +
            fControl2 * (vTarget + m_fPointToPointBezier3 * m_pTarget->GetOrientation().zdir()) +
            fEnd * vTarget;
        double fBlend = pow(fEnd, m_fCombineTime);
        vNewPosition = fBlend * vBezier + (1.0f - (float)fBlend) * vNewPosition;
        pParticle->m_vPosition = vNewPosition +
            (pParticle->m_fSpeed * pParticle->m_vVelocity) * fTimeStep;
        pParticle->m_vDrift.m_fZ -= fTimeStep * m_fMass * -EMITTER_GRAVITY;
        pParticle->m_fSpeed *= 1.0f - fTimeStep;
        if (pParticle->m_fSpeed < 0.0f)
            pParticle->m_fSpeed *= -1.0f;
        m_pHitInfo->ClearOutput();
        if (bBouncing)
        {
            vPosition = pParticle->m_vPosition;
            if (pNode->m_sChunkName[0])
            {
                m_pHitInfo->m_lstHits.SetSize(0);
                Gob* pGob = static_cast<ChunkyParticle*>(pParticle)->m_pGob;
                if (pGob)
                    m_pHitInfo->m_lstHits.Add(pGob);
                vNewPosition = pParticle->m_vPosition +
                               (pParticle->m_vPosition - vOldPosition) * m_fXSizeStart;
            }
            else
            {
                vNewPosition = pParticle->m_vPosition;
            }
            if (currentscene->HitCheck(vOldPosition, vNewPosition,
                                      &pParticle->m_vPosition, m_pHitInfo))
            {
                pParticle->m_vDrift =
                    (m_pHitInfo->m_vOutputNormal * -2.0f *
                     (m_pHitInfo->m_vOutputNormal * pParticle->m_vDrift) +
                     pParticle->m_vDrift) * m_fBounceCoefficient;
                pParticle->m_vPosition += m_pHitInfo->m_vOutputNormal * EMITTER_COLLISION_OFFSET;
                if (pParticle->m_vDrift.m_fX * pParticle->m_vDrift.m_fX +
                    pParticle->m_vDrift.m_fY * pParticle->m_vDrift.m_fY +
                    pParticle->m_vDrift.m_fZ * pParticle->m_vDrift.m_fZ < EMITTER_SETTLED_SPEED_SQUARED)
                {
                    pParticle->m_bReserved6C = 1;
                    pParticle->m_qOrientation = RotationArc(Vector(0.0f, 0.0f, 1.0f),
                                                           m_pHitInfo->m_vOutputNormal);
                    pParticle->m_vPosition += m_pHitInfo->m_vOutputNormal * EMITTER_SETTLED_OFFSET;
                    pParticle->m_vHeading = m_pHitInfo->m_vOutputNormal;
                }
                if (pNode->m_sChunkName[0])
                    pParticle->m_vPosition += m_fXSizeStart * m_pHitInfo->m_vOutputNormal;
            }
        }
    }
}
// PC 0x004BF280 GOG 0x004BF390
void PartEmitter::moveParticleP2PGrav(float fTimeStep)
{
    MdlNodeEmitter* pNode = (MdlNodeEmitter*)m_pNode;
    Vector vPosition;
    Vector vTarget;
    Vector vNext;
    Vector vDelta;
    Vector vAcceleration;
#ifdef _XBOX
    unsigned char bInheriting = pNode->IsInheriting();
#else
    unsigned char bInheriting = (unsigned char)pNode->m_EmitterFlags.m_bInheriting;
#endif
    vTarget = m_pTarget->GetPosition();
    if (m_fTargetSize != 0.0f)
    {
        Vector vRandom;
        int nRandom = rand();
        if (nRandom & 1)
            vRandom.m_fX = nRandom % (int)(m_fTargetSize * 50.0f) * 0.01f;
        else
            vRandom.m_fX = -(nRandom % (int)(m_fTargetSize * 50.0f)) * 0.01f;
        nRandom = rand();
        if (nRandom & 1)
            vRandom.m_fY = nRandom % (int)(m_fTargetSize * 50.0f) * 0.01f;
        else
            vRandom.m_fY = -(nRandom % (int)(m_fTargetSize * 50.0f)) * 0.01f;
        nRandom = rand();
        if (nRandom & 1)
            vRandom.m_fZ = nRandom % (int)(m_fTargetSize * 50.0f) * 0.01f;
        else
            vRandom.m_fZ = -(nRandom % (int)(m_fTargetSize * 50.0f)) * 0.01f;
        vTarget += vRandom;
    }
#ifdef _XBOX
    unsigned char bBouncing = pNode->IsBouncing();
#else
    unsigned char bBouncing = (unsigned char)pNode->m_EmitterFlags.m_bBouncing;
#endif
    for (int i = 0; i < m_lstActive.m_nCount; i++)
    {
        Particle* pParticle = m_lstActive[i];
        Vector vPrevious = pParticle->m_vPosition;
        if (bInheriting)
        {
            vPosition = (m_qOrientation * -m_qWorldOrientation) *
                (pParticle->m_vPosition - m_vLastOrigin);
            pParticle->m_vDrift = (m_qOrientation * -m_qWorldOrientation) *
                pParticle->m_vDrift;
            vPosition += m_vOrigin;
        }
        else
            vPosition = pParticle->m_vPosition;
        if (pNode->IsInheritingLocal() &&
            !pNode->IsInheriting() && m_pAurObject)
        {
            vPosition = m_pAurObject->m_qOrientation *
                (-m_qReserved194 * (vPosition - m_vReserved158));
            Vector vTranslation = m_pAurObject->m_vPosition - m_vReserved158;
            vPosition += vTranslation;
            pParticle->m_vOrigin += vTranslation;
            vPosition += m_vReserved158;
            pParticle->m_vDrift = m_pAurObject->m_qOrientation *
                (-m_qReserved194 * pParticle->m_vDrift);
        }
        if (pNode->IsInheritingPart() && m_pAurObject)
        {
            vPosition = m_pAurObject->m_qOrientation *
                (-m_qReserved194 * (vPosition - m_vReserved158));
            Vector vTranslation = m_vOrigin - m_vReserved158;
            vPosition += vTranslation;
            pParticle->m_vOrigin += vTranslation;
            pParticle->m_vOrigin = pParticle->m_vOrigin - m_pAurObject->m_vPosition;
            pParticle->m_vOrigin = m_pAurObject->m_qOrientation *
                (-m_qReserved194 * pParticle->m_vOrigin);
            pParticle->m_vOrigin = pParticle->m_vOrigin + m_pAurObject->m_vPosition;
            vPosition += m_vReserved158;
            pParticle->m_vDrift = m_pAurObject->m_qOrientation *
                (-m_qReserved194 * pParticle->m_vDrift);
        }
        vNext = vPosition + fTimeStep * pParticle->m_vDrift;
        if (pNode->IsAffectedByWind())
            vNext += currentscene->GetWindVector(vPosition, fTimeStep);
        vDelta = vTarget - vPosition;
        float fThresholdSquared = m_fThreshold * m_fThreshold;
        if (vDelta * vDelta <= fThresholdSquared)
        {
            pParticle->m_fAge = m_fLifeTime;
            return;
        }
        pParticle->m_vDrift = (float)pow(m_fDrag, fTimeStep) * pParticle->m_vDrift;
        vAcceleration = normalize(vDelta) * m_fGravity * fTimeStep;
        vNext += vAcceleration * fTimeStep;
        pParticle->m_vDrift += vAcceleration;
        vDelta = vNext - vPosition;
        if (!equal(vDelta, Vector()))
            pParticle->m_vHeading = normalize(vDelta);
        vDelta = segmentproject(vPosition, vNext, vTarget);
        vDelta = vTarget - vDelta;
        if (vDelta * vDelta <= fThresholdSquared)
            pParticle->m_fAge = m_fLifeTime;
        pParticle->m_vPosition = vNext +
            (pParticle->m_fSpeed * pParticle->m_vVelocity) * fTimeStep;
        pParticle->m_vDrift.m_fZ -= fTimeStep * m_fMass * EMITTER_GRAVITY_MAGNITUDE;
        pParticle->m_fSpeed = (1.0f - fTimeStep) * pParticle->m_fSpeed;
        if (pParticle->m_fSpeed < 0.0f)
            pParticle->m_fSpeed *= -1.0f;
        m_pHitInfo->ClearOutput();
        if (bBouncing)
        {
            vPosition = pParticle->m_vPosition;
            if (pNode->m_sChunkName[0])
            {
                m_pHitInfo->m_lstHits.SetSize(0);
                Gob* pGob = ((ChunkyParticle*)pParticle)->m_pGob;
                if (pGob)
                    m_pHitInfo->m_lstHits.Add(pGob);
                vNext = (pParticle->m_vPosition - vPrevious) * m_fXSizeStart + pParticle->m_vPosition;
            }
            else
                vNext = pParticle->m_vPosition;
            if (currentscene->HitCheck(vPrevious, vNext,
                                      &pParticle->m_vPosition, m_pHitInfo))
            {
                pParticle->m_vDrift =
                    ((-2.0f * m_pHitInfo->m_vOutputNormal) *
                     (m_pHitInfo->m_vOutputNormal * pParticle->m_vDrift) +
                     pParticle->m_vDrift) * m_fBounceCoefficient;
                pParticle->m_vPosition += m_pHitInfo->m_vOutputNormal * EMITTER_COLLISION_PUSH_OFF;
                if (pParticle->m_vDrift.m_fX * pParticle->m_vDrift.m_fX +
                    pParticle->m_vDrift.m_fY * pParticle->m_vDrift.m_fY +
                    pParticle->m_vDrift.m_fZ * pParticle->m_vDrift.m_fZ <
                    EMITTER_SETTLED_SPEED_SQUARED)
                {
                    pParticle->m_bReserved6C = 1;
                    pParticle->m_qOrientation = RotationArc(
                        Vector(0.0f, 0.0f, 1.0f), m_pHitInfo->m_vOutputNormal);
                    pParticle->m_vPosition += m_pHitInfo->m_vOutputNormal * EMITTER_SETTLED_PUSH_OFF;
                    pParticle->m_vHeading = m_pHitInfo->m_vOutputNormal;
                }
                if (pNode->m_sChunkName[0])
                    pParticle->m_vPosition += m_fXSizeStart * m_pHitInfo->m_vOutputNormal;
            }
        }
    }
    if (m_pAurObject && pNode->IsInheritingLocal())
    {
        m_vReserved158 = m_pAurObject->m_vPosition;
        m_qReserved194 = m_pAurObject->m_qOrientation;
    }
    else if (pNode->IsInheritingPart())
    {
        m_vReserved158 = m_vOrigin;
        m_qReserved194 = m_pAurObject->m_qOrientation;
    }
}
// PC 0x004C1590 GOG 0x004C16A0
void PartEmitter::chkParticleLife(float fTimeStep)
{
    int i;
    bool bChunky = false;

    if (static_cast<MdlNodeEmitter*>(m_pNode)->m_sChunkName[0])
    {
        bChunky = true;
    }

    for (i = m_lstActive.m_nCount - 1; i >= 0; i--)
    {
        Particle* pParticle = m_lstActive.m_pList[i];

        if (m_fLifeTime >= 0.0f && pParticle->m_fAge >= m_fLifeTime)
        {
            m_lstActive.DelIndex(i);

            m_lstFree.Add(pParticle);

            if (bChunky)
            {
                ChunkyParticle* pChunk = static_cast<ChunkyParticle*>(pParticle);
                if (pChunk->m_pGob)
                {
                    pChunk->m_pGob->AttachToScene(0);
                }
            }
        }
        else
        {
            pParticle->m_fAge += fTimeStep;
            if (pParticle->m_fAge < 0.0f)
            {
                pParticle->m_fAge = 0;
                pParticle->m_fFrameStartAge = 0;
            }
        }
    }
}
// PC 0x004C16A0 GOG 0x004C17B0
Particle::Particle(PartEmitter* pEmitter, float fTime)
{
    m_pEmitter = pEmitter;
    Particle::initialize(fTime, 1.0f);
    ++totalparticles;
}
// PC 0x004BD0E0 GOG 0x004BD1F0
void Particle::initialize(float fTime, float fScale)
{
    int nJitter;

    m_bReserved6C = 0;
    m_fRotation = 0;
    m_fFrameStartAge = 0;
    m_fAge = 0;
    m_fFrameAge = 0;
    m_qOrientation = m_pEmitter->m_qOrientation;
    m_fSpeed = 0.0f;
    m_vVelocity.m_fZ = 0.0f;
    m_vVelocity.m_fY = 0.0f;
    m_vVelocity.m_fX = 0.0f;
    nJitter = 0;
    m_nFrame = (int)m_pEmitter->m_fFrameStart;

    float fRandom = m_pEmitter->m_fVelocityRandom;

    if (fRandom > 0.01)
    {
        nJitter = rand() % (int)(fRandom * 100.0f);

        if (rand() % 2 > 0)
        {
            nJitter = -nJitter;
        }
    }

    int nWidth = (int)(fScale * m_pEmitter->m_fXSize * 100.0f);
    int nHeight = (int)(fScale * m_pEmitter->m_fYSize * 100.0f);

    if (nWidth != 0 || nHeight != 0)
    {
        randomPosition(nWidth, nHeight);
    }
    else
    {
        m_vPosition = m_pEmitter->m_vOrigin;
    }

    m_vOrigin = m_vPosition;

    if (static_cast<MdlNodeEmitter*>(m_pEmitter->m_pNode)
            ->IsInheritingVelocity())
    {
        inheritVelocity(fTime);
    }

    if (m_pEmitter->m_fSpread != 0.0f)
    {
        randomDirection(nWidth, nHeight);
    }

    m_vHeading = m_qOrientation.zdir();

    m_vDrift = m_vHeading * (nJitter * 0.01f + m_pEmitter->m_fVelocity);
}
// PC 0x004C1710 GOG 0x004C1820
ChunkyParticle::ChunkyParticle(PartEmitter* pEmitter, float fTime)
    : Particle(pEmitter, fTime)
{
    if (pEmitter != 0)
    {
        m_pGob = static_cast<Gob*>(NewCAurObject(
            static_cast<MdlNodeEmitter*>(pEmitter->m_pNode)->m_sChunkName, "",
            0, 0));

        if (m_pGob != 0)
        {
            m_pGob->AttachToScene(currentscene);
        }
    }
}

#ifdef _XBOX
unsigned char MdlNodeEmitter::IsBouncing() { unsigned char bBouncing = m_EmitterFlags.m_bBouncing; return bBouncing; }
#else
inline unsigned char MdlNodeEmitter::IsBouncing() { return (unsigned char)m_EmitterFlags.m_bBouncing; }
#endif
inline void PartEmitter::SetLastOrigin(Vector v) { m_vLastOrigin = v; }
inline void LightningEmitter::SetReserved1F8(Vector v) { m_vReserved1F8 = v; }
// PC 0x004C1800 GOG 0x004C1910
void PartEmitter::AnimateParticles(float fTimeStep)
{
    if (m_pAurObject != 0 && m_pAurObject->m_pTileData != 0 &&
        m_pAurObject->m_pTileData->m_fFadeAlpha < 1.0f)
        return;

    MdlNodeEmitter* pNode = m_pNode->AsMdlNodeEmitter();
    Vector vOffset;
    if (pNode->IsInheritingLocal())
    {
        if (m_pAurObject != 0 && m_pReserved1F7 != 0)
            vOffset = m_pAurObject->m_vPosition - m_vReserved158;
    }

    if (m_pReserved1F7 == 0)
    {
        GetWorld(m_vOrigin, m_qOrientation);
        if (m_pAurObject != 0)
        {
            if (pNode->IsInheritingPart() ||
                pNode->IsInheritingLocal())
            {
                m_vReserved158 = m_pAurObject->m_vPosition;
                m_qReserved194 = m_pAurObject->m_qOrientation;
            }
        }
        if (pNode->IsInheritingPart())
        {
            m_vReserved158 = m_vOrigin;
            m_qReserved194 = m_qOrientation;
        }

        if (m_pTarget != 0)
        {
            Vector vEnd = m_pTarget->GetPosition();
            Quaternion qWorld;
            Vector vStart;
            GetWorld(vStart, qWorld);
            m_fRadius = magnitude(m_pTarget->GetPosition() - vStart);
            LightningEmitter* pLightning = AsLightningEmitter();
            if (pLightning != 0)
            {
                m_fBirthRate = m_fLightningSubDivisions * m_fRadius + 2.0f;
                Vector vForward = Part::m_qOrientation.zdir();
                int nPoints = (int)(m_fNumControlPoints * m_fRadius + 0.5f);
                pLightning->m_lstReserved210.SetSize(nPoints + 2);
                pLightning->m_lstReserved21C.SetSize(nPoints + 2);
                pLightning->m_lstReserved228.SetSize(nPoints + 2);
                pLightning->m_lstReserved234.SetSize(nPoints + 2);
                pLightning->m_lstReserved240.SetSize(nPoints + 2);
                pLightning->m_lstReserved24C.SetSize(nPoints + 2);

                Vector vTangent;
                pLightning->m_lstReserved210[0] =
                    pLightning->m_lstReserved228[0] = vStart;
                pLightning->m_lstReserved210[nPoints + 1] =
                    pLightning->m_lstReserved228[nPoints + 1] = vEnd;
                pLightning->m_lstReserved234[0] =
                    pLightning->m_lstReserved24C[0] = vForward * m_fTangentLength;
                pLightning->m_lstReserved234[nPoints + 1] =
                    pLightning->m_lstReserved24C[nPoints + 1] =
                        normalize(m_pTarget->GetPosition() - vStart) * m_fTangentLength;

                for (int i = 1; i <= nPoints; i++)
                {
                    float fPosition = (float)i / (nPoints + 1);
                    if (fPosition > 1.0f)
                        fPosition = 1.0f;
                    float fRemaining = 1.0f - fPosition;
                    float fStartWeight = fRemaining * fRemaining * fRemaining;
                    float fControl1Weight = fRemaining * fRemaining * fPosition * 3.0f;
                    float fControl2Weight = fRemaining * fPosition * fPosition * 3.0f;
                    float fEndWeight = fPosition * fPosition * fPosition;
                    pLightning->m_lstReserved210[i] = pLightning->m_lstReserved228[i] =
                        vStart * fStartWeight +
                        (vStart + vForward * m_fTangentLength) * fControl1Weight +
                        (vEnd + m_pTarget->GetOrientation().zdir() * m_fTangentLength) *
                            fControl2Weight + vEnd * fEndWeight;

                    fStartWeight = -3.0f * fRemaining * fRemaining;
                    fControl1Weight = (1.0f - 3.0f * fPosition) * fRemaining * 3.0f;
                    fControl2Weight = (2.0f - 3.0f * fPosition) * fPosition * 3.0f;
                    fEndWeight = 3.0f * fPosition * fPosition;
                    vTangent = vStart * fStartWeight +
                        (vStart + vForward * m_fTangentLength) * fControl1Weight +
                        (vEnd + m_pTarget->GetOrientation().zdir() * m_fTangentLength) *
                            fControl2Weight + vEnd * fEndWeight;
                    vTangent.Normalize();
                    pLightning->m_lstReserved234[i] = pLightning->m_lstReserved24C[i] =
                        vTangent * m_fTangentLength;
                }

                Vector vDirection = (m_pTarget->GetPosition() - vStart) / m_fRadius;
                pLightning->m_vReserved27C = Vector(-vDirection.m_fY,
                                                   vDirection.m_fZ,
                                                   (-vDirection.GetX()));
                pLightning->SetReserved1F8(m_pTarget->GetPosition() - vStart);
                pLightning->m_vReserved204 = vStart;
            }
        }
        m_pReserved1F7 = 1;
    }

    if (!pNode->IsInheritingLocal())
    {
        SetLastOrigin(m_vOrigin);
        m_qWorldOrientation = m_qOrientation;
    }
    if (pNode->IsInheritingLocal())
    {
        SetLastOrigin(vOffset + m_vOrigin);
        m_qWorldOrientation = m_qOrientation;
    }
    GetWorld(m_vOrigin, m_qOrientation);
    m_vXAxis = m_qOrientation.xdir();

    m_vXAxis = m_vXAxis;
    m_vYAxis = m_qOrientation.ydir();
    m_vDirection = m_qOrientation.zdir();
    Update(fTimeStep);
}

static inline float GetGobAlpha(Gob* pGob)
{
    float fAlpha = 1.0f;
    if (pGob)
        fAlpha = pGob->m_fAlpha;
    return fAlpha;
}
#ifndef _XBOX
void PartEmitter::RenderLinked()  // PC 0x004C2590 GOG 0x004C26A0
{
    static unsigned long poolSize = 1000;
    static RenderVertexVTC* particlePool =
        static_cast<RenderVertexVTC*>(operator new(sizeof(RenderVertexVTC) * 1000));

    if (m_pAurObject && m_pAurObject->m_pTileData &&
        m_pAurObject->m_pTileData->m_fFadeAlpha < 1.0f)
        return;

    Quaternion qRotation;
    Quaternion qCamera;
    int bFog = 0;
    int i;
    Vector vCameraX = CurrentCamera->m_qOrientation.xdir();
    Vector vCameraY(
        2.0f * (CurrentCamera->m_qOrientation.m_fX * CurrentCamera->m_qOrientation.m_fY -
                CurrentCamera->m_qOrientation.m_fW * CurrentCamera->m_qOrientation.m_fZ),
        1.0f - 2.0f * (CurrentCamera->m_qOrientation.m_fX * CurrentCamera->m_qOrientation.m_fX +
                       CurrentCamera->m_qOrientation.m_fZ * CurrentCamera->m_qOrientation.m_fZ),
        2.0f * ((double)CurrentCamera->m_qOrientation.m_fY * CurrentCamera->m_qOrientation.m_fZ +
                CurrentCamera->m_qOrientation.m_fX * CurrentCamera->m_qOrientation.m_fW));
    Vector vCameraZ = CurrentCamera->m_qOrientation.zdir();
    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);
    float fFrameTime = m_fFramesPerSecond;
    List<Vector> lstPositions;
    float fScale = 1.0f;

    if (m_lstActive.m_nCount == 0)
        return;

    LightningEmitter* pLightning = AsLightningEmitter();
    lstPositions.SetSize(m_lstActive.m_nCount);
    for (i = 0; i < m_lstActive.m_nCount; i++)
    {
        lstPositions[i] = m_lstActive[i]->m_vPosition;
        if (pLightning)
            lstPositions[i] += pLightning->m_lstReserved270[i];
    }

    if (renderemitterspheres)
        RenderBoundingSphere();

    if (4 * m_lstActive.m_nCount > poolSize)
    {
        unsigned long nOldSize = poolSize;
        poolSize = 8 * m_lstActive.m_nCount;
        particlePool = ResizeEmitterPool(particlePool, nOldSize, poolSize);
    }
    if (m_pAurObject && m_pAurObject->m_fScale != 1.0f)
        fScale = m_pAurObject->m_fScale;
    if (fFrameTime > 0.0f)
        fFrameTime = 1.0f / fFrameTime;
    else
        fFrameTime = 0.0f;

    m_pEmitterMaterial->BindTexture0();
    m_pEmitterMaterial->SetBlendingMode(0);
    qCamera = CurrentCamera->m_qOrientation;
    float fInverseLife;
    if (m_pReserved1F4 == LIGHTNING)
        fInverseLife = 1.0f / m_lstActive.m_nCount;
    else if (m_fLifeTime > 0.0f)
        fInverseLife = 1.0f / m_fLifeTime;
    else
        fInverseLife = 0.01f;
    int nXGrid = pNode->m_nXGrid;
    if (!nXGrid) nXGrid = 1;
    int nYGrid = pNode->m_nYGrid;
    if (!nYGrid) nYGrid = 1;

    GLRender_SetDepthMask(GL_FALSE);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_Enable(ENABLE_BLEND);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    if (m_nBlendMode == BLEND_NORMAL)
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                    BLEND_ONE_MINUS_SRC_ALPHA);
    else if (m_nBlendMode == BLEND_PUNCH_THROUGH)
    {
        GLRender_Disable(ENABLE_BLEND);
        GLRender_Enable(ENABLE_ALPHA_TEST);
    }
    else
    {
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                    BLEND_ONE);
        bFog = GLRender_IsEnabled(ENABLE_FOG);
        GLRender_Disable(ENABLE_FOG);
    }
    updateParticleFrame(fFrameTime);
    particlecount += m_lstActive.m_nCount;
    float fUScale = 1.0f / nXGrid;
    float fVScale = 1.0f / nYGrid;
    float fTileAlpha = GetTileAlpha();
    float fGobAlpha = GetGobAlpha(m_pAurObject);

    for (i = 0; i < m_lstActive.m_nCount; i++)
    {
        Vector vColor;
        float fAlpha;
        float fXSize;
        float fYSize;
        if (m_fLifeTime >= 0.0f)
        {
            float fAge;
            if (m_pReserved1F4 == LIGHTNING)
                fAge = i * fInverseLife;
            else
                fAge = fInverseLife * m_lstActive[i]->m_fAge;
            if (m_fPercentStart == EMITTER_UNSET_COLOR_KEY_TIME)
            {

                vColor = Interpolate(m_vColorStart, m_vColorEnd, fAge);
                fAlpha = (1.0f - fAge) * m_fAlphaStart + fAge * m_fAlphaEnd;
                fXSize = ((1.0f - fAge) * m_fXSizeStart + fAge * m_fXSizeEnd) * 0.5f;
                if (m_fYSizeStart != 0.0f || m_fYSizeEnd != 0.0f)
                    fYSize = (fAge * m_fYSizeEnd + (1.0f - fAge) * m_fYSizeStart) * 0.5f;
                else
                    fYSize = fXSize;
            }
            else if (fAge >= m_fPercentEnd)
            {
                vColor = m_vColorEnd;
                fAlpha = m_fAlphaEnd;
                fXSize = m_fXSizeEnd;
                fYSize = m_fYSizeEnd != 0.0f ? m_fYSizeEnd : fXSize;
            }

            else if (fAge >= m_fPercentMid)
            {
                fAge = (fAge - m_fPercentMid) / (m_fPercentEnd - m_fPercentMid);
                vColor = Interpolate(m_vColorMid, m_vColorEnd, fAge);
                fAlpha = (1.0f - fAge) * m_fAlphaMid + fAge * m_fAlphaEnd;
                fXSize = ((1.0f - fAge) * m_fXSizeMid + fAge * m_fXSizeEnd) * 0.5f;
                if (m_fYSizeMid == 0.0f && m_fYSizeEnd == 0.0f)
                    fYSize = fXSize;
                else
                    fYSize = (fAge * m_fYSizeEnd + (1.0f - fAge) * m_fYSizeMid) * 0.5f;
            }
            else if (fAge >= m_fPercentStart)
            {
                fAge = (fAge - m_fPercentStart) / (m_fPercentMid - m_fPercentStart);
                vColor = Interpolate(m_vColorStart, m_vColorMid, fAge);
                fAlpha = (1.0f - fAge) * m_fAlphaStart + fAge * m_fAlphaMid;
                fXSize = ((1.0f - fAge) * m_fXSizeStart + fAge * m_fXSizeMid) * 0.5f;
                if (m_fYSizeStart == 0.0f && m_fYSizeMid == 0.0f)
                    fYSize = fXSize;
                else
                    fYSize = (fAge * m_fYSizeMid + (1.0f - fAge) * m_fYSizeStart) * 0.5f;
            }
            else
            {
                vColor = m_vColorStart;
                fAlpha = m_fAlphaStart;
                fXSize = m_fXSizeStart;
                if (m_fYSizeStart != 0.0f)
                    fYSize = m_fYSizeStart;
                else
                    fYSize = fXSize;
            }
            if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
            else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
            if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
            else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
            if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
            else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
            if (fAlpha < 0.0f) fAlpha = 0.0f;
            else if (fAlpha > 1.0f) fAlpha = 1.0f;
        }
        else
        {
            vColor = m_vColorStart;
            fAlpha = m_fAlphaStart;
            if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
            else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
            if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
            else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
            if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
            else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
            if (fAlpha < 0.0f) fAlpha = 0.0f;
            else if (fAlpha > 1.0f) fAlpha = 1.0f;
            fXSize = m_fXSizeStart * 0.5f;
            if (m_fYSizeStart == 0.0f && m_fYSizeMid == 0.0f && m_fYSizeEnd == 0.0f)
                fYSize = fXSize;
            else
                fYSize = m_fYSizeStart * 0.5f;
        }

        qRotation = Quaternion(vCameraZ, m_lstActive[i]->m_fRotation);
        qRotation = qRotation * qCamera;
        int nRow = (int)(m_lstActive[i]->m_nFrame * fUScale);
        int nColumn = m_lstActive[i]->m_nFrame - nXGrid * nRow;
        if (fScale != 1.0f)
        {
            fXSize *= fScale;
            fYSize *= fScale;
        }
        if (i < m_lstActive.m_nCount - 1)
        {
            Vector vSegment = lstPositions[i + 1] - lstPositions[i];
            float fX = vCameraX * vSegment;
            float fY = vSegment * vCameraY;
            float fLength = (float)sqrt(fX * fX + fY * fY);
            Vector vSide;
            Vector vAlong;
            if (fLength > EMITTER_LINK_LENGTH_EPSILON)
            {

                float fInverse = 1.0f / fLength;
                fX *= fInverse;
                fY *= fInverse;
                vSide = vCameraX * fY - vCameraY * fX;
                vAlong = vCameraX * fX + vCameraY * fY;
            }
            else
            {
                vSide = vCameraX;
                vAlong = vCameraY;
            }
            vSide = vSide * (fXSize + fXSize);
            vAlong = vAlong * (fYSize * 0.5f);

            unsigned long nColor = (unsigned long)(vColor.m_fX * 255.0f) |
                (((unsigned long)(vColor.m_fY * 255.0f) |
                (((unsigned long)(vColor.m_fZ * 255.0f) |
                ((unsigned long)(fAlpha * (fGobAlpha * fTileAlpha) * 255.0f) << 8)) << 8)) << 8);

            int nVertex = i * 4;
            *reinterpret_cast<unsigned long*>(particlePool[nVertex].color) = nColor;
            *reinterpret_cast<unsigned long*>(particlePool[nVertex + 1].color) = nColor;
            *reinterpret_cast<unsigned long*>(particlePool[nVertex + 2].color) = nColor;
            *reinterpret_cast<unsigned long*>(particlePool[nVertex + 3].color) = nColor;
            Vector vCorner;
            if (nVertex > 0)
            {
                vCorner.m_fX = particlePool[4 * i - 3].position[0];
                vCorner.m_fY = particlePool[4 * i - 3].position[1];
                vCorner.m_fZ = particlePool[4 * i - 3].position[2];
            }
            else
                vCorner = (vSide + lstPositions[i]) - vAlong;
            particlePool[4 * i].position[0] = vCorner.m_fX;
            particlePool[4 * i].position[1] = vCorner.m_fY;
            particlePool[4 * i].position[2] = vCorner.m_fZ;
            particlePool[4 * i].u = (nColumn + 1) * fUScale;
            particlePool[4 * i].v = 1.0f - (nRow + 1) * fVScale;
            vCorner = (vSide + lstPositions[i + 1]) + vAlong;
            particlePool[4 * i + 1].position[0] = vCorner.m_fX;
            particlePool[4 * i + 1].position[1] = vCorner.m_fY;
            particlePool[4 * i + 1].position[2] = vCorner.m_fZ;
            particlePool[4 * i + 1].u = (nColumn + 1) * fUScale;
            particlePool[4 * i + 1].v = 1.0f - nRow * fVScale;
            vCorner = (lstPositions[i + 1] - vSide) + vAlong;
            particlePool[4 * i + 2].position[0] = vCorner.m_fX;
            particlePool[4 * i + 2].position[1] = vCorner.m_fY;
            particlePool[4 * i + 2].position[2] = vCorner.m_fZ;
            particlePool[4 * i + 2].u = nColumn * fUScale;
            particlePool[4 * i + 2].v = 1.0f - nRow * fVScale;
            if (nVertex > 0)
            {
                vCorner.m_fX = particlePool[4 * i - 2].position[0];
                vCorner.m_fY = particlePool[4 * i - 2].position[1];
                vCorner.m_fZ = particlePool[4 * i - 2].position[2];
            }
            else
                vCorner = (lstPositions[i] - vSide) - vAlong;
            vAlong.m_fX = vAlong.m_fY = vAlong.m_fZ = 0.0f;
            particlePool[4 * i + 3].position[0] = vCorner.m_fX;
            particlePool[4 * i + 3].position[1] = vCorner.m_fY;
            particlePool[4 * i + 3].position[2] = vCorner.m_fZ;
            particlePool[4 * i + 3].u = nColumn * fUScale;
            particlePool[4 * i + 3].v = 1.0f - (nRow + 1) * fVScale;
        }
    }
    countfaces += m_lstActive.m_nCount;
    int nVertices = 4 * m_lstActive.m_nCount - 4;
    GLRender::RenderObjectsVTCQuadUP(particlePool, nVertices);
    GLRender_SetDepthMask(GL_TRUE);
    GLRender_Enable(ENABLE_LIGHTING);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                BLEND_ONE_MINUS_SRC_ALPHA);
    if (bFog)
        GLRender_Enable(ENABLE_FOG);
}
#endif
void FountainEmitter::Update(float fTimeStep)  // PC 0x004C37E0 GOG 0x004C38F0
{
    MdlNodeEmitter* pNode = (MdlNodeEmitter*)m_pNode;
    float fBirthRate = m_fBirthRate;
    float fScale = 1.0f;
    if (m_pAurObject)
        fScale = m_pAurObject->m_fScale;

    chkParticleLife(fTimeStep);

    int nMotionBlurred = MotionBlurred;
    float fBlend;
    if (m_nReserved114 == nMotionBlurred)
        fBlend = (fTimeStep < m_fBlurLength ? fTimeStep : m_fBlurLength) / m_fBlurLength;

    for (int i = m_lstActive.m_nCount; i > 0; )
    {
        --i;
        Particle* pParticle = m_lstActive[i];
        if (m_nReserved114 == nMotionBlurred)
        {
            pParticle->m_vOrigin = fBlend * pParticle->m_vPosition +
                (1.0f - fBlend) * pParticle->m_vOrigin;
        }
        pParticle->m_fRotation += fTimeStep * m_fParticleRotation;
        if (pParticle->m_fRotation > TWO_PI)
            pParticle->m_fRotation -= TWO_PI;
        else if (pParticle->m_fRotation < -TWO_PI)
            pParticle->m_fRotation += TWO_PI;
    }

    MoveFunc(fTimeStep);

    if ((int)fBirthRate > 0)
    {
        float fInterval = 1.0f / fBirthRate;
        if (pNode->m_nSpawnType != 0)
        {
            Vector vDistance = m_vOrigin - m_vLastOrigin + m_vReserved14C;
            float fDistance = (float)sqrt(vDistance.m_fY * vDistance.m_fY +
                vDistance.m_fZ * vDistance.m_fZ + vDistance.m_fX * vDistance.m_fX);
            Vector vDirection = vDistance * (fDistance > 0.0f ? 1.0f / fDistance : 0.0f);
            int nParticles = (int)(fDistance * fBirthRate);
            for (int j = 0; j < nParticles; j++)
            {
                Particle* pParticle;
                if (m_lstFree.m_nCount > 0)
                {
                    pParticle = m_lstFree[m_lstFree.m_nCount - 1];
                    m_lstFree.DelIndex(m_lstFree.m_nCount - 1);
                    pParticle->initialize(fTimeStep, fScale);
                }
                else
                {
                    if (pNode->m_sChunkName[0])
                        pParticle = new ChunkyParticle(this, fTimeStep);
                    else
                        pParticle = new Particle(this, fTimeStep);
                    pParticle->initialize(fTimeStep, fScale);
                }
                m_lstActive.Add(pParticle);
                chkRandom(m_lstActive[m_lstActive.m_nCount - 1]);
                if (m_fXSize == 0.0f && m_fYSize == 0.0f)
                {
                    if (m_lstActive.m_nCount > 1)
                        m_lstActive[m_lstActive.m_nCount - 1]->m_vPosition =
                            m_vLastOrigin - m_vReserved14C + ((j + 1) * fInterval) * vDirection;
                    else
                        m_lstActive[0]->m_vPosition = m_vOrigin - vDirection * fDistance;
                }
            }
            m_vReserved14C = vDistance - vDirection * (nParticles * fInterval);
        }
        else
        {
            m_fBirthElapsed += fTimeStep;
            if (m_fBirthElapsed >= fInterval)
            {
                int nVariation;
                roundToInt(nVariation, m_fBirthRateRandom);
                if (nVariation > 0)
                {
                    if (rand() % 2)
                        fBirthRate += rand() % nVariation;
                    else
                        fBirthRate -= rand() % nVariation;
                    if (fBirthRate < 0.0f)
                        fBirthRate = 0.0f;
                }
                for (int j = 0; j < (int)(fBirthRate * m_fBirthElapsed) % ((int)fBirthRate + 1); j++)
                {
                    Particle* pParticle;
                    if (m_lstFree.m_nCount > 0)
                    {
                        pParticle = m_lstFree[m_lstFree.m_nCount - 1];
                        m_lstFree.DelIndex(m_lstFree.m_nCount - 1);
                        pParticle->initialize(fTimeStep, fScale);
                    }
                    else
                    {
                        if (pNode->m_sChunkName[0])
                            pParticle = new ChunkyParticle(this, fTimeStep);
                        else
                            pParticle = new Particle(this, fTimeStep);
                        pParticle->initialize(fTimeStep, fScale);
                    }
                    m_lstActive.Add(pParticle);
                    chkRandom(m_lstActive[m_lstActive.m_nCount - 1]);
                }
                m_fBirthElapsed = 0.0f;
            }
        }
    }
}
// PC 0x004C3FE0 GOG 0x004C40F0
void ExplosionEmitter::Update(float fTimeStep)
{
    int nParticles = (int)m_fBirthRate;
    MdlNodeEmitter* pNode = (MdlNodeEmitter*)m_pNode;
    float fScale = 1.0f;
    if (m_pAurObject)
        fScale = m_pAurObject->m_fScale;

    chkParticleLife(fTimeStep);

    int nMotionBlurred = MotionBlurred;
    int i = m_lstActive.m_nCount;
    float fBlend;
    if (m_nReserved114 == nMotionBlurred)
        fBlend = (fTimeStep < m_fBlurLength ? fTimeStep : m_fBlurLength) / m_fBlurLength;

    for (; i > 0; )
    {
        --i;
        if (m_nReserved114 == nMotionBlurred)
        {
            m_lstActive[i]->m_vOrigin = fBlend * m_lstActive[i]->m_vPosition +
                (1.0f - fBlend) * m_lstActive[i]->m_vOrigin;
        }
        m_lstActive[i]->m_fRotation += fTimeStep * m_fParticleRotation;
        if (m_lstActive[i]->m_fRotation > TWO_PI)
            m_lstActive[i]->m_fRotation -= TWO_PI;
        else if (m_lstActive[i]->m_fRotation < -TWO_PI)
            m_lstActive[i]->m_fRotation += TWO_PI;
    }

    MoveFunc(fTimeStep);

    if (nParticles != 0 && m_bDetonated && m_fReservedF4 < 1.0f)
    {
        if (pNode->m_fBlastRadius != 0.0f)
        {
            Vector vPosition;
            Quaternion qOrientation;
            GetWorld(vPosition, qOrientation);
            currentscene->m_pWindManager->AddPointSourceWind(vPosition,
                pNode->m_fBlastRadius, pNode->m_fBlastLength, 1.0f);
        }

        for (int j = 0; j < nParticles; j++)
        {
            Particle* pParticle;
            if (m_lstFree.m_nCount > 0)
            {
                pParticle = m_lstFree[m_lstFree.m_nCount - 1];
                m_lstFree.DelIndex(m_lstFree.m_nCount - 1);
                pParticle->initialize(fTimeStep, 1.0f);
            }
            else
            {
                if (pNode->m_sChunkName[0])
                    pParticle = new ChunkyParticle(this, fTimeStep);
                else
                    pParticle = new Particle(this, fTimeStep);
                pParticle->initialize(fTimeStep, fScale);
            }
            m_lstActive.Add(pParticle);
            chkRandom(m_lstActive[m_lstActive.m_nCount - 1]);
        }
    }

    m_fReservedF4 = m_bDetonated ? 1.0f : 0.0f;
    m_bDetonated = 0;
}
// PC 0x004C4360 GOG 0x004C4470
void SingleEmitter::Update(float fTimeStep)
{
    MdlNodeEmitter* pNode = (MdlNodeEmitter*)m_pNode;
    if ((int)m_fBirthRate != 0)
    {
        if (m_lstActive.m_nCount == 0)
        {
            if (pNode->m_sChunkName[0] != 0)
                m_lstActive.Add(new ChunkyParticle(this, fTimeStep));
            else
                m_lstActive.Add(new Particle(this, fTimeStep));
            chkRandom(m_lstActive[0]);
        }
        else
        {
            m_lstActive[0]->m_fAge += fTimeStep;
            if (m_fLifeTime > 0.0f)
            {
                if (m_lstActive[0]->m_fAge > m_fLifeTime && pNode->m_nLoop)
                    m_lstActive[0]->m_fAge = 0.0f;
            }
            else if (m_lstActive[0]->m_fAge < 0.0f)
            {
                if (m_fLifeTime > 0.0f)
                    m_lstActive[0]->m_fAge = 0.0f;
                else
                    m_lstActive[0]->m_fAge = fTimeStep;
            }
            m_lstActive[0]->m_fRotation += fTimeStep * m_fParticleRotation;
            if (m_lstActive[0]->m_fRotation > TWO_PI)
                m_lstActive[0]->m_fRotation -= TWO_PI;
            else if (m_lstActive[0]->m_fRotation < -TWO_PI)
                m_lstActive[0]->m_fRotation += TWO_PI;
            if (m_nReserved114 == MotionBlurred)
            {
                float fBlend = (fTimeStep < m_fBlurLength ? fTimeStep : m_fBlurLength) / m_fBlurLength;
                m_lstActive[0]->m_vOrigin = fBlend * m_lstActive[0]->m_vPosition +
                    (1.0f - fBlend) * m_lstActive[0]->m_vOrigin;
            }
            MoveFunc(fTimeStep);
        }
    }
    else if (m_lstActive.m_nCount != 0)
    {
        Particle* pParticle = m_lstActive[0];
        m_lstActive.DelIndex(0);
        delete pParticle;
    }
}

unsigned char MdlNodeEmitter::IsPointToPoint()
{
#ifdef _XBOX
    unsigned char bPointToPoint = m_EmitterFlags.m_bPointToPoint;
    return bPointToPoint;
#else
    return (unsigned char)m_EmitterFlags.m_bPointToPoint;
#endif
}

unsigned char MdlNodeEmitter::IsPointToPointSel()
{
#ifdef _XBOX
    unsigned char bPointToPointSel = m_EmitterFlags.m_bPointToPointSel;
    return bPointToPointSel;
#else
    return (unsigned char)m_EmitterFlags.m_bPointToPointSel;
#endif
}
// PC 0x004C17A0 GOG 0x004C18B0
void PartEmitter::MoveFunc(float fTimeStep)
{
    MdlNodeEmitter* pNode = m_pNode->AsMdlNodeEmitter();

    if (pNode->IsPointToPoint())
    {
        if (m_pTarget != 0)
        {
            if (pNode->IsPointToPointSel())
            {
                moveParticleP2PBezier(fTimeStep);
            }
            else
            {
                moveParticleP2PGrav(fTimeStep);
            }
        }
    }
    else
    {
        moveParticleStandard(fTimeStep);
    }
}
// PC 0x004C4600 GOG 0x004C4710
void LightningEmitter::UpdateControlPoints()
{
    int nInterpolationPoint;
    float fA, fB, fC, fD, fTime;
    int nPositionRandom = (int)(m_fControlPointRadius * 100.0f);
    int nTangentRandom = (int)(m_fTangentSpread * 100.0f);
    Vector vTarget = m_pTarget->GetPosition();
    int nControlPoints = (int)(magnitude(vTarget - m_vOrigin) * m_fNumControlPoints + 0.5f) + 2;
    if (nControlPoints != m_lstReserved210.m_nCount)
    {
        if (nControlPoints > m_lstReserved210.m_nCount)
        {
            Vector vLast = m_lstReserved210[m_lstReserved210.m_nCount - 1];
            m_lstReserved210.SetSize(nControlPoints);
            m_lstReserved234.SetSize(nControlPoints);
            m_lstReserved210[nControlPoints - 1] = vLast;
            m_lstReserved21C.SetSize(nControlPoints);
            m_lstReserved228.SetSize(nControlPoints);
            m_lstReserved240.SetSize(nControlPoints);
            m_lstReserved24C.SetSize(nControlPoints);
        }
        else
        {
            m_lstReserved210[nControlPoints - 1] =
                m_lstReserved210[m_lstReserved210.m_nCount - 1];
            while (nControlPoints < m_lstReserved210.m_nCount)
            {
                m_lstReserved210.DelIndex(m_lstReserved210.m_nCount - 1);
                m_lstReserved21C.DelIndex(m_lstReserved21C.m_nCount - 1);
                m_lstReserved228.DelIndex(m_lstReserved228.m_nCount - 1);
                m_lstReserved234.DelIndex(m_lstReserved234.m_nCount - 1);
                m_lstReserved240.DelIndex(m_lstReserved240.m_nCount - 1);
                m_lstReserved24C.DelIndex(m_lstReserved24C.m_nCount - 1);
            }
        }
        Vector vForward;
        vForward = Part::m_qOrientation.zdir();
        m_lstReserved234[0] = vForward * m_fTangentLength;
        m_lstReserved234[nControlPoints - 1] = normalize(vTarget - m_vOrigin) * m_fTangentLength;
        Vector vTangent;
        for (nInterpolationPoint = 1; nInterpolationPoint < m_lstReserved210.m_nCount - 1; nInterpolationPoint++)
        {
            fTime = (float)nInterpolationPoint / (m_lstReserved210.m_nCount - 1);
            if (fTime > 1.0f)
                fTime = 1.0f;
            float fOther = 1.0f - fTime;
            fA = fOther * fOther * fOther;
            fB = fOther * fOther * fTime * 3.0f;
            fC = fOther * fTime * fTime * 3.0f;
            fD = fTime * fTime * fTime;
            m_lstReserved210[nInterpolationPoint] =
                m_lstReserved210[0] * fA +
                (m_lstReserved210[0] + vForward * m_fTangentLength) * fB +
                (m_lstReserved210[m_lstReserved210.m_nCount - 1] +
                       m_pTarget->GetOrientation().zdir() * m_fTangentLength) * fC +
                m_lstReserved210[m_lstReserved210.m_nCount - 1] * fD;
            fA = fOther * fOther * -3.0f;
            fB = (1.0f - 3.0f * fTime) * fOther * 3.0f;
            fC = (2.0f - 3.0f * fTime) * fTime * 3.0f;
            fD = fTime * fTime * 3.0f;
            vTangent =
                m_lstReserved210[0] * fA +
                (m_lstReserved210[0] + vForward * m_fTangentLength) * fB +
                (m_lstReserved210[m_lstReserved210.m_nCount - 1] +
                       m_pTarget->GetOrientation().zdir() * m_fTangentLength) * fC +
                m_lstReserved210[m_lstReserved210.m_nCount - 1] * fD;
            vTangent.Normalize();
            m_lstReserved234[nInterpolationPoint] = vTangent * m_fTangentLength;
        }
    }
    for (nInterpolationPoint = 0; nInterpolationPoint < m_lstReserved210.m_nCount; nInterpolationPoint++)
    {
        m_lstReserved21C[nInterpolationPoint] = m_lstReserved228[nInterpolationPoint];
        m_lstReserved240[nInterpolationPoint] = m_lstReserved24C[nInterpolationPoint];
        if (nInterpolationPoint == 0 || nInterpolationPoint == m_lstReserved210.m_nCount - 1)
        {
            m_lstReserved228[nInterpolationPoint] = m_lstReserved210[nInterpolationPoint];
            if (nInterpolationPoint == 0)
                m_lstReserved24C[nInterpolationPoint] = m_qOrientation.zdir() * m_fTangentLength;
            else
                m_lstReserved24C[nInterpolationPoint] = normalize(
                    m_pTarget->GetPosition() - m_vOrigin) * m_fTangentLength;
            if (m_bBranch)
                m_lstReserved21C[nInterpolationPoint] = m_lstReserved228[nInterpolationPoint];
        }
        else
        {
            float fRandom;
            if (nPositionRandom != 0)
            {
                fRandom = (rand() % nPositionRandom) * 0.01f;
                if (rand() & 1)
                    fRandom = -fRandom;
                m_lstReserved228[nInterpolationPoint] = m_lstReserved210[nInterpolationPoint] +
                    YawPitchRoll(0.0f, rand() % 360, 0.0f) * (m_vReserved27C * fRandom);
            }
            else
                m_lstReserved228[nInterpolationPoint] = m_lstReserved210[nInterpolationPoint];
            if (nTangentRandom != 0)
            {
                fRandom = (rand() % nTangentRandom) * 0.01f;
                if (rand() & 1)
                    fRandom = -fRandom;
                float fPitch = (rand() % nTangentRandom) * 0.01f;
                if (rand() & 1)
                    fPitch = -fPitch;
                m_lstReserved24C[nInterpolationPoint] = YawPitchRoll(fRandom, fPitch, 0.0f) * m_lstReserved234[nInterpolationPoint];
            }
            else
                m_lstReserved24C[nInterpolationPoint] = m_lstReserved234[nInterpolationPoint];
            if (m_bBranch)
                m_lstReserved21C[nInterpolationPoint] = m_lstReserved228[nInterpolationPoint];
        }
    }
    int nSegment = 1;
    float fStart = 0.0f;
    float fEnd = 1.0f / (m_lstReserved21C.m_nCount - 1);
    for (nInterpolationPoint = 1; nInterpolationPoint < m_lstActive.m_nCount; nInterpolationPoint++)
    {
        fTime = (float)nInterpolationPoint / (m_lstActive.m_nCount - 1);
        if (fTime > fEnd)
        {
            do
            {
                nSegment++;
                fStart = fEnd;
                fEnd += 1.0f / (m_lstReserved21C.m_nCount - 1);
                if (nSegment < m_lstReserved21C.m_nCount)
                    continue;
                nSegment--;
                fStart -= 1.0f / (m_lstReserved21C.m_nCount - 1);
                fEnd -= 1.0f / (m_lstReserved21C.m_nCount - 1);
                break;
            } while (fTime > fEnd);
        }
        float fPosition = (fTime - fStart) / (fEnd - fStart);
        if (fPosition > 1.0f)
            fPosition = 1.0f;
        float fOther = 1.0f - fPosition;
        float fFinalA = fOther * fOther * fOther;
        float fFinalB = fPosition * fOther * fOther * 3.0f;
        float fFinalC = fOther * fPosition * fPosition * 3.0f;
        float fFinalD = fPosition * fPosition * fPosition;
        m_lstActive[nInterpolationPoint]->m_vPosition =
            fFinalA * m_lstReserved21C[nSegment - 1] +
            fFinalB * (m_lstReserved21C[nSegment - 1] + m_lstReserved240[nSegment - 1]) +
            fFinalC * (m_lstReserved21C[nSegment] - m_lstReserved240[nSegment]) +
            fFinalD * m_lstReserved21C[nSegment];
    }
}

inline Vector PartEmitter::GetOrigin() { return m_vOrigin; }

#define LIGHTNING_BRANCH_DEBUG 0
// PC 0x004C5730 GOG 0x004C5840
void LightningEmitter::Update(float fTimeStep)
{
    int nParticles = (int)m_fBirthRate;
    Vector vDirection;
    Vector vPerpendicular;
    Vector vDistance;
    float fScale = 1.0f;
    bool bUpdateBranches = false;
    bool bRandomStart = false;
    bool bRandomEnd = false;
    int j;
    if (m_pAurObject)
        fScale = m_pAurObject->m_fScale;
    int nActive = m_lstActive.m_nCount;
    m_fReservedF4 += fTimeStep;
    if (nParticles == 0 || !m_pTarget)
    {
        for (; nActive > 0; )
        {
            Particle* pParticle = m_lstActive[--nActive];
            m_lstActive.DelIndex(nActive);
            m_lstFree.Add(pParticle);
        }
    }
    else
    {
        if (nActive < nParticles)
        {
            int nNew = nParticles - nActive;
            nActive += nNew;
            do
            {
                Particle* pParticle;
                if (m_lstFree.m_nCount > 0)
                {
                    pParticle = m_lstFree[m_lstFree.m_nCount - 1];
                    m_lstFree.DelIndex(m_lstFree.m_nCount - 1);
                }
                else
                    pParticle = new Particle(this, fTimeStep);
                chkRandom(pParticle);
                m_lstActive.Add(pParticle);
            } while (--nNew);
        }
        while (nActive > nParticles)
        {
            Particle* pParticle = m_lstActive[nActive - 1];
            m_lstActive.DelIndex(nActive - 1);
            m_lstFree.Add(pParticle);
            nActive--;
        }
        if (m_lstReserved270.m_nCount < m_lstActive.m_nCount)
            m_lstReserved270.SetSize(m_lstActive.m_nCount);
        for (int i = 0; i < m_lstActive.m_nCount; i++)
        {
            Particle* pParticle = m_lstActive[i];
            if (pParticle->m_fAge == pParticle->m_fFrameStartAge)
            {
                pParticle->m_fAge = 0;
                pParticle->m_fFrameStartAge = 0;
                pParticle->m_fFrameAge = 0;
            }
            pParticle->m_fAge += fTimeStep;
            if (m_nReserved114 == MotionBlurred)
            {
                float fBlend = (fTimeStep < m_fBlurLength ? fTimeStep : m_fBlurLength) / m_fBlurLength;
                pParticle->m_vOrigin = fBlend * pParticle->m_vPosition +
                    (1.0f - fBlend) * pParticle->m_vOrigin;
            }
        }
        m_fReservedF8 += fTimeStep;
        MdlNodeEmitter* pNode = m_pNode->AsMdlNodeEmitter();
        if (m_fReservedF8 > m_fControlPointDelay)
        {
            m_fReservedF8 = 0;
            m_lstReserved210[0] = m_vOrigin;
            m_lstReserved210[m_lstReserved210.m_nCount - 1] =
                m_pTarget->GetPosition();
            UpdateControlPoints();
            if (pNode->m_nNumBranches > 0 && !m_bBranch)
            {
                UpdateBranches();
                bUpdateBranches = true;
                for (j = 0; j < m_nBranches; j++)
                    m_lstBranches[j]->m_fReservedF8 = m_fControlPointDelay;
            }
            m_fReservedF4 = (m_fLightningDelay + 0.01f);
        }
        else if (m_pNode->AsMdlNodeEmitter()->m_nControlPointSmoothing != 0 && !m_bBranch)
        {
            int nPoint = 1;
            float fBlend = 0.0f;
            if (m_fControlPointDelay != 0.0f)
                fBlend = (m_fReservedF8 / m_fControlPointDelay);
            float fEnd = 1.0f / (m_lstReserved228.m_nCount - 1);
            float fStart = 0.0f;
            for (j = 1; j < m_lstActive.m_nCount - 1; j++)
            {
                float fPosition = (float)j / (m_lstActive.m_nCount - 1);
                while (fPosition > fEnd)
                {
                    nPoint++;
                    fStart = fEnd;
                    fEnd += 1.0f / (m_lstReserved228.m_nCount - 1);
                }
                float fTime = (fPosition - fStart) / (fEnd - fStart);
                if (fTime > 1.0f)
                    fTime = 1.0f;
                float fInverse = 1.0f - fTime;
                float fStartWeight = fInverse * fInverse * fInverse;
                float fStartControlWeight = fInverse * fInverse * fTime * 3.0f;
                float fEndControlWeight = fInverse * fTime * fTime * 3.0f;
                float fEndWeight = fTime * fTime * fTime;
                m_lstActive[j]->m_vPosition =
                    fStartWeight *
                        ((1.0f - fBlend) * m_lstReserved21C[nPoint - 1] + fBlend * m_lstReserved228[nPoint - 1]) +
                    fStartControlWeight *
                        (((1.0f - fBlend) * m_lstReserved21C[nPoint - 1] + fBlend * m_lstReserved228[nPoint - 1]) +
                         (1.0f - fBlend) * m_lstReserved240[nPoint - 1] + fBlend * m_lstReserved24C[nPoint - 1]) +
                    fEndControlWeight *
                        (((1.0f - fBlend) * m_lstReserved21C[nPoint] + fBlend * m_lstReserved228[nPoint]) -
                         ((1.0f - fBlend) * m_lstReserved240[nPoint] + fBlend * m_lstReserved24C[nPoint])) +
                    fEndWeight *
                        ((1.0f - fBlend) * m_lstReserved21C[nPoint] + fBlend * m_lstReserved228[nPoint]);
            }
        }
        if (m_fReservedF4 > m_fLightningDelay)
        {
            Particle* pFirstParticle = m_lstActive[0];
            if (m_fXSize == 0.0f && m_fYSize == 0.0f)
                pFirstParticle->m_vPosition = m_vOrigin;
            else
            {
                pFirstParticle->randomPosition((int)(fScale * m_fXSize * 100.0f),
                                              (int)(fScale * m_fYSize * 100.0f));
                bRandomStart = true;
            }
            Vector& vEnd = m_lstActive[m_lstActive.m_nCount - 1]->m_vPosition;
            vEnd = m_pTarget->GetPosition();
            if (m_fTargetSize != 0.0f)
            {
                Vector vOffset;
                int nRandom = rand();
                if (nRandom & 1)
                    vOffset.m_fX = nRandom % (int)(m_fTargetSize * 50.0f) / 100.0f;
                else
                    vOffset.m_fX = -(nRandom % (int)(m_fTargetSize * 50.0f)) / 100.0f;
                nRandom = rand();
                if (nRandom & 1)
                    vOffset.m_fY = nRandom % (int)(m_fTargetSize * 50.0f) / 100.0f;
                else
                    vOffset.m_fY = -(nRandom % (int)(m_fTargetSize * 50.0f)) / 100.0f;
                nRandom = rand();
                if (nRandom & 1)
                    vOffset.m_fZ = nRandom % (int)(m_fTargetSize * 50.0f) / 100.0f;
                else
                    vOffset.m_fZ = -(nRandom % (int)(m_fTargetSize * 50.0f)) / 100.0f;
                vEnd += vOffset;
                bRandomEnd = true;
            }
            if (bRandomStart || bRandomEnd)
            {
                if (m_lstReserved210.m_nCount == 2)
                {
                    if (bRandomStart)
                        m_lstReserved210[0] = m_lstActive[0]->m_vPosition;
                    else
                        m_lstReserved210[1] = m_lstActive[m_lstActive.m_nCount - 1]->m_vPosition;
                    UpdateControlPoints();
                }
                else if (bRandomStart)
                {
                    m_lstReserved210[0] = m_lstReserved228[0] = m_lstActive[0]->m_vPosition;
                    int nPoint = 1;
                    float fBlend = 0.0f;
                    if (m_fControlPointDelay != 0.0f)
                        fBlend = m_fReservedF8 / m_fControlPointDelay;
                    float fStart = 0.0f;
                    float fEnd = 1.0f / (m_lstReserved228.m_nCount - 1);
                    for (j = 1; j < m_lstActive.m_nCount - 1 && nPoint <= 1; j++)
                    {
                        float fPosition = (float)j / (m_lstActive.m_nCount - 1);
                        while (fPosition > fEnd)
                        {
                            nPoint++;
                            fStart = fEnd;
                            fEnd += 1.0f / (m_lstReserved228.m_nCount - 1);
                        }
                        float fTime = (fPosition - fStart) / (fEnd - fStart);
                        if (fTime > 1.0f)
                            fTime = 1.0f;
                        float fInverse = 1.0f - fTime;
                        float fStartWeight = fInverse * fInverse * fInverse;
                        float fStartControlWeight = fInverse * fInverse * fTime * 3.0f;
                        float fEndControlWeight = fInverse * fTime * fTime * 3.0f;
                        float fEndWeight = fTime * fTime * fTime;
                        m_lstActive[j]->m_vPosition =
                            fStartWeight *
                                ((1.0f - fBlend) * m_lstReserved21C[nPoint - 1] + fBlend * m_lstReserved228[nPoint - 1]) +
                            fStartControlWeight *
                                (((1.0f - fBlend) * m_lstReserved21C[nPoint - 1] + fBlend * m_lstReserved228[nPoint - 1]) +
                                 (1.0f - fBlend) * m_lstReserved240[nPoint - 1] + fBlend * m_lstReserved24C[nPoint - 1]) +
                            fEndControlWeight *
                                (((1.0f - fBlend) * m_lstReserved21C[nPoint] + fBlend * m_lstReserved228[nPoint]) -
                                 ((1.0f - fBlend) * m_lstReserved240[nPoint] + fBlend * m_lstReserved24C[nPoint])) +
                            fEndWeight *
                                ((1.0f - fBlend) * m_lstReserved21C[nPoint] + fBlend * m_lstReserved228[nPoint]);
                    }
                }
                else if (bRandomEnd)
                {
                    if (LIGHTNING_BRANCH_DEBUG)
                    {
                        vDistance.m_fX = 0.0f;
                    }
                    m_lstReserved210[m_lstReserved210.m_nCount - 1] =
                        m_lstReserved228[m_lstReserved210.m_nCount - 1] =
                        m_lstActive[m_lstActive.m_nCount - 1]->m_vPosition;
                    int nPoint = m_lstReserved228.m_nCount - 1;
                    float fBlend = 0.0f;
                    if (m_fControlPointDelay != 0.0f)
                        fBlend = m_fReservedF8 / m_fControlPointDelay;
                    float fStart = (float)(m_lstReserved228.m_nCount - 2) / nPoint;
                    float fEnd = 1.0f;
                    for (j = (int)(m_lstActive.m_nCount * fStart) + 1; j < m_lstActive.m_nCount - 1; j++)
                    {
                        float fPosition = (float)j / (m_lstActive.m_nCount - 1);
                        float fTime = (fPosition - fStart) / (fEnd - fStart);
                        if (fTime > 1.0f)
                            fTime = 1.0f;
                        float fInverse = 1.0f - fTime;
                        float fStartWeight = fInverse * fInverse * fInverse;
                        float fStartControlWeight = fInverse * fInverse * fTime * 3.0f;
                        float fEndControlWeight = fInverse * fTime * fTime * 3.0f;
                        float fEndWeight = fTime * fTime * fTime;
                        m_lstActive[j]->m_vPosition =
                            fStartWeight *
                                ((1.0f - fBlend) * m_lstReserved21C[nPoint - 1] + fBlend * m_lstReserved228[nPoint - 1]) +
                            fStartControlWeight *
                                (((1.0f - fBlend) * m_lstReserved21C[nPoint - 1] + fBlend * m_lstReserved228[nPoint - 1]) +
                                 (1.0f - fBlend) * m_lstReserved240[nPoint - 1] + fBlend * m_lstReserved24C[nPoint - 1]) +
                            fEndControlWeight *
                                (((1.0f - fBlend) * m_lstReserved21C[nPoint] + fBlend * m_lstReserved228[nPoint]) -
                                 ((1.0f - fBlend) * m_lstReserved240[nPoint] + fBlend * m_lstReserved24C[nPoint])) +
                            fEndWeight *
                                ((1.0f - fBlend) * m_lstReserved21C[nPoint] + fBlend * m_lstReserved228[nPoint]);
                    }
                }
            }
            if (LIGHTNING_BRANCH_DEBUG)
            {
                vDistance.m_fY = 0.0f;
            }
            vDistance = m_lstActive[0]->m_vPosition - vEnd;
            if (!equal(vDistance, Vector()))
            {
                m_fReservedF4 = 0;
                if (useoldlightninginterp)
                {
                    int nMidpoint = (int)(magnitude(vDistance) * m_fLightningSubDivisions * 0.5f);
                    vDirection = normalize(vDistance);
                    vPerpendicular = Vector(-vDirection.m_fY, vDirection.m_fZ, -vDirection.m_fX);
                    RecursiveFractal(nMidpoint, nMidpoint, 1, vPerpendicular,
                                     cross(vDirection, vPerpendicular), fScale);
                    float fRange = sqrt(vDistance.m_fZ * vDistance.m_fZ +
                                        vDistance.m_fY * vDistance.m_fY +
                                        vDistance.m_fX * vDistance.m_fX) / m_lstActive.m_nCount * 0.5;
                    for (j = 1; j < m_lstActive.m_nCount - 1; j++)
                    {
                        if (!(fRange >= 0.01))
                            break;
                        Vector vOffset;
                        if (rand() & 1)
                            vOffset = vDirection;
                        else
                            vOffset = -vDirection;
                        float fRandom = (rand() % (int)(fRange * 100.0f)) * 0.01;
                        m_lstActive[j]->m_vPosition = m_lstActive[j]->m_vPosition +
                            vOffset * fRandom * m_fLightningScale * m_fScale;
                    }
                }
                else
                {
                    float fRange = (sqrt(vDistance.m_fZ * vDistance.m_fZ +
                                         vDistance.m_fY * vDistance.m_fY +
                                         vDistance.m_fX * vDistance.m_fX) / m_lstActive.m_nCount * 0.5) *
                        m_fLightningScale * m_fScale;
                    Vector vLongitudinal = fRange * normalize(vDistance);
                    Vector vJitterDirection = m_vReserved27C;
                    int nAngleRange = (int)m_fLightningZigZag;
                    Vector vAxis = m_pTarget->GetPosition() - m_vOrigin;
                    int nAngle = 0;
                    for (int i = 1; i < m_lstActive.m_nCount - 1; i++)
                    {
                        if (nAngleRange != 0)
                        {
                            int nRandom = rand() % nAngleRange;
                            if (rand() & 1)
                                nRandom = 360 - nRandom;
                            nAngle += nRandom;
                            vJitterDirection = Quaternion(vAxis, nAngle * DEG_TO_RAD) * vJitterDirection;
                        }
                        float fPosition = (float)i / m_lstActive.m_nCount;
                        if (fPosition > 0.5f)
                            fPosition = 1.0f - fPosition;
                        float fRandom = (rand() % 100) * ((fPosition * 2.0f) * m_fLightningRadius) * 0.01f;
                        Vector vOffset;
                        vOffset = (rand() % 100) * 0.01f * vLongitudinal;
                        if (rand() & 1)
                            vOffset = -vOffset;
                        m_lstReserved270[i] = fRandom * vJitterDirection + vOffset;
                    }
                    if (pNode->m_nNumBranches > 0 && m_lstBranches.m_nCount != 0)
                    {
                        for (int i = 0; i < m_nBranches; i++)
                        {
                            if (LIGHTNING_BRANCH_DEBUG)
                            {
                                if (!m_vOrigin.IsZero())
                                {
                                    Vector vDebug = m_vOrigin + m_vOrigin;
                                }
                            }
                            m_lstBranches[i]->m_vOrigin =
                                m_lstBranches[i]->m_lstReserved21C[0] =
                                m_lstBranches[i]->m_lstReserved228[0] =
                                m_lstActive[m_lstBranchIndices[i]]->m_vPosition + m_lstReserved270[m_lstBranchIndices[i]];
                            m_lstBranches[i]->m_vReserved1F8 =
                                m_lstBranches[i]->m_pTarget->GetPosition() - m_lstBranches[i]->m_vOrigin;
                            m_lstBranches[i]->m_vReserved204 = m_lstBranches[i]->m_vOrigin;
                        }
                    }
                }
            }
        }
        else
        {
            Vector vNewDirection = m_pTarget->GetPosition() - m_vOrigin;
            Quaternion qRotation = RotationArc(m_vReserved1F8, vNewDirection);
            for (j = 0; j < m_lstActive.m_nCount; j++)
                m_lstActive[j]->m_vPosition =
                    qRotation * (m_lstActive[j]->m_vPosition - m_vReserved204) + m_vOrigin;
            for (j = 0; j < m_lstReserved210.m_nCount; j++)
            {
                m_lstReserved210[j] = qRotation * (m_lstReserved210[j] - m_vReserved204) + m_vOrigin;
                m_lstReserved234[j] = qRotation * m_lstReserved234[j];
            }
            if (pNode->m_nNumBranches > 0 && m_lstBranches.m_nCount != 0)
            {
                for (j = 0; j < m_nBranches; j++)
                {
                    m_lstBranches[j]->m_vOrigin =
                        m_lstBranches[j]->m_lstReserved21C[0] =
                        m_lstBranches[j]->m_lstReserved228[0] =
                        m_lstActive[m_lstBranchIndices[j]]->m_vPosition + m_lstReserved270[m_lstBranchIndices[j]];
                    Vector vPosition;
                    vPosition = m_lstBranches[j]->m_pTarget->GetPosition();
                    vPosition = qRotation * (vPosition - m_vReserved204) + m_vOrigin;
                    m_lstBranches[j]->m_pTarget->SetPosition(vPosition);
                }
            }
            m_vReserved1F8 = vNewDirection;
            m_vReserved204 = m_vOrigin;
        }
        if (pNode->m_nNumBranches > 0 && !m_bBranch)
        {
            for (j = 0; j < m_nBranches; j++)
            {
                if (bUpdateBranches)
                    m_lstBranches[j]->Update(m_fLightningDelay + 0.01f);
                else
                    m_lstBranches[j]->Update(fTimeStep);
            }
        }
    }
}
// PC 0x004C8360 GOG 0x004C8470
void SetEmitterTarget(CAurObject* pObject, char* sEvent, void* pData)
{
    PartEmitter* pEmitter = static_cast<PartEmitter*>(pData);
    for (int i = 0; i < pEmitter->m_lstChildren.m_nCount; i++)
    {
        Part* pChild = pEmitter->m_lstChildren.m_pList[i];
        if (pChild->AsPartReference())
        {
            pEmitter->m_pTarget = pChild->AsPartReference()->m_pReferenceObject;
            Vector vPosition;
            Quaternion qOrientation;
            pEmitter->GetWorld(vPosition, qOrientation);
            if (pEmitter->m_pTarget)
            {
                pEmitter->m_fRadius = magnitude(
                    static_cast<Gob*>(pEmitter->m_pTarget.m_pObject)->GetPosition() - vPosition);
            }
            else
            {
                pEmitter->m_fRadius = 0.0f;
            }
            return;
        }
    }
    pEmitter->m_pAurObject->DeregisterCallback(pEmitter->m_pEmitterTargetCallback);
}

#ifdef _XBOX
FountainEmitter::FountainEmitter(MdlNode* pNode) : PartEmitter(pNode) {}

ExplosionEmitter::ExplosionEmitter(MdlNode* pNode) : PartEmitter(pNode) {}

SingleEmitter::SingleEmitter(MdlNode* pNode) : PartEmitter(pNode) {}

#endif
LightningEmitter::LightningEmitter(MdlNode* pNode)  // PC 0x004CA3C0 GOG 0x004CA4D0
    : PartEmitter(pNode),
      m_lstReserved210(0),
      m_lstReserved21C(0),
      m_lstReserved228(0),
      m_lstReserved234(0),
      m_lstReserved240(0),
      m_lstReserved24C(0),
      m_lstBranches(0),
      m_lstBranchIndices(0),
      m_lstReserved270(0)
{
    m_fLightningScale = 0;
    m_nBranches = 0;
    m_bBranch = false;
}
// PC 0x004CA4F0 GOG 0x004CA600
LightningEmitter::~LightningEmitter()
{
    MdlNodeEmitter* pNode = m_pNode->AsMdlNodeEmitter();
    if (m_lstBranches.m_nCount != 0)
    {
        for (int i = 0; i < pNode->m_nNumBranches; i++)
        {
            delete m_lstBranches[i]->m_pTarget;
            delete m_lstBranches[i];
        }
    }
}
// PC 0x004CA670 GOG 0x004CA780
void LightningEmitter::Initialize()
{
    PartEmitter::Initialize();
    m_fLifeTime = 1.0f;
    CallBack* pCallBack = m_pAurObject->RegisterCallBack(
        SetEmitterTarget, "_EmitterTarget", this, 9999.0f, 0);
    m_pReserved1F4 = LIGHTNING;
    m_pEmitterTargetCallback = pCallBack;

    MdlNodeEmitter* pNode = m_pNode->AsMdlNodeEmitter();
    m_lstBranches.SetSize(pNode->m_nNumBranches);
    m_lstBranchIndices.SetSize(pNode->m_nNumBranches);
    for (int i = 0; i < pNode->m_nNumBranches; i++)
    {
        m_lstBranches[i] = new LightningEmitter(m_pNode);
        m_lstBranches[i]->InitializeBranch();
    }
}
// PC 0x004CA7B0 GOG 0x004CA8C0
Part* MdlNodeEmitter::InternalCreateInstance(Gob* pGob)
{
    PartEmitter* pEmitter;
    if (strcmp(m_sUpdate, "Fountain") == 0)
    {
        pEmitter = new FountainEmitter(this);
    }
    else if (strcmp(m_sUpdate, "Explosion") == 0)
    {
        pEmitter = new ExplosionEmitter(this);
        if (pGob)
            pGob->RegisterCallBack(Detonate, "Detonate", pEmitter,
                                   AUR_CALLBACK_LONG_DELAY, false);
    }
    else if (strcmp(m_sUpdate, "Single") == 0)
    {
        pEmitter = new SingleEmitter(this);
    }
    else
    {
        pEmitter = new LightningEmitter(this);
    }
    pEmitter->m_pAurObject = pGob;
    pEmitter->Initialize();
    return pEmitter;
}
// PC 0x004C84B0 GOG 0x004C85C0
PartEmitter::PartEmitter(MdlNode* pNode)
    : Part(pNode)
{
    m_fAlphaEnd = 0.0f;
    m_fAlphaMid = 0.0f;
    m_fAlphaStart = 0;
    m_fPercentStart = EMITTER_UNSET_COLOR_KEY_TIME;
    m_fPercentMid = EMITTER_UNSET_COLOR_KEY_TIME;
    m_fPercentEnd = EMITTER_UNSET_COLOR_KEY_TIME;
    m_fBirthRate = 0.0f;
    m_fBirthRateRandom = 0;
    m_fBounceCoefficient = 0.0f;
    m_fCombineTime = 0.0f;
    m_fDrag = 0.0f;
    m_fFramesPerSecond = 0;
    m_fFrameEnd = 0.0f;
    m_fFrameStart = 0.0f;
    m_fGravity = 0;
    m_fLifeTime = 0.0f;
    m_fMass = 0.0f;
    m_fPointToPointBezier2 = 0.0f;
    m_fPointToPointBezier3 = 0;
    m_fParticleRotation = 0.0f;
    m_fVelocityRandom = 0.0f;
    m_fXSizeStart = 0;
    m_fXSizeMid = 0.0f;
    m_fXSizeEnd = 0.0f;
    m_fYSizeStart = 0.0f;
    m_fYSizeMid = 0.0f;
    m_fYSizeEnd = 0.0f;
    m_fSpread = 0.0f;
    m_fThreshold = 0.0f;
    m_fVelocity = 0.0f;
    m_fXSize = 0.0f;
    m_fYSize = 0.0f;
    m_fBlurLength = 1.0f;
    m_fLightningDelay = 0.0f;
    m_fLightningRadius = 0.0f;
    m_fLightningScale = 0;
    m_fLightningSubDivisions = 0.0f;
    m_fLightningZigZag = 0;
    m_fControlPointRadius = 0;
    m_fControlPointDelay = 0.0f;
    m_fTangentSpread = 0;
    m_fTangentLength = 0.0f;
    m_fTargetSize = 0.0f;
    m_fReservedF4 = EMITTER_INITIAL_ELAPSED_TIME;
    m_fReservedF8 = EMITTER_INITIAL_ELAPSED_TIME;
    m_bDetonated = false;
    m_pEmitterMaterial = 0;
    m_fRadius = 1.0f;
    m_fGravityAcceleration = 0;
    m_pTileData = 0;
    m_pTarget = 0;
    m_pWindManager = 0;
    m_pHitInfo = new CHitInfo(1, ~CHitInfo::HIT_TRIGGER, 0);
    totalemitters++;
    m_bWeather = false;
}

#define AUREMITTER_DEBUG_CONTROL_POINTS 0
extern unsigned int ATI_FRAGMENT_SHADER_BIT;
// PC 0x004C87B0 GOG 0x004C88C0
void PartEmitter::RenderFunc()
{
    AuremitterTrace4();
    if (m_pAurObject != 0)
    {
        if (m_pAurObject->m_pModel != 0 &&
            (m_pAurObject->m_bDistortion ||
             bool(m_pAurObject->m_pModel->m_Classification.m_bSceneHitDisabled)))
        {
            GLRender_Enable(ENABLE_STENCIL_TEST);
            GLRender_SetColorMask(0, 0, 0, 0);
            GLRender_SetStencilOp(STENCIL_KEEP,
                        STENCIL_KEEP,
                        STENCIL_REPLACE);
            GLRender_SetStencilMask(GLRender::DISTORTION_STENCIL_MASK);
            GLRender_SetStencilFunc(CMP_ALWAYS,
                          GLRender::DISTORTION_STENCIL_MASK,
                          GLRender::STENCIL_WRITE_MASK);
        }
        else
        {
            GLRender_SetColorMask(1, 1, 1, 1);
            GLRender_SetStencilMask(0);
            GLRender_Disable(ENABLE_STENCIL_TEST);
        }
    }
    else
    {
        GLRender_SetColorMask(1, 1, 1, 1);
        GLRender_SetStencilMask(0);
        GLRender_Disable(ENABLE_STENCIL_TEST);
    }

    MdlNodeEmitter* pNode = m_pNode->AsMdlNodeEmitter();
    if (pNode->IsPointToPoint() && m_pTarget == 0)
    {
        if (m_pAurObject != 0 && m_pAurObject->IsDistortionEnabled())
        {
            GLRender_SetColorMask(1, 1, 1, 1);
#ifdef _XBOX
            g_pD3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0);
#else
            GLRender_SetStencilMask(0);
#endif
            GLRender_Disable(ENABLE_STENCIL_TEST);
        }
        return;
    }
    if (pNode->m_sChunkName[0] != 0)
    {
        RenderNothing();
        if (m_pAurObject != 0 && m_pAurObject->m_pModel != 0 &&
            (m_pAurObject->m_bDistortion ||
             bool(m_pAurObject->m_pModel->m_Classification.m_bSceneHitDisabled)))
        {
            GLRender_SetColorMask(1, 1, 1, 1);
            GLRender_SetStencilMask(0);
            GLRender_Disable(ENABLE_STENCIL_TEST);
        }
        return;
    }

    if (g_ForceSight)
    {
        GLRender_EnablePixelShaders(MODEL_SHADER_DESAT, 0, 0, 0, 0);
        float afColor[4];
        afColor[0] = 0.0f;
        afColor[1] = 0.0f;
        afColor[2] = 0.0f;
        afColor[3] = 1.0f;
        if ((existingextensions & ATI_FRAGMENT_SHADER_BIT) == ATI_FRAGMENT_SHADER_BIT)
            glSetFragmentShaderConstantATI(GL_CON_1_ATI, afColor);
        else
            glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, afColor);
    }

    if (m_nReserved114 == Linked)
    {
        LightningEmitter* pLightning = AsLightningEmitter();
        int i;
        if (pLightning != 0)
        {
#ifndef _XBOX
            if (auremitterrendertangents)
            {
                glDisable(GL_LIGHTING);
                glDisable(GL_TEXTURE_2D);
                glColor3f(0.0f, 1.0f, 0.0f);
                glLineWidth(2.5f);
                float fBlend = 0.0f;
                if (pLightning->m_fControlPointDelay != 0.0f)
                    fBlend = (pLightning->m_fReservedF8 / pLightning->m_fControlPointDelay);
                m_fFramesPerSecond = m_fFramesPerSecond;
                m_fBirthRate = m_fBirthRate;
                if (AUREMITTER_DEBUG_CONTROL_POINTS)
                {
                    glBegin(GL_POINTS);
                    for (i = 0; i < pLightning->m_lstReserved21C.m_nCount; i++)
                    {
                        glVertex3f((1.0f - fBlend) * pLightning->m_lstReserved21C[i].m_fX + fBlend * pLightning->m_lstReserved228[i].m_fX,
                                   (1.0f - fBlend) * pLightning->m_lstReserved21C[i].m_fY + fBlend * pLightning->m_lstReserved228[i].m_fY,
                                   (1.0f - fBlend) * pLightning->m_lstReserved21C[i].m_fZ + fBlend * pLightning->m_lstReserved228[i].m_fZ);
                    }
                    glEnd();
                }
                glBegin(GL_LINES);
                glVertex3f((1.0f - fBlend) * pLightning->m_lstReserved21C[0].m_fX +
                                   fBlend * pLightning->m_lstReserved228[0].m_fX,
                           (1.0f - fBlend) * pLightning->m_lstReserved21C[0].m_fY +
                                   fBlend * pLightning->m_lstReserved228[0].m_fY,
                           (1.0f - fBlend) * pLightning->m_lstReserved21C[0].m_fZ +
                                   fBlend * pLightning->m_lstReserved228[0].m_fZ);
                glVertex3f(((1.0f - fBlend)) * pLightning->m_lstReserved21C[0].m_fX +
                            fBlend * pLightning->m_lstReserved228[0].m_fX +
                            (1.0f - fBlend) * pLightning->m_lstReserved240[0].m_fX +
                            fBlend * pLightning->m_lstReserved24C[0].m_fX,
                           (1.0f - fBlend) * pLightning->m_lstReserved21C[0].m_fY +
                            fBlend * pLightning->m_lstReserved228[0].m_fY +
                            (1.0f - fBlend) * pLightning->m_lstReserved240[0].m_fY +
                            fBlend * pLightning->m_lstReserved24C[0].m_fY,
                           (1.0f - fBlend) * pLightning->m_lstReserved21C[0].m_fZ +
                            fBlend * pLightning->m_lstReserved228[0].m_fZ +
                            (1.0f - fBlend) * pLightning->m_lstReserved240[0].m_fZ +
                            fBlend * pLightning->m_lstReserved24C[0].m_fZ);
                for (i = 1; i < pLightning->m_lstReserved21C.m_nCount - 1; i++)
                {
                    glVertex3f((1.0f - fBlend) * pLightning->m_lstReserved21C[i].m_fX +
                                fBlend * pLightning->m_lstReserved228[i].m_fX -
                                ((1.0f - fBlend) * pLightning->m_lstReserved240[i].m_fX +
                                 fBlend * pLightning->m_lstReserved24C[i].m_fX),
                               (1.0f - fBlend) * pLightning->m_lstReserved21C[i].m_fY +
                                fBlend * pLightning->m_lstReserved228[i].m_fY -
                                ((1.0f - fBlend) * pLightning->m_lstReserved240[i].m_fY +
                                 fBlend * pLightning->m_lstReserved24C[i].m_fY),
                               (1.0f - fBlend) * pLightning->m_lstReserved21C[i].m_fZ +
                                fBlend * pLightning->m_lstReserved228[i].m_fZ -
                                ((1.0f - fBlend) * pLightning->m_lstReserved240[i].m_fZ +
                                 fBlend * pLightning->m_lstReserved24C[i].m_fZ));
                    glVertex3f((1.0f - fBlend) * pLightning->m_lstReserved21C[i].m_fX +
                                fBlend * pLightning->m_lstReserved228[i].m_fX +
                                (1.0f - fBlend) * pLightning->m_lstReserved240[i].m_fX +
                                fBlend * pLightning->m_lstReserved24C[i].m_fX,
                               (1.0f - fBlend) * pLightning->m_lstReserved21C[i].m_fY +
                                fBlend * pLightning->m_lstReserved228[i].m_fY +
                                (1.0f - fBlend) * pLightning->m_lstReserved240[i].m_fY +
                                fBlend * pLightning->m_lstReserved24C[i].m_fY,
                               (1.0f - fBlend) * pLightning->m_lstReserved21C[i].m_fZ +
                                fBlend * pLightning->m_lstReserved228[i].m_fZ +
                                (1.0f - fBlend) * pLightning->m_lstReserved240[i].m_fZ +
                                fBlend * pLightning->m_lstReserved24C[i].m_fZ);
                }
                int nLast = pLightning->m_lstReserved21C.m_nCount - 1;
                glVertex3f((1.0f - fBlend) * pLightning->m_lstReserved21C[nLast].m_fX +
                            fBlend * pLightning->m_lstReserved228[nLast].m_fX -
                            ((1.0f - fBlend) * pLightning->m_lstReserved240[nLast].m_fX +
                             fBlend * pLightning->m_lstReserved24C[nLast].m_fX),
                           (1.0f - fBlend) * pLightning->m_lstReserved21C[nLast].m_fY +
                            fBlend * pLightning->m_lstReserved228[nLast].m_fY -
                            ((1.0f - fBlend) * pLightning->m_lstReserved240[nLast].m_fY +
                             fBlend * pLightning->m_lstReserved24C[nLast].m_fY),
                           (1.0f - fBlend) * pLightning->m_lstReserved21C[nLast].m_fZ +
                            fBlend * pLightning->m_lstReserved228[nLast].m_fZ -
                            ((1.0f - fBlend) * pLightning->m_lstReserved240[nLast].m_fZ +
                             fBlend * pLightning->m_lstReserved24C[nLast].m_fZ));
                glVertex3f((1.0f - fBlend) * pLightning->m_lstReserved21C[nLast].m_fX +
                            fBlend * pLightning->m_lstReserved228[nLast].m_fX,
                           (1.0f - fBlend) * pLightning->m_lstReserved21C[nLast].m_fY +
                            fBlend * pLightning->m_lstReserved228[nLast].m_fY,
                           (1.0f - fBlend) * pLightning->m_lstReserved21C[nLast].m_fZ +
                            fBlend * pLightning->m_lstReserved228[nLast].m_fZ);
                glEnd();
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_LIGHTING);
            }

            if (auremitterrenderbezier)
            {
                List<Vector>& lstPoints = pLightning->m_lstReserved21C;
                float fBlend = 0.0f;
                if (pLightning->m_fControlPointDelay != 0.0f)
                    fBlend = pLightning->m_fReservedF8 / pLightning->m_fControlPointDelay;
                List<Vector>& lstTargetPoints = pLightning->m_lstReserved228;
                glDisable(GL_LIGHTING);
                glDisable(GL_TEXTURE_2D);
                glColor3f(1.0f, 0.0f, 0.0f);
                glPointSize(10.0f);
                glBegin(GL_POINTS);
                for (i = 0; i < lstPoints.m_nCount; i++)
                {
                    glVertex3f((1.0f - fBlend) * lstPoints[i].m_fX +
                                       fBlend * lstTargetPoints[i].m_fX,
                               (1.0f - fBlend) * lstPoints[i].m_fY +
                                       fBlend * lstTargetPoints[i].m_fY,
                               (1.0f - fBlend) * lstPoints[i].m_fZ +
                                       fBlend * lstTargetPoints[i].m_fZ);
                }
                glEnd();
                glColor3f(0.0f, 1.0f, 1.0f);
                glLineWidth(1.0f);
                glBegin(GL_LINE_STRIP);
                glVertex3f((1.0f - fBlend) * lstPoints[0].m_fX +
                                   fBlend * lstTargetPoints[0].m_fX,
                           (1.0f - fBlend) * lstPoints[0].m_fY +
                                   fBlend * lstTargetPoints[0].m_fY,
                           (1.0f - fBlend) * lstPoints[0].m_fZ +
                                   fBlend * lstTargetPoints[0].m_fZ);
                Vector vTarget = static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition();
                for (int j = 1; j < lstPoints.m_nCount; j++)
                {
                    float fTime = 0.0f;
                    do
                    {
                        if (fTime > 1.0f)
                            fTime = 1.0f;
                        float fInvTime = 1.0f - fTime;
                        float fWeight0 = fInvTime * fInvTime * fInvTime;
                        float fWeight1 = fInvTime * fInvTime * fTime * 3.0f;
                        float fWeight2 = fInvTime * fTime * fTime * 3.0f;
                        float fWeight3 = fTime * fTime * fTime;
                        Vector vPosition =
                            fWeight0 *
                                ((1.0f - fBlend) * lstPoints[j - 1] +
                                 fBlend * lstTargetPoints[j - 1]) +
                            fWeight1 *
                                ((1.0f - fBlend) * lstPoints[j - 1] +
                                 fBlend * lstTargetPoints[j - 1] +
                                 (1.0f - fBlend) * pLightning->m_lstReserved240[j - 1] +
                                 fBlend * pLightning->m_lstReserved24C[j - 1]) +
                            fWeight2 *
                                ((1.0f - fBlend) * lstPoints[j] +
                                 fBlend * lstTargetPoints[j] -
                                 ((1.0f - fBlend) * pLightning->m_lstReserved240[j] +
                                  fBlend * pLightning->m_lstReserved24C[j])) +
                            fWeight3 *
                                ((1.0f - fBlend) * lstPoints[j] +
                                 fBlend * lstTargetPoints[j]);
                        glVertex3f(vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ);
                        fTime += 0.05f;
                    } while (fTime <= 1.0f);
                }
                glVertex3f((1.0f - fBlend) * lstPoints[lstPoints.m_nCount - 1].m_fX +
                            fBlend * lstTargetPoints[lstTargetPoints.m_nCount - 1].m_fX,
                           (1.0f - fBlend) * lstPoints[lstPoints.m_nCount - 1].m_fY +
                            fBlend * lstTargetPoints[lstTargetPoints.m_nCount - 1].m_fY,
                           (1.0f - fBlend) * lstPoints[lstPoints.m_nCount - 1].m_fZ +
                            fBlend * lstTargetPoints[lstTargetPoints.m_nCount - 1].m_fZ);
                glEnd();
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_LIGHTING);
            }

            if (auremitterrenderbranchbezier && pLightning->m_nBranches > 0)
            {
                float fBlend = 0.0f;
                if (pLightning->m_fControlPointDelay != 0.0f)
                    fBlend = pLightning->m_fReservedF8 / pLightning->m_fControlPointDelay;
                glDisable(GL_LIGHTING);
                glDisable(GL_TEXTURE_2D);
                for (int i = 0; i < pLightning->m_nBranches; i++)
                {
                    int j;
                    glColor3f(1.0f, 0.0f, 0.0f);
                    glPointSize(10.0f);
                    glBegin(GL_POINTS);
                    for (j = 0; j < pLightning->m_lstBranches[i]->m_lstReserved21C.m_nCount; j++)
                    {
                        glVertex3f((1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[j].m_fX +
                                           fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[j].m_fX,
                                   (1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[j].m_fY +
                                           fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[j].m_fY,
                                   (1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[j].m_fZ +
                                           fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[j].m_fZ);
                    }
                    glEnd();
                    glColor3f(0.0f, 1.0f, 1.0f);
                    glLineWidth(1.0f);
                    glBegin(GL_LINE_STRIP);
                    glVertex3f((1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[0].m_fX +
                                       fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[0].m_fX,
                               (1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[0].m_fY +
                                       fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[0].m_fY,
                               (1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[0].m_fZ +
                                       fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[0].m_fZ);
                    Vector vTarget = static_cast<Gob*>(pLightning->m_lstBranches[i]->m_pTarget.m_pObject)->GetPosition();
                    for (j = 1; j < pLightning->m_lstBranches[i]->m_lstReserved21C.m_nCount; j++)
                    {
                        float fTime = 0.0f;
                        do
                        {
                            if (fTime > 1.0f)
                                fTime = 1.0f;
                            float fInvTime = 1.0f - fTime;
                            float fWeight0 = fInvTime * fInvTime * fInvTime;
                            float fWeight1 = fInvTime * fInvTime * fTime * 3.0f;
                            float fWeight2 = fInvTime * fTime * fTime * 3.0f;
                            float fWeight3 = fTime * fTime * fTime;
                            Vector vPosition =
                                fWeight0 *
                                    ((1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[j - 1] +
                                     fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[j - 1]) +
                                fWeight1 *
                                    ((1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[j - 1] +
                                     fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[j - 1] +
                                     (1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved240[j - 1] +
                                     fBlend * pLightning->m_lstBranches[i]->m_lstReserved24C[j - 1]) +
                                fWeight2 *
                                    ((1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[j] +
                                     fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[j] -
                                     ((1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved240[j] +
                                      fBlend * pLightning->m_lstBranches[i]->m_lstReserved24C[j])) +
                                fWeight3 *
                                    ((1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[j] +
                                     fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[j]);
                            glVertex3f(vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ);
                            fTime += 0.05f;
                        } while (fTime <= 1.0f);
                    }
                    int nLast = pLightning->m_lstBranches[i]->m_lstReserved21C.m_nCount - 1;
                    glVertex3f((1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[nLast].m_fX +
                                fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[nLast].m_fX,
                               (1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[nLast].m_fY +
                                fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[nLast].m_fY,
                               (1.0f - fBlend) * pLightning->m_lstBranches[i]->m_lstReserved21C[nLast].m_fZ +
                                fBlend * pLightning->m_lstBranches[i]->m_lstReserved228[nLast].m_fZ);
                    glEnd();
                }
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_LIGHTING);
            }

            if (AUREMITTER_DEBUG_CONTROL_POINTS)
            {
                glBegin(GL_POINTS);
                for (i = 0; i < pLightning->m_lstReserved21C.m_nCount; i++)
                {
                    Vector vPoint = pLightning->m_lstReserved21C[i];
                    Vector vTangent = pLightning->m_lstReserved240[i];
                    Vector vHandle0 = vPoint + vTangent;
                    glVertex3f(vHandle0.m_fX, vHandle0.m_fY, vHandle0.m_fZ);
                    Vector vHandle1 = vPoint - vTangent;
                    glVertex3f(vHandle1.m_fX, vHandle1.m_fY, vHandle1.m_fZ);
                }
                glEnd();
            }
#endif

            if (auremitterrenderlightning)
            {
                if (pLightning->m_nBranches > 0)
                {
                    for (i = 0; i < pLightning->m_nBranches; i++)
                        pLightning->m_lstBranches[i]->RenderLinked();
                }
            }
        }
        if (pLightning == 0 || auremitterrenderlightning)
            RenderLinked();
    }
    else if (m_nReserved114 == MotionBlurred)
    {
        RenderMotionBlurred();
    }
    else
    {
        RenderStandard();
    }

    if (g_ForceSight)
        GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);

    if (m_pAurObject != 0 && m_pAurObject->m_pModel != 0 &&
        (m_pAurObject->m_bDistortion ||
         bool(m_pAurObject->m_pModel->m_Classification.m_bSceneHitDisabled)))
    {
        GLRender_SetColorMask(1, 1, 1, 1);
        GLRender_SetStencilMask(0);
        GLRender_Disable(ENABLE_STENCIL_TEST);
    }
}
// PC 0x004C9C90 GOG 0x004C9DA0
void PartEmitter::Initialize()
{
    MdlNodeEmitter* pNode = (MdlNodeEmitter*)m_pNode;
    char* sRender = pNode->m_sRender;
    if (strcmp(sRender, "Normal") == 0)
        m_nReserved114 = NORMAL;
    else if (strcmp(sRender, "Billboard_to_World_Z") == 0)
        m_nReserved114 = WorldZ;
    else if (strcmp(sRender, "Billboard_to_Local_Z") == 0)
        m_nReserved114 = LocalZ;
    else if (strcmp(sRender, "Linked") == 0)
        m_nReserved114 = Linked;
    else if (strcmp(sRender, "Aligned_to_World_Z") == 0)
        m_nReserved114 = Aligned;
    else if (strcmp(sRender, "Aligned_to_Particle_Dir") == 0)
        m_nReserved114 = AlignedToParticleDir;
    else if (strcmp(sRender, "Motion_Blur") == 0)
        m_nReserved114 = MotionBlurred;

    m_pReserved1F7 = 0;
    m_pReserved1F4 = OTHER;
    if (pNode->IsPointToPoint())
    {
        m_pEmitterTargetCallback = m_pAurObject->RegisterCallBack(
            SetEmitterTarget, "_EmitterTarget", this, AUR_CALLBACK_LONG_DELAY, false);
        if (!pNode->IsPointToPointSel())
            m_fGravityAcceleration = m_fGravity;
    }

    if (stricmp(pNode->m_sBlend, "Normal") == 0)
        m_nBlendMode = BLEND_NORMAL;
    else if (stricmp(pNode->m_sBlend, "PunchThrough") == 0 ||
             stricmp(pNode->m_sBlend, "Punch-Through") == 0)
        m_nBlendMode = BLEND_PUNCH_THROUGH;
    else
        m_nBlendMode = BLEND_OTHER;

    m_pEmitterMaterial = new Material;
    m_pEmitterMaterial->LoadTextures(this);
#ifdef _XBOX
    if (pNode->IsDepthTexture())
    {
        m_pEmitterMaterial->m_pTexture1 = AurTextureGetReference(pNode->m_sDepthTextureName, 0);
        m_pEmitterMaterial->BindTexture1();
    }
#endif
    if (AsLightningEmitter())
        m_fReservedF4 = EMITTER_INITIAL_ELAPSED_TIME;
    else
        m_fReservedF4 = 0.0f;
    m_fReservedF8 = EMITTER_INITIAL_ELAPSED_TIME;
    m_fBirthElapsed = 0.0f;
    m_vReserved14C = Vector(0.0f, 0.0f, 0.0f);
    m_vLastOrigin = m_vOrigin;
    m_qWorldOrientation = m_qOrientation;
    m_vXAxis = m_qOrientation.xdir();
    m_vYAxis = m_qOrientation.ydir();
    m_vDirection = m_qOrientation.zdir();
}
// PC 0x004CA020 GOG 0x004CA130
void LightningEmitter::InitializeBranch()
{
    PartEmitter::Initialize();
    m_pNode->ControlDispatch(this, -1.0f, 0.0f, 1.0f);
    m_lstReserved210.SetSize(2);
    m_lstReserved21C.SetSize(2);
    m_lstReserved228.SetSize(2);
    m_lstReserved234.SetSize(2);
    m_lstReserved240.SetSize(2);
    m_lstReserved24C.SetSize(2);
    m_lstBranches.SetSize(0);
    m_lstBranchIndices.SetSize(0);
    m_bBranch = true;
    m_pTarget = new Gob("_AUR_lightningbranch");
    m_fYSize = 0.0f;
    m_fXSize = 0.0f;
    m_fTargetSize = 0.0f;
    m_fReservedF4 = m_fLightningDelay;
    m_fReservedF8 = m_fControlPointDelay;
}
// PC 0x004CA190 GOG 0x004CA2A0
PartEmitter::~PartEmitter()
{
    delete m_pHitInfo;
    for (int i = m_lstActive.m_nCount - 1; i >= 0; i--)
    {
        Particle* pParticle = m_lstActive[i];
        m_lstActive.DelIndex(i);
        delete pParticle;
    }
    for (int i = m_lstFree.m_nCount - 1; i >= 0; i--)
    {
        Particle* pParticle = m_lstFree[i];
        m_lstFree.DelIndex(i);
        delete pParticle;
    }
    if (m_pWindManager)
    {
        m_pWindManager->DeregisterWindyEmitter(this);
    }
    if (m_pAurObject)
    {
        Scene::RemoveFromSceneManager(this);
    }
    delete m_pEmitterMaterial;
    totalemitters--;
}

unsigned char MdlNodeEmitter::IsInheritingVelocity()
{
#ifdef _XBOX
    unsigned char bInheritingVelocity = m_EmitterFlags.m_bInheritingVelocity;
    return bInheritingVelocity;
#else
    return (unsigned char)m_EmitterFlags.m_bInheritingVelocity;
#endif
}

static inline void AuremitterTrace4()
{
    AuremitterTrace3();
}

static inline void AuremitterTrace3()
{
    AuremitterTrace2();
}

static inline void AuremitterTrace2()
{
    AuremitterTrace1();
}

static inline void AuremitterTrace1()
{
}

#ifdef _XBOX
#undef GetNextFrame

int PartEmitter::GetNextFrame(int nFrame)
{
    if (m_fFrameStart != m_fFrameEnd)
    {
        if (m_fFrameEnd < m_fFrameStart)
        {
            int nPrev = nFrame - 1;
            if (nPrev >= m_fFrameEnd)
                return nPrev;
        }
        else
        {
            int nNext = nFrame + 1;
            if (nNext <= m_fFrameEnd)
                return nNext;
        }
    }
    return -1;
}

float PartEmitter::GetObjectAlpha()
{
    float fAlpha = 1.0f;
    if (m_pAurObject)
        fAlpha = m_pAurObject->m_fAlpha;
    return fAlpha;
}
#endif

#ifdef _XBOX

unsigned char MdlNodeEmitter::IsRandomOrder()
{
    unsigned char bRandomOrder = m_EmitterFlags.m_bRandomOrder;
    return bRandomOrder;
}

unsigned char MdlNodeEmitter::IsInheriting()
{
    unsigned char bInheriting = m_EmitterFlags.m_bInheriting;
    return bInheriting;
}

unsigned char MdlNodeEmitter::IsInheritingLocal()
{
    unsigned char bInheritingLocal = m_EmitterFlags.m_bInheritingLocal;
    return bInheritingLocal;
}

unsigned char MdlNodeEmitter::IsInheritingPart()
{
    unsigned char bInheritingPart = m_EmitterFlags.m_bInheritingPart;
    return bInheritingPart;
}

unsigned char MdlNodeEmitter::IsSplat()
{
    unsigned char bSplat = m_EmitterFlags.m_bSplat;
    return bSplat;
}

unsigned char MdlNodeEmitter::IsDepthTexture()
{
    return false;
}
#endif

#ifdef _XBOX

char* aurstartlog(char* sArguments)
{
    pAurLogFile = fopen("AurLogFile.txt", "w");
    if (pAurLogFile)
        return "success";
    return "failed";
}
#endif

#ifdef _XBOX
void PartEmitter::RenderLinked()
{
    static unsigned long poolSize = 1000;
    static RenderVertexVTC* particlePool =
        static_cast<RenderVertexVTC*>(operator new(sizeof(RenderVertexVTC) * 1000));

    Quaternion qRotation;
    Quaternion qCamera;
    int bFog = 0;
    int i;
    Vector vCameraX = CurrentCamera->m_qOrientation.xdir();
    Vector vCameraY = CurrentCamera->m_qOrientation.ydir();
    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);
    float fFrameTime = m_fFramesPerSecond;
    List<Vector> lstPositions;
    float fScale = 1.0f;

    if (m_lstActive.m_nCount == 0)
        return;

    LightningEmitter* pLightning = AsLightningEmitter();
    lstPositions.SetSize(m_lstActive.m_nCount);
    for (i = 0; i < m_lstActive.m_nCount; i++)
    {
        lstPositions[i] = m_lstActive[i]->m_vPosition;
        if (pLightning)
            lstPositions[i] += pLightning->m_lstReserved270[i];
    }

    if (renderemitterspheres)
        RenderBoundingSphere();

    if (4 * m_lstActive.m_nCount > poolSize)
    {
        particlePool = ResizeEmitterPool(particlePool, poolSize, 8 * m_lstActive.m_nCount);
        poolSize = 8 * m_lstActive.m_nCount;
    }
    if (m_pAurObject && m_pAurObject->m_fScale != 1.0f)
        fScale = m_pAurObject->m_fScale;
    if (fFrameTime > 0.0f)
        fFrameTime = 1.0f / fFrameTime;
    else
        fFrameTime = 0.0f;

    m_pEmitterMaterial->BindTexture0();
    m_pEmitterMaterial->SetBlendingMode(0);
    qCamera = CurrentCamera->m_qOrientation;
    Vector vCameraZ = CurrentCamera->m_qOrientation.zdir();
    float fInverseLife;
    if (m_pReserved1F4 == LIGHTNING)
        fInverseLife = 1.0f / m_lstActive.m_nCount;
    else if (m_fLifeTime > 0.0f)
        fInverseLife = 1.0f / m_fLifeTime;
    else
        fInverseLife = 0.01f;
    int nXGrid = pNode->m_nXGrid;
    if (!nXGrid) nXGrid = 1;
    int nYGrid = pNode->m_nYGrid;
    if (!nYGrid) nYGrid = 1;

    GLRender_SetDepthMask(GL_FALSE);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_Enable(ENABLE_BLEND);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    if (m_nBlendMode == BLEND_NORMAL)
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                    BLEND_ONE_MINUS_SRC_ALPHA);
    else if (m_nBlendMode == BLEND_PUNCH_THROUGH)
    {
        GLRender_Disable(ENABLE_BLEND);
        GLRender_Enable(ENABLE_ALPHA_TEST);
    }
    else
    {
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                    BLEND_ONE);
        bFog = GLRender_IsEnabled(ENABLE_FOG);
        GLRender_Disable(ENABLE_FOG);
    }
    updateParticleFrame(fFrameTime);
    particlecount += m_lstActive.m_nCount;
    float fUScale = 1.0f / nXGrid;
    float fVScale = 1.0f / nYGrid;
    float fTileAlpha = GetTileAlpha();
    float fGobAlpha = GetObjectAlpha();

    for (i = 0; i < m_lstActive.m_nCount; i++)
    {
        Vector vColor;
        float fAlpha;
        float fXSize;
        float fYSize;
        if (m_fLifeTime >= 0.0f)
        {
            float fAge;
            if (m_pReserved1F4 == LIGHTNING)
                fAge = i * fInverseLife;
            else
                fAge = fInverseLife * m_lstActive[i]->m_fAge;
            if (m_fPercentStart == EMITTER_UNSET_COLOR_KEY_TIME)
            {

                vColor = Interpolate(m_vColorStart, m_vColorEnd, fAge);
                fAlpha = (1.0f - fAge) * m_fAlphaStart + fAge * m_fAlphaEnd;
                fXSize = ((1.0f - fAge) * m_fXSizeStart + fAge * m_fXSizeEnd) * 0.5f;
                if (m_fYSizeStart != 0.0f || m_fYSizeEnd != 0.0f)
                    fYSize = (fAge * m_fYSizeEnd + (1.0f - fAge) * m_fYSizeStart) * 0.5f;
                else
                    fYSize = fXSize;
            }
            else if (fAge >= m_fPercentEnd)
            {
                vColor = m_vColorEnd;
                fAlpha = m_fAlphaEnd;
                fXSize = m_fXSizeEnd;
                fYSize = m_fYSizeEnd != 0.0f ? m_fYSizeEnd : fXSize;
            }

            else if (fAge >= m_fPercentMid)
            {
                fAge = (fAge - m_fPercentMid) / (m_fPercentEnd - m_fPercentMid);
                vColor = Interpolate(m_vColorMid, m_vColorEnd, fAge);
                fAlpha = (1.0f - fAge) * m_fAlphaMid + fAge * m_fAlphaEnd;
                fXSize = ((1.0f - fAge) * m_fXSizeMid + fAge * m_fXSizeEnd) * 0.5f;
                if (m_fYSizeMid == 0.0f && m_fYSizeEnd == 0.0f)
                    fYSize = fXSize;
                else
                    fYSize = (fAge * m_fYSizeEnd + (1.0f - fAge) * m_fYSizeMid) * 0.5f;
            }
            else if (fAge >= m_fPercentStart)
            {
                fAge = (fAge - m_fPercentStart) / (m_fPercentMid - m_fPercentStart);
                vColor = Interpolate(m_vColorStart, m_vColorMid, fAge);
                fAlpha = (1.0f - fAge) * m_fAlphaStart + fAge * m_fAlphaMid;
                fXSize = ((1.0f - fAge) * m_fXSizeStart + fAge * m_fXSizeMid) * 0.5f;
                if (m_fYSizeStart == 0.0f && m_fYSizeMid == 0.0f)
                    fYSize = fXSize;
                else
                    fYSize = (fAge * m_fYSizeMid + (1.0f - fAge) * m_fYSizeStart) * 0.5f;
            }
            else
            {
                vColor = m_vColorStart;
                fAlpha = m_fAlphaStart;
                fXSize = m_fXSizeStart;
                if (m_fYSizeStart != 0.0f)
                    fYSize = m_fYSizeStart;
                else
                    fYSize = fXSize;
            }
            if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
            else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
            if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
            else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
            if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
            else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
            if (fAlpha < 0.0f) fAlpha = 0.0f;
            else if (fAlpha > 1.0f) fAlpha = 1.0f;
        }
        else
        {
            vColor = m_vColorStart;
            fAlpha = m_fAlphaStart;
            if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
            else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
            if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
            else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
            if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
            else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
            if (fAlpha < 0.0f) fAlpha = 0.0f;
            else if (fAlpha > 1.0f) fAlpha = 1.0f;
            fXSize = m_fXSizeStart * 0.5f;
            if (m_fYSizeStart == 0.0f && m_fYSizeMid == 0.0f && m_fYSizeEnd == 0.0f)
                fYSize = fXSize;
            else
                fYSize = m_fYSizeStart * 0.5f;
        }

        qRotation = Quaternion(vCameraZ, m_lstActive[i]->m_fRotation);
        qRotation = qRotation * qCamera;
        int nRow = (int)(m_lstActive[i]->m_nFrame * fUScale);
        int nColumn = m_lstActive[i]->m_nFrame - nXGrid * nRow;
        if (fScale != 1.0f)
        {
            fXSize *= fScale;
            fYSize *= fScale;
        }
        if (i < m_lstActive.m_nCount - 1)
        {
            Vector vSegment = lstPositions[i + 1] - lstPositions[i];
            float fX = vCameraX * vSegment;
            float fY = vSegment * vCameraY;
            float fLength = (float)sqrt(fX * fX + fY * fY);
            Vector vSide;
            Vector vAlong;
            if (fLength > EMITTER_LINK_LENGTH_EPSILON)
            {

                fX /= fLength;
                fY /= fLength;
                vSide = vCameraX * fY - vCameraY * fX;
                vAlong = vCameraX * fX + vCameraY * fY;
            }
            else
            {
                vSide = vCameraX;
                vAlong = vCameraY;
            }
            vSide = vSide * (fXSize + fXSize);
            vAlong = vAlong * (fYSize * 0.5f);

            unsigned long nColor = (unsigned long)(vColor.m_fX * 255.0f) |
                (((unsigned long)(vColor.m_fY * 255.0f) |
                (((unsigned long)(vColor.m_fZ * 255.0f) |
                ((unsigned long)(fAlpha * (fGobAlpha * fTileAlpha) * 255.0f) << 8)) << 8)) << 8);

            int nVertex = i * 4;
            *reinterpret_cast<unsigned long*>(particlePool[nVertex].color) = nColor;
            *reinterpret_cast<unsigned long*>(particlePool[nVertex + 1].color) = nColor;
            *reinterpret_cast<unsigned long*>(particlePool[nVertex + 2].color) = nColor;
            *reinterpret_cast<unsigned long*>(particlePool[nVertex + 3].color) = nColor;
            Vector vCorner;
            if (nVertex > 0)
            {
                vCorner.m_fX = particlePool[4 * i - 3].position[0];
                vCorner.m_fY = particlePool[4 * i - 3].position[1];
                vCorner.m_fZ = particlePool[4 * i - 3].position[2];
            }
            else
                vCorner = (vSide + lstPositions[i]) - vAlong;
            particlePool[4 * i].position[0] = vCorner.m_fX;
            particlePool[4 * i].position[1] = vCorner.m_fY;
            particlePool[4 * i].position[2] = vCorner.m_fZ;
            particlePool[4 * i].u = (nColumn + 1) * fUScale;
            particlePool[4 * i].v = 1.0f - (nRow + 1) * fVScale;
            vCorner = (vSide + lstPositions[i + 1]) + vAlong;
            particlePool[4 * i + 1].position[0] = vCorner.m_fX;
            particlePool[4 * i + 1].position[1] = vCorner.m_fY;
            particlePool[4 * i + 1].position[2] = vCorner.m_fZ;
            particlePool[4 * i + 1].u = (nColumn + 1) * fUScale;
            particlePool[4 * i + 1].v = 1.0f - nRow * fVScale;
            vCorner = (lstPositions[i + 1] - vSide) + vAlong;
            particlePool[4 * i + 2].position[0] = vCorner.m_fX;
            particlePool[4 * i + 2].position[1] = vCorner.m_fY;
            particlePool[4 * i + 2].position[2] = vCorner.m_fZ;
            particlePool[4 * i + 2].u = nColumn * fUScale;
            particlePool[4 * i + 2].v = 1.0f - nRow * fVScale;
            if (nVertex > 0)
            {
                vCorner.m_fX = particlePool[4 * i - 2].position[0];
                vCorner.m_fY = particlePool[4 * i - 2].position[1];
                vCorner.m_fZ = particlePool[4 * i - 2].position[2];
            }
            else
                vCorner = (lstPositions[i] - vSide) - vAlong;
            vAlong.m_fX = vAlong.m_fY = vAlong.m_fZ = 0.0f;
            particlePool[4 * i + 3].position[0] = vCorner.m_fX;
            particlePool[4 * i + 3].position[1] = vCorner.m_fY;
            particlePool[4 * i + 3].position[2] = vCorner.m_fZ;
            particlePool[4 * i + 3].u = nColumn * fUScale;
            particlePool[4 * i + 3].v = 1.0f - (nRow + 1) * fVScale;
        }
    }
    countfaces += m_lstActive.m_nCount;
    int nVertices = 4 * m_lstActive.m_nCount - 4;
    GLRender::RenderObjectsVTCQuadUP(particlePool, nVertices);
    GLRender_SetDepthMask(GL_TRUE);
    GLRender_Enable(ENABLE_LIGHTING);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                BLEND_ONE_MINUS_SRC_ALPHA);
    if (bFog)
        GLRender_Enable(ENABLE_FOG);
}
#endif

#ifdef _XBOX
void PartEmitter::RenderMotionBlurred()
{

    static unsigned long poolSize = 1000;
    static RenderVertexVTC* particlePool =
        static_cast<RenderVertexVTC*>(operator new(1000 * sizeof(RenderVertexVTC)));

    Vector vCameraDirection;
    Quaternion qRotation;
    Quaternion qCamera;
    Vector vCameraX = CurrentCamera->m_qOrientation.xdir();
    Vector vCameraY = CurrentCamera->m_qOrientation.ydir();
    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);
    int bFog = 0;
    float fScale = 1.0f;
    float fStart = 0.0f;
    float fFrameTime = m_fFramesPerSecond;

    if (m_lstActive.m_nCount == 0)
        return;
    if (renderemitterspheres)
        RenderBoundingSphere();

    if (4 * m_lstActive.m_nCount > poolSize)
    {
        particlePool = ResizeEmitterPool(particlePool, poolSize, 8 * m_lstActive.m_nCount);
        poolSize = 8 * m_lstActive.m_nCount;
    }

    if (m_pAurObject && m_pAurObject->m_fScale != 1.0f)
        fScale = m_pAurObject->m_fScale;
    if (fFrameTime > 0.0f)
        fFrameTime = 1.0f / fFrameTime;
    else
        fFrameTime = 0.0f;
    m_pEmitterMaterial->BindTexture0();
    m_pEmitterMaterial->SetBlendingMode(0);
    if (EMITTER_MOTIONBLUR_DEBUG)
    {
        vCameraDirection.m_fX = 0.0f;
        vCameraDirection.m_fY = 0.0f;
        vCameraDirection.m_fZ = 0.0f;
    }
    vCameraDirection = CurrentCamera->m_qOrientation.zdir();
    qCamera = CurrentCamera->m_qOrientation;

    float fReciprocalLife;
    if (m_pReserved1F4 == LIGHTNING)
        fReciprocalLife = 1.0f / m_lstActive.m_nCount;
    else if (m_fLifeTime > 0.0f)
        fReciprocalLife = 1.0f / m_fLifeTime;
    else
        fReciprocalLife = 0.01f;

    int nXGrid = pNode->m_nXGrid;
    if (nXGrid == 0)
        nXGrid = 1;
    int nYGrid = pNode->m_nYGrid;
    if (nYGrid == 0)
        nYGrid = 1;

    GLRender_SetDepthMask(0);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_Enable(ENABLE_BLEND);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    GLRender_Disable(ENABLE_CULL_FACE);
    if (m_nBlendMode == BLEND_NORMAL)
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    else if (m_nBlendMode == BLEND_PUNCH_THROUGH)
    {
        GLRender_Disable(ENABLE_BLEND);
        GLRender_Enable(ENABLE_ALPHA_TEST);
    }
    else
    {
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE);
        bFog = GLRender_IsEnabled(ENABLE_FOG);
        GLRender_Disable(ENABLE_FOG);
    }

    updateParticleFrame(fFrameTime);
    particlecount += m_lstActive.m_nCount;
    float fU = 1.0f / nXGrid;
    float fV = 1.0f / nYGrid;
    float fTileAlpha = GetTileAlpha();
    float fAlpha = 0.0f;
    float fTailAlpha = 0.0f;
#ifdef _XBOX
    float fObjectAlpha = GetObjectAlpha();
#else
    float fObjectAlpha = 1.0f;
    if (m_pAurObject)
        fObjectAlpha = m_pAurObject->m_fAlpha;
#endif

    int i = m_lstActive.m_nCount;
    while (i != 0)
    {
        --i;
        Particle* pParticle = m_lstActive[i];
        Vector vColor;
        float fWidth;
        float fHeight;
        if (m_fLifeTime >= 0.0f)
        {
            if (m_pReserved1F4 == LIGHTNING)
                fFrameTime = i * fReciprocalLife;
            else
                fFrameTime = fReciprocalLife * pParticle->m_fAge;

            if (m_fPercentStart == EMITTER_UNSET_COLOR_KEY_TIME)
            {
                vColor = Interpolate(m_vColorStart, m_vColorEnd, fFrameTime);
                fStart = 1.0f - fFrameTime;
                fAlpha = fFrameTime * m_fAlphaEnd + fStart * m_fAlphaStart;
                fWidth = fFrameTime * m_fXSizeEnd;
                fWidth += fStart * m_fXSizeStart;
                fWidth *= 0.5f;
                if (m_fYSizeStart == 0.0f && m_fYSizeEnd == 0.0f)
                    fHeight = fWidth;
                else
                {
                    fHeight = fFrameTime * m_fYSizeEnd;
                    fHeight += fStart * m_fYSizeStart;
                    fHeight *= 0.5f;
                }
            }
            else if (fFrameTime >= m_fPercentEnd)
            {
                vColor = m_vColorEnd;
                fAlpha = m_fAlphaEnd;
                fWidth = m_fXSizeEnd;
                fHeight = m_fYSizeEnd != 0.0f ? m_fYSizeEnd : fWidth;
            }
            else if (fFrameTime >= m_fPercentMid)
            {
                fFrameTime = (fFrameTime - m_fPercentMid) / (m_fPercentEnd - m_fPercentMid);
                vColor = Interpolate(m_vColorMid, m_vColorEnd, fFrameTime);
                float fStart = 1.0f - fFrameTime;
                fAlpha = fStart * m_fAlphaMid;
                fAlpha += fFrameTime * m_fAlphaEnd;
                fWidth = fStart * m_fXSizeMid;
                fWidth += fFrameTime * m_fXSizeEnd;
                fWidth *= 0.5f;
                if (m_fYSizeMid == 0.0f && m_fYSizeEnd == 0.0f)
                    fHeight = fWidth;
                else
                {
                    fHeight = fFrameTime * m_fYSizeEnd;
                    fHeight += fStart * m_fYSizeMid;
                    fHeight *= 0.5f;
                }
            }
            else if (fFrameTime >= m_fPercentStart)
            {
                fFrameTime = (fFrameTime - m_fPercentStart) / (m_fPercentMid - m_fPercentStart);
                vColor = Interpolate(m_vColorStart, m_vColorMid, fFrameTime);
                float fStart = 1.0f - fFrameTime;
                fAlpha = fStart * m_fAlphaStart;
                fAlpha += fFrameTime * m_fAlphaMid;
                fWidth = fStart * m_fXSizeStart;
                fWidth += fFrameTime * m_fXSizeMid;
                fWidth *= 0.5f;
                if (m_fYSizeStart != 0.0f || m_fYSizeMid != 0.0f)
                {
                    fHeight = fFrameTime * m_fYSizeMid;
                    fHeight += fStart * m_fYSizeStart;
                    fHeight *= 0.5f;
                }
                else
                    fHeight = fWidth;
            }
            else
            {
                vColor = m_vColorStart;
                fAlpha = m_fAlphaStart;
                fWidth = m_fXSizeStart;
                if (m_fYSizeStart != 0.0f)
                    fHeight = m_fYSizeStart;
                else
                    fHeight = fWidth;
            }
            if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
            else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
            if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
            else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
            if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
            else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
            if (fAlpha < 0.0f) fAlpha = 0.0f;
            else if (fAlpha > 1.0f) fAlpha = 1.0f;
        }
        else
        {
            vColor = m_vColorStart;
            fAlpha = m_fAlphaStart;
            if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
            else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
            if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
            else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
            if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
            else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
            if (fAlpha < 0.0f) fAlpha = 0.0f;
            else if (fAlpha > 1.0f) fAlpha = 1.0f;
            fWidth = m_fXSizeStart * 0.5f;

            if (m_fYSizeStart == 0.0f)
            {
                if (m_fYSizeEnd == 0.0f)
                    fHeight = fWidth;
                else
                    fHeight = m_fYSizeStart * 0.5f;
            }
            else
                fHeight = m_fYSizeStart * 0.5f;
        }

        Vector vImaginary = normalize(vCameraDirection) *
            sineLUT(pParticle->m_fRotation * 0.5f);
        qRotation = Quaternion(cosineLUT(pParticle->m_fRotation * 0.5f),
            vImaginary.m_fX, vImaginary.m_fY, vImaginary.m_fZ);
        qRotation = qRotation * qCamera;
        int nRow = (int)(pParticle->m_nFrame * fU);
        int nColumn = pParticle->m_nFrame - nXGrid * nRow;
        Vector vOrigin = pParticle->m_vOrigin;
        Vector vTravel = pParticle->m_vPosition - vOrigin;
        float fTravelX = vCameraX * vTravel;
        float fTravelY = vCameraY * vTravel;
        float fLength = sqrt(fTravelY * fTravelY + fTravelX * fTravelX);
        Vector vRight;
        Vector vUp;
        if (fLength != 1.0f && fLength > 0.00001)
        {
            float fDirectionX = (fTravelX * (1.0f / fLength));
            float fDirectionY = ((1.0f / fLength) * fTravelY);
            vRight = vCameraX * fDirectionY - vCameraY * fDirectionX;
            vUp = vCameraX * fDirectionX + vCameraY * fDirectionY;
        }
        else
        {
            vRight = vCameraX;
            vUp = vCameraY;
            if (fLength == 0.0f)
                fLength = 1.0f;
        }
        fTailAlpha = fWidth / fLength * 2.0f;
        if (!(fTailAlpha < 1.0f))
            fTailAlpha = 1.0f;
        if (fScale != 1.0f)
        {
            fWidth *= fScale;
            fHeight *= fScale;
        }
        Vector vWidth;
        Vector vHeight;
        vWidth = vRight * (fWidth * 2.0f);
        vHeight = vUp * (fHeight * 0.5f);
        int nBlue = (int)(vColor.m_fZ * 255.0f);
        int nGreen = (int)(vColor.m_fY * 255.0f);
        int nRed = (int)(vColor.m_fX * 255.0f);
        unsigned long nColor = nRed | ((nGreen | ((nBlue |
            ((unsigned long)((fObjectAlpha * fAlpha) * fTileAlpha * 255.0f) << 8)) << 8)) << 8);
        int nVertex = i * 4;
        *reinterpret_cast<unsigned long*>(particlePool[nVertex + 0].color) = nColor;
        *reinterpret_cast<unsigned long*>(particlePool[nVertex + 1].color) = nColor;
        nColor = nRed | ((nGreen | ((nBlue |
            ((unsigned long)((fObjectAlpha * fTailAlpha * fAlpha * fTileAlpha) * 255.0f) << 8)) << 8)) << 8);
        *reinterpret_cast<unsigned long*>(particlePool[nVertex + 2].color) = nColor;
        *reinterpret_cast<unsigned long*>(particlePool[nVertex + 3].color) = nColor;

        Vector vVertexPosition = pParticle->m_vPosition + vWidth + vHeight;
        particlePool[i * 4 + 0].position[0] = vVertexPosition.m_fX;
        particlePool[i * 4 + 0].position[1] = vVertexPosition.m_fY;
        particlePool[i * 4 + 0].position[2] = vVertexPosition.m_fZ;
        particlePool[i * 4 + 0].u = (nColumn + 1) * fU;
        particlePool[i * 4 + 0].v = 1.0f - nRow * fV;
        vVertexPosition = pParticle->m_vPosition - vWidth + vHeight;
        particlePool[i * 4 + 1].position[0] = vVertexPosition.m_fX;
        particlePool[i * 4 + 1].position[1] = vVertexPosition.m_fY;
        particlePool[i * 4 + 1].position[2] = vVertexPosition.m_fZ;
        particlePool[i * 4 + 1].u = nColumn * fU;
        particlePool[i * 4 + 1].v = 1.0f - nRow * fV;
        vVertexPosition = vOrigin - vWidth - vHeight;
        particlePool[i * 4 + 2].position[0] = vVertexPosition.m_fX;
        particlePool[i * 4 + 2].position[1] = vVertexPosition.m_fY;
        particlePool[i * 4 + 2].position[2] = vVertexPosition.m_fZ;
        particlePool[i * 4 + 2].u = nColumn * fU;
        particlePool[i * 4 + 2].v = 1.0f - (nRow + 1) * fV;
        vVertexPosition = vOrigin + vWidth - vHeight;
        particlePool[i * 4 + 3].position[0] = vVertexPosition.m_fX;
        particlePool[i * 4 + 3].position[1] = vVertexPosition.m_fY;
        particlePool[i * 4 + 3].position[2] = vVertexPosition.m_fZ;
        particlePool[i * 4 + 3].u = (nColumn + 1) * fU;
        particlePool[i * 4 + 3].v = 1.0f - (nRow + 1) * fV;
    }

    countfaces += m_lstActive.m_nCount;
    GLRender::RenderObjectsVTCQuadUP(particlePool, 4 * m_lstActive.m_nCount);
    GLRender_SetDepthMask(1);
    GLRender_Enable(ENABLE_LIGHTING);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    GLRender_Enable(ENABLE_CULL_FACE);
    if (bFog)
        GLRender_Enable(ENABLE_FOG);
    if (EMITTER_MOTIONBLUR_DEBUG)
    {
        qCamera.m_fW = 0.0f;
        qCamera.m_fX = 0.0f;
        qCamera.m_fY = 0.0f;
        qCamera.m_fZ = 0.0f;
    }
}
#endif

#ifdef _XBOX

#define AUR_EMITTER_DEPTH_PIXEL_SHADER 29
#define AUR_EMITTER_VP_DEPTH_SETUP 16
#define AUR_EMITTER_VP_DEPTH_SCALE 18
#define AUR_EMITTER_VP_PARTICLE_POSITION 19
#define AUR_EMITTER_VP_PARTICLE_NEAR 20
void PartEmitter::RenderStandard()
{
    static unsigned long poolSize = 1000;
    static RenderVertexVTC* particlePool =
        static_cast<RenderVertexVTC*>(operator new(1000 * sizeof(RenderVertexVTC)));

    Vector vUp;
    Vector vDirection;
    Vector vRight;
    MdlNodeEmitter* pNode = static_cast<MdlNodeEmitter*>(m_pNode);
    unsigned char bSplat = pNode->IsSplat();
    Quaternion qRotation;
    Quaternion qCamera;
    float fFrameTime = m_fFramesPerSecond;
    int bFog = 0;
    float fScale = 1.0f;
    int nPass = 0;
    int nPasses = 1;
    if (pNode->m_bFrameBlending)
        nPasses = 5;

    if (renderemitterspheres)
        RenderBoundingSphere();
    if (m_lstActive.m_nCount == 0)
        return;
    if (4 * m_lstActive.m_nCount > poolSize)
    {
        particlePool = ResizeEmitterPool(particlePool, poolSize, 8 * m_lstActive.m_nCount);
        poolSize = 8 * m_lstActive.m_nCount;
    }
    if (m_pAurObject && m_pAurObject->m_fScale != 1.0f)
        fScale = m_pAurObject->m_fScale;
    if (fFrameTime > 0.0f)
        fFrameTime = 1.0f / fFrameTime;
    else
        fFrameTime = 0.0f;

    m_pEmitterMaterial->BindTexture0();
    m_pEmitterMaterial->SetBlendingMode(0);
    unsigned char bDepthTexture = 0;
    if (pNode->IsDepthTexture() && !m_pEmitterMaterial->m_pTexture1->IsNull())
    {
        bDepthTexture = 1;
        m_pEmitterMaterial->BindTexture1();
        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        static VertexProgram* emittervp = new VertexProgram(cEmitterVP, cEmitterVPDeclaration);
        emittervp->Enable(this, false);
        GLRender_EnablePixelShaders(AUR_EMITTER_DEPTH_PIXEL_SHADER, m_pEmitterMaterial->m_pTexture0,
                                    m_pEmitterMaterial->m_pTexture1, 0, 0);
        AurSetVertexProgramParameter4f(AUR_EMITTER_VP_DEPTH_SETUP, 0.0f, 2.0f, 3.0f, 1.0f);
        AurSetVertexProgramParameter4f(AUR_EMITTER_VP_DEPTH_SCALE, 16777216.0f, 8388608.0f, 0.0625f, 1.0f);
    }
    vUp.m_fX = vUp.m_fY = 0.0f;
    vUp.m_fZ = 1.0f;
    if (m_nReserved114 == NORMAL)
    {
        vDirection = CurrentCamera->m_qOrientation.zdir();
        qCamera = CurrentCamera->m_qOrientation;
    }
    else if (m_nReserved114 == WorldZ)
    {
        vDirection = vUp;
        qCamera = Quaternion(vUp, 0.0f);
    }
    else if (m_nReserved114 == LocalZ)
    {
        vDirection = m_qOrientation.zdir();
        qCamera = m_qOrientation;
    }
    else
    {
        vDirection = CurrentCamera->m_qOrientation.zdir();
        qCamera = CurrentCamera->m_qOrientation;
        if (qCamera.ydir() * vUp < 0.0f)
            vUp.m_fZ = -1.0f;
    }

    float fInverseLife = m_fLifeTime > 0.0f ? 1.0f / m_fLifeTime : 0.01f;
    int nXGrid = pNode->m_nXGrid ? pNode->m_nXGrid : 1;
    int nYGrid = pNode->m_nYGrid ? pNode->m_nYGrid : 1;

    GLRender_SetDepthMask(GL_FALSE);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_Enable(ENABLE_BLEND);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    if (static_cast<MdlNodeEmitter*>(m_pNode)->m_nTwoSidedTex)
    {
        GLRender_SetPolygonRenderMode(FACING_FRONT_AND_BACK,
                      FILL_SOLID);
        GLRender_Disable(ENABLE_CULL_FACE);
    }
    if (m_nBlendMode == BLEND_NORMAL)
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                    BLEND_ONE_MINUS_SRC_ALPHA);
    else if (m_nBlendMode == BLEND_PUNCH_THROUGH)
    {
        GLRender_Disable(ENABLE_BLEND);
        GLRender_Enable(ENABLE_ALPHA_TEST);
    }
    else
    {
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE);
        bFog = GLRender_IsEnabled(ENABLE_FOG);
        GLRender_Disable(ENABLE_FOG);
    }
    updateParticleFrame(fFrameTime);
    particlecount += m_lstActive.m_nCount;
    float fUScale = 1.0f / nXGrid;
    float fVScale = 1.0f / nYGrid;
    GLRender_PushCurrentMatrix();
    float fTileAlpha = GetTileAlpha();
    float fAlpha = 0.0f;
    float fFrameAlpha = 0.0f;
#ifdef _XBOX
    float fObjectAlpha = GetObjectAlpha();
#else
    float fObjectAlpha = 1.0f;
    if (m_pAurObject)
        fObjectAlpha = m_pAurObject->m_fAlpha;
#endif
    int nNextFrame;
    int nPreviousFrame;
    for (; nPass < nPasses; nPass++)
    {
        Vector vCameraDirection = CurrentCamera->m_qOrientation.zdir();
        int i = m_lstActive.m_nCount;
        while (i--)
        {
            Particle* pParticle = m_lstActive[m_lstActive.m_nCount - i - 1];
            Vector vColor;
            float fXSize;
            float fYSize;
            float fAge;
            float fT;
            if (m_fLifeTime >= 0.0f)
            {
                fAge = fInverseLife * pParticle->m_fAge;
                if (m_fPercentStart == EMITTER_UNSET_COLOR_KEY_TIME)
                {

                    fT = fAge;
                    vColor = Interpolate(m_vColorStart, m_vColorEnd, fT);
                    fAlpha = (1.0f - fT) * m_fAlphaStart + fT * m_fAlphaEnd;
                    fXSize = ((1.0f - fT) * m_fXSizeStart + fT * m_fXSizeEnd) * 0.5f;
                    if (m_fYSizeStart != 0.0f || m_fYSizeEnd != 0.0f)
                        fYSize = (fT * m_fYSizeEnd + (1.0f - fT) * m_fYSizeStart) * 0.5f;
                    else
                        fYSize = fXSize;
                }
                else if (fAge >= m_fPercentEnd)
                {
                    vColor = m_vColorEnd;
                    fAlpha = m_fAlphaEnd;
                    fXSize = m_fXSizeEnd * 0.5f;
                    fYSize = m_fYSizeEnd != 0.0f ? m_fYSizeEnd * 0.5f : fXSize;
                }

                else if (fAge >= m_fPercentMid)
                {
                    fT = (fAge - m_fPercentMid) / (m_fPercentEnd - m_fPercentMid);
                    vColor = Interpolate(m_vColorMid, m_vColorEnd, fT);
                    fAlpha = (1.0f - fT) * m_fAlphaMid + fT * m_fAlphaEnd;
                    fXSize = ((1.0f - fT) * m_fXSizeMid + fT * m_fXSizeEnd) * 0.5f;
                    if (m_fYSizeMid == 0.0f && m_fYSizeEnd == 0.0f)
                        fYSize = fXSize;
                    else
                        fYSize = (fT * m_fYSizeEnd + (1.0f - fT) * m_fYSizeMid) * 0.5f;
                }
                else if (fAge >= m_fPercentStart)
                {
                    fT = (fAge - m_fPercentStart) / (m_fPercentMid - m_fPercentStart);
                    vColor = Interpolate(m_vColorStart, m_vColorMid, fT);
                    fAlpha = (1.0f - fT) * m_fAlphaStart + fT * m_fAlphaMid;
                    fXSize = ((1.0f - fT) * m_fXSizeStart + fT * m_fXSizeMid) * 0.5f;
                    if (m_fYSizeStart == 0.0f && m_fYSizeMid == 0.0f)
                        fYSize = fXSize;
                    else
                        fYSize = (fT * m_fYSizeMid + (1.0f - fT) * m_fYSizeStart) * 0.5f;
                }
                else
                {
                    vColor = m_vColorStart;
                    fAlpha = m_fAlphaStart;
                    fXSize = m_fXSizeStart * 0.5f;
                    if (m_fYSizeStart != 0.0f)
                        fYSize = m_fYSizeStart * 0.5f;
                    else
                        fYSize = fXSize;
                }
                if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
                else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
                if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
                else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
                if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
                else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
                if (fAlpha < 0.0f) fAlpha = 0.0f;
                else if (fAlpha > 1.0f) fAlpha = 1.0f;
            }
            else
            {
                vColor = m_vColorStart;
                fAlpha = m_fAlphaStart;
                if (vColor.m_fX < 0.0f) vColor.m_fX = 0.0f;
                else if (vColor.m_fX > 1.0f) vColor.m_fX = 1.0f;
                if (vColor.m_fY < 0.0f) vColor.m_fY = 0.0f;
                else if (vColor.m_fY > 1.0f) vColor.m_fY = 1.0f;
                if (vColor.m_fZ < 0.0f) vColor.m_fZ = 0.0f;
                else if (vColor.m_fZ > 1.0f) vColor.m_fZ = 1.0f;
                if (fAlpha < 0.0f) fAlpha = 0.0f;
                else if (fAlpha > 1.0f) fAlpha = 1.0f;
                fXSize = m_fXSizeStart * 0.5f;
                if (m_fYSizeStart == 0.0f && m_fYSizeMid == 0.0f && m_fYSizeEnd == 0.0f)
                    fYSize = fXSize;
                else
                    fYSize = m_fYSizeStart * 0.5f;
            }
            if (bDepthTexture)
            {
                Vector vNear = pParticle->m_vPosition - vCameraDirection * fXSize;
                AurSetVertexProgramParameter4f(AUR_EMITTER_VP_PARTICLE_POSITION, pParticle->m_vPosition.m_fX,
                    pParticle->m_vPosition.m_fY, pParticle->m_vPosition.m_fZ, 1.0f);
                AurSetVertexProgramParameter4f(AUR_EMITTER_VP_PARTICLE_NEAR, vNear.m_fX, vNear.m_fY, vNear.m_fZ,
                    1.0f);
            }

            if (bSplat)
            {
                if (pParticle->m_bReserved6C)
                {
                    fXSize = m_fXSizeEnd;
                    if (m_fYSizeEnd != 0.0f)
                        fYSize = m_fYSizeEnd;
                    else
                        fYSize = fXSize;
                }
                else
                {
                    fXSize = m_fXSizeStart;
                    if (m_fYSizeStart != 0.0f)
                        fYSize = m_fYSizeStart;
                    else
                        fYSize = fXSize;
                }
            }
            if (pParticle->m_bReserved6C)
            {
                qRotation = Quaternion(pParticle->m_vHeading, pParticle->m_fRotation);
                qRotation = qRotation * pParticle->m_qOrientation;
            }
            else
            {
                qRotation = Quaternion(vDirection, pParticle->m_fRotation);
                qRotation = qRotation * qCamera;
            }
            if (fScale != 1.0f)
            {
                fXSize *= fScale;
                fYSize *= fScale;
            }

            fFrameAlpha = 0.0f;
            int nColumn = 0;
            int nRow = 0;
            if (nPass == 1)
            {
                nNextFrame = GetNextFrame(pParticle->m_nFrame);
                if (nNextFrame >= 0)
                {
                    fFrameAlpha = (pParticle->m_fAge - pParticle->m_fFrameAge) /
                                  fFrameTime * 0.5f;
                    nRow = (int)(nNextFrame * fUScale);
                    nColumn = nNextFrame - nXGrid * nRow;
                }
            }
            else if (nPass == 2)
            {
                nPreviousFrame = GetNeighborFrame(pParticle->m_nFrame);
                if (nPreviousFrame >= 0)
                {
                    fAge = (pParticle->m_fAge - pParticle->m_fFrameAge) / fFrameTime;
                    fFrameAlpha = 1.0f - fAge * 0.5f;
                    nRow = (int)(nPreviousFrame * fUScale);
                    nColumn = nPreviousFrame - nXGrid * nRow;
                }
            }
            else if (nPass == 3)
            {
                nNextFrame = GetNextFrame(nNextFrame);
                if (nNextFrame >= 0)
                {
                    fFrameAlpha = (pParticle->m_fAge - pParticle->m_fFrameAge) /
                                  fFrameTime * 0.25f;
                    nRow = (int)(nNextFrame * fUScale);
                    nColumn = nNextFrame - nXGrid * nRow;
                }
            }
            else if (nPass == 4)
            {
                nPreviousFrame = GetNeighborFrame(nPreviousFrame);
                if (nPreviousFrame >= 0)
                {
                    fAge = (pParticle->m_fAge - pParticle->m_fFrameAge) / fFrameTime;
                    fFrameAlpha = (1.0f - fAge) * 0.25f;
                    nRow = (int)(nPreviousFrame * fUScale);
                    nColumn = nPreviousFrame - nXGrid * nRow;
                }
            }
            else
            {
                roundToInt(nRow, (float)floor(pParticle->m_nFrame * fUScale));
                nColumn = pParticle->m_nFrame - nXGrid * nRow;
                if (pNode->m_bFrameBlending)
                    fFrameAlpha = 0.5f;
                else
                    fFrameAlpha = 1.0f;
            }
            Vector vVertical;
            if (m_nReserved114 == AlignedToParticleDir)
            {
                vRight = pParticle->m_qOrientation.xdir();
                vVertical = pParticle->m_qOrientation.zdir();
            }
            else
            {
                vRight = qRotation.xdir();
                if (m_nReserved114 == Aligned)
                    vVertical = vUp;
                else if (bSplat)
                    vVertical = cross(pParticle->m_vHeading, vRight);
                else
                    vVertical = cross(vDirection, vRight);
            }
            vRight = vRight * fXSize;
            vVertical = vVertical * fYSize;
            int nRed, nGreen, nBlue, nAlpha;
            roundToInt(nRed, vColor.m_fX * 255.0f);
            roundToInt(nGreen, vColor.m_fY * 255.0f);
            roundToInt(nBlue, vColor.m_fZ * 255.0f);
            roundToInt(nAlpha, fObjectAlpha * fFrameAlpha * fAlpha * fTileAlpha * 255.0f);
            unsigned long nColor = nRed | ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8);

            RenderVertexVTC* pVertices = particlePool + 4 * i;
            for (int j = 0; j < 4; j++)
                *reinterpret_cast<unsigned long*>(pVertices[j].color) = nColor;

            Vector vLeftCorner = pParticle->m_vPosition - vRight - vVertical;
            pVertices[0].position[0] = vLeftCorner.m_fX;
            particlePool[4 * i + 0].position[1] = vLeftCorner.m_fY;
            particlePool[4 * i + 0].position[2] = vLeftCorner.m_fZ;
            pVertices[0].u = nColumn * fUScale;
            pVertices[0].v = 1.0f - (nRow + 1) * fVScale;
            Vector vRightCorner = pParticle->m_vPosition + vRight - vVertical;
            pVertices[1].position[0] = vRightCorner.m_fX;
            particlePool[4 * i + 1].position[1] = vRightCorner.m_fY;
            particlePool[4 * i + 1].position[2] = vRightCorner.m_fZ;
            pVertices[1].u = (nColumn + 1) * fUScale;
            pVertices[1].v = 1.0f - (nRow + 1) * fVScale;
            vRightCorner = pParticle->m_vPosition + vRight + vVertical;
            pVertices[2].position[0] = vRightCorner.m_fX;
            particlePool[4 * i + 2].position[1] = vRightCorner.m_fY;
            particlePool[4 * i + 2].position[2] = vRightCorner.m_fZ;
            pVertices[2].u = (nColumn + 1) * fUScale;
            pVertices[2].v = 1.0f - nRow * fVScale;
            vLeftCorner = pParticle->m_vPosition - vRight + vVertical;
            pVertices[3].position[0] = vLeftCorner.m_fX;
            particlePool[4 * i + 3].position[1] = vLeftCorner.m_fY;
            particlePool[4 * i + 3].position[2] = vLeftCorner.m_fZ;
            pVertices[3].u = nColumn * fUScale;
            pVertices[3].v = 1.0f - nRow * fVScale;
        }
        if (nPass > 0)
        {
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE);
            bFog = GLRender_IsEnabled(ENABLE_FOG);
            GLRender_Disable(ENABLE_FOG);
        }
        countfaces += m_lstActive.m_nCount;
        if (bDepthTexture)
            GLRender::RenderObjectsVTCQuadUPVP(particlePool, 4 * m_lstActive.m_nCount);
        else
            GLRender::RenderObjectsVTCQuadUP(particlePool, 4 * m_lstActive.m_nCount);
    }
    GLRender_PopCurrentMatrix();
    GLRender_SetDepthMask(GL_TRUE);
    GLRender_Enable(ENABLE_LIGHTING);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    if (static_cast<MdlNodeEmitter*>(m_pNode)->m_nTwoSidedTex)
    {
        GLRender_SetPolygonRenderMode(FACING_FRONT_AND_BACK, FILL_SOLID);
        GLRender_Enable(ENABLE_CULL_FACE);
    }
    if (bDepthTexture)
    {
        GLRender_Disable(ENABLE_VERTEX_PROGRAM);
        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
    }
    if (bFog) GLRender_Enable(ENABLE_FOG);
}
#endif
