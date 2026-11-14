#define WIN32_LEAN_AND_MEAN
#if defined(_WIN32) && !defined(_XBOX)
#include "math3d.h"
#include <windows.h>
#else
#include "platform.h"
#endif
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "aurlightmanager.h"
#include "aurmodel.h"
#include "auremitter.h"
#include "aurscene.h"
#include "aurmaterial.h"
#include "aurprimitive.h"
#include "aurrendergl.h"
#include "aurwindmanager.h"

static inline void AuranimTrace4();
static inline void AuranimTrace3();
static inline void AuranimTrace2();
static inline void AuranimTrace1();

#pragma inline_recursion(on)
#pragma inline_depth(10)

int AnimRun::m_AnimFlags = 0;
AnimRun* AnimRun::m_pTraceRun = 0;
float AnimRun::m_AnimLength = 0;
int checkevents = 1;

int depth = 0;
int maxdepth = 1000;
int realmaxdepth = 0;
float animdepthdist1 = 12.0f;
float animdepthdist2 = 30.0f;
int animdepth2 = 8;
int animdepth1 = 12;
int checkdepth = 1;
float lightsabersegmentlife = 0.012f;

int lightsaberSegments = 0;
float lightsaberdelta = 0.01f;
float position_scale_factor = 1.0f;

int rebuildbboxes = 0;
int rebuildspheres = 1;
int enablegobanimations = 1;
int disablegobdist = 0;
float talkalpha = 0;
int testanimationshortcircuit = 1;

static const float DANGLY_MAX_TIME_STEP = 0.035f;
static const float DANGLY_MIN_TIME_STEP = 0.0125f;
static const float DANGLY_PERIOD_SCALE = 1.5f;
static const float DANGLY_TIGHTNESS_SCALE = 0.5f;
static const float DANGLY_WIND_SCALE = 20.0f;
// PC 0x004B5D30 GOG 0x004B5E40
CallBack* Gob::RegisterCallBack(void (*pfnCallBack)(CAurObject*, char*, void*),
                                char* sEvent, void* pData, float fDelay,
                                bool bRepeat)
{
    CallBack* pCallBack =
        new CallBack(pfnCallBack, sEvent, pData, fDelay, bRepeat);
    m_lCallBacks.Add(pCallBack);
    return pCallBack;
}
// PC 0x004B5DB0 GOG 0x004B5EC0
void NewController::ReportKey(Part* pPart, MdlNode* pNode, int nKey,
                              float fDelta)
{
    SWMGReportKeyData report;
    report.m_fTime = fDelta;
    report.m_pPosition = 0;
    report.m_pOrientation = 0;
    report.m_bPositionKey = true;
    Gob* pObject = pPart->m_pAurObject;
    float fTime = (pNode->m_lstControllerData.m_pList)[nKey +
                                                    (unsigned short)m_nKeyIndex];

    int nValueType = m_nValueFormat & VALUE_TYPE_MASK;
    if (nValueType == VALUE_TYPE_FLOAT)
    {
        return;
    }

    if (nValueType == VALUE_TYPE_VECTOR)
    {
        Vector vPosition;
        GetVectorValue(fTime, pNode, vPosition);
        report.m_pPosition = &vPosition;
        report.m_bPositionKey = true;

        Quaternion qOrientation;
        int bFound = 0;
        for (int nController = 0; nController < pNode->m_lstControllers.m_nCount;
             ++nController)
        {
            if (&pNode->m_lstControllers[nController] != this &&
                pNode->m_lstControllers[nController].m_nOffset ==
                    OFFSET_ORIENTATION)
            {
                bFound = pNode->m_lstControllers[nController].GetQuaternionValue(
                    fTime, pNode, qOrientation);
                if (bFound)
                {
                    break;
                }
            }
        }
        if (!bFound)
        {
            qOrientation = pPart->m_qOrientation;
        }
        report.m_pOrientation = &qOrientation;
        pObject->DoEvent("ReportKey", &report);
    }
    else if (nValueType == VALUE_TYPE_QUATERNION ||
             nValueType == VALUE_TYPE_COMPRESSED_QUATERNION)
    {
        Quaternion qOrientation;
        GetQuaternionValue(fTime, pNode, qOrientation);
        report.m_pOrientation = &qOrientation;
        report.m_bPositionKey = false;

        Vector vPosition;
        for (int nController = 0; nController < pNode->m_lstControllers.m_nCount;
             ++nController)
        {
            NewController* pController = &pNode->m_lstControllers[nController];
            if (pController != this &&
                pController->m_nOffset == OFFSET_POSITION)
            {
                if (pController->GetVectorValue(fTime, pNode, vPosition))
                {
                    report.m_pPosition = &vPosition;
                    break;
                }
            }
        }
        pObject->DoEvent("ReportKey", &report);
    }
}
// PC 0x004B1EE0 GOG 0x004B1FF0
unsigned char LookAtAnimation::IsActive()
{
    return m_pOutOfOrderRun->m_bDone == 0;
}
// PC 0x004B1EF0 GOG 0x004B2000
void LookAtAnimation::Deactivate()
{
    m_bDeactivated = 1;
}
// PC 0x004B1F00 GOG 0x004B2010
void Gob::EndLookAtAnimate()
{
    if (m_pLookAtAnimation != 0)
    {
        m_pLookAtAnimation->Deactivate();
    }
}
// PC 0x004B1F10 GOG 0x004B2020
int AnimationParseField(void* pAnimation, char* sField)
{
    return ((Animation*)pAnimation)->InternalParseField(sField);
}
// PC 0x004B1F20 GOG 0x004B2030
AnimRun::AnimRun(Animation* pAnimation)
{
    m_pAnimation = pAnimation;
    m_fSpeed = 1.0f;
    m_nFlags = 0;
    m_fTime = 0.0f;
    m_fPreviousTime = 0.0f;
    m_fBlendWeight = 1.0f;
    m_fTransitionTime = 0.0f;
    m_bDone = 0;
    m_fScale = 1.0f;
}
// PC 0x004B1F50 GOG 0x004B2060
OOAnimRun::OOAnimRun(Animation* pAnimation, float* pKeyTimes, float* pValues,
                     int nCount, float fBlendTime)
    : AnimRun(pAnimation)
{
    m_pDeltaValues = new float[nCount];
    m_pStartValues = new float[nCount];

    int i;
    for (i = 0; i < nCount; i++)
    {
        m_pDeltaValues[i] = pValues[i] * pAnimation->m_fLength;
        m_pStartValues[i] = pKeyTimes[i];
    }

    m_nNumValues = nCount;
    m_fBlendTime = fBlendTime;
}
// PC 0x004B20E0 GOG 0x004B21F0
CallBack::CallBack(void (*pfnCallBack)(CAurObject*, char*, void*), char* sEvent,
                   void* pData, float fDelay, bool bRepeat)
{
    m_pfnCallBack = pfnCallBack;
    if (sEvent == 0)
    {
        sEvent = "";
    }
    strcpy(m_sEvent, sEvent);
    m_pData = pData;
    m_fDelay = fDelay;
    m_bRepeat = bRepeat;
}
// PC 0x004B2130 GOG 0x004B2240
inline Quaternion Interpolate(const Quaternion& qFirst,
                              const Quaternion& qSecond, float fPosition)
{
    return slerp(qFirst, qSecond, fPosition);
}
// PC 0x004B2170 GOG 0x004B2280
void PartLightsaber::Animate(float fTimeStep)
{
    if (!(fTimeStep > lightsaberdelta))
        fTimeStep = lightsaberdelta;

    short nSegment = m_nFirstSegment;
    short nExpired = 0;
    if (currentscene && currentscene->IsAnimating())
    {
        for (short nIndex = 0; nIndex < m_nSegments; ++nIndex)
        {
            m_afSegmentAge[nSegment] += fTimeStep;
            if (m_afSegmentAge[nSegment] > lightsabersegmentlife &&
                m_nSegments - nExpired > 1)
            {
                ++nExpired;
                if (++m_nFirstSegment == MdlNodeLightsaber::NUM_BACK_SEGMENTS)
                    m_nFirstSegment = 0;
            }
            if (++nSegment == MdlNodeLightsaber::NUM_BACK_SEGMENTS)
                nSegment = 0;
        }
        m_nSegments -= nExpired;
    }

    Vector vPosition;
    Quaternion qOrientation;
    GetWorld(vPosition, qOrientation);
    if (!equal(vPosition, m_vLastPosition) ||
        m_afSegmentAge[m_nFirstSegment] > 60.0f)
    {
        if (magnitude(vPosition - m_vLastPosition) > 1.0f)
        {
            m_nNextSegment = 0;
            m_nFirstSegment = 0;
            m_nSegments = 0;
        }
        m_vLastPosition = vPosition;
        m_afSegmentAge[m_nNextSegment] = 0.0f;
        m_avSegmentPosition[m_nNextSegment] = vPosition;
        m_aqSegmentOrientation[m_nNextSegment] = qOrientation;
        if (m_nSegments != MdlNodeLightsaber::NUM_BACK_SEGMENTS)
        {
            ++m_nNextSegment;
            ++m_nSegments;
            if (m_nNextSegment >= MdlNodeLightsaber::NUM_BACK_SEGMENTS)
                m_nNextSegment = 0;
        }
    }
    lightsaberSegments = m_nSegments;
}
#ifndef _XBOX
OOAnimRun::~OOAnimRun()
{
    if (m_pDeltaValues != 0)
    {
        delete[] m_pDeltaValues;
    }
    if (m_pStartValues != 0)
    {
        delete[] m_pStartValues;
    }
    m_pDeltaValues = 0;
    m_pStartValues = 0;
}
#endif
// PC 0x004B2400 GOG 0x004B2510
bool Gob::IsCharacter()
{
    if (m_pModel != 0)
    {
        char bCharacter = ((GobModel*)m_pModel)->m_bCharacter;
        return bCharacter;
    }

    return false;
}
// PC 0x004B2420 GOG 0x004B2530
inline bool Gob::IsRenderEarly()
{
    if (m_pModel != 0)
    {
        char bRenderEarly = ((GobModel*)m_pModel)->m_bRenderEarly;
        return bRenderEarly;
    }

    return false;
}
// PC 0x004B2440 GOG 0x004B2550
CustomAnimation::~CustomAnimation()
{
    if (m_pTree != 0)
    {
        delete m_pTree;
    }
}
// PC 0x004B2460 GOG 0x004B2570
void LookAtAnimation::Control()
{
    if (m_pTarget == 0)
        Deactivate();
    if (m_pTarget != 0)
    {
        Gob* pTarget = (Gob*)m_pTarget.m_pObject;
        if (magnitude(m_pGob->m_vPosition - pTarget->m_vPosition) > m_fMaxDistance)
            Deactivate();
    }
    if (!m_bDeactivated)
    {
        Quaternion& qLocal = m_pHeadPart->m_qOrientation;
        m_pStartOrientation[0] = qLocal.m_fX;
        m_pStartOrientation[1] = qLocal.m_fY;
        m_pStartOrientation[2] = qLocal.m_fZ;
        m_pStartOrientation[3] = qLocal.m_fW;

        Quaternion qHead;
        Vector vHead;
        m_pHeadPart->GetWorld(vHead, qHead);
        Vector vForward = qHead * Vector(0.0f, 1.0f, 0.0f);
        Vector vTarget;
        Quaternion qTarget;
        m_pPart->GetWorld(vTarget, qTarget);
        Vector vDirection = vTarget - vHead;
        Vector vHorizontal = vForward;
        vHorizontal.m_fZ = 0.0f;
        Vector vTargetHorizontal = vDirection;
        vTargetHorizontal.m_fZ = 0.0f;
        Quaternion qArc = RotationArc(vHorizontal, vTargetHorizontal);
        Quaternion qLook;
        qLook = qArc * qLocal;
        float fYaw = Yaw(qLook);
        vDirection = normalize(vDirection);
        float fAngle = (float)asin(vDirection.m_fZ);
        float fPitch = Pitch(Quaternion(Vector(1.0f, 0.0f, 0.0f), fAngle));
        fYaw = min(max(fYaw, -m_fYawLimit), m_fYawLimit);
        fPitch = min(max(fPitch, -m_fPitchLimit), m_fPitchLimit);
        qLook = YawPitchRoll(fYaw, 0.0f, 0.0f) * YawPitchRoll(0.0f, fPitch, 0.0f);
        m_pEndOrientation[0] = qLook.m_fX;
        m_pEndOrientation[1] = qLook.m_fY;
        m_pEndOrientation[2] = qLook.m_fZ;
        m_pEndOrientation[3] = qLook.m_fW;
        m_pOutOfOrderRun->m_fPreviousTime = 0.0f;
        m_pOutOfOrderRun->m_fTime = 0.0f;
        m_pOutOfOrderRun->m_fBlendWeight = 1.0f;
        m_pOutOfOrderRun->m_bDone = 0;
    }
    else if (!m_bRestoringOrientation)
    {
        m_bRestoringOrientation = 1;
        Quaternion& qLocal = m_pHeadPart->m_qOrientation;
        m_pStartOrientation[0] = qLocal.m_fX;
        m_pStartOrientation[1] = qLocal.m_fY;
        m_pStartOrientation[2] = qLocal.m_fZ;
        m_pStartOrientation[3] = qLocal.m_fW;
        m_pEndOrientation[0] = m_pEndOrientation[1] = m_pEndOrientation[2] = 0.0f;
        m_pEndOrientation[3] = 1.0f;
        m_pOutOfOrderRun->m_fPreviousTime = 0.0f;
        m_pOutOfOrderRun->m_fTime = 0.0f;
        m_pOutOfOrderRun->m_fBlendWeight = 1.0f;
        m_pOutOfOrderRun->m_nFlags = AnimRun::FLAG_SUPPRESS_KEY_REPORTS | AnimRun::FLAG_CURRENT_ANIMATION_PENDING;
        m_pOutOfOrderRun->m_bDone = 0;
        ((Animation*)m_pTree)->m_fLength = 0.25f;
    }
}
// PC 0x004B2B20 GOG 0x004B2C30
int Gob::StopOutOfOrderAnimation(char* sName)
{
    int nCount = m_lOutOfOrderAnimations.m_nCount;
    int nIndex;

    if (nCount <= 0)
    {
        return 0;
    }

    for (nIndex = 0; nIndex < nCount; ++nIndex)
    {
        AnimRun* pRun = m_lOutOfOrderAnimations[nIndex];
        if (pRun->m_nFlags & AnimRun::FLAG_OUT_OF_ORDER)
        {
            return PlayAnimation(sName, 1.0f, ANIMATION_FLAG_STOP_OUT_OF_ORDER,
                                 0);
        }
    }

    return nIndex;
}
// PC 0x004B2B70 GOG 0x004B2C80
Animation* FindAnimation(Model* pModel, char* sName)
{
    if (sName == NULL)
    {
        return NULL;
    }
    int i;
    for (i = 0; i < pModel->m_lAnimations.m_nCount; i++)
    {
        if (stricmp(pModel->m_lAnimations[i]->m_sName, sName) == 0)
        {
            return pModel->m_lAnimations[i];
        }
    }
    if (pModel->m_pParentModel != NULL)
    {
        return FindAnimation(pModel->m_pParentModel, sName);
    }

    char sDefaultAnimation[] = "default";
    for (i = 0; i < pModel->m_lAnimations.m_nCount; i++)
    {
        if (stricmp(pModel->m_lAnimations[i]->m_sName,
                    sDefaultAnimation) == 0)
        {
            return pModel->m_lAnimations[i];
        }
    }
    if (pModel->m_pParentModel != NULL)
    {
        return FindAnimation(pModel->m_pParentModel, sDefaultAnimation);
    }
    return NULL;
}
// PC 0x004B2F10 GOG 0x004B3020
void Gob::DeregisterCallback(CallBack* pCallBack)
{
    for (int nIndex = 0; nIndex < m_lCallBacks.m_nCount; ++nIndex)
    {
        if (pCallBack == m_lCallBacks[nIndex])
        {
            m_lCallBacks.DelIndex(nIndex);
            delete pCallBack;
            return;
        }
    }
}
// PC 0x004B2F80 GOG 0x004B3090
void AnimateEvents(Gob* pGob, Animation* pAnimation, float fPreviousTime,
                   float fTime, int nFlags)
{
    if (nFlags & Animation::EVENT_ALL)
    {
        AnimateEvents(pGob, pAnimation, -ANIMATION_EVENT_TIME_LIMIT,
                      ANIMATION_EVENT_TIME_LIMIT, nFlags & ~Animation::EVENT_ALL);
    }
    else
    {
        if (nFlags & Animation::EVENT_REVERSE)
        {
            if (fPreviousTime < fTime)
            {
                AnimateEvents(pGob, pAnimation, fPreviousTime,
                              -ANIMATION_EVENT_TIME_LIMIT, nFlags);
                AnimateEvents(pGob, pAnimation, ANIMATION_EVENT_TIME_LIMIT, fTime, nFlags);
                return;
            }
        }
        else
        {
            if (fPreviousTime > fTime)
            {
                AnimateEvents(pGob, pAnimation, fPreviousTime,
                              ANIMATION_EVENT_TIME_LIMIT, nFlags);
                AnimateEvents(pGob, pAnimation, -ANIMATION_EVENT_TIME_LIMIT, fTime, nFlags);
                return;
            }
        }
        if (nFlags & Animation::EVENT_REVERSE)
        {
            for (int nIndex = pAnimation->m_lEvents.m_nCount - 1; nIndex >= 0; --nIndex)
            {
                if (pAnimation->m_lEvents[nIndex].m_fTime >= fTime && pAnimation->m_lEvents[nIndex].m_fTime < fPreviousTime)
                    pGob->DoEvent(pAnimation->m_lEvents[nIndex].m_Value, 0);
            }
        }
        else
        {
            for (int nIndex = 0; nIndex < pAnimation->m_lEvents.m_nCount; ++nIndex)
            {
                if (pAnimation->m_lEvents[nIndex].m_fTime <= fTime && pAnimation->m_lEvents[nIndex].m_fTime > fPreviousTime)
                    pGob->DoEvent(pAnimation->m_lEvents[nIndex].m_Value, 0);
            }
        }
    }
}
// PC 0x004B3440 GOG 0x004B3550
void NewController::GetKeyIndexAndAlpha(float fTime, MdlNode* pNode, int& nKey,
                                        float& fAlpha)
{
    List<float>* pData = &pNode->m_lstControllerData;
    nKey = 0;
    fAlpha = 1.0f;

    for (; nKey < (unsigned short)m_nRows; ++nKey)
    {
        if (!(fTime > (*pData)[nKey + (unsigned short)m_nKeyIndex]))
        {
            break;
        }
    }

    if (nKey != 0 && nKey != (unsigned short)m_nRows)
    {
        float* pfKey = &(*pData)[nKey + (unsigned short)m_nKeyIndex];
        fAlpha = (fTime - pfKey[-1]) / (pfKey[0] - pfKey[-1]);
    }
}
// PC 0x004B34C0 GOG 0x004B35D0
int NewController::GetFloatValue(float fTime, MdlNode* pNode, float& value)
{
    if ((m_nValueFormat & VALUE_TYPE_MASK) != VALUE_TYPE_FLOAT)
    {
        return 0;
    }

    int nKey;
    float fAlpha;
    GetKeyIndexAndAlpha(fTime, pNode, nKey, fAlpha);

    if (nKey == 0)
    {
        value = (pNode->m_lstControllerData.m_pList)
            [(unsigned short)m_nColumnIndex];
    }
    else if (nKey == (unsigned short)m_nRows)
    {
        if ((m_nValueFormat & INTERPOLATION_MASK) == INTERPOLATION_BEZIER)
        {
            value = (pNode->m_lstControllerData.m_pList)
                [(unsigned short)m_nColumnIndex + nKey * 3 - 3];
        }
        else
        {
            value = (pNode->m_lstControllerData.m_pList)
                [(unsigned short)m_nColumnIndex + (unsigned short)m_nRows - 1];
        }
    }
    else if ((m_nValueFormat & INTERPOLATION_MASK) == INTERPOLATION_BEZIER)
    {
        int nIndex = (unsigned short)m_nColumnIndex + nKey * 3 - 3;
        value = InterpolateBezier(
            (pNode->m_lstControllerData.m_pList)[nIndex],
            (pNode->m_lstControllerData.m_pList)[nIndex + 2],
            (pNode->m_lstControllerData.m_pList)[nIndex + 3] +
                (pNode->m_lstControllerData.m_pList)[nIndex],
            (pNode->m_lstControllerData.m_pList)[nIndex + 4] +
                (pNode->m_lstControllerData.m_pList)[nIndex + 2],
            fAlpha);
    }
    else
    {
        float* pfKey = pNode->m_lstControllerData.m_pList +
                       (unsigned short)m_nColumnIndex + nKey;
        value = (1.0f - fAlpha) * pfKey[-1] + fAlpha * pfKey[0];
    }
    return 1;
}
// PC 0x004B35F0 GOG 0x004B3700
int NewController::GetVectorValue(float fTime, MdlNode* pNode, Vector& value)
{
    if ((m_nValueFormat & VALUE_TYPE_MASK) != VALUE_TYPE_VECTOR)
    {
        return 0;
    }

    int nKey;
    float fAlpha;
    GetKeyIndexAndAlpha(fTime, pNode, nKey, fAlpha);

    static Vector valuea;
    static Vector valueb;

    float fScale = 1.0f;
    if (m_nOffset == OFFSET_POSITION)
    {
        fScale = position_scale_factor;
    }

    int nIndex;
    if (nKey == 0)
    {
        nIndex = (unsigned short)m_nColumnIndex;
        value.m_fX = (pNode->m_lstControllerData.m_pList)[nIndex++];
        value.m_fY = (pNode->m_lstControllerData.m_pList)[nIndex++];
        value.m_fZ = (pNode->m_lstControllerData.m_pList)[nIndex];
        value = value * fScale;
    }

    else if ((m_nValueFormat & INTERPOLATION_MASK) == INTERPOLATION_BEZIER)
    {
        if (nKey == (unsigned short)m_nRows)
        {
            nIndex = (unsigned short)m_nColumnIndex + nKey * 9 - 9;
            value.m_fX = (pNode->m_lstControllerData.m_pList)[nIndex++];
            value.m_fY = (pNode->m_lstControllerData.m_pList)[nIndex++];
            value.m_fZ = (pNode->m_lstControllerData.m_pList)[nIndex];
            value = value * fScale;
        }

        else
        {
            nIndex = (unsigned short)m_nColumnIndex + nKey * 9 - 9;
            valuea.m_fX = (pNode->m_lstControllerData.m_pList)[nIndex];
            valuea.m_fY = (pNode->m_lstControllerData.m_pList)[nIndex + 1];
            valuea.m_fZ = (pNode->m_lstControllerData.m_pList)[nIndex + 2];
            valueb.m_fX = (pNode->m_lstControllerData.m_pList)[nIndex + 9];
            valueb.m_fY = (pNode->m_lstControllerData.m_pList)[nIndex + 10];
            valueb.m_fZ = (pNode->m_lstControllerData.m_pList)[nIndex + 11];

            Vector vTangentA;
            vTangentA.m_fX = (pNode->m_lstControllerData.m_pList)[nIndex + 6];
            vTangentA.m_fY = (pNode->m_lstControllerData.m_pList)[nIndex + 7];
            vTangentA.m_fZ = (pNode->m_lstControllerData.m_pList)[nIndex + 8];
            Vector vTangentB;
            vTangentB.m_fX = (pNode->m_lstControllerData.m_pList)[nIndex + 12];
            vTangentB.m_fY = (pNode->m_lstControllerData.m_pList)[nIndex + 13];
            vTangentB.m_fZ = (pNode->m_lstControllerData.m_pList)[nIndex + 14];
            value = InterpolateBezier(valuea, valueb, valuea + vTangentA,
            valueb + vTangentB, fAlpha) * fScale;
        }
    }
    else
    {
        if (nKey == (unsigned short)m_nRows)
        {
            nIndex = (unsigned short)m_nColumnIndex + nKey * 3 - 3;
            value.m_fX = (pNode->m_lstControllerData.m_pList)[nIndex++];
            value.m_fY = (pNode->m_lstControllerData.m_pList)[nIndex++];
            value.m_fZ = (pNode->m_lstControllerData.m_pList)[nIndex];
            value = value * fScale;
        }

        else
        {
            nIndex = (unsigned short)m_nColumnIndex + nKey * 3 - 3;
            valuea.m_fX = (pNode->m_lstControllerData.m_pList)[nIndex++];
            valuea.m_fY = (pNode->m_lstControllerData.m_pList)[nIndex++];
            valuea.m_fZ = (pNode->m_lstControllerData.m_pList)[nIndex++];
            valueb.m_fX = (pNode->m_lstControllerData.m_pList)[nIndex++];
            valueb.m_fY = (pNode->m_lstControllerData.m_pList)[nIndex++];
            valueb.m_fZ = (pNode->m_lstControllerData.m_pList)[nIndex];
            value = Interpolate(valuea, valueb, fAlpha) * fScale;
        }
    }
    return 1;
}
// PC 0x004B3920 GOG 0x004B3A30
void NewController::GetQuaternionFromIndexLocation(int nKey, MdlNode* pNode,
                                                   Quaternion& qValue)
{
    List<float>* pData = &pNode->m_lstControllerData;
    int nColumns = m_nValueFormat & VALUE_TYPE_MASK;
    if (nColumns == VALUE_TYPE_QUATERNION)
    {
        int nIndex = nKey - 1 < 0 ? 0 : nKey - 1;
        nIndex = (unsigned short)m_nColumnIndex + 4 * nIndex;
        qValue.m_fX = (*pData)[nIndex++];
        qValue.m_fY = (*pData)[nIndex++];
        qValue.m_fZ = (*pData)[nIndex++];
        qValue.m_fW = (*pData)[nIndex];
    }
    else if (nColumns == VALUE_TYPE_COMPRESSED_QUATERNION)
    {
        float fScale = 1.0f;
        unsigned int nPacked = *(unsigned int*)&(*pData)
            [(unsigned short)m_nColumnIndex + (nKey - 1 < 0 ? 0 : nKey - 1)];
        float fX = (float)(int)(nPacked & QUATERNION_COMPONENT_MASK) *
                   2.0f / QUATERNION_COMPONENT_RANGE - 1.0f;
        qValue.m_fX = fX;
        float fY = (float)(int)((nPacked >> QUATERNION_COMPONENT_BITS) &
                   QUATERNION_COMPONENT_MASK) *
                   2.0f / QUATERNION_COMPONENT_RANGE - 1.0f;
        qValue.m_fY = fY;
        float fZ = (float)(int)(nPacked >> QUATERNION_Z_SHIFT) *
                   2.0f / QUATERNION_Z_RANGE - 1.0f;
        qValue.m_fZ = fZ;
        float fLengthSquared = fX * fX + fY * fY + fZ * fZ;
        if (fLengthSquared <= 1.0f)
        {
            qValue.m_fW = sqrt(1.0f - fLengthSquared);
        }
        else
        {
            qValue.m_fW = 0.0f;
            float fLength = sqrt(fLengthSquared);
            fScale /= fLength;
            qValue.m_fX = fX * fScale;
            qValue.m_fY = fY * fScale;
            qValue.m_fZ = fZ * fScale;
        }
    }
}
// PC 0x004B3A80 GOG 0x004B3B90
int NewController::GetQuaternionValue(float fTime, MdlNode* pNode,
                                      Quaternion& value)
{
    int nValueType = m_nValueFormat & VALUE_TYPE_MASK;
    if (nValueType != VALUE_TYPE_QUATERNION &&
        nValueType != VALUE_TYPE_COMPRESSED_QUATERNION)
    {
        return 0;
    }

    int nKey;
    float fAlpha;
    GetKeyIndexAndAlpha(fTime, pNode, nKey, fAlpha);

    static Quaternion valuea;
    static Quaternion valueb;

    if (nKey == 0)
    {
        GetQuaternionFromIndexLocation(nKey, pNode, value);
        return 1;
    }
    if (nKey == (unsigned short)m_nRows)
    {
        GetQuaternionFromIndexLocation((unsigned short)m_nRows, pNode, value);
        return 1;
    }

    GetQuaternionFromIndexLocation(nKey, pNode, valuea);
    GetQuaternionFromIndexLocation(++nKey, pNode, valueb);
    Quaternion qValue = slerp(valuea, valueb, fAlpha);
    value = qValue;
    return 1;
}
// PC 0x004B3C00 GOG 0x004B3D10
void NewController::Control(Part* pPart, MdlNode* pNode, float fTime,
                            float fPreviousTime, float fBlend)
{
    int nType = m_nValueFormat & VALUE_TYPE_MASK;
    if (nType == VALUE_TYPE_FLOAT)
    {
        float* pValue = (float*)((char*)pPart + m_nOffset);

        static float value;
        static float value0;
        static float value_old;
        value_old = *pValue;
        GetFloatValue(fPreviousTime, pNode, value);
        if (m_nNumKeys != NODE_OFFSET_NONE)
        {
            value += *(float*)((char*)pPart->m_pNode + m_nNumKeys);
        }
        if (fTime < ANIMATION_TRANSITION_TIME_LIMIT)
        {
            *pValue = (1.0f - fBlend) * value_old + value * fBlend;
        }
        else
        {
            GetFloatValue(fTime, pNode, value0);
            *pValue = (1.0f - fBlend) * value0 + value * fBlend;
        }
        if (m_nOffset == OFFSET_SCALE)
        {
            MdlNodeTriMesh* pTriMesh = pPart->m_pNode->AsMdlNodeTriMesh();
            if (pTriMesh)
            {
                Model* pModel = pTriMesh->m_pModel->AsModel();
                if (pModel && (bool)pModel->m_Classification.m_bRenderEarly)
                    pTriMesh->m_bRender = true;
            }
        }
    }
    else if (nType == VALUE_TYPE_VECTOR)
    {
        Vector* pValue = (Vector*)((char*)pPart + m_nOffset);
        Vector value_old = *pValue;
        Vector value0;
        Vector value;
        Vector* pBaseValue;
        GetVectorValue(fPreviousTime, pNode, value);
        if (m_nNumKeys != NODE_OFFSET_NONE)
        {
            pBaseValue = (Vector*)((char*)pPart->m_pNode + m_nNumKeys);
            value += *pBaseValue;
        }
        if (fTime < ANIMATION_TRANSITION_TIME_LIMIT)
        {
            *pValue = Interpolate(value_old, value, fBlend);
        }
        else
        {
            GetVectorValue(fTime, pNode, value0);
            if (m_nNumKeys != NODE_OFFSET_NONE)
            {
                value0 += *pBaseValue;
            }
            *pValue = Interpolate(value0, value, fBlend);
        }
    }
    else if (nType == VALUE_TYPE_QUATERNION ||
             nType == VALUE_TYPE_COMPRESSED_QUATERNION)
    {
        Quaternion value;
        Quaternion* pValue = (Quaternion*)((char*)pPart + m_nOffset);
        Quaternion value0;
        Quaternion value_old = *pValue;
        GetQuaternionValue(fPreviousTime, pNode, value);
        if (fTime < ANIMATION_TRANSITION_TIME_LIMIT)
        {
            *pValue = Interpolate(value_old, value, fBlend);
        }
        else
        {
            GetQuaternionValue(fTime, pNode, value0);
            *pValue = Interpolate(value0, value, fBlend);
        }
    }
}
// PC 0x004B3F60 GOG 0x004B4070
void ProceduralAnimateHierarchy(Part* pPart)
{
    int i;
    PartTriMesh* pMesh = pPart->AsPartTriMesh();
    if (pMesh && pMesh->m_nTexCoordPool != RequestPoolManager::INVALID_POOL)
    {
        MdlNodeTriMesh* pNode = pMesh->m_pNode->AsMdlNodeTriMesh();
        if (pNode->m_bAnimateUV && pNode->m_nTexCoord0Offset != MdlNodeTriMesh::NO_VERTEX_ARRAY_OFFSET)
        {
            unsigned long nPool = pMesh->m_nTexCoordPool;
            if (nPool != RequestPoolManager::INVALID_POOL)
            {
                int nJitterRange = (int)(pNode->m_fUVJitter * 1000.0f);
                float fHalfJitter = pNode->m_fUVJitter * 0.5f;
                float* pTexCoords = (float*)GLRender::LockPool(nPool, 0, 0,
                    RequestPoolManager::POOL_LOCK_READ_WRITE);
                for (i = 0; i < pNode->m_nVertexCount; ++i)
                {
                    float* pOriginal = pMesh->m_pOriginalTexCoords + 2 * i;
                    pOriginal[0] += pNode->m_fUVDirectionX;
                    pOriginal[1] += pNode->m_fUVDirectionY;
                    if (nJitterRange != 0)
                    {
                        float fJitterU = ((rand() % nJitterRange) * 0.001f - fHalfJitter) * pNode->m_fUVJitterSpeed;
                        pTexCoords[2 * i] += fJitterU;
                        float fDelta = pTexCoords[2 * i] - pOriginal[0];
                        if (fDelta > fHalfJitter)
                            pTexCoords[2 * i] = pOriginal[0] + fHalfJitter;
                        else if (fDelta < -fHalfJitter)
                            pTexCoords[2 * i] = pOriginal[0] - fHalfJitter;

                        float fJitterV = ((rand() % nJitterRange) * 0.001f - fHalfJitter) * pNode->m_fUVJitterSpeed;
                        pTexCoords[2 * i + 1] += fJitterV;
                        fDelta = pTexCoords[2 * i + 1] - pOriginal[1];
                        if (fDelta > fHalfJitter)
                            pTexCoords[2 * i + 1] = pOriginal[1] + fHalfJitter;
                        else if (fDelta < -fHalfJitter)
                            pTexCoords[2 * i + 1] = pOriginal[1] - fHalfJitter;
                    }
                }

                pMesh->m_vPosition = pMesh->m_vPosition;
                pMesh->m_qOrientation = pMesh->m_qOrientation;
                nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
                unsigned char* pVertices = (unsigned char*)GLRender::LockPool(
                    nPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
                for (int j = 0; j < pNode->m_nVertexCount; ++j)
                {
                    unsigned char* pDest = pVertices + pNode->m_nVertexDataOffset +
                        j * pNode->m_nVertexStride + pNode->m_nTexCoord0Offset;
                    memcpy(pDest, &pTexCoords[2 * j], 2 * sizeof(float));
                }

                pPart->m_vPosition = pPart->m_vPosition;
                pPart->m_qOrientation = pPart->m_qOrientation;
                nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
                GLRender::UnlockPool(nPool);
                GLRender::UnlockPool(pMesh->m_nTexCoordPool);
            }
        }
    }
    for (i = 0; i < pPart->m_lstChildren.m_nCount; ++i)
        ProceduralAnimateHierarchy(pPart->m_lstChildren[i]);
}
// PC 0x004B46B0 GOG 0x004B47C0
void AnimateHierarchy(Part* pPart, MdlNode* pNode, float fTime,
                      float fPreviousTime, float fBlend, bool bAnimate)
{
    depth++;
    if (depth > realmaxdepth)
    {
        realmaxdepth = depth;
    }

    if (!bAnimate && pPart &&
        pPart->m_pNode == pPart->m_pNode->m_pModel->AsModel()->m_pBinaryData)
    {
        bAnimate = true;
    }
    if (!pPart->m_nInstanceState && bAnimate)
    {
        pNode->ControlDispatch(pPart, fTime, fPreviousTime, fBlend);
    }

    Gob* pGob = pPart->m_pAurObject;
    int nMaxDepth = Gob::MAX_ANIMATION_DEPTH;
    if (pGob && checkdepth && !pGob->IsInCutscene() &&
        pGob->m_bHierarchyAnimated)
    {
        float fScale = (pGob->m_fMinimumBoundingSphereRadius < 0.5f ? 0.5f :
                        pGob->m_fMinimumBoundingSphereRadius) / 1.6f;
        if (pGob->m_fCameraDistance > animdepthdist2 * fScale)
        {
            nMaxDepth = animdepth2;
        }
        else if (pGob->m_fCameraDistance > animdepthdist1 * fScale)
        {
            nMaxDepth = animdepth1;
        }
    }

    if (depth < maxdepth && depth < nMaxDepth)
    {
        for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        {
            if (pPart->m_lstChildren[i]->m_nNodeID == -1)
            {
                continue;
            }
            for (int j = 0; j < pNode->m_lChildren.m_nCount; j++)
            {
                int nChild = (j + i) % pNode->m_lChildren.m_nCount;
                if (pPart->m_lstChildren[i]->m_nNodeID == pNode->m_lChildren[nChild]->m_nNodeID)
                {
                    AnimateHierarchy(pPart->m_lstChildren[i], pNode->m_lChildren[nChild],
                                     fTime, fPreviousTime, fBlend, bAnimate);
                    break;
                }
            }
        }
    }
    depth--;
}
// PC 0x004B4B90 GOG 0x004B4CA0
void Part::Animate(float fTimeStep)
{
    for (int nChild = 0; nChild < m_lstChildren.m_nCount; ++nChild)
    {
        m_lstChildren[nChild]->Animate(fTimeStep);
    }
}
// PC 0x004B4BC0 GOG 0x004B4CD0
void PartDanglyMesh::Animate(float fTimeStep)
{
    if (renderdangly == 0 || currentscene == 0)
        return;

    if (fTimeStep > DANGLY_MAX_TIME_STEP)
        fTimeStep = DANGLY_MAX_TIME_STEP;
    else if (fTimeStep < DANGLY_MIN_TIME_STEP)
        fTimeStep = DANGLY_MIN_TIME_STEP;

    MdlNodeDanglyMesh* pNode = (MdlNodeDanglyMesh*)m_pNode;
    int i;
    static Vector tempCoord;
    static Vector nodevertex;
    static Vector vertex;
    static Vector currentvelocity;
    static Vector faccel;
    Vector vWorldPosition(0.0f, 0.0f, 0.0f);
    Quaternion qWorldOrientation;
    GetWorld(vWorldPosition, qWorldOrientation);
    VertexPrimitiveDangly* pPrimitive = m_pVertexPrimitive->AsVertexPrimitiveDangly();
    if (magnitude(vWorldPosition - m_vLastPosition) > DANGLY_RESET_DISTANCE)
    {
        for (i = 0; i < pNode->m_nVertexCount; ++i)
        {
            Vector* pVertex = &m_pVertexPositions[i];
            pVertex->m_fX = pNode->m_pVertexPositions[i].m_fX;
            pVertex->m_fY = pNode->m_pVertexPositions[i].m_fY;
            pVertex->m_fZ = pNode->m_pVertexPositions[i].m_fZ;
        }
    }
    else
    {
        if (m_qLastOrientation.m_fX == 0.0f &&
            m_qLastOrientation.m_fY == 0.0f &&
            m_qLastOrientation.m_fZ == 0.0f)
        {
            m_qLastOrientation = qWorldOrientation;
            m_vLastPosition = vWorldPosition;
        }
        Vector vWind = (-qWorldOrientation * currentscene->GetWindVector(vWorldPosition, fTimeStep)) * DANGLY_WIND_SCALE;
        Quaternion qRotation = -qWorldOrientation * m_qLastOrientation;
        Vector vTranslation = -m_qLastOrientation * (m_vLastPosition - vWorldPosition);
        float fTightness = DANGLY_TIGHTNESS_SCALE * pNode->m_fTightness * fTimeStep;
        for (i = 0; i < pNode->m_nVertexCount; ++i)
        {
            if (pNode->m_lConstraints.m_pList[i] == 0.0f)
                continue;
            Vector* pVertex = &m_pVertexPositions[i];
            nodevertex.m_fX = pNode->m_pVertexPositions[i].m_fX;
            nodevertex.m_fY = pNode->m_pVertexPositions[i].m_fY;
            nodevertex.m_fZ = pNode->m_pVertexPositions[i].m_fZ;
            vertex.m_fX = pVertex->m_fX;
            vertex.m_fY = pVertex->m_fY;
            vertex.m_fZ = pVertex->m_fZ;
            currentvelocity = pPrimitive->m_pOffsets[i];
            vertex = vTranslation + vertex;
            vertex = qRotation * vertex;
            currentvelocity = qRotation * currentvelocity;
            currentvelocity += (nodevertex - vertex) * (fTightness * pNode->m_lConstraints.m_pList[i])
                - currentvelocity * (DANGLY_PERIOD_SCALE * pNode->m_fPeriod * fTimeStep);
            pPrimitive->m_pOffsets[i] = currentvelocity;
            tempCoord = currentvelocity * fTimeStep;
            tempCoord += vertex;
            if (currentscene != 0)
                tempCoord += vWind * ((1.0f - pNode->m_lConstraints.m_pList[i] / DANGLY_MAX_CONSTRAINT) * pNode->m_fDisplacement);
            float fDisplacement = (1.0f - pNode->m_lConstraints.m_pList[i] / DANGLY_MAX_CONSTRAINT) * pNode->m_fDisplacement;
            float fX = tempCoord.m_fX - nodevertex.m_fX;
            float fY = tempCoord.m_fY - nodevertex.m_fY;
            float fZ = tempCoord.m_fZ - nodevertex.m_fZ;
            if (fX < -DANGLY_MAX_COORDINATE_DELTA || fY < -DANGLY_MAX_COORDINATE_DELTA || fZ < -DANGLY_MAX_COORDINATE_DELTA ||
                fX > DANGLY_MAX_COORDINATE_DELTA || fY > DANGLY_MAX_COORDINATE_DELTA || fZ > DANGLY_MAX_COORDINATE_DELTA)
            {
                *pVertex = nodevertex;
            }
            else
            {
                if (fX > fDisplacement)
                    tempCoord.m_fX = nodevertex.m_fX + fDisplacement;
                else if (fX < -fDisplacement)
                    tempCoord.m_fX = nodevertex.m_fX - fDisplacement;
                if (fY > fDisplacement)
                    tempCoord.m_fY = nodevertex.m_fY + fDisplacement;
                else if (fY < -fDisplacement)
                    tempCoord.m_fY = nodevertex.m_fY - fDisplacement;
                if (fZ > fDisplacement)
                    tempCoord.m_fZ = nodevertex.m_fZ + fDisplacement;
                else if (fZ < -fDisplacement)
                    tempCoord.m_fZ = nodevertex.m_fZ - fDisplacement;
                *pVertex = tempCoord;
            }
        }
    }
    unsigned long nPool = m_pNode->m_pModel->AsModel()->m_nVertexPool;
    unsigned char* pBuffer = (unsigned char*)GLRender::LockPool(
        nPool, 0, 0,
        RequestPoolManager::POOL_LOCK_WRITE);
    for (i = 0; i < pNode->m_nVertexCount; ++i)
    {
        *(Vector*)(pBuffer + pNode->m_nVertexDataOffset +
            i * pNode->m_nVertexStride + pNode->m_nPositionOffset) = m_pVertexPositions[i];
    }
    nPool = m_pNode->m_pModel->AsModel()->m_nVertexPool;
    GLRender::UnlockPool(nPool);
    m_vLastPosition = vWorldPosition;
    m_qLastOrientation = qWorldOrientation;
    Part::Animate(fTimeStep);
}
// PC 0x004B5B30 GOG 0x004B5C40
void Gob::AddAnimationFlag(int nFlag)
{
    int nIndex = m_lOutOfOrderAnimations.m_nCount - 1;
    if (nIndex >= 0)
    {
        m_lOutOfOrderAnimations[nIndex]->m_nFlags |= nFlag;
    }
}
// PC 0x004B5B60 GOG 0x004B5C70
void Gob::RemoveAnimationFlag(int nFlag)
{
    int nIndex = m_lOutOfOrderAnimations.m_nCount - 1;
    if (nIndex >= 0)
    {
        AnimRun* pRun = m_lOutOfOrderAnimations[nIndex];
        if (pRun->m_nFlags != 0 && nFlag != 0)
        {
            pRun->m_nFlags -= nFlag;
        }
    }
}
// PC 0x004B5B90 GOG 0x004B5CA0
AnimRun* CurrentAnimation(Gob* pGob)
{
    int nCount = pGob->m_lOutOfOrderAnimations.m_nCount;
    int nIndex;

    for (nIndex = 0; nIndex < nCount; ++nIndex)
    {
        if (!(pGob->m_lOutOfOrderAnimations[nIndex]->m_nFlags &
              AnimRun::FLAG_CURRENT_ANIMATION_PENDING) &&
            !(pGob->m_lOutOfOrderAnimations[nIndex]->m_nFlags &
              AnimRun::FLAG_CURRENT_ANIMATION_SUPPRESSED))
        {
            return pGob->m_lOutOfOrderAnimations[nIndex];
        }
    }

    return 0;
}
// PC 0x004B5BD0 GOG 0x004B5CE0
float FindAnimScale(Model* pModel, Animation* pAnimation)
{
    float fScale = 1.0f;

#ifdef _XBOX
    while (!pModel->m_lAnimations.Contains(pAnimation))
    {
        fScale *= pModel->m_fAnimationScale;
        pModel = pModel->m_pParentModel;
    }
#else
    while (1)
    {
        int nFound = 0;
        for (int i = 0; i < pModel->m_lAnimations.m_nCount; ++i)
        {
            if (pModel->m_lAnimations[i] == pAnimation)
            {
                ++nFound;
            }
        }

        if (nFound != 0)
        {
            break;
        }

        fScale *= pModel->m_fAnimationScale;
        pModel = pModel->m_pParentModel;
    }
#endif

    return fScale;
}
// PC 0x004B5C10 GOG 0x004B5D20
OOAnimRun* Gob::GetAnimation(char* sName, float* pfTime, float* pfLength)
{
    OOAnimRun* pRun = 0;
    for (int nRun = 0; nRun < m_lOutOfOrderAnimations.m_nCount; ++nRun)
    {
        if (stricmp(m_lOutOfOrderAnimations[nRun]->m_pAnimation->m_sName,
                    sName) == 0)
        {
            pRun = (OOAnimRun*)m_lOutOfOrderAnimations[nRun];
            break;
        }
    }
    if (pRun != 0)
    {
        if (pfTime != 0)
            *pfTime = pRun->m_fTime;
        if (pfLength != 0)
            *pfLength = pRun->m_pAnimation->m_fLength;
    }
    else
    {
        Animation* pAnimation = 0;
        if (m_pAnimationModel != 0)
            pAnimation = FindAnimation(m_pAnimationModel, sName);
        if (pAnimation == 0)
            pAnimation = FindAnimation(m_pModel, sName);
        if (pfTime != 0)
            *pfTime = ANIMATION_TIME_UNAVAILABLE;
        if (pfLength != 0)
        {
            if (pAnimation != 0)
                *pfLength = pAnimation->m_fLength;
            else
                *pfLength = ANIMATION_TIME_UNAVAILABLE;
        }
    }
    return pRun;
}
// PC 0x004B5FE0 GOG 0x004B60F0
Animation::~Animation()
{
    if (m_nType != 0)
    {
        m_nType = 0;
        if (m_lEvents.m_nCount != 0)
        {
            m_lEvents.SetSize(0);
        }
    }
}
// PC 0x004B6070 GOG 0x004B6180
void Gob::PlayOutOfOrderAnimation(char* sName, float* pfWeights,
                                 float* pfTimes, int nCount, float fLength)
{
    AuranimTrace3();
    Animation* pAnimation = FindAnimation(m_pModel, sName);
    if (pAnimation == 0)
    {
        return;
    }

    OOAnimRun* pRun = new OOAnimRun(pAnimation, pfWeights, pfTimes, nCount,
                                    fLength);
    pRun->m_fTime = 0.0f;
    pRun->m_fBlendWeight = 0.0f;
    pRun->m_fTransitionTime = pAnimation->m_fTransitionTime;
    pRun->m_nFlags = 0x209;
    m_lOutOfOrderAnimations.Insert(pRun, 0);
}
// PC 0x004B6130 GOG 0x004B6240
Animation::Animation(char* sName)
    : MaxTree(sName), m_lEvents(0)
{
    m_pBinaryData = 0;
    m_nType = MAXTREE_ANIMATION_ID;
    m_sAnimRoot[0] = 0;
    m_fLength = 1.0f;
    m_fTransitionTime = 0.25f;
    m_pfnParseField = AnimationParseField;
}
// PC 0x004B61A0 GOG 0x004B62B0
void NewController::ReportKeys(Part* pPart, MdlNode* pNode,
                               float fStartTime, float fEndTime)
{
    AuranimTrace3();
    if (pPart->m_pAurObject == 0)
        return;

    int nKeyIndex = (unsigned short)m_nKeyIndex;
    float** ppfData = &pNode->m_lstControllerData.m_pList;
    if (fStartTime < 0.0f)
        fStartTime = 0.0f;
    if (fEndTime > AnimRun::m_AnimLength)
        fEndTime = AnimRun::m_AnimLength;

    SWMGReportKeyData report;
    report.m_pPosition = 0;
    report.m_pOrientation = 0;
    report.m_bPositionKey = true;
    float fDelta = 0.0f;
    float fReportedTime = 0.0f;
    int nStartKey = 0;
    int nEndKey = 0;
    for (; nStartKey < (unsigned short)m_nRows; ++nStartKey)
    {
        if (!(fStartTime > (*ppfData)[nStartKey + nKeyIndex]))
            break;
    }
    for (; nEndKey < (unsigned short)m_nRows; ++nEndKey)
    {
        if (!(fEndTime > (*ppfData)[nEndKey + nKeyIndex]))
            break;
    }

    if (!(AnimRun::m_AnimFlags & AnimRun::FLAG_SUPPRESS_KEY_REPORTS))
    {
        if (fStartTime < fEndTime)
        {
            for (int nKey = nStartKey; nKey < nEndKey; ++nKey)
            {
                if (nKey == nStartKey)
                    fDelta = (*ppfData)[nKey + nKeyIndex] - fStartTime;
                else
                    fDelta = (*ppfData)[nKey + nKeyIndex] -
                             (*ppfData)[nKey + nKeyIndex - 1];
                ReportKey(pPart, pNode, nKey, fDelta);
                fReportedTime += fDelta;
            }
        }
        else
        {
            float fWrapTime;
            if (nStartKey == (unsigned short)m_nRows)
                fWrapTime = AnimRun::m_AnimLength - fStartTime;
            else
                fWrapTime = AnimRun::m_AnimLength -
                            (*ppfData)[(unsigned short)m_nRows + nKeyIndex - 1];

            for (int nKey = nStartKey; nKey < (unsigned short)m_nRows; ++nKey)
            {
                if (nKey == nStartKey && nKey < (unsigned short)m_nRows)
                    fDelta = (*ppfData)[nKey] - fStartTime;
                else
                    fDelta = (*ppfData)[nKey + nKeyIndex] -
                             (*ppfData)[nKey + nKeyIndex - 1];
                ReportKey(pPart, pNode, nKey, fDelta);
                fReportedTime += fDelta;
            }
            for (int nKey = 0; nKey < nEndKey; ++nKey)
            {
                if (nKey == 0)
                    fDelta = fWrapTime + (*ppfData)[nKeyIndex];
                else
                    fDelta = (*ppfData)[nKey + nKeyIndex] -
                             (*ppfData)[nKey + nKeyIndex - 1];
                ReportKey(pPart, pNode, nKey, fDelta);
                fReportedTime += fDelta;
            }
        }
        report.m_fTime = DeltaT - fReportedTime;
        report.m_pOrientation = 0;
        report.m_pPosition = 0;
        report.m_bPositionKey = (m_nValueFormat & VALUE_TYPE_MASK) == VALUE_TYPE_VECTOR;
        pPart->m_pAurObject->DoEvent("ReportKey", &report);
    }
}
// PC 0x004B6420 GOG 0x004B6530
void MdlNode::InternalControl(Part* pPart, float fTime,
                             float fPreviousTime, float fBlend)
{
    pPart->AsPartTriMesh();
    for (int nController = 0; nController < m_lstControllers.m_nCount; ++nController)
    {
        m_lstControllers.m_pList[nController].Control(pPart, this, fTime,
                                               fPreviousTime, fBlend);
        if (AnimRun::m_pTraceRun != 0)
        {
            if (fTime < ANIMATION_TRANSITION_TIME_LIMIT)
                m_lstControllers.m_pList[nController].ReportKeys(
                    pPart, this, ANIMATION_TRANSITION_TIME_ORIGIN - fTime,
                    fPreviousTime);
            else
                m_lstControllers.m_pList[nController].ReportKeys(
                    pPart, this, fTime, fPreviousTime);
        }
    }
}
// PC 0x004B64B0 GOG 0x004B65C0
void MdlNodeLight::InternalControl(Part* pPart, float fTime,
                                   float fPreviousTime, float fBlend)
{
    PartLight* pLight = pPart->AsPartLight();
    MdlNode::InternalControl(pPart, fTime, fPreviousTime, fBlend);
}
// PC 0x004B64E0 GOG 0x004B65F0
void MdlNodeEmitter::InternalControl(Part* pPart, float fTime,
                                     float fPreviousTime, float fBlend)
{
    PartEmitter* pEmitter = pPart->AsPartEmitter();
    MdlNode::InternalControl(pPart, fTime, fPreviousTime, fBlend);
    pEmitter->CalculateRadius();
}
// PC 0x004B6520 GOG 0x004B6630
void MdlNodeCamera::InternalControl(Part* pPart, float fTime,
                                    float fPreviousTime, float fBlend)
{
    PartCamera* pCamera = pPart->AsPartCamera();
    MdlNode::InternalControl(pPart, fTime, fPreviousTime, fBlend);
}
// PC 0x004B6550 GOG 0x004B6660
void MdlNodeReference::InternalControl(Part* pPart, float fTime,
                                       float fPreviousTime, float fBlend)
{
    PartReference* pReference = pPart->AsPartReference();
    MdlNode::InternalControl(pPart, fTime, fPreviousTime, fBlend);
}
// PC 0x004B6580 GOG 0x004B6690
void MdlNodeTriMesh::InternalControl(Part* pPart, float fTime,
                                     float fPreviousTime, float fBlend)
{
    PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
    MdlNode::InternalControl(pPart, fTime, fPreviousTime, fBlend);
}
// PC 0x004B65B0 GOG 0x004B66C0
void MdlNodeAnimMesh::InternalControl(Part* pPart, float fTime,
                                      float fPreviousTime, float fBlend)
{
    PartAnimMesh* pAnimMesh = pPart->AsPartAnimMesh();
    int nFrame;
    if (m_fSamplePeriod != 0.0f)
        nFrame = (int)(fPreviousTime / m_fSamplePeriod);
    else
        nFrame = 0;

    if (nFrame >= 0 && nFrame < m_nAnimationDataCount1 - 1)
    {
        float fAlpha = fPreviousTime / m_fSamplePeriod - nFrame;
        VertexPrimitiveAnim* pPrimitive = pAnimMesh->m_pVertexPrimitive->AsVertexPrimitiveAnim();
        Vector* pVertices = (Vector*)GLRender::LockPool(pPrimitive->m_nVertexPool, 0, 0,
                                                      RequestPoolManager::POOL_LOCK_WRITE);
        Vector vFirst, vSecond, vNew;
        for (int i = 0; i < m_nVertexCount; i++)
        {
            Vector vOld;
            vOld.m_fX = pVertices[i].m_fX;
            vOld.m_fY = pVertices[i].m_fY;
            vOld.m_fZ = pVertices[i].m_fZ;
            int nIndex = (i * m_nAnimationDataCount1 + nFrame) * 3;
            vFirst.m_fX = ((float*)m_pAnimationData1)[nIndex];
            vFirst.m_fY = ((float*)m_pAnimationData1)[nIndex + 1];
            vFirst.m_fZ = ((float*)m_pAnimationData1)[nIndex + 2];
            vSecond.m_fX = ((float*)m_pAnimationData1)[nIndex + 3];
            vSecond.m_fY = ((float*)m_pAnimationData1)[nIndex + 4];
            vSecond.m_fZ = ((float*)m_pAnimationData1)[nIndex + 5];
            vNew = Interpolate(vFirst, vSecond, fAlpha);
            vOld = Interpolate(vOld, vNew, fBlend);
            pVertices[i].m_fX = vOld.m_fX;
            pVertices[i].m_fY = vOld.m_fY;
            pVertices[i].m_fZ = vOld.m_fZ;
        }
        GLRender::UnlockPool(pPrimitive->m_nVertexPool);
    }
    else if (m_nAnimationDataCount1 != 0)
    {
        VertexPrimitiveAnim* pPrimitive = pAnimMesh->m_pVertexPrimitive->AsVertexPrimitiveAnim();
        unsigned long nPool = pPrimitive->m_nVertexPool;
        Vector* pVertices = (Vector*)GLRender::LockPool(nPool, 0, 0,
                                                      RequestPoolManager::POOL_LOCK_WRITE);
        if (nFrame < 0)
            nFrame = 0;
        if (nFrame > m_nAnimationDataCount1 - 1)
            nFrame = m_nAnimationDataCount1 - 1;
        Vector vNew;
        for (int i = 0; i < m_nVertexCount; i++)
        {
            float* pData = (float*)m_pAnimationData1;
            Vector vOld;
            vOld.m_fX = pVertices[i].m_fX;
            vOld.m_fY = pVertices[i].m_fY;
            vOld.m_fZ = pVertices[i].m_fZ;
            int nIndex = (i * m_nAnimationDataCount1 + nFrame) * 3;
            vNew.m_fX = pData[nIndex];
            vNew.m_fY = pData[nIndex + 1];
            vNew.m_fZ = pData[nIndex + 2];
            vOld = Interpolate(vOld, vNew, fBlend);
            pVertices[i].m_fX = vOld.m_fX;
            pVertices[i].m_fY = vOld.m_fY;
            pVertices[i].m_fZ = vOld.m_fZ;
        }
        GLRender::UnlockPool(pPrimitive->m_nVertexPool);
    }

    if (m_nAnimationDataCount2 != 0)
    {
        VertexPrimitiveAnim* pPrimitive = pAnimMesh->m_pVertexPrimitive->AsVertexPrimitiveAnim();
        float* pTexCoords = (float*)GLRender::LockPool(pPrimitive->m_nTexCoordPool, 0, 0,
                                                    RequestPoolManager::POOL_LOCK_WRITE);
        if (nFrame >= 0 && nFrame < m_nAnimationDataCount2 - 1)
        {
            float fAlpha = fPreviousTime / m_fSamplePeriod - nFrame;
            Vector vFirst, vSecond;
            for (int i = 0; i < m_nVertexCount; i++)
            {
                Vector vOld(pTexCoords[i * 2], pTexCoords[i * 2 + 1], 0.0f);
                int nIndex = (i << 1) * m_nAnimationDataCount2 + (nFrame << 1);
                vFirst.m_fX = ((float*)m_pAnimationData2)[nIndex];
                vFirst.m_fY = ((float*)m_pAnimationData2)[nIndex + 1];
                vSecond.m_fX = ((float*)m_pAnimationData2)[nIndex + 2];
                vSecond.m_fY = ((float*)m_pAnimationData2)[nIndex + 3];
                Vector vNew = Interpolate(vFirst, vSecond, fAlpha);
                vOld = Interpolate(vOld, vNew, fBlend);
                pTexCoords[i * 2] = vOld.m_fX;
                pTexCoords[i * 2 + 1] = vOld.m_fY;
            }
        }
        else if (m_nAnimationDataCount2 != 0)
        {
            int nTexFrame = nFrame > 0 ? nFrame : 0;
            if (nTexFrame >= m_nAnimationDataCount2 - 1)
                nTexFrame = m_nAnimationDataCount2 - 1;
            Vector vNew;
            for (int i = 0; i < m_nVertexCount; i++)
            {
                Vector vOld(pTexCoords[i * 2], pTexCoords[i * 2 + 1], 0.0f);
                int nIndex = (i << 1) * m_nAnimationDataCount2 + (nTexFrame << 1);
                vNew.m_fX = ((float*)m_pAnimationData2)[nIndex];
                vNew.m_fY = ((float*)m_pAnimationData2)[nIndex + 1];
                vOld = Interpolate(vOld, vNew, fBlend);
                pTexCoords[i * 2] = vOld.m_fX;
                pTexCoords[i * 2 + 1] = vOld.m_fY;
            }
        }
        GLRender::UnlockPool(pPrimitive->m_nTexCoordPool);
    }
    MdlNodeTriMesh::InternalControl(pPart, fTime, fPreviousTime, fBlend);
}
// PC 0x004B6B20 GOG 0x004B6C30
int Gob::PlayAnimation(char* sName, float fSpeed, int nFlags, float fTime)
{
    AuranimTrace3();
    if (sName == 0 || *sName == 0 || stricmp("NULL", sName) == 0)
    {
        for (int i = m_lOutOfOrderAnimations.m_nCount; i != 0; )
        {
            --i;
            if (m_pLookAtAnimation != 0 &&
                m_pLookAtAnimation->m_pOutOfOrderRun == m_lOutOfOrderAnimations[i])
            {
                m_pLookAtAnimation->m_pOutOfOrderRun = 0;
                delete m_pLookAtAnimation;
                m_pLookAtAnimation = 0;
            }
            delete m_lOutOfOrderAnimations[i];
        }
        m_lOutOfOrderAnimations.m_nCount = 0;
        return 1;
    }

    AnimRun* pRun = 0;
    if (!(nFlags & ANIMATION_FLAG_RESTART))
    {
        for (int i = m_lOutOfOrderAnimations.m_nCount - 1; i >= 0; --i)
        {
            if (stricmp(m_lOutOfOrderAnimations[i]->m_pAnimation->m_sName, sName) == 0)
            {
                pRun = m_lOutOfOrderAnimations[i];
                if (nFlags & ANIMATION_FLAG_STOP_OUT_OF_ORDER)
                {
                    pRun->m_nFlags |= ANIMATION_FLAG_STOP_OUT_OF_ORDER | ANIMATION_FLAG_NO_LOOP;
                    return 1;
                }
                break;
            }
        }
    }
    if (pRun == 0 && stricmp(sName, "current") == 0)
        pRun = CurrentAnimation(this);
    if (pRun != 0)
        m_lOutOfOrderAnimations.Remove(pRun);

    if (nFlags & ANIMATION_FLAG_STOP)
    {
        if (pRun == 0)
            return 0;
        char sEvent[64];
        sprintf(sEvent, "done%s", pRun->m_pAnimation->m_sName);
        DoEvent(sEvent, 0);
        if (m_pLookAtAnimation != 0 && m_pLookAtAnimation->m_pOutOfOrderRun == pRun)
        {
            m_pLookAtAnimation->m_pOutOfOrderRun = 0;
            delete m_pLookAtAnimation;
            m_pLookAtAnimation = 0;
        }
        delete pRun;
        return 1;
    }

    Animation* pAnimation = 0;
    Model* pModel;
    if (m_pAnimationModel != 0)
    {
        pAnimation = FindAnimation(m_pAnimationModel, sName);
        pModel = m_pAnimationModel;
    }
    if (pAnimation == 0)
    {
        pAnimation = FindAnimation(m_pModel, sName);
        pModel = m_pModel;
    }
    if (fTime < 0.0f)
        fTime = 0.0f;
    if (pAnimation != 0)
    {
        while (fTime > pAnimation->m_fLength)
            fTime -= pAnimation->m_fLength;
    }
    if (pRun == 0)
    {
        if (pAnimation == 0)
            return 0;
        pRun = new AnimRun(pAnimation);
        if (fSpeed == 0.0f)
            pRun->m_fTime = 0.0f;
        if (fSpeed < 0.0f)
            pRun->m_fTime = pAnimation->m_fLength + ANIMATION_EVENT_TIME_EPSILON;
        if (fSpeed > 0.0f)
            pRun->m_fTime = -ANIMATION_EVENT_TIME_EPSILON;
        if (fTime != 0.0f)
            pRun->m_fTime = fTime;
        if (!(nFlags & ANIMATION_FLAG_OUT_OF_PLACE))
        {
            pRun->m_fBlendWeight = 0.0f;
            pRun->m_fTransitionTime = pRun->m_pAnimation->m_fTransitionTime;
        }
    }
    pRun->m_fScale = FindAnimScale(pModel, pAnimation);
    pRun->m_bDone = 0;
    pRun->m_fSpeed = fSpeed;
    pRun->m_nFlags = nFlags;
    int nIndex = 0;
    if (nFlags & AnimRun::FLAG_CURRENT_ANIMATION_PENDING)
    {
        while (nIndex < m_lOutOfOrderAnimations.m_nCount)
        {
            if (!(m_lOutOfOrderAnimations[nIndex]->m_nFlags & AnimRun::FLAG_OUT_OF_ORDER))
                break;
            ++nIndex;
        }
    }
    else
    {
        while (nIndex < m_lOutOfOrderAnimations.m_nCount)
        {
            int nRunFlags = m_lOutOfOrderAnimations[nIndex]->m_nFlags;
            if (!(nRunFlags & AnimRun::FLAG_CURRENT_ANIMATION_PENDING) &&
                ((nFlags & ANIMATION_FLAG_QUEUE) || !(nRunFlags & ANIMATION_FLAG_QUEUE)))
                break;
            ++nIndex;
        }
    }
    m_lOutOfOrderAnimations.Insert(pRun, nIndex);
    if ((nFlags & ANIMATION_FLAG_SYNCHRONIZE) &&
        m_lOutOfOrderAnimations.m_nCount > nIndex + 1)
    {
        pRun->m_fTime = m_lOutOfOrderAnimations[nIndex + 1]->m_fTime;
        return 1;
    }
    if ((nFlags & AnimRun::FLAG_CURRENT_ANIMATION_PENDING) || (nFlags & ANIMATION_FLAG_QUEUE))
        return 1;
    char sEvent[256];
    while (m_lOutOfOrderAnimations.m_nCount > nIndex + 1)
    {
        AnimRun* pOldRun = m_lOutOfOrderAnimations[nIndex + 1];
        sprintf(sEvent, "done%s", pOldRun->m_pAnimation->m_sName);
        m_lOutOfOrderAnimations.Remove(pOldRun);
        DoEvent(sEvent, 0);
        delete pOldRun;
    }
    sprintf(sEvent, "start%s", pRun->m_pAnimation->m_sName);
    DoEvent(sEvent, 0);
    return 1;
}
float FindAnimScale(Model* pModel, Animation* pAnimation);
// PC 0x004B6FE0 GOG 0x004B70F0
LookAtAnimation::~LookAtAnimation()
{
    int nIndex = m_pGob->m_lOutOfOrderAnimations.m_nCount;
    while (nIndex != 0)
    {
        if (m_pGob->m_lOutOfOrderAnimations[--nIndex] == m_pOutOfOrderRun)
        {
            m_pGob->m_lOutOfOrderAnimations.DelIndex(nIndex);
        }
    }
}
// PC 0x004B7090 GOG 0x004B71A0
void LookAtAnimation::BuildAnimation()
{
    AuranimTrace4();
    m_pTree = new Animation("LookAtAnimation");
    MdlNode* pLooking = new MdlNode("looking");
    pLooking->m_nNodeID = m_pHeadPart->m_nNodeID;
    MdlNode* pRoot = pLooking;
    Part* pPart = m_pHeadPart;
    while (pPart->m_pParent != 0)
    {
        pPart = pPart->m_pParent;
        MdlNode* pChild = pRoot;
        MdlNode* pParent = new MdlNode("lookingParent");
        pRoot->m_pParentNode = pParent;
        pRoot = pParent;
        pParent->m_lChildren.Add(pChild);
        pParent->m_nNodeID = pPart->m_nNodeID;
    }
    pRoot->m_pParentNode = 0;
    pRoot->m_pModel = m_pTree;
    m_pTree->m_pRootNode = pRoot;

    NewController controller;
    controller.m_nKeyIndex = pLooking->m_lstControllerData.m_nCount;
    pLooking->m_lstControllerData.Add(0.0f);
    pLooking->m_lstControllerData.Add(LOOKAT_KEY_END_TIME);
    unsigned short nOrientationIndex = pLooking->m_lstControllerData.m_nCount;
    controller.m_nColumnIndex = nOrientationIndex;
    pLooking->m_lstControllerData.Add(0.0f);
    pLooking->m_lstControllerData.Add(0.0f);
    pLooking->m_lstControllerData.Add(0.0f);
    pLooking->m_lstControllerData.Add(1.0f);
    pLooking->m_lstControllerData.Add(0.0f);
    pLooking->m_lstControllerData.Add(0.0f);
    pLooking->m_lstControllerData.Add(0.0f);
    pLooking->m_lstControllerData.Add(1.0f);
    controller.m_nValueFormat = NewController::VALUE_TYPE_QUATERNION;
    controller.m_nRows = 2;
    controller.m_nOffset = NewController::OFFSET_ORIENTATION;
    pLooking->m_lstControllers.Add(controller);
    m_pStartOrientation = &pLooking->m_lstControllerData[nOrientationIndex];
    m_pEndOrientation = &pLooking->m_lstControllerData[nOrientationIndex + 4];
    ((Animation*)m_pTree)->m_fLength = m_fDuration;
    ((Animation*)m_pTree)->m_fTransitionTime = m_fDuration;
}
#ifndef _XBOX
// PC 0x004B74B0 GOG 0x004B75C0
void Gob::DoEvent(char* sEvent, void* pData)
{
    int nIndex = m_lCallBacks.m_nCount - 1;
    SafePointer<CAurObject> pObject(this);
    while (pObject && nIndex >= 0)
    {
        CallBack* pCallBack = m_lCallBacks[nIndex];
        if ((pCallBack->m_sEvent[0] == 0 ||
             pCallBack->m_sEvent[0] == '*' ||
             stricmp(pCallBack->m_sEvent, sEvent) == 0) &&
            (!checkevents || pCallBack->m_fDelay > m_fCameraDistance ||
             (pCallBack->m_bRepeat && IsInViewVolume())))
        {
            pCallBack->m_pfnCallBack(this, sEvent,
                                   pData ? pData : pCallBack->m_pData);
        }
        --nIndex;
    }
}
#endif
void Gob::Animate(float fTimeStep)  // PC 0x004B75D0 GOG 0x004B76E0
{
    ProcessGob();
    if (!enablegobanimations)
        return;
    if (m_pScene && m_nLastAnimationFrame == m_pScene->m_nAnimationFrame)
        return;
    if (m_pScene)
        m_nLastAnimationFrame = m_pScene->m_nAnimationFrame;

    if (m_bWind && m_pScene && m_pScene->m_pWindManager)
    {
        float fRadius = m_fWindX == 0.0f ? 1.2f : m_fWindX;
        float fStrength;
        if (m_fWindZ == 0.0f)
            fStrength = magnitude(m_vPosition - m_vPreviousPosition) < 1.0f ?
                magnitude(m_vPosition - m_vPreviousPosition) : 1.0f;
        else
            fStrength = m_fWindZ;
        m_pScene->m_pWindManager->AddPointSourceWind(m_vPosition, fRadius, fStrength, m_fWindY);
    }

    SafePointer<CAurObject> pSafeObject(this);
    if (m_pBehavior)
    {
        m_pBehavior->Control(fTimeStep);
        if (pSafeObject == 0)
            return;
    }
    if (fTimeStep <= 0.0f && !IsRenderEarly())
        return;

    if (!m_pScene || disablegobdist)
        m_fCameraDistance = 0.0f;
    else
    {
        float fX = AbsoluteValue(m_vPosition.m_fX - m_pScene->m_vSceneFocus.m_fX);
        float fY = AbsoluteValue(m_vPosition.m_fY - m_pScene->m_vSceneFocus.m_fY);
        m_fCameraDistance = fX > fY ? fX : fY;
    }
    bool bAnimateHierarchy = true;
    if (m_pScene && !m_pScene->IsAnimating())
    {
        if (!IsForceAnimated() && !IsRenderEarly())
            return;
        if (IsRenderEarly() && !IsForceAnimated())
            bAnimateHierarchy = false;
    }
    if (m_pRootPart && m_bAnimateUV == 1)
        ProceduralAnimateHierarchy(m_pRootPart);

    if (m_pLookAtAnimation)
    {
        if (m_pLookAtAnimation->IsActive())
            m_pLookAtAnimation->Control();
        else
        {
            int nRun = m_lOutOfOrderAnimations.m_nCount;
            MaxTree* pTree = m_pLookAtAnimation->m_pTree;
            while (nRun != 0)
            {
                if (m_lOutOfOrderAnimations[--nRun]->m_pAnimation == pTree)
                    m_lOutOfOrderAnimations.DelIndex(nRun);
            }
            AnimRun* pRun = m_pLookAtAnimation->m_pOutOfOrderRun;
            if (pRun)
            {
                delete pRun;
                m_pLookAtAnimation->m_pOutOfOrderRun = 0;
            }
            delete m_pLookAtAnimation;
            m_pLookAtAnimation = 0;
        }
    }

    if (m_pModel && !IsInCutscene() && !m_bFirstAnimate && !m_bHasDynamicLights &&
        !char(m_pModel->m_Classification.m_bEmitterBounds) &&
        !char(m_pModel->m_Classification.m_bSkipVisibilityCheck) &&
        !IsAlwaysRendering() && !char(m_pModel->m_Classification.m_bDoor) &&
        !IsInViewVolume() && !char(m_pModel->m_Classification.m_bRenderEarly))
    {
        if (IsInViewVolume() || !IsCharacter())
            return;
        bAnimateHierarchy = false;
    }
    if (m_n1F8)
        bAnimateHierarchy = false;
    int nRun = m_lOutOfOrderAnimations.m_nCount;
    m_bFirstAnimate = false;
    if (nRun > 0)
        m_bAnimating = true;
    else
        m_bAnimating = false;
    if (nRun != 0 &&
        (m_lOutOfOrderAnimations[nRun - 1]->m_nFlags & AnimRun::FLAG_CURRENT_ANIMATION_SUPPRESSED))
    {
        m_lOutOfOrderAnimations[nRun - 1]->m_nFlags -= AnimRun::FLAG_CURRENT_ANIMATION_SUPPRESSED;
        char sEvent[64];
        sprintf(sEvent, "start%s", m_lOutOfOrderAnimations[nRun - 1]->m_pAnimation->m_sName);
        DoEvent(sEvent, 0);
    }
    while (pSafeObject != 0 && nRun-- != 0)
    {
        AnimRun* pRun = m_lOutOfOrderAnimations[nRun];
        bool bLastKey = false;
        if (!(pRun->m_nFlags & AnimRun::FLAG_CURRENT_ANIMATION_SUPPRESSED))
        {
            pRun->m_fPreviousTime = pRun->m_fTime;
            pRun->m_fTime += fTimeStep * pRun->m_fSpeed;
            if (pRun->m_nFlags & AnimRun::FLAG_STOP_AT_END)
            {
                if (pRun->m_bDone)
                {
                    char sEvent[64];
                    sprintf(sEvent, "done%s", pRun->m_pAnimation->m_sName);
                    m_lOutOfOrderAnimations.Remove(pRun);
                    DoEvent(sEvent, 0);
                    if (pRun->m_nFlags & AnimRun::FLAG_OUT_OF_ORDER)
                    {
                        OOAnimRun* pOutOfOrder = (OOAnimRun*)pRun;
#ifdef _XBOX
                        delete pOutOfOrder;
#else
                        delete[] pOutOfOrder->m_pDeltaValues; delete[] pOutOfOrder->m_pStartValues; pOutOfOrder->m_pDeltaValues = 0; pOutOfOrder->m_pStartValues = 0; delete pRun;
#endif
                    }
                    else
                        delete pRun;
                    continue;
                }
                int bAtEnd = 0;
                if (pRun->m_fTime < 0.0f)
                {
                    pRun->m_fTime = 0.0f;
                    bAtEnd = 1;
                }
                else if (pRun->m_nFlags & AnimRun::FLAG_OUT_OF_ORDER)
                {
                    OOAnimRun* pOutOfOrder = (OOAnimRun*)pRun;
                    if (pRun->m_fTime > pOutOfOrder->m_fBlendTime)
                    {
                        pRun->m_fTransitionTime = 0.5f;
                        pRun->m_fTime = pOutOfOrder->m_fBlendTime;
                        bLastKey = true;
                        bAtEnd = 1;
                    }
                }
                else if (pRun->m_fTime > pRun->m_pAnimation->m_fLength)
                {
                    pRun->m_fTime = pRun->m_pAnimation->m_fLength;
                    bAtEnd = 1;
                }

                if ((pRun->m_nFlags & AnimRun::FLAG_BLEND_OUT) || bAtEnd)
                {
                    if (nRun > 0 &&
                        (m_lOutOfOrderAnimations[nRun - 1]->m_nFlags & AnimRun::FLAG_CURRENT_ANIMATION_SUPPRESSED))
                    {
                        m_lOutOfOrderAnimations[nRun - 1]->m_nFlags -= AnimRun::FLAG_CURRENT_ANIMATION_SUPPRESSED;
                        char sEvent[64];
                        sprintf(sEvent, "start%s", m_lOutOfOrderAnimations[nRun - 1]->m_pAnimation->m_sName);
                        DoEvent(sEvent, 0);
                    }
                    float fBlend = pRun->m_fTransitionTime <= 0.0f ? 2.0f :
                        2.0f * (fTimeStep / pRun->m_fTransitionTime);
                    pRun->m_fBlendWeight -= fBlend;
                    if (pRun->m_fBlendWeight <= 0.0f)
                    {
                        pRun->m_fBlendWeight = 0.0f;
                        pRun->m_bDone = 1;
                        if (pRun->m_fTransitionTime == 0.0f)
                            pRun->m_fBlendWeight = 1.0f;
                    }
                }
            }
            else
            {
                if (pRun->m_fTime < 0.0f && pRun->m_fSpeed <= 0.0f)
                {
                    if (pRun->m_nFlags & AnimRun::FLAG_REPORT_LOOP_EVENTS)
                    {
                        char sEvent[64];
                        sprintf(sEvent, "loop%s", pRun->m_pAnimation->m_sName);
                        DoEvent(sEvent, 0);
                    }
                    pRun->m_fTime += pRun->m_pAnimation->m_fLength;
                    if (pRun->m_fTime < 0.0f)
                        pRun->m_fTime = 0.0f;
                }
                if (pRun->m_fTime > pRun->m_pAnimation->m_fLength && pRun->m_fSpeed >= 0.0f)
                {
                    if (pRun->m_nFlags & AnimRun::FLAG_REPORT_LOOP_EVENTS)
                    {
                        char sEvent[64];
                        sprintf(sEvent, "loop%s", pRun->m_pAnimation->m_sName);
                        DoEvent(sEvent, 0);
                    }
                    pRun->m_fTime -= pRun->m_pAnimation->m_fLength;
                    if (pRun->m_fTime > pRun->m_pAnimation->m_fLength)
                        pRun->m_fTime = pRun->m_pAnimation->m_fLength;
                }
            }

            if (!pRun->m_bDone && pRun->m_fBlendWeight < 1.0f)
            {
                pRun->m_fBlendWeight += pRun->m_fTransitionTime <= 0.0f ? 1.0f :
                    fTimeStep / pRun->m_fTransitionTime;
                if (pRun->m_fBlendWeight > 1.0)
                {
                    pRun->m_fBlendWeight = 1.0f;
                    if (!(pRun->m_nFlags & AnimRun::FLAG_RETAIN_OLDER_RUNS))
                    {
                        while (m_lOutOfOrderAnimations.m_nCount > nRun + 1)
                        {
                            AnimRun* pOldRun = m_lOutOfOrderAnimations[nRun + 1];
                            m_lOutOfOrderAnimations.Remove(pOldRun);
                            delete pOldRun;
                        }
                    }
                }
            }

            int nEventFlags = pRun->m_fSpeed < 0.0f;
            position_scale_factor = pRun->m_fScale;
            if (pRun->m_nFlags & AnimRun::FLAG_REPORT_KEYS)
            {
                AnimRun::m_pTraceRun = pRun;
                AnimRun::m_AnimFlags = nEventFlags;
                AnimRun::m_AnimLength = pRun->m_pAnimation->m_fLength;
            }
            if (pRun->m_nFlags & AnimRun::FLAG_OUT_OF_ORDER)
            {
                OOAnimRun* pOutOfOrder = (OOAnimRun*)pRun;
                float fPosition = pRun->m_fTime / pOutOfOrder->m_fBlendTime;
                int nKey = 0;
                while (fPosition > pOutOfOrder->m_pStartValues[nKey] &&
                       nKey < pOutOfOrder->m_nNumValues)
                    nKey++;
                float fBlend = nKey == pOutOfOrder->m_nNumValues ? 0.0f :
                    (fPosition - pOutOfOrder->m_pStartValues[nKey - 1]) /
                    (pOutOfOrder->m_pStartValues[nKey] - pOutOfOrder->m_pStartValues[nKey - 1]);
                if (bAnimateHierarchy)
                {
                    if (bLastKey)
                    {
                        AnimateHierarchy(m_pRootPart, pRun->m_pAnimation->m_pRootNode,
                            ANIMATION_TRANSITION_TIME_ORIGIN, pOutOfOrder->m_pDeltaValues[nKey],
                            pRun->m_fBlendWeight, false);
                        talkalpha = pRun->m_fBlendWeight;
                    }
                    else
                        AnimateHierarchy(m_pRootPart, pRun->m_pAnimation->m_pRootNode,
                            pOutOfOrder->m_pDeltaValues[nKey - 1], pOutOfOrder->m_pDeltaValues[nKey],
                            fBlend, false);
                    m_bHierarchyAnimated = true;
                }
            }
            else if (bAnimateHierarchy)
            {
                AnimateHierarchy(m_pRootPart, pRun->m_pAnimation->m_pRootNode,
                    ANIMATION_TRANSITION_TIME_ORIGIN - pRun->m_fPreviousTime,
                    pRun->m_fTime, pRun->m_fBlendWeight, false);
                m_bHierarchyAnimated = true;
            }
            AnimateEvents(this, pRun->m_pAnimation, pRun->m_fPreviousTime, pRun->m_fTime, nEventFlags);
            AnimRun::m_pTraceRun = 0;
            AnimRun::m_AnimFlags = 0;
            AnimRun::m_AnimLength = 0.0f;
        }
    }
    if (pSafeObject == 0)
        return;

    if (m_pRootPart && ((m_pScene && (m_bAnimating || !testanimationshortcircuit)) ||
                       IsRenderEarly()))
    {
        m_pRootPart->Animate(fTimeStep);
        if (rebuildbboxes)
            GetMinimumBoundingBox();
        if (rebuildspheres)
            GetMinimumSphere();
    }
}

// PC 0x004B7FA0 GOG 0x004B80B0
LookAtAnimation::LookAtAnimation(CAurObject* pTarget, Part* pPart, Gob* pGob,
                                 Part* pHeadPart, float fHorizontalLimit,
                                 float fVerticalLimit, float fDuration,
                                 float fMaxDelta)
    : m_pTarget()
{
    m_pTarget = pTarget;
    m_pPart = pPart;
    m_pGob = pGob;
    m_pHeadPart = pHeadPart;
    m_fYawLimit = (fHorizontalLimit < 0.0f ? -fHorizontalLimit : fHorizontalLimit) < LOOKAT_MAX_ANGLE_DEGREES
        ? (fHorizontalLimit < 0.0f ? -fHorizontalLimit : fHorizontalLimit) : LOOKAT_MAX_ANGLE_DEGREES;
    m_fPitchLimit = (fVerticalLimit < 0.0f ? -fVerticalLimit : fVerticalLimit) < LOOKAT_MAX_ANGLE_DEGREES
        ? (fVerticalLimit < 0.0f ? -fVerticalLimit : fVerticalLimit) : LOOKAT_MAX_ANGLE_DEGREES;
    m_fDuration = fDuration < 0.0f ? -fDuration : fDuration;
    m_fMaxDistance = fMaxDelta < 0.0f ? -fMaxDelta : fMaxDelta;
    m_pEndOrientation = 0;
    m_pStartOrientation = 0;
    m_bRestoringOrientation = 0;
    BuildAnimation();
}
// PC 0x004B8110 GOG 0x004B8220
void Gob::BeginLookAtAnimate(CAurObject* pTarget, char* sTargetPart,
                             char* sHeadPart, float fHorizontalLimit,
                             float fVerticalLimit, float fDuration,
                             float fMaxDelta)
{
    Part* pTargetPart = FindPart(((Gob*)pTarget)->m_pRootPart, sTargetPart);
    Part* pHeadPart = FindPart(m_pRootPart, sHeadPart);
    if (pTargetPart != 0 && pHeadPart != 0)
    {
        if (m_pLookAtAnimation != 0)
        {
            OOAnimRun* pRun = m_pLookAtAnimation->m_pOutOfOrderRun;
            if (pRun != 0)
            {
                if (m_lOutOfOrderAnimations.Contains(pRun))
                    m_lOutOfOrderAnimations.Remove(pRun);
                delete (AnimRun*)pRun;
                CustomAnimation* pOldAnimation = m_pLookAtAnimation;
                pOldAnimation->m_pOutOfOrderRun = 0;
            }
            delete m_pLookAtAnimation;
            m_pLookAtAnimation = 0;
        }
        m_pLookAtAnimation = new LookAtAnimation(pTarget, pTargetPart, this,
            pHeadPart, fHorizontalLimit, fVerticalLimit, fDuration, fMaxDelta);
        Animation* pAnimation = (Animation*)m_pLookAtAnimation->m_pTree;
        OOAnimRun* pRun = (OOAnimRun*)new AnimRun(pAnimation);
        pRun->m_fTime = 0.0f;
        pRun->m_fBlendWeight = 0.0f;
        pRun->m_fTransitionTime = pAnimation->m_fTransitionTime;
        pRun->m_nFlags = AnimRun::FLAG_CURRENT_ANIMATION_PENDING;
        m_lOutOfOrderAnimations.Insert(pRun, 0);
        m_pLookAtAnimation->m_pOutOfOrderRun = pRun;
    }
    else
    {
        EndLookAtAnimate();
    }
}

static inline void AuranimTrace4()
{
    AuranimTrace3();
}

static inline void AuranimTrace3()
{
    AuranimTrace2();
}

static inline void AuranimTrace2()
{
    AuranimTrace1();
}

static inline void AuranimTrace1()
{
}

#ifdef _XBOX

Gob* FindGob(char* sName);

const char* beginlookatanimate(char* sCommand)
{
    char sLookingGob[32];
    char sLookAtGob[32];
    char sLookingPart[32];
    char sLookAtPart[32];

    if (sscanf(sCommand, "%s %s %s %s", sLookingGob, sLookingPart, sLookAtGob, sLookAtPart) != 4)
    {
        return "Usage: beginlookatanimate lookinggob lookingpart lookatgob lookatpart";
    }
    Gob* pLooking = FindGob(sLookingGob);
    if (pLooking == 0)
    {
        return "Cannot find looking gob.";
    }
    Gob* pLookAt = FindGob(sLookAtGob);
    if (pLookAt == 0)
    {
        return "Cannot find lookat gob.";
    }
    pLooking->BeginLookAtAnimate(pLookAt, sLookAtPart, sLookingPart, 60.0f, 45.0f, 0.25f, 10.0f);
    return "success";
}

const char* endlookatanimate(char* sCommand)
{
    Gob* pGob = FindGob(sCommand);
    if (pGob == 0)
    {
        return "Cannot find gob.";
    }
    pGob->EndLookAtAnimate();
    return "success";
}

int animqueue(char* sCommand)
{
    return Gob::ANIMATION_FLAG_QUEUE;
}

int animremove(char* sCommand)
{
    return Gob::ANIMATION_FLAG_STOP;
}

const char* addflag(char* sCommand)
{
    int nFlag;
    char sName[64];

    if (sscanf(sCommand, "%s %d", sName, &nFlag) != 2)
    {
        return "Usage: addflag <gobname> <flag>";
    }
    Gob* pGob = FindGob(sName);
    if (pGob == 0)
    {
        return "Failure - could not find gob with that name.";
    }
    pGob->AddAnimationFlag(nFlag);
    return "Success";
}

const char* play(char* sCommand)
{
    float fSpeed = 1.0f;
    float fStartTime;
    int nFlags;
    char sObject[64];
    char sAnimation[64];

    nFlags = 0;
    fStartTime = 0.0f;
    int nRead = sscanf(sCommand, "%s %s %f %d %f", sObject, sAnimation, &fSpeed, &nFlags, &fStartTime);
    if (nRead != 2 && nRead != 3 && nRead != 4 && nRead != 5)
    {
        return "usage:  play object anim [speed] [flags] [starttime]";
    }
    Gob* pGob = FindGob(sObject);
    if (pGob == 0)
    {
        return "unable to find object by that name";
    }
    if (pGob->PlayAnimation(sAnimation, fSpeed, nFlags, fStartTime) == 0)
    {
        return "didn't find animation by that name";
    }
    return "success";
}

const char* getanim(char* sCommand)
{
    static char sResult[128];
    float fTime;
    float fLength;
    char sObject[64];
    char sAnimation[64];

    if (sscanf(sCommand, "%s %s", sObject, sAnimation) != 2)
    {
        return "usage:  getanim object anim";
    }
    Gob* pGob = FindGob(sObject);
    if (pGob == 0)
    {
        return "unable to find object by that name";
    }
    if (pGob->GetAnimation(sAnimation, &fTime, &fLength) == 0)
    {
        return "animation not found on that gob";
    }
    sprintf(sResult, "animation found: time=%.2f, length=%.2f", fTime, fLength);
    return sResult;
}

const char* animadvance(char* sCommand)
{
    static char sResult[128];
    float fAdvance = 0.0f;
    char sName[64];

    sName[0] = 0;
    sscanf(sCommand, "%s %f", sName, &fAdvance);
    Gob* pGob = FindGob(sName);
    if (pGob == 0)
    {
        return "No Object with that name";
    }
    AnimRun* pRun = CurrentAnimation(pGob);
    if (pRun == 0)
    {
        return "No Current Animation";
    }
    pRun->m_fTime += fAdvance;
    if (pRun->m_nFlags & AnimRun::FLAG_STOP_AT_END)
    {
        if (pRun->m_fTime > pRun->m_pAnimation->m_fLength)
        {
            pRun->m_fTime = pRun->m_pAnimation->m_fLength;
        }
        if (pRun->m_fTime < 0.0f)
        {
            pRun->m_fTime = 0.0f;
        }
    }
    else
    {
        if (pRun->m_fTime > pRun->m_pAnimation->m_fLength)
        {
            pRun->m_fTime -= pRun->m_pAnimation->m_fLength;
        }
        if (pRun->m_fTime < 0.0f)
        {
            pRun->m_fTime += pRun->m_pAnimation->m_fLength;
        }
    }
    sprintf(sResult, "%s %s %f", sName, pRun->m_pAnimation->m_sName, pRun->m_fTime);
    return sResult;
}
#endif

#ifdef _XBOX
void AnimateHierarchy(Part* pPart, MdlNode* pNode, float fTime, float fPreviousTime, float fWeight, bool bBlend);

void Gob::ResetAnimationScales()
{
    for (int i = 0; i < m_lOutOfOrderAnimations.m_nCount; i++)
    {
        AnimRun* pRun = m_lOutOfOrderAnimations[i];
        pRun->m_fScale = FindAnimScale(m_pModel, FindAnimation(m_pModel, pRun->m_pAnimation->m_sName));
    }
    position_scale_factor = 1.0f;
    AnimateHierarchy(m_pRootPart, m_pRootPart->m_pNode, 0.0f, 0.0f, 1.0f, false);
}

const char* resetgob(char* sCommand)
{
    char sName[64];

    sName[0] = 0;
    sscanf(sCommand, "%s", sName);
    Gob* pGob = FindGob(sName);
    if (pGob == 0)
    {
        return "No Object with that name";
    }
    pGob->ResetAnimationScales();
    return "Gob has been reset";
}
#endif

#ifdef _XBOX
bool Gob::CheckEventCallBack(float fDelay, bool bRepeat)
{
    if (!checkevents)
        return true;
    bool bFire = false;
    if (fDelay > m_fCameraDistance || (bRepeat && IsInViewVolume()))
        bFire = true;
    return bFire;
}
#endif

#ifdef _XBOX
CustomAnimation::CustomAnimation()
{
    m_pTree = 0;
    m_pOutOfOrderRun = 0;
    m_bDeactivated = 0;
}
#endif

#ifdef _XBOX
OOAnimRun::~OOAnimRun()
{
    delete[] m_pDeltaValues;
    delete[] m_pStartValues;
    m_pDeltaValues = 0;
    m_pStartValues = 0;
}
#endif

#ifdef _XBOX
void Gob::DoEvent(char* sEvent, void* pData)
{
    int nIndex = m_lCallBacks.m_nCount - 1;
    SafePointer<CAurObject> pObject(this);
    while (pObject && nIndex >= 0)
    {
        CallBack* pCallBack = m_lCallBacks[nIndex];
        if ((pCallBack->m_sEvent[0] == 0 ||
             pCallBack->m_sEvent[0] == '*' ||
             stricmp(pCallBack->m_sEvent, sEvent) == 0) &&
            CheckEventCallBack(pCallBack->m_fDelay, pCallBack->m_bRepeat))
        {
            pCallBack->m_pfnCallBack(this, sEvent,
                                   pData ? pData : pCallBack->m_pData);
        }
        --nIndex;
    }
}
#endif
