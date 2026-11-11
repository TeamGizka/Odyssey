#include "nwportrait.h"
#include "exobase.h"
#include "exoresman.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "nwclass.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "nwrules.h"
// PC 0x004015A0
CSWPortrait::~CSWPortrait()
{
}
// PC 0x00678EC0  Xbox 0x00093060
CSWPortrait::CSWPortrait()
{
    CResRef::operator=("");
}
// PC 0x00678EF0  Xbox 0x00093090
CResRef CSWPortrait::GetPortrait(unsigned char nSize)
{
    CResRef cPortrait = *this;

    if (*cPortrait.GetResRef() == 0)
    {
        return CResRef("");
    }

    switch (nSize)
    {
    case 1:
        cPortrait += "T";
        break;
    case 2:
        cPortrait += "S";
        break;
    case 3:
        cPortrait += "M";
        break;
    case 4:
        cPortrait += "L";
        break;
    case 5:
        cPortrait += "H";
        break;
    }
    return cPortrait;
}
// PC 0x006790D0  Xbox 0x000932A0
int CSWPortrait::SetPortraitId(unsigned short nPortraitId)
{
    CExoString sResRef;
    CExoString sPrefix;

    if (nPortraitId >= 0xFFFE)
    {
        return 0;
    }

    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nGoreLevel ==
        0)
    {
        bool bMissing;
        {
            CExoString sColumn("LowGore");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* p2DA = p2DArrays->m_pPortrait;
            bMissing =
                p2DA->GetCExoStringEntry(nPortraitId, sColumn, &sResRef) == 0;
        }
        if (bMissing)
        {
            {
                CExoString sColumn("BaseResRef");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* p2DA = p2DArrays->m_pPortrait;
                bMissing = p2DA->GetCExoStringEntry(nPortraitId, sColumn,
                                                    &sResRef) == 0;
            }
            if (bMissing)
            {
                sPrefix = "";
                sResRef = "";
            }
        }
    }
    else
    {
        bool bMissing;
        {
            CExoString sColumn("BaseResRef");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* p2DA = p2DArrays->m_pPortrait;
            bMissing =
                p2DA->GetCExoStringEntry(nPortraitId, sColumn, &sResRef) == 0;
        }
        if (bMissing)
        {
            sPrefix = "";
            sResRef = "";
        }
    }

    sResRef = sPrefix + sResRef;
    CResRef::operator=(sResRef);
    return 1;
}
