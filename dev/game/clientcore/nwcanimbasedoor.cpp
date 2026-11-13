#include "nwcanimbase.h"
#include "aurmodel.h"
#include "nwcanimbase.h"
#include "nwrules.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include <stdio.h>

static inline void NwcanimbasedoorTrace1();
#define NWCANIMBASEDOOR_TRACE() NwcanimbasedoorTrace1()
// PC 0x007563C0 GOG 0x007561D0
CSWCAnimBaseDoor::CSWCAnimBaseDoor()
{
    m_nAnimationType = 5;
}
// PC 0x007563E0 GOG 0x007561F0
CSWCAnimBaseDoor::~CSWCAnimBaseDoor()
{
    UnloadModel();
}
// PC 0x00756430 GOG 0x00756240
int CSWCAnimBaseDoor::LoadModel(const CResRef& sResRef, unsigned char nType)
{
    Gob* pModel;
    char szModel[20];
    char szError[128];

    if (nType != 0xFF)
    {
        return 0;
    }

    sResRef.CopyToString(szModel);
    pModel = static_cast<Gob*>(NewCAurObject(szModel, "door", 0, 0));
    m_pModel = pModel;
    if (pModel == 0)
    {
        sprintf(szError, "Cannot load door model '%s'.", szModel);
        return 0;
    }

    m_resLoadedModel = sResRef;
    m_bPaletteReplaced = 0;
    return 1;
}
// PC 0x007565E0 GOG 0x007562E0
int CSWCAnimBaseDoor::SetAnimationInternal(CAurObject* pObject,
                                           unsigned short nAnimation,
                                           float fSpeed, int nFlags,
                                           float fBlend)
{
    if (pObject != 0)
    {
        CExoString sName = GetAnimationName(nAnimation);
        if (fSpeed == 0.0f)
        {
            fSpeed = 1.0f;
        }

        switch (nAnimation)
        {
        case DOOR_ANIMATION_CLOSED:
        case DOOR_ANIMATION_OPEN1:
        case DOOR_ANIMATION_OPEN2:
        case DOOR_ANIMATION_DESTROYED:
        case DOOR_ANIMATION_BUSTED:
            static_cast<Gob*>(pObject)->PlayAnimation(sName.CStr(), fSpeed, 2,
                                                      fBlend);
            return 1;
        case 329:
        case 334:
        case 335:
        case 336:
        case 337:
        case 10014:
        case 10077:
        case 10213:
        case 10214:
        case 10215:
        case 10216:
            static_cast<Gob*>(pObject)->PlayAnimation(sName.CStr(), fSpeed, 3,
                                                      fBlend);
            return 1;
        default:
            static_cast<Gob*>(pObject)->PlayAnimation("", 1.0f, 0, fBlend);
            return 1;
        }
    }
    return 0;
}
// PC 0x007564D0 GOG 0x007564F0
CExoString CSWCAnimBaseDoor::GetAnimationName(unsigned short nAnimation)
{
    NWCANIMBASEDOOR_TRACE();
    unsigned short nClientAnimation = ServerToClientAnimation(nAnimation);
    CExoString sName;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pAnimations = p2DArrays->m_pAnimations;

    if (pAnimations->GetCExoStringEntry(nClientAnimation,
                                        CTwoDimArrays::COL2DA_ANIMATIONS_NAME,
                                        &sName) == 0)
    {
        sName = "";
    }

    if (sName.GetLength() <= 0)
    {
        CExoString sDiagnostic;

        sDiagnostic.Format("CSWCAnimBaseDoor::GetAnimationName(): No name for "
                           "server animation %d",
                           nAnimation);
    }

    return sName;
}
// PC 0x007567D0 GOG 0x00756600
unsigned short CSWCAnimBaseDoor::ServerToClientAnimation(
    unsigned short nAnimation)
{
    NWCANIMBASEDOOR_TRACE();
    char szBuffer[256];
    unsigned short nClientAnimation = nAnimation;

    switch (nAnimation)
    {
    case DOOR_ANIMATION_OPEN1:
        nClientAnimation = 331;
        break;
    case DOOR_ANIMATION_OPEN2:
        nClientAnimation = 332;
        break;
    case DOOR_ANIMATION_CLOSED:
        nClientAnimation = 333;
        break;
    case 10072:
        nClientAnimation = m_bUseable ? 330 : 327;
        break;
    case 10014:
    case 10077:
    case 10213:
    case 10214:
    case 10215:
    case 10216:
        nClientAnimation = 328;
        break;
    case 10269:
        nClientAnimation = 344;
        break;
    case DOOR_ANIMATION_BUSTED:
        nClientAnimation = 366;
        break;
    default:
        if (nAnimation > 9999)
        {
            sprintf(szBuffer,
                    "CSWCAnimBasePlaceable::ServerToClientAnimation(): Failed "
                    "to map server anim %i to client anim.",
                    nAnimation);
        }
        break;
    }
    return nClientAnimation;
}

static inline void NwcanimbasedoorTrace1()
{
}
