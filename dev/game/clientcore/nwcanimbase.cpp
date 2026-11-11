#include "nwcanimbase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "nwcmodule.h"
#include "nwclass.h"
#include "nwrules.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "nwcanimbase.h"
#include "math3d.h"
#include "aurcamera.h"
#include "swgui.h"
#include <float.h>
#include <stdio.h>
#include <string.h>

float g_fMaxLean = 5.0f;
// PC 0x0060E760  Xbox 0x000C8B70
void CSWCAnimBase::SetWieldLimited(int bLimited)
{
}
// PC 0x0069CD80  Xbox 0x000C8B90
void CSWCAnimBase::AttachToScene(CAurScene* pScene)
{
    if (m_pModel != 0)
    {
        m_pModel->AttachToScene((Scene*)pScene);
    }
}
// PC 0x0069CDA0  Xbox 0x000C8BB0
Gob* CSWCAnimBase::GetModel(unsigned char nSlot)
{
    if (nSlot != 0xFF)
    {
        return 0;
    }
    return m_pModel;
}
// PC 0x0069CDC0  Xbox 0x000C8BD0
int CSWCAnimBase::ReplaceTexture(const CResRef& sOld, const CResRef& sNew,
                                 int bRemember)
{
    char sNewName[20];
    char sOldName[20];

    if (m_pModel == 0)
    {
        return 0;
    }
    sOld.CopyToString(sOldName);
    sNew.CopyToString(sNewName);
    strlwr(sNewName);
    strlwr(sOldName);
    m_pModel->ReplaceTexture(sOldName, sNewName);
    if (bRemember != 0)
    {
        m_resLoadedModel = sNew;
        m_bPaletteReplaced = 0;
    }
    return 1;
}
// PC 0x0069CE50  Xbox 0x000C8C80
int CSWCAnimBase::ReplaceTexture(unsigned char nModel, const CResRef& sOld,
                                 const CResRef& sNew, int bRemember)
{
    char sNewName[20];
    char sOldName[20];

    if (m_pModel == 0 || nModel != 0xFF)
    {
        return 0;
    }
    sOld.CopyToString(sOldName);
    sNew.CopyToString(sNewName);
    strlwr(sNewName);
    strlwr(sOldName);
    m_pModel->ReplaceTexture(sOldName, sNewName);
    if (bRemember != 0)
    {
        m_resLoadedModel = sNew;
        m_bPaletteReplaced = 0;
    }
    return 1;
}
// PC 0x0069CEF0  Xbox 0x000C8D40
int CSWCAnimBase::ReplaceTexture(unsigned char nModel, const CResRef& sOld,
                                 const CResRef& sNew, unsigned long nColor,
                                 unsigned short* pnPalette, int nRemember,
                                 unsigned long nOffset)
{
    char sOldName[20];
    char sNewName[20];

    if (m_pModel == 0 || nModel != 0xFF)
    {
        return 0;
    }
    sOld.CopyToString(sOldName);
    sNew.CopyToString(sNewName);
    strlwr(sOldName);
    strlwr(sNewName);
    m_pModel->ReplaceTexture(sOldName, sNewName, nColor, pnPalette, nOffset);
    if (nRemember != 0)
    {
        m_resLoadedModel = sNew;
        m_bPaletteReplaced = 1;
        m_nPaletteCount = nColor;
        m_nPaletteOffset = nOffset;
        memcpy(m_anPalette, &pnPalette[nOffset], 2 * nColor);
    }
    return 1;
}
// PC 0x0069CFD0  Xbox 0x000C8E30
int CSWCAnimBase::ReplaceTexture(const CResRef& sOld, const CResRef& sNew,
                                 unsigned long nColor,
                                 unsigned short* pnPalette, int nRemember,
                                 unsigned long nOffset)
{
    char sOldName[20];
    char sNewName[20];

    if (m_pModel == 0)
    {
        return 0;
    }
    sOld.CopyToString(sOldName);
    sNew.CopyToString(sNewName);
    strlwr(sOldName);
    strlwr(sNewName);
    m_pModel->ReplaceTexture(sOldName, sNewName, nColor, pnPalette, nOffset);
    if (nRemember != 0)
    {
        m_resLoadedModel = sNew;
        m_bPaletteReplaced = 1;
        m_nPaletteCount = nColor;
        m_nPaletteOffset = nOffset;
        memcpy(m_anPalette, &pnPalette[nOffset], 2 * nColor);
    }
    return 1;
}
// PC 0x0069D0A0  Xbox 0x000C8F10
int CSWCAnimBase::RestoreTexture()
{
    if (m_pModel == 0)
    {
        return 0;
    }
    if (m_bPaletteReplaced != 0)
    {
        CResRef sTexture("");
        ReplaceTexture(MODEL_SLOT_MAIN, sTexture, m_resLoadedModel,
                       m_nPaletteCount, m_anPalette, 0, m_nPaletteOffset);
    }
    return 1;
}
// PC 0x0069D100  Xbox 0x000C8F70
int CSWCAnimBase::RestoreTexture(unsigned char nModel)
{
    if (m_pModel == 0 || nModel != MODEL_SLOT_MAIN)
    {
        return 0;
    }
    if (m_bPaletteReplaced != 0)
    {
        CResRef sTexture("");
        ReplaceTexture(MODEL_SLOT_MAIN, sTexture, m_resLoadedModel,
                       m_nPaletteCount, m_anPalette, 0, m_nPaletteOffset);
        return 1;
    }
    m_pModel->RestoreTexture();
    return 1;
}
// PC 0x0069D180  Xbox 0x000C8FF0
float CSWCAnimBase::GetAnimationTime(unsigned short nAnimation)
{
    float fTime;
    float fLength;
    CExoString sName = GetAnimationName(nAnimation);

    if (m_pModel != 0)
    {
        m_pModel->GetAnimation((char*)sName.CStr(), &fTime, &fLength);
        if (fTime > 0.0f)
        {
            return fTime * 1000.0f;
        }
    }
    return 0.0f;
}
// PC 0x0069D250  Xbox 0x000C90C0
float CSWCAnimBase::GetAnimationLength(unsigned short nAnimation)
{
    float fTime;
    float fLength;
    CExoString sName = GetAnimationName(nAnimation);

    if (m_pModel != 0)
    {
        m_pModel->GetAnimation((char*)sName.CStr(), &fTime, &fLength);
        if (fLength > 0.0f)
        {
            return fLength * 1000.0f;
        }
    }
    return 0.0f;
}
// PC 0x0069D320  Xbox 0x000C9190
int CSWCAnimBase::GetOverlayAnimationPlaying(unsigned short nAnimation)
{
    float fTime;
    float fLength;

    if (m_pModel == 0)
    {
        return 0;
    }
    CExoString sName = GetAnimationName(nAnimation);
    m_pModel->GetAnimation((char*)sName.CStr(), &fTime, &fLength);
    return fTime > 0.0f;
}
// PC 0x0069D3E0  Xbox 0x000C9250
int CSWCAnimBase::GetOverlayAnimation(unsigned short nAnimation)
{
    float fTime;
    float fLength;

    if (m_pModel == 0)
    {
        return 0;
    }
    CExoString sName = GetAnimationName(nAnimation);
    return m_pModel->GetAnimation((char*)sName.CStr(), &fTime, &fLength) != 0;
}
// PC 0x0069D490  Xbox 0x000C9300
int CSWCAnimBase::AddAnimationFlagInternal(CAurObject* pObject, int nFlag)
{
    if (pObject == 0)
    {
        return 0;
    }
    pObject->AddAnimationFlag(nFlag);
    return 1;
}
// PC 0x0069D4C0  Xbox 0x000C9330
int CSWCAnimBase::RemoveAnimationFlagInternal(CAurObject* pObject, int nFlag)
{
    if (pObject == 0)
    {
        return 0;
    }
    pObject->RemoveAnimationFlag(nFlag);
    return 1;
}
// PC 0x0069D4F0  Xbox 0x000C9360
int CSWCAnimBase::SetAnimation(unsigned short nAnimation, float fSpeed,
                               int nFlags, float fBlend)
{
    return SetAnimationInternal(m_pModel, nAnimation, fSpeed, nFlags, fBlend);
}
// PC 0x0069D520  Xbox 0x000C9390
int CSWCAnimBase::SetAnimationOnSubObject(unsigned char nSubObject,
                                          unsigned short nAnimation,
                                          float fSpeed, int nFlags,
                                          float fBlend)
{
    if (nSubObject != 0xFF)
    {
        return 0;
    }
    return SetAnimationInternal(m_pModel, nAnimation, fSpeed, nFlags, fBlend);
}
// PC 0x0069D540  Xbox 0x000C93B0
int CSWCAnimBase::SetAnimationVisemes(float* pfWeights, float* pfTimes,
                                      int nCount, float fLength)
{
    if (m_pModel != 0)
    {
        if (pfWeights != 0 && pfTimes != 0)
        {
            m_pModel->PlayOutOfOrderAnimation("talk", pfWeights, pfTimes,
                                              nCount, fLength);
        }
        else
        {
            m_pModel->StopOutOfOrderAnimation("talk");
        }
        return 1;
    }
    return 0;
}
// PC 0x0069D5A0  Xbox 0x000C9420
int CSWCAnimBase::SetAnimationOnSubObjectInternal(CAurObject* pObject,
                                                  unsigned char nSubObject,
                                                  unsigned short nAnimation,
                                                  float fSpeed, int nFlags,
                                                  float fBlend)
{
    if (nSubObject != 0xFF)
    {
        return 0;
    }
    return SetAnimationInternal(pObject, nAnimation, fSpeed, nFlags, fBlend);
}
// PC 0x0069D5D0  Xbox 0x000C9450
int CSWCAnimBase::SetOrientation(const Vector& vFacing)
{
    if (m_pModel == 0)
    {
        return 0;
    }
    m_pModel->SetOrientation(YawPitchRoll(Yaw(vFacing), 0.0f, 0.0f));
    return 1;
}
// PC 0x0069D640  Xbox 0x000C94D0
int CSWCAnimBase::SetOrientation(const Quaternion& qOrientation)
{
    if (m_pModel == 0)
    {
        return 0;
    }
    m_pModel->SetOrientation(qOrientation);
    return 1;
}
// PC 0x0069D690  Xbox 0x000C9530
int CSWCAnimBase::SetPosition(const Vector& vPosition)
{
    if (m_pModel == 0)
    {
        return 0;
    }
    m_pModel->SetPosition(vPosition);
    return 1;
}
// PC 0x0069D6E0  Xbox 0x000C9580
void CSWCAnimBase::SetAlwaysRender(int bRender)
{
    if (m_pModel != 0)
    {
        if (bRender != 0)
        {
            m_pModel->EnableAlwaysRender();
        }
        else
        {
            m_pModel->DisableAlwaysRender();
        }
    }
}
// PC 0x0069D710  Xbox 0x000C95B0
void CSWCAnimBase::SetInCutscene(int bCutscene)
{
    if (m_pModel != 0)
    {
        if (bCutscene != 0)
        {
            m_pModel->EnableInCutscene();
        }
        else
        {
            m_pModel->DisableInCutscene();
        }
    }
}
// PC 0x0069D740  Xbox 0x000C95E0
void CSWCAnimBase::UnloadModel()
{
    if (m_pModel != 0)
    {
        AttachToScene(0);
        delete m_pModel;
        m_pModel = 0;
    }
}
// PC 0x0069D770  Xbox 0x000C9610
void CSWCAnimBase::UnloadModel(unsigned char nModel)
{
    if (nModel == 0xFF)
    {
        CSWCAnimBase::UnloadModel();
    }
}
// PC 0x0069D7B0  Xbox 0x000C9650
int CSWCAnimBase::SetEnvMap(int nType, int bRemember, const CResRef& sMap)
{
    char sName[20];

    if (m_pModel == 0)
    {
        return 0;
    }
    if (nType != 0)
    {
        sMap.CopyToString(sName);
        m_pModel->SetEnvironmentMap(sName);
    }
    else
    {
        m_pModel->SetEnvironmentMap("");
    }
    if (bRemember != 0)
    {
        m_resEnvMap = sMap;
        m_bEnvMap = nType;
    }
    return 1;
}
// PC 0x0069D830  Xbox 0x000C96E0
int CSWCAnimBase::SetEnvMap(unsigned char nModel, int nType, int bRemember,
                            const CResRef& sMap)
{
    char sName[20];

    if (m_pModel == 0 || nModel != 0xFF)
    {
        return 0;
    }
    if (nType != 0)
    {
        sMap.CopyToString(sName);
        m_pModel->SetEnvironmentMap(sName);
    }
    else
    {
        m_pModel->SetEnvironmentMap("");
    }
    if (bRemember != 0)
    {
        m_resEnvMap = sMap;
        m_bEnvMap = nType;
    }
    return 1;
}
// PC 0x0069D8B0  Xbox 0x000C9780
int CSWCAnimBase::RestoreEnvMap()
{
    char sName[20];

    if (m_pModel == 0)
    {
        return 0;
    }
    if (m_bEnvMap != 0)
    {
        m_resEnvMap.CopyToString(sName);
        m_pModel->SetEnvironmentMap(sName);
    }
    else
    {
        m_pModel->SetEnvironmentMap("");
    }
    return 1;
}
// PC 0x0069D910  Xbox 0x000C97F0
int CSWCAnimBase::AnimationExists(unsigned short nAnimation)
{
    float fTime;
    float fLength;
    CExoString sName;

    sName = GetAnimationName(nAnimation);
    if (m_pModel == 0)
    {
        return 0;
    }
    m_pModel->GetAnimation((char*)sName.CStr(), &fTime, &fLength);
    return fLength >= 0.0f;
}
// PC 0x0069DA00  Xbox 0x000C98E0
int CSWCAnimBase::ApplyShadowBlob(const CExoString& sModel)
{
    m_pShadowBlob = static_cast<Gob*>(
        NewCAurObject((char*)sModel.CStr(), "shadowblob", 0, 0));
    if (m_pShadowBlob == 0)
    {
        return 0;
    }
    m_pShadowBlob->TurnOffShadows();
    if (m_pModel != 0)
    {
        m_pShadowBlob->AttachToObject(m_pModel, "root", Gob::ATTACH_NO_ORIENTATION);
        m_bShadowBlobShown = 1;
    }
    return 1;
}
// PC 0x0069DA70  Xbox 0x000C9950
void CSWCAnimBase::RemoveShadowBlob()
{
    if (m_pShadowBlob != 0)
    {
        m_pShadowBlob->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        m_pShadowBlob->AttachToScene((Scene*)0);
        delete m_pShadowBlob;
        m_pShadowBlob = 0;
        m_bShadowBlobShown = 0;
    }
}
// PC 0x0069DAC0  Xbox 0x000C99A0
void CSWCAnimBase::ShowShadowBlob(int bShow)
{
    if (m_pShadowBlob != 0)
    {
        if (m_pModel != 0 && (bShow != 0 || m_bShadowBlobShown == 0))
        {
            m_pShadowBlob->AttachToObject(m_pModel, "root", Gob::ATTACH_NO_ORIENTATION);
            m_bShadowBlobShown = 1;
        }
    }
}
// PC 0x0069DB10  Xbox 0x000C99F0
void CSWCAnimBase::HideShadowBlob()
{
    if (m_pShadowBlob != 0)
    {
        if (m_bShadowBlobShown != 0)
        {
            m_pShadowBlob->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            m_pShadowBlob->AttachToScene((Scene*)0);
            m_bShadowBlobShown = 0;
        }
    }
}
// PC 0x0069DB50  Xbox 0x000CACD0
void CSWCAnimBase::SetShadowBlobOrientation(const Quaternion& qOrientation)
{
    Quaternion qCurrent;
    Vector vPosition;

    if (m_pShadowBlob != 0)
    {
        m_pShadowBlob->GetPartPosition("root", &vPosition, &qCurrent);
        m_pShadowBlob->SetPartPosition("root", vPosition, qOrientation);
    }
}
// PC 0x0069DC10  Xbox 0x000C9A30
void CSWCAnimBase::TurnOnShadows()
{
    if (m_pModel != 0)
    {
        m_pModel->TurnOnShadows();
    }
}
// PC 0x0069DC30  Xbox 0x000C9A50
void CSWCAnimBase::TurnOffShadows()
{
    if (m_pModel != 0)
    {
        m_pModel->TurnOffShadows();
    }
}
// PC 0x0069DC50  Xbox 0x000CAD90
Quaternion CSWCAnimBase::GetOrientation()
{
    if (m_pModel == 0)
    {
        return Quaternion();
    }
    return m_pModel->GetOrientation();
}
// PC 0x0069DC90  Xbox 0x000C9A70
void CSWCAnimBase::Lean()
{
    if (m_pModel != 0)
    {
        float fYaw = Yaw(m_pModel->GetOrientation());
        float fPitch = Pitch(m_pModel->GetOrientation());
        m_pModel->SetOrientation(YawPitchRoll(fYaw, fPitch, m_fLean));
    }
}
// PC 0x0069DD30  Xbox 0x000C9B10
void CSWCAnimBase::SetLean(float fLean)
{
    m_fLean = min(1.0f, max(-1.0f, fLean)) * g_fMaxLean;
}
// PC 0x0069DD90  Xbox 0x000C9B70
int CSWCAnimBase::EnableBumpedOutShell(const CResRef& sShell, float fScale)
{
    char sName[20];

    if (m_pModel == 0)
    {
        return 0;
    }
    sShell.CopyToString(sName);
    strlwr(sName);
    m_pModel->EnableRenderBumpedOut(sName, fScale);
    return 1;
}
// PC 0x0069DDF0  Xbox 0x000C9BF0
int CSWCAnimBase::DisableBumpedOutShell()
{
    if (m_pModel == 0)
    {
        return 0;
    }
    m_pModel->DisableRenderBumpedOut();
    return 1;
}
// PC 0x0069DE10  Xbox 0x000C9C20
int CSWCAnimBase::SetMotionBlur(int bBlur)
{
    if (m_pModel == 0)
    {
        return 0;
    }
    m_pModel->SetMotionBlurred(bBlur != 0);
    return 1;
}
// PC 0x0069DE40  Xbox 0x000C9C50
int CSWCAnimBase::SetMaxMotionBlurLength(float fLength)
{
    return m_pModel != 0;
}
// PC 0x0069DE50  Xbox 0x000C9C60
void CSWCAnimBase::EnableDistortion()
{
    if (m_pModel != 0)
    {
        m_pModel->EnableDistortion();
    }
}
// PC 0x0069DE70  Xbox 0x000C9C80
void CSWCAnimBase::DisableDistortion()
{
    if (m_pModel != 0)
    {
        m_pModel->DisableDistortion();
    }
}
// PC 0x0069DE90  Xbox 0x000C9CA0
void CSWCAnimBase::SetGroundTilt(const Vector& vGroundNormal,
                                const Vector& vFacing)
{
    if (m_pModel != 0)
    {
        float fPitch = NWCANIMBASE_GROUND_NORMAL_UP_PITCH - Pitch(vGroundNormal);
        Vector vAxis = cross(zaxis, vGroundNormal);
        Quaternion qTilt(vAxis, fPitch * DEG_TO_RAD);
        Quaternion qBlended = slerp(qTilt, m_qGroundTilt, NWCANIMBASE_GROUND_TILT_BLEND);
        m_qGroundTilt = qTilt;
        Quaternion qOrientation = qBlended * YawPitchRoll(Yaw(vFacing), 0.0f, 0.0f);
        m_pModel->SetOrientation(qOrientation);
    }
}

static Vector* AllocateVectorArray(int nCount)
{
    return new Vector[nCount];
}
// PC 0x0069DFB0  Xbox 0x000C9DC0
CSWCAnimBase::CSWCAnimBase()
{
    m_pModel = 0;
    m_nAnimationType = ANIMATION_TYPE_BASE;
    m_nWieldClass = CSWCAnimBase::WIELD_CLASS_NONE;
    m_nAnimSet = 0;
    m_nOffHandType = 0;
    m_resLoadedModel = "";
    m_bPaletteReplaced = 0;
    m_nPaletteCount = 0;
    m_nPaletteOffset = 0;
    memset(m_anPalette, 0, sizeof(m_anPalette));
    for (int i = 0; i < 5; ++i)
    {
        m_abHiliteActive[i] = 0;
        m_abHilitePulse[i] = 0;
    }
    m_resEnvMap = "";
    m_bEnvMap = 0;
    m_fAlpha = 1.0f;
    m_nSpecialWalkAnimation = 0;
    m_bRunLooksAwful = 0;
    m_bUseable = 1;
    m_pShadowBlob = 0;
    m_bShadowBlobShown = 0;
    m_fLean = 0.0f;
    m_nPoweredAnimations = 0;
    m_qGroundTilt = Quaternion(zaxis, 0.0f);
}
// PC 0x006A03C0  Xbox 0x000CC500
int CSWCAnimBase::SetOverlayAnimation(unsigned short nAnimation, int bLoop)
{
    if (m_pModel == 0)
    {
        return 0;
    }
    return SetOverlayAnimation(m_pModel, nAnimation, bLoop);
}
// PC 0x006A03E0  Xbox 0x000CC520
int CSWCAnimBase::Hilite(const Vector& vColor, unsigned char nModel, int bPulse)
{
    if (m_pModel == 0)
    {
        return 0;
    }
    if (nModel >= 5)
    {
        return 0;
    }
    m_avHilite[nModel] = vColor;
    m_abHiliteActive[nModel] = 1;
    m_abHilitePulse[nModel] = bPulse;
    RefreshHilite();
    return 1;
}
// PC 0x006A0440
int CSWCAnimBase::UnHilite(unsigned char nModel)
{
    if (m_pModel == 0)
    {
        return 0;
    }
    if (nModel >= 5)
    {
        return 0;
    }
    m_abHiliteActive[nModel] = 0;
    RefreshHilite();
    return 1;
}
// PC 0x0069E140  Xbox 0x000C9FD0
CSWCAnimBase::~CSWCAnimBase()
{
    RemoveShadowBlob();
    UnloadModel();
}
// PC 0x0069E1F0  Xbox 0x000CA080
int CSWCAnimBase::LoadModel(const CResRef& sResRef, unsigned char nType)
{
    Quaternion qOrientation;
    Vector vPosition;
    char szModel[20];

    if (nType != MODEL_SLOT_MAIN)
    {
        return 0;
    }

    sResRef.CopyToString(szModel);
    m_pModel = static_cast<Gob*>(NewCAurObject(szModel, "body", 0, 0));
    if (m_pModel == 0)
    {
        return 0;
    }

    if (m_pModel->GetPartLocalPosition("handconjure", &vPosition,
                                       &qOrientation) != 0 &&
        (qOrientation.m_fX != 0.0f || qOrientation.m_fY != 0.0f ||
         qOrientation.m_fZ != 0.0f || qOrientation.m_fW != 1.0f))
    {
        CExoString sError;
        sError.Format("CSWCAnimBase::LoadModel(): The handconjure dummy has "
                      "an orientation....It shouldn't!!  The %s model needs "
                      "to be fixed or else the spell visuals will not be "
                      "correct.", szModel);
    }

    if (m_pModel->GetPartLocalPosition("headconjure", &vPosition,
                                       &qOrientation) != 0 &&
        (qOrientation.m_fX != 0.0f || qOrientation.m_fY != 0.0f ||
         qOrientation.m_fZ != 0.0f || qOrientation.m_fW != 1.0f))
    {
        CExoString sError;
        sError.Format("CSWCAnimBase::LoadModel(): The headconjure dummy has "
                      "an orientation....It shouldn't!!  The %s model needs "
                      "to be fixed or else the spell visuals will not be "
                      "correct.", szModel);
    }

    m_resLoadedModel = sResRef;
    m_bPaletteReplaced = 0;
    ShowShadowBlob(0);
    return 1;
}
// PC 0x0069E420  Xbox 0x000CA2A0
int CSWCAnimBase::SetOverlayAnimation(CAurObject* pObject,
                                      unsigned short nAnimation, int bLoop)
{
    CExoString sName;
    unsigned short nFlags;
    float fSpeed;

    if (pObject == 0)
    {
        return 0;
    }
    if (bLoop == 0)
    {
        switch (nAnimation)
        {
        case 10029:
        case 10030:
        case 10034:
        case 10038:
        case 10039:
        case 10040:
        case 10041:
        case 10042:
        case 10070:
        case 10071:
        case 10136:
        case 10145:
        case 10154:
        case 10155:
            nFlags = 1024;
            break;
        default:
            nFlags = 128;
            break;
        }
    }
    else
    {
        nFlags = 8;
    }
    fSpeed = 1.0f;
    sName = GetAnimationName(nAnimation);
    switch (nAnimation)
    {
    case 10030:
    case 10038:
    case 10039:
    case 10040:
    case 10041:
    case 10042:
    case 10154:
    case 10155:
        break;
    case 373:
        fSpeed = -1.0f;
    default:
        nFlags |= 1;
        break;
    }
    if (sName == "pause1")
    {
        return 0;
    }
    return static_cast<Gob*>(pObject)->PlayAnimation((char*)sName.CStr(),
                                                     fSpeed, nFlags, 0.0f);
}
// PC 0x0069E620  Xbox 0x000CA4C0
CExoString CSWCAnimBase::GetAnimationName(unsigned short nAnimation)
{
    unsigned short nClientAnimation = ServerToClientAnimation(nAnimation);
    CExoString sName;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pAnimations = p2DArrays->m_pAnimations;

    if (pAnimations->GetCExoStringEntry(
            nClientAnimation, CTwoDimArrays::COL2DA_ANIMATIONS_NAME, &sName) == 0)
    {
        sName = "";
    }
    if (sName.GetLength() > 0)
    {
        return sName;
    }

    int nIndex = -1;
    CExoString sSuffix;
    if (nAnimation >= CUTSCENE_ANIMATION_FIRST &&
        nAnimation <= CUTSCENE_ANIMATION_LAST)
    {
        nIndex = nAnimation - CUTSCENE_ANIMATION_FIRST;
    }
    else if (nAnimation >= CUTSCENE_ANIMATION_W_FIRST &&
             nAnimation <= CUTSCENE_ANIMATION_W_LAST)
    {
        sSuffix = "w";
        nIndex = nAnimation - CUTSCENE_ANIMATION_W_FIRST;
    }
    else if (nAnimation >= CUTSCENE_ANIMATION_L_FIRST &&
             nAnimation <= CUTSCENE_ANIMATION_L_LAST)
    {
        sSuffix = "l";
        nIndex = nAnimation - CUTSCENE_ANIMATION_L_FIRST;
    }
    else if (nAnimation >= CUTSCENE_ANIMATION_WL_FIRST &&
             nAnimation <= CUTSCENE_ANIMATION_WL_LAST)
    {
        sSuffix = "wl";
        nIndex = nAnimation - CUTSCENE_ANIMATION_WL_FIRST;
    }

    nIndex++;
    if (nIndex != 0)
    {
        char szNumber[4];
        sprintf(szNumber, "%03d", nIndex);
        CExoString sNumber(szNumber);
        sName = CExoString("cut") + sNumber + sSuffix;
    }

    if (sName.IsEmpty() && nAnimation != CREATURE_ANIMATION_NO_IDLE_FALLBACK)
    {
        nClientAnimation = ServerToClientAnimation(CREATURE_ANIMATION_COMBAT_IDLE);
        p2DArrays = g_pRules->m_p2DArrays;
        pAnimations = p2DArrays->m_pAnimations;
        if (pAnimations->GetCExoStringEntry(
                nClientAnimation, CTwoDimArrays::COL2DA_ANIMATIONS_NAME, &sName) == 0)
        {
            sName = "";
        }
        if (sName.GetLength() > 0)
        {
            return sName;
        }
    }
    return sName;
}
// PC 0x0069E8D0  Xbox 0x000CA7F0
int CSWCAnimBase::SetAnimationInternal(CAurObject* pObject,
                                       unsigned short nAnimation, float fSpeed,
                                       int nFlags, float fBlend)
{
    int bResult = 0;
    CExoString sName = GetAnimationName(nAnimation);

    if (pObject != 0)
    {
        if (fSpeed == 0.0f)
        {
            fSpeed = 1.0f;
        }
        switch (nAnimation)
        {
        case 10003:
        case 10081:
        case 10083:
        case 10212:
        case 10238:
        case 10256:
        case 10258:
        case 10271:
            if (fSpeed >= 0.0f)
            {
                fSpeed = -fSpeed;
            }
            static_cast<Gob*>(pObject)->PlayAnimation(sName.CStr(), fSpeed,
                                                      nFlags, fBlend);
            bResult = 1;
            break;
        default:
            static_cast<Gob*>(pObject)->PlayAnimation(sName.CStr(), fSpeed,
                                                      nFlags, fBlend);
            bResult = 1;
            break;
        case 10029:
        case 10030:
        case 10034:
        case 10038:
        case 10039:
        case 10040:
        case 10041:
        case 10042:
        case 10070:
        case 10071:
        case 10129:
        case 10130:
        case 10136:
        case 10154:
        case 10155:
        case 10302:
            SetOverlayAnimation(pObject, nAnimation, 1);
            bResult = 1;
            break;
        }
    }
    return bResult;
}
// PC 0x0069EB20  Xbox 0x000CAA30
int CSWCAnimBase::SetAlpha(float fAlpha, unsigned char nModel)
{
    if (m_pModel == 0 || nModel != 0xFF)
    {
        return 0;
    }
    m_fAlpha = fAlpha;
    int i;
    for (i = 0; i < 5; i++)
    {
        if (m_abHiliteActive[i] != 0)
        {
            m_pModel->SetColorShifting(m_avHilite[i], fAlpha, 1);
            break;
        }
    }
    if (i == 5)
    {
        m_pModel->SetColorShifting(Vector(1.0f, 1.0f, 1.0f), fAlpha, 1);
    }
    return 1;
}
// PC 0x0069EC00  Xbox 0x000CAB10
void CSWCAnimBase::RefreshHilite()
{
    if (m_pModel != 0)
    {
        int i;
        for (i = 0; i < 5; i++)
        {
            if (m_abHiliteActive[i] != 0)
            {
                m_pModel->SetIllumination(m_avHilite[i], 1);
                m_pModel->SetColorShifting(m_avHilite[i], m_fAlpha, 1);
                if (m_abHilitePulse[i] != 0 &&
                    g_pAppManager->m_pClientExoApp->GetModule() != 0 &&
                    g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo != 0)
                {

                    g_pAppManager->m_pClientExoApp->GetModule();
                    m_pModel->GetScene();
                }
                break;
            }
        }
        if (i == 5)
        {
            m_pModel->SetIllumination(Vector(0.0f, 0.0f, 0.0f), 1);
            m_pModel->SetColorShifting(Vector(1.0f, 1.0f, 1.0f), m_fAlpha, 1);
            if (g_pAppManager->m_pClientExoApp->GetModule() != 0 &&
                g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo != 0)
            {

                g_pAppManager->m_pClientExoApp->GetModule();
            }
        }
    }
}
// PC 0x0069EDC0  Xbox 0x000CADE0
int CSWCAnimBase::GetBoundingRect(CAurObject* pObject, CSWGuiExtent& extent)
{
    CAurCamera* pCamera = g_pAppManager->GetClientExoApp()->GetModuleCamera();
    if (pCamera != 0 && pObject != 0)
    {
        Vector vMinimum;
        Vector vMaximum;
        Vector vPosition = pObject->GetPosition();
        Quaternion qOrientation = pObject->GetOrientation();
        Vector vCameraPosition = pCamera->GetPosition();
        Vector vCameraDirection = -pCamera->GetOrientation().zdir();
        float fNear;
        float fFar;
        pCamera->GetClipDist(fNear, fFar);
        pObject->GetBoundingBox(vMinimum, vMaximum);

        Vector avCorners[8];
        float fTop = FLT_MAX;
        float fLeft = FLT_MAX;
        float fBottom = -FLT_MAX;
        float fRight = -FLT_MAX;

        avCorners[0] = vMinimum;
        avCorners[1] = vMaximum;
        avCorners[2] = Vector(vMinimum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
        avCorners[3] = Vector(vMinimum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
        avCorners[4] = Vector(vMaximum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
        avCorners[5] = Vector(vMinimum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);
        avCorners[6] = Vector(vMaximum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
        avCorners[7] = Vector(vMaximum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
        for (int i = 0; i < 8; i++)
        {
            avCorners[i] = vPosition + qOrientation * avCorners[i];
            if ((avCorners[i] - vCameraPosition) * vCameraDirection <= fNear)
            {
                return 0;
            }
            avCorners[i] = pCamera->MapToScreen(avCorners[i], true);
            fLeft = min(fLeft, avCorners[i].m_fX);
            fTop = min(fTop, avCorners[i].m_fY);
            fRight = max(fRight, avCorners[i].m_fX);
            fBottom = max(fBottom, avCorners[i].m_fY);
        }

        CSWGuiManager* pManager = g_pAppManager->GetClientExoApp()->GetSWGuiManager();
        extent.m_nLeft = (int)(pManager->m_nWidth * fLeft);
        extent.m_nTop = (int)(pManager->m_nHeight * fTop);
        extent.m_nWidth = (int)((fRight - fLeft) * pManager->m_nWidth);
        extent.m_nHeight = (int)((fBottom - fTop) * pManager->m_nHeight);
        return 1;
    }
    return 0;
}
// PC 0x0069F1E0  Xbox 0x000CB2E0
unsigned char CSWCAnimBase::GetCenterPoint(Vector& vCenter)
{
    CAurCamera* pCamera = g_pAppManager->m_pClientExoApp->GetModuleCamera();
    unsigned char nResult = CENTER_POINT_UNAVAILABLE;
    if (pCamera != 0 && m_pModel != 0)
    {
        Vector vMinimum;
        Vector vMaximum;
        Vector vPosition = m_pModel->GetPosition();
        Quaternion qOrientation = m_pModel->GetOrientation();
        Vector vCameraPosition = pCamera->GetPosition();
        Vector vView = -pCamera->GetOrientation().zdir();
        m_pModel->GetBoundingBox(vMinimum, vMaximum);

        Vector avCorners[8];
        Vector vWorldMinimum(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector vWorldMaximum(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        avCorners[0] = vMinimum;
        avCorners[1] = vMaximum;
        avCorners[2] = Vector(vMinimum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
        avCorners[3] = Vector(vMinimum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);
        avCorners[4] = Vector(vMaximum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
        avCorners[5] = Vector(vMinimum.m_fX, vMaximum.m_fY, vMaximum.m_fZ);
        avCorners[6] = Vector(vMaximum.m_fX, vMinimum.m_fY, vMaximum.m_fZ);
        avCorners[7] = Vector(vMaximum.m_fX, vMaximum.m_fY, vMinimum.m_fZ);

        for (int i = 0; i < 8; ++i)
        {
            avCorners[i] = qOrientation * avCorners[i] + vPosition;
            vWorldMinimum.m_fX = min(vWorldMinimum.m_fX, avCorners[i].m_fX);
            vWorldMinimum.m_fY = min(vWorldMinimum.m_fY, avCorners[i].m_fY);
            vWorldMinimum.m_fZ = min(vWorldMinimum.m_fZ, avCorners[i].m_fZ);
            vWorldMaximum.m_fX = max(vWorldMaximum.m_fX, avCorners[i].m_fX);
            vWorldMaximum.m_fY = max(vWorldMaximum.m_fY, avCorners[i].m_fY);
            vWorldMaximum.m_fZ = max(vWorldMaximum.m_fZ, avCorners[i].m_fZ);
        }

        Vector vPoint = (vWorldMaximum - vWorldMinimum) / 2.0f + vWorldMinimum;
        if (vView * vPoint > vView * vCameraPosition)
            nResult = CENTER_POINT_IN_FRONT;
        else
            nResult = CENTER_POINT_BEHIND;

        vPoint = pCamera->MapToScreen(vPoint, true);
        CSWGuiManager* pGuiManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
        vCenter.m_fX = vPoint.m_fX * pGuiManager->m_nWidth;
        vCenter.m_fY = vPoint.m_fY * pGuiManager->m_nHeight;
    }
    return nResult;
}
// PC 0x0069F630  Xbox 0x000CB820
int CSWCAnimBase::GetBoundingRect(CSWGuiExtent& extent)
{
    return GetBoundingRect(m_pModel, extent);
}
// PC 0x0069F650  Xbox 0x000CB840
unsigned short CSWCAnimBase::ServerToClientAnimation(unsigned short nAnimation)
{
    unsigned short nClientAnimation = nAnimation;
    unsigned char nWieldClass = m_nWieldClass;

    if (m_nAnimSet == 1)
    {
        switch (nAnimation)
        {
        case 10002:
        case 10003:
            if (nWieldClass == 2)
            {
                if ((unsigned char)m_nPoweredAnimations > 0)
                {
                    nClientAnimation = 338;
                break;
                }
            }
            else if (nWieldClass == 4)
            {
                if ((unsigned char)m_nPoweredAnimations > 0)
                {
                    nClientAnimation = 339;
                break;
                }
            }
            else if (nWieldClass == 3)
            {
                if ((unsigned char)m_nPoweredAnimations > 0)
                {
                    nClientAnimation = 341;
                break;
                }
            }
            else if (nWieldClass == 7)
            {
                nClientAnimation = 340;
                break;
            }
            else if (nWieldClass == 9)
            {
                nClientAnimation = 340;
                break;
            }
            nClientAnimation = 0;
                break;
        case 10004:
            if (m_bRunLooksAwful)
            {
                nClientAnimation = 0;
                break;
            }
            if (nWieldClass == 2)
            {
                if ((unsigned char)m_nPoweredAnimations > 0)
                {
                    nClientAnimation = 345;
                break;
                }
            }
            else if (nWieldClass == 4)
            {
                if ((unsigned char)m_nPoweredAnimations > 0)
                {
                    nClientAnimation = 3;
                break;
                }
            }
            else if (nWieldClass == 3)
            {
                if ((unsigned char)m_nPoweredAnimations > 0)
                {
                    nClientAnimation = 343;
                break;
                }
            }
            else if (nWieldClass == 7)
            {
                nClientAnimation = 342;
                break;
            }
            else if (nWieldClass == 9)
            {
                nClientAnimation = 342;
                break;
            }
            nClientAnimation = 2;
                break;
        case 10015:
            nClientAnimation = 62;
                break;
        case 10016:
            nClientAnimation = 64;
                break;
        case 10006:
            nClientAnimation = 81;
                break;
        case 10008:
            nClientAnimation = 83;
                break;
        case 10017:
            nClientAnimation = 63;
                break;
        case 10018:
            nClientAnimation = 65;
                break;
        case 10019:
            nClientAnimation = 67;
                break;
        case 10012:
            nClientAnimation = 301;
                break;
        case 10011:
            nClientAnimation = 302;
                break;
        case 10023:
            nClientAnimation = 77;
                break;
        case 10052:
            nClientAnimation = 7;
                break;
        case 10055:
            nClientAnimation = 12;
                break;
        case 10056:
            nClientAnimation = 13;
                break;
        case 10057:
            nClientAnimation = 14;
                break;
        case 10058:
            nClientAnimation = 15;
                break;
        case 10054:
            nClientAnimation = 10;
                break;
        case 10053:
            nClientAnimation = 11;
                break;
        case 10028:
            nClientAnimation = 33;
                break;
        case 10044:
            nClientAnimation = 17;
                break;
        case 10032:
        case 10033:
            nClientAnimation = 24;
                break;
        case 10035:
            nClientAnimation = 19;
                break;
        case 10059:
            nClientAnimation = 40;
                break;
        case 10060:
            nClientAnimation = 41;
                break;
        case 10061:
            nClientAnimation = 70;
                break;
        case 10039:
            nClientAnimation = 27;
                break;
        case 10040:
            nClientAnimation = 26;
                break;
        case 10041:
            nClientAnimation = 29;
                break;
        case 10042:
            nClientAnimation = 28;
                break;
        case 10038:
            nClientAnimation = 25;
                break;
        case 10070:
            nClientAnimation = 37;
                break;
        case 10029:
            nClientAnimation = 31;
                break;
        case 10030:
            nClientAnimation = 18;
                break;
        case 10034:
            nClientAnimation = 16;
                break;
        case 10014:
        case 10077:
            nClientAnimation = 303;
                break;
        case 10093:
            nClientAnimation = 1;
                break;
        case 10094:
            if (m_bRunLooksAwful)
            {
                nClientAnimation = 1;
                break;
            }
            else
            {
                nClientAnimation = 4;
                break;
            }
            break;
        case 10133:
            nClientAnimation = 5;
                break;
        case 10092:
            nClientAnimation = 8;
                break;
        case 10129:
            nClientAnimation = 57;
                break;
        case 10130:
            nClientAnimation = 58;
                break;
        case 10117:
            nClientAnimation = 75;
                break;
        case 10120:
            nClientAnimation = 32;
                break;
        case 10121:
            nClientAnimation = 44;
                break;
        case 10122:
            nClientAnimation = 53;
                break;
        case 10123:
            nClientAnimation = 54;
                break;
        case 10125:
            nClientAnimation = 43;
                break;
        case 10126:
            nClientAnimation = 68;
                break;
        case 10127:
        case 10136:
            nClientAnimation = 38;
                break;
        case 10118:
            nClientAnimation = 270;
                break;
        case 10124:
            nClientAnimation = 74;
                break;
        case 10128:
        case 10134:
            nClientAnimation = 47;
                break;
        case 10131:
        case 10135:
            nClientAnimation = 48;
                break;
        case 10132:
        case 10141:
            nClientAnimation = 51;
                break;
        case 10140:
            nClientAnimation = 52;
                break;
        case 10137:
            nClientAnimation = 76;
                break;
        case 10138:
            nClientAnimation = 78;
                break;
        case 10139:
            nClientAnimation = 79;
                break;
        case 10001:
        case 10148:
            switch (nWieldClass)
            {
            case 1:
                nClientAnimation = 92;
                break;
            case 2:
                nClientAnimation = 133;
                break;
            case 3:
                nClientAnimation = 174;
                break;
            case 4:
                nClientAnimation = 215;
                break;
            case 5:
                nClientAnimation = 223;
                break;
            case 6:
                nClientAnimation = 237;
                break;
            case 8:
                nClientAnimation = 249;
                break;
            case 7:
            case 9:
                nClientAnimation = 245;
                break;
            }
            break;
        case 10009:
        case 10109:
            nClientAnimation = 300;
                break;
        case 10142:
            nClientAnimation = 346;
                break;
        case 10145:
            nClientAnimation = 355;
                break;
        case 10147:
            nClientAnimation = 357;
                break;
        case CREATURE_ANIMATION_COMBAT_IDLE:
        case 10149:
            nClientAnimation = 6;
                break;
        case 10150:
            nClientAnimation = 72;
                break;
        case 10162:
            nClientAnimation = 69;
                break;
        case 10151:
            nClientAnimation = 359;
                break;
        case 10152:
            nClientAnimation = 360;
                break;
        case 10156:
            nClientAnimation = 375;
                break;
        case 10154:
            nClientAnimation = 370;
                break;
        case 10155:
            nClientAnimation = 371;
                break;
        case 10157:
            switch (nWieldClass)
            {
            case 2:
                nClientAnimation = 377;
                break;
            case 3:
                nClientAnimation = 378;
                break;
            case 4:
                nClientAnimation = 376;
                break;
            }
            break;
        case 10158:
            switch (nWieldClass)
            {
            case 2:
                nClientAnimation = 132;
                break;
            case 3:
                nClientAnimation = 173;
                break;
            case 4:
                nClientAnimation = 214;
                break;
            }
            break;
        case 10159:
            nClientAnimation = 34;
                break;
        case 10160:
            nClientAnimation = 35;
                break;
        case 10161:
            nClientAnimation = 71;
                break;
        case 10163:
            nClientAnimation = 384;
                break;
        case 10164:
            nClientAnimation = 385;
                break;
        case 10219:
        case 10220:
            nClientAnimation = 85;
                break;
        case 10221:
            nClientAnimation = 80;
                break;
        case 10222:
            nClientAnimation = 82;
                break;
        case 10223:
            nClientAnimation = 381;
                break;
        case 10224:
            nClientAnimation = 382;
                break;
        case 10246:
            switch (nWieldClass)
            {
            case 1:
                nClientAnimation = 91;
                break;
            case 2:
                nClientAnimation = 132;
                break;
            case 3:
                nClientAnimation = 173;
                break;
            case 4:
                nClientAnimation = 214;
                break;
            case 5:
                nClientAnimation = 222;
                break;
            case 6:
                nClientAnimation = 236;
                break;
            case 8:
                nClientAnimation = 373;
                break;
            case 7:
            case 9:
                nClientAnimation = 244;
                break;
            }
            break;
        case 10300:
            switch (nWieldClass)
            {
            case 2:
                nClientAnimation = 109;
                break;
            case 3:
                nClientAnimation = 151;
                break;
            case 4:
                nClientAnimation = 192;
                break;
            }
            break;
        case 10237:
        case 10238:
            nClientAnimation = 23;
                break;
        case 10271:
            nClientAnimation = 383;
                break;
        case 10301:
            switch (nWieldClass)
            {
            case 2:
                nClientAnimation = 110;
                break;
            case 3:
                nClientAnimation = 151;
                break;
            case 4:
                nClientAnimation = 192;
                break;
            }
            break;
        case 10401:
            nClientAnimation = 115;
                break;
        case 10400:
            nClientAnimation = 84;
            break;
        case 10402:
            nClientAnimation = 84;
            break;
        case 10302:
            switch (nWieldClass)
            {
            case 2:
                nClientAnimation = 124;
                break;
            case 4:
                nClientAnimation = 165;
                break;
            case 3:
                nClientAnimation = 206;
                break;
            case 5:
                nClientAnimation = 220;
                break;
            case 6:
                nClientAnimation = 234;
                break;
            case 8:
                nClientAnimation = 280;
                break;
            case 7:
            case 9:
                nClientAnimation = 242;
                break;
            }
            break;
        case 11000:
            nClientAnimation = 66;
                break;
        case 11001:
            nClientAnimation = 392;
                break;
        case 11002:
            nClientAnimation = 393;
                break;
        }
    }
    else
    {
        switch (nAnimation)
        {
        case 10028:
            nClientAnimation = 263;
                break;
        case 10023:
            nClientAnimation = 268;
                break;
        case 10038:
        case 10039:
        case 10040:
        case 10041:
        case 10042:
            nClientAnimation = 261;
                break;
        case 10015:
        case 10016:
            nClientAnimation = 379;
                break;
        case 10044:
            nClientAnimation = 260;
                break;
        case 10004:
        case 10094:
            if (m_bRunLooksAwful)
            {
                nClientAnimation = 253;
                break;
            }
            else
            {
                nClientAnimation = 255;
                break;
            }
            break;
        case 10093:
            nClientAnimation = 254;
                break;
        case 10052:
            nClientAnimation = 257;
                break;
        case 10054:
            nClientAnimation = 259;
                break;
        case 10053:
            nClientAnimation = 258;
                break;
        case 10117:
            nClientAnimation = 266;
                break;
        case 10002:
        case 10003:
        case 10084:
        case 10133:
            nClientAnimation = 253;
                break;
        case 10137:
            nClientAnimation = 267;
                break;
        case 10138:
            nClientAnimation = 269;
                break;
        case 10074:
            nClientAnimation = 299;
                break;
        case 10147:
            nClientAnimation = 358;
                break;
        case CREATURE_ANIMATION_COMBAT_IDLE:
        case 10055:
        case 10056:
        case 10057:
        case 10058:
        case 10092:
        case 10149:
            nClientAnimation = 256;
                break;
        case 10150:
            nClientAnimation = 264;
                break;
        case 10001:
        case 10148:
            nClientAnimation = 278;
                break;
        case 10006:
        case 10008:
        case 10156:
            nClientAnimation = 275;
                break;
        case 10154:
            nClientAnimation = 372;
                break;
        case 10152:
            nClientAnimation = 369;
                break;
        case 10219:
        case 10220:
            nClientAnimation = 272;
                break;
        case 10155:
        case 10030:
            nClientAnimation = 18;
                break;
        case 10009:
        case 10109:
            nClientAnimation = 300;
                break;
        case 10142:
            nClientAnimation = 346;
                break;
        case 10118:
            nClientAnimation = 270;
                break;
        case 10124:
            nClientAnimation = 74;
                break;
        case 10125:
            nClientAnimation = 43;
                break;
        case 10014:
            nClientAnimation = 280;
            break;
        case 10077:
            nClientAnimation = 280;
            break;
        case 10012:
            nClientAnimation = 301;
                break;
        case 10011:
            nClientAnimation = 302;
                break;
        case 10246:
            switch (nWieldClass)
            {
            case 1:
                nClientAnimation = 91;
                break;
            case 2:
                nClientAnimation = 132;
                break;
            case 3:
                nClientAnimation = 173;
                break;
            case 4:
                nClientAnimation = 214;
                break;
            case 5:
                nClientAnimation = 222;
                break;
            case 6:
                nClientAnimation = 236;
                break;
            case 7:
                nClientAnimation = 244;
                break;
            case 9:
                nClientAnimation = 244;
                break;
            }
            break;
        case 10221:
        case 10222:
            nClientAnimation = 274;
                break;
        case 10223:
        case 10224:
            nClientAnimation = 273;
                break;
        case 10401:
            nClientAnimation = 115;
                break;
        case 11001:
            nClientAnimation = 392;
                break;
        case 11000:
            nClientAnimation = 380;
                break;
        case 10400:
        case 10402:
            nClientAnimation = 271;
                break;
        case 11002:
            nClientAnimation = 393;
                break;
        case 10302:
            nClientAnimation = 280;
            break;
        }
    }
    return nClientAnimation;
}
