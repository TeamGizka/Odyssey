#include "nwcanimbase.h"

#include "aurmodel.h"
#include "nwrules.h"
#include "twodimarray.h"
#include "twodimarrays.h"

#include <stdio.h>

static inline void NwcanimbasetriggerTrace1();
#define NWCANIMBASETRIGGER_TRACE() NwcanimbasetriggerTrace1()
// PC 0x006E5F40  Xbox 0x0011EB60
CSWCAnimBaseTrigger::CSWCAnimBaseTrigger()
{
    m_nAnimationType = 9;
}
// PC 0x006E5F60  Xbox 0x0011EB80
CSWCAnimBaseTrigger::~CSWCAnimBaseTrigger()
{
    CSWCAnimBase::UnloadModel();
}
// PC 0x006E5FB0  Xbox 0x0011EBD0
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
// PC 0x006E6040  Xbox 0x0011EC70
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
// PC 0x006E6100  Xbox 0x0011ED50
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
