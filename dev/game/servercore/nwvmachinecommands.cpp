#include "nwvmachinecommands.h"
#include "exotlktable.h"

#include <stdlib.h>
#include <math.h>

#include "appmanager.h"
#include "clientexoapp.h"
#include "exoinput.h"
#include "nwscript.h"
#include "serverexoapp.h"
#include "serverinfo.h"
#include "serveraimaster.h"
#include "nwsobject.h"
#include "nwsscriptevent.h"
#include "nwseffect.h"
#include "resgff.h"
#include "exobase.h"
#include "guiingame.h"
#include "nwscreaturestats.h"
#include "nwsdoor.h"
#include "nwsitem.h"
#include "nwitem.h"
#include "nwmessage.h"
#include "nwsmodule.h"
#include "nwsparty.h"
#include "swparty.h"
#include "nwsplayer.h"
#include "nwssoundobject.h"
#include "partytables.h"
#include "worldtimer.h"
#include "nwsplaceable.h"
#include "nwstrigger.h"
#include "nwswaypoint.h"
#include "gameobjectarray.h"
#include "nwsarea.h"
#include "nwccreature.h"
#include "nwcarea.h"
#include "nwcmodule.h"
#include "swcroom.h"
#include "nwsambientsound.h"
#include "factionmanager.h"
#include "pathfindinfo.h"
#include "serverexoappinternal.h"
#include "nwseffectlist.h"
#include "nwsencounter.h"
#include "nwsspellimpact.h"
#include "aurora.h"
#include "swmain.h"
#include "clientoptions.h"
#include "nwrules.h"
#include "nwspell.h"
#include "nwfeat.h"
#include "nwrace.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "nwitem.h"
#include "nwrules.h"
#include "nwjournal.h"
#include "nwsexpression.h"
#include "nwsstore.h"
#include "nwscombatround.h"
#include "nwclass.h"
#include "nwcprojectile.h"
#include "nwcitem.h"

static inline void NwvmachinecommandsTrace1();
#define NWVMACHINECOMMANDS_TRACE() NwvmachinecommandsTrace1()
int g_nRunScriptVar = 0;
#ifdef _XBOX
int bDisableScriptLogging = 1;
#else
int bDisableScriptLogging = 0;
#endif
int LogCommandExecution = 0;
int bShipBuild = 0;
int CSWVirtualMachineCommands::ExecuteCommandExploreAreaForPlayer(int, int)  // PC 0x0040A2D0
{
    return 0;
}
// PC 0x0040A2D0
int CSWVirtualMachineCommands::ExecuteCommandGetLastAssociateCommand(int, int)
{
    return 0;
}
// PC 0x0040A2D0
int CSWVirtualMachineCommands::ExecuteCommandSetAssociateListenPatterns(int, int)
{
    return 0;
}
// PC 0x005AF400
int CSWVirtualMachineCommands::ExecuteCommandSetCreatureAILevel(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nAILevel;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAILevel) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject != NULL && pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            pCreature->m_c7C = (char)pCreature->m_nAILevel;
            int nLevel;
            switch (nAILevel)
            {
                case 0:
                    nLevel = 0;
                    break;
                case 1:
                    nLevel = 1;
                    break;
                case 2:
                    nLevel = 2;
                    break;
                case 4:
                    nLevel = 4;
                    break;
                default:
                    nLevel = 3;
                    break;
            }
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->SetAILevel(pCreature, nLevel);
        }
    }
    return 0;
}
// PC 0x005AF4D0
int CSWVirtualMachineCommands::ExecuteCommandChangeObjectAppearance(int nCommand, int nArgs)
{
    unsigned long oidObject;
    int nAppearance;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAppearance) == 0)
    {
        return -2001;
    }
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetGameObject(oidObject)->AsSWSCreature();
    if (pCreature != NULL)
    {
        pCreature->Disguise(nAppearance, NULL, 0, 1);
    }
    return 0;
}
// PC 0x005AF540
int CSWVirtualMachineCommands::ExecuteCommandResetCreatureAILevel(int nCommand, int nArgs)
{
    unsigned long oidCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject != NULL && pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            unsigned char nAILevel = pCreature->m_c7C;
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->SetAILevel(pCreature, nAILevel);
        }
    }
    return 0;
}
// PC 0x005B3270
int CSWVirtualMachineCommands::ExecuteCommandEffectBlind(int nCommand, int nArgs)
{
    CGameEffect* pGameEffect;
    CGameObject* pGameObject;

    pGameEffect = new CGameEffect(1);
    pGameEffect->SetCreator(m_oidObject);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_BLINDNESS;
    pGameEffect->SetInteger(0, 16);
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B3370
int CSWVirtualMachineCommands::ExecuteCommandEffectCrush(int nCommand, int nArgs)
{
    CGameEffect* pGameEffect;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
    pGameEffect->SetInteger(0, 15);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B49D0
int CSWVirtualMachineCommands::ExecuteCommandEffectFactionModifier(int nCommand, int nArgs)
{
    int nFaction;
    CGameEffect* pGameEffect;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nFaction) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_FACTIONMODIFIER;
    pGameEffect->SetInteger(0, nFaction);
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B4AF0
int CSWVirtualMachineCommands::ExecuteCommandEffectForceBody(int nCommand, int nArgs)
{
    int nBonus;
    CGameEffect* pGameEffect;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nBonus) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEBODY;
    pGameEffect->SetInteger(0, nBonus);
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B4C10
int CSWVirtualMachineCommands::ExecuteCommandEffectForceSight(int nCommand, int nArgs)
{
    CGameEffect* pGameEffect;
    CGameObject* pGameObject;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCESIGHT;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B4D00
int CSWVirtualMachineCommands::ExecuteCommandEffectFPRegenModifier(int nCommand, int nArgs)
{
    int nPercent;
    CGameEffect* pGameEffect;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nPercent) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_FPREGENMODIFIER;
    pGameEffect->SetInteger(0, nPercent);
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B4E20
int CSWVirtualMachineCommands::ExecuteCommandEffectVPRegenModifier(int nCommand, int nArgs)
{
    int nPercent;
    CGameEffect* pGameEffect;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nPercent) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_VPREGENMODIFIER;
    pGameEffect->SetInteger(0, nPercent);
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B4F40
int CSWVirtualMachineCommands::ExecuteCommandEffectFury(int nCommand, int nArgs)
{
    CGameEffect* pGameEffect;
    CGameObject* pGameObject;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_FURY;
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B8980
int CSWVirtualMachineCommands::ExecuteCommandForceHeartbeat(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(oidCreature, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL)
    {
        unsigned long oidSelf = pGameObject->GetId();
        CExoString* pScript = &pGameObject->AsSWSCreature()->m_sScripts[CSWSCreature::SCRIPT_HEARTBEAT];
        g_pVirtualMachine->RunScript(pScript, oidSelf, 1);
        pGameObject->AsSWSCreature()->ResetHeartbeatTimer();
    }
    return 0;
}
// PC 0x005BB9D0
int CSWVirtualMachineCommands::ExecuteCommandGetChemical(int nCommand, int nArgs)
{
    unsigned long oidObject;
    int nChemicals;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    nChemicals = 0;
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidObject);
    if (pCreature != NULL)
    {
        nChemicals = pCreature->GetChemical();
    }
    else
    {
        CSWSPlaceable* pPlaceable = g_pAppManager->GetServerExoApp()->GetPlaceableByGameObjectID(oidObject);
        if (pPlaceable != NULL && pPlaceable->m_pcItemRepository != NULL)
        {
            unsigned long oidItem = pPlaceable->m_pcItemRepository->FindItemWithBaseItemType(CSWBaseItem::ITEM_TYPE_CREDITS, 0);
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
            if (pItem != NULL)
            {
                nChemicals = pItem->m_nStackSize;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nChemicals) != 0 ? 0 : -2000;
}
// PC 0x005BBA80
int CSWVirtualMachineCommands::ExecuteCommandGetChemicalPieceValue(int nCommand, int nArgs)
{
    unsigned long oidItem;
    CGameObject* pGameObject;
    int nValue = 0;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(oidItem, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSItem() != NULL)
    {
        nValue = (unsigned int)pGameObject->AsSWSItem()->GetCost() / 20;
    }
    return g_pVirtualMachine->StackPushInteger(nValue) != 0 ? 0 : -2000;
}
// PC 0x005BF260
int CSWVirtualMachineCommands::ExecuteCommandGetOwnerDemolitionsSkill(int nCommand, int nArgs)
{
    unsigned long oidObject;
    int nSkill = 0;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        CSWSObject* pObject = pGameObject->AsSWSObject();
        if (pObject != NULL)
        {
            if (pObject->AsSWSDoor() != NULL)
            {
                nSkill = pObject->AsSWSDoor()->m_nOwnerDemolitionsSkill;
            }
            else if (pObject->AsSWSPlaceable() != NULL)
            {
                nSkill = pObject->AsSWSPlaceable()->m_nOwnerDemolitionsSkill;
            }
            else if (pObject->AsSWSTrigger() != NULL)
            {
                nSkill = pObject->AsSWSTrigger()->m_nOwnerDemolitionsSkill;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nSkill) != 0 ? 0 : -2000;
}
// PC 0x005BF320
int CSWVirtualMachineCommands::ExecuteCommandGetLastForfeitViolation(int nCommand, int nArgs)
{
    CSWPartyTable* pPartyTable = g_pAppManager->GetServerExoApp()->GetPartyTable();
    if (pPartyTable == NULL)
    {
        return g_pVirtualMachine->StackPushInteger(0) != 0 ? 0 : -2000;
    }
    int nViolation = pPartyTable->m_nForfeitViolation;
    return g_pVirtualMachine->StackPushInteger(nViolation) != 0 ? 0 : -2000;
}
// PC 0x005BF360
int CSWVirtualMachineCommands::ExecuteCommandAdjustCreatureAttributes(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nAttribute;
    int nAmount;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAttribute) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAmount) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject == NULL || pGameObject->m_nObjectType != CGameObject::OBJECT_TYPE_CREATURE)
    {
        return 0;
    }
    CSWSCreature* pCreature = pGameObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 0;
    }
    CSWSCreatureStats* pStats = pCreature->m_pStats;
    if (pStats == NULL)
    {
        return 0;
    }
    switch (nAttribute)
    {
        case 0:
            pStats->SetSTRBase(pStats->m_nStrengthBase + nAmount);
            break;
        case 1:
            pStats->SetDEXBase(pStats->m_nDexterityBase + nAmount);
            break;
        case 2:
            pStats->SetCONBase(pStats->m_nConstitutionBase + nAmount, 1);
            break;
        case 3:
            pStats->SetINTBase(pStats->m_nIntelligenceBase + nAmount);
            break;
        case 4:
            pStats->SetWISBase(pStats->m_nWisdomBase + nAmount);
            break;
        case 5:
            pStats->SetCHABase(pStats->m_nCharismaBase + nAmount);
            break;
    }
    return 0;
}
// PC 0x005BF4D0
int CSWVirtualMachineCommands::ExecuteCommandAdjustCreatureSkills(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nSkill;
    int nAmount;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nSkill) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAmount) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject != NULL && pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL && pCreature->m_pStats != NULL)
        {
            int nRank = pCreature->m_pStats->GetSkillRank(nSkill, NULL, 1);
            pCreature->m_pStats->SetSkillRank(nSkill, (unsigned char)(nAmount + nRank));
        }
    }
    return 0;
}
// PC 0x005BF590
int CSWVirtualMachineCommands::ExecuteCommandModifyBaseSavingThrow(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nAmount;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAmount) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject != NULL && pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            if (pStats != NULL)
            {
                switch (nCommand)
                {
                    case COMMAND_MODIFY_REFLEX_SAVING_THROW_BASE:
                        pStats->m_nReflexSaveBonus += nAmount;
                        break;
                    case COMMAND_MODIFY_FORTITUDE_SAVING_THROW_BASE:
                        pStats->m_nFortSaveBonus += nAmount;
                        break;
                    case COMMAND_MODIFY_WILL_SAVING_THROW_BASE:
                        pStats->m_nWillSaveBonus += nAmount;
                        break;
                }
            }
        }
    }
    return 0;
}
// PC 0x005BF660
int CSWVirtualMachineCommands::ExecuteCommandGetObjectPersonalSpace(int nCommand, int nArgs)
{
    unsigned long oidObject;
    float fPersonalSpace;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    fPersonalSpace = 0.0f;
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        fPersonalSpace = pGameObject->AsSWSCreature()->m_pPathfindInformation->m_fStepScaleY;
    }
    return g_pVirtualMachine->StackPushFloat(fPersonalSpace) != 0 ? 0 : -2000;
}
// PC 0x005BF6F0
int CSWVirtualMachineCommands::ExecuteCommandInfluence(int nCommand, int nArgs)
{
    int nNPC;
    int nValue = 0;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    CSWPartyTable* pPartyTable = g_pAppManager->m_pServerExoApp->GetPartyTable();
    if (pPartyTable == NULL)
    {
        if (nCommand == COMMAND_GET_INFLUENCE)
        {
            if (g_pVirtualMachine->StackPushInteger(nValue) == 0)
            {
                return -2000;
            }
        }
        return 0;
    }
    CSWSCreature* pCreature = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
        pPartyTable->GetNPCObject(nNPC, 0, 1));
    switch (nCommand)
    {
    case COMMAND_GET_INFLUENCE:
        nValue = pPartyTable->GetInfluence(nNPC);
        if (g_pVirtualMachine->StackPushInteger(nValue) == 0)
        {
            return -2000;
        }
        break;
    case COMMAND_SET_INFLUENCE:
    {
        int nOldInfluence = pPartyTable->GetInfluence(nNPC);
        if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
        {
            return -2001;
        }
        if (nValue < 0)
        {
            nValue = 0;
        }
        else if (nValue > 100)
        {
            nValue = 100;
        }
        pPartyTable->SetInfluence(nNPC, nValue);
        if (nOldInfluence == nValue)
        {
            return 0;
        }
        if (nOldInfluence < nValue)
        {
            g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_INFLUENCE_GAIN, nNPC);
        }
        else
        {
            g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_INFLUENCE_LOST, nNPC);
        }
        if (pCreature != NULL)
        {
            pCreature->HandleAlignmentInfluence();
        }
        return 0;
    }
    case COMMAND_MODIFY_INFLUENCE:
    {
        int nInfluence = pPartyTable->GetInfluence(nNPC);
        if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
        {
            return -2001;
        }
        if (nInfluence == -1)
        {
            pPartyTable->SetInfluence(nNPC, 50);
            nInfluence = 50;
        }
        int nNewInfluence = nInfluence + nValue;
        if (nNewInfluence < 0)
        {
            nNewInfluence = 0;
        }
        else if (nNewInfluence > 100)
        {
            nNewInfluence = 100;
        }
        pPartyTable->SetInfluence(nNPC, nNewInfluence);
        if (nInfluence == nNewInfluence)
        {
            return 0;
        }
        if (nValue > 0)
        {
            g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_INFLUENCE_GAIN, nNPC);
        }
        else
        {
            g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_INFLUENCE_LOST, nNPC);
        }
        if (pCreature != NULL)
        {
            pCreature->HandleAlignmentInfluence();
        }
        break;
    }
    }
    return 0;
}
// PC 0x005C0340
int CSWVirtualMachineCommands::ExecuteCommandGetSpellForcePointCost(int nCommand, int nArgs)
{
    int nCost = 0;
    CGameObject* pGameObject;

    if (m_nCommandContext == COMMAND_CONTEXT_SERVER &&
        g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS)
    {
        CSWSObject* pObject = pGameObject->AsSWSObject();
        if (pObject != NULL && (pObject->AsSWSCreature() != NULL || pObject->AsSWSPlaceable() != NULL))
        {
            nCost = pObject->m_n208;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nCost) != 0 ? 0 : -2000;
}
// PC 0x005C03C0
int CSWVirtualMachineCommands::ExecuteCommandGrantAbility(int nCommand, int nArgs)
{
    int nAbility;
    unsigned long oidCreature;

    if (g_pVirtualMachine->StackPopInteger(&nAbility) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        switch (nCommand)
        {
        case COMMAND_GRANT_FEAT:
            pCreature->m_pStats->AddFeat(nAbility);
            break;
        case COMMAND_GRANT_SPELL:
            pCreature->m_pStats->AddKnownSpell(pCreature->m_pStats->m_nClassCount - 1, nAbility);
            break;
        }
    }
    return 0;
}
// PC 0x005C0460
int CSWVirtualMachineCommands::ExecuteCommandGetSpellFormMask(int nCommand, int nArgs)
{
    int nSpell;
    int nMask = 0;

    if (g_pVirtualMachine->StackPopInteger(&nSpell) == 0)
    {
        return -2001;
    }
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
    if (pSpell != NULL)
    {
        nMask = pSpell->m_nFormMask;
    }
    return g_pVirtualMachine->StackPushInteger(nMask) != 0 ? 0 : -2000;
}
// PC 0x005C04D0
int CSWVirtualMachineCommands::ExecuteCommandGetSpellBaseForcePointCost(int nCommand, int nArgs)
{
    int nSpell;
    int nCost = 0;

    if (g_pVirtualMachine->StackPopInteger(&nSpell) == 0)
    {
        return -2001;
    }
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
    if (pSpell != NULL)
    {
        nCost = pSpell->m_nForcePoints;
    }
    return g_pVirtualMachine->StackPushInteger(nCost) != 0 ? 0 : -2000;
}
// PC 0x005C24F0
int CSWVirtualMachineCommands::ExecuteCommandIsStealthed(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int bStealthed = 0;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        bStealthed = pCreature->GetActivity(1);
    }
    return g_pVirtualMachine->StackPushInteger(bStealthed) != 0 ? 0 : -2000;
}
// PC 0x005C2560
int CSWVirtualMachineCommands::ExecuteCommandIsMeditating(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int bMeditating = 0;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        if (pCreature->m_c110C == 1)
        {
            bMeditating = 1;
        }
    }
    return g_pVirtualMachine->StackPushInteger(bMeditating) != 0 ? 0 : -2000;
}
// PC 0x005C25D0
int CSWVirtualMachineCommands::ExecuteCommandIsInTotalDefense(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int bTotalDefense = 0;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        if (pCreature->IsInTotalDefense())
        {
            bTotalDefense = 1;
        }
    }
    return g_pVirtualMachine->StackPushInteger(bTotalDefense) != 0 ? 0 : -2000;
}
// PC 0x005C2640
int CSWVirtualMachineCommands::ExecuteCommandIsFormActive(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nForm;
    int bActive = 0;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nForm) == 0)
        {
            return -2001;
        }
        CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
        if (pCreature != NULL && pCreature->m_n1110 == nForm &&
            (nForm < CSWSpell::FORM_SABER_I_SHII_CHO || nForm > CSWSpell::FORM_SABER_VII_JUYO || pCreature->IsWieldingLightsaber() != 0))
        {
            bActive = 1;
        }
        return g_pVirtualMachine->StackPushInteger(bActive) != 0 ? 0 : -2000;
    }
}
// PC 0x005C3690
int CSWVirtualMachineCommands::ExecuteCommandModifyGlobal(int nCommand, int nArgs)
{
    CExoString sIdentifier;
    int nAmount;
    char nValue;

    if (g_pVirtualMachine->StackPopString(&sIdentifier) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAmount) == 0)
    {
        return -2001;
    }
    switch (nCommand)
    {
    case COMMAND_INCREMENT_GLOBAL_NUMBER:
        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->GetValueNumber(sIdentifier, nValue);
        if (nAmount + nValue > 127)
        {
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueNumber(sIdentifier, 127);
        }
        else
        {
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueNumber(sIdentifier, nValue + nAmount);
        }
        break;
    case COMMAND_DECREMENT_GLOBAL_NUMBER:
        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->GetValueNumber(sIdentifier, nValue);
        if (nValue - nAmount < -128)
        {
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueNumber(sIdentifier, -128);
        }
        else
        {
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueNumber(sIdentifier, nValue - nAmount);
        }
        break;
    }
    return 0;
}
// PC 0x005C5470
int CSWVirtualMachineCommands::ExecuteCommandRemoveHeartbeat(int nCommand, int nArgs)
{
    unsigned long oidPlaceable;

    if (g_pVirtualMachine->StackPopObject(&oidPlaceable) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidPlaceable);
    if (pGameObject != NULL)
    {
        CSWSPlaceable* pPlaceable = pGameObject->AsSWSPlaceable();
        if (pPlaceable == NULL)
        {
            return 0;
        }
        pPlaceable->SetScript(CSWSPlaceable::SCRIPT_ONHEARTBEAT, "");
    }
    return 0;
}
// PC 0x005C6090
int CSWVirtualMachineCommands::ExecuteCommandSetCurrentForm(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nForm;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nForm) == 0)
    {
        return -2001;
    }
    g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(oidCreature, &pGameObject);
    if (pGameObject != NULL)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL && pCreature->GetStats()->HasSpell(0, nForm, 0) != 0)
        {
            pCreature->m_n1110 = nForm;
        }
    }
    return 0;
}
// PC 0x005C8490
int CSWVirtualMachineCommands::ExecuteCommandSetForfeitConditions(int nCommand, int nArgs)
{
    int nConditions = 0;

    if (g_pVirtualMachine->StackPopInteger(&nConditions) == 0)
    {
        return -2001;
    }
    CSWPartyTable* pPartyTable = g_pAppManager->GetServerExoApp()->GetPartyTable();
    if (pPartyTable != NULL)
    {
        pPartyTable->m_nForfeitConditions = nConditions;
    }
    return 0;
}
// PC 0x005CBF80
int CSWVirtualMachineCommands::ExecuteCommandGetScriptParameter(int nCommand, int nArgs)
{
    int nIndex;
    int nValue;

    if (g_pVirtualMachine->StackPopInteger(&nIndex) == 0)
    {
        return -2001;
    }
    if (nIndex < 1 || nIndex > 5)
    {
        nValue = 0;
    }
    else
    {
        nValue = g_pVirtualMachine->m_nScriptParameters[nIndex - 1];
    }
    return g_pVirtualMachine->StackPushInteger(nValue) != 0 ? 0 : -2000;
}
// PC 0x005CBFE0
int CSWVirtualMachineCommands::ExecuteCommandGetScriptStringParameter(int nCommand, int nArgs)
{
    CExoString sParameter(g_pVirtualMachine->m_sScriptStringParameter[0]);
    if (g_pVirtualMachine->StackPushString(sParameter) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005CC070
int CSWVirtualMachineCommands::ExecuteCommandSetFadeUntilScript(int nCommand, int nArgs)
{
    g_pAppManager->GetClientExoApp()->GetInGameGui()->m_n1024 = 1;
    return 0;
}
// PC 0x005CC090
int CSWVirtualMachineCommands::ExecuteCommandShowChemicalUpgradeScreen(int nCommand, int nArgs)
{
    unsigned long oidCharacter = OBJECT_INVALID;

    if (g_pVirtualMachine->StackPopObject(&oidCharacter) == 0)
    {
        return -2001;
    }
    g_pAppManager->GetClientExoApp()->GetInGameGui()->ShowChemicalUpgradeScreen(oidCharacter);
    return 0;
}
// PC 0x005CC0E0
int CSWVirtualMachineCommands::ExecuteCommandShowSwoopUpgradeScreen(int nCommand, int nArgs)
{
    g_pAppManager->GetClientExoApp()->GetInGameGui()->ShowSwoopUpgradeScreen(OBJECT_INVALID);
    return 0;
}
// PC 0x005CC100
int CSWVirtualMachineCommands::ExecuteCommandSetFakeCombatState(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int bEnable;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&bEnable) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject != NULL && pGameObject->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = (CSWSCreature*)pGameObject;
        if (bEnable != 0)
        {
            if (pCreature->m_bCombatState == 0 || pCreature->GetItemUseState() != 1)
            {
                pCreature->SetCombatState(1, 2, 1);
            }
        }
        else if (pCreature->GetItemUseState() == 2)
        {
            pCreature->SetCombatState(0, 2);
        }
    }
    return 0;
}
// PC 0x005CC1B0
int CSWVirtualMachineCommands::ExecuteCommandSetOrientOnClick(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int bOrient;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&bOrient) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject != NULL && pGameObject->AsSWSCreature() != NULL)
    {
        ((CSWSCreature*)pGameObject)->m_bDialogFaceSpeaker = bOrient;
    }
    return 0;
}
// PC 0x005CC220
int CSWVirtualMachineCommands::ExecuteCommandGetRacialSubType(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    CGameObject* pGameObject;
    int nSubType = 0;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(oidCreature, &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        if (pGameObject->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
        {
            nSubType = pGameObject->AsSWSCreature()->m_pStats->m_nSubraceIndex;
        }
        else
        {
            nSubType = -1;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nSubType) != 0 ? 0 : -2000;
}
// PC 0x005CC2B0
int CSWVirtualMachineCommands::ExecuteCommandSetBonusForcePoints(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nBonus;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2000;
    }
    if (g_pVirtualMachine->StackPopInteger(&nBonus) == 0)
    {
        return -2000;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject != NULL)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            if (pCreature->m_bPlayerCreated != 0 && pCreature->m_n11F4 != nBonus)
            {
                if (pCreature->m_n11F4 < nBonus)
                {
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_MAX_FP_GAIN, nBonus);
                }
                else
                {
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_MAX_FP_LOST, nBonus);
                }
            }
            pCreature->m_n11F4 = nBonus;
        }
    }
    return 0;
}
// PC 0x005CC360
int CSWVirtualMachineCommands::ExecuteCommandAddBonusForcePoints(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nBonus;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2000;
    }
    if (g_pVirtualMachine->StackPopInteger(&nBonus) == 0)
    {
        return -2000;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject != NULL)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            pCreature->m_n11F4 += nBonus;
            if (pCreature->m_bPlayerCreated != 0)
            {
                if (nBonus < 0)
                {
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_MAX_FP_LOST, nBonus);
                }
                else
                {
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_MAX_FP_GAIN, nBonus);
                }
            }
        }
    }
    return 0;
}
// PC 0x005CC420
int CSWVirtualMachineCommands::ExecuteCommandGetBonusForcePoints(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nBonus;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2000;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    CSWSCreature* pCreature;
    if (pGameObject != NULL && (pCreature = pGameObject->AsSWSCreature()) != NULL)
    {
        nBonus = pCreature->m_n11F4;
    }
    return g_pVirtualMachine->StackPushInteger(nBonus) != 0 ? 0 : -2000;
}
// PC 0x005CC610
int CSWVirtualMachineCommands::ExecuteCommandSetHealTarget(int nCommand, int nArgs)
{
    unsigned long oidHealer;
    unsigned long oidTarget;

    if (g_pVirtualMachine->StackPopObject(&oidHealer) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidHealer);
    if (pCreature != NULL)
    {
        pCreature->m_n5AC = oidTarget;
    }
    return 0;
}
// PC 0x005CC670
int CSWVirtualMachineCommands::ExecuteCommandGetHealTarget(int nCommand, int nArgs)
{
    unsigned long oidHealer;

    if (g_pVirtualMachine->StackPopObject(&oidHealer) == 0)
    {
        return -2001;
    }
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidHealer);
    unsigned long oidTarget = OBJECT_INVALID;
    if (pCreature != NULL)
    {
        oidTarget = pCreature->m_n5AC;
    }
    return g_pVirtualMachine->StackPushObject(oidTarget) != 0 ? 0 : -2000;
}
// PC 0x005CC6D0
int CSWVirtualMachineCommands::ExecuteCommandGetRandomDestination(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nRadius = 0;
    Vector vStart(0.0f, 0.0f, 0.0f);
    Vector vDirection(0.0f, 0.0f, 0.0f);
    CSWSArea* pArea;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nRadius) == 0)
        {
            return -2001;
        }
        CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
        if (pCreature == NULL || (pArea = pCreature->GetArea()) == NULL)
        {
            return g_pVirtualMachine->StackPushVector(vStart) != 0 ? 0 : -2000;
        }
        vStart = pCreature->m_vPosition;
        Vector vDestination(vStart.m_fX, vStart.m_fY, 0.0f);
        vDestination.m_fX += (float)(rand() % (2 * nRadius) - nRadius);
        vDestination.m_fY += (float)(rand() % (2 * nRadius) - nRadius);
        vDestination.m_fZ = pArea->ComputeHeight(vDestination, 1, NULL);
        vDirection = vDestination - vStart;
        float fDistance = magnitude(vDirection);
        float fMaxDistance = fDistance;
        vDirection = normalize(vDirection);
        pArea->m_pPathfindInformation = pCreature->m_pPathfindInformation;
        int nTries = 20;
        if (pArea->TestDirectLine(vStart, vDestination, pCreature->m_pPathfindInformation->GetStepScale(),
                                  pCreature->m_pPathfindInformation->GetHalfStep(), 0, NULL) != 1)
        {
            while (1)
            {
                fDistance *= 0.75;
                if (fDistance < 2.5f)
                {
                    int nChoice = rand() % 3;
                    if (nChoice == 0)
                    {
                        vDirection = -vDirection;
                    }
                    else if (nChoice == 1)
                    {
                        vDirection = Vector(vDirection.m_fZ * 0.0f - vDirection.m_fY,
                                            vDirection.m_fX - vDirection.m_fZ * 0.0f,
                                            vDirection.m_fY * 0.0f - vDirection.m_fX * 0.0f);
                    }
                    else if (nChoice == 2)
                    {
                        vDirection = -Vector(vDirection.m_fZ * 0.0f - vDirection.m_fY,
                                             vDirection.m_fX - vDirection.m_fZ * 0.0f,
                                             vDirection.m_fY * 0.0f - vDirection.m_fX * 0.0f);
                    }
                    fDistance = fMaxDistance;
                }
                vDestination = vStart + vDirection * fDistance;
                if (fDistance < 1.0f || nTries-- < 0)
                {
                    vDestination = vStart;
                    break;
                }
                if (pArea->TestDirectLine(vStart, vDestination, pCreature->m_pPathfindInformation->GetStepScale(),
                                          pCreature->m_pPathfindInformation->GetHalfStep(), 0, NULL) == 1)
                {
                    break;
                }
            }
        }
        return g_pVirtualMachine->StackPushVector(vDestination) != 0 ? 0 : -2000;
    }
}
// PC 0x005CCB10
int CSWVirtualMachineCommands::ExecuteCommandSetKeepStealthInDialog(int nCommand, int nArgs)
{
    int bKeepStealth = 0;

    if (g_pVirtualMachine->StackPopInteger(&bKeepStealth) == 0)
    {
        return -2001;
    }
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SetKeepStealthInDialog(bKeepStealth);
    return 0;
}
// PC 0x005CCB60
int CSWVirtualMachineCommands::ExecuteCommandHasLineOfSight(int nCommand, int nArgs)
{
    Vector vSource(0.0f, 0.0f, 0.0f);
    Vector vTarget(0.0f, 0.0f, 0.0f);
    unsigned long oidSource;
    unsigned long oidTarget;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopVector(&vSource) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopVector(&vTarget) == 0)
        {
            return -2001;
        }
        if (m_nCommandContext == 0)
        {
            return g_pVirtualMachine->StackPushInteger(0) != 0 ? 0 : -2000;
        }
        if (g_pVirtualMachine->StackPopObject(&oidSource) == 0)
        {
            return -2001;
        }
        if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
        {
            return -2001;
        }
        pGameObject = NULL;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(m_oidObject, &pGameObject) !=
            CGameObjectArray::SUCCESS)
        {
            return g_pVirtualMachine->StackPushInteger(0) != 0 ? 0 : -2000;
        }
        CSWSArea* pArea = NULL;
        if (pGameObject->AsSWSArea() != NULL)
        {
            pArea = pGameObject->AsSWSArea();
        }
        else if (pGameObject->AsSWSCreature() != NULL)
        {
            pArea = pGameObject->AsSWSCreature()->GetArea();
        }
        else if (pGameObject->AsSWSDoor() != NULL)
        {
            pArea = pGameObject->AsSWSDoor()->GetArea();
        }
        else if (pGameObject->AsSWSPlaceable() != NULL)
        {
            pArea = pGameObject->AsSWSPlaceable()->GetArea();
        }
        if (pArea == NULL)
        {
            if (g_pVirtualMachine->StackPushInteger(0) == 0)
            {
                return -2000;
            }
        }
        unsigned long oidHit = OBJECT_INVALID;
        Vector vHit(0.0f, 0.0f, 0.0f);
        return g_pVirtualMachine->StackPushInteger(
                   pArea->ClearLineOfSight(vSource, vTarget, &vHit, &oidHit, oidSource, oidTarget, 0, 0)) != 0
                   ? 0
                   : -2000;
    }
}
// PC 0x005CCD20
int CSWVirtualMachineCommands::ExecuteCommandShowDemoScreen(int nCommand, int nArgs)
{
    CExoString sImage;
    int nMinShowTime;
    int nStrRef;
    int nLeft;
    int nTop;

    if (g_pVirtualMachine->StackPopString(&sImage) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nMinShowTime) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nStrRef) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nLeft) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nTop) == 0)
    {
        return -2001;
    }
    g_pAppManager->GetClientExoApp()->GetInGameGui()->ShowDemoScreen(sImage, nMinShowTime, nStrRef, nLeft, nTop);
    return 0;
}
// PC 0x005CCE40
int CSWVirtualMachineCommands::ExecuteCommandIsRunning(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int bRunning = 0;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        CSWCCreature* pClientCreature = pCreature->GetClientCreature();
        if (pClientCreature != NULL)
        {
            bRunning = pClientCreature->m_bDriveRunning;
        }
    }
    return g_pVirtualMachine->StackPushInteger(bRunning) != 0 ? 0 : -2000;
}
// PC 0x005CCEB0
int CSWVirtualMachineCommands::ExecuteCommandAddAvailablePUPByTemplate(int nCommand, int nArgs)
{
    CExoString sTemplate;
    int nPUP;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nPUP) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopString(&sTemplate) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPushInteger(pServerExoApp->GetPartyTable()->AddPUP(nPUP, sTemplate)) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005CCFA0
int CSWVirtualMachineCommands::ExecuteCommandAddAvailablePUPByObject(int nCommand, int nArgs)
{
    int nPUP;
    unsigned long oidPuppet;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nPUP) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopObject(&oidPuppet) == 0)
        {
            return -2001;
        }
        return g_pVirtualMachine->StackPushInteger(pServerExoApp->GetPartyTable()->AddPUP(
                   nPUP, pServerExoApp->GetCreatureByGameObjectID(oidPuppet))) != 0
                   ? 0
                   : -2000;
    }
}
// PC 0x005CD030
int CSWVirtualMachineCommands::ExecuteCommandAssignPUP(int nCommand, int nArgs)
{
    int nPUP;
    int nNPC;

    if (g_pVirtualMachine->StackPopInteger(&nPUP) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
        {
            return -2001;
        }
        return g_pVirtualMachine->StackPushInteger(
                   g_pAppManager->GetServerExoApp()->GetPartyTable()->AssignPUP(nPUP, nNPC)) != 0
                   ? 0
                   : -2000;
    }
}
// PC 0x005CD0B0
int CSWVirtualMachineCommands::ExecuteCommandSpawnAvailablePUP(int nCommand, int nArgs)
{
    int nPUP;
    CScriptLocation* pLocation;

    if (g_pVirtualMachine->StackPopInteger(&nPUP) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION, (void**)&pLocation) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPushObject(g_pAppManager->GetServerExoApp()->GetPartyTable()->SpawnPUP(
            nPUP, 1, pLocation->m_vPosition, pLocation->m_vOrientation, 1)) == 0)
    {
        return -2000;
    }
    delete pLocation;
    return 0;
}
// PC 0x005CD150
int CSWVirtualMachineCommands::ExecuteCommandAddPartyPuppet(int nCommand, int nArgs)
{
    int nPUP;
    unsigned long oidPuppet;

    if (g_pVirtualMachine->StackPopInteger(&nPUP) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopObject(&oidPuppet) == 0)
        {
            return -2001;
        }
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        int bResult = pServerExoApp->GetPartyTable()->AddPuppet(nPUP, oidPuppet);
        if (bResult != 0)
        {
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            CSWCCreature* pClientCreature =
                pClientExoApp->GetCreatureByGameObjectID(pClientExoApp->ServerToClientObjectId(oidPuppet));
            CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(oidPuppet);
            if (pClientCreature != NULL)
            {
                pClientCreature->m_n46C = 1;
            }
            if (pCreature != NULL)
            {
                pCreature->SetIsPuppet(1);
            }
        }
        return g_pVirtualMachine->StackPushInteger(bResult) != 0 ? 0 : -2000;
    }
}
// PC 0x005CD220
int CSWVirtualMachineCommands::ExecuteCommandGetPUPOwner(int nCommand, int nArgs)
{
    unsigned long oidPuppet;

    if (g_pVirtualMachine->StackPopObject(&oidPuppet) == 0)
    {
        return -2001;
    }
    int nPUP = g_pAppManager->GetServerExoApp()->GetPartyTable()->GetPUPID(oidPuppet);
    CSWSCreature* pOwner = g_pAppManager->GetServerExoApp()->GetPartyTable()->GetPUPOwner(nPUP);
    unsigned long oidOwner = OBJECT_INVALID;
    if (pOwner != NULL)
    {
        oidOwner = pOwner->GetId();
    }
    return g_pVirtualMachine->StackPushObject(oidOwner) != 0 ? 0 : -2000;
}
// PC 0x005CD2A0
int CSWVirtualMachineCommands::ExecuteCommandGetIsPuppet(int nCommand, int nArgs)
{
    unsigned long oidCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    int bPuppet = 0;
    if (pCreature != NULL)
    {
        bPuppet = pCreature->m_n11FC;
    }
    return g_pVirtualMachine->StackPushInteger(bPuppet) != 0 ? 0 : -2000;
}
// PC 0x005CD300
int CSWVirtualMachineCommands::ExecuteCommandActionFollowOwner(int nCommand, int nArgs)
{
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(m_oidObject);
    float fRange = 0.0f;

    if (g_pVirtualMachine->StackPopFloat(&fRange) == 0)
    {
        return -2001;
    }
    if (pCreature != NULL && pCreature->m_n11FC != 0 && pCreature->m_bCommandable == 1)
    {
        pCreature->AddAction(CSWSObject::ACTION_FOLLOW_OWNER, CSWSObject::ACTION_GROUP_NEW,
                             CSWSObject::ACTION_PARAM_FLOAT, &fRange, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                             NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
    }
    return 0;
}
// PC 0x005CD3B0
int CSWVirtualMachineCommands::ExecuteCommandGetIsPartyLeader(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int bLeader = 0;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CSWCCreature* pCharacter = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
    if (pCharacter != NULL)
    {
        if (oidCreature == g_pAppManager->GetServerExoApp()->ClientToServerObjectId(pCharacter->GetId()))
        {
            bLeader = 1;
        }
    }
    return g_pVirtualMachine->StackPushInteger(bLeader) != 0 ? 0 : -2000;
}
// PC 0x005CD490
int CSWVirtualMachineCommands::ExecuteCommandRemoveNPCFromPartyToBase(int nCommand, int nArgs)
{
    int nNPC;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWPartyTable* pPartyTable = pServerExoApp->GetPartyTable();
    unsigned long oidNPC = pPartyTable->GetNPCObject(nNPC, 0, 1);
    if (pPartyTable->RemoveMember(nNPC) != 0)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pClientCreature =
            pClientExoApp->GetCreatureByGameObjectID(pClientExoApp->ServerToClientObjectId(oidNPC));
        CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(oidNPC);
        if (pClientCreature != NULL)
        {
            pClientCreature->SetInParty(0);
        }
        if (pCreature != NULL)
        {
            pCreature->SetInParty(0, 1);
        }
    }
    return g_pVirtualMachine->StackPushInteger(pPartyTable->KillNPCObject(nNPC, 1)) != 0 ? 0 : -2000;
}
// PC 0x005CD560
int CSWVirtualMachineCommands::ExecuteCommandGetIsXBox(int nCommand, int nArgs)
{
    return g_pVirtualMachine->StackPushInteger(0) != 0 ? 0 : -2000;
}
// PC 0x005CD580
int CSWVirtualMachineCommands::ExecuteCommandActionSwitchWeapons(int nCommand, int nArgs)
{
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != NULL)
    {
        pCreature->AddAction(CSWSObject::ACTION_SWITCH_WEAPONS, CSWSObject::ACTION_GROUP_NEW, 0, NULL, 0, NULL,
                             0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                             NULL, 0, NULL);
    }
    return 0;
}
// PC 0x005CD5E0
int CSWVirtualMachineCommands::ExecuteCommandUnlockAllMusic(int nCommand, int nArgs)
{
    CExoIni cIni;
    cIni.WriteIniEntry(CExoString(2047), ".\\swkotor2.ini", "Game Options", "UnlockedPlanetSongs");
    return 0;
}
// PC 0x005CD6D0
int CSWVirtualMachineCommands::ExecuteCommandDisableMap(int nCommand, int nArgs)
{
    int bDisable = 0;

    if (g_pVirtualMachine->StackPopInteger(&bDisable) == 0)
    {
        return -2001;
    }
    g_pAppManager->GetServerExoApp()->GetPartyTable()->SetDisableMap(bDisable);
    return 0;
}
// PC 0x005CD720
int CSWVirtualMachineCommands::ExecuteCommandDetonateMine(int nCommand, int nArgs)
{
    unsigned long oidMine = OBJECT_INVALID;

    if (g_pVirtualMachine->StackPopObject(&oidMine) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidMine);
    if (pGameObject->AsSWSTrigger() != NULL)
    {
        pGameObject->AsSWSTrigger()->SetFadeOutTime(1);
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(0, 0, m_oidObject, oidMine, CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
    }
    return 0;
}
// PC 0x005CD7B0
int CSWVirtualMachineCommands::ExecuteCommandDisableHealthRegen(int nCommand, int nArgs)
{
    int bDisable = 0;

    if (g_pVirtualMachine->StackPopInteger(&bDisable) == 0)
    {
        return -2001;
    }
    g_pAppManager->GetServerExoApp()->GetPartyTable()->SetDisableRegen(bDisable);
    return 0;
}
// PC 0x005CD800
int CSWVirtualMachineCommands::ExecuteCommandSetDisableTransit(int nCommand, int nArgs)
{
    int bDisable = 0;

    if (g_pVirtualMachine->StackPopInteger(&bDisable) == 0)
    {
        return -2001;
    }
    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
    if (pModule != NULL)
    {
        CSWSArea* pArea = pModule->GetArea();
        if (pArea != NULL)
        {
            pArea->m_bDisableTransit = bDisable;
        }
    }
    return 0;
}
// PC 0x005CD860
int CSWVirtualMachineCommands::ExecuteCommandSetInputClass(int nCommand, int nArgs)
{
    int nClass = 0;

    if (g_pVirtualMachine->StackPopInteger(&nClass) == 0)
    {
        return -2001;
    }
    if (nClass >= 0 && nClass < 6)
    {
        g_pAppManager->GetClientExoApp()->SetInputClass(nClass, 1);
    }
    return 0;
}
// PC 0x005CD8B0
int CSWVirtualMachineCommands::ExecuteCommandForceAlwaysUpdate(int nCommand, int nArgs)
{
    unsigned long oidObject;
    int bAlwaysUpdate = 0;

    oidObject = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&bAlwaysUpdate) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    if (pGameObject != NULL)
    {
        ((CSWSObject*)pGameObject)->m_n260 = bAlwaysUpdate;
    }
    return 0;
}
// PC 0x005CD920
int CSWVirtualMachineCommands::ExecuteCommandEnableRain(int nCommand, int nArgs)
{
    int bEnable = 0;

    if (g_pVirtualMachine->StackPopInteger(&bEnable) == 0)
    {
        return -2001;
    }
    float fStrength = 100.0f;
    if (bEnable == 0)
    {
        fStrength = 0.0f;
    }
    if (g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo != NULL)
    {
        g_pAppManager->GetClientExoApp()->GetModule()->GetArea()->SetWeather(1, fStrength);
    }
    return 0;
}
// PC 0x005CD9A0
int CSWVirtualMachineCommands::ExecuteCommandDisplayMessageBox(int nCommand, int nArgs)
{
    int nStrRef = -1;

    if (g_pVirtualMachine->StackPopInteger(&nStrRef) == 0)
    {
        return -2001;
    }
    CExoString sIcon;
    if (nArgs >= 2 && g_pVirtualMachine->StackPopString(&sIcon) == 0)
    {
        return -2001;
    }
    CSWGuiMessageBox* pMessageBox = g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
    if (pMessageBox != NULL)
    {
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetCallback(NULL, NULL, NULL);
        pMessageBox->SetIcon(sIcon);
        pMessageBox->SetMessage((unsigned long)nStrRef);
        pMessageBox->m_n9C4 = 1;
        g_pAppManager->GetClientExoApp()->SetPausedByCombat(1, 12, 0);
        g_pAppManager->GetClientExoApp()->GetSWGuiManager()->AddPanel(pMessageBox, 1, 0);
    }
    return 0;
}
// PC 0x005CDAE0
int CSWVirtualMachineCommands::ExecuteCommandDisplayDatapad(int nCommand, int nArgs)
{
    unsigned long oidItem;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidItem);
    if (pGameObject != NULL)
    {
        CSWSItem* pItem = (CSWSItem*)pGameObject;
        if (pItem != NULL)
        {
            CSWGuiMessageBox* pDatapad = g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pA4;
            pItem->m_lVarTable.SetLocalBoolean(94, 1);
            CExoString sText;
            CExoLocString* pDescription = &pItem->m_sDescIdentified;
            pDescription->GetString(0, &sText, 0);
            pDatapad->SetAllowCancel(0);
            pDatapad->m_lblMessage.m_cText.m_cParams.SetAlignment(9);
            pDatapad->m_lblMessage.m_cText.m_cParams.SetFont("fnt_d16x16");
            pDatapad->SetMessage(sText);
            pDatapad->SetCallback(NULL, NULL, NULL);
            g_pAppManager->GetClientExoApp()->GetSWGuiManager()->AddPanel(pDatapad, 1, 1);
            g_pAppManager->GetClientExoApp()->SetPausedByCombat(1, 12, 0);
        }
    }
    return 0;
}
// PC 0x005CDC40
int CSWVirtualMachineCommands::ExecuteCommandSaveNPCByObject(int nCommand, int nArgs)
{
    int nNPC = -1;
    unsigned long oidNPC = OBJECT_INVALID;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidNPC) == 0)
    {
        return -2001;
    }
    g_pAppManager->GetServerExoApp()->GetPartyTable()->SaveMemberByObject(nNPC, oidNPC);
    return 0;
}
// PC 0x005CDCB0
int CSWVirtualMachineCommands::ExecuteCommandSavePUPByObject(int nCommand, int nArgs)
{
    int nPUP = -1;
    unsigned long oidPUP = OBJECT_INVALID;

    if (g_pVirtualMachine->StackPopInteger(&nPUP) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidPUP) == 0)
    {
        return -2001;
    }
    g_pAppManager->GetServerExoApp()->GetPartyTable()->SavePuppetByObject(nPUP, oidPUP);
    return 0;
}
// PC 0x005CDD20
int CSWVirtualMachineCommands::ExecuteCommandGetIsPlayerMadeCharacter(int nCommand, int nArgs)
{
    unsigned long oidCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    int bPlayerMade = 0;
    if (pGameObject != NULL && pGameObject->AsSWSCreature() != NULL)
    {
        bPlayerMade = pGameObject->AsSWSCreature()->m_bPlayerCreated;
    }
    return g_pVirtualMachine->StackPushInteger(bPlayerMade) != 0 ? 0 : -2000;
}
// PC 0x005CDDA0
int CSWVirtualMachineCommands::ExecuteCommandRebuildPartyTable(int nCommand, int nArgs)
{
    CSWPartyTable* pPartyTable = g_pAppManager->GetServerExoApp()->GetPartyTable();
    if (pPartyTable != NULL)
    {
        CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
        if (pModule != NULL)
        {
            unsigned long oidNPC;
            oidNPC = pModule->FindObjectByTagOrdinal("atton", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(0) != 0)
            {
                pPartyTable->SetNPCObject(0, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("baodur", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(1) != 0)
            {
                pPartyTable->SetNPCObject(1, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("mand", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(2) != 0)
            {
                pPartyTable->SetNPCObject(2, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("g0t0", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(3) != 0)
            {
                pPartyTable->SetNPCObject(3, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("handmaiden", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(4) != 0)
            {
                pPartyTable->SetNPCObject(4, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("hk47", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(5) != 0)
            {
                pPartyTable->SetNPCObject(5, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("kreia", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(6) != 0)
            {
                pPartyTable->SetNPCObject(6, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("mira", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(7) != 0)
            {
                pPartyTable->SetNPCObject(7, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("t3m4", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(8) != 0)
            {
                pPartyTable->SetNPCObject(8, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("visasmarr", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(9) != 0)
            {
                pPartyTable->SetNPCObject(9, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("hanharr", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(10) != 0)
            {
                pPartyTable->SetNPCObject(10, oidNPC);
            }
            oidNPC = pModule->FindObjectByTagOrdinal("disciple", 0);
            if (oidNPC != OBJECT_INVALID && pPartyTable->GetIsNPCAvailable(11) != 0)
            {
                pPartyTable->SetNPCObject(11, oidNPC);
            }
            unsigned long oidRemote = pModule->FindObjectByTagOrdinal("remote", 0);
            if (oidRemote != OBJECT_INVALID && pPartyTable->GetIsPUPAvailable(0) != 0)
            {
                pPartyTable->SetPUPObject(0, oidRemote);
            }
        }
    }
    return 0;
}
// PC 0x005D0EA0
int CSWVirtualMachineCommands::ExecuteCommandRemoveEffectByID(int nCommand, int nArgs)
{
    unsigned long oidObject;
    int nEffectID;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nEffectID) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        CExoArrayList<CGameEffect*>* pEffects = &pGameObject->AsSWSObject()->m_lstEffects;
        if (pEffects != NULL && pEffects->num > 0)
        {
            for (unsigned long nEffect = 0; nEffect < pEffects->num; ++nEffect)
            {
                if ((*pEffects)[nEffect]->GetInteger(0) == nEffectID)
                {
                    CGameEffect* pRemoveEffect = (*pEffects)[nEffect];
                    (*pEffects)[nEffect]->m_bExposed = 0;
                    if (pRemoveEffect != NULL)
                    {
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, m_oidObject, oidObject, CServerAIMaster::EVENT_REMOVE_EFFECT, pRemoveEffect);
                    }
                }
            }
        }
    }
    return 0;
}
// PC 0x005D0F90
int CSWVirtualMachineCommands::ExecuteCommandRemoveEffectByExactMatch(int nCommand, int nArgs)
{
    unsigned long oidObject;
    CGameEffect* pEffect = NULL;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pEffect) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        CExoArrayList<CGameEffect*>* pEffects = &pGameObject->AsSWSObject()->m_lstEffects;
        if (pEffects != NULL && pEffects->num > 0)
        {
            for (unsigned long nEffect = 0; nEffect < pEffects->num; ++nEffect)
            {
                if ((*pEffects)[nEffect]->m_nType == pEffect->m_nType &&
                    (*pEffects)[nEffect]->GetInteger(0) == pEffect->GetInteger(0) &&
                    (*pEffects)[nEffect]->GetInteger(1) == pEffect->GetInteger(1))
                {
                    CGameEffect* pRemoveEffect = (*pEffects)[nEffect];
                    (*pEffects)[nEffect]->m_bExposed = 0;
                    if (pRemoveEffect != NULL)
                    {
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, m_oidObject, oidObject, CServerAIMaster::EVENT_REMOVE_EFFECT, pRemoveEffect);
                    }
                    break;
                }
            }
        }
    }
    delete pEffect;
    return 0;
}
// PC 0x005D16E0
int CSWVirtualMachineCommands::ExecuteCommandGetCombatActionPending(int nCommand, int nArgs)
{
    unsigned long oidCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    int bPending = 0;
    if (pGameObject != NULL && pGameObject->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            CExoLinkedList<CSWSObjectActionNode>* pActions = &pCreature->m_lstActions;
            if (pActions != NULL)
            {
                bPending = pActions->GetHead() != NULL;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(bPending) != 0 ? 0 : -2000;
}
// PC 0x005D2B20
int CSWVirtualMachineCommands::ExecuteCommandSpawnMine(int nCommand, int nArgs)
{
    int nMineType;
    CScriptLocation* pLocation = NULL;
    int nDetectDC;
    int nDisarmDC;
    unsigned long oidCreator;

    if (g_pVirtualMachine->StackPopInteger(&nMineType) == 0 ||
        g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION, (void**)&pLocation) == 0 ||
        g_pVirtualMachine->StackPopInteger(&nDetectDC) == 0 ||
        g_pVirtualMachine->StackPopInteger(&nDisarmDC) == 0 ||
        g_pVirtualMachine->StackPopObject(&oidCreator) == 0)
    {
        delete pLocation;
        return -2001;
    }
    C2DA* pTraps = g_pRules->Get2DArrays()->m_pTraps;
    CExoString sTrapScript;
    CSWSCreature* pCreature = NULL;
    int nFaction;
    int nDetectDCMod;
    int nDisarmDCMod;
    CGameObject* pCreator = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreator);
    if (pCreator != NULL)
    {
        if (pCreator->AsSWSCreature() != NULL)
        {
            pCreature = pCreator->AsSWSCreature();
            nFaction = pCreature->GetFactionId();
        }
        else if (pCreator->AsSWSPlaceable() != NULL)
        {
            CSWSPlaceable* pPlaceable = pCreator->AsSWSPlaceable();
            nFaction = pPlaceable->GetFactionId();
        }
        else
        {
            nFaction = 1;
        }
    }
    else
    {
        nFaction = 1;
    }
    pTraps->GetINTEntry(nMineType, "DetectDCMod", &nDetectDCMod);
    pTraps->GetINTEntry(nMineType, "DisarmDCMod", &nDisarmDCMod);
    pTraps->GetCExoStringEntry(nMineType, "TrapScript", &sTrapScript);
    CSWSTrigger* pTrigger = new CSWSTrigger(OBJECT_INVALID);
    pTrigger->SetCreator(oidCreator);
    pTrigger->m_nTrapType = (unsigned char)nMineType;
    pTrigger->m_asScripts[CSWSTrigger::SCRIPT_ONTRAPTRIGGERED] = sTrapScript;
    int nTrapDetectDC = nDetectDCMod + nDetectDC;
    pTrigger->m_nTrapDetectDC = nTrapDetectDC;
    int nTrapDisarmDC = nDisarmDC + nDisarmDCMod;
    pTrigger->m_nTrapDisarmDC = nTrapDisarmDC;
    pTrigger->m_bMineTrigger = 1;
    pTrigger->m_bTrapDetectable = 1;
    pTrigger->m_bTrapDisarmable = 1;
    pTrigger->SetPosition(pLocation->m_vPosition, 1, 1, 0);
    pTrigger->m_nFaction = nFaction;
    if (pCreature != NULL)
    {
        int nSkillRank = pCreature->GetStats()->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, 0);
        pTrigger->m_nOwnerDemolitionsSkill = nSkillRank;
    }
    int nTrapName = 0;
    pTraps->GetINTEntry(nMineType, "TrapName", &nTrapName);
    pTrigger->m_lsName.m_dwStringRef = nTrapName;
    CSWSArea* pArea = g_pAppManager->GetServerExoApp()->GetModule()->GetArea();
    pTrigger->CreateNewGeometry(2.0f, pLocation->m_vPosition, pArea);
    pTrigger->AddToArea(pArea, pLocation->m_vPosition.m_fX, pLocation->m_vPosition.m_fY, pLocation->m_vPosition.m_fZ, 1);
    pArea->m_lstTrapObjectIds.Add(pTrigger->GetId());
    delete pLocation;
    return 0;
}
// PC 0x006049D0
void CSWVirtualMachineCommands::ReportError(CExoString&, int)
{
}
// PC 0x005AD8C0
inline void CServerAIMaster::AppendToScriptsRun(const CExoString& sScript)
{
    CExoString sComma(",");

    m_sLastEvent = m_sLastEvent + sScript + sComma;
}
// PC 0x005AD970
CSWVirtualMachineCommands::~CSWVirtualMachineCommands()
{
    if (m_pCommands != NULL)
    {
        delete m_pCommands;
        m_pCommands = NULL;
    }
}
// PC 0x005AD9D0
int CSWVirtualMachineCommands::ExecuteCommand(int nCommand, int nArgs)
{
    int (CSWVirtualMachineCommands::*pCommand)(int, int);

#ifdef _XBOX
    if (nCommand < 768 && (pCommand = m_pCommands[nCommand]) != NULL)
#else
    if (nCommand < 877 && (pCommand = m_pCommands[nCommand]) != NULL)
#endif
    {
        return (this->*pCommand)(nCommand, nArgs);
    }
    else
    {
        return -2002;
    }
}
// PC 0x005ADA00
void CSWVirtualMachineCommands::RunScriptCallback(CExoString& sScript)
{
    CServerAIMaster* pServerAIMaster;

    pServerAIMaster = g_pAppManager->m_pServerExoApp->GetServerAIMaster();
    if (sScript != "")
    {
        pServerAIMaster->AppendToScriptsRun(sScript);
    }
}
// PC 0x005ADA40
void* CSWVirtualMachineCommands::CopyGameDefinedStructure(int nType,
                                                     void* pStructure)
{
    if (nType == 0)
    {
        CGameEffect* pEffect = new CGameEffect(0);
        pEffect->CopyEffect((CGameEffect*)pStructure, 0);
        return pEffect;
    }

    if (nType == 1)
    {
        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->CopyScriptEvent((CScriptEvent*)pStructure);
        return pEvent;
    }
    if (nType == 2)
    {
        CScriptLocation* pLocation = new CScriptLocation;
        pLocation->CopyScriptLocation((CScriptLocation*)pStructure);
        return pLocation;
    }
    if (nType == 3)
    {
        CScriptTalent* pTalent = new CScriptTalent;
        pTalent->CopyScriptTalent((CScriptTalent*)pStructure);
        return pTalent;
    }
    return NULL;
}
// PC 0x005ADBD0
int CSWVirtualMachineCommands::SaveGameDefinedStructure(int nType, void* pStructure,
                                                   CResGFF* pRes,
                                                   CResStruct* pParent)
{
    CResStruct sStruct;
    pRes->AddStructToStruct(&sStruct, pParent, "GameDefinedStrct", nType);
    if (nType == 0)
    {
        ((CGameEffect*)pStructure)->SaveGameEffect(pRes, &sStruct);
        return 1;
    }
    if (nType == 1)
    {
        return ((CScriptEvent*)pStructure)->SaveEvent(pRes, &sStruct);
    }
    if (nType == 2)
    {
        return ((CScriptLocation*)pStructure)->SaveLocation(pRes, &sStruct);
    }
    if (nType == 3)
    {
        return ((CScriptTalent*)pStructure)->SaveTalent(pRes, &sStruct);
    }
    return 0;
}
// PC 0x005ADC60
int CSWVirtualMachineCommands::LoadGameDefinedStructure(int nType,
                                                   void** ppStructure,
                                                   CResGFF* pRes,
                                                   CResStruct* pParent)
{
    CResStruct sStruct;
    pRes->GetStructFromStruct(&sStruct, pParent, "GameDefinedStrct");
    if (nType == 0)
    {
        CGameEffect* pEffect = new CGameEffect(1);
        *ppStructure = pEffect;
        pEffect->LoadGameEffect(pRes, &sStruct);
        return 1;
    }
    if (nType == 1)
    {
        CScriptEvent* pEvent = new CScriptEvent;
        *ppStructure = pEvent;
        return pEvent->LoadEvent(pRes, &sStruct);
    }
    if (nType == 2)
    {
        CScriptLocation* pLocation = new CScriptLocation;
        *ppStructure = pLocation;
        return pLocation->LoadLocation(pRes, &sStruct);
    }
    if (nType == 3)
    {
        CScriptTalent* pTalent = new CScriptTalent;
        *ppStructure = pTalent;
        return pTalent->LoadTalent(pRes, &sStruct);
    }
    return 0;
}
// PC 0x005ADE10
int CSWVirtualMachineCommands::GetEqualGameDefinedStructure(int nType, void* pFirst,
                                                       void* pSecond)
{
    if (nType == 0)
    {
        CGameEffect* pEffect = (CGameEffect*)pFirst;
        CGameEffect* pOtherEffect = (CGameEffect*)pSecond;
        if (pEffect->m_nID == pOtherEffect->m_nID)
        {
            return 1;
        }
    }
    else
    {
        if (nType == 1)
        {
            return ((CScriptEvent*)pFirst)->operator==(*(CScriptEvent*)pSecond);
        }
        if (nType == 2)
        {
            return ((CScriptLocation*)pFirst)
                ->Equals((CScriptLocation*)pSecond);
        }
        if (nType == 3)
        {
            return ((CScriptTalent*)pFirst)->Equals((CScriptTalent*)pSecond);
        }
    }
    return 0;
}
// PC 0x005ADE80
void* CSWVirtualMachineCommands::CreateGameDefinedStructure(int nType)
{
    if (nType == 0)
    {
        return new CGameEffect(0);
    }
    if (nType == 1)
    {
        return new CScriptEvent;
    }
    if (nType == 2)
    {
        return new CScriptLocation;
    }
    if (nType == 3)
    {
        return new CScriptTalent;
    }
    return NULL;
}
// PC 0x005CE240
void CSWVirtualMachineCommands::DestroyGameDefinedStructure(int nType,
                                                          void* pStructure)
{
    if (nType == STRUCTURE_EFFECT)
    {
        delete (CGameEffect*)pStructure;
    }
    else if (nType == STRUCTURE_EVENT)
    {
        delete (CScriptEvent*)pStructure;
    }
    else if (nType == STRUCTURE_LOCATION)
    {
        delete (CScriptLocation*)pStructure;
    }
    else if (nType == STRUCTURE_TALENT)
    {
        delete (CScriptTalent*)pStructure;
    }
}
// PC 0x005ADFA0
int CSWVirtualMachineCommands::ExecuteCommandActionBarkString(int nCommand,
                                                            int nArgs)
{
    CSWSCreature* pCreature;
    int nStringRef;

    if (g_pVirtualMachine->StackPopInteger(&nStringRef) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nGlobalDialogState == 0)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
        if (pCreature != NULL)
        {
            pCreature->AddAction(CSWSObject::ACTION_BARK_STRING,
                CSWSObject::ACTION_GROUP_NEW, CSWSObject::ACTION_PARAM_INTEGER,
                &nStringRef, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        }
    }
    return 0;
}
// PC 0x005AE040
int CSWVirtualMachineCommands::ExecuteCommandActionDoCommand(int nCommand,
                                                             int nArgs)
{
    void* nTemp;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    nTemp = NULL;
    if (g_pVirtualMachine->StackPopCommand(&nTemp) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
            ->GetGameObject(m_oidObject, (struct CGameObject**)&pObject) ==
        CGameObjectArray::SUCCESS)
    {
        pObject->AddDoCommandAction(nTemp);
    }
    return 0;
}
// PC 0x005AE0B0
int CSWVirtualMachineCommands::ExecuteCommandActionEquipMostDamagingMelee(
    int nCommand, int nArgs)
{
    int nSlot;
    unsigned long oidTarget;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nSlot) == 0)
    {
        return -2001;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature =
        pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != NULL)
    {
        pCreature->EquipMostDamagingMeleeWeapon(oidTarget, nSlot, 0);
    }
    return 0;
}
// PC 0x005AE120
int CSWVirtualMachineCommands::ExecuteCommandActionEquipMostDamagingRanged(
    int nCommand, int nArgs)
{
    unsigned long oidTarget;
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature =
        pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != NULL &&
        pCreature->EquipMostDamagingRangedWeapon(oidTarget) == 0)
    {
        pCreature->EquipMostDamagingMeleeWeapon(oidTarget, 0, 0);
    }
    return 0;
}
// PC 0x005AE180
int CSWVirtualMachineCommands::ExecuteCommandActionEquipMostEffectiveArmor(
    int nCommand, int nArgs)
{
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != NULL)
    {
        pCreature->EquipMostEffectiveArmor();
    }
    return 0;
}
// PC 0x005AE1B0
int CSWVirtualMachineCommands::ExecuteCommandActionExchangeItem(int nCommand,
                                                                int nArgs)
{
    unsigned long oidItem;
    unsigned long oidTarget;
    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pObject != NULL)
    {
        CServerExoApp* pItemApp = g_pAppManager->m_pServerExoApp;
        if (pItemApp->GetItemByGameObjectID(oidItem) != NULL)
        {
            if (nCommand == 135)
            {
                pObject->AsSWSObject()->AddGiveItemActions(oidItem, oidTarget,
                                                           -1, 1);
            }
            else
            {
                pObject->AsSWSObject()->AddTakeItemActions(oidItem, oidTarget,
                                                           1);
            }
        }
    }
    return 0;
}
// PC 0x005AE260
int CSWVirtualMachineCommands::ExecuteCommandActionFollowObject(
    int nCommand, int nArgs)
{

    unsigned long oidTarget;
    int bRun = 1;
    float fDistance = 0.0f;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fDistance) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (oidTarget != OBJECT_INVALID)
    {
        CSWSCreature* pCreature =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(m_oidObject);
        CGameObject* pTarget =
            g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
        CSWSObject* pTargetObject = pTarget->AsSWSObject();
        Vector vOffset(fDistance * pTargetObject->m_vOrientation.m_fX,
            fDistance * pTargetObject->m_vOrientation.m_fY,
            fDistance * pTargetObject->m_vOrientation.m_fZ);
        Vector vDestination = pTarget->AsSWSObject()->m_vPosition + vOffset;

        if (pCreature != NULL)
        {
            unsigned long oidCreature = pCreature->GetId();
            unsigned long oidClientCreature =
                g_pAppManager->GetClientExoApp()->ServerToClientObjectId(
                    oidCreature);
            CSWCCreature* pClientCreature =
                g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
                    oidClientCreature);
            if (pClientCreature != NULL && pClientCreature->m_bInParty != 0)
            {
                unsigned long oidPartyMember = pClientCreature->GetObjectId();
                int nIndex = g_pAppManager->GetClientExoApp()->GetSWParty()->
                    GetIndex(oidPartyMember);
                if (nIndex >= 0)
                {
                    CSWParty* pParty =
                        g_pAppManager->GetClientExoApp()->GetSWParty();
                    vDestination = pParty->m_aPartyMemberData[nIndex].m_vFormationOffset;
                    pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
                    float fRange = pParty->m_fFormationRange[pParty->m_nFormation];
                    vDestination += Vector(0.0f, -fRange, 0.0f);
                }
            }
        }

        Vector vPosition = pCreature->m_vPosition;
        int bStarted = 0;
        EXOXBOXASSERT(vDestination.m_fX <= 100000.0f && vDestination.m_fX >= -100000.0f && vDestination.m_fY <= 100000.0f && vDestination.m_fY >= -100000.0f);
        pCreature->AddAction(CSWSObject::ACTION_FORCE_FOLLOW_OBJECT, CSWSObject::ACTION_GROUP_NEW,
            CSWSObject::ACTION_PARAM_OBJECT, &oidTarget,
            CSWSObject::ACTION_PARAM_INTEGER, &bRun,
            CSWSObject::ACTION_PARAM_FLOAT, &vDestination.m_fX,
            CSWSObject::ACTION_PARAM_FLOAT, &vDestination.m_fY,
            CSWSObject::ACTION_PARAM_INTEGER, &bStarted,
            CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fX,
            CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fY,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        pCreature->AddAction(CSWSObject::ACTION_CHECK_FORCE_FOLLOW_OBJECT, CSWSObject::ACTION_GROUP_NEW,
            CSWSObject::ACTION_PARAM_OBJECT, &oidTarget,
            CSWSObject::ACTION_PARAM_INTEGER, &bRun,
            CSWSObject::ACTION_PARAM_FLOAT, &vDestination.m_fX,
            CSWSObject::ACTION_PARAM_FLOAT, &vDestination.m_fY,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL);
    }
    return 0;
}
// PC 0x005AE4E0
int CSWVirtualMachineCommands::ExecuteCommandActionFollowLeader(
    int nCommand, int nArgs)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature =
        pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != NULL && pCreature->m_bInParty != 0 &&
        pCreature->m_bCommandable == 1)
    {
        pCreature->AddAction(CSWSObject::ACTION_FOLLOW_LEADER,
            CSWSObject::ACTION_GROUP_NEW, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL);
    }
    return 0;
}
// PC 0x005AE560
int CSWVirtualMachineCommands::ExecuteCommandActionInteractObject(int nCommand,
                                                                  int nArgs)
{
    unsigned long oidPlaceable;
    CServerExoApp* pServerExoApp;
    CSWSPlaceable* pPlaceable;
    CSWSObject* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidPlaceable) == 0)
    {
        return -2001;
    }

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pPlaceable =
        (CSWSPlaceable*)pServerExoApp->GetPlaceableByGameObjectID(oidPlaceable);
    if (pPlaceable != NULL)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pCreature =
            (CSWSObject*)pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
        if (pCreature != NULL)
        {
            pCreature->AddUseObjectAction(pPlaceable->m_nId);
        }
    }
    return 0;
}
// PC 0x005AE5D0
int CSWVirtualMachineCommands::ExecuteCommandActionJumpToObject(
    int nCommand, int nArgs)
{
    CSWSObject* pCreature;
    int bWalkStraightLineToPoint = 1;
    unsigned long oidTarget;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs > 1)
    {
        if (g_pVirtualMachine->StackPopInteger(&bWalkStraightLineToPoint) == 0)
        {
            return ERROR_STACK_POP;
        }
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = (CSWSObject*)pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != NULL && pCreature->m_bCommandable != 0)
    {

        if (nCommand == COMMAND_ACTION_JUMP_TO_OBJECT)
        {
            pCreature->AddAction(48, CSWSObject::ACTION_GROUP_NEW,
                CSWSObject::ACTION_PARAM_OBJECT, &oidTarget,
                CSWSObject::ACTION_PARAM_INTEGER, &bWalkStraightLineToPoint,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            return 0;
        }
        pCreature->AddActionToFront(48, CSWSObject::ACTION_GROUP_NEW,
            CSWSObject::ACTION_PARAM_OBJECT, &oidTarget,
            CSWSObject::ACTION_PARAM_INTEGER, &bWalkStraightLineToPoint,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
    }
    return 0;
}
// PC 0x005AE6B0
int CSWVirtualMachineCommands::ExecuteCommandActionJumpToPoint(
    int nCommand, int nArgs)
{
    CScriptLocation* pLocation;
    int bWalkStraightLineToPoint;
    unsigned long oidArea;
    Vector vPosition(-1.0f, -1.0f, -1.0f);
    Vector vOrientation;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocation) == 0)
    {
        return ERROR_STACK_POP;
    }
    vPosition = pLocation->m_vPosition;
    vOrientation = pLocation->m_vOrientation;
    delete pLocation;
    pLocation = NULL;
    if (nArgs < 2)
    {
        bWalkStraightLineToPoint = 1;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&bWalkStraightLineToPoint) == 0)
        {
            return ERROR_STACK_POP;
        }
    }

    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject,
                                                           &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL &&
            pGameObject->AsSWSCreature()->m_bCommandable != 0)
        {
            CSWSArea* pArea = pGameObject->AsSWSCreature()->GetArea();
            CGameObject* pAreaGameObject =
                pArea != NULL ? (CGameObject*)pArea : NULL;
            oidArea = pAreaGameObject->GetId();
            if (pAreaGameObject->m_nObjectType ==
                CGameObject::OBJECT_TYPE_AREA)
            {
                float fTimeout = 20.0f;
                if (nCommand == COMMAND_ACTION_JUMP_TO_POINT)
                {
                    ((CSWSObject*)pGameObject)->AddAction(
                        CSWSObject::ACTION_JUMP_TO_POINT,
                        CSWSObject::ACTION_GROUP_NEW,
                        CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fX,
                        CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fY,
                        CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fZ,
                        CSWSObject::ACTION_PARAM_OBJECT, &oidArea,
                        CSWSObject::ACTION_PARAM_INTEGER,
                        &bWalkStraightLineToPoint,
                        CSWSObject::ACTION_PARAM_FLOAT, &fTimeout,
                        CSWSObject::ACTION_PARAM_FLOAT, &vOrientation.m_fX,
                        CSWSObject::ACTION_PARAM_FLOAT, &vOrientation.m_fY,
                        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                    return 0;
                }
                ((CSWSObject*)pGameObject)->AddActionToFront(
                    CSWSObject::ACTION_JUMP_TO_POINT,
                    CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fX,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fY,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fZ,
                    CSWSObject::ACTION_PARAM_OBJECT, &oidArea,
                    CSWSObject::ACTION_PARAM_INTEGER,
                    &bWalkStraightLineToPoint,
                    CSWSObject::ACTION_PARAM_FLOAT, &fTimeout,
                    CSWSObject::ACTION_PARAM_FLOAT, &vOrientation.m_fX,
                    CSWSObject::ACTION_PARAM_FLOAT, &vOrientation.m_fY,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            }
        }
    }
    return 0;
}
// PC 0x005AE8E0
int CSWVirtualMachineCommands::ExecuteCommandActionLockActions(int nCommand,
                                                               int nArgs)
{
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    unsigned long oidTarget;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        if (nCommand == 483)
        {
            pGameObject->AsSWSObject()->AddUnlockObjectAction(oidTarget,
                                                              OBJECT_INVALID, 0);
            return 0;
        }
        if (nCommand == 484)
        {
            pGameObject->AsSWSObject()->AddLockObjectAction(oidTarget);
        }
    }
    return 0;
}
// PC 0x005AE980
int CSWVirtualMachineCommands::ExecuteCommandActionMoveAwayFromLocation(
    int nCommand, int nArgs)
{

    CScriptLocation* pLocation;
    int bRun = 0;
    float fMoveAwayRange = CSWSCreature::MOVE_AWAY_RADIUS;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocation) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&bRun) == 0)
        {
            if (pLocation != NULL)
            {
                delete pLocation;
            }
            return ERROR_STACK_POP;
        }
    }
    if (nArgs >= 3)
    {
        if (g_pVirtualMachine->StackPopFloat(&fMoveAwayRange) == 0)
        {
            if (pLocation != NULL)
            {
                delete pLocation;
            }
            return ERROR_STACK_POP;
        }
    }

    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject,
                                                           &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL &&
            pGameObject->AsSWSCreature()->m_bCommandable == 1)
        {
            Vector vMoveAwayFrom = pLocation->m_vPosition;
            CSWSArea* pArea = pGameObject->AsSWSCreature()->GetArea();

            CResRef cArea;
            if (pArea != NULL)
            {
                Vector vDifference =
                    ((CSWSObject*)pGameObject)->m_vPosition - vMoveAwayFrom;
                if (vDifference * vDifference <=
                    fMoveAwayRange * fMoveAwayRange)
                {
                    Vector vDestination = pArea->ComputeAwayVector(
                        ((CSWSObject*)pGameObject)->m_vPosition, vMoveAwayFrom,
                        fMoveAwayRange,
                        pGameObject->AsSWSCreature()->m_pPathfindInformation);
                    int nTimeout = 10;
                    pGameObject->AsSWSCreature()->AddMoveToPointAction(
                        CSWSObject::ACTION_GROUP_NEW, vDestination,
                        pArea->GetId(), CGameObjectArray::INVALIDOBJID, bRun,
                        0.0f, 0.0f, 0, 0, 0, 0, 1, 0, Vector(), 0, 0, 0);
                    ((CSWSObject*)pGameObject)->AddAction(
                        CSWSObject::ACTION_CHECK_MOVE_AWAY_FROM_LOCATION,
                        CSWSObject::ACTION_GROUP_LAST,
                        CSWSObject::ACTION_PARAM_FLOAT,
                        &pLocation->m_vPosition.m_fX,
                        CSWSObject::ACTION_PARAM_FLOAT,
                        &pLocation->m_vPosition.m_fY,
                        CSWSObject::ACTION_PARAM_FLOAT,
                        &pLocation->m_vPosition.m_fZ,
                        CSWSObject::ACTION_PARAM_INTEGER, &bRun,
                        CSWSObject::ACTION_PARAM_FLOAT, &fMoveAwayRange,
                        CSWSObject::ACTION_PARAM_INTEGER, &nTimeout,
                        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                        0, NULL);
                }
                return 0;
            }
        }
    }
    delete pLocation;
    return 0;
}
// PC 0x005AEC20
int CSWVirtualMachineCommands::ExecuteCommandActionPauseConversation(
    int nCommand, int nArgs)
{
    CGameObject* pGameObject;
    int nDeadTemp;
    CServerExoApp* pServerExoApp;

    if (m_nCommandContext == 1)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                    ->GetGameObject(m_oidObject, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSObject() != NULL &&
            pGameObject->AsSWSObject()->m_bCommandable != 0)
        {
            nDeadTemp = 0;
            if (pGameObject->AsSWSCreature() != NULL)
            {
                CSWSCreature* pCreature = pGameObject->AsSWSCreature();
                nDeadTemp = pCreature->GetDeadTemp();
            }
            if (!(pGameObject->AsSWSObject()->GetDead() == 1 ||
                  pGameObject->AsSWSObject()->GetIsPCDying() == 1 ||
                  nDeadTemp == 1))
            {
                pGameObject->AsSWSObject()->m_nDialogPending = 1;
                if (pGameObject->AsSWSCreature() != NULL)
                {
                    CSWSCreature* pCreature = pGameObject->AsSWSCreature();
                    pCreature->m_bInDialog |= CSWSCreature::DIALOG_PAUSED;
                    return 0;
                }
            }
            else
            {
                pGameObject->AsSWSObject()->SetLockOrientationToObject(
                    CGameObjectArray::INVALIDOBJID);
            }
        }
    }
    return 0;
}
// PC 0x005AED20
int CSWVirtualMachineCommands::ExecuteCommandActionRandomWalk(
    int nCommand, int nArgs)
{
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CSWSObject* pObject;
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, (CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS)
        {
            Vector vPosition = pObject->m_vPosition;
            CSWSArea* pArea = pObject->GetArea();
            CResRef cResRef;
            if (pArea != NULL && pObject->m_bCommandable != 0)
            {
                unsigned long oidArea = pArea->m_nId;

                pObject->AddAction(45, CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fX,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fY,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fZ,
                    CSWSObject::ACTION_PARAM_OBJECT, &oidArea,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL);
            }
        }
    }
    return 0;
}
// PC 0x005AEE00
int CSWVirtualMachineCommands::ExecuteCommandActionResumeConversation(
    int nCommand, int nArgs)
{
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    if (m_nCommandContext == 1)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                    ->GetGameObject(m_oidObject,
                                    (struct CGameObject**)&pObject) ==
                CGameObjectArray::SUCCESS &&
            pObject->m_bCommandable != 0)
        {
            pObject->AddAction(0x20u, 0xFFFFu, 0, NULL, 0, NULL, 0, NULL, 0,
                               NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                               NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        }
    }
    return 0;
}
// PC 0x005AEE90
int CSWVirtualMachineCommands::ExecuteCommandActionStartConversation(
    int nCommand, int nArgs)
{

    unsigned long oidObjectToConverse;
    CExoString sDialogResRef("");
    int bPrivateConversation = 0;
    int nDialogType = CONVERSATION_TYPE_COMPUTER;
    int nConversationType = CONVERSATION_TYPE_COMPUTER;
    int bIgnoreStartRange = 0;
    CSWSObject* pObject;
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();

    if (g_pVirtualMachine->StackPopObject(&oidObjectToConverse) == 0 ||
        (nArgs >= 2 &&
         g_pVirtualMachine->StackPopString(&sDialogResRef) == 0) ||
        (nArgs >= 3 &&
         g_pVirtualMachine->StackPopInteger(&bPrivateConversation) == 0) ||
        (nArgs >= 4 &&
         g_pVirtualMachine->StackPopInteger(&nConversationType) == 0) ||
        (nArgs >= 5 &&
         g_pVirtualMachine->StackPopInteger(&bIgnoreStartRange) == 0))
    {
        return ERROR_STACK_PUSH;
    }
    bIgnoreStartRange = 1;
    CExoString sNameObjectToIgnore;
    if (nArgs > 5)
    {
        for (int nIgnore = 0; nIgnore < 6; ++nIgnore)
        {
            if (g_pVirtualMachine->StackPopString(&sNameObjectToIgnore) == 0)
            {
                return ERROR_STACK_PUSH;
            }
            g_pAppManager->GetClientExoApp()->GetInGameGui()
                ->AddToDialogNonDependantList(sNameObjectToIgnore);
        }
    }
    int bUseLeader = 0;
    if (nArgs >= 12 &&
        g_pVirtualMachine->StackPopInteger(&bUseLeader) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    int nBarkX = -1;
    int nBarkY = -1;
    int bDontClearAllActions;
    if ((nArgs >= 14 &&
         (g_pVirtualMachine->StackPopInteger(&nBarkX) == 0 ||
          g_pVirtualMachine->StackPopInteger(&nBarkY) == 0)) ||
        ((bDontClearAllActions = 0), nArgs >= 15) &&
         g_pVirtualMachine->StackPopInteger(&bDontClearAllActions) == 0)
    {
        return ERROR_STACK_POP;
    }
    CSWGuiBarkBubble* pBarkBubble =
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pBarkBubble;
    if (pBarkBubble != NULL)
    {
        int nY = nBarkY;
        int nX = nBarkX;
        pBarkBubble->m_nBarkX = nX;
        pBarkBubble->m_nBarkY = nY;
    }
    if (bUseLeader == 0)
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bUsePartyCharacterForDialog = 0;
    }
    else
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bUsePartyCharacterForDialog = 1;
    }

    CSWCCreature* pCharacter =
        g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
    if (pCharacter != NULL)
    {
        pCharacter->ClearAllActions();
        unsigned long oidCharacter = pCharacter->GetId();
        oidCharacter = g_pAppManager->GetServerExoApp()
            ->ClientToServerObjectId(oidCharacter);
        CGameObject* pGameObject =
            g_pAppManager->GetServerExoApp()->GetGameObject(oidCharacter);
        if (pGameObject != NULL && pGameObject->AsSWSCreature() != NULL)
        {
            pGameObject->AsSWSCreature()->ClearAllActions(1);
        }
    }

    int bPartyDead = 1;
    for (int nMember = 0; nMember < 3; ++nMember)
    {
        CSWCCreature* pMember =
            g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(nMember);
        if (pMember == NULL)
        {
            if (nMember == 0)
            {
                bPartyDead = 0;
                break;
            }
        }
        else
        {
            CSWSCreature* pServerMember = pMember->GetServerCreature();
            if (pServerMember != NULL && pServerMember->GetCurrentHitPoints(0) > 0)
            {
                bPartyDead = 0;
                break;
            }
        }
    }

    if (g_pAppManager->GetClientExoApp()->GetInGameGui()
                ->m_nGlobalDialogState == 0 &&
        bPartyDead == 0 && m_nCommandContext == 1)
    {
        if ((unsigned char)pObjectArray->GetGameObject(
                m_oidObject, (CGameObject**)&pObject) ==
                CGameObjectArray::SUCCESS &&
            pObject->m_bCommandable != 0)
        {
            for (int nMember = 0; nMember < 3; ++nMember)
            {
                CSWCCreature* pMember =
                    g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(nMember);
                if (pMember != NULL)
                {
                    CSWSCreature* pServerMember = pMember->GetServerCreature();
                    if (pServerMember != NULL &&
                        pServerMember->GetCurrentHitPoints(0) < 1)
                    {
                        CGameEffect* pEffect = new CGameEffect(1);
                        pEffect->m_nType = CSWSEffectListHandler::EFFECT_RESURRECTION;
                        pEffect->m_nSubType =
                            (pEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
                            SUBTYPE_MAGICAL;
                        pEffect->SetInteger(0, 2);
                        if (g_pAppManager->GetServerExoApp()->GetGameObject(m_oidObject) != NULL)
                        {
                            pEffect->SetCreator(m_oidObject);
                        }
                        pEffect->m_nSubType &= ~CGameEffect::DURATION_MASK;
                        pServerMember->AsSWSObject()->ApplyEffect(pEffect, 0, 1);
                    }
                }
            }
            int nAILevel = pObject->m_nAILevel;
            if ((unsigned char)nAILevel < 3)
            {
                pObject->m_c7C = (unsigned char)nAILevel;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                    ->SetAILevel(pObject, 3);
            }
            g_pAppManager->GetClientExoApp()->GetInGameGui()
                ->SetGlobalDialogState(1);
            unsigned long oidDialogOwner = OBJECT_INVALID;
            if (bDontClearAllActions == 0)
            {
                pObject->ClearAllActions(1);
            }
            pObject->AddAction(CSWSObject::ACTION_START_CONVERSATION,
                CSWSObject::ACTION_GROUP_NEW,
                CSWSObject::ACTION_PARAM_OBJECT, &oidObjectToConverse,
                CSWSObject::ACTION_PARAM_STRING, &sDialogResRef,
                CSWSObject::ACTION_PARAM_INTEGER, &bPrivateConversation,
                CSWSObject::ACTION_PARAM_INTEGER, &nDialogType,
                CSWSObject::ACTION_PARAM_INTEGER, &bIgnoreStartRange,
                CSWSObject::ACTION_PARAM_OBJECT, &oidDialogOwner,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL);
        }
    }
    return 0;
}
// PC 0x005AF5B0
int CSWVirtualMachineCommands::ExecuteCommandCreatureFlourishWeapon(int nCommand, int nArgs)
{
    unsigned long oidCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject != NULL && pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            CSWCCreature* pClientCreature = pCreature->GetClientCreature();
            if (pClientCreature != NULL)
            {
                pClientCreature->ComputeWeaponOverlays(0, 1);
                if (pClientCreature->m_n464 > 0)
                {
                    return 0;
                }
                if (pCreature->GetDead() == 0 && pCreature->GetDeadTemp() == 0)
                {
                    switch (pClientCreature->GetAnimationBase()->m_nWieldClass)
                    {
                    case CSWCAnimBase::WIELD_CLASS_1:
                    case CSWCAnimBase::WIELD_CLASS_UNARMED:
                        return 0;
                    }

                    unsigned long oidWeapon = pClientCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND);
                    if (oidWeapon != OBJECT_INVALID)
                    {
                        CSWCObject* pWeaponObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidWeapon);
                        if (pWeaponObject != NULL && pWeaponObject->AsSWCItem() != NULL)
                        {
                            pWeaponObject->AsSWCItem()->PowerItem(1, 0, 1, 0);
                        }
                    }

                    oidWeapon = pClientCreature->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND);
                    if (oidWeapon != OBJECT_INVALID)
                    {
                        CSWCObject* pWeaponObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidWeapon);
                        if (pWeaponObject != NULL && pWeaponObject->AsSWCItem() != NULL)
                        {
                            pWeaponObject->AsSWCItem()->PowerItem(1, 0, 1, 0);
                        }
                    }

                    pCreature->SetCombatState(1, CSWSCreature::COMBAT_STATE_EQUIPMENT, 0);
                }

                if (g_pAppManager->GetClientExoApp()->GetPlayerMovementInput())
                {
                    pClientCreature->GetAnimationBase()->SetOverlayAnimation(
                        pClientCreature->GetAnimationBase()->ServerToClientAnimation(CREATURE_ANIMATION_OVERLAY_DRAW), 1);
                    pClientCreature->m_n464 = 1000;
                    return 0;
                }
                pClientCreature->InsertFireForgetAnimation(10158, 1500, 1.0f, NULL, 0);
                pClientCreature->m_n464 = 1000;
            }
        }
    }
    return 0;
}
// PC 0x005AF790
int CSWVirtualMachineCommands::ExecuteCommandActionUseFeat(int nCommand,
                                                          int nArgs)
{
    int nFeat;
    unsigned long oidTarget;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nFeat) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject,
                                                        &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL &&
        pGameObject->AsSWSCreature()->GetArea() != NULL)
    {
        pGameObject->AsSWSCreature()->UseFeat(nFeat, 0, oidTarget,
            pGameObject->AsSWSCreature()->GetArea()->GetId());
    }
    return 0;
}
// PC 0x005AF850
int CSWVirtualMachineCommands::ExecuteCommandActionUseSkill(int nCommand,
                                                           int nArgs)
{

    int nSkill;
    unsigned long oidTarget;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nSkill) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }

    int nSubSkill = 0;
    if (nArgs >= 3 &&
        g_pVirtualMachine->StackPopInteger(&nSubSkill) == 0)
    {
        return ERROR_STACK_POP;
    }
    unsigned long oidItemUsed = OBJECT_INVALID;
    if (nArgs >= 4 &&
        g_pVirtualMachine->StackPopObject(&oidItemUsed) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject,
                                                    &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL &&
        pGameObject->AsSWSCreature()->GetArea() != NULL)
    {
        pGameObject->AsSWSCreature()->UseSkill(nSkill, nSubSkill, oidTarget,
            Vector(0.0f, 0.0f, 0.0f),
            pGameObject->AsSWSCreature()->GetArea()->GetId(), oidItemUsed, 0);
    }
    return 0;
}
// PC 0x005AF990
int CSWVirtualMachineCommands::ExecuteCommandActionUseTalent(int nCommand,
                                                            int nArgs)
{
    CScriptTalent* pTalent;
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_TALENT,
                                                  (void**)&pTalent) == 0)
    {
        return ERROR_STACK_POP;
    }

    int nTalentType = pTalent->m_nType;
    if (nTalentType > -1)
    {
        int nTalentId = pTalent->m_nID;
        unsigned char nMultiClass = pTalent->m_nMultiClass;
        unsigned long oidItem = pTalent->m_oidItem;
        int nItemProperty = pTalent->m_nItemPropertyIndex;
        unsigned char nCasterLevel = pTalent->m_nCasterLevel;
        unsigned char nMetaType = pTalent->m_nMetaType;

        CSWSCreature* pCreature =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                m_oidObject);
        if (pCreature != NULL)
        {
            if (nCommand == COMMAND_ACTION_USE_TALENT_ON_OBJECT)
            {
                unsigned long oidTarget = OBJECT_INVALID;
                if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
                {
                    return ERROR_STACK_POP;
                }
                pCreature->AddUseTalentOnObjectActions(
                    nTalentType, nTalentId, oidTarget, nMultiClass, oidItem,
                    nItemProperty, nCasterLevel, nMetaType);
            }
            else if (nCommand == COMMAND_ACTION_USE_TALENT_AT_LOCATION)
            {
                CScriptLocation* pLocation;
                if (g_pVirtualMachine->StackPopEngineStructure(
                        STRUCTURE_LOCATION, (void**)&pLocation) == 0)
                {
                    return ERROR_STACK_POP;
                }
                pCreature->AddUseTalentAtLocationActions(
                    nTalentType, nTalentId, pLocation->m_vPosition,
                    nMultiClass, oidItem, nItemProperty, nCasterLevel,
                    nMetaType);
                delete pLocation;
            }
        }
    }
    delete pTalent;
    return 0;
}
// PC 0x005AFAE0
int CSWVirtualMachineCommands::ExecuteCommandAddAvailableNPCByObject(
    int nCommand, int nArgs)
{
    CServerExoApp* pServerExoApp;
    int nNPC;
    unsigned long oidCreature;
    CSWSCreature* pCreature;
    int nResult2;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
        {
            return -2001;
        }
        pCreature = (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(
            oidCreature);
        nResult2 = pServerExoApp->GetPartyTable()->AddNPC(nNPC, pCreature);
        return g_pVirtualMachine->StackPushInteger(nResult2) != 0 ? 0 : -2000;
    }
}
// PC 0x005AFB70
int CSWVirtualMachineCommands::ExecuteCommandAddAvailableNPCByTemplate(
    int nCommand, int nArgs)
{
    CServerExoApp* pServerExoApp;
    int nNPC;
    CExoString sTemplate;
    int nResult2;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopString(&sTemplate) == 0)
    {
        return -2001;
    }
    nResult2 = pServerExoApp->GetPartyTable()->AddNPC(nNPC, sTemplate);
    if (g_pVirtualMachine->StackPushInteger(nResult2) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005AFC60
int CSWVirtualMachineCommands::ExecuteCommandAddMultiClass(int nCommand,
                                                           int nArgs)
{
    int nClass;
    unsigned long oidCreature;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    int nExperience;
    int nExperienceToNextLevel;

    if (g_pVirtualMachine->StackPopInteger(&nClass) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    nExperience = pCreature->m_pStats->m_nExperience;
    if (pCreature->m_bPlayerCreated != 0)
    {
        nExperienceToNextLevel = pCreature->m_pStats->GetExpNeededForLevelUp() - nExperience;
        pCreature->m_pStats->AddClass((unsigned char)nClass, 0);
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            g_pAppManager->m_pServerExoApp->GetPartyTable()
                ->DistributeExperience(nExperienceToNextLevel, 1);
        }
    }
    else
    {
        nExperienceToNextLevel = pCreature->m_pStats->GetExpNeededForLevelUp() - nExperience;
        pCreature->m_pStats->AddClass((unsigned char)nClass, 0);
        pCreature->m_pStats->AddExperience(nExperienceToNextLevel);
        pCreature->m_nUnknown22C += nExperienceToNextLevel;
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bAutoLevelUpNPCs &&
            pCreature->m_pStats->CanLevelUp() != 0)
        {
            pCreature->m_pStats->AutoLevelup(1);
        }
    }
    return 0;
}
// PC 0x005AFD80
int CSWVirtualMachineCommands::ExecuteCommandAddPartyMember(int nCommand,
                                                            int nArgs)
{
    int nNPC;
    unsigned long oidCreature;
    CServerExoApp* pServerExoApp;
    int nResult2;
    CClientExoApp* pClientExoApp;
    unsigned long oidResult2;
    CSWCCreature* pSWCCreature;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
        {
            return -2001;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        nResult2 = pServerExoApp->GetPartyTable()->AddMember(nNPC, oidCreature);
        if (nResult2 != 0)
        {
            pClientExoApp = (CClientExoApp*)g_pAppManager->m_pClientExoApp;
            oidResult2 = pClientExoApp->ServerToClientObjectId(oidCreature);
            pSWCCreature = pClientExoApp->GetCreatureByGameObjectID(oidResult2);
            pCreature = (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(
                oidCreature);
            if (pSWCCreature != NULL)
            {
                pSWCCreature->SetInParty(1);
            }
            if (pCreature != NULL)
            {
                pCreature->SetInParty(1, 1);
            }
        }
        return g_pVirtualMachine->StackPushInteger(nResult2) != 0 ? 0 : -2000;
    }
}
// PC 0x005AFE50
int CSWVirtualMachineCommands::ExecuteCommandAddToParty(int nCommand, int nArgs)
{
    unsigned long oidPC;
    unsigned long oidPartyLeader;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWSCreature* pCreature2;
    CSWSFaction* pFaction;

    if (g_pVirtualMachine->StackPopObject(&oidPC) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidPartyLeader) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature =
        (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(oidPartyLeader);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature2 = pServerExoApp->GetCreatureByGameObjectID((unsigned int)oidPC);
    if (pCreature != NULL && pCreature->m_nAreaWaitState != 0 &&
        pCreature2 != 0 && pCreature2->m_nAreaWaitState != 0)
    {
        pFaction = pCreature->GetFaction();
        if (pFaction != NULL)
        {
            pFaction->AddMember((unsigned int)oidPC, 0);
        }
    }
    return 0;
}
// PC 0x005AFEF0
int CSWVirtualMachineCommands::ExecuteCommandAdjustAlignment(int nCommand, int nArgs)
{

    unsigned long oidSubject;
    int nAlignment;
    int nShift;
    int bDontModifyNPCs;
    if (g_pVirtualMachine->StackPopObject(&oidSubject) == 0 ||
        g_pVirtualMachine->StackPopInteger(&nAlignment) == 0 ||
        g_pVirtualMachine->StackPopInteger(&nShift) == 0)
    {
        return ERROR_STACK_POP;
    }
    short nAdjustment = 0;
    bDontModifyNPCs = 0;
    if (nArgs >= 4 && g_pVirtualMachine->StackPopInteger(&bDontModifyNPCs) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(oidSubject);
    if (pCreature == NULL)
    {
        return 0;
    }

    CSWSCreatureStats* pStats = pCreature->m_pStats;
    short nOldAlignment = pStats->m_nGoodEvilAlignment;
    CExoString sTotal;
    char nTotal;
    switch (nAlignment)
    {
    case CSWSCreatureStats::ALIGNMENT_NEUTRAL:

        if (nOldAlignment > 50)
        {
            nAdjustment = max(-nShift, 50 - nOldAlignment);
        }
        else
        {
            nAdjustment = min(nShift, 50 - nOldAlignment);
        }
        if (pCreature->m_bPlayerCreated != 0 && bDontModifyNPCs == 0)
        {
            if (nAdjustment < 0)
            {
                sTotal = "G_PC_Dark_Total";
                g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                    ->GetValueNumber(sTotal, nTotal);
                if (nTotal < SWVARTABLE_LOCAL_NUMBER_MAX)
                {
                    if (nTotal - nAdjustment <= SWVARTABLE_LOCAL_NUMBER_MAX)
                    {
                        nTotal = nTotal - nAdjustment;
                        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                            ->SetValueNumber(sTotal, nTotal);
                    }
                    else
                    {
                        nTotal = SWVARTABLE_LOCAL_NUMBER_MAX;
                        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                            ->SetValueNumber(sTotal, nTotal);
                    }
                }
            }
            else if (nAdjustment > 0)
            {
                sTotal = "G_PC_Light_Total";
                g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                    ->GetValueNumber(sTotal, nTotal);
                if (nTotal < SWVARTABLE_LOCAL_NUMBER_MAX)
                {
                    if (nAdjustment + nTotal <= SWVARTABLE_LOCAL_NUMBER_MAX)
                    {
                        nTotal = nAdjustment + nTotal;
                        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                            ->SetValueNumber(sTotal, nTotal);
                    }
                    else
                    {
                        nTotal = SWVARTABLE_LOCAL_NUMBER_MAX;
                        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                            ->SetValueNumber(sTotal, nTotal);
                    }
                }
            }
        }
        break;
    case CSWSCreatureStats::ALIGNMENT_GOOD:
        nAdjustment = nShift;
        if (pCreature->m_bPlayerCreated != 0 && bDontModifyNPCs == 0)
        {
            sTotal = "G_PC_Light_Total";
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                ->GetValueNumber(sTotal, nTotal);
            if (nTotal < SWVARTABLE_LOCAL_NUMBER_MAX)
            {
                if (nAdjustment + nTotal <= SWVARTABLE_LOCAL_NUMBER_MAX)
                {
                    nTotal = nAdjustment + nTotal;
                    g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                        ->SetValueNumber(sTotal, nTotal);
                }
                else
                {
                    nTotal = SWVARTABLE_LOCAL_NUMBER_MAX;
                    g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                        ->SetValueNumber(sTotal, nTotal);
                }
            }
        }
        break;
    case CSWSCreatureStats::ALIGNMENT_EVIL:
        nAdjustment = -nShift;
        if (pCreature->m_bPlayerCreated != 0 && bDontModifyNPCs == 0)
        {
            sTotal = "G_PC_Dark_Total";
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                ->GetValueNumber(sTotal, nTotal);
            if (nTotal < SWVARTABLE_LOCAL_NUMBER_MAX)
            {
                if (nTotal - nAdjustment <= SWVARTABLE_LOCAL_NUMBER_MAX)
                {
                    nTotal = nTotal - nAdjustment;
                    g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                        ->SetValueNumber(sTotal, nTotal);
                }
                else
                {
                    nTotal = SWVARTABLE_LOCAL_NUMBER_MAX;
                    g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                        ->SetValueNumber(sTotal, nTotal);
                }
            }
        }
        break;
    }
    pStats->ModifyAlignment(nAdjustment, bDontModifyNPCs);

    if (bDontModifyNPCs == 0)
    {
        CSWCCMessageData* pMessageData = new CSWCCMessageData;
        if (nAlignment != CSWSCreatureStats::ALIGNMENT_NEUTRAL)
        {
            pMessageData->SetInteger(0, nAlignment);
            pMessageData->SetInteger(1, nShift);
        }
        else
        {
            if (nOldAlignment > 50)
            {
                pMessageData->SetInteger(0, CSWSCreatureStats::ALIGNMENT_EVIL);
            }
            else
            {
                pMessageData->SetInteger(0, CSWSCreatureStats::ALIGNMENT_GOOD);
            }
            pMessageData->SetInteger(1, abs(nShift));
        }
        pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_ALIGNMENT_SHIFT, pMessageData);
        if (nAdjustment != 0 && pCreature->m_bInParty != 0)
        {
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            pClientExoApp->GetInGameGui()->UpdateStatus(
                GUI_STATUS_DARK_SHIFT, nAdjustment);
        }
    }
    return 0;
}
// PC 0x005B0230
int CSWVirtualMachineCommands::ExecuteCommandAdjustReputation(int nCommand,
                                                              int nArgs)
{
    unsigned long oidResult;
    unsigned long oidResult2;
    int nAdjustment;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWSCreature* pCreature2;
    int nResult;

    oidResult = OBJECT_INVALID;
    oidResult2 = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidResult2) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAdjustment) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult2);
    if (pCreature == 0 || pCreature->m_bPlotObject == 0)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pCreature2 =
            (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(oidResult);
        if (pCreature2 != NULL)
        {
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            if (pServerExoApp->GetFactionOfObject(oidResult2, &nResult) == 1)
            {
                pCreature2->AdjustReputation(nResult, nAdjustment);
            }
        }
    }
    return 0;
}
// PC 0x005B0300
int CSWVirtualMachineCommands::ExecuteCommandAmbientSound(int nCommand,
                                                          int nArgs)
{
    int nValue = 0;
    unsigned long oidArea;
    CServerExoApp* pServerExoApp;
    CSWSArea* pArea;

    if (g_pVirtualMachine->StackPopObject(&oidArea) == 0)
    {
        return -2001;
    }
    if (nArgs > 1 && g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pArea = pServerExoApp->GetAreaByGameObjectID(oidArea);
    if (pArea == NULL)
    {
        return 0;
    }
    switch (nCommand)
    {
    case 433:
        pArea->m_pAmbientSound->PlayAmbientSound(1);
        return 0;
    case 434:
        pArea->m_pAmbientSound->PlayAmbientSound(0);
        return 0;
    case 435:
        pArea->m_pAmbientSound->SetAmbientDayTrack(nValue);
        return 0;
    case 436:
        pArea->m_pAmbientSound->SetAmbientNightTrack(nValue);
        return 0;
    case 567:
        pArea->m_pAmbientSound->SetAmbientDayVolume(nValue);
        return 0;
    case 568:
        pArea->m_pAmbientSound->SetAmbientNightVolume(nValue);
        return 0;
    }
    return 0;
}
// PC 0x005B04C0
int CSWVirtualMachineCommands::ExecuteCommandApplyEffectAtPoint(int nCommand,
                                                              int nArgs)
{

    int nDurationType;
    CGameEffect* pEffect;
    void* pLocationData;
    float fDuration;

    if (g_pVirtualMachine->StackPopInteger(&nDurationType) == 0 ||
        g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT,
                                                   (void**)&pEffect) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  &pLocationData) != 0)
    {
        if (g_pVirtualMachine->StackPopFloat(&fDuration) == 0)
        {
            if (pLocationData != NULL)
            {
                operator delete(pLocationData);
                pLocationData = NULL;
            }
            goto StackPopFailure;
        }
        CScriptLocation* pLocation = (CScriptLocation*)pLocationData;
        CGameEffect* pGameEffect = pEffect;
        switch (nDurationType)
        {
        case DURATION_TYPE_INSTANT:
            pEffect->m_nSubType &= ~CGameEffect::DURATION_MASK;
            break;
        case DURATION_TYPE_TEMPORARY:
            pEffect->m_nSubType =
                (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                DURATION_TYPE_TEMPORARY;
            pGameEffect->m_fDuration = fDuration;
            break;
        default:
            pEffect->m_nSubType =
                (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                DURATION_TYPE_PERMANENT;
            break;
        }
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        unsigned long oidPlayer = pServer->GetPlayerCreatureId();
        CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidPlayer);
        CSWSArea* pArea;
        if (pCreature != NULL)
        {
            pArea = pCreature->GetArea();
            if (pArea != NULL)
            {
                pArea->ApplyEffect(pGameEffect, pLocation->m_vPosition,
                                  pLocation->m_vOrientation);
            }
            else
            {
                delete pGameEffect;
            }
        }
        else
        {
            delete pGameEffect;
        }
        delete pLocation;
        return 0;
    }
StackPopFailure:
    if (pEffect != NULL)
    {
        operator delete(pEffect);
    }
    return ERROR_STACK_POP;
}
// PC 0x005B0600
int CSWVirtualMachineCommands::ExecuteCommandApplyEffectOnObject(int, int)
{
    CGameObjectArray* pObjects = g_pAppManager->GetServerExoApp()->GetObjectArray();
    int nDurationType;
    CGameEffect* pEffect;
    unsigned long oidTarget;
    float fDuration;
    CGameObject* pTarget;
    CGameObject* pCreator;

    if (g_pVirtualMachine->StackPopInteger(&nDurationType) == 0 ||
        g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT,
                                                   (void**)&pEffect) == 0 ||
        g_pVirtualMachine->StackPopObject(&oidTarget) == 0 ||
        g_pVirtualMachine->StackPopFloat(&fDuration) == 0)
        return ERROR_STACK_POP;

    CGameEffect* pGameEffect = pEffect;
    switch (nDurationType)
    {
    case DURATION_TYPE_INSTANT:
        pGameEffect->m_nSubType &= ~CGameEffect::DURATION_MASK;
        break;
    case DURATION_TYPE_TEMPORARY:
        pGameEffect->m_nSubType =
            (pGameEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            DURATION_TYPE_TEMPORARY;
        pGameEffect->m_fDuration = fDuration;
        break;
    default:
        pGameEffect->m_nSubType =
            (pGameEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            DURATION_TYPE_PERMANENT;
        break;
    }
    if (pObjects->GetGameObject(oidTarget, &pTarget) == CGameObjectArray::SUCCESS)
    {
        if (pTarget != NULL)
        {
            if (pObjects->GetGameObject(pGameEffect->GetCreator(), &pCreator) ==
                    CGameObjectArray::SUCCESS &&
                pCreator != NULL &&
                pCreator->m_nObjectType == SW_OBJECT_TYPE_CREATURE &&
                pCreator->AsSWSCreature()->m_bCasterLevelOverride != 0)
            {
                pGameEffect->m_nCasterLevel =
                    pCreator->AsSWSCreature()->m_nCasterLevelOverride;
            }
            if (pTarget->AsSWSObject() != NULL)
            {
                if (pGameEffect->m_nType == CSWSEffectListHandler::EFFECT_LINK)
                    pTarget->AsSWSObject()->m_bApplyingLinkedEffect = 1;
                pTarget->AsSWSObject()->ApplyEffect(pGameEffect, 0, 1);
                pTarget->AsSWSObject()->m_bApplyingLinkedEffect = 0;
            }
        }
        else
        {
            delete pGameEffect;
        }
    }
    else
    {
        delete pGameEffect;
    }
    return 0;
}
// PC 0x005B07B0
int CSWVirtualMachineCommands::ExecuteCommandAssignCommand(int nCommand,
                                                           int nArgs)
{
    unsigned long oidActionSubject;
    void* pAction;
    CGameObject* pObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidActionSubject) == 0)
    {
        return ERROR_STACK_POP;
    }
    pAction = NULL;
    if (g_pVirtualMachine->StackPopCommand(&pAction) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)pServerExoApp->GetObjectArray()->GetGameObject(
            oidActionSubject, &pObject) == CGameObjectArray::SUCCESS)
    {
        unsigned long oidCaller = CGameObjectArray::INVALIDOBJID;
        if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
        {
            oidCaller = m_oidObject;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoApp->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, oidCaller, oidActionSubject,
            CServerAIMaster::EVENT_TIMED_EVENT, pAction);
    }
    else if (pAction != NULL)
    {
        delete pAction;
    }
    return 0;
}
// PC 0x005B0880
int CSWVirtualMachineCommands::ExecuteCommandAttack(int nCommand, int nArgs)
{
    int nResult;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature =
        (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != NULL)
    {
        pCreature->AddAttackActions(oidResult, 0, 0, nResult, 0, 0, 0, 1, 10009,
                                    1500, 1, 0, 0, 4, 0);
    }
    return 0;
}
// PC 0x005B0920
int CSWVirtualMachineCommands::ExecuteCommandCutsceneAttack(int nCommand,
                                                            int nArgs)
{
    unsigned long oidResult;
    int nResult;
    int nResult2;
    int nResult3;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    nResult = 0;
    nResult2 = 0;
    nResult3 = 0;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nResult2) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nResult3) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature =
        (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != NULL)
    {
        pCreature->AddCutsceneAttackActions(oidResult, nResult, nResult2,
                                            nResult3);
    }
    return 0;
}
// PC 0x005B09E0
int CSWVirtualMachineCommands::ExecuteCommandCutsceneMove(int nCommand,
                                                          int nArgs)
{
    int nRun;
    unsigned long oidTarget;
    Vector vPosition;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopVector(&vPosition) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nRun) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != NULL)
    {
        pCreature->AddCutsceneMoveActions(oidTarget, vPosition, nRun);
    }
    return 0;
}
// PC 0x005B0A80
int CSWVirtualMachineCommands::ExecuteCommandBeginConversation(int nCommand,
                                                               int nArgs)
{

    unsigned long oidObjectToDialog = OBJECT_INVALID;
    int bResult = 0;
    CExoString sResRef;
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    Vector vObjectOrientation;
    Vector vSpeakerOrientation;
    CGameObject* pGameObject;
    CGameObject* pSpeakerGameObject;

    if (nArgs > 0 &&
        (g_pVirtualMachine->StackPopString(&sResRef) == 0 ||
         g_pVirtualMachine->StackPopObject(&oidObjectToDialog) == 0))
    {
        return ERROR_STACK_POP;
    }

    int nDialogState = g_pAppManager->GetClientExoApp()->GetInGameGui()
        ->m_nGlobalDialogState;
    int bActionConversation =
        g_pAppManager->GetServerExoApp()->GetActionConversation();
    g_pAppManager->GetServerExoApp()->SetActionConversation(0);
    if ((bActionConversation != 0 || nDialogState == 0) &&
        m_nCommandContext == COMMAND_CONTEXT_SERVER &&
        pObjectArray->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS)
    {
        if (oidObjectToDialog == OBJECT_INVALID)
            oidObjectToDialog = ((CSWSObject*)pGameObject)->m_oidLastSpeaker;
        if (pObjectArray->GetGameObject(oidObjectToDialog,
                                      &pSpeakerGameObject) ==
                CGameObjectArray::SUCCESS)
        {
            CSWSObject* pSpeaker = (CSWSObject*)pSpeakerGameObject;
            if (pSpeaker != NULL)
            {
                CSWSObject* pObject = (CSWSObject*)pGameObject;
                if (pObject != NULL)
                {
                    unsigned long oidPlayer =
                        g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                    CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
                        ->GetClientObjectByObjectId(oidPlayer);
                    if (pSpeakerGameObject->AsSWSCreature() != NULL && pPlayer != NULL)
                    {
                        if (pSpeakerGameObject->GetId() != pPlayer->m_oidPCObject)
                            pSpeakerGameObject->AsSWSCreature()->ClearAllActions(1);
                        vSpeakerOrientation = pSpeaker->AsSWSCreature()->m_vOrientation;
                        CLastUpdateObject* pLastUpdate =
                            pPlayer->GetLastUpdateObject(pSpeaker->GetId());
                        if (pLastUpdate != NULL)
                        {
                            pSpeakerGameObject->AsSWSCreature()->SetAnimation(CSWSObject::ANIMATION_IDLE);
                            pSpeakerGameObject->AsSWSCreature()->ClearDirtyFlag(CSWSObject::DIRTY_ANIMATION_INDEX);
                            pLastUpdate->m_nAnimation = CSWSObject::ANIMATION_IDLE;
                        }
                    }
                    if (pGameObject->AsSWSCreature() != NULL && pPlayer != NULL)
                    {
                        if (pGameObject->GetId() != pPlayer->m_oidPCObject)
                            pGameObject->AsSWSCreature()->ClearAllActions(1);
                        vObjectOrientation = pObject->AsSWSCreature()->m_vOrientation;
                        CLastUpdateObject* pLastUpdate =
                            pPlayer->GetLastUpdateObject(pObject->GetId());
                        if (pLastUpdate != NULL)
                        {
                            pGameObject->AsSWSCreature()->SetAnimation(CSWSObject::ANIMATION_IDLE);
                            pGameObject->AsSWSCreature()->ClearDirtyFlag(CSWSObject::DIRTY_ANIMATION_INDEX);
                            pLastUpdate->m_nAnimation = CSWSObject::ANIMATION_IDLE;
                        }
                    }
                    if (sResRef.IsEmpty())
                        sResRef = pObject->m_sLastListenText;
                    int nConversationType = pObject->m_nConversationType;
                    CSWSPlayer* pSpeakerPlayer = g_pAppManager->GetServerExoApp()
                        ->GetClientObjectByObjectId(pSpeaker->GetId());
                    if (oidObjectToDialog == pPlayer->m_oidPCObject)
                    {
                        if (pObject->StartDialog(oidObjectToDialog, sResRef, nConversationType))
                            bResult = 1;
                        else
                        {
                            if (pSpeaker->AsSWSCreature() != NULL)
                            {
                                pSpeaker->AsSWSCreature()->SetLockOrientationToObject(
                                    OBJECT_INVALID, 0);
                                pSpeaker->AsSWSCreature()->SetOrientation(vSpeakerOrientation);
                            }
                            if (pObject->AsSWSCreature() != NULL)
                            {
                                pObject->AsSWSCreature()->SetLockOrientationToObject(
                                    OBJECT_INVALID, 0);
                                pObject->AsSWSCreature()->SetOrientation(vObjectOrientation);
                            }
                        }
                    }
                    else if (m_oidObject == pPlayer->m_oidPCObject)
                    {
                        if (pSpeaker->StartDialog(m_oidObject, sResRef, nConversationType))
                            bResult = 1;
                        else
                        {
                            if (pSpeaker->AsSWSCreature() != NULL)
                            {
                                pSpeaker->AsSWSCreature()->SetLockOrientationToObject(
                                    OBJECT_INVALID, 0);
                                pSpeaker->AsSWSCreature()->SetOrientation(vSpeakerOrientation);
                            }
                            if (pObject->AsSWSCreature() != NULL)
                            {
                                pObject->AsSWSCreature()->SetLockOrientationToObject(
                                    OBJECT_INVALID, 0);
                                pObject->AsSWSCreature()->SetOrientation(vObjectOrientation);
                            }
                        }
                    }
                    else if (pObject->AsSWSCreature() != NULL &&
                             pObject->AsSWSCreature()->GetInParty())
                    {
                        if (pSpeaker->StartDialog(m_oidObject, sResRef, nConversationType))
                            bResult = 1;
                        else
                        {
                            if (pSpeaker->AsSWSCreature() != NULL)
                            {
                                pSpeaker->AsSWSCreature()->SetLockOrientationToObject(
                                    OBJECT_INVALID, 0);
                                pSpeaker->AsSWSCreature()->SetOrientation(vSpeakerOrientation);
                            }
                            if (pObject->AsSWSCreature() != NULL)
                            {
                                pObject->AsSWSCreature()->SetLockOrientationToObject(
                                    OBJECT_INVALID, 0);
                                pObject->AsSWSCreature()->SetOrientation(vObjectOrientation);
                            }
                        }
                    }
                    else
                    {
                        if (pObject->StartDialog(oidObjectToDialog, sResRef, nConversationType))
                            bResult = 1;
                        else
                        {
                            if (pSpeaker->AsSWSCreature() != NULL)
                            {
                                pSpeaker->AsSWSCreature()->SetLockOrientationToObject(
                                    OBJECT_INVALID, 0);
                                pSpeaker->AsSWSCreature()->SetOrientation(vSpeakerOrientation);
                            }
                            if (pObject->AsSWSCreature() != NULL)
                            {
                                pObject->AsSWSCreature()->SetLockOrientationToObject(
                                    OBJECT_INVALID, 0);
                                pObject->AsSWSCreature()->SetOrientation(vObjectOrientation);
                            }
                        }
                    }
                }
            }
        }
    }
    if (bResult == 0)
        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(0);
    if (g_pVirtualMachine->StackPushInteger(bResult) == 0)
        return ERROR_STACK_PUSH;
    return 0;
}
// PC 0x005B0EE0
int CSWVirtualMachineCommands::ExecuteCommandCastSpell(int nCommand, int nArgs)
{

    int nSpell;
    int bLocation = 0;
    int bInstantSpell = 0;
    int nDomainLevel;
    int bCheat;
    int nMetaMagic;
    int nProjectilePath;
    if (g_pVirtualMachine->StackPopInteger(&nSpell) == 0)
        return ERROR_STACK_POP;

    unsigned long oidTarget = OBJECT_INVALID;
    CScriptLocation lTarget;
    if (nCommand == COMMAND_ACTION_CAST_SPELL_AT_OBJECT ||
        nCommand == COMMAND_ACTION_CAST_FAKE_SPELL_AT_OBJECT)
    {
        if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
            return ERROR_STACK_POP;
    }
    else
    {
        CScriptLocation* pLocation;
        if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                     (void**)&pLocation) == 0)
            return ERROR_STACK_POP;
        lTarget = *pLocation;
        bLocation = 1;
        delete pLocation;
    }
    int bFake = 0;
    if (nCommand == COMMAND_ACTION_CAST_FAKE_SPELL_AT_OBJECT ||
        nCommand == COMMAND_ACTION_CAST_FAKE_SPELL_AT_LOCATION)
        bFake = 1;
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nMetaMagic) == 0 ||
            g_pVirtualMachine->StackPopInteger(&bCheat) == 0)
            return ERROR_STACK_POP;
        if (nCommand == COMMAND_ACTION_CAST_SPELL_AT_OBJECT)
        {
            if (g_pVirtualMachine->StackPopInteger(&nDomainLevel) == 0)
                return ERROR_STACK_POP;
        }
        else
            nDomainLevel = 0;
    }
    if (g_pVirtualMachine->StackPopInteger(&nProjectilePath) == 0)
        return ERROR_STACK_POP;
    if ((nCommand == COMMAND_ACTION_CAST_SPELL_AT_OBJECT ||
         nCommand == COMMAND_ACTION_CAST_SPELL_AT_LOCATION) && nArgs >= 7)
    {
        if (g_pVirtualMachine->StackPopInteger(&bInstantSpell) == 0)
            return ERROR_STACK_POP;
    }
    if (nSpell > g_pRules->m_pSpellArray->GetNumSpells() || nSpell < 0)
        return 0;
    switch (nProjectilePath)
    {
    case SCRIPT_PROJECTILE_PATH_DEFAULT:
        nProjectilePath = CSWSObject::SPELL_PROJECTILE_PATH_DEFAULT;
        break;
    case SCRIPT_PROJECTILE_PATH_HOMING:
        nProjectilePath = CSWCProjectile::PROJECTILE_PATH_TYPE_HOMING;
        break;
    case SCRIPT_PROJECTILE_PATH_BALLISTIC:
        nProjectilePath = CSWCProjectile::PROJECTILE_PATH_TYPE_BALLISTIC;
        break;
    case SCRIPT_PROJECTILE_PATH_HIGH_BALLISTIC:
        nProjectilePath = CSWCProjectile::PROJECTILE_PATH_TYPE_HIGH_BALLISTIC;
        break;
    case SCRIPT_PROJECTILE_PATH_ACCELERATING:
        nProjectilePath = CSWCProjectile::PROJECTILE_PATH_TYPE_ACCELERATING;
        break;
    default:
        return 0;
    }

    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(m_oidObject);
    if (m_nCommandContext != COMMAND_CONTEXT_SERVER || pObject == NULL)
        return 0;
    if ((nCommand == COMMAND_ACTION_CAST_SPELL_AT_OBJECT ||
         nCommand == COMMAND_ACTION_CAST_FAKE_SPELL_AT_OBJECT) &&
        g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget) == NULL)
        return 0;
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
    if (bFake)
    {
        if (pCreature != NULL)
            pCreature->AddCastSpellActions(nSpell, 0, 0, 0, 0,
                lTarget.m_vPosition, oidTarget, 0, bLocation, 0, 1,
                (unsigned char)nProjectilePath, bInstantSpell,
                CSWSCreature::SPELL_ACTION_OPTION10_UNSET,
                CSWSCreature::SPELL_ACTION_OPTION11_UNSET, 0);
        else if (pPlaceable != NULL)
            pPlaceable->AddCastSpellActions(nSpell, 0, lTarget.m_vPosition,
                oidTarget, 1, (unsigned char)nProjectilePath);
        return 0;
    }
    if (pCreature != NULL)
    {
        unsigned char nCasterLevel = CSWSCreature::SPELL_ACTION_OPTION11_UNSET;
        if (pCreature->m_bCommandable == 0)
            return 0;
        unsigned char nMultiClass = CSWSCreatureStats::MAX_CLASSES;
        if (bCheat)
        {
            nMultiClass = CSWSCreatureStats::SPELL_CLASS_CHEAT;
            nMetaMagic = 0;
        }
        else
        {
            for (int nClass = 0; nClass < pCreature->GetStats()->GetNumClasses(); ++nClass)
            {
                if (CSWClass::IsJedi(pCreature->GetStats()->GetClass(nClass)) &&
                    pCreature->GetStats()->GetSpellUsesLeft(nSpell, nClass, 0, 0))
                {
                    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
                    if (pSpell != NULL &&
                        pSpell->GetSpellLevel(pCreature->GetStats()->GetClass(nClass)) <
                            CSWSpell::SPELL_LEVEL_INVALID)
                    {
                        nMultiClass = (unsigned char)nClass;
                        break;
                    }
                }
            }
            if (nMultiClass == CSWSCreatureStats::MAX_CLASSES)
            {
                unsigned char nAbilityCasterLevel;
                if (pCreature->GetStats()->GetIsInSpellLikeAbilityList(nSpell) != 1 ||
                    (nAbilityCasterLevel = pCreature->GetStats()->GetSpellLikeAbilityCasterLevel(nSpell)) == -1)
                    return 0;
                nMultiClass = CSWSCreatureStats::SPELL_CLASS_SPELL_LIKE_ABILITY;
                nCasterLevel = nAbilityCasterLevel;
            }
            nMetaMagic = 0;
        }
        pCreature->AddCastSpellActions(nSpell, nMultiClass, nDomainLevel, 0, 0,
            lTarget.m_vPosition, oidTarget, 0, bLocation, 0, 0,
            (unsigned char)nProjectilePath, bInstantSpell,
            CSWSCreature::SPELL_ACTION_OPTION10_UNSET, nCasterLevel, 0);
        return 0;
    }
    else if (pPlaceable != NULL && pPlaceable->m_bCommandable != 0)
        pPlaceable->AddCastSpellActions(nSpell, nMetaMagic, lTarget.m_vPosition,
            oidTarget, 0, (unsigned char)nProjectilePath);
    return 0;
}
// PC 0x005B1380
int CSWVirtualMachineCommands::ExecuteCommandChangeFaction(int nCommand, int nArgs)
{

    unsigned long oidObjectToChangeFaction;
    int nFaction;
    unsigned long oidMemberOfFactionToJoin;
    int nStandardFaction;

    if (g_pVirtualMachine->StackPopObject(&oidObjectToChangeFaction) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nCommand == COMMAND_CHANGE_FACTION)
    {
        if (g_pVirtualMachine->StackPopObject(&oidMemberOfFactionToJoin) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    else if (nCommand == COMMAND_CHANGE_TO_STANDARD_FACTION &&
             g_pVirtualMachine->StackPopInteger(&nStandardFaction) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CServerExoAppInternal* pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
    if (pServerExoAppInternal == NULL)
    {
        return 0;
    }
    CFactionManager* pFactionManager = pServerExoAppInternal->m_pFactionManager;
    if (pFactionManager == NULL)
    {
        return 0;
    }
    if (nCommand == COMMAND_CHANGE_FACTION)
    {
        CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(oidObjectToChangeFaction);
        if (pCreature == NULL)
        {
            return 0;
        }
        if (pCreature->m_pStats->m_bIsPC != 0)
        {
            return 0;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetFactionOfObject(oidMemberOfFactionToJoin, &nFaction) == 0)
        {
            return 0;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pMember = pServerExoApp->GetCreatureByGameObjectID(oidMemberOfFactionToJoin);
        if (pMember == NULL)
        {
            return 0;
        }
        if (pMember->m_pStats->m_bIsPC != 0)
        {
            return 0;
        }
    }
    else if (nCommand == COMMAND_CHANGE_TO_STANDARD_FACTION)
    {
        if (pFactionManager->GetIsNPCFaction(nStandardFaction) == 0)
        {
            return 0;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(oidObjectToChangeFaction);
        if (pCreature != NULL && pCreature->m_pStats->m_bIsPC != 0)
        {
            return 0;
        }
        nFaction = nStandardFaction;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pGameObject = pServerExoApp->GetGameObject(oidObjectToChangeFaction);
    if (pGameObject != NULL)
    {
        if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            pGameObject->AsSWSPlaceable()->SetFactionId(nFaction);
            return 0;
        }
        if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
        {
            pGameObject->AsSWSTrigger()->SetFactionId(nFaction);
            return 0;
        }
        pFactionManager->GetFaction(nFaction)->AddMember(oidObjectToChangeFaction, 0);
    }
    return 0;
}
// PC 0x005B1570
int CSWVirtualMachineCommands::ExecuteCommandClearAllActions(int nCommand,
                                                             int nArgs)
{
    CSWSObject* pObject;
    int nResult;
    CServerExoApp* pServerExoApp;

    pObject = (CSWSObject*)this;
    nResult = m_nCommandContext;
    pObject = NULL;
    if (nResult != 0)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)(CGameObjectArray*)pServerExoApp->GetObjectArray()
                ->GetGameObject(m_oidObject, (struct CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS)
        {
            pObject->ClearAllActions(1);
        }
    }
    return 0;
}
// PC 0x005B15C0
int CSWVirtualMachineCommands::ExecuteCommandCloseDoor(int nCommand, int nArgs)
{
    unsigned long oidDoor;
    CSWSObject* pObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidDoor) == 0)
    {
        return -2001;
    }
    if (m_nCommandContext == 1)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(
                m_oidObject, (CGameObject**)&pObject) ==
                CGameObjectArray::SUCCESS &&
            pObject->GetArea() != NULL)
        {
            pObject->AsSWSObject()->AddCloseDoorAction(oidDoor, 0);
        }
    }
    return 0;
}
// PC 0x005D2F50
int CSWVirtualMachineCommands::ExecuteCommandCreateItemOnObject(int nCommand,
                                                             int nArgs)
{

    CExoString sItemTemplate;
    unsigned long oidTarget;
    CGameObject* pTarget;

    if (g_pVirtualMachine->StackPopString(&sItemTemplate) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    int nStackSize = 1;
    if (nArgs >= 3 && g_pVirtualMachine->StackPopInteger(&nStackSize) == 0)
    {
        return ERROR_STACK_POP;
    }
    int bHideMessage = 0;
    if (nArgs >= 4 && g_pVirtualMachine->StackPopInteger(&bHideMessage) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    unsigned long oidItem = CGameObjectArray::INVALIDOBJID;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            oidTarget, &pTarget) == CGameObjectArray::SUCCESS)
    {
        CResRef cTemplate((char*)sItemTemplate.CStr());
        CSWSItem* pItem = new CSWSItem(OBJECT_INVALID);
        if (pItem->LoadFromTemplate(cTemplate))
        {
            pItem->m_bDropable = 1;
            CSWBaseItem* pBaseItem =
                g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
            if (pBaseItem != NULL)
            {
                if (nStackSize > pBaseItem->GetMaxStackSize())
                {
                    nStackSize = pBaseItem->GetMaxStackSize();
                }
            }
            if (nStackSize >= 1)
            {
                pItem->m_nStackSize = (unsigned short)nStackSize;
                if (pTarget->AsSWSCreature() != NULL)
                {
                    CExoLocString sName;
                    if (pItem != NULL)
                    {
                        sName = pItem->m_sLocalizedName;
                    }
                    pTarget->AsSWSCreature()->AcquireItem(
                        &pItem, OBJECT_INVALID, OBJECT_INVALID, 1);

                    if (pTarget->AsSWSCreature()->m_bInParty != 0 && bHideMessage == 0)
                    {
                        g_pAppManager->GetClientExoApp()->GetInGameGui()
                            ->AddItemReceived(sName);
                        g_pAppManager->GetClientExoApp()->GetInGameGui()
                            ->UpdateStatus(GUI_STATUS_ITEM_RECEIVED, 0);
                    }
                }
                else if (pTarget->AsSWSPlaceable() != NULL)
                {
                    pTarget->AsSWSPlaceable()->AcquireItem(
                        &pItem, OBJECT_INVALID, 1);
                }
                else if (pTarget->AsSWSStore() != NULL)
                {
                    pTarget->AsSWSStore()->AcquireItem(pItem);
                }
                else
                {
                    delete pItem;
                    pItem = NULL;
                }
                if (pItem != NULL)
                {
                    oidItem = pItem->m_nId;
                }
            }
            else
            {
                delete pItem;
            }
        }
        else
        {
            delete pItem;
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidItem) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005B1640
int CSWVirtualMachineCommands::ExecuteCommandCreateObject(int nCommand, int nArgs)
{

    unsigned long oidObject = CGameObjectArray::INVALIDOBJID;
    CExoString sTemplate;
    int nObjectType;
    CScriptLocation* pLocation;
    int bUseAppearAnimation = 0;

    if (g_pVirtualMachine->StackPopInteger(&nObjectType) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopString(&sTemplate) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocation) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 4)
    {
        if (g_pVirtualMachine->StackPopInteger(&bUseAppearAnimation) == 0)
        {
            return ERROR_STACK_POP;
        }
    }

    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
    if (pModule != NULL)
    {
        CSWSArea* pArea = pModule->GetArea();
        if (pArea != NULL)
        {
            CResRef cTemplate((char*)sTemplate.CStr());
            switch (nObjectType)
            {
            case OBJECT_TYPE_ITEM:
                {
                    CSWSItem* pItem = new CSWSItem(OBJECT_INVALID);
                    pItem->LoadFromTemplate(cTemplate);
                    pItem->SetPosition(pLocation->m_vPosition, 1, 1, 0);
                    pItem->SetOrientation(pLocation->m_vOrientation);
                    pItem->AddToArea(pArea, pLocation->m_vPosition.m_fX,
                                     pLocation->m_vPosition.m_fY,
                                     pLocation->m_vPosition.m_fZ, 1);
                    oidObject = pItem->GetId();
                    break;
                }
            case OBJECT_TYPE_CREATURE:
                {
                    Vector vPosition;
                    CSWSCreature* pCreature = new CSWSCreature(OBJECT_INVALID, 0);
                    if (pCreature->LoadFromTemplate(cTemplate, 0) == 0)
                    {
                        delete pCreature;
                        if (pLocation != NULL)
                        {
                            delete pLocation;
                        }
                        if (g_pVirtualMachine->StackPushObject(oidObject) == 0)
                            return ERROR_STACK_PUSH;
                        return 0;
                    }
                    pCreature->SetOrientation(pLocation->m_vOrientation);
                    vPosition = pLocation->m_vPosition;
                    if (pArea->ComputeSafeLocation(pLocation->m_vPosition,
                            SWVM_CREATE_OBJECT_SEARCH_RADIUS,
                            pCreature->GetPathfindInformation(), 1,
                            &vPosition) != 0)
                    {
                        pLocation->m_vPosition = vPosition;
                    }
                    vPosition.m_fZ = pArea->AsSWSArea()->ComputeHeight(vPosition, 1, NULL);
                    if (bUseAppearAnimation)
                    {
                        CGameEffect* pEffect = new CGameEffect(1);
                        pEffect->m_nType = CSWSEffectListHandler::EFFECT_APPEAR;
                        pEffect->SetCreator(m_oidObject);
                        pCreature->ApplyEffect(pEffect, 0, 0);
                    }
                    pCreature->AddToArea(pArea, vPosition.m_fX, vPosition.m_fY,
                                        vPosition.m_fZ, 1, 1);
                    oidObject = pCreature->GetId();
                    break;
                }
            case OBJECT_TYPE_PLACEABLE:
                {
                    CSWSPlaceable* pPlaceable = new CSWSPlaceable(OBJECT_INVALID);
                    pPlaceable->LoadFromTemplate(cTemplate, 0);
                    pPlaceable->SetPosition(pLocation->m_vPosition, 1, 1, 0);
                    pPlaceable->CSWSObject::SetOrientation(pLocation->m_vOrientation);
                    const Vector& vOrientation = pLocation->m_vOrientation;
                    Vector vAxis(0.0f, 0.0f, 1.0f);
                    Quaternion qOrientation(vAxis, Yaw(vOrientation) *
                        (float)SWVM_DEGREES_TO_RADIANS);
                    pPlaceable->SetOrientation(qOrientation);
                    pPlaceable->AddToArea(pArea, pLocation->m_vPosition, 1);
                    oidObject = pPlaceable->GetId();
                    break;
                }
            case OBJECT_TYPE_STORE:
                {
                    CSWSStore* pStore = new CSWSStore(OBJECT_INVALID);
                    pStore->LoadFromTemplate(cTemplate, 0);
                    pStore->SetPosition(pLocation->m_vPosition, 1, 1, 0);
                    pStore->SetOrientation(pLocation->m_vOrientation);
                    pStore->AddToArea(pArea, pLocation->m_vPosition.m_fX,
                                      pLocation->m_vPosition.m_fY,
                                      pLocation->m_vPosition.m_fZ, 1);
                    oidObject = pStore->GetId();
                    break;
                }
            case OBJECT_TYPE_WAYPOINT:
                {
                    CSWSWaypoint* pWaypoint = new CSWSWaypoint(OBJECT_INVALID);
                    pWaypoint->LoadFromTemplate(cTemplate);
                    pWaypoint->SetPosition(pLocation->m_vPosition, 1, 1, 0);
                    pWaypoint->SetOrientation(pLocation->m_vOrientation);
                    pWaypoint->AddToArea(pArea, pLocation->m_vPosition.m_fX,
                                         pLocation->m_vPosition.m_fY,
                                         pLocation->m_vPosition.m_fZ, 0);
                    break;
                }
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidObject) == 0)
    {
        if (pLocation != NULL)
        {
            delete pLocation;
            pLocation = NULL;
        }
        return ERROR_STACK_PUSH;
    }
    if (pLocation != NULL)
    {
        delete pLocation;
        pLocation = NULL;
    }
    return 0;
}
// PC 0x005B1C50
int CSWVirtualMachineCommands::ExecuteCommandDelayCommand(int nCommand,
                                                          int nArgs)
{
    float fSeconds;
    void* pAction = NULL;
    CGameObject* pObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopFloat(&fSeconds) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopCommand(&pAction) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER &&
        (unsigned char)(pServerExoApp = g_pAppManager->m_pServerExoApp)
                ->GetObjectArray()->GetGameObject(m_oidObject, &pObject) ==
            CGameObjectArray::SUCCESS)
    {
        unsigned long oidCaller = CGameObjectArray::INVALIDOBJID;
        if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
        {
            oidCaller = m_oidObject;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoApp->GetServerAIMaster()->AddEventDeltaTime(
            0, (int)(fSeconds * 1000.0f), oidCaller, m_oidObject,
            CServerAIMaster::EVENT_TIMED_EVENT, pAction);
    }
    else if (pAction != NULL)
    {
        delete pAction;
    }
    return 0;
}
// PC 0x005B1D40
int CSWVirtualMachineCommands::ExecuteCommandDestroyObject(int nCommand, int nArgs)
{
    unsigned long oidDestroy;
    float fDelay = 0.0f;
    int bNoFade = 0;
    float fDelayUntilFade = 0.0f;

    if (g_pVirtualMachine->StackPopObject(&oidDestroy) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 2 && g_pVirtualMachine->StackPopFloat(&fDelay) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 3 && g_pVirtualMachine->StackPopInteger(&bNoFade) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 4 && g_pVirtualMachine->StackPopFloat(&fDelayUntilFade) == 0)
    {
        return ERROR_STACK_POP;
    }
    int bNoDrop = 0;
    if (nArgs >= 5 && g_pVirtualMachine->StackPopInteger(&bNoDrop) == 0)
    {
        return ERROR_STACK_POP;
    }

    CSWCCreature* pCharacter =
        g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
    if (pCharacter != NULL &&
        g_pAppManager->GetServerExoApp()->ClientToServerObjectId(
            pCharacter->GetId()) == oidDestroy)
    {
        unsigned long oidCharacter = pCharacter->GetId();
        unsigned char nAttempts = 0;
        while (nAttempts < g_pAppManager->GetClientExoApp()->GetSWParty()->m_nCharacters)
        {
            g_pAppManager->GetClientExoApp()->ChangeCharacter(CSWParty::LEADER_NEXT, 1);
            pCharacter =
                g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
            if (pCharacter != NULL &&
                g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(
                    g_pAppManager->GetServerExoApp()->ClientToServerObjectId(
                        pCharacter->GetId())) != NULL)
            {
                g_pAppManager->GetClientExoApp()->SetPlayerCreature(pCharacter->GetId());
                g_pAppManager->GetClientExoApp()->AttachPlayerGameObjectToArea();
                break;
            }
            ++nAttempts;
        }
        g_pAppManager->GetClientExoApp()->GetSWParty()->RemoveCharacter(oidCharacter);
    }

    CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
    CGameObject* pObject = pServerExoApp->GetGameObject(oidDestroy);
    if (pObject != NULL && pObject->AsSWSObject() != NULL)
    {
        unsigned long nFadeOutTime = (unsigned long)(fDelayUntilFade * 1000.0f);
        pObject->AsSWSObject()->SetDeleteNoFade(bNoFade);
        pObject->AsSWSObject()->SetFadeOutTime(nFadeOutTime);
    }
    pServerExoApp->GetPartyTable()->ClearMember(
        pServerExoApp->GetPartyTable()->GetNPCID(oidDestroy));
    pServerExoApp->GetPartyTable()->ClearPuppet(
        pServerExoApp->GetPartyTable()->GetPUPID(oidDestroy));
    if (bNoDrop == 0)
    {
        pServerExoApp->GetServerAIMaster()->AddEventDeltaTime(
            0, (int)(fDelay * 1000.0f), m_oidObject, oidDestroy,
            CServerAIMaster::EVENT_DESTROY_OBJECT, (void*)1);
    }
    else
    {
        pServerExoApp->GetServerAIMaster()->AddEventDeltaTime(
            0, (int)(fDelay * 1000.0f), m_oidObject, oidDestroy,
            CServerAIMaster::EVENT_DESTROY_OBJECT, (void*)0);
    }
    return 0;
}
// PC 0x005B1FF0
int CSWVirtualMachineCommands::ExecuteCommandDisplayFeedBackText(int nCommand,
                                                               int nArgs)
{

    unsigned long oidCreature;
    int nTextConstant;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    oidCreature = pClientExoApp->ServerToClientObjectId(oidCreature);
    if (g_pVirtualMachine->StackPopInteger(&nTextConstant) == 0)
    {
        return ERROR_STACK_POP;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    CGameObject* pObject = pClientExoApp->GetGameObject(oidCreature);
    int nStrRef = 0;
    if (pObject != NULL && pObject->AsSWCObject() != NULL)
    {
        {
            CExoString sColumn("StrRef");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pFeedBackText);
            C2DA* pFeedBackText = p2DArrays->m_pFeedBackText;
            pFeedBackText->GetINTEntry(nTextConstant, sColumn, &nStrRef);
        }
        if (nStrRef != 0)
        {
            CTlkTable* pTlkTable = g_pTlkTable;
            CExoString sFeedback = pTlkTable->GetSimpleString(nStrRef);
            pObject->AsSWCObject()->SetFeedbackInfo(sFeedback);
        }
        else
        {
            CExoString sFeedback("BAD STRREF");
            pObject->AsSWCObject()->SetFeedbackInfo(sFeedback);
        }
    }
    return 0;
}
// PC 0x005B2170
int CSWVirtualMachineCommands::ExecuteCommandDistanceConversions(int nCommand,
                                                                 int nArgs)
{
    float fDistance;
    float fResult;

    if (g_pVirtualMachine->StackPopFloat(&fDistance) == 0)
    {
        return -2001;
    }

    switch (nCommand)
    {
    case 218:
        fResult = fDistance * 0.3048f;
        break;
    case 219:
        fResult = fDistance * 0.91439998f;
        break;
    }

    return g_pVirtualMachine->StackPushFloat(fResult) != 0 ? 0 : -2000;
}
// PC 0x005B21E0
int CSWVirtualMachineCommands::ExecuteCommandDoDoorAction(int nCommand, int nArgs)
{

    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(m_oidObject);
    unsigned long oidTarget;
    int nDoorAction;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nDoorAction) == 0)
    {
        return ERROR_STACK_POP;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL && pCreature != NULL)
    {
        CSWSObject* pObject = pGameObject->AsSWSObject();
        switch (nDoorAction)
        {
        case DOOR_ACTION_OPEN:
            if (nCommand == COMMAND_DO_DOOR_ACTION)
                pCreature->AddActionToFront(CSWSObject::ACTION_OPEN_DOOR, CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_OBJECT, &oidTarget,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            else
                pCreature->AddActionToFront(CSWSObject::ACTION_USE_OBJECT, CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_OBJECT, &oidTarget,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            break;
        case DOOR_ACTION_UNLOCK:
            if (nCommand == COMMAND_DO_DOOR_ACTION)
                pCreature->AddActionToFront(CSWSObject::ACTION_OPEN_DOOR, CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_OBJECT, &oidTarget,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            else
                pCreature->AddActionToFront(CSWSObject::ACTION_USE_OBJECT, CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_OBJECT, &oidTarget,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            pCreature->AddActionToFront(CSWSObject::ACTION_UNLOCK_OBJECT, CSWSObject::ACTION_GROUP_LAST,
                CSWSObject::ACTION_PARAM_OBJECT, &oidTarget,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            break;
        case DOOR_ACTION_BASH:
            pCreature->AddAttackActions(oidTarget, 0, 0, 0, 0, 1, 0, 1,
                CSWSCombatRound::ATTACK_ANIMATION_DEFAULT, CSWSCombatRound::ATTACK_ACTION_TIME,
                1, 0, 0, CSWSCombatRound::ATTACK_ACTION_OPTION_DEFAULT, 0);
            break;
        case DOOR_ACTION_IGNORE:
            if (nCommand == COMMAND_DO_DOOR_ACTION)
                pCreature->m_pPathfindInformation->m_nField_BC = oidTarget;
            break;
        case DOOR_ACTION_KNOCK:
            if (pObject->m_bPlotObject == 0)
            {
                for (unsigned char nClass = 0; nClass < pCreature->m_pStats->m_nClassCount; ++nClass)
                {
                    if (pCreature->m_pStats->GetSpellUsesLeft(CSWSpell::SPELL_KNOCK, nClass, 0, 0) > 0)
                    {
                        pCreature->AddCastSpellActions(CSWSpell::SPELL_KNOCK, nClass, 0, 0, 0,
                            pObject->m_vPosition, oidTarget, 0, 0, 1, 0, 0, 0,
                            CSWSCreature::SPELL_ACTION_OPTION10_UNSET,
                            CSWSCreature::SPELL_ACTION_OPTION11_UNSET, 0);
                        break;
                    }
                }
            }
            break;
        }
    }
    return 0;
}
// PC 0x005B24E0
int CSWVirtualMachineCommands::ExecuteCommandDoSinglePlayerAutoSave(
    int nCommand, int nArgs)
{
    CServerExoApp* pServerExoApp;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->SetAutoSavePending(1);
    return 0;
}
// PC 0x005B2500
int CSWVirtualMachineCommands::ExecuteCommandEffectAbilityDecrease(int nCommand,
                                                                   int nArgs)
{
    int nAbility;
    int nModifyBy;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nAbility) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nModifyBy) == 0)
    {
        return -2001;
    }
    if (nAbility < 0 || nAbility > CSWSEffectListHandler::EFFECT_ABILITY_CHARISMA)
    {
        nAbility = CSWSEffectListHandler::EFFECT_ABILITY_STRENGTH;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_DECREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nAbility);
    pGameEffect->SetInteger(1, nModifyBy);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B2670
int CSWVirtualMachineCommands::ExecuteCommandEffectAbilityIncrease(int nCommand,
                                                                   int nArgs)
{
    int nAbilityToIncrease;
    int nModifyBy;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nAbilityToIncrease) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nModifyBy) == 0)
    {
        return -2001;
    }
    if (nAbilityToIncrease < 0 ||
        nAbilityToIncrease > CSWSEffectListHandler::EFFECT_ABILITY_CHARISMA)
    {
        nAbilityToIncrease = CSWSEffectListHandler::EFFECT_ABILITY_STRENGTH;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_ABILITY_INCREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nAbilityToIncrease);
    pGameEffect->SetInteger(1, nModifyBy);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B27E0
int CSWVirtualMachineCommands::ExecuteCommandEffectACDecrease(int nCommand,
                                                              int nArgs)
{
    int nValue;
    int nModifyType;
    int nResult;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nModifyType) == 0)
    {
        return -2001;
    }
    nResult = CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL;
    if (nArgs >= 3 && g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    if (nModifyType < 0 || nModifyType > CSWSEffectListHandler::AC_DEFLECTION)
    {
        nModifyType = 0;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_AC_DECREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nModifyType);
    pGameEffect->SetInteger(1, nValue);
    pGameEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
    pGameEffect->SetInteger(5, nResult);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B29A0
int CSWVirtualMachineCommands::ExecuteCommandEffectACIncrease(int nCommand,
                                                              int nArgs)
{
    int nValue;
    int nModifyType;
    int nResult;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nModifyType) == 0)
    {
        return -2001;
    }
    nResult = CSWSEffectListHandler::DAMAGE_FLAG_PHYSICAL;
    if (nArgs >= 3 && g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    if (nModifyType < 0 || nModifyType > CSWSEffectListHandler::AC_DEFLECTION)
    {
        nModifyType = 0;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_AC_INCREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nModifyType);
    pGameEffect->SetInteger(1, nValue);
    pGameEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
    pGameEffect->SetInteger(5, nResult);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B2B60
int CSWVirtualMachineCommands::ExecuteCommandEffectAreaEffect(int nCommand,
                                                             int nArgs)
{

    int nAreaEffectId;
    CExoString sOnEnterScript;
    CExoString sHeartbeatScript;
    CExoString sOnExitScript;
    CGameEffect* pGameEffect;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nAreaEffectId) == 0 ||
        g_pVirtualMachine->StackPopString(&sOnEnterScript) == 0 ||
        g_pVirtualMachine->StackPopString(&sHeartbeatScript) == 0 ||
        g_pVirtualMachine->StackPopString(&sOnExitScript) == 0)
    {
        return ERROR_STACK_POP;
    }

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_AREA_OF_EFFECT;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nAreaEffectId);
    pGameEffect->SetFloat(0, 0.0f);
    pGameEffect->SetFloat(1, 0.0f);
    pGameEffect->SetFloat(2, 0.0f);
    pGameEffect->SetString(0, sOnEnterScript);
    pGameEffect->SetString(1, sHeartbeatScript);
    pGameEffect->SetString(2, sOnExitScript);

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()
            ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_EFFECT,
                                                  pGameEffect) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B2DE0
int CSWVirtualMachineCommands::ExecuteCommandEffectAttackDecrease(int nCommand,
                                                                  int nArgs)
{
    int nPenalty;
    int nModifierType;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nPenalty) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nModifierType) == 0)
    {
        return -2001;
    }
    if (nModifierType < 0 ||
        nModifierType > CSWSEffectListHandler::ATTACK_BONUS_UNARMED)
    {
        nModifierType = CSWSEffectListHandler::ATTACK_BONUS_MISC;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_ATTACK_DECREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nPenalty);
    pGameEffect->SetInteger(1, nModifierType);
    pGameEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B2F60
int CSWVirtualMachineCommands::ExecuteCommandEffectAttackIncrease(int nCommand,
                                                                  int nArgs)
{
    int nBonus;
    int nModifierType;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nBonus) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nModifierType) == 0)
    {
        return -2001;
    }
    if (nModifierType < 0 ||
        nModifierType > CSWSEffectListHandler::ATTACK_BONUS_UNARMED)
    {
        nModifierType = CSWSEffectListHandler::ATTACK_BONUS_MISC;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_ATTACK_INCREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nBonus);
    pGameEffect->SetInteger(1, nModifierType);
    pGameEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B30E0
int CSWVirtualMachineCommands::ExecuteCommandEffectBeam(int nCommand, int nArgs)
{

    int nBeamVisualEffect;
    unsigned long oidEffector;
    int nBodyPart;
    int bMissEffect = 0;
    CGameEffect* pGameEffect;

    if (g_pVirtualMachine->StackPopInteger(&nBeamVisualEffect) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidEffector) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nBodyPart) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&bMissEffect) == 0)
    {
        return ERROR_STACK_POP;
    }
    pGameEffect = new CGameEffect(1);
    if (CLoopingVisualEffect::GetIsBeam(nBeamVisualEffect) == 0)
    {
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    else
    {
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_BEAM;
        pGameEffect->SetCreator(m_oidObject);
        pGameEffect->SetInteger(0, nBeamVisualEffect);
        pGameEffect->SetInteger(1, nBodyPart);
        pGameEffect->SetInteger(2, bMissEffect);
        pGameEffect->SetObjectID(0, oidEffector);
        pGameEffect->m_nSubType =
            (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
            SUBTYPE_MAGICAL;
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B3420
int CSWVirtualMachineCommands::ExecuteCommandEffectComplex(int nCommand, int nArgs)
{

    CGameEffect* pGameEffect;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    switch (nCommand)
    {
    case COMMAND_EFFECT_ENTANGLE:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_ENTANGLE;
        break;
    case COMMAND_EFFECT_DEATH:
        {
            int bSpectacularDeath = 0;
            int bDisplayFeedback = 1;
            int bNoFadeAway = 0;
            if ((nArgs >= 1 && g_pVirtualMachine->StackPopInteger(&bSpectacularDeath) == 0) ||
                (nArgs >= 2 && g_pVirtualMachine->StackPopInteger(&bDisplayFeedback) == 0) ||
                (nArgs >= 3 && g_pVirtualMachine->StackPopInteger(&bNoFadeAway) == 0))
            {
                return ERROR_STACK_POP;
            }
            pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_DEATH;
            pGameEffect->SetInteger(0, bSpectacularDeath);
            pGameEffect->SetInteger(1, bDisplayFeedback);
            pGameEffect->SetInteger(2, bNoFadeAway);
        }
        break;
    case COMMAND_EFFECT_KNOCKDOWN:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_KNOCKDOWN;
        break;
    case COMMAND_EFFECT_PARALYZE:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_HOLD);
        break;
    case COMMAND_EFFECT_SLEEP:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_SLEEP);
        break;
    case COMMAND_EFFECT_CONFUSED:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_CONFUSION);
        break;
    case COMMAND_EFFECT_FRIGHTENED:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_FEAR);
        break;
    case COMMAND_EFFECT_STUNNED:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_STUN);
        break;
    case COMMAND_EFFECT_HASTE:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_HASTE;
        break;
    case COMMAND_EFFECT_REGENERATE:
        {
            int nAmount;
            float fIntervalSeconds;
            if (g_pVirtualMachine->StackPopInteger(&nAmount) == 0)
            {
                return ERROR_STACK_POP;
            }
            if (g_pVirtualMachine->StackPopFloat(&fIntervalSeconds) == 0)
            {
                return ERROR_STACK_POP;
            }
            int nInterval = (int)(fIntervalSeconds * EFFECT_MILLISECONDS_PER_SECOND);
            pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_REGENERATE;
            pGameEffect->SetInteger(0, nAmount);
            pGameEffect->SetInteger(1, nInterval);
        }
        break;
    case COMMAND_EFFECT_ASSURED_HIT:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_ASSUREDHIT;
        break;
    case COMMAND_EFFECT_ASSURED_DEFLECTION:
        {
            int bReturn = 0;
            if (g_pVirtualMachine->StackPopInteger(&bReturn) == 0)
            {
                return ERROR_STACK_POP;
            }
            pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_ASSUREDDEFLECTION;
            pGameEffect->SetInteger(0, bReturn);
        }
        break;
    int nInteger2;
    case COMMAND_EFFECT_TIME_STOP:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_TIMESTOP;
        break;
    case COMMAND_EFFECT_DISPEL_MAGIC_BEST:
        {
            int nCasterLevel;
            pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_DISPEL_BEST_MAGIC;
            if (g_pVirtualMachine->StackPopInteger(&nCasterLevel) == 0)
            {
                return ERROR_STACK_POP;
            }
        }
        break;
    case COMMAND_EFFECT_DISPEL_MAGIC_ALL:
        {
            int nCasterLevel;
            pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_DISPEL_ALL_MAGIC;
            if (g_pVirtualMachine->StackPopInteger(&nCasterLevel) == 0)
            {
                return ERROR_STACK_POP;
            }
        }
        break;
    case COMMAND_EFFECT_DISGUISE:
        {
            int nDisguise;
            if (g_pVirtualMachine->StackPopInteger(&nDisguise) == 0)
            {
                return ERROR_STACK_POP;
            }
            pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_DISGUISE;
            pGameEffect->SetInteger(0, nDisguise);
            pGameEffect->SetInteger(1, 0);
        }
        break;
    case COMMAND_EFFECT_FORCE_RESISTED:
        {
            unsigned long oidSource = OBJECT_INVALID;
            pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCERESISTED;
            if (g_pVirtualMachine->StackPopObject(&oidSource) == 0)
            {
                return ERROR_STACK_POP;
            }
            pGameEffect->SetObjectID(0, oidSource);
        }
        break;
    case COMMAND_EFFECT_FORCE_FIZZLE:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEFIZZLE;
        break;
    case COMMAND_EFFECT_DROID_CONFUSED:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_VARIANT_16);
        break;
    case COMMAND_EFFECT_MIND_TRICK:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_MIND_TRICK_18);
        break;
    case COMMAND_EFFECT_DROID_SCRAMBLE:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_MIND_TRICK_19);
        break;
    case COMMAND_EFFECT_CUTSCENE_HORRIFIED:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_HORRIFIED);
        pGameEffect->SetInteger(1, 1);
        break;
    case COMMAND_EFFECT_CUTSCENE_PARALYZE:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_HOLD);
        pGameEffect->SetInteger(1, 1);
        break;
    case COMMAND_EFFECT_CUTSCENE_STUNNED:
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
        pGameEffect->SetInteger(0, CSWSEffectListHandler::STATE_STUN);
        pGameEffect->SetInteger(1, 1);
        break;
    }
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS && pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B3A50
int CSWVirtualMachineCommands::ExecuteCommandEffectConcealment(int nCommand,
                                                               int nArgs)
{
    int nPercentage;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nPercentage) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    if (nPercentage < 1 || nPercentage > 100)
    {
        pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    else
    {
        pGameEffect2->SetCreator(m_oidObject);
        pGameEffect2->m_nSubType =
            (pGameEffect2->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
            SUBTYPE_MAGICAL;
        pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_CONCEALMENT;
        pGameEffect2->SetInteger(0, nPercentage);
        pGameEffect2->SetInteger(1, (unsigned char)g_pRules->m_nNumRaces);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B3B90
int CSWVirtualMachineCommands::ExecuteCommandEffectDamage(int nCommand, int nArgs)
{

    int nDamageAmount;
    int nDamageType;
    int nDamagePower;
    CGameEffect* pGameEffect;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nDamageAmount) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nDamageType) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    else
    {
        nDamageType = SCRIPT_EFFECT_DAMAGE_DEFAULT_TYPE;
    }
    if (nArgs >= 3)
    {
        if (g_pVirtualMachine->StackPopInteger(&nDamagePower) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    else
    {
        nDamagePower = CSWSEffectListHandler::DAMAGE_POWER_NORMAL;
    }
    if (nDamageAmount < 0 || nDamageAmount > SCRIPT_MAX_DAMAGE_AMOUNT)
    {
        nDamageAmount = 1;
    }
    if (nDamageType < 0 ||
        nDamageType > CSWSEffectListHandler::DAMAGE_FLAG_POISON)
    {
        nDamageType = CSWSEffectListHandler::DAMAGE_FLAG_UNIVERSAL;
    }
    if (nDamagePower < 0 ||
        nDamagePower > CSWSEffectListHandler::DAMAGE_POWER_ENERGY)
    {
        nDamagePower = CSWSEffectListHandler::DAMAGE_POWER_NORMAL;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE;
    pGameEffect->SetNumIntegers(21);
    for (int nDamageIndex = 0; nDamageIndex < 15; ++nDamageIndex)
    {
        pGameEffect->SetInteger(nDamageIndex, -1);
    }
    pGameEffect->SetInteger(Log_Base2(nDamageType), nDamageAmount);
    pGameEffect->SetInteger(14, nDamageAmount);
    pGameEffect->SetInteger(16, 1000);
    pGameEffect->SetInteger(17, nDamageType);
    pGameEffect->SetInteger(18, nDamagePower);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()
            ->GetGameObject(m_oidObject, &pGameObject) == CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B3E00
int CSWVirtualMachineCommands::ExecuteCommandEffectDamageDecrease(int nCommand,
                                                                  int nArgs)
{
    int nPenalty;
    int nDamageType;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nPenalty) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nDamageType) == 0)
    {
        return -2001;
    }
    if (nPenalty <= 0 || nPenalty > 10)
    {
        nPenalty = 0;
    }
    if (nDamageType < 0 ||
        nDamageType > CSWSEffectListHandler::DAMAGE_FLAG_POISON)
    {
        nDamageType = CSWSEffectListHandler::DAMAGE_FLAG_UNIVERSAL;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nPenalty);
    pGameEffect->SetInteger(1, nDamageType);
    pGameEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B3FA0
int CSWVirtualMachineCommands::ExecuteCommandEffectDamageImmunityDecrease(
    int nCommand, int nArgs)
{
    CGameEffect* pEffect;
    int nAmount;
    int nDamageType;

    pEffect = new CGameEffect(1);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_DECREASE;
    if (g_pVirtualMachine->StackPopInteger(&nDamageType) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAmount) == 0)
    {
        return -2001;
    }
    if (nAmount <= 100)
    {
        if (nAmount < -100)
        {
            nAmount = -100;
        }
    }
    else
    {
        nAmount = 100;
    }
    if (nDamageType < 0 ||
        nDamageType > CSWSEffectListHandler::DAMAGE_FLAG_POISON)
    {
        nDamageType = CSWSEffectListHandler::DAMAGE_FLAG_UNIVERSAL;
    }
    pEffect->m_nSubType =
        (pEffect->m_nSubType & ~SUBTYPE_SUPERNATURAL) |
        SUBTYPE_MAGICAL;
    pEffect->SetCreator(m_oidObject);
    pEffect->SetInteger(0, nDamageType);
    pEffect->SetInteger(1, nAmount);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005B4100
int CSWVirtualMachineCommands::ExecuteCommandEffectDamageImmunityIncrease(
    int nCommand, int nArgs)
{
    CGameEffect* pEffect;
    int nAmount;
    int nDamageType;

    pEffect = new CGameEffect(1);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_INCREASE;
    if (g_pVirtualMachine->StackPopInteger(&nDamageType) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAmount) == 0)
    {
        return -2001;
    }
    if (nAmount <= 100)
    {
        if (nAmount < 0)
        {
            nAmount = 0;
        }
    }
    else
    {
        nAmount = 100;
    }
    if (nDamageType < 0 ||
        nDamageType > CSWSEffectListHandler::DAMAGE_FLAG_POISON)
    {
        nDamageType = CSWSEffectListHandler::DAMAGE_FLAG_UNIVERSAL;
    }
    pEffect->m_nSubType =
        (pEffect->m_nSubType & ~SUBTYPE_SUPERNATURAL) |
        SUBTYPE_MAGICAL;
    pEffect->SetCreator(m_oidObject);
    pEffect->SetInteger(0, nDamageType);
    pEffect->SetInteger(1, nAmount);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005B4260
int CSWVirtualMachineCommands::ExecuteCommandEffectDamageIncrease(int nCommand,
                                                                  int nArgs)
{
    int nBonus;
    int nDamageType;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nBonus) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nDamageType) == 0)
    {
        return -2001;
    }
    if (nBonus <= 0 || nBonus > 10)
    {
        nBonus = 0;
    }
    if (nDamageType < 0 ||
        nDamageType > CSWSEffectListHandler::DAMAGE_FLAG_POISON)
    {
        nDamageType = CSWSEffectListHandler::DAMAGE_FLAG_UNIVERSAL;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nBonus);
    pGameEffect->SetInteger(1, nDamageType);
    pGameEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B4400
int CSWVirtualMachineCommands::ExecuteCommandEffectDamageReduction(int nCommand,
                                                                   int nArgs)
{
    int nAmount;
    int nDamagePower;
    int nLimit;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nAmount) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nDamagePower) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nLimit) == 0)
    {
        return -2001;
    }
    if (nDamagePower < 0 ||
        nDamagePower > CSWSEffectListHandler::DAMAGE_POWER_ENERGY)
    {
        nDamagePower = CSWSEffectListHandler::DAMAGE_POWER_NORMAL;
    }
    nAmount = (nAmount < 0) ? 0 : nAmount;
    nLimit = (nLimit < 0) ? 0 : nLimit;
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_REDUCTION;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nAmount);
    pGameEffect->SetInteger(1, nDamagePower);
    pGameEffect->SetInteger(2, nLimit);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B45B0
int CSWVirtualMachineCommands::ExecuteCommandEffectDamageResistance(
    int nCommand, int nArgs)
{
    int nDamageType;
    int nAmount;
    int nLimit;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nDamageType) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAmount) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nLimit) == 0)
    {
        return -2001;
    }
    if (nDamageType < 0 ||
        nDamageType > CSWSEffectListHandler::DAMAGE_FLAG_POISON)
    {
        nDamageType = CSWSEffectListHandler::DAMAGE_FLAG_UNIVERSAL;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nDamageType);
    pGameEffect->SetInteger(1, nAmount);
    pGameEffect->SetInteger(2, nLimit);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B4740
int CSWVirtualMachineCommands::ExecuteCommandEffectForceShield(int nCommand,
                                                               int nArgs)
{
    int nResult;
    CGameEffect* pGameEffect2;

    nResult = 0;
    if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    pGameEffect2->SetCreator(m_oidObject);
    pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_FORCESHIELD;
    pGameEffect2->SetInteger(0, nResult);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B4840
int CSWVirtualMachineCommands::ExecuteCommandEffectDamageShield(int nCommand,
                                                                int nArgs)
{
    int nDamageAmount;
    int nRandomAmount;
    int nDamageType;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nDamageAmount) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nRandomAmount) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nDamageType) == 0)
    {
        return -2001;
    }
    if (nDamageAmount < 0 || nDamageAmount > SCRIPT_MAX_DAMAGE_AMOUNT)
    {
        nDamageAmount = 1;
    }
    if (nRandomAmount <= 0 || nRandomAmount > 10)
    {
        nRandomAmount = 0;
    }
    if (nDamageType < 0 ||
        nDamageType > CSWSEffectListHandler::DAMAGE_FLAG_POISON)
    {
        nDamageType = CSWSEffectListHandler::DAMAGE_FLAG_UNIVERSAL;
    }
    pGameEffect2 = new CGameEffect(1);
    pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_SHIELD;
    pGameEffect2->SetCreator(m_oidObject);
    pGameEffect2->m_nSubType =
        (pGameEffect2->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect2->SetInteger(0, nDamageAmount);
    pGameEffect2->SetInteger(1, nRandomAmount);
    pGameEffect2->SetInteger(2, nDamageType);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B5030
int CSWVirtualMachineCommands::ExecuteCommandEffectForceDrain(int nCommand,
                                                              int nArgs)
{
    int nDamage;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nDamage) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    if (nDamage > 0)
    {
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEDRAIN;
        pGameEffect->SetNumIntegers(1);
        pGameEffect->SetInteger(0, nDamage);
    }
    else
    {
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B5180
int CSWVirtualMachineCommands::ExecuteCommandEffectHeal(int nCommand, int nArgs)
{
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (nArgs != 1)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nArgs) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    if (nArgs > 0)
    {
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_HEAL;
        pGameEffect->SetInteger(0, nArgs);
    }
    else
    {
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != 0)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B52E0
int CSWVirtualMachineCommands::ExecuteCommandEffectHitPointChangeWhenDying(
    int nCommand, int nArgs)
{
    float fChange;
    CGameEffect* pEffect;

    if (g_pVirtualMachine->StackPopFloat(&fChange) == 0)
    {
        return -2001;
    }
    pEffect = new CGameEffect(1);
    if (fChange == 0.0f)
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    else
    {
        pEffect->SetCreator(m_oidObject);
        pEffect->m_nType =
            CSWSEffectListHandler::EFFECT_HITPOINTCHANGEWHENDYING;
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        pEffect->SetFloat(0, fChange);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005B5410
int CSWVirtualMachineCommands::ExecuteCommandEffectImmunity(int nCommand,
                                                            int nArgs)
{
    int nImmunityType;
    CGameEffect* pEffect;

    pEffect = new CGameEffect(1);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_IMMUNITY;
    pEffect->SetCreator(m_oidObject);
    pEffect->m_nSubType =
        (pEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    if (g_pVirtualMachine->StackPopInteger(&nImmunityType) == 0)
    {
        return -2001;
    }
    nImmunityType = max(0, min(CSWSEffectListHandler::IMMUNITY_TYPE_MAX,
                               nImmunityType));
    pEffect->SetInteger(0, nImmunityType);
    pEffect->SetInteger(1, (unsigned char)g_pRules->m_nNumRaces);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005B5550
int CSWVirtualMachineCommands::ExecuteCommandEffectInvisibility(int nCommand,
                                                                int nArgs)
{
    int nInvisibilityType;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nInvisibilityType) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    if (nInvisibilityType == 1 || nInvisibilityType == 2 ||
        nInvisibilityType == 4)
    {
        pGameEffect2->SetCreator(m_oidObject);
        pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_INVISIBILITY;
        pGameEffect2->m_nSubType =
            (pGameEffect2->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
            SUBTYPE_MAGICAL;
        pGameEffect2->SetInteger(0, nInvisibilityType);
        pGameEffect2->SetInteger(1, (unsigned char)g_pRules->m_nNumRaces);
    }
    else
    {
        pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B5690
int CSWVirtualMachineCommands::ExecuteCommandEffectLinkEffects(int nCommand,
                                                               int nArgs)
{
    CGameEffect* pGameEffect;
    CGameEffect* pGameEffect2;
    CGameEffect* pGameEffect3;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_LINK;
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pGameEffect2) ==
        0)
    {
        return -2000;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pGameEffect3) ==
        0)
    {
        return -2000;
    }
    pGameEffect->SetLinked((CGameEffect*)pGameEffect2, pGameEffect3);
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetCreator(m_oidObject);
    pGameEffect->UpdateLinked();
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B57A0
int CSWVirtualMachineCommands::ExecuteCommandEffectMissChance(int nCommand,
                                                              int nArgs)
{
    int nChance;
    CGameEffect* pEffect;

    if (g_pVirtualMachine->StackPopInteger(&nChance) == 0)
    {
        return -2001;
    }
    pEffect = new CGameEffect(1);
    if (nChance >= 1 && nChance <= 100)
    {
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
            SUBTYPE_MAGICAL;
        pEffect->SetCreator(m_oidObject);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_CONCEALMENT;
        pEffect->SetInteger(0, nChance);
        pEffect->SetInteger(1, CSWSEffectListHandler::MISS_CHANCE_TYPE_NORMAL);
    }
    else
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005B58D0
int CSWVirtualMachineCommands::ExecuteCommandEffectModifyAttacks(int nCommand,
                                                                 int nArgs)
{
    int nAttacks;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nAttacks) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    if (nAttacks > 5)
    {
        pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    else
    {
        pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_MODIFYNUMATTACKS;
        pGameEffect2->SetCreator(m_oidObject);
        pGameEffect2->m_nSubType =
            (pGameEffect2->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
            SUBTYPE_MAGICAL;
        pGameEffect2->SetInteger(0, nAttacks);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B59F0
int CSWVirtualMachineCommands::ExecuteCommandEffectMovementSpeedDecrease(
    int nCommand, int nArgs)
{
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (nArgs != 1)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nArgs) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType =
        CSWSEffectListHandler::EFFECT_MOVEMENT_SPEED_DECREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nArgs);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B5B30
int CSWVirtualMachineCommands::ExecuteCommandEffectMovementSpeedIncrease(
    int nCommand, int nArgs)
{
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (nArgs != 1)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nArgs) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType =
        CSWSEffectListHandler::EFFECT_MOVEMENT_SPEED_INCREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nArgs);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B5C70
int CSWVirtualMachineCommands::ExecuteCommandEffectPoison(int nCommand,
                                                          int nArgs)
{
    int nPoisonType;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nPoisonType) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    pGameEffect2->m_nSubType =
        (pGameEffect2->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect2->SetCreator(m_oidObject);
    pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_POISON;
    pGameEffect2->SetInteger(0, nPoisonType);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B5D70
int CSWVirtualMachineCommands::ExecuteCommandEffectResurrection(int nCommand,
                                                                int nArgs)
{
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    int nHitPointPercent;

    if (g_pVirtualMachine->StackPopInteger(&nHitPointPercent) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_RESURRECTION;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nHitPointPercent);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B5EA0
int CSWVirtualMachineCommands::ExecuteCommandEffectSavingThrowDecrease(
    int nCommand, int nArgs)
{
    int nResult;
    int nSave;
    int nResult2;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    nResult = 0;
    if (g_pVirtualMachine->StackPopInteger(&nSave) == 0)
    {
        return -2001;
    }
    if (nSave < 0 || nSave >= 4)
    {
        nSave = 0;
    }
    if (g_pVirtualMachine->StackPopInteger(&nResult2) == 0)
    {
        return -2001;
    }
    if (nArgs > 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SAVING_THROW_DECREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nResult2);
    pGameEffect->SetInteger(1, nSave);
    pGameEffect->SetInteger(2, nResult);
    pGameEffect->SetInteger(3, (unsigned char)g_pRules->m_nNumRaces);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B6050
int CSWVirtualMachineCommands::ExecuteCommandEffectSavingThrowIncrease(
    int nCommand, int nArgs)
{
    int nResult;
    int nSave;
    int nResult2;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    nResult = 0;
    if (g_pVirtualMachine->StackPopInteger(&nSave) == 0)
    {
        return -2001;
    }
    if (nSave < 0 || nSave >= 4)
    {
        nSave = 0;
    }
    if (g_pVirtualMachine->StackPopInteger(&nResult2) == 0)
    {
        return -2001;
    }
    if (nArgs > 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SAVING_THROW_INCREASE;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nResult2);
    pGameEffect->SetInteger(1, nSave);
    pGameEffect->SetInteger(2, nResult);
    pGameEffect->SetInteger(3, (unsigned char)g_pRules->m_nNumRaces);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B6200
int CSWVirtualMachineCommands::ExecuteCommandEffectSeeInvisible(int nCommand,
                                                                int nArgs)
{
    CGameEffect* pGameEffect;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SEEINVISIBLE;
    pGameEffect->SetCreator(m_oidObject);
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B62D0
int CSWVirtualMachineCommands::ExecuteCommandEffectSkillDecrease(int nCommand,
                                                                 int nArgs)
{
    int nSkill;
    int nValue;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nSkill) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
        {
            return -2001;
        }
        pGameEffect = new CGameEffect(1);
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SKILL_DECREASE;
        pGameEffect->m_nSubType =
            (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
            SUBTYPE_MAGICAL;
        if (nSkill > 0 && nSkill < (unsigned char)g_pRules->m_nNumSkills ||
            nSkill == 255)
        {
            pGameEffect->SetInteger(0, nSkill);
            pGameEffect->SetInteger(1, nValue);
            pGameEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
        }
        else
        {
            pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                    ->GetGameObject(m_oidObject, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject != NULL)
        {
            pGameEffect->SetCreator(m_oidObject);
        }
        return g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) != 0
                   ? 0
                   : -2000;
    }
}
// PC 0x005B6440
int CSWVirtualMachineCommands::ExecuteCommandEffectSkillIncrease(int nCommand,
                                                                 int nArgs)
{
    int nSkill;
    int nValue;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nSkill) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
        {
            return -2001;
        }
        pGameEffect = new CGameEffect(1);
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SKILL_INCREASE;
        pGameEffect->m_nSubType =
            (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
            SUBTYPE_MAGICAL;
        if (nSkill > 0 && nSkill < (unsigned char)g_pRules->m_nNumSkills ||
            nSkill == 255)
        {
            pGameEffect->SetInteger(0, nSkill);
            pGameEffect->SetInteger(1, nValue);
            pGameEffect->SetInteger(2, (unsigned char)g_pRules->m_nNumRaces);
        }
        else
        {
            pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                    ->GetGameObject(m_oidObject, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject != NULL)
        {
            pGameEffect->SetCreator(m_oidObject);
        }
        return g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) != 0
                   ? 0
                   : -2000;
    }
}
// PC 0x005B65B0
int CSWVirtualMachineCommands::ExecuteCommandEffectSpellImmunity(int nCommand,
                                                                 int nArgs)
{
    int nSpell;
    CGameEffect* pEffect;

    if (g_pVirtualMachine->StackPopInteger(&nSpell) == 0)
    {
        return -2001;
    }
    pEffect = new CGameEffect(1);
    if (nSpell < -1 || nSpell > g_pRules->m_pSpellArray->m_nNumSpells)
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    else
    {
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_SPELL_IMMUNITY;
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
            SUBTYPE_MAGICAL;
        pEffect->SetCreator(m_oidObject);
        pEffect->SetInteger(0, nSpell);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005B66E0
int CSWVirtualMachineCommands::ExecuteCommandEffectSpellLevelAbsorption(
    int nCommand, int nArgs)
{
    int nMaxSpellLevelAbsorbed;
    int nTotalSpellLevelsAbsorbed;
    int nSpellSchool;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nMaxSpellLevelAbsorbed) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nTotalSpellLevelsAbsorbed) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nSpellSchool) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    if (nMaxSpellLevelAbsorbed > 9 || nMaxSpellLevelAbsorbed < -1)
    {
        pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    else
    {
        pGameEffect2->m_nType =
            CSWSEffectListHandler::EFFECT_SPELL_LEVEL_ABSORPTION;
        pGameEffect2->m_nSubType =
            (pGameEffect2->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
            SUBTYPE_MAGICAL;
        pGameEffect2->SetCreator(m_oidObject);
        pGameEffect2->SetInteger(0, nMaxSpellLevelAbsorbed);
        pGameEffect2->SetInteger(1, nTotalSpellLevelsAbsorbed);
        pGameEffect2->SetInteger(2, nSpellSchool);
    }
    if (nTotalSpellLevelsAbsorbed == 0)
    {
        pGameEffect2->SetInteger(3, 1);
    }
    else
    {
        pGameEffect2->SetInteger(3, 0);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B6860
int CSWVirtualMachineCommands::ExecuteCommandEffectForceResistanceDecrease(
    int nCommand, int nArgs)
{
    int nValue;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    pGameEffect2->m_nType =
        CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_DECREASE;
    pGameEffect2->SetCreator(m_oidObject);
    pGameEffect2->SetInteger(0, nValue);
    pGameEffect2->m_nSubType =
        (pGameEffect2->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B6970
int CSWVirtualMachineCommands::ExecuteCommandEffectForceResistanceIncrease(
    int nCommand, int nArgs)
{
    int nValue;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    pGameEffect2->m_nType =
        CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_INCREASE;
    pGameEffect2->SetCreator(m_oidObject);
    pGameEffect2->SetInteger(0, nValue);
    pGameEffect2->m_nSubType =
        (pGameEffect2->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B6A80
int CSWVirtualMachineCommands::ExecuteCommandEffectTemporaryFP(int nCommand,
                                                               int nArgs)
{
    int nTempForce;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nTempForce) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    if (nTempForce > 0)
    {
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_TEMPORARY_FORCE;
        pGameEffect->SetInteger(0, nTempForce);
    }
    else
    {
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != 0)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B6BD0
int CSWVirtualMachineCommands::ExecuteCommandEffectTemporaryHP(int nCommand,
                                                               int nArgs)
{
    int nHitPoints;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nHitPoints) == 0)
    {
        return -2001;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    if (nHitPoints > 0)
    {
        pGameEffect->m_nType =
            CSWSEffectListHandler::EFFECT_TEMPORARY_HITPOINTS;
        pGameEffect->SetInteger(0, nHitPoints);
    }
    else
    {
        pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != 0)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B6D20
int CSWVirtualMachineCommands::ExecuteCommandEffectTrueSeeing(int nCommand,
                                                              int nArgs)
{
    CGameEffect* pGameEffect;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_TRUESEEING;
    pGameEffect->SetCreator(m_oidObject);
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B6DF0
int CSWVirtualMachineCommands::ExecuteCommandEffectVisualEffect(int nCommand,
                                                                int nArgs)
{
    int nResult;
    int nResult2;
    CGameEffect* pGameEffect;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    nResult = 0;
    if (g_pVirtualMachine->StackPopInteger(&nResult2) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    if (nResult2 == VISUAL_EFFECT_FRAME_BUFFER &&
        (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nFrameBuffer == 0 ||
         !AurCheckFrameBufferModificationsAvailable()))
    {
        nResult2 = VISUAL_EFFECT_FRAME_BUFFER_FALLBACK;
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
    pGameEffect->m_nSubType =
        (pGameEffect->m_nSubType & ~CGameEffect::SUBTYPE_MASK) |
        SUBTYPE_MAGICAL;
    pGameEffect->SetInteger(0, nResult2);
    pGameEffect->SetInteger(2, nResult);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pGameEffect->SetCreator(m_oidObject);
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B6F80
int CSWVirtualMachineCommands::ExecuteCommandEffectDroidStun(int nCommand,
                                                             int nArgs)
{
    CGameEffect* pGameEffect;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
    pGameEffect->SetInteger(0, 3);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B7030
int CSWVirtualMachineCommands::ExecuteCommandEffectForceJump(int, int nArgs)
{
    unsigned long oidTarget = OBJECT_INVALID;
    int bAdvanced;
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
        return ERROR_STACK_POP;
    bAdvanced = 0;
    if (nArgs >= 2 && g_pVirtualMachine->StackPopInteger(&bAdvanced) == 0)
        return ERROR_STACK_POP;

    CGameEffect* pEffect = new CGameEffect(1);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEJUMP;
    pEffect->SetObjectID(0, oidTarget);
    pEffect->SetInteger(0, bAdvanced);
    pEffect->SetCreator(m_oidObject);
    if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_EFFECT, pEffect) == 0)
        return ERROR_STACK_PUSH;
    delete pEffect;
    return 0;
}
// PC 0x005B7160
int CSWVirtualMachineCommands::ExecuteCommandEffectForcePushed(int nCommand,
                                                               int nArgs)
{
    CGameEffect* pGameEffect;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEPUSHED;
    pGameEffect->SetCreator(m_oidObject);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B7220
int CSWVirtualMachineCommands::ExecuteCommandEffectForcePushTargeted(
    int nCommand, int nArgs)
{
    CScriptLocation* pLocation;
    int nForce;
    CGameEffect* pEffect;

    pLocation = NULL;
    nForce = 0;
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION, (void**)&pLocation) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nForce) == 0)
    {
        return -2001;
    }
    pEffect = new CGameEffect(1);
    pEffect->m_nType = CSWSEffectListHandler::EFFECT_FORCEPUSHED;
    pEffect->SetCreator(m_oidObject);
    pEffect->SetInteger(0, 1);
    pEffect->SetInteger(1, nForce);
    pEffect->SetFloat(0, pLocation->m_vPosition.m_fX);
    pEffect->SetFloat(1, pLocation->m_vPosition.m_fY);
    pEffect->SetFloat(2, pLocation->m_vPosition.m_fZ);
    delete pLocation;
    pLocation = NULL;
    if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005B7390
int CSWVirtualMachineCommands::ExecuteCommandEffectWhirlwind(int nCommand,
                                                             int nArgs)
{
    CGameEffect* pGameEffect;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
    pGameEffect->SetInteger(0, 10);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B7440
int CSWVirtualMachineCommands::ExecuteCommandEffectBlasterDeflectionIncrease(
    int nCommand, int nArgs)
{
    int nChange;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nChange) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    pGameEffect2->m_nType =
        CSWSEffectListHandler::EFFECT_BLASTERDEFLECTIONINCREASE;
    pGameEffect2->SetInteger(0, nChange);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B7520
int CSWVirtualMachineCommands::ExecuteCommandEffectBlasterDeflectionDecrease(
    int nCommand, int nArgs)
{
    int nChange;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nChange) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    pGameEffect2->m_nType =
        CSWSEffectListHandler::EFFECT_BLASTERDEFLECTIONDECREASE;
    pGameEffect2->SetInteger(0, nChange);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B7600
int CSWVirtualMachineCommands::ExecuteCommandEffectHorrified(int nCommand,
                                                             int nArgs)
{
    CGameEffect* pGameEffect;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
    pGameEffect->SetInteger(0, 8);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B76B0
int CSWVirtualMachineCommands::ExecuteCommandEffectDamageForcePoints(
    int nCommand, int nArgs)
{
    int nDamage;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nDamage) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_DAMAGEFORCEPOINTS;
    pGameEffect2->SetInteger(0, nDamage);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B7790
int CSWVirtualMachineCommands::ExecuteCommandEffectHealForcePoints(int nCommand,
                                                                   int nArgs)
{
    int nHeal;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nHeal) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_HEALFORCEPOINTS;
    pGameEffect2->SetInteger(0, nHeal);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B7870
int CSWVirtualMachineCommands::ExecuteCommandEffectTemporaryForcePoints(
    int nCommand, int nArgs)
{
    int nTempForce;
    CGameEffect* pGameEffect2;

    if (g_pVirtualMachine->StackPopInteger(&nTempForce) == 0)
    {
        return -2001;
    }
    pGameEffect2 = new CGameEffect(1);
    pGameEffect2->m_nType = CSWSEffectListHandler::EFFECT_TEMPORARY_FORCE;
    pGameEffect2->SetInteger(0, nTempForce);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect2) == 0)
    {
        return -2000;
    }
    delete pGameEffect2;
    return 0;
}
// PC 0x005B7950
int CSWVirtualMachineCommands::ExecuteCommandEffectChoke(int nCommand,
                                                         int nArgs)
{
    CGameEffect* pGameEffect;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_SETSTATE;
    pGameEffect->SetInteger(0, 7);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B7A00
int CSWVirtualMachineCommands::ExecuteCommandEffectBodyFuel(int nCommand,
                                                            int nArgs)
{
    CGameEffect* pGameEffect;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_BODYFUEL;
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B7AB0
int CSWVirtualMachineCommands::ExecuteCommandEffectPsychicStatic(int nCommand,
                                                                 int nArgs)
{
    CGameEffect* pGameEffect;

    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_PSYCHICSTATIC;
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B7B60
int CSWVirtualMachineCommands::ExecuteCommandEffectLightsaberThrow(int nCommand,
                                                                   int nArgs)
{
    unsigned long oidResult;
    unsigned long oidResult2;
    unsigned long oidResult3;
    int nResult;
    CGameEffect* pGameEffect;

    oidResult = OBJECT_INVALID;
    oidResult2 = OBJECT_INVALID;
    oidResult3 = OBJECT_INVALID;
    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopObject(&oidResult2) == 0)
        {
            return -2001;
        }
    }
    if (nArgs >= 3)
    {
        if (g_pVirtualMachine->StackPopObject(&oidResult3) == 0)
        {
            return -2001;
        }
    }
    if (nArgs >= 4)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    pGameEffect = new CGameEffect(1);
    pGameEffect->m_nType = CSWSEffectListHandler::EFFECT_LIGHTSABERTHROW;
    pGameEffect->SetObjectID(0, oidResult);
    pGameEffect->SetObjectID(1, oidResult2);
    pGameEffect->SetObjectID(2, oidResult3);
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return -2000;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005B7CE0
int CSWVirtualMachineCommands::ExecuteCommandEndGame(int nCommand, int nArgs)
{
    int bEndGame = 1;

    if (nArgs >= 1 && g_pVirtualMachine->StackPopInteger(&bEndGame) == 0)
    {
        return -2001;
    }
    g_pAppManager->m_pClientExoApp->SetEndGamePending(
        1, bEndGame != 0 ? 60.0f : 0.0f, bEndGame);
    return 0;
}
// PC 0x005B7D50
int CSWVirtualMachineCommands::ExecuteCommandEquipItem(int nCommand, int nArgs)
{
    unsigned long oidItem;
    int nInventorySlot;
    int bInstant;
    unsigned long oidCaller;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CGameObject* pItemObject;
    CSWSCreature* pCreature;
    CSWSItem* pItem;

    oidCaller = m_oidObject;
    bInstant = 0;
    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nInventorySlot) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 3)
    {
        if (g_pVirtualMachine->StackPopInteger(&bInstant) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    if (nInventorySlot >= 0 && nInventorySlot < 20)
    {
        nInventorySlot = (int)pow(2.0, nInventorySlot);
        if (m_nCommandContext == 1)
        {
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            if (pServerExoApp->GetObjectArray()->GetGameObject(
                    oidCaller, &pGameObject) == CGameObjectArray::SUCCESS)
            {
                pCreature = pGameObject->AsSWSCreature();
                if (pCreature != NULL)
                {
                    pServerExoApp = g_pAppManager->m_pServerExoApp;
                    if (pServerExoApp->GetObjectArray()->GetGameObject(
                            oidItem, &pItemObject) ==
                        CGameObjectArray::SUCCESS)
                    {
                        pItem = pItemObject->AsSWSItem();
                        if (pItem != NULL)
                        {
                            pCreature->AddEquipItemActions(
                                pItem, nInventorySlot, 0, 0, bInstant, 0, 0);
                        }
                    }
                }
            }
        }
    }
    return 0;
}
// PC 0x005B7E80
int CSWVirtualMachineCommands::ExecuteCommandEventActivateItem(int nCommand,
                                                               int nArgs)
{

    CScriptEvent* pEvent = new CScriptEvent;
    unsigned long oidItem;
    unsigned long oidTarget;
    unsigned long oidArea = OBJECT_INVALID;
    CScriptLocation* pLocation = NULL;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0 ||
        g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                 (void**)&pLocation) == 0 ||
        g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        delete pEvent;
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSItem* pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        oidArea = pItem->m_oidArea;
    }
    pEvent->m_nType = CScriptEvent::EVENT_ACTIVATE_ITEM;
    pEvent->SetObjectID(0, oidItem);
    pEvent->SetObjectID(1, m_oidObject);
    pEvent->SetObjectID(2, oidArea);
    pEvent->SetObjectID(3, oidTarget);
    pEvent->SetFloat(0, pLocation->m_vPosition.m_fX);
    pEvent->SetFloat(1, pLocation->m_vPosition.m_fY);
    pEvent->SetFloat(2, pLocation->m_vPosition.m_fZ);
    if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_EVENT, pEvent) == 0)
    {
        delete pEvent;
        return ERROR_STACK_PUSH;
    }
    delete pEvent;
    if (pLocation != NULL)
    {
        delete pLocation;
    }
    return 0;
}
// PC 0x005B8050
int CSWVirtualMachineCommands::ExecuteCommandEventConversation(int nCommand,
                                                               int nArgs)
{
    CScriptEvent* pEvent = new CScriptEvent;
    pEvent->m_nType = 7;
    if (g_pVirtualMachine->StackPushEngineStructure(1, pEvent) == 0)
    {
        return -2000;
    }
    delete pEvent;
    return 0;
}
// PC 0x005B80F0
int CSWVirtualMachineCommands::ExecuteCommandEventSpellCastAt(int nCommand,
                                                              int nArgs)
{
    CScriptEvent* pEvent = new CScriptEvent;
    unsigned long oidObject;
    int nSpell;
    int nEventType = 1;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        delete pEvent;
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nSpell) == 0)
    {
        delete pEvent;
        return -2001;
    }
    if (nArgs > 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nEventType) == 0)
        {
            delete pEvent;
            return -2001;
        }
    }
    pEvent->m_nType = 2;
    pEvent->SetInteger(0, nSpell);
    pEvent->SetInteger(1, nEventType);
    pEvent->SetObjectID(0, oidObject);
    if (g_pVirtualMachine->StackPushEngineStructure(1, pEvent) == 0)
    {
        delete pEvent;
        return -2000;
    }
    delete pEvent;
    return 0;
}
// PC 0x005B8240
int CSWVirtualMachineCommands::ExecuteCommandEventUserDefined(int nCommand,
                                                              int nArgs)
{
    CScriptEvent* pEvent = new CScriptEvent;
    int nEvent;

    if (g_pVirtualMachine->StackPopInteger(&nEvent) == 0)
    {
        delete pEvent;
        return -2001;
    }
    pEvent->m_nType = 11;
    pEvent->SetInteger(0, nEvent);
    if (g_pVirtualMachine->StackPushEngineStructure(1, pEvent) == 0)
    {
        delete pEvent;
        return -2000;
    }
    delete pEvent;
    return 0;
}
// PC 0x005B8340
int CSWVirtualMachineCommands::ExecuteCommandExecuteScript(int nCommand,
                                                           int nArgs)
{
    CExoString sScript;
    unsigned long oidObject;

    if (g_pVirtualMachine->StackPopString(&sScript) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    if (nArgs >= 3)
    {
        int nRunScriptVar = -1;
        if (g_pVirtualMachine->StackPopInteger(&nRunScriptVar) == 0)
        {
            return -2001;
        }
        g_nRunScriptVar = nRunScriptVar;
    }
    if (m_nCommandContext == 1)
    {
        g_pVirtualMachine->RunScript(&sScript, oidObject, 1);
    }
    else
    {
        g_pVirtualMachine->RunScript(&sScript, OBJECT_INVALID, 0);
    }
    return 0;
}
// PC 0x005B8440
int CSWVirtualMachineCommands::ExecuteCommandExportAllCharacters(int nCommand,
                                                                 int nArgs)
{
    CServerExoApp* pServerExoApp;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->SetExportAllCharactersPending(1);
    return 0;
}
// PC 0x005B8460
int CSWVirtualMachineCommands::ExecuteCommandFindSubString(int nCommand,
                                                           int nArgs)
{
    CExoString sString;
    CExoString sSubString;
    int nResult;

    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopString(&sSubString) == 0)
    {
        return -2001;
    }
    nResult = sString.Find(sSubString, 0);
    if (g_pVirtualMachine->StackPushInteger(nResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005CE2D0
int CSWVirtualMachineCommands::ExecuteCommandAddJournalQuestEntry(
    int nCommand, int nArgs)
{

    CExoString szPlotID;
    int nState;
    int bAllowOverrideHigher = 0;
    if (g_pVirtualMachine->StackPopString(&szPlotID) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nState) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 3)
    {
        if (g_pVirtualMachine->StackPopInteger(&bAllowOverrideHigher) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    if (nState < 0)
    {
        nState = abs(nState);
    }
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetWorldTimer()->GetWorldTime(&nCalendarDay, &nTimeOfDay);
    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayerList == NULL)
    {
        return 1;
    }
    int nExperience = 0;
    CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
    while (pPosition != NULL)
    {
        CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
        if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
        {
            CSWSCreature* pCreature =
                pPlayer->GetGameObject()->AsSWSCreature();
            if (pCreature != NULL)
            {
                if (bAllowOverrideHigher ||
                    pCreature->GetJournal()->GetState(szPlotID) < nState)
                {
                    pCreature->GetJournal()->SetDate(szPlotID, nCalendarDay, 0);
                    pCreature->GetJournal()->SetTime(szPlotID, nTimeOfDay, 0);
                    pCreature->GetJournal()->SetState(szPlotID, nState, 0);
                    nExperience = pCreature->GetJournal()->GetQuestEntryXP(szPlotID);
                }
            }
        }
        pPlayerList->GetNext(pPosition);
    }
    if (nExperience != 0 &&
        g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
    {
        g_pAppManager->m_pServerExoApp->GetPartyTable()->DistributeExperience(
            nExperience, 1);
        g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateStatus(
            GUI_STATUS_XP, nExperience);
    }
    return 0;
}
// PC 0x005CE540
int CSWVirtualMachineCommands::ExecuteCommandAddJournalWorldEntry(
    int nCommand, int nArgs)
{

    CExoString szEntry;
    CExoString szTitle;
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    int nIndex;
    if (g_pVirtualMachine->StackPopInteger(&nIndex) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopString(&szEntry) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopString(&szTitle) == 0)
    {
        return ERROR_STACK_POP;
    }

    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetWorldTimer()->GetWorldTime(&nCalendarDay, &nTimeOfDay);
    if (pModule != NULL)
    {
        pModule->AddWorldJournalEntry(nIndex, szEntry, szTitle,
                                      nCalendarDay, nTimeOfDay);
    }

    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayerList != NULL)
    {
        CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
        while (pPosition != NULL)
        {
            CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
            if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
                pMessage != NULL)
            {
                pMessage->SendServerToPlayerJournalAddWorld(
                    pPlayer, nIndex, szEntry, szTitle, nCalendarDay, nTimeOfDay);
            }
            pPlayerList->GetNext(pPosition);
        }
    }
    return 0;
}
// PC 0x005CE6F0
int CSWVirtualMachineCommands::ExecuteCommandAddJournalWorldEntryStrref(
    int nCommand, int nArgs)
{

    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    int nStrRef;
    int nStrRefTitle;
    if (g_pVirtualMachine->StackPopInteger(&nStrRef) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nStrRefTitle) == 0)
    {
        return ERROR_STACK_POP;
    }

    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetWorldTimer()->GetWorldTime(&nCalendarDay, &nTimeOfDay);
    if (pModule != NULL)
    {
        pModule->AddWorldJournalEntryStrref(nStrRef, nStrRefTitle,
                                           nCalendarDay, nTimeOfDay);
    }

    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayerList != NULL)
    {
        CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
        while (pPosition != NULL)
        {
            CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
            if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
                pMessage != NULL)
            {
                pMessage->SendServerToPlayerJournalAddWorldStrref(
                    pPlayer, nStrRef, nStrRefTitle, nCalendarDay, nTimeOfDay);
            }
            pPlayerList->GetNext(pPosition);
        }
    }
    return 0;
}
// PC 0x005CE800
int CSWVirtualMachineCommands::ExecuteCommandBarkString(int nCommand, int nArgs)
{
    NWVMACHINECOMMANDS_TRACE();

    unsigned long oidCreature;
    int nStrRef;
    int nBarkX;
    int nBarkY;
    CClientExoApp* pClientExoApp;
    int nDialogState;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    if (g_pVirtualMachine->StackPopInteger(&nStrRef) == 0)
    {
        return ERROR_STACK_POP;
    }
    nBarkX = -1;
    nBarkY = -1;
    if (nArgs >= 3 &&
        (g_pVirtualMachine->StackPopInteger(&nBarkX) == 0 ||
         g_pVirtualMachine->StackPopInteger(&nBarkY) == 0))
    {
        return ERROR_STACK_POP;
    }
    CSWGuiBarkBubble* pBarkBubble =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pBarkBubble;
    if (pBarkBubble != NULL)
    {
        int nY = nBarkY;
        int nX = nBarkX;
        pBarkBubble->m_nBarkX = nX;
        pBarkBubble->m_nBarkY = nY;
    }
    nDialogState = g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nGlobalDialogState;
    if (nDialogState == 0)
    {
        STR_RES cResult;
        g_pTlkTable->Fetch(nStrRef, cResult, CTlkTable::GENDER_MALE);
        CResRef resSound(cResult.m_resSound);
        CExoString sBarkString(cResult.m_sString);
        if (oidCreature == OBJECT_INVALID)
        {
            pClientExoApp = g_pAppManager->m_pClientExoApp;
            pClientExoApp->GetInGameGui()->ShowBarkBubble(
                OBJECT_INVALID, sBarkString, resSound, 0);
        }
        else
        {
            pClientExoApp = g_pAppManager->m_pClientExoApp;
            oidCreature = pClientExoApp->ServerToClientObjectId(oidCreature);
            pClientExoApp = g_pAppManager->m_pClientExoApp;
            pClientExoApp->BarkString(oidCreature, sBarkString, resSound, 0);
        }
    }
    return 0;
}
// PC 0x005CE9F0
int CSWVirtualMachineCommands::ExecuteCommandDeleteJournalWorldAllEntries(
    int nCommand, int nArgs)
{
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        pModule->DeleteWorldJournalAllEntries();
    }

    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayerList != NULL)
    {
        CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
        while (pPosition != NULL)
        {
            CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
            if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
                pMessage != NULL)
            {
                pMessage->SendServerToPlayerJournalDeleteWorldAll(pPlayer);
            }
            pPlayerList->GetNext(pPosition);
        }
    }
    return 0;
}
// PC 0x005CEA90
int CSWVirtualMachineCommands::ExecuteCommandDeleteJournalWorldEntry(
    int nCommand, int nArgs)
{
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    int nIndex;
    if (g_pVirtualMachine->StackPopInteger(&nIndex) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (pModule != NULL)
    {
        pModule->DeleteWorldJournalEntry(nIndex);
    }

    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayerList != NULL)
    {
        CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
        while (pPosition != NULL)
        {
            CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
            if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
                pMessage != NULL)
            {
                pMessage->SendServerToPlayerJournalDeleteWorld(pPlayer, nIndex);
            }
            pPlayerList->GetNext(pPosition);
        }
    }
    return 0;
}
// PC 0x005CEB50
int CSWVirtualMachineCommands::ExecuteCommandDeleteJournalWorldEntryStrref(
    int nCommand, int nArgs)
{
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    int nStrRef;
    if (g_pVirtualMachine->StackPopInteger(&nStrRef) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (pModule != NULL)
    {
        pModule->DeleteWorldJournalEntryStrref(nStrRef);
    }

    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayerList != NULL)
    {
        CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
        while (pPosition != NULL)
        {
            CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
            if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
                pMessage != NULL)
            {
                pMessage->SendServerToPlayerJournalDeleteWorldStrref(pPlayer,
                                                                    nStrRef);
            }
            pPlayerList->GetNext(pPosition);
        }
    }
    return 0;
}
// PC 0x005CEC10
int CSWVirtualMachineCommands::ExecuteCommandDoTouchAttack(int nCommand, int nArgs)
{

    int nResult = CSWCCMessageData::TOUCH_ATTACK_MISS;
    int bFeedback = 1;
    unsigned char nAttackResult = 0;
    unsigned char nConcealment = 0;
    unsigned long oidTarget;
    CGameObject* pTarget;
    CGameObject* pAttacker;
    int nAttackBonus;
    int nRoll;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        if (nArgs >= 2)
        {
            if (g_pVirtualMachine->StackPopInteger(&bFeedback) == 0)
                return ERROR_STACK_POP;
        }
        CGameObjectArray* pObjectArray = g_pAppManager->GetServerExoApp()->GetObjectArray();
        if (pObjectArray->GetGameObject(oidTarget, &pTarget) == CGameObjectArray::SUCCESS &&
            pObjectArray->GetGameObject(m_oidObject, &pAttacker) == CGameObjectArray::SUCCESS)
        {
            if (pAttacker->AsSWSCreature() != NULL)
            {
                if (pTarget->AsSWSCreature() != NULL)
                {
                    if (nCommand == COMMAND_TOUCH_ATTACK_MELEE)
                        nAttackBonus = pAttacker->AsSWSCreature()->GetStats()->GetMeleeAttackBonus(0, 1, 1, 0);
                    else
                        nAttackBonus = pAttacker->AsSWSCreature()->GetStats()->GetRangedAttackBonus(0, 1, 1);
                    int nArmorClass = pTarget->AsSWSCreature()->GetStats()->GetArmorClassVersus(pAttacker->AsSWSCreature(), 1);
                    nRoll = g_pRules->RollDice(1, 20);
                    if (pAttacker->AsSWSCreature()->ResolveDefensiveEffects(pTarget->AsSWSObject(), 1))
                    {
                        nResult = CSWCCMessageData::TOUCH_ATTACK_MISS;
                        CSWSCombatRound* pCurrentRound = pAttacker->AsSWSCreature()->m_pcCombatRound;
                        int nCurrentAttack = pCurrentRound->m_nCurrentAttack;
                        CSWSCombatRound* pCombatRound = pAttacker->AsSWSCreature()->m_pcCombatRound;
                        CSWSCombatAttackData* pAttack = pCombatRound->GetAttack(nCurrentAttack);
                        nAttackResult = pAttack->m_nAttackResult;
                        nConcealment = pAttack->m_nConcealment;
                    }
                    else if (nRoll != CSWRules::ATTACK_ROLL_AUTOMATIC_MISS && (nAttackBonus + nRoll >= nArmorClass || nRoll == CSWRules::ATTACK_ROLL_AUTOMATIC_HIT))
                    {
                        if (nRoll == CSWRules::ATTACK_ROLL_CRITICAL_THREAT)
                        {
                            int nCriticalRoll = g_pRules->RollDice(1, 20);
                            if (nAttackBonus + nCriticalRoll >= nArmorClass)
                                nResult = CSWCCMessageData::TOUCH_ATTACK_CRITICAL;
                            else
                                nResult = CSWCCMessageData::TOUCH_ATTACK_HIT;
                        }
                        else
                            nResult = CSWCCMessageData::TOUCH_ATTACK_HIT;
                    }
                    else
                        nResult = CSWCCMessageData::TOUCH_ATTACK_MISS;
                }
                else if (pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR ||
                         pTarget->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                    nResult = CSWCCMessageData::TOUCH_ATTACK_HIT;
                else
                    nResult = CSWCCMessageData::TOUCH_ATTACK_MISS;
            }
            else
                nResult = CSWCCMessageData::TOUCH_ATTACK_MISS;
        }
        else
            bFeedback = 0;
        if (bFeedback == 1)
        {
            CSWCCMessageData* pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(CSWCCMessageData::TOUCH_ATTACK_ATTACKER, m_oidObject);
            pMessageData->SetObjectID(CSWCCMessageData::TOUCH_ATTACK_TARGET, pTarget->GetId());
            pMessageData->SetInteger(CSWCCMessageData::TOUCH_ATTACK_BONUS, nAttackBonus);
            pMessageData->SetInteger(CSWCCMessageData::TOUCH_ATTACK_ROLL, nRoll);
            if (nResult == CSWCCMessageData::TOUCH_ATTACK_MISS)
            {
                if (nAttackResult == 0)
                    pMessageData->SetInteger(CSWCCMessageData::TOUCH_ATTACK_RESULT, CSWCCMessageData::TOUCH_ATTACK_MESSAGE_MISS);
                else
                {
                    pMessageData->SetInteger(CSWCCMessageData::TOUCH_ATTACK_RESULT, nAttackResult);
                    pMessageData->SetInteger(CSWCCMessageData::TOUCH_ATTACK_CONCEALMENT, nConcealment);
                }
            }
            else if (nResult == CSWCCMessageData::TOUCH_ATTACK_HIT)
                pMessageData->SetInteger(CSWCCMessageData::TOUCH_ATTACK_RESULT, CSWCCMessageData::TOUCH_ATTACK_HIT);
            else if (nResult == CSWCCMessageData::TOUCH_ATTACK_CRITICAL)
                pMessageData->SetInteger(CSWCCMessageData::TOUCH_ATTACK_RESULT, CSWCCMessageData::TOUCH_ATTACK_CRITICAL);
            if (nCommand == COMMAND_TOUCH_ATTACK_RANGED)
                pMessageData->SetInteger(CSWCCMessageData::TOUCH_ATTACK_RANGED, 1);
            CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(m_oidObject);
            if (pPlayer != NULL)
                g_pAppManager->GetServerExoApp()->GetSWSMessage()->SendServerToPlayerCCMessage(
                    pPlayer->GetPlayerId(), CSWSMessage::CC_MESSAGE_TOUCH_ATTACK, pMessageData, NULL);
            pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(pTarget->GetId());
            if (pPlayer != NULL)
                g_pAppManager->GetServerExoApp()->GetSWSMessage()->SendServerToPlayerCCMessage(
                    pPlayer->GetPlayerId(), CSWSMessage::CC_MESSAGE_TOUCH_ATTACK, pMessageData, NULL);
            delete pMessageData;
        }
        return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : ERROR_STACK_PUSH;
    }
}
// PC 0x005B8580
int CSWVirtualMachineCommands::ExecuteCommandFloatingText(int nCommand, int nArgs)
{

    CExoString sStringToDisplay;
    int nStrRefToDisplay;
    int nValue;
    unsigned long oidCreatureToFloatAbove;
    int bBroadcastToFaction = TRUE;

    if (nCommand == COMMAND_FLOATING_TEXT_STR_REF_ON_CREATURE)
    {
        if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
            return ERROR_STACK_POP;
        nStrRefToDisplay = nValue;
    }
    else if (nCommand == COMMAND_FLOATING_TEXT_STRING_ON_CREATURE)
    {
        if (g_pVirtualMachine->StackPopString(&sStringToDisplay) == 0)
            return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidCreatureToFloatAbove) == 0)
        return ERROR_STACK_POP;
    if (nArgs >= 3 &&
        g_pVirtualMachine->StackPopInteger(&bBroadcastToFaction) == 0)
        return ERROR_STACK_POP;

    CGameObject* pGameObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidCreatureToFloatAbove);
    CSWSObject* pObject = NULL;
    if (pGameObject != NULL &&
        pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        pObject = pGameObject->AsSWSObject();
    }
    if (pObject != NULL)
    {
        CSWCCMessageData* pMessageData = new CSWCCMessageData;
        if (pMessageData != NULL)
        {
            pMessageData->SetObjectID(CSWCCMessageData::FLOATING_TEXT_CREATURE,
                                      oidCreatureToFloatAbove);
            if (nCommand == COMMAND_FLOATING_TEXT_STR_REF_ON_CREATURE)
            {
                pMessageData->SetInteger(CSWCCMessageData::FLOATING_TEXT_FEEDBACK,
                                         CSWSCreature::FEEDBACK_FLOATY_DATA);
                pMessageData->SetInteger(CSWCCMessageData::FLOATING_TEXT_STR_REF,
                                         nStrRefToDisplay);
            }
            else if (nCommand == COMMAND_FLOATING_TEXT_STRING_ON_CREATURE)
            {
                pMessageData->SetInteger(CSWCCMessageData::FLOATING_TEXT_FEEDBACK,
                                         CSWSCreature::FEEDBACK_FLOATY_STRING);
                pMessageData->SetString(CSWCCMessageData::FLOATING_TEXT_STRING,
                                        sStringToDisplay);
            }
            if (bBroadcastToFaction == FALSE)
            {
                CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
                    ->GetClientObjectByObjectId(oidCreatureToFloatAbove);
                if (pPlayer != NULL)
                {
                    g_pAppManager->GetServerExoApp()->GetSWSMessage()
                        ->SendServerToPlayerCCMessage(pPlayer->GetPlayerId(),
                            CSWSMessage::CC_MESSAGE_FEEDBACK, pMessageData, NULL);
                }
            }
            delete pMessageData;
        }
    }
    return 0;
}
// PC 0x005B8780
int CSWVirtualMachineCommands::ExecuteCommandFloatToInt(int nCommand, int nArgs)
{
    float fValue;

    if (g_pVirtualMachine->StackPopFloat(&fValue) == 0)
    {
        return -2001;
    }

#ifdef _XBOX
    int nValue = (int)fValue; if (g_pVirtualMachine->StackPushInteger(nValue) == 0) return -2000;
#else
    if (g_pVirtualMachine->StackPushInteger((int)fValue) == 0) { return -2000; }
#endif
    return 0;
}
// PC 0x005B87D0
int CSWVirtualMachineCommands::ExecuteCommandFloatToString(int nCommand, int nArgs)
{
    int nDecimals;
    int nWidth;
    float fFloat;

    if (g_pVirtualMachine->StackPopFloat(&fFloat) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nWidth) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nWidth >= 18)
        nWidth = 18;
    else if (nWidth < 0)
        nWidth = 0;

    if (g_pVirtualMachine->StackPopInteger(&nDecimals) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nDecimals >= 10)
        nDecimals = 9;
    else if (nDecimals < 0)
        nDecimals = 0;

    CExoString sFormat;
    if (nWidth >= 10)
        sFormat.Format("%%%2d.%02df\n", nWidth, nDecimals);
    else
        sFormat.Format("%%%1d.%02df\n", nWidth, nDecimals);

    CExoString sString;
    sString.Format((char*)sFormat.CStr(), fFloat);
    if (g_pVirtualMachine->StackPushString(sString.CStr()) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    else
    {
        return 0;
    }
}
// PC 0x005B8A10
int CSWVirtualMachineCommands::ExecuteCommandGetAbilityScore(
    int nCommand, int nArgs)
{

    unsigned long oidCreature;
    int nAbilityType;
    CGameObject* pGameObject;
    int nAbilityScore = 0;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAbilityType) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidCreature,
            &pGameObject) == CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL)
    {
        if (nAbilityType == CSWSEffectListHandler::EFFECT_ABILITY_STRENGTH)
            nAbilityScore = pGameObject->AsSWSCreature()->m_pStats->GetSTRStat();
        else if (nAbilityType == CSWSEffectListHandler::EFFECT_ABILITY_DEXTERITY)
            nAbilityScore = pGameObject->AsSWSCreature()->m_pStats->GetDEXStat();
        else if (nAbilityType == CSWSEffectListHandler::EFFECT_ABILITY_CONSTITUTION)
            nAbilityScore = pGameObject->AsSWSCreature()->m_pStats->GetCONStat();
        else if (nAbilityType == CSWSEffectListHandler::EFFECT_ABILITY_INTELLIGENCE)
            nAbilityScore = pGameObject->AsSWSCreature()->m_pStats->GetINTStat();
        else if (nAbilityType == CSWSEffectListHandler::EFFECT_ABILITY_WISDOM)
            nAbilityScore = pGameObject->AsSWSCreature()->m_pStats->GetWISStat();
        else if (nAbilityType == CSWSEffectListHandler::EFFECT_ABILITY_CHARISMA)
            nAbilityScore = pGameObject->AsSWSCreature()->m_pStats->GetCHAStat();
    }

    if (g_pVirtualMachine->StackPushInteger(nAbilityScore) != 0)
        return 0;
    return ERROR_STACK_PUSH;
}
// PC 0x005B8B70
int CSWVirtualMachineCommands::ExecuteCommandGetAbilityModifier(
    int nCommand, int nArgs)
{
    int nAbility;
    unsigned long oidCreature;
    int nModifier;

    if (!g_pVirtualMachine->StackPopInteger(&nAbility))
    {
        return ERROR_STACK_POP;
    }
    else
    {
        if (!g_pVirtualMachine->StackPopObject(&oidCreature))
            return ERROR_STACK_POP;

        nModifier = 0;
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature =
            pServerExoApp->GetCreatureByGameObjectID(oidCreature);
        if (pCreature != NULL)
        {
            switch (nAbility)
            {
            case CSWRules::ABILITY_STRENGTH:
                nModifier = pCreature->m_pStats->m_nStrengthModifier;
                break;
            case CSWRules::ABILITY_DEXTERITY:
            {
                CSWSCreatureStats* pStats = pCreature->m_pStats;
                nModifier = pStats->GetDEXMod(0);
                break;
            }
            case CSWRules::ABILITY_CONSTITUTION:
                nModifier = pCreature->m_pStats->m_nConstitutionModifier;
                break;
            case CSWRules::ABILITY_INTELLIGENCE:
                nModifier = pCreature->m_pStats->m_nIntelligenceModifier;
                break;
            case CSWRules::ABILITY_WISDOM:
                nModifier = pCreature->m_pStats->m_nWisdomModifier;
                break;
            case CSWRules::ABILITY_CHARISMA:
                nModifier = pCreature->m_pStats->m_nCharismaModifier;
                break;
            }
        }
        return g_pVirtualMachine->StackPushInteger(nModifier) != 0
            ? 0 : ERROR_STACK_PUSH;
    }
}
// PC 0x005B8C70
int CSWVirtualMachineCommands::ExecuteCommandGetAC(int nCommand, int nArgs)
{
    int nArmorClass = -1;
    int nForFutureUse = 0;
    unsigned long oidObject;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        if (nArgs >= 2)
        {
            if (g_pVirtualMachine->StackPopInteger(&nForFutureUse) == 0)
            {
                return ERROR_STACK_POP;
            }
        }

        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(oidObject, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
        {
            switch (pGameObject->m_nObjectType)
            {
            case SW_OBJECT_TYPE_CREATURE:
                nArmorClass = pGameObject->AsSWSCreature()->GetArmorClass();
                break;
            case SW_OBJECT_TYPE_ITEM:
            case SW_OBJECT_TYPE_PLACEABLE:
            case SW_OBJECT_TYPE_DOOR:
                nArmorClass = 0;
                break;
            }
        }
        return g_pVirtualMachine->StackPushInteger(nArmorClass) != 0 ? 0 : ERROR_STACK_PUSH;
    }
}
// PC 0x005B8D50
int CSWVirtualMachineCommands::ExecuteCommandGetAlignment(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    CGameObject* pGameObject;
    int nAlignment = -1;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidCreature, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        pCreature = pGameObject->AsSWSCreature();
        CSWSCreatureStats* pStats = pCreature->m_pStats;
        switch (nCommand)
        {
        case COMMAND_GET_GOOD_EVIL_VALUE:
            nAlignment = pStats->m_nGoodEvilAlignment;
            break;
        case COMMAND_GET_ALIGNMENT_GOOD_EVIL:
            nAlignment = pStats->GetSimpleAlignmentGoodEvil();
            break;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nAlignment) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005B8E00
int CSWVirtualMachineCommands::ExecuteCommandGetAOEObjectCreator(int nCommand,
                                                                 int nArgs)
{
    unsigned long oidAOE;
    unsigned long oidCreator;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidAOE) == 0)
    {
        return -2001;
    }
    oidCreator = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
            ->GetGameObject(oidAOE, &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        if (pGameObject->AsSWSAreaOfEffectObject() != NULL)
        {
            oidCreator = pGameObject->AsSWSAreaOfEffectObject()->m_oidCreator;
        }
    }
    return g_pVirtualMachine->StackPushObject(oidCreator) != 0 ? 0 : -2000;
}
// PC 0x005B8E90
int CSWVirtualMachineCommands::ExecuteCommandGetAppearanceType(int nCommand,
                                                               int nArgs)
{
    int nAppearance;
    unsigned long oidCreature;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    nAppearance = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        nAppearance = pCreature->m_pStats->m_nAppearance;
    }
    return g_pVirtualMachine->StackPushInteger(nAppearance) != 0 ? 0 : -2000;
}
// PC 0x005B8F00
int CSWVirtualMachineCommands::ExecuteCommandGetArea(int nCommand, int nArgs)
{

    unsigned long oidTarget;
    CGameObject* pGameObject;
    CExoString sString;
    unsigned long oidArea = CGameObjectArray::INVALIDOBJID;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidTarget, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        if (pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
        {
            CSWSArea* pArea = ((CSWSObject*)pGameObject)->GetArea();
            if (pArea != NULL)
            {
                oidArea = pArea->GetId();
            }
        }
        else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREA)
        {
            oidArea = pGameObject->GetId();
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidArea) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005B9010
int CSWVirtualMachineCommands::ExecuteCommandGetAreaUnescapable(int nCommand,
                                                                int nArgs)
{
    CServerExoApp* pServerExoApp;
    unsigned long oidResult;
    CSWSCreature* pCreature;
    CSWSArea* pArea;
    int nResult;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidResult = pServerExoApp->GetPlayerCreatureId();
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    pArea = pCreature->GetArea();
    nResult = pArea->m_bUnescapable;
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005B9060
int CSWVirtualMachineCommands::ExecuteCommandGetAttackTarget(int nCommand,
                                                             int nArgs)
{
    unsigned long oidCreature;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    oidResult = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        oidResult = pCreature->m_oidAttackTarget;
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005B90C0
int CSWVirtualMachineCommands::ExecuteCommandGetAttemptedAttackTarget(
    int nCommand, int nArgs)
{
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != 0)
    {
        oidResult = pCreature->m_oidAttemptedAttackTarget;
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005B9110
int CSWVirtualMachineCommands::ExecuteCommandGetAttemptedSpellTarget(
    int nCommand, int nArgs)
{
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != 0)
    {
        oidResult = pCreature->m_oidAttemptedSpellTarget;
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005B9160
int CSWVirtualMachineCommands::ExecuteCommandGetAttemptedMovementTarget(
    int nCommand, int nArgs)
{
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != 0)
    {
        oidResult = pCreature->m_oidAttemptedMovementTarget;
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005B91B0
int CSWVirtualMachineCommands::ExecuteCommandGetSpellTarget(int nCommand,
                                                            int nArgs)
{
    unsigned long oidCreature;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    oidResult = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        oidResult = pCreature->m_oidSpellTarget;
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005B9210
int CSWVirtualMachineCommands::ExecuteCommandGetBaseItemType(int nCommand,
                                                             int nArgs)
{
    unsigned long oidItem;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    nResult = 256;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        nResult = pItem->m_nBaseItem;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005B9270
int CSWVirtualMachineCommands::ExecuteCommandGetBattleTrack(int nCommand,
                                                            int nArgs)
{
    unsigned long oidArea;
    int nTrack;
    CServerExoApp* pServerExoApp;
    CSWSArea* pArea;

    nTrack = 0;
    if (g_pVirtualMachine->StackPopObject(&oidArea) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pArea = pServerExoApp->GetAreaByGameObjectID(oidArea);
    if (pArea != NULL)
    {
        nTrack = pArea->m_pAmbientSound->m_nBattleMusicTrack;
    }
    return g_pVirtualMachine->StackPushInteger(nTrack) != 0 ? 0 : -2000;
}
// PC 0x005B92E0
int CSWVirtualMachineCommands::ExecuteCommandGetBlockingDoor(int nCommand,
                                                             int nArgs)
{
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != 0)
    {
        oidResult = pCreature->m_oidBlockingDoor;
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005B9330
int CSWVirtualMachineCommands::ExecuteCommandGetCasterLevel(int nCommand,
                                                            int nArgs)
{
    unsigned long oidCreature;
    CGameObject* pGameObject;
    int nCasterLevel = 0;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
            oidCreature, &pGameObject) == CGameObjectArray::SUCCESS)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        CSWSPlaceable* pPlaceable = pGameObject->AsSWSPlaceable();
        int nSpellId;

        if (pCreature != NULL)
        {
            nSpellId = pCreature->m_nLastSpellId;
        }
        else if (pPlaceable == NULL &&
                 pGameObject->AsSWSAreaOfEffectObject() != NULL)
        {
            nSpellId =
                pGameObject->AsSWSAreaOfEffectObject()->GetEffectSpellId();
            unsigned long oidCreator =
                pGameObject->AsSWSAreaOfEffectObject()->m_oidCreator;
            pCreature = g_pAppManager->GetServerExoApp()
                            ->GetCreatureByGameObjectID(oidCreator);
            pPlaceable = g_pAppManager->GetServerExoApp()
                             ->GetPlaceableByGameObjectID(oidCreator);
        }

        if (pCreature != NULL)
        {
            if (pCreature->m_bCasterLevelOverride)
            {
                nCasterLevel = pCreature->m_nCasterLevelOverride;
            }
            else
            {
                int nClass = pCreature->m_c1D4;

                if (nClass == CSWSObject::CASTING_CLASS_INNATE_FF)
                {
                    int nLastSpellId = pCreature->m_nLastSpellId;
                    CSWSpell* pSpell =
                        g_pRules->m_pSpellArray->GetSpell(nLastSpellId);
                    if (pSpell != NULL)
                    {
                        nCasterLevel = pSpell->m_nInnateLevel * 2 - 1;
                        if (nCasterLevel < 10)
                        {
                            nCasterLevel = 10;
                        }
                    }
                }
                else if (nClass == CSWSObject::CASTING_CLASS_INNATE_FE)
                {
                    nCasterLevel =
                        pCreature->m_pStats->GetSpellLikeAbilityCasterLevel(
                            nSpellId);
                }
                else
                {
                    nCasterLevel =
                        pCreature->m_pStats->GetClassLevel(nClass, 1);
                }
            }
        }
        else if (pPlaceable != NULL)
        {
            int nLastSpellId = pPlaceable->m_nLastSpellId;
            CSWSpell* pSpell =
                g_pRules->m_pSpellArray->GetSpell(nLastSpellId);
            if (pSpell != NULL)
            {
                nCasterLevel = pSpell->m_nInnateLevel * 2 - 1;
                if (nCasterLevel < 10)
                {
                    nCasterLevel = 10;
                }
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nCasterLevel) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005B94C0
int CSWVirtualMachineCommands::ExecuteCommandGetChallengeRating(int nCommand,
                                                                int nArgs)
{
    unsigned long oidCreature;
    float fChallengeRating;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWSCreatureStats* pStats;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    fChallengeRating = 0.0f;
    if (pCreature != 0)
    {
        pStats = pCreature->m_pStats;
        if (pStats != 0)
        {
            fChallengeRating = pStats->m_fChallengeRating;
        }
    }
    return g_pVirtualMachine->StackPushFloat(fChallengeRating) != 0 ? 0 : -2000;
}
// PC 0x005B9540
int CSWVirtualMachineCommands::ExecuteCommandGetClassInformation(int nCommand,
                                                                int nArgs)
{

    int nClassPosition;
    unsigned long oidCreature;
    int nResult = 0;
    if (g_pVirtualMachine->StackPopInteger(&nClassPosition) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        if (nCommand == COMMAND_GET_CLASS_BY_POSITION)
        {
            nResult = CLASS_TYPE_INVALID;
            if (nClassPosition >= 1 &&
                nClassPosition <= pCreature->m_pStats->m_nClassCount)
            {
                if (pCreature->m_pStats->GetClass(nClassPosition - 1) <=
                    MAX_SCRIPT_CLASS_TYPE)
                {
                    CSWSCreatureStats* pStats = pCreature->m_pStats;
                    nResult = pStats->GetClass(nClassPosition - 1);
                }
            }
        }
        else if (nCommand == COMMAND_GET_LEVEL_BY_POSITION)
        {
            if (nClassPosition >= 1 &&
                nClassPosition <= pCreature->m_pStats->m_nClassCount)
            {
                nResult = pCreature->m_pStats->GetClassLevel(nClassPosition - 1, 0);
            }
        }
        else if (nCommand == COMMAND_GET_LEVEL_BY_CLASS)
        {
            int nClass = 0;
            while (nClass < CSWSCreatureStats::MAX_CLASSES)
            {
                if (pCreature->m_pStats->GetClass(nClass) == nClassPosition)
                {
                    nResult = pCreature->m_pStats->GetClassLevel(nClass, 0);
                }
                ++nClass;
            }
        }
    }
    if (g_pVirtualMachine->StackPushInteger(nResult) != 0)
    {
        return 0;
    }
    return ERROR_STACK_PUSH;
}
// PC 0x005B9690
int CSWVirtualMachineCommands::ExecuteCommandGetCommandable(int nCommand,
                                                            int nArgs)
{
    unsigned long oidObject;
    CServerExoApp* pServerExoApp;
    int nResult;

    oidObject = m_oidObject;
    nResult = 0;
    if (nArgs >= 1 && g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
            ->GetGameObject(oidObject, (CGameObject**)&nArgs) ==
        CGameObjectArray::SUCCESS)
    {
        nResult = ((CSWSObject*)nArgs)->m_bCommandable;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005B9720
int CSWVirtualMachineCommands::ExecuteCommandGetCreatureHasTalent(int nCommand,
                                                               int nArgs)
{
    CScriptTalent* pTalent;
    unsigned long oidCreature;
    int bHasTalent;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidCreature = OBJECT_INVALID;
    bHasTalent = 0;
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_TALENT,
                                                 (void**)&pTalent) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs > 1 && g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        CSWSCreatureStats* pStats = pCreature->m_pStats;
        bHasTalent = pStats->GetCreatureHasTalent(pTalent->m_nType,
                                                 pTalent->m_nID);
    }
    if (g_pVirtualMachine->StackPushInteger(bHasTalent) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    delete pTalent;
    return 0;
}
// PC 0x005B97E0
int CSWVirtualMachineCommands::ExecuteCommandGetCreatureSize(int nCommand,
                                                             int nArgs)
{
    int nResult;
    unsigned long oidCreature;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        nResult = pCreature->m_nCreatureSize;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005B9840
int CSWVirtualMachineCommands::ExecuteCommandGetCreatureTalent(int nCommand,
                                                               int nArgs)
{
    int nCategory;
    int nCRMax;
    unsigned long oidCreature;
    int nInclusion;
    int nExcludeType;
    int nExcludeId;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    nInclusion = 0;
    nExcludeType = -1;
    nExcludeId = -1;
    oidCreature = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopInteger(&nCategory) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nCommand == COMMAND_GET_CREATURE_TALENT_BEST)
    {
        if (g_pVirtualMachine->StackPopInteger(&nCRMax) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (nArgs > 2 && g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (nArgs > 3 && g_pVirtualMachine->StackPopInteger(&nInclusion) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (nArgs > 4 && g_pVirtualMachine->StackPopInteger(&nExcludeType) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (nArgs > 5 && g_pVirtualMachine->StackPopInteger(&nExcludeId) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    else if (nCommand == COMMAND_GET_CREATURE_TALENT_RANDOM)
    {
        if (nArgs > 1 && g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (nArgs > 2 && g_pVirtualMachine->StackPopInteger(&nInclusion) == 0)
        {
            return ERROR_STACK_POP;
        }
    }

    CScriptTalent talent;
    pServerExoApp = g_pAppManager->GetServerExoApp();
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        int nTalentType;
        int nTalent;
        unsigned char nMultiClass;
        unsigned long oidItem;
        int nItemPropertyIndex;
        unsigned char nCasterLevel;

        if (nCommand == COMMAND_GET_CREATURE_TALENT_BEST)
        {
            pCreature->GetStats()->GetCreatureTalentRandom(
                nCategory, nCRMax, nTalentType, nTalent, nMultiClass, oidItem,
                nItemPropertyIndex, nCasterLevel, nInclusion, nExcludeType,
                nExcludeId);
        }
        else if (nCommand == COMMAND_GET_CREATURE_TALENT_RANDOM)
        {
            pCreature->GetStats()->GetCreatureTalentRandom(
                nCategory, -1, nTalentType, nTalent, nMultiClass, oidItem,
                nItemPropertyIndex, nCasterLevel, nInclusion, -1, -1);
        }
        talent.m_nID = nTalent;
        talent.m_nType = nTalentType;
        talent.m_nMultiClass = nMultiClass;
        talent.m_oidItem = oidItem;
        talent.m_nItemPropertyIndex = nItemPropertyIndex;
        talent.m_nCasterLevel = nCasterLevel;
    }
    else
    {
        talent.m_nID = -1;
    }
    if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_TALENT, &talent) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005B9AB0
int CSWVirtualMachineCommands::ExecuteCommandGetDamageDealtByType(int nCommand,
                                                                  int nArgs)
{
    int nResult;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;
    int nDamageType;
    char cResult;

    pObject = NULL;
    nResult = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != 0)
    {
        pObject = (CSWSObject*)pGameObject->AsSWSObject();
    }
    if (g_pVirtualMachine->StackPopInteger(&nDamageType) == 0)
    {
        return -2001;
    }
    if (pObject != NULL && nDamageType > 0 &&
        nDamageType < CSWSEffectListHandler::DAMAGE_FLAG_BASE_DAMAGE)
    {
        cResult = pObject->m_nObjectType;
        if (cResult == 5 || cResult == 9 || cResult == 10)
        {
            nResult = pObject->GetLastDamageAmountByFlags(nDamageType);
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005B9B60
int CSWVirtualMachineCommands::ExecuteCommandGetDayTrack(int nCommand,
                                                         int nArgs)
{
    unsigned long oidArea;
    int nTrack;
    CServerExoApp* pServerExoApp;
    CSWSArea* pArea;

    nTrack = 0;
    if (g_pVirtualMachine->StackPopObject(&oidArea) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pArea = pServerExoApp->GetAreaByGameObjectID(oidArea);
    if (pArea != NULL)
    {
        nTrack = pArea->m_pAmbientSound->m_nMusicDayTrack;
    }
    return g_pVirtualMachine->StackPushInteger(nTrack) != 0 ? 0 : -2000;
}
// PC 0x005B9BD0
int CSWVirtualMachineCommands::ExecuteCommandGetDistanceBetween(int nCommand, int nArgs)
{

    float fDistance = 0.0f;
    unsigned long oidObjectA;
    unsigned long oidObjectB;
    CGameObject* pObjectA;
    CGameObject* pObjectB;
    CGameObjectArray* pObjectArray = g_pAppManager->m_pServerExoApp->GetObjectArray();

    if (g_pVirtualMachine->StackPopObject(&oidObjectA) != 0)
    {
        if (g_pVirtualMachine->StackPopObject(&oidObjectB) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (pObjectArray->GetGameObject(oidObjectA, &pObjectA) == CGameObjectArray::SUCCESS &&
            pObjectArray->GetGameObject(oidObjectB, &pObjectB) == CGameObjectArray::SUCCESS)
        {
            fDistance = magnitude(((CSWSObject*)pObjectA)->m_vPosition -
                                  ((CSWSObject*)pObjectB)->m_vPosition);
        }
        return g_pVirtualMachine->StackPushFloat(fDistance) != 0 ? 0 : ERROR_STACK_PUSH;
    }
    else
    {
        return ERROR_STACK_POP;
    }
}
// PC 0x005B9CD0
int CSWVirtualMachineCommands::ExecuteCommandGetDistanceBetween2D(int nCommand, int nArgs)
{

    float fDistance = 0.0f;
    unsigned long oidObjectA;
    unsigned long oidObjectB;
    CGameObject* pObjectA;
    CGameObject* pObjectB;
    CGameObjectArray* pObjectArray = g_pAppManager->m_pServerExoApp->GetObjectArray();

    if (g_pVirtualMachine->StackPopObject(&oidObjectA) != 0)
    {
        if (g_pVirtualMachine->StackPopObject(&oidObjectB) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (pObjectArray->GetGameObject(oidObjectA, &pObjectA) == CGameObjectArray::SUCCESS &&
            pObjectArray->GetGameObject(oidObjectB, &pObjectB) == CGameObjectArray::SUCCESS)
        {
            Vector vPositionA = ((CSWSObject*)pObjectA)->m_vPosition;
            Vector vPositionB = ((CSWSObject*)pObjectB)->m_vPosition;
            Vector vDifference(vPositionA.m_fX - vPositionB.m_fX,
                               vPositionA.m_fY - vPositionB.m_fY, 0.0f);
            fDistance = magnitude(vDifference);
        }
        return g_pVirtualMachine->StackPushFloat(fDistance) != 0 ? 0 : ERROR_STACK_PUSH;
    }
    else
    {
        return ERROR_STACK_POP;
    }
}
// PC 0x005B9DF0
int CSWVirtualMachineCommands::ExecuteCommandGetDistanceBetweenLocations(int nCommand, int nArgs)
{

    CScriptLocation* pLocationA;
    CScriptLocation* pLocationB;
    float fDistance;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocationA) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocationB) == 0)
    {
        if (pLocationA != NULL)
        {
            delete pLocationA;
        }
        return ERROR_STACK_POP;
    }
    else
    {
        fDistance = magnitude(pLocationA->m_vPosition - pLocationB->m_vPosition);
        if (g_pVirtualMachine->StackPushFloat(fDistance) == 0)
        {
            if (pLocationA != NULL)
            {
                delete pLocationA;
                pLocationA = NULL;
            }
            if (pLocationB != NULL)
            {
                delete pLocationB;
            }
            return ERROR_STACK_PUSH;
        }
        else
        {
            if (pLocationA != NULL)
            {
                delete pLocationA;
                pLocationA = NULL;
            }
            if (pLocationB != NULL)
            {
                delete pLocationB;
            }
            return 0;
        }
    }
}
// PC 0x005B9F20
int CSWVirtualMachineCommands::ExecuteCommandGetDistanceBetweenLocations2D(int nCommand, int nArgs)
{

    CScriptLocation* pLocationA;
    CScriptLocation* pLocationB;
    float fDistance;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocationA) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocationB) == 0)
    {
        if (pLocationA != NULL)
        {
            delete pLocationA;
        }
        return ERROR_STACK_POP;
    }
    else
    {
        Vector vPositionA = pLocationA->m_vPosition;
        Vector vPositionB = pLocationB->m_vPosition;
        Vector vDifference(vPositionA.m_fX - vPositionB.m_fX,
                           vPositionA.m_fY - vPositionB.m_fY, 0.0f);
        fDistance = magnitude(vDifference);
        if (g_pVirtualMachine->StackPushFloat(fDistance) == 0)
        {
            if (pLocationA != NULL)
            {
                delete pLocationA;
                pLocationA = NULL;
            }
            if (pLocationB != NULL)
            {
                delete pLocationB;
            }
            return ERROR_STACK_PUSH;
        }
        else
        {
            if (pLocationA != NULL)
            {
                delete pLocationA;
                pLocationA = NULL;
            }
            if (pLocationB != NULL)
            {
                delete pLocationB;
            }
            return 0;
        }
    }
}
// PC 0x005BA080
int CSWVirtualMachineCommands::ExecuteCommandGetDistanceToObject(int nCommand, int nArgs)
{

    float fDistance = -1.0f;
    unsigned long oidTarget;
    CGameObject* pCaller;
    CGameObject* pTarget;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject, &pCaller) ==
            CGameObjectArray::SUCCESS)
        {
            Vector vCallerPosition = ((CSWSObject*)pCaller)->m_vPosition;
            CSWSArea* pCallerArea = ((CSWSObject*)pCaller)->GetArea();
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            if (pServerExoApp->GetObjectArray()->GetGameObject(
                    oidTarget, &pTarget) == CGameObjectArray::SUCCESS &&
                pTarget != NULL &&
                pTarget->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
            {
                Vector vTargetPosition = ((CSWSObject*)pTarget)->m_vPosition;
                if (((CSWSObject*)pTarget)->GetArea() == pCallerArea)
                {
                    fDistance = magnitude(vTargetPosition - vCallerPosition);
                }
            }
        }
    }
    return g_pVirtualMachine->StackPushFloat(fDistance) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BA1C0
int CSWVirtualMachineCommands::ExecuteCommandGetDistanceToObject2D(int nCommand, int nArgs)
{

    float fDistance = -1.0f;
    unsigned long oidTarget;
    CGameObject* pCaller;
    CGameObject* pTarget;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject, &pCaller) ==
            CGameObjectArray::SUCCESS)
        {
            Vector vCallerPosition = ((CSWSObject*)pCaller)->m_vPosition;
            CSWSArea* pCallerArea = ((CSWSObject*)pCaller)->GetArea();
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            if (pServerExoApp->GetObjectArray()->GetGameObject(
                    oidTarget, &pTarget) == CGameObjectArray::SUCCESS &&
                pTarget != NULL &&
                pTarget->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
            {
                Vector vTargetPosition = ((CSWSObject*)pTarget)->m_vPosition;
                if (((CSWSObject*)pTarget)->GetArea() == pCallerArea)
                {
                    Vector vDifference(vTargetPosition.m_fX - vCallerPosition.m_fX,
                                       vTargetPosition.m_fY - vCallerPosition.m_fY,
                                       0.0f);
                    fDistance = magnitude(vDifference);
                }
            }
        }
    }
    return g_pVirtualMachine->StackPushFloat(fDistance) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BA300
int CSWVirtualMachineCommands::ExecuteCommandGetEffectCreator(int nCommand,
                                                              int nArgs)
{
    CGameEffect* pEffect = NULL;
    unsigned long oidCreator = OBJECT_INVALID;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pEffect) == 0)
    {
        return -2001;
    }
    if (pEffect != NULL)
    {
        oidCreator = pEffect->m_oidCreator;
    }
    if (g_pVirtualMachine->StackPushObject(oidCreator) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005BA380
int CSWVirtualMachineCommands::ExecuteCommandGetEffectDurationType(int nCommand,
                                                                   int nArgs)
{
    CGameEffect* pEffect = NULL;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pEffect) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPushInteger(pEffect->m_nSubType &
                                            CGameEffect::DURATION_MASK) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005BA3F0
int CSWVirtualMachineCommands::ExecuteCommandGetEffectSpellId(int nCommand,
                                                              int nArgs)
{
    CGameEffect* pEffect = NULL;
    int nSpellId;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pEffect) == 0)
    {
        return -2001;
    }
    nSpellId = pEffect->m_nSpellId;
    if (g_pVirtualMachine->StackPushInteger(nSpellId) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005BA460
int CSWVirtualMachineCommands::ExecuteCommandGetEffectSubType(int nCommand,
                                                              int nArgs)
{
    CGameEffect* pEffect = NULL;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pEffect) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPushInteger(pEffect->m_nSubType &
                                            CGameEffect::SUBTYPE_MASK) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005BA4D0
int CSWVirtualMachineCommands::ExecuteCommandGetEffectType(int nCommand,
                                                           int nArgs)
{
    CGameEffect* pEffect = NULL;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pEffect) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPushInteger(
            pEffect->GetEffectTypeScriptConstant()) == 0)
    {
        return -2000;
    }
    delete pEffect;
    return 0;
}
// PC 0x005BA540
int CSWVirtualMachineCommands::ExecuteCommandGetEmotions(int nCommand, int nArgs)
{
    unsigned long oidSource = OBJECT_INVALID;
    unsigned long oidTarget = OBJECT_INVALID;
    CGameObject* pGameObject;
    int nResult = 0;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
        {
            oidSource = m_oidObject;
        }
        if (nArgs > 1 && g_pVirtualMachine->StackPopObject(&oidSource) == 0)
        {
            return ERROR_STACK_POP;
        }
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(oidTarget, &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSObject* pObject = pGameObject->AsSWSObject();
            if (pObject != NULL)
            {
                switch (nCommand)
                {
                case COMMAND_GET_IS_ENEMY:
                    if (pGameObject->AsSWSPlaceable() != NULL)
                    {
                        if (pGameObject->AsSWSPlaceable()->m_bHostile != 0)
                        {
                            nResult = pObject->GetReputation(oidSource, 0) <= 10;
                        }
                        else
                        {
                            nResult = 0;
                        }
                    }
                    else
                    {
                        nResult = pObject->GetReputation(oidSource, 0) <= 10;
                    }
                    break;
                case COMMAND_GET_IS_FRIEND:
                    nResult = pObject->GetReputation(oidSource, 0) >= 90;
                    break;
                case COMMAND_GET_IS_NEUTRAL:
                    {
                        int nReputation = pObject->GetReputation(oidSource, 0);
                        nResult = nReputation > 10 && nReputation < 90;
                    }
                    break;
                }
            }
        }
        return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : ERROR_STACK_PUSH;
    }
}
// PC 0x005BA690
int CSWVirtualMachineCommands::ExecuteCommandGetEncounterData(int nCommand,
                                                              int nArgs)
{
    unsigned long oidEncounter = OBJECT_INVALID;
    int nResult = 0;
    CServerExoApp* pServerExoApp;
    CSWSEncounter* pEncounter;

    if (nArgs >= 1)
    {
        if (g_pVirtualMachine->StackPopObject(&oidEncounter) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pEncounter = pServerExoApp->GetEncounterByGameObjectID(oidEncounter);
    if (pEncounter != NULL)
    {
        if (nCommand == COMMAND_GET_ENCOUNTER_ACTIVE)
        {
            nResult = pEncounter->m_bActive;
        }
        else if (nCommand == COMMAND_GET_ENCOUNTER_DIFFICULTY)
        {
            nResult = pEncounter->m_nDifficulty;
        }
        else if (nCommand == COMMAND_GET_ENCOUNTER_SPAWNS_MAX)
        {
            nResult = pEncounter->m_nRespawns;
        }
        else if (nCommand == COMMAND_GET_ENCOUNTER_SPAWNS_CURRENT)
        {
            nResult = pEncounter->m_nCurrentSpawns;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0
               ? 0
               : ERROR_STACK_PUSH;
}
// PC 0x005BA740
int CSWVirtualMachineCommands::ExecuteCommandGetEnteringObject(int nCommand,
                                                              int nArgs)
{
    unsigned long oidEntering = CGameObjectArray::INVALIDOBJID;

    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        CGameObject* pGameObject = pServerExoApp->GetGameObject(m_oidObject);
        if (pGameObject != NULL)
        {
            switch (pGameObject->m_nObjectType)
            {
            case SW_OBJECT_TYPE_DOOR:
            {
                CSWSDoor* pDoor = pGameObject->AsSWSDoor();
                if (pDoor != NULL)
                    oidEntering = pDoor->m_oidLastTriggeredBy;
                break;
            }
            case SW_OBJECT_TYPE_PLACEABLE:
            {
                CSWSPlaceable* pPlaceable = pGameObject->AsSWSPlaceable();
                if (pPlaceable != NULL)
                    oidEntering = pPlaceable->m_oidLastTriggeredBy;
                break;
            }
            case SW_OBJECT_TYPE_TRIGGER:
            {
                CSWSTrigger* pTrigger = pGameObject->AsSWSTrigger();
                if (pTrigger != NULL)
                    oidEntering = pTrigger->m_oidLastEntered;
                break;
            }
            case SW_OBJECT_TYPE_AREA_OF_EFFECT:
            {
                CSWSAreaOfEffectObject* pAreaOfEffect =
                    pGameObject->AsSWSAreaOfEffectObject();
                if (pAreaOfEffect != NULL)
                    oidEntering = pAreaOfEffect->m_oidLastEntered;
                break;
            }
            case SW_OBJECT_TYPE_MODULE:
            {
                CSWSModule* pModule = pGameObject->AsSWSModule();
                if (pModule != NULL)
                    oidEntering = pModule->m_oidClientEnter;
                break;
            }
            case SW_OBJECT_TYPE_AREA:
            {
                CSWSArea* pArea = pGameObject->AsSWSArea();
                if (pArea != NULL)
                    oidEntering = pArea->m_oidEnteringObject;
                break;
            }
            case SW_OBJECT_TYPE_ENCOUNTER:
            {
                CSWSEncounter* pEncounter = pGameObject->AsSWSEncounter();
                if (pEncounter != NULL)
                    oidEntering = pEncounter->m_oidLastEntered;
                break;
            }
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidEntering) != 0 ?
        0 : ERROR_STACK_PUSH;
}
// PC 0x005BA840
int CSWVirtualMachineCommands::ExecuteCommandGetFacing(int nCommand, int nArgs)
{

    float fFacing = -1.0f;
    unsigned long oidTarget;
    CGameObject* pGameObject;
    Vector vOrientation;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidTarget, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL &&
        pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        vOrientation = ((CSWSObject*)pGameObject)->m_vOrientation;
        vOrientation.m_fZ = 0.0f;
        Vector vNormalized = normalize(vOrientation);
        fFacing = (float)atan2(vOrientation.m_fY, vOrientation.m_fX) *
                  RAD_TO_DEG;
        if (vNormalized.m_fY < 0.0f)
        {
            fFacing += 360.0f;
        }
    }
    return g_pVirtualMachine->StackPushFloat(fFacing) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BA950
int CSWVirtualMachineCommands::ExecuteCommandGetFactionAverageGoodEvilAlignment(
    int nCommand, int nArgs)
{
    int nResult;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    int nResult2;
    CServerExoAppInternal* pServerExoAppInternal;
    CFactionManager* pFactionManager;
    CSWSFaction* pFaction;

    nResult = -1;
    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                pFaction = pFactionManager->GetFaction(nResult2);
                nResult = pFaction->GetAverageGoodEvilAlignment();
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BA9F0
int CSWVirtualMachineCommands::ExecuteCommandGetFactionAverageLevel(
    int nCommand, int nArgs)
{
    int nResult;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    int nResult2;
    CServerExoAppInternal* pServerExoAppInternal;
    CFactionManager* pFactionManager;
    CSWSFaction* pFaction;

    nResult = -1;
    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                pFaction = pFactionManager->GetFaction(nResult2);
                nResult = pFaction->GetAverageLevel();
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BAA90
int CSWVirtualMachineCommands::ExecuteCommandGetFactionAverageReputation(
    int nCommand, int nArgs)
{
    int nResult;
    unsigned long oidSourceFactionMember;
    unsigned long oidTarget;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CServerExoAppInternal* pServerExoAppInternal;
    int nResult2;
    CFactionManager* pFactionManager;

    nResult = -1;
    if (g_pVirtualMachine->StackPopObject(&oidSourceFactionMember) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature =
        pServerExoApp->GetCreatureByGameObjectID(oidSourceFactionMember);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                nResult =
                    pFactionManager->GetFaction(nResult2)->GetAverageReputation(
                        oidTarget);
            }
        }
    }
    if (g_pVirtualMachine->StackPushInteger(nResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005BAB40
int CSWVirtualMachineCommands::ExecuteCommandGetFactionAverageXP(int nCommand,
                                                                 int nArgs)
{
    int nResult;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    int nResult2;
    CServerExoAppInternal* pServerExoAppInternal;
    CFactionManager* pFactionManager;
    CSWSFaction* pFaction;

    nResult = -1;
    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                pFaction = pFactionManager->GetFaction(nResult2);
                nResult = pFaction->GetAverageXP();
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BABE0
int CSWVirtualMachineCommands::ExecuteCommandGetFactionBestAC(int nCommand,
                                                              int nArgs)
{
    unsigned long oidResult;
    unsigned long oidResult2;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CServerExoAppInternal* pServerExoAppInternal;
    int nResult2;
    CFactionManager* pFactionManager;

    oidResult = OBJECT_INVALID;
    oidResult2 = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult2) == 0)
    {
        return -2001;
    }
    nResult = 1;
    if (nArgs == 2 && g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult2);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                oidResult = pFactionManager->GetFaction(nResult2)->GetBestAC(
                    oidResult2, nResult);
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005BACB0
int CSWVirtualMachineCommands::ExecuteCommandGetFactionEqual(int nCommand,
                                                             int nArgs)
{
    int nResult;
    unsigned long oidFirstObject;
    unsigned long oidSecondObject;
    int nResult2;
    int nResult3;
    CServerExoApp* pServerExoApp;

    nResult = false;
    if (g_pVirtualMachine->StackPopObject(&oidFirstObject) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidSecondObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetFactionOfObject(oidFirstObject, &nResult3) == 1)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetFactionOfObject(oidSecondObject, &nResult2) == 1)
        {
            if (nResult3 == nResult2)
            {
                nResult = 1;
            }
        }
    }
    if (g_pVirtualMachine->StackPushInteger(nResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005BAD60
int CSWVirtualMachineCommands::ExecuteCommandGetFactionGold(int nCommand,
                                                            int nArgs)
{
    int nResult;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    int nResult2;
    CServerExoAppInternal* pServerExoAppInternal;
    CFactionManager* pFactionManager;
    CSWSFaction* pFaction;

    nResult = -1;
    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                pFaction = pFactionManager->GetFaction(nResult2);
                nResult = pFaction->GetGold();
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BAE00
int CSWVirtualMachineCommands::ExecuteCommandGetFactionLeastDamagedMember(
    int nCommand, int nArgs)
{
    unsigned long oidResult;
    unsigned long oidResult2;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CServerExoAppInternal* pServerExoAppInternal;
    int nResult2;
    CFactionManager* pFactionManager;

    oidResult = OBJECT_INVALID;
    oidResult2 = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult2) == 0)
    {
        return -2001;
    }
    nResult = 1;
    if (nArgs == 2 && g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult2);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                oidResult = pFactionManager->GetFaction(nResult2)
                                ->GetLeastDamagedMember(oidResult2, nResult);
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005BAED0
int CSWVirtualMachineCommands::ExecuteCommandGetFactionLeader(int nCommand,
                                                              int nArgs)
{
    unsigned long oidResult;
    unsigned long oidMemberOfFaction;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWSCreature* pCreature2;
    CSWSFaction* pFaction;

    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidMemberOfFaction) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(
        oidMemberOfFaction);
    pCreature2 = pCreature;
    if (pCreature != NULL && pCreature->GetFaction() != NULL)
    {
        pFaction = pCreature2->GetFaction();
        oidResult = pFaction->GetLeader();
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005BAF50
int CSWVirtualMachineCommands::ExecuteCommandGetFactionMember(int nCommand,
                                                            int nArgs)
{
    unsigned long oidMemberOfFaction = OBJECT_INVALID;
    int bPCOnly = 1;
    unsigned long oidResult = OBJECT_INVALID;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidMemberOfFaction) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&bPCOnly) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        pServerExoApp = g_pAppManager->GetServerExoApp();
        if (pServerExoApp->GetObjectArray()->GetGameObject(
                oidMemberOfFaction, &pGameObject) == CGameObjectArray::SUCCESS)
        {
            CServerExoAppInternal* pServer =
                g_pAppManager->GetServerExoApp()->m_pcExoAppInternal;
            if (pServer != NULL)
            {
                CFactionManager* pFactionManager = pServer->m_pFactionManager;
                int nFaction;
                if (pFactionManager != NULL &&
                    g_pAppManager->GetServerExoApp()->GetFactionOfObject(
                        oidMemberOfFaction, &nFaction) == 1)
                {
                    CSWSFaction* pFaction = pFactionManager->GetFaction(nFaction);
                    if (pFaction != NULL)
                    {
                        if (nCommand == COMMAND_GET_FIRST_FACTION_MEMBER)
                        {
                            oidResult = pFaction->GetFactionMember(0, bPCOnly);
                        }
                        else if (nCommand == COMMAND_GET_NEXT_FACTION_MEMBER)
                        {
                            int nMember = pFaction->m_nLastFactionMember;
                            oidResult = pFaction->GetFactionMember(nMember, bPCOnly);
                        }
                    }
                }
            }
        }
        return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : ERROR_STACK_PUSH;
    }
}
// PC 0x005BB060
int CSWVirtualMachineCommands::ExecuteCommandGetFactionMostDamagedMember(
    int nCommand, int nArgs)
{
    unsigned long oidResult;
    unsigned long oidResult2;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CServerExoAppInternal* pServerExoAppInternal;
    int nResult2;
    CFactionManager* pFactionManager;

    oidResult = OBJECT_INVALID;
    oidResult2 = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult2) == 0)
    {
        return -2001;
    }
    nResult = 1;
    if (nArgs == 2 && g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult2);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                oidResult =
                    pFactionManager->GetFaction(nResult2)->GetMostDamagedMember(
                        oidResult2, nResult);
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005BB130
int CSWVirtualMachineCommands::ExecuteCommandGetFactionMostFrequentClass(
    int nCommand, int nArgs)
{
    int nResult;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    int nResult2;
    CServerExoAppInternal* pServerExoAppInternal;
    CFactionManager* pFactionManager;
    CSWSFaction* pFaction;

    nResult = -1;
    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                pFaction = pFactionManager->GetFaction(nResult2);
                nResult = pFaction->GetMostFrequentClass();
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BB1D0
int CSWVirtualMachineCommands::ExecuteCommandGetFactionStrongestMember(
    int nCommand, int nArgs)
{
    unsigned long oidResult;
    unsigned long oidResult2;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CServerExoAppInternal* pServerExoAppInternal;
    int nResult2;
    CFactionManager* pFactionManager;

    oidResult = OBJECT_INVALID;
    oidResult2 = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult2) == 0)
    {
        return -2001;
    }
    nResult = 1;
    if (nArgs == 2 && g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult2);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                oidResult =
                    pFactionManager->GetFaction(nResult2)->GetStrongestMember(
                        oidResult2, nResult);
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005BB2A0
int CSWVirtualMachineCommands::ExecuteCommandGetFactionWeakestMember(
    int nCommand, int nArgs)
{
    unsigned long oidResult;
    unsigned long oidResult2;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CServerExoAppInternal* pServerExoAppInternal;
    int nResult2;
    CFactionManager* pFactionManager;

    oidResult = OBJECT_INVALID;
    oidResult2 = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult2) == 0)
    {
        return -2001;
    }
    nResult = 1;
    if (nArgs == 2 && g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult2);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                oidResult =
                    pFactionManager->GetFaction(nResult2)->GetWeakestMember(
                        oidResult2, nResult);
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005BB370
int CSWVirtualMachineCommands::ExecuteCommandGetFactionWorstAC(int nCommand,
                                                               int nArgs)
{
    unsigned long oidResult;
    unsigned long oidResult2;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CServerExoAppInternal* pServerExoAppInternal;
    int nResult2;
    CFactionManager* pFactionManager;

    oidResult = OBJECT_INVALID;
    oidResult2 = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult2) == 0)
    {
        return -2001;
    }
    nResult = 1;
    if (nArgs == 2 && g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult2);
    if (pCreature != 0)
    {
        nResult2 = pCreature->m_pStats->m_nFactionId;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
#ifdef _XBOX
        if (pServerExoAppInternal == NULL)
        {
            EXOXBOXASSERT(0);
        }
        else
#else
        if (pServerExoAppInternal != NULL)
#endif
        {
            pFactionManager =
                (CFactionManager*)pServerExoAppInternal->m_pFactionManager;
#ifdef _XBOX
            if (pFactionManager == NULL)
            {
                EXOXBOXASSERT(0);
            }
            else
#else
            if (pFactionManager != NULL)
#endif
            {
                oidResult = pFactionManager->GetFaction(nResult2)->GetWorstAC(
                    oidResult2, nResult);
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005BB440
int CSWVirtualMachineCommands::ExecuteCommandGetFortitudeSavingThrow(int nCommand,
                                                                  int nArgs)
{
    unsigned long oidTarget;
    CServerExoApp* pServerExoApp;
    CGameObject* pObject;
    int nSavingThrow;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pObject = pServerExoApp->GetGameObject(oidTarget);
    nSavingThrow = 0;
    if (pObject != NULL)
    {
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            if (pCreature != NULL)
            {
                CSWSCreatureStats* pStats = pCreature->m_pStats;
                if (pStats != NULL)
                {
                    nSavingThrow = pStats->GetFortSavingThrow(0);
                }
            }
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
        {
            CSWSDoor* pDoor = pObject->AsSWSDoor();
            if (pDoor != NULL)
            {
                nSavingThrow = pDoor->m_nFortSave;
            }
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
            if (pPlaceable != NULL)
            {
                nSavingThrow = pPlaceable->m_nFortSave;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nSavingThrow) != 0
               ? 0
               : ERROR_STACK_PUSH;
}
// PC 0x005CA3A0
int CSWVirtualMachineCommands::ExecuteCommandGetGameDifficulty(int nCommand,
                                                                int nArgs)
{
    return g_pVirtualMachine->StackPushInteger(
               g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty) != 0
               ? 0 : -2000;
}
// PC 0x005BB530
int CSWVirtualMachineCommands::ExecuteCommandGetGlobalLocation(int nCommand,
                                                               int nArgs)
{
    CExoString sIdentifier;
    CScriptLocation lValue;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopString(&sIdentifier) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetGlobalVariableTable()->GetValueLocation(sIdentifier,
                                                            lValue);
    if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_LOCATION,
                                                   &lValue) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BB620
int CSWVirtualMachineCommands::ExecuteCommandGetGlobalNumber(int nCommand,
                                                             int nArgs)
{

    CExoString sIdentifier;
    char cValue = 0;
    int bValue;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopString(&sIdentifier) == 0)
    {
        return ERROR_STACK_PUSH;
    }

    switch (nCommand)
    {
        case COMMAND_GET_GLOBAL_BOOLEAN:
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            pServerExoApp->GetGlobalVariableTable()->GetValueBoolean(sIdentifier,
                                                                   bValue);
            cValue = (bValue != 0);
            break;
        case COMMAND_GET_GLOBAL_NUMBER:
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            pServerExoApp->GetGlobalVariableTable()->GetValueNumber(sIdentifier,
                                                                  cValue);
            break;
#ifdef _XBOX
        default:
            EXOXBOXASSERT(0);
            break;
#endif
    }
    if (g_pVirtualMachine->StackPushInteger(cValue) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BB730
int CSWVirtualMachineCommands::ExecuteCommandGetGlobalString(int nCommand,
                                                             int nArgs)
{
    CExoString sIdentifier;
    CExoString sValue;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopString(&sIdentifier) == 0)
    {

        return ERROR_STACK_PUSH;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetGlobalVariableTable()->GetValueString(sIdentifier,
                                                          sValue);
    if (g_pVirtualMachine->StackPushString(sValue) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BB820
int CSWVirtualMachineCommands::ExecuteCommandGetGoingToBeAttackedBy(
    int nCommand, int nArgs)
{
    unsigned long oidCreature;
    unsigned long oidAttacker;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidAttacker = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2000;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        oidAttacker = pCreature->m_oidGoingToBeAttackedBy;
    }
    return g_pVirtualMachine->StackPushObject(oidAttacker) != 0 ? 0 : -2001;
}
// PC 0x005BB890
int CSWVirtualMachineCommands::ExecuteCommandGetGold(int nCommand, int nArgs)
{
    unsigned long oidTarget;
    int nGold;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWSPlaceable* pPlaceable;
    CItemRepository* pRepository;
    unsigned long oidItem;
    CSWSItem* pItem;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }

    nGold = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidTarget);
    if (pCreature != NULL)
    {
        nGold = pCreature->GetGold();
    }
    else
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pPlaceable = pServerExoApp->GetPlaceableByGameObjectID(oidTarget);
        if (pPlaceable != NULL)
        {
            pRepository = pPlaceable->m_pcItemRepository;
            if (pRepository != NULL)
            {
                oidItem = pRepository->FindItemWithBaseItemType(CSWBaseItem::ITEM_TYPE_CREDITS, 0);
                pServerExoApp = g_pAppManager->m_pServerExoApp;
                pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
                if (pItem != NULL)
                {
                    nGold = pItem->m_nStackSize;
                }
            }
        }
    }

    return g_pVirtualMachine->StackPushInteger(nGold) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BB940
int CSWVirtualMachineCommands::ExecuteCommandGetGoldPieceValue(int nCommand,
                                                               int nArgs)
{
    int nResult;
    unsigned long oidItem;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSItem* pItem;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)(CGameObjectArray*)pServerExoApp->GetObjectArray()
                ->GetGameObject(oidItem, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSItem() != 0)
    {
        pItem = (CSWSItem*)pGameObject->AsSWSItem();
        nResult = pItem->GetCost();
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BBB20
int CSWVirtualMachineCommands::ExecuteCommandGetHasFeat(int nCommand, int nArgs)
{
    int nResult = 0;
    int nFeat;
    unsigned long oidCreature;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopInteger(&nFeat) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
        {
            return ERROR_STACK_POP;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                    ->GetGameObject(oidCreature, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL)
        {
            pCreature = pGameObject->AsSWSCreature();
            if (nCommand != COMMAND_GET_FEAT_ACQUIRED)
            {
                nResult = pCreature->GetStats()->GetFeatRemainingUses(nFeat) > 0;
            }
            else
            {
                nResult = pCreature->GetStats()->HasFeat(nFeat);
            }
        }
        return g_pVirtualMachine->StackPushInteger(nResult) != 0
                   ? 0 : ERROR_STACK_PUSH;
    }
}
// PC 0x005BBBF0
int CSWVirtualMachineCommands::ExecuteCommandGetHasFeatEffect(int nCommand,
                                                              int nArgs)
{
    int nResult;
    int nFeat;
    unsigned long oidObject;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    nResult = 0;
    if (g_pVirtualMachine->StackPopInteger(&nFeat) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
        {
            return -2001;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                    ->GetGameObject(oidObject,
                                    (struct CGameObject**)&pObject) ==
                CGameObjectArray::SUCCESS &&
            pObject->AsSWSObject() != 0)
        {
            nResult = pObject->GetHasFeatEffectApplied(nFeat);
        }
        return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
    }
}
// PC 0x005BBC90
int CSWVirtualMachineCommands::ExecuteCommandGetHasInventory(int nCommand, int nArgs)
{
    unsigned long oidObject;
    int bHasInventory = 0;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL)
    {
        if (pGameObject->AsSWSCreature() != NULL ||
            pGameObject->AsSWSStore() != NULL)
        {
            bHasInventory = 1;
        }
        else if (pGameObject->AsSWSPlaceable() != NULL)
        {
            bHasInventory = pGameObject->AsSWSPlaceable()->m_bHasInventory;
        }
        else if (pGameObject->AsSWSItem() != NULL &&
                 pGameObject->AsSWSItem()->m_pRepository != NULL)
        {
            bHasInventory = 1;
        }
    }
    return g_pVirtualMachine->StackPushInteger(bHasInventory) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BBD40
int CSWVirtualMachineCommands::ExecuteCommandGetHasSkill(int nCommand, int nArgs)
{
    int nResult = 0;
    int nSkill;
    unsigned long oidCreature;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWSCreatureStats* pStats;

    if (g_pVirtualMachine->StackPopInteger(&nSkill) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
        {
            return ERROR_STACK_POP;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(oidCreature, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL)
        {
            pCreature = pGameObject->AsSWSCreature();
            pStats = pCreature->m_pStats;
            if (pStats->GetCanUseSkill(nSkill) == 1)
            {
                nResult = 1;
            }
        }
        return g_pVirtualMachine->StackPushInteger(nResult) != 0
            ? 0 : ERROR_STACK_PUSH;
    }
}
// PC 0x005BBE00
int CSWVirtualMachineCommands::ExecuteCommandGetHasSpell(int nCommand, int nArgs)
{
    int nResult = 0;
    int nSpell;
    unsigned long oidCreature;
    CSWSCreature* pCreature;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nSpell) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
        {
            return ERROR_STACK_POP;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
        if (pCreature != NULL)
        {
            if (nCommand != COMMAND_GET_SPELL_ACQUIRED)
            {
                nResult = pCreature->GetStats()->HasSpell(0, nSpell, 1);
            }
            else
            {
                nResult = pCreature->GetStats()->HasSpell(0, nSpell, 0);
            }
        }
        return g_pVirtualMachine->StackPushInteger(nResult) != 0
                   ? 0 : ERROR_STACK_PUSH;
    }
}
// PC 0x005BBEA0
int CSWVirtualMachineCommands::ExecuteCommandGetHasSpellEffect(int nCommand,
                                                               int nArgs)
{
    int nResult;
    int nSpell;
    unsigned long oidObject;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    nResult = 0;
    if (g_pVirtualMachine->StackPopInteger(&nSpell) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
        {
            return -2001;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                    ->GetGameObject(oidObject,
                                    (struct CGameObject**)&pObject) ==
                CGameObjectArray::SUCCESS &&
            pObject->AsSWSObject() != 0)
        {
            nResult = pObject->HasSpellEffectApplied(nSpell);
        }
        return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
    }
}
// PC 0x005BBF40
int CSWVirtualMachineCommands::ExecuteCommandGetHitDice(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nHitDice = 0;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)pServerExoApp->GetObjectArray()
            ->GetGameObject(oidCreature, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            nHitDice = pStats->GetLevel(0);
        }
    }
    return g_pVirtualMachine->StackPushInteger(nHitDice) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BBFE0
int CSWVirtualMachineCommands::ExecuteCommandGetHitpoints(int nCommand,
                                                          int nArgs)
{
    unsigned long oidObject;
    int nHitPoints = 0;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidObject,
                                                       &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        if (nCommand == COMMAND_GET_CURRENT_HIT_POINTS)
        {
            if (pGameObject->AsSWSCreature() != NULL)
            {
                nHitPoints = pGameObject->AsSWSCreature()->GetCurrentHitPoints(0);
            }
            else
            {
                nHitPoints = pGameObject->AsSWSObject()->GetCurrentHitPoints(0);
            }
        }
        else
        {
            if (pGameObject->AsSWSCreature() != NULL)
            {
                nHitPoints = pGameObject->AsSWSCreature()->GetMaxHitPoints(1);
            }
            else
            {
                nHitPoints = pGameObject->AsSWSObject()->GetMaxHitPoints(1);
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nHitPoints) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BC0C0
int CSWVirtualMachineCommands::ExecuteCommandGetCurrentForcePoints(int nCommand,
                                                                   int nArgs)
{
    unsigned long oidObject;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    int nResult;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidObject);
    nResult = 0;
    if (pCreature != 0)
    {
        nResult = (__int16)(pCreature->m_pStats->m_nCurrentForcePoints +
                            pCreature->m_pStats->m_nTemporaryForcePoints);
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BC130
int CSWVirtualMachineCommands::ExecuteCommandGetMaxForcePoints(int nCommand,
                                                               int nArgs)
{
    unsigned long oidObject;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    int nResult;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature =
        (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(oidObject);
    nResult = 0;
    if (pCreature != NULL)
    {
        nResult = (__int16)pCreature->GetMaxForcePoints();
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BC190
int CSWVirtualMachineCommands::ExecuteCommandGetIdentified(int nCommand,
                                                           int nArgs)
{
    unsigned long oidItem;
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;
    int nResult;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nResult = 0;
    pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        nResult = 1;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BC1F0
int CSWVirtualMachineCommands::ExecuteCommandGetInventoryDisturbItem(
    int nCommand, int nArgs)
{
    unsigned long oidItem;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    oidItem = CGameObjectArray::INVALIDOBJID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL)
    {
        pObject = pGameObject->AsSWSObject();
        if (pObject != NULL)
        {
            oidItem = pObject->m_oidInventoryDisturbItem;
        }
    }
    return g_pVirtualMachine->StackPushObject(oidItem) != 0 ? 0 : -2000;
}
// PC 0x005BC240
int CSWVirtualMachineCommands::ExecuteCommandGetInventoryDisturbType(
    int nCommand, int nArgs)
{
    int nResult;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    nResult = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL)
    {
        pObject = (CSWSObject*)pGameObject->AsSWSObject();
        if (pObject != 0)
        {
            nResult = pObject->m_n178;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BC290
int CSWVirtualMachineCommands::ExecuteCommandGetIsDawn(int nCommand, int nArgs)
{
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSModule* pModule;

    nResult = false;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pModule = pServerExoApp->GetModule();
    if (pModule != 0)
    {
        if (pModule->m_nTimeDawnDusk == CSWSModule::TIME_DAWN)
        {
            nResult = 1;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BC2D0
int CSWVirtualMachineCommands::ExecuteCommandGetIsDay(int nCommand, int nArgs)
{
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSModule* pModule;

    nResult = false;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pModule = pServerExoApp->GetModule();
    if (pModule != 0)
    {
        if (pModule->m_nTimeDawnDusk == CSWSModule::TIME_DAY)
        {
            nResult = 1;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BC310
int CSWVirtualMachineCommands::ExecuteCommandGetIsDoorActionPossible(int nCommand, int nArgs)
{
    CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
    int bResult = 0;
    CSWSDoor* pDoor = NULL;
    CSWSPlaceable* pPlaceable = NULL;
    CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    unsigned long oidTarget;
    int nDoorAction;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nDoorAction) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (nCommand == COMMAND_GET_IS_DOOR_ACTION_POSSIBLE)
        {
            pDoor = g_pAppManager->GetServerExoApp()->GetDoorByGameObjectID(oidTarget);
        }
        else
        {
            pPlaceable = g_pAppManager->GetServerExoApp()->GetPlaceableByGameObjectID(oidTarget);
        }
        if (pCreature != NULL &&
            ((pDoor != NULL && pDoor->m_bStatic == 0) ||
             (pPlaceable != NULL && pPlaceable->m_bStatic == 0)))
        {
            switch (nDoorAction)
            {
            case DOOR_ACTION_OPEN:
                if (nCommand == COMMAND_GET_IS_DOOR_ACTION_POSSIBLE && pDoor != NULL)
                {
                    if (pDoor->m_nAnimation == DOOR_ANIMATION_CLOSED &&
                        pDoor->m_bLocked == 0)
                    {
                        bResult = 1;
                    }
                }
                else if (pPlaceable != NULL &&
                         pPlaceable->m_nAnimation == PLACEABLE_ANIMATION_CLOSED &&
                         pPlaceable->m_bLocked == 0)
                {
                    bResult = 1;
                }
                break;
            case DOOR_ACTION_UNLOCK:
                {
                    char nSkillRank = pCreature->GetStats()->GetSkillRank(CSWRules::SKILL_SECURITY, NULL, 0);
                    if (nCommand == COMMAND_GET_IS_DOOR_ACTION_POSSIBLE && pDoor != NULL)
                    {
                        if (pDoor->m_bLocked != 0 && pDoor->m_bKeyRequired == 0 &&
                            nSkillRank > 0 &&
                            nSkillRank + CSWSCreatureStats::SKILL_CHECK_TAKE_20 >= pDoor->GetOpenLockDC())
                        {
                            bResult = 1;
                        }
                    }
                    else if (pPlaceable != NULL && pPlaceable->m_bLocked != 0 &&
                             pPlaceable->m_bKeyRequired == 0 && nSkillRank > 0 &&
                             nSkillRank + CSWSCreatureStats::SKILL_CHECK_TAKE_20 >= pPlaceable->GetOpenLockDC())
                    {
                        bResult = 1;
                    }
                }
                break;
            case DOOR_ACTION_BASH:
                if ((pDoor != NULL && pDoor->m_bPlotObject == 0) ||
                    (pPlaceable != NULL && pPlaceable->m_bPlotObject == 0))
                {
                    bResult = 1;
                }
                break;
            case DOOR_ACTION_IGNORE:
                bResult = 1;
                break;
            case DOOR_ACTION_KNOCK:
                if ((pDoor != NULL && pDoor->m_bPlotObject == 0 && pDoor->m_bKeyRequired == 0) ||
                    (pPlaceable != NULL && pPlaceable->m_bPlotObject == 0 && pPlaceable->m_bKeyRequired == 0))
                {
                    unsigned char nClass = 0;
                    do
                    {
                        bResult = pCreature->GetStats()->GetSpellUsesLeft(CSWSpell::SPELL_KNOCK, nClass, 0, 0) > 0;
                        ++nClass;
                    }
                    while (bResult == 0 && nClass <= pCreature->GetStats()->m_nClassCount);
                }
                break;
            }
        }
        return g_pVirtualMachine->StackPushInteger(bResult) != 0 ? 0 : ERROR_STACK_PUSH;
    }
}
// PC 0x005BC5E0
int CSWVirtualMachineCommands::ExecuteCommandGetIsDusk(int nCommand, int nArgs)
{
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSModule* pModule;

    nResult = false;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pModule = pServerExoApp->GetModule();
    if (pModule != 0)
    {
        if (pModule->m_nTimeDawnDusk == CSWSModule::TIME_DUSK)
        {
            nResult = 1;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BC620
int CSWVirtualMachineCommands::ExecuteCommandGetIsEncounterCreature(
    int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int bEncounterCreature = 0;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL && pCreature->m_oidSpawnedBy != OBJECT_INVALID)
    {
        bEncounterCreature = 1;
    }
    return g_pVirtualMachine->StackPushInteger(bEncounterCreature) != 0
               ? 0
               : -2000;
}
// PC 0x005BC690
int CSWVirtualMachineCommands::ExecuteCommandGetIsImmune(int nCommand, int nArgs)
{

    int bIsImmune = 0;
    unsigned long oidCreature;
    int nImmunityType;
    unsigned long oidVersus;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nImmunityType) == 0)
    {
        return ERROR_STACK_POP;
    }
    oidVersus = OBJECT_INVALID;
    if (nArgs >= 3)
    {
        if (g_pVirtualMachine->StackPopObject(&oidVersus) == 0)
        {
            return ERROR_STACK_POP;
        }
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pVersus = pServerExoApp->GetCreatureByGameObjectID(oidVersus);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()
            ->GetGameObject(oidCreature, &pGameObject) == CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL &&
        nImmunityType > 0 &&
        nImmunityType < CSWSEffectListHandler::IMMUNITY_TYPE_MAX)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        CSWSCreatureStats* pStats = pCreature->m_pStats;
        bIsImmune = pStats->GetEffectImmunity(nImmunityType, pVersus);
    }
    if (g_pVirtualMachine->StackPushInteger(bIsImmune) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BC790
int CSWVirtualMachineCommands::ExecuteCommandGetIsInCombat(int nCommand,
                                                           int nArgs)
{
    unsigned long oidCreature;
    int bInCombat;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    int bOnlyCountReal;

    bInCombat = 0;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    else
    {
        bOnlyCountReal = 0;
        if (nArgs >= 2)
        {
            if (g_pVirtualMachine->StackPopInteger(&bOnlyCountReal) == 0)
            {
                return -2001;
            }
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
        if (pCreature != NULL)
        {
            bInCombat = pCreature->m_bCombatState;
            if (bInCombat != 0 && bOnlyCountReal != 0 && pCreature->GetItemUseState() != 1)
            {
                bInCombat = 0;
            }
        }
        return g_pVirtualMachine->StackPushInteger(bInCombat) != 0 ? 0 : -2000;
    }
}
// PC 0x005BC830
int CSWVirtualMachineCommands::ExecuteCommandGetIsNight(int nCommand, int nArgs)
{
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSModule* pModule;

    nResult = false;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pModule = pServerExoApp->GetModule();
    if (pModule != 0)
    {
        if (pModule->m_nTimeDawnDusk == CSWSModule::TIME_NIGHT)
        {
            nResult = 1;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BC870
int CSWVirtualMachineCommands::ExecuteCommandGetIsOpen(int nCommand, int nArgs)
{
    unsigned long oidObject = OBJECT_INVALID;
    int bIsOpen = 0;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL)
    {
        if (pGameObject->AsSWSDoor() != NULL)
        {
            bIsOpen = pGameObject->AsSWSDoor()->m_nOpenState != CSWSDoor::OPEN_STATE_CLOSED;
        }
        else if (pGameObject->AsSWSPlaceable() != NULL)
        {
            bIsOpen = pGameObject->AsSWSPlaceable()->m_nAnimation == 10075;
        }
    }
    return g_pVirtualMachine->StackPushInteger(bIsOpen) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BC920
int CSWVirtualMachineCommands::ExecuteCommandGetIsRacialTypePlayable(int nCommand,
                                                                   int nArgs)
{
    int nRace = g_pRules->m_nNumRaces;
    int bPlayable = false;
    unsigned long oidCreature;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
            ->GetGameObject(oidCreature, &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        if (pGameObject->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
        {
            nRace = pGameObject->AsSWSCreature()->m_pStats->m_nRace;
        }
        else
        {
            nRace = g_pRules->m_nNumRaces;
        }
    }
    if (g_pRules->m_pRaces[(unsigned short)nRace].m_bPlayerRace != 0)
    {
        bPlayable = true;
    }
    return g_pVirtualMachine->StackPushInteger(bPlayable) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BC9F0
int CSWVirtualMachineCommands::ExecuteCommandGetIsTalentValid(int nCommand,
                                                              int nArgs)
{
    CScriptTalent* pTalent;
    int nResult;

    nResult = 1;
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_TALENT, (void**)&pTalent) == 0)
    {
        return -2001;
    }
    if (pTalent->m_nType == -1)
    {
        nResult = 0;
    }
    delete pTalent;
    pTalent = NULL;
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BCA60
int CSWVirtualMachineCommands::ExecuteCommandGetIsMined(int nCommand, int nArgs)
{
    unsigned long oidObject;
    int bIsMined;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    bIsMined = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL)
    {
        if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
        {
            bIsMined = pGameObject->AsSWSTrigger()->m_bMineTrigger;
        }
        else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            bIsMined = pGameObject->AsSWSPlaceable()->m_nTrapFlag;
        }
        else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
        {
            bIsMined = pGameObject->AsSWSDoor()->m_bTrapFlag;
        }
    }
    return g_pVirtualMachine->StackPushInteger(bIsMined) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BCB00
int CSWVirtualMachineCommands::ExecuteCommandGetIsWeaponEffective(int nCommand,
                                                                  int nArgs)
{
    int nResult;
    unsigned long oidVersus;
    int nOffHand;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidVersus) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nOffHand) == 0)
        {
            return -2001;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pCreature = (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(
            m_oidObject);
        if (pCreature != NULL)
        {
            nResult = pCreature->GetIsWeaponEffective(oidVersus, nOffHand);
        }
        return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
    }
}
// PC 0x005BCB90
int CSWVirtualMachineCommands::ExecuteCommandGetItemACValue(int nCommand,
                                                          int nArgs)
{
    unsigned long oidItem;
    int nArmorClass;
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    nArmorClass = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        nArmorClass = pItem->m_nBaseAC;
        int nProperty = 0;
        if (pItem->m_nPassiveProperties > 0)
        {
            do
            {
                CSWItemProperty* pProperty = pItem->GetPassiveProperty(nProperty);
                if (pProperty != NULL &&
                    pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS)
                {
                    nArmorClass += pProperty->m_nCostValue;
                }
                ++nProperty;
            }
            while (nProperty < pItem->m_nPassiveProperties);
        }
    }
    return g_pVirtualMachine->StackPushInteger(nArmorClass) != 0 ? 0 : -2000;
}
// PC 0x005BCC30
int CSWVirtualMachineCommands::ExecuteCommandGetItemHasItemProperty(int nCommand,
                                                                  int nArgs)
{

    unsigned long oidItem;
    int nProperty;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nProperty) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    int bHasProperty = 0;
    CSWSItem* pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        int nIndex;
        for (nIndex = 0; nIndex < pItem->m_nActiveProperties; ++nIndex)
        {
            CSWItemProperty* pProperty = pItem->GetActiveProperty(nIndex);
            if (pProperty != NULL &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((1 << pProperty->m_nUpgradeType) & pItem->m_nUpgrades) != 0) &&
                pProperty->m_nPropertyType == nProperty)
            {
                bHasProperty = 1;
                goto Exit;
            }
        }
        for (nIndex = 0; nIndex < pItem->m_nPassiveProperties; ++nIndex)
        {
            CSWItemProperty* pProperty = pItem->GetPassiveProperty(nIndex);
            if (pProperty != NULL && pProperty->m_nPropertyType == nProperty)
            {
                bHasProperty = 1;
                break;
            }
        }
    }
Exit:
    if (g_pVirtualMachine->StackPushInteger(bHasProperty) != 0)
        return 0;
    return ERROR_STACK_PUSH;
}
// PC 0x005BCD20
int CSWVirtualMachineCommands::ExecuteCommandGetItemInSlot(int nCommand,
                                                          int nArgs)
{
    int nInventorySlot;
    unsigned long oidCreature;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWInventory* pInventory;
    CSWSItem* pItem;

    if (g_pVirtualMachine->StackPopInteger(&nInventorySlot) == 0)
    {
        return ERROR_STACK_POP;
    }
    nInventorySlot = (int)pow(2.0, nInventorySlot);
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    unsigned long oidItem = CGameObjectArray::INVALIDOBJID;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidCreature,
                                                     &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            pInventory = pCreature->m_pInventory;
            if (pInventory != NULL)
            {
                pItem = pInventory->GetItemInSlot(nInventorySlot);
                if (pItem != NULL)
                {
                    oidItem = pItem->m_nId;
                }
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidItem) != 0)
        return 0;
    return ERROR_STACK_PUSH;
}
// PC 0x005BCDF0
int CSWVirtualMachineCommands::ExecuteCommandGetItemPossessedBy(int nCommand,
                                                              int nArgs)
{

    unsigned long oidItem = OBJECT_INVALID;
    unsigned long oidCreature;
    CGameObject* pGameObject;
    CExoString sItemTag;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopString(&sItemTag) == 0)
    {
        return ERROR_STACK_POP;
    }
    CExoString sLowerTag = sItemTag.LowerCase();
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            oidCreature, &pGameObject) == CGameObjectArray::SUCCESS)
    {
        if (pGameObject->AsSWSCreature() != NULL)
        {
            CSWSCreature* pCreature = pGameObject->AsSWSCreature();
            unsigned long oidFoundItem =
                pCreature->GetItemRepository(1)->FindItemWithTag(sLowerTag);
            int bFound = oidFoundItem != OBJECT_INVALID;
            CSWInventory* pInventory = pCreature->m_pInventory;
            for (int nSlot = 0; nSlot < CSWInventory::TOTAL_SLOT_COUNT && !bFound; ++nSlot)
            {
                CSWSItem* pItem = pInventory->GetItemInSlot(1 << nSlot);
                if (pItem != NULL && pItem->m_sTag == sLowerTag)
                {
                    oidFoundItem = pItem->m_nId;
                    bFound = 1;
                }
            }
            oidItem = oidFoundItem;
        }
        else if (pGameObject->AsSWSPlaceable() != NULL)
        {
            if (pGameObject->AsSWSPlaceable()->m_bHasInventory &&
                pGameObject->AsSWSPlaceable()->m_pcItemRepository != NULL)
            {
                CItemRepository* pRepository =
                    pGameObject->AsSWSPlaceable()->m_pcItemRepository;
                oidItem = pRepository->FindItemWithTag(sLowerTag);
            }
        }
        else if (pGameObject->AsSWSItem() != NULL)
        {
            if (pGameObject->AsSWSItem()->m_pRepository != NULL)
            {
                CItemRepository* pRepository =
                    pGameObject->AsSWSItem()->m_pRepository;
                oidItem = pRepository->FindItemWithTag(sLowerTag);
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidItem) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BD020
int CSWVirtualMachineCommands::ExecuteCommandGetItemPossessor(int nCommand,
                                                            int nArgs)
{

    unsigned long oidItem;
    unsigned long oidPossessor;
    CGameObject* pGameObject;
    CGameObject* pPossessor;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidPossessor = CGameObjectArray::INVALIDOBJID;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            oidItem, &pGameObject) == CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSItem() != NULL)
    {
        oidPossessor = pGameObject->AsSWSItem()->m_oidPossessor;
    }
    pPossessor = g_pAppManager->m_pServerExoApp->GetGameObject(oidPossessor);
    if (pPossessor != NULL && pPossessor->AsSWSItem() != NULL)
    {
        oidPossessor = pPossessor->AsSWSItem()->m_oidPossessor;
    }
    return g_pVirtualMachine->StackPushObject(oidPossessor) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BD0E0
int CSWVirtualMachineCommands::ExecuteCommandGetJournalQuestExperience(
    int nCommand, int nArgs)
{

    CExoString sPlotID;
    unsigned long nXP = 0;
    int bSuccess = 1;
    if (g_pVirtualMachine->StackPopString(&sPlotID) == 0)
    {
        return ERROR_STACK_POP;
    }

    CResGFF* pJournal = new CResGFF(RESTYPE_JRL, "JRL ", "Global");
    if (pJournal->m_bLoaded)
    {
        CResStruct cTopLevel;
        CResList lstCategories;
        CResStruct cCategory;
        pJournal->GetTopLevelStruct(&cTopLevel);
        pJournal->GetList(&lstCategories, &cTopLevel, "Categories");
        int nCategories = pJournal->GetListCount(&lstCategories);
        for (int nCategory = 0; nCategory < nCategories; ++nCategory)
        {
            pJournal->GetListElement(&cCategory, &lstCategories, nCategory);
            if (pJournal->ReadFieldCExoString(
                    &cCategory, "Tag", bSuccess, "") == sPlotID)
            {
                nXP = pJournal->ReadFieldDWORD(&cCategory, "XP", bSuccess, 0);
                break;
            }
        }
        delete pJournal;
    }
    if (g_pVirtualMachine->StackPushInteger(nXP) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BD2E0
int CSWVirtualMachineCommands::ExecuteCommandGetLastAttacker(int nCommand,
                                                             int nArgs)
{
    unsigned long oidObject = OBJECT_INVALID;
    unsigned long oidAttacker;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    oidAttacker = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        oidAttacker = pGameObject->AsSWSObject()->m_oidLastAttacker;
    }
    return g_pVirtualMachine->StackPushObject(oidAttacker) != 0 ? 0 : -2000;
}
// PC 0x005BD360
int CSWVirtualMachineCommands::ExecuteCommandGetLastHostileTarget(int nCommand,
                                                                  int nArgs)
{
    unsigned long oidResult;
    unsigned long oidCreature;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    oidCreature = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        oidResult = pCreature->m_oidLastHostileTarget;
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005BD3D0
int CSWVirtualMachineCommands::ExecuteCommandGetLastAttackAction(int nCommand,
                                                                 int nArgs)
{
    unsigned long oidCreature;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidCreature = OBJECT_INVALID;
    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        nResult = pCreature->m_nLastAttackAction;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BD440
int CSWVirtualMachineCommands::ExecuteCommandGetLastAttackMode(int nCommand,
                                                               int nArgs)
{
    unsigned long oidCreature;
    CGameObject* pGameObject;
    int nAttackMode;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nAttackMode = OBJECT_INVALID;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidCreature, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature->m_bCombatState != 0)
        {
            switch (pCreature->m_nAttackMode)
            {
            case SWSOBJECT_COMBAT_MODE_PARRY:
                nAttackMode = SWSOBJECT_SCRIPT_COMBAT_MODE_PARRY;
                break;
            case SWSOBJECT_COMBAT_MODE_POWER_ATTACK:
                nAttackMode = SWSOBJECT_SCRIPT_COMBAT_MODE_POWER_ATTACK;
                break;
            case SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK:
                nAttackMode = SWSOBJECT_SCRIPT_COMBAT_MODE_IMPROVED_POWER_ATTACK;
                break;
            case SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS:
                nAttackMode = SWSOBJECT_SCRIPT_COMBAT_MODE_FLURRY_OF_BLOWS;
                break;
            case SWSOBJECT_COMBAT_MODE_RAPID_SHOT:
                nAttackMode = SWSOBJECT_SCRIPT_COMBAT_MODE_RAPID_SHOT;
                break;
            default:
                nAttackMode = SWSOBJECT_SCRIPT_COMBAT_MODE_INVALID;
                break;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nAttackMode) != 0
        ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BD530
int CSWVirtualMachineCommands::ExecuteCommandGetLastAttackResult(int nCommand,
                                                                 int nArgs)
{
    unsigned long oidCreature;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidCreature = OBJECT_INVALID;
    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        nResult = pCreature->m_nLastAttackResult;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BD5A0
int CSWVirtualMachineCommands::ExecuteCommandGetWasForcePowerSuccessful(
    int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int bResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidCreature = OBJECT_INVALID;
    bResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        bResult = pCreature->m_bLastForcePowerSuccessful;
    }
    return g_pVirtualMachine->StackPushInteger(bResult) != 0 ? 0 : -2000;
}
// PC 0x005BD610
int CSWVirtualMachineCommands::ExecuteCommandGetLastAttackType(int nCommand,
                                                             int nArgs)
{
    unsigned long oidCreature;
    CGameObject* pGameObject;
    int nAttackType;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nAttackType = SWSOBJECT_SCRIPT_ATTACK_NONE;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidCreature, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature->m_bCombatState != 0)
        {
            switch (pCreature->m_nAttackType)
            {
            case SWSOBJECT_COMBAT_ATTACK_MELEE:
                nAttackType = SWSOBJECT_SCRIPT_ATTACK_MELEE;
                break;
            case SWSOBJECT_COMBAT_ATTACK_RANGED:
                nAttackType = SWSOBJECT_SCRIPT_ATTACK_RANGED;
                break;
            default:
                nAttackType = SWSOBJECT_SCRIPT_ATTACK_NONE;
                break;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nAttackType) != 0
        ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BD6C0
int CSWVirtualMachineCommands::ExecuteCommandGetLastClosedBy(int nCommand,
                                                             int nArgs)
{
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    unsigned long oidCloser;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    oidCloser = OBJECT_INVALID;
    if (pGameObject != NULL)
    {
        if (pGameObject->AsSWSPlaceable() != NULL)
        {
            oidCloser = pGameObject->AsSWSPlaceable()->m_oidLastClosedBy;
        }
        else if (pGameObject->AsSWSDoor() != NULL)
        {
            oidCloser = pGameObject->AsSWSDoor()->m_oidLastCloser;
        }
    }
    return g_pVirtualMachine->StackPushObject(oidCloser) != 0 ? 0 : -2000;
}
// PC 0x005BD730
int CSWVirtualMachineCommands::ExecuteCommandGetLastCombatFeatUsed(int nCommand,
                                                                   int nArgs)
{
    unsigned long oidCreature;
    int nFeat;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidCreature = OBJECT_INVALID;
    nFeat = 0;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        nFeat = pCreature->m_nLastCombatFeatUsed;
    }
    return g_pVirtualMachine->StackPushInteger(nFeat) != 0 ? 0 : -2000;
}
// PC 0x005BD7A0
int CSWVirtualMachineCommands::ExecuteCommandGetLastDamager(int nCommand,
                                                            int nArgs)
{
    unsigned long oidDamager;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    oidDamager = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        pObject = pGameObject->AsSWSObject();
        if (pObject != NULL)
        {
            oidDamager = pObject->m_oidLastDamager;
        }
    }
    return g_pVirtualMachine->StackPushObject(oidDamager) != 0 ? 0 : -2000;
}
// PC 0x005BD800
int CSWVirtualMachineCommands::ExecuteCommandGetLastDisarmed(int nCommand,
                                                             int nArgs)
{
    unsigned long oidDisarmer;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    oidDisarmer = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        pObject = pGameObject->AsSWSObject();
    }
    if (pObject != NULL)
    {
        if (pObject->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
        {
            oidDisarmer = pObject->AsSWSPlaceable()->m_oidLastDisarmedBy;
        }
        else if (pObject->m_nObjectType == SW_OBJECT_TYPE_TRIGGER)
        {
            oidDisarmer = pObject->AsSWSTrigger()->m_oidLastDisarmedBy;
        }
        else if (pObject->m_nObjectType == SW_OBJECT_TYPE_DOOR)
        {
            oidDisarmer = pObject->AsSWSDoor()->m_oidLastDisarmedBy;
        }
    }
    return g_pVirtualMachine->StackPushObject(oidDisarmer) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BD8A0
int CSWVirtualMachineCommands::ExecuteCommandGetLastDisturbed(int nCommand,
                                                              int nArgs)
{
    unsigned long oidDisturber;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    oidDisturber = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        pObject = pGameObject->AsSWSObject();
        if (pObject != NULL)
        {
            if (pObject->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
            {
                oidDisturber = pObject->AsSWSCreature()->m_oidLastDisturbed;
            }
            else if (pObject->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
            {
                oidDisturber = pObject->AsSWSPlaceable()->m_oidLastDisturbed;
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidDisturber) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BD920
int CSWVirtualMachineCommands::ExecuteCommandGetLastForcePowerUsed(int nCommand,
                                                                   int nArgs)
{
    unsigned long oidCreature;
    int nPower;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidCreature = OBJECT_INVALID;
    nPower = 0;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        nPower = pCreature->m_nLastForcePowerUsed;
    }
    return g_pVirtualMachine->StackPushInteger(nPower) != 0 ? 0 : -2000;
}
// PC 0x005BD990
int CSWVirtualMachineCommands::ExecuteCommandGetLastHostileActor(int nCommand,
                                                                 int nArgs)
{
    unsigned long oidResult;
    unsigned long oidVictim;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;
    CGameObject* pActor;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    oidVictim = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidVictim) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidVictim);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != 0)
    {
        pObject = (CSWSObject*)pGameObject->AsSWSObject();
        if (pObject != 0)
        {
            if (pObject->m_bPlotObject == 1)
            {
                if (pObject->m_oidLastHostileActor != OBJECT_INVALID)
                {
                    pObject->SetLastHostileActor(OBJECT_INVALID, 1);
                }
                oidResult = OBJECT_INVALID;
            }
            else
            {
                oidResult = pObject->m_oidLastHostileActor;
                pActor = g_pAppManager->m_pServerExoApp->GetGameObject(oidResult);
                if (pActor != NULL)
                {
                    if (pActor->m_nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT)
                    {
                        oidResult = pActor->AsSWSAreaOfEffectObject()->m_oidCreator;
                        pCreature = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidResult);
                    }
                    else if (pActor->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                    {
                        pCreature = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidResult);
                    }
                    else
                    {
                        pCreature = NULL;
                    }
                    if (pCreature == NULL ||
                        pCreature->GetDead() == 1 ||
                        pCreature->GetIsPCDying() == 1 ||
                        pCreature->GetDeadTemp() != 0)
                    {
                        pObject->SetLastHostileActor(OBJECT_INVALID, 1);
                        oidResult = OBJECT_INVALID;
                    }
                }
                else
                {
                    pObject->SetLastHostileActor(OBJECT_INVALID, 1);
                    oidResult = OBJECT_INVALID;
                }
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BDAC0
int CSWVirtualMachineCommands::ExecuteCommandGetLastKiller(int nCommand,
                                                           int nArgs)
{
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    oidResult = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != 0)
    {
        pObject = (CSWSObject*)pGameObject->AsSWSObject();
        if (pObject != 0)
        {
            oidResult = pObject->m_n154;
        }
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005BDB20
int CSWVirtualMachineCommands::ExecuteCommandGetLastLocked(int nCommand,
                                                           int nArgs)
{
    unsigned long oidLocker = OBJECT_INVALID;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        pObject = pGameObject->AsSWSObject();
        if (pObject != NULL)
        {
            if (pObject->m_nObjectType == SW_OBJECT_TYPE_DOOR)
            {
                oidLocker = pObject->AsSWSDoor()->m_oidLastLockedBy;
            }
            else if (pObject->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
            {
                oidLocker = pObject->AsSWSPlaceable()->m_oidLastLockedBy;
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidLocker) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BDBA0
int CSWVirtualMachineCommands::ExecuteCommandGetLastOpenedBy(int nCommand,
                                                             int nArgs)
{
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    unsigned long oidOpener;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    oidOpener = OBJECT_INVALID;
    if (pGameObject != NULL)
    {
        if (pGameObject->AsSWSPlaceable() != NULL)
        {
            oidOpener = pGameObject->AsSWSPlaceable()->m_oidLastOpenedBy;
        }
        else if (pGameObject->AsSWSDoor() != NULL)
        {
            oidOpener = pGameObject->AsSWSDoor()->m_oidLastOpener;
        }
    }
    return g_pVirtualMachine->StackPushObject(oidOpener) != 0 ? 0 : -2000;
}
// PC 0x005BDC10
int CSWVirtualMachineCommands::ExecuteCommandGetLastPazaakResult(int nCommand,
                                                                 int nArgs)
{
    int nResult = 0;
    int nUnused = 0;

    g_pAppManager->m_pClientExoApp->GetPazaakResults(&nResult, &nUnused);
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BDC60
int CSWVirtualMachineCommands::ExecuteCommandGetLastPerceived(int nCommand,
                                                               int nArgs)
{
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    unsigned long oidLastPerceived;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    ((CGameObjectArray*)pServerExoApp->GetObjectArray())
        ->GetGameObject(m_oidObject, &pGameObject);
    oidLastPerceived = CGameObjectArray::INVALIDOBJID;
    if (pGameObject->AsSWSCreature() != NULL)
    {
        oidLastPerceived =
            pGameObject->AsSWSCreature()->m_oidLastPerceived;
    }
    return g_pVirtualMachine->StackPushObject(oidLastPerceived) != 0 ? 0
                                                                     : -2000;
}
// PC 0x005BDCD0
int CSWVirtualMachineCommands::ExecuteCommandGetLastPerception(int nCommand,
                                                              int nArgs)
{
    int nResult = 0;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (nCommand == COMMAND_GET_LAST_PERCEPTION_HEARD)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL)
        {
            nResult = pGameObject->AsSWSCreature()->m_bLastPerceptionHeard;
        }
    }
    else if (nCommand == COMMAND_GET_LAST_PERCEPTION_INAUDIBLE)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL)
        {
            nResult = pGameObject->AsSWSCreature()->m_bLastPerceptionInaudible;
        }
    }
    else if (nCommand == COMMAND_GET_LAST_PERCEPTION_SEEN)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL)
        {
            nResult = pGameObject->AsSWSCreature()->m_bLastPerceptionSeen;
        }
    }
    else if (nCommand == COMMAND_GET_LAST_PERCEPTION_VANISHED)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL)
        {
            nResult = pGameObject->AsSWSCreature()->m_bLastPerceptionVanished;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0
                                                           : ERROR_STACK_PUSH;
}
// PC 0x005BDE40
int CSWVirtualMachineCommands::ExecuteCommandGetLastSpeaker(int nCommand,
                                                             int nArgs)
{
    unsigned long oidLastSpeaker = OBJECT_INVALID;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject,
                                                        &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL)
    {
        oidLastSpeaker =
            pGameObject->AsSWSCreature()->m_oidLastSpeaker;
    }
    return g_pVirtualMachine->StackPushObject(oidLastSpeaker) != 0 ? 0
                                                                    : -2000;
}
// PC 0x005BDEB0
int CSWVirtualMachineCommands::ExecuteCommandGetLastMineDetected(int nCommand,
                                                                 int nArgs)
{
    unsigned long oidCreature;
    unsigned long oidMine;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    oidMine = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        oidMine = pCreature->m_oidLastMineDetected;
    }
    return g_pVirtualMachine->StackPushObject(oidMine) != 0 ? 0 : -2000;
}
// PC 0x005BDF10
int CSWVirtualMachineCommands::ExecuteCommandGetLastUnlocked(int nCommand,
                                                             int nArgs)
{
    unsigned long oidUnlocker = OBJECT_INVALID;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        pObject = pGameObject->AsSWSObject();
        if (pObject != NULL)
        {
            if (pObject->m_nObjectType == SW_OBJECT_TYPE_DOOR)
            {
                oidUnlocker = pObject->AsSWSDoor()->m_oidLastUnlockedBy;
            }
            else if (pObject->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
            {
                oidUnlocker = pObject->AsSWSPlaceable()->m_oidLastUnlockedBy;
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidUnlocker) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BDF90
int CSWVirtualMachineCommands::ExecuteCommandGetLastUsedBy(int nCommand,
                                                           int nArgs)
{
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    unsigned long oidUser;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    oidUser = OBJECT_INVALID;
    if (pGameObject->AsSWSPlaceable() != NULL)
    {
        oidUser = pGameObject->AsSWSPlaceable()->m_oidLastUsedBy;
    }
    else if (pGameObject->AsSWSDoor() != NULL)
    {
        oidUser = pGameObject->AsSWSDoor()->m_oidLastOpener;
    }
    return g_pVirtualMachine->StackPushObject(oidUser) != 0 ? 0 : -2000;
}
// PC 0x005BE000
int CSWVirtualMachineCommands::ExecuteCommandGetLastWeaponUsed(int nCommand,
                                                               int nArgs)
{
    unsigned long oidCreature;
    unsigned long oidWeapon;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    oidWeapon = OBJECT_INVALID;
    if (pCreature != 0)
    {
        oidWeapon = pCreature->m_oidLastWeaponUsed;
    }
    return g_pVirtualMachine->StackPushObject(oidWeapon) != 0 ? 0 : -2000;
}
// PC 0x005BE060
int CSWVirtualMachineCommands::ExecuteCommandGetLeavingObject(int nCommand,
                                                             int nArgs)
{
    unsigned long oidLeaving = CGameObjectArray::INVALIDOBJID;
    CGameObject* pGameObject;

    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            switch (pGameObject->m_nObjectType)
            {
            case SW_OBJECT_TYPE_TRIGGER:
            {
                CSWSTrigger* pTrigger = pGameObject->AsSWSTrigger();
                if (pTrigger != NULL)
                    oidLeaving = pTrigger->m_oidLastLeft;
                break;
            }
            case SW_OBJECT_TYPE_AREA_OF_EFFECT:
            {
                CSWSAreaOfEffectObject* pAreaOfEffect =
                    pGameObject->AsSWSAreaOfEffectObject();
                if (pAreaOfEffect != NULL)
                    oidLeaving = pAreaOfEffect->m_oidLastLeft;
                break;
            }
            case SW_OBJECT_TYPE_MODULE:
            {
                CSWSModule* pModule = pGameObject->AsSWSModule();
                if (pModule != NULL)
                    oidLeaving = pModule->m_oidClientLeave;
                break;
            }
            case SW_OBJECT_TYPE_AREA:
            {
                CSWSArea* pArea = pGameObject->AsSWSArea();
                if (pArea != NULL)
                    oidLeaving = pArea->m_oidExitingObject;
                break;
            }
            case SW_OBJECT_TYPE_ENCOUNTER:
            {
                CSWSEncounter* pEncounter = pGameObject->AsSWSEncounter();
                if (pEncounter != NULL)
                    oidLeaving = pEncounter->m_oidLastLeft;
                break;
            }
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidLeaving) != 0 ?
        0 : ERROR_STACK_PUSH;
}
// PC 0x005BE150
int CSWVirtualMachineCommands::ExecuteCommandGetLoadFromSaveGame(int nCommand,
                                                                 int nArgs)
{
    CServerExoApp* pServerExoApp;
    int nResult;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nResult = pServerExoApp->GetLoadFromSaveGame();
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BE180
int CSWVirtualMachineCommands::ExecuteCommandGetLocalValue(int nCommand,
                                                          int nArgs)
{
    int nResult = 0;
    unsigned long oidObject;
    int nIndex;
    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nIndex) == 0)
        {
            return ERROR_STACK_POP;
        }

        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        CGameObject* pGameObject = pServerExoApp->GetGameObject(oidObject);
        if (pGameObject != NULL)
        {
            CSWVarTable* pVarTable;
            int nObjectType = pGameObject->m_nObjectType;
            if (nObjectType == CGameObject::OBJECT_TYPE_AREA)
            {
                pVarTable = &pGameObject->AsSWSArea()->m_lVarTable;
            }
            else if (nObjectType == CGameObject::OBJECT_TYPE_MODULE)
            {
                pVarTable = &pGameObject->AsSWSModule()->m_lVarTable;
            }
            else
            {
                pVarTable = &((CSWSObject*)pGameObject)->m_lVarTable;
            }
            if (pVarTable != NULL)
            {
                switch (nCommand)
                {
                case COMMAND_GET_LOCAL_BOOLEAN:
                    nResult = pVarTable->GetLocalBoolean(nIndex);
                    break;
                case COMMAND_GET_LOCAL_NUMBER:
                    nResult = pVarTable->GetLocalNumber(nIndex);
                    break;
                }
            }
        }
        return g_pVirtualMachine->StackPushInteger(nResult) != 0
                   ? 0 : ERROR_STACK_PUSH;
    }
}
// PC 0x005BE260
int CSWVirtualMachineCommands::ExecuteCommandGetLocation(int nCommand, int nArgs)
{
    unsigned long oidTarget;
    CGameObject* pGameObject;
    CScriptLocation lLocation;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidTarget, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL &&
        pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        lLocation = ((CSWSObject*)pGameObject)->GetScriptLocation();
    }
    if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_LOCATION,
                                                   &lLocation) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BE380
int CSWVirtualMachineCommands::ExecuteCommandGetLocked(int nCommand, int nArgs)
{
    int bLocked = 0;
    unsigned long oidTarget;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            oidTarget, &pGameObject) == CGameObjectArray::SUCCESS)
    {
        if (pGameObject->AsSWSDoor() != NULL)
        {
            bLocked = pGameObject->AsSWSDoor()->m_bLocked;
        }
        else if (pGameObject->AsSWSPlaceable() != NULL)
        {
            bLocked = pGameObject->AsSWSPlaceable()->m_bLocked;
        }
    }
    return g_pVirtualMachine->StackPushInteger(bLocked) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BE430
int CSWVirtualMachineCommands::ExecuteCommandGetLockInfo(int nCommand, int nArgs)
{

    unsigned long oidTarget;
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pGameObject = pServerExoApp->GetGameObject(oidTarget);
    if (nCommand == COMMAND_GET_LOCK_KEY_REQUIRED)
    {
        int bKeyRequired = 0;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                bKeyRequired = pGameObject->AsSWSDoor()->m_bKeyRequired;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                bKeyRequired = pGameObject->AsSWSPlaceable()->m_bKeyRequired;
        }
        if (g_pVirtualMachine->StackPushInteger(bKeyRequired) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_LOCK_KEY_TAG)
    {
        CExoString* psKeyName = NULL;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                psKeyName = &pGameObject->AsSWSDoor()->m_sKeyName;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                psKeyName = &pGameObject->AsSWSPlaceable()->m_sKeyName;
        }
        if (psKeyName == NULL)
        {
            if (g_pVirtualMachine->StackPushString("") == 0)
                return ERROR_STACK_PUSH;
        }
        else
        {
            if (g_pVirtualMachine->StackPushString(*psKeyName) == 0)
                return ERROR_STACK_PUSH;
        }
    }
    else if (nCommand == COMMAND_GET_LOCK_LOCKABLE)
    {
        int bLockable = 0;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                bLockable = pGameObject->AsSWSDoor()->m_bLockable;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                bLockable = pGameObject->AsSWSPlaceable()->m_bLockable;
        }
        if (g_pVirtualMachine->StackPushInteger(bLockable) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_LOCK_UNLOCK_DC)
    {
        int nUnlockDC = 0;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                nUnlockDC = pGameObject->AsSWSDoor()->GetOpenLockDC();
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                nUnlockDC = pGameObject->AsSWSPlaceable()->GetOpenLockDC();
        }
        if (g_pVirtualMachine->StackPushInteger(nUnlockDC) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_LOCK_LOCK_DC)
    {
        int nLockDC = 0;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                nLockDC = pGameObject->AsSWSDoor()->m_nCloseLockDC;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                nLockDC = pGameObject->AsSWSPlaceable()->m_nCloseLockDC;
        }
        if (g_pVirtualMachine->StackPushInteger(nLockDC) == 0)
            return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BE6E0
int CSWVirtualMachineCommands::ExecuteCommandGetMatchedStringsCount(
    int nCommand, int nArgs)
{
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    nResult = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)(CGameObjectArray*)pServerExoApp->GetObjectArray()
                ->GetGameObject(m_oidObject, (struct CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL)
    {
        nResult = pObject->m_lstMatchedStrings.num;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BE740
int CSWVirtualMachineCommands::ExecuteCommandGetMatchedStringsNum(int nCommand,
                                                                   int nArgs)
{
    int nMatchedStringsNum;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    nMatchedStringsNum = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            m_oidObject, (CGameObject**)&pObject) == CGameObjectArray::SUCCESS &&
        pObject != NULL)
    {
    nMatchedStringsNum = pObject->m_nMatchedStringsNum;
    }
    return g_pVirtualMachine->StackPushInteger(nMatchedStringsNum) != 0 ? 0 :
                                                                        -2000;
}
// PC 0x005BE7A0
int CSWVirtualMachineCommands::ExecuteCommandGetMetaMagicFeat(int nCommand,
                                                              int nArgs)
{
    unsigned char nResult;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    nResult = -1;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != 0)
    {
        pObject = (CSWSObject*)pGameObject->AsSWSObject();
        if (pObject != 0)
        {
            nResult = pObject->m_c1D5;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BE800
int CSWVirtualMachineCommands::ExecuteCommandGetModule(int nCommand, int nArgs)
{
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSModule* pModule;

    oidResult = OBJECT_INVALID;
    if (g_pAppManager != NULL)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp != NULL)
        {
            pModule = pServerExoApp->GetModule();
            if (pModule != 0)
            {
                oidResult = pModule->m_nId;
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005BE840
int CSWVirtualMachineCommands::ExecuteCommandGetModuleItemStuff(int nCommand,
                                                                int nArgs)
{
    unsigned long oidResult;
    CSWSModule* pModule;

    oidResult = OBJECT_INVALID;
    pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        switch (nCommand)
        {
        case 282:
            oidResult = pModule->m_oidAcquiredItem;
            break;
        case 283:
            oidResult = pModule->m_oidItemAcquirer;
            break;
        case 292:
            oidResult = pModule->m_oidUnacquiredItem;
            break;
        case 293:
            oidResult = pModule->m_oidItemUnacquirer;
            break;
        }
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005BE8D0
int CSWVirtualMachineCommands::ExecuteCommandGetModuleName(int nCommand, int nArgs)
{

    CExoString sResult;

    switch (nCommand)
    {
    case COMMAND_GET_MODULE_NAME:
        sResult = g_pAppManager->GetServerExoApp()->GetModuleName();
        break;
    case COMMAND_GET_MODULE_FILE_NAME:
        {
            sResult = g_pAppManager->GetServerExoApp()->GetModule()->m_sModuleName;
            int nSeparator = sResult.Find(':', 0);
            if (nSeparator != -1)
            {
                sResult = sResult.Right(sResult.GetLength() - nSeparator - 1);
            }
        }
        break;
    }

    if (g_pVirtualMachine->StackPushString(sResult) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BEA00
int CSWVirtualMachineCommands::ExecuteCommandGetMovementRate(int nCommand,
                                                             int nArgs)
{
    unsigned long oidCreature;
    int nRate;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWSCreatureStats* pStats;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    nRate = 0;
    if (pCreature != 0)
    {
        pStats = pCreature->m_pStats;
        if (pStats != 0)
        {
            nRate = pStats->m_nMovementRate;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nRate) != 0 ? 0 : -2000;
}
// PC 0x005BEA70
int CSWVirtualMachineCommands::ExecuteCommandGetName(int nCommand, int nArgs)
{

    CExoString sName;
    unsigned long oidObject;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidObject, &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        if (pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
        {
            CSWSMessage::GetLocStringServer(
                (unsigned long)-1, *pGameObject->AsSWSObject()->GetFirstName(),
                pGameObject->AsSWSObject()->GetLastName(), sName, 0);
        }
    }

    if (g_pVirtualMachine->StackPushString(sName) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BEBA0
int CSWVirtualMachineCommands::ExecuteCommandGetNightTrack(int nCommand,
                                                           int nArgs)
{
    unsigned long oidArea;
    int nTrack;
    CServerExoApp* pServerExoApp;
    CSWSArea* pArea;

    nTrack = 0;
    if (g_pVirtualMachine->StackPopObject(&oidArea) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pArea = pServerExoApp->GetAreaByGameObjectID(oidArea);
    if (pArea != NULL)
    {
        nTrack = pArea->m_pAmbientSound->m_nMusicNightTrack;
    }
    return g_pVirtualMachine->StackPushInteger(nTrack) != 0 ? 0 : -2000;
}
// PC 0x005BEC10
int CSWVirtualMachineCommands::ExecuteCommandGetNumStackedItems(int nCommand,
                                                                int nArgs)
{
    unsigned long oidItem;
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;
    int nResult;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        nResult = pItem->m_nStackSize;
    }
    else
    {
        nResult = -1;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BEC70
int CSWVirtualMachineCommands::ExecuteCommandGetObjectByTag(int nCommand,
                                                          int nArgs)
{

    CExoString sTag;
    int nNth;
    unsigned long oidObject;

    if (g_pVirtualMachine->StackPopString(&sTag) == 0 ||
        g_pVirtualMachine->StackPopInteger(&nNth) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp;
    oidObject = (pServerExoApp = g_pAppManager->m_pServerExoApp)->GetModule()
                    ->FindObjectByTagOrdinal(sTag.LowerCase(),
                                             (unsigned long)nNth);
    if (g_pVirtualMachine->StackPushObject(oidObject) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BED80
int CSWVirtualMachineCommands::ExecuteCommandGetObjectInArea(int nCommand,
                                                           int nArgs)
{

    unsigned long oidArea;
    CGameObject* pGameObject;
    int nObjectFilter;
    CSWSArea* pArea = NULL;
    CExoString sString;
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    unsigned long oidObject = CGameObjectArray::INVALIDOBJID;

    if (nArgs >= 1)
    {
        if (g_pVirtualMachine->StackPopObject(&oidArea) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (pObjectArray->GetGameObject(oidArea, &pGameObject) ==
                CGameObjectArray::SUCCESS && pGameObject != NULL)
        {
            pArea = pGameObject->AsSWSArea();
        }
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nObjectFilter) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    else
    {
        nObjectFilter = OBJECT_TYPE_ALL;
    }

    if (pArea == NULL)
    {
        if (pObjectArray->GetGameObject(m_oidObject, &pGameObject) ==
                CGameObjectArray::SUCCESS && pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
            {
                pArea = ((CSWSObject*)pGameObject)->GetArea();
            }
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREA)
            {
                pArea = pGameObject->AsSWSArea();
            }
        }
    }

    if (pArea != NULL)
    {
        if (nCommand == COMMAND_GET_FIRST_OBJECT_IN_AREA)
        {
            if (pArea->GetFirstObjectInArea(oidObject) == 0)
            {
                oidObject = CGameObjectArray::INVALIDOBJID;
            }
        }
        else
        {
            if (pArea->GetNextObjectInArea(oidObject) == 0)
            {
                oidObject = CGameObjectArray::INVALIDOBJID;
            }
        }

        CGameObject* pAreaObject;
        while (pObjectArray->GetGameObject(oidObject, &pAreaObject) ==
               CGameObjectArray::SUCCESS)
        {
            int nObjectType = pAreaObject->m_nObjectType;
            if (nObjectType == 0 ||
                ((nObjectFilter & OBJECT_TYPE_CREATURE) &&
                 nObjectType == SW_OBJECT_TYPE_CREATURE) ||
                ((nObjectFilter & OBJECT_TYPE_ITEM) &&
                 nObjectType == SW_OBJECT_TYPE_ITEM) ||
                ((nObjectFilter & OBJECT_TYPE_TRIGGER) &&
                 nObjectType == SW_OBJECT_TYPE_TRIGGER) ||
                ((nObjectFilter & OBJECT_TYPE_DOOR) &&
                 nObjectType == SW_OBJECT_TYPE_DOOR) ||
                ((nObjectFilter & OBJECT_TYPE_AREA_OF_EFFECT) &&
                 nObjectType == SW_OBJECT_TYPE_AREA_OF_EFFECT) ||

                ((nObjectFilter & OBJECT_TYPE_WAYPOINT) && nObjectType == 12) ||
                ((nObjectFilter & OBJECT_TYPE_PLACEABLE) &&
                 nObjectType == SW_OBJECT_TYPE_PLACEABLE) ||
                ((nObjectFilter & OBJECT_TYPE_STORE) && nObjectType == 14) ||
                ((nObjectFilter & OBJECT_TYPE_ENCOUNTER) &&
                 nObjectType == SW_OBJECT_TYPE_ENCOUNTER) ||
                ((nObjectFilter & OBJECT_TYPE_SOUND) && nObjectType == 16))
            {
                break;
            }
            if (pArea->GetNextObjectInArea(oidObject) == 0)
            {
                oidObject = CGameObjectArray::INVALIDOBJID;
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidObject) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BF000
int CSWVirtualMachineCommands::ExecuteCommandGetObjectType(int nCommand,
                                                           int nArgs)
{
    int nResult;
    unsigned long oidTarget;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    unsigned char nResult2;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
            ->GetGameObject(oidTarget, &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        nResult2 = pGameObject->m_nObjectType;
        if (nResult2 > (unsigned char)CGameObject::OBJECT_TYPE_AREA)
        {
            switch (nResult2)
            {
            case 5:
                nResult = 1;
                break;
            case 6:
                nResult = 2;
                break;
            case 7:
                nResult = 4;
                break;
            case 0xA:
                nResult = 8;
                break;
            case 0xB:
                nResult = 16;
                break;
            case 0xC:
                nResult = 32;
                break;
            case 9:
                nResult = 64;
                break;
            case 0xE:
                nResult = 128;
                break;
            case 0xD:
                nResult = 256;
                break;
            case 0x10:
                nResult = 512;
                break;
            default:
                nResult = 0x7FFF;
                break;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BF110
int CSWVirtualMachineCommands::ExecuteCommandGetObjectValid(int nCommand,
                                                            int nArgs)
{
    unsigned long oidObject;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    int nResult;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL)
    {
        nResult = 1;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BF170
int CSWVirtualMachineCommands::ExecuteCommandGetObjectVisibility(int nCommand,
                                                                 int nArgs)
{
    unsigned long oidTarget;
    unsigned long oidSource;
    CGameObject* pSource;
    CSWVisibilityNode* pNode;
    int nResult = 0;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidSource) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidSource,
            &pSource) == CGameObjectArray::SUCCESS &&
        pSource->AsSWSCreature() != NULL)
    {
        if (nCommand == COMMAND_GET_OBJECT_SEEN)
        {
            pNode = pSource->AsSWSCreature()->GetVisibleListElement(oidTarget);
            if (pNode != NULL)
            {
                nResult = pNode->m_bSeen;
            }
        }
        else if (nCommand == COMMAND_GET_OBJECT_HEARD)
        {
            pNode = pSource->AsSWSCreature()->GetVisibleListElement(oidTarget);
            if (pNode != NULL)
            {
                nResult = pNode->m_bHeard;
            }
        }
    }

    if (g_pVirtualMachine->StackPushInteger(nResult) != 0)
        return 0;
    return ERROR_STACK_PUSH;
}
// PC 0x005BF8F0
int CSWVirtualMachineCommands::ExecuteCommandGetPartyMemberByIndex(int nCommand,
                                                                   int nArgs)
{
    int nIndex;
    unsigned long oidResult;
    CClientExoApp* pClientExoApp;
    CSWCCreature* pCharacter;

    if (g_pVirtualMachine->StackPopInteger(&nIndex) == 0)
    {
        return -2001;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    oidResult = OBJECT_INVALID;
    pCharacter = pClientExoApp->GetSWParty()->GetCharacter(nIndex);
    if (pCharacter != 0)
    {
        oidResult = g_pAppManager->m_pServerExoApp->ClientToServerObjectId(
            pCharacter->m_nId);
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005BF970
int CSWVirtualMachineCommands::ExecuteCommandGetPartyMemberCount(int nCommand,
                                                                 int nArgs)
{
    int nPartyMembers;

    nPartyMembers = g_pAppManager->m_pClientExoApp->GetSWParty()->m_nCharacters;
    return g_pVirtualMachine->StackPushInteger(nPartyMembers) != 0 ? 0 : -2000;
}
// PC 0x005BF9A0
int CSWVirtualMachineCommands::ExecuteCommandGetPC(int nCommand, int nArgs)
{
    unsigned long oidPlayer;

    oidPlayer = OBJECT_INVALID;
    if (nCommand == 548)
    {
        g_pAppManager->m_pServerExoApp->GetPartyTable()->m_n254 = 0;
        g_pAppManager->m_pClientExoApp->GetSWParty();
        oidPlayer = g_pAppManager->m_pServerExoApp->GetFirstPCObject();
    }
    else if (nCommand == 549)
    {
        oidPlayer = g_pAppManager->m_pServerExoApp->GetPartyTable()->GetNextPC();
    }
    return g_pVirtualMachine->StackPushObject(oidPlayer) != 0 ? 0 : -2000;
}
// PC 0x005BFA20
int CSWVirtualMachineCommands::ExecuteCommandGetPCLevellingUp(int nCommand,
                                                              int nArgs)
{
    unsigned long oidPlayer;
    CSWSModule* pModule;

    oidPlayer = OBJECT_INVALID;
    pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        oidPlayer = pModule->m_oidLevelingPlayer;
    }
    return g_pVirtualMachine->StackPushObject(oidPlayer) != 0 ? 0 : -2000;
}
// PC 0x005CD430
int CSWVirtualMachineCommands::ExecuteCommandGetPCSpeaker(int nCommand,
                                                          int nArgs)
{
    unsigned long oidPCSpeaker = OBJECT_INVALID;
    CSWCCreature* pCharacter = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
    if (pCharacter != NULL)
    {
        oidPCSpeaker = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(pCharacter->GetId());
    }
    return g_pVirtualMachine->StackPushObject(oidPCSpeaker) != 0 ? 0 : -2000;
}
// PC 0x005BFA60
int CSWVirtualMachineCommands::ExecuteCommandGetPlaceableIllumination(
    int nCommand, int nArgs)
{
    unsigned long oidPlaceable;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSPlaceable* pPlaceable;

    if (g_pVirtualMachine->StackPopObject(&oidPlaceable) == 0)
    {
        return -2001;
    }
    nResult = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pPlaceable =
        (CSWSPlaceable*)pServerExoApp->GetPlaceableByGameObjectID(oidPlaceable);
    if (pPlaceable != NULL)
    {
        nResult = pPlaceable->GetLightIsOn();
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BFAC0
int CSWVirtualMachineCommands::ExecuteCommandGetPlotFlag(int nCommand,
                                                         int nArgs)
{
    unsigned long oidObject;
    int bPlot = 0;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        if (pGameObject->AsSWSObject()->m_bPlotObject != 0)
        {
            bPlot = 1;
        }
    }
    return g_pVirtualMachine->StackPushInteger(bPlot) != 0 ? 0 : -2000;
}
// PC 0x005BFB40
int CSWVirtualMachineCommands::ExecuteCommandGetPosition(int nCommand, int nArgs)
{
    unsigned long oidTarget;
    CGameObject* pGameObject;
    Vector vPosition;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidTarget, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL &&
        pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        vPosition = ((CSWSObject*)pGameObject)->m_vPosition;
    }
    return g_pVirtualMachine->StackPushVector(vPosition) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BFC00
int CSWVirtualMachineCommands::ExecuteCommandGetRacialType(int nCommand,
                                                          int nArgs)
{
    int nRace = 0;
    unsigned long oidCreature;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
            ->GetGameObject(oidCreature, &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        if (pGameObject->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
        {
            nRace = pGameObject->AsSWSCreature()->m_pStats->m_nRace;
        }
        else
        {
            nRace = 0;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nRace) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005BFC90
int CSWVirtualMachineCommands::ExecuteCommandGetReflexAdjustedDamage(
    int nCommand, int nArgs)
{

    int nDamage;
    unsigned long oidTarget;
    int nDC = 0;
    int nSaveType;
    unsigned long oidSaveVersus;

    if (nArgs == 4)
    {
        if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (g_pVirtualMachine->StackPopInteger(&nDamage) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (g_pVirtualMachine->StackPopInteger(&nSaveType) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (g_pVirtualMachine->StackPopObject(&oidSaveVersus) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nDamage) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (g_pVirtualMachine->StackPopInteger(&nDC) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (g_pVirtualMachine->StackPopInteger(&nSaveType) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (g_pVirtualMachine->StackPopObject(&oidSaveVersus) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature =
        pServerExoApp->GetCreatureByGameObjectID(oidTarget);
    if (pCreature != NULL &&
        pCreature->SavingThrowRoll(CSWSCreature::SAVING_THROW_REFLEX,
                                  nDC, nSaveType, oidSaveVersus, 1, 0, 0))
    {
        if (pCreature->GetStats()->HasFeat(CSWFeat::FEAT_EVASION))
        {
            nDamage = 0;
        }
        else
        {
            nDamage /= 2;
        }
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nSubType =
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        pEffect->SetCreator(oidSaveVersus);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;

        pEffect->SetInteger(0, 4000);
        pCreature->ApplyEffect(pEffect, 0, 0);
    }
    if (g_pVirtualMachine->StackPushInteger(nDamage) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005BFE90
int CSWVirtualMachineCommands::ExecuteCommandGetReflexSavingThrow(int nCommand,
                                                                  int nArgs)
{
    unsigned long oidTarget;
    CServerExoApp* pServerExoApp;
    CGameObject* pObject;
    int nSavingThrow;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pObject = pServerExoApp->GetGameObject(oidTarget);
    nSavingThrow = 0;
    if (pObject != NULL)
    {
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            if (pCreature != NULL)
            {
                CSWSCreatureStats* pStats = pCreature->m_pStats;
                if (pStats != NULL)
                {
                    nSavingThrow = pStats->GetReflexSavingThrow(0);
                }
            }
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
        {
            CSWSDoor* pDoor = pObject->AsSWSDoor();
            if (pDoor != NULL)
            {
                nSavingThrow = pDoor->m_nReflexSave;
            }
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
            if (pPlaceable != NULL)
            {
                nSavingThrow = pPlaceable->m_nReflexSave;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nSavingThrow) != 0
               ? 0
               : ERROR_STACK_PUSH;
}
// PC 0x005BFF50
int CSWVirtualMachineCommands::ExecuteCommandGetReputation(int nCommand,
                                                           int nArgs)
{
    int nResult;
    unsigned long oidSource;
    unsigned long oidTarget;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    nResult = -1;
    if (g_pVirtualMachine->StackPopObject(&oidSource) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
        {
            return -2001;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(oidTarget, &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            nResult = ((CSWSObject*)pGameObject->AsSWSObject())
                          ->GetReputation(oidSource, 0);
        }
        return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
    }
}
// PC 0x005BFFF0
int CSWVirtualMachineCommands::ExecuteCommandGetRestrictMode(int nCommand,
                                                             int nArgs)
{
    int nRestrict = 0;
    unsigned long oidCreature = OBJECT_INVALID;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL && pCreature->m_bInParty != 0)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetModule() != NULL)
        {
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            if (pServerExoApp->GetModule()->GetArea() != NULL)
            {
                pServerExoApp = g_pAppManager->m_pServerExoApp;
                nRestrict = pServerExoApp->GetModule()->GetArea()
                               ->m_nPlayerRestrictMode;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nRestrict) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C00A0
int CSWVirtualMachineCommands::ExecuteCommandGetRunScriptVar(int nCommand,
                                                             int nArgs)
{
    return g_pVirtualMachine->StackPushInteger(g_nRunScriptVar) != 0 ? 0
                                                                     : -2000;
}
// PC 0x005C00D0
int CSWVirtualMachineCommands::ExecuteCommandGetSkillRank(int nCommand, int nArgs)
{
    int nSkill;
    unsigned long oidCreature;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    int nResult;

    if (g_pVirtualMachine->StackPopInteger(&nSkill) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nResult = -1;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidCreature, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL)
    {
        pCreature = pGameObject->AsSWSCreature();
        if (nCommand == COMMAND_GET_SKILL_RANK_BASE)
        {
            nResult = pCreature->GetStats()->GetSkillRank(nSkill, NULL, 1);
        }
        else
        {
            nResult = pCreature->GetStats()->GetSkillRank(nSkill, NULL, 0);
        }
    }
    if (g_pVirtualMachine->StackPushInteger(nResult) != 0)
        return 0;
    return ERROR_STACK_PUSH;
}
// PC 0x005C01A0
int CSWVirtualMachineCommands::ExecuteCommandGetSpellCast(int nCommand,
                                                          int nArgs)
{
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    int bValid;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    bValid = pServerExoApp->GetObjectArray()->GetGameObject(
                 m_oidObject, &pGameObject) == CGameObjectArray::SUCCESS;
    if (nCommand == 245)
    {
        unsigned long oidCaster = OBJECT_INVALID;
        if (bValid)
        {
            switch (pGameObject->m_nObjectType)
            {
            case SW_OBJECT_TYPE_CREATURE:
                oidCaster = pGameObject->AsSWSCreature()->m_n1D8;
                break;
            case SW_OBJECT_TYPE_PLACEABLE:
                oidCaster = pGameObject->AsSWSPlaceable()->m_n1D8;
                break;
            case SW_OBJECT_TYPE_DOOR:
                oidCaster = pGameObject->AsSWSDoor()->m_n1D8;
                break;
            }
        }
        if (g_pVirtualMachine->StackPushObject(oidCaster) == 0)
        {
            return -2000;
        }
    }
    else if (nCommand == 246)
    {
        int nSpell = -1;
        if (bValid)
        {
            switch (pGameObject->m_nObjectType)
            {
            case SW_OBJECT_TYPE_CREATURE:
                nSpell = pGameObject->AsSWSCreature()->m_n1DC;
                break;
            case SW_OBJECT_TYPE_PLACEABLE:
                nSpell = pGameObject->AsSWSPlaceable()->m_n1DC;
                break;
            case SW_OBJECT_TYPE_DOOR:
                nSpell = pGameObject->AsSWSDoor()->m_n1DC;
                break;
            }
        }
        if (g_pVirtualMachine->StackPushInteger(nSpell) == 0)
        {
            return -2000;
        }
    }
    else if (nCommand == 423)
    {
        int bHarmful = 0;
        if (bValid)
        {
            switch (pGameObject->m_nObjectType)
            {
            case SW_OBJECT_TYPE_CREATURE:
                bHarmful = pGameObject->AsSWSCreature()->m_n1E0;
                break;
            case SW_OBJECT_TYPE_PLACEABLE:
                bHarmful = pGameObject->AsSWSPlaceable()->m_n1E0;
                break;
            case SW_OBJECT_TYPE_DOOR:
                bHarmful = pGameObject->AsSWSDoor()->m_n1E0;
                break;
            }
        }
        if (g_pVirtualMachine->StackPushInteger(bHarmful) == 0)
        {
            return -2000;
        }
    }
    return 0;
}
// PC 0x005C02E0
int CSWVirtualMachineCommands::ExecuteCommandGetSpellCasterItem(int nCommand,
                                                                int nArgs)
{
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    oidResult = OBJECT_INVALID;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSCreature() != NULL)
    {
        oidResult = pGameObject->AsSWSCreature()->m_oidSpellCasterItem;
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005C0540
int CSWVirtualMachineCommands::ExecuteCommandGetSpellId(int nCommand,
                                                       int nArgs)
{
    CGameObject* pGameObject;
    int nSpellId = -1;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;

    if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject,
                                                     &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL)
    {
        nSpellId = pGameObject->AsSWSCreature()->m_nLastSpellId;
    }
    return g_pVirtualMachine->StackPushInteger(nSpellId) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C05B0
int CSWVirtualMachineCommands::ExecuteCommandGetSpellSaveDC(int nCommand,
                                                           int nArgs)
{
    CGameObject* pGameObject;
    int nSaveDC = CSWSAreaOfEffectObject::DEFAULT_SPELL_SAVE_DC;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;

    if (pServerExoApp->GetObjectArray()->GetGameObject(
            m_oidObject, &pGameObject) == CGameObjectArray::SUCCESS)
    {
        CSWSCreature* pCreature = NULL;
        int nSpellId = -1;

        if (pGameObject->AsSWSCreature() != NULL)
        {
            pCreature = pGameObject->AsSWSCreature();
            if (pCreature != NULL)
            {
                nSpellId = pCreature->m_nLastSpellId;
            }
        }
        else if (pGameObject->AsSWSAreaOfEffectObject() != NULL)
        {
            unsigned long oidCreator =
                pGameObject->AsSWSAreaOfEffectObject()->m_oidCreator;
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreator);
            if (pCreature == NULL)
            {
                nSaveDC = pGameObject->AsSWSAreaOfEffectObject()->m_nSpellSaveDC;
            }
            nSpellId = pGameObject->AsSWSAreaOfEffectObject()->GetEffectSpellId();
        }

        if (pCreature != NULL)
        {
            nSaveDC = pCreature->CalculateSpellSaveDC(nSpellId);
        }
    }
    return g_pVirtualMachine->StackPushInteger(nSaveDC) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C0690
int CSWVirtualMachineCommands::ExecuteCommandGetSpellTargetLoc(
    int nCommand, int nArgs)
{

    CScriptLocation lTarget;
    CGameObject* pGameObject;
    CGameObjectArray* pObjectArray =
        (CGameObjectArray*)g_pAppManager->m_pServerExoApp->GetObjectArray();

    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        if (pObjectArray->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSObject* pObject = pGameObject->AsSWSObject();
            if (pObject != NULL &&
                (pObject->AsSWSCreature() != NULL ||
                 pObject->AsSWSPlaceable() != NULL))
            {
                lTarget.m_vPosition = pObject->m_vSpellTargetPosition;
                lTarget.m_vOrientation = pObject->m_vOrientation;
            }
        }
    }
    if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_LOCATION,
                                                   &lTarget) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C07A0
int CSWVirtualMachineCommands::ExecuteCommandGetSpellTargetObject(
    int nCommand, int nArgs)
{
    unsigned long oidTarget = CGameObjectArray::INVALIDOBJID;
    CGameObject* pGameObject;

    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSObject* pObject = pGameObject->AsSWSObject();
            if (pObject != NULL &&
                (pObject->AsSWSCreature() != NULL ||
                 pObject->AsSWSPlaceable() != NULL))
            {
                oidTarget = pObject->m_oidSpellTargetObject;
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidTarget) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C0830
int CSWVirtualMachineCommands::ExecuteCommandGetStat(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nStat;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    nStat = -1;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0 && nCommand == 358)
    {
        nStat = pCreature->m_pStats->m_nGender;
    }
    return g_pVirtualMachine->StackPushInteger(nStat) != 0 ? 0 : -2000;
}
// PC 0x005C08A0
int CSWVirtualMachineCommands::ExecuteCommandGetStrByStrRef(int nCommand,
                                                        int nArgs)
{

    int nStrRef;
    if (g_pVirtualMachine->StackPopInteger(&nStrRef) == 0)
    {
        return ERROR_STACK_POP;
    }
    STR_RES cResult;
    g_pTlkTable->Fetch(nStrRef, cResult, CTlkTable::GENDER_MALE);
    CExoString sResult(cResult.m_sString);
    if (g_pVirtualMachine->StackPushString(sResult) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C09B0
int CSWVirtualMachineCommands::ExecuteCommandGetStringLeft(int nCommand,
                                                         int nArgs)
{

    CExoString sString;
    CExoString sResult;
    int nCount;

    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nCount) == 0)
    {
        return ERROR_STACK_POP;
    }
    sResult = sString.Left(nCount);
    if (g_pVirtualMachine->StackPushString(sResult) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C0AF0
int CSWVirtualMachineCommands::ExecuteCommandGetStringLength(int nCommand,
                                                             int nArgs)
{
    CExoString sString;
    int nResult;

    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return -2001;
    }
    nResult = sString.GetLength();
    if (g_pVirtualMachine->StackPushInteger(nResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005C0BC0
int CSWVirtualMachineCommands::ExecuteCommandGetStringLowerCase(int nCommand,
                                                                int nArgs)
{

    CExoString sString;
    CExoString sResult;

    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return ERROR_STACK_POP;
    }
    sResult = sString.LowerCase();
    if (g_pVirtualMachine->StackPushString(sResult) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C0CE0
int CSWVirtualMachineCommands::ExecuteCommandGetStringRight(int nCommand,
                                                          int nArgs)
{

    CExoString sString;
    CExoString sResult;
    int nCount;

    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nCount) == 0)
    {
        return ERROR_STACK_POP;
    }
    sResult = sString.Right(nCount);
    if (g_pVirtualMachine->StackPushString(sResult) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C0E20
int CSWVirtualMachineCommands::ExecuteCommandGetStringUpperCase(int nCommand,
                                                                int nArgs)
{

    CExoString sString;
    CExoString sResult;

    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return ERROR_STACK_POP;
    }
    sResult = sString.UpperCase();
    if (g_pVirtualMachine->StackPushString(sResult) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C0F40
int CSWVirtualMachineCommands::ExecuteCommandGetStrRefSoundDuration(int nCommand,
                                                                 int nArgs)
{

    int nStrRef;
    STR_RES cResult;

    if (g_pVirtualMachine->StackPopInteger(&nStrRef) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nStrRef < 0 ||
        g_pTlkTable->Fetch(nStrRef, cResult, CTlkTable::GENDER_MALE) == 0)
    {
        cResult.m_fSoundLength = 0.0f;
    }
    if (g_pVirtualMachine->StackPushFloat(cResult.m_fSoundLength) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C1040
int CSWVirtualMachineCommands::ExecuteCommandGetSubString(int nCommand,
                                                         int nArgs)
{

    CExoString sString;
    CExoString sResult;
    int nStart;
    int nCount;

    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nStart) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nCount) == 0)
    {
        return ERROR_STACK_POP;
    }
    sResult = sString.SubString(nStart, nCount);
    if (g_pVirtualMachine->StackPushString(sResult) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C11A0
int CSWVirtualMachineCommands::ExecuteCommandGetTag(int nCommand, int nArgs)
{

    CExoString sTag;
    unsigned long oidObject;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(oidObject, &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_MODULE)
        {
            sTag = pGameObject->AsSWSModule()->m_sTag;
        }
        else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREA)
        {
            sTag = pGameObject->AsSWSArea()->m_sTag;
        }
        else
        {
            sTag = ((CSWSObject*)pGameObject)->m_sTag;
        }
    }

    if (g_pVirtualMachine->StackPushString(sTag) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C12C0
int CSWVirtualMachineCommands::ExecuteCommandGetTimeHour(int nCommand,
                                                         int nArgs)
{
    CServerExoApp* pServerExoApp;
    CWorldTimer* pWorldTimer;
    int nResult;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pWorldTimer = (CWorldTimer*)pServerExoApp->GetWorldTimer();
    nResult = pWorldTimer->GetWorldTimeHour();
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C1300
int CSWVirtualMachineCommands::ExecuteCommandGetTimeMillisecond(int nCommand,
                                                                int nArgs)
{
    CServerExoApp* pServerExoApp;
    CWorldTimer* pWorldTimer;
    unsigned long oidResult;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pWorldTimer = (CWorldTimer*)pServerExoApp->GetWorldTimer();
    oidResult = pWorldTimer->GetWorldTimeMillisecond();
    return g_pVirtualMachine->StackPushInteger(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005C1340
int CSWVirtualMachineCommands::ExecuteCommandGetTimeMinute(int nCommand,
                                                           int nArgs)
{
    CServerExoApp* pServerExoApp;
    CWorldTimer* pWorldTimer;
    int nResult;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pWorldTimer = (CWorldTimer*)pServerExoApp->GetWorldTimer();
    nResult = pWorldTimer->GetWorldTimeMinute();
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C1380
int CSWVirtualMachineCommands::ExecuteCommandGetTimeSecond(int nCommand,
                                                           int nArgs)
{
    CServerExoApp* pServerExoApp;
    CWorldTimer* pWorldTimer;
    unsigned long oidResult;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pWorldTimer = (CWorldTimer*)pServerExoApp->GetWorldTimer();
    oidResult = pWorldTimer->GetWorldTimeSecond();
    return g_pVirtualMachine->StackPushInteger(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005C13C0
int CSWVirtualMachineCommands::ExecuteCommandGetTotalDamageDealt(int nCommand,
                                                               int nArgs)
{
    int nResult;
    CGameObject* pGameObject;
    CSWSObject* pObject;
    int nDamageType;

    nResult = 0;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(m_oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        pObject = pGameObject->AsSWSObject();
        if (pObject != NULL)
        {
            for (nDamageType = 0; nDamageType < CSWSObject::LAST_DAMAGE_TYPE_COUNT;
                 ++nDamageType)
            {
                if (pObject->GetLastDamageAmountByFlags(1 << nDamageType) > 0)
                {
                    nResult += pObject->GetLastDamageAmountByFlags(1 << nDamageType);
                }
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C1450
int CSWVirtualMachineCommands::ExecuteCommandGetUserActionsPending(int nCommand,
                                                                   int nArgs)
{
    int nResult;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSCreature* pCreature;

    nResult = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)(CGameObjectArray*)pServerExoApp->GetObjectArray()
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != 0)
    {
        pCreature = (CSWSCreature*)pGameObject->AsSWSCreature();
        nResult = pCreature->GetUserActionsPending();
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C14C0
int CSWVirtualMachineCommands::ExecuteCommandGetUserDefinedEventNumber(
    int nCommand, int nArgs)
{
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    int nEventNumber;

    nEventNumber = -1;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
            ->GetGameObject(m_oidObject, &pGameObject) ==
        CGameObjectArray::SUCCESS)
    {
        switch (pGameObject->m_nObjectType)
        {
        case SW_OBJECT_TYPE_MODULE:
            nEventNumber = pGameObject->AsSWSModule()->m_nUserDefinedEvent;
            break;
        case SW_OBJECT_TYPE_AREA:
            nEventNumber = pGameObject->AsSWSArea()->m_nCustomScriptEventId;
            break;
        case SW_OBJECT_TYPE_ENCOUNTER:
            nEventNumber = pGameObject->AsSWSEncounter()->m_nCustomScriptId;
            break;
        default:
            nEventNumber = ((CSWSObject*)pGameObject)->m_nCustomScriptEventId;
            break;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nEventNumber) != 0 ? 0 : -2000;
}
// PC 0x005C1550
int CSWVirtualMachineCommands::ExecuteCommandGetWaypointByTag(int nCommand,
                                                              int nArgs)
{
    unsigned long oidResult;
    CExoString sWaypointTag;
    CServerExoApp* pServerExoApp;
    CSWSModule* pModule;

    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopString(&sWaypointTag) == 0)
    {
        return -2001;
    }
    if (g_pAppManager != NULL)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp != NULL)
        {
            pModule = (CSWSModule*)pServerExoApp->GetModule();
            if (pModule != NULL)
            {
                oidResult = pModule->GetWaypoint(sWaypointTag.LowerCase());
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidResult) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005C1660
int CSWVirtualMachineCommands::ExecuteCommandGetWaypointByTrigger(int nCommand,
                                                                 int nArgs)
{
    unsigned long oidWaypoint;
    unsigned long oidTrigger;
    CGameObject* pGameObject;

    oidWaypoint = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidTrigger) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            oidTrigger, &pGameObject) == CGameObjectArray::SUCCESS)
    {
        if (pGameObject->AsSWSTrigger() != NULL)
        {
            CSWSTrigger* pTrigger = pGameObject->AsSWSTrigger();
            if (pGameObject->AsSWSTrigger()->m_nLinkedToFlags ==
                CSWSTrigger::TRIGGER_LINK_WAYPOINT)
            {
                oidWaypoint = ((CSWSModule*)g_pAppManager->m_pServerExoApp
                                   ->GetModule())->GetWaypoint(pTrigger->m_sLinkedTo);
            }
            else
            {
                oidWaypoint = ((CSWSModule*)g_pAppManager->m_pServerExoApp
                                   ->GetModule())->FindObjectByTagOrdinal(pTrigger->m_sLinkedTo, 0UL);
            }
        }
        else if (pGameObject->AsSWSDoor() != NULL)
        {
            CSWSObject* pLinkedObject = pGameObject->AsSWSDoor()->GetLinkedObject();
            if (pLinkedObject != NULL)
            {
                oidWaypoint = pLinkedObject->m_nId;
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidWaypoint) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C1760
int CSWVirtualMachineCommands::ExecuteCommandGetWeaponRanged(int nCommand,
                                                             int nArgs)
{
    unsigned long oidItem;
    int bRanged;
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;
    int nBaseItem;

    bRanged = 0;
    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != 0)
    {
        nBaseItem = pItem->m_nBaseItem;
        bRanged = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)
                      ->m_nRangedWeapon > 0;
    }
    return g_pVirtualMachine->StackPushInteger(bRanged) != 0 ? 0 : -2000;
}
// PC 0x005C17E0
int CSWVirtualMachineCommands::ExecuteCommandGetWillSavingThrow(int nCommand,
                                                                  int nArgs)
{
    unsigned long oidTarget;
    CServerExoApp* pServerExoApp;
    CGameObject* pObject;
    int nSavingThrow;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pObject = pServerExoApp->GetGameObject(oidTarget);
    nSavingThrow = 0;
    if (pObject != NULL)
    {
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            if (pCreature != NULL)
            {
                CSWSCreatureStats* pStats = pCreature->m_pStats;
                if (pStats != NULL)
                {
                    nSavingThrow = pStats->GetWillSavingThrow(0);
                }
            }
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
        {
            CSWSDoor* pDoor = pObject->AsSWSDoor();
            if (pDoor != NULL)
            {
                nSavingThrow = pDoor->m_nWillSave;
            }
        }
        else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
            if (pPlaceable != NULL)
            {
                nSavingThrow = pPlaceable->m_nWillSave;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nSavingThrow) != 0
               ? 0
               : ERROR_STACK_PUSH;
}
// PC 0x005C18A0
int CSWVirtualMachineCommands::ExecuteCommandGetXP(int nCommand, int nArgs)
{
    int nResult;
    unsigned long oidCreature;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSCreature* pCreature;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(oidCreature, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        pCreature = (CSWSCreature*)pGameObject->AsSWSCreature();
        if (pCreature != 0)
        {
            nResult = pCreature->m_pStats->m_nExperience;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C1930
int CSWVirtualMachineCommands::ExecuteCommandGiveGoldToCreature(int nCommand,
                                                                int nArgs)
{
    unsigned long oidCreature;
    int nGP;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSCreature* pCreature;
    CSWSCreature* pCreature2;
    CClientExoApp* pClientExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nGP) == 0)
    {
        return -2001;
    }
    if (nGP > 0)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)(CGameObjectArray*)pServerExoApp->GetObjectArray()
                    ->GetGameObject(oidCreature, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject != NULL)
        {
            pCreature = (CSWSCreature*)pGameObject->AsSWSCreature();
            pCreature2 = pCreature;
            if (pCreature != NULL)
            {
                pCreature->AddGold(nGP, 1);
                if (pCreature2->m_bInParty != 0)
                {
                    pClientExoApp = g_pAppManager->m_pClientExoApp;
                    pClientExoApp->GetInGameGui()->UpdateStatus(GUI_STATUS_CREDITS, nGP);
                }
            }
        }
    }
    return 0;
}
// PC 0x005C19F0
int CSWVirtualMachineCommands::ExecuteCommandGiveXPToCreature(int nCommand,
                                                              int nArgs)
{
    unsigned long oidCreature;
    int nXpAmount;
    CGameObject* pGameObject;
    CSWSCreature* pCreature;
    CServerExoApp* pServerExoApp;
    CClientExoApp* pClientExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nXpAmount) == 0)
    {
        return -2001;
    }
    if (nXpAmount > 0)
    {
        int nExperience = nXpAmount;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)pServerExoApp->GetObjectArray()
                    ->GetGameObject(oidCreature, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject != NULL)
        {
            pCreature = pGameObject->AsSWSCreature();
            if (pCreature != NULL)
            {
                if (pCreature->m_bInParty != 0)
                {
                    pClientExoApp = g_pAppManager->m_pClientExoApp;
                    pClientExoApp->GetInGameGui()->UpdateStatus(GUI_STATUS_XP,
                                                              nExperience);
                }
                if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
                {
                    pServerExoApp = g_pAppManager->m_pServerExoApp;
                    pServerExoApp->GetPartyTable()
                        ->DistributeExperience(nXpAmount, 1);
                }
            }
        }
    }
    return 0;
}
// PC 0x005C1AD0
int CSWVirtualMachineCommands::ExecuteCommandGiveItem(int nCommand, int nArgs)
{

    unsigned long oidItem;
    unsigned long oidGiveTo;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidGiveTo) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSItem* pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pGiveTo = pServerExoApp->GetGameObject(oidGiveTo);
    if (pItem != NULL && pGiveTo != NULL)
    {
        if (pGiveTo->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
        {
            unsigned long oidPossessor = pItem->m_oidPossessor;
            pGiveTo->AsSWSCreature()->AcquireItem(
                &pItem, oidPossessor, OBJECT_INVALID, 0);
        }
        else if (pGiveTo->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
        {
            unsigned long oidPossessor = pItem->m_oidPossessor;
            pGiveTo->AsSWSPlaceable()->AcquireItem(&pItem, oidPossessor, 0);
        }
        else if (pGiveTo->m_nObjectType == SW_OBJECT_TYPE_ITEM)
        {
            if (g_pRules->m_pBaseItems
                    ->GetBaseItem(pGiveTo->AsSWSItem()->GetBaseItemType())
                    ->m_bIsContainer != 0)
            {
                unsigned long oidPossessor = pItem->m_oidPossessor;
                pGiveTo->AsSWSItem()->AcquireItem(&pItem, oidPossessor, 0);
            }
        }
    }
    return 0;
}
// PC 0x005C1C00
int CSWVirtualMachineCommands::ExecuteCommandInsertString(int nCommand, int nArgs)
{

    CExoString sString;
    CExoString sDestination;
    int nPosition;

    if (g_pVirtualMachine->StackPopString(&sDestination) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nPosition) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        sDestination.Insert(sString, nPosition);
        if (g_pVirtualMachine->StackPushString(sDestination) == 0)
        {
            return ERROR_STACK_PUSH;
        }
        else
        {
            return 0;
        }
    }
}
// PC 0x005C1D40
int CSWVirtualMachineCommands::ExecuteCommandIntToFloat(int nCommand, int nArgs)
{
    int nValue;
    float fValue;

    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }

    fValue = (float)nValue;
    return g_pVirtualMachine->StackPushFloat(fValue) != 0 ? 0 : -2000;
}
// PC 0x005C1D90
int CSWVirtualMachineCommands::ExecuteCommandIntToHexString(int nCommand, int nArgs)
{
    int nInteger;
    CExoString sHexString;

    if (g_pVirtualMachine->StackPopInteger(&nInteger) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        sHexString.Format("0x%08x", nInteger);
        if (g_pVirtualMachine->StackPushString(sHexString) == 0)
        {
            return ERROR_STACK_PUSH;
        }
        else
        {
            return 0;
        }
    }
}
// PC 0x005C1E60
int CSWVirtualMachineCommands::ExecuteCommandIntToString(int nCommand, int nArgs)
{
    int nInteger;
    CExoString sString;

    if (g_pVirtualMachine->StackPopInteger(&nInteger) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        sString = nInteger;
        if (g_pVirtualMachine->StackPushString(sString) == 0)
        {
            return ERROR_STACK_PUSH;
        }
        else
        {
            return 0;
        }
    }
}
// PC 0x005C1F50
int CSWVirtualMachineCommands::ExecuteCommandIsAIState(int nCommand,
                                                        int nArgs)
{
    unsigned long oidObject;
    int bValue;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;

    bValue = 1;
    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL)
    {
        if (pGameObject->AsSWSCreature() != NULL)
        {
            bValue = pGameObject->AsSWSCreature()->GetDead() ||
                     pGameObject->AsSWSCreature()->GetIsPCDying() ||
                     pGameObject->AsSWSCreature()->GetDeadTemp();
        }
        else
        {
            bValue = ((CSWSObject*)pGameObject)->GetDead();
        }
    }
    return g_pVirtualMachine->StackPushInteger(bValue) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C2000
int CSWVirtualMachineCommands::ExecuteCommandIsAvailableNPC(int nCommand,
                                                           int nArgs)
{
    int nNPC;
    int nResult = 0;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return ERROR_STACK_POP;
    }
    switch (nCommand)
    {
    case COMMAND_IS_AVAILABLE_NPC:
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            nResult = pServerExoApp->GetPartyTable()->GetIsNPCAvailable(nNPC);
            break;
        }
    case COMMAND_GET_NPC_SELECTABILITY:
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            nResult = pServerExoApp->GetPartyTable()->GetNPCSelectability(nNPC);
            if (nResult == 255)
            {
                nResult = -1;
            }
            break;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C20A0
int CSWVirtualMachineCommands::ExecuteCommandIsEffectValid(int nCommand, int nArgs)
{
    CGameEffect* pEffect = NULL;
    int nResult;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pEffect) == 0)
    {
        return -2001;
    }
    nResult = 1;
    if (pEffect == NULL || pEffect->m_nType == CSWSEffectListHandler::EFFECT_INVALIDEFFECT)
    {
        nResult = 0;
    }
    else if (pEffect->m_bExposed == 0)
    {
        nResult = 0;
    }
    if ((pEffect->m_nSubType & (unsigned short)CGameEffect::DURATION_MASK) != CGameEffect::DURATION_TYPE_INSTANTANEOUS
        && (pEffect->m_nSubType & (unsigned short)CGameEffect::DURATION_MASK) != CGameEffect::DURATION_TYPE_TEMPORARY
        && (pEffect->m_nSubType & (unsigned short)CGameEffect::DURATION_MASK) != CGameEffect::DURATION_TYPE_PERMANENT
        && (pEffect->m_nSubType & (unsigned short)CGameEffect::DURATION_MASK) != CGameEffect::DURATION_TYPE_EQUIPPED
        && (pEffect->m_nSubType & (unsigned short)CGameEffect::DURATION_MASK) != CGameEffect::DURATION_TYPE_INNATE)
    {
        nResult = 0;
    }
    if (g_pVirtualMachine->StackPushInteger(nResult) == 0)
    {
        return -2000;
    }
    CGameEffect* pTempEffect;
    pTempEffect = pEffect;
    if (pTempEffect != NULL)
    {
        pTempEffect->~CGameEffect();
        operator delete(pTempEffect);
    }
    return 0;
}
// PC 0x005C2160
int CSWVirtualMachineCommands::ExecuteCommandGetIsInConversation(
    int nCommand, int nArgs)
{
    unsigned long oidObject;
    unsigned long oidDialogOwner;
    CGameObject* pGameObject;
    CGameObject* pOwner;
    int bInConversation;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL &&
        (oidDialogOwner = pGameObject->AsSWSObject()->m_oidDialogOwner,
         pOwner = g_pAppManager->m_pServerExoApp->GetGameObject(oidDialogOwner),
         pOwner != NULL) && pOwner->AsSWSObject() != NULL)
    {
        bInConversation =
            pOwner->AsSWSObject()->m_oidDialogOwner == oidDialogOwner;
    }
    else
    {
        bInConversation = 0;
    }

    return g_pVirtualMachine->StackPushInteger(bInConversation) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C2210
int CSWVirtualMachineCommands::ExecuteCommandGetIsConversationActive(
    int nCommand, int nArgs)
{
    CClientExoApp* pClientExoApp;
    CGuiInGame* pGuiInGame;
    int nResult;

    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pGuiInGame = pClientExoApp->GetInGameGui();
    nResult = pGuiInGame->m_nGlobalDialogState;
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C2240
int CSWVirtualMachineCommands::ExecuteCommandGetCreatureMovmentType(
    int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nRate;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    nRate = 1;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        nRate = pCreature->m_pStats->m_nMovementRate;
    }
    return g_pVirtualMachine->StackPushInteger(nRate) != 0 ? 0 : -2000;
}
// PC 0x005C22B0
int CSWVirtualMachineCommands::ExecuteCommandGetLastConversation(int nCommand,
                                                                 int nArgs)
{
    int nResult;
    CServerExoApp* pServerExoApp;
    int nResult2;

    nResult = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nResult2 = g_pVirtualMachine->StackPushString(
        pServerExoApp->GetLastConversation());
    nResult = -1;
    return nResult2 != 0 ? -2000 : 0;
}
// PC 0x005C2320
int CSWVirtualMachineCommands::ExecuteCommandIsListening(int nCommand,
                                                         int nArgs)
{
    unsigned long oidObject;
    int bListening = 0;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(oidObject, (struct CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL)
    {
        bListening = pObject->m_bListening;
    }
    return g_pVirtualMachine->StackPushInteger(bListening) != 0 ? 0 : -2000;
}
// PC 0x005C23A0
int CSWVirtualMachineCommands::ExecuteCommandIsNPCPartyMember(int nCommand,
                                                              int nArgs)
{
    int nNPC;
    CServerExoApp* pServerExoApp;
    int nResult2;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nResult2 = pServerExoApp->GetPartyTable()->GetIsMember(nNPC);
    if (nResult2 == 0 && g_pAppManager->m_pServerExoApp->GetPartyTable()->m_nControlledNPC == nNPC)
    {
        nResult2 = 1;
    }
    return g_pVirtualMachine->StackPushInteger(nResult2) != 0 ? 0 : -2000;
}
// PC 0x005C2420
int CSWVirtualMachineCommands::ExecuteCommandIsObjectPartyMember(int nCommand,
                                                                 int nArgs)
{
    unsigned long oidCreature;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    nResult = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        nResult = pCreature->m_bInParty;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C2480
int CSWVirtualMachineCommands::ExecuteCommandIsPC(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    CSWSCreature* pCreature;
    int nResult = 0;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL && pCreature->GetStats() != NULL)
    {
        nResult = pCreature->GetStats()->GetIsPC();
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C26E0
int CSWVirtualMachineCommands::ExecuteCommandItemActivated(int nCommand,
                                                         int nArgs)
{

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSModule* pModule = (CSWSModule*)pServerExoApp->GetModule();
    if (nCommand == COMMAND_GET_ITEM_ACTIVATED_TARGET_LOCATION)
    {
        CScriptLocation lLocation;
        if (pModule != NULL)
        {
            unsigned long oidActivator = pModule->m_oidItemActivator;
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(
                oidActivator);
            lLocation.m_vPosition = pModule->m_vActivatedLocation;
            if (pCreature != NULL &&
                equal(pCreature->m_vPosition, lLocation.m_vPosition) == 0)
            {
                lLocation.m_vOrientation =
                    normalize(lLocation.m_vPosition - pCreature->m_vPosition);
            }
        }
        if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_LOCATION,
                                                       &lLocation) == 0)
        {
            return ERROR_STACK_PUSH;
        }
    }
    else
    {
        unsigned long oidResult = OBJECT_INVALID;
        if (pModule != NULL)
        {
            switch (nCommand)
            {
            case COMMAND_GET_ITEM_ACTIVATED:
                oidResult = pModule->m_oidActivatedItem;
                break;
            case COMMAND_GET_ITEM_ACTIVATOR:
                oidResult = pModule->m_oidItemActivator;
                break;
            case COMMAND_GET_ITEM_ACTIVATED_TARGET:
                oidResult = pModule->m_oidActivatedTarget2;
                break;
            }
        }
        if (g_pVirtualMachine->StackPushObject(oidResult) == 0)
        {
            return ERROR_STACK_PUSH;
        }
    }
    return 0;
}
// PC 0x005C2870
int CSWVirtualMachineCommands::ExecuteCommandLocation(int nCommand, int nArgs)
{

    Vector vPosition;
    float fOrientation;
    CScriptLocation* pLocation = new CScriptLocation;

    if (g_pVirtualMachine->StackPopVector(&vPosition) != 0)
    {
        if (g_pVirtualMachine->StackPopFloat(&fOrientation) == 0)
        {
            delete pLocation;
            return ERROR_STACK_POP;
        }

        pLocation->m_vPosition = vPosition;
        pLocation->m_vOrientation = normalize(Vector(
            (float)cos(fOrientation * SWVM_DEGREES_TO_RADIANS),
            (float)sin(fOrientation * SWVM_DEGREES_TO_RADIANS), 0.0f));
        if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_LOCATION,
                                                       pLocation) == 0)
        {
            delete pLocation;
            return ERROR_STACK_PUSH;
        }
        delete pLocation;
        return 0;
    }
    else
    {
        delete pLocation;
        return ERROR_STACK_POP;
    }
}
// PC 0x005C29F0
int CSWVirtualMachineCommands::ExecuteCommandLocationAccess(int nCommand,
                                                          int nArgs)
{
    CScriptLocation* pLocation;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocation) == 0)
    {
        return ERROR_STACK_POP;
    }

    int nResult = 1;
    switch (nCommand)
    {
    case COMMAND_GET_POSITION_FROM_LOCATION:
        nResult = g_pVirtualMachine->StackPushVector(pLocation->m_vPosition);
        break;
    case COMMAND_GET_FACING_FROM_LOCATION:
        {
            float fFacing = (float)atan2(pLocation->m_vOrientation.m_fY,
                                        pLocation->m_vOrientation.m_fX) * RAD_TO_DEG;
            nResult = g_pVirtualMachine->StackPushFloat(fFacing);
        }
        break;
    }
    if (pLocation != NULL)
    {
        delete pLocation;
    }
    if (nResult != 0)
        return 0;
    return ERROR_STACK_PUSH;
}
// PC 0x005C2AB0
int CSWVirtualMachineCommands::ExecuteCommandMath(int nCommand, int nArgs)
{

    if (nCommand == COMMAND_ABS)
    {
        int nValue;
        if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (g_pVirtualMachine->StackPushInteger(abs(nValue)) == 0)
        {
            return ERROR_STACK_PUSH;
        }
    }
    else
    {
        float fValue;
        float fExponent;
        float fResult = 0.0f;
        if (g_pVirtualMachine->StackPopFloat(&fValue) == 0)
        {
            return ERROR_STACK_POP;
        }
        switch (nCommand)
        {
        case COMMAND_FABS:
            fResult = fabs(fValue);
            break;
        case COMMAND_COS:
            fResult = cos(fValue * SWVM_DEGREES_TO_RADIANS);
            break;
        case COMMAND_SIN:
            fResult = sin(fValue * SWVM_DEGREES_TO_RADIANS);
            break;
        case COMMAND_TAN:
            fResult = tan(fValue * SWVM_DEGREES_TO_RADIANS);
            break;
        case COMMAND_ACOS:
            fResult = fabs(fValue) <= 1.0
                          ? (float)acos(fValue) * RAD_TO_DEG
                          : 0.0f;
            break;
        case COMMAND_ASIN:
            fResult = fabs(fValue) <= 1.0
                          ? (float)asin(fValue) * RAD_TO_DEG
                          : 0.0f;
            break;
        case COMMAND_ATAN:
            fResult = (float)atan(fValue) * RAD_TO_DEG;
            break;
        case COMMAND_LOG:
            fResult = fValue > 0.0f ? (float)log(fValue) : 0.0f;
            break;
        case COMMAND_POW:
            if (g_pVirtualMachine->StackPopFloat(&fExponent) == 0)
            {
                return ERROR_STACK_POP;
            }
            fResult = fValue != 0.0 || fExponent >= 0.0f
                          ? (float)pow(fValue, fExponent) : 0.0f;
            break;
        case COMMAND_SQRT:
            fResult = fValue > 0.0f ? (float)sqrt(fValue) : 0.0f;
            break;
        }
        if (g_pVirtualMachine->StackPushFloat(fResult) == 0)
        {
            return ERROR_STACK_PUSH;
        }
    }
    return 0;
}
// PC 0x005C2CF0
int CSWVirtualMachineCommands::ExecuteCommandModuleAccess(int nCommand, int nArgs)
{
    CScriptLocation lLocation;
    unsigned long oidObject = OBJECT_INVALID;
    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
    if (pModule != NULL)
    {
        if (nCommand == COMMAND_GET_LAST_PLAYER_DIED)
            oidObject = pModule->m_oidDeadPlayer;
        else if (nCommand == COMMAND_GET_LAST_PLAYER_DYING)
            oidObject = pModule->m_oidDyingPlayer;
        else if (nCommand == COMMAND_GET_STARTING_LOCATION)
        {
            lLocation.m_vPosition.m_fX = pModule->m_pEntryInfo->m_fEntryX;
            lLocation.m_vPosition.m_fY = pModule->m_pEntryInfo->m_fEntryY;
            lLocation.m_vPosition.m_fZ = pModule->m_pEntryInfo->m_fEntryZ;
            lLocation.m_vOrientation.m_fX = pModule->m_pEntryInfo->m_fEntryDirX;
            CSWSModuleEntryInfo* pEntryInfo = pModule->m_pEntryInfo;
            lLocation.m_vOrientation.m_fY = pEntryInfo->m_fEntryDirY;
        }
        else if (nCommand == COMMAND_GET_LAST_RESPAWN_BUTTON_PRESSER)
            oidObject = pModule->m_oidRespawningPlayer;
        else if (nCommand == COMMAND_GET_LAST_ITEM_EQUIPPED)
            oidObject = pModule->m_oidEquippedItem;
    }

    switch (nCommand)
    {
    case COMMAND_GET_LAST_PLAYER_DIED:
    case COMMAND_GET_LAST_PLAYER_DYING:
    case COMMAND_GET_LAST_RESPAWN_BUTTON_PRESSER:
    case COMMAND_GET_LAST_ITEM_EQUIPPED:
        if (g_pVirtualMachine->StackPushObject(oidObject) == 0)
            return ERROR_STACK_PUSH;
        break;
    case COMMAND_GET_STARTING_LOCATION:
        if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_LOCATION,
                                                       &lLocation) == 0)
            return ERROR_STACK_PUSH;
        break;
    }
    return 0;
}
// PC 0x005C2E80
int CSWVirtualMachineCommands::ExecuteCommandMoveAwayFromObject(int nCommand,
                                                                int nArgs)
{

    unsigned long oidFleeFrom;
    int bRun = 0;
    float fMoveAwayRange = CSWSCreature::MOVE_AWAY_RADIUS;
    CGameObject* pGameObject;
    CGameObject* pFleeFrom;

    if (g_pVirtualMachine->StackPopObject(&oidFleeFrom) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&bRun) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    if (nArgs >= 3)
    {
        if (g_pVirtualMachine->StackPopFloat(&fMoveAwayRange) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
        unsigned char nResult = pServerExoApp->GetObjectArray()->GetGameObject(
            m_oidObject, &pGameObject);
        if (nResult == CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL &&
            pGameObject->AsSWSCreature()->m_bCommandable != 0)
        {
            pServerExoApp = g_pAppManager->GetServerExoApp();
            if (pServerExoApp->GetObjectArray()->GetGameObject(oidFleeFrom,
                                                               &pFleeFrom) ==
                CGameObjectArray::SUCCESS)
            {
                int nTimeout = 10;
                ((CSWSObject*)pGameObject)->AddAction(
                    CSWSObject::ACTION_CHECK_MOVE_AWAY_FROM_OBJECT,
                    CSWSObject::ACTION_GROUP_LAST,
                    CSWSObject::ACTION_PARAM_OBJECT, &oidFleeFrom,
                    CSWSObject::ACTION_PARAM_INTEGER, &bRun,
                    CSWSObject::ACTION_PARAM_FLOAT, &fMoveAwayRange,
                    CSWSObject::ACTION_PARAM_INTEGER, &nTimeout, 0, NULL, 0,
                    NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                    NULL, 0, NULL);
            }
        }
    }
    return 0;
}
// PC 0x005C2FF0
int CSWVirtualMachineCommands::ExecuteCommandMoveToObject(int nCommand,
                                                         int nArgs)
{

    unsigned long oidMoveTo;
    int bRun = 0;
    float fUseRange = 0.5f;
    float fRange = 1.0f;
    float fTimeout = 0.0f;
    CGameObject* pGameObject;
    if (g_pVirtualMachine->StackPopObject(&oidMoveTo) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&bRun) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    if (nArgs >= 3)
    {
        if (g_pVirtualMachine->StackPopFloat(&fRange) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    if (nCommand == COMMAND_ACTION_FORCE_MOVE_TO_OBJECT)
    {
        if (nArgs >= 4)
        {
            if (g_pVirtualMachine->StackPopFloat(&fTimeout) == 0)
            {
                return ERROR_STACK_POP;
            }
        }
        else
        {
            fTimeout = 30.0f;
        }
    }
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
        if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject,
                                                           &pGameObject) ==
                CGameObjectArray::SUCCESS)
        {
            CGameObject* pMoveTo;
            pServerExoApp = g_pAppManager->GetServerExoApp();
            if (pServerExoApp->GetObjectArray()->GetGameObject(oidMoveTo,
                                                               &pMoveTo) ==
                    CGameObjectArray::SUCCESS &&
                pGameObject->AsSWSCreature() != NULL &&
                pGameObject->AsSWSCreature()->m_bCommandable != 0)
            {
                Vector vUsePosition;
                pGameObject->AsSWSCreature()->GetUseRange(
                    oidMoveTo, vUsePosition, fUseRange, 0);
                fRange = fRange > fUseRange ? fRange : fUseRange;
                Vector vDestination = ((CSWSObject*)pMoveTo)->m_vPosition;
                CSWSArea* pArea = ((CSWSObject*)pMoveTo)->GetArea();

                CResRef cArea;
                if (pArea != NULL)
                {
                    int bCheckRange = 1;
                    pGameObject->AsSWSCreature()->AddMoveToPointAction(
                        CSWSObject::ACTION_GROUP_NEW, vDestination,
                        pArea->GetId(), oidMoveTo, bRun, fRange, fTimeout,
                        0, 0, 0, 0, 0, 0, Vector(), 0, 0, 0);
                    ((CSWSObject*)pGameObject)->AddAction(
                        CSWSObject::ACTION_CHECK_MOVE_TO_OBJECT_RADIUS,
                        CSWSObject::ACTION_GROUP_LAST,
                        CSWSObject::ACTION_PARAM_OBJECT, &oidMoveTo,
                        CSWSObject::ACTION_PARAM_INTEGER, &bRun,
                        CSWSObject::ACTION_PARAM_FLOAT, &fRange,
                        CSWSObject::ACTION_PARAM_FLOAT, &fRange,
                        CSWSObject::ACTION_PARAM_INTEGER, &bCheckRange,
                        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                        NULL, 0, NULL, 0, NULL);
                    if (pGameObject->AsSWSPlaceable() != NULL &&
                        pGameObject->AsSWSPlaceable()->m_bPreciseUse != 0)
                    {
                        ((CSWSObject*)pGameObject)->AddAction(
                            CSWSObject::ACTION_CHANGE_FACING_OBJECT,
                            CSWSObject::ACTION_GROUP_LAST,
                            CSWSObject::ACTION_PARAM_OBJECT, &oidMoveTo,
                            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                            NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                            NULL, 0, NULL);
                    }
                }
            }
        }
    }
    return 0;
}
// PC 0x005C32F0
int CSWVirtualMachineCommands::ExecuteCommandMoveToPoint(int nCommand,
                                                        int nArgs)
{
    CScriptLocation* pLocation;
    int bRun = 0;
    float fTimeout = 30.0f;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocation) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs > 1)
    {
        if (g_pVirtualMachine->StackPopInteger(&bRun) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (nCommand == COMMAND_ACTION_FORCE_MOVE_TO_LOCATION && nArgs > 2)
        {
            if (g_pVirtualMachine->StackPopFloat(&fTimeout) == 0)
            {
                return ERROR_STACK_POP;
            }
        }
    }

    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject,
                                                            &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL)
        {
            CSWSArea* pArea = pGameObject->AsSWSCreature()->GetArea();
            if (pArea != NULL)
            {
                unsigned long oidTarget = CGameObjectArray::INVALIDOBJID;
                if (nCommand == COMMAND_ACTION_MOVE_TO_LOCATION)
                {
                    pGameObject->AsSWSCreature()->AddMoveToPointAction(
                        CSWSObject::ACTION_GROUP_NEW, pLocation->m_vPosition,
                        pArea->GetId(), oidTarget, bRun,
                        0.0f, 0.0f, 0, 0, 0, 0, 0, 0, Vector(), 0, 0, 0);
                }
                else if (nCommand == COMMAND_ACTION_FORCE_MOVE_TO_LOCATION)
                {
                    pGameObject->AsSWSCreature()->AddMoveToPointAction(
                        CSWSObject::ACTION_GROUP_NEW, pLocation->m_vPosition,
                        pArea->GetId(), oidTarget, bRun,
                        0.0f, fTimeout, 0, 0, 0, 0, 0, 0, Vector(), 0, 0, 0);
                }
                if (pLocation != NULL)
                {
                    delete pLocation;
                }
                return 0;
            }
        }
    }

    delete pLocation;
    return 0;
}
// PC 0x005C3490
int CSWVirtualMachineCommands::ExecuteCommandMusicBackground(int nCommand,
                                                             int nArgs)
{
    int nResult;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSArea* pArea;
    int bStreamingMusicForMovie;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (nArgs > 1)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    bStreamingMusicForMovie = 0;
    if (nArgs > 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&bStreamingMusicForMovie) == 0)
        {
            return -2001;
        }
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pArea = pServerExoApp->GetAreaByGameObjectID(oidResult);
    if (pArea == NULL)
    {
        return 0;
    }
    switch (nCommand)
    {
    case 425:
        pArea->m_pAmbientSound->PlayMusic(1);
        return 0;
    case 426:
        pArea->m_pAmbientSound->PlayMusic(0);
        return 0;
    case 427:
        pArea->m_pAmbientSound->SetMusicDelay(nResult);
        return 0;
    case 428:
        pArea->m_pAmbientSound->SetMusicDayTrack(nResult,
                                                 bStreamingMusicForMovie);
        return 0;
    case 429:
        pArea->m_pAmbientSound->SetMusicNightTrack(nResult,
                                                   bStreamingMusicForMovie);
        return 0;
    default:
        return 0;
    }
}
// PC 0x005C35E0
int CSWVirtualMachineCommands::ExecuteCommandMusicBattle(int nCommand,
                                                         int nArgs)
{
    int nResult;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSArea* pArea;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (nArgs > 1)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pArea = pServerExoApp->GetAreaByGameObjectID(oidResult);
    if (pArea != NULL)
    {
        switch (nCommand)
        {
        case 430:
            pArea->m_pAmbientSound->PlayBattleMusic(1);
            break;
        case 431:
            pArea->m_pAmbientSound->PlayBattleMusic(0);
            break;
        case 432:
            pArea->m_pAmbientSound->SetBattleMusicTrack(nResult);
            return 0;
        default:
            break;
        }
    }
    return 0;
}
// PC 0x005C37F0
int CSWVirtualMachineCommands::ExecuteCommandObjectToString(int nCommand, int nArgs)
{

    unsigned long oidObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        CExoString sObject;
        sObject.Format("%x", oidObject);
        if (g_pVirtualMachine->StackPushString(sObject.CStr()) == 0)
        {
            return ERROR_STACK_PUSH;
        }
        else
        {
            return 0;
        }
    }
}
// PC 0x005C38E0
int CSWVirtualMachineCommands::ExecuteCommandOpenDoor(int nCommand, int nArgs)
{
    unsigned long oidDoor;
    CSWSObject* pObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidDoor) == 0)
    {
        return -2001;
    }
    if (m_nCommandContext == 1)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(
                m_oidObject, (CGameObject**)&pObject) ==
                CGameObjectArray::SUCCESS &&
            pObject->GetArea() != NULL &&
            pObject->AsSWSObject()->m_bCommandable == 1)
        {
            pObject->AsSWSObject()->AddOpenDoorAction(oidDoor, 0);
        }
    }
    return 0;
}
// PC 0x005C3980
int CSWVirtualMachineCommands::ExecuteCommandOpenStore(int nCommand, int nArgs)
{
    unsigned long oidStore;
    unsigned long oidPC;
    int nBonusMarkUp = 0;
    int nBonusMarkDown = 0;

    if (g_pVirtualMachine->StackPopObject(&oidStore) == 0 ||
        g_pVirtualMachine->StackPopObject(&oidPC) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs > 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nBonusMarkUp) == 0 ||
            g_pVirtualMachine->StackPopInteger(&nBonusMarkDown) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    if (nBonusMarkUp <= CSWSStore::MAX_BONUS_PERCENT)
    {
        if (nBonusMarkUp < CSWSStore::MIN_BONUS_PERCENT)
            nBonusMarkUp = CSWSStore::MIN_BONUS_PERCENT;
    }
    else
        nBonusMarkUp = CSWSStore::MAX_BONUS_PERCENT;
    if (nBonusMarkDown <= CSWSStore::MAX_BONUS_PERCENT)
    {
        if (nBonusMarkDown < CSWSStore::MIN_BONUS_PERCENT)
            nBonusMarkDown = CSWSStore::MIN_BONUS_PERCENT;
    }
    else
        nBonusMarkDown = CSWSStore::MAX_BONUS_PERCENT;

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSPlayer* pPlayer = pServerExoApp->GetClientObjectByObjectId(oidPC);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pObject = pServerExoApp->GetGameObject(oidStore);
    if (pObject != NULL)
    {
        CSWSStore* pStore = pObject->AsSWSStore();
        if (pStore != NULL)
        {
            pStore->m_nBonusMarkUp = nBonusMarkUp;
            pStore->m_nBonusMarkDown = nBonusMarkDown;
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            pClientExoApp->GetInGameGui()->ShowStoreGui(oidStore, oidPC);
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OPEN;
            if (pPlayer == NULL)
            {
                CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
                pServerExoApp->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, pServerExoApp->GetPlayerCreatureId(),
                    oidStore, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
            else
            {
                unsigned long oidPlayer = pPlayer->GetGameObject()->GetId();
                CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
                pServerExoApp->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, oidPlayer, oidStore,
                    CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }
    }
    return 0;
}
// PC 0x005C3B70
int CSWVirtualMachineCommands::ExecuteCommandPickUpItem(int nCommand, int nArgs)
{
    unsigned long oidItem;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature =
        (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != NULL)
    {
        pCreature->AddPickUpItemActions(oidItem, OBJECT_INVALID, 0xFFu);
    }
    return 0;
}
// PC 0x005C3BD0
int CSWVirtualMachineCommands::ExecuteCommandPlayAnimation(int nCommand, int nArgs)
{

    int nAnimation;
    float fSpeed;
    float fSeconds;
    CGameObject* pObject;
    int bPlay;

    int bObjectFound;

    if (g_pVirtualMachine->StackPopInteger(&nAnimation) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fSpeed) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fSeconds) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
        if ((unsigned char)pServerExoApp->GetObjectArray()->GetGameObject(
                m_oidObject, &pObject) == CGameObjectArray::SUCCESS)
        {
            bObjectFound = 1;
        }
    }
    int nDialogState = g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState;
    if (nDialogState != 0)
    {
        unsigned long oidClient = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(m_oidObject);
        CGuiInGame* pInGameGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
        unsigned long oidDialog1 = pInGameGui->m_oidDialogObject1;
        pInGameGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
        unsigned long oidDialog2 = pInGameGui->m_oidDialogObject2;
        if (oidClient == oidDialog1)
        {
            CSWCObject* pClientObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidDialog1);
            if (pClientObject != NULL && pClientObject->AsSWCObject() != NULL)
                pClientObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 0);
        }
        if (oidClient == oidDialog2)
        {
            CSWCObject* pClientObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidDialog2);
            if (pClientObject != NULL && pClientObject->AsSWCObject() != NULL)
                pClientObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 0);
        }
    }
    int nScriptAnimation = nAnimation;
    if (bObjectFound == 1)
    {
        if (pObject->AsSWSCreature() != NULL)
        {
            if (nAnimation > CREATURE_ANIMATION_COMBAT_IDLE)
            {
                nAnimation -= CREATURE_ANIMATION_COMBAT_IDLE;
            }
            else
            switch (nAnimation)
            {
            case ANIMATION_LOOPING_PAUSE:
                nAnimation = CREATURE_ANIMATION_COMBAT_IDLE;
                break;
            case ANIMATION_LOOPING_PAUSE2:
                nAnimation = CREATURE_ANIMATION_LOOPING_PAUSE2;
                break;
            case ANIMATION_LOOPING_LISTEN:
                nAnimation = CREATURE_ANIMATION_LOOPING_LISTEN;
                break;
            case ANIMATION_LOOPING_MEDITATE:
                nAnimation = CREATURE_ANIMATION_LOOPING_MEDITATE;
                break;
            case ANIMATION_LOOPING_WORSHIP:
                nAnimation = CREATURE_ANIMATION_LOOPING_WORSHIP;
                break;
            case ANIMATION_LOOPING_TALK_NORMAL:
                nAnimation = CREATURE_ANIMATION_LOOPING_TALK_NORMAL;
                break;
            case ANIMATION_LOOPING_TALK_PLEADING:
                nAnimation = CREATURE_ANIMATION_LOOPING_TALK_PLEADING;
                break;
            case ANIMATION_LOOPING_TALK_FORCEFUL:
                nAnimation = CREATURE_ANIMATION_LOOPING_TALK_FORCEFUL;
                break;
            case ANIMATION_LOOPING_TALK_LAUGHING:
                nAnimation = CREATURE_ANIMATION_LOOPING_TALK_LAUGHING;
                break;
            case ANIMATION_LOOPING_TALK_SAD:
                nAnimation = CREATURE_ANIMATION_LOOPING_TALK_SAD;
                break;
            case ANIMATION_LOOPING_GET_LOW:
                nAnimation = CREATURE_ANIMATION_LOOPING_GET_LOW;
                break;
            case ANIMATION_LOOPING_GET_MID:
                nAnimation = CREATURE_ANIMATION_LOOPING_GET_MID;
                break;
            case ANIMATION_LOOPING_PAUSE_TIRED:
                nAnimation = CREATURE_ANIMATION_LOOPING_PAUSE_TIRED;
                break;
            case ANIMATION_LOOPING_PAUSE_DRUNK:
                nAnimation = CREATURE_ANIMATION_LOOPING_PAUSE_DRUNK;
                break;
            case ANIMATION_LOOPING_FLIRT:
                nAnimation = CREATURE_ANIMATION_LOOPING_FLIRT;
                break;
            case ANIMATION_LOOPING_USE_COMPUTER:
                nAnimation = CREATURE_ANIMATION_LOOPING_USE_COMPUTER;
                break;
            case ANIMATION_LOOPING_DANCE:
                nAnimation = CREATURE_ANIMATION_LOOPING_DANCE;
                break;
            case ANIMATION_LOOPING_DANCE1:
                nAnimation = CREATURE_ANIMATION_LOOPING_DANCE1;
                break;
            case ANIMATION_LOOPING_HORROR:
                nAnimation = CREATURE_ANIMATION_LOOPING_HORROR;
                break;
            case ANIMATION_LOOPING_READY:
                nAnimation = CREATURE_ANIMATION_PAUSE_READY;
                break;
            case ANIMATION_LOOPING_DEACTIVATE:
                nAnimation = CREATURE_ANIMATION_LOOPING_DEACTIVATE;
                break;
            case ANIMATION_LOOPING_SPASM:
                nAnimation = CREATURE_ANIMATION_LOOPING_SPASM;
                break;
            case ANIMATION_LOOPING_SLEEP:
                nAnimation = CREATURE_ANIMATION_LOOPING_SLEEP;
                break;
            case ANIMATION_LOOPING_PRONE:
                nAnimation = CREATURE_ANIMATION_LOOPING_PRONE;
                break;
            case ANIMATION_LOOPING_PAUSE3:
                nAnimation = CREATURE_ANIMATION_LOOPING_PAUSE3;
                break;
            case ANIMATION_LOOPING_WELD:
                nAnimation = CREATURE_ANIMATION_LOOPING_WELD;
                break;
            case ANIMATION_LOOPING_DEAD:
                nAnimation = CREATURE_ANIMATION_LOOPING_DEAD;
                break;
            case ANIMATION_LOOPING_TALK_INJURED:
                nAnimation = CREATURE_ANIMATION_LOOPING_TALK_INJURED;
                break;
            case ANIMATION_LOOPING_LISTEN_INJURED:
                nAnimation = CREATURE_ANIMATION_LOOPING_LISTEN_INJURED;
                break;
            case ANIMATION_LOOPING_TREAT_INJURED:
                nAnimation = CREATURE_ANIMATION_LOOPING_TREAT_INJURED;
                break;
            case ANIMATION_LOOPING_DEAD_PRONE:
                nAnimation = CREATURE_ANIMATION_LOOPING_DEAD_PRONE;
                break;
            case ANIMATION_LOOPING_KNEEL_TALK_ANGRY:
                nAnimation = CREATURE_ANIMATION_LOOPING_KNEEL_TALK_ANGRY;
                break;
            case ANIMATION_LOOPING_KNEEL_TALK_SAD:
                nAnimation = CREATURE_ANIMATION_LOOPING_KNEEL_TALK_SAD;
                break;
            case ANIMATION_LOOPING_CHECK_BODY:
                nAnimation = CREATURE_ANIMATION_LOOPING_CHECK_BODY;
                break;
            case ANIMATION_LOOPING_UNLOCK_DOOR:
                nAnimation = CREATURE_ANIMATION_LOCKPICK_DOOR;
                break;
            case ANIMATION_LOOPING_SIT_AND_MEDITATE:
                nAnimation = CREATURE_ANIMATION_LOOPING_SIT_AND_MEDITATE;
                break;
            case ANIMATION_LOOPING_SIT_CHAIR:
                nAnimation = CREATURE_ANIMATION_LOOPING_SIT_CHAIR;
                break;
            case ANIMATION_LOOPING_SIT_CHAIR_DRINK:
                nAnimation = CREATURE_ANIMATION_LOOPING_SIT_CHAIR_DRINK;
                break;
            case ANIMATION_LOOPING_SIT_CHAIR_PAZAK:
                nAnimation = CREATURE_ANIMATION_LOOPING_SIT_CHAIR_PAZAK;
                break;
            case ANIMATION_LOOPING_SIT_CHAIR_COMP1:
                nAnimation = CREATURE_ANIMATION_LOOPING_SIT_CHAIR_COMP1;
                break;
            case ANIMATION_LOOPING_SIT_CHAIR_COMP2:
                nAnimation = CREATURE_ANIMATION_LOOPING_SIT_CHAIR_COMP2;
                break;
            case ANIMATION_LOOPING_DIVE_ROLL:
                break;
            case ANIMATION_LOOPING_CLOSED:
                nAnimation = CREATURE_ANIMATION_LOOPING_CLOSED;
                break;
            case ANIMATION_LOOPING_STEALTH:
                nAnimation = CREATURE_ANIMATION_SPECIAL_WALK;
                break;
            case ANIMATION_LOOPING_CHOKE_WORKING:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_CHOKE;
                break;
            case ANIMATION_LOOPING_MEDITATE_STAND:
                nAnimation = CREATURE_ANIMATION_LOOPING_MEDITATE_STAND;
                break;
            case ANIMATION_FIREFORGET_HEAD_TURN_LEFT:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_HEAD_TURN_LEFT;
                break;
            case ANIMATION_FIREFORGET_HEAD_TURN_RIGHT:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_HEAD_TURN_RIGHT;
                break;
            case ANIMATION_FIREFORGET_PAUSE_SCRATCH_HEAD:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_PAUSE_SCRATCH_HEAD;
                break;
            case ANIMATION_FIREFORGET_PAUSE_BORED:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_PAUSE_BORED;
                break;
            case ANIMATION_FIREFORGET_SALUTE:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_SALUTE;
                break;
            case ANIMATION_FIREFORGET_BOW:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_BOW;
                break;
            case ANIMATION_FIREFORGET_GREETING:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_GREETING;
                break;
            case ANIMATION_FIREFORGET_TAUNT:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_TAUNT;
                break;
            case ANIMATION_FIREFORGET_VICTORY1:
            case ANIMATION_FIREFORGET_VICTORY2:
            case ANIMATION_FIREFORGET_VICTORY3:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_VICTORY;
                break;
            case ANIMATION_FIREFORGET_INJECT:
            {
                CExoString sModelType;
                {
                    CExoString sColumn("MODELTYPE");
                    CSWSCreature* pCreature = pObject->AsSWSCreature();
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    p2DArrays->m_pAppearance->GetCExoStringEntry(
                        pCreature->m_cAppearanceInfo.m_nAppearance, sColumn, &sModelType);
                }
                if (sModelType.CompareNoCase("B") || sModelType.CompareNoCase("F"))
                    nAnimation = CREATURE_ANIMATION_FIREFORGET_INJECT;
                else
                    nAnimation = CREATURE_ANIMATION_COMBAT_IDLE;
                break;
            }
            case ANIMATION_FIREFORGET_USE_COMPUTER:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_USE_COMPUTER;
                break;
            case ANIMATION_FIREFORGET_PERSUADE:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_PERSUADE;
                break;
            case ANIMATION_FIREFORGET_ACTIVATE:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_ACTIVATE;
                break;
            case ANIMATION_FIREFORGET_THROW_HIGH:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_THROW_HIGH;
                break;
            case ANIMATION_FIREFORGET_THROW_LOW:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_THROW_LOW;
                break;
            case ANIMATION_FIREFORGET_CUSTOM01:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_CUSTOM01;
                break;
            case ANIMATION_FIREFORGET_TREAT_INJURED:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_TREAT_INJURED;
                break;
            case ANIMATION_FIREFORGET_FORCE_CAST:
                nAnimation = CREATURE_ANIMATION_SPELLCAST_1;
                break;
            case ANIMATION_FIREFORGET_OPEN:
                nAnimation = CREATURE_ANIMATION_FIREFORGET_OPEN;
                break;
            case ANIMATION_LOOPING_RAGE:
            case ANIMATION_FIREFORGET_SCREAM:
                nAnimation = CREATURE_ANIMATION_LOOPING_RAGE;
                break;
            default:
                nAnimation = CREATURE_ANIMATION_COMBAT_IDLE;
                break;
            }
        }
        else if (pObject->AsSWSPlaceable() != NULL)
        {
            switch (nAnimation)
            {
            case ANIMATION_PLACEABLE_ACTIVATE:
                nAnimation = PLACEABLE_ANIMATION_ACTIVATE;
                break;
            case ANIMATION_PLACEABLE_DEACTIVATE:
                nAnimation = PLACEABLE_ANIMATION_DEACTIVATE;
                break;
            case ANIMATION_PLACEABLE_OPEN:
                nAnimation = PLACEABLE_ANIMATION_OPEN;
                break;
            case ANIMATION_PLACEABLE_CLOSE:
                nAnimation = PLACEABLE_ANIMATION_CLOSED;
                break;
            case ANIMATION_PLACEABLE_ANIMLOOP01:
                nAnimation = PLACEABLE_ANIMATION_ANIMLOOP01;
                break;
            case ANIMATION_PLACEABLE_ANIMLOOP02:
                nAnimation = PLACEABLE_ANIMATION_ANIMLOOP02;
                break;
            case ANIMATION_PLACEABLE_ANIMLOOP03:
                nAnimation = PLACEABLE_ANIMATION_ANIMLOOP03;
                break;
            case ANIMATION_PLACEABLE_ANIMLOOP04:
                nAnimation = PLACEABLE_ANIMATION_ANIMLOOP04;
                break;
            case ANIMATION_PLACEABLE_ANIMLOOP05:
                nAnimation = PLACEABLE_ANIMATION_ANIMLOOP05;
                break;
            case ANIMATION_PLACEABLE_ANIMLOOP06:
                nAnimation = PLACEABLE_ANIMATION_ANIMLOOP06;
                break;
            case ANIMATION_PLACEABLE_ANIMLOOP07:
                nAnimation = PLACEABLE_ANIMATION_ANIMLOOP07;
                break;
            case ANIMATION_PLACEABLE_ANIMLOOP08:
                nAnimation = PLACEABLE_ANIMATION_ANIMLOOP08;
                break;
            case ANIMATION_PLACEABLE_ANIMLOOP09:
                nAnimation = PLACEABLE_ANIMATION_ANIMLOOP09;
                break;
            case ANIMATION_PLACEABLE_ANIMLOOP10:
                nAnimation = PLACEABLE_ANIMATION_ANIMLOOP10;
                break;
            default:
                nAnimation = PLACEABLE_ANIMATION_BASE;
                break;
            }
        }
        if (fSeconds < 0.0f)
        {
            if ((nScriptAnimation >= ANIMATION_LOOPING_PAUSE &&
                  nScriptAnimation <= ANIMATION_LOOPING_MEDITATE_STAND) ||
                 nScriptAnimation > CREATURE_ANIMATION_COMBAT_IDLE)
            {
                CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(m_oidObject);
                if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
                    pGameObject->AsSWSObject()->SetAnimation(nAnimation);
            }
        }
        else
        {
          if (pObject->AsSWSObject()->m_bCommandable != 0)
          {
            bPlay = 1;
            if (nCommand == COMMAND_PLAY_ANIMATION)
            {
                pObject->AsSWSObject()->ClearAllActions(1);
                pObject->AsSWSObject()->AddActionToFront(CSWSObject::ACTION_PLAY_ANIMATION,
                    CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_INTEGER, &nAnimation,
                    CSWSObject::ACTION_PARAM_FLOAT, &fSpeed,
                    CSWSObject::ACTION_PARAM_FLOAT, &fSeconds,
                    CSWSObject::ACTION_PARAM_INTEGER, &bPlay,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL);
            }
            else
            {
                pObject->AsSWSObject()->AddAction(CSWSObject::ACTION_PLAY_ANIMATION,
                    CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_INTEGER, &nAnimation,
                    CSWSObject::ACTION_PARAM_FLOAT, &fSpeed,
                    CSWSObject::ACTION_PARAM_FLOAT, &fSeconds,
                    CSWSObject::ACTION_PARAM_INTEGER, &bPlay,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL);
            }
          }
        }
    }
    return 0;
}
// PC 0x005C4560
int CSWVirtualMachineCommands::ExecuteCommandPlayRoomAnimation(int nCommand,
                                                            int nArgs)
{

    CExoString sRoom;
    int nAnimation;
    if (g_pVirtualMachine->StackPopString(&sRoom) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAnimation) == 0)
    {
        return ERROR_STACK_POP;
    }

    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    if (pModule != NULL)
    {
        CSWCArea* pArea = pModule->m_pInfo;
        if (pArea != NULL)
        {
            CSWCRoom* pRoom = pArea->GetRoom(sRoom.CStr());
            if (pRoom != NULL)
            {
                pRoom->PlayAnimation(nAnimation);
                return 0;
            }
        }
    }
    return 0;
}
// PC 0x005C4660
int CSWVirtualMachineCommands::ExecuteCommandPlayMovie(int nCommand, int nArgs)
{

    CExoString sMovie;
    int nMovieResult;

    if (g_pVirtualMachine->StackPopString(&sMovie) == 0)
    {
        return ERROR_STACK_POP;
    }

    g_pExoInput->ClearEvents();
    g_pAppManager->m_pClientExoApp->ResetDriveAcceleration();
    int bKeepMusic = 0;
    if (g_pVirtualMachine->StackPopInteger(&bKeepMusic) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (bKeepMusic == 0)
    {
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL);
    }
#ifdef _XBOX
    g_pAppManager->m_pClientExoApp->HideLoadScreen(0);
#else
    g_pAppManager->m_pClientExoApp->HideLoadScreen(0, 1);
#endif
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->PlayMovie(sMovie, nMovieResult, 0, 0, bKeepMusic != 0);
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    return 0;
}
// PC 0x005C4790
int CSWVirtualMachineCommands::ExecuteCommandPlayPazaak(int nCommand, int nArgs)
{

    int nOpponentPazaakDeck;
    if (g_pVirtualMachine->StackPopInteger(&nOpponentPazaakDeck) == 0)
    {
        return ERROR_STACK_POP;
    }

    CExoString sEndScript;
    if (g_pVirtualMachine->StackPopString(&sEndScript) == 0)
    {
        return ERROR_STACK_POP;
    }
    int nMaxWager;
    if (g_pVirtualMachine->StackPopInteger(&nMaxWager) == 0)
    {
        return ERROR_STACK_POP;
    }

    int bShowTutorial = 0;
    if (nArgs >= 4)
    {
        if (g_pVirtualMachine->StackPopInteger(&bShowTutorial) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    unsigned long oidOpponent = OBJECT_INVALID;
    if (nArgs >= 5)
    {
        if (g_pVirtualMachine->StackPopObject(&oidOpponent) == 0)
        {
            return ERROR_STACK_POP;
        }
    }

    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->StartPazaakGame(
        nOpponentPazaakDeck, sEndScript, nMaxWager, bShowTutorial != 0,
        oidOpponent);
    return 0;
}
// PC 0x005C48E0
int CSWVirtualMachineCommands::ExecuteCommandPlayRumblePattern(int nCommand,
                                                               int nArgs)
{
    int nPattern;
    CClientExoApp* pClientExoApp;
    int nResult;

    if (g_pVirtualMachine->StackPopInteger(&nPattern) == 0)
    {
        return -2001;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    nResult = pClientExoApp->PlayRumblePattern(nPattern);
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C4930
int CSWVirtualMachineCommands::ExecuteCommandPlaySound(int nCommand, int nArgs)
{
    CExoString sSoundName;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    if (g_pVirtualMachine->StackPopString(&sSoundName) == 0)
    {
        return -2001;
    }
    if (m_nCommandContext == 1)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                    ->GetGameObject(m_oidObject,
                                    (struct CGameObject**)&pObject) ==
                CGameObjectArray::SUCCESS &&
            pObject->m_bCommandable != 0)
        {
            pObject->AddAction(0x17u, 0xFFFFu, 4u, &sSoundName, 0, NULL, 0,
                               NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                               NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                               NULL);
        }
    }
    return 0;
}
// PC 0x005C4A40
int CSWVirtualMachineCommands::ExecuteCommandPlayVisualAreaEffect(int nCommand,
                                                                 int nArgs)
{
    int nEffectID;
    CScriptLocation* pLocation;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWSArea* pArea;

    if (g_pVirtualMachine->StackPopInteger(&nEffectID) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION, (void**)&pLocation) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(
        pServerExoApp->GetPlayerCreatureId());
    if (pCreature != NULL)
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            pArea->PlayVisualEffect(nEffectID, pLocation->m_vPosition);
        }
        if (pLocation != NULL)
        {
            delete pLocation;
        }
    }
    return 0;
}
// PC 0x005C4AE0
int CSWVirtualMachineCommands::ExecuteCommandPopUpGUIPanel(int nCommand, int nArgs)
{

    unsigned long oidPC = OBJECT_INVALID;
    int nGUIPanel = 0;
    int bRespawnButtonEnabled = 1;
    int bWaitForHelpButtonEnabled = 1;
    int nHelpStringReference = 0;
    CExoString sHelpString("");

    if (g_pVirtualMachine->StackPopObject(&oidPC) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nCommand == COMMAND_POP_UP_GUI_PANEL)
    {
        if (g_pVirtualMachine->StackPopInteger(&nGUIPanel) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    else if (nCommand == COMMAND_POP_UP_DEATH_GUI_PANEL)
    {
        if (g_pVirtualMachine->StackPopInteger(&bRespawnButtonEnabled) == 0 ||
            g_pVirtualMachine->StackPopInteger(&bWaitForHelpButtonEnabled) == 0 ||
            g_pVirtualMachine->StackPopInteger(&nHelpStringReference) == 0 ||
            g_pVirtualMachine->StackPopString(&sHelpString) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    if (oidPC != OBJECT_INVALID)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoApp->GetSWSMessage()->SendServerToPlayerPopUpGUIPanel(
            oidPC, nGUIPanel, bRespawnButtonEnabled != 0,
            bWaitForHelpButtonEnabled != 0, nHelpStringReference, sHelpString);
    }
    return 0;
}
// PC 0x005C4C90
int CSWVirtualMachineCommands::ExecuteCommandPrintFloat(int nCommand, int nArgs)
{
    int nDecimals;
    int nWidth;

    int bPrefix;
    float fFloat;

    if (g_pVirtualMachine->StackPopFloat(&fFloat) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nWidth) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (nWidth >= 18)
            nWidth = 18;
        else if (nWidth < 0)
            nWidth = 0;
    }
    else
        nWidth = 18;

    if (nArgs >= 3)
    {
        if (g_pVirtualMachine->StackPopInteger(&nDecimals) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (nDecimals >= 10)
            nDecimals = 9;
        else if (nDecimals < 0)
            nDecimals = 0;
    }
    else
        nDecimals = 9;

    if (nArgs >= 4)
    {
        if (g_pVirtualMachine->StackPopInteger(&bPrefix) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    else
        bPrefix = 0;

    CExoString sFormat;
    if (bPrefix == 0)
    {
        if (nWidth >= 10)
            sFormat.Format("%%%2d.%02df\n", nWidth, nDecimals);
        else
            sFormat.Format("%%%1d.%02df\n", nWidth, nDecimals);
    }
    else
    {
        if (nWidth >= 10)
            sFormat.Format("PRINTFLOAT2: %%%2d.%02df\n", nWidth, nDecimals);
        else
            sFormat.Format("PRINTFLOAT2: %%%1d.%02df\n", nWidth, nDecimals);
    }

    if (!bDisableScriptLogging)
        EXOLOG(sFormat.CStr(), fFloat);
    return 0;
}
// PC 0x005C4E00
int CSWVirtualMachineCommands::ExecuteCommandPrintInteger(int nCommand,
                                                          int nArgs)
{
    int nResult;
    int nResult2;

    if (g_pVirtualMachine->StackPopInteger(&nResult2) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    return 0;
}
// PC 0x005C4E50
int CSWVirtualMachineCommands::ExecuteCommandPrintLogEntry(int nCommand,
                                                         int nArgs)
{
    CExoString sLogEntry;

    if (g_pVirtualMachine->StackPopString(&sLogEntry) == 0)
    {
        return ERROR_STACK_POP;
    }

    CExoString sTimestamp;
    g_pExoBase->m_pExoDebug->GetCurrentTimestamp(sTimestamp);

    if (!bDisableScriptLogging)
        EXOLOG("%s [%s]\n", sLogEntry.CStr(), sTimestamp.CStr());
    return 0;
}
// PC 0x005C4F30
int CSWVirtualMachineCommands::ExecuteCommandPrintObject(int nCommand,
                                                         int nArgs)
{
    int nResult;
    unsigned long oidResult;

    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    return 0;
}
// PC 0x005C4F80
int CSWVirtualMachineCommands::ExecuteCommandPrintString(int nCommand, int nArgs)
{
    CExoString sString;

    int bPrintPrefix;

    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&bPrintPrefix) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    else
    {
        bPrintPrefix = FALSE;
    }

    if (bPrintPrefix == TRUE)
    {
        if (!bDisableScriptLogging)
            EXOLOG("PRINTSTRING: %s\n", sString.CStr());
        g_pExoBase->m_pExoDebug->m_sLogDirectory.Format("PRINTSTRING: %s\n", sString.CStr());
    }
    else
    {
        if (!bDisableScriptLogging)
            EXOLOG("%s\n", sString.CStr());
        g_pExoBase->m_pExoDebug->m_sLogDirectory.Format("%s\n", sString.CStr());
    }
    if (!bDisableScriptLogging)
    {
        g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory);
        g_pExoBase->m_pExoDebug->FlushScriptLogFile();
    }
    return 0;
}
// PC 0x005C50D0
int CSWVirtualMachineCommands::ExecuteCommandPrintVector(int nCommand,
                                                         int nArgs)
{
    int nResult;
    Vector vResult;

    if (g_pVirtualMachine->StackPopVector(&vResult) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    return 0;
}
// PC 0x005C5130
int CSWVirtualMachineCommands::ExecuteCommandPlayOverlayAnimation(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nAnimation;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAnimation) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidCreature);
    if (pGameObject != NULL && pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            CSWCCreature* pClientCreature = pCreature->GetClientCreature();
            if (pClientCreature != NULL)
            {
                if (nAnimation != ANIMATION_FIREFORGET_DIVE_ROLL)
                {
                    return 0;
                }
                pClientCreature->GetAnimationBase()->SetOverlayAnimation(
                    pClientCreature->GetAnimationBase()->ServerToClientAnimation(10509), 1);
            }
        }
    }
    return 0;
}
// PC 0x005C51E0
int CSWVirtualMachineCommands::ExecuteCommandPutDownItem(int nCommand,
                                                         int nArgs)
{
    unsigned long oidItem;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(m_oidObject);
    if (pCreature != 0)
    {
        pCreature->AddDropItemActions(oidItem, pCreature->m_vPosition, 0);
    }
    return 0;
}
// PC 0x005C5240
int CSWVirtualMachineCommands::ExecuteCommandRandom(int nCommand, int nArgs)
{
    int nRandom = 0;
    int nMaximum;

    if (g_pVirtualMachine->StackPopInteger(&nMaximum) == 0)
    {
        return -2001;
    }

    if (nMaximum != 0)
    {
        nRandom = rand() % nMaximum;
        if (nMaximum < 0)
        {
            nRandom = -nRandom;
        }
    }

    return g_pVirtualMachine->StackPushInteger(nRandom) != 0 ? 0 : -2000;
}
// PC 0x005C52A0
int CSWVirtualMachineCommands::ExecuteCommandRandomName(int nCommand, int nArgs)
{

    CSWNameGen cNameGen;
    CExoString sName;

    cNameGen.LoadNameTable("names");
    sName = cNameGen.GetRandomName(0);
    cNameGen.UnloadNameTable();

    if (g_pVirtualMachine->StackPushString(sName) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C53B0
int CSWVirtualMachineCommands::ExecuteCommandRemoveAvailableNPC(int nCommand,
                                                                int nArgs)
{
    int nNPC;
    CServerExoApp* pServerExoApp;
    int nResult2;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nResult2 = pServerExoApp->GetPartyTable()->RemoveNPC(nNPC);
    return g_pVirtualMachine->StackPushInteger(nResult2) != 0 ? 0 : -2000;
}
// PC 0x005C5410
int CSWVirtualMachineCommands::ExecuteCommandRemoveFromParty(int nCommand,
                                                             int nArgs)
{
    unsigned long oidPC;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWSFaction* pFaction;

    if (g_pVirtualMachine->StackPopObject(&oidPC) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(oidPC);
    if (pCreature != NULL && pCreature->m_nAreaWaitState != 0)
    {
        pFaction = pCreature->GetFaction();
        if (pFaction != NULL)
        {
            pFaction->RemoveMember(oidPC);
        }
    }
    return 0;
}
// PC 0x005C5530
int CSWVirtualMachineCommands::ExecuteCommandRemovePartyMember(int nCommand,
                                                               int nArgs)
{
    CServerExoApp* pServerExoApp;
    CSWPartyTable* pPartyTable;
    unsigned int oidNPC;
    int nResult;
    CClientExoApp* pClientExoApp;
    CSWCCreature* pClientCreature;
    CSWSCreature* pServerCreature;
    int nNPC;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pPartyTable = pServerExoApp->GetPartyTable();
    oidNPC = pPartyTable->GetNPCObject(nNPC, 0, 1);
    nResult = pPartyTable->RemoveMember(nNPC);
    if (nResult != 0)
    {
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientCreature = pClientExoApp->GetCreatureByGameObjectID(
            pClientExoApp->ServerToClientObjectId(oidNPC));
        pServerCreature = pServerExoApp->GetCreatureByGameObjectID(oidNPC);
        if (pClientCreature != NULL)
        {
            pClientCreature->SetInParty(0);
        }
        if (pServerCreature != NULL)
        {
            pServerCreature->SetInParty(0, 1);
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}

inline unsigned char CSWSpell::GetInnateLevel() { return m_nInnateLevel; }
inline unsigned char CSWSpell::GetUserType() { return m_nUserType; }
inline int CSWSCreature::GetCasterLevelOverrideEnabled() { return m_bCasterLevelOverride; }
// PC 0x005C55F0
int CSWVirtualMachineCommands::ExecuteCommandResistSpell(int nCommand,
                                                       int nArgs)
{
    unsigned long oidCaster;
    unsigned long oidTarget;
    CGameObject* pCasterGameObject;
    CGameObject* pTargetGameObject;
    CGameObject* pOwnerGameObject;
    CSWSCreature* pCasterCreature;
    CSWSCreature* pTargetCreature;
    CSWSpell* pSpell;
    int nSpellLevel;
    int nResult;
    int nForceResistance;
    int nRoll;
    CSWCCMessageData* pMessageData;
    CSWSPlayer* pPlayer;

    int nStackPopObject = g_pVirtualMachine->StackPopObject(&oidCaster);
    if (nStackPopObject == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    pCasterGameObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidCaster);
    pTargetGameObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pCasterGameObject != NULL && pTargetGameObject != NULL)
    {
        int nSpellId = pCasterGameObject->AsSWSObject()->m_nLastSpellId;
        pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
        if (pSpell != NULL)
        {
            pOwnerGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(
                m_oidObject);
            if (pOwnerGameObject != NULL &&
                pOwnerGameObject->AsSWSAreaOfEffectObject() != NULL)
            {
                nSpellLevel =
                    pOwnerGameObject->AsSWSAreaOfEffectObject()->m_nSpellLevel;
            }
            else if (pCasterGameObject->m_nObjectType ==
                     CGameObject::OBJECT_TYPE_CREATURE)
            {
                pCasterCreature = pCasterGameObject->AsSWSCreature();
                if (pCasterCreature->GetCasterLevelOverrideEnabled() == 1)
                {
                    nSpellLevel = pCasterCreature->m_nCasterLevelOverride;
                }
                else
                {
                    unsigned char nClass = pCasterCreature->m_c1D4;

                    if (nClass == CSWSObject::CASTING_CLASS_INNATE_FF ||
                        nClass == CSWSObject::CASTING_CLASS_INNATE_FE)
                    {
                        nSpellLevel = pSpell->GetInnateLevel() * 2 - 1;
                    }
                    else
                    {
                        nSpellLevel = pCasterCreature->GetStats()->GetLevel(0);
                    }
                }
            }
            else
            {
                nSpellLevel = pSpell->GetInnateLevel() * 2 - 1;
            }

            if (pTargetGameObject->m_nObjectType ==
                CGameObject::OBJECT_TYPE_CREATURE)
            {
                pTargetCreature = pTargetGameObject->AsSWSCreature();
                nResult = pTargetCreature->DoSpellLevelAbsorption(
                    pCasterGameObject->AsSWSObject());
                if (nResult == CSWSObject::SPELL_RESISTANCE_UNRESOLVED)
                {
                    nResult = pTargetCreature->DoSpellImmunity(
                        pCasterGameObject->AsSWSObject());
                }

                nForceResistance = 0;
                if (nResult == CSWSObject::SPELL_RESISTANCE_UNRESOLVED)
                {
                    if (pSpell->GetUserType() == CSWSpell::USER_TYPE_FORCE_POWER)
                    {
                        nForceResistance = (char)pTargetCreature->GetStats()
                                               ->GetForceResistance();
                        if (nForceResistance > 0)
                        {
                            int nTotal;
                            unsigned short nRollDice = g_pRules->RollDice(1, 20);
                            nRoll = nTotal = nRollDice;
                            nTotal += nSpellLevel;
                            if (nTotal < nForceResistance)
                            {
                                nResult =
                                    CSWSObject::SPELL_RESISTANCE_RESISTED;
                            }
                            else
                            {
                                nResult = 0;
                            }
                        }
                        else
                        {
                            nResult = 0;
                        }
                    }
                    else if (pSpell->GetUserType() == 2)
                    {
                        nResult = 0;
                    }
                }
            }
            else
            {
                nResult = 0;
            }
        }
        else
        {
            nResult = 0;
        }
    }
    else
    {
        nResult = 0;
    }
    if (nResult == CSWSObject::SPELL_RESISTANCE_RESISTED)
    {
        pMessageData = new CSWCCMessageData;
        pMessageData->SetObjectID(0, oidTarget);
        pMessageData->SetInteger(0, 1);
        pPlayer = g_pAppManager->GetServerExoApp()
                      ->GetClientObjectByObjectId(oidCaster);
        if (pPlayer != NULL)
        {
            g_pAppManager->GetServerExoApp()->GetSWSMessage()
                ->SendServerToPlayerCCMessage(pPlayer->GetPlayerId(),
                                              CSWSMessage::CC_MESSAGE_FORCE_RESISTANCE, pMessageData,
                                              NULL);
        }
        pPlayer = g_pAppManager->GetServerExoApp()
                      ->GetClientObjectByObjectId(oidTarget);
        if (pPlayer != NULL)
        {
            g_pAppManager->GetServerExoApp()->GetSWSMessage()
                ->SendServerToPlayerCCMessage(pPlayer->GetPlayerId(),
                                              CSWSMessage::CC_MESSAGE_FORCE_RESISTANCE, pMessageData,
                                              NULL);
        }
        delete pMessageData;

        pMessageData = new CSWCCMessageData;
        pMessageData->SetObjectID(0, oidTarget);
        pMessageData->SetObjectID(1, oidCaster);
        pMessageData->SetInteger(0, nRoll);
        pMessageData->SetInteger(1, nSpellLevel);
        pMessageData->SetInteger(2, nForceResistance);
        pPlayer = g_pAppManager->GetServerExoApp()
                      ->GetClientObjectByObjectId(oidCaster);
        if (pPlayer != NULL)
        {
            g_pAppManager->GetServerExoApp()->GetSWSMessage()
                ->SendServerToPlayerCCMessage(pPlayer->GetPlayerId(),
                                              CSWSMessage::CC_MESSAGE_SPELL_RESISTANCE_CHECK, pMessageData,
                                              NULL);
        }
        pPlayer = g_pAppManager->GetServerExoApp()
                      ->GetClientObjectByObjectId(oidTarget);
        if (pPlayer != NULL)
        {
            g_pAppManager->GetServerExoApp()->GetSWSMessage()
                ->SendServerToPlayerCCMessage(pPlayer->GetPlayerId(),
                                              CSWSMessage::CC_MESSAGE_SPELL_RESISTANCE_CHECK, pMessageData,
                                              NULL);
        }
        delete pMessageData;
    }
    if (g_pVirtualMachine->StackPushInteger(nResult) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C59E0
int CSWVirtualMachineCommands::ExecuteCommandRollDice(int nCommand, int nArgs)
{

    int nNumDice = 1;
    int nDie;

    switch (nCommand)
    {
    case COMMAND_D2: nDie = 2; break;
    case COMMAND_D3: nDie = 3; break;
    case COMMAND_D4: nDie = 4; break;
    case COMMAND_D6: nDie = 6; break;
    case COMMAND_D8: nDie = 8; break;
    case COMMAND_D10: nDie = 10; break;
    case COMMAND_D12: nDie = 12; break;
    case COMMAND_D20: nDie = 20; break;
    case COMMAND_D100: nDie = 100; break;
    }

    if (nArgs == 1)
    {
        if (g_pVirtualMachine->StackPopInteger(&nNumDice) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    if (nNumDice <= 1)
    {
        nNumDice = 1;
    }

    int nResult = (unsigned short)g_pRules->RollDice(nNumDice, nDie);
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C5AD0
int CSWVirtualMachineCommands::ExecuteCommandSaveNPCState(int nCommand,
                                                          int nArgs)
{
    int nNPC;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetPartyTable()->SaveMember(nNPC, 0);
    return 0;
}
// PC 0x005C5B10
int CSWVirtualMachineCommands::ExecuteCommandSavingThrow(int nCommand, int nArgs)
{
    unsigned long oidTarget = OBJECT_INVALID;
    unsigned long oidSaveVersus = m_oidObject;
    int nDC;
    int nSaveVsType;
    CGameObject* pObject;
    int nSaveType;
    int nResult = 0;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nDC) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs > 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nSaveVsType) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (g_pVirtualMachine->StackPopObject(&oidSaveVersus) == 0)
        {
            return ERROR_STACK_POP;
        }
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            oidTarget, &pObject) == CGameObjectArray::SUCCESS &&
        pObject->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
    {
        switch (nCommand)
        {
        case COMMAND_FORTITUDE_SAVE:
            nSaveType = CSWSCreature::SAVING_THROW_FORT;
            break;
        case COMMAND_REFLEX_SAVE:
            nSaveType = CSWSCreature::SAVING_THROW_REFLEX;
            break;
        case COMMAND_WILL_SAVE:
            nSaveType = CSWSCreature::SAVING_THROW_WILL;
            break;
        }
        nResult = pObject->AsSWSCreature()->SavingThrowRoll(
            nSaveType, (unsigned short)nDC, (unsigned char)nSaveVsType,
            oidSaveVersus, 1, 0, 0);
    }

    if (g_pVirtualMachine->StackPushInteger(nResult) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C5C30
int CSWVirtualMachineCommands::ExecuteCommandSendMessageToPC(int nCommand,
                                                             int nArgs)
{
    CExoString sResult;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWCCMessageData* pSWCCMessageData;

    if (nArgs != 2)
    {
        EXOXBOXASSERT(0);
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopString(&sResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature =
        (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (pCreature != NULL)
    {
        pSWCCMessageData = new CSWCCMessageData();
        pSWCCMessageData->SetString(0, sResult);
        pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_CUSTOM_STRING, pSWCCMessageData);
    }
    return 0;
}
// PC 0x005C5D40
int CSWVirtualMachineCommands::ExecuteCommandSetAreaTransitionBMP(int nCommand,
                                                                  int nArgs)
{
    int nResult;
    int nPredefinedAreaTransition;
    CExoString sCustomAreaTransitionBMP;
    CServerExoApp* pServerExoApp;
    CSWSPlayer* pPlayer;

    nResult = 0;
    if (g_pVirtualMachine->StackPopInteger(&nPredefinedAreaTransition) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopString(&sCustomAreaTransitionBMP) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pPlayer = pServerExoApp->GetClientObjectByObjectId(m_oidObject);
    if (pPlayer != NULL)
    {
        pPlayer->SetAreaTransitionBMP(nPredefinedAreaTransition,
                                      sCustomAreaTransitionBMP);
    }
    nResult = -1;
    return 0;
}
// PC 0x005C5E10
int CSWVirtualMachineCommands::ExecuteCommandSetAreaUnescapable(int nCommand,
                                                                int nArgs)
{
    int nUnescapable;
    CServerExoApp* pServerExoApp;
    CSWSModule* pModule;
    CSWSArea* pArea;

    if (g_pVirtualMachine->StackPopInteger(&nUnescapable) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pModule = (CSWSModule*)pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        pArea = pModule->GetArea();
        if (pArea != NULL)
        {
            pArea->m_bUnescapable = nUnescapable;
        }
    }
    return 0;
}
// PC 0x005C5E60
int CSWVirtualMachineCommands::ExecuteCommandSetCameraLocation(int nCommand,
                                                               int nArgs)
{

    float fFacing;
    CGameObject* pGameObject;
    float fPitch;
    float fDistance;
    int bTransition;
    int nCameraMode;

    if (g_pVirtualMachine->StackPopFloat(&fFacing) == 0)
    {
        return ERROR_STACK_POP;
    }
#ifdef _XBOX
    fFacing = fFacing * RAD_TO_DEG; nCameraMode = 1; fPitch = 0.0f; fDistance = 0.0f; bTransition = 0;
#else
    nCameraMode = 1; fPitch = 0.0f; fFacing = fFacing * RAD_TO_DEG; fDistance = 0.0f; bTransition = 0;
#endif
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject,
                                                        &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSCreature() != NULL &&
            pGameObject->AsSWSCreature()->m_bCommandable != 0)
        {

            pGameObject->AsSWSCreature()->AddAction(22,
                CSWSObject::ACTION_GROUP_NEW, CSWSObject::ACTION_PARAM_INTEGER,
                &nCameraMode, CSWSObject::ACTION_PARAM_FLOAT, &fFacing,
                CSWSObject::ACTION_PARAM_FLOAT, &fPitch,
                CSWSObject::ACTION_PARAM_FLOAT, &fDistance,
                CSWSObject::ACTION_PARAM_INTEGER, &bTransition, 0, NULL, 0,
                NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        }
    }
    return 0;
}
// PC 0x005C5F70
int CSWVirtualMachineCommands::ExecuteCommandSetCameraMode(int nCommand,
                                                           int nArgs)
{
    unsigned long oidPlayer;
    int nCameraMode;
    CServerExoApp* pServerExoApp;
    CSWSPlayer* pPlayer;
    CSWSMessage* pMessage;

    if (g_pVirtualMachine->StackPopObject(&oidPlayer) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nCameraMode) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pPlayer = pServerExoApp->GetClientObjectByObjectId(oidPlayer);
    if (pPlayer != NULL)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pMessage = (CSWSMessage*)pServerExoApp->GetSWSMessage();
        if (pMessage != NULL)
        {
            pMessage->SendServerToPlayerCamera_SetMode(pPlayer, nCameraMode);
        }
    }
    return 0;
}
// PC 0x005C5FF0
int CSWVirtualMachineCommands::ExecuteCommandSetCommandable(int nCommand,
                                                            int nArgs)
{
    unsigned long oidTarget;
    CGameObject* pGameObject;
    int bCommandable;
    CServerExoApp* pServerExoApp;

    oidTarget = m_oidObject;
    if (g_pVirtualMachine->StackPopInteger(&bCommandable) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2 &&
        g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
            ->GetGameObject(oidTarget, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        ((CSWSObject*)pGameObject)->m_bCommandable = bCommandable != 0;
    }
    return 0;
}
// PC 0x005C6130
int CSWVirtualMachineCommands::ExecuteCommandSetEncounterData(int nCommand,
                                                              int nArgs)
{
    unsigned long oidResult;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSEncounter* pEncounter;

    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    if (nArgs > 1)
    {
        if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
        {
            return -2001;
        }
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pEncounter = pServerExoApp->GetEncounterByGameObjectID(oidResult);
    if (pEncounter != NULL)
    {
        if (nCommand == 277)
        {
            pEncounter->SetActive(nResult);
            return 0;
        }
        else if (nCommand == 296)
        {
            pEncounter->SetDifficulty(nResult);
            return 0;
        }
        else if (nCommand == 279)
        {
            pEncounter->m_nRespawns = nResult;
            return 0;
        }
        else if (nCommand == 281)
        {
            pEncounter->m_nCurrentSpawns = nResult;
        }
    }
    return 0;
}
// PC 0x005C6200
int CSWVirtualMachineCommands::ExecuteCommandSetFacing(int nCommand, int nArgs)
{

    Vector vOrientation;
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    bool bSetOrientation = false;
    Vector vTarget;

    if (nCommand == COMMAND_SET_FACING)
    {
        float fDirection;
        if (g_pVirtualMachine->StackPopFloat(&fDirection) == 0)
        {
            return ERROR_STACK_POP;
        }
        fDirection *= (float)SWVM_DEGREES_TO_RADIANS;
        vOrientation = Vector((float)cos(fDirection),
                              (float)sin(fDirection), 0.0f);
        bSetOrientation = true;
    }
    else if (nCommand == COMMAND_SET_FACING_POINT)
    {
        if (g_pVirtualMachine->StackPopVector(&vTarget) == 0)
        {
            return ERROR_STACK_POP;
        }
    }

    CSWSObject* pObject;
    if (pObjectArray->GetGameObject(m_oidObject, (CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        if (nCommand == COMMAND_SET_FACING_POINT)
        {
            Vector vDirection = vTarget - pObject->m_vPosition;
            if (vDirection.m_fX != 0.0f || vDirection.m_fY != 0.0f ||
                vDirection.m_fZ != 0.0f)
            {
                vOrientation = normalize(vDirection);
                bSetOrientation = true;
            }
        }

        if (m_nCommandContext == COMMAND_CONTEXT_SERVER && bSetOrientation)
        {
            CSWCObject* pClientObject =
                g_pAppManager->m_pClientExoApp->GetGameObject(
                    g_pAppManager->m_pClientExoApp->ServerToClientObjectId(
                        pObject->m_nId));
            if (pClientObject != NULL && pClientObject->AsSWCCreature() != NULL)
            {
                pClientObject->AsSWCCreature()->SetLockOrientationToObject(
                    OBJECT_INVALID, 0);
            }
            pObject->SetLockOrientationToObject(OBJECT_INVALID);
            pObject->SetOrientation(vOrientation);
        }
    }
    return 0;
}
// PC 0x005C6420
int CSWVirtualMachineCommands::ExecuteCommandSetGlobalLocation(int nCommand,
                                                               int nArgs)
{
    CExoString sIdentifier;
    void* pStructure;

    if (g_pVirtualMachine->StackPopString(&sIdentifier) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                 &pStructure) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (pStructure != NULL)
    {
        CScriptLocation* pLocation = (CScriptLocation*)pStructure;
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoApp->GetGlobalVariableTable()
            ->SetValueLocation(sIdentifier, *pLocation);
        delete (CScriptLocation*)pStructure;
        pStructure = NULL;
    }
    return 0;
}
// PC 0x005C6530
int CSWVirtualMachineCommands::ExecuteCommandSetGlobalNumber(int nCommand,
                                                             int nArgs)
{
    CExoString sIdentifier;
    int nValue;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopString(&sIdentifier) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return ERROR_STACK_PUSH;
    }

    switch (nCommand)
    {
        case COMMAND_SET_GLOBAL_BOOLEAN:
        {
            int bValue = (nValue != 0);
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            pServerExoApp->GetGlobalVariableTable()->SetValueBoolean(sIdentifier,
                                                                   bValue);
            break;
        }
        case COMMAND_SET_GLOBAL_NUMBER:
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            pServerExoApp->GetGlobalVariableTable()->SetValueNumber(sIdentifier,
                                                                  nValue);
            break;
#ifdef _XBOX
        default:
            EXOXBOXASSERT(0);
            break;
#endif
    }
    return 0;
}
// PC 0x005C6630
int CSWVirtualMachineCommands::ExecuteCommandSetGlobalString(int nCommand,
                                                             int nArgs)
{
    CExoString sIdentifier;
    CExoString sValue;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopString(&sIdentifier) == 0)
    {
        return -2000;
    }
    if (g_pVirtualMachine->StackPopString(&sValue) == 0)
    {
        return -2000;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetGlobalVariableTable()->SetValueString(sIdentifier,
                                                            sValue);
    return 0;
}
// PC 0x005C6720
int CSWVirtualMachineCommands::ExecuteCommandSetIdentified(int nCommand,
                                                           int nArgs)
{
    unsigned long oidItem;
    int nIdentified;
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nIdentified) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        pItem->SetIdentified(nIdentified);
    }
    return 0;
}
// PC 0x005C6790
int CSWVirtualMachineCommands::ExecuteCommandSetIsDestroyable(int nCommand,
                                                              int nArgs)
{
    int bDestroyable;
    int bRaiseable = 1;
    int bSelectableWhenDead = 0;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;
    CSWSCreature* pCreature;
    CServerAIMaster* pAIMaster;

    if (g_pVirtualMachine->StackPopInteger(&bDestroyable) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&bRaiseable) == 0)
        {
            return ERROR_STACK_POP;
        }
        if (g_pVirtualMachine->StackPopInteger(&bSelectableWhenDead) == 0)
        {
            return ERROR_STACK_POP;
        }
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            m_oidObject, (CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
    {
        pObject->m_nEC = bDestroyable != 0;
        if (pObject->m_nEC == 1)
        {
            pCreature = pObject->AsSWSCreature();
            if (pCreature != NULL && pCreature->GetDead() &&
                pCreature->m_bInParty == 0)
            {
                pCreature->SetLockOrientationToObject(
                    CGameObjectArray::INVALIDOBJID, 0);
                if (pCreature->m_nAreaWaitState == 0)
                {
                    pAIMaster =
                        g_pAppManager->m_pServerExoApp->GetServerAIMaster();
                    pAIMaster->AddEventDeltaTime(
                        0, 3000, pCreature->m_n154, pCreature->m_nId,
                        CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
                }
            }
        }
        pObject->m_nF0 = bRaiseable;
        pObject->m_nF4 = bSelectableWhenDead;
    }
    return 0;
}
// PC 0x005C68E0
int CSWVirtualMachineCommands::ExecuteCommandSetLightsaberPowered(int nCommand,
                                                                  int nArgs)
{
    unsigned long oidCreature;
    int nOverride;
    int nPowered;
    int nShowTransition;
    CClientExoApp* pClientExoApp;
    unsigned long oidResult;
    CSWCCreature* pSWCCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nOverride) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nPowered) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nShowTransition) == 0)
    {
        return -2001;
    }
    pClientExoApp = (CClientExoApp*)g_pAppManager->m_pClientExoApp;
    oidResult = pClientExoApp->ServerToClientObjectId(oidCreature);
    pSWCCreature = pClientExoApp->GetCreatureByGameObjectID(oidResult);
    if (pSWCCreature != NULL)
    {
        pSWCCreature->SetOverrideLightsaberState(nOverride, nPowered,
                                                 nShowTransition);
    }
    return 0;
}
// PC 0x005C6980
int CSWVirtualMachineCommands::ExecuteCommandSetListening(int nCommand,
                                                          int nArgs)
{
    unsigned long oidObject;
    int bListening;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0 ||
        g_pVirtualMachine->StackPopInteger(&bListening) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(oidObject, (struct CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL)
    {
        pObject->m_bListening = bListening;
    }
    return 0;
}
// PC 0x005C6A00
int CSWVirtualMachineCommands::ExecuteCommandSetListenString(int nCommand,
                                                             int nArgs)
{
    unsigned long oidObject;
    CExoString sPattern;
    int nNumber;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0 ||
        g_pVirtualMachine->StackPopString(&sPattern) == 0 ||
        g_pVirtualMachine->StackPopInteger(&nNumber) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(oidObject, (struct CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL)
    {
        pObject->SetListenExpression(sPattern, nNumber);
    }
    return 0;
}
// PC 0x005C6B00
int CSWVirtualMachineCommands::ExecuteCommandSetLocalValue(int nCommand,
                                                          int nArgs)
{

    unsigned long oidObject;
    int nIndex;
    int nValue;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nIndex) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL)
    {
        CSWVarTable* pVarTable;
        int nObjectType = pGameObject->m_nObjectType;
        if (nObjectType == CGameObject::OBJECT_TYPE_AREA)
        {
            pVarTable = &pGameObject->AsSWSArea()->m_lVarTable;
        }
        else if (nObjectType == CGameObject::OBJECT_TYPE_MODULE)
        {
            pVarTable = &pGameObject->AsSWSModule()->m_lVarTable;
        }
        else
        {
            pVarTable = &((CSWSObject*)pGameObject)->m_lVarTable;
        }
        if (pVarTable != NULL)
        {
            switch (nCommand)
            {
            case COMMAND_SET_LOCAL_BOOLEAN:
                pVarTable->SetLocalBoolean(nIndex, nValue);
                break;
            case COMMAND_SET_LOCAL_NUMBER:
                if (nValue < SWVARTABLE_LOCAL_NUMBER_MIN ||
                    nValue > SWVARTABLE_LOCAL_NUMBER_MAX)
                {

                    EXOLOG("SetLocalNumber on %s: value %d out of range\n",
                           ((CSWSObject*)pGameObject)->m_sTag.CStr(), nValue);
                    EXOLOG("SetLocalNumber on %s failed\n",
                           ((CSWSObject*)pGameObject)->m_sTag.CStr());
                }
                else
                {
                    pVarTable->SetLocalNumber(nIndex, (unsigned char)nValue);
                }
                break;
            }
        }
    }
    return 0;
}
// PC 0x005C6C10
int CSWVirtualMachineCommands::ExecuteCommandSetLocked(int nCommand, int nArgs)
{
    int bLocked = 0;
    unsigned long oidTarget;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&bLocked) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            oidTarget, &pGameObject) == CGameObjectArray::SUCCESS)
    {
        if (pGameObject->AsSWSDoor() != NULL)
        {
            pGameObject->AsSWSDoor()->SetLocked(bLocked);
        }
        else if (pGameObject->AsSWSPlaceable() != NULL)
        {
            pGameObject->AsSWSPlaceable()->SetLocked(bLocked);
        }
    }
    return 0;
}
// PC 0x005C6CD0
int CSWVirtualMachineCommands::ExecuteCommandSetNPCSelectability(int nCommand,
                                                                 int nArgs)
{
    int nNPC;
    int nSelectability;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nSelectability) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetPartyTable()->SetNPCSelectability(nNPC, nSelectability);
    return 0;
}
// PC 0x005C6D30
int CSWVirtualMachineCommands::ExecuteCommandSetPlaceableIllumination(
    int nCommand, int nArgs)
{
    unsigned long oidPlaceable;
    int nIlluminate;
    CServerExoApp* pServerExoApp;
    CSWSPlaceable* pPlaceable;

    if (g_pVirtualMachine->StackPopObject(&oidPlaceable) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nIlluminate) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pPlaceable =
        (CSWSPlaceable*)pServerExoApp->GetPlaceableByGameObjectID(oidPlaceable);
    if (pPlaceable != NULL)
    {
        pPlaceable->SetLightIsOn(nIlluminate);
    }
    return 0;
}
// PC 0x005C6DA0
int CSWVirtualMachineCommands::ExecuteCommandSetPlotFlag(int nCommand,
                                                         int nArgs)
{
    unsigned long oidObject;
    int bPlot;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&bPlot) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        pObject = pGameObject->AsSWSObject();
        pObject->m_bPlotObject = bPlot;
    }
    return 0;
}
// PC 0x005C6E20
int CSWVirtualMachineCommands::ExecuteCommandSetRestrictMode(int nCommand,
                                                             int nArgs)
{
    int nRestrict;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nRestrict) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetModule() != 0)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (((CSWSModule*)pServerExoApp->GetModule())->GetArea() != NULL)
        {
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            (((CSWSModule*)pServerExoApp->GetModule())->GetArea())
                ->SetPlayerRestrictMode(nRestrict);
        }
    }
    return 0;
}
// PC 0x005C6E90
int CSWVirtualMachineCommands::ExecuteCommandSetReturnStrref(int nCommand,
                                                             int nArgs)
{
    int nResult;
    int nResult2;
    int nResult3;
    CServerExoApp* pServerExoApp;

    nResult = 0;
    if (g_pVirtualMachine->StackPopInteger(&nResult3) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nResult2) == 0)
    {
        return -2001;
    }
    if (nArgs > 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetPartyTable()->SetReturnStrref(nResult3, nResult2,
                                                    nResult);
    return 0;
}
// PC 0x005C6F20
int CSWVirtualMachineCommands::ExecuteCommandSetSpeedBlurEffect(int nCommand,
                                                                int nArgs)
{
    int bEnabled;

    if (g_pVirtualMachine->StackPopInteger(&bEnabled) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        float fRatio = 0.75f;
        if (g_pVirtualMachine->StackPopFloat(&fRatio) == 0)
        {
            return -2001;
        }
        AurSetSpeedBlurRatio(fRatio);
    }
    if (bEnabled != 0)
    {
        AurEnableSpeedBlur();
    }
    else
    {
        AurDisableSpeedBlur();
    }
    return 0;
}
// PC 0x005C6F90
int CSWVirtualMachineCommands::ExecuteCommandSetSubType(int nCommand, int nArgs)
{
    CGameEffect* pEffect;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pEffect) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    switch (nCommand)
    {
    case COMMAND_MAGICAL_EFFECT:
        pEffect->m_nSubType =
            (pEffect->m_nSubType & (unsigned short)~SUBTYPE_SUPERNATURAL) |
            SUBTYPE_MAGICAL;
        break;
    case COMMAND_SUPERNATURAL_EFFECT:
        pEffect->m_nSubType =
            (pEffect->m_nSubType & (unsigned short)~SUBTYPE_MAGICAL) |
            SUBTYPE_SUPERNATURAL;
        break;
    case COMMAND_EXTRAORDINARY_EFFECT:
        pEffect->m_nSubType |= CGameEffect::SUBTYPE_MASK;
        break;
    }
    if (pEffect->m_nType == CSWSEffectListHandler::EFFECT_LINK)
    {
        pEffect->UpdateLinked();
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    delete (void*)pEffect;
    return 0;
}
// PC 0x005C7040
int CSWVirtualMachineCommands::ExecuteCommandSetTime(int nCommand, int nArgs)
{
    int nHour;
    int nMinute;
    int nSecond;
    int nMillisecond;
    CServerExoApp* pServerExoApp;
    unsigned long oidResult;
    unsigned long oidResult2;

    if (g_pVirtualMachine->StackPopInteger(&nHour) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nMinute) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nSecond) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nMillisecond) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    ((CWorldTimer*)pServerExoApp->GetWorldTimer())
        ->AdvanceToTime(nHour, nMinute, nSecond, nMillisecond);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    ((CWorldTimer*)pServerExoApp->GetWorldTimer())
        ->GetWorldTime(&oidResult, &oidResult2);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    ((CSWSModule*)pServerExoApp->GetModule())
        ->UpdateTime(oidResult, oidResult2, 0);
    return 0;
}
// PC 0x005C7110
int CSWVirtualMachineCommands::ExecuteCommandSetForcePowerUnsuccessful(
    int nCommand, int nArgs)
{
    unsigned long oidCreature;
    unsigned long nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    nResult = 0;
    oidCreature = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&nResult) == 0 ||
        g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != 0)
    {
        pCreature->m_bLastForcePowerSuccessful = nResult;
    }
    return 0;
}
// PC 0x005C7180
int CSWVirtualMachineCommands::ExecuteCommandSetXP(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    int nXpAmount;
    CGameObject* pGameObject;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nXpAmount) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nXpAmount >= 0)
    {
        unsigned long nExperience = nXpAmount;
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)pServerExoApp->GetObjectArray()
                    ->GetGameObject(oidCreature, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject != NULL)
        {
            pCreature = pGameObject->AsSWSCreature();
            if (pCreature != NULL)
            {
                pCreature->m_pStats->SetExperience(nExperience, 1);
            }
        }
    }
    return 0;
}
// PC 0x005C7210
int CSWVirtualMachineCommands::ExecuteCommandShipBuild(int nCommand, int nArgs)
{
    bShipBuild = 1;
    return g_pVirtualMachine->StackPushInteger(1) != 0 ? 0 : -2000;
}
// PC 0x005C7240
int CSWVirtualMachineCommands::ExecuteCommandShowLevelUpGUI(int nCommand,
                                                            int nArgs)
{
    int nResult;

    nResult = g_pAppManager->m_pClientExoApp->GetInGameGui()->ShowLevelUpGUI(1);
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C7280
int CSWVirtualMachineCommands::ExecuteCommandShowPartySelection(int nCommand,
                                                                int nArgs)
{
    CExoString sExitScript;
    int nUnknown = 0;
    int nForceNPC1 = -1;
    int nForceNPC2 = -1;

    if (g_pVirtualMachine->StackPopString(&sExitScript) == 0)
    {
        return -2001;
    }
    if (nArgs >= 3)
    {
        if (g_pVirtualMachine->StackPopInteger(&nForceNPC1) == 0)
        {
            return -2001;
        }
        else
        {
            if (g_pVirtualMachine->StackPopInteger(&nForceNPC2) == 0)
            {
                return -2001;
            }
        }
    }
    if (nArgs >= 4)
    {
        if (g_pVirtualMachine->StackPopInteger(&nUnknown) == 0)
        {
            return -2001;
        }
    }
    g_pAppManager->GetClientExoApp()->GetInGameGui()->ShowPartySelection(sExitScript, 1, nForceNPC1,
                                                                       nForceNPC2, nUnknown);
    return 0;
}
// PC 0x005C73B0
int CSWVirtualMachineCommands::ExecuteCommandShowUpgradeScreen(int nCommand,
                                                               int nArgs)
{
    unsigned long oidItem;
    unsigned long oidCharacter = OBJECT_INVALID;
    int bDisableItemCreation = 0;
    int bDisableUpgrade = 0;
    CExoString sOverride2DA;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopObject(&oidCharacter) == 0)
        {
            return -2001;
        }
        if (nArgs > 2)
        {
            if (g_pVirtualMachine->StackPopInteger(&bDisableItemCreation) == 0)
            {
                return -2001;
            }
            if (g_pVirtualMachine->StackPopInteger(&bDisableUpgrade) == 0)
            {
                return -2001;
            }
        }
    }
    if (nArgs > 4)
    {
        if (g_pVirtualMachine->StackPopString(&sOverride2DA) == 0)
        {
            return -2001;
        }
    }
    g_pAppManager->GetClientExoApp()->GetInGameGui()->ShowUpgradeScreen(
        oidItem, oidCharacter, bDisableItemCreation, bDisableUpgrade,
        sOverride2DA);
    return 0;
}
// PC 0x005C74F0
int CSWVirtualMachineCommands::ExecuteCommandSignalEvent(int nCommand, int nArgs)
{
    unsigned long oidObject;
    void* pStructure;
    CGameObject* pCaller;
    CGameObject* pObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EVENT, &pStructure) == 0)
    {
        return ERROR_STACK_POP;
    }

    CScriptEvent* pEvent = (CScriptEvent*)pStructure;
    unsigned long oidCaller = OBJECT_INVALID;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()
            ->GetGameObject(m_oidObject, &pCaller) == CGameObjectArray::SUCCESS)
    {
        oidCaller = m_oidObject;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()
            ->GetGameObject(oidObject, &pObject) == CGameObjectArray::SUCCESS)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoApp->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, oidCaller, oidObject, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
    }
    return 0;
}
// PC 0x005C75C0
int CSWVirtualMachineCommands::ExecuteCommandSoundObjectGetFixedVariance(
    int nCommand, int nArgs)
{
    unsigned long oidSoundObject;
    float fFixedVariance;
    CServerExoApp* pServerExoApp;
    CSWSSoundObject* pSoundObject;

    if (g_pVirtualMachine->StackPopObject(&oidSoundObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSoundObject = pServerExoApp->GetSoundObjectByGameObjectID(oidSoundObject);
    if (pSoundObject != NULL)
    {
        fFixedVariance = pSoundObject->m_fFixedVariance;
    }
    return g_pVirtualMachine->StackPushFloat(fFixedVariance) != 0 ? 0 : -2000;
}
// PC 0x005C7630
int CSWVirtualMachineCommands::ExecuteCommandSoundObjectGetPitchVariation(
    int nCommand, int nArgs)
{
    unsigned long oidSoundObject;
    float fPitchVariation;
    CServerExoApp* pServerExoApp;
    CSWSSoundObject* pSoundObject;

    if (g_pVirtualMachine->StackPopObject(&oidSoundObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSoundObject = pServerExoApp->GetSoundObjectByGameObjectID(oidSoundObject);
    if (pSoundObject != NULL)
    {
        fPitchVariation = pSoundObject->m_fPitchVariation;
    }
    return g_pVirtualMachine->StackPushFloat(fPitchVariation) != 0 ? 0 : -2000;
}
// PC 0x005C76A0
int CSWVirtualMachineCommands::ExecuteCommandSoundObjectGetVolume(int nCommand,
                                                                  int nArgs)
{
    unsigned long oidSound;
    CServerExoApp* pServerExoApp;
    CSWSSoundObject* pSoundObject;
    int nResult;

    if (g_pVirtualMachine->StackPopObject(&oidSound) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSoundObject = pServerExoApp->GetSoundObjectByGameObjectID(oidSound);
    if (pSoundObject != NULL)
    {
        nResult = pSoundObject->m_nVolume;
    }
    else
    {
        nResult = oidSound;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C7700
int CSWVirtualMachineCommands::ExecuteCommandSoundObjectPlay(int nCommand,
                                                             int nArgs)
{
    unsigned long oidSound;
    CServerExoApp* pServerExoApp;
    CSWSSoundObject* pSoundObject;

    if (g_pVirtualMachine->StackPopObject(&oidSound) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSoundObject = pServerExoApp->GetSoundObjectByGameObjectID(oidSound);
    if (pSoundObject != NULL)
    {
        pSoundObject->Play();
    }
    return 0;
}
// PC 0x005C7750
int CSWVirtualMachineCommands::ExecuteCommandSoundObjectSetFixedVariance(
    int nCommand, int nArgs)
{
    unsigned long oidSound;
    float fFixedVariance;
    CServerExoApp* pServerExoApp;
    CSWSSoundObject* pSoundObject;

    if (g_pVirtualMachine->StackPopObject(&oidSound) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&fFixedVariance) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSoundObject = pServerExoApp->GetSoundObjectByGameObjectID(oidSound);
    if (pSoundObject != NULL)
    {
        pSoundObject->ChangeFixedVariance(fFixedVariance);
    }
    return 0;
}
// PC 0x005C77C0
int CSWVirtualMachineCommands::ExecuteCommandSoundObjectSetPitchVariation(
    int nCommand, int nArgs)
{
    unsigned long oidSound;
    float fVariance;
    CServerExoApp* pServerExoApp;
    CSWSSoundObject* pSoundObject;

    if (g_pVirtualMachine->StackPopObject(&oidSound) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&fVariance) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSoundObject = pServerExoApp->GetSoundObjectByGameObjectID(oidSound);
    if (pSoundObject != NULL)
    {
        pSoundObject->ChangePitchVariation(fVariance);
    }
    return 0;
}
// PC 0x005C7830
int CSWVirtualMachineCommands::ExecuteCommandSoundObjectSetPosition(int nCommand,
                                                                    int nArgs)
{
    unsigned long oidSound;
    Vector vPosition;
    CServerExoApp* pServerExoApp;
    CSWSSoundObject* pSoundObject;

    if (g_pVirtualMachine->StackPopObject(&oidSound) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopVector(&vPosition) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSoundObject = pServerExoApp->GetSoundObjectByGameObjectID(oidSound);
    if (pSoundObject != NULL)
    {
        pSoundObject->ChangePosition(vPosition);
    }
    return 0;
}
// PC 0x005C78C0
int CSWVirtualMachineCommands::ExecuteCommandSoundObjectSetVolume(int nCommand,
                                                                  int nArgs)
{
    unsigned long oidSound;
    int nVolume;
    CServerExoApp* pServerExoApp;
    CSWSSoundObject* pSoundObject;

    if (g_pVirtualMachine->StackPopObject(&oidSound) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nVolume) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSoundObject = pServerExoApp->GetSoundObjectByGameObjectID(oidSound);
    if (pSoundObject != NULL)
    {
        pSoundObject->ChangeVolume(nVolume);
    }
    return 0;
}
// PC 0x005C7930
int CSWVirtualMachineCommands::ExecuteCommandSoundObjectStop(int nCommand,
                                                             int nArgs)
{
    unsigned long oidSound;
    CServerExoApp* pServerExoApp;
    CSWSSoundObject* pSoundObject;

    if (g_pVirtualMachine->StackPopObject(&oidSound) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSoundObject = pServerExoApp->GetSoundObjectByGameObjectID(oidSound);
    if (pSoundObject != NULL)
    {
        pSoundObject->Stop();
    }
    return 0;
}
// PC 0x005C7980
int CSWVirtualMachineCommands::ExecuteCommandSoundObjectFadeAndStop(
    int nCommand, int nArgs)
{
    unsigned long oidSound;
    float fSeconds;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CSWSSoundObject* pSoundObject;

    if (g_pVirtualMachine->StackPopObject(&oidSound) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&fSeconds) == 0)
    {
        return -2001;
    }
    oidResult = (int)(fSeconds * 1000.0f);
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSoundObject = pServerExoApp->GetSoundObjectByGameObjectID(oidSound);
    if (pSoundObject != NULL)
    {
        pSoundObject->FadeAndStop(oidResult);
    }
    return 0;
}
// PC 0x005C79F0
int CSWVirtualMachineCommands::ExecuteCommandSpawnAvailableNPC(int nCommand,
                                                              int nArgs)
{
    int nNPC;
    CScriptLocation* pLocation;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocation) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (g_pVirtualMachine->StackPushObject(pServerExoApp->GetPartyTable()->SpawnNPC(
            nNPC, 1, pLocation->m_vPosition, pLocation->m_vOrientation, 1)) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    delete pLocation;
    return 0;
}
// PC 0x005C7A90
int CSWVirtualMachineCommands::ExecuteCommandSpeakOneLinerConversation(
    int nCommand, int nArgs)
{
    CExoString sDialogResRef("");
    unsigned long oidTokenTarget = OBJECT_INVALID;
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CGameObject* pGameObject;

    if (nArgs >= 1 && g_pVirtualMachine->StackPopString(&sDialogResRef) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    if (nArgs >= 2 && g_pVirtualMachine->StackPopObject(&oidTokenTarget) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER &&
        pObjectArray->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSObject() != NULL)
    {
        pGameObject->AsSWSObject()->RunDialogOneLiner(sDialogResRef,
                                                     oidTokenTarget);
    }
    return 0;
}
// PC 0x005C7BB0
int CSWVirtualMachineCommands::ExecuteCommandSpeakString(int nCommand, int nArgs)
{
    CExoString sString;
    int nTalkVolume;
    CSWSObject* pObject;
    int nTalkType;

    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nTalkVolume) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(
                m_oidObject, (CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS)
        {

            nTalkType = 1;
            if (nTalkVolume == 1)
            {
                nTalkType = 3;
            }
            else if (nTalkVolume == 2)
            {
                nTalkType = 2;
            }
            else if (nTalkVolume == 3)
            {
                nTalkType = 13;
            }
            else if (nTalkVolume == 4)
            {
                nTalkType = 14;
            }
            if (pObject != NULL)
            {
                if (nCommand == COMMAND_SPEAK_STRING)
                {
                    CExoString sTargetName("");
                    g_pAppManager->GetServerExoApp()->GetSWSMessage()
                        ->SendServerToPlayerChatMessage(nTalkType, m_oidObject,
                                                        sString, -1, sTargetName);
                }
                else if (pObject->m_bCommandable != 0)
                {
                    pObject->AddAction(CSWSObject::ACTION_SPEAK_STRING,
                        CSWSObject::ACTION_GROUP_NEW,
                        CSWSObject::ACTION_PARAM_STRING, &sString,
                        CSWSObject::ACTION_PARAM_INTEGER, &nTalkType, 0, NULL,
                        0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                        0, NULL, 0, NULL, 0, NULL, 0, NULL);
                }
            }
        }
    }
    return 0;
}
// PC 0x005C7D90
int CSWVirtualMachineCommands::ExecuteCommandSpeakStrRef(int nCommand, int nArgs)
{
    int nStrRef;
    int nTalkVolume;
    CGameObject* pGameObject;
    BOOL bValidObject;
    int nTalkType;

    if (g_pVirtualMachine->StackPopInteger(&nStrRef) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nTalkVolume) == 0)
    {
        return ERROR_STACK_POP;
    }
    bValidObject = FALSE;
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        bValidObject = pServerExoApp->GetObjectArray()->GetGameObject(
                           m_oidObject, &pGameObject) ==
                       CGameObjectArray::SUCCESS;
    }
    if (bValidObject == TRUE)
    {

        nTalkType = 8;
        if (nTalkVolume == 1)
        {
            nTalkType = 10;
        }
        else if (nTalkVolume == 2)
        {
            nTalkType = 9;
        }
        if (pGameObject->AsSWSObject() != NULL &&
            pGameObject->AsSWSObject()->m_bCommandable != 0)
        {

            pGameObject->AsSWSObject()->AddAction(33,
                CSWSObject::ACTION_GROUP_NEW,
                CSWSObject::ACTION_PARAM_INTEGER, &nStrRef,
                CSWSObject::ACTION_PARAM_INTEGER, &nTalkType, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL);
        }
    }
    return 0;
}
// PC 0x005C7EB0
int CSWVirtualMachineCommands::ExecuteCommandStartNewModule(int nCommand,
                                                          int nArgs)
{

    CExoString sModuleName;
    if (g_pVirtualMachine->StackPopString(&sModuleName) == 0)
    {
        return ERROR_STACK_POP;
    }
    CExoString sWayPoint;
    if (g_pVirtualMachine->StackPopString(&sWayPoint) == 0)
    {
        return ERROR_STACK_POP;
    }
    CExoString sMovie1;
    if (g_pVirtualMachine->StackPopString(&sMovie1) == 0)
    {
        return ERROR_STACK_POP;
    }
    CExoString sMovie2;
    if (g_pVirtualMachine->StackPopString(&sMovie2) == 0)
    {
        return ERROR_STACK_POP;
    }
    CExoString sMovie3;
    CExoString sMovie4;
    CExoString sMovie5;
    CExoString sMovie6;
    if (nArgs > 4)
    {
        if (g_pVirtualMachine->StackPopString(&sMovie3) == 0 ||
            g_pVirtualMachine->StackPopString(&sMovie4) == 0 ||
            g_pVirtualMachine->StackPopString(&sMovie5) == 0 ||
            g_pVirtualMachine->StackPopString(&sMovie6) == 0)
        {
            return ERROR_STACK_POP;
        }
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    pServerExoApp->SetMoveToModulePending(1);
    pServerExoApp->SetMoveToModuleString(sModuleName);
    pServerExoApp->SetMoveToModuleStartWaypoint(sWayPoint);
    pClientExoApp->AddMoveToModuleMovie(sMovie1);
    pClientExoApp->AddMoveToModuleMovie(sMovie2);
    pClientExoApp->AddMoveToModuleMovie(sMovie3);
    pClientExoApp->AddMoveToModuleMovie(sMovie4);
    pClientExoApp->AddMoveToModuleMovie(sMovie5);
    pClientExoApp->AddMoveToModuleMovie(sMovie6);

    CGuiInGame* pGuiInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();
    if (pGuiInGame != NULL &&
        (pGuiInGame->m_nGlobalDialogState != 0 ||
         pGuiInGame->m_bTransitionScriptRunning != 0))
    {
        Vector vColor;
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->GetInGameGui()->StartGlobalFade(0, 0.0f, 0.0f, vColor);
    }
    return 0;
}
// PC 0x005C81E0
int CSWVirtualMachineCommands::ExecuteCommandStopRumblePattern(int nCommand,
                                                               int nArgs)
{
    int nPattern;
    CClientExoApp* pClientExoApp;
    int nResult;

    if (g_pVirtualMachine->StackPopInteger(&nPattern) == 0)
    {
        return -2001;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    nResult = pClientExoApp->StopRumblePattern(nPattern);
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C8230
int CSWVirtualMachineCommands::ExecuteCommandStringConversions(int nCommand,
                                                               int nArgs)
{

    CExoString sNumber;

    if (g_pVirtualMachine->StackPopString(&sNumber) == 0)
    {
        return ERROR_STACK_POP;
    }

    if (nCommand == COMMAND_STRING_TO_INT)
    {
        if (g_pVirtualMachine->StackPushInteger(atoi(sNumber.CStr())) == 0)
        {
            return ERROR_STACK_PUSH;
        }
    }
    else
    {
        if (g_pVirtualMachine->StackPushFloat((float)atof(sNumber.CStr())) == 0)
        {
            return ERROR_STACK_PUSH;
        }
    }

    return 0;
}
// PC 0x005C8350
int CSWVirtualMachineCommands::ExecuteCommandSurrenderToEnemies(
    int nCommand, int nArgs)
{
    CGameObject* pGameObject;
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();

    if (m_nCommandContext == COMMAND_CONTEXT_SERVER &&
        pObjectArray->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        if (pCreature != NULL && pCreature->m_pStats->m_bIsPC == 0)
        {
            if (nCommand == COMMAND_ACTION_SURRENDER_TO_ENEMIES)
            {

                pCreature->AddAction(65,
                    CSWSObject::ACTION_GROUP_NEW, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            }
            else if (nCommand == COMMAND_SURRENDER_TO_ENEMIES)
            {
                pCreature->SurrenderToEnemies(0);
            }
            else if (nCommand == COMMAND_SURRENDER_RETAIN_BUFFS)
            {
                pCreature->SurrenderToEnemies(1);
            }
        }
    }
    return 0;
}
// PC 0x005C8430
int CSWVirtualMachineCommands::ExecuteCommandSwitchPlayerCreature(int nCommand,
                                                                  int nArgs)
{
    int nNPC;
    CServerExoApp* pServerExoApp;
    int nResult2;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nResult2 = pServerExoApp->GetPartyTable()->SwitchPlayerCharacter(nNPC);
    return g_pVirtualMachine->StackPushInteger(nResult2) != 0 ? 0 : -2000;
}
// PC 0x005C84E0
int CSWVirtualMachineCommands::ExecuteCommandTakeGoldFromCreature(int nCommand,
                                                               int nArgs)
{

    int nAmount;
    unsigned long oidCreature;
    int bDestroy;

    if (g_pVirtualMachine->StackPopInteger(&nAmount) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&bDestroy) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nAmount > 0)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
        if (pCreature != NULL)
        {
            if (nAmount > pCreature->GetGold())
            {
                nAmount = pCreature->GetGold();
            }
            if (nAmount > 0)
            {
                pServerExoApp = g_pAppManager->m_pServerExoApp;
                CSWSObject* pCaller = pServerExoApp->GetGameObject(m_oidObject)->AsSWSObject();
                if (pCaller != NULL)
                {
                    if (pCaller->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
                    {
                        pCreature->RemoveGold(nAmount, 1);
                        if (!bDestroy)
                        {
                            CSWSCreature* pRecipient =
                                pCaller->AsSWSCreature();
                            if (pRecipient != NULL)
                            {
                                pRecipient->AddGold(nAmount, 1);
                            }
                        }
                    }
                    else if (pCaller->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
                    {
                        pCreature->RemoveGold(nAmount, 1);
                        if (!bDestroy)
                        {
                            CSWSPlaceable* pPlaceable = pCaller->AsSWSPlaceable();
                            if (pPlaceable != NULL && pPlaceable->m_bHasInventory &&
                                pPlaceable->m_pcItemRepository != NULL)
                            {
                                CSWSItem* pGold = new CSWSItem(OBJECT_INVALID);
                                CResRef cTemplate("g_i_credits001");
                                pGold->LoadFromTemplate(cTemplate);
                                pGold->m_nStackSize = nAmount;
                                pPlaceable->AcquireItem(&pGold, OBJECT_INVALID, 1);
                            }
                        }
                    }
                    else if (pCaller->m_nObjectType == SW_OBJECT_TYPE_ITEM)
                    {
                        pCreature->RemoveGold(nAmount, 1);
                        if (!bDestroy)
                        {
                            CSWSItem* pContainer = pCaller->AsSWSItem();
                            if (pContainer != NULL && g_pRules->m_pBaseItems
                                    ->GetBaseItem(pContainer->GetBaseItemType())
                                    ->m_bIsContainer)
                            {
                                CItemRepository* pRepository = pContainer->m_pRepository;
                                if (pRepository != NULL)
                                {
                                    CSWSItem* pGold = new CSWSItem(OBJECT_INVALID);
                                    CResRef cTemplate("g_i_credits001");
                                    pGold->LoadFromTemplate(cTemplate);
                                    pGold->m_nStackSize = nAmount;
                                    pRepository->AddItem(&pGold, 1, 0, 1);
                                }
                            }
                        }
                    }
                    else
                    {
                        pCreature->RemoveGold(nAmount, 1);
                    }
                    if (pCreature->m_bInParty != 0)
                    {
                        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
                        pClientExoApp->GetInGameGui()->UpdateStatus(GUI_STATUS_CREDITS, -nAmount);
                    }
                }
            }
        }
    }
    return 0;
}
// PC 0x005C87E0
int CSWVirtualMachineCommands::ExecuteCommandTalent(int nCommand, int nArgs)
{
    CScriptTalent* pTalent = new CScriptTalent;
    int nID;
    if (g_pVirtualMachine->StackPopInteger(&nID) == 0)
    {
        delete pTalent;
        return ERROR_STACK_POP;
    }

    if (nCommand == COMMAND_TALENT_SPELL)
    {
        pTalent->m_nType = 0;
    }
    else if (nCommand == COMMAND_TALENT_FEAT)
    {
        pTalent->m_nType = 1;
    }
    else if (nCommand == COMMAND_TALENT_SKILL)
    {
        pTalent->m_nType = 2;
    }
    pTalent->m_nID = nID;
    if (g_pVirtualMachine->StackPushEngineStructure(STRUCTURE_TALENT, pTalent) == 0)
    {
        delete pTalent;
        return ERROR_STACK_PUSH;
    }
    delete pTalent;
    return 0;
}
// PC 0x005C8900
int CSWVirtualMachineCommands::ExecuteCommandTalentAccess(int nCommand,
                                                        int nArgs)
{
    CScriptTalent* pTalent;
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_TALENT,
                                                  (void**)&pTalent) == 0)
    {
        return ERROR_STACK_POP;
    }

    int nResult = 1;
    int nCategory = 0;
    switch (nCommand)
    {
    case COMMAND_GET_TYPE_FROM_TALENT:
        nResult = g_pVirtualMachine->StackPushInteger(pTalent->m_nType);
        break;
    case COMMAND_GET_ID_FROM_TALENT:
        nResult = g_pVirtualMachine->StackPushInteger(pTalent->m_nID);
        break;
    case COMMAND_GET_CATEGORY_FROM_TALENT:

        if (pTalent->m_nType == 0)
        {
            CSWSpell* pSpell;
            int nID = pTalent->m_nID;
            pSpell = g_pRules->m_pSpellArray->GetSpell(nID);
            if (pSpell != NULL)
            {
                nCategory = pSpell->m_nCategory;
            }
        }
        else if (pTalent->m_nType == 1)
        {
            CSWFeat* pFeat = g_pRules->GetFeat((unsigned short)pTalent->m_nID);
            if (pFeat != NULL)
            {
                nCategory = pFeat->m_nCategory;
            }
        }
        nResult = g_pVirtualMachine->StackPushInteger(nCategory);
        break;
    }
    if (pTalent != NULL)
    {
        delete pTalent;
    }
    if (nResult != 0)
        return 0;
    return ERROR_STACK_PUSH;
}
// PC 0x005C89E0
int CSWVirtualMachineCommands::ExecuteCommandTimeConversions(int nCommand,
                                                             int nArgs)
{
    float fSeconds = 0.0f;
    int nTime;

    if (g_pVirtualMachine->StackPopInteger(&nTime) == 0)
    {
        return ERROR_STACK_POP;
    }

    switch (nCommand)
    {
    case COMMAND_ROUNDS_TO_SECONDS:
        fSeconds = nTime * 3.0f;
        break;
    case COMMAND_HOURS_TO_SECONDS:
        fSeconds = (float)g_pAppManager->m_pServerExoApp->GetWorldTimer()
                       ->m_nMinutesPerHour * nTime * 60.0f;
        break;
    case COMMAND_TURNS_TO_SECONDS:
        fSeconds = nTime * 30.0f;
        break;
    }

    return g_pVirtualMachine->StackPushFloat(fSeconds) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C8A80
int CSWVirtualMachineCommands::ExecuteCommandTestString(int nCommand, int nArgs)
{

    CExoString sPattern;
    CExoString sStringToTest;

    if (g_pVirtualMachine->StackPopString(&sPattern) == 0 ||
        g_pVirtualMachine->StackPopString(&sStringToTest) == 0)
    {
        return ERROR_STACK_POP;
    }

    CSWSExpression expression;
    expression.ParseString(sPattern);
    if (g_pVirtualMachine->StackPushInteger(expression.TestString(sStringToTest, TRUE)) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005C8BE0
int CSWVirtualMachineCommands::ExecuteCommandUnequipItem(int nCommand,
                                                         int nArgs)
{
    int nResult;
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CGameObject* pGameObject2;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    if (m_nCommandContext == 1)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            if ((unsigned char)((CGameObjectArray*)
                                    pServerExoApp->GetObjectArray())
                    ->GetGameObject(oidResult, &pGameObject2) ==
                CGameObjectArray::SUCCESS)
            {
                ((CSWSCreature*)pGameObject->AsSWSCreature())
                    ->AddUnequipActions((CSWSItem*)pGameObject2->AsSWSItem(),
                                        CGameObjectArray::INVALIDOBJID, 0, 0,
                                        nResult);
            }
        }
    }
    return 0;
}
// PC 0x005C8CC0
int CSWVirtualMachineCommands::ExecuteCommandVector(int nCommand, int nArgs)
{
    Vector vResult;

    if (g_pVirtualMachine->StackPopFloat(&vResult.m_fX) == 0 ||
        g_pVirtualMachine->StackPopFloat(&vResult.m_fY) == 0 ||
        g_pVirtualMachine->StackPopFloat(&vResult.m_fZ) == 0)
    {
        return ERROR_STACK_POP;
    }
    return g_pVirtualMachine->StackPushVector(vResult) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C8D60
int CSWVirtualMachineCommands::ExecuteCommandVectorConversions(int nCommand,
                                                              int nArgs)
{

    if (nCommand == COMMAND_VECTOR_TO_ANGLE)
    {
        Vector vVector;
        float fAngle;

        if (g_pVirtualMachine->StackPopVector(&vVector) == 0)
        {
            return ERROR_STACK_POP;
        }
        vVector = normalize(vVector);
        fAngle = (float)acos(vVector.m_fX);
        if (vVector.m_fY < 0.0f)
        {
            fAngle = SWVM_FULL_CIRCLE_DEGREES - fAngle;
        }
        if (g_pVirtualMachine->StackPushFloat(fAngle) == 0)
        {
            return ERROR_STACK_PUSH;
        }
    }
    else if (nCommand == COMMAND_ANGLE_TO_VECTOR)
    {
        Vector vVector;
        float fAngle;

        if (g_pVirtualMachine->StackPopFloat(&fAngle) == 0)
        {
            return ERROR_STACK_POP;
        }
        vVector.m_fX = (float)cos(fAngle * (float)SWVM_DEGREES_TO_RADIANS);
        vVector.m_fY = (float)sin(fAngle * (float)SWVM_DEGREES_TO_RADIANS);
        vVector.m_fZ = 0.0f;
        if (g_pVirtualMachine->StackPushVector(vVector) == 0)
        {
            return ERROR_STACK_PUSH;
        }
    }
    return 0;
}
// PC 0x005C8EC0
int CSWVirtualMachineCommands::ExecuteCommandVectorMagnitude(int nCommand,
                                                             int nArgs)
{
    float fMagnitude;
    Vector vVector;

    if (g_pVirtualMachine->StackPopVector(&vVector) == 0)
    {
        return -2001;
    }
    fMagnitude = magnitude(vVector);
    return g_pVirtualMachine->StackPushFloat(fMagnitude) != 0 ? 0 : -2000;
}
// PC 0x005C8F30
int CSWVirtualMachineCommands::ExecuteCommandVectorNormalize(int nCommand,
                                                            int nArgs)
{
    Vector vVector;
    Vector vResult;

    if (g_pVirtualMachine->StackPopVector(&vVector) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (equal(vVector, Vector()) == 0)
    {
        vResult = normalize(vVector);
    }
    return g_pVirtualMachine->StackPushVector(vResult) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C9010
int CSWVirtualMachineCommands::ExecuteCommandVersusEffect(int nCommand, int nArgs)
{

    CGameEffect* pGameEffect;
    int nLawChaos;
    int nGoodEvil;

    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT, (void**)&pGameEffect) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    if (nArgs > 1 && g_pVirtualMachine->StackPopInteger(&nLawChaos) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    if (nArgs > 2 && g_pVirtualMachine->StackPopInteger(&nGoodEvil) == 0)
    {
        return ERROR_STACK_PUSH;
    }

    int bValid = 1;
    if (nCommand == COMMAND_VERSUS_ALIGNMENT_EFFECT)
    {
        if (nLawChaos <= -1 || nLawChaos >= SCRIPT_ALIGNMENT_COUNT)
        {
            bValid = 0;
        }
    }
    else if (nCommand == COMMAND_VERSUS_RACIAL_TYPE_EFFECT)
    {
        if (nLawChaos < 0 || nLawChaos > CSWRace::CRACE_ALL_RACES)
        {
            bValid = 0;
        }
    }

    if (pGameEffect != 0 && bValid != 0)
    {
        if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_ATTACK_INCREASE)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(3, nLawChaos);
                pGameEffect->SetInteger(4, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(2, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_ATTACK_DECREASE)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(3, nLawChaos);
                pGameEffect->SetInteger(4, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(2, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_DAMAGE_INCREASE)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(3, nLawChaos);
                pGameEffect->SetInteger(4, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(2, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_DAMAGE_DECREASE)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(3, nLawChaos);
                pGameEffect->SetInteger(4, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(2, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SANCTUARY)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(3, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(1, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_INVISIBILITY)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(3, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(1, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_CONCEALMENT)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(2, nLawChaos);
                pGameEffect->SetInteger(3, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(1, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE)
        {
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_DAMAGE_REDUCTION)
        {
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_INCREASE)
        {
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_DAMAGE_IMMUNITY_DECREASE)
        {
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_IMMUNITY)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(2, nLawChaos);
                pGameEffect->SetInteger(3, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(1, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_AC_INCREASE)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(3, nLawChaos);
                pGameEffect->SetInteger(4, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(2, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_AC_DECREASE)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(3, nLawChaos);
                pGameEffect->SetInteger(4, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(2, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_INCREASE)
        {
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_FORCE_RESISTANCE_DECREASE)
        {
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SAVING_THROW_INCREASE)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(4, nLawChaos);
                pGameEffect->SetInteger(5, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(3, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SAVING_THROW_DECREASE)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(4, nLawChaos);
                pGameEffect->SetInteger(5, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(3, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SKILL_INCREASE)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(3, nLawChaos);
                pGameEffect->SetInteger(4, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(2, nLawChaos);
                break;
            }
        }
        else if (pGameEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SKILL_DECREASE)
        {
            switch (nCommand)
            {
            case COMMAND_VERSUS_ALIGNMENT_EFFECT:
                pGameEffect->SetInteger(3, nLawChaos);
                pGameEffect->SetInteger(4, nGoodEvil);
                break;
            case COMMAND_VERSUS_RACIAL_TYPE_EFFECT:
                pGameEffect->SetInteger(2, nLawChaos);
                break;
            }
        }
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pGameEffect) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    delete pGameEffect;
    return 0;
}
// PC 0x005C93A0
int CSWVirtualMachineCommands::ExecuteCommandWait(int nCommand, int nArgs)
{
    float fSeconds;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopFloat(&fSeconds) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (m_nCommandContext == COMMAND_CONTEXT_SERVER)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(m_oidObject,
                                                        &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSObject() != NULL &&
            pGameObject->AsSWSObject()->m_bCommandable != 0)
        {

            pGameObject->AsSWSObject()->AddAction(30,
                CSWSObject::ACTION_GROUP_NEW, CSWSObject::ACTION_PARAM_FLOAT,
                &fSeconds, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL);
        }
    }
    return 0;
}
// PC 0x005C9470
int CSWVirtualMachineCommands::ExecuteCommandGetIsLinkImmune(int nCommand,
                                                          int nArgs)
{
    int bLinkImmune = 0;
    unsigned long oidTarget;
    CGameEffect* pEffect;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT,
                                                  (void**)&pEffect) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()
            ->GetGameObject(oidTarget, &pGameObject) == CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL)
    {
        CSWSCreature* pCreature = pGameObject->AsSWSCreature();
        CSWSCreatureStats* pStats = pCreature->m_pStats;
        bLinkImmune = pStats->GetEffectLinkImmunity(pEffect);
    }
    if (g_pVirtualMachine->StackPushInteger(bLinkImmune) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    delete pEffect;
    return 0;
}
// PC 0x005C9540
int CSWVirtualMachineCommands::ExecuteCommandGetPartyAIStyle(int nCommand,
                                                             int nArgs)
{
    CServerExoApp* pServerExoApp;
    CSWPartyTable* pSWPartyTable;
    int nResult;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSWPartyTable = pServerExoApp->GetPartyTable();
    nResult = pSWPartyTable->m_nAIState;
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C9570
int CSWVirtualMachineCommands::ExecuteCommandGetNPCAIStyle(int nCommand,
                                                           int nArgs)
{
    int nAIStyle = 0;
    unsigned long oidCreature;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            oidCreature, &pGameObject) == CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL)
    {
        nAIStyle = pGameObject->AsSWSCreature()->m_pStats->m_nAIState;
    }
    return g_pVirtualMachine->StackPushInteger(nAIStyle) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C9610
int CSWVirtualMachineCommands::ExecuteCommandSetPartyLeader(int nCommand,
                                                          int nArgs)
{

    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    int nResult = 0;
    CSWParty* pParty = pClientExoApp->GetSWParty();
    int nNPC;
    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (nNPC == NPC_PLAYER)
    {
        int nMember = 0;
        if (pParty->m_nCharacters > 0)
        for (; nMember < pParty->m_nCharacters; ++nMember)
        {
            EXOXBOXASSERT(nMember >= 0 && nMember < 3);
            unsigned long oidCreature = pParty->m_aPartyMemberData[nMember].m_oidCreature;
            CSWCCreature* pCreature =
                g_pAppManager->m_pClientExoApp->GetCreatureByGameObjectID(
                    oidCreature);
            if (pCreature != NULL && pCreature->GetServerCreature() != NULL)
            {
                CSWSCreature* pServerCreature = pCreature->GetServerCreature();
                if (pServerCreature->m_bInParty == 1 &&
                    pServerCreature->m_nAreaWaitState == 1 &&
                    (pServerCreature->GetDead() == 1 ||
                     pServerCreature->GetDeadTemp() == 1))
                {
                    CGameEffect* pEffect = new CGameEffect(1);
                    pEffect->m_nType = CSWSEffectListHandler::EFFECT_RESURRECTION;
                    pEffect->m_nSubType =
                        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                        CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                    pServerCreature->ApplyEffect(pEffect, 0, 0);
                    pServerCreature->m_nF0 = 1;
                }
            }
        }
        pParty->SetLeader(CSWParty::LEADER_PLAYER);
        nResult = 1;
    }
    else
    {
        unsigned long oidNPC = pClientExoApp->ServerToClientObjectId(
            pServerExoApp->GetPartyTable()->GetNPCObject(nNPC, 0, 1));
        for (int nMember = 0; nMember < pParty->m_nCharacters; ++nMember)
        {
            EXOXBOXASSERT(nMember >= 0 && nMember < 3);
            if (pParty->m_aPartyMemberData[nMember].m_oidCreature == oidNPC)
            {
                pParty->SetLeader(nMember);
                nResult = 1;
                break;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C97F0
int CSWVirtualMachineCommands::ExecuteCommandSetPartyAIStyle(int nCommand,
                                                             int nArgs)
{
    int nAIStyle = 0;
    int nStyle;
    CSWPartyTable* pPartyTable;

    if (g_pVirtualMachine->StackPopInteger(&nAIStyle) == 0)
    {
        return -2001;
    }
    nStyle = nAIStyle;
    pPartyTable = g_pAppManager->m_pServerExoApp->GetPartyTable();
    pPartyTable->m_nAIState = nStyle;
    return 0;
}
// PC 0x005C9840
int CSWVirtualMachineCommands::ExecuteCommandSetNPCAIStyle(int nCommand,
                                                         int nArgs)
{
    int nAIStyle = 0;
    unsigned long oidCreature;
    CGameObject* pGameObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAIStyle) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            oidCreature, &pGameObject) == CGameObjectArray::SUCCESS &&
        pGameObject->AsSWSCreature() != NULL)
    {
        pGameObject->AsSWSCreature()->m_pStats->m_nAIState = nAIStyle;
    }
    return 0;
}
// PC 0x005C98E0
int CSWVirtualMachineCommands::ExecuteCommandClearAllEffects(int nCommand,
                                                             int nArgs)
{
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;

    if (m_nCommandContext == 1)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if ((unsigned char)((CGameObjectArray*)pServerExoApp->GetObjectArray())
                ->GetGameObject(m_oidObject, (struct CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS)
        {
            pObject->ClearAllEffects(0);
        }
    }
    return 0;
}
// PC 0x005C9920
int CSWVirtualMachineCommands::ExecuteCommandGetStandardFaction(int nCommand,
                                                              int nArgs)
{
    unsigned long oidObject;
    int nFaction;
    CServerExoApp* pServerExoApp;
    CServerExoAppInternal* pServerExoAppInternal;

    nFaction = INVALID_STANDARD_FACTION;
    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoAppInternal = pServerExoApp->m_pcExoAppInternal;
    if (pServerExoAppInternal == NULL)
    {
        return g_pVirtualMachine->StackPushInteger(nFaction) != 0 ? 0 : ERROR_STACK_PUSH;
    }
    if (pServerExoAppInternal->m_pFactionManager == NULL)
    {
        return g_pVirtualMachine->StackPushInteger(nFaction) != 0 ? 0 : ERROR_STACK_PUSH;
    }
    pServerExoApp->GetFactionOfObject(oidObject, &nFaction);
    if (nFaction < STANDARD_FACTION_HOSTILE_1 &&
        nFaction > STANDARD_FACTION_INSANE)
    {
        nFaction = INVALID_STANDARD_FACTION;
    }
    return g_pVirtualMachine->StackPushInteger(nFaction) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C99C0
int CSWVirtualMachineCommands::ExecuteCommandGivePlotXP(int nCommand, int nArgs)
{
    CExoString sPlotName("");
    int nPercentage;

    if (g_pVirtualMachine->StackPopString(&sPlotName) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    if (g_pVirtualMachine->StackPopInteger(&nPercentage) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetPartyTable()->GivePlotXP(sPlotName, nPercentage);
    return 0;
}
// PC 0x005C9AB0
int CSWVirtualMachineCommands::ExecuteCommandGetMinOneHP(int nCommand,
                                                         int nArgs)
{
    unsigned long oidObject;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    int nResult = 0;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        nResult = pGameObject->AsSWSObject()->m_bMin1HP;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C9B30
int CSWVirtualMachineCommands::ExecuteCommandSetMinOneHP(int nCommand,
                                                         int nArgs)
{
    unsigned long oidObject;
    int bMinOneHP;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pSWSObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&bMinOneHP) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        pSWSObject = pGameObject->AsSWSObject();
        pSWSObject->m_bMin1HP = bMinOneHP != 0;
    }
    return 0;
}
// PC 0x005C9BB0
int CSWVirtualMachineCommands::ExecuteCommandSetFadeIn(int nCommand, int nArgs)
{

    float fDelay;
    float fLength;
    float fRed;
    float fGreen;
    float fBlue;

    if (g_pVirtualMachine->StackPopFloat(&fDelay) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fLength) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fRed) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fGreen) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fBlue) == 0)
    {
        return ERROR_STACK_POP;
    }
    Vector vColor(fRed, fGreen, fBlue);
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_n1024 = 0;
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetInGameGui()
        ->StartGlobalFade(1, fDelay, fLength, vColor);
    return 0;
}
// PC 0x005C9C80
int CSWVirtualMachineCommands::ExecuteCommandSetFadeOut(int nCommand, int nArgs)
{

    float fDelay;
    float fLength;
    float fRed;
    float fGreen;
    float fBlue;

    if (g_pVirtualMachine->StackPopFloat(&fDelay) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fLength) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fRed) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fGreen) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fBlue) == 0)
    {
        return ERROR_STACK_POP;
    }
    Vector vColor(fRed, fGreen, fBlue);
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetInGameGui()
        ->StartGlobalFade(0, fDelay, fLength, vColor);
    return 0;
}
// PC 0x005C9D40
int CSWVirtualMachineCommands::ExecuteCommandGetAttacker(int nCommand, int nArgs)
{
    unsigned long oidCreature = m_oidObject;
    unsigned long oidAttacker = CGameObjectArray::INVALIDOBJID;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (nArgs >= 1 && g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        if (nCommand == COMMAND_GET_FIRST_ATTACKER)
        {
            if (pCreature->GetFirstAttacker() == OBJECT_INVALID)
            {
                oidAttacker = CGameObjectArray::INVALIDOBJID;
            }
        }
        else if (nCommand == COMMAND_GET_NEXT_ATTACKER)
        {
            if (pCreature->GetNextAttacker() == OBJECT_INVALID)
            {
                oidAttacker = CGameObjectArray::INVALIDOBJID;
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidAttacker) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005C9DE0
int CSWVirtualMachineCommands::ExecuteCommandSetFormation(int nCommand,
                                                          int nArgs)
{
    unsigned long oidResult;
    unsigned long oidResult2;
    int nFormationPattern;
    int nPosition;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    oidResult2 = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidResult2) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nFormationPattern) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nPosition) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature =
        (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(oidResult2);
    if (pCreature != NULL)
    {
        pCreature->SetFormation(oidResult, nFormationPattern, nPosition);
    }
    return 0;
}
// PC 0x005C9E80
int CSWVirtualMachineCommands::ExecuteCommandGetIsDebilitated(int nCommand,
                                                              int nArgs)
{
    unsigned long oidResult;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    nResult = 0;
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature =
        (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (pCreature != NULL)
    {
        nResult = pCreature->IsDebilitated(FALSE);
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005C9EF0
int CSWVirtualMachineCommands::ExecuteCommandChangeByFaction(int nCommand,
                                                            int nArgs)
{

    int nOldFaction;
    int nNewFaction;
    unsigned long oidCreature;
    CGameObject* pGameObject;

    if (g_pVirtualMachine->StackPopInteger(&nOldFaction) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nNewFaction) == 0)
    {
        return ERROR_STACK_POP;
    }
    CFactionManager* pFactionManager =
        g_pAppManager->m_pServerExoApp->GetFactionManager();
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    if (m_nCommandContext != 0 &&
        pObjectArray->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS)
    {
        CSWSArea* pArea = NULL;
        if (pGameObject->AsSWSObject() != NULL)
        {
            pArea = pGameObject->AsSWSObject()->GetArea();
        }
        else if (pGameObject->AsSWSArea() != NULL)
        {
            pArea = pGameObject->AsSWSArea();
        }
        if (pArea != NULL && pFactionManager != NULL)
        {
            CSWSFaction* pFaction = pFactionManager->GetFaction(nNewFaction);
            if (pArea->GetFirstObjectInArea(oidCreature) == 1)
            {
                do
                {
                    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
                    CSWSCreature* pCreature =
                        pServerExoApp->GetCreatureByGameObjectID(oidCreature);
                    if (pCreature != NULL)
                    {

                        EXOLOG("ChangeFaction: faction %d, creature %s\n",
                               pCreature->GetStats()->m_nFactionId, pCreature->m_sTag.CStr());
                        if (pCreature->GetStats()->m_nFactionId == nOldFaction)
                        {
                            pFaction->AddMember(oidCreature, FALSE);
                            if (nCommand == COMMAND_CHANGE_BY_FACTION_AND_PACIFY)
                            {
                                pCreature->SetCombatState(FALSE, TRUE);
                                pCreature->ClearAllActions(TRUE);
                                pCreature->PacifyCreature();
                            }
                            else
                            {
                                pCreature->ClearVisibleList();
                                pCreature->UpdateVisibleList(0);
                            }
                        }
                    }
                } while (pArea->GetNextObjectInArea(oidCreature) == 1);
            }
        }
    }
    return 0;
}
// PC 0x005CA080
int CSWVirtualMachineCommands::ExecuteCommandGetSubScreenID(int nCommand,
                                                            int nArgs)
{
    CClientExoApp* pClientExoApp;
    CGuiInGame* pGuiInGame;
    int nResult;

    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pGuiInGame = pClientExoApp->GetInGameGui();
    nResult = 0;
    if (pGuiInGame != NULL && pGuiInGame->m_pPartySelection != 0)
    {
        nResult = (int)pGuiInGame->m_nLastGuiPanel + 1;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CA0C0
int CSWVirtualMachineCommands::ExecuteCommandCancelCombat(int nCommand,
                                                          int nArgs)
{
    unsigned long oidResult;
    int nResult;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidResult) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature =
        (CSWSCreature*)pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (pCreature != NULL)
    {
        pCreature->CancelCombat(nResult);
    }
    return 0;
}
// PC 0x005CA140
int CSWVirtualMachineCommands::ExecuteCommandPauseGame(int nCommand, int nArgs)
{
    int nPause;
    CClientExoApp* pClientExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nPause) == 0)
    {
        return -2001;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->SetPausedByCombat(nPause, 0, 0);
    return 0;
}
// PC 0x005CA180
int CSWVirtualMachineCommands::ExecuteCommandGetItemStackSize(int nCommand,
                                                              int nArgs)
{
    int nResult;
    unsigned long oidItem;
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;

    nResult = 0;
    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        nResult = pItem->m_nStackSize;
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005D33D0
int CSWVirtualMachineCommands::ExecuteCommandSetItemStackSize(int nCommand,
                                                              int nArgs)
{
    unsigned long oidItem;
    int nStackSize = 0;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nStackSize) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->GetServerExoApp();
    CSWSItem* pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
        if (nStackSize > pBaseItem->GetMaxStackSize())
        {
            nStackSize = pBaseItem->GetMaxStackSize();
        }
        if (nStackSize < 1)
        {
            nStackSize = 1;
        }

        int nOldStackSize = pItem->m_nStackSize;
        if (nStackSize != nOldStackSize)
        {
            pItem->m_nStackSize = nStackSize;
            unsigned long oidPossessor = pItem->m_oidPossessor;
            CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidPossessor);
            CSWCCMessageData* pMessageData = new CSWCCMessageData;
            pMessageData->SetObjectID(0, pItem->GetId());
            pMessageData->SetInteger(0, 1);
            if (nStackSize > nOldStackSize)
            {
                if (pCreature != NULL && pCreature->m_bInParty != 0)
                {
                    pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_ITEM_RECEIVED, pMessageData);
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->AddItemReceived(pItem->m_sLocalizedName);
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_ITEM_RECEIVED, 0);
                }
            }
            else
            {
                if (pCreature != NULL && pCreature->m_bInParty != 0)
                {
                    pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_ITEM_LOST, pMessageData);
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_ITEM_LOST, 0);
                }
            }
        }
    }
    return 0;
}
// PC 0x005CA1E0
int CSWVirtualMachineCommands::ExecuteCommandShowGalaxyMapGui(int nCommand,
                                                              int nArgs)
{
    int nPlanet;
    CClientExoApp* pClientExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nPlanet) == 0)
    {
        return -2001;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetInGameGui()->ShowGalaxyMapGui(nPlanet);
    return 0;
}
// PC 0x005CA220
int CSWVirtualMachineCommands::ExecuteCommandSetGalaxyMapPlanetAvailable(
    int nCommand, int nArgs)
{
    int nPlanet;
    int nAvailable;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nPlanet) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAvailable) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetPartyTable()->SetPlanetAvailable(nPlanet, nAvailable);
    return 0;
}
// PC 0x005CA280
int CSWVirtualMachineCommands::ExecuteCommandGetGalaxyMapPlanetAvailable(
    int nCommand, int nArgs)
{
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    int nResult;

    if (g_pVirtualMachine->StackPopInteger((int*)&oidResult) == 0)
    {
        {
            return -2001;
        }
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nResult = pServerExoApp->GetPartyTable()->GetPlanetAvailable(oidResult);
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CA2E0
int CSWVirtualMachineCommands::ExecuteCommandSetGalaxyMapPlanetSelectable(
    int nCommand, int nArgs)
{
    int nPlanet;
    int nSelectable;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nPlanet) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nSelectable) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetPartyTable()->SetPlanetSelectable(nPlanet, nSelectable);
    return 0;
}
// PC 0x005CA340
int CSWVirtualMachineCommands::ExecuteCommandGetGalaxyMapPlanetSelectable(
    int nCommand, int nArgs)
{
    unsigned long oidResult;
    CServerExoApp* pServerExoApp;
    int nResult;

    if (g_pVirtualMachine->StackPopInteger((int*)&oidResult) == 0)
    {
        {
            return -2001;
        }
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nResult = pServerExoApp->GetPartyTable()->GetPlanetSelectable(oidResult);
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005BB500
int CSWVirtualMachineCommands::ExecuteCommandGetGalaxyMapSelectedPlanet(
    int nCommand, int nArgs)
{
    CServerExoApp* pServerExoApp;
    CSWPartyTable* pSWPartyTable;
    int nResult;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSWPartyTable = pServerExoApp->GetPartyTable();
    nResult = pSWPartyTable->m_nSelectedPlanet;
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CA3D0
int CSWVirtualMachineCommands::ExecuteCommandSetItemNonEquippable(int nCommand,
                                                                  int nArgs)
{
    unsigned long oidItem;
    int bNonEquippable;
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;

    if (g_pVirtualMachine->StackPopObject(&oidItem) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&bNonEquippable) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL)
    {
        pItem->m_bNonEquippable = bNonEquippable;
    }
    return 0;
}
// PC 0x005CA440
int CSWVirtualMachineCommands::ExecuteCommandGetButtonMashCheck(int nCommand,
                                                                int nArgs)
{
    CServerExoApp* pServerExoApp;
    int nResult;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    nResult = pServerExoApp->GetButtonMashCheck();
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CA470
int CSWVirtualMachineCommands::ExecuteCommandSetButtonMashCheck(int nCommand,
                                                                int nArgs)
{
    int nResult;
    CServerExoApp* pServerExoApp;

    nResult = 0;
    if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->SetButtonMashCheck(nResult);
    return 0;
}
// PC 0x005CA4B0
int CSWVirtualMachineCommands::ExecuteCommandSetDialogPlaceableCamera(
    int nCommand, int nArgs)
{
    int nResult;
    int nResult2;
    CClientExoApp* pClientExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nResult2) == 0)
    {
        return -2001;
    }
    if (nArgs >= 2)
    {
        if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
        {
            return -2001;
        }
    }
    nResult = -1;
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetInGameGui()->SetShotFromScript(nResult2, -1);
    return 0;
}
// PC 0x005CA520
int CSWVirtualMachineCommands::ExecuteCommandGetSoloMode(int nCommand,
                                                         int nArgs)
{
    CServerExoApp* pServerExoApp;
    CSWPartyTable* pSWPartyTable;
    int nResult;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pSWPartyTable = pServerExoApp->GetPartyTable();
    nResult = pSWPartyTable->m_bSoloMode;
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CA550
int CSWVirtualMachineCommands::ExecuteCommandGetMaxStealthXP(int nCommand,
                                                             int nArgs)
{
    CServerExoApp* pServerExoApp;
    unsigned long oidResult;
    CSWSCreature* pCreature;
    int nResult;
    CSWSArea* pArea;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidResult = pServerExoApp->GetPlayerCreatureId();
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    nResult = 0;
    if (pCreature != NULL)
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            nResult = pArea->m_nStealthXPMax;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CA5A0
int CSWVirtualMachineCommands::ExecuteCommandSetMaxStealthXP(int nCommand,
                                                             int nArgs)
{
    CServerExoApp* pServerExoApp;
    unsigned long oidResult;
    CSWSCreature* pCreature;
    unsigned long oidResult2;
    CSWSArea* pArea;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidResult = pServerExoApp->GetPlayerCreatureId();
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (g_pVirtualMachine->StackPopInteger((int*)&oidResult2) == 0)
    {
        return -2001;
    }
    if (pCreature != NULL)
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            pArea->SetMaxStealthXP(oidResult2);
        }
    }
    return 0;
}
// PC 0x005CA600
int CSWVirtualMachineCommands::ExecuteCommandGetCurrentStealthXP(int nCommand,
                                                                 int nArgs)
{
    CServerExoApp* pServerExoApp;
    unsigned long oidResult;
    CSWSCreature* pCreature;
    int nResult;
    CSWSArea* pArea;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidResult = pServerExoApp->GetPlayerCreatureId();
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    nResult = 0;
    if (pCreature != NULL)
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            nResult = pArea->m_nStealthXPCurrent;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CA650
int CSWVirtualMachineCommands::ExecuteCommandSetCurrentStealthXP(int nCommand,
                                                                 int nArgs)
{
    CServerExoApp* pServerExoApp;
    unsigned long oidResult;
    CSWSCreature* pCreature;
    unsigned long oidResult2;
    CSWSArea* pArea;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidResult = pServerExoApp->GetPlayerCreatureId();
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (g_pVirtualMachine->StackPopInteger((int*)&oidResult2) == 0)
    {
        return -2001;
    }
    if (pCreature != NULL)
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            pArea->SetCurrentStealthXP(oidResult2);
        }
    }
    return 0;
}
// PC 0x005CA6B0
int CSWVirtualMachineCommands::ExecuteCommandAwardStealthXP(int nCommand,
                                                            int nArgs)
{
    unsigned long oidTarget;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;
    CSWSArea* pArea;

    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidTarget);
    if (pCreature != NULL)
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            pArea->AwardStealthXP(pCreature->m_nId);
        }
    }
    return 0;
}
// PC 0x005CA710
int CSWVirtualMachineCommands::ExecuteCommandGetStealthXPEnabled(int nCommand,
                                                                 int nArgs)
{
    CServerExoApp* pServerExoApp;
    unsigned long oidResult;
    CSWSCreature* pCreature;
    int nResult;
    CSWSArea* pArea;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidResult = pServerExoApp->GetPlayerCreatureId();
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    nResult = 0;
    if (pCreature != NULL)
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            nResult = pArea->m_bStealthXPEnabled;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CA760
int CSWVirtualMachineCommands::ExecuteCommandSetStealthXPEnabled(int nCommand,
                                                                 int nArgs)
{
    CServerExoApp* pServerExoApp;
    unsigned long oidResult;
    CSWSCreature* pCreature;
    int nEnabled;
    CSWSArea* pArea;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidResult = pServerExoApp->GetPlayerCreatureId();
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (g_pVirtualMachine->StackPopInteger(&nEnabled) == 0)
    {
        return -2001;
    }
    if (pCreature != NULL)
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            pArea->m_bStealthXPEnabled = nEnabled;
        }
    }
    return 0;
}
// PC 0x005CA7C0
int CSWVirtualMachineCommands::ExecuteCommandGetSubRace(int nCommand, int nArgs)
{
    unsigned long oidCreature;
    unsigned long nSubRace;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL && pCreature->m_pStats != NULL)
    {
        nSubRace = pCreature->m_pStats->m_nSubraceIndex;
    }
    else
    {
        nSubRace = oidCreature;
    }
    return g_pVirtualMachine->StackPushInteger(nSubRace) != 0 ? 0 : -2000;
}
// PC 0x005CA830
int CSWVirtualMachineCommands::ExecuteCommandGetBlockingCreature(int nCommand,
                                                                 int nArgs)
{
    unsigned long oidResult;
    unsigned long oidTarget;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidTarget);
    if (pCreature != 0)
    {
        oidResult = pCreature->m_pPathfindInformation->m_oidBlockingCreature;
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005CA8A0
int CSWVirtualMachineCommands::ExecuteCommandGetFoundEnemyCreature(int nCommand,
                                                                   int nArgs)
{
    unsigned long oidResult;
    unsigned long oidTarget;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    oidResult = OBJECT_INVALID;
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidTarget);
    if (pCreature != 0)
    {
        oidResult = pCreature->m_pPathfindInformation->m_nField_264;
    }
    return g_pVirtualMachine->StackPushObject(oidResult) != 0 ? 0 : -2000;
}
// PC 0x005CA910
int CSWVirtualMachineCommands::ExecuteCommandGetStealthXPDecrement(int nCommand,
                                                                   int nArgs)
{
    CServerExoApp* pServerExoApp;
    unsigned long oidResult;
    CSWSCreature* pCreature;
    int nResult;
    CSWSArea* pArea;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidResult = pServerExoApp->GetPlayerCreatureId();
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    nResult = 0;
    if (pCreature != NULL)
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            nResult = pArea->m_nStealthXPLoss;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CA960
int CSWVirtualMachineCommands::ExecuteCommandSetStealthXPDecrement(int nCommand,
                                                                   int nArgs)
{
    CServerExoApp* pServerExoApp;
    unsigned long oidResult;
    CSWSCreature* pCreature;
    int nDecrement;
    CSWSArea* pArea;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    oidResult = pServerExoApp->GetPlayerCreatureId();
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidResult);
    if (g_pVirtualMachine->StackPopInteger(&nDecrement) == 0)
    {
        return -2001;
    }
    if (pCreature != NULL)
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            pArea->m_nStealthXPLoss = nDecrement;
        }
    }
    return 0;
}
// PC 0x005CA9C0
int CSWVirtualMachineCommands::ExecuteCommandDuplicateHeadAppearance(int nCommand,
                                                                     int nArgs)
{

    unsigned long oidCreatureToChange;
    unsigned long oidCreatureToMatch;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreatureToChange;
    CSWSCreature* pCreatureToMatch;
    int nHead;

    if (g_pVirtualMachine->StackPopObject(&oidCreatureToChange) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidCreatureToMatch) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->GetServerExoApp();
    pCreatureToChange = pServerExoApp->GetCreatureByGameObjectID(oidCreatureToChange);
    pCreatureToMatch = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreatureToMatch);
    if (pCreatureToChange != NULL && pCreatureToMatch != NULL)
    {
        if (pCreatureToMatch->m_cAppearanceInfo.m_bUseBackupHead != 0)
        {
            CExoString sColumn("BACKUPHEAD");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pAppearance = p2DArrays->m_pAppearance;
            pAppearance->GetINTEntry(
                pCreatureToMatch->m_cAppearanceInfo.m_nAppearance, sColumn, &nHead);
        }
        else
        {
            CExoString sColumn("NORMALHEAD");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pAppearance = p2DArrays->m_pAppearance;
            pAppearance->GetINTEntry(
                pCreatureToMatch->m_cAppearanceInfo.m_nAppearance, sColumn, &nHead);
        }
        pCreatureToChange->m_cAppearanceInfo.m_nAppearanceHead = (unsigned char)nHead;
    }
    return 0;
}
// PC 0x005CAB10
int CSWVirtualMachineCommands::ExecuteCommandSetLockOrientationInDialog(
    int nCommand, int nArgs)
{
    unsigned long oidObject;
    int nValue;
    CClientExoApp* pClientExoApp;
    int nResult;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }
    if (oidObject != OBJECT_INVALID)
    {
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        nResult = pClientExoApp->ServerToClientObjectId(oidObject);
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->GetInGameGui()->SetLockObjectOrientationInDialog(nResult,
                                                                        nValue);
    }
    return 0;
}
// PC 0x005CAB90
int CSWVirtualMachineCommands::ExecuteCommandSetLockHeadFollowInDialog(
    int nCommand, int nArgs)
{
    unsigned long oidObject;
    int nValue;
    CClientExoApp* pClientExoApp;
    int nResult;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }
    if (oidObject != OBJECT_INVALID)
    {
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        nResult = pClientExoApp->ServerToClientObjectId(oidObject);
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->GetInGameGui()->SetLockHeadFollowInDialog(nResult,
                                                                 nValue);
    }
    return 0;
}
// PC 0x005CAC10
int CSWVirtualMachineCommands::ExecuteCommandEnableRendering(int nCommand, int nArgs)
{
    unsigned long oidObject;
    int bEnable;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&bEnable) == 0)
    {
        return -2001;
    }
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    if (pGameObject != NULL)
    {
        CSWSObject* pObject = pGameObject->AsSWSObject();
        if (pObject != NULL)
        {
            if (bEnable)
            {
                if (pObject->m_n254 != 0)
                {
                    pObject->m_n254 = 0;
                    CSWCObject* pClientObject = pObject->GetClientObject();
                    if (pClientObject != NULL && pClientObject->GetAnimationBase() != NULL)
                    {
                        Gob* pModel = pClientObject->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
                        if (pModel != NULL)
                        {
                            pModel->EnableRendering(true);
                        }
                        pModel = pClientObject->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD);
                        if (pModel != NULL)
                        {
                            pModel->EnableRendering(true);
                        }
                        CSWSCreature* pCreature = pObject->AsSWSCreature();
                        if (pCreature != NULL)
                        {
                            CSWSItem* pItem = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
                            if (pItem != NULL)
                            {
                                CSWCItem* pClientItem = pItem->GetClientItem();
                                if (pClientItem != NULL && pClientItem->GetAnimationBase() != NULL)
                                {
                                    pModel = pClientItem->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
                                    if (pModel != NULL)
                                    {
                                        pModel->EnableRendering(true);
                                    }
                                }
                            }
                            pItem = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
                            if (pItem != NULL)
                            {
                                CSWCItem* pClientItem = pItem->GetClientItem();
                                if (pClientItem != NULL && pClientItem->GetAnimationBase() != NULL)
                                {
                                    pModel = pClientItem->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
                                    if (pModel != NULL)
                                    {
                                        pModel->EnableRendering(true);
                                    }
                                }
                            }
                            pItem = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_HEAD);
                            if (pItem != NULL)
                            {
                                CSWCItem* pClientItem = pItem->GetClientItem();
                                if (pClientItem != NULL && pClientItem->GetAnimationBase() != NULL)
                                {
                                    pModel = pClientItem->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
                                    if (pModel != NULL)
                                    {
                                        pModel->EnableRendering(true);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (pObject->m_n254 == 0)
                {
                    pObject->m_n254 = 1;
                    CSWCObject* pClientObject = pObject->GetClientObject();
                    if (pClientObject != NULL && pClientObject->GetAnimationBase() != NULL)
                    {
                        Gob* pModel = pClientObject->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
                        if (pModel != NULL)
                        {
                            pModel->EnableRendering(false);
                        }
                        pModel = pClientObject->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD);
                        if (pModel != NULL)
                        {
                            pModel->EnableRendering(false);
                        }
                        CSWSCreature* pCreature = pObject->AsSWSCreature();
                        if (pCreature != NULL)
                        {
                            CSWSItem* pItem = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
                            if (pItem != NULL)
                            {
                                CSWCItem* pClientItem = pItem->GetClientItem();
                                if (pClientItem != NULL && pClientItem->GetAnimationBase() != NULL)
                                {
                                    pModel = pClientItem->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
                                    if (pModel != NULL)
                                    {
                                        pModel->EnableRendering(false);
                                    }
                                }
                            }
                            pItem = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
                            if (pItem != NULL)
                            {
                                CSWCItem* pClientItem = pItem->GetClientItem();
                                if (pClientItem != NULL && pClientItem->GetAnimationBase() != NULL)
                                {
                                    pModel = pClientItem->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
                                    if (pModel != NULL)
                                    {
                                        pModel->EnableRendering(false);
                                    }
                                }
                            }
                            pItem = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_HEAD);
                            if (pItem != NULL)
                            {
                                CSWCItem* pClientItem = pItem->GetClientItem();
                                if (pClientItem != NULL && pClientItem->GetAnimationBase() != NULL)
                                {
                                    pModel = pClientItem->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
                                    if (pModel != NULL)
                                    {
                                        pModel->EnableRendering(false);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
// PC 0x005CAF90
int CSWVirtualMachineCommands::ExecuteCommandEnableVideoEffect(int nCommand,
                                                               int nArgs)
{
    int nEffect;

    if (g_pVirtualMachine->StackPopInteger(&nEffect) == 0)
    {
        return -2001;
    }

    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_n1028 = 1;
    g_pAppManager->m_pClientExoApp->EnableVideoEffect(nEffect);
    return 0;
}
// PC 0x005CAFE0
int CSWVirtualMachineCommands::ExecuteCommandDisableVideoEffect(int nCommand,
                                                                int nArgs)
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_n1028 = 0;
    g_pAppManager->m_pClientExoApp->DisableVideoEffect();
    return 0;
}
// PC 0x005CB010
int CSWVirtualMachineCommands::ExecuteCommandRevealMap(int nCommand, int nArgs)
{
    Vector vPoint;
    int nRadius;
    CSWSModule* pModule;

    if (g_pVirtualMachine->StackPopVector(&vPoint) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nRadius) == 0)
    {
        return ERROR_STACK_POP;
    }

    pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != 0 && pModule->m_pAreaMap != 0)
    {
        if (nRadius != -1)
        {
            pModule->m_pAreaMap->SetWorldPointExplored(vPoint, nRadius);
        }
        else
        {
            pModule->m_pAreaMap->SetEntireMapExplored();
        }
    }
    return 0;
}
// PC 0x005CB0C0
int CSWVirtualMachineCommands::ExecuteCommandSetTutorialWindowsEnabled(
    int nCommand, int nArgs)
{
    int nEnabled;
    int nValue;
    CClientOptions* pOptions;

    if (g_pVirtualMachine->StackPopInteger(&nEnabled) == 0)
    {
        return -2001;
    }
    nValue = nEnabled;
    pOptions = g_pAppManager->m_pClientExoApp->GetClientOptions();
    pOptions->m_bTutorialPopups = nValue;
    return 0;
}
// PC 0x005CB110
int CSWVirtualMachineCommands::ExecuteCommandShowTutorialWindow(int nCommand,
                                                                int nArgs)
{
    int nTutorial;

    if (g_pVirtualMachine->StackPopInteger(&nTutorial) == 0)
    {
        return -2001;
    }

    g_pAppManager->m_pClientExoApp->ShowTutorialWindow(nTutorial, OBJECT_INVALID,
                                                       OBJECT_INVALID, 0);
    return 0;
}
#ifndef _XBOX
int CSWVirtualMachineCommands::ExecuteCommandIsCreditSequenceInProgress(  // PC 0x005CB160
    int nCommand, int nArgs)
{
    int bInProgress;

    bInProgress = g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bCreditSequenceInProgress;
    return g_pVirtualMachine->StackPushInteger(bInProgress) != 0 ? 0 : -2000;
}
// PC 0x005CB190
int CSWVirtualMachineCommands::ExecuteCommandStartCreditSequence(
    int nCommand, int nArgs)
{
    int bTransparentBackground;

    CExoString sMusic;

    if (g_pVirtualMachine->StackPopInteger(&bTransparentBackground) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopString(&sMusic) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (bTransparentBackground)
    {
        Vector vColor(-1.0f, -1.0f, -1.0f);
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->GetInGameGui()->StartCreditSequence(vColor, sMusic);
    }
    else
    {
        Vector vColor(0.0f, 0.0f, 0.0f);
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->GetInGameGui()->StartCreditSequence(vColor, sMusic);
    }
    return 0;
}
#endif
int CSWVirtualMachineCommands::ExecuteCommandGetDifficultyMultiplier(  // PC 0x005CB2A0
    int nCommand, int nArgs)
{
    float fMultiplier = 1.0f;
    int nDifficulty =
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty;

    {
        CExoString sColumn("MULTIPLIER");
        CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(pArrays->m_pDifficultyOptions);
        C2DA* pDifficultyOptions = pArrays->m_pDifficultyOptions;
        pDifficultyOptions->GetFLOATEntry(nDifficulty, sColumn, &fMultiplier);
    }
    return g_pVirtualMachine->StackPushFloat(fMultiplier) != 0
               ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005CB350
int CSWVirtualMachineCommands::ExecuteCommandSetEffectIcon(int nCommand, int nArgs)
{
    void* pEffectStructure;
    int nIcon;

    if (g_pVirtualMachine->StackPopEngineStructure(0, &pEffectStructure) == 0)
    {
        return ERROR_STACK_POP;
    }
    CGameEffect* pEffect = (CGameEffect*)pEffectStructure;
    if (g_pVirtualMachine->StackPopInteger(&nIcon) == 0)
    {
        delete pEffect;
        return ERROR_STACK_POP;
    }

    CGameEffect* pIconEffect = new CGameEffect(pEffect);
    if (pIconEffect != NULL)
    {
        pIconEffect->m_nType = CSWSEffectListHandler::EFFECT_EFFECT_ICON;
        pIconEffect->m_bExposed = 1;
        pIconEffect->SetCreator(m_oidObject);
        pIconEffect->m_bExposed = 1;
        pIconEffect->SetInteger(0, nIcon);

        CGameEffect* pLinkedEffect = new CGameEffect(pEffect);
        if (pLinkedEffect != NULL)
        {
            pLinkedEffect->m_nType = CSWSEffectListHandler::EFFECT_LINK;
            pLinkedEffect->m_bExposed = 1;
            pLinkedEffect->SetCreator(m_oidObject);
            pLinkedEffect->SetLinked(pIconEffect, pEffect);
            pEffect = pLinkedEffect;
        }
        else
        {
            delete pIconEffect;
        }
    }
    if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    delete pEffect;
    return 0;
}
// PC 0x005CB510
int CSWVirtualMachineCommands::ExecuteCommandFaceObjectAwayFromObject(
    int nCommand, int nArgs)
{

    unsigned long oidFacer;
    unsigned long oidObjectToFaceAwayFrom;
    CSWSObject* pFacer = NULL;
    CSWSObject* pObjectToFaceAwayFrom = NULL;

    if (g_pVirtualMachine->StackPopObject(&oidFacer) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopObject(&oidObjectToFaceAwayFrom) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pGameObject = pServerExoApp->GetGameObject(oidFacer);
    if (pGameObject != NULL)
    {
        pFacer = pGameObject->AsSWSObject();
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObjectToFaceAwayFrom);
    if (pGameObject != NULL)
    {
        pObjectToFaceAwayFrom = pGameObject->AsSWSObject();
    }
    if (pFacer != NULL && pObjectToFaceAwayFrom != NULL &&
        pFacer->m_oidArea == pObjectToFaceAwayFrom->m_oidArea)
    {
        Vector vDirection;
        vDirection = pFacer->m_vPosition - pObjectToFaceAwayFrom->m_vPosition;
        if (vDirection * vDirection >
            SWVM_FACE_DIRECTION_EPSILON_SQUARED)
        {
            vDirection = normalize(vDirection);
            pFacer->SetOrientation(vDirection);
        }
    }
    return 0;
}
// PC 0x005CB660
int CSWVirtualMachineCommands::ExecuteCommandSetAreaFogColor(int nCommand,
                                                             int nArgs)
{
    unsigned long oidArea;
    Vector vRed;
    CServerExoApp* pServerExoApp;
    CSWSArea* pArea;
    CSWCArea* pSWCArea;

    if (g_pVirtualMachine->StackPopObject(&oidArea) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&vRed.m_fX) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&vRed.m_fY) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&vRed.m_fZ) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pArea = pServerExoApp->GetAreaByGameObjectID(oidArea);
    if (pArea != NULL)
    {
        pArea->SetFogColor(vRed);
        pSWCArea = pArea->GetClientArea();
        if (pSWCArea != NULL)
        {
            pSWCArea->SetFogColor(vRed);
        }
    }
    return 0;
}
// PC 0x005CB720
int CSWVirtualMachineCommands::ExecuteCommandChangeItemCost(int nCommand,
                                                          int nArgs)
{

    float fCostMultiplier = 1.0f;
    CExoString sItem;

    if (g_pVirtualMachine->StackPopString(&sItem) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopFloat(&fCostMultiplier) == 0)
    {
        return ERROR_STACK_POP;
    }

    CResRef cResRef((char*)sItem.CStr());
    CSWSItem* pItem = new CSWSItem(OBJECT_INVALID);
    if (pItem != NULL)
    {
        if (pItem->LoadFromTemplate(cResRef) != 0)
        {
            pItem->GetBaseItem()->SetCostMultiplier(fCostMultiplier);
        }
    }
    return 0;
}
// PC 0x005CB860
int CSWVirtualMachineCommands::ExecuteCommandGetIsLiveContentAvailable(
    int nCommand, int nArgs)
{

    BOOL bAvailable = FALSE;
    int nPackage;
    CExoString sAlias;

    if (g_pVirtualMachine->StackPopInteger(&nPackage) == 0)
    {
        return ERROR_STACK_POP;
    }
    else
    {
        if (nPackage < LIVE_CONTENT_PKG1 || nPackage > LIVE_CONTENT_PKG6) bAvailable = FALSE; else
        {
            sAlias.Format("LIVE%d", nPackage);
            bAvailable = g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, 0)
                             .GetLength() != 0;
        }
        if (g_pVirtualMachine->StackPushInteger(bAvailable) == 0)
        {
            return ERROR_STACK_PUSH;
        }
        else
        {
            return 0;
        }
    }
}
// PC 0x005CB980
int CSWVirtualMachineCommands::ExecuteCommandResetDialogState(int nCommand,
                                                              int nArgs)
{

    g_pAppManager->m_pClientExoApp->GetInGameGui()->SetGlobalDialogState(0);
    return 0;
}
// PC 0x005CB9A0
int CSWVirtualMachineCommands::ExecuteCommandSetGoodEvilValue(int nCommand,
                                                              int nArgs)
{
    unsigned long oidCreature;
    int nAlignment;
    CServerExoApp* pServerExoApp;
    CSWSCreature* pCreature;

    if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nAlignment) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pCreature = pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        pCreature->m_pStats->SetGoodEvilAlignment(nAlignment);
    }
    return 0;
}
// PC 0x005CBA10
int CSWVirtualMachineCommands::ExecuteCommandGetIsPoisoned(int nCommand,
                                                       int nArgs)
{
    unsigned long oidObject;
    int nResult;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2001;
    }
    nResult = 0;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        pObject = pGameObject->AsSWSObject();
        if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
        {
            nResult = pObject->AsSWSCreature()->m_nPoisonState;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CBAA0
int CSWVirtualMachineCommands::ExecuteCommandSetSoloMode(int nCommand,
                                                         int nArgs)
{
    int nActivate;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nActivate) == 0)
    {
        return -2000;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetPartyTable()->SetSoloMode(nActivate, 1);
    return 0;
}
// PC 0x005CBAE0
int CSWVirtualMachineCommands::ExecuteCommandCancelPostDialogCharacterSwitch(
    int nCommand, int nArgs)
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()
        ->m_oidPostDialogCharacterSwitch = OBJECT_INVALID;
    return 0;
}
// PC 0x005CBB00
int CSWVirtualMachineCommands::ExecuteCommandSetMaxHitPoints(int nCommand,
                                                             int nArgs)
{
    unsigned long oidObject;
    int nMaxHP;
    CServerExoApp* pServerExoApp;
    CGameObject* pGameObject;
    CSWSObject* pObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return -2000;
    }
    if (g_pVirtualMachine->StackPopInteger(&nMaxHP) == 0)
    {
        return -2000;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL)
    {
        pObject = (CSWSObject*)pGameObject->AsSWSObject();
        if (pObject != NULL)
        {
            pObject->m_nMaxHitPoints = nMaxHP;
            pObject->SetCurrentHitPoints(nMaxHP);
        }
    }
    return 0;
}
// PC 0x005CBB80
int CSWVirtualMachineCommands::ExecuteCommandNoClicksFor(int nCommand,
                                                         int nArgs)
{
    float fResult;
    unsigned long oidResult;
    CClientExoApp* pClientExoApp;

    fResult = 0.0;
    if (g_pVirtualMachine->StackPopFloat(&fResult) == 0)
    {
        return -2000;
    }
    if (fResult > 0.0f)
    {
        oidResult = (int)(fResult * 1000.0f);
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->GetInGameGui()->MarkNoClickEvent(oidResult);
    }
    return 0;
}
// PC 0x005CBBF0
int CSWVirtualMachineCommands::ExecuteCommandHoldWorldFadeInForDialog(
    int nCommand, int nArgs)
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()
        ->m_bHoldWorldFadeInForDialog = 1;
    return 0;
}
// PC 0x005CBC10
int CSWVirtualMachineCommands::ExecuteCommandSuppressStatusSummaryEntry(
    int nCommand, int nArgs)
{
    int nNumEntries;
    CClientExoApp* pClientExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nNumEntries) == 0)
    {
        return -2000;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetInGameGui()->SuppressStatusSummary(nNumEntries);
    return 0;
}
// PC 0x005CBC50
int CSWVirtualMachineCommands::ExecuteCommandGetCheatCode(int nCommand,
                                                           int nArgs)
{
    int nResult;

    if (g_pVirtualMachine->StackPopInteger(&nResult) == 0)
    {
        return -2000;
    }
#ifdef _XBOX
    return g_pVirtualMachine->StackPushInteger(g_pExoInput->GetCheatCombo(nResult)) != 0 ? 0 : -2000;
#else
    return g_pVirtualMachine->StackPushInteger(0) != 0 ? 0 : -2000;
#endif
}
// PC 0x005CBC90
int CSWVirtualMachineCommands::ExecuteCommandSetMusicVolume(int nCommand,
                                                             int nArgs)
{
    float fVolume;

    if (g_pVirtualMachine->StackPopFloat(&fVolume) == 0)
    {
        return -2000;
    }
    fVolume = min(max(0.0f, fVolume), 1.0f);
    g_pExoSound->SetMusicVolume(fVolume, 1);
    return 0;
}
// PC 0x005CBD00
int CSWVirtualMachineCommands::ExecuteCommandCreateItemOnFloor(int nCommand,
                                                              int nArgs)
{

    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    unsigned long oidItem = CGameObjectArray::INVALIDOBJID;
    CExoString sTemplate;
    CSWSItem* pItem = NULL;
    CScriptLocation* pLocation;
    int bUseAppearAnimation = 0;

    if (g_pVirtualMachine->StackPopString(&sTemplate) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocation) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&bUseAppearAnimation) == 0)
    {
        return ERROR_STACK_POP;
    }

    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        CSWSArea* pArea = pModule->GetArea();
        if (pArea != NULL)
        {
            {
                CResRef cTemplate((char*)sTemplate.CStr());
                pItem = new CSWSItem(OBJECT_INVALID);
                pItem->LoadFromTemplate(cTemplate);
            }
            pItem->SetPosition(pLocation->m_vPosition, 1, 1, 0);
            pItem->SetOrientation(pLocation->m_vOrientation);
            pItem->AddToArea(pArea, pLocation->m_vPosition.m_fX,
                             pLocation->m_vPosition.m_fY,
                             pLocation->m_vPosition.m_fZ, 1);
            oidItem = pItem->m_nId;
            if (pItem != NULL && pMessage != NULL)
            {
                pMessage->SendServerToPlayerItemScriptAdd(
                    CGameObjectArray::INVALIDOBJID, pArea, pItem,
                    Vector(pLocation->m_vPosition.m_fX,
                           pLocation->m_vPosition.m_fY,
                           pLocation->m_vPosition.m_fZ));
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidItem) == 0)
    {
        if (pLocation != NULL)
        {
            delete pLocation;
            pLocation = NULL;
        }
        return ERROR_STACK_PUSH;
    }
    if (pLocation != NULL)
    {
        delete pLocation;
        pLocation = NULL;
    }
    return 0;
}
// PC 0x005CBF20
int CSWVirtualMachineCommands::ExecuteCommandSetAvailableNPCID(int nCommand,
                                                               int nArgs)
{
    int nNPC;
    unsigned long oidOidNPC;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nNPC) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopObject(&oidOidNPC) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pServerExoApp->GetPartyTable()->SetNPCObject(nNPC, oidOidNPC);
    return 0;
}
// PC 0x005CC490
int CSWVirtualMachineCommands::ExecuteCommandIsMoviePlaying(int nCommand,
                                                            int nArgs)
{
    int nResult;
    CClientExoApp* pClientExoApp;

    pClientExoApp = g_pAppManager->m_pClientExoApp;
    nResult = pClientExoApp->IsMoviePlaying();
    return g_pVirtualMachine->StackPushInteger(nResult) != 0 ? 0 : -2000;
}
// PC 0x005CC4C0
int CSWVirtualMachineCommands::ExecuteCommandQueueMovie(int nCommand, int nArgs)
{
    int nResult;
    CExoString sMovie;
    int nSkippable;
    CClientExoApp* pClientExoApp;

    nResult = 0;
    if (g_pVirtualMachine->StackPopString(&sMovie) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nSkippable) == 0)
    {
        return -2001;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
#ifndef _XBOX
    pClientExoApp->AddMovieToMovieQueue(sMovie, nSkippable);
#endif
    nResult = -1;
    return 0;
}
// PC 0x005CC580
int CSWVirtualMachineCommands::ExecuteCommandPlayMovieQueue(int nCommand,
                                                            int nArgs)
{
    int nAllowSeparateSkips;
    CClientExoApp* pClientExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nAllowSeparateSkips) == 0)
    {
        return -2001;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
#ifndef _XBOX
    pClientExoApp->PlayMovieQueue(nAllowSeparateSkips);
#endif
    return 0;
}
// PC 0x005CC5C0
int CSWVirtualMachineCommands::ExecuteCommandYavinHackCloseDoor(int nCommand,
                                                                int nArgs)
{
    unsigned long oididDoor;
    CServerExoApp* pServerExoApp;
    CSWSDoor* pDoor;

    if (g_pVirtualMachine->StackPopObject(&oididDoor) == 0)
    {
        return -2001;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pDoor = pServerExoApp->GetDoorByGameObjectID(oididDoor);
    if (pDoor != NULL)
    {
        pDoor->SetOpenState(CSWSDoor::OPEN_STATE_CLOSED, 0);
    }
    return 0;
}
// PC 0x005CEFB0
int CSWVirtualMachineCommands::ExecuteCommandGetCurrentAction(int nCommand,
                                                            int nArgs)
{

    int nAction = ACTION_INVALID;
    unsigned long oidTarget;
    if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pGameObject = pServerExoApp->GetGameObject(oidTarget);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        CSWSObject* pObject = pGameObject->AsSWSObject();
        CExoLinkedList<CSWSObjectActionNode>* pActions = &pObject->m_lstActions;
        if (pActions != NULL)
        {
            CSWSObjectActionNode* pAction = pActions->GetHead();
            CExoLinkedListNode* pPosition = pActions->GetHeadPos();
            if (pAction != NULL)
            {
                unsigned short nGroupId = pAction->m_nGroupId;
                while (pAction != NULL && pAction->m_nGroupId == nGroupId)
                {
                    if (pObject->GetAcceptableAction(pAction->m_nActionType) != 0)
                    {
                        nAction = (unsigned short)pObject->GetAQActionIDByID(
                            (unsigned short)pAction->m_nActionType, 0);
                    }
                    pAction = pActions->GetNext(pPosition);
                }
            }
            else
            {
                nAction = ACTION_QUEUEEMPTY;
            }
        }
    }
    return g_pVirtualMachine->StackPushInteger(nAction) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005CF0B0
int CSWVirtualMachineCommands::ExecuteCommandGetEffect(int nCommand, int nArgs)
{

    unsigned long oidObject;
    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }

    CGameObject* pGameObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    int bAppliedEffect = 1;
    CSWSObject* pObject;
    if (pGameObject != NULL &&
        (pObject = pGameObject->AsSWSObject()) != NULL)
    {
        int nEffect;
        if (nCommand == COMMAND_GET_FIRST_EFFECT)
        {
            pObject->m_nEffectIterator = 0;
            nEffect = 0;
        }
        else
        {
            nEffect = pObject->m_nEffectIterator;
        }

        int nNumEffects = pObject->m_lstEffects.num;
        for (; nEffect < nNumEffects; ++nEffect)
        {
            CGameEffect* pEffect = pObject->m_lstEffects[nEffect];
            if (pEffect->m_bExposed != 0 &&
                pEffect->m_nType != CSWSEffectListHandler::EFFECT_EFFECT_ICON)
            {
                if ((pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                        DURATION_TYPE_PERMANENT ||
                    (pEffect->m_nSubType & CGameEffect::DURATION_MASK) ==
                        DURATION_TYPE_TEMPORARY)
                {
                    break;
                }
            }
        }

        CGameEffect* pEffect;
        if (nEffect >= pObject->m_lstEffects.num)
        {
            bAppliedEffect = 0;
            pObject->m_nEffectIterator = 0;
            pEffect = new CGameEffect(1);
            pEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
        }
        else
        {
            pEffect = pObject->m_lstEffects[nEffect];
            pObject->m_nEffectIterator = nEffect + 1;
        }
        if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
        {
            return ERROR_STACK_PUSH;
        }
        if (bAppliedEffect == 0)
        {
            delete pEffect;
        }
    }
    else
    {
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_INVALIDEFFECT;
        if (g_pVirtualMachine->StackPushEngineStructure(0, pEffect) == 0)
        {
            return ERROR_STACK_PUSH;
        }
        delete pEffect;
    }
    return 0;
}
// PC 0x005CF290
int CSWVirtualMachineCommands::ExecuteCommandGetInPersistentObject(int nCommand,
                                                                  int nArgs)
{

    unsigned long oidPersistentObject = OBJECT_INVALID;
    unsigned long oidResult = OBJECT_INVALID;
    int nObjectType;
    int nPersistentZone;
    int nIndex;
    CGameObject* pGameObject;
    float fMinX;
    float fMaxX;

    if (nArgs > 0 && g_pVirtualMachine->StackPopObject(&oidPersistentObject) == 0)
        return ERROR_STACK_POP;
    if (nArgs > 1 && g_pVirtualMachine->StackPopInteger(&nObjectType) == 0)
        return ERROR_STACK_POP;
    if (nArgs > 2 && g_pVirtualMachine->StackPopInteger(&nPersistentZone) == 0)
        return ERROR_STACK_POP;

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    if ((unsigned char)pServer->GetObjectArray()->GetGameObject(
            oidPersistentObject, &pGameObject) == CGameObjectArray::SUCCESS)
    {
        if (pGameObject->AsSWSEncounter() != NULL)
        {
            CSWSEncounter* pEncounter = pGameObject->AsSWSEncounter();
            CSWSArea* pArea = pGameObject->AsSWSEncounter()->GetArea();
            if (pArea != NULL)
            {
                if (nPersistentZone == PERSISTENT_ZONE_ACTIVE)
                {
                    fMinX = pEncounter->m_fMinX;
                    fMaxX = pEncounter->m_fMaxX;
                }
                if (nCommand == COMMAND_GET_FIRST_IN_PERSISTENT_OBJECT)
                {
                    if (pArea->GetFirstObjectIndiceByX(&nIndex, fMinX) == 0)
                        return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
                }
                else
                {
                    if (nPersistentZone == PERSISTENT_ZONE_ACTIVE)
                        nIndex = pEncounter->m_nActivateObjectArrayIndex + 1;
                    if (nIndex == pArea->m_lstObjectIds.num)
                        return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
                }
                bool bFound = false;
                Vector vPosition;
                CGameObject* pObject;
                while (nIndex < pArea->m_lstObjectIds.num && !bFound)
                {
                    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                    if ((unsigned char)pServer->GetObjectArray()->GetGameObject(
                            pArea->m_lstObjectIds[nIndex], &pObject) == CGameObjectArray::SUCCESS && pObject != NULL)
                    {
                        int nResidentType = pObject->m_nObjectType;
                        if (
                        (nObjectType == OBJECT_TYPE_CREATURE &&
                         nResidentType == SW_OBJECT_TYPE_CREATURE) ||
                        (nObjectType == OBJECT_TYPE_ITEM &&
                         nResidentType == SW_OBJECT_TYPE_ITEM) ||
                        (nObjectType == OBJECT_TYPE_TRIGGER &&
                         nResidentType == SW_OBJECT_TYPE_TRIGGER) ||
                        (nObjectType == OBJECT_TYPE_DOOR &&
                         nResidentType == SW_OBJECT_TYPE_DOOR) ||
                        (nObjectType == OBJECT_TYPE_AREA_OF_EFFECT &&
                         nResidentType == SW_OBJECT_TYPE_AREA_OF_EFFECT) ||
                        (nObjectType == OBJECT_TYPE_WAYPOINT &&
                         nResidentType == SW_OBJECT_TYPE_WAYPOINT) ||
                        (nObjectType == OBJECT_TYPE_PLACEABLE &&
                         nResidentType == SW_OBJECT_TYPE_PLACEABLE) ||
                        (nObjectType == OBJECT_TYPE_STORE &&
                         nResidentType == SW_OBJECT_TYPE_STORE) ||
                        (nObjectType == OBJECT_TYPE_ENCOUNTER &&
                         nResidentType == SW_OBJECT_TYPE_ENCOUNTER) ||
                        (nObjectType == OBJECT_TYPE_SOUND &&
                         nResidentType == SW_OBJECT_TYPE_SOUND) ||
                        nObjectType == OBJECT_TYPE_ALL
                        )
                        {
                            bFound = true;
                            vPosition = ((CSWSObject*)pObject)->m_vPosition;
                            if (vPosition.m_fX > fMaxX)
                                return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
                            if (nPersistentZone == PERSISTENT_ZONE_ACTIVE)
                            {
                                if (pEncounter->GetInActivateArea(vPosition))
                                {
                                    oidResult = pArea->m_lstObjectIds[nIndex];
                                    pEncounter->m_nActivateObjectArrayIndex = nIndex + 1;
                                }
                                else
                                    ++nIndex;
                            }
                        }
                        else
                            ++nIndex;
                    }
                    else
                        oidResult = OBJECT_INVALID;
                }
            }
        }
        else if (pGameObject->AsSWSAreaOfEffectObject() != NULL)
        {
            if (nPersistentZone != PERSISTENT_ZONE_ACTIVE)
                return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
            CSWSAreaOfEffectObject* pAreaEffect = pGameObject->AsSWSAreaOfEffectObject();
            CSWSAreaOfEffectObject* pAreaObject = pGameObject->AsSWSAreaOfEffectObject();
            CSWSArea* pArea = pAreaObject->GetArea();
            if (pArea != NULL)
            {
                fMinX = pAreaEffect->GetPosition().m_fX - pAreaEffect->GetRadius();
                fMaxX = pAreaEffect->GetPosition().m_fX + pAreaEffect->GetRadius();
                if (nCommand == COMMAND_GET_FIRST_IN_PERSISTENT_OBJECT)
                {
                    if (pArea->GetFirstObjectIndiceByX(&nIndex, fMinX) == 0)
                        return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
                }
                else
                {
                    nIndex = pAreaEffect->m_nObjectIterator;
                    if (nIndex == pArea->m_lstObjectIds.num)
                        return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
                }
                bool bFound = false;
                CGameObject* pObject;
                while (nIndex < pArea->m_lstObjectIds.num && !bFound)
                {
                    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                    if ((unsigned char)pServer->GetObjectArray()->GetGameObject(
                            pArea->m_lstObjectIds[nIndex], &pObject) == CGameObjectArray::SUCCESS && pObject != NULL)
                    {
                        int nResidentType = pObject->m_nObjectType;
                        if (
                        (nObjectType == OBJECT_TYPE_CREATURE &&
                         nResidentType == SW_OBJECT_TYPE_CREATURE) ||
                        (nObjectType == OBJECT_TYPE_ITEM &&
                         nResidentType == SW_OBJECT_TYPE_ITEM) ||
                        (nObjectType == OBJECT_TYPE_TRIGGER &&
                         nResidentType == SW_OBJECT_TYPE_TRIGGER) ||
                        (nObjectType == OBJECT_TYPE_DOOR &&
                         nResidentType == SW_OBJECT_TYPE_DOOR) ||
                        (nObjectType == OBJECT_TYPE_AREA_OF_EFFECT &&
                         nResidentType == SW_OBJECT_TYPE_AREA_OF_EFFECT) ||
                        (nObjectType == OBJECT_TYPE_WAYPOINT &&
                         nResidentType == SW_OBJECT_TYPE_WAYPOINT) ||
                        (nObjectType == OBJECT_TYPE_PLACEABLE &&
                         nResidentType == SW_OBJECT_TYPE_PLACEABLE) ||
                        (nObjectType == OBJECT_TYPE_STORE &&
                         nResidentType == SW_OBJECT_TYPE_STORE) ||
                        (nObjectType == OBJECT_TYPE_ENCOUNTER &&
                         nResidentType == SW_OBJECT_TYPE_ENCOUNTER) ||
                        (nObjectType == OBJECT_TYPE_SOUND &&
                         nResidentType == SW_OBJECT_TYPE_SOUND) ||
                        nObjectType == OBJECT_TYPE_ALL
                        )
                        {
                            Vector vPosition = ((CSWSObject*)pObject)->m_vPosition;
                            if (vPosition.m_fX > fMaxX)
                            {
                                pAreaEffect->m_nObjectIterator = 0;
                                return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
                            }
                            if (pObject->AsSWSCreature() != NULL)
                                pObject->AsSWSCreature()->GetDeadTemp();
                            if (pAreaEffect->InAreaOfEffect(vPosition) &&
                                !pObject->AsSWSObject()->GetDead() &&
                                !pObject->AsSWSCreature()->GetDeadTemp())
                            {
                                oidResult = pArea->m_lstObjectIds[nIndex];
                                pAreaEffect->m_nObjectIterator = nIndex + 1;
                                bFound = true;
                            }
                            else
                                ++nIndex;
                        }
                        else
                            ++nIndex;
                    }
                }
            }
        }
        else if (pGameObject->AsSWSTrigger() != NULL)
        {
            if (nPersistentZone != PERSISTENT_ZONE_ACTIVE)
                return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
            CSWSTrigger* pTrigger = pGameObject->AsSWSTrigger();
            CSWSArea* pArea = pTrigger->GetArea();
            if (pArea != NULL)
            {
                fMaxX = pTrigger->m_pGeometry[0].m_fX;
                fMinX = fMaxX;
                for (int i = 1; i < pTrigger->m_nGeometryPointCount; ++i)
                {
                    if (pTrigger->m_pGeometry[i].m_fX < fMinX)
                        fMinX = pTrigger->m_pGeometry[i].m_fX;
                    if (pTrigger->m_pGeometry[i].m_fX > fMaxX)
                        fMaxX = pTrigger->m_pGeometry[i].m_fX;
                }
                if (nCommand == COMMAND_GET_FIRST_IN_PERSISTENT_OBJECT)
                {
                    if (pArea->GetFirstObjectIndiceByX(&nIndex, fMinX) == 0)
                        return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
                }
                else
                {
                    nIndex = pTrigger->m_nObjectIterator;
                    if (nIndex == pArea->m_lstObjectIds.num)
                        return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
                }
                bool bFound = false;
                Vector vPosition;
                CGameObject* pObject;
                while (nIndex < pArea->m_lstObjectIds.num && !bFound)
                {
                    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                    if ((unsigned char)pServer->GetObjectArray()->GetGameObject(
                            pArea->m_lstObjectIds[nIndex], &pObject) == CGameObjectArray::SUCCESS && pObject != NULL)
                    {
                        int nResidentType = pObject->m_nObjectType;
                        if (
                        (nObjectType == OBJECT_TYPE_CREATURE &&
                         nResidentType == SW_OBJECT_TYPE_CREATURE) ||
                        (nObjectType == OBJECT_TYPE_ITEM &&
                         nResidentType == SW_OBJECT_TYPE_ITEM) ||
                        (nObjectType == OBJECT_TYPE_TRIGGER &&
                         nResidentType == SW_OBJECT_TYPE_TRIGGER) ||
                        (nObjectType == OBJECT_TYPE_DOOR &&
                         nResidentType == SW_OBJECT_TYPE_DOOR) ||
                        (nObjectType == OBJECT_TYPE_AREA_OF_EFFECT &&
                         nResidentType == SW_OBJECT_TYPE_AREA_OF_EFFECT) ||
                        (nObjectType == OBJECT_TYPE_WAYPOINT &&
                         nResidentType == SW_OBJECT_TYPE_WAYPOINT) ||
                        (nObjectType == OBJECT_TYPE_PLACEABLE &&
                         nResidentType == SW_OBJECT_TYPE_PLACEABLE) ||
                        (nObjectType == OBJECT_TYPE_STORE &&
                         nResidentType == SW_OBJECT_TYPE_STORE) ||
                        (nObjectType == OBJECT_TYPE_ENCOUNTER &&
                         nResidentType == SW_OBJECT_TYPE_ENCOUNTER) ||
                        (nObjectType == OBJECT_TYPE_SOUND &&
                         nResidentType == SW_OBJECT_TYPE_SOUND) ||
                        nObjectType == OBJECT_TYPE_ALL
                        )
                        {
                            vPosition = ((CSWSObject*)pObject)->m_vPosition;
                            if (vPosition.m_fX > fMaxX)
                            {
                                pTrigger->m_nObjectIterator = 0;
                                return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
                            }
                            if (pTrigger->InTrigger(vPosition) &&
                                !pObject->AsSWSObject()->GetDead() &&
                                !pObject->AsSWSCreature()->GetDeadTemp())
                            {
                                oidResult = pArea->m_lstObjectIds[nIndex];
                                pTrigger->m_nObjectIterator = nIndex + 1;
                                bFound = true;
                            }
                            else
                                ++nIndex;
                        }
                        else
                            ++nIndex;
                    }
                }
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidResult) == 0)
        return ERROR_STACK_PUSH;
    return 0;
}
// PC 0x005CFA80
int CSWVirtualMachineCommands::ExecuteCommandGetItemInInventory(int nCommand,
                                                               int nArgs)
{
    NwvmachinecommandsTrace1();

    unsigned short nItemIndex;
    unsigned long oidItem = OBJECT_INVALID;
    CItemRepository* pMainRepository = NULL;
    unsigned long oidObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }

    CGameObject* pGameObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    if (pGameObject != NULL)
    {
        CItemRepository* pRepository = NULL;
        if (pGameObject->AsSWSCreature() != NULL)
        {
            CSWSCreature* pCreature = pGameObject->AsSWSCreature();
            pRepository = pCreature->GetItemRepository(TRUE);
            pMainRepository = pRepository;
            if (nCommand == COMMAND_GET_FIRST_ITEM_IN_INVENTORY)
            {
                nItemIndex = 0;
                pCreature->SetInventoryContainer(CGameObjectArray::INVALIDOBJID);
                pCreature->SetInventoryContainerIndex(0);
                pCreature->SetInventoryIndex(1);
            }
            else
            {
                CSWSItem* pContainer = g_pAppManager->GetServerExoApp()->
                    GetItemByGameObjectID(pCreature->GetInventoryContainer());
                if (pContainer != NULL && pContainer->m_oidPossessor == pCreature->GetId())
                {
                    pRepository = pContainer->m_pRepository;
                    nItemIndex = pCreature->GetInventoryContainerIndex();
                    pCreature->SetInventoryContainerIndex(nItemIndex + 1);
                }
                else
                {
                    pCreature->SetInventoryContainer(CGameObjectArray::INVALIDOBJID);
                    pCreature->SetInventoryContainerIndex(0);
                    nItemIndex = pCreature->GetInventoryIndex();
                    pCreature->SetInventoryIndex(nItemIndex + 1);
                }
            }
        }
        else if (pGameObject->AsSWSPlaceable() != NULL)
        {
            CSWSPlaceable* pPlaceable = pGameObject->AsSWSPlaceable();
            pRepository = pPlaceable->m_pcItemRepository;
            pMainRepository = pRepository;
            if (nCommand == COMMAND_GET_FIRST_ITEM_IN_INVENTORY)
            {
                nItemIndex = 0;
                pPlaceable->SetInventoryContainer(CGameObjectArray::INVALIDOBJID);
                pPlaceable->SetInventoryContainerIndex(0);
                pPlaceable->SetInventoryIndex(1);
            }
            else
            {
                CSWSItem* pContainer = g_pAppManager->GetServerExoApp()->
                    GetItemByGameObjectID(pPlaceable->GetInventoryContainer());
                if (pContainer != NULL && pContainer->m_oidPossessor == pPlaceable->GetId())
                {
                    pRepository = pContainer->m_pRepository;
                    nItemIndex = pPlaceable->GetInventoryContainerIndex();
                    pPlaceable->SetInventoryContainerIndex(nItemIndex + 1);
                }
                else
                {
                    pPlaceable->SetInventoryContainer(CGameObjectArray::INVALIDOBJID);
                    pPlaceable->SetInventoryContainerIndex(0);
                    nItemIndex = pPlaceable->GetInventoryIndex();
                    pPlaceable->SetInventoryIndex(nItemIndex + 1);
                }
            }
        }
        else if (pGameObject->AsSWSItem() != NULL)
        {
            nItemIndex = 0;
            pRepository = pGameObject->AsSWSItem()->m_pRepository;
            if (nCommand != COMMAND_GET_FIRST_ITEM_IN_INVENTORY)
            {
                nItemIndex = pGameObject->AsSWSItem()->m_nInventoryIndex;
            }
            pGameObject->AsSWSItem()->m_nInventoryIndex = nItemIndex + 1;
        }
        else if (pGameObject->AsSWSStore() != NULL)
        {
            CSWSStore* pStore = pGameObject->AsSWSStore();
            unsigned short nStoreIndex;
            if (nCommand == COMMAND_GET_FIRST_ITEM_IN_INVENTORY)
                nStoreIndex = 0;
            else
                nStoreIndex = pStore->m_nInventoryIndex;
            pStore->m_nInventoryIndex = nStoreIndex + 1;
        }

        if (pRepository != NULL)
        {
            if ((unsigned int)pRepository->GetNumItems() <= nItemIndex &&
                pRepository != pMainRepository)
            {
                pRepository = pMainRepository;
                if (pGameObject->AsSWSCreature() != NULL)
                {
                    nItemIndex = pGameObject->AsSWSCreature()->GetInventoryIndex();
                    pGameObject->AsSWSCreature()->SetInventoryIndex(nItemIndex + 1);
                    pGameObject->AsSWSCreature()->SetInventoryContainer(CGameObjectArray::INVALIDOBJID);
                    pGameObject->AsSWSCreature()->SetInventoryContainerIndex(0);
                }
                else if (pGameObject->AsSWSPlaceable() != NULL)
                {
                    nItemIndex = pGameObject->AsSWSPlaceable()->GetInventoryIndex();
                    pGameObject->AsSWSPlaceable()->SetInventoryIndex(nItemIndex + 1);
                    pGameObject->AsSWSPlaceable()->SetInventoryContainer(CGameObjectArray::INVALIDOBJID);
                    pGameObject->AsSWSPlaceable()->SetInventoryContainerIndex(0);
                }
            }
            if ((unsigned int)pRepository->GetNumItems() > nItemIndex)
            {
                CSWSItem* pItem = pRepository->ItemListGetItem(nItemIndex);
                if (pItem != NULL)
                {
                    CSWBaseItem* pBaseItem = g_pRules->GetBaseItemArray()->
                        GetBaseItem(pItem->GetBaseItemType());
                    if (pBaseItem != NULL && pBaseItem->m_bIsContainer)
                    {
                        if (pGameObject->AsSWSCreature() != NULL)
                        {
                            pGameObject->AsSWSCreature()->SetInventoryContainer(pItem->GetId());
                            pGameObject->AsSWSCreature()->SetInventoryContainerIndex(0);
                        }
                        else if (pGameObject->AsSWSPlaceable() != NULL)
                        {
                            pGameObject->AsSWSPlaceable()->SetInventoryContainer(pItem->GetId());
                            pGameObject->AsSWSPlaceable()->SetInventoryContainerIndex(0);
                        }
                    }
                    oidItem = pItem->GetId();
                }
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(oidItem) != 0 ? 0 : ERROR_STACK_PUSH;
}
// PC 0x005CFE60
int CSWVirtualMachineCommands::ExecuteCommandGetJournalEntry(
    int nCommand, int nArgs)
{

    CExoString sPlotID;
    int nState = 0;
    if (g_pVirtualMachine->StackPopString(&sPlotID) == 0)
    {
        return ERROR_STACK_POP;
    }
    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayerList == NULL)
    {
        return 1;
    }
    CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
    while (pPosition != NULL)
    {
        CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
        if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
        {
            CSWSCreature* pCreature =
                pPlayer->GetGameObject()->AsSWSCreature();
            if (pCreature != NULL)
            {
                nState = pCreature->GetJournal()->GetState(sPlotID);
                break;
            }
        }
        pPlayerList->GetNext(pPosition);
    }
    if (g_pVirtualMachine->StackPushInteger(nState) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005CFFC0
int CSWVirtualMachineCommands::ExecuteCommandGetNearestMine(int nCommand, int nArgs)
{
    CExoArrayList<unsigned long>* pTrapList = NULL;
    float fNearestDistanceSquared = SWVM_NEAREST_MINE_DISTANCE_SQUARED;
    unsigned long oidObject;
    int bDetected;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&bDetected) == 0)
    {
        goto StackError;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pGameObject = pServerExoApp->GetGameObject(oidObject);
    CSWSObject* pObject;
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        pObject = pGameObject->AsSWSObject();
        if (pObject->GetArea() != NULL)
        {
            pTrapList = &pGameObject->AsSWSObject()->GetArea()->m_lstTrapObjectIds;
        }
    }

    unsigned long oidNearest = OBJECT_INVALID;
    if (pTrapList != NULL)
    {
        for (int nTrap = 0; nTrap < pTrapList->num; ++nTrap)
        {
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            CGameObject* pTrap = pServerExoApp->GetGameObject((*pTrapList)[nTrap]);
            if (pTrap != NULL)
            {
                if (bDetected == 1)
                {
                    int bKnown;
                    if (pTrap->AsSWSTrigger() != NULL)
                    {
                        CSWSTrigger* pMine = pTrap->AsSWSTrigger();
                        bKnown = pMine->m_lstTrapTargets.Contains(oidObject);
                    }
                    else if (pTrap->AsSWSDoor() != NULL)
                    {
                        CSWSDoor* pMine = pTrap->AsSWSDoor();
                        bKnown = pMine->m_lstTrapDetectedBy.Contains(oidObject);
                    }
                    else if (pTrap->AsSWSPlaceable() != NULL)
                    {
                        CSWSPlaceable* pMine = pTrap->AsSWSPlaceable();
                        bKnown = pMine->m_lstTrapDetectedBy.Contains(oidObject);
                    }
                    else
                    {
                        continue;
                    }
                    if (bKnown == 0)
                    {
                        continue;
                    }
                }
                Vector& vTrapPosition = pTrap->AsSWSObject()->m_vPosition;
                float fX = pObject->m_vPosition.m_fX - vTrapPosition.m_fX;
                float fY = pObject->m_vPosition.m_fY - vTrapPosition.m_fY;
                float fZ = pObject->m_vPosition.m_fZ - vTrapPosition.m_fZ;
                float fDistanceSquared = fX * fX + fZ * fZ;
                fDistanceSquared += fY * fY;
                if (fDistanceSquared < fNearestDistanceSquared)
                {
                    fNearestDistanceSquared = fDistanceSquared;
                    oidNearest = (*pTrapList)[nTrap];
                }
            }
        }
    }
    if (g_pVirtualMachine->StackPushObject(oidNearest) != 0)
    {
        return 0;
    }
    else
    {
        return ERROR_STACK_PUSH;
    }
StackError:
    return ERROR_STACK_POP;
}
// PC 0x005D0190
int CSWVirtualMachineCommands::ExecuteCommandGetObjectInShape(int nCommand,
                                                           int nArgs)
{

    Vector vTarget;
    Vector vOrigin;
    float fSize;
    Vector vObject;
    Vector vProjection;
    Vector vDistance;
    int nShape;
    int bLineOfSight;
    int nObjectFilter;
    CScriptLocation* pLocation = NULL;
    CGameObject* pGameObject = NULL;
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();

    if (g_pVirtualMachine->StackPopInteger(&nShape) == 0 ||
        g_pVirtualMachine->StackPopFloat(&fSize) == 0 ||
        g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_LOCATION,
                                                  (void**)&pLocation) == 0 ||
        g_pVirtualMachine->StackPopInteger(&bLineOfSight) == 0 ||
        g_pVirtualMachine->StackPopInteger(&nObjectFilter) == 0 ||
        g_pVirtualMachine->StackPopVector(&vOrigin) == 0)
    {
        if (pLocation != NULL)
            delete pLocation;
        return ERROR_STACK_POP;
    }

    CSWSCreature* pPlayer =
        g_pAppManager->GetServerExoApp()->GetPlayerCreature();
    CSWSArea* pArea =
        g_pAppManager->GetServerExoApp()->GetModule()->GetArea();
    if (pPlayer == NULL || pArea == NULL)
    {
        if (pLocation != NULL)
        {
            delete pLocation;
            pLocation = NULL;
        }
        return g_pVirtualMachine->StackPushObject(
                   CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
    }

    vTarget = pLocation->m_vPosition;
    if (nShape == SHAPE_SPELLCONE || nShape == SHAPE_SPELLCYLINDER)
    {
        if (pObjectArray->GetGameObject(m_oidObject, &pGameObject) ==
            CGameObjectArray::SUCCESS)
            vOrigin = ((CSWSObject*)pGameObject)->m_vPosition;
    }
    if (pLocation != NULL)
    {
        delete pLocation;
        pLocation = NULL;
    }

    float fMinX;
    float fMaxX;
    switch (nShape)
    {
    case SHAPE_CONE:
    case SHAPE_SPELLCONE:
        fMinX = (vTarget.m_fX < vOrigin.m_fX ?
                 vTarget.m_fX : vOrigin.m_fX) - fSize;
        fMaxX = (vTarget.m_fX > vOrigin.m_fX ?
                 vTarget.m_fX : vOrigin.m_fX) + fSize;
        break;
    default:
        fMinX = vTarget.m_fX - fSize;
        fMaxX = vTarget.m_fX + fSize;
        break;
    }
    if (nShape == SHAPE_SPELLCYLINDER)
    {
        Vector vCorners[4];
        Vector vDirection;
        vDirection = normalize(vTarget - vOrigin);
        if (vDirection * vDirection <= SWVM_SHAPE_DIRECTION_EPSILON_SQUARED)
            vDirection = Vector(0.0f, 1.0f, 0.0f);
        vTarget = vOrigin + vDirection * fSize;
        float fX = vDirection.m_fX;
        vDirection.m_fX = -vDirection.m_fY;
        vDirection.m_fY = fX;
        vDistance = vDirection * SWVM_SHAPE_CYLINDER_RADIUS;
        vCorners[0] = vOrigin + vDistance;
        vCorners[1] = vOrigin - vDistance;
        vCorners[2] = vTarget + vDistance;
        vCorners[3] = vTarget - vDistance;
        fMinX = vCorners[0].m_fX;
        fMaxX = vCorners[0].m_fX;
        for (int i = 1; i < 4; ++i)
        {
            if (vCorners[i].m_fX < fMinX)
                fMinX = vCorners[i].m_fX;
            if (vCorners[i].m_fX > fMaxX)
                fMaxX = vCorners[i].m_fX;
        }
    }

    int nIndex;
    if (nCommand == COMMAND_GET_FIRST_OBJECT_IN_SHAPE)
    {
        if (pArea->GetFirstObjectIndiceByX(&nIndex, fMinX) == 0)
            return g_pVirtualMachine->StackPushObject(
                       CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
    }
    else
    {
        nIndex = pArea->m_nObjectIterator + 1;
        if (nIndex == pArea->m_lstObjectIds.num)
            return g_pVirtualMachine->StackPushObject(
                       CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
    }
    float fSizeSquared = fSize * fSize;
    for (; nIndex < pArea->m_lstObjectIds.num; ++nIndex)
    {
        int nResult = pObjectArray->GetGameObject(pArea->m_lstObjectIds[nIndex],
                                                 &pGameObject);
        if (nResult == CGameObjectArray::SUCCESS &&
            pGameObject != NULL &&
            pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
            vObject = ((CSWSObject*)pGameObject)->m_vPosition;
        if (vObject.m_fX > fMaxX)
            return g_pVirtualMachine->StackPushObject(
                       CGameObjectArray::INVALIDOBJID) != 0 ? 0 : ERROR_STACK_PUSH;
        if (((nObjectFilter & OBJECT_TYPE_CREATURE) &&
             pGameObject->m_nObjectType == SW_OBJECT_TYPE_CREATURE) ||
            ((nObjectFilter & OBJECT_TYPE_ITEM) &&
             pGameObject->m_nObjectType == SW_OBJECT_TYPE_ITEM) ||
            ((nObjectFilter & OBJECT_TYPE_TRIGGER) &&
             pGameObject->m_nObjectType == SW_OBJECT_TYPE_TRIGGER) ||
            ((nObjectFilter & OBJECT_TYPE_DOOR) &&
             pGameObject->m_nObjectType == SW_OBJECT_TYPE_DOOR) ||
            ((nObjectFilter & OBJECT_TYPE_AREA_OF_EFFECT) &&
             pGameObject->m_nObjectType == SW_OBJECT_TYPE_AREA_OF_EFFECT) ||
            ((nObjectFilter & OBJECT_TYPE_WAYPOINT) &&
             pGameObject->m_nObjectType == SW_OBJECT_TYPE_WAYPOINT) ||
            ((nObjectFilter & OBJECT_TYPE_PLACEABLE) &&
             pGameObject->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE) ||
            ((nObjectFilter & OBJECT_TYPE_STORE) == OBJECT_TYPE_STORE &&
             pGameObject->m_nObjectType == SW_OBJECT_TYPE_STORE) ||
            ((nObjectFilter & OBJECT_TYPE_ENCOUNTER) &&
             pGameObject->m_nObjectType == SW_OBJECT_TYPE_ENCOUNTER) ||
            ((nObjectFilter & OBJECT_TYPE_SOUND) &&
             pGameObject->m_nObjectType == SW_OBJECT_TYPE_SOUND) ||
            ((nObjectFilter & OBJECT_TYPE_ALL) == OBJECT_TYPE_ALL))
        {
            BOOL bInShape = FALSE;
            switch (nShape)
            {
            case SHAPE_SPELLCYLINDER:
            {
                float fX = vObject.m_fX - vOrigin.m_fX;
                float fY = vObject.m_fY - vOrigin.m_fY;
                float fZ = vObject.m_fZ - vOrigin.m_fZ;
                if (fZ * fZ + fY * fY + fX * fX <= fSizeSquared &&
                    normalize(vTarget - vOrigin) * normalize(vObject - vOrigin) >= 0.0f)
                {
                    vProjection = lineproject(vOrigin, vTarget, vObject);
                    vDistance = vObject - vProjection;
                    if (vDistance * vDistance <= SWVM_SHAPE_CYLINDER_RADIUS_SQUARED)
                        bInShape = TRUE;
                }
                break;
            }
            case SHAPE_CONE:
            {
                vProjection = lineproject(vOrigin, vTarget, vObject);
                vDistance = vObject - vProjection;
                if (normalize(vTarget - vOrigin) * normalize(vObject - vOrigin) > 0.0f)
                {
                    float fRatio = fSize / magnitude(vTarget - vOrigin);
                    float fRadius = magnitude(vProjection) * fRatio;
                    if (vDistance * vDistance <= fRadius * fRadius)
                        bInShape = TRUE;
                }
                break;
            }
            case SHAPE_CUBE:
                if (vTarget.m_fX + fSize >= vObject.m_fX &&
                    vTarget.m_fX - fSize <= vObject.m_fX &&
                    vTarget.m_fY + fSize >= vObject.m_fY &&
                    vTarget.m_fY - fSize <= vObject.m_fY &&
                    vTarget.m_fZ + fSize >= vObject.m_fZ &&
                    vTarget.m_fZ - fSize <= vObject.m_fZ)
                    bInShape = TRUE;
                break;
            case SHAPE_SPELLCONE:
            {
                Vector vDirection;
                vDirection = lineproject(vOrigin, vTarget, vObject) - vOrigin;
                if (normalize(vTarget - vOrigin) * normalize(vObject - vOrigin) >=
                        SWVM_SHAPE_SPELLCONE_COSINE &&
                    vDirection * vDirection <= fSizeSquared)
                    bInShape = TRUE;
                break;
            }
            default:
            {
                Vector vDistance = vObject - vTarget;
                if (vDistance * vDistance <= fSizeSquared)
                    bInShape = TRUE;
                break;
            }
            }
            if (bInShape)
            {
                pArea->m_nObjectIterator = nIndex;
                return g_pVirtualMachine->StackPushObject(pGameObject->GetId()) != 0 ?
                       0 : ERROR_STACK_PUSH;
            }
        }
    }
    return g_pVirtualMachine->StackPushObject(CGameObjectArray::INVALIDOBJID) != 0 ?
           0 : ERROR_STACK_PUSH;
}
// PC 0x005D0D80
int CSWVirtualMachineCommands::ExecuteCommandRemoveEffect(int nCommand,
                                                          int nArgs)
{

    unsigned long oidObject;
    CGameEffect* pEffect = NULL;

    if (g_pVirtualMachine->StackPopObject(&oidObject) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopEngineStructure(STRUCTURE_EFFECT,
                                                   (void**)&pEffect) == 0)
    {
        return ERROR_STACK_POP;
    }

    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObject* pGameObject = pServerExoApp->GetGameObject(oidObject);
    if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
    {
        CExoArrayList<CGameEffect*>* pEffects =
            &pGameObject->AsSWSObject()->m_lstEffects;
        CGameEffect* pRemoveEffect = NULL;
        if (pEffects != NULL && pEffects->num > 0)
        {
            for (unsigned long nEffect = 0; nEffect < pEffects->num; ++nEffect)
            {
                CGameEffect* pAppliedEffect = (*pEffects)[nEffect];
                if (pAppliedEffect->m_nID == pEffect->m_nID)
                {
                    pAppliedEffect->m_bExposed = 0;
                    pRemoveEffect = pAppliedEffect;
                }
            }
            if (pRemoveEffect != NULL)
            {
                pServerExoApp = g_pAppManager->m_pServerExoApp;
                pServerExoApp->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_oidObject, oidObject,
                    CServerAIMaster::EVENT_REMOVE_EFFECT, pRemoveEffect);
            }
        }
    }
    delete pEffect;
    return 0;
}
// PC 0x005D10F0
int CSWVirtualMachineCommands::ExecuteCommandRemoveJournalQuestEntry(
    int nCommand, int nArgs)
{

    CExoString sPlotID;
    if (g_pVirtualMachine->StackPopString(&sPlotID) == 0)
    {
        return ERROR_STACK_POP;
    }
    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pPlayerList == NULL)
    {
        return 1;
    }
    CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
    while (pPosition != NULL)
    {
        CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
        if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
        {
            CSWSCreature* pCreature =
                pPlayer->GetGameObject()->AsSWSCreature();
            if (pCreature != NULL)
            {
                pCreature->GetJournal()->Destroy(sPlotID);
            }
        }
        pPlayerList->GetNext(pPosition);
    }
    return 0;
}
// PC 0x005D1220
int CSWVirtualMachineCommands::ExecuteCommandSetCustomToken(int nCommand, int nArgs)
{

    int nCustomTokenNumber;
    CExoString sTokenValue;
    if (g_pVirtualMachine->StackPopInteger(&nCustomTokenNumber) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    if (g_pVirtualMachine->StackPopString(&sTokenValue) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    if (nCustomTokenNumber > CTlkTable::CUSTOMTOKEN_RESERVED_END)
    {
        g_pTlkTable->SetCustomToken(nCustomTokenNumber, sTokenValue);
        CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
        CExoLinkedList<CSWSClient>* pPlayerList =
            g_pAppManager->m_pServerExoApp->GetPlayerList();
        if (pMessage != NULL && pPlayerList != NULL)
        {
            CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
            while (pPosition != NULL)
            {
                CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
                if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
                {
                    pMessage->SendServerToPlayerSetCustomToken(
                        pPlayer->GetPlayerId(), nCustomTokenNumber, sTokenValue);
                }
                pPlayerList->GetNext(pPosition);
            }
        }
    }
    return 0;
}
// PC 0x005D1370
int CSWVirtualMachineCommands::ExecuteCommandSetJournalQuestEntryPicture(
    int nCommand, int nArgs)
{

    CExoString szPlotID;
    CExoString sVariable;
    unsigned long oObject;
    int nPictureIndex;
    int bAllPartyMembers;
    int bAllPlayers;
    CExoLinkedListNode* pPosition;

    if (g_pVirtualMachine->StackPopString(&szPlotID) == 0)
        return ERROR_STACK_POP;
    if (g_pVirtualMachine->StackPopObject(&oObject) == 0)
        return ERROR_STACK_POP;
    if (g_pVirtualMachine->StackPopInteger(&nPictureIndex) == 0)
        return ERROR_STACK_POP;
    if (g_pVirtualMachine->StackPopInteger(&bAllPartyMembers) == 0)
        return ERROR_STACK_POP;
    if (g_pVirtualMachine->StackPopInteger(&bAllPlayers) == 0)
        return ERROR_STACK_POP;

    sVariable.Format("NW_JOURNAL_PICTURE%s", szPlotID);
    if (bAllPlayers == 0)
    {
        CSWSCreature* pCreature =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oObject);
        if (pCreature != NULL)
        {
            CSWSScriptVarTable* pVariables = &pCreature->m_lScriptVars;
            if (pVariables != NULL)
                pVariables->SetInt(sVariable, nPictureIndex, 0);
            if (bAllPartyMembers)
            {
                CSWSFaction* pFaction = pCreature->GetFaction();
                if (pFaction != NULL)
                {
                    unsigned long* pMembers = pFaction->m_lstMembers.element;
                    int nMembers = pFaction->m_lstMembers.num;
                    if (pMembers != NULL)
                    {
                        for (int i = 0; i < nMembers; ++i)
                        {
                            CSWSCreature* pMember = g_pAppManager->GetServerExoApp()
                                ->GetCreatureByGameObjectID(pMembers[i]);
                            if (pMember != NULL && pMember != pCreature)
                            {
                                CSWSScriptVarTable* pVariables = &pMember->m_lScriptVars;
                                if (pVariables != NULL)
                                    pVariables->SetInt(sVariable, nPictureIndex, 0);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        CExoLinkedList<CSWSClient>* pPlayerList =
            g_pAppManager->GetServerExoApp()->GetPlayerList();
        if (pPlayerList == NULL)
            return 1;
        pPosition = pPlayerList->GetHeadPos();
        while (pPosition != NULL)
        {
            CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
            if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
            {
                CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
                if (pCreature != NULL)
                {
                    CSWSScriptVarTable* pVariables = &pCreature->m_lScriptVars;
                    if (pVariables != NULL)
                        pVariables->SetInt(sVariable, nPictureIndex, 0);
                }
            }
            pPlayerList->GetNext(pPosition);
        }
    }
    return 0;
}
// PC 0x005D15F0
int CSWVirtualMachineCommands::ExecuteCommandSetMapPinEnabled(int nCommand,
                                                            int nArgs)
{

    unsigned long oidMapPin;
    int nEnabled;
    if (g_pVirtualMachine->StackPopObject(&oidMapPin) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nEnabled) == 0)
    {
        return ERROR_STACK_POP;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSWaypoint* pWaypoint = pServerExoApp->GetWaypointByGameObjectID(oidMapPin);
    if (pWaypoint != NULL && pWaypoint->m_bHasMapNote != 0)
    {
        pWaypoint->m_bMapNoteEnabled = nEnabled;
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        CSWSMessage* pMessage = pServerExoApp->GetSWSMessage();
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        CExoLinkedList<CSWSClient>* pPlayerList = pServerExoApp->GetPlayerList();
        if (pPlayerList != NULL)
        {
            CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
            while (pPosition != NULL)
            {
                CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
                if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
                    pMessage != NULL)
                {
                    pMessage->SendServerToPlayerMapPinEnabled(
                        pPlayer, oidMapPin, nEnabled);
                }
                pPlayerList->GetNext(pPosition);
            }
        }
    }
    return 0;
}
// PC 0x005D1840
int CSWVirtualMachineCommands::ExecuteCommandGetNearestObject(int nCommand,
                                                            int nArgs)
{
    int nSecondCriteriaType = -1;
    int nSecondCriteriaValue = -1;
    int nThirdCriteriaType = -1;
    int nThirdCriteriaValue = -1;
    unsigned long oidResult = OBJECT_INVALID;
    CExoString sTag;
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    CScriptLocation* pLocation = NULL;
    CExoString sLowerTag;
    int nFirstCriteriaType;
    int nFirstCriteriaValue;
    int nObjectFilter;
    unsigned long oidTarget;
    int nNth;
    Vector vDifference;

    if (nCommand == COMMAND_GET_NEAREST_CREATURE ||
        nCommand == COMMAND_GET_NEAREST_CREATURE_TO_LOCATION)
    {
        if (g_pVirtualMachine->StackPopInteger(&nFirstCriteriaType) == 0 ||
            g_pVirtualMachine->StackPopInteger(&nFirstCriteriaValue) == 0)
            return ERROR_STACK_POP;
    }
    else if (nCommand == COMMAND_GET_NEAREST_OBJECT ||
             nCommand == COMMAND_GET_NEAREST_OBJECT_TO_LOCATION)
    {
        if (g_pVirtualMachine->StackPopInteger(&nObjectFilter) == 0)
            return ERROR_STACK_POP;
    }
    else if (nCommand == COMMAND_GET_NEAREST_OBJECT_BY_TAG)
    {
        if (g_pVirtualMachine->StackPopString(&sTag) == 0)
            return ERROR_STACK_POP;
        sLowerTag = sTag.LowerCase();
    }

    if (nCommand == COMMAND_GET_NEAREST_CREATURE ||
        nCommand == COMMAND_GET_NEAREST_OBJECT ||
        nCommand == COMMAND_GET_NEAREST_OBJECT_BY_TAG)
    {
        if (g_pVirtualMachine->StackPopObject(&oidTarget) == 0)
            return ERROR_STACK_POP;
    }
    else if (nCommand == COMMAND_GET_NEAREST_CREATURE_TO_LOCATION ||
             nCommand == COMMAND_GET_NEAREST_OBJECT_TO_LOCATION)
    {
        if (g_pVirtualMachine->StackPopEngineStructure(
                STRUCTURE_LOCATION, (void**)&pLocation) == 0)
            return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopInteger(&nNth) == 0)
        return ERROR_STACK_POP;
    if (nArgs > 4 &&
        (nCommand == COMMAND_GET_NEAREST_CREATURE ||
         nCommand == COMMAND_GET_NEAREST_CREATURE_TO_LOCATION))
    {
        if (g_pVirtualMachine->StackPopInteger(&nSecondCriteriaType) == 0 ||
            g_pVirtualMachine->StackPopInteger(&nSecondCriteriaValue) == 0 ||
            g_pVirtualMachine->StackPopInteger(&nThirdCriteriaType) == 0 ||
            g_pVirtualMachine->StackPopInteger(&nThirdCriteriaValue) == 0)
            return ERROR_STACK_POP;
    }

    int bSearchReady = 0;
    Vector vTarget;
    CGameObject* pTarget;
    CExoArrayList<unsigned long>* pAreaObjects;
    int nTargetIndex;
    if (nCommand == COMMAND_GET_NEAREST_CREATURE ||
        nCommand == COMMAND_GET_NEAREST_OBJECT ||
        nCommand == COMMAND_GET_NEAREST_OBJECT_BY_TAG)
    {
        if (pObjectArray->GetGameObject(oidTarget, &pTarget) ==
                CGameObjectArray::SUCCESS &&
            pTarget->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
        {
            CSWSObject* pObject = (CSWSObject*)pTarget;
            vTarget = pObject->m_vPosition;
            CSWSArea* pArea = pObject->GetArea();
            pAreaObjects = &pArea->m_lstObjectIds;
            if (pArea != NULL &&
                pArea->GetFirstObjectIndiceByX(&nTargetIndex, vTarget.m_fX) != 0)
            {
                for (; nTargetIndex < pAreaObjects->num; ++nTargetIndex)
                {
                    if ((*pAreaObjects)[nTargetIndex] == oidTarget)
                        break;
                }
                if (nTargetIndex < pAreaObjects->num &&
                    (*pAreaObjects)[nTargetIndex] == oidTarget)
                    bSearchReady = 1;
            }
        }
    }
    else
    {
        vTarget = pLocation->m_vPosition;
        CSWSCreature* pPlayer =
            g_pAppManager->GetServerExoApp()->GetPlayerCreature();
        if (pPlayer != NULL)
        {
            CSWSArea* pArea = pPlayer->GetArea();
            if (pArea != NULL)
            {
                pAreaObjects = &pArea->m_lstObjectIds;
                if (pArea->GetFirstObjectIndiceByX(&nTargetIndex, vTarget.m_fX) != 0)
                    bSearchReady = 1;
            }
        }
    }
    if (bSearchReady)
    {
        CExoArrayList<unsigned long> lstObjects;
        CExoArrayList<float> lstDistances;
        int nOffset = 1;
        int bDoneRight = 0;
        int bDoneLeft = 0;
        while (!(bDoneRight && bDoneLeft))
        {
            if (nTargetIndex + nOffset < 0)
            {
                bDoneLeft = 1;
                nOffset = abs(nOffset);
                continue;
            }
            if (nTargetIndex + nOffset >= pAreaObjects->num)
            {
                bDoneRight = 1;
                nOffset = -abs(nOffset);
                continue;
            }
            unsigned long oidObject = (*pAreaObjects)[nTargetIndex + nOffset];
            if (!lstObjects.Contains(oidObject))
            {
                CGameObject* pGameObject;
                if (pObjectArray->GetGameObject(oidObject, &pGameObject) ==
                    CGameObjectArray::SUCCESS)
                {
                    int bMatch = 0;
                    switch (nCommand)
                    {
                    case COMMAND_GET_NEAREST_CREATURE:
                    case COMMAND_GET_NEAREST_CREATURE_TO_LOCATION:
                        if (pGameObject->AsSWSCreature() != NULL)
                        {
                            CSWSCreature* pCreature = pGameObject->AsSWSCreature();
                            if (pCreature != NULL)
                            {
                                int nType = nFirstCriteriaType;
                                int nValue = nFirstCriteriaValue;
                                for (int nCriteria = 0; nCriteria < 3; ++nCriteria)
                                {
                                    if (nCriteria == 1)
                                    {
                                        nType = nSecondCriteriaType;
                                        nValue = nSecondCriteriaValue;
                                    }
                                    else if (nCriteria == 2)
                                    {
                                        nType = nThirdCriteriaType;
                                        nValue = nThirdCriteriaValue;
                                    }
                                    if (nType == -1 || nValue == -1)
                                        continue;
                                    switch (nType)
                                    {
                                    case CREATURE_TYPE_RACIAL_TYPE:

                                        if (nFirstCriteriaValue == g_pRules->m_nNumRaces)
                                            bMatch = 1;
                                        else if (nFirstCriteriaValue == g_pRules->m_nNumRaces + 1)
                                            bMatch = 0;
                                        else
                                            bMatch = pCreature->m_pStats->m_nRace == nValue;
                                        break;
                                    case CREATURE_TYPE_PLAYER_CHAR:
                                        bMatch = pCreature->m_pStats->GetIsPC() == nValue;
                                        break;
                                    case CREATURE_TYPE_CLASS:
                                        {
                                            CSWSCreatureStats* pStats = pCreature->m_pStats;
                                            int nClasses = pStats->GetNumClasses();
                                            bMatch = pStats->GetClass(0) == nValue ||
                                                (nClasses > 1 && pStats->GetClass(1) == nValue) ||
                                                (nClasses > 2 && pStats->GetClass(2) == nValue);
                                        }
                                        break;
                                    case CREATURE_TYPE_REPUTATION:
                                        if (nValue == REPUTATION_TYPE_FRIEND)
                                            bMatch = pCreature->AsSWSObject()->GetReputation(
                                                pTarget->GetId(), 0) >= CSWSObject::MIN_FRIENDLY_REPUTATION;
                                        else if (nValue == REPUTATION_TYPE_ENEMY)
                                            bMatch = pCreature->AsSWSObject()->GetReputation(
                                                pTarget->GetId(), 0) <= CSWSObject::MAX_HOSTILE_REPUTATION;
                                        else if (nValue == REPUTATION_TYPE_NEUTRAL)
                                        {
                                            int nReputation = pCreature->AsSWSObject()->GetReputation(
                                                pTarget->GetId(), 0);
                                            bMatch = nReputation > CSWSObject::MAX_HOSTILE_REPUTATION &&
                                                nReputation < CSWSObject::MIN_FRIENDLY_REPUTATION;
                                        }
                                        break;
                                    case CREATURE_TYPE_IS_ALIVE:
                                        break;
                                    case CREATURE_TYPE_HAS_SPELL_EFFECT:
                                        bMatch = pCreature->HasSpellEffectApplied(nValue);
                                        break;
                                    case CREATURE_TYPE_DOES_NOT_HAVE_SPELL_EFFECT:
                                        bMatch = !pCreature->HasSpellEffectApplied(nValue);
                                        break;
                                    case CREATURE_TYPE_PERCEPTION:
                                        if (pTarget->AsSWSCreature() != NULL)
                                        {
                                            unsigned char nSeen = PERCEPTION_REQUIREMENT_ANY;
                                            unsigned char nHeard = PERCEPTION_REQUIREMENT_ANY;
                                            switch (nValue)
                                            {
                                            case PERCEPTION_SEEN_AND_HEARD: nHeard = 1; nSeen = 1; break;
                                            case PERCEPTION_NOT_SEEN_AND_NOT_HEARD: nHeard = 0; nSeen = 0; break;
                                            case PERCEPTION_HEARD_AND_NOT_SEEN: nSeen = 0; nHeard = 1; break;
                                            case PERCEPTION_SEEN_AND_NOT_HEARD: nSeen = 1; nHeard = 0; break;
                                            case PERCEPTION_NOT_HEARD: nHeard = 0; break;
                                            case PERCEPTION_HEARD: nHeard = 1; break;
                                            case PERCEPTION_NOT_SEEN: nSeen = 0; break;
                                            case PERCEPTION_SEEN: nSeen = 1; break;
                                            }
                                            CSWVisibilityNode* pNode = pTarget->AsSWSCreature()->
                                                GetVisibleListElement(pCreature->GetId());
                                            int bHeard = 0;
                                            int bSeen = 0;
                                            if (pNode != NULL)
                                            {
                                                bSeen = pNode->m_bSeen;
                                                bHeard = pNode->m_bHeard;
                                            }
                                            bMatch = (nSeen == PERCEPTION_REQUIREMENT_ANY || bSeen == nSeen) &&
                                                (nHeard == PERCEPTION_REQUIREMENT_ANY || bHeard == nHeard);
                                        }
                                        break;
                                    default:
                                        bMatch = 0;
                                        break;
                                    }
                                    if (!bMatch)
                                        break;
                                }
                                if (pCreature->GetDead() == 1 ||
                                    pCreature->GetIsPCDying() == 1 ||
                                    pCreature->GetDeadTemp() == 1)
                                    bMatch = 0;
                            }
                        }
                        break;
                    case COMMAND_GET_NEAREST_OBJECT:
                    case COMMAND_GET_NEAREST_OBJECT_TO_LOCATION:
                        {
                            int nObjectType = pGameObject->m_nObjectType;
                            bMatch = ((nObjectFilter & OBJECT_TYPE_CREATURE) && nObjectType == SW_OBJECT_TYPE_CREATURE) ||
                                ((nObjectFilter & OBJECT_TYPE_ITEM) && nObjectType == SW_OBJECT_TYPE_ITEM) ||
                                ((nObjectFilter & OBJECT_TYPE_TRIGGER) && nObjectType == SW_OBJECT_TYPE_TRIGGER) ||
                                ((nObjectFilter & OBJECT_TYPE_DOOR) && nObjectType == SW_OBJECT_TYPE_DOOR) ||
                                ((nObjectFilter & OBJECT_TYPE_AREA_OF_EFFECT) && nObjectType == SW_OBJECT_TYPE_AREA_OF_EFFECT) ||
                                ((nObjectFilter & OBJECT_TYPE_WAYPOINT) && nObjectType == SW_OBJECT_TYPE_WAYPOINT) ||
                                ((nObjectFilter & OBJECT_TYPE_PLACEABLE) && nObjectType == SW_OBJECT_TYPE_PLACEABLE) ||
                                ((nObjectFilter & OBJECT_TYPE_STORE) == OBJECT_TYPE_STORE && nObjectType == SW_OBJECT_TYPE_STORE) ||
                                ((nObjectFilter & OBJECT_TYPE_ENCOUNTER) && nObjectType == SW_OBJECT_TYPE_ENCOUNTER) ||
                                ((nObjectFilter & OBJECT_TYPE_SOUND) && nObjectType == SW_OBJECT_TYPE_SOUND) ||
                                (nObjectFilter & OBJECT_TYPE_ALL) == OBJECT_TYPE_ALL;
                        }
                        break;
                    case COMMAND_GET_NEAREST_OBJECT_BY_TAG:
                        bMatch = ((CSWSObject*)pGameObject)->m_sTag == sLowerTag;
                        break;
                    }
                    if (bMatch)
                    {
                        vDifference = ((CSWSObject*)pGameObject)->m_vPosition - vTarget;
                        float fDistance = vDifference.m_fX * vDifference.m_fX + vDifference.m_fY * vDifference.m_fY +
                            vDifference.m_fZ * vDifference.m_fZ;
                        float fX = ((CSWSObject*)pGameObject)->m_vPosition.m_fX - vTarget.m_fX;
                        if (lstDistances.num >= nNth && fX * fX > lstDistances[nNth - 1])
                        {
                            if (nOffset > 0)
                            {
                                bDoneRight = 1;
                                nOffset = -abs(nOffset);
                            }
                            else
                            {
                                bDoneLeft = 1;
                                nOffset = abs(nOffset);
                            }
                        }
                        int nIndex;
                        for (nIndex = 0; nIndex < lstDistances.num &&
                             fDistance >= lstDistances[nIndex]; ++nIndex)
                        {
                        }
                        lstDistances.Insert(fDistance, nIndex);
                        lstObjects.Insert(oidObject, nIndex);
                    }
                }
            }
            if (abs(nOffset) % 2 == (nOffset > 0) && !bDoneRight && !bDoneLeft)
                nOffset = -nOffset;
            else
                nOffset += nOffset / abs(nOffset);
        }
        if (nNth <= lstObjects.num)
            oidResult = lstObjects[nNth - 1];
        else
            oidResult = OBJECT_INVALID;
    }
    CSWSCreature* pResult = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidResult);

    if (pResult != NULL && nCommand != COMMAND_GET_NEAREST_OBJECT_BY_TAG &&
        !pResult->GetDead())
        pResult->GetDeadTemp();
    if (g_pVirtualMachine->StackPushObject(oidResult) == 0)
        return ERROR_STACK_PUSH;
    if (pLocation != NULL)
        delete pLocation;
    return 0;
}
// PC 0x005D23C0
int CSWVirtualMachineCommands::ExecuteCommandGetMineInfo(int nCommand, int nArgs)
{

    unsigned long oidCreature;
    unsigned long oidTrap;
    if (g_pVirtualMachine->StackPopObject(&oidTrap) == 0)
        return ERROR_STACK_POP;
    CGameObject* pGameObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTrap);

    if (nCommand == COMMAND_GET_TRAP_DISARMABLE)
    {
        int bDisarmable = 0;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
                bDisarmable = pGameObject->AsSWSTrigger()->m_bTrapDisarmable;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                bDisarmable = pGameObject->AsSWSPlaceable()->m_bTrapDisarmable;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                bDisarmable = pGameObject->AsSWSDoor()->m_bTrapDisarmable;
        }
        if (g_pVirtualMachine->StackPushInteger(bDisarmable) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_TRAP_DETECTABLE)
    {
        int bDetectable = 0;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
                bDetectable = pGameObject->AsSWSTrigger()->m_bTrapDetectable;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                bDetectable = pGameObject->AsSWSPlaceable()->m_bTrapDetectable;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                bDetectable = pGameObject->AsSWSDoor()->m_bTrapDetectable;
        }
        if (g_pVirtualMachine->StackPushInteger(bDetectable) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_TRAP_DETECTED_BY)
    {
        int bDetected = 0;
        if (g_pVirtualMachine->StackPopObject(&oidCreature) == 0)
            return ERROR_STACK_POP;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
            {
                CSWSTrigger* pTrap = pGameObject->AsSWSTrigger();
                bDetected = pTrap->m_lstTrapTargets.Contains(oidCreature);
            }
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
            {
                CSWSPlaceable* pTrap = pGameObject->AsSWSPlaceable();
                bDetected = pTrap->m_lstTrapDetectedBy.Contains(oidCreature);
            }
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
            {
                CSWSDoor* pTrap = pGameObject->AsSWSDoor();
                bDetected = pTrap->m_lstTrapDetectedBy.Contains(oidCreature);
            }
        }
        if (g_pVirtualMachine->StackPushInteger(bDetected) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_TRAP_FLAGGED)
    {
        int bFlagged = 0;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
                bFlagged = pGameObject->AsSWSTrigger()->m_bTrapFlagged;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                bFlagged = (unsigned char)pGameObject->AsSWSPlaceable()->m_bTrapFlagged;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                bFlagged = (unsigned char)pGameObject->AsSWSDoor()->m_bTrapFlagged;
        }
        if (g_pVirtualMachine->StackPushInteger(bFlagged) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_TRAP_BASE_TYPE)
    {
        int nTrapType = INVALID_TRAP_BASE_TYPE;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
                nTrapType = pGameObject->AsSWSTrigger()->m_nTrapType;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                nTrapType = pGameObject->AsSWSPlaceable()->m_nTrapType;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                nTrapType = pGameObject->AsSWSDoor()->m_nTrapType;
        }
        if (g_pVirtualMachine->StackPushInteger(nTrapType) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_TRAP_ONE_SHOT)
    {
        int bOneShot = 0;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
                bOneShot = pGameObject->AsSWSTrigger()->m_bTrapOneShot;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                bOneShot = pGameObject->AsSWSPlaceable()->m_bTrapOneShot;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                bOneShot = pGameObject->AsSWSDoor()->m_bTrapOneShot;
        }
        if (g_pVirtualMachine->StackPushInteger(bOneShot) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_TRAP_CREATOR)
    {
        unsigned long oidCreator = OBJECT_INVALID;
        if (pGameObject != NULL && pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
            oidCreator = pGameObject->AsSWSTrigger()->m_oidCreator;
        if (g_pVirtualMachine->StackPushObject(oidCreator) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_TRAP_KEY_TAG)
    {
        CExoString* psKeyName = NULL;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
                psKeyName = &pGameObject->AsSWSTrigger()->m_sKeyName;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                psKeyName = &pGameObject->AsSWSPlaceable()->m_sKeyName;
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                psKeyName = &pGameObject->AsSWSDoor()->m_sKeyName;
        }
        if (psKeyName == NULL)
        {
            if (g_pVirtualMachine->StackPushString("") == 0)
                return ERROR_STACK_PUSH;
        }
        else
        {
            if (g_pVirtualMachine->StackPushString(*psKeyName) == 0)
                return ERROR_STACK_PUSH;
        }
    }
    else if (nCommand == COMMAND_GET_TRAP_DISARM_DC)
    {
        int nDisarmDC = 0;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
                nDisarmDC = pGameObject->AsSWSTrigger()->GetDisarmDC();
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                nDisarmDC = pGameObject->AsSWSPlaceable()->GetDisarmDC();
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                nDisarmDC = pGameObject->AsSWSDoor()->GetDisarmDC();
        }
        if (g_pVirtualMachine->StackPushInteger(nDisarmDC) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_GET_TRAP_DETECT_DC)
    {
        int nDetectDC = 0;
        if (pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
                nDetectDC = pGameObject->AsSWSTrigger()->GetDetectDC();
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                nDetectDC = pGameObject->AsSWSPlaceable()->GetDetectDC();
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
                nDetectDC = pGameObject->AsSWSDoor()->GetDetectDC();
        }
        if (g_pVirtualMachine->StackPushInteger(nDetectDC) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_SET_TRAP_DETECTED_BY)
    {
        int bDetected = 0;
        unsigned long oidDetector;
        if (g_pVirtualMachine->StackPopObject(&oidDetector) == 0)
            return ERROR_STACK_POP;
        if (g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidDetector) != NULL && pGameObject != NULL)
        {
            if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
            {
                CSWSTrigger* pTrap = pGameObject->AsSWSTrigger();
                pTrap->m_lstTrapTargets.AddUnique(oidDetector);
                bDetected = 1;
            }
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
            {
                CSWSPlaceable* pTrap = pGameObject->AsSWSPlaceable();
                pTrap->m_lstTrapDetectedBy.AddUnique(oidDetector);
                bDetected = 1;
            }
            else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
            {
                CSWSDoor* pTrap = pGameObject->AsSWSDoor();
                pTrap->m_lstTrapDetectedBy.AddUnique(oidDetector);
                bDetected = 1;
            }
        }
        if (g_pVirtualMachine->StackPushInteger(bDetected) == 0)
            return ERROR_STACK_PUSH;
    }
    else if (nCommand == COMMAND_SET_TRAP_DISABLED && pGameObject != NULL)
    {
        if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
        {
            if (pGameObject->AsSWSTrigger()->GetTrapFlag() == 1)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_oidObject, pGameObject->GetId(),
                    CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 1, m_oidObject, pGameObject->GetId(),
                    CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
            }
        }
        else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
        {
            if (pGameObject->AsSWSPlaceable()->m_nTrapFlag == 1)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_oidObject, pGameObject->GetId(),
                    CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }
        else if (pGameObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
        {
            if (pGameObject->AsSWSDoor()->m_bTrapFlag == 1)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_oidObject, pGameObject->GetId(),
                    CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }
    }
    return 0;
}
// PC 0x005D32A0
int CSWVirtualMachineCommands::ExecuteCommandGetMatchedString(int nCommand,
                                                             int nArgs)
{

    CExoString sMatchedString;
    int nIndex;
    CSWSObject* pObject;
    CServerExoApp* pServerExoApp;

    if (g_pVirtualMachine->StackPopInteger(&nIndex) == 0)
    {
        return ERROR_STACK_POP;
    }
    pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp->GetObjectArray()->GetGameObject(
            m_oidObject, (CGameObject**)&pObject) == CGameObjectArray::SUCCESS &&
        pObject != NULL)
    {
        CExoArrayList<CExoString*> lstMatchedStrings = pObject->m_lstMatchedStrings;
        if (nIndex >= 0 && nIndex < lstMatchedStrings.num)
        {
            sMatchedString = *lstMatchedStrings[nIndex];
        }
    }
    if (g_pVirtualMachine->StackPushString(sMatchedString) == 0)
    {
        return ERROR_STACK_PUSH;
    }
    return 0;
}
// PC 0x005D3580
void CSWVirtualMachineCommands::InitializeCommands()
{
    m_pCommands = new CommandFunction[COMMAND_COUNT];
    for (int nCommand = 0; nCommand < COMMAND_COUNT; nCommand++)
    {
        m_pCommands[nCommand] = NULL;
    }

    m_pCommands[COMMAND_ABS] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_ACOS] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_ACTION_ATTACK] = &CSWVirtualMachineCommands::ExecuteCommandAttack;
    m_pCommands[COMMAND_ACTION_BARK_STRING] = &CSWVirtualMachineCommands::ExecuteCommandActionBarkString;
    m_pCommands[COMMAND_ACTION_CAST_FAKE_SPELL_AT_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandCastSpell;
    m_pCommands[COMMAND_ACTION_CAST_FAKE_SPELL_AT_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandCastSpell;
    m_pCommands[COMMAND_ACTION_CAST_SPELL_AT_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandCastSpell;
    m_pCommands[COMMAND_ACTION_CLOSE_DOOR] = &CSWVirtualMachineCommands::ExecuteCommandCloseDoor;
    m_pCommands[COMMAND_ACTION_DO_COMMAND] = &CSWVirtualMachineCommands::ExecuteCommandActionDoCommand;
    m_pCommands[COMMAND_ACTION_EQUIP_ITEM] = &CSWVirtualMachineCommands::ExecuteCommandEquipItem;
    m_pCommands[COMMAND_ACTION_EQUIP_MOST_DAMAGING_MELEE] = &CSWVirtualMachineCommands::ExecuteCommandActionEquipMostDamagingMelee;
    m_pCommands[COMMAND_ACTION_EQUIP_MOST_DAMAGING_RANGED] = &CSWVirtualMachineCommands::ExecuteCommandActionEquipMostDamagingRanged;
    m_pCommands[COMMAND_ACTION_EQUIP_MOST_EFFECTIVE_ARMOR] = &CSWVirtualMachineCommands::ExecuteCommandActionEquipMostEffectiveArmor;
    m_pCommands[COMMAND_ACTION_FOLLOW_LEADER] = &CSWVirtualMachineCommands::ExecuteCommandActionFollowLeader;
    m_pCommands[COMMAND_ACTION_FOLLOW_OWNER] = &CSWVirtualMachineCommands::ExecuteCommandActionFollowOwner;
    m_pCommands[COMMAND_ACTION_FORCE_FOLLOW_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandActionFollowObject;
    m_pCommands[COMMAND_ACTION_FORCE_MOVE_TO_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandMoveToPoint;
    m_pCommands[COMMAND_ACTION_FORCE_MOVE_TO_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandMoveToObject;
    m_pCommands[COMMAND_ACTION_GIVE_ITEM] = &CSWVirtualMachineCommands::ExecuteCommandActionExchangeItem;
    m_pCommands[COMMAND_ACTION_INTERACT_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandActionInteractObject;
    m_pCommands[COMMAND_ACTION_JUMP_TO_POINT] = &CSWVirtualMachineCommands::ExecuteCommandActionJumpToPoint;
    m_pCommands[COMMAND_ACTION_JUMP_TO_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandActionJumpToObject;
    m_pCommands[COMMAND_ACTION_LOCK_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandActionLockActions;
    m_pCommands[COMMAND_ACTION_MOVE_AWAY_FROM_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandActionMoveAwayFromLocation;
    m_pCommands[COMMAND_ACTION_MOVE_AWAY_FROM_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandMoveAwayFromObject;
    m_pCommands[COMMAND_ACTION_MOVE_TO_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandMoveToObject;
    m_pCommands[COMMAND_ACTION_MOVE_TO_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandMoveToPoint;
    m_pCommands[COMMAND_ACTION_OPEN_DOOR] = &CSWVirtualMachineCommands::ExecuteCommandOpenDoor;
    m_pCommands[COMMAND_ACTION_PAUSE_CONVERSATION] = &CSWVirtualMachineCommands::ExecuteCommandActionPauseConversation;
    m_pCommands[COMMAND_ACTION_PICK_UP_ITEM] = &CSWVirtualMachineCommands::ExecuteCommandPickUpItem;
    m_pCommands[COMMAND_ACTION_PLAY_ANIMATION] = &CSWVirtualMachineCommands::ExecuteCommandPlayAnimation;
    m_pCommands[COMMAND_ACTION_PUT_DOWN_ITEM] = &CSWVirtualMachineCommands::ExecuteCommandPutDownItem;
    m_pCommands[COMMAND_ACTION_RANDOM_WALK] = &CSWVirtualMachineCommands::ExecuteCommandActionRandomWalk;
    m_pCommands[COMMAND_ACTION_RESUME_CONVERSATION] = &CSWVirtualMachineCommands::ExecuteCommandActionResumeConversation;
    m_pCommands[COMMAND_ACTION_SPEAK_STRING] = &CSWVirtualMachineCommands::ExecuteCommandSpeakString;
    m_pCommands[COMMAND_ACTION_SPEAK_STRING_BY_STR_REF] = &CSWVirtualMachineCommands::ExecuteCommandSpeakStrRef;
    m_pCommands[COMMAND_ACTION_START_CONVERSATION] = &CSWVirtualMachineCommands::ExecuteCommandActionStartConversation;
    m_pCommands[COMMAND_ACTION_SURRENDER_TO_ENEMIES] = &CSWVirtualMachineCommands::ExecuteCommandSurrenderToEnemies;
    m_pCommands[COMMAND_ACTION_SWITCH_WEAPONS] = &CSWVirtualMachineCommands::ExecuteCommandActionSwitchWeapons;
    m_pCommands[COMMAND_ACTION_TAKE_ITEM] = &CSWVirtualMachineCommands::ExecuteCommandActionExchangeItem;
    m_pCommands[COMMAND_ACTION_UNEQUIP_ITEM] = &CSWVirtualMachineCommands::ExecuteCommandUnequipItem;
    m_pCommands[COMMAND_ACTION_UNLOCK_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandActionLockActions;
    m_pCommands[COMMAND_ACTION_USE_FEAT] = &CSWVirtualMachineCommands::ExecuteCommandActionUseFeat;
    m_pCommands[COMMAND_ACTION_USE_SKILL] = &CSWVirtualMachineCommands::ExecuteCommandActionUseSkill;
    m_pCommands[COMMAND_ACTION_USE_TALENT_AT_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandActionUseTalent;
    m_pCommands[COMMAND_ACTION_USE_TALENT_ON_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandActionUseTalent;
    m_pCommands[COMMAND_ADD_AVAILABLE_NPC_BY_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandAddAvailableNPCByObject;
    m_pCommands[COMMAND_ADD_AVAILABLE_NPC_BY_TEMPLATE] = &CSWVirtualMachineCommands::ExecuteCommandAddAvailableNPCByTemplate;
    m_pCommands[COMMAND_ADD_AVAILABLE_PUP_BY_TEMPLATE] = &CSWVirtualMachineCommands::ExecuteCommandAddAvailablePUPByTemplate;
    m_pCommands[COMMAND_ADD_AVAILABLE_PUP_BY_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandAddAvailablePUPByObject;
    m_pCommands[COMMAND_ADD_JOURNAL_QUEST_ENTRY] = &CSWVirtualMachineCommands::ExecuteCommandAddJournalQuestEntry;
    m_pCommands[COMMAND_ADD_JOURNAL_WORLD_ENTRY] = &CSWVirtualMachineCommands::ExecuteCommandAddJournalWorldEntry;
    m_pCommands[COMMAND_ADD_JOURNAL_WORLD_ENTRY_STRREF] = &CSWVirtualMachineCommands::ExecuteCommandAddJournalWorldEntryStrref;
    m_pCommands[COMMAND_ADD_MULTI_CLASS] = &CSWVirtualMachineCommands::ExecuteCommandAddMultiClass;
    m_pCommands[COMMAND_ADD_PARTY_MEMBER] = &CSWVirtualMachineCommands::ExecuteCommandAddPartyMember;
    m_pCommands[COMMAND_ADD_PARTY_PUPPET] = &CSWVirtualMachineCommands::ExecuteCommandAddPartyPuppet;
    m_pCommands[COMMAND_ADD_TO_PARTY] = &CSWVirtualMachineCommands::ExecuteCommandAddToParty;
    m_pCommands[COMMAND_ADJUST_ALIGNMENT] = &CSWVirtualMachineCommands::ExecuteCommandAdjustAlignment;
    m_pCommands[COMMAND_ADJUST_REPUTATION] = &CSWVirtualMachineCommands::ExecuteCommandAdjustReputation;
    m_pCommands[COMMAND_AMBIENT_SOUND_CHANGE_DAY] = &CSWVirtualMachineCommands::ExecuteCommandAmbientSound;
    m_pCommands[COMMAND_AMBIENT_SOUND_CHANGE_NIGHT] = &CSWVirtualMachineCommands::ExecuteCommandAmbientSound;
    m_pCommands[COMMAND_AMBIENT_SOUND_PLAY] = &CSWVirtualMachineCommands::ExecuteCommandAmbientSound;
    m_pCommands[COMMAND_AMBIENT_SOUND_SET_DAY_VOLUME] = &CSWVirtualMachineCommands::ExecuteCommandAmbientSound;
    m_pCommands[COMMAND_AMBIENT_SOUND_SET_NIGHT_VOLUME] = &CSWVirtualMachineCommands::ExecuteCommandAmbientSound;
    m_pCommands[COMMAND_AMBIENT_SOUND_STOP] = &CSWVirtualMachineCommands::ExecuteCommandAmbientSound;
    m_pCommands[COMMAND_ANGLE_TO_VECTOR] = &CSWVirtualMachineCommands::ExecuteCommandVectorConversions;
    m_pCommands[COMMAND_APPLY_EFFECT_AT_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandApplyEffectAtPoint;
    m_pCommands[COMMAND_APPLY_EFFECT_TO_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandApplyEffectOnObject;
    m_pCommands[COMMAND_ASIN] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_ASSIGN_COMMAND] = &CSWVirtualMachineCommands::ExecuteCommandAssignCommand;
    m_pCommands[COMMAND_ASSIGN_PUP] = &CSWVirtualMachineCommands::ExecuteCommandAssignPUP;
    m_pCommands[COMMAND_EFFECT_ASSURED_HIT] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_ASSURED_DEFLECTION] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_ATAN] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_AWARD_STEALTH_XP] = &CSWVirtualMachineCommands::ExecuteCommandAwardStealthXP;
    m_pCommands[COMMAND_BARK_STRING] = &CSWVirtualMachineCommands::ExecuteCommandBarkString;
    m_pCommands[COMMAND_BEGIN_CONVERSATION] = &CSWVirtualMachineCommands::ExecuteCommandBeginConversation;
    m_pCommands[COMMAND_CANCEL_COMBAT] = &CSWVirtualMachineCommands::ExecuteCommandCancelCombat;
    m_pCommands[COMMAND_CANCEL_POST_DIALOG_CHARACTER_SWITCH] = &CSWVirtualMachineCommands::ExecuteCommandCancelPostDialogCharacterSwitch;
    m_pCommands[COMMAND_ACTION_CAST_SPELL_AT_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandCastSpell;
    m_pCommands[COMMAND_CHANGE_FACTION] = &CSWVirtualMachineCommands::ExecuteCommandChangeFaction;
    m_pCommands[COMMAND_CHANGE_FACTION_BY_FACTION] = &CSWVirtualMachineCommands::ExecuteCommandChangeByFaction;
    m_pCommands[COMMAND_CHANGE_ITEM_COST] = &CSWVirtualMachineCommands::ExecuteCommandChangeItemCost;
    m_pCommands[COMMAND_CHANGE_TO_STANDARD_FACTION] = &CSWVirtualMachineCommands::ExecuteCommandChangeFaction;
    m_pCommands[COMMAND_CLEAR_ALL_ACTIONS] = &CSWVirtualMachineCommands::ExecuteCommandClearAllActions;
    m_pCommands[COMMAND_CLEAR_ALL_EFFECTS] = &CSWVirtualMachineCommands::ExecuteCommandClearAllEffects;
    m_pCommands[COMMAND_COS] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_CREATE_ITEM_ON_FLOOR] = &CSWVirtualMachineCommands::ExecuteCommandCreateItemOnFloor;
    m_pCommands[COMMAND_CREATE_ITEM_ON_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandCreateItemOnObject;
    m_pCommands[COMMAND_CREATE_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandCreateObject;
    m_pCommands[COMMAND_CUTSCENE_ATTACK] = &CSWVirtualMachineCommands::ExecuteCommandCutsceneAttack;
    m_pCommands[COMMAND_CUTSCENE_MOVE] = &CSWVirtualMachineCommands::ExecuteCommandCutsceneMove;
    m_pCommands[COMMAND_D10] = &CSWVirtualMachineCommands::ExecuteCommandRollDice;
    m_pCommands[COMMAND_D100] = &CSWVirtualMachineCommands::ExecuteCommandRollDice;
    m_pCommands[COMMAND_D12] = &CSWVirtualMachineCommands::ExecuteCommandRollDice;
    m_pCommands[COMMAND_D2] = &CSWVirtualMachineCommands::ExecuteCommandRollDice;
    m_pCommands[COMMAND_D20] = &CSWVirtualMachineCommands::ExecuteCommandRollDice;
    m_pCommands[COMMAND_D3] = &CSWVirtualMachineCommands::ExecuteCommandRollDice;
    m_pCommands[COMMAND_D4] = &CSWVirtualMachineCommands::ExecuteCommandRollDice;
    m_pCommands[COMMAND_D6] = &CSWVirtualMachineCommands::ExecuteCommandRollDice;
    m_pCommands[COMMAND_D8] = &CSWVirtualMachineCommands::ExecuteCommandRollDice;
    m_pCommands[COMMAND_DELAY_COMMAND] = &CSWVirtualMachineCommands::ExecuteCommandDelayCommand;
    m_pCommands[COMMAND_DELETE_JOURNAL_WORLD_ALL_ENTRIES] = &CSWVirtualMachineCommands::ExecuteCommandDeleteJournalWorldAllEntries;
    m_pCommands[COMMAND_DELETE_JOURNAL_WORLD_ENTRY] = &CSWVirtualMachineCommands::ExecuteCommandDeleteJournalWorldEntry;
    m_pCommands[COMMAND_DELETE_JOURNAL_WORLD_ENTRY_STRREF] = &CSWVirtualMachineCommands::ExecuteCommandDeleteJournalWorldEntryStrref;
    m_pCommands[COMMAND_DESTROY_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandDestroyObject;
    m_pCommands[COMMAND_DETONATE_MINE] = &CSWVirtualMachineCommands::ExecuteCommandDetonateMine;
    m_pCommands[COMMAND_DISABLE_MAP] = &CSWVirtualMachineCommands::ExecuteCommandDisableMap;
    m_pCommands[COMMAND_DISABLE_HEALTH_REGEN] = &CSWVirtualMachineCommands::ExecuteCommandDisableHealthRegen;
    m_pCommands[COMMAND_DISABLE_VIDEO_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandDisableVideoEffect;
    m_pCommands[COMMAND_DISPLAY_FEED_BACK_TEXT] = &CSWVirtualMachineCommands::ExecuteCommandDisplayFeedBackText;
    m_pCommands[COMMAND_DISPLAY_MESSAGE_BOX] = &CSWVirtualMachineCommands::ExecuteCommandDisplayMessageBox;
    m_pCommands[COMMAND_DISPLAY_DATAPAD] = &CSWVirtualMachineCommands::ExecuteCommandDisplayDatapad;
    m_pCommands[COMMAND_DO_DOOR_ACTION] = &CSWVirtualMachineCommands::ExecuteCommandDoDoorAction;
    m_pCommands[COMMAND_DO_PLACEABLE_OBJECT_ACTION] = &CSWVirtualMachineCommands::ExecuteCommandDoDoorAction;
    m_pCommands[COMMAND_DO_SINGLE_PLAYER_AUTO_SAVE] = &CSWVirtualMachineCommands::ExecuteCommandDoSinglePlayerAutoSave;
    m_pCommands[COMMAND_DUPLICATE_HEAD_APPEARANCE] = &CSWVirtualMachineCommands::ExecuteCommandDuplicateHeadAppearance;
    m_pCommands[COMMAND_DECREMENT_GLOBAL_NUMBER] = &CSWVirtualMachineCommands::ExecuteCommandModifyGlobal;
    m_pCommands[COMMAND_EFFECT_CUTSCENE_HORRIFIED] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_CUTSCENE_PARALYZE] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_CUTSCENE_STUNNED] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_DROID_STUN] = &CSWVirtualMachineCommands::ExecuteCommandEffectDroidStun;
    m_pCommands[COMMAND_EFFECT_FORCE_JUMP] = &CSWVirtualMachineCommands::ExecuteCommandEffectForceJump;
    m_pCommands[COMMAND_EFFECT_FORCE_PUSHED] = &CSWVirtualMachineCommands::ExecuteCommandEffectForcePushed;
    m_pCommands[COMMAND_EFFECT_FORCE_PUSH_TARGETED] = &CSWVirtualMachineCommands::ExecuteCommandEffectForcePushTargeted;
    m_pCommands[COMMAND_EFFECT_WHIRL_WIND] = &CSWVirtualMachineCommands::ExecuteCommandEffectWhirlwind;
    m_pCommands[COMMAND_EFFECT_BLASTER_DEFLECTION_INCREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectBlasterDeflectionIncrease;
    m_pCommands[COMMAND_EFFECT_BLASTER_DEFLECTION_DECREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectBlasterDeflectionDecrease;
    m_pCommands[COMMAND_EFFECT_HORRIFIED] = &CSWVirtualMachineCommands::ExecuteCommandEffectHorrified;
    m_pCommands[COMMAND_EFFECT_DAMAGE_FORCE_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandEffectDamageForcePoints;
    m_pCommands[COMMAND_EFFECT_HEAL_FORCE_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandEffectHealForcePoints;
    m_pCommands[COMMAND_EFFECT_TEMPORARY_FORCE_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandEffectTemporaryForcePoints;
    m_pCommands[COMMAND_EFFECT_CHOKE] = &CSWVirtualMachineCommands::ExecuteCommandEffectChoke;
    m_pCommands[COMMAND_EFFECT_BODY_FUEL] = &CSWVirtualMachineCommands::ExecuteCommandEffectBodyFuel;
    m_pCommands[COMMAND_EFFECT_PSYCHIC_STATIC] = &CSWVirtualMachineCommands::ExecuteCommandEffectPsychicStatic;
    m_pCommands[COMMAND_EFFECT_LIGHTSABER_THROW] = &CSWVirtualMachineCommands::ExecuteCommandEffectLightsaberThrow;
    m_pCommands[COMMAND_EFFECT_ABILITY_DECREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectAbilityDecrease;
    m_pCommands[COMMAND_EFFECT_ABILITY_INCREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectAbilityIncrease;
    m_pCommands[COMMAND_EFFECT_AC_DECREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectACDecrease;
    m_pCommands[COMMAND_EFFECT_AC_INCREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectACIncrease;
    m_pCommands[COMMAND_EFFECT_AREA_OF_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandEffectAreaEffect;
    m_pCommands[COMMAND_EFFECT_ATTACK_DECREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectAttackDecrease;
    m_pCommands[COMMAND_EFFECT_ATTACK_INCREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectAttackIncrease;
    m_pCommands[COMMAND_EFFECT_BEAM] = &CSWVirtualMachineCommands::ExecuteCommandEffectBeam;
    m_pCommands[COMMAND_EFFECT_BLIND] = &CSWVirtualMachineCommands::ExecuteCommandEffectBlind;
    m_pCommands[COMMAND_EFFECT_CRUSH] = &CSWVirtualMachineCommands::ExecuteCommandEffectCrush;
    m_pCommands[COMMAND_EFFECT_CONCEALMENT] = &CSWVirtualMachineCommands::ExecuteCommandEffectConcealment;
    m_pCommands[COMMAND_EFFECT_CONFUSED] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_DAMAGE] = &CSWVirtualMachineCommands::ExecuteCommandEffectDamage;
    m_pCommands[COMMAND_EFFECT_DAMAGE_DECREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectDamageDecrease;
    m_pCommands[COMMAND_EFFECT_DAMAGE_IMMUNITY_DECREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectDamageImmunityDecrease;
    m_pCommands[COMMAND_EFFECT_DAMAGE_IMMUNITY_INCREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectDamageImmunityIncrease;
    m_pCommands[COMMAND_EFFECT_DAMAGE_INCREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectDamageIncrease;
    m_pCommands[COMMAND_EFFECT_DAMAGE_REDUCTION] = &CSWVirtualMachineCommands::ExecuteCommandEffectDamageReduction;
    m_pCommands[COMMAND_EFFECT_DAMAGE_RESISTANCE] = &CSWVirtualMachineCommands::ExecuteCommandEffectDamageResistance;
    m_pCommands[COMMAND_EFFECT_DAMAGE_SHIELD] = &CSWVirtualMachineCommands::ExecuteCommandEffectDamageShield;
    m_pCommands[COMMAND_EFFECT_DEATH] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_DISGUISE] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_DISPEL_MAGIC_ALL] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_DISPEL_MAGIC_BEST] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_DROID_CONFUSED] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_ENTANGLE] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_FACTION_MODIFIER] = &CSWVirtualMachineCommands::ExecuteCommandEffectFactionModifier;
    m_pCommands[COMMAND_EFFECT_FORCE_BODY] = &CSWVirtualMachineCommands::ExecuteCommandEffectForceBody;
    m_pCommands[COMMAND_EFFECT_FORCE_SIGHT] = &CSWVirtualMachineCommands::ExecuteCommandEffectForceSight;
    m_pCommands[COMMAND_EFFECT_FORCE_DRAIN] = &CSWVirtualMachineCommands::ExecuteCommandEffectForceDrain;
    m_pCommands[COMMAND_EFFECT_FORCE_FIZZLE] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_FORCE_RESISTANCE_DECREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectForceResistanceDecrease;
    m_pCommands[COMMAND_EFFECT_FORCE_RESISTANCE_INCREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectForceResistanceIncrease;
    m_pCommands[COMMAND_EFFECT_FORCE_RESISTED] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_FORCE_SHIELD] = &CSWVirtualMachineCommands::ExecuteCommandEffectForceShield;
    m_pCommands[COMMAND_EFFECT_FP_REGEN_MODIFIER] = &CSWVirtualMachineCommands::ExecuteCommandEffectFPRegenModifier;
    m_pCommands[COMMAND_EFFECT_FRIGHTENED] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_FURY] = &CSWVirtualMachineCommands::ExecuteCommandEffectFury;
    m_pCommands[COMMAND_EFFECT_HASTE] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_HEAL] = &CSWVirtualMachineCommands::ExecuteCommandEffectHeal;
    m_pCommands[COMMAND_EFFECT_HIT_POINT_CHANGE_WHEN_DYING] = &CSWVirtualMachineCommands::ExecuteCommandEffectHitPointChangeWhenDying;
    m_pCommands[COMMAND_EFFECT_IMMUNITY] = &CSWVirtualMachineCommands::ExecuteCommandEffectImmunity;
    m_pCommands[COMMAND_EFFECT_INVISIBILITY] = &CSWVirtualMachineCommands::ExecuteCommandEffectInvisibility;
    m_pCommands[COMMAND_EFFECT_KNOCKDOWN] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_LINK_EFFECTS] = &CSWVirtualMachineCommands::ExecuteCommandEffectLinkEffects;
    m_pCommands[COMMAND_EFFECT_MISS_CHANCE] = &CSWVirtualMachineCommands::ExecuteCommandEffectMissChance;
    m_pCommands[COMMAND_EFFECT_MODIFY_ATTACKS] = &CSWVirtualMachineCommands::ExecuteCommandEffectModifyAttacks;
    m_pCommands[COMMAND_EFFECT_MOVEMENT_SPEED_DECREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectMovementSpeedDecrease;
    m_pCommands[COMMAND_EFFECT_MOVEMENT_SPEED_INCREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectMovementSpeedIncrease;
    m_pCommands[COMMAND_EFFECT_PARALYZE] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_POISON] = &CSWVirtualMachineCommands::ExecuteCommandEffectPoison;
    m_pCommands[COMMAND_EFFECT_REGENERATE] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_RESURRECTION] = &CSWVirtualMachineCommands::ExecuteCommandEffectResurrection;
    m_pCommands[COMMAND_EFFECT_SAVING_THROW_DECREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectSavingThrowDecrease;
    m_pCommands[COMMAND_EFFECT_SAVING_THROW_INCREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectSavingThrowIncrease;
    m_pCommands[COMMAND_EFFECT_SEE_INVISIBLE] = &CSWVirtualMachineCommands::ExecuteCommandEffectSeeInvisible;
    m_pCommands[COMMAND_EFFECT_SKILL_DECREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectSkillDecrease;
    m_pCommands[COMMAND_EFFECT_SKILL_INCREASE] = &CSWVirtualMachineCommands::ExecuteCommandEffectSkillIncrease;
    m_pCommands[COMMAND_EFFECT_SLEEP] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_SPELL_IMMUNITY] = &CSWVirtualMachineCommands::ExecuteCommandEffectSpellImmunity;
    m_pCommands[COMMAND_EFFECT_SPELL_LEVEL_ABSORPTION] = &CSWVirtualMachineCommands::ExecuteCommandEffectSpellLevelAbsorption;
    m_pCommands[COMMAND_EFFECT_STUNNED] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_TEMPORARY_FORCE_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandEffectTemporaryFP;
    m_pCommands[COMMAND_EFFECT_TEMPORARY_HITPOINTS] = &CSWVirtualMachineCommands::ExecuteCommandEffectTemporaryHP;
    m_pCommands[COMMAND_EFFECT_TIME_STOP] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_TRUE_SEEING] = &CSWVirtualMachineCommands::ExecuteCommandEffectTrueSeeing;
    m_pCommands[COMMAND_EFFECT_VISUAL_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandEffectVisualEffect;
    m_pCommands[COMMAND_EFFECT_VP_REGEN_MODIFIER] = &CSWVirtualMachineCommands::ExecuteCommandEffectVPRegenModifier;
    m_pCommands[COMMAND_EFFECT_MIND_TRICK] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_EFFECT_DROID_SCRAMBLE] = &CSWVirtualMachineCommands::ExecuteCommandEffectComplex;
    m_pCommands[COMMAND_ENABLE_RENDERING] = &CSWVirtualMachineCommands::ExecuteCommandEnableRendering;
    m_pCommands[COMMAND_ENABLE_VIDEO_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandEnableVideoEffect;
    m_pCommands[COMMAND_END_GAME] = &CSWVirtualMachineCommands::ExecuteCommandEndGame;
    m_pCommands[COMMAND_EVENT_ACTIVATE_ITEM] = &CSWVirtualMachineCommands::ExecuteCommandEventActivateItem;
    m_pCommands[COMMAND_EVENT_CONVERSATION] = &CSWVirtualMachineCommands::ExecuteCommandEventConversation;
    m_pCommands[COMMAND_EVENT_SPELL_CAST_AT] = &CSWVirtualMachineCommands::ExecuteCommandEventSpellCastAt;
    m_pCommands[COMMAND_EVENT_USER_DEFINED] = &CSWVirtualMachineCommands::ExecuteCommandEventUserDefined;
    m_pCommands[COMMAND_EXECUTE_SCRIPT] = &CSWVirtualMachineCommands::ExecuteCommandExecuteScript;
    m_pCommands[COMMAND_EXPLORE_AREA_FOR_PLAYER] = &CSWVirtualMachineCommands::ExecuteCommandExploreAreaForPlayer;
    m_pCommands[COMMAND_EXPORT_ALL_CHARACTERS] = &CSWVirtualMachineCommands::ExecuteCommandExportAllCharacters;
    m_pCommands[COMMAND_EXTRAORDINARY_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandSetSubType;
    m_pCommands[COMMAND_FABS] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_FACE_OBJECT_AWAY_FROM_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandFaceObjectAwayFromObject;
    m_pCommands[COMMAND_FEET_TO_METERS] = &CSWVirtualMachineCommands::ExecuteCommandDistanceConversions;
    m_pCommands[COMMAND_FIND_SUB_STRING] = &CSWVirtualMachineCommands::ExecuteCommandFindSubString;
    m_pCommands[COMMAND_FLOATING_TEXT_STRING_ON_CREATURE] = &CSWVirtualMachineCommands::ExecuteCommandFloatingText;
    m_pCommands[COMMAND_FLOATING_TEXT_STR_REF_ON_CREATURE] = &CSWVirtualMachineCommands::ExecuteCommandFloatingText;
    m_pCommands[COMMAND_FLOAT_TO_INT] = &CSWVirtualMachineCommands::ExecuteCommandFloatToInt;
    m_pCommands[COMMAND_FLOAT_TO_STRING] = &CSWVirtualMachineCommands::ExecuteCommandFloatToString;
    m_pCommands[COMMAND_FORTITUDE_SAVE] = &CSWVirtualMachineCommands::ExecuteCommandSavingThrow;
    m_pCommands[COMMAND_FORCE_HEARTBEAT] = &CSWVirtualMachineCommands::ExecuteCommandForceHeartbeat;
    m_pCommands[COMMAND_SET_FORCE_ALWAYS_UPDATE] = &CSWVirtualMachineCommands::ExecuteCommandForceAlwaysUpdate;
    m_pCommands[COMMAND_GET_ABILITY_MODIFIER] = &CSWVirtualMachineCommands::ExecuteCommandGetAbilityModifier;
    m_pCommands[COMMAND_GET_ABILITY_SCORE] = &CSWVirtualMachineCommands::ExecuteCommandGetAbilityScore;
    m_pCommands[COMMAND_GET_AC] = &CSWVirtualMachineCommands::ExecuteCommandGetAC;
    m_pCommands[COMMAND_GET_ALIGNMENT_GOOD_EVIL] = &CSWVirtualMachineCommands::ExecuteCommandGetAlignment;
    m_pCommands[COMMAND_GET_APPEARANCE_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetAppearanceType;
    m_pCommands[COMMAND_GET_AREA] = &CSWVirtualMachineCommands::ExecuteCommandGetArea;
    m_pCommands[COMMAND_GET_AREA_OF_EFFECT_CREATOR] = &CSWVirtualMachineCommands::ExecuteCommandGetAOEObjectCreator;
    m_pCommands[COMMAND_GET_AREA_UNESCAPABLE] = &CSWVirtualMachineCommands::ExecuteCommandGetAreaUnescapable;
    m_pCommands[COMMAND_GET_ATTACK_TARGET] = &CSWVirtualMachineCommands::ExecuteCommandGetAttackTarget;
    m_pCommands[COMMAND_GET_ATTEMPTED_ATTACK_TARGET] = &CSWVirtualMachineCommands::ExecuteCommandGetAttemptedAttackTarget;
    m_pCommands[COMMAND_GET_ATTEMPTED_SPELL_TARGET] = &CSWVirtualMachineCommands::ExecuteCommandGetAttemptedSpellTarget;
    m_pCommands[COMMAND_GET_ATTEMPTED_MOVEMENT_TARGET] = &CSWVirtualMachineCommands::ExecuteCommandGetAttemptedMovementTarget;
    m_pCommands[COMMAND_GET_BLOCKING_CREATURE] = &CSWVirtualMachineCommands::ExecuteCommandGetBlockingCreature;
    m_pCommands[COMMAND_GET_BASE_ITEM_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetBaseItemType;
    m_pCommands[COMMAND_GET_BLOCKING_DOOR] = &CSWVirtualMachineCommands::ExecuteCommandGetBlockingDoor;
    m_pCommands[COMMAND_GET_BUTTON_MASH_CHECK] = &CSWVirtualMachineCommands::ExecuteCommandGetButtonMashCheck;
    m_pCommands[COMMAND_GET_CASTER_LEVEL] = &CSWVirtualMachineCommands::ExecuteCommandGetCasterLevel;
    m_pCommands[COMMAND_GET_CATEGORY_FROM_TALENT] = &CSWVirtualMachineCommands::ExecuteCommandTalentAccess;
    m_pCommands[COMMAND_GET_CHALLENGE_RATING] = &CSWVirtualMachineCommands::ExecuteCommandGetChallengeRating;
    m_pCommands[COMMAND_GET_CHEAT_CODE] = &CSWVirtualMachineCommands::ExecuteCommandGetCheatCode;
    m_pCommands[COMMAND_GET_CLASS_BY_POSITION] = &CSWVirtualMachineCommands::ExecuteCommandGetClassInformation;
    m_pCommands[COMMAND_GET_CLICKING_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandGetEnteringObject;
    m_pCommands[COMMAND_GET_COMMANDABLE] = &CSWVirtualMachineCommands::ExecuteCommandGetCommandable;
    m_pCommands[COMMAND_GET_CREATURE_HAS_TALENT] = &CSWVirtualMachineCommands::ExecuteCommandGetCreatureHasTalent;
    m_pCommands[COMMAND_GET_CREATURE_SIZE] = &CSWVirtualMachineCommands::ExecuteCommandGetCreatureSize;
    m_pCommands[COMMAND_GET_CREATURE_TALENT_BEST] = &CSWVirtualMachineCommands::ExecuteCommandGetCreatureTalent;
    m_pCommands[COMMAND_GET_CREATURE_TALENT_RANDOM] = &CSWVirtualMachineCommands::ExecuteCommandGetCreatureTalent;
    m_pCommands[COMMAND_GET_CURRENT_ACTION] = &CSWVirtualMachineCommands::ExecuteCommandGetCurrentAction;
    m_pCommands[COMMAND_GET_CURRENT_FORCE_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandGetCurrentForcePoints;
    m_pCommands[COMMAND_GET_CURRENT_HIT_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandGetHitpoints;
    m_pCommands[COMMAND_GET_CURRENT_STEALTH_XP] = &CSWVirtualMachineCommands::ExecuteCommandGetCurrentStealthXP;
    m_pCommands[COMMAND_GET_DAMAGE_DEALT_BY_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetDamageDealtByType;
    m_pCommands[COMMAND_GET_DIFFICULTY_MODIFIER] = &CSWVirtualMachineCommands::ExecuteCommandGetDifficultyMultiplier;
    m_pCommands[COMMAND_GET_DISTANCE_BETWEEN] = &CSWVirtualMachineCommands::ExecuteCommandGetDistanceBetween;
    m_pCommands[COMMAND_GET_DISTANCE_BETWEEN2_D] = &CSWVirtualMachineCommands::ExecuteCommandGetDistanceBetween2D;
    m_pCommands[COMMAND_GET_DISTANCE_BETWEEN_LOCATIONS] = &CSWVirtualMachineCommands::ExecuteCommandGetDistanceBetweenLocations;
    m_pCommands[COMMAND_GET_DISTANCE_BETWEEN_LOCATIONS2_D] = &CSWVirtualMachineCommands::ExecuteCommandGetDistanceBetweenLocations2D;
    m_pCommands[COMMAND_GET_DISTANCE_TO_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandGetDistanceToObject;
    m_pCommands[COMMAND_GET_DISTANCE_TO_OBJECT2_D] = &CSWVirtualMachineCommands::ExecuteCommandGetDistanceToObject2D;
    m_pCommands[COMMAND_GET_EFFECT_CREATOR] = &CSWVirtualMachineCommands::ExecuteCommandGetEffectCreator;
    m_pCommands[COMMAND_GET_EFFECT_DURATION_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetEffectDurationType;
    m_pCommands[COMMAND_GET_EFFECT_SPELL_ID] = &CSWVirtualMachineCommands::ExecuteCommandGetEffectSpellId;
    m_pCommands[COMMAND_GET_EFFECT_SUB_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetEffectSubType;
    m_pCommands[COMMAND_GET_EFFECT_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetEffectType;
    m_pCommands[COMMAND_GET_ENCOUNTER_ACTIVE] = &CSWVirtualMachineCommands::ExecuteCommandGetEncounterData;
    m_pCommands[COMMAND_GET_ENCOUNTER_DIFFICULTY] = &CSWVirtualMachineCommands::ExecuteCommandGetEncounterData;
    m_pCommands[COMMAND_GET_ENCOUNTER_SPAWNS_CURRENT] = &CSWVirtualMachineCommands::ExecuteCommandGetEncounterData;
    m_pCommands[COMMAND_GET_ENCOUNTER_SPAWNS_MAX] = &CSWVirtualMachineCommands::ExecuteCommandGetEncounterData;
    m_pCommands[COMMAND_GET_ENTERING_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandGetEnteringObject;
    m_pCommands[COMMAND_GET_EXITING_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandGetLeavingObject;
    m_pCommands[COMMAND_GET_FACING] = &CSWVirtualMachineCommands::ExecuteCommandGetFacing;
    m_pCommands[COMMAND_GET_FACING_FROM_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandLocationAccess;
    m_pCommands[COMMAND_GET_FACTION_AVERAGE_GOOD_EVIL_ALIGNMENT] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionAverageGoodEvilAlignment;
    m_pCommands[COMMAND_GET_FACTION_AVERAGE_LEVEL] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionAverageLevel;
    m_pCommands[COMMAND_GET_FACTION_AVERAGE_REPUTATION] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionAverageReputation;
    m_pCommands[COMMAND_GET_FACTION_AVERAGE_XP] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionAverageXP;
    m_pCommands[COMMAND_GET_FACTION_BEST_AC] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionBestAC;
    m_pCommands[COMMAND_GET_FACTION_EQUAL] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionEqual;
    m_pCommands[COMMAND_GET_FACTION_GOLD] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionGold;
    m_pCommands[COMMAND_GET_FACTION_LEADER] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionLeader;
    m_pCommands[COMMAND_GET_FACTION_LEAST_DAMAGED_MEMBER] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionLeastDamagedMember;
    m_pCommands[COMMAND_GET_FACTION_MOST_DAMAGED_MEMBER] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionMostDamagedMember;
    m_pCommands[COMMAND_GET_FACTION_MOST_FREQUENT_CLASS] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionMostFrequentClass;
    m_pCommands[COMMAND_GET_FACTION_STRONGEST_MEMBER] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionStrongestMember;
    m_pCommands[COMMAND_GET_FACTION_WEAKEST_MEMBER] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionWeakestMember;
    m_pCommands[COMMAND_GET_FACTION_WORST_AC] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionWorstAC;
    m_pCommands[COMMAND_GET_FIRST_ATTACKER] = &CSWVirtualMachineCommands::ExecuteCommandGetAttacker;
    m_pCommands[COMMAND_GET_FIRST_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandGetEffect;
    m_pCommands[COMMAND_GET_FIRST_FACTION_MEMBER] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionMember;
    m_pCommands[COMMAND_GET_FIRST_IN_PERSISTENT_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandGetInPersistentObject;
    m_pCommands[COMMAND_GET_FIRST_ITEM_IN_INVENTORY] = &CSWVirtualMachineCommands::ExecuteCommandGetItemInInventory;
    m_pCommands[COMMAND_GET_FIRST_OBJECT_IN_AREA] = &CSWVirtualMachineCommands::ExecuteCommandGetObjectInArea;
    m_pCommands[COMMAND_GET_FIRST_OBJECT_IN_SHAPE] = &CSWVirtualMachineCommands::ExecuteCommandGetObjectInShape;
    m_pCommands[COMMAND_GET_FIRST_PC] = &CSWVirtualMachineCommands::ExecuteCommandGetPC;
    m_pCommands[COMMAND_GET_FORTITUDE_SAVING_THROW] = &CSWVirtualMachineCommands::ExecuteCommandGetFortitudeSavingThrow;
    m_pCommands[COMMAND_GET_FOUND_ENEMY_CREATURE] = &CSWVirtualMachineCommands::ExecuteCommandGetFoundEnemyCreature;
    m_pCommands[COMMAND_GET_GAME_DIFFICULTY] = &CSWVirtualMachineCommands::ExecuteCommandGetGameDifficulty;
    m_pCommands[COMMAND_GET_GENDER] = &CSWVirtualMachineCommands::ExecuteCommandGetStat;
    m_pCommands[COMMAND_GET_GLOBAL_BOOLEAN] = &CSWVirtualMachineCommands::ExecuteCommandGetGlobalNumber;
    m_pCommands[COMMAND_GET_GLOBAL_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandGetGlobalLocation;
    m_pCommands[COMMAND_GET_GLOBAL_NUMBER] = &CSWVirtualMachineCommands::ExecuteCommandGetGlobalNumber;
    m_pCommands[COMMAND_GET_GLOBAL_STRING] = &CSWVirtualMachineCommands::ExecuteCommandGetGlobalString;
    m_pCommands[COMMAND_GET_GOING_TO_BE_ATTACKED_BY] = &CSWVirtualMachineCommands::ExecuteCommandGetGoingToBeAttackedBy;
    m_pCommands[COMMAND_GET_GOLD] = &CSWVirtualMachineCommands::ExecuteCommandGetGold;
    m_pCommands[COMMAND_GET_GOLD_PIECE_VALUE] = &CSWVirtualMachineCommands::ExecuteCommandGetGoldPieceValue;
    m_pCommands[COMMAND_GET_ITEM_COMPONENT] = &CSWVirtualMachineCommands::ExecuteCommandGetGold;
    m_pCommands[COMMAND_GET_ITEM_COMPONENT_PIECE_VALUE] = &CSWVirtualMachineCommands::ExecuteCommandGetGoldPieceValue;
    m_pCommands[COMMAND_GET_CHEMICALS] = &CSWVirtualMachineCommands::ExecuteCommandGetChemical;
    m_pCommands[COMMAND_GET_CHEMICAL_PIECE_VALUE] = &CSWVirtualMachineCommands::ExecuteCommandGetChemicalPieceValue;
    m_pCommands[COMMAND_GET_GOOD_EVIL_VALUE] = &CSWVirtualMachineCommands::ExecuteCommandGetAlignment;
    m_pCommands[COMMAND_GET_HAS_FEAT] = &CSWVirtualMachineCommands::ExecuteCommandGetHasFeat;
    m_pCommands[COMMAND_GET_HAS_FEAT_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandGetHasFeatEffect;
    m_pCommands[COMMAND_GET_HAS_INVENTORY] = &CSWVirtualMachineCommands::ExecuteCommandGetHasInventory;
    m_pCommands[COMMAND_GET_HAS_SKILL] = &CSWVirtualMachineCommands::ExecuteCommandGetHasSkill;
    m_pCommands[COMMAND_GET_HAS_SPELL] = &CSWVirtualMachineCommands::ExecuteCommandGetHasSpell;
    m_pCommands[COMMAND_GET_HAS_SPELL_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandGetHasSpellEffect;
    m_pCommands[COMMAND_GET_HEAL_TARGET] = &CSWVirtualMachineCommands::ExecuteCommandGetHealTarget;
    m_pCommands[COMMAND_GET_HIT_DICE] = &CSWVirtualMachineCommands::ExecuteCommandGetHitDice;
    m_pCommands[COMMAND_GET_IDENTIFIED] = &CSWVirtualMachineCommands::ExecuteCommandGetIdentified;
    m_pCommands[COMMAND_GET_ID_FROM_TALENT] = &CSWVirtualMachineCommands::ExecuteCommandTalentAccess;
    m_pCommands[COMMAND_GET_INVENTORY_DISTURB_ITEM] = &CSWVirtualMachineCommands::ExecuteCommandGetInventoryDisturbItem;
    m_pCommands[COMMAND_GET_INVENTORY_DISTURB_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetInventoryDisturbType;
    m_pCommands[COMMAND_GET_IS_CONVERSATION_ACTIVE] = &CSWVirtualMachineCommands::ExecuteCommandGetIsConversationActive;
    m_pCommands[COMMAND_GET_CREATURE_MOVMENT_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetCreatureMovmentType;
    m_pCommands[COMMAND_GET_IS_DAWN] = &CSWVirtualMachineCommands::ExecuteCommandGetIsDawn;
    m_pCommands[COMMAND_GET_IS_DAY] = &CSWVirtualMachineCommands::ExecuteCommandGetIsDay;
    m_pCommands[COMMAND_GET_IS_DEBILITATED] = &CSWVirtualMachineCommands::ExecuteCommandGetIsDebilitated;
    m_pCommands[COMMAND_GET_IS_DOOR_ACTION_POSSIBLE] = &CSWVirtualMachineCommands::ExecuteCommandGetIsDoorActionPossible;
    m_pCommands[COMMAND_GET_IS_DUSK] = &CSWVirtualMachineCommands::ExecuteCommandGetIsDusk;
    m_pCommands[COMMAND_GET_IS_ENCOUNTER_CREATURE] = &CSWVirtualMachineCommands::ExecuteCommandGetIsEncounterCreature;
    m_pCommands[COMMAND_GET_IS_ENEMY] = &CSWVirtualMachineCommands::ExecuteCommandGetEmotions;
    m_pCommands[COMMAND_GET_IS_FRIEND] = &CSWVirtualMachineCommands::ExecuteCommandGetEmotions;
    m_pCommands[COMMAND_GET_IS_IMMUNE] = &CSWVirtualMachineCommands::ExecuteCommandGetIsImmune;
    m_pCommands[COMMAND_GET_IS_IN_COMBAT] = &CSWVirtualMachineCommands::ExecuteCommandGetIsInCombat;
    m_pCommands[COMMAND_GET_IS_IN_CONVERSATION] = &CSWVirtualMachineCommands::ExecuteCommandGetIsInConversation;
    m_pCommands[COMMAND_GET_IS_LINK_IMMUNE] = &CSWVirtualMachineCommands::ExecuteCommandGetIsLinkImmune;
    m_pCommands[COMMAND_GET_IS_LISTENING] = &CSWVirtualMachineCommands::ExecuteCommandIsListening;
    m_pCommands[COMMAND_GET_IS_LIVE_CONTENT_AVAILABLE] = &CSWVirtualMachineCommands::ExecuteCommandGetIsLiveContentAvailable;
    m_pCommands[COMMAND_GET_IS_NEUTRAL] = &CSWVirtualMachineCommands::ExecuteCommandGetEmotions;
    m_pCommands[COMMAND_GET_IS_NIGHT] = &CSWVirtualMachineCommands::ExecuteCommandGetIsNight;
    m_pCommands[COMMAND_GET_IS_OBJECT_VALID] = &CSWVirtualMachineCommands::ExecuteCommandGetObjectValid;
    m_pCommands[COMMAND_GET_IS_OPEN] = &CSWVirtualMachineCommands::ExecuteCommandGetIsOpen;
    m_pCommands[COMMAND_GET_IS_PLACEABLE_OBJECT_ACTION_POSSIBLE] = &CSWVirtualMachineCommands::ExecuteCommandGetIsDoorActionPossible;
    m_pCommands[COMMAND_GET_IS_POISONED] = &CSWVirtualMachineCommands::ExecuteCommandGetIsPoisoned;
    m_pCommands[COMMAND_GET_IS_PUPPET] = &CSWVirtualMachineCommands::ExecuteCommandGetIsPuppet;
    m_pCommands[COMMAND_GET_IS_XBOX] = &CSWVirtualMachineCommands::ExecuteCommandGetIsXBox;
    m_pCommands[COMMAND_GET_IS_PLAYABLE_RACIAL_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetIsRacialTypePlayable;
    m_pCommands[COMMAND_GET_IS_TALENT_VALID] = &CSWVirtualMachineCommands::ExecuteCommandGetIsTalentValid;
    m_pCommands[COMMAND_GET_IS_TRAPPED] = &CSWVirtualMachineCommands::ExecuteCommandGetIsMined;
    m_pCommands[COMMAND_GET_IS_PARTY_LEADER] = &CSWVirtualMachineCommands::ExecuteCommandGetIsPartyLeader;
    m_pCommands[COMMAND_GET_PARTY_LEADER] = &CSWVirtualMachineCommands::ExecuteCommandGetPCSpeaker;
    m_pCommands[COMMAND_GET_IS_WEAPON_EFFECTIVE] = &CSWVirtualMachineCommands::ExecuteCommandGetIsWeaponEffective;
    m_pCommands[COMMAND_GET_ITEM_ACTIVATED] = &CSWVirtualMachineCommands::ExecuteCommandItemActivated;
    m_pCommands[COMMAND_GET_ITEM_ACTIVATED_TARGET_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandItemActivated;
    m_pCommands[COMMAND_GET_ITEM_ACTIVATED_TARGET] = &CSWVirtualMachineCommands::ExecuteCommandItemActivated;
    m_pCommands[COMMAND_GET_ITEM_ACTIVATOR] = &CSWVirtualMachineCommands::ExecuteCommandItemActivated;
    m_pCommands[COMMAND_GET_ITEM_AC_VALUE] = &CSWVirtualMachineCommands::ExecuteCommandGetItemACValue;
    m_pCommands[COMMAND_GET_ITEM_HAS_ITEM_PROPERTY] = &CSWVirtualMachineCommands::ExecuteCommandGetItemHasItemProperty;
    m_pCommands[COMMAND_GET_ITEM_IN_SLOT] = &CSWVirtualMachineCommands::ExecuteCommandGetItemInSlot;
    m_pCommands[COMMAND_GET_ITEM_POSSESSED_BY] = &CSWVirtualMachineCommands::ExecuteCommandGetItemPossessedBy;
    m_pCommands[COMMAND_GET_ITEM_POSSESSOR] = &CSWVirtualMachineCommands::ExecuteCommandGetItemPossessor;
    m_pCommands[COMMAND_GET_ITEM_STACK_SIZE] = &CSWVirtualMachineCommands::ExecuteCommandGetItemStackSize;
    m_pCommands[COMMAND_GET_JOURNAL_ENTRY] = &CSWVirtualMachineCommands::ExecuteCommandGetJournalEntry;
    m_pCommands[COMMAND_GET_JOURNAL_QUEST_EXPERIENCE] = &CSWVirtualMachineCommands::ExecuteCommandGetJournalQuestExperience;
    m_pCommands[COMMAND_GET_LAST_ASSOCIATE_COMMAND] = &CSWVirtualMachineCommands::ExecuteCommandGetLastAssociateCommand;
    m_pCommands[COMMAND_GET_LAST_ATTACKER] = &CSWVirtualMachineCommands::ExecuteCommandGetLastAttacker;
    m_pCommands[COMMAND_GET_LAST_ATTACK_MODE] = &CSWVirtualMachineCommands::ExecuteCommandGetLastAttackMode;
    m_pCommands[COMMAND_GET_LAST_ATTACK_ACTION] = &CSWVirtualMachineCommands::ExecuteCommandGetLastAttackAction;
    m_pCommands[COMMAND_GET_LAST_ATTACK_RESULT] = &CSWVirtualMachineCommands::ExecuteCommandGetLastAttackResult;
    m_pCommands[COMMAND_GET_LAST_HOSTILE_TARGET] = &CSWVirtualMachineCommands::ExecuteCommandGetLastHostileTarget;
    m_pCommands[COMMAND_GET_LAST_ATTACK_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetLastAttackType;
    m_pCommands[COMMAND_GET_LAST_CLOSED_BY] = &CSWVirtualMachineCommands::ExecuteCommandGetLastClosedBy;
    m_pCommands[COMMAND_GET_LAST_COMBAT_FEAT_USED] = &CSWVirtualMachineCommands::ExecuteCommandGetLastCombatFeatUsed;
    m_pCommands[COMMAND_GET_LAST_CONVERSATION] = &CSWVirtualMachineCommands::ExecuteCommandGetLastConversation;
    m_pCommands[COMMAND_GET_LAST_DAMAGER] = &CSWVirtualMachineCommands::ExecuteCommandGetLastDamager;
    m_pCommands[COMMAND_GET_LAST_DISARMED] = &CSWVirtualMachineCommands::ExecuteCommandGetLastDisarmed;
    m_pCommands[COMMAND_GET_LAST_DISTURBED] = &CSWVirtualMachineCommands::ExecuteCommandGetLastDisturbed;
    m_pCommands[COMMAND_GET_LAST_FORCE_POWER_USED] = &CSWVirtualMachineCommands::ExecuteCommandGetLastForcePowerUsed;
    m_pCommands[COMMAND_GET_LAST_HOSTILE_ACTOR] = &CSWVirtualMachineCommands::ExecuteCommandGetLastHostileActor;
    m_pCommands[COMMAND_GET_LAST_KILLER] = &CSWVirtualMachineCommands::ExecuteCommandGetLastKiller;
    m_pCommands[COMMAND_GET_LAST_LOCKED] = &CSWVirtualMachineCommands::ExecuteCommandGetLastLocked;
    m_pCommands[COMMAND_GET_LAST_OPENED_BY] = &CSWVirtualMachineCommands::ExecuteCommandGetLastOpenedBy;
    m_pCommands[COMMAND_GET_LAST_PAZAAK_RESULT] = &CSWVirtualMachineCommands::ExecuteCommandGetLastPazaakResult;
    m_pCommands[COMMAND_GET_LAST_PERCEIVED] = &CSWVirtualMachineCommands::ExecuteCommandGetLastPerceived;
    m_pCommands[COMMAND_GET_LAST_PERCEPTION_HEARD] = &CSWVirtualMachineCommands::ExecuteCommandGetLastPerception;
    m_pCommands[COMMAND_GET_LAST_PERCEPTION_INAUDIBLE] = &CSWVirtualMachineCommands::ExecuteCommandGetLastPerception;
    m_pCommands[COMMAND_GET_LAST_PERCEPTION_SEEN] = &CSWVirtualMachineCommands::ExecuteCommandGetLastPerception;
    m_pCommands[COMMAND_GET_LAST_PERCEPTION_VANISHED] = &CSWVirtualMachineCommands::ExecuteCommandGetLastPerception;
    m_pCommands[COMMAND_GET_LAST_PLAYER_DIED] = &CSWVirtualMachineCommands::ExecuteCommandModuleAccess;
    m_pCommands[COMMAND_GET_LAST_PLAYER_DYING] = &CSWVirtualMachineCommands::ExecuteCommandModuleAccess;
    m_pCommands[COMMAND_GET_LAST_RESPAWN_BUTTON_PRESSER] = &CSWVirtualMachineCommands::ExecuteCommandModuleAccess;
    m_pCommands[COMMAND_GET_LAST_ITEM_EQUIPPED] = &CSWVirtualMachineCommands::ExecuteCommandModuleAccess;
    m_pCommands[COMMAND_GET_LAST_SPEAKER] = &CSWVirtualMachineCommands::ExecuteCommandGetLastSpeaker;
    m_pCommands[COMMAND_GET_LAST_SPELL] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellCast;
    m_pCommands[COMMAND_GET_LAST_SPELL_CASTER] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellCast;
    m_pCommands[COMMAND_GET_LAST_SPELL_HARMFUL] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellCast;
    m_pCommands[COMMAND_GET_LAST_TRAP_DETECTED] = &CSWVirtualMachineCommands::ExecuteCommandGetLastMineDetected;
    m_pCommands[COMMAND_GET_LAST_UNLOCKED] = &CSWVirtualMachineCommands::ExecuteCommandGetLastUnlocked;
    m_pCommands[COMMAND_GET_LAST_USED_BY] = &CSWVirtualMachineCommands::ExecuteCommandGetLastUsedBy;
    m_pCommands[COMMAND_GET_LAST_WEAPON_USED] = &CSWVirtualMachineCommands::ExecuteCommandGetLastWeaponUsed;
    m_pCommands[COMMAND_GET_LEVEL_BY_CLASS] = &CSWVirtualMachineCommands::ExecuteCommandGetClassInformation;
    m_pCommands[COMMAND_GET_LEVEL_BY_POSITION] = &CSWVirtualMachineCommands::ExecuteCommandGetClassInformation;
    m_pCommands[COMMAND_GET_LOAD_FROM_SAVE_GAME] = &CSWVirtualMachineCommands::ExecuteCommandGetLoadFromSaveGame;
    m_pCommands[COMMAND_GET_LOCAL_BOOLEAN] = &CSWVirtualMachineCommands::ExecuteCommandGetLocalValue;
    m_pCommands[COMMAND_GET_LOCAL_NUMBER] = &CSWVirtualMachineCommands::ExecuteCommandGetLocalValue;
    m_pCommands[COMMAND_GET_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandGetLocation;
    m_pCommands[COMMAND_GET_LOCKED] = &CSWVirtualMachineCommands::ExecuteCommandGetLocked;
    m_pCommands[COMMAND_GET_LOCK_KEY_REQUIRED] = &CSWVirtualMachineCommands::ExecuteCommandGetLockInfo;
    m_pCommands[COMMAND_GET_LOCK_KEY_TAG] = &CSWVirtualMachineCommands::ExecuteCommandGetLockInfo;
    m_pCommands[COMMAND_GET_LOCK_LOCKABLE] = &CSWVirtualMachineCommands::ExecuteCommandGetLockInfo;
    m_pCommands[COMMAND_GET_LOCK_LOCK_DC] = &CSWVirtualMachineCommands::ExecuteCommandGetLockInfo;
    m_pCommands[COMMAND_GET_LOCK_UNLOCK_DC] = &CSWVirtualMachineCommands::ExecuteCommandGetLockInfo;
    m_pCommands[COMMAND_GET_LISTEN_PATTERN_NUMBER] = &CSWVirtualMachineCommands::ExecuteCommandGetMatchedStringsNum;
    m_pCommands[COMMAND_GET_MATCHED_SUBSTRING] = &CSWVirtualMachineCommands::ExecuteCommandGetMatchedString;
    m_pCommands[COMMAND_GET_MATCHED_SUBSTRINGS_COUNT] = &CSWVirtualMachineCommands::ExecuteCommandGetMatchedStringsCount;
    m_pCommands[COMMAND_GET_MAX_FORCE_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandGetMaxForcePoints;
    m_pCommands[COMMAND_GET_MAX_HIT_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandGetHitpoints;
    m_pCommands[COMMAND_GET_MAX_STEALTH_XP] = &CSWVirtualMachineCommands::ExecuteCommandGetMaxStealthXP;
    m_pCommands[COMMAND_GET_META_MAGIC_FEAT] = &CSWVirtualMachineCommands::ExecuteCommandGetMetaMagicFeat;
    m_pCommands[COMMAND_GET_MIN_ONE_HP] = &CSWVirtualMachineCommands::ExecuteCommandGetMinOneHP;
    m_pCommands[COMMAND_GET_MODULE] = &CSWVirtualMachineCommands::ExecuteCommandGetModule;
    m_pCommands[COMMAND_GET_MODULE_FILE_NAME] = &CSWVirtualMachineCommands::ExecuteCommandGetModuleName;
    m_pCommands[COMMAND_GET_MODULE_ITEM_ACQUIRED] = &CSWVirtualMachineCommands::ExecuteCommandGetModuleItemStuff;
    m_pCommands[COMMAND_GET_MODULE_ITEM_ACQUIRED_FROM] = &CSWVirtualMachineCommands::ExecuteCommandGetModuleItemStuff;
    m_pCommands[COMMAND_GET_MODULE_ITEM_LOST] = &CSWVirtualMachineCommands::ExecuteCommandGetModuleItemStuff;
    m_pCommands[COMMAND_GET_MODULE_ITEM_LOST_BY] = &CSWVirtualMachineCommands::ExecuteCommandGetModuleItemStuff;
    m_pCommands[COMMAND_GET_MODULE_NAME] = &CSWVirtualMachineCommands::ExecuteCommandGetModuleName;
    m_pCommands[COMMAND_GET_MOVEMENT_RATE] = &CSWVirtualMachineCommands::ExecuteCommandGetMovementRate;
    m_pCommands[COMMAND_MUSIC_BACKGROUND_GET_DAY_TRACK] = &CSWVirtualMachineCommands::ExecuteCommandGetDayTrack;
    m_pCommands[COMMAND_MUSIC_BACKGROUND_GET_NIGHT_TRACK] = &CSWVirtualMachineCommands::ExecuteCommandGetNightTrack;
    m_pCommands[COMMAND_GET_NAME] = &CSWVirtualMachineCommands::ExecuteCommandGetName;
    m_pCommands[COMMAND_GET_NEAREST_CREATURE_TO_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandGetNearestObject;
    m_pCommands[COMMAND_GET_NEAREST_CREATURE] = &CSWVirtualMachineCommands::ExecuteCommandGetNearestObject;
    m_pCommands[COMMAND_GET_NEAREST_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandGetNearestObject;
    m_pCommands[COMMAND_GET_NEAREST_OBJECT_BY_TAG] = &CSWVirtualMachineCommands::ExecuteCommandGetNearestObject;
    m_pCommands[COMMAND_GET_NEAREST_OBJECT_TO_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandGetNearestObject;
    m_pCommands[COMMAND_GET_NEAREST_TRAP_TO_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandGetNearestMine;
    m_pCommands[COMMAND_GET_NEXT_ATTACKER] = &CSWVirtualMachineCommands::ExecuteCommandGetAttacker;
    m_pCommands[COMMAND_GET_NEXT_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandGetEffect;
    m_pCommands[COMMAND_GET_NEXT_FACTION_MEMBER] = &CSWVirtualMachineCommands::ExecuteCommandGetFactionMember;
    m_pCommands[COMMAND_GET_NEXT_IN_PERSISTENT_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandGetInPersistentObject;
    m_pCommands[COMMAND_GET_NEXT_ITEM_IN_INVENTORY] = &CSWVirtualMachineCommands::ExecuteCommandGetItemInInventory;
    m_pCommands[COMMAND_GET_NEXT_OBJECT_IN_AREA] = &CSWVirtualMachineCommands::ExecuteCommandGetObjectInArea;
    m_pCommands[COMMAND_GET_NPC_SELECTABILITY] = &CSWVirtualMachineCommands::ExecuteCommandIsAvailableNPC;
    m_pCommands[COMMAND_GET_NEXT_OBJECT_IN_SHAPE] = &CSWVirtualMachineCommands::ExecuteCommandGetObjectInShape;
    m_pCommands[COMMAND_GET_NEXT_PC] = &CSWVirtualMachineCommands::ExecuteCommandGetPC;
    m_pCommands[COMMAND_GET_NPCAI_STYLE] = &CSWVirtualMachineCommands::ExecuteCommandGetNPCAIStyle;
    m_pCommands[COMMAND_GET_NPC_SELECTABILITY] = &CSWVirtualMachineCommands::ExecuteCommandIsAvailableNPC;
    m_pCommands[COMMAND_GET_NUM_STACKED_ITEMS] = &CSWVirtualMachineCommands::ExecuteCommandGetNumStackedItems;
    m_pCommands[COMMAND_GET_OBJECT_BY_TAG] = &CSWVirtualMachineCommands::ExecuteCommandGetObjectByTag;
    m_pCommands[COMMAND_GET_OBJECT_HEARD] = &CSWVirtualMachineCommands::ExecuteCommandGetObjectVisibility;
    m_pCommands[COMMAND_GET_OBJECT_SEEN] = &CSWVirtualMachineCommands::ExecuteCommandGetObjectVisibility;
    m_pCommands[COMMAND_GET_OBJECT_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetObjectType;
    m_pCommands[COMMAND_GET_OWNER_DEMOLITIONS_SKILL] = &CSWVirtualMachineCommands::ExecuteCommandGetOwnerDemolitionsSkill;
    m_pCommands[COMMAND_GET_INFLUENCE] = &CSWVirtualMachineCommands::ExecuteCommandInfluence;
    m_pCommands[COMMAND_SET_INFLUENCE] = &CSWVirtualMachineCommands::ExecuteCommandInfluence;
    m_pCommands[COMMAND_MODIFY_INFLUENCE] = &CSWVirtualMachineCommands::ExecuteCommandInfluence;
    m_pCommands[COMMAND_GET_PARTY_AI_STYLE] = &CSWVirtualMachineCommands::ExecuteCommandGetPartyAIStyle;
    m_pCommands[COMMAND_GET_PARTY_MEMBER_BY_INDEX] = &CSWVirtualMachineCommands::ExecuteCommandGetPartyMemberByIndex;
    m_pCommands[COMMAND_GET_PARTY_MEMBER_COUNT] = &CSWVirtualMachineCommands::ExecuteCommandGetPartyMemberCount;
    m_pCommands[COMMAND_GET_PLANET_AVAILABLE] = &CSWVirtualMachineCommands::ExecuteCommandGetGalaxyMapPlanetAvailable;
    m_pCommands[COMMAND_GET_PLANET_SELECTABLE] = &CSWVirtualMachineCommands::ExecuteCommandGetGalaxyMapPlanetSelectable;
    m_pCommands[COMMAND_GET_RUN_SCRIPT_VAR] = &CSWVirtualMachineCommands::ExecuteCommandGetRunScriptVar;
    m_pCommands[COMMAND_GET_PC_LEVELLING_UP] = &CSWVirtualMachineCommands::ExecuteCommandGetPCLevellingUp;
    m_pCommands[COMMAND_GET_PC_SPEAKER] = &CSWVirtualMachineCommands::ExecuteCommandGetPCSpeaker;
    m_pCommands[COMMAND_GET_PLACEABLE_ILLUMINATION] = &CSWVirtualMachineCommands::ExecuteCommandGetPlaceableIllumination;
    m_pCommands[COMMAND_GET_PLOT_FLAG] = &CSWVirtualMachineCommands::ExecuteCommandGetPlotFlag;
    m_pCommands[COMMAND_GET_POSITION] = &CSWVirtualMachineCommands::ExecuteCommandGetPosition;
    m_pCommands[COMMAND_GET_POSITION_FROM_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandLocationAccess;
    m_pCommands[COMMAND_GET_RACIAL_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetRacialType;
    m_pCommands[COMMAND_GET_RACIAL_SUB_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetRacialSubType;
    m_pCommands[COMMAND_GET_RANDOM_DESTINATION] = &CSWVirtualMachineCommands::ExecuteCommandGetRandomDestination;
    m_pCommands[COMMAND_GET_REFLEX_ADJUSTED_DAMAGE] = &CSWVirtualMachineCommands::ExecuteCommandGetReflexAdjustedDamage;
    m_pCommands[COMMAND_GET_REFLEX_SAVING_THROW] = &CSWVirtualMachineCommands::ExecuteCommandGetReflexSavingThrow;
    m_pCommands[COMMAND_GET_REPUTATION] = &CSWVirtualMachineCommands::ExecuteCommandGetReputation;
    m_pCommands[COMMAND_GET_PLAYER_RESTRICT_MODE] = &CSWVirtualMachineCommands::ExecuteCommandGetRestrictMode;
    m_pCommands[COMMAND_GET_SCRIPT_PARAMETER] = &CSWVirtualMachineCommands::ExecuteCommandGetScriptParameter;
    m_pCommands[COMMAND_SET_FADE_UNTIL_SCRIPT] = &CSWVirtualMachineCommands::ExecuteCommandSetFadeUntilScript;
    m_pCommands[COMMAND_GET_COMBAT_ACTIONS_PENDING] = &CSWVirtualMachineCommands::ExecuteCommandGetCombatActionPending;
    m_pCommands[COMMAND_GET_SELECTED_PLANET] = &CSWVirtualMachineCommands::ExecuteCommandGetGalaxyMapSelectedPlanet;
    m_pCommands[COMMAND_GET_SKILL_RANK] = &CSWVirtualMachineCommands::ExecuteCommandGetSkillRank;
    m_pCommands[COMMAND_GET_SOLO_MODE] = &CSWVirtualMachineCommands::ExecuteCommandGetSoloMode;
    m_pCommands[COMMAND_GET_SPELL_CAST_ITEM] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellCasterItem;
    m_pCommands[COMMAND_GET_SPELL_FORCE_POINT_COST] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellForcePointCost;
    m_pCommands[COMMAND_GET_FEAT_ACQUIRED] = &CSWVirtualMachineCommands::ExecuteCommandGetHasFeat;
    m_pCommands[COMMAND_GET_SPELL_ACQUIRED] = &CSWVirtualMachineCommands::ExecuteCommandGetHasSpell;
    m_pCommands[COMMAND_GRANT_FEAT] = &CSWVirtualMachineCommands::ExecuteCommandGrantAbility;
    m_pCommands[COMMAND_GRANT_SPELL] = &CSWVirtualMachineCommands::ExecuteCommandGrantAbility;
    m_pCommands[COMMAND_GET_SPELL_FORM_MASK] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellFormMask;
    m_pCommands[COMMAND_GET_SPELL_BASE_FORCE_POINT_COST] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellBaseForcePointCost;
    m_pCommands[COMMAND_GET_LAST_FORFEIT_VIOLATION] = &CSWVirtualMachineCommands::ExecuteCommandGetLastForfeitViolation;
    m_pCommands[COMMAND_GET_SCRIPT_STRING_PARAMETER] = &CSWVirtualMachineCommands::ExecuteCommandGetScriptStringParameter;
    m_pCommands[COMMAND_GET_SKILL_RANK_BASE] = &CSWVirtualMachineCommands::ExecuteCommandGetSkillRank;
    m_pCommands[COMMAND_MODIFY_REFLEX_SAVING_THROW_BASE] = &CSWVirtualMachineCommands::ExecuteCommandModifyBaseSavingThrow;
    m_pCommands[COMMAND_MODIFY_FORTITUDE_SAVING_THROW_BASE] = &CSWVirtualMachineCommands::ExecuteCommandModifyBaseSavingThrow;
    m_pCommands[COMMAND_MODIFY_WILL_SAVING_THROW_BASE] = &CSWVirtualMachineCommands::ExecuteCommandModifyBaseSavingThrow;
    m_pCommands[COMMAND_GET_OBJECT_PERSONAL_SPACE] = &CSWVirtualMachineCommands::ExecuteCommandGetObjectPersonalSpace;
    m_pCommands[COMMAND_ADJUST_CREATURE_ATTRIBUTES] = &CSWVirtualMachineCommands::ExecuteCommandAdjustCreatureAttributes;
    m_pCommands[COMMAND_SET_CREATURE_AI_LEVEL] = &CSWVirtualMachineCommands::ExecuteCommandSetCreatureAILevel;
    m_pCommands[COMMAND_RESET_CREATURE_AI_LEVEL] = &CSWVirtualMachineCommands::ExecuteCommandResetCreatureAILevel;
    m_pCommands[COMMAND_CHANGE_OBJECT_APPEARANCE] = &CSWVirtualMachineCommands::ExecuteCommandChangeObjectAppearance;
    m_pCommands[COMMAND_ADJUST_CREATURE_SKILLS] = &CSWVirtualMachineCommands::ExecuteCommandAdjustCreatureSkills;
    m_pCommands[COMMAND_CREATURE_FLOURISH_WEAPON] = &CSWVirtualMachineCommands::ExecuteCommandCreatureFlourishWeapon;
    m_pCommands[COMMAND_GET_SPELL_ID] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellId;
    m_pCommands[COMMAND_GET_SPELL_SAVE_DC] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellSaveDC;
    m_pCommands[COMMAND_GET_SPELL_TARGET_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellTargetLoc;
    m_pCommands[COMMAND_GET_SPELL_TARGET_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellTargetObject;
    m_pCommands[COMMAND_GET_SPELL_TARGET] = &CSWVirtualMachineCommands::ExecuteCommandGetSpellTarget;
    m_pCommands[COMMAND_GET_STANDARD_FACTION] = &CSWVirtualMachineCommands::ExecuteCommandGetStandardFaction;
    m_pCommands[COMMAND_GET_STARTING_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandModuleAccess;
    m_pCommands[COMMAND_GET_STEALTH_XP_DECREMENT] = &CSWVirtualMachineCommands::ExecuteCommandGetStealthXPDecrement;
    m_pCommands[COMMAND_GET_STEALTH_XP_ENABLED] = &CSWVirtualMachineCommands::ExecuteCommandGetStealthXPEnabled;
    m_pCommands[COMMAND_GET_STRING_BY_STR_REF] = &CSWVirtualMachineCommands::ExecuteCommandGetStrByStrRef;
    m_pCommands[COMMAND_GET_STRING_LEFT] = &CSWVirtualMachineCommands::ExecuteCommandGetStringLeft;
    m_pCommands[COMMAND_GET_STRING_LENGTH] = &CSWVirtualMachineCommands::ExecuteCommandGetStringLength;
    m_pCommands[COMMAND_GET_STRING_LOWER_CASE] = &CSWVirtualMachineCommands::ExecuteCommandGetStringLowerCase;
    m_pCommands[COMMAND_GET_STRING_RIGHT] = &CSWVirtualMachineCommands::ExecuteCommandGetStringRight;
    m_pCommands[COMMAND_GET_STRING_UPPER_CASE] = &CSWVirtualMachineCommands::ExecuteCommandGetStringUpperCase;
    m_pCommands[COMMAND_GET_STR_REF_SOUND_DURATION] = &CSWVirtualMachineCommands::ExecuteCommandGetStrRefSoundDuration;
    m_pCommands[COMMAND_GET_SUB_RACE] = &CSWVirtualMachineCommands::ExecuteCommandGetSubRace;
    m_pCommands[COMMAND_GET_SUB_STRING] = &CSWVirtualMachineCommands::ExecuteCommandGetSubString;
    m_pCommands[COMMAND_GET_SUB_SCREEN_ID] = &CSWVirtualMachineCommands::ExecuteCommandGetSubScreenID;
    m_pCommands[COMMAND_GET_TAG] = &CSWVirtualMachineCommands::ExecuteCommandGetTag;
    m_pCommands[COMMAND_GET_TIME_HOUR] = &CSWVirtualMachineCommands::ExecuteCommandGetTimeHour;
    m_pCommands[COMMAND_GET_TIME_MILLISECOND] = &CSWVirtualMachineCommands::ExecuteCommandGetTimeMillisecond;
    m_pCommands[COMMAND_GET_TIME_MINUTE] = &CSWVirtualMachineCommands::ExecuteCommandGetTimeMinute;
    m_pCommands[COMMAND_GET_TIME_SECOND] = &CSWVirtualMachineCommands::ExecuteCommandGetTimeSecond;
    m_pCommands[COMMAND_GET_TOTAL_DAMAGE_DEALT] = &CSWVirtualMachineCommands::ExecuteCommandGetTotalDamageDealt;
    m_pCommands[COMMAND_GET_TRANSITION_TARGET] = &CSWVirtualMachineCommands::ExecuteCommandGetWaypointByTrigger;
    m_pCommands[COMMAND_GET_TRAP_BASE_TYPE] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_GET_TRAP_CREATOR] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_GET_TRAP_DETECTABLE] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_GET_TRAP_DETECT_DC] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_GET_TRAP_DETECTED_BY] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_GET_TRAP_DISARMABLE] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_GET_TRAP_DISARM_DC] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_GET_TRAP_FLAGGED] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_GET_TRAP_KEY_TAG] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_GET_TRAP_ONE_SHOT] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_GET_PUP_OWNER] = &CSWVirtualMachineCommands::ExecuteCommandGetPUPOwner;
    m_pCommands[COMMAND_GET_TYPE_FROM_TALENT] = &CSWVirtualMachineCommands::ExecuteCommandTalentAccess;
    m_pCommands[COMMAND_GET_USER_ACTIONS_PENDING] = &CSWVirtualMachineCommands::ExecuteCommandGetUserActionsPending;
    m_pCommands[COMMAND_GET_USER_DEFINED_EVENT_NUMBER] = &CSWVirtualMachineCommands::ExecuteCommandGetUserDefinedEventNumber;
    m_pCommands[COMMAND_GET_WAS_FORCE_POWER_SUCCESSFUL] = &CSWVirtualMachineCommands::ExecuteCommandGetWasForcePowerSuccessful;
    m_pCommands[COMMAND_GET_WAYPOINT_BY_TAG] = &CSWVirtualMachineCommands::ExecuteCommandGetWaypointByTag;
    m_pCommands[COMMAND_GET_WEAPON_RANGED] = &CSWVirtualMachineCommands::ExecuteCommandGetWeaponRanged;
    m_pCommands[COMMAND_GET_WILL_SAVING_THROW] = &CSWVirtualMachineCommands::ExecuteCommandGetWillSavingThrow;
    m_pCommands[COMMAND_GET_XP] = &CSWVirtualMachineCommands::ExecuteCommandGetXP;
    m_pCommands[COMMAND_GIVE_GOLD_TO_CREATURE] = &CSWVirtualMachineCommands::ExecuteCommandGiveGoldToCreature;
    m_pCommands[COMMAND_GIVE_ITEM] = &CSWVirtualMachineCommands::ExecuteCommandGiveItem;
    m_pCommands[COMMAND_GIVE_PLOT_XP] = &CSWVirtualMachineCommands::ExecuteCommandGivePlotXP;
    m_pCommands[COMMAND_GIVE_XP_TO_CREATURE] = &CSWVirtualMachineCommands::ExecuteCommandGiveXPToCreature;
    m_pCommands[COMMAND_HAS_LINE_OF_SIGHT] = &CSWVirtualMachineCommands::ExecuteCommandHasLineOfSight;
    m_pCommands[COMMAND_HOURS_TO_SECONDS] = &CSWVirtualMachineCommands::ExecuteCommandTimeConversions;
    m_pCommands[COMMAND_HOLD_WORLD_FADE_IN_FOR_DIALOG] = &CSWVirtualMachineCommands::ExecuteCommandHoldWorldFadeInForDialog;
    m_pCommands[COMMAND_INSERT_STRING] = &CSWVirtualMachineCommands::ExecuteCommandInsertString;
    m_pCommands[COMMAND_INT_TO_FLOAT] = &CSWVirtualMachineCommands::ExecuteCommandIntToFloat;
    m_pCommands[COMMAND_INT_TO_HEX_STRING] = &CSWVirtualMachineCommands::ExecuteCommandIntToHexString;
    m_pCommands[COMMAND_INT_TO_STRING] = &CSWVirtualMachineCommands::ExecuteCommandIntToString;
    m_pCommands[COMMAND_IS_AVAILABLE_NPC] = &CSWVirtualMachineCommands::ExecuteCommandIsAvailableNPC;
    m_pCommands[COMMAND_IS_CREDIT_SEQUENCE_IN_PROGRESS] = &CSWVirtualMachineCommands::ExecuteCommandIsCreditSequenceInProgress;
    m_pCommands[COMMAND_GET_IS_DEAD] = &CSWVirtualMachineCommands::ExecuteCommandIsAIState;
    m_pCommands[COMMAND_GET_IS_EFFECT_VALID] = &CSWVirtualMachineCommands::ExecuteCommandIsEffectValid;
    m_pCommands[COMMAND_IS_MOVIE_PLAYING] = &CSWVirtualMachineCommands::ExecuteCommandIsMoviePlaying;
    m_pCommands[COMMAND_IS_NPC_PARTY_MEMBER] = &CSWVirtualMachineCommands::ExecuteCommandIsNPCPartyMember;
    m_pCommands[COMMAND_IS_OBJECT_PARTY_MEMBER] = &CSWVirtualMachineCommands::ExecuteCommandIsObjectPartyMember;
    m_pCommands[COMMAND_GET_IS_PC] = &CSWVirtualMachineCommands::ExecuteCommandIsPC;
    m_pCommands[COMMAND_IS_RUNNING] = &CSWVirtualMachineCommands::ExecuteCommandIsRunning;
    m_pCommands[COMMAND_INCREMENT_GLOBAL_NUMBER] = &CSWVirtualMachineCommands::ExecuteCommandModifyGlobal;
    m_pCommands[COMMAND_IS_STEALTHED] = &CSWVirtualMachineCommands::ExecuteCommandIsStealthed;
    m_pCommands[COMMAND_IS_MEDITATING] = &CSWVirtualMachineCommands::ExecuteCommandIsMeditating;
    m_pCommands[COMMAND_IS_IN_TOTAL_DEFENSE] = &CSWVirtualMachineCommands::ExecuteCommandIsInTotalDefense;
    m_pCommands[COMMAND_IS_FORM_ACTIVE] = &CSWVirtualMachineCommands::ExecuteCommandIsFormActive;
    m_pCommands[COMMAND_JUMP_TO_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandActionJumpToPoint;
    m_pCommands[COMMAND_JUMP_TO_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandActionJumpToObject;
    m_pCommands[COMMAND_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandLocation;
    m_pCommands[COMMAND_LOG] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_MAGICAL_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandSetSubType;
    m_pCommands[COMMAND_MUSIC_BACKGROUND_CHANGE_DAY] = &CSWVirtualMachineCommands::ExecuteCommandMusicBackground;
    m_pCommands[COMMAND_MUSIC_BACKGROUND_CHANGE_NIGHT] = &CSWVirtualMachineCommands::ExecuteCommandMusicBackground;
    m_pCommands[COMMAND_MUSIC_BACKGROUND_GET_BATTLE_TRACK] = &CSWVirtualMachineCommands::ExecuteCommandGetBattleTrack;
    m_pCommands[COMMAND_MUSIC_BACKGROUND_PLAY] = &CSWVirtualMachineCommands::ExecuteCommandMusicBackground;
    m_pCommands[COMMAND_MUSIC_BACKGROUND_SET_DELAY] = &CSWVirtualMachineCommands::ExecuteCommandMusicBackground;
    m_pCommands[COMMAND_MUSIC_BACKGROUND_STOP] = &CSWVirtualMachineCommands::ExecuteCommandMusicBackground;
    m_pCommands[COMMAND_MUSIC_BATTLE_CHANGE] = &CSWVirtualMachineCommands::ExecuteCommandMusicBattle;
    m_pCommands[COMMAND_MUSIC_BATTLE_PLAY] = &CSWVirtualMachineCommands::ExecuteCommandMusicBattle;
    m_pCommands[COMMAND_MUSIC_BATTLE_STOP] = &CSWVirtualMachineCommands::ExecuteCommandMusicBattle;
    m_pCommands[COMMAND_NO_CLICKS_FOR] = &CSWVirtualMachineCommands::ExecuteCommandNoClicksFor;
    m_pCommands[COMMAND_OBJECT_TO_STRING] = &CSWVirtualMachineCommands::ExecuteCommandObjectToString;
    m_pCommands[COMMAND_OPEN_STORE] = &CSWVirtualMachineCommands::ExecuteCommandOpenStore;
    m_pCommands[COMMAND_PAUSE_GAME] = &CSWVirtualMachineCommands::ExecuteCommandPauseGame;
    m_pCommands[COMMAND_PLAY_ANIMATION] = &CSWVirtualMachineCommands::ExecuteCommandPlayAnimation;
    m_pCommands[COMMAND_PLAY_MOVIE] = &CSWVirtualMachineCommands::ExecuteCommandPlayMovie;
    m_pCommands[COMMAND_PLAY_MOVIE_QUEUE] = &CSWVirtualMachineCommands::ExecuteCommandPlayMovieQueue;
    m_pCommands[COMMAND_PLAY_PAZAAK] = &CSWVirtualMachineCommands::ExecuteCommandPlayPazaak;
    m_pCommands[COMMAND_PLAY_ROOM_ANIMATION] = &CSWVirtualMachineCommands::ExecuteCommandPlayRoomAnimation;
    m_pCommands[COMMAND_PLAY_RUMBLE_PATTERN] = &CSWVirtualMachineCommands::ExecuteCommandPlayRumblePattern;
    m_pCommands[COMMAND_PLAY_SOUND] = &CSWVirtualMachineCommands::ExecuteCommandPlaySound;
    m_pCommands[COMMAND_PLAY_VISUAL_AREA_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandPlayVisualAreaEffect;
    m_pCommands[COMMAND_POP_UP_DEATH_GUI_PANEL] = &CSWVirtualMachineCommands::ExecuteCommandPopUpGUIPanel;
    m_pCommands[COMMAND_POP_UP_GUI_PANEL] = &CSWVirtualMachineCommands::ExecuteCommandPopUpGUIPanel;
    m_pCommands[COMMAND_POW] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_PRINT_FLOAT] = &CSWVirtualMachineCommands::ExecuteCommandPrintFloat;
    m_pCommands[COMMAND_PRINT_INTEGER] = &CSWVirtualMachineCommands::ExecuteCommandPrintInteger;
    m_pCommands[COMMAND_PRINT_LOG_ENTRY] = &CSWVirtualMachineCommands::ExecuteCommandPrintLogEntry;
    m_pCommands[COMMAND_PRINT_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandPrintObject;
    m_pCommands[COMMAND_CONTEXT_SERVER] = &CSWVirtualMachineCommands::ExecuteCommandPrintString;
    m_pCommands[COMMAND_PRINT_VECTOR] = &CSWVirtualMachineCommands::ExecuteCommandPrintVector;
    m_pCommands[COMMAND_PLAY_OVERLAY_ANIMATION] = &CSWVirtualMachineCommands::ExecuteCommandPlayOverlayAnimation;
    m_pCommands[COMMAND_QUEUE_MOVIE] = &CSWVirtualMachineCommands::ExecuteCommandQueueMovie;
    m_pCommands[COMMAND_RANDOM] = &CSWVirtualMachineCommands::ExecuteCommandRandom;
    m_pCommands[COMMAND_RANDOM_NAME] = &CSWVirtualMachineCommands::ExecuteCommandRandomName;
    m_pCommands[COMMAND_REBUILD_PARTY_TABLE] = &CSWVirtualMachineCommands::ExecuteCommandRebuildPartyTable;
    m_pCommands[COMMAND_REFLEX_SAVE] = &CSWVirtualMachineCommands::ExecuteCommandSavingThrow;
    m_pCommands[COMMAND_REMOVE_AVAILABLE_NPC] = &CSWVirtualMachineCommands::ExecuteCommandRemoveAvailableNPC;
    m_pCommands[COMMAND_REMOVE_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandRemoveEffect;
    m_pCommands[COMMAND_REMOVE_EFFECT_BY_ID] = &CSWVirtualMachineCommands::ExecuteCommandRemoveEffectByID;
    m_pCommands[COMMAND_REMOVE_EFFECT_BY_EXACT_MATCH] = &CSWVirtualMachineCommands::ExecuteCommandRemoveEffectByExactMatch;
    m_pCommands[COMMAND_REMOVE_FROM_PARTY] = &CSWVirtualMachineCommands::ExecuteCommandRemoveFromParty;
    m_pCommands[COMMAND_REMOVE_HEARTBEAT] = &CSWVirtualMachineCommands::ExecuteCommandRemoveHeartbeat;
    m_pCommands[COMMAND_REMOVE_JOURNAL_QUEST_ENTRY] = &CSWVirtualMachineCommands::ExecuteCommandRemoveJournalQuestEntry;
    m_pCommands[COMMAND_REMOVE_NPC_FROM_PARTY_TO_BASE] = &CSWVirtualMachineCommands::ExecuteCommandRemoveNPCFromPartyToBase;
    m_pCommands[COMMAND_REMOVE_PARTY_MEMBER] = &CSWVirtualMachineCommands::ExecuteCommandRemovePartyMember;
    m_pCommands[COMMAND_RESET_DIALOG_STATE] = &CSWVirtualMachineCommands::ExecuteCommandResetDialogState;
    m_pCommands[COMMAND_RESIST_SPELL] = &CSWVirtualMachineCommands::ExecuteCommandResistSpell;
    m_pCommands[COMMAND_REVEAL_MAP] = &CSWVirtualMachineCommands::ExecuteCommandRevealMap;
    m_pCommands[COMMAND_ROUNDS_TO_SECONDS] = &CSWVirtualMachineCommands::ExecuteCommandTimeConversions;
    m_pCommands[COMMAND_SAVE_NPC_STATE] = &CSWVirtualMachineCommands::ExecuteCommandSaveNPCState;
    m_pCommands[COMMAND_SAVE_NPC_BY_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandSaveNPCByObject;
    m_pCommands[COMMAND_SAVE_PUP_BY_OBJECT] = &CSWVirtualMachineCommands::ExecuteCommandSavePUPByObject;
    m_pCommands[COMMAND_SEND_MESSAGE_TO_PC] = &CSWVirtualMachineCommands::ExecuteCommandSendMessageToPC;
    m_pCommands[COMMAND_SET_AREA_FOG_COLOR] = &CSWVirtualMachineCommands::ExecuteCommandSetAreaFogColor;
    m_pCommands[COMMAND_SET_AREA_TRANSITION_BMP] = &CSWVirtualMachineCommands::ExecuteCommandSetAreaTransitionBMP;
    m_pCommands[COMMAND_SET_AREA_UNESCAPABLE] = &CSWVirtualMachineCommands::ExecuteCommandSetAreaUnescapable;
    m_pCommands[COMMAND_SET_ASSOCIATE_LISTEN_PATTERNS] = &CSWVirtualMachineCommands::ExecuteCommandSetAssociateListenPatterns;
    m_pCommands[COMMAND_SET_AVAILABLE_NPC_ID] = &CSWVirtualMachineCommands::ExecuteCommandSetAvailableNPCID;
    m_pCommands[COMMAND_SET_BUTTON_MASH_CHECK] = &CSWVirtualMachineCommands::ExecuteCommandSetButtonMashCheck;
    m_pCommands[COMMAND_SET_CAMERA_FACING] = &CSWVirtualMachineCommands::ExecuteCommandSetCameraLocation;
    m_pCommands[COMMAND_SET_CAMERA_MODE] = &CSWVirtualMachineCommands::ExecuteCommandSetCameraMode;
    m_pCommands[COMMAND_SET_FAKE_COMBAT_STATE] = &CSWVirtualMachineCommands::ExecuteCommandSetFakeCombatState;
    m_pCommands[COMMAND_SET_COMMANDABLE] = &CSWVirtualMachineCommands::ExecuteCommandSetCommandable;
    m_pCommands[COMMAND_SET_CURRENT_FORM] = &CSWVirtualMachineCommands::ExecuteCommandSetCurrentForm;
    m_pCommands[COMMAND_SET_CURRENT_STEALTH_XP] = &CSWVirtualMachineCommands::ExecuteCommandSetCurrentStealthXP;
    m_pCommands[COMMAND_SET_CUSTOM_TOKEN] = &CSWVirtualMachineCommands::ExecuteCommandSetCustomToken;
    m_pCommands[COMMAND_SET_DIALOG_PLACEABLE_CAMERA] = &CSWVirtualMachineCommands::ExecuteCommandSetDialogPlaceableCamera;
    m_pCommands[COMMAND_SET_EFFECT_ICON] = &CSWVirtualMachineCommands::ExecuteCommandSetEffectIcon;
    m_pCommands[COMMAND_SET_ENCOUNTER_ACTIVE] = &CSWVirtualMachineCommands::ExecuteCommandSetEncounterData;
    m_pCommands[COMMAND_SET_ENCOUNTER_DIFFICULTY] = &CSWVirtualMachineCommands::ExecuteCommandSetEncounterData;
    m_pCommands[COMMAND_SET_ENCOUNTER_SPAWNS_CURRENT] = &CSWVirtualMachineCommands::ExecuteCommandSetEncounterData;
    m_pCommands[COMMAND_SET_ENCOUNTER_SPAWNS_MAX] = &CSWVirtualMachineCommands::ExecuteCommandSetEncounterData;
    m_pCommands[COMMAND_SET_FACING] = &CSWVirtualMachineCommands::ExecuteCommandSetFacing;
    m_pCommands[COMMAND_SET_FACING_POINT] = &CSWVirtualMachineCommands::ExecuteCommandSetFacing;
    m_pCommands[COMMAND_SET_GLOBAL_FADE_IN] = &CSWVirtualMachineCommands::ExecuteCommandSetFadeIn;
    m_pCommands[COMMAND_SET_GLOBAL_FADE_OUT] = &CSWVirtualMachineCommands::ExecuteCommandSetFadeOut;
    m_pCommands[COMMAND_SET_FORCE_POWER_UNSUCCESSFUL] = &CSWVirtualMachineCommands::ExecuteCommandSetForcePowerUnsuccessful;
    m_pCommands[COMMAND_SET_FORMATION] = &CSWVirtualMachineCommands::ExecuteCommandSetFormation;
    m_pCommands[COMMAND_SET_GLOBAL_BOOLEAN] = &CSWVirtualMachineCommands::ExecuteCommandSetGlobalNumber;
    m_pCommands[COMMAND_SET_GLOBAL_LOCATION] = &CSWVirtualMachineCommands::ExecuteCommandSetGlobalLocation;
    m_pCommands[COMMAND_SET_GLOBAL_NUMBER] = &CSWVirtualMachineCommands::ExecuteCommandSetGlobalNumber;
    m_pCommands[COMMAND_SET_GLOBAL_STRING] = &CSWVirtualMachineCommands::ExecuteCommandSetGlobalString;
    m_pCommands[COMMAND_SET_GOOD_EVIL_VALUE] = &CSWVirtualMachineCommands::ExecuteCommandSetGoodEvilValue;
    m_pCommands[COMMAND_SET_HEAL_TARGET] = &CSWVirtualMachineCommands::ExecuteCommandSetHealTarget;
    m_pCommands[COMMAND_SET_IDENTIFIED] = &CSWVirtualMachineCommands::ExecuteCommandSetIdentified;
    m_pCommands[COMMAND_SET_INPUT_CLASS] = &CSWVirtualMachineCommands::ExecuteCommandSetInputClass;
    m_pCommands[COMMAND_SET_IS_DESTROYABLE] = &CSWVirtualMachineCommands::ExecuteCommandSetIsDestroyable;
    m_pCommands[COMMAND_SET_ITEM_NON_EQUIPPABLE] = &CSWVirtualMachineCommands::ExecuteCommandSetItemNonEquippable;
    m_pCommands[COMMAND_SET_ITEM_STACK_SIZE] = &CSWVirtualMachineCommands::ExecuteCommandSetItemStackSize;
    m_pCommands[COMMAND_SET_JOURNAL_QUEST_ENTRY_PICTURE] = &CSWVirtualMachineCommands::ExecuteCommandSetJournalQuestEntryPicture;
    m_pCommands[COMMAND_SET_KEEP_STEALTH_IN_DIALOG] = &CSWVirtualMachineCommands::ExecuteCommandSetKeepStealthInDialog;
    m_pCommands[COMMAND_SET_LIGHTSABER_POWERED] = &CSWVirtualMachineCommands::ExecuteCommandSetLightsaberPowered;
    m_pCommands[COMMAND_SET_LISTENING] = &CSWVirtualMachineCommands::ExecuteCommandSetListening;
    m_pCommands[COMMAND_SET_LISTEN_PATTERN] = &CSWVirtualMachineCommands::ExecuteCommandSetListenString;
    m_pCommands[COMMAND_SET_LOCAL_BOOLEAN] = &CSWVirtualMachineCommands::ExecuteCommandSetLocalValue;
    m_pCommands[COMMAND_SET_LOCAL_NUMBER] = &CSWVirtualMachineCommands::ExecuteCommandSetLocalValue;
    m_pCommands[COMMAND_SET_LOCKED] = &CSWVirtualMachineCommands::ExecuteCommandSetLocked;
    m_pCommands[COMMAND_SET_LOCK_ORIENTATION_IN_DIALOG] = &CSWVirtualMachineCommands::ExecuteCommandSetLockOrientationInDialog;
    m_pCommands[COMMAND_SET_LOCK_HEAD_FOLLOW_IN_DIALOG] = &CSWVirtualMachineCommands::ExecuteCommandSetLockHeadFollowInDialog;
    m_pCommands[COMMAND_SET_NPC_SELECTABILITY] = &CSWVirtualMachineCommands::ExecuteCommandSetNPCSelectability;
    m_pCommands[COMMAND_SET_MAP_PIN_ENABLED] = &CSWVirtualMachineCommands::ExecuteCommandSetMapPinEnabled;
    m_pCommands[COMMAND_SET_MAX_HIT_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandSetMaxHitPoints;
    m_pCommands[COMMAND_SET_MAX_STEALTH_XP] = &CSWVirtualMachineCommands::ExecuteCommandSetMaxStealthXP;
    m_pCommands[COMMAND_SET_TRAP_DETECTED_BY] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_SET_TRAP_DISABLED] = &CSWVirtualMachineCommands::ExecuteCommandGetMineInfo;
    m_pCommands[COMMAND_SET_MIN_ONE_HP] = &CSWVirtualMachineCommands::ExecuteCommandSetMinOneHP;
    m_pCommands[COMMAND_SET_MUSIC_VOLUME] = &CSWVirtualMachineCommands::ExecuteCommandSetMusicVolume;
    m_pCommands[COMMAND_SET_NPCAI_STYLE] = &CSWVirtualMachineCommands::ExecuteCommandSetNPCAIStyle;
    m_pCommands[COMMAND_SET_PARTY_AI_STYLE] = &CSWVirtualMachineCommands::ExecuteCommandSetPartyAIStyle;
    m_pCommands[COMMAND_SET_PARTY_LEADER] = &CSWVirtualMachineCommands::ExecuteCommandSetPartyLeader;
    m_pCommands[COMMAND_SET_PLACEABLE_ILLUMINATION] = &CSWVirtualMachineCommands::ExecuteCommandSetPlaceableIllumination;
    m_pCommands[COMMAND_SET_PLANET_AVAILABLE] = &CSWVirtualMachineCommands::ExecuteCommandSetGalaxyMapPlanetAvailable;
    m_pCommands[COMMAND_SET_PLANET_SELECTABLE] = &CSWVirtualMachineCommands::ExecuteCommandSetGalaxyMapPlanetSelectable;
    m_pCommands[COMMAND_SET_PLOT_FLAG] = &CSWVirtualMachineCommands::ExecuteCommandSetPlotFlag;
    m_pCommands[COMMAND_SET_PLAYER_RESTRICT_MODE] = &CSWVirtualMachineCommands::ExecuteCommandSetRestrictMode;
    m_pCommands[COMMAND_SET_RETURN_STRREF] = &CSWVirtualMachineCommands::ExecuteCommandSetReturnStrref;
    m_pCommands[COMMAND_SET_SOLO_MODE] = &CSWVirtualMachineCommands::ExecuteCommandSetSoloMode;
    m_pCommands[COMMAND_SWMG_SET_SPEED_BLUR_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandSetSpeedBlurEffect;
    m_pCommands[COMMAND_SET_TIME] = &CSWVirtualMachineCommands::ExecuteCommandSetTime;
    m_pCommands[COMMAND_SET_TUTORIAL_WINDOWS_ENABLED] = &CSWVirtualMachineCommands::ExecuteCommandSetTutorialWindowsEnabled;
    m_pCommands[COMMAND_ENABLE_RAIN] = &CSWVirtualMachineCommands::ExecuteCommandEnableRain;
    m_pCommands[COMMAND_SET_XP] = &CSWVirtualMachineCommands::ExecuteCommandSetXP;
    m_pCommands[COMMAND_SHIP_BUILD] = &CSWVirtualMachineCommands::ExecuteCommandShipBuild;
    m_pCommands[COMMAND_SHOW_DEMO_SCREEN] = &CSWVirtualMachineCommands::ExecuteCommandShowDemoScreen;
    m_pCommands[COMMAND_SHOW_GALAXY_MAP_GUI] = &CSWVirtualMachineCommands::ExecuteCommandShowGalaxyMapGui;
    m_pCommands[COMMAND_SHOW_LEVEL_UP_GUI] = &CSWVirtualMachineCommands::ExecuteCommandShowLevelUpGUI;
    m_pCommands[COMMAND_SHOW_PARTY_SELECTION_GUI] = &CSWVirtualMachineCommands::ExecuteCommandShowPartySelection;
    m_pCommands[COMMAND_SHOW_TUTORIAL_WINDOW] = &CSWVirtualMachineCommands::ExecuteCommandShowTutorialWindow;
    m_pCommands[COMMAND_SHOW_UPGRADE_SCREEN] = &CSWVirtualMachineCommands::ExecuteCommandShowUpgradeScreen;
    m_pCommands[COMMAND_SHOW_CHEMICAL_UPGRADE_SCREEN] = &CSWVirtualMachineCommands::ExecuteCommandShowChemicalUpgradeScreen;
    m_pCommands[COMMAND_SHOW_SWOOP_UPGRADE_SCREEN] = &CSWVirtualMachineCommands::ExecuteCommandShowSwoopUpgradeScreen;
    m_pCommands[COMMAND_SIGNAL_EVENT] = &CSWVirtualMachineCommands::ExecuteCommandSignalEvent;
    m_pCommands[COMMAND_SIN] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_SOUND_OBJECT_GET_FIXED_VARIANCE] = &CSWVirtualMachineCommands::ExecuteCommandSoundObjectGetFixedVariance;
    m_pCommands[COMMAND_SOUND_OBJECT_GET_PITCH_VARIANCE] = &CSWVirtualMachineCommands::ExecuteCommandSoundObjectGetPitchVariation;
    m_pCommands[COMMAND_SOUND_OBJECT_GET_VOLUME] = &CSWVirtualMachineCommands::ExecuteCommandSoundObjectGetVolume;
    m_pCommands[COMMAND_SOUND_OBJECT_PLAY] = &CSWVirtualMachineCommands::ExecuteCommandSoundObjectPlay;
    m_pCommands[COMMAND_SOUND_OBJECT_SET_FIXED_VARIANCE] = &CSWVirtualMachineCommands::ExecuteCommandSoundObjectSetFixedVariance;
    m_pCommands[COMMAND_SOUND_OBJECT_SET_PITCH_VARIANCE] = &CSWVirtualMachineCommands::ExecuteCommandSoundObjectSetPitchVariation;
    m_pCommands[COMMAND_SOUND_OBJECT_SET_POSITION] = &CSWVirtualMachineCommands::ExecuteCommandSoundObjectSetPosition;
    m_pCommands[COMMAND_SOUND_OBJECT_SET_VOLUME] = &CSWVirtualMachineCommands::ExecuteCommandSoundObjectSetVolume;
    m_pCommands[COMMAND_SOUND_OBJECT_STOP] = &CSWVirtualMachineCommands::ExecuteCommandSoundObjectStop;
    m_pCommands[COMMAND_SOUND_OBJECT_FADE_AND_STOP] = &CSWVirtualMachineCommands::ExecuteCommandSoundObjectFadeAndStop;
    m_pCommands[COMMAND_SPAWN_AVAILABLE_NPC] = &CSWVirtualMachineCommands::ExecuteCommandSpawnAvailableNPC;
    m_pCommands[COMMAND_SPAWN_AVAILABLE_PUP] = &CSWVirtualMachineCommands::ExecuteCommandSpawnAvailablePUP;
    m_pCommands[COMMAND_SPEAK_ONE_LINER_CONVERSATION] = &CSWVirtualMachineCommands::ExecuteCommandSpeakOneLinerConversation;
    m_pCommands[COMMAND_SPEAK_STRING] = &CSWVirtualMachineCommands::ExecuteCommandSpeakString;
    m_pCommands[COMMAND_SQRT] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_START_NEW_MODULE] = &CSWVirtualMachineCommands::ExecuteCommandStartNewModule;
    m_pCommands[COMMAND_SET_STEALTH_XP_DECREMENT] = &CSWVirtualMachineCommands::ExecuteCommandSetStealthXPDecrement;
    m_pCommands[COMMAND_SET_STEALTH_XP_ENABLED] = &CSWVirtualMachineCommands::ExecuteCommandSetStealthXPEnabled;
    m_pCommands[COMMAND_START_CREDIT_SEQUENCE] = &CSWVirtualMachineCommands::ExecuteCommandStartCreditSequence;
    m_pCommands[COMMAND_STOP_RUMBLE_PATTERN] = &CSWVirtualMachineCommands::ExecuteCommandStopRumblePattern;
    m_pCommands[COMMAND_STRING_TO_FLOAT] = &CSWVirtualMachineCommands::ExecuteCommandStringConversions;
    m_pCommands[COMMAND_STRING_TO_INT] = &CSWVirtualMachineCommands::ExecuteCommandStringConversions;
    m_pCommands[COMMAND_SUPERNATURAL_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandSetSubType;
    m_pCommands[COMMAND_SUPPRESS_STATUS_SUMMARY_ENTRY] = &CSWVirtualMachineCommands::ExecuteCommandSuppressStatusSummaryEntry;
    m_pCommands[COMMAND_CHANGE_BY_FACTION_AND_PACIFY] = &CSWVirtualMachineCommands::ExecuteCommandChangeByFaction;
    m_pCommands[COMMAND_SURRENDER_RETAIN_BUFFS] = &CSWVirtualMachineCommands::ExecuteCommandSurrenderToEnemies;
    m_pCommands[COMMAND_SURRENDER_TO_ENEMIES] = &CSWVirtualMachineCommands::ExecuteCommandSurrenderToEnemies;
    m_pCommands[COMMAND_SWITCH_PLAYER_CREATURE] = &CSWVirtualMachineCommands::ExecuteCommandSwitchPlayerCreature;
    m_pCommands[COMMAND_SPAWN_MINE] = &CSWVirtualMachineCommands::ExecuteCommandSpawnMine;
    m_pCommands[COMMAND_SET_FORFEIT_CONDITIONS] = &CSWVirtualMachineCommands::ExecuteCommandSetForfeitConditions;
    m_pCommands[COMMAND_SET_ORIENT_ON_CLICK] = &CSWVirtualMachineCommands::ExecuteCommandSetOrientOnClick;
    m_pCommands[COMMAND_SET_DISABLE_TRANSIT] = &CSWVirtualMachineCommands::ExecuteCommandSetDisableTransit;
    m_pCommands[COMMAND_SET_BONUS_FORCE_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandSetBonusForcePoints;
    m_pCommands[COMMAND_ADD_BONUS_FORCE_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandAddBonusForcePoints;
    m_pCommands[COMMAND_GET_BONUS_FORCE_POINTS] = &CSWVirtualMachineCommands::ExecuteCommandGetBonusForcePoints;
    m_pCommands[COMMAND_TAKE_GOLD_FROM_CREATURE] = &CSWVirtualMachineCommands::ExecuteCommandTakeGoldFromCreature;
    m_pCommands[COMMAND_TALENT_FEAT] = &CSWVirtualMachineCommands::ExecuteCommandTalent;
    m_pCommands[COMMAND_TALENT_SKILL] = &CSWVirtualMachineCommands::ExecuteCommandTalent;
    m_pCommands[COMMAND_TALENT_SPELL] = &CSWVirtualMachineCommands::ExecuteCommandTalent;
    m_pCommands[COMMAND_TAN] = &CSWVirtualMachineCommands::ExecuteCommandMath;
    m_pCommands[COMMAND_TEST_STRING_AGAINST_PATTERN] = &CSWVirtualMachineCommands::ExecuteCommandTestString;
    m_pCommands[COMMAND_TOUCH_ATTACK_MELEE] = &CSWVirtualMachineCommands::ExecuteCommandDoTouchAttack;
    m_pCommands[COMMAND_TOUCH_ATTACK_RANGED] = &CSWVirtualMachineCommands::ExecuteCommandDoTouchAttack;
    m_pCommands[COMMAND_TURNS_TO_SECONDS] = &CSWVirtualMachineCommands::ExecuteCommandTimeConversions;
    m_pCommands[COMMAND_UNLOCK_ALL_SONGS] = &CSWVirtualMachineCommands::ExecuteCommandUnlockAllMusic;
    m_pCommands[COMMAND_VECTOR] = &CSWVirtualMachineCommands::ExecuteCommandVector;
    m_pCommands[COMMAND_VECTOR_MAGNITUDE] = &CSWVirtualMachineCommands::ExecuteCommandVectorMagnitude;
    m_pCommands[COMMAND_VECTOR_NORMALIZE] = &CSWVirtualMachineCommands::ExecuteCommandVectorNormalize;
    m_pCommands[COMMAND_VECTOR_TO_ANGLE] = &CSWVirtualMachineCommands::ExecuteCommandVectorConversions;
    m_pCommands[COMMAND_VERSUS_ALIGNMENT_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandVersusEffect;
    m_pCommands[COMMAND_VERSUS_RACIAL_TYPE_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandVersusEffect;
    m_pCommands[COMMAND_VERSUS_TRAP_EFFECT] = &CSWVirtualMachineCommands::ExecuteCommandVersusEffect;
    m_pCommands[COMMAND_ACTION_WAIT] = &CSWVirtualMachineCommands::ExecuteCommandWait;
    m_pCommands[COMMAND_WILL_SAVE] = &CSWVirtualMachineCommands::ExecuteCommandSavingThrow;
    m_pCommands[COMMAND_YARDS_TO_METERS] = &CSWVirtualMachineCommands::ExecuteCommandDistanceConversions;
    m_pCommands[COMMAND_YAVIN_HACK_CLOSE_DOOR] = &CSWVirtualMachineCommands::ExecuteCommandYavinHackCloseDoor;
    m_pCommands[COMMAND_GET_IS_PLAYER_MADE_CHARACTER] = &CSWVirtualMachineCommands::ExecuteCommandGetIsPlayerMadeCharacter;

    InitializeSWMGCommands();
}

static inline void NwvmachinecommandsTrace1()
{
}

#ifdef _XBOX

int CSWVirtualMachineCommands::ExecuteCommandIsCreditSequenceInProgress(
    int nCommand, int nArgs)
{
    int bInProgress;

    bInProgress = g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bCreditSequenceInProgress;
    return g_pVirtualMachine->StackPushInteger(bInProgress) != 0 ? 0 : -2000;
}

int CSWVirtualMachineCommands::ExecuteCommandStartCreditSequence(
    int nCommand, int nArgs)
{
    int bTransparentBackground;

    CExoString sMusic;

    if (g_pVirtualMachine->StackPopInteger(&bTransparentBackground) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (g_pVirtualMachine->StackPopString(&sMusic) == 0)
    {
        return ERROR_STACK_POP;
    }
    if (bTransparentBackground)
    {
        Vector vColor(-1.0f, -1.0f, -1.0f);
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->GetInGameGui()->StartCreditSequence(vColor, sMusic);
    }
    else
    {
        Vector vColor(0.0f, 0.0f, 0.0f);
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->GetInGameGui()->StartCreditSequence(vColor, sMusic);
    }
    return 0;
}
#include <stdio.h>
#include "aurconsole.h"

static void runscript(char* szArgs)
{
    CExoString sScript;
    char szScript[20];
    sscanf(szArgs, "%s %d", szScript, &g_nRunScriptVar);
    sScript = szScript;
    g_pVirtualMachine->RunScript(&sScript, 0, 0);
}

ConsoleFunc ConVbDisableScriptLogging("bDisableScriptLogging", &bDisableScriptLogging);
ConsoleFunc ConFrunscript("runscript", runscript);
ConsoleFunc ConVbShipBuild("bShipBuild", &bShipBuild);
#endif
