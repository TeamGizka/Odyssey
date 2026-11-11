#include "exobase.h"
#include "resgff.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "nwsobject.h"
#include "nwseffectlist.h"
#include "nwseffect.h"
#include "nwsspellimpact.h"

const unsigned short CGameEffect::DURATION_TYPE_INSTANTANEOUS = 0;
const unsigned short CGameEffect::DURATION_TYPE_EQUIPPED = 3;
const unsigned short CGameEffect::DURATION_TYPE_TEMPORARY = 1;
const unsigned short CGameEffect::DURATION_TYPE_PERMANENT = 2;
const unsigned short CGameEffect::DURATION_TYPE_INNATE = 4;
const unsigned short CGameEffect::SUBTYPE_MAGICAL = 8;
const unsigned short CGameEffect::SUBTYPE_SUPERNATURAL = 16;
const unsigned short CGameEffect::SUBTYPE_EXTRAORDINARY = 24;
unsigned __int64 CGameEffect::m_nNextID = 1;
// PC 0x00540BF0
CGameEffect::~CGameEffect()
{
    if (m_pLinkLeft != NULL)
    {
        delete m_pLinkLeft;
        m_pLinkLeft = NULL;
    }

    if (m_pLinkRight != NULL)
    {
        delete m_pLinkRight;
        m_pLinkRight = NULL;
    }

    if (m_pIntegers != NULL)
    {
        delete m_pIntegers;
        m_pIntegers = NULL;
    }
}
// PC 0x00540C90
void CGameEffect::SetNumIntegers(int nNumIntegers)
{
    m_nNumIntegers = nNumIntegers;

    if (m_pIntegers != NULL)
    {
        delete m_pIntegers;
        m_pIntegers = NULL;
    }

    m_pIntegers = new int[m_nNumIntegers];
    memset(m_pIntegers, 0, m_nNumIntegers * 4);
}
// PC 0x00540CF0
int CGameEffect::GetInteger(int nIndex)
{
    return m_pIntegers[nIndex];
}
// PC 0x00540D00
void CGameEffect::SetInteger(int nIndex, int nValue)
{
    EXOXBOXASSERT(nIndex >= 0 && nIndex < m_nNumIntegers);
    m_pIntegers[nIndex] = nValue;
}
// PC 0x00540D20
float CGameEffect::GetFloat(int nIndex)
{
    EXOXBOXASSERT(nIndex >= 0 && nIndex < 4);
    return m_fFloat[nIndex];
}
// PC 0x00540D30
void CGameEffect::SetFloat(int nIndex, float fValue)
{
    EXOXBOXASSERT(nIndex >= 0 && nIndex < 4);
    m_fFloat[nIndex] = fValue;
}
// PC 0x00540D40
unsigned long CGameEffect::GetObjectID(int nIndex)
{
    EXOXBOXASSERT(nIndex >= 0 && nIndex < 4);
    return m_oidObject[nIndex];
}
// PC 0x00540D50
void CGameEffect::SetObjectID(int nIndex, unsigned long oidValue)
{
    EXOXBOXASSERT(nIndex >= 0 && nIndex < 4);
    m_oidObject[nIndex] = oidValue;
}
// PC 0x00540D60
CExoString CGameEffect::GetString(int nIndex)
{
    EXOXBOXASSERT(nIndex >= 0 && nIndex < 6);
    return m_sString[nIndex];
}
// PC 0x00540D90
void CGameEffect::SetString(int nIndex, const CExoString& sValue)
{
    EXOXBOXASSERT(nIndex >= 0 && nIndex < 6);
    m_sString[nIndex] = sValue;
}
// PC 0x00540DB0
void CGameEffect::SetExpiryTime(unsigned long nDay, unsigned long nTime)
{
    m_nExpireDay = nDay;
    m_nExpireTime = nTime;
}
// PC 0x00540DD0
void CGameEffect::GetExpiryTime(unsigned long* pnDay, unsigned long* pnTime)
{
    *pnDay = m_nExpireDay;
    *pnTime = m_nExpireTime;
}
// PC 0x00540DF0
int CGameEffect::SaveGameEffect(CResGFF* pRes, CResStruct* pStruct)
{
    CResList list;
    CResStruct element;
    unsigned int i;
    int result;

    unsigned __int64 nID = m_nID;
    pRes->WriteFieldDWORD64(pStruct, nID, "Id");
    unsigned short nType = m_nType;
    pRes->WriteFieldWORD(pStruct, nType, "Type");
    pRes->WriteFieldWORD(pStruct, m_nSubType, "SubType");
    float fDuration = m_fDuration;
    pRes->WriteFieldFLOAT(pStruct, fDuration, "Duration");
    int bSkipOnLoad = m_bSkipOnLoad;
    pRes->WriteFieldBYTE(pStruct, bSkipOnLoad, "SkipOnLoad");
    unsigned long nExpireDay = m_nExpireDay;
    unsigned long nExpireTime = m_nExpireTime;
    pRes->WriteFieldDWORD(pStruct, nExpireDay, "ExpireDay");
    pRes->WriteFieldDWORD(pStruct, nExpireTime, "ExpireTime");
    unsigned long oidCreator = m_oidCreator;
    pRes->WriteFieldDWORD(pStruct, oidCreator, "CreatorId");
    unsigned long nSpellId = m_nSpellId;
    pRes->WriteFieldDWORD(pStruct, nSpellId, "SpellId");
    int bExposed = m_bExposed;
    pRes->WriteFieldINT(pStruct, bExposed, "IsExposed");
    int nNumIntegers = m_nNumIntegers;
    pRes->WriteFieldINT(pStruct, nNumIntegers, "NumIntegers");

    if (m_nNumIntegers > 0)
    {
        pRes->AddList(&list, pStruct, "IntList");
        for (i = 0; i < m_nNumIntegers; i++)
        {
            pRes->AddListElement(&element, &list, 3);
            int nValue = m_pIntegers[i];
            pRes->WriteFieldINT(&element, nValue, "Value");
        }
    }

    pRes->AddList(&list, pStruct, "FloatList");
#ifdef _XBOX
    for (unsigned int nFloat = 0; nFloat < 4; nFloat++)
    {
        pRes->AddListElement(&element, &list, 4);
        pRes->WriteFieldFLOAT(&element, GetFloat(nFloat), "Value");
    }
#else
    float* pFloat = m_fFloat;
    for (int nFloat = 4; nFloat != 0; nFloat--)
    {
        pRes->AddListElement(&element, &list, 4);
        pRes->WriteFieldFLOAT(&element, *pFloat++, "Value");
    }
#endif

    pRes->AddList(&list, pStruct, "StringList");
    for (i = 0; i < 6; i++)
    {
        pRes->AddListElement(&element, &list, 5);
        pRes->WriteFieldCExoString(&element, GetString(i), "Value");
    }

    pRes->AddList(&list, pStruct, "ObjectList");
    for (i = 0; i < 4; i++)
    {
        pRes->AddListElement(&element, &list, 6);
#ifdef _XBOX
        result = pRes->WriteFieldDWORD(&element, GetObjectID(i), "Value");
#else
        result = pRes->WriteFieldDWORD(&element, m_oidObject[i], "Value");
#endif
    }

    return result;
}
// PC 0x00541060
void CGameEffect::SetCreator(unsigned long oidCreator)
{
    CGameObject* pObject =
        g_pAppManager->m_pServerExoApp->GetGameObject(oidCreator);

    m_oidCreator = oidCreator;

    if (pObject != NULL)
    {
        if (pObject->AsSWSObject() != NULL)
        {
            m_nSpellId = pObject->AsSWSObject()->GetEffectSpellId();

            if (pObject->AsSWSAreaOfEffectObject() != NULL)
            {
                pObject->AsSWSAreaOfEffectObject();
                m_oidCreator = pObject->AsSWSAreaOfEffectObject()->m_oidCreator;
            }
        }
    }
}
// PC 0x005410D0
int CGameEffect::GetEffectTypeScriptConstant()
{
    int nResult = 0;
    int nType = m_nType;

    if (nType == CSWSEffectListHandler::EFFECT_INVALIDEFFECT)
    {
        return nResult;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE)
    {
        nResult = 1;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_REGENERATE)
    {
        nResult = 3;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_REDUCTION)
    {
        nResult = 7;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_TEMPORARY_HITPOINTS)
    {
        nResult = 9;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_ENTANGLE)
    {
        nResult = 11;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DEAF)
    {
        nResult = 13;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_IMMUNITY)
    {
        nResult = 15;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_ENEMY_ATTACK_BONUS)
    {
        nResult = 17;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_ARCANE_SPELL_FAILURE)
    {
        nResult = 18;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_AREA_OF_EFFECT)
    {
        nResult = 20;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_BEAM)
    {
        nResult = 21;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SETSTATE)
    {
        switch (m_pIntegers[0])
        {
        case 1:
            nResult = 24;
            break;
        case 2:
            nResult = 25;
            break;
        case 5:
            nResult = 27;
            break;
        case 4:
            nResult = 29;
            break;
        case 6:
            nResult = 30;
            break;
        case 3:
            nResult = 35;
            break;
        }
    }
    else if (nType == CSWSEffectListHandler::EFFECT_POISON)
    {
        nResult = 31;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DISEASE)
    {
        nResult = 32;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_CURSE)
    {
        nResult = 33;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SILENCE)
    {
        nResult = 34;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_HASTE)
    {
        nResult = 36;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SLOW)
    {
        nResult = 37;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_ABILITY_INCREASE)
    {
        nResult = 38;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_ABILITY_DECREASE)
    {
        nResult = 39;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_ATTACK_INCREASE)
    {
        nResult = 40;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_ATTACK_DECREASE)
    {
        nResult = 41;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE)
    {
        nResult = 42;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE)
    {
        nResult = 43;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_INCREASE)
    {
        nResult = 44;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_DECREASE)
    {
        nResult = 45;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_AC_INCREASE)
    {
        nResult = 46;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_AC_DECREASE)
    {
        nResult = 47;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_MOVEMENT_SPEED_INCREASE)
    {
        nResult = 48;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_MOVEMENT_SPEED_DECREASE)
    {
        nResult = 49;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SAVING_THROW_INCREASE)
    {
        nResult = 50;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SAVING_THROW_DECREASE)
    {
        nResult = 51;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_INCREASE)
    {
        nResult = 52;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_DECREASE)
    {
        nResult = 53;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SKILL_INCREASE)
    {
        nResult = 54;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SKILL_DECREASE)
    {
        nResult = 55;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_INVISIBILITY)
    {
        nResult = 56;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DARKNESS)
    {
        nResult = 58;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DISPEL_ALL_MAGIC)
    {
        nResult = 59;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DAMAGE_SHIELD)
    {
        nResult = 60;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_NEGATIVE_LEVEL)
    {
        nResult = 61;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DISGUISE)
    {
        nResult = 62;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SANCTUARY)
    {
        nResult = 63;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_TRUESEEING)
    {
        nResult = 64;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SEEINVISIBLE)
    {
        nResult = 65;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_TIMESTOP)
    {
        nResult = 66;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_BLINDNESS)
    {
        nResult = 67;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SPELL_LEVEL_ABSORPTION)
    {
        nResult = 68;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_DISPEL_BEST_MAGIC)
    {
        nResult = 69;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_ULTRAVISION)
    {
        nResult = 70;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_MISS_CHANCE)
    {
        nResult = 71;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_CONCEALMENT)
    {
        nResult = 72;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_SPELL_IMMUNITY)
    {
        nResult = 73;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_ASSUREDHIT)
    {
        nResult = 74;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_VISUALEFFECT)
    {
        nResult = 75;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_LIGHTSABERTHROW)
    {
        nResult = 76;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_FORCEJUMP)
    {
        nResult = 77;
    }
    else if (nType == CSWSEffectListHandler::EFFECT_ASSUREDDEFLECTION)
    {
        nResult = 78;
    }

    return nResult;
}
// PC 0x005414E0
CGameEffect::CGameEffect(int bDynamic)
{
    if (bDynamic == 1)
    {
        m_nID = m_nNextID++;
    }
    else
    {
        m_nID = 0;
    }

    m_bExposed = bDynamic;
    m_bSkipOnLoad = 0;
    m_nType = 0;
    m_nSubType = 0;
    m_fDuration = 0.0f;
    m_nExpireDay = 0;
    m_nExpireTime = 0;
    m_oidCreator = OBJECT_INVALID;
    m_pLinkLeft = NULL;
    m_pLinkRight = NULL;
    m_nCasterLevel = -1;
    m_nSpellId = -1;
    m_pIntegers = NULL;
    m_nNumIntegers = 8;
    m_pIntegers = new int[m_nNumIntegers];
    memset(m_pIntegers, 0, m_nNumIntegers * 4);

    memset(m_fFloat, 0, sizeof(m_fFloat));

    for (int nString = 0; nString < 6; nString++)
    {
        m_sString[nString] = "";
    }

    for (int nObject = 0; nObject < 4; nObject++)
    {
        m_oidObject[nObject] = OBJECT_INVALID;
    }
}
// PC 0x00541610
CGameEffect::CGameEffect(CGameEffect* pEffect)
{
    m_nID = 0;
    m_bExposed = 0;
    m_nType = 0;
    m_nSubType = 0;
    m_fDuration = 0.0f;
    m_nExpireDay = 0;
    m_nExpireTime = 0;
    m_oidCreator = OBJECT_INVALID;
    m_pLinkLeft = NULL;
    m_pLinkRight = NULL;
    m_pIntegers = NULL;
    m_nSpellId = -1;
    m_nCasterLevel = 0;
    SetNumIntegers(pEffect->m_nNumIntegers);

    memset(m_fFloat, 0, sizeof(m_fFloat));

    for (int nString = 0; nString < 6; nString++)
    {
        m_sString[nString] = "";
    }

    for (int nObject = 0; nObject < 4; nObject++)
    {
        m_oidObject[nObject] = OBJECT_INVALID;
    }

    m_nID = pEffect->m_nID;
    m_nSubType = pEffect->m_nSubType;
    m_fDuration = pEffect->m_fDuration;
    m_nExpireDay = pEffect->m_nExpireDay;
    m_nExpireTime = pEffect->m_nExpireTime;
    m_oidCreator = pEffect->m_oidCreator;
    m_nSpellId = pEffect->m_nSpellId;
    m_bSkipOnLoad = 0;
}
// PC 0x00541730
void CGameEffect::CopyEffect(CGameEffect* pEffect, int bSkipIntegers)
{
    if (pEffect != NULL)
    {
        if (m_nID == 0)
        {
            m_nID = pEffect->m_nID;
        }

        m_nType = pEffect->m_nType;
        m_nSubType = pEffect->m_nSubType;
        m_bExposed = pEffect->m_bExposed;
        m_fDuration = pEffect->m_fDuration;
        m_nExpireDay = pEffect->m_nExpireDay;
        m_nExpireTime = pEffect->m_nExpireTime;
        m_oidCreator = pEffect->m_oidCreator;
        m_nSpellId = pEffect->m_nSpellId;
        m_nCasterLevel = pEffect->m_nCasterLevel;

        if (pEffect->m_pLinkLeft != NULL)
        {
            m_pLinkLeft = new CGameEffect(0);
            m_pLinkLeft->CopyEffect(pEffect->m_pLinkLeft, 0);
        }

        if (pEffect->m_pLinkRight != NULL)
        {
            m_pLinkRight = new CGameEffect(0);
            m_pLinkRight->CopyEffect(pEffect->m_pLinkRight, 0);
        }

        SetNumIntegers(pEffect->m_nNumIntegers);

        if (bSkipIntegers == 0)
        {
            for (int i = 0; i < m_nNumIntegers; i++)
            {
                m_pIntegers[i] = pEffect->m_pIntegers[i];
            }

            m_fFloat[0] = pEffect->m_fFloat[0];
            m_fFloat[1] = pEffect->m_fFloat[1];
            m_fFloat[2] = pEffect->m_fFloat[2];
            m_fFloat[3] = pEffect->m_fFloat[3];
            m_oidObject[0] = pEffect->m_oidObject[0];
            m_oidObject[1] = pEffect->m_oidObject[1];
            m_oidObject[2] = pEffect->m_oidObject[2];
            m_oidObject[3] = pEffect->m_oidObject[3];

            for (int j = 0; j < 6; j++)
            {
                m_sString[j] = pEffect->m_sString[j];
            }
        }
    }
}
// PC 0x005418E0
void CGameEffect::UpdateLinked()
{
    if (m_pLinkLeft != NULL)
    {
        switch (m_nSubType & SUBTYPE_MASK)
        {
        case SUBTYPE_MAGICAL:
            m_pLinkLeft->m_nSubType =
                (m_pLinkLeft->m_nSubType & ~SUBTYPE_SUPERNATURAL) |
                SUBTYPE_MAGICAL;
            break;
        case SUBTYPE_SUPERNATURAL:
            m_pLinkLeft->m_nSubType =
                (m_pLinkLeft->m_nSubType & ~SUBTYPE_MAGICAL) |
                SUBTYPE_SUPERNATURAL;
            break;
        case SUBTYPE_MASK:
            m_pLinkLeft->m_nSubType |= SUBTYPE_MASK;
            break;
        }

        m_pLinkLeft->UpdateLinked();
        m_pLinkLeft->SetCreator(m_oidCreator);
        float fDuration = m_fDuration;
        m_pLinkLeft->m_fDuration = fDuration;
        m_pLinkLeft->m_nSubType ^=
            ((m_pLinkLeft->m_nSubType ^ m_nSubType) & DURATION_MASK);
        m_pLinkLeft->m_nExpireDay = m_nExpireDay;
        m_pLinkLeft->m_nExpireTime = m_nExpireTime;
        m_pLinkLeft->m_nSpellId = m_nSpellId;
        m_pLinkLeft->m_nID = m_nID;
    }

    if (m_pLinkRight != NULL)
    {
        switch (m_nSubType & SUBTYPE_MASK)
        {
        case SUBTYPE_MAGICAL:
            m_pLinkRight->m_nSubType =
                (m_pLinkRight->m_nSubType & ~SUBTYPE_SUPERNATURAL) |
                SUBTYPE_MAGICAL;
            break;
        case SUBTYPE_SUPERNATURAL:
            m_pLinkRight->m_nSubType =
                (m_pLinkRight->m_nSubType & ~SUBTYPE_MAGICAL) |
                SUBTYPE_SUPERNATURAL;
            break;
        case SUBTYPE_MASK:
            m_pLinkRight->m_nSubType |= SUBTYPE_MASK;
            break;
        }

        m_pLinkRight->UpdateLinked();
        m_pLinkRight->SetCreator(m_oidCreator);
        float fDuration = m_fDuration;
        m_pLinkRight->m_fDuration = fDuration;
        m_pLinkRight->m_nSubType ^=
            ((m_pLinkRight->m_nSubType ^ m_nSubType) & DURATION_MASK);
        m_pLinkRight->m_nExpireDay = m_nExpireDay;
        m_pLinkRight->m_nExpireTime = m_nExpireTime;
        m_pLinkRight->m_nSpellId = m_nSpellId;
        m_pLinkRight->m_nID = m_nID;
    }
}
// PC 0x00541A40
void CGameEffect::LoadGameEffect(CResGFF* pRes, CResStruct* pStruct)
{
    CResList list;
    CResStruct element;
    CExoString sValue;
    int bSuccess;

    m_nID = pRes->ReadFieldDWORD64(pStruct, "Id", bSuccess, 0);
    int nType = pRes->ReadFieldWORD(pStruct, "Type", bSuccess, 0);
    m_nType = nType;
    m_nSubType = pRes->ReadFieldWORD(pStruct, "SubType", bSuccess, 0);
    m_fDuration = pRes->ReadFieldFLOAT(pStruct, "Duration", bSuccess, 0.0f);
    int nSkip = pRes->ReadFieldBYTE(pStruct, "SkipOnLoad", bSuccess, 0);
    m_bSkipOnLoad = nSkip;
    unsigned long nExpireDay =
        pRes->ReadFieldDWORD(pStruct, "ExpireDay", bSuccess, 0);
    unsigned long nExpireTime =
        pRes->ReadFieldDWORD(pStruct, "ExpireTime", bSuccess, 0);
    m_nExpireDay = nExpireDay;
    m_nExpireTime = nExpireTime;
    SetCreator(pRes->ReadFieldDWORD(pStruct, "CreatorId", bSuccess, 0));
    m_nSpellId = pRes->ReadFieldDWORD(pStruct, "SpellId", bSuccess, 0);
    m_bExposed = pRes->ReadFieldINT(pStruct, "IsExposed", bSuccess, 0);
    SetNumIntegers(pRes->ReadFieldINT(pStruct, "NumIntegers", bSuccess, 0));

    if ((GetEffectType() == CSWSEffectListHandler::EFFECT_DAMAGE ||
         GetEffectType() == CSWSEffectListHandler::EFFECT_POISON) &&
        m_nNumIntegers < 21)
    {
        unsigned int nOldNum = m_nNumIntegers - 6;

        if (pRes->GetList(&list, pStruct, "IntList") == 1)
        {
            unsigned int nIndex = 0;
            unsigned int nElement = 0;

            do
            {
                if (nIndex < nOldNum)
                {
                    if (nElement < pRes->GetListCount(&list))
                    {
                        pRes->GetListElement(&element, &list, nElement);
#ifdef _XBOX
                        SetInteger(nElement,
                            pRes->ReadFieldINT(&element, "Value", bSuccess, 0));
#else
                        m_pIntegers[nElement] =
                            pRes->ReadFieldINT(&element, "Value", bSuccess, 0);
#endif
                        nElement++;
                        nIndex++;
                    }
                }
                else if (nIndex < 15)
                {
#ifdef _XBOX
                    SetInteger(nElement, 0);
#else
                    m_pIntegers[nElement] = 0;
#endif
                    nIndex++;
                }
                else
                {
                    if (nElement < pRes->GetListCount(&list))
                    {
                        pRes->GetListElement(&element, &list, nElement);
#ifdef _XBOX
                        SetInteger(nElement,
                            pRes->ReadFieldINT(&element, "Value", bSuccess, 0));
#else
                        m_pIntegers[nElement] =
                            pRes->ReadFieldINT(&element, "Value", bSuccess, 0);
#endif
                        nElement++;
                        nIndex++;
                    }
                }
            } while (nIndex < 21);
        }

        SetNumIntegers(21);
    }
    else
    {
        if (m_nNumIntegers > 0 && pRes->GetList(&list, pStruct, "IntList") == 1)
        {
            for (unsigned int i = 0; i < pRes->GetListCount(&list); i++)
            {
                pRes->GetListElement(&element, &list, i);
#ifdef _XBOX
                SetInteger(i,
                    pRes->ReadFieldINT(&element, "Value", bSuccess, 0));
#else
                m_pIntegers[i] =
                    pRes->ReadFieldINT(&element, "Value", bSuccess, 0);
#endif
            }
        }
    }

    if (pRes->GetList(&list, pStruct, "FloatList") == 1)
    {
        unsigned int j = 0;
        float* pFloat = m_fFloat;
        for (; j < 4; j++, pFloat++)
        {
            pRes->GetListElement(&element, &list, j);
#ifdef _XBOX
            SetFloat(j, pRes->ReadFieldFLOAT(&element, "Value", bSuccess, 0.0f));
#else
            *pFloat = pRes->ReadFieldFLOAT(&element, "Value", bSuccess, 0.0f);
#endif
        }
    }

    if (pRes->GetList(&list, pStruct, "StringList") == 1)
    {
        unsigned int k = 0;
        CExoString* pString = m_sString;
        for (; k < 6; k++, pString++)
        {
            pRes->GetListElement(&element, &list, k);
            {
                CExoString sDefault("");
                sValue = pRes->ReadFieldCExoString(&element, "Value", bSuccess,
                                                   sDefault);
            }
#ifdef _XBOX
            SetString(k, sValue);
#else
            *pString = sValue;
#endif
        }
    }

    if (pRes->GetList(&list, pStruct, "ObjectList") == 1)
    {
        unsigned int m = 0;
        unsigned long* pObject = m_oidObject;
        for (; m < 4; m++, pObject++)
        {
            pRes->GetListElement(&element, &list, m);
#ifdef _XBOX
            SetObjectID(m, pRes->ReadFieldDWORD(&element, "Value", bSuccess, 0));
#else
            *pObject = pRes->ReadFieldDWORD(&element, "Value", bSuccess, 0);
#endif
        }
    }
}
// PC 0x00541E90
void CGameEffect::SetLinked(CGameEffect* pLeft, CGameEffect* pRight)
{
    m_pLinkLeft = pLeft;
    m_pLinkRight = pRight;
    UpdateLinked();
}
