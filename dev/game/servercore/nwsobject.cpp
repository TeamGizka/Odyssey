#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "gameobjectarray.h"
#include "twodimarrays.h"
#include "nwsscriptevent.h"
#include "nwportrait.h"
#include "nwseffectlist.h"
#include "swmain.h"

#include "math3d.h"
#include "pathfindinfo.h"
#include "serverexoapp.h"
#include "worldtimer.h"
#include "nwsexpression.h"
#include "nwseffect.h"
#include "nwscombatround.h"
#include "guiingame.h"
#include "nwcmodule.h"
#include "nwcobject.h"
#include "nwfeat.h"
#include "nwmessage.h"
#include "nwrules.h"
#include "nwspell.h"
#include "nwsarea.h"
#include "nwscreaturestats.h"
#include "nwsitem.h"
#include "nwsobject.h"
#include "nwsplaceable.h"
#include "nwsplayer.h"
#include "nwsplayerluo.h"
#include "nwsspellimpact.h"
#include "serveraimaster.h"
#include "exotlktable.h"
#include "nwitem.h"
#include "nwscript.h"
#include "resgff.h"
#include "twodimarray.h"
#include "swparty.h"
#include "clientexoappinternal.h"
#include "nwclass.h"

static inline void NwsobjectTrace2();
static inline void NwsobjectTrace1();

inline void NwsobjectDestroyScriptVar(CSWSScriptVar* pVar)
{
    pVar->~CSWSScriptVar();
}

const int CSWSObject::ACTION_IN_PROGRESS = 1;
const int CSWSObject::ACTION_COMPLETE = 2;
const int CSWSObject::ACTION_FAILED = 3;
const int CSWSObject::ACTION_IN_DELAY = 4;
const unsigned long CSWSObject::AI_TIME_SLICE = 1000;
// PC 0x004F6080
void CSWSDialog::AddPlayer(CSWSDialogPlayer* pPlayer)
{
    if (pPlayer != NULL)
        m_lstPlayers.AddTail(pPlayer);
}
// PC 0x004F60A0
inline unsigned long CSWSDialog::GetCurrentPlayerId()
{
    if (m_lstPlayers.GetHeadPos() != NULL)
    {
        return m_oidCurrentPlayer;
    }
    return OBJECT_INVALID;
}
// PC 0x004F60C0
CSWSObjectActionNode::~CSWSObjectActionNode()
{
    int i = 0;

    if (m_nNumParams > 0)
    {
        do
        {
            if (m_apParam[i] != NULL)
            {
                if (m_anParamType[i] == 4)
                {
                    delete (CExoString*)m_apParam[i];
                    m_apParam[i] = NULL;
                }
                else if (m_anParamType[i] == 5 && g_pVirtualMachine != NULL)
                {
                    g_pVirtualMachine->DeleteScriptSituation(m_apParam[i]);
                    m_apParam[i] = NULL;
                }
            }
            i++;
        } while (i < m_nNumParams);
    }
}

void CSWSObject::SaveVarTable(CResGFF* pRes, CResStruct* pStruct)
{
    m_lScriptVars.SaveVarTable(pRes, pStruct);
    m_lVarTable.SaveVarTable(pRes, pStruct);
}
// PC 0x004F8A10
int CSWSObject::SaveObjectState(CResGFF* pRes, CResStruct* pStruct)
{
    SaveEffectList(pRes, pStruct);
    SaveVarTable(pRes, pStruct);
    SaveActionQueue(pRes, pStruct);
    return pRes->WriteFieldBYTE(pStruct, m_bCommandable, "Commandable");
}

void CSWSObject::TestActionList()
{
    CExoLinkedListNode* pPos = m_lstActions.GetHeadPos();
    if (m_lstActions.GetHead() != NULL)
    {
        while (m_lstActions.GetNext(pPos) != NULL)
        {
        }
    }
}
// PC 0x00406590
unsigned long CSWSObject::GetEffectSpellId()
{
    return CGameEffect::SPELL_ID_INVALID;
}
// PC 0x004015A0
CScriptLocation::~CScriptLocation()
{
}
// PC 0x004F44A0
CScriptLocation::CScriptLocation()
{
}
// PC 0x004F44C0
void CScriptLocation::CopyScriptLocation(CScriptLocation* pLocation)
{
    if (pLocation != NULL)
    {
        m_vOrientation = pLocation->m_vOrientation;
        m_vPosition = pLocation->m_vPosition;
    }
}
// PC 0x004F4500
int CScriptLocation::LoadLocation(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;
    Vector vOrientation;
    Vector vPosition;

    vOrientation.m_fX =
        pRes->ReadFieldFLOAT(pStruct, "OrientationX", bSuccess, 0.0f);
    vOrientation.m_fY =
        pRes->ReadFieldFLOAT(pStruct, "OrientationY", bSuccess, 0.0f);
    vOrientation.m_fZ =
        pRes->ReadFieldFLOAT(pStruct, "OrientationZ", bSuccess, 0.0f);
    m_vOrientation = vOrientation;
    vPosition.m_fX = pRes->ReadFieldFLOAT(pStruct, "PositionX", bSuccess, 0.0f);
    vPosition.m_fY = pRes->ReadFieldFLOAT(pStruct, "PositionY", bSuccess, 0.0f);
    vPosition.m_fZ = pRes->ReadFieldFLOAT(pStruct, "PositionZ", bSuccess, 0.0f);
    m_vPosition = vPosition;
    return 1;
}
// PC 0x004F45E0
int CScriptLocation::SaveLocation(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fX, "OrientationX");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fY, "OrientationY");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fZ, "OrientationZ");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fX, "PositionX");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fY, "PositionY");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fZ, "PositionZ");
    return 1;
}
// PC 0x004F4660
int CLoopingVisualEffect::GetIsBeam(unsigned short nVisualEffectId)
{
    return nVisualEffectId == 2037 || nVisualEffectId == 2038 ||
           nVisualEffectId == 6000 || nVisualEffectId == 2026 ||
           nVisualEffectId == 2027 || nVisualEffectId == 2028 ||
           nVisualEffectId == 2049 || nVisualEffectId == 2050 ||
           nVisualEffectId == 2051 || nVisualEffectId == 2052 ||
           nVisualEffectId == 2053 || nVisualEffectId == 4037 ||
           nVisualEffectId == 2029 || nVisualEffectId == 2061 ||
           nVisualEffectId == 2065 || nVisualEffectId == 2066;
}
// PC 0x004F46E0
int CLoopingVisualEffect::GetIsBeam()
{
    return GetIsBeam(m_nVisualEffectId);
}
// PC 0x004015A0
CScriptTalent::~CScriptTalent()
{
}
// PC 0x004F46F0
CScriptTalent::CScriptTalent()
{
    m_nID = -1;
    m_nType = -1;
    m_nItemPropertyIndex = -1;
    m_nMultiClass = 0;
    m_oidItem = OBJECT_INVALID;
    m_nCasterLevel = -1;
    m_nMetaType = -1;
}
// PC 0x004F4720
void CScriptTalent::CopyScriptTalent(CScriptTalent* pTalent)
{
    if (pTalent != NULL)
    {
        m_nID = pTalent->m_nID;
        m_nType = pTalent->m_nType;
        m_nMultiClass = pTalent->m_nMultiClass;
        m_oidItem = pTalent->m_oidItem;
        m_nItemPropertyIndex = pTalent->m_nItemPropertyIndex;
        m_nCasterLevel = pTalent->m_nCasterLevel;
        m_nMetaType = pTalent->m_nMetaType;
    }
}
// PC 0x004F4760
int CScriptTalent::SaveTalent(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldINT(pStruct, m_nID, "ID");
    pRes->WriteFieldINT(pStruct, m_nType, "Type");
    pRes->WriteFieldBYTE(pStruct, m_nMultiClass, "MultiClass");
    pRes->WriteFieldDWORD(pStruct, m_oidItem, "Item");
    pRes->WriteFieldINT(pStruct, m_nItemPropertyIndex, "ItemPropertyIndex");
    pRes->WriteFieldBYTE(pStruct, m_nCasterLevel, "CasterLevel");
    pRes->WriteFieldBYTE(pStruct, m_nMetaType, "MetaType");
    return 1;
}
// PC 0x004F4800
int CScriptTalent::LoadTalent(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;

    m_nID = pRes->ReadFieldINT(pStruct, "ID", bSuccess, 0);
    m_nType = pRes->ReadFieldINT(pStruct, "Type", bSuccess, 0);
    m_nMultiClass = pRes->ReadFieldBYTE(pStruct, "MultiClass", bSuccess, 0);
    m_oidItem = pRes->ReadFieldDWORD(pStruct, "Item", bSuccess, 0);
    m_nItemPropertyIndex =
        pRes->ReadFieldINT(pStruct, "ItemPropertyIndex", bSuccess, 0);
    m_nCasterLevel = pRes->ReadFieldBYTE(pStruct, "CasterLevel", bSuccess, 0);
    m_nMetaType = pRes->ReadFieldBYTE(pStruct, "MetaType", bSuccess, 0);
    return 1;
}
// PC 0x004F48C0
int CScriptTalent::Equals(CScriptTalent* pTalent)
{
    return m_nType == pTalent->m_nType && m_nID == pTalent->m_nID &&
           m_nMultiClass == pTalent->m_nMultiClass &&
           m_oidItem == pTalent->m_oidItem &&
           m_nItemPropertyIndex == pTalent->m_nItemPropertyIndex &&
           m_nCasterLevel == pTalent->m_nCasterLevel &&
           m_nMetaType == pTalent->m_nMetaType;
}
// PC 0x004F4910
CSWSObjectActionNode::CSWSObjectActionNode()
{
    for (int i = 0; i < 13; i++)
    {
        m_anParamType[i] = 0;
        m_apParam[i] = NULL;
    }
    m_nNumParams = 0;
}
// PC 0x004F4940
void CSWSObject::AddActionNodeParameter(CSWSObjectActionNode* pNode,
                                        unsigned long nIndex,
                                        unsigned long nType, void* pData)
{
    pNode->m_nNumParams = nIndex + 1;
    if (nIndex >= CSWSObjectActionNode::MAX_ACTION_PARAMETERS)
    {
        return;
    }

    pNode->m_anParamType[nIndex] = nType;
    if (nType == ACTION_PARAM_INTEGER)
    {
        pNode->m_apParam[nIndex] = (void*)*(int*)pData;
    }
    if (nType == ACTION_PARAM_FLOAT)
    {
        *(float*)&pNode->m_apParam[nIndex] = *(float*)pData;
    }
    if (nType == ACTION_PARAM_STRING)
    {
        CExoString* pString = new CExoString;

        pNode->m_apParam[nIndex] = pString;
        *pString = *(CExoString*)pData;
    }
    if (nType == ACTION_PARAM_OBJECT)
    {
        pNode->m_apParam[nIndex] = (void*)*(unsigned long*)pData;
    }
    if (nType == ACTION_PARAM_SCRIPT_SITUATION)
    {
        pNode->m_apParam[nIndex] = pData;
    }
}
// PC 0x004F4A30
void CSWSObject::SetPortraitId(unsigned short nPortraitId)
{
    CExoString sPrefix("po_");

    if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
    {
        CResRef cPortrait("");
        SetPortrait(cPortrait);
    }
    else
    {
        CExoString sResRef;

        {
            CExoString sColumn("BaseResRef");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPortraits = p2DArrays->m_pPortrait;
            pPortraits->GetCExoStringEntry(
                nPortraitId, sColumn, &sResRef);
        }
        sResRef = sPrefix + sResRef;
        {
            CResRef cPortrait((char*)sResRef.CStr());
            SetPortrait(cPortrait);
        }
    }

    m_nPortraitId = nPortraitId;
}
// PC 0x004F4B70
int CSWSObject::AnimationStationary(unsigned short nAnimation)
{
    return nAnimation != 10002 && nAnimation != 10003 && nAnimation != 10004 &&
           nAnimation != 10078 && nAnimation != 10079 && nAnimation != 10085 &&
           nAnimation != 10084 && nAnimation != 10087 && nAnimation != 10086 &&
           nAnimation != 10093 && nAnimation != 10094 && nAnimation != 10133;
}
// PC 0x004F4BD0
short CSWSObject::GetCurrentHitPoints(int bBaseOnly)
{
    if (bBaseOnly == 1)
    {
        return m_nCurrentHitPoints;
    }
    return m_nCurrentHitPoints + m_nTemporaryHitPoints;
}
// PC 0x004F4BF0
char CSWSObject::GetDamageImmunity(unsigned char nType)
{
    return max(-100, min(m_pDamageImmunity[nType], 100));
}
// PC 0x004F4C30
void CSWSObject::SetDamageImmunity(unsigned short nFlags, char nValue)
{
    if (nValue < -100)
    {
        nValue = -100;
    }
    else if (nValue > 100)
    {
        nValue = 100;
    }

    for (int i = 0; i < 15; i++)
    {
        if ((nFlags >> i) & 1)
        {
            m_pDamageImmunity[i] = nValue;
            return;
        }
    }
}
// PC 0x004F4C80
int CSWSObject::GetDamageImmunityByFlags(unsigned short nFlags)
{
    short nImmunity = 0;

    for (int i = 0; i < 15; i++)
    {
        if ((nFlags >> i) & 1)
        {
            if (nImmunity == 0)
            {
                nImmunity = GetDamageImmunity(i);
            }
            else if (GetDamageImmunity(i) < nImmunity)
            {
                nImmunity = GetDamageImmunity(i);
            }
        }
    }

    if (nImmunity < 100)
    {
        int nResult = nImmunity;
        if (nResult < -100)
        {
            return -100;
        }
        return nResult;
    }
    return 100;
}
// PC 0x004F4CF0
int CSWSObject::GetLastDamageAmountByFlags(int nFlags)
{
    for (int i = 0; i < 15; i++)
    {
        if ((1 << i) == nFlags)
        {
            return m_pLastDamageAmounts[i];
        }
    }
    return 0;
}
// PC 0x004F4D30
unsigned char CSWSObject::GetDamageLevel()
{
    float fHitPointRatio;

    if (AsSWSCreature() != NULL)
    {
        fHitPointRatio =
            (float)((CSWSObject*)AsSWSCreature())->GetCurrentHitPoints(0) /
            (float)((CSWSObject*)AsSWSCreature())->GetMaxHitPoints(1);
    }
    else
    {
        fHitPointRatio = (float)GetCurrentHitPoints(0) /
                        (float)GetMaxHitPoints(1);
    }

    if (fHitPointRatio >= 0.95f)
    {
        return DAMAGE_LEVEL_UNINJURED;
    }
    else if (fHitPointRatio >= 0.75f)
    {
        return DAMAGE_LEVEL_SLIGHTLY_INJURED;
    }
    else if (fHitPointRatio >= 0.5f)
    {
        return DAMAGE_LEVEL_INJURED;
    }
    else if (fHitPointRatio >= 0.25f)
    {
        return DAMAGE_LEVEL_BADLY_INJURED;
    }
    else if (fHitPointRatio <= 0.0f)
    {
        return DAMAGE_LEVEL_DEAD;
    }
    return DAMAGE_LEVEL_NEAR_DEATH;
}

CSWSpell* CSWSObject::GetLastSpell()
{
    return g_pRules->m_pSpellArray->GetSpell(m_nLastSpellId);
}
// PC 0x004F8A70
void CSWSObject::LoadActionQueue(CResGFF* pRes, CResStruct* pStruct)
{
    CResList ActionList;
    if (pRes->GetList(&ActionList, pStruct, "ActionList"))
    {
        for (unsigned int nAction = 0; nAction < pRes->GetListCount(&ActionList); nAction++)
        {
            CResStruct ActionStruct;
            if (pRes->GetListElement(&ActionStruct, &ActionList, nAction))
            {

                void* apParam[CSWSObjectActionNode::MAX_ACTION_PARAMETERS];
                unsigned long anParamType[CSWSObjectActionNode::MAX_ACTION_PARAMETERS];
                unsigned long anAllocatedType[CSWSObjectActionNode::MAX_ACTION_PARAMETERS];
                for (unsigned int i = 0; i < CSWSObjectActionNode::MAX_ACTION_PARAMETERS; i++)
                    apParam[i] = NULL;
                for (unsigned int i = 0; i < CSWSObjectActionNode::MAX_ACTION_PARAMETERS; i++)
                    anParamType[i] = 0;

                int bSuccess;
                unsigned long nActionId = pRes->ReadFieldDWORD(&ActionStruct, "ActionId", bSuccess, 0);
                unsigned short nGroupActionId = pRes->ReadFieldWORD(&ActionStruct, "GroupActionId", bSuccess, 0);
                unsigned short nNumParams = pRes->ReadFieldWORD(&ActionStruct, "NumParams", bSuccess, 0);
                if (nNumParams > 0)
                {
                    CResList ParameterList;
                    pRes->GetList(&ParameterList, &ActionStruct, "Paramaters");
                    for (unsigned int i = 0; i < nNumParams; i++)
                    {
                        CResStruct ParameterStruct;
                        pRes->GetListElement(&ParameterStruct, &ParameterList, i);
                        anParamType[i] = pRes->ReadFieldDWORD(&ParameterStruct, "Type", bSuccess, 0);
                        switch (anParamType[i])
                        {
                        case ACTION_PARAM_INTEGER:
                        {
                            int* pnValue = new int;
                            *pnValue = pRes->ReadFieldINT(&ParameterStruct, "Value", bSuccess, 0);
                            apParam[i] = pnValue;
                            anAllocatedType[i] = ACTION_PARAM_INTEGER;
                            break;
                        }
                        case ACTION_PARAM_FLOAT:
                        {
                            float* pfValue = new float;
                            *pfValue = pRes->ReadFieldFLOAT(&ParameterStruct, "Value", bSuccess, 0.0f);
                            apParam[i] = pfValue;
                            anAllocatedType[i] = ACTION_PARAM_FLOAT;
                            break;
                        }
                        case ACTION_PARAM_STRING:
                        {
                            CExoString* psValue = new CExoString;
                            *psValue = pRes->ReadFieldCExoString(&ParameterStruct, "Value", bSuccess, "");
                            apParam[i] = psValue;
                            anAllocatedType[i] = ACTION_PARAM_STRING;
                            break;
                        }
                        case ACTION_PARAM_OBJECT:
                        {
                            unsigned long* pnValue = new unsigned long;
                            *pnValue = pRes->ReadFieldDWORD(&ParameterStruct, "Value", bSuccess, 0);
                            apParam[i] = pnValue;
                            anAllocatedType[i] = ACTION_PARAM_OBJECT;
                            break;
                        }
                        case ACTION_PARAM_SCRIPT_SITUATION:
                        {
                            CResStruct ScriptStruct;
                            pRes->GetStructFromStruct(&ScriptStruct, &ParameterStruct, "Value");
                            void* pScript;
                            g_pVirtualMachine->LoadScriptSituation(&pScript, pRes, &ScriptStruct);
                            apParam[i] = pScript;
                            anAllocatedType[i] = ACTION_PARAM_SCRIPT_SITUATION;
                            break;
                        }
                        }
                    }
                }
                AddAction(nActionId, nGroupActionId,
                          anParamType[0], apParam[0], anParamType[1], apParam[1],
                          anParamType[2], apParam[2], anParamType[3], apParam[3],
                          anParamType[4], apParam[4], anParamType[5], apParam[5],
                          anParamType[6], apParam[6], anParamType[7], apParam[7],
                          anParamType[8], apParam[8], anParamType[9], apParam[9],
                          anParamType[10], apParam[10], anParamType[11], apParam[11],
                          anParamType[12], apParam[12]);
                for (unsigned int i = 0; i < nNumParams; i++)
                {
                    switch (anAllocatedType[i])
                    {
                    case ACTION_PARAM_INTEGER:
                        delete (int*)apParam[i];
                        break;
                    case ACTION_PARAM_FLOAT:
                        delete (float*)apParam[i];
                        break;
                    case ACTION_PARAM_STRING:
                        delete (CExoString*)apParam[i];
                        break;
                    case ACTION_PARAM_OBJECT:
                        delete (unsigned long*)apParam[i];
                        break;
                    }
                }
            }
        }
    }
}
// PC 0x004F8F20
int CSWSObject::DoDamageImmunity(CSWSCreature* pAttacker, int nDamage,
                                unsigned short nDamageFlags,
                                int bSuppressFeedback, int bAttackDamage)
{
    int nImmunity = (char)GetDamageImmunityByFlags(nDamageFlags);
    int nRemainingDamage;
    if (nImmunity > 0)
        nRemainingDamage = nDamage - max(1, nDamage * nImmunity / 100);
    else
        nRemainingDamage = nDamage - nDamage * nImmunity / 100;

    if (!bSuppressFeedback && nImmunity > 0)
    {
        if (bAttackDamage == 1 && pAttacker != NULL)
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            pData->SetInteger(0, CSWSCreature::FEEDBACK_DAMAGE_IMMUNITY);
            pData->SetInteger(1, nDamage - nRemainingDamage);
            pData->SetInteger(2, nDamageFlags);
            pData->SetObjectID(0, m_nId);
            CSWSCombatAttackData* pAttack =
                pAttacker->m_pcCombatRound->GetAttack(
                    pAttacker->m_pcCombatRound->m_nCurrentAttack);
            pAttack->m_lstFeedbackMessages.Add(pData);
        }
        else
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            CSWCCMessageData* pAttackerData = new CSWCCMessageData;
            pData->SetInteger(0, CSWSCreature::FEEDBACK_DAMAGE_IMMUNITY);
            pData->SetInteger(1, nDamage - nRemainingDamage);
            pData->SetInteger(2, nDamageFlags);
            pData->SetObjectID(0, m_nId);
            pData->CopyTo(pAttackerData);
            if (AsSWSCreature() != NULL)
                AsSWSCreature()->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_DAMAGE_IMMUNITY, pData);
            else
                delete pData;
            if (pAttacker != NULL)
                pAttacker->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_DAMAGE_IMMUNITY, pAttackerData);
            else
                delete pAttackerData;
        }
    }
    return nRemainingDamage;
}
// PC 0x004F9150
void CSWSObject::AddLoopingVisualEffect(unsigned short nVisualEffectId,
                                        unsigned long oidCreator,
                                        unsigned char nBeamFlag)
{
    CLoopingVisualEffect* pEffect = new CLoopingVisualEffect;

    pEffect->m_nVisualEffectId = nVisualEffectId;
    pEffect->m_oidCreator = oidCreator;
    pEffect->m_nBeamFlag = nBeamFlag;
    m_lstLoopingVisualEffects.Add(pEffect);
}
// PC 0x004F91B0
void CSWSObject::RemoveEffectTarget(unsigned long oidTarget)
{
    NwsobjectTrace2();
    CGameObject* pTarget =
        g_pAppManager->m_pServerExoApp->GetGameObject(oidTarget);

    if (pTarget != NULL && pTarget->AsSWSObject() != NULL &&
        m_lstEffectTargets.num > 0)
    {
        CExoArrayList<CGameEffect*>* pEffects =
            &pTarget->AsSWSObject()->m_lstEffects;
        unsigned long nEffects = pEffects->num;
        int bFound = 0;

        for (unsigned long i = 0; i < nEffects && !bFound; i++)
        {
            if ((*pEffects)[i]->m_oidCreator == m_nId)
            {
                bFound = 1;
            }
        }
        if (!bFound && m_lstEffectTargets.Contains(oidTarget) != 0)
        {
            m_lstEffectTargets.Remove(oidTarget);
        }
    }
}
// PC 0x004F9250
int CSWSObject::StartDialog(unsigned long oidTarget,
                            const CExoString& sDialogResRef,
                            int nConversationType)
{
    CGameObject* pTarget = NULL;
    unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
    CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
    g_pAppManager->GetClientExoApp()->SetDialogSent(0);
    g_pAppManager->GetClientExoApp()->SetDialogModelsSent(0);

    if (m_pDialog == NULL)
    {
        m_nDialogDelayDay = 0;
        m_nDialogDelayTime = 0;
        m_nDialogStartDay = 0;
        m_nDialogStartTime = 0;
        m_nDialogState = 0;
        m_nDialogPending = 0;
        m_n74 = 1;
        m_n60 = 0;
        m_n64 = 0;
        m_nCameraAnimState = 0;
        m_nCameraTimeStamp = 0;

        {
            CResRef cDialogResRef;
            if (sDialogResRef == "")
            {
                cDialogResRef = GetDialogResref();
            }
            else
            {
                cDialogResRef = sDialogResRef;
            }
            if (cDialogResRef == "")
                return 0;

            CResGFF* pGFF = new CResGFF(RESTYPE_DLG, "DLG ", cDialogResRef);
            if (pGFF->m_bLoaded == 0)
            {
                delete pGFF;
                return 0;
            }
            m_pDialog = new CSWSDialog;
            if (m_pDialog->LoadDialog(pGFF, 1) == 0)
            {
                delete m_pDialog;
                m_pDialog = NULL;
                delete pGFF;
                return 0;
            }
            delete pGFF;
            CSWSDialog* pDialog = m_pDialog;
            pDialog->m_n90 = nConversationType;
            g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(1);
            CExoString sDialog;
            cDialogResRef.CopyToString(sDialog);
        }
        if (g_pAppManager->GetClientExoApp()->m_pcExoAppInternal->m_pClientOptions->m_nCameraMode ==
            CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
        {
            g_bShowIndicator = 1;
            g_pAppManager->GetClientExoApp()->GetModule()->RestoreCamera();
            g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GAME, 1);
        }
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidTarget, &pTarget) != CGameObjectArray::SUCCESS)
            return 0;

        int nGender = CTlkTable::GENDER_MALE;
        unsigned long oidPC = pPlayer->m_oidPCObject;
        CGameObject* pPlayerObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidPC);
        if (pPlayerObject != NULL && pPlayerObject->AsSWSCreature() != NULL)
            nGender = pPlayerObject->AsSWSCreature()->GetGender();

        CSWSObject* pTargetObject = (CSWSObject*)pTarget;
        CSWSDialogPlayer* pDialogPlayer = new CSWSDialogPlayer;
        pDialogPlayer->m_nPlayerId = pPlayer->m_nPlayerId;
        pDialogPlayer->m_oidPlayer = oidTarget;
        pDialogPlayer->m_nGender = nGender;
        m_pDialog->AddPlayer(pDialogPlayer);
        m_pDialog->m_oidCurrentPlayer = oidTarget;
        m_pDialog->m_nCurrentPlayerGender = nGender;
        pTargetObject->SetDialogOwner(GetId());
        if (pTargetObject->AsSWSCreature() != NULL)
        {
            pTargetObject->AsSWSCreature()->ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
            pTargetObject->AsSWSCreature()->SetActivity(CSWSCreature::ACTIVITY_DIALOG, 1, 0);
        }
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_oidCameraAnimationObject = GetId();
        SetDialogOwner(GetId());
        m_nWaitFlags = 0;
        if (AsSWSCreature() != NULL)
        {
            AsSWSCreature()->ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
            AsSWSCreature()->SetActivity(CSWSCreature::ACTIVITY_DIALOG, 1, 0);
        }
        m_pDialog->SetDialogOwner(this);
        int bResult = SendDialogEntry(m_pDialog->GetStartEntry(this));
        g_pAppManager->GetClientExoApp()->GetInGameGui()->HideContainerGui();
        if (bResult != 0)
        {
            g_pAppManager->GetClientExoApp()->ClearEnemySighted();
        }
        else
        {
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bHoldWorldFadeInForDialog != 0)
            {
                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bHoldWorldFadeInForDialog = 0;
                g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(
                    1, 0.0f, 1.0f, Vector());
            }
            g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(0);
        }
        return bResult;
    }
    if (m_bDialogEnded != 0)
        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(0);
    return 1;
}
// PC 0x004F96B0
void CSWSObject::SetListenExpression(const CExoString& sPattern, int nNumber)
{
    CSWSExpression* pExpression = GetListenExpressionObj(nNumber);

    if (pExpression == NULL)
    {
        pExpression = new CSWSExpression;
        pExpression->m_nReserved = nNumber;
        m_lstExpressions.Add(pExpression);
    }
    pExpression->ParseString(sPattern);
}
// PC 0x004F9770
void CSWSObject::AddMatchedExpressionString(const CExoString& sString)
{
    CExoString* pString = new CExoString(sString);

    m_lstMatchedStrings.Add(pString);
}
// PC 0x004F9810
void CSWSObject::ClearMatchedExpressionStrings()
{
    for (int i = 0; i < m_lstMatchedStrings.num; i++)
    {
        CExoString* pString = m_lstMatchedStrings[i];

        if (pString != NULL)
        {
            delete pString;
            m_lstMatchedStrings[i] = NULL;
        }
    }

    m_lstMatchedStrings.SetSize(0);
}
// PC 0x004F9880
int CSWSObject::AddOpenDoorAction(unsigned long oidDoor, int bSuccess)
{
    if (m_bCommandable == 0)
    {
        return 0;
    }

    AddAction(0x14, 0xffff, 3, &oidDoor, 1, &bSuccess, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x004F98E0
int CSWSObject::AddCloseDoorAction(unsigned long oidDoor, int bSuccess)
{
    if (m_bCommandable == 0)
    {
        return 0;
    }

    AddAction(0x15, 0xffff, 3, &oidDoor, 1, &bSuccess, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x004F9940
int CSWSObject::AddUnlockObjectAction(unsigned long oidObject,
                                      unsigned long oidKey, int bUnknown)
{
    if (AsSWSCreature() != NULL)
    {
        CSWSCreatureStats* pStats = AsSWSCreature()->m_pStats;

        if (pStats->GetCanUseSkill(CSWRules::SKILL_SECURITY) == 0)
        {
            AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_CANNOT_USE_SKILL, NULL);
            return 0;
        }
    }

    if (m_bCommandable == 0)
    {
        return 0;
    }

    AddAction(0x26, 0xffff, 3, &oidObject, 3, &oidKey, 1, &bUnknown, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x004F99E0
int CSWSObject::AddLockObjectAction(unsigned long oidObject)
{
    if (AsSWSCreature() != NULL)
    {
        CSWSCreatureStats* pStats = AsSWSCreature()->m_pStats;

        if (pStats->GetCanUseSkill(CSWRules::SKILL_SECURITY) == 0)
        {
            return 0;
        }
    }

    if (m_bCommandable == 0)
    {
        return 0;
    }

    AddAction(0x27, 0xffff, 3, &oidObject, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x004F9A70
int CSWSObject::AddDoorSaberAction(unsigned long oidDoor)
{
    if (m_bCommandable == 0)
    {
        return 0;
    }

    AddAction(ACTION_DOOR_SABER, 0xffff, 3, &oidDoor, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x004F9AD0
int CSWSObject::AddSwitchWeaponsAction(int bInstant)
{
    if (m_bCommandable == 0)
    {
        return 0;
    }

    CSWSCreature* pCreature = AsSWSCreature();
    if (pCreature == NULL)
    {
        return 0;
    }

    if (bInstant == 0)
    {
        if (pCreature->m_bCombatState != 1 &&
            pCreature->m_pcCombatRound->GetActionPending() == 0)
        {
            pCreature->ActionManager(4);
        }
        if (pCreature->m_lstActions.GetHead() != NULL)
        {
            pCreature->m_pcCombatRound->AddSwitchWeaponsAction();
            if (GetNodeById(ACTION_COMBAT) == NULL)
            {
                int bCombat = 1;
                AddAction(ACTION_COMBAT, ACTION_GROUP_NEW, 1, &bCombat, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0);
            }
            return 1;
        }
    }

    ClearAllActions(1);
    AddAction(ACTION_SWITCH_WEAPONS, ACTION_GROUP_NEW, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}
// PC 0x004F9BF0
CSWSObject::CSWSObject(unsigned char nObjectType, unsigned long oidObject,
                       int bFlag)
    : CGameObject(nObjectType, oidObject)
{
    SetPosition(Vector(0.0f, 0.0f, 0.0f), 1, 0, 0);
    SetOrientation(Vector(1.0f, 0.0f, 0.0f));
    m_nAnimation = ANIMATION_IDLE;
    m_nActionStartDay = 0;
    m_nActionStartTime = 0;
    m_fMovementRate = 1.0f;
    m_n88 = 0;

    CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
    if (oidObject == OBJECT_INVALID)
        pServerApp->GetObjectArray()->AddInternalObject(m_nId, this, bFlag);
    else
        pServerApp->GetObjectArray()->AddObjectAtPos(m_nId, this);

    m_nCurrentHitPoints = 1;
    m_nMaxHitPoints = 1;
    m_nTemporaryHitPoints = 0;
    m_pDialog = NULL;
    m_oidDialogOwner = CGameObjectArray::INVALIDOBJID;
    m_oidArea = CGameObjectArray::INVALIDOBJID;
    m_nDialogPending = 0;
    m_n58 = ANIMATION_COMBAT_READY;
    m_nWaitFlags = 0;
    m_bCommandable = 1;
    m_bListening = 0;
    m_sTag = "";
    m_c1D5 = -1;
    m_c1D4 = -1;
    m_nEC = 1;
    m_nF0 = 0;
    m_nF4 = 0;
    m_fSyncHeight = 1.0f;
    m_nAILevel = -1;
    m_oidLastSpeaker = OBJECT_INVALID;
    m_oidDialogOwner = OBJECT_INVALID;
    m_oidLastHostileActor = OBJECT_INVALID;
    m_oidLastDamager = OBJECT_INVALID;
    m_oidLastAttacker = OBJECT_INVALID;
    m_n154 = OBJECT_INVALID;
    m_oidLastDisturbed = OBJECT_INVALID;
    m_n1D0 = 0;
    m_nLastSpellId = -1;
    m_c1C4 = 0;
    m_oidSpellTargetObject = OBJECT_INVALID;
    m_vSpellTargetPosition = Vector(0.0f, 0.0f, 0.0f);
    m_bSafeProjectileBroadcast = 0;
    m_n1DC = -1;
    m_n1E0 = 0;
    m_n1D8 = OBJECT_INVALID;
    m_oidInventoryDisturbItem = OBJECT_INVALID;
    m_n178 = 0;
    m_nSavingThrowSpellId = -1;
    m_pDamageImmunity = new char[LAST_DAMAGE_TYPE_COUNT];
    for (int i = 0; i < LAST_DAMAGE_TYPE_COUNT; i++)
        m_pDamageImmunity[i] = 0;
    m_pLastDamageAmounts = new int[LAST_DAMAGE_TYPE_COUNT];
    for (int j = 0; j < LAST_DAMAGE_TYPE_COUNT; j++)
        m_pLastDamageAmounts[j] = -1;
    m_nAttackType = 0;
    m_nAttackMode = 0;
    m_oidLastWeaponUsed = OBJECT_INVALID;
    m_bPlotObject = 0;
    m_nNextGroupId = 0;
    m_nLastGroupId = 0;
    m_pCurrentActionNode = NULL;
    m_nEffectIterator = 0;
    m_n1EC = 0;
    m_bApplyingLinkedEffect = 0;
    m_nPortraitId = 0xFFFF;
    m_bOpenDoorStarted = 0;
    m_nDialogReplyState = 0;
    m_nClientState = 0;
    m_aDirtyFlags[0] = 0;
    m_bMin1HP = 0;
    m_bDeleteNoFade = 0;
    m_bKeepCorpse = 0;
    m_nFadeOutTime = 0;
    m_n74 = 0;
    m_bDialogEnded = 0;
    m_nDialogStartDay = 0;
    m_nDialogStartTime = 0;
    m_nCameraAnimState = 0;
    m_n60 = 0;
    m_n64 = 0;
    m_bPartyInteract = 0;
    m_bDialogFaceSpeaker = 1;
    m_pClientObject = NULL;
    m_bDialogSoundStopped = 0;
    pServerApp = g_pAppManager->m_pServerExoApp;
    pServerApp->GetWorldTimer()->GetWorldTime(&m_nC4, &m_nC8);
}

void CSWSObject::CopyScriptVars(CSWSScriptVarTable* pVarTable)
{
    CExoString sName;
    CExoString sValue;
    CScriptLocation lLocation;

    for (int i = 0; i < pVarTable->num; i++)
    {
        sName = (*pVarTable)[i].m_sName;
        switch ((*pVarTable)[i].m_nType)
        {
        case SCRIPTVAR_TYPE_FLOAT:
            m_lScriptVars.SetFloat(sName, pVarTable->GetFloat(sName));
            break;
        case SCRIPTVAR_TYPE_INT:
            m_lScriptVars.SetInt(sName, pVarTable->GetInt(sName), TRUE);
            break;
        case SCRIPTVAR_TYPE_LOCATION:
            lLocation = pVarTable->GetLocation(sName);
            m_lScriptVars.SetLocation(sName, lLocation);
            break;
        case SCRIPTVAR_TYPE_OBJECT:
            m_lScriptVars.SetObject(sName, pVarTable->GetObject(sName));
            break;
        case SCRIPTVAR_TYPE_STRING:
            sValue = pVarTable->GetString(sName);
            m_lScriptVars.SetString(sName, sValue);
            break;
        }
    }
}
// PC 0x004FA1A0
CSWSObject::~CSWSObject()
{
    if (m_pDialog != NULL)
    {
        delete m_pDialog;
        m_pDialog = NULL;
    }

    for (int i = 0; i < m_lstExpressions.num; i++)
        delete m_lstExpressions[i];

    ClearMatchedExpressionStrings();
    CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
    pServerApp->GetObjectArray()->Delete(m_nId);

    int nEffects = m_lstEffects.num;
    for (int j = nEffects - 1; j >= 0; j--)
        delete m_lstEffects[j];
    m_lstEffects.SetSize(0);

    if (m_pDamageImmunity != NULL)
    {
        delete m_pDamageImmunity;
        m_pDamageImmunity = NULL;
    }
    if (m_pLastDamageAmounts != NULL)
    {
        delete m_pLastDamageAmounts;
        m_pLastDamageAmounts = NULL;
    }

    while (!m_lstActions.IsEmpty())
        delete m_lstActions.RemoveHead();

    if (m_pClientObject != NULL)
    {
        m_pClientObject->DetachFromServerObject();
        m_pClientObject = NULL;
    }
}
// PC 0x004FA400
void CSWSObject::LoadListenData(CResGFF* pRes, CResStruct* pStruct)
{
    CResList cExpressionList;
    CResStruct cExpressionStruct;
    int bSuccess;
    CExoString sPattern;

    if (pRes->GetList(&cExpressionList, pStruct, "ExpressionList") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cExpressionList); i++)
        {
            pRes->GetListElement(&cExpressionStruct, &cExpressionList, i);
            int nId = pRes->ReadFieldINT(&cExpressionStruct, "ExpressionId",
                                         bSuccess, 0);
            sPattern = pRes->ReadFieldCExoString(
                &cExpressionStruct, "ExpressionString", bSuccess, "");
            SetListenExpression(sPattern, nId);
        }
    }
}
// PC 0x004FA540
void CSWSObject::RemoveEffect(CGameEffect* pEffect)
{
    CServerAIMaster* pAIMaster =
        g_pAppManager->m_pServerExoApp->GetServerAIMaster();
    if (pAIMaster->OnEffectRemoved(this, pEffect) ==
        CSWSEffectListHandler::DELETE_EFFECT)
    {
        m_lstEffects.DelIndex(m_lstEffects.IndexOf(pEffect));

        CGameObject* pCreator =
            g_pAppManager->m_pServerExoApp->GetGameObject(pEffect->m_oidCreator);
        if (pCreator != NULL && pCreator->AsSWSObject() != NULL)
        {
            unsigned long oidTarget = m_nId;
            pCreator->AsSWSObject()->RemoveEffectTarget(oidTarget);
        }
        if (m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
        {
            UpdateAttributesOnEffect(pEffect);
        }
        delete pEffect;
        if (m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
        {
            UpdateEffectPtrs();
        }
    }
}
// PC 0x004FA960
int CSWSObject::DoDamageReduction(CSWSCreature* pAttacker, int nDamage,
                                 unsigned char nDamagePower,
                                 int bSuppressFeedback, int bAttackDamage)
{
    int nReductionPower = 0;
    int nReduction = 0;
    int nEffectIndex = 0;
    int nStart;
    if (AsSWSCreature() != NULL)
    {
        CSWSCreatureStats* pStats = AsSWSCreature()->m_pStats;
        nStart = pStats->m_nDamageReductionEffectIndex;
    }
    else
        nStart = 0;
    int bFound = 0;

    if (m_lstEffects.num > 0)
    {
        for (int i = nStart; i < m_lstEffects.num; ++i)
        {
            CGameEffect* pEffect = m_lstEffects.element[i];
            if (m_lstEffects[i]->m_nType > CSWSEffectListHandler::EFFECT_DAMAGE_REDUCTION)
                break;
            if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_DAMAGE_REDUCTION)
            {
                int nAmount = pEffect->GetInteger(0);
                int nPower = pEffect->GetInteger(1);
                if (nAmount > nReduction)
                {
                    nEffectIndex = i;
                    nReduction = nAmount;
                    nReductionPower = nPower;
                    bFound = 1;
                }
            }
        }
    }

    if (!bFound)
    {
        if (AsSWSCreature() != NULL && nDamage < 0)
            return 0;
        return nDamage;
    }

    int nRemaining = m_lstEffects[nEffectIndex]->GetInteger(2);
    if (nRemaining != 0)
    {
        if (nRemaining - nDamage > 0)
        {
            if (!bSuppressFeedback)
                m_lstEffects[nEffectIndex]->SetInteger(2, nRemaining - nDamage);
        }
        else
        {
            nReduction = nRemaining;
            if (!bSuppressFeedback)
            {
                unsigned __int64 nEffectId = m_lstEffects[nEffectIndex]->m_nID;
                RemoveEffectById(nEffectId);
            }
        }
    }

    if (FALSE)
    {
        if (bAttackDamage == 1 && pAttacker != NULL)
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            pData->SetObjectID(0, m_nId);
            pData->SetInteger(0, CSWSCreature::FEEDBACK_DAMAGE_REDUCTION);
            pData->SetInteger(1, min(nDamage, nReduction));
            CSWSCombatRound* pCombatRound = pAttacker->m_pcCombatRound;
            pCombatRound->GetAttack(pCombatRound->m_nCurrentAttack)
                ->m_lstFeedbackMessages.Add(pData);
        }
        else
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            CSWCCMessageData* pAttackerData = new CSWCCMessageData;
            pData->SetObjectID(0, m_nId);
            pData->SetInteger(1, min(nDamage, nReduction));
            pData->CopyTo(pAttackerData);
            if (AsSWSCreature() != NULL)
                AsSWSCreature()->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_DAMAGE_REDUCTION, pData);
            else
                delete pData;
            if (pAttacker != NULL)
                pAttacker->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_DAMAGE_REDUCTION, pAttackerData);
            else
                delete pAttackerData;
        }
    }
    if (!bSuppressFeedback && nDamagePower < nReductionPower)
    {
        if (bAttackDamage == 1 && pAttacker != NULL)
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            pData->SetObjectID(0, m_nId);
            int nAttack = pAttacker->m_pcCombatRound->m_nCurrentAttack;
            if (nRemaining != 0)
            {
                pData->SetInteger(0, CSWSCreature::FEEDBACK_DAMAGE_REDUCTION_REMAINING);
                if (nRemaining - nDamage > 0)
                {
                    pData->SetInteger(1, min(nDamage, nReduction));
                    pData->SetInteger(2, nRemaining - min(nDamage, nReduction));
                }
                else
                {
                    pData->SetInteger(1, nRemaining);
                    pData->SetInteger(2, 0);
                }
                CSWSCombatRound* pCombatRound = pAttacker->m_pcCombatRound;
                pCombatRound->GetAttack(nAttack)->m_lstFeedbackMessages.Add(pData);
            }
            else
            {
                pData->SetInteger(0, CSWSCreature::FEEDBACK_DAMAGE_REDUCTION);
                pData->SetInteger(1, min(nDamage, nReduction));
                CSWSCombatRound* pCombatRound = pAttacker->m_pcCombatRound;
                pCombatRound->GetAttack(nAttack)->m_lstFeedbackMessages.Add(pData);
            }
        }
        else
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            CSWCCMessageData* pAttackerData = new CSWCCMessageData;
            pData->SetObjectID(0, m_nId);
            if (nRemaining != 0)
            {
                if (nRemaining - nDamage > 0)
                {
                    pData->SetInteger(1, min(nDamage, nReduction));
                    pData->SetInteger(2, nRemaining - min(nDamage, nReduction));
                }
                else
                {
                    pData->SetInteger(1, nRemaining);
                    pData->SetInteger(2, 0);
                }
                pData->CopyTo(pAttackerData);
                if (AsSWSCreature() != NULL)
                    AsSWSCreature()->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_DAMAGE_REDUCTION_REMAINING, pData);
                else
                    delete pData;
                if (pAttacker != NULL)
                    pAttacker->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_DAMAGE_REDUCTION_REMAINING, pAttackerData);
                else
                    delete pAttackerData;
            }
            else
            {
                pData->SetInteger(1, min(nDamage, nReduction));
                pData->CopyTo(pAttackerData);
                if (AsSWSCreature() != NULL)
                    AsSWSCreature()->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_DAMAGE_REDUCTION, pData);
                else
                    delete pData;
                if (pAttacker != NULL)
                    pAttacker->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_DAMAGE_REDUCTION, pAttackerData);
                else
                    delete pAttackerData;
            }
        }
    }
    if (nDamagePower < nReductionPower)
        return nDamage - nReduction <= 0 ? 0 : nDamage - nReduction;
    return nDamage;
}
// PC 0x004FADC0
int CSWSObject::DoDamageResistance(CSWSCreature* pAttacker, int nDamage,
                                  unsigned short nDamageFlags,
                                  int bSuppressFeedback, int bAttackDamage,
                                  int bRemainingDamage)
{
    int nResistance = 0;
    int bVulnerable = 0;
    int nDamageType = 0;
    int nBestEffect;
    if (m_lstEffects.num > 0)
    {
        for (int i = 0; i < CSWSCombatAttackData::DAMAGE_COUNT; i++)
        {
            if (((nDamageFlags >> i) & 1) != 0)
            {
                int nTypeResistance = 0;
                int nEffectIndex = 0;
                int j;
                if (AsSWSCreature() != NULL)
                    j = AsSWSCreature()->GetStats()->m_nDamageResistanceEffectIndex;
                else
                    j = 0;
                for (; j < m_lstEffects.num; j++)
                {
                    CGameEffect* pEffect = m_lstEffects.element[j];
                    if (m_lstEffects[j]->m_nType > CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE)
                        break;
                    if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE)
                    {
                        int nFlags = pEffect->GetInteger(CSWSEffectListHandler::RESISTANCE_INTEGER_TYPE);
                        int nAmount = pEffect->GetInteger(CSWSEffectListHandler::RESISTANCE_INTEGER_AMOUNT);
                        if ((nDamageFlags & nFlags) != 0 && nAmount > nTypeResistance)
                        {
                            nTypeResistance = nAmount;
                            nEffectIndex = j;
                        }
                        int nVulnerability = pEffect->GetInteger(CSWSEffectListHandler::RESISTANCE_INTEGER_VULNERABILITY_TYPE);
                        if ((nDamageFlags & nVulnerability) != 0)
                            bVulnerable = 1;
                    }
                }
                if (nTypeResistance > nResistance)
                {
                    nResistance = nTypeResistance;
                    nBestEffect = nEffectIndex;
                }
                if (bRemainingDamage)
                    nDamageType = nDamageFlags >> i;
            }
        }
    }
    if (nResistance != 0)
    {
        int nLimit = m_lstEffects[nBestEffect]->GetInteger(CSWSEffectListHandler::RESISTANCE_INTEGER_LIMIT);
        int nScaledDamage = nDamage * (bVulnerable + 1);
        if (nLimit != 0)
        {
            if (nLimit - nScaledDamage > 0)
            {
                if (!bSuppressFeedback)
                    m_lstEffects[nBestEffect]->SetInteger(CSWSEffectListHandler::RESISTANCE_INTEGER_LIMIT, nLimit - nScaledDamage);
            }
            else
            {
                nResistance = nLimit;
                if (!bSuppressFeedback)
                {
                    unsigned __int64 nEffectId = m_lstEffects[nBestEffect]->m_nID;
                    RemoveEffectById(nEffectId);
                }
            }
        }
        if (!bSuppressFeedback)
        {
            if (bAttackDamage == 1 && pAttacker != NULL)
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(CSWCCMessageData::RESISTANCE_FEEDBACK_TARGET, m_nId);
                int nAttack = pAttacker->m_pcCombatRound->m_nCurrentAttack;
                if (nLimit != 0)
                {
                    pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_MESSAGE, CSWSCreature::FEEDBACK_DAMAGE_RESISTANCE_REMAINING);
                    if (nLimit - nScaledDamage > 0)
                    {
                        pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_ABSORBED, min(nScaledDamage, nResistance));
                        pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_REMAINING, nLimit - min(nScaledDamage, nResistance));
                    }
                    else
                    {
                        pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_ABSORBED, nLimit);
                        pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_REMAINING, 0);
                    }
                    CSWSCombatRound* pCombatRound = pAttacker->m_pcCombatRound;
                    pCombatRound->GetAttack(nAttack)->m_lstFeedbackMessages.Add(pData);
                }
                else
                {
                    pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_MESSAGE, CSWSCreature::FEEDBACK_DAMAGE_RESISTED);
                    pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_ABSORBED, min(nScaledDamage, nResistance));
                    CSWSCombatRound* pCombatRound = pAttacker->m_pcCombatRound;
                    pCombatRound->GetAttack(nAttack)->m_lstFeedbackMessages.Add(pData);
                }
            }
            else
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                CSWCCMessageData* pAttackerData = new CSWCCMessageData;
                pData->SetObjectID(CSWCCMessageData::RESISTANCE_FEEDBACK_TARGET, m_nId);
                if (nLimit != 0)
                {
                    if (nLimit - nScaledDamage > 0)
                    {
                        pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_ABSORBED, min(nScaledDamage, nResistance));
                        pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_REMAINING, nLimit - min(nScaledDamage, nResistance));
                    }
                    else
                    {
                        pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_ABSORBED, nLimit);
                        pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_REMAINING, 0);
                    }
                    pData->CopyTo(pAttackerData);
                    if (AsSWSCreature() != NULL)
                        AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_DAMAGE_RESISTANCE_REMAINING, pData);
                    else
                        delete pData;
                    if (pAttacker != NULL)
                        pAttacker->SendFeedbackMessage(CSWSCreature::FEEDBACK_DAMAGE_RESISTANCE_REMAINING, pAttackerData);
                    else
                        delete pAttackerData;
                }
                else
                {
                    pData->SetInteger(CSWCCMessageData::RESISTANCE_FEEDBACK_ABSORBED, min(nScaledDamage, nResistance));
                    pData->CopyTo(pAttackerData);
                    if (AsSWSCreature() != NULL)
                        AsSWSCreature()->SendFeedbackMessage(CSWSCreature::FEEDBACK_DAMAGE_RESISTED, pData);
                    else
                        delete pData;
                    if (pAttacker != NULL)
                        pAttacker->SendFeedbackMessage(CSWSCreature::FEEDBACK_DAMAGE_RESISTED, pAttackerData);
                    else
                        delete pAttackerData;
                }
            }
        }
    }
    if (AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = AsSWSCreature();
        if (pCreature->m_pStats->HasFeat(CSWFeat::FEAT_IMPROVED_TOUGHNESS))
        {
            nDamage -= 2;
            if (bRemainingDamage)
            {
                CSWSCombatAttackData* pAttack = pAttacker->m_pcCombatRound->GetAttack(pAttacker->m_pcCombatRound->m_nCurrentAttack);
                pAttack->m_DamageBreakdown.m_nUnknown9 = nDamage;
                pAttack->m_DamageBreakdown.m_nUnknown6 = CSWSCombatAttackExtra3::DAMAGE_ABSORPTION_ACTIVE;
                pAttack->m_DamageBreakdown.m_nUnknown8 = nDamageType;
            }
        }
        if (pCreature->m_pStats->HasFeat(CSWFeat::FEAT_WOOKIE_ENDURANCE))
        {
            nDamage -= 2;
            if (bRemainingDamage)
            {
                CSWSCombatAttackData* pAttack = pAttacker->m_pcCombatRound->GetAttack(pAttacker->m_pcCombatRound->m_nCurrentAttack);
                pAttack->m_DamageBreakdown.m_nUnknownA = nDamage;
                pAttack->m_DamageBreakdown.m_nUnknown7 = CSWSCombatAttackExtra3::DAMAGE_ABSORPTION_ACTIVE;
                pAttack->m_DamageBreakdown.m_nUnknown8 = nDamageType;
            }
        }
    }
    return nDamage - nResistance > 0 ? nDamage - nResistance : 0;
}

#ifndef _XBOX
int CSWSObject::DoSpellLevelAbsorption(CSWSObject* pCaster)  // PC 0x004FB4F0
{
    if (pCaster != NULL && pCaster->AsSWSCreature() != NULL &&
        m_lstEffects.num > 0)
    {
        int nSpellId = pCaster->m_nLastSpellId;
        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
        if (pSpell != NULL)
        {
            int bFromItem = pCaster->AsSWSCreature()->m_bCasterLevelOverride;
            int nClass = CSWClass::CCLASS_INVALID;
            if (bFromItem != 1)
            {
                unsigned char nMultiClass = pCaster->m_c1D4;
                CSWSCreatureStats* pStats = pCaster->AsSWSCreature()->m_pStats;
                nClass = pStats->GetClass(nMultiClass);
            }
            if (bFromItem != 1 && nClass != CSWClass::CCLASS_INVALID)
                pSpell->GetSpellLevel(nClass);
            if (pSpell->m_nUserType != CSWSpell::USER_TYPE_FORCE_POWER)
                return SPELL_RESISTANCE_UNRESOLVED;

            int nAbsorbIndex = -1;
            CSWSCreature* pCreature = AsSWSCreature();
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            int i = pStats->m_nSpellLevelAbsorptionEffectIndex;
            while (i < m_lstEffects.num)
            {
                if (m_lstEffects[i]->GetEffectType() >
                    CSWSEffectListHandler::EFFECT_SPELL_LEVEL_ABSORPTION)
                    break;
                ++i;
            }
            if (nAbsorbIndex != -1 && pStats != NULL)
                RemoveEffectById(m_lstEffects[nAbsorbIndex]->m_nID);
            pStats = AsSWSCreature()->m_pStats;
            i = pStats->m_nSpellLevelAbsorptionEffectIndex;
            while (i < m_lstEffects.num)
            {
                if (m_lstEffects[i]->GetEffectType() >
                    CSWSEffectListHandler::EFFECT_SPELL_LEVEL_ABSORPTION)
                    break;
                ++i;
            }
            if (AsSWSCreature() != NULL)
                pCaster->AsSWSCreature();
        }
    }
    return SPELL_RESISTANCE_UNRESOLVED;
}
#endif
void CSWSObject::ApplyEffect(CGameEffect* pEffect, int bLoadingGame,  // PC 0x004FB640
                           int bLoadingModule)
{
    unsigned long nCurrentDay, nCurrentTime;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
        &nCurrentDay, &nCurrentTime);

    pEffect->m_bSkipOnLoad = m_n1EC != 0 || pEffect->m_bExposed == 0;
    if (g_pAppManager->GetServerExoApp()->GetServerAIMaster()->OnEffectApplied(
            this, pEffect, bLoadingGame) != CSWSEffectListHandler::DELETE_EFFECT)
    {
        if ((pEffect->m_nType != CSWSEffectListHandler::EFFECT_VISUALEFFECT &&
             pEffect->m_nType != CSWSEffectListHandler::EFFECT_EFFECT_ICON) ||
            (pEffect->m_nType == CSWSEffectListHandler::EFFECT_VISUALEFFECT &&
             (pEffect->GetInteger(0) == VFX_ARD_LIGHT_YELLOW_10 ||
              pEffect->GetInteger(0) == VFX_ARD_LIGHT_YELLOW_20)))
        {
            m_bApplyingLinkedEffect = 0;
        }

        if ((pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_TEMPORARY &&
            bLoadingGame == 0)
        {
            float fDuration = pEffect->m_fDuration;
            unsigned long nDurationDay =
                g_pAppManager->GetServerExoApp()->GetWorldTimer()->
                    GetCalendarDayFromSeconds(fDuration);
            fDuration = pEffect->m_fDuration;
            unsigned long nDurationTime =
                g_pAppManager->GetServerExoApp()->GetWorldTimer()->
                    GetTimeOfDayFromSeconds(fDuration);
            unsigned long nExpiryDay, nExpiryTime;
            g_pAppManager->GetServerExoApp()->GetWorldTimer()->AddWorldTimes(
                nCurrentDay, nCurrentTime, nDurationDay, nDurationTime,
                &nExpiryDay, &nExpiryTime);
            pEffect->SetExpiryTime(nExpiryDay, nExpiryTime);
        }

        int nIndex;
        for (nIndex = 0; nIndex < m_lstEffects.num; nIndex++)
        {
            if (pEffect->m_nType < m_lstEffects[nIndex]->m_nType)
                break;
        }
        m_lstEffects.Insert(pEffect, nIndex);
        if (m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
        {
            UpdateEffectPtrs();
            UpdateAttributesOnEffect(pEffect);
        }

        if ((pEffect->m_nSubType & CGameEffect::SUBTYPE_MASK) ==
            SUBTYPE_MAGICAL)
        {
            unsigned long oidCreator = pEffect->m_oidCreator;
            CGameObject* pCreator =
                g_pAppManager->GetServerExoApp()->GetGameObject(oidCreator);
            if (pCreator != NULL && pCreator->AsSWSObject() != NULL)
            {
                unsigned long oidTarget = m_nId;
                CSWSObject* pCreatorObject = pCreator->AsSWSObject();
                pCreatorObject->m_lstEffectTargets.AddUnique(oidTarget);
            }
        }
    }
    else if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_LINK &&
             bLoadingModule == 1 && m_bApplyingLinkedEffect == 1)
    {
        RemoveEffectById(pEffect->m_nID);
    }
    else
    {
        delete pEffect;
    }
}
// PC 0x004FA640
int CSWSObject::RemoveEffectById(unsigned __int64 nEffectId)
{
    int bRemoved = FALSE;
    CServerAIMaster* pAIMaster =
        g_pAppManager->m_pServerExoApp->GetServerAIMaster();

    int i = 0;
    while (i < m_lstEffects.num)
    {
        CGameEffect* pEffect = m_lstEffects[i];

        if (pEffect->m_nID == nEffectId)
        {
            unsigned long oidCreator = pEffect->m_oidCreator;
            CGameObject* pCreator =
                g_pAppManager->m_pServerExoApp->GetGameObject(oidCreator);
            if (pAIMaster->OnEffectRemoved(this, pEffect) ==
                CSWSEffectListHandler::DELETE_EFFECT)
            {
                m_lstEffects.Remove(pEffect);
                if (m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                {
                    UpdateEffectPtrs();
                }
                if (m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                {
                    UpdateAttributesOnEffect(pEffect);
                }
                delete pEffect;
                bRemoved = TRUE;

                if (pCreator != NULL && pCreator->AsSWSObject() != NULL)
                {
                    unsigned long oidTarget = m_nId;
                    pCreator->AsSWSObject()->RemoveEffectTarget(oidTarget);
                }
            }
            else
            {
                i++;
            }
        }
        else
        {
            i++;
        }
    }
    return bRemoved;
}
// PC 0x004FA7A0
void CSWSObject::RemoveEffectByCreator(unsigned long oidCreator)
{
    if (m_lstEffects.num > 0)
    {
        for (int i = 0; i < m_lstEffects.num; i++)
        {
            if (m_lstEffects[i]->m_oidCreator == oidCreator)
            {
                RemoveEffect(m_lstEffects[i]);
            }
        }
    }
}
// PC 0x004FA7F0
void CSWSObject::RemoveSomeEffectsOfDurationType(unsigned short nDurationType)
{
    unsigned long nRemoved = 0;

    for (unsigned long i = 0; i < m_lstEffects.num; i++)
    {
        CGameEffect* pEffect = m_lstEffects[i];

        if ((pEffect->m_nSubType & CGameEffect::DURATION_MASK) == nDurationType)
        {
            RemoveEffectById(pEffect->m_nID);
            i = 0;
            if (++nRemoved > 20)
            {
                break;
            }
        }
    }
}
// PC 0x004FA850
void CSWSObject::RemoveEffectByType(unsigned long nSpellId, int nType,
                                    int nInteger)
{
    for (int i = 0; i < m_lstEffects.num; i++)
    {
        CGameEffect* pEffect = m_lstEffects.element[i];

        if (pEffect->m_nSpellId == nSpellId && pEffect->m_nType == nType &&
            pEffect->GetInteger(0) == nInteger)
        {
            RemoveEffect(m_lstEffects.element[i]);
        }
    }
}
// PC 0x004FA8C0
void CSWSObject::ClearAllEffects(int bKeepOwnEffects)
{
    if (m_lstEffects.num != 0)
    {
        for (int i = m_lstEffects.num - 1; i >= 0; i--)
        {
            CGameEffect* pEffect = m_lstEffects.element[i];

            if (pEffect != NULL &&
                (pEffect->m_nSubType & CGameEffect::DURATION_MASK) !=
                    CGameEffect::DURATION_TYPE_INNATE &&
                (pEffect->m_nSubType & CGameEffect::DURATION_MASK) !=
                    CGameEffect::DURATION_TYPE_EQUIPPED &&
                pEffect->m_nType !=
                    CSWSEffectListHandler::EFFECT_SETSTATE_INTERNAL &&
                (bKeepOwnEffects == 0 || pEffect->m_oidCreator != m_nId))
            {
                RemoveEffect(pEffect);
                i = m_lstEffects.num;
            }
        }
        m_bCommandable = 1;
    }
}
// PC 0x004FB880
void CSWSObject::UpdateEffectList(unsigned long nDay, unsigned long nTime)
{
    unsigned long nElapsedDay = 0;
    unsigned long nElapsedTime = 0;
    CWorldTimer* pTimer = g_pAppManager->GetServerExoApp()->GetActiveTimer(GetId());

    if (m_lstEffects.num != 0)
    {
        unsigned long nEffect = 0;
        while (nEffect < m_lstEffects.num)
        {
            int bRestart = 0;
            CGameEffect* pEffect = m_lstEffects[nEffect];
            if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_REGENERATE)
            {
                unsigned long nLastDay = pEffect->GetInteger(2);
                unsigned long nLastTime = pEffect->GetInteger(3);
                pTimer->SubtractWorldTimes(nDay, nTime, nLastDay, nLastTime,
                                           &nElapsedDay, &nElapsedTime);
                int bRegenerate = 0;
                if (AsSWSCreature() != NULL)
                {
                    if (!AsSWSCreature()->GetDead() && !AsSWSCreature()->GetDeadTemp())
                    {
                        if (pEffect->GetInteger(4) == CSWSEffectListHandler::HEAL_FORCE_POINTS)
                        {
                            if (AsSWSCreature()->GetStats()->m_nCurrentForcePoints <
                                AsSWSCreature()->GetMaxForcePoints())
                                bRegenerate = 1;
                        }
                        else
                        {
                            CSWSCreature* pCreature = AsSWSCreature();
                            if (pCreature->GetCurrentHitPoints(1) <
                                AsSWSCreature()->GetMaxHitPoints(1))
                                bRegenerate = 1;
                        }
                    }
                }
                else if (GetCurrentHitPoints(1) < GetMaxHitPoints(1) && !GetDead())
                    bRegenerate = 1;

                if (bRegenerate && nElapsedTime > (unsigned long)pEffect->GetInteger(1))
                {
                    CGameEffect* pHeal = new CGameEffect(1);
                    pHeal->m_nType = CSWSEffectListHandler::EFFECT_HEAL;
                    pHeal->SetInteger(0, pEffect->GetInteger(0));
                    pHeal->SetInteger(1, pEffect->GetInteger(4));
                    pHeal->m_nSubType = (pHeal->m_nSubType & ~CGameEffect::DURATION_MASK) |
                                        CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                    pHeal->SetCreator(pEffect->m_oidCreator);
                    ApplyEffect(pHeal, 0, 0);
                    pEffect->SetInteger(2, nDay);
                    pEffect->SetInteger(3, nTime);
                    bRestart = 1;
                }
            }
            else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_POISON)
            {
                unsigned long nStartDay = pEffect->GetInteger(1);
                unsigned long nStartTime = pEffect->GetInteger(2);
                int nDuration = pEffect->GetInteger(5);
                unsigned long nDurationDay, nDurationTime;
                pTimer->SubtractWorldTimes(nDay, nTime, nStartDay, nStartTime,
                                           &nDurationDay, &nDurationTime);
                if (nDurationTime > (unsigned long)(1000 * nDuration))
                {
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), GetId(), CServerAIMaster::EVENT_REMOVE_EFFECT, pEffect);
                }
                else
                {
                    unsigned long nLastDay = pEffect->GetInteger(3);
                    unsigned long nLastTime = pEffect->GetInteger(4);
                    int nInterval = pEffect->GetInteger(6);
                    unsigned long nIntervalDay, nIntervalTime;
                    pTimer->SubtractWorldTimes(nDay, nTime, nLastDay, nLastTime,
                                               &nIntervalDay, &nIntervalTime);
                    if (nIntervalTime > (unsigned long)(1000 * nInterval))
                    {
                        if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState)
                            pEffect->SetInteger(5, nDuration + nInterval);
                        else
                        {
                            AsSWSCreature()->ApplyPoisonDamage(pEffect->GetInteger(0), pEffect,
                                pEffect->GetInteger(7), pEffect->GetFloat(0));
                            pEffect->SetFloat(0, pEffect->GetFloat(0) + 1.0f);
                        }
                        pEffect->SetInteger(3, nDay);
                        pEffect->SetInteger(4, nTime);
                    }
                }
            }
            else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_DISEASE)
            {
                unsigned long nNextDay = pEffect->GetInteger(3);
                unsigned long nNextTime = pEffect->GetInteger(4);
                if (pTimer->CompareWorldTimes(nDay, nTime, nNextDay, nNextTime) >= 0 &&
                    AsSWSCreature() != NULL)
                {
                    AsSWSCreature()->ApplyDiseasePayload(pEffect, nDay, nTime);
                    bRestart = 1;
                }
            }
            if ((pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                CGameEffect::DURATION_TYPE_TEMPORARY)
            {
                pEffect->GetExpiryTime(&nElapsedDay, &nElapsedTime);
                if (pTimer->CompareWorldTimes(nDay, nTime, nElapsedDay, nElapsedTime) == 1)
                {
                    RemoveEffectById(pEffect->m_nID);
                    bRestart = 1;
                }
                else
                    ++nEffect;
            }
            else
                ++nEffect;
            if (bRestart)
                nEffect = 0;
        }
    }
}
// PC 0x004FBD50
void CSWSObject::LoadEffectList(CResGFF* pRes, CResStruct* pStruct)
{
    CResList cEffectList;
    CResStruct cEffectStruct;

    if (pRes->GetList(&cEffectList, pStruct, "EffectList") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cEffectList); i++)
        {
            pRes->GetListElement(&cEffectStruct, &cEffectList, i);
            CGameEffect* pEffect = new CGameEffect(0);

            pEffect->LoadGameEffect(pRes, &cEffectStruct);
            if (pEffect->m_bSkipOnLoad == 1 ||
                (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                    CGameEffect::DURATION_TYPE_EQUIPPED)
            {
                delete pEffect;
            }
            else
            {
                ApplyEffect(pEffect, 1, 0);
            }
        }
    }
}

void CSWSObject::LoadVarTable(CResGFF* pRes, CResStruct* pStruct)
{
    m_lScriptVars.LoadVarTable(pRes, pStruct);
    m_lVarTable.LoadVarTable(pRes, pStruct);
}
// PC 0x004FBE60
unsigned char CSWSObject::LoadObjectState(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;
    unsigned char bCommandable;

    LoadEffectList(pRes, pStruct);
    LoadVarTable(pRes, pStruct);
    LoadActionQueue(pRes, pStruct);
    bCommandable = pRes->ReadFieldBYTE(pStruct, "Commandable", bSuccess, 1);
    m_bCommandable = bCommandable;
    return bCommandable;
}
#ifndef _XBOX
unsigned short CSWSObject::GetActionByGroupId(unsigned short nGroupId,  // PC 0x004F6120
                                              CSWSObjectActionNode** ppNode)
{
    *ppNode = NULL;
    CExoLinkedListNode* pNode = m_lstActions.GetHeadPos();
    unsigned short nActionType = 0xffff;

    if (pNode == NULL)
    {
        return 0xffff;
    }

    CExoLinkedListNode* pPos = pNode;
    CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pNode);
    int bFound = 0;

    while (pAction != NULL)
    {
        if (pAction->m_nGroupId == nGroupId)
        {
            bFound = 1;
            if (GetAcceptableAction(pAction->m_nActionType) == 1)
            {
                nActionType = pAction->m_nActionType;
                *ppNode = pAction;
            }
        }
        else if (bFound == 1)
        {
            break;
        }
        pAction = m_lstActions.GetNext(pPos);
    }
    return nActionType;
}
// PC 0x004F61B0
CExoLinkedListNode* CSWSObject::GetPositionByGroupIndex(int nIndex)
{
    CExoLinkedListNode* pNode = m_lstActions.GetHeadPos();
    int nGroup = 0;

    if (pNode == NULL)
    {
        return NULL;
    }

    CExoLinkedListNode* pPos = pNode;
    CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pNode);
    unsigned short nGroupId = pAction->m_nGroupId;

    if (nIndex == 0)
    {
        return pPos;
    }

    while (1)
    {
        if (pAction->m_nGroupId != nGroupId)
        {
            nGroupId = pAction->m_nGroupId;
            nGroup++;
        }
        if (nGroup == nIndex)
        {
            break;
        }
        pAction = m_lstActions.GetNext(pPos);
        if (pAction == NULL)
        {
            return NULL;
        }
    }
    return pPos;
}
#endif
CSWSObjectActionNode* CSWSObject::GetNodeById(unsigned short nNodeId)  // PC 0x004F6220
{
#ifdef _XBOX
    if (m_lstActions.GetHeadPos() == NULL) return NULL;
    CExoLinkedListNode* pPos = m_lstActions.GetHeadPos(); CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPos);
#else
    CExoLinkedListNode* pNode = m_lstActions.GetHeadPos();
    if (pNode == NULL) { return NULL; }
    CExoLinkedListNode* pPos = pNode;
    CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pNode);
#endif

    while (pPos != NULL)
    {
        if (pAction->m_nActionType == nNodeId)
        {
            return pAction;
        }
        pAction = m_lstActions.GetNext(pPos);
    }
    return NULL;
}
// PC 0x004F6280
int CSWSObject::GetNumActionGroups()
{
#ifdef _XBOX
    if (m_lstActions.GetHeadPos() == NULL) return 0;
    CExoLinkedListNode* pPos = m_lstActions.GetHeadPos(); CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPos);
#else
    CExoLinkedListNode* pNode = m_lstActions.GetHeadPos();
    if (pNode == NULL) { return 0; }
    CExoLinkedListNode* pPos = pNode;
    CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pNode);
#endif
    int nGroups = pAction != NULL;
    unsigned short nGroupId = pAction->m_nGroupId;

    while (pPos != NULL)
    {
        if (pAction->m_nGroupId != nGroupId)
        {
            nGroupId = pAction->m_nGroupId;
            nGroups++;
        }
        pAction = m_lstActions.GetNext(pPos);
    }
    return nGroups;
}
#ifndef _XBOX
void CSWSObject::RemoveGroup(unsigned short nGroupId)  // PC 0x004F62E0
{
    CExoLinkedListNode* pNode = m_lstActions.GetHeadPos();

    if (pNode != NULL)
    {
        CExoLinkedListNode* pPos = pNode;
        CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pNode);

        while (pPos != NULL)
        {
            pNode = pPos;
            if (pAction->m_nGroupId == nGroupId)
            {
                CSWSObjectActionNode* pRemove = pAction;
                pAction = m_lstActions.GetNext(pPos);
                m_lstActions.Remove(pNode);
                if (pRemove != NULL)
                {
                    delete pRemove;
                }
            }
            else
            {
                pAction = m_lstActions.GetNext(pPos);
            }
        }
    }
}
#endif
void CSWSObject::SetGroupInterruptable(unsigned short nGroupId,  // PC 0x004F6380
                                       int bInterruptable)
{
    CExoLinkedListNode* pPos = m_lstActions.GetHeadPos();
    CSWSObjectActionNode* pAction = m_lstActions.GetHead();

    while (pAction != NULL)
    {
        if (pAction->m_nGroupId == nGroupId)
        {
            pAction->m_nField70 = bInterruptable;
        }
        pAction = m_lstActions.GetNext(pPos);
    }
}
// PC 0x004F63D0
void CSWSObject::SaveActionQueue(CResGFF* pRes, CResStruct* pStruct)
{
    CResList ActionList;
    pRes->AddList(&ActionList, pStruct, "ActionList");
    CExoLinkedListNode* pPosition = m_lstActions.GetHeadPos();
    while (pPosition != NULL)
    {
        CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPosition);
        CResStruct ActionStruct;
        pRes->AddListElement(&ActionStruct, &ActionList, 0);
        pRes->WriteFieldDWORD(&ActionStruct, pAction->m_nActionType, "ActionId");
        pRes->WriteFieldWORD(&ActionStruct, pAction->m_nGroupId, "GroupActionId");
        pRes->WriteFieldWORD(&ActionStruct, pAction->m_nNumParams, "NumParams");
        if (pAction->m_nNumParams > 0)
        {
            CResList ParameterList;
            pRes->AddList(&ParameterList, &ActionStruct, "Paramaters");
            for (unsigned int i = 0; i < pAction->m_nNumParams; i++)
            {
                CResStruct ParameterStruct;
                pRes->AddListElement(&ParameterStruct, &ParameterList, 1);
                pRes->WriteFieldDWORD(&ParameterStruct, pAction->m_anParamType[i], "Type");
                switch (pAction->m_anParamType[i])
                {
                case ACTION_PARAM_INTEGER:
                    pRes->WriteFieldINT(&ParameterStruct, (int)pAction->m_apParam[i], "Value");
                    break;
                case ACTION_PARAM_FLOAT:
                    pRes->WriteFieldFLOAT(&ParameterStruct, *(float*)&pAction->m_apParam[i], "Value");
                    break;
                case ACTION_PARAM_STRING:
                    pRes->WriteFieldCExoString(&ParameterStruct, *(CExoString*)pAction->m_apParam[i], "Value");
                    break;
                case ACTION_PARAM_OBJECT:
                    pRes->WriteFieldDWORD(&ParameterStruct, (unsigned long)pAction->m_apParam[i], "Value");
                    break;
                case ACTION_PARAM_SCRIPT_SITUATION:
                    CResStruct ScriptStruct;
                    pRes->AddStructToStruct(&ScriptStruct, &ParameterStruct, "Value", 2);
                    g_pVirtualMachine->SaveScriptSituation(pAction->m_apParam[i], pRes, &ScriptStruct);
                    break;
                }
            }
        }
        m_lstActions.GetNext(pPosition);
    }
}
// PC 0x004F65C0
void CSWSObject::SaveEffectList(CResGFF* pRes, CResStruct* pStruct)
{
    CResList cEffectList;
    CResStruct cEffectStruct;

    if (m_lstEffects.num > 0)
    {
        pRes->AddList(&cEffectList, pStruct, "EffectList");
        for (int i = 0; i < m_lstEffects.num; i++)
        {
            pRes->AddListElement(&cEffectStruct, &cEffectList, 2);
            m_lstEffects[i]->SaveGameEffect(pRes, &cEffectStruct);
        }
    }
}
// PC 0x004F6640
void CSWSObject::SaveListenData(CResGFF* pRes, CResStruct* pStruct)
{
    CResList cExpressionList;
    CResStruct cExpressionStruct;

    if (m_lstExpressions.num > 0)
    {
        pRes->AddList(&cExpressionList, pStruct, "ExpressionList");
        for (int i = 0; i < m_lstExpressions.num; i++)
        {
            pRes->AddListElement(&cExpressionStruct, &cExpressionList, 5);
            pRes->WriteFieldINT(&cExpressionStruct,
                                m_lstExpressions[i]->m_nReserved,
                                "ExpressionId");
            pRes->WriteFieldCExoString(&cExpressionStruct,
                                       m_lstExpressions[i]->m_sPattern,
                                       "ExpressionString");
        }
    }
}
// PC 0x004F66F0
void CSWSObject::AddActionToFront(
    unsigned long nActionType, unsigned short nGroupId,
    unsigned long nParamType1, void* pParam1, unsigned long nParamType2,
    void* pParam2, unsigned long nParamType3, void* pParam3,
    unsigned long nParamType4, void* pParam4, unsigned long nParamType5,
    void* pParam5, unsigned long nParamType6, void* pParam6,
    unsigned long nParamType7, void* pParam7, unsigned long nParamType8,
    void* pParam8, unsigned long nParamType9, void* pParam9,
    unsigned long nParamType10, void* pParam10, unsigned long nParamType11,
    void* pParam11, unsigned long nParamType12, void* pParam12,
    unsigned long nParamType13, void* pParam13)
{
    CSWSObjectActionNode* pNode = new CSWSObjectActionNode;

    unsigned short nNewGroupId;

    pNode->m_nActionType = nActionType;
    nNewGroupId = nGroupId;
    pNode->m_nField70 = 1;
    if (nGroupId == ACTION_GROUP_NEW)
    {
        nNewGroupId = m_nNextGroupId;
        m_nNextGroupId = nNewGroupId + 1;
        if (m_nNextGroupId == ACTION_GROUP_NEW)
        {
            m_nNextGroupId = 0;
        }
        m_nLastGroupId = nNewGroupId;
        pNode->m_nGroupId = nNewGroupId;
    }
    else if (nGroupId == ACTION_GROUP_LAST)
    {
        pNode->m_nGroupId = m_nLastGroupId;
    }
    else
    {
        pNode->m_nGroupId = nNewGroupId;
    }
    if (nParamType1 != 0)
    {
        AddActionNodeParameter(pNode, 0, nParamType1, pParam1);
        if (nParamType2 != 0)
        {
            AddActionNodeParameter(pNode, 1, nParamType2, pParam2);
            if (nParamType3 != 0)
            {
                AddActionNodeParameter(pNode, 2, nParamType3, pParam3);
                if (nParamType4 != 0)
                {
                    AddActionNodeParameter(pNode, 3, nParamType4, pParam4);
                    if (nParamType5 != 0)
                    {
                        AddActionNodeParameter(pNode, 4, nParamType5, pParam5);
                        if (nParamType6 != 0)
                        {
                            AddActionNodeParameter(pNode, 5, nParamType6,
                                                   pParam6);
                            if (nParamType7 != 0)
                            {
                                AddActionNodeParameter(pNode, 6, nParamType7,
                                                       pParam7);
                                if (nParamType8 != 0)
                                {
                                    AddActionNodeParameter(
                                        pNode, 7, nParamType8, pParam8);
                                    if (nParamType9 != 0)
                                    {
                                        AddActionNodeParameter(
                                            pNode, 8, nParamType9, pParam9);
                                        if (nParamType10 != 0)
                                        {
                                            AddActionNodeParameter(pNode, 9,
                                                                   nParamType10,
                                                                   pParam10);
                                            if (nParamType11 != 0)
                                            {
                                                AddActionNodeParameter(
                                                    pNode, 10, nParamType11,
                                                    pParam11);
                                                if (nParamType12 != 0)
                                                {
                                                    AddActionNodeParameter(
                                                        pNode, 11, nParamType12,
                                                        pParam12);
                                                    if (nParamType13 != 0)
                                                    {
                                                        AddActionNodeParameter(
                                                            pNode, 12,
                                                            nParamType13,
                                                            pParam13);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    m_lstActions.AddHead(pNode);

    TestActionList();
}
// PC 0x004F6920
void CSWSObject::DeleteCurrentAIAction()
{
    if (!m_lstActions.IsEmpty())
    {
        if (ClearAction(m_lstActions.GetHead(), 1) == 1)
        {
            CSWSObjectActionNode* pAction = m_lstActions.RemoveHead();

            if (pAction != NULL)
            {
                delete pAction;
            }
        }
    }
}
// PC 0x004F6970
void CSWSObject::ClearAllActions(int bClearAll)
{
    if (RenderPartyFollow && AsSWSCreature() && AsSWSCreature()->m_bInParty)
        m_sTag.CStr();

    if (m_bCommandable == 1)
    {
        CExoLinkedListNode* pPosition = m_lstActions.GetHeadPos();
        if (pPosition != NULL)
        {
            do
            {
                CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPosition);
                CExoLinkedListNode* pCurrent = pPosition;
                m_lstActions.GetNext(pPosition);
                if (ClearAction(pAction, pAction == m_lstActions.GetHead()) == 1)
                {
                    CSWSObjectActionNode* pRemoved = m_lstActions.Remove(pCurrent);
                    if (pRemoved != NULL)
                        delete pRemoved;
                }
            } while (pPosition != NULL);
        }

        if (AsSWSCreature() != NULL)
        {
            AsSWSCreature()->m_pPathfindInformation->ResetAtEndOfPath();
            AsSWSCreature()->m_nAnimationState = 1;
            AsSWSCreature()->m_oidAttemptedMovementTarget = OBJECT_INVALID;
            AsSWSCreature()->m_pPathfindInformation->SetBlockingCreature(OBJECT_INVALID);
            if (bClearAll)
                AsSWSCreature()->m_pcCombatRound->ClearAllAttacks();

            CSWSCreature* pCreature = AsSWSCreature();
            pCreature->m_sTag.CStr();
            if (AsSWSCreature()->m_bInParty)
            {
                unsigned long oidServer = AsSWSCreature()->m_nId;
                CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                unsigned long oidClient = pClient->ServerToClientObjectId(oidServer);
                int nIndex = g_pAppManager->m_pClientExoApp->GetSWParty()->GetIndex(oidClient);
                if (nIndex != -1)
                    g_pAppManager->m_pClientExoApp->GetSWParty()
                        ->m_aPartyMemberData[nIndex].m_nCharacterIndex =
                            CSWPartyMemberData::FOLLOW_UNINITIALIZED;
            }
        }
    }
}
// PC 0x004F6B50
void CSWSObject::SetAnimation(int nAnimation)
{
    SetDirtyFlag(DIRTY_ANIMATION_INDEX);
    m_nAnimation = nAnimation;
}
// PC 0x004F6B70
void CSWSObject::DoDamage(int nDamage)
{
    SetCurrentHitPoints(GetCurrentHitPoints(1) - nDamage);
}
// PC 0x004F6BB0
int CSWSObject::DoSpellImmunity(CSWSObject* pCaster)
{
    int nSpellId = pCaster->m_nLastSpellId;
    if (AsSWSCreature() != NULL && m_lstEffects.num > 0)
    {
        CSWSCreatureStats* pStats = AsSWSCreature()->m_pStats;
        for (int i = pStats->m_nSpellImmunityEffectIndex;
             i < m_lstEffects.num; i++)
        {
            CGameEffect* pEffect = m_lstEffects.element[i];
            if (m_lstEffects[i]->m_nType > CSWSEffectListHandler::EFFECT_SPELL_IMMUNITY)
            {
                break;
            }
            if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_SPELL_IMMUNITY &&
                (pEffect->GetInteger(0) == nSpellId || pEffect->GetInteger(0) == -1))
            {
                CSWCCMessageData* pTargetData = new CSWCCMessageData;
                CSWCCMessageData* pCasterData = new CSWCCMessageData;
                pTargetData->SetInteger(1, nSpellId);
                pTargetData->SetObjectID(0, m_nId);
                pTargetData->CopyTo(pCasterData);
                if (AsSWSCreature() != NULL)
                {
                    AsSWSCreature()->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_SPELL_IMMUNITY, pTargetData);
                }
                else
                {
                    delete pTargetData;
                }
                if (pCaster->AsSWSCreature() != NULL)
                {
                    pCaster->AsSWSCreature()->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_SPELL_IMMUNITY, pCasterData);
                }
                else
                {
                    delete pCasterData;
                }
                return SPELL_RESISTANCE_IMMUNE;
            }
        }
    }
    return SPELL_RESISTANCE_UNRESOLVED;
}
// PC 0x004F6D60
void CSWSObject::SetOrientation(const Vector& vOrientation)
{
    m_vOrientation = vOrientation;
    SetDirtyFlag(DIRTY_ORIENTATION_INDEX);
}
// PC 0x004F6D90
void CSWSObject::SetPosition(const Vector& vPosition, int bUpdateArea,
                             int bUnused, int bNoDirty)
{
    if (equal(m_vPosition, vPosition) == 0)
    {
        if (bNoDirty == 0)
        {
            m_aDirtyFlags[0] |= DIRTY_POSITION_MASK;
        }
        CSWSArea* pArea = GetArea();
        m_vPosition = vPosition;
        if (bUpdateArea != 0)
        {
            if (AsSWSCreature() != NULL)
            {
                AsSWSCreature();
            }
            if (pArea != NULL)
            {
                pArea->UpdatePositionInObjectsArray(this);
            }
        }
    }
}
// PC 0x004F6E30
void CSWSObject::RemoveLoopingVisualEffect(unsigned short nVisualEffectId)
{
    for (int i = m_lstLoopingVisualEffects.num - 1; i >= 0; i--)
    {
        if (m_lstLoopingVisualEffects[i]->m_nVisualEffectId ==
            nVisualEffectId)
        {
            delete m_lstLoopingVisualEffects[i];
            m_lstLoopingVisualEffects[i] = NULL;
            m_lstLoopingVisualEffects.DelIndex(i);
            return;
        }
    }
}

int CSWSObject::IsSoundPlayingInDialog()
{
    return g_pAppManager->m_pClientExoApp->IsSoundPlayingInDialog();
}

int CSWSObject::IsStuntAnimationPlayingInDialog()
{
    return g_pAppManager->m_pClientExoApp->GetInGameGui()->IsAnimationPlayingInDialog();
}

int CSWSObject::IsFadingInDialog()
{
    return g_pAppManager->m_pClientExoApp->GetInGameGui()->IsFadingInDialog();
}
// PC 0x004F6EC0
int CSWSObject::IsDialogDelay()
{
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;

    if (g_pAppManager->GetClientExoApp()->GetCanSendDialog(
            GetId(), m_pDialog->GetCurrentPlayerId(), m_pDialog) == 0)
    {
        return 1;
    }

    unsigned long nWaitFlags = m_nWaitFlags;
    int bDelay = 0;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
        &nCalendarDay, &nTimeOfDay);

    if (nWaitFlags & DIALOG_WAIT_FLAG_SOUND)
    {
        bDelay = IsSoundPlayingInDialog();
    }
    if (nWaitFlags & DIALOG_WAIT_FLAG_ANIMATION)
    {
        bDelay |= IsStuntAnimationPlayingInDialog();
    }
    if (nWaitFlags & DIALOG_WAIT_FLAG_CAMERA)
    {
        bDelay |= g_pAppManager->GetClientExoApp()->GetModule()
                      ->IsCameraAnimated();
        CGuiInGame* pGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
        unsigned __int64 nTime =
            g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    }
    if (nWaitFlags & DIALOG_WAIT_FLAG_FADE)
    {
        bDelay |= IsFadingInDialog();
    }

    if (m_nDialogReplyState != 0 && m_pDialog->m_n3C != 0)
    {
        StopSoundPlayingInDialog();
        g_pAppManager->GetServerExoApp()->GetWorldTimer()->AddWorldTimes(
            nCalendarDay, nTimeOfDay, 0, 1, &m_nDialogDelayDay,
            &m_nDialogDelayTime);
        return 0;
    }

    if (bDelay != 0)
    {
        return 1;
    }

    return g_pAppManager->GetServerExoApp()->GetWorldTimer()->CompareWorldTimes(
               nCalendarDay, nTimeOfDay, m_nDialogDelayDay,
               m_nDialogDelayTime) == -1;
}
// PC 0x004F7050
int CSWSObject::SendDialogEntry(unsigned long nEntry)
{
    if (m_pDialog == NULL)
    {
        return 0;
    }

    if (nEntry != 0xffffffff && (m_nDialogPending != 0 || IsDialogDelay() != 0))
    {
        m_nDialogReplyEntry = nEntry;
        m_nDialogState = 1;
        return 1;
    }

    m_nDialogReplyState = 0;
    unsigned long nResult = 0;
    int bSent = m_pDialog->SendDialogEntry(this, 0xffffffff, nEntry, nResult);

    if (nResult == 10)
    {
        m_nDialogReplyEntry = nEntry;
        m_nDialogState = 1;
        return 1;
    }
    if (bSent == 0)
    {
        if (m_pDialog != NULL)
        {
            m_pDialog->RunEndConversationScript(this, 0);
            m_nDialogDelayDay = 0;
            m_nDialogDelayTime = 0;
            m_nDialogState = 0;
            m_bDialogEnded = 1;
        }
        return 0;
    }
    return 1;
}
// PC 0x004F70F0
int CSWSObject::SendDialogReplies()
{
    if (m_pDialog == NULL)
    {
        return 0;
    }

    if (m_nDialogPending != 0 || IsDialogDelay() != 0)
    {
        m_nDialogState = 1;
        m_nDialogReplyEntry = -1;
    }
    else
    {
        m_nDialogReplyState = 0;
        if (m_pDialog->SendDialogReplies(this, 0xffffffff) == 0)
        {
            m_pDialog->RunEndConversationScript(this, 0);
            m_nDialogDelayDay = 0;
            m_nDialogDelayTime = 0;
            m_nDialogState = 0;
            m_bDialogEnded = 1;
        }
    }
    return 1;
}
// PC 0x004F7170
int CSWSObject::UpdateDialog()
{
    if (m_oidDialogOwner == OBJECT_INVALID)
    {
        return 0;
    }

    if (m_oidDialogOwner != m_nId)
    {
        return 0;
    }

    g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateDialogLipSync();
    m_pDialog->ClearAllPlayerHostileActions();

    if (m_nDialogPending != 0)
    {
        return 1;
    }

    if (IsDialogDelay() != 0 || m_nDialogState == 0)
    {
        if (m_pDialog->m_n60 != 0 &&
            IsSoundPlayingInDialog() == 0)
        {
            StopDialog();
        }
        return 1;
    }

    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nC0 == 0 &&
        m_bDialogSoundStopped != 0 &&
        IsSoundPlayingInDialog() != 0)
    {
        return 1;
    }

    unsigned long nReplyEntry = m_nDialogReplyEntry;
    m_nDialogState = 0;
    m_bDialogSoundStopped = 0;
    if (nReplyEntry != 0xffffffff)
    {
        return SendDialogEntry(nReplyEntry);
    }
    return SendDialogReplies();
}
// PC 0x004F7250
int CSWSObject::RemoveObjectFromDialog(unsigned long oidObject)
{
    if (m_pDialog != NULL && m_nId != oidObject)
    {
        if ((m_pDialog->m_lstPlayers.IsEmpty() &&
             m_pDialog->GetCurrentPlayerId() == oidObject) ||
            (!m_pDialog->m_lstPlayers.IsEmpty() &&
             m_pDialog->RemovePlayer(oidObject) == 0))
        {
            return StopDialog();
        }
        return 1;
    }
    return 0;
}
// PC 0x004F72B0
CSWSExpression* CSWSObject::GetListenExpressionObj(int nId)
{
    for (int i = 0; i < m_lstExpressions.num; i++)
    {
        if (m_lstExpressions[i]->m_nReserved == nId)
        {
            return m_lstExpressions[i];
        }
    }
    return NULL;
}
// PC 0x004F72F0
int CSWSObject::TestListenExpression(const CExoString& sString)
{
    int nResult = -1;

    for (int i = 0; i < m_lstExpressions.num; i++)
    {
        CSWSExpression* pExpression = m_lstExpressions[i];
        if (pExpression != NULL && pExpression->TestString(sString, 1) != 0)
        {
            nResult = m_lstExpressions[i]->m_nReserved;
            break;
        }
    }
    return nResult;
}
// PC 0x004F7350
void CSWSObject::BroadcastDialog(const CExoString& sString, float fRange,
                                int bCheckVisibility)
{
    int nObjectIndex = -1;
    CSWSObject* pObject = NULL;
    CSWSArea* pArea = GetArea();
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();

    if (pArea != NULL)
    {
        pArea->GetFirstObjectIndiceByX(&nObjectIndex, m_vPosition.m_fX - fRange);
        while (nObjectIndex >= 0)
        {
            if (nObjectIndex >= pArea->m_lstObjectIds.num)
                break;
            unsigned long oidObject = pArea->m_lstObjectIds[nObjectIndex++];
            if (pObjectArray->GetGameObject(oidObject, (CGameObject**)&pObject) !=
                CGameObjectArray::SUCCESS)
                break;
            if (pObject == NULL || oidObject == GetId())
                continue;

            Vector vPosition = pObject->m_vPosition;
            if (vPosition.m_fX > m_vPosition.m_fX + fRange)
                break;
            Vector vDelta = m_vPosition - vPosition;
            float fDistanceSquared = vDelta * vDelta;
            if (fDistanceSquared <= fRange * fRange && pObject->m_bListening != 0)
            {
                int bHeard = TRUE;
                if (pObject->AsSWSCreature() != NULL &&
                    AsSWSCreature() != NULL)
                {
                    CSWVisibilityNode* pVisible =
                        pObject->AsSWSCreature()->GetVisibleListElement(GetId());
                    if (pVisible == NULL ||
                        pVisible->m_bHeard == 0)
                        continue;
                    if (bCheckVisibility != 0 &&
                        pVisible->m_bSeen == 0 &&
                        fDistanceSquared >= OBJECT_DIALOG_UNSEEN_HEARING_DISTANCE_SQUARED)
                        bHeard = FALSE;
                }
                else if (pObject->AsSWSCreature() != NULL &&
                         AsSWSCreature() == NULL)
                {
                    Vector vDistance = m_vPosition - vPosition;
                    float fListenDistance =
                        pObject->AsSWSCreature()->GetListenCheckDistance();
                    if (vDistance * vDistance > fListenDistance * fListenDistance)
                        continue;
                }

                if (bHeard == FALSE)
                    continue;

                int nExpression = pObject->TestListenExpression(sString);
                if (nExpression >= 0)
                {
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DIALOGUE;
                    pEvent->SetInteger(0,
                        pObject->GetListenExpressionObj(nExpression)->m_lstMatches.num);
                    pEvent->SetInteger(1, nExpression);
                    for (int i = 1;
                         i < pObject->GetListenExpressionObj(nExpression)->m_lstMatches.num;
                         i++)
                    {
                        pEvent->SetString(i,
                            *pObject->GetListenExpressionObj(nExpression)->m_lstMatches.element[i]);
                    }
                    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, GetId(), oidObject,
                        CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                }
            }
        }
    }
}
// PC 0x004F76B0
int CSWSObject::HasSpellEffectApplied(unsigned long nSpellId)
{
    if (nSpellId != -1)
    {
        int nEffects = m_lstEffects.num;

        for (int i = 0; i < nEffects; i++)
        {
            if (m_lstEffects[i]->m_nSpellId == nSpellId)
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x004F76F0
void CSWSObject::BroadcastSafeProjectile(
    unsigned long oidSource, unsigned long oidTarget, const Vector& vStart,
    const Vector& vEnd, unsigned long nProjectileType,
    unsigned char nProjectileFlags, int nDelay, unsigned char nSpellType,
    unsigned char nSpellLevel, int nSpellId)
{
    if (GetArea() != NULL)
    {
        if (&GetArea()->m_lstObjectIds != NULL)
        {
            CExoArrayList<unsigned long>* pObjects = &GetArea()->m_lstObjectIds;
            for (int i = 0; i < pObjects->num; i++)
            {
                CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                    (*pObjects)[i]);
                if (pCreature != NULL)
                {
                    CSWSPlayer* pPlayer =
                        g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(
                            pCreature->m_nId);
                    if (pPlayer != NULL)
                    {
                        Vector vStartDistance;
                        vStartDistance.m_fX =
                            pCreature->m_vPosition.m_fX - vStart.m_fX;
                        vStartDistance.m_fY =
                            pCreature->m_vPosition.m_fY - vStart.m_fY;
                        vStartDistance.m_fZ =
                            pCreature->m_vPosition.m_fZ - vStart.m_fZ;
                        Vector vEndDistance;
                        vEndDistance.m_fX =
                            pCreature->m_vPosition.m_fX - vEnd.m_fX;
                        vEndDistance.m_fY =
                            pCreature->m_vPosition.m_fY - vEnd.m_fY;
                        vEndDistance.m_fZ =
                            pCreature->m_vPosition.m_fZ - vEnd.m_fZ;
                        float fRangeSquared =
                            CSWSArea::MAX_OBJECT_VISIBLE_DISTANCE *
                            CSWSArea::MAX_OBJECT_VISIBLE_DISTANCE;
                        if (vStartDistance.m_fX * vStartDistance.m_fX +
                                vStartDistance.m_fY * vStartDistance.m_fY +
                                vStartDistance.m_fZ * vStartDistance.m_fZ <=
                                    fRangeSquared ||
                            vEndDistance.m_fX * vEndDistance.m_fX +
                                vEndDistance.m_fY * vEndDistance.m_fY +
                                vEndDistance.m_fZ * vEndDistance.m_fZ <=
                                    fRangeSquared)
                        {
                            CServerExoApp* pServer =
                                g_pAppManager->m_pServerExoApp;
                            pServer->GetSWSMessage()
                                ->SendServerToPlayerSafeProjectileProjectile(
                                    pPlayer, oidSource, oidTarget, vStart, vEnd,
                                    nProjectileType, nProjectileFlags, nDelay,
                                    nSpellType, nSpellLevel, nSpellId);
                        }
                    }
                }
            }
        }
    }
}
// PC 0x004F7920
void CSWSObject::BroadcastSafeProjectile(
    unsigned long oidSource, unsigned long oidTarget, const Vector& vStart,
    const Vector& vEnd, unsigned long nProjectileType, unsigned char nSpellType,
    unsigned long nSpellId, unsigned char nSpellLevel,
    unsigned char nCasterLevel, int nDelay, int nVisualEffect)
{

    const unsigned char INVALID_BASE_ITEM = 255;
    unsigned char nBaseItem = INVALID_BASE_ITEM;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pSource = pServer->GetCreatureByGameObjectID(oidSource);
    if (pSource != NULL)
    {
        CSWInventory* pInventory = pSource->m_pInventory;
        CSWSItem* pWeapon = pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
        if (pWeapon != NULL)
        {
            nBaseItem = pWeapon->m_nBaseItem;
        }
    }

    CSWSArea* pArea = GetArea();
    if (pArea != NULL)
    {
        CExoArrayList<unsigned long>* pObjects = &pArea->m_lstObjectIds;
        if (pObjects != NULL)
        {
            for (int i = 0; i < pObjects->num; i++)
            {
                CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
                    (*pObjects)[i]);
                if (pCreature != NULL)
                {
                    CSWSPlayer* pPlayer =
                        g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(
                            pCreature->m_nId);
                    if (pPlayer != NULL)
                    {
                        Vector vStartDistance;
                        vStartDistance.m_fX =
                            pCreature->m_vPosition.m_fX - vStart.m_fX;
                        vStartDistance.m_fY =
                            pCreature->m_vPosition.m_fY - vStart.m_fY;
                        vStartDistance.m_fZ =
                            pCreature->m_vPosition.m_fZ - vStart.m_fZ;
                        Vector vEndDistance;
                        vEndDistance.m_fX =
                            pCreature->m_vPosition.m_fX - vEnd.m_fX;
                        vEndDistance.m_fY =
                            pCreature->m_vPosition.m_fY - vEnd.m_fY;
                        vEndDistance.m_fZ =
                            pCreature->m_vPosition.m_fZ - vEnd.m_fZ;
                        float fRangeSquared =
                            CSWSArea::MAX_OBJECT_VISIBLE_DISTANCE *
                            CSWSArea::MAX_OBJECT_VISIBLE_DISTANCE;
                        if (vStartDistance.m_fX * vStartDistance.m_fX +
                                vStartDistance.m_fY * vStartDistance.m_fY +
                                vStartDistance.m_fZ * vStartDistance.m_fZ <=
                                    fRangeSquared ||
                            vEndDistance.m_fX * vEndDistance.m_fX +
                                vEndDistance.m_fY * vEndDistance.m_fY +
                                vEndDistance.m_fZ * vEndDistance.m_fZ <=
                                    fRangeSquared)
                        {
                            CServerExoApp* pServer =
                                g_pAppManager->m_pServerExoApp;
                            pServer->GetSWSMessage()
                                ->SendServerToPlayerSafeProjectileSpell(
                                    pPlayer, oidSource, oidTarget, vStart, vEnd,
                                    nProjectileType, nSpellType, nSpellId,
                                    nBaseItem, nSpellLevel, nCasterLevel,
                                    nDelay, nVisualEffect);
                        }
                    }
                }
            }
        }
    }
}
// PC 0x004F7B40
void CSWSObject::SpellCastAndImpact(
    unsigned long nSpellId, const Vector& vTargetPosition,
    unsigned long oidTarget, unsigned char nMultiClass,
    unsigned long oidItem, int bSpellCountered, int bCounteringSpell,
    unsigned char nProjectilePathType, int bInstantSpell)
{
    CExoString sScript;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
    if (pSpell != NULL)
    {
        unsigned char nSpellType = nProjectilePathType ?
            SPELL_PROJECTILE_TYPE_CUSTOM_PATH : SPELL_PROJECTILE_TYPE_NORMAL;
        if (!m_bSafeProjectileBroadcast)
        {
            if (!bInstantSpell && AsSWSCreature() != NULL)
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, GetId());
                pData->SetObjectID(1, oidItem);
                pData->SetObjectID(2, oidTarget);
                pData->SetInteger(0, nSpellId);
                pData->SetInteger(1, pSpell->m_nForcePoints);
                short nAlignment = AsSWSCreature()->GetStats()->m_nGoodEvilAlignment;
                pData->SetInteger(2, pSpell->GetAdjustedForcePoints(
                    nAlignment, AsSWSCreature()->GetInParty()));
                int nForcePoints = AsSWSCreature()->GetStats()->GetCurrentForcePoints();
                pData->SetInteger(3, nForcePoints);
                BroadcastSpellData(pSpell, pData);
                delete pData;
            }

            CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()
                ->GetCreatureByGameObjectID(oidTarget);
            if (pTarget != NULL)
                BroadcastSafeProjectile(GetId(), oidTarget, m_vPosition,
                    pTarget->m_vPosition, m_nLastSpellProjectileTime,
                    nSpellType, nSpellId, 0, nProjectilePathType, 0, 0);
            else
                BroadcastSafeProjectile(GetId(), oidTarget, m_vPosition,
                    m_vSpellTargetPosition, m_nLastSpellProjectileTime,
                    nSpellType, nSpellId, 0, nProjectilePathType, 0, 0);

            CSWSSpellScriptData* pScriptData = new CSWSSpellScriptData;
            pScriptData->m_nSpellId = nSpellId;
            pScriptData->m_sScript = pSpell->m_sImpactScript;
            pScriptData->m_oidTarget = oidTarget;
            pScriptData->m_vTargetPosition = vTargetPosition;
            pScriptData->m_oidCaster = m_nId;
            pScriptData->m_oidItem = oidItem;
            if (GetArea() != NULL)
                pScriptData->m_oidArea = GetArea()->GetId();
            else
                pScriptData->m_oidArea = CGameObjectArray::INVALIDOBJID;
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                ->AddEventDeltaTime(0, m_nLastSpellProjectileTime, GetId(),
                    GetId(), CServerAIMaster::EVENT_SPELL_IMPACT, pScriptData);
        }
        m_bSafeProjectileBroadcast = 1;
    }
}
// PC 0x004F7FE0
unsigned long CSWSObject::SpawnBodyBag()
{
    if (m_nObjectType != CGameObject::OBJECT_TYPE_CREATURE &&
        m_nObjectType != CGameObject::OBJECT_TYPE_PLACEABLE)
        return OBJECT_INVALID;

    CSWSCreature* pCreature = AsSWSCreature();
    CSWSPlaceable* pPlaceable = AsSWSPlaceable();
    CSWSPlaceable* pBodyBag = NULL;
    if (pCreature == NULL && pPlaceable == NULL)
        return OBJECT_INVALID;

    g_pAppManager->GetServerExoApp()->GetSWSMessage()
        ->SendServerToAllPlayersCreatureUpdate_StripEquippedItems(GetId());

    if (GetArea() == NULL)
        return OBJECT_INVALID;

    int bCorpse = 0;
    unsigned char nAppearance;
    if (pCreature != NULL)
    {
        nAppearance = pCreature->GetBodyBagAppearance();
        int nBodyBag = pCreature->m_nBodyBag;
        if (nBodyBag == 0)
        {
            bool bFailed;
            {
                CExoString sColumn("BODY_BAG");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pAppearance = p2DArrays->m_pAppearance;
                bFailed = !pAppearance->GetINTEntry(
                    pCreature->m_cAppearanceInfo.m_nAppearance, sColumn, &nBodyBag);
            }
            if (bFailed)
                nBodyBag = 0;
        }
        int nCorpse;
        int bFoundCorpse;
        {
            CExoString sColumn("Corpse");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pBodyBag);
            C2DA* pBodyBagTable = p2DArrays->m_pBodyBag;
            bFoundCorpse = pBodyBagTable->GetINTEntry(
                nBodyBag, sColumn, &nCorpse);
        }
        if (bFoundCorpse)
            bCorpse = nCorpse != 0;
        if (!bCorpse && pCreature->GetItemCount(1, 0, 0) == 0 &&
            pCreature->GetGold() == 0)
            return OBJECT_INVALID;
    }
    else if (pPlaceable != NULL)
    {
        if (pPlaceable->GetItemCount(0) == 0)
            return OBJECT_INVALID;
        nAppearance = pPlaceable->GetBodyBagAppearance();
    }

    pBodyBag = new CSWSPlaceable(OBJECT_INVALID);
    if (pBodyBag == NULL)
        return OBJECT_INVALID;
    if (!pBodyBag->LoadBodyBag(nAppearance))
    {
        delete pBodyBag;
        return OBJECT_INVALID;
    }
    pBodyBag->AcquireItemsFromObject(GetId(), 1);
    pBodyBag->m_bIsBodyBag = 1;
    if (pCreature != NULL)
    {
        pBodyBag->m_oidBodyBagCreature = GetId();
        pBodyBag->m_bIsBodyBagVisible = 0;
    }

    CServerAIBodyBagInfo* pInfo = new CServerAIBodyBagInfo;
    if (pInfo == NULL)
    {
        delete pBodyBag;
        return OBJECT_INVALID;
    }
    pInfo->m_nBodyBagId = pBodyBag->GetId();
    Vector vPosition = m_vPosition;
    pInfo->m_vPosition = vPosition;

    CExoLocString lsName;
    int nName;
    if (pCreature != NULL)
    {
        int bFoundName;
        {
            CExoString sColumn("Name");
            unsigned char nBodyBag = pCreature->m_nBodyBag;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pBodyBagTable = p2DArrays->m_pBodyBag;
            bFoundName = pBodyBagTable->GetINTEntry(
                nBodyBag, sColumn, &nName);
        }
        if (bFoundName)
        {
            lsName.m_dwStringRef = nName;
            pBodyBag->m_lsLocName = lsName;
        }
    }
    else if (pPlaceable != NULL)
    {
        int bFoundName;
        {
            CExoString sColumn("Name");
            unsigned char nBodyBag = pPlaceable->m_nBodyBag;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pBodyBagTable = p2DArrays->m_pBodyBag;
            bFoundName = pBodyBagTable->GetINTEntry(
                nBodyBag, sColumn, &nName);
        }
        if (bFoundName)
        {
            lsName.m_dwStringRef = nName;
            pBodyBag->m_lsLocName = lsName;
        }
    }
    pBodyBag->m_bIsCorpse = bCorpse;
    pBodyBag->m_bDieWhenEmpty = !bCorpse;
    if (bCorpse)
    {
        CItemRepository* pRepository = pBodyBag->m_pcItemRepository;
        if (pRepository->m_lstItems.num <= 0u)
            pBodyBag->m_bHasInventory = 0;
    }
    pBodyBag->m_bPlotObject = !bCorpse;
    pBodyBag->SetOrientation(YawPitchRoll(Yaw(m_vOrientation), Pitch(m_vOrientation), 0.0f));
    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
        0, BODY_BAG_SPAWN_DELAY, GetId(), GetArea()->GetId(), CServerAIMaster::EVENT_SPAWN_BODY_BAG, pInfo);
    pBodyBag->m_bPartyInteract = 1;
    return pBodyBag->GetId();
}
// PC 0x004F4E30
CSWSArea* CSWSObject::GetArea()
{
    CGameObject* pObject = NULL;

    CGameObjectArray* pArray = g_pAppManager->m_pServerExoApp->GetObjectArray();

    EXOXBOXASSERT(pArray);
    pArray->GetGameObject(m_oidArea, &pObject);
    if (pObject != NULL)
    {
        return pObject->AsSWSArea();
    }
    return NULL;
}
// PC 0x004F4E70
void CSWSObject::SetArea(CSWSArea* pArea)
{
    if (pArea != NULL)
    {
        m_oidArea = pArea->m_nId;
    }
    else
    {
        m_oidArea = CGameObjectArray::INVALIDOBJID;
    }
}
// PC 0x004F4EA0
unsigned char CSWSObject::GetGender()
{
    return CTlkTable::GENDER_MALE;
}
// PC 0x004F4EB0
void CSWSObject::SetDialogDelay(float fDelay)
{
    g_pAppManager->m_pServerExoApp->GetWorldTimer()->GetWorldTime(
        &m_nDialogStartDay, &m_nDialogStartTime);

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetWorldTimer()->AddWorldTimes(
        m_nDialogStartDay, m_nDialogStartTime, 0,
        (unsigned __int64)(CWorldTimer::MILLISECONDS_IN_SECOND * fDelay),
        &m_nDialogDelayDay, &m_nDialogDelayTime);
}
// PC 0x004F4F30
int CSWSObject::RunDialogOneLiner(const CExoString& sDialogResRef,
                                  unsigned long oidTokenTarget)
{
    CResRef cDialogResRef;
    CSWSDialog cDialog;
    CExoLocString lsText;
    CResRef cSound;
    CResRef cScript;
    unsigned char nGender = CTlkTable::GENDER_MALE;
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();

    if (sDialogResRef == "")
    {
        cDialogResRef = GetDialogResref();
    }
    else
    {
        cDialogResRef = sDialogResRef;
    }

    if (!(cDialogResRef == ""))
    {
        CResGFF* pGFF = new CResGFF(RESTYPE_DLG, "DLG ", cDialogResRef);
        if (pGFF->m_bLoaded == 0)
        {
            delete pGFF;
            return 0;
        }

        if (cDialog.LoadDialog(pGFF, 0) == 0)
        {
            delete pGFF;
            return 0;
        }
        delete pGFF;

        if (cDialog.GetStartEntryOneLiner(this, lsText, cSound, cScript) == 0)
        {
            return 0;
        }

        CGameObject* pTarget;
        if (pObjectArray->GetGameObject(oidTokenTarget, &pTarget) ==
                CGameObjectArray::SUCCESS &&
            pTarget->AsSWSObject() != NULL)
        {
            nGender = pTarget->AsSWSObject()->GetGender();
        }

        pMessage->SendServerToPlayerChatMultiLangMessage(
            CSWSMessage::CHAT_TYPE_MULTILANG, GetId(), lsText, oidTokenTarget,
            nGender, NULL, 0, 0, cSound, OBJECT_INVALID, 0);
        cDialog.RunScript(this, cScript);
        return 1;
    }
    return 0;
}
// PC 0x004F5190
int CSWSObject::ReplyDialog(unsigned long nEntry, unsigned long nReply,
                            int bEscape, unsigned long oidSpeaker, int bSkip)
{
    if (m_pDialog == NULL)
    {
        return 0;
    }

    if (m_pDialog->HandleReply(nEntry, this, nReply, bEscape, oidSpeaker,
                               bSkip) == 0)
    {
        if (m_pDialog != NULL)
        {
            m_pDialog->RunEndConversationScript(this, 0);
            m_nDialogDelayDay = 0;
            m_nDialogDelayTime = 0;
            m_nDialogState = 0;
            m_bDialogEnded = 1;
        }
    }
    return 1;
}
// PC 0x004F5200
int CSWSObject::StopDialog()
{
    CSWSObject* pOwner;

    if (AsSWSCreature() != NULL)
    {
        AsSWSCreature()->SetLockOrientationToObject(
            CGameObjectArray::INVALIDOBJID, 0);
    }

    if (m_oidDialogOwner == OBJECT_INVALID)
    {
        return 0;
    }

    if (m_oidDialogOwner != m_nId)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;

        if (pServerExoApp->GetObjectArray()->GetGameObject(
                m_oidDialogOwner, (CGameObject**)&pOwner) ==
            CGameObjectArray::SUCCESS)
        {
            pOwner->RemoveObjectFromDialog(m_nId);
            m_oidDialogOwner = OBJECT_INVALID;
        }
        return 1;
    }

    if (m_pDialog == NULL)
    {
        return 0;
    }

    m_pDialog->RunEndConversationScript(this, 0);
    m_nDialogDelayDay = 0;
    m_nDialogDelayTime = 0;
    m_nDialogState = 0;
    m_nDialogPending = 0;
    delete m_pDialog;
    m_pDialog = NULL;
    return 1;
}
// PC 0x004F52B0
void CSWSObject::StopSoundPlayingInDialog()
{
    m_bDialogSoundStopped = 1;
    g_pAppManager->m_pClientExoApp->StopSoundPlayingInDialog();
}
// PC 0x004F52D0
unsigned long CSWSObject::GetNearestObjectByName(const CExoString& sName,
                                               float fDistance)
{
    unsigned long oidNearest = OBJECT_INVALID;
    int bDistanceLimit = fDistance != 0.0f;
    float fNearestDistance = fDistance * fDistance;
    CSWSArea* pArea = GetArea();
    if (pArea == NULL || sName == "")
    {
        return OBJECT_INVALID;
    }

    Vector vPosition = m_vPosition;
    unsigned long oidObject;
    int bMore = pArea->GetFirstObjectInArea(oidObject);
    while (bMore == 1)
    {
        CGameObject* pGameObject;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidObject, &pGameObject) != CGameObjectArray::SUCCESS)
        {
            bMore = pArea->GetNextObjectInArea(oidObject);
            continue;
        }
        CSWSObject* pObject = (CSWSObject*)pGameObject;
        Vector vObjectPosition = pObject->m_vPosition;
        if (pObject->GetArea() == pArea &&
            pObject->m_sTag.CompareNoCase(sName))
        {
            Vector vDifference = vObjectPosition - vPosition;
            float fDistanceSquared = vDifference * vDifference;
            if (!bDistanceLimit || fDistanceSquared <= fNearestDistance)
            {
                oidNearest = pObject->GetId();
                bDistanceLimit = 1;
                fNearestDistance = (float)sqrt(fDistanceSquared);
            }
        }
        bMore = pArea->GetNextObjectInArea(oidObject);
    }
    return oidNearest;
}
// PC 0x004F54C0
void CSWSObject::SetDialogOwner(unsigned long oidOwner)
{
    m_oidDialogOwner = oidOwner;
}
// PC 0x004F54D0
CScriptLocation CSWSObject::GetScriptLocation()
{
    CScriptLocation cLocation;

    cLocation.m_vPosition = m_vPosition;
    cLocation.m_vOrientation = m_vOrientation;
    return cLocation;
}
// PC 0x004F5530
int CSWSObject::GetDead()
{
    if (AsSWSCreature() != NULL &&
        AsSWSCreature()->m_nAreaWaitState != 0)
    {
        return ((CSWSObject*)AsSWSCreature())->GetCurrentHitPoints(0) <=
               -10;
    }
    else if (AsSWSCreature() != NULL)
    {
        return ((CSWSObject*)AsSWSCreature())->GetCurrentHitPoints(0) <=
               0;
    }
    else
    {
        return GetCurrentHitPoints(0) <= 0;
    }
}
// PC 0x004F55B0
void CSWSObject::BroadcastSpellData(CSWSpell* pSpell, CSWCCMessageData* pData)
{
    if (GetArea() != NULL)
    {
        unsigned long oidPlayer =
            g_pAppManager->GetServerExoApp()->GetFirstPCObject();
        CSWSCreature* pCreature =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidPlayer);
        if (pCreature != NULL)
        {
            CSWSPlayer* pPlayer =
                g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
            Vector vDistance = pCreature->m_vPosition - m_vPosition;
            if (pPlayer != NULL &&
                vDistance * vDistance <= SPELL_DATA_BROADCAST_RANGE_SQUARED)
            {
                if (AsSWSCreature() != NULL &&
                    pData->GetObjectID(CSWCCMessageData::SPELL_DATA_ITEM) != OBJECT_INVALID)
                {
                    CSWSItem* pItem =
                        g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(
                            pData->GetObjectID(CSWCCMessageData::SPELL_DATA_ITEM));
                    if (pItem != NULL)
                        pData->SetInteger(CSWCCMessageData::SPELL_DATA_BASE_ITEM,
                                          pItem->GetBaseItemType());
                }
                unsigned long nPlayerId = pPlayer->m_nPlayerId;
                g_pAppManager->GetServerExoApp()->GetSWSMessage()
                    ->SendServerToPlayerCCMessage(nPlayerId,
                        CSWSMessage::CC_MESSAGE_SPELL_DATA, pData, NULL);
            }
        }
    }
}
// PC 0x004F5700
void CSWSObject::CalculateLastSpellProjectileTime(unsigned char nProjectileType)
{
    Vector vDistance;
    int nSpellId = m_nLastSpellId;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
    if (pSpell == NULL)
        return;
    if (!pSpell->m_nProjectile)
    {
        m_nLastSpellProjectileTime = 0;
        return;
    }

    if (m_oidSpellTargetObject != CGameObjectArray::INVALIDOBJID)
    {
        CGameObject* pTarget;
        CGameObjectArray* pArray = g_pAppManager->m_pServerExoApp->GetObjectArray();
        if (pArray->GetGameObject(
                m_oidSpellTargetObject, &pTarget) == CGameObjectArray::SUCCESS)
            vDistance = ((CSWSObject*)pTarget)->m_vPosition - m_vPosition;
    }
    else
        vDistance = m_vSpellTargetPosition - m_vPosition;

    float fSpeed = (float)log(magnitude(vDistance)) * 3.0f + 2.0f;
    if (nProjectileType == 0)
        nProjectileType = pSpell->m_nProjectileType;
    switch (nProjectileType)
    {
    case CSWSpell::PROJECTILE_TYPE_DOUBLE_SPEED:
        fSpeed *= 2.0f;
        break;
    case CSWSpell::PROJECTILE_TYPE_FIXED_TIME:
        m_nLastSpellProjectileTime = SPELL_PROJECTILE_FIXED_TIME;
        return;
    case CSWSpell::PROJECTILE_TYPE_FAST:
        fSpeed *= 1.5f;
        break;
    case CSWSpell::PROJECTILE_TYPE_DISTANCE_SPEED:
        fSpeed = magnitude(vDistance) * 0.5f;
        break;
    case CSWSpell::PROJECTILE_TYPE_SLOW:
        fSpeed *= 0.4f;
        break;
    }

    if (fSpeed <= 0.0f)
        m_nLastSpellProjectileTime = 1;
    else
    {
        m_nLastSpellProjectileTime = (unsigned long)(magnitude(vDistance) / fSpeed * 1000.0f);
        if (pSpell->m_nProjectileType == CSWSpell::PROJECTILE_TYPE_DELAYED)
            m_nLastSpellProjectileTime += SPELL_PROJECTILE_EXTRA_DELAY;
    }
}
// PC 0x004F5920
Vector CSWSObject::CalculateSpellRangedMissTarget(unsigned long oidSource,
                                                 unsigned long oidTarget)
{
    float fPersonalSpace = 0.5f;
    Vector vMiss;
    Vector vResult;
    CGameObject* pObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidSource);
    if (pObject == NULL || pObject->AsSWSObject() == NULL)
        return vResult;
    CSWSObject* pSource = pObject->AsSWSObject();
    pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pObject == NULL || pObject->AsSWSObject() == NULL)
        return vResult;
    CSWSObject* pTarget = pObject->AsSWSObject();
    if (pTarget->AsSWSCreature() != NULL)
    {
        g_pRules->Get2DArrays()->GetAppearance()->GetFLOATEntry(
            pTarget->AsSWSCreature()->GetStats()->GetAppearanceType(),
            "creperspace", &fPersonalSpace);
        fPersonalSpace -= 0.1f;
    }
    float fDistance =
        (float)(rand() % SPELL_RANGED_MISS_DISTANCE_STEPS + 1) / 1000.0f;
    Vector vDistanceToTarget = pSource->m_vPosition - pTarget->m_vPosition;
    if (vDistanceToTarget * vDistanceToTarget >
        SPELL_RANGED_MISS_LONG_DISTANCE_SQUARED)
    {
        Vector vDirection;
        vDirection = normalize(pSource->m_vPosition - pTarget->m_vPosition);
        float fAngle = (float)atan(vDirection.m_fY / vDirection.m_fX);
        if (rand() % 2)
            fAngle -= (float)(rand() % SPELL_RANGED_MISS_ANGLE_STEPS) / 1000.0f;
        else
            fAngle += (float)(rand() % SPELL_RANGED_MISS_ANGLE_STEPS) / 1000.0f;
        if (fAngle > TWO_PI)
            fAngle -= TWO_PI;
        if (fAngle < 0.0f)
            fAngle += TWO_PI;
        vMiss.m_fX = (float)cos(fAngle);
        vMiss.m_fY = (float)sin(fAngle);
        if (vDirection.m_fX < 0.0f)
            vMiss = -vMiss;
        vMiss *= fDistance;
        vMiss += pTarget->m_vPosition;
    }
    else
    {
        vMiss = pTarget->m_vPosition - pSource->m_vPosition;
        Vector vDirection;
        if (rand() % 2)
        {
            vDirection.m_fX = normalize(vMiss).m_fY;
            vDirection.m_fY = -normalize(vMiss).m_fX;
        }
        else
        {
            vDirection.m_fX = -normalize(vMiss).m_fY;
            vDirection.m_fY = normalize(vMiss).m_fX;
        }
        vDirection += fPersonalSpace * vDirection;
        Vector vDistance = pTarget->m_vPosition - pSource->m_vPosition;
        if (vDistance * vDistance >
            SPELL_RANGED_MISS_SCATTER_DISTANCE_SQUARED)
            vDirection += ((float)(rand() % (int)(fDistance * 1000.0f)) / 1000.0f) * vDirection;
        vMiss += vDirection;
        vMiss = normalize(vMiss) * ((float)(rand() % 10) + 10.0f);
        vMiss.m_fZ = pTarget->m_vPosition.m_fZ + 1.0f;
        vMiss += pTarget->m_vPosition;
    }
    return vResult;
}
// PC 0x004F5CB0
void CSWSObject::SetLastHostileActor(unsigned long oidActor, int bForce)
{
    if (bForce == 1)
    {
        m_oidLastHostileActor = oidActor;
        return;
    }
    if (m_bPlotObject == 1)
    {
        m_oidLastHostileActor = OBJECT_INVALID;
        return;
    }

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSObject* pObject = (CSWSObject*)pServer->GetGameObject(oidActor);
    if (pObject != NULL)
    {
        CSWSCreature* pCreature = NULL;
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT)
        {
            unsigned long oidCreator = pObject->AsSWSAreaOfEffectObject()->m_oidCreator;
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            pCreature = pServer->GetCreatureByGameObjectID(oidCreator);
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE &&
                 pObject->m_nId != m_nId)
        {
            pCreature = pObject->AsSWSCreature();
        }

        if (pCreature != NULL &&
            ((CSWSObject*)pCreature)->GetDead() != 1 &&
            pCreature->GetDeadTemp() != 1)
        {
            if (AsSWSCreature() != NULL && GetArea() != NULL)
            {
                int nFactionId = pCreature->m_pStats->m_nFactionId;
                if (nFactionId == AsSWSCreature()->m_pStats->m_nFactionId)
                {
                    return;
                }
            }
            m_oidLastHostileActor = pCreature->m_nId;
        }
        else
        {
            m_oidLastHostileActor = OBJECT_INVALID;
        }
    }
}
// PC 0x004F5DB0
int CSWSObject::AnimationWalking(unsigned short nAnimation)
{
    return nAnimation == 10002 || nAnimation == 10093 || nAnimation == 10133;
}
// PC 0x004F5DE0
void CSWSObject::SetDirtyFlag(unsigned long nFlag)
{
#ifdef _XBOX
    unsigned long nWord = nFlag >> 5;
    unsigned long nBit = nFlag - 32 * nWord;
    EXOXBOXASSERT(nWord <= 0);
    EXOXBOXASSERT(nBit <= 31);
    m_aDirtyFlags[nWord] |= 1 << nBit;
#else
    m_aDirtyFlags[nFlag >> 5] |= 1 << (nFlag - 32 * (nFlag >> 5));
#endif
}
// PC 0x004F5E10
int CSWSObject::IsDirtyFlagSet(unsigned long nFlag)
{
#ifdef _XBOX
    unsigned long nWord = nFlag >> 5;
    unsigned long nBit = nFlag - 32 * nWord;
    EXOXBOXASSERT(nWord <= 0);
    EXOXBOXASSERT(nBit <= 31);
    return (m_aDirtyFlags[nWord] & (1 << nBit)) != 0;
#else
    return (m_aDirtyFlags[nFlag >> 5] & (1 << (nFlag - 32 * (nFlag >> 5)))) !=
           0;
#endif
}
// PC 0x004F5E40
void CSWSObject::ClearDirtyFlag(unsigned long nFlag)
{
#ifdef _XBOX
    unsigned long nWord = nFlag >> 5;
    unsigned long nBit = nFlag - 32 * nWord;
    EXOXBOXASSERT(nWord <= 0);
    EXOXBOXASSERT(nBit <= 31);
    m_aDirtyFlags[nWord] &= -1 - (1 << nBit);
#else
    m_aDirtyFlags[nFlag >> 5] &= -1 - (1 << (nFlag - 32 * (nFlag >> 5)));
#endif
}
// PC 0x004F5E70
void CSWSObject::SetCurrentHitPoints(int nHitPoints)
{
    m_nCurrentHitPoints = nHitPoints;
    if (m_bMin1HP != 0 && nHitPoints < 1)
    {
        m_nCurrentHitPoints = 1;
    }
}
// PC 0x004F5EA0
CSWCObject* CSWSObject::GetClientObject()
{
    if (m_pClientObject == NULL)
    {
        unsigned long oidClient =
            g_pAppManager->m_pClientExoApp->ServerToClientObjectId(m_nId);
        CSWCObject* pObject =
            g_pAppManager->m_pClientExoApp->GetGameObject(oidClient);
        if (pObject != NULL)
        {
            m_pClientObject = pObject->AsSWCObject();
            if (m_pClientObject != NULL)
            {
                m_pClientObject->AttachmentFromServerObject(this);
            }
        }
    }
    return m_pClientObject;
}
// PC 0x004F5F00
void CSWSObject::AttachmentFromClientObject(CSWCObject* pObject)
{
    m_pClientObject = pObject;
}
// PC 0x004F5F10
void CSWSObject::DetachFromClientObject()
{
    m_pClientObject = NULL;
}
// PC 0x004F5F20
void CSWSObject::DeleteDialog()
{
    if (m_pDialog != NULL)
    {
        delete m_pDialog;
        m_pDialog = NULL;
    }
    m_bDialogEnded = 0;
}
// PC 0x004F5F50
void CSWSObject::TimeStampCameraAnimation()
{
    m_nCameraAnimState = 0;
    m_nCameraTimeStamp =
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
}
// PC 0x004F5F80
int CSWSObject::ClearAction(CSWSObjectActionNode* pNode, int bRemove)
{
    if (pNode->m_nActionType == 24)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->SetGlobalDialogState(0);
        return 1;
    }
    if (pNode->m_nActionType == 32)
    {
        m_nDialogPending = 0;
    }
    return 1;
}
// PC 0x004F8440
int CSWSObject::GetHasFeatEffectApplied(unsigned short nFeat)
{
    CSWFeat* pFeat = g_pRules->GetFeat(nFeat);

    if (pFeat == NULL)
    {
        return 0;
    }

    unsigned short nSpellId = pFeat->m_nSpellId;

    if (nSpellId != -1)
    {
        int nEffects = m_lstEffects.num;

        for (int i = 0; i < nEffects; i++)
        {
            if (m_lstEffects[i]->m_nSpellId == nSpellId)
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x004F84A0
void CSWSObject::ReportOverflow(int nType, int nValue, int nLimit, int nExtra)
{
    if (g_pAppManager->m_pServerExoApp->GetDebugMode() == 1)
    {
        CSWSMessage* pMessage =
            g_pAppManager->m_pServerExoApp->GetSWSMessage();
        CExoLinkedList<CSWSClient>* pPlayerList =
            g_pAppManager->m_pServerExoApp->GetPlayerList();
        if (pMessage != NULL && pPlayerList != NULL)
        {
            CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
            while (pPosition != NULL)
            {
                CSWSPlayer* pPlayer =
                    (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
                if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
                {
                    CSWSCreature* pCreature =
                        pPlayer->GetGameObject()->AsSWSCreature();
                    if (pCreature != NULL)
                    {
                        CSWCCMessageData* pData = new CSWCCMessageData;
                        pData->SetInteger(0, nValue);
                        pData->SetInteger(1, nLimit);
                        pData->SetObjectID(0, OBJECT_INVALID);
                        pData->SetObjectID(1, m_nId);
                        pData->SetString(1, m_sTag);
                        if (nType == CSWSCreature::FEEDBACK_AI_UPDATE_TIME_OVERFLOW)
                        {
                            CServerAIMaster* pAIMaster =
                                g_pAppManager->m_pServerExoApp->GetServerAIMaster();
                            pData->SetString(2, pAIMaster->m_sLastEvent);
                            pData->SetInteger(2, pAIMaster->m_nLastEvent[5]);
                            pData->SetInteger(3,
                                pAIMaster->m_nLastEvent[0] + 2 *
                                (pAIMaster->m_nLastEvent[1] + 2 *
                                (pAIMaster->m_nLastEvent[2] + 2 *
                                (pAIMaster->m_nLastEvent[3] + 2 *
                                 pAIMaster->m_nLastEvent[4]))));
                        }
                        pCreature->SendFeedbackMessage(nType, pData);
                    }
                }
                pPlayerList->GetNext(pPosition);
            }
        }
    }
}
// PC 0x004F8660
void CSWSObject::SetFadeOutTime(unsigned long nFadeOutTime)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSPlayer* pPlayer = pServerExoApp->GetClientObjectByObjectId(
        pServerExoApp->GetPlayerCreatureId());

    if (pPlayer != NULL)
    {
        CExoLinkedListNode* pNode =
            pPlayer->m_pActiveObjectsLastUpdate->GetHeadPos();

        while (pNode != NULL)
        {
            CLastUpdateObject* pLastUpdateObject =
                pPlayer->m_pActiveObjectsLastUpdate->GetAtPos(pNode);

            if (pLastUpdateObject->m_oidObject == m_nId)
            {
                pLastUpdateObject->m_nFadeOutTime = nFadeOutTime;
                return;
            }
            pPlayer->m_pActiveObjectsLastUpdate->GetNext(pNode);
        }
    }
    m_nFadeOutTime = nFadeOutTime;
}
// PC 0x004F86E0
void CSWSObject::SetDeleteNoFade(int bDeleteNoFade)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSPlayer* pPlayer = pServerExoApp->GetClientObjectByObjectId(
        pServerExoApp->GetPlayerCreatureId());

    if (pPlayer != NULL)
    {
        CExoLinkedListNode* pNode =
            pPlayer->m_pActiveObjectsLastUpdate->GetHeadPos();

        while (pNode != NULL)
        {
            CLastUpdateObject* pLastUpdateObject =
                pPlayer->m_pActiveObjectsLastUpdate->GetAtPos(pNode);

            if (pLastUpdateObject->m_oidObject == m_nId)
            {
                pLastUpdateObject->m_bDeleteNoFade = bDeleteNoFade;
                return;
            }
            pPlayer->m_pActiveObjectsLastUpdate->GetNext(pNode);
        }
    }
    m_bDeleteNoFade = bDeleteNoFade;
}
// PC 0x004F8760
void CSWSObject::SetKeepCorpse(int bKeepCorpse)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSPlayer* pPlayer = pServerExoApp->GetClientObjectByObjectId(
        pServerExoApp->GetPlayerCreatureId());

    if (pPlayer != NULL)
    {
        CExoLinkedListNode* pNode =
            pPlayer->m_pActiveObjectsLastUpdate->GetHeadPos();

        while (pNode != NULL)
        {
            CLastUpdateObject* pLastUpdateObject =
                pPlayer->m_pActiveObjectsLastUpdate->GetAtPos(pNode);

            if (pLastUpdateObject->m_oidObject == m_nId)
            {
                pLastUpdateObject->m_bKeepCorpse = bKeepCorpse;
                return;
            }
            pPlayer->m_pActiveObjectsLastUpdate->GetNext(pNode);
        }
    }
    m_bKeepCorpse = bKeepCorpse;
}
// PC 0x004F87E0
void CSWSObject::AddAction(
    unsigned long nActionType, unsigned short nGroupId,
    unsigned long nParamType1, void* pParam1, unsigned long nParamType2,
    void* pParam2, unsigned long nParamType3, void* pParam3,
    unsigned long nParamType4, void* pParam4, unsigned long nParamType5,
    void* pParam5, unsigned long nParamType6, void* pParam6,
    unsigned long nParamType7, void* pParam7, unsigned long nParamType8,
    void* pParam8, unsigned long nParamType9, void* pParam9,
    unsigned long nParamType10, void* pParam10, unsigned long nParamType11,
    void* pParam11, unsigned long nParamType12, void* pParam12,
    unsigned long nParamType13, void* pParam13)
{
    CSWSObjectActionNode* pNode = new CSWSObjectActionNode;

    unsigned short nNewGroupId;

    pNode->m_nActionType = nActionType;
    nNewGroupId = nGroupId;
    pNode->m_nField70 = 1;
    if (nGroupId == ACTION_GROUP_NEW)
    {
        nNewGroupId = m_nNextGroupId;
        m_nNextGroupId = nNewGroupId + 1;
        if (m_nNextGroupId == ACTION_GROUP_NEW)
        {
            m_nNextGroupId = 0;
        }
        m_nLastGroupId = nNewGroupId;
        pNode->m_nGroupId = nNewGroupId;
    }
    else if (nGroupId == ACTION_GROUP_LAST)
    {
        pNode->m_nGroupId = m_nLastGroupId;
    }
    else
    {
        pNode->m_nGroupId = nNewGroupId;
    }
    if (nParamType1 != 0)
    {
        AddActionNodeParameter(pNode, 0, nParamType1, pParam1);
        if (nParamType2 != 0)
        {
            AddActionNodeParameter(pNode, 1, nParamType2, pParam2);
            if (nParamType3 != 0)
            {
                AddActionNodeParameter(pNode, 2, nParamType3, pParam3);
                if (nParamType4 != 0)
                {
                    AddActionNodeParameter(pNode, 3, nParamType4, pParam4);
                    if (nParamType5 != 0)
                    {
                        AddActionNodeParameter(pNode, 4, nParamType5, pParam5);
                        if (nParamType6 != 0)
                        {
                            AddActionNodeParameter(pNode, 5, nParamType6,
                                                   pParam6);
                            if (nParamType7 != 0)
                            {
                                AddActionNodeParameter(pNode, 6, nParamType7,
                                                       pParam7);
                                if (nParamType8 != 0)
                                {
                                    AddActionNodeParameter(
                                        pNode, 7, nParamType8, pParam8);
                                    if (nParamType9 != 0)
                                    {
                                        AddActionNodeParameter(
                                            pNode, 8, nParamType9, pParam9);
                                        if (nParamType10 != 0)
                                        {
                                            AddActionNodeParameter(pNode, 9,
                                                                   nParamType10,
                                                                   pParam10);
                                            if (nParamType11 != 0)
                                            {
                                                AddActionNodeParameter(
                                                    pNode, 10, nParamType11,
                                                    pParam11);
                                                if (nParamType12 != 0)
                                                {
                                                    AddActionNodeParameter(
                                                        pNode, 11, nParamType12,
                                                        pParam12);
                                                    if (nParamType13 != 0)
                                                    {
                                                        AddActionNodeParameter(
                                                            pNode, 12,
                                                            nParamType13,
                                                            pParam13);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    m_lstActions.AddTail(pNode);

    TestActionList();
}

static inline void NwsobjectTrace2()
{
    NwsobjectTrace1();
}

static inline void NwsobjectTrace1()
{
}

int NwsobjectRemoveFromDialog(CSWSObject* pObject, unsigned long oidObject)
{
    return pObject->RemoveObjectFromDialog(oidObject);
}

#ifdef _XBOX

unsigned short CSWSObject::GetActionByGroupId(unsigned short nGroupId,
                                              CSWSObjectActionNode** ppNode)
{
    *ppNode = NULL;
    unsigned short nActionType = 0xffff;

    if (m_lstActions.GetHeadPos() == NULL)
    {
        return 0xffff;
    }

    CExoLinkedListNode* pPos = m_lstActions.GetHeadPos();
    CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPos);
    int bFound = 0;

    while (pAction != NULL)
    {
        if (pAction->m_nGroupId == nGroupId)
        {
            bFound = 1;
            if (GetAcceptableAction(pAction->m_nActionType) == 1)
            {
                nActionType = pAction->m_nActionType;
                *ppNode = pAction;
            }
        }
        else if (bFound == 1)
        {
            break;
        }
        pAction = m_lstActions.GetNext(pPos);
    }
    return nActionType;
}

CExoLinkedListNode* CSWSObject::GetPositionByGroupIndex(int nIndex)
{
    int nGroup = 0;

    if (m_lstActions.GetHeadPos() == NULL)
    {
        return NULL;
    }

    CExoLinkedListNode* pPos = m_lstActions.GetHeadPos();
    CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPos);
    unsigned short nGroupId = pAction->m_nGroupId;

    if (nIndex == 0)
    {
        return pPos;
    }

    while (1)
    {
        if (pAction->m_nGroupId != nGroupId)
        {
            nGroupId = pAction->m_nGroupId;
            nGroup++;
        }
        if (nGroup == nIndex)
        {
            break;
        }
        pAction = m_lstActions.GetNext(pPos);
        if (pAction == NULL)
        {
            return NULL;
        }
    }
    return pPos;
}

void CSWSObject::RemoveGroup(unsigned short nGroupId)
{
    CExoLinkedListNode* pNode;

    if (m_lstActions.GetHeadPos() != NULL)
    {
        CExoLinkedListNode* pPos = m_lstActions.GetHeadPos();
        CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPos);

        while (pPos != NULL)
        {
            pNode = pPos;
            if (pAction->m_nGroupId == nGroupId)
            {
                CSWSObjectActionNode* pRemove = pAction;
                pAction = m_lstActions.GetNext(pPos);
                m_lstActions.Remove(pNode);
                if (pRemove != NULL)
                {
                    delete pRemove;
                }
            }
            else
            {
                pAction = m_lstActions.GetNext(pPos);
            }
        }
    }
}

int CSWSObject::DoSpellLevelAbsorption(CSWSObject* pCaster)
{
    if (pCaster == NULL || pCaster->AsSWSCreature() == NULL ||
        m_lstEffects.num <= 0)
    {
        return SPELL_RESISTANCE_UNRESOLVED;
    }
    {
        int nSpellId = pCaster->m_nLastSpellId;
        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
        if (pSpell != NULL)
        {
            int bFromItem = pCaster->AsSWSCreature()->m_bCasterLevelOverride;
            int nClass = CSWClass::CCLASS_INVALID;
            if (bFromItem != 1)
            {
                unsigned char nMultiClass = pCaster->m_c1D4;
                CSWSCreatureStats* pStats = pCaster->AsSWSCreature()->m_pStats;
                nClass = pStats->GetClass(nMultiClass);
            }
            if (bFromItem != 1 && nClass != CSWClass::CCLASS_INVALID)
                pSpell->GetSpellLevel(nClass);
            if (pSpell->m_nUserType != CSWSpell::USER_TYPE_FORCE_POWER)
                return SPELL_RESISTANCE_UNRESOLVED;

            int nAbsorbIndex = -1;
            CSWSCreature* pCreature = AsSWSCreature();
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            int i = pStats->m_nSpellLevelAbsorptionEffectIndex;
            while (i < m_lstEffects.num)
            {
                if (m_lstEffects[i]->GetEffectType() >
                    CSWSEffectListHandler::EFFECT_SPELL_LEVEL_ABSORPTION)
                    break;
                ++i;
            }
            if (nAbsorbIndex != -1 && pStats != NULL)
                RemoveEffectById(m_lstEffects[nAbsorbIndex]->m_nID);
            pStats = AsSWSCreature()->m_pStats;
            i = pStats->m_nSpellLevelAbsorptionEffectIndex;
            while (i < m_lstEffects.num)
            {
                if (m_lstEffects[i]->GetEffectType() >
                    CSWSEffectListHandler::EFFECT_SPELL_LEVEL_ABSORPTION)
                    break;
                ++i;
            }
            if (AsSWSCreature() != NULL)
                pCaster->AsSWSCreature();
        }
    }
    return SPELL_RESISTANCE_UNRESOLVED;
}
#endif
