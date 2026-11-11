#include "nwcanimbase.h"

#include "aurmodel.h"
#include "nwrules.h"
#include "twodimarray.h"
#include "twodimarrays.h"

#include <stdio.h>

static inline void NwcanimbasetriggerTrace1();
#define NWCANIMBASETRIGGER_TRACE() NwcanimbasetriggerTrace1()
// PC 0x00756C70
CSWCAnimBaseTrigger::CSWCAnimBaseTrigger()
{
    m_nAnimationType = 9;
}
// PC 0x00756C90
CSWCAnimBaseTrigger::~CSWCAnimBaseTrigger()
{
    CSWCAnimBase::UnloadModel();
}
// PC 0x00756CE0
int CSWCAnimBaseTrigger::LoadModel(const CResRef& sResRef, unsigned char nType)
{
    Gob* pModel;
    char szModel[20];

    sResRef.CopyToString(szModel);
    pModel = static_cast<Gob*>(NewCAurObject(szModel, "trigger", 0, 0));
    m_pModel = pModel;
    if (pModel == 0)
    {
        return 0;
    }

    m_resLoadedModel = sResRef;
    m_bPaletteReplaced = 0;
    return 1;
}
// PC 0x00756D50
CExoString CSWCAnimBaseTrigger::GetAnimationName(unsigned short nAnimation)
{
    NWCANIMBASETRIGGER_TRACE();
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
    return sName;
}
// PC 0x00756E10
unsigned short CSWCAnimBaseTrigger::ServerToClientAnimation(
    unsigned short nAnimation)
{
    NWCANIMBASETRIGGER_TRACE();
    char szBuffer[256];

    switch (nAnimation)
    {
    case 10143:
        nAnimation = 347;
        break;
    case 10144:
        nAnimation = 350;
        break;
    }
    if (nAnimation > 9999)
    {
        sprintf(szBuffer,
                "CSWCAnimBasePlaceable::ServerToClientAnimation(): Failed to "
                "map server anim %i to client anim.",
                nAnimation);
    }
    return nAnimation;
}

static inline void NwcanimbasetriggerTrace1()
{
}
