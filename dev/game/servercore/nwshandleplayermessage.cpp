#include "exobase.h"
#include "exoresman.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "serverexoappinternal.h"
#include "netlayer.h"
#include "nwsobject.h"
#include "nwsarea.h"
#include "gameobjectarray.h"
#include "nwsplayer.h"
#include "nwsplaceable.h"
#include "pathfindinfo.h"
#include "nwsitem.h"
#include "nwscombatround.h"
#include "nwupdatestats.h"
#include "nwsscriptevent.h"

#define FORFEIT_NO_FORCE_POWERS 1
#define USER_DEFINED_EVENT_FORFEIT_VIOLATION 4001

#include "nwseffect.h"
#include "nwseffectlist.h"
#include "serveraimaster.h"
#include "nwsmodule.h"
#include "nwsparty.h"
#include "partytables.h"
#include "worldtimer.h"
#include "nwscript.h"
#include "nwcreaturelevelstats.h"
#include "nwscreaturestats.h"
#include "nwmessage.h"
#include "nwscreature.h"
#include "nwsdoor.h"
#include "nwsencounter.h"
#include "nwstrigger.h"
#include "serverinfo.h"
#include "nwrules.h"
#include "nwspell.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "swparty.h"
#include "exotlktable.h"
#include <math.h>
#include <stdlib.h>
#include "nwclass.h"

static inline void NwshandleplayermessageTrace1();

static const int nVolume = 1;

const int CSWSMessage::GUI_PANEL_PLAYER_DEATH = 0;
const int CSWSMessage::GUI_PANEL_PARTY_INVITATION_RECEIVED = 1;
// PC 0x005A1E10 GOG 0x005A1E80
CSWSMessage::CSWSMessage()
{
}
// PC 0x005A1E30 GOG 0x005A1EA0
CSWSMessage::~CSWSMessage()
{
}

void ResetPlayerInventoryLastUpdate(CSWSPlayer* pPlayer)
{
    CSWSPlayerLUOInventory* pLastUpdate =
        pPlayer->m_pInventoryGUI->m_pLastUpdateObject;
    pLastUpdate->ClearSlots();
}
// PC 0x005A1E40 GOG 0x005A1EB0
int CSWSMessage::HandlePlayerToServerQuickChatMessage(CSWSPlayer* pPlayer,
                                                      unsigned char nMinor)
{
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    unsigned short nQuickChat = ReadWORD(16);
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    CSWSObject* pObject = pPlayer->GetGameObject();
    if (pObject != NULL)
    {
        return SendServerToPlayerQuickChatMessage(pObject->m_nId,
                                                  nQuickChat);
    }
    return 0;
}
// PC 0x005A1EA0 GOG 0x005A1F10
int CSWSMessage::HandlePlayerToServerChatMessage(CSWSPlayer* pPlayer,
                                                 unsigned char nMinor)
{
    CExoString sMessage;
    CExoString sTarget;
    int nTargetPlayerId;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMinor == 4)
    {
        if (ReadBOOL() == 0)
        {
            nTargetPlayerId = ReadDWORD(32);
        }
        else
        {
            sTarget = ReadCExoString(32);
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            nTargetPlayerId = pServerExoApp->ResolvePlayerByFirstName(sTarget);
        }
    }
    else
    {
        nTargetPlayerId = -1;
    }
    sMessage = ReadCExoString(32);
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    CSWSObject* pObject = pPlayer->GetGameObject();
    if (pObject == NULL)
    {
        return 0;
    }
    unsigned long oidSpeaker = pObject->m_nId;
    int nResult;
    switch (nMinor)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 14:
    {
        nResult = SendServerToPlayerChatMessage(nMinor, oidSpeaker, sMessage,
                                                nTargetPlayerId, sTarget);
        break;
    }
    default:
    {
        nResult = 0;
        break;
    }
    }
    return nResult;
}

#include "nwccreature.h"
// PC 0x005A2080 GOG 0x005A20F0
int CSWSMessage::HandlePlayerToServerCheatMessage(CSWSPlayer* pPlayer,
                                                  unsigned char nMinor)
{
    if (MessageReadOverflow(1))
    {
        return 0;
    }

    CNetLayer* pNetLayer = g_pAppManager->GetServerExoApp()->GetNetLayer();
    if (nMinor == 21)
    {
        int bDebugMode = ReadBOOL();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
        {
            return 0;
        }
        CNetLayerPlayerInfo* pPlayerInfo =
            pNetLayer->GetPlayerInfo(pPlayer->GetPlayerId());
        if (pPlayerInfo->m_bAuthorized == 0)
        {
            return 1;
        }
        g_pAppManager->GetServerExoApp()->SetDebugMode(bDebugMode);
        SendServerToPlayerCheatDebugMode(bDebugMode);
    }

    if (nMinor == 4)
    {
        int bResult = 0;
        if (g_pAppManager->GetServerExoApp() != NULL && g_pAppManager->m_pClientExoApp != NULL &&
            g_pAppManager->m_pClientExoApp->GetSWParty() != NULL)
        {
            int nMembers = g_pAppManager->m_pClientExoApp->GetSWParty()->m_nCharacters;
            CSWCCreature* pCharacter = g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
            if (pCharacter != NULL)
            {
                CSWSCreature* pServerCreature = pCharacter->GetServerCreature();
                if (pServerCreature != NULL)
                {
                    int bPlot = pServerCreature->m_bPlotObject == 0;
                    for (int nMember = 0; nMember < nMembers; ++nMember)
                    {
                        pCharacter = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(nMember);
                        if (pCharacter != NULL)
                        {
                            pServerCreature = pCharacter->GetServerCreature();
                            if (pServerCreature != NULL)
                                pServerCreature->m_bPlotObject = bPlot;
                        }
                    }
                    bResult = 1;
                }
            }
        }
        return bResult;
    }
    if (nMinor == 2)
    {
        CSWSObject* pObject = pPlayer->GetGameObject();
        if (pObject == NULL)
            return 0;
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_RESURRECTION;
        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        pObject->ApplyEffect(pEffect, 0, 0);
        pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_HEAL;
        pEffect->SetInteger(0, pObject->GetMaxHitPoints(1));
        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        pObject->ApplyEffect(pEffect, 0, 0);
        if (pObject->AsSWSCreature() != NULL)
        {
            CSWSCreatureStats* pStats = pObject->AsSWSCreature()->GetStats();
            pStats->m_nCurrentForcePoints = pObject->AsSWSCreature()->GetMaxForcePoints();
        }
        return 1;
    }
    if (g_pAppManager->GetServerExoApp()->GetDebugMode() == 0)
    {
        ClearReadMessage();
        return 1;
    }

    if (nMinor == 18)
    {
        if (pPlayer->GetGameObject() != NULL)
        {
            CSWSCreatureStats* pStats = pPlayer->GetGameObject()->AsSWSCreature()->GetStats();
            if (pStats->GetLevel(0) < 50)
            {
                pPlayer->GetGameObject()->AsSWSCreature()->GetStats()->SetExperience(
                    pPlayer->GetGameObject()->AsSWSCreature()->GetStats()->GetExpNeededForLevelUp(), 1);
            }
        }
        return 0;
    }

    if (nMinor == 1)
    {
        CExoString sAreaName;
        CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
        unsigned long oidArea;
        int bRun;
        CSWSObject* pObject = pPlayer->GetGameObject();
        if (pObject == NULL)
            return 0;
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        sAreaName = ReadCExoString(32);
        float fX = ReadFLOAT(1.0f, 32);
        float fY = ReadFLOAT(1.0f, 32);
        float fZ = ReadFLOAT(1.0f, 32);
        if (MessageReadOverflow(1))
            return 0;
        CSWSArea* pArea = pModule->GetArea();
        if (pArea == NULL)
        {
            pArea = pCreature->GetArea();
            if (pArea == NULL)
            {
                CExoString sMessage;
                sMessage.Format("Area %s not valid.", sAreaName.CStr());
                SendServerToPlayerChatMessage(5, OBJECT_INVALID, sMessage,
                    pPlayer->GetPlayerId(), "");
                return 0;
            }
            if (sAreaName.GetLength() > 0)
            {
                CExoString sMessage;
                sMessage.Format("Area %s is not a valid area.", sAreaName.CStr());
                SendServerToPlayerChatMessage(5, OBJECT_INVALID, sMessage,
                    pPlayer->GetPlayerId(), "");
                return 0;
            }
        }
        if (fX < 0.0f || fX > pArea->GetWidth() * 10.0f)
        {
            CExoString sMessage;
            sMessage.Format("X co-ordinate outside of area, should be in [%f, %f]",
                0.0, (double)(pArea->GetWidth() * 10.0f));
            SendServerToPlayerChatMessage(5, OBJECT_INVALID, sMessage,
                pPlayer->GetPlayerId(), "");
            return 0;
        }
        else if (fY < 0.0f || fY > pArea->GetHeight() * 10.0f)
        {
            CExoString sMessage;
            sMessage.Format("Y co-ordinate outside of area, should be in [%f, %f]",
                0.0, (double)(pArea->GetHeight() * 10.0f));
            SendServerToPlayerChatMessage(5, OBJECT_INVALID, sMessage,
                pPlayer->GetPlayerId(), "");
            return 0;
        }
        else
        {
            oidArea = pArea->GetId();
            bRun = 0;
            {
                Vector vPosition(fX, fY, fZ);
                Vector vSafePosition;
                if (!pArea->ComputeSafeLocation(vPosition, 20.0f,
                        pCreature->GetPathfindInformation(), 1, &vSafePosition))
                    vSafePosition = vPosition;
                fX = vSafePosition.m_fX;
                fY = vSafePosition.m_fY;
                fZ = vSafePosition.m_fZ;
            }
            pCreature->AddAction(CSWSObject::ACTION_JUMP_TO_POINT,
                CSWSObject::ACTION_GROUP_NEW,
                CSWSObject::ACTION_PARAM_FLOAT, &fX,
                CSWSObject::ACTION_PARAM_FLOAT, &fY,
                CSWSObject::ACTION_PARAM_FLOAT, &fZ,
                CSWSObject::ACTION_PARAM_OBJECT, &oidArea,
                CSWSObject::ACTION_PARAM_INTEGER, &bRun,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL);
            return 1;
        }
        CExoString sMessage;
        sMessage.Format("Area %s not valid.", sAreaName.CStr());
        SendServerToPlayerChatMessage(5, OBJECT_INVALID, sMessage,
            pPlayer->GetPlayerId(), "");
        return 0;
    }
    if (nMinor == 25)
    {
        CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
        unsigned char nValue = ReadBYTE(8);
        if (!MessageReadOverflow(1) && pModule != NULL)
            pModule->m_n210 = nValue;
        return 0;
    }
    if (nMinor == 32)
    {
        CSWSObject* pObject = pPlayer->GetGameObject();
        if (pObject == NULL)
            return 0;
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        CSWSArea* pArea = pCreature->GetArea();
        if (pArea == NULL)
            return 0;
        Vector vPosition;
        unsigned long oidArea = pArea->GetId();
        int bRun = 1;
        CPathfindInformation* pPathfind = pCreature->GetPathfindInformation();
        float fStepScale = pPathfind->m_fStepScale;
        pPathfind->m_fStepScale = 0.1f;
        pPathfind->ComputeStepTolerance();
        if (pArea->ComputeSafeLocation(pCreature->m_vPosition, 10.0f,
                pCreature->GetPathfindInformation(), 1, &vPosition))
        {
            Vector vDifference = vPosition - pCreature->m_vPosition;
            if (vDifference * vDifference > 0.01f)
            {
                pCreature->ClearAllActions(1);
                pCreature->AddAction(CSWSObject::ACTION_JUMP_TO_POINT,
                    CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fX,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fY,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fZ,
                    CSWSObject::ACTION_PARAM_OBJECT, &oidArea,
                    CSWSObject::ACTION_PARAM_INTEGER, &bRun,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL);
            }
        }
        pPathfind = pCreature->GetPathfindInformation();
        pPathfind->m_fStepScale = fStepScale;
        pPathfind->ComputeStepTolerance();
        return 0;
    }
    if (nMinor == 7)
    {
        CGameObject* pObject = NULL;
        unsigned long oidCreator = ReadOBJECTIDServer();
        unsigned long oidTarget = ReadOBJECTIDServer();
        if (MessageReadOverflow(1))
            return 0;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(oidTarget, &pObject) !=
                CGameObjectArray::SUCCESS || pObject == NULL || pObject->AsSWSObject() == NULL)
            return 0;
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_DEATH;
        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        pEffect->SetCreator(oidCreator);
        pEffect->SetInteger(0, 1);
        pEffect->SetInteger(1, 1);
        pObject->AsSWSObject()->ApplyEffect(pEffect, 0, 0);
        return 1;
    }
    if (nMinor == 4)
    {
        int bResult = 0;
        if (g_pAppManager->GetServerExoApp() != NULL && g_pAppManager->m_pClientExoApp != NULL &&
            g_pAppManager->m_pClientExoApp->GetSWParty() != NULL)
        {
            int nMembers = g_pAppManager->m_pClientExoApp->GetSWParty()->m_nCharacters;
            CSWCCreature* pCharacter = g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
            if (pCharacter != NULL)
            {
                CSWSCreature* pServerCreature = pCharacter->GetServerCreature();
                if (pServerCreature != NULL)
                {
                    int bPlot = pServerCreature->m_bPlotObject == 0;
                    for (int nMember = 0; nMember < nMembers; ++nMember)
                    {
                        pCharacter = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(nMember);
                        if (pCharacter != NULL)
                        {
                            pServerCreature = pCharacter->GetServerCreature();
                            if (pServerCreature != NULL)
                                pServerCreature->m_bPlotObject = bPlot;
                        }
                    }
                    bResult = 1;
                }
            }
        }
        return bResult;
    }
    if (nMinor == 9)
    {
        int bEnabled = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        g_bEnableCombatDebugging = bEnabled;
        g_bEnableSavingThrowDebugging = bEnabled;
        return 1;
    }
    if (nMinor == 22)
    {
        int bEnabled = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        g_bEnableMovementSpeedDebugging = bEnabled;
        return 1;
    }
    if (nMinor == 3)
    {
        CSWSObject* pObject = pPlayer->GetGameObject();
        if (pObject == NULL)
            return 0;
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        unsigned long nSpell = ReadDWORD(32);
        unsigned char nFlags = ReadBYTE(8);
        unsigned long oidTarget = OBJECT_INVALID;
        Vector vTarget;
        if (MessageReadOverflow(1))
            return 0;
        int bTargetLocation = 0;
        if ((nFlags & 1) != 0)
        {
            oidTarget = ReadOBJECTIDServer();
            if (MessageReadOverflow(1))
                return 0;
        }
        if ((nFlags & 2) != 0)
        {
            float fX = ReadFLOAT(1.0f, 32);
            float fY = ReadFLOAT(1.0f, 32);
            float fZ = ReadFLOAT(1.0f, 32);
            if (MessageReadOverflow(1))
                return 0;
            vTarget = Vector(fX, fY, fZ);
            bTargetLocation = 1;
        }
        pCreature->ActionManager(1);
        pCreature->AddCastSpellActions(nSpell, 255, 0, 0, 0, vTarget,
            oidTarget, 0, bTargetLocation, 0, 0, 0, 0,
            CSWSCreature::SPELL_ACTION_OPTION10_UNSET,
            CSWSCreature::SPELL_ACTION_OPTION11_UNSET, 0);
        return 1;
    }
    if (nMinor == 15)
    {
        pPlayer->m_bFloatyEffects = pPlayer->m_bFloatyEffects == 0;
        return 1;
    }
    if (nMinor == 8)
    {
        CSWSObject* pObject = pPlayer->GetGameObject();
        if (pObject == NULL)
            return 0;
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            CExoString sScript = ReadCExoString(32);
            if (!MessageReadOverflow(1) && !MessageReadUnderflow(1))
            {
                g_pVirtualMachine->RunScript(&sScript, pCreature->GetId(), 1);
                return 1;
            }
        }
        return 0;
    }
    if (nMinor == 10)
    {
        unsigned long oidTarget = ReadOBJECTIDServer();
        unsigned short nVisualEffect = ReadWORD(16);
        float fDuration = ReadFLOAT(1.0f, 32);
        unsigned long oidSource = ReadOBJECTIDServer();
        unsigned char nOption = ReadBYTE(8);
        CGameObject* pObject = NULL;
        if (MessageReadOverflow(1))
            return 0;
        unsigned char nResult = g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(oidTarget, &pObject);
        if (nResult != CGameObjectArray::SUCCESS)
            return 0;
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
        pEffect->SetObjectID(0, oidSource);
        pEffect->SetInteger(0, nVisualEffect);
        pEffect->SetInteger(1, nOption);
        if (fDuration == 0.0f)
            pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                CGameEffect::DURATION_TYPE_INSTANTANEOUS;
        else
        {
            pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                CGameEffect::DURATION_TYPE_TEMPORARY;
            pEffect->m_fDuration = fDuration;
        }
        static_cast<CSWSObject*>(pObject)->ApplyEffect(pEffect, 0, 0);
        return 1;
    }
    if (nMinor == 14)
    {
        CResRef cItem;
        Vector vSpawn;
        ReadOBJECTIDServer();
        unsigned char nAppearance = ReadBYTE(8);
        cItem = ReadCResRef(16);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        CSWSCreature* pGoon = new CSWSCreature(OBJECT_INVALID, 0);
        pGoon->m_cAppearanceInfo.m_nAppearance = nAppearance;
        pGoon->m_cAppearanceInfo.m_nAppearancePart0 = 0;
        pGoon->m_cAppearanceInfo.m_nAppearancePart1 = 0;
        CExoLocString lsName;
        lsName.AddString(0, "Hostile", 0);
        pGoon->GetStats()->m_lsFirstName = lsName;
        lsName.AddString(0, "Nudist", 0);
        pGoon->GetStats()->m_lsLastName = lsName;
        CSWSArea* pArea = g_pAppManager->GetServerExoApp()->GetModule()->GetArea();
        if (pArea != NULL)
        {
            CSWSEncounter* pEncounter = new CSWSEncounter(OBJECT_INVALID);
            pEncounter->AddToArea(pArea, pEncounter->m_vPosition.m_fX,
                pEncounter->m_vPosition.m_fY, pEncounter->m_vPosition.m_fZ, 1);
            CSWSObject* pPlayerObject = pPlayer->GetGameObject();
            Vector vSearch;
            vSearch.m_fX = pPlayerObject->m_vPosition.m_fX;
            vSearch.m_fY = pPlayerObject->m_vPosition.m_fY + 15.0f;
            vSearch.m_fZ = pPlayerObject->m_vPosition.m_fZ;
            CExoString sMessage;
            int bHidden = pArea->ComputeNonVisibleLocation(vSearch,
                pGoon->GetPathfindInformation(), 1, &vSpawn, 30.0f);
            float fX;
            float fY;
            float fZ;
            if (bHidden == 1)
            {
                fX = vSpawn.m_fX;
                fY = vSpawn.m_fY;
                fZ = pArea->ComputeHeight(vSpawn, 1, NULL);
                sMessage.Format("My spider sense tells me the goon is hiding at (%d, %d, %d).",
                    (int)fX, (int)fY, (int)fZ);
                pPlayer->GetGameObject()->AsSWSCreature()->AddAction(
                    CSWSObject::ACTION_SPEAK_STRING, CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_STRING, &sMessage,
                    CSWSObject::ACTION_PARAM_INTEGER, (void*)&nVolume,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            }
            else if (vSpawn.m_fX == AREA_NON_VISIBLE_INVALID_COORDINATE &&
                vSpawn.m_fY == AREA_NON_VISIBLE_INVALID_COORDINATE &&
                vSpawn.m_fZ == AREA_NON_VISIBLE_INVALID_COORDINATE)
            {
                sMessage.Format("There was no place to spawn the goon.");
                pPlayer->GetGameObject()->AsSWSCreature()->AddAction(
                    CSWSObject::ACTION_SPEAK_STRING, CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_STRING, &sMessage,
                    CSWSObject::ACTION_PARAM_INTEGER, (void*)&nVolume,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                return 0;
            }
            else
            {
                fX = vSpawn.m_fX;
                fY = vSpawn.m_fY;
                fZ = pArea->ComputeHeight(vSpawn, 1, NULL);
                sMessage.Format("The goon couldn't find a place to hide, he's at (%d, %d, %d)",
                    (int)fX, (int)fY, (int)fZ);
                pPlayer->GetGameObject()->AsSWSCreature()->AddAction(
                    CSWSObject::ACTION_SPEAK_STRING, CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_STRING, &sMessage,
                    CSWSObject::ACTION_PARAM_INTEGER, (void*)&nVolume,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            }
            pGoon->SetScript(CSWSCreature::SCRIPT_HEARTBEAT, "b_heart");
            pGoon->SetScript(CSWSCreature::SCRIPT_ATTACKED, "b_attacked");
            pGoon->SetScript(CSWSCreature::SCRIPT_DAMAGED, "b_damage");
            pGoon->SetScript(CSWSCreature::SCRIPT_DEATH, "b_death");
            pGoon->AddToArea(pArea, fX, fY, fZ, 0, 0);
            if (g_pExoResMan->Exists(cItem, RESTYPE_UTI, (unsigned long*)NULL))
            {
                CSWSItem* pItem = new CSWSItem(OBJECT_INVALID);
                pItem->LoadFromTemplate(cItem);
                pItem->SetPosition(Vector(fX, fY, fZ), 1, 1, 0);
                pItem->AddToArea(pArea, fX, fY, fZ, 1);
            }
            return 1;
        }
        return 0;
    }
    if (nMinor == 11)
    {
        ReadBOOL();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        CSWSArea* pArea = pCreature->GetArea();
        CSWSTrigger* pTrigger = new CSWSTrigger(OBJECT_INVALID);
        pTrigger->CreateNewGeometry(2.5f,
            pCreature->m_vPosition + (-pCreature->m_vOrientation) * 2.5f,
            pCreature->GetArea());
        CSWSClient* pClient = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(pCreature->GetId());
        if (pClient != NULL)
        {
            CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
            pMessage->SendServerToPlayerCheatNasty(pClient->GetPlayerId(), pCreature->GetId());
        }
        unsigned long oidObject;
        if (pArea->GetFirstObjectInArea(oidObject))
        {
            do
            {
                CGameObject* pObject;
                if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(oidObject, &pObject) ==
                        CGameObjectArray::SUCCESS && pObject->AsSWSCreature() != NULL &&
                    pObject->GetId() != pCreature->GetId())
                {
                    CSWSCreature* pTarget = pObject->AsSWSCreature();
                    if (pTrigger->InTrigger(pTarget->m_vPosition))
                    {
                        CGameEffect* pEffect = new CGameEffect(1);
                        pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE;
                        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                        pEffect->SetNumIntegers(21);
                        pEffect->SetInteger(0, 10);
                        pEffect->SetInteger(16, 1000);
                        unsigned long oidSelf = pCreature->GetId();
                        if (pCreature == NULL && oidSelf != 0)
                            return 0;
                        pEffect->SetObjectID(0, oidSelf);
                        pTarget->ApplyEffect(pEffect, 0, 0);
                        pClient = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(pTarget->GetId());
                        if (pClient != NULL)
                        {
                            CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
                            unsigned long oidCheater = pCreature->GetId();
                            unsigned long nPlayerId = pClient->GetPlayerId();
                            if (pClient == NULL && nPlayerId != 0)
                                return 0;
                            pMessage->SendServerToPlayerCheatNasty(nPlayerId, oidCheater);
                        }
                    }
                }
            } while (pArea->GetNextObjectInArea(oidObject));
        }
        pCreature->SetAnimation(ANIMATION_KNOCKDOWN_BACK);
        delete pTrigger;
        return 0;
    }
    if (nMinor == 13)
    {
        ReadBOOL();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        CSWSArea* pArea = pCreature->GetArea();
        unsigned long oidObject;
        if (pArea->GetFirstObjectInArea(oidObject))
        {
            Vector vPosition = pCreature->m_vPosition + pCreature->m_vOrientation * 4.0f;
            CGameEffect* pEffect = new CGameEffect(1);
            pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
            pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                CGameEffect::DURATION_TYPE_INSTANTANEOUS;
            pEffect->SetCreator(pCreature->GetId());
            pEffect->SetInteger(0, 32);
            pArea->ApplyEffect(pEffect, vPosition, pCreature->m_vOrientation);
            pCreature->SetAnimation(10017);
            do
            {
                CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidObject);
                if (pTarget != NULL && pTarget->GetId() != pCreature->GetId())
                {
                    for (int i = 0; i < rand() % 3; ++i)
                    {
                        pEffect = new CGameEffect(1);
                        pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
                        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                        pEffect->SetCreator(pCreature->GetId());
                        unsigned long nDelay = rand() % 2300 + 3000;
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(0, nDelay,
                            pCreature->GetId(), pTarget->GetId(),
                            CServerAIMaster::EVENT_APPLY_EFFECT, pEffect);
                        float fDistance = magnitude(pTarget->m_vPosition - pCreature->m_vPosition);
                        nDelay += (int)(fDistance / (logf(fDistance) * 3.0f + 2.0f) * 1000.0f);
                        pEffect = new CGameEffect(1);
                        pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
                        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                        pEffect->SetCreator(pCreature->GetId());
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(0, nDelay,
                            pCreature->GetId(), pTarget->GetId(),
                            CServerAIMaster::EVENT_APPLY_EFFECT, pEffect);
                        pEffect = new CGameEffect(1);
                        pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
                        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                        pEffect->SetCreator(pCreature->GetId());
                        pEffect->SetInteger(0, 61);
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(0, nDelay,
                            pCreature->GetId(), pTarget->GetId(),
                            CServerAIMaster::EVENT_APPLY_EFFECT, pEffect);
                        pEffect = new CGameEffect(1);
                        pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE;
                        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                            CGameEffect::DURATION_TYPE_INSTANTANEOUS;
                        pEffect->SetCreator(pCreature->GetId());
                        pEffect->SetNumIntegers(21);
                        pEffect->SetInteger(0, 50);
                        pEffect->SetInteger(1, -1);
                        pEffect->SetInteger(2, -1);
                        pEffect->SetInteger(3, -1);
                        pEffect->SetInteger(4, -1);
                        pEffect->SetInteger(5, -1);
                        pEffect->SetInteger(6, -1);
                        pEffect->SetInteger(7, -1);
                        pEffect->SetInteger(8, -1);
                        pEffect->SetInteger(9, -1);
                        pEffect->SetInteger(10, -1);
                        pEffect->SetInteger(11, -1);
                        pEffect->SetInteger(12, -1);
                        pEffect->SetInteger(16, 1000);
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(0, nDelay,
                            pCreature->GetId(), pTarget->GetId(),
                            CServerAIMaster::EVENT_APPLY_EFFECT, pEffect);
                    }
                }
            } while (pArea->GetNextObjectInArea(oidObject));
        }
        return 0;
    }
    return 0;
}
#ifndef _XBOX
int CSWSMessage::HandlePlayerToServerDialogMessage(CSWSPlayer* pPlayer,  // PC 0x005A3D50 GOG 0x005A3DC0
                                                   unsigned char nMinor)
{
    unsigned long oidObject;
    unsigned long nReplyIndex;
    int bEscape;
    CServerExoApp* pServerExoApp;
    CGameObject* pObject;
    unsigned long nEntryId;
    unsigned char nResult;

    if (!MessageReadOverflow(1) && nMinor == 3)
    {
        oidObject = ReadOBJECTIDServer();
        nReplyIndex = ReadDWORD(32);
        bEscape = (unsigned char)ReadBYTE(8) != 0;
        nEntryId = ReadDWORD(32);
        if (!MessageReadOverflow(1) && MessageReadUnderflow(1) == 0)
        {
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            nResult = pServerExoApp->GetObjectArray()->GetGameObject(
                oidObject, &pObject);
            if (nResult == CGameObjectArray::SUCCESS)
            {
                return ((CSWSObject*)pObject)
                    ->ReplyDialog(pPlayer->GetPlayerId(), nReplyIndex, bEscape,
                                  nEntryId, 0);
            }
        }
    }
    return 0;
}
#endif
int CSWSMessage::HandlePlayerToServerGameObjectUpdate(CSWSPlayer* pPlayer,  // PC 0x005A3E10 GOG 0x005A3E80
                                                      unsigned char nMinor)
{
    if (MessageReadOverflow(1))
        return 0;
    if (nMinor != PLAYER_OBJECT_UPDATE)
        goto ReadFailed;

    {
    CResRef resTemplate;
    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
    CGameObjectArray* pObjects = g_pAppManager->GetServerExoApp()->GetObjectArray();
    CNetLayer* pNetLayer = g_pAppManager->GetServerExoApp()->GetNetLayer();
    if (pModule == NULL || pObjects == NULL || pNetLayer == NULL)
    {
        ClearReadMessage();
        return 0;
    }

    char nUpdate = ReadCHAR(8);
    if (MessageReadOverflow(1))
        return 0;
    CGameObject* pObject;
    float fX, fY, fZ;
    switch (nUpdate)
    {
    case PLAYER_OBJECT_ADD_ITEM:
    {
        ReadOBJECTIDServer();
        fX = ReadFLOAT(1.0f, 32);
        fY = ReadFLOAT(1.0f, 32);
        fZ = ReadFLOAT(1.0f, 32);
        char nModelVariation = ReadBYTE(8);
        ReadBYTE(8);
        ReadBYTE(8);
        resTemplate = ReadCResRef(16);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        if (g_pExoResMan->Exists(resTemplate, RESTYPE_UTI, (unsigned long*)NULL))
        {
            CSWSItem* pItem = new CSWSItem(OBJECT_INVALID);
            pItem->LoadFromTemplate(resTemplate);
            if (nModelVariation != 0)
                pItem->m_nModelVariation = nModelVariation;
            pItem->SetPosition(Vector(fX, fY, fZ), 1, 1, 0);
            CSWSArea* pArea = pModule->GetArea();
            pItem->AddToArea(pArea, fX, fY, fZ, 1);
        }
        return 1;
    }
    case PLAYER_OBJECT_DELETE:
    {
        unsigned long oidObject = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        pObjects->Delete(oidObject, &pObject);
        if (pObject == NULL)
            return 0;
        delete pObject->AsSWSItem();
        return 1;
    }
    case PLAYER_OBJECT_APPEARANCE_PART:
    {
        unsigned long oidObject = ReadOBJECTIDServer();
        unsigned char nPart = ReadBYTE(8);
        unsigned char nValue = ReadBYTE(8);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        pObjects->GetGameObject(oidObject, &pObject);
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        switch (nPart)
        {
        case PLAYER_APPEARANCE_PART2:
            pCreature->m_cAppearanceInfo.m_nAppearancePart2 = nValue;
            break;
        case PLAYER_APPEARANCE_PART3:
            pCreature->m_cAppearanceInfo.m_nAppearancePart3 = nValue;
            break;
        case PLAYER_APPEARANCE_PART4:
            pCreature->m_cAppearanceInfo.m_nAppearancePart4 = nValue;
            break;
        case PLAYER_APPEARANCE_PART5:
            pCreature->m_cAppearanceInfo.m_nAppearancePart5 = nValue;
            break;
        }
        return 1;
    }
    case PLAYER_OBJECT_APPEARANCE_PART1:
    {
        unsigned long oidObject = ReadOBJECTIDServer();
        unsigned char nValue = ReadBYTE(8);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        pObjects->GetGameObject(oidObject, &pObject);
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        pCreature->m_cAppearanceInfo.m_nAppearancePart1 = nValue;
        return 1;
    }
    case PLAYER_OBJECT_APPEARANCE_TYPE:
    {
        unsigned long oidObject = ReadOBJECTIDServer();
        unsigned short nAppearance = ReadWORD(16);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        pObjects->GetGameObject(oidObject, &pObject);
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        unsigned short& nCreatureAppearance = pCreature->m_cAppearanceInfo.m_nAppearance;
        nCreatureAppearance = nAppearance;
        pCreature->m_pStats->m_nAppearance = nCreatureAppearance;
        pCreature->m_pStats->SetMovementRate(CSWSCreatureStats::MOVEMENT_RATE_APPEARANCE);
        if (g_pRules->m_p2DArrays->GetAppearance()->GetFLOATEntry(
                nAppearance, CTwoDimArrays::COL2DA_APPEARANCETABLE_PERSPACE, &fZ))
        {
            CPathfindInformation* pInfo = pCreature->m_pPathfindInformation;
            pInfo->m_fStepScale = fZ;
            pInfo->ComputeStepTolerance();
        }
        return 1;
    }
    case PLAYER_OBJECT_APPEARANCE_PART0:
    {
        unsigned long oidObject = ReadOBJECTIDServer();
        unsigned char nValue = ReadBYTE(8);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        pObjects->GetGameObject(oidObject, &pObject);
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        pCreature->m_cAppearanceInfo.m_nAppearancePart0 = nValue;
        return 1;
    }
    case PLAYER_OBJECT_APPEARANCE:
    {
        unsigned long oidObject = ReadOBJECTIDServer();
        if (MessageReadOverflow(1))
            return 0;
        pObjects->GetGameObject(oidObject, &pObject);
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature == NULL)
        {
            ClearReadMessage();
            return 0;
        }
        CSWSCreatureAppearanceInfo cAppearance;
        cAppearance.m_nAppearance = ReadWORD(16);
        cAppearance.m_nAppearancePart0 = ReadBYTE(8);
        cAppearance.m_nAppearancePart1 = ReadBYTE(8);
        cAppearance.m_nAppearancePart2 = ReadBYTE(8);
        cAppearance.m_nAppearancePart3 = ReadBYTE(8);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        pCreature->m_cAppearanceInfo.m_nAppearance = cAppearance.m_nAppearance;
        pCreature->m_cAppearanceInfo.m_nAppearancePart0 = cAppearance.m_nAppearancePart0;
        pCreature->m_cAppearanceInfo.m_nAppearancePart1 = cAppearance.m_nAppearancePart1;
        pCreature->m_cAppearanceInfo.m_nAppearancePart2 = cAppearance.m_nAppearancePart2;
        pCreature->m_cAppearanceInfo.m_nAppearancePart3 = cAppearance.m_nAppearancePart3;
        pCreature->UpdatePersonalSpace();
        return 1;
    }
    case PLAYER_OBJECT_APPEARANCE_PART6:
    {
        unsigned long oidObject = ReadOBJECTIDServer();
        unsigned char nValue = ReadBYTE(8);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        pObjects->GetGameObject(oidObject, &pObject);
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        pCreature->m_cAppearanceInfo.m_nAppearancePart6 = nValue;
        return 1;
    }
    case PLAYER_OBJECT_SUMMON_CREATURE:
    {
        unsigned long oidCreator = ReadOBJECTIDServer();
        fX = ReadFLOAT(1.0f, 32);
        fY = ReadFLOAT(1.0f, 32);
        fZ = ReadFLOAT(1.0f, 32);
        CExoString sTemplate = ReadCExoString(32);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        unsigned long oidPlayer = pPlayer->GetGameObject() ?
            pPlayer->GetGameObject()->GetId() : OBJECT_INVALID;
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_SUMMON_CREATURE;
        pEffect->m_nSubType = CGameEffect::DURATION_TYPE_INSTANTANEOUS |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetObjectID(0, oidCreator);
        pEffect->SetFloat(0, fX);
        pEffect->SetFloat(1, fY);
        pEffect->SetFloat(2, fZ);
        pEffect->SetString(0, sTemplate);
        pPlayer->GetGameObject()->ApplyEffect(pEffect, 0, 0);
        return 1;
    }
    case PLAYER_OBJECT_EFFECT_M:
    {
        unsigned long oidCreator = ReadOBJECTIDServer();
        fX = ReadFLOAT(1.0f, 32);
        fY = ReadFLOAT(1.0f, 32);
        fZ = ReadFLOAT(1.0f, 32);
        CExoString sTemplate = ReadCExoString(32);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        unsigned long oidPlayer = pPlayer->GetGameObject() ?
            pPlayer->GetGameObject()->GetId() : OBJECT_INVALID;
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_SUMMON_PARTY;
        pEffect->m_nSubType = CGameEffect::DURATION_TYPE_INSTANTANEOUS |
            (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
        pEffect->SetObjectID(0, oidCreator);
        pEffect->SetFloat(0, fX);
        pEffect->SetFloat(1, fY);
        pEffect->SetFloat(2, fZ);
        pEffect->SetString(0, sTemplate);
        pPlayer->GetGameObject()->ApplyEffect(pEffect, 0, 0);
        return 1;
    }
    default:
        break;
    }
    }
ReadFailed:
    return 0;
}
// PC 0x005A4780 GOG 0x005A47F0
int CSWSMessage::HandlePlayerToServerGuiInventoryMessage(CSWSPlayer* pPlayer,
                                                         unsigned char nMinor)
{
    unsigned char nOpen = ReadBYTE(8);
    if (!(MessageReadOverflow(1) || MessageReadUnderflow(1) != 0))
    {
        if (nMinor == 1)
        {
            pPlayer->m_pInventoryGUI->SetOpen(nOpen != 0);
        }
        return 1;
    }
    return 0;
}
// PC 0x005A47E0 GOG 0x005A4850
int CSWSMessage::HandlePlayerToServerGuiContainerMessage(CSWSPlayer* pPlayer,
                                                         unsigned char nMinor)
{
    unsigned long oidContainer;
    int bUpdatePlayer;
    CServerExoApp* pServerExoApp;
    CSWSObject* pObject;
    unsigned char nObjectType;
    unsigned long oidPlayer;

    if (nMinor == 2)
    {
        oidContainer = ReadOBJECTIDServer();
        bUpdatePlayer = ReadBOOL();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
        {
            return 0;
        }
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(
                oidContainer, (CGameObject**)&pObject) ==
            CGameObjectArray::SUCCESS)
        {
            nObjectType = pObject->m_nObjectType;
            if (nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
            {
                oidPlayer = pPlayer->GetGameObject()->m_nId;
                pObject->AsSWSPlaceable()->CloseInventory(oidPlayer,
                                                          bUpdatePlayer);
            }
            else if (nObjectType == CGameObject::OBJECT_TYPE_ITEM)
            {
                oidPlayer = pPlayer->GetGameObject()->m_nId;
                pObject->AsSWSItem()->CloseInventory(oidPlayer, bUpdatePlayer);
            }
        }
    }
    return 1;
}
// PC 0x005A48C0 GOG 0x005A4930
int CSWSMessage::HandlePlayerToServerCharacterSheetMessage(CSWSPlayer* pPlayer,
                                                           unsigned char nMinor)
{
    unsigned char nPanel;
    unsigned long oidCreature;
    CSWSCreature* pCreature;
    unsigned long oidDisplayedCreature;

    nPanel = ReadCHAR(8);
    oidCreature = ReadOBJECTIDServer();
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    if (nMinor == 1)
    {
        if (pPlayer->PermittedToDisplayCharacterSheet(oidCreature) == 0)
        {
            nPanel = 0xFF;
            SendServerToPlayerGUICharacterSheet_NotPermitted(
                pPlayer->m_nPlayerId, oidCreature);
            oidCreature = OBJECT_INVALID;
        }
        oidDisplayedCreature = pPlayer->m_pCharSheetGUI->m_oidCreature;
        pCreature = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            oidDisplayedCreature);
        if (pCreature != NULL)
        {
            pCreature->m_nCharacterSheetDisplayCount--;
        }
        pCreature = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            oidCreature);
        if (pCreature != NULL)
        {
            pCreature->m_nCharacterSheetDisplayCount++;
        }
        pPlayer->m_pCharSheetGUI->SetCreatureDisplayed(oidCreature);
        pPlayer->m_pCharSheetGUI->m_nUpdateType = nPanel;
        if (nPanel == 0xFF)
        {
            pPlayer->m_pCharSheetGUI->m_pStatsUpdate->ClearEffectIcons();
        }
    }
    return 1;
}
// PC 0x005A4990 GOG 0x005A4A00
int CSWSMessage::HandlePlayerToServerInputDriveControl(CSWSPlayer* pPlayer)
{
    float fPositionX;
    float fPositionY;
    unsigned short nDirection;
    unsigned char nDriveMode;
    unsigned char nFlags;
    CSWSObject* pObject;
    CSWSCreature* pCreature;

    fPositionX = ReadFLOAT(1.0f, 32);
    fPositionY = ReadFLOAT(1.0f, 32);
    nDirection = ReadWORD(12);
    nDriveMode = ReadBYTE(8);
    nFlags = ReadBYTE(8);
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    pObject = pPlayer->GetGameObject();
    if (pObject == NULL)
    {
        return 0;
    }
    Vector vPosition(fPositionX, fPositionY, 0.0f);
    pCreature = pObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 1;
    }
    if (pCreature->m_bRestorePlotOnDownload != 0)
    {
        return 0;
    }
    if (pCreature->m_bCommandable == 1)
    {
        pCreature->SetCombatMode(0, 1);
        pCreature->m_pcCombatRound->ClearAllSpecialAttacks();
        pCreature->ClearAllActions(1);
        pCreature->ActionManager(CSWSCreature::ACTION_MANAGER_REST);
        pCreature->AddDriveAction(0xFFFF, vPosition, nDirection, nDriveMode,
                                  nFlags, 2);
    }
    return 1;
}
// PC 0x005A4AB0 GOG 0x005A4B20
int CSWSMessage::HandlePlayerToServerInputCancelGuiTimingEvent(
    CSWSPlayer* pPlayer)
{
    CSWSObject* pObject = pPlayer->GetGameObject();
    if (pObject == NULL)
    {
        return 0;
    }
    CSWSObject* pCreature = (CSWSObject*)pObject->AsSWSCreature();
    if (pCreature != NULL && pCreature->m_bCommandable == 1)
    {
        pCreature->ClearAllActions(1);
    }
    return 1;
}
// PC 0x005A4AF0 GOG 0x005A4B60
int CSWSMessage::HandlePlayerToServerInputWalkToWaypoint(CSWSPlayer* pPlayer)
{
    unsigned long oidArea = ReadOBJECTIDServer();
    float fPositionX = ReadFLOAT(1.0f, 32);
    float fPositionY = ReadFLOAT(1.0f, 32);
    float fPositionZ = ReadFLOAT(1.0f, 32);
    int nMovementRate = ReadBYTE(8);
    int bRun = ReadBOOL();
    int bUsePath = ReadBOOL();
    unsigned char nClientMoving = ReadBYTE(8);
    unsigned long oidTarget = ReadOBJECTIDServer();

    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }

    CSWSObject* pObject = pPlayer->GetGameObject();
    if (pPlayer->GetGameObject() == NULL || pObject == NULL ||
        pObject->GetArea() == NULL)
    {
        return 0;
    }

    if (pPlayer->GetGameObject()->AsSWSCreature() != NULL)
    {
        if (pPlayer->GetGameObject()->AsSWSCreature()->m_bRestorePlotOnDownload)
        {
            return 0;
        }

        CPathfindInformation* pPathfind =
            pPlayer->GetGameObject()->AsSWSCreature()->m_pPathfindInformation;
        Vector vDestination;
        unsigned long oidDestinationArea;
        if (pPathfind->m_nField_84 == 0)
        {
            vDestination = Vector(pPathfind->m_fStartX, pPathfind->m_fStartY,
                                  pPathfind->m_fStartZ);
            oidDestinationArea = pPathfind->m_nStartArea;
        }
        else
        {
            vDestination = Vector(pPathfind->m_fEndX, pPathfind->m_fEndY,
                                  pPathfind->m_fEndZ);
            oidDestinationArea = pPathfind->m_nEndArea;
        }

        Vector vDelta =
            Vector(fPositionX, fPositionY, fPositionZ) - vDestination;
        if (oidDestinationArea == oidArea && vDelta * vDelta < 0.01f)
        {
            pPathfind->m_nField_28 = (unsigned char)nMovementRate;
            return 1;
        }
        pPathfind->ResetAtEndOfPath();
        if (nClientMoving == 1)
        {
            pPathfind->m_nField_194 = 1;
        }
    }

    if (pObject->AsSWSCreature() != NULL)
    {
        Vector vWaypoint(fPositionX, fPositionY, fPositionZ);
        CSWSCreature* pCreature = pObject->AsSWSCreature();
        Vector vDelta = pCreature->m_vPosition - vWaypoint;
        if (vDelta * vDelta < 2.25f && pCreature->m_nAIState == 2)
        {
            unsigned long oidAITarget = pCreature->m_oidAITarget;
            Vector vCross;
            Vector vToWaypoint;
            Vector vToTarget;
            CGameObject* pGameObject =
                g_pAppManager->GetServerExoApp()->GetGameObject(oidAITarget);
            if (pGameObject == NULL || pGameObject->AsSWSObject() == NULL)
            {
                return 1;
            }
            CSWSObject* pTarget = pGameObject->AsSWSObject();
            if (pTarget == NULL)
            {
                return 1;
            }
            pCreature->m_pcCombatRound->SetRoundPaused(1, pCreature->m_nId);
            pCreature->m_pcCombatRound->SetPauseTimer(500, 0);
            vToTarget = normalize(pTarget->m_vPosition - pCreature->m_vPosition);
            vToWaypoint = normalize(vWaypoint - pCreature->m_vPosition);
            vToWaypoint.m_fZ = vToTarget.m_fZ;
            float fDot = vToTarget * vToWaypoint;
            if (fDot >= 0.707f)
            {
                pCreature->DoCombatStep(2, 500, oidAITarget);
            }
            else if (fDot <= -0.707f)
            {
                pCreature->DoCombatStep(3, 500, oidAITarget);
            }
            else
            {
                vCross = cross(vToTarget, vToWaypoint);
                if (vCross.m_fZ >= 0.0f)
                {
                    pCreature->DoCombatStep(4, 500, oidAITarget);
                }
                else
                {
                    pCreature->DoCombatStep(5, 500, oidAITarget);
                }
            }
            return 1;
        }
    }

    CSWSCreature* pCreature = pObject->AsSWSCreature();
    if (pCreature != NULL && pCreature->m_bCommandable == 1)
    {
        unsigned short nGroupId = CSWSObject::ACTION_GROUP_NEW;
        pCreature->ComputeAIState();
        if (pCreature->m_nAIState == 1)
        {
            nGroupId = CSWSObject::ACTION_GROUP_LAST;
        }
        pObject->AsSWSCreature()->SetCombatMode(0, 1);
        pObject->AsSWSCreature()->m_pcCombatRound->ClearAllSpecialAttacks();
        pObject->AsSWSCreature()->ActionManager(CSWSCreature::ACTION_MANAGER_REST);
        pObject->AsSWSCreature()->AddMoveToPointAction(
            nGroupId, Vector(fPositionX, fPositionY, fPositionZ), oidArea,
            oidTarget, bRun, 0.0f, 0.0f, bUsePath, nMovementRate, 0,
            nClientMoving, 0, 0, Vector(), 0, 0, 0);
    }
    return 1;
}
// PC 0x005A5000 GOG 0x005A5070
int CSWSMessage::HandlePlayerToServerGoldMessage(CSWSPlayer* pPlayer,
                                                 unsigned char nMinor)
{
    Vector vPosition;
    CSWSObject* pObject;
    CSWSCreature* pCreature;

    pObject = pPlayer->GetGameObject();
    if (pObject != NULL && pObject->AsSWSCreature() != NULL)
    {
        pCreature = pObject->AsSWSCreature();
    }
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nMinor)
    {
    case 1:
    {
        unsigned long oidTarget = ReadOBJECTIDServer();
        int nAmount = ReadINT(32);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (oidTarget == OBJECT_INVALID)
        {
            vPosition.m_fX = ReadFLOAT(1.0f, 32);
            vPosition.m_fY = ReadFLOAT(1.0f, 32);
            vPosition.m_fZ = ReadFLOAT(1.0f, 32);
        }
        if (MessageReadOverflow(1) || MessageReadOverflow(1))
        {
            return 0;
        }
        pCreature->TransferGold(oidTarget, vPosition, nAmount);
        break;
    }
    }
    return 0;
}
// PC 0x005A5100 GOG 0x005A5170
int CSWSMessage::HandlePlayerToServerInventoryMessage(CSWSPlayer* pPlayer,
                                                      unsigned char nMinor)
{
    CGameObject* pGameObject = NULL;
    CSWSCreature* pCreature;
    if (MessageReadOverflow(1))
        return 0;
    switch (nMinor)
    {
    case INVENTORY_EQUIP:
    {
        unsigned long oidItem = ReadOBJECTIDServer();
        unsigned long nSlot = ReadDWORD(32);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        int nOverrideCharacter = g_pAppManager->GetClientExoApp()->GetInGameGui()
                                     ->GetTopMenuOverrideCharacter();
        if (nOverrideCharacter == -1)
        {
            pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        }
        else
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            pCreature = (CSWSCreature*)pServerExoApp->GetGameObject(
                pServerExoApp->GetPartyTable()->GetNPCObject(nOverrideCharacter, 0, 1));
            if (pCreature == NULL)
                break;
        }
        unsigned long nEquipSlot = nSlot;
        if (nSlot == INVENTORY_SLOT_LEFTHAND2)
            nSlot = INVENTORY_SLOT_LEFTHAND2 | INVENTORY_SLOT_LEFTHAND;
        else if (nSlot == INVENTORY_SLOT_RIGHTHAND2)
            nSlot = INVENTORY_SLOT_RIGHTHAND2 | INVENTORY_SLOT_RIGHTHAND;
        if (pCreature != NULL &&
            g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidItem, &pGameObject) == CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSItem() != NULL)
        {
            if (nSlot & g_pRules->m_pBaseItems->GetBaseItem(
                    pGameObject->AsSWSItem()->GetBaseItemType())->m_nEquipableSlots)
            {
                pCreature->ClearAllActions(1);
                pCreature->m_pcCombatRound->RemoveAllActions();
                pCreature->AddEquipItemActions(pGameObject->AsSWSItem(),
                                               nEquipSlot, 1, 1, 0, 1, 0);
                unsigned long oidAttack = pCreature->m_pcCombatRound->m_oidUnknown9CC;
                if (oidAttack != OBJECT_INVALID)
                    pCreature->AddAttackActions(oidAttack, 0, 0, 0, 0, 0, 1,
                                                1, 10009, 1500, 1, 0, 0, 4, 0);
                return 1;
            }
        }
        CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
        pMessage->SendServerToPlayerInventory_EquipCancel(
            pPlayer->m_nPlayerId, oidItem, nEquipSlot);
        return 0;
    }
    case 11:
    {
        unsigned long oidItem = ReadOBJECTIDServer();
        int bHasOtherItem = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        unsigned long oidOtherItem = bHasOtherItem ? ReadOBJECTIDServer() :
            CGameObjectArray::INVALIDOBJID;
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        CSWInventory* pInventory = pCreature->m_pInventory;
        if (pInventory == NULL)
            return 0;
        CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
        CSWSItem* pOtherItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidOtherItem);
        int bItemEquipped = 0;
        int bOtherItemEquipped = 0;
        if (pItem != NULL && pInventory->GetItemInInventory(pItem))
        {
            CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
            if (pBaseItem != NULL)
            {
                for (int i = 0; i < CSWInventory::EQUIPMENT_SLOT_COUNT; ++i)
                {
                    if (((1 << i) & pBaseItem->m_nEquipableSlots) &&
                        pInventory->GetSlotFromItem(pItem) == (1 << i) &&
                        (oidOtherItem == CGameObjectArray::INVALIDOBJID ||
                         pInventory->GetSlotFromItem(pItem) == INVENTORY_SLOT_RIGHTHAND))
                        bItemEquipped = 1;
                }
            }
        }
        if (oidOtherItem == CGameObjectArray::INVALIDOBJID)
            bOtherItemEquipped = 1;
        else if (pInventory->GetItemInInventory(pOtherItem))
        {
            CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pOtherItem->GetBaseItemType());
            if (pBaseItem != NULL)
            {
                for (int i = 0; i < CSWInventory::EQUIPMENT_SLOT_COUNT; ++i)
                {
                    if (((1 << i) & pBaseItem->m_nEquipableSlots) &&
                        pInventory->GetSlotFromItem(pOtherItem) == (1 << i))
                        bOtherItemEquipped = 1;
                }
            }
        }
        if (pItem == NULL)
            return 0;
        if (!pCreature->GetItemRepository(1)->GetItemInRepository(pItem, 1) &&
            !pInventory->GetItemInInventory(pItem))
            return 0;
        if (bItemEquipped && bOtherItemEquipped)
        {
            pCreature->AddUnequipActions(pItem, CGameObjectArray::INVALIDOBJID, 1, 0, 0);
            if (pOtherItem != NULL)
                pCreature->AddUnequipActions(pOtherItem, CGameObjectArray::INVALIDOBJID, 1, 0, 0);
            return 1;
        }
        if (pOtherItem != NULL &&
            (pCreature->GetItemRepository(1)->GetItemInRepository(pOtherItem, 1) ||
             pInventory->GetItemInInventory(pOtherItem)))
        {
            if (g_pRules->m_pBaseItems->GetBaseItem(pOtherItem->GetBaseItemType())->m_nWeaponType)
                pCreature->AddEquipItemActions(pOtherItem, INVENTORY_SLOT_LEFTHAND, 1, 0, 0, 1, 0);
            else
            {
                CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pOtherItem->GetBaseItemType());
                if (pBaseItem == NULL)
                    return 0;
                for (int i = 0; i < CSWInventory::TOTAL_SLOT_COUNT; ++i)
                {
                    if ((1 << i) & pBaseItem->m_nEquipableSlots)
                    {
                        pCreature->AddEquipItemActions(pOtherItem, 1 << i, 1, 0, 0, 1, 0);
                        break;
                    }
                }
            }
        }
        if (pCreature->GetItemRepository(1)->GetItemInRepository(pItem, 1) ||
            pInventory->GetItemInInventory(pItem))
        {
            CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType());
            if (pBaseItem == NULL)
                return 0;
            if (g_pRules->m_pBaseItems->GetBaseItem(pItem->GetBaseItemType())->m_nWeaponType &&
                (pBaseItem->m_nEquipableSlots & INVENTORY_SLOT_RIGHTHAND))
                pCreature->AddEquipItemActions(pItem, INVENTORY_SLOT_RIGHTHAND, 1, 0, 0, 1, 0);
            else
            {
                for (int i = 0; i < CSWInventory::TOTAL_SLOT_COUNT; ++i)
                {
                    if ((1 << i) & pBaseItem->m_nEquipableSlots)
                    {
                        pCreature->AddEquipItemActions(pItem, 1 << i, 1, 0, 0, 1, 0);
                        break;
                    }
                }
            }
        }
        if (pOtherItem != NULL)
        {
            CSWSItem* pRightHand = pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
            CSWSItem* pLeftHand = pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
            if (pRightHand != NULL && pRightHand->m_nId != pItem->m_nId)
                pCreature->AddUnequipActions(pRightHand, CGameObjectArray::INVALIDOBJID, 1, 0, 0);
            if (pLeftHand != NULL && pLeftHand->m_nId != pOtherItem->m_nId)
                pCreature->AddUnequipActions(pLeftHand, CGameObjectArray::INVALIDOBJID, 1, 0, 0);
        }
        return 1;
    }
    case INVENTORY_PICKUP:
    {
        unsigned long oidItem = ReadOBJECTIDServer();
        unsigned long oidRepository = ReadOBJECTIDServer();
        unsigned char nPanel = ReadBYTE(8);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (pCreature != NULL)
        {
            pCreature->ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
            pCreature->AddPickUpItemActions(oidItem, oidRepository, nPanel);
            return 1;
        }
        CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
        pMessage->SendServerToPlayerInventory_PickupCancel(
            pPlayer->m_nPlayerId, oidItem);
        return 0;
    }
    case INVENTORY_UNEQUIP:
    {
        unsigned long oidItem = ReadOBJECTIDServer();
        unsigned long oidRepository = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        int nOverrideCharacter = g_pAppManager->GetClientExoApp()->GetInGameGui()
                                     ->GetTopMenuOverrideCharacter();
        if (nOverrideCharacter == -1)
        {
            pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        }
        else
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            pCreature = (CSWSCreature*)pServerExoApp->GetGameObject(
                pServerExoApp->GetPartyTable()->GetNPCObject(nOverrideCharacter, 0, 1));
            if (pCreature == NULL)
                break;
        }
        if (pCreature != NULL &&
            g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidItem, &pGameObject) == CGameObjectArray::SUCCESS &&
            pGameObject->AsSWSItem() != NULL)
        {
            pCreature->ClearAllActions(1);
            pCreature->m_pcCombatRound->RemoveAllActions();
            pCreature->AddUnequipActions(pGameObject->AsSWSItem(), oidRepository, 1, 1, 0);
            unsigned long oidAttack = pCreature->m_pcCombatRound->m_oidUnknown9CC;
            if (oidAttack != OBJECT_INVALID)
                pCreature->AddAttackActions(oidAttack, 0, 0, 0, 0, 0, 1,
                                            1, 10009, 1500, 1, 0, 0, 4, 0);
            return 1;
        }
        CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
        pMessage->SendServerToPlayerInventory_UnequipCancel(
            pPlayer->m_nPlayerId, oidItem);
        return 0;
    }
    case INVENTORY_DROP:
    {
        unsigned long oidItem = ReadOBJECTIDServer();
        float fX = ReadFLOAT(1.0f, 32);
        float fY = ReadFLOAT(1.0f, 32);
        float fZ = ReadFLOAT(1.0f, 32);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        fZ += 0.1f;
        Vector vPosition(fX, fY, fZ);
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (pCreature != NULL)
        {
            pCreature->ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
            pCreature->AddDropItemActions(oidItem, vPosition, 0);
            return 1;
        }
        CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
        pMessage->SendServerToPlayerInventory_DropCancel(
            pPlayer->m_nPlayerId, oidItem);
        return 0;
    }
    case 12:
    {
        unsigned long oidItem = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        CExoString sScript("itemuse");
        g_pVirtualMachine->RunScript(&sScript, oidItem, 1);
        return 0;
    }
    case 13:
    {
        unsigned long oidItem = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (pCreature == NULL)
            return 0;
        pCreature->LearnScroll(oidItem);
        pMessage->SendServerToPlayerInventory_LearnScroll(
            pPlayer->m_nPlayerId, oidItem, INVENTORY_LEARN_SCROLL);
        return 1;
    }
    }
    return 0;
}
// PC 0x005A5AC0 GOG 0x005A5B30
int CSWSMessage::HandlePlayerToServerLoginMessage(CSWSPlayer* pPlayer,
                                                  unsigned char nMinor)
{
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    EXOXBOXASSERT(pModule);

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMinor == 2 || nMinor == 14 || nMinor == 17)
    {
        CResRef resCharacter = ReadCResRef(16);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
        {
            return 0;
        }
        g_pAppManager->m_pServerExoApp->LoadCharacterStart(
            nMinor, pPlayer, resCharacter, NULL, 0);
        return 1;
    }
    else if (nMinor == 15)
    {
        if (MessageReadUnderflow(1) != 0 || MessageReadOverflow(1))
        {
            return 0;
        }
        if (pPlayer->m_nPlayerIndex != -1)
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            pServerExoApp->LoadCharacterStart(15, pPlayer, "", NULL, 0);
        }
        else
        {
            SendServerToPlayerLogin_NeedCharacter(pPlayer->m_nPlayerId);
        }
        return 1;
    }
    else if (nMinor == 1 || nMinor == 19)
    {
        unsigned long nSize = ReadDWORD(32);
        void* pData = ReadVOIDPtr(nSize);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
        {
            return 0;
        }
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        pServerExoApp->LoadCharacterStart(nMinor, pPlayer, "", pData, nSize);
        return 1;
    }
    else if (nMinor == 11)
    {
        int bSuccess = ReadBOOL();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
        {
            return 0;
        }
        g_pAppManager->m_pServerExoApp->LoadCharacterFinish(pPlayer, bSuccess);
        return 1;
    }
    return 0;
}
// PC 0x005A5CD0 GOG 0x005A5D40
int CSWSMessage::HandlePlayerToServerCharListMessage(CSWSPlayer* pPlayer,
                                                     unsigned char nMinor)
{
    CResRef resFile;
    unsigned char nResponse;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nMinor)
    {
    case 1:
    {
        if (MessageReadUnderflow(1) != 0)
        {
            return 0;
        }
        if (!(g_pAppManager->m_pServerExoApp->GetServerMode() == 1 ||
              g_pAppManager->m_pServerExoApp->GetServerMode() == 2))
        {
            g_pAppManager->m_pServerExoApp->AddCharListRequest(
                pPlayer->m_nPlayerId);
            return 1;
        }
        return SendServerToPlayerCharList(pPlayer);
    }
    case 3:
    {
        nResponse = ReadBYTE(8);
        resFile = ReadCResRef(16);
        if (MessageReadUnderflow(1) == 0)
        {
            return SendServerToPlayerUpdateCharResponse(pPlayer, nResponse,
                                                        resFile);
        }
        return 0;
    }
    }
    return 0;
}
// PC 0x005A5DE0 GOG 0x005A5E50
int CSWSMessage::HandlePlayerToServerModuleMessage(CSWSPlayer* pPlayer, unsigned char nMinor)
{
    CExoString sName;
    CExoString sModule;
#ifndef _XBOX
    CExoString sSaveGameName;
#endif
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    int bResult = 1;
    if (nMinor == MODULE_LOADED)
    {
        if (pPlayer->m_nAreaLoadState == CSWSPlayer::AREA_LOAD_PENDING &&
            g_pAppManager->m_pServerExoApp->GetServerMode() == CServerExoAppInternal::SERVER_MODE_RUNNING)
        {
            pPlayer->m_n7C = 1;
            g_pAppManager->m_pServerExoApp->InitiateModuleForPlayer(pPlayer);
            bResult = SendServerToPlayerPlayerList_Add(
                          CNetLayer::PLAYERID_ALL_PLAYERS, pPlayer) & 1;
            bResult &= SendServerToPlayerPlayerList_All(pPlayer);
        }
        return bResult;
    }
    if (nMinor == MODULE_SAVE_GAME)
    {
        unsigned long nSlot = ReadDWORD(32);
        sName = ReadCExoString(32);
#ifndef _XBOX
        sSaveGameName = ReadCExoString(32); g_pAppManager->GetServerExoApp()->SetSaveGameName(sSaveGameName);
#endif
        if (MessageReadUnderflow(1) == 0 && pPlayer->GetIsAllowedToSave() != 0)
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            return pServerExoApp->SaveGame(nSlot, sName, pPlayer, 0);
        }
        return 0;
    }
    else if (nMinor == MODULE_LOAD_GAME)
    {
        unsigned long nSlot = ReadDWORD(32);
        sName = ReadCExoString(32);
        sModule = ReadCExoString(32);
        if (MessageReadUnderflow(1) == 0 &&
            pPlayer->GetIsAllowedToSave() != 0 &&
            g_pAppManager->m_pServerExoApp->GetLoadingModule() != 1)
        {
            g_pAppManager->m_pServerExoApp->SetLoadingModule(1);
            SendServerToPlayerModule_Loading(pPlayer);
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            return pServerExoApp->LoadGame(nSlot, sName, sModule, pPlayer);
        }
    }
    else if (nMinor == MODULE_RUN && MessageReadUnderflow(1) == 0)
    {
        g_pAppManager->m_pServerExoApp->RunModule();
        return 1;
    }
    return 0;
}
// PC 0x005A6160 GOG 0x005A61D0
int CSWSMessage::HandlePlayerToServerAreaMessage(CSWSPlayer* pPlayer,
                                                 unsigned char nMinor)
{
    int bSuccess;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    bSuccess = 1;
    if (nMinor == 3)
    {
        if (pPlayer->m_nAreaLoadState == CSWSPlayer::AREA_LOAD_PENDING)
        {
            pPlayer->m_nAreaLoadState = CSWSPlayer::AREA_LOAD_COMPLETE;
        }
        if (g_pAppManager->m_pServerExoApp->GetServerMode() == 2)
        {
            g_pAppManager->m_pServerExoApp->MovePlayerToArea(pPlayer);
            bSuccess = SendServerToPlayerPlayerList_Add(
                           CNetLayer::PLAYERID_ALL_PLAYERS, pPlayer) &
                       1;
            bSuccess &= SendServerToPlayerPlayerList_All(pPlayer);
            if (g_pAppManager->m_pServerExoApp->GetPauseState(1) == 1)
            {
                SendServerToPlayerModule_SetPauseState(1, 1);
            }
            if (g_pAppManager->m_pServerExoApp->GetPauseState(2) == 1)
            {
                SendServerToPlayerModule_SetPauseState(2, 1);
            }
        }
        return bSuccess;
    }
    return 0;
}
// PC 0x005A6220 GOG 0x005A6290
int CSWSMessage::HandlePlayerToServerServerStatusMessage(CSWSPlayer* pPlayer,
                                                         unsigned char nMinor)
{
    if (!(MessageReadOverflow(1) || nMinor != 0))
    {
        unsigned long nPlayerId = pPlayer->m_nPlayerId;
        return SendServerToPlayerServerStatus_Status(nPlayerId);
    }
    return 0;
}
// PC 0x005A6250 GOG 0x005A62C0
int CSWSMessage::HandlePlayerToServerMapPinSetMapPinAt(CSWSPlayer* pPlayer)
{
    Vector vPosition;
    CExoString sNote;
    CExoString sVariable;
    CSWSObject* pGameObject;
    CSWSObject* pCreature;
    CSWSScriptVarTable* pVariables;
    int nMapPin;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    vPosition.m_fX = ReadFLOAT(1.0f, 32);
    vPosition.m_fY = ReadFLOAT(1.0f, 32);
    vPosition.m_fZ = ReadFLOAT(1.0f, 32);
    sNote = ReadCExoString(32);
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    pGameObject = pPlayer->GetGameObject();
    if (pGameObject == NULL)
    {
        return 0;
    }
    pCreature = (CSWSObject*)pGameObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 0;
    }
    pVariables = &pCreature->m_lScriptVars;
    if (pVariables == NULL)
    {
        return 0;
    }
    CExoString sCount("NW_TOTAL_MAP_PINS");
    nMapPin = pVariables->GetInt(sCount) + 1;
    pVariables->SetInt(sCount, nMapPin, 0);
    sVariable.Format("NW_MAP_PIN_NTRY_%i", nMapPin);
    pVariables->SetString(sVariable, sNote);
    sVariable.Format("NW_MAP_PIN_XPOS_%i", nMapPin);
    pVariables->SetFloat(sVariable, vPosition.m_fX);
    sVariable.Format("NW_MAP_PIN_YPOS_%i", nMapPin);
    pVariables->SetFloat(sVariable, vPosition.m_fY);
    sVariable.Format("NW_MAP_PIN_ZPOS_%i", nMapPin);
    pVariables->SetFloat(sVariable, vPosition.m_fZ);
    sVariable.Format("NW_MAP_PIN_AREA_%i", nMapPin);
    pVariables->SetObject(sVariable, pCreature->GetArea()->GetId());
    SendServerToPlayerMapPinAdded(pPlayer, vPosition, sNote, nMapPin);
    return 1;
}
// PC 0x005A64F0 GOG 0x005A6560
int CSWSMessage::HandlePlayerToServerMapPinDestroyMapPin(CSWSPlayer* pPlayer)
{
    CExoString sVariable;
    unsigned long nMapPin;
    CSWSObject* pGameObject;
    CSWSObject* pCreature;
    CSWSScriptVarTable* pVariables;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    nMapPin = ReadINT(32);
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    pGameObject = pPlayer->GetGameObject();
    if (pGameObject == NULL)
    {
        return 0;
    }
    pCreature = (CSWSObject*)pGameObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 0;
    }
    pVariables = &pCreature->m_lScriptVars;
    if (pVariables == NULL)
    {
        return 0;
    }
    sVariable.Format("NW_MAP_PIN_NTRY_%i", nMapPin);
    pVariables->DestroyString(sVariable);
    sVariable.Format("NW_MAP_PIN_XPOS_%i", nMapPin);
    pVariables->DestroyFloat(sVariable);
    sVariable.Format("NW_MAP_PIN_YPOS_%i", nMapPin);
    pVariables->DestroyFloat(sVariable);
    sVariable.Format("NW_MAP_PIN_ZPOS_%i", nMapPin);
    pVariables->DestroyFloat(sVariable);
    sVariable.Format("NW_MAP_PIN_AREA_%i", nMapPin);
    pVariables->DestroyObject(sVariable);
    return 1;
}
// PC 0x005A6A30 GOG 0x005A6AA0
int CSWSMessage::HandlePlayerToServerInputAbortDriveControl(CSWSPlayer* pPlayer)
{
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    CSWSObject* pObject = pPlayer->GetGameObject();
    if (pObject != NULL)
    {
        CSWSObject* pCreature = (CSWSObject*)pObject->AsSWSCreature();
        if (pCreature != NULL && pCreature->m_bCommandable == 1)
        {
            CExoLinkedList<CSWSObjectActionNode>* pActionList =
                &pCreature->m_lstActions;
            if (pActionList != NULL)
            {
                CSWSObjectActionNode* pAction = pActionList->GetHead();
                if (pAction != NULL && pAction->m_nActionType == 51)
                {
                    pAction->m_apParam[5] = NULL;
                }
            }
        }
        return 1;
    }
    return 0;
}
// PC 0x005A6AA0 GOG 0x005A6B10
int CSWSMessage::HandlePlayerToServerInputMessage(CSWSPlayer* pPlayer,
                                                  unsigned char nMinor)
{
    CSWSCreature* pCreature = NULL;
    CSWSCreatureStats* pStats = NULL;
    CSWSObject* pObject = pPlayer->GetGameObject();
    if (pObject != NULL)
    {
        pCreature = pObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            pStats = pCreature->m_pStats;
            if (pCreature->m_bRestorePlotOnDownload)
                pCreature = NULL;
        }
    }
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nMinor)
    {
    case 1:
        return HandlePlayerToServerInputWalkToWaypoint(pPlayer);
    case 29:
        return HandlePlayerToServerInputDriveControl(pPlayer);
    case 33:
        return HandlePlayerToServerInputAbortDriveControl(pPlayer);
    case 34:
        return HandlePlayerToServerInputCancelGuiTimingEvent(pPlayer);
    case 2:
    {
        unsigned long oidTarget = ReadOBJECTIDServer();
        unsigned short nAction = ReadWORD(16);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) || !pCreature)
            return 0;
        pCreature->AddAttackActions(oidTarget, nAction, 1, 0, 0, 0, 0,
                                    1, 10009, 1500, 1, 0, 0, 4, 0);
        if (pCreature->m_bInParty)
        {
            CGameObject* pTarget =
                g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
            int bTargetCreature = 0;
            if (pTarget != NULL && pTarget->AsSWSCreature() != NULL)
                bTargetCreature = 1;
            int nMembers = g_pAppManager->GetClientExoApp()->GetSWParty()->GetNumCharacters();
            if (!g_pAppManager->GetServerExoApp()->GetPartyTable()->m_bSoloMode &&
                bTargetCreature)
            {
                for (int nMember = 1; nMember < nMembers; ++nMember)
                {
                    CSWSCreature* pMember = g_pAppManager->GetClientExoApp()
                        ->GetSWParty()->GetCharacter(nMember)->GetServerCreature();
                    if (pMember->m_oidAttemptedAttackTarget == OBJECT_INVALID)
                    {
                        pMember->AddAttackActions(oidTarget, 0, 0, 0, 1, 1, 1,
                                                  1, 10009, 1500, 1, 0, 0, 4, 0);
                        pMember->m_oidAttemptedMoveToTarget = oidTarget;
                    }
                }
            }
        }
        if (pCreature->m_oidAttemptedAttackTarget != oidTarget)
            pCreature->m_oidAttemptedMoveToTarget = oidTarget;
        return 1;
    }
    case 6:
    {
        unsigned short nFeat = ReadWORD(16);
        unsigned short nAction = ReadWORD(16);
        unsigned long oidTarget = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) ||
            !pCreature || !pCreature->GetArea())
            return 0;
        pCreature->UseFeat(nFeat, nAction, oidTarget, pCreature->GetArea()->GetId());
        return 1;
    }
    case 7:
    {
        unsigned char nSkill = ReadBYTE(8);
        unsigned char nSubSkill = ReadBYTE(8);
        unsigned long oidTarget = ReadOBJECTIDServer();
        Vector vTarget;
        vTarget.m_fX = ReadFLOAT(1.0f, 32);
        vTarget.m_fY = ReadFLOAT(1.0f, 32);
        vTarget.m_fZ = ReadFLOAT(1.0f, 32);
        if (!MessageReadOverflow(1) && !MessageReadUnderflow(1) && pCreature)
            pCreature->UseSkill(nSkill, nSubSkill, oidTarget, vTarget,
                pCreature->GetArea()->GetId(), OBJECT_INVALID, 0);
        return 0;
    }
    case 9:
    {
        Vector vTarget;
        unsigned long oidItem = ReadOBJECTIDServer();
        unsigned char nProperty = ReadBYTE(8);
        unsigned char nOption = ReadBOOL() ? ReadBYTE(8) : 0;
        unsigned long oidTarget = ReadBOOL() == 1 ? ReadOBJECTIDServer() :
            CGameObjectArray::INVALIDOBJID;
        if (ReadBOOL() == 1)
        {
            vTarget.m_fX = ReadFLOAT(1.0f, 32);
            vTarget.m_fY = ReadFLOAT(1.0f, 32);
            vTarget.m_fZ = ReadFLOAT(1.0f, 32);
        }
        if (!MessageReadOverflow(1) && !MessageReadUnderflow(1) && pCreature)
        {
            unsigned long oidArea = pCreature->GetArea()->GetId();
            if (!pCreature->m_bCombatState)
                pCreature->ActionManager(1);
            if (!pCreature->UseItem(oidItem, nProperty, nOption, oidTarget,
                                    vTarget, oidArea, 1))
                pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_ITEM_NO_USES, NULL);
        }
        return 0;
    }
    case 36:
    {
        unsigned long oidCreature = ReadOBJECTIDServer();
        unsigned long oidItem = ReadOBJECTIDServer();
        unsigned long oidPlaceable = ReadOBJECTIDServer();
        int nCount = ReadINT(32);
        if (!MessageReadOverflow(1) && !MessageReadUnderflow(1))
        {
            CSWSCreature* pGiver = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidCreature);
            CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
            CSWSPlaceable* pPlaceable = g_pAppManager->m_pServerExoApp->GetPlaceableByGameObjectID(oidPlaceable);
            if (pGiver && pItem && pPlaceable)
                pGiver->AddGiveItemActions(oidItem, oidPlaceable, nCount, 0);
        }
        return 0;
    }
    case 10:
    {
        unsigned char nMode = ReadBYTE(8);
        if (!MessageReadOverflow(1))
        {
            unsigned long oidTarget;
            if (nMode == 5)
                oidTarget = ReadOBJECTIDServer();
            if (!MessageReadOverflow(1) && !MessageReadUnderflow(1) &&
                pCreature && pCreature->m_bCommandable)
            {
                if (nMode == 5)
                    pCreature->AddCounterSpellActions(oidTarget);
                pCreature->ToggleMode(nMode);
            }
        }
        return 0;
    }
    case 5:
    {
        unsigned long oidTarget = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) || !pCreature)
            return 0;
        CGameObject* pTarget;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidTarget, &pTarget) == CGameObjectArray::SUCCESS)
        {
            switch (pTarget->m_nObjectType)
            {
            case SW_OBJECT_TYPE_CREATURE:
                return SendServerToPlayerExamineGui_CreatureData(pPlayer, oidTarget);
            case SW_OBJECT_TYPE_ITEM:
                pCreature->UseLoreOnItem(oidTarget);
                return SendServerToPlayerExamineGui_ItemData(pPlayer, oidTarget);
            case SW_OBJECT_TYPE_PLACEABLE:
                return SendServerToPlayerExamineGui_PlaceableData(pPlayer, oidTarget);
            case SW_OBJECT_TYPE_TRIGGER:
            {
                pCreature->UseSkill(CSWRules::SKILL_DEMOLITIONS, 102, oidTarget, Vector(),
                    pCreature->GetArea()->GetId(), OBJECT_INVALID, 0);
                return 1;
            }
            case SW_OBJECT_TYPE_DOOR:
                return SendServerToPlayerExamineGui_DoorData(pPlayer, oidTarget);
            default:
            {
                CExoString sMessage("Feature not implemented: Examine unhandled for this object type.");
                SendServerToPlayerChatMessage(5, OBJECT_INVALID,
                                              sMessage, pPlayer->GetPlayerId(), "");
                break;
            }
            }
        }
        return 1;
    }
    case 11:
    {
        unsigned long oidObject = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) || !pCreature)
        {
            return 0;
        }
        if (pCreature != NULL)
        {
            pCreature->ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
            pCreature->AddUseObjectAction(oidObject);
            return 1;
        }
        return 0;
    }
    case 3:
    {
        unsigned long oidDoor = ReadOBJECTIDServer();
        unsigned short nAction = ReadWORD(16);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) || !pCreature)
        {
            return 0;
        }
        if (pCreature != NULL)
        {
            pCreature->ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
            if (nAction == 10021)
            {
                pCreature->AddOpenDoorAction(oidDoor, 0);
            }
            else
            {
                pCreature->AddCloseDoorAction(oidDoor, 0);
            }
            return 1;
        }
        return 0;
    }
    case 12:
    {
        unsigned long oidTarget = ReadOBJECTIDServer();
        unsigned long oidKey = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) ||
            !pCreature || !pCreature->m_bCommandable)
            return 0;
        pCreature->ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
        CGameObject* pTarget;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidTarget, &pTarget) == CGameObjectArray::SUCCESS)
        {
            if (pTarget->AsSWSDoor() && pTarget->AsSWSDoor()->m_bLocked)
                pCreature->AddUnlockObjectAction(oidTarget, oidKey, 0);
            if (pTarget->AsSWSPlaceable() && pTarget->AsSWSPlaceable()->m_bLocked)
                pCreature->AddUnlockObjectAction(oidTarget, oidKey, 0);
        }
        return 1;
    }
    case 14:
    {
        unsigned long oidTarget = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) || !pCreature)
            return 0;
        pCreature->ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
        CGameObject* pTarget;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidTarget, &pTarget) == CGameObjectArray::SUCCESS)
        {
            if (pTarget->AsSWSDoor() && !pTarget->AsSWSDoor()->m_bLocked)
                pCreature->AddLockObjectAction(oidTarget);
            if (pTarget->AsSWSPlaceable() && !pTarget->AsSWSPlaceable()->m_bLocked)
                pCreature->AddLockObjectAction(oidTarget);
        }
        return 1;
    }
    case 4:
    {
        unsigned short nAnimationId = ReadWORD(16);
        unsigned long oidTarget = ReadOBJECTIDServer();
        float fX = ReadFLOAT(1.0f, 32);
        float fY = ReadFLOAT(1.0f, 32);
        float fZ = ReadFLOAT(1.0f, 32);
        float fSpeed = 1.0f;
        float fDuration = 0.0f;
        int nAnimation = nAnimationId;
        int bLooping = 1;
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) ||
            !pCreature || !pCreature->m_bCommandable)
            return 0;
        if (pCreature->m_nActivity & CSWSCreature::ACTIVITY_REST)
            pCreature->ClearAllActions(1);
        if (pCreature->m_nId == oidTarget)
        {
            pCreature->AddActionToFront(CSWSObject::ACTION_PLAY_ANIMATION,
                CSWSObject::ACTION_GROUP_NEW,
                1, &nAnimation, 2, &fSpeed, 2, &fDuration, 1, &bLooping,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL);
        }
        else
        {
            if (oidTarget != OBJECT_INVALID)
                pCreature->AddAction(CSWSObject::ACTION_CHANGE_FACING_OBJECT,
                    CSWSObject::ACTION_GROUP_NEW, 3, &oidTarget,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL);
            else
                pCreature->AddAction(CSWSObject::ACTION_CHANGE_FACING_POINT,
                    CSWSObject::ACTION_GROUP_NEW,
                    2, &fX, 2, &fY, 2, &fZ,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
            pCreature->AddAction(CSWSObject::ACTION_PLAY_ANIMATION,
                CSWSObject::ACTION_GROUP_LAST,
                1, &nAnimation, 2, &fSpeed, 2, &fDuration, 1, &bLooping,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL,
                0, NULL, 0, NULL, 0, NULL, 0, NULL);
        }
        return 1;
    }
    case 8:
    {
        if (!pCreature)
            return 0;
        unsigned long oidTarget = ReadOBJECTIDServer();
        unsigned long oidOther = OBJECT_INVALID;
        pCreature->ActionManager(CSWSCreature::ACTION_MANAGER_REST);
        CExoString sDialog("");
        int nUnknown = 0;
        int bPrivate = 1;
        int bPlayHello = 0;
        if (!pCreature->m_nAILevel)
            g_pAppManager->m_pServerExoApp->GetServerAIMaster()->SetAILevel(pObject, 1);
        pObject->AddAction(CSWSObject::ACTION_START_CONVERSATION,
            CSWSObject::ACTION_GROUP_NEW,
            3, &oidTarget, 4, &sDialog, 1, &nUnknown, 1, &bPrivate,
            1, &bPlayHello, 3, &oidOther,
            0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        return 1;
    }
    case 13:
        if (pCreature && pCreature->m_bCommandable)
        {
            if (!pCreature->GetActivity(CSWSCreature::ACTIVITY_REST))
                pCreature->Rest(0);
        }
        else
            pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_UNKNOWN_213, NULL);
        return 0;
    case 16:
        ReadBYTE(8);
        ReadDWORD(32);
    case 17:
        ReadBYTE(8);
        ReadBYTE(8);
        ReadBYTE(8);
        if (!MessageReadOverflow(1))
        {
            MessageReadUnderflow(1);
        }
        return 0;
    case 18:
    {
        int bTargetLocation = 0;
        Vector vTarget;
        unsigned long oidTarget = OBJECT_INVALID;
        unsigned long nSpell = ReadDWORD(32);
        unsigned char nClass = ReadBYTE(8);
        unsigned char nLevel = ReadBYTE(8);
        unsigned char nIndex = ReadBYTE(8);
        unsigned char nFlags = ReadBYTE(8);
        if (nFlags & 1)
            oidTarget = ReadOBJECTIDServer();
        if (nFlags & 2)
        {
            vTarget.m_fX = ReadFLOAT(1.0f, 32);
            vTarget.m_fY = ReadFLOAT(1.0f, 32);
            vTarget.m_fZ = ReadFLOAT(1.0f, 32);
            bTargetLocation = 1;
        }
        int bItemSpell = (nFlags >> 2) & 1;
        if (!MessageReadOverflow(1) && !MessageReadUnderflow(1) && pCreature && pStats)
        {
            g_pRules->m_pSpellArray->GetSpell(nSpell);
            if (!bItemSpell && pStats->GetSpellUsesLeft(nSpell, nClass, nLevel, nIndex) > 0)
            {
                if (pCreature->m_bCombatState != 1)
                    pCreature->ActionManager(1);
                pCreature->AddCastSpellActions(nSpell, nClass, nLevel, nIndex,
                    bItemSpell, vTarget, oidTarget, 1, bTargetLocation, 1, 0, 0, 0,
                    CSWSCreature::SPELL_ACTION_OPTION10_UNSET,
                    CSWSCreature::SPELL_ACTION_OPTION11_UNSET, 0);
                if (pCreature->m_bInParty)
                {
                    CSWPartyTable* pPartyTable =
                        g_pAppManager->GetServerExoApp()->GetPartyTable();
                    if (pPartyTable != NULL &&
                        (pPartyTable->m_nForfeitConditions & FORFEIT_NO_FORCE_POWERS))
                    {
                        pPartyTable->m_nForfeitViolation = 1;
                        CSWSArea* pArea = pCreature->GetArea();
                        if (pArea != NULL)
                        {
                            CScriptEvent* pEvent = new CScriptEvent;
                            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_USER_DEFINED_EVENT;
                            pEvent->SetInteger(0, USER_DEFINED_EVENT_FORFEIT_VIOLATION);
                            g_pAppManager->GetServerExoApp()->GetServerAIMaster()
                                ->AddEventDeltaTime(0, 0, pArea->m_nId, pArea->m_nId,
                                                    CServerAIMaster::EVENT_SIGNAL_EVENT,
                                                    pEvent);
                        }
                    }
                }
            }
        }
        return 0;
    }
    case 35:
    {
        Vector vTarget;
        unsigned long oidTarget = OBJECT_INVALID;
        unsigned long nSpell = ReadDWORD(32);
        unsigned char nLevel = ReadBYTE(8);
        unsigned char nFlags = ReadBYTE(8);
        if (nFlags & 1)
            oidTarget = ReadOBJECTIDServer();
        if (nFlags & 2)
        {
            vTarget.m_fX = ReadFLOAT(1.0f, 32);
            vTarget.m_fY = ReadFLOAT(1.0f, 32);
            vTarget.m_fZ = ReadFLOAT(1.0f, 32);
        }
        if (!MessageReadOverflow(1) && !MessageReadUnderflow(1) && pCreature && pStats)
        {
            g_pRules->m_pSpellArray->GetSpell(nSpell);
            pCreature->ActionManager(1);
            if (oidTarget != CGameObjectArray::INVALIDOBJID)
                pCreature->AddUseTalentOnObjectActions(CScriptTalent::TALENT_TYPE_SPELL,
                    nSpell, oidTarget, CSWSObject::CASTING_CLASS_INNATE_FE,
                    CGameObjectArray::INVALIDOBJID, CSWSCreature::ITEM_PROPERTY_INVALID,
                    nLevel, 0);
            else
                pCreature->AddUseTalentAtLocationActions(CScriptTalent::TALENT_TYPE_SPELL,
                    nSpell, vTarget, CSWSObject::CASTING_CLASS_INNATE_FE,
                    CGameObjectArray::INVALIDOBJID, CSWSCreature::ITEM_PROPERTY_INVALID,
                    nLevel, 0);
        }
        return 0;
    }
    case 20:
    {
        unsigned short nGroupId = ReadWORD(16);
        unsigned short nActionId = ReadWORD(16);
        if (!MessageReadOverflow(1))
        {
            if (pCreature != NULL)
            {
                pCreature->CancelAction(nGroupId, nActionId);
            }
        }
        return 0;
    }
    case 21:
    {
        unsigned long oidTarget = ReadOBJECTIDServer();
        if (!MessageReadOverflow(1))
        {
            CGameObject* pTarget = g_pAppManager->m_pServerExoApp->GetGameObject(oidTarget);
            if (pTarget != NULL)
            {
                switch (pTarget->m_nObjectType)
                {
                case SW_OBJECT_TYPE_CREATURE:
                    SendServerToPlayerDebugInfo_Creature(pPlayer, oidTarget);
                    break;
                case SW_OBJECT_TYPE_ITEM:
                    SendServerToPlayerDebugInfo_Item(pPlayer, oidTarget);
                    break;
                case SW_OBJECT_TYPE_PLACEABLE:
                    SendServerToPlayerDebugInfo_Placeable(pPlayer, oidTarget);
                    break;
                case SW_OBJECT_TYPE_AREA:
                    SendServerToPlayerDebugInfo_Area(pPlayer, oidTarget);
                    break;
                default:
                    return 0;
                }
            }
        }
        return 0;
    }
    case 22:
    {
        CSWSItem* pContainerItem = NULL;
        int bCanMerge = 0;
        unsigned long oidItem = ReadOBJECTIDServer();
        unsigned long oidMergeItem = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || !pCreature || !pCreature->m_bCommandable)
            return 0;
        CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
        CSWSItem* pMergeItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidMergeItem);
        if (!pItem || !pMergeItem)
            return 0;
        if (pItem->m_oidPossessor == pMergeItem->m_oidPossessor &&
            pCreature->m_nId == pMergeItem->m_oidPossessor)
            bCanMerge = 1;
        if (pMergeItem->m_oidPossessor == pCreature->m_nId)
        {
            bCanMerge = 1;
        }
        else
        {
            pContainerItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(
                pMergeItem->m_oidPossessor);
            if (pContainerItem != NULL && pContainerItem->m_oidPossessor == pCreature->m_nId)
                bCanMerge = 1;
        }
        if (pPlayer->m_pContainerGUI->m_oidContainer == pMergeItem->m_oidPossessor &&
            pItem->m_oidPossessor == pCreature->m_nId)
        {
            if (pContainerItem != NULL)
            {
                if (pContainerItem->m_nId != pMergeItem->m_oidPossessor)
                    bCanMerge = 1;
            }
            else
            {
                CSWSPlaceable* pPlaceable = g_pAppManager->GetServerExoApp()->GetPlaceableByGameObjectID(
                    pMergeItem->m_oidPossessor);
                if (pPlaceable != NULL)
                {
                    pPlaceable->RemoveItem(pMergeItem);
                    if (!pCreature->GetItemRepository(1)->AddItem(&pMergeItem, 0, 0, 1))
                        pPlaceable->m_pcItemRepository->AddItem(&pMergeItem, 0, 0, 1);
                    else
                    {
                        pMergeItem->SetPossessor(pCreature->m_nId, 1, 0,
                                                OBJECT_INVALID);
                        bCanMerge = 1;
                    }
                }
            }
        }
        if (bCanMerge != 1)
            return 0;
        pCreature->MergeItem(pItem, pMergeItem);
        pItem->SetPossessor(pCreature->m_nId, 1, 0, OBJECT_INVALID);
        return 0;
    }
    case 23:
    {
        unsigned long oidItem = ReadOBJECTIDServer();
        unsigned short nCount = ReadWORD(16);
        if (!MessageReadOverflow(1) && pCreature && pCreature->m_bCommandable)
        {
            CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
            if (pItem != NULL)
                pCreature->SplitItem(pItem, nCount);
        }
        return 0;
    }
    case 24:
        if (pCreature && !pCreature->m_bRestorePlotOnDownload && pCreature->GetArea() &&
            g_pAppManager->m_pServerExoApp->GetServerInfo()->m_bPauseAndPlay)
            g_pAppManager->m_pServerExoApp->TogglePauseState(2);
        return 0;
    case 25:
    {
        int bPause = ReadBOOL();
        if (!MessageReadOverflow(1) && !MessageReadUnderflow(1) &&
            (!pCreature || !pCreature->m_bRestorePlotOnDownload))
            g_pAppManager->m_pServerExoApp->SetPauseState(CServerExoAppInternal::PAUSE_STATE_COMBAT, bPause);
        return 0;
    }
    case 31:
        g_pAppManager->m_pServerExoApp->GetSWSMessage()->SendServerToPlayerModule_ExportReply(pPlayer);
        return 0;
    case 26:
        ReadBOOL();
        if (!MessageReadOverflow(1))
        {
            pPlayer->m_n60 = 1;
        }
        return 0;
    case 28:
    {
        Vector vOrientation;
        vOrientation.m_fX = ReadFLOAT(1.0f, 32);
        vOrientation.m_fY = ReadFLOAT(1.0f, 32);
        vOrientation.Normalize();
        if (!MessageReadOverflow(1))
        {
            CSWSObject* pControlled = pPlayer->GetGameObject();
            if (pControlled != NULL)
            {
                int bDeadTemp = 0;
                if (pControlled->AsSWSCreature())
                    bDeadTemp = pControlled->AsSWSCreature()->GetDeadTemp();
                if (pControlled->m_bCommandable && pControlled->GetIsPCDying() != 1 &&
                    pControlled->GetDead() != 1 && bDeadTemp != 1 &&
                    (!pCreature || !pCreature->m_bRestorePlotOnDownload))
                    pControlled->SetOrientation(vOrientation);
            }
        }
        return 0;
    }
    case 30:
    {
        unsigned long oidCreature = ReadOBJECTIDServer();
        if (MessageReadOverflow(1))
            return 0;
        CExoArrayList<CGameEffect*>* pEffects = NULL;
        CSWSCreature* pTarget = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidCreature);
        if (pTarget && !pTarget->m_bRestorePlotOnDownload)
            pEffects = &pTarget->m_lstEffects;
        if (pEffects != NULL)
        {
            for (int i = 0; i < pEffects->num; ++i)
            {
                if ((*pEffects)[i]->m_nType ==
                    CSWSEffectListHandler::EFFECT_DISGUISE)
                {
                    pTarget->RemoveEffectById((*pEffects)[i]->m_nID);
                    break;
                }
            }
        }
        return 0;
    }
    case 37:
    {
        CGameObject* pObject = NULL;
        unsigned long oidDoor = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) || !pCreature ||
            !pCreature->m_bCommandable)
            return 0;
        pCreature->ActionManager(2);
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidDoor, &pObject) == CGameObjectArray::SUCCESS)
        {
            CSWSDoor* pDoor = pObject->AsSWSDoor();
            if (pDoor == NULL)
                return 0;
            if (pDoor->m_bLocked)
                pCreature->AddDoorSaberAction(oidDoor);
        }
        return 1;
    }
    case 38:
    {
        ReadOBJECTIDServer();
        int bInstant = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        if (MessageReadUnderflow(1))
            return 0;
        CSWSCreature* pSwitcher = pPlayer->GetGameObject()->AsSWSCreature();
        if (pSwitcher == NULL)
            return 0;
        return pSwitcher->AddSwitchWeaponsAction(bInstant);
    }
    case 39:
    {
        char nStance = ReadCHAR(8);
        unsigned long oidTarget = ReadOBJECTIDServer();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        pCreature->AddStanceActions(nStance, oidTarget, 1, 0, 0, 0);
        return 1;
    }
    default:
        break;
    }
    return 0;
}
// PC 0x005A81B0 GOG 0x005A8220
int CSWSMessage::HandlePlayerToServerParty(CSWSPlayer* pPlayer,
                                           unsigned char nMinor)
{
    NwshandleplayermessageTrace1();
    STR_RES strRes;
    CExoString sMessage;

    if (MessageReadOverflow(1) || pPlayer == NULL)
    {
        return 0;
    }
    switch (nMinor)
    {
    case 6:
    {
        ReadOBJECTIDServer();
        unsigned long oidCreature = pPlayer->m_oidPCObject;
        CSWSCreature* pCreature =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
        if (g_pAppManager->GetServerExoApp()->GetServerInfo()->m_nXboxUnmapped0D0 == 1)
        {
            if (pCreature != NULL)
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_UNKNOWN_196, NULL);
            }
            break;
        }
        CSWSPlayer* pClient =
            g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidCreature);
        if (pCreature != NULL && pClient != NULL)
        {
            CSWSFaction* pFaction = pCreature->GetFaction();
            if (pFaction != NULL)
            {
                pFaction->RemoveMember(oidCreature);
                unsigned long* pMembers = pFaction->m_lstMembers.element;
                for (int i = 0; i < pFaction->GetMemberCount(); ++i)
                {
                    CSWSCreature* pMember = g_pAppManager->GetServerExoApp()->
                        GetCreatureByGameObjectID(pMembers[i]);
                    if (pMember != NULL)
                    {
                        CSWCCMessageData* pData = new CSWCCMessageData;
                        pData->SetObjectID(0, oidCreature);
                        pMember->SendFeedbackMessage(
                            CSWSCreature::FEEDBACK_PARTY_MEMBER_LEFT, pData);
                    }
                }
                pCreature = g_pAppManager->GetServerExoApp()->
                    GetCreatureByGameObjectID(oidCreature);
                if (pCreature != NULL)
                {
                    pCreature->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_UNKNOWN_045, NULL);
                }
            }
        }
        break;
    }
    case 7:
    {
        unsigned long oidCreature = ReadOBJECTIDServer();
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (g_pAppManager->GetServerExoApp()->GetServerInfo()->m_nXboxUnmapped0D0 == 1)
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_UNKNOWN_197, NULL);
            break;
        }
        CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->
            GetCreatureByGameObjectID(oidCreature);
        CSWSPlayer* pClient = g_pAppManager->GetServerExoApp()->
            GetClientObjectByObjectId(oidCreature);
        CSWSFaction* pFaction = pCreature->GetFaction();
        if (pTarget != NULL && pClient != NULL && pCreature != NULL &&
            pFaction != NULL)
        {
            if (pFaction->AttemptRemove(oidCreature, pCreature->GetId()) == 1)
            {
                unsigned long* pMembers = pFaction->m_lstMembers.element;
                for (int i = 0; i < pFaction->GetMemberCount(); ++i)
                {
                    CSWSCreature* pMember = g_pAppManager->GetServerExoApp()->
                        GetCreatureByGameObjectID(pMembers[i]);
                    if (pMember != NULL)
                    {
                        CSWCCMessageData* pData = new CSWCCMessageData;
                        pData->SetObjectID(0, oidCreature);
                        pMember->SendFeedbackMessage(
                            CSWSCreature::FEEDBACK_UNKNOWN_032, pData);
                    }
                }
                CSWSCreature* pRemoved = g_pAppManager->GetServerExoApp()->
                    GetCreatureByGameObjectID(oidCreature);
                if (pRemoved != NULL)
                {
                    pRemoved->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_UNKNOWN_033, NULL);
                }
            }
            if (pTarget->m_bNewAttackTarget)
            {
                pTarget->m_bNewAttackTarget = FALSE;
            }
        }
        break;
    }
    case 8:
    {
        unsigned long oidCreature = ReadOBJECTIDServer();
        CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->
            GetCreatureByGameObjectID(oidCreature);
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        CSWSFaction* pFaction = pCreature->GetFaction();
        if (pTarget != NULL && pCreature != NULL && pFaction != NULL)
        {
            pFaction->TransferLeadership(oidCreature, pCreature->GetId());
            unsigned long* pMembers = pFaction->m_lstMembers.element;
            for (int i = 0; i < pFaction->GetMemberCount(); ++i)
            {
                CSWSCreature* pMember = g_pAppManager->GetServerExoApp()->
                    GetCreatureByGameObjectID(pMembers[i]);
                if (pMember != NULL)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData;
                    pData->SetObjectID(0, oidCreature);
                    pMember->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_PARTY_LEADER_CHANGED, pData);
                }
            }
        }
        break;
    }
    case 9:
    {
        unsigned long oidCreature = ReadOBJECTIDServer();
        CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->
            GetCreatureByGameObjectID(oidCreature);
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (pTarget != NULL && pCreature != NULL)
        {
            if (pTarget->m_bInvitedToParty || pCreature->GetInvitedToParty())
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, oidCreature);
                pCreature->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_UNKNOWN_034, pData);
            }
            else if (pTarget->GetNumInvited() != 0)
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, oidCreature);
                pCreature->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_UNKNOWN_035, pData);
            }
            else if (pTarget->GetIsInInvitationsIgnored(pCreature->GetId()) == 1)
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, oidCreature);
                pCreature->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_UNKNOWN_039, pData);
            }
            else if (!pTarget->GetFaction()->GetSingletonParty())
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, oidCreature);
                pCreature->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_UNKNOWN_202, pData);
            }
            else
            {
                CSWSFaction* pFaction = pCreature->GetFaction();
                if (g_pAppManager->GetServerExoApp()->
                        GetClientObjectByObjectId(oidCreature) != NULL &&
                    pFaction != NULL &&
                    pFaction->InviteMember(oidCreature, pCreature->GetId()) == 1)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData;
                    pData->SetObjectID(0, pCreature->GetId());
                    pTarget->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_UNKNOWN_037, pData);
                    pData = new CSWCCMessageData;
                    pData->SetObjectID(0, oidCreature);
                    pCreature->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_UNKNOWN_036, pData);
                    g_pAppManager->GetServerExoApp()->GetSWSMessage()->
                        SendServerToPlayerPopUpGUIPanel(
                            oidCreature, 1, 0, 0, 0,
                            pCreature->GetStats()->GetFullName());
                }
            }
        }
        break;
    }
    case 10:
    {
        unsigned long oidInviter = ReadOBJECTIDServer();
        CSWSCreature* pInviter = g_pAppManager->GetServerExoApp()->
            GetCreatureByGameObjectID(oidInviter);
        g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidInviter);
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (pCreature != NULL &&
            pCreature->ReplyToInvitation(2, pInviter->GetId()) == 1)
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            pData->SetObjectID(0, pInviter->GetId());
            pCreature->SendFeedbackMessage(
                CSWSCreature::FEEDBACK_UNKNOWN_040, pData);
            pData = new CSWCCMessageData;
            pData->SetObjectID(0, pCreature->GetId());
            pInviter->SendFeedbackMessage(
                CSWSCreature::FEEDBACK_UNKNOWN_039, pData);
        }
        else
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            pData->SetObjectID(0, pCreature->GetId());
            pInviter->SendFeedbackMessage(
                CSWSCreature::FEEDBACK_INVITATION_EXPIRED, pData);
        }
        break;
    }
    case 11:
    {
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (pCreature != NULL)
        {
            unsigned long oidInviter = ReadOBJECTIDServer();
            if (oidInviter == OBJECT_INVALID)
            {
                oidInviter = pCreature->m_oidInvitedBy;
            }
            CSWSCreature* pInviter = g_pAppManager->GetServerExoApp()->
                GetCreatureByGameObjectID(oidInviter);
            if (pInviter != NULL)
            {
                if (pCreature->ReplyToInvitation(0, pInviter->GetId()) == 1)
                {
                    unsigned long* pMembers =
                        pInviter->GetFaction()->m_lstMembers.element;
                    for (int i = 0; i < pInviter->GetFaction()->GetMemberCount(); ++i)
                    {
                        CSWSCreature* pMember = g_pAppManager->GetServerExoApp()->
                            GetCreatureByGameObjectID(pMembers[i]);
                        if (pMember != NULL)
                        {
                            CSWCCMessageData* pData = new CSWCCMessageData;
                            pData->SetObjectID(0, pCreature->GetId());
                            pMember->SendFeedbackMessage(
                                CSWSCreature::FEEDBACK_PARTY_MEMBER_JOINED, pData);
                        }
                    }
                }
                else
                {
                    CSWCCMessageData* pData = new CSWCCMessageData;
                    pData->SetObjectID(0, pInviter->GetId());
                    pCreature->SendFeedbackMessage(
                        CSWSCreature::FEEDBACK_INVITATION_EXPIRED, pData);
                }
            }
        }
        break;
    }
    case 12:
    {
        unsigned long oidInviter = ReadOBJECTIDServer();
        CSWSCreature* pInviter = g_pAppManager->GetServerExoApp()->
            GetCreatureByGameObjectID(oidInviter);
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        CSWSPlayer* pClient = g_pAppManager->GetServerExoApp()->
            GetClientObjectByObjectId(oidInviter);
        if (pInviter != NULL && pClient != NULL && pCreature != NULL)
        {
            if (pCreature->ReplyToInvitation(1, pInviter->GetId()) == 1)
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, pInviter->GetId());
                pCreature->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_UNKNOWN_042, pData);
                pData = new CSWCCMessageData;
                pData->SetObjectID(0, pCreature->GetId());
                pInviter->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_UNKNOWN_041, pData);
            }
            else
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, pInviter->GetId());
                pCreature->SendFeedbackMessage(
                    CSWSCreature::FEEDBACK_INVITATION_EXPIRED, pData);
            }
        }
        break;
    }
    case 2:
    {
        int nNumMembers = 0;
        unsigned long* pMembers = NULL;
        if (pPlayer->GetGameObject() != NULL)
        {
            CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
            CSWSFaction* pFaction = pCreature->GetFaction();
            pMembers = pFaction->m_lstMembers.element;
            nNumMembers = pFaction->m_lstMembers.num;
            SendServerToPlayerParty_List(pPlayer->GetPlayerId(), nNumMembers,
                                         pMembers, 1, pCreature->GetId());
        }
        else
        {
            SendServerToPlayerParty_List(pPlayer->GetPlayerId(), nNumMembers,
                                         pMembers, 1, OBJECT_INVALID);
        }
        break;
    }
    default:
        return 0;
    }
    return 1;
}
// PC 0x005A8CD0 GOG 0x005A8D40
int CSWSMessage::HandlePlayerToServerMapPinMessage(CSWSPlayer* pPlayer,
                                                   unsigned char nMinor)
{
    switch (nMinor)
    {
    case 2:
    {
        return HandlePlayerToServerMapPinSetMapPinAt(pPlayer);
    }
    case 3:
    {
        return HandlePlayerToServerMapPinDestroyMapPin(pPlayer);
    }
    case 5:
    {
        return HandlePlayerToServerMapPinChangePin(pPlayer);
    }
    default:
    {
        break;
    }
    }
    return 1;
}
// PC 0x005A6660 GOG 0x005A66D0
int CSWSMessage::HandlePlayerToServerMapPinChangePin(CSWSPlayer* pPlayer)
{
    Vector vPosition;
    CExoString sNote;
    CExoString sVariable;
    unsigned long nMapPin;
    CSWSObject* pGameObject;
    CSWSObject* pCreature;
    CSWSScriptVarTable* pVariables;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    vPosition.m_fX = ReadFLOAT(1.0f, 32);
    vPosition.m_fY = ReadFLOAT(1.0f, 32);
    vPosition.m_fZ = ReadFLOAT(1.0f, 32);
    sNote = ReadCExoString(32);
    nMapPin = ReadDWORD(32);
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    pGameObject = pPlayer->GetGameObject();
    if (pGameObject == NULL)
    {
        return 0;
    }
    pCreature = (CSWSObject*)pGameObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 0;
    }
    pVariables = &pCreature->m_lScriptVars;
    if (pVariables == NULL)
    {
        return 0;
    }
    sVariable.Format("NW_MAP_PIN_NTRY_%i", nMapPin);
    pVariables->SetString(sVariable, sNote);
    sVariable.Format("NW_MAP_PIN_XPOS_%i", nMapPin);
    pVariables->SetFloat(sVariable, vPosition.m_fX);
    sVariable.Format("NW_MAP_PIN_YPOS_%i", nMapPin);
    pVariables->SetFloat(sVariable, vPosition.m_fY);
    sVariable.Format("NW_MAP_PIN_ZPOS_%i", nMapPin);
    pVariables->SetFloat(sVariable, vPosition.m_fZ);
    sVariable.Format("NW_MAP_PIN_AREA_%i", nMapPin);
    pVariables->SetObject(sVariable, pCreature->GetArea()->GetId());
    return 1;
}
// PC 0x005A6880 GOG 0x005A68F0
int CSWSMessage::HandlePlayerToServerPlayerDeath(CSWSPlayer* pPlayer,
                                                 unsigned char nMinor)
{
    switch (nMinor)
    {
    case 1:
    {
        unsigned long oidModule =
            g_pAppManager->m_pServerExoApp->GetModule()->GetId();
        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_SPAWN_BUTTON_DOWN;
        unsigned long oidPlayer = pPlayer->GetGameObject()->GetId();
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, oidPlayer, oidModule, CServerAIMaster::EVENT_SIGNAL_EVENT,
            pEvent);
        break;
    }
    }
    return 1;
}
// PC 0x005A6930 GOG 0x005A69A0
int CSWSMessage::HandlePlayerToServerCharacterDownload(CSWSPlayer* pPlayer,
                                                       unsigned char nMinor)
{
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nMinor)
    {
    case 1:
    {
        if (pPlayer->m_nCharacterType != CHARACTER_TYPE_LOCAL)
        {
            return SendServerToPlayerCharacterDownloadFail(pPlayer);
        }
        return SendServerToPlayerCharacterDownloadReply(pPlayer);
    }
    default:
    {
        return 0;
    }
    }
}
// PC 0x005A6970 GOG 0x005A69E0
int CSWSMessage::HandlePlayerToServerShutDownServer(CSWSPlayer* pPlayer,
                                                    unsigned char nMinor)
{
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    int bSave = ReadBOOL();
    if (!(MessageReadOverflow(1) || MessageReadUnderflow(1) != 0))
    {
        g_pAppManager->m_pServerExoApp->Shutdown(bSave == 0, 1);
        return 1;
    }
    return 0;
}
// PC 0x005A69D0 GOG 0x005A6A40
int CSWSMessage::HandlePlayerToServerPlayModuleCharacterList_Stop(
    CSWSPlayer* pPlayer)
{
    if (!(MessageReadOverflow(1) || MessageReadUnderflow(1) != 0))
    {
        pPlayer->m_nPlayModuleCharacterListState = 0;
        return 1;
    }
    return 0;
}
// PC 0x005A8DE0 GOG 0x005A8E50
int CSWSMessage::HandlePlayerToServerLevelUpMessage(CSWSPlayer* pPlayer,
                                                    unsigned char nMinor)
{
    int nResult = 0;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMinor == 0)
    {
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (pCreature == NULL)
        {
            return 0;
        }

        CSWLevelStats* pLevelStats = new CSWLevelStats;
        pLevelStats->m_nClass = ReadBYTE(8);
        if ((pCreature->m_pStats->GetLevel(0) + 1) % 4 == 0)
        {
            pLevelStats->m_nAbilityIncrease = ReadBYTE(8);
        }

        CSWClass* pClass = &g_pRules->m_pClasses[(unsigned char)pLevelStats->m_nClass];
        pLevelStats->m_nHitDie = pClass->m_nHitDie;
        pLevelStats->m_nForceDie = pClass->m_nForceDie;

        unsigned long nSkillMask = ReadDWORD(32);
        unsigned char nSkill;
        for (nSkill = 0; nSkill < g_pRules->m_nNumSkills; nSkill++)
        {
            if ((nSkillMask & (1 << nSkill)) != 0)
            {
                pLevelStats->SetSkillRankChange(nSkill, ReadCHAR(8));
            }
        }

        pLevelStats->m_nSkillPoints = ReadWORD(16);
        unsigned char nFeatCount = ReadBYTE(8);
        while (nFeatCount != 0)
        {
            pLevelStats->AddFeat(ReadWORD(16));
            --nFeatCount;
        }

        unsigned char nFirstCount = ReadBYTE(8);
        while (nFirstCount != 0)
        {
            pLevelStats->m_lstSpellsAdded[0].Add(ReadDWORD(32));
            --nFirstCount;
        }

        unsigned char nSecondCount = ReadBYTE(8);
        while (nSecondCount != 0)
        {
            pLevelStats->m_lstSpellsRemoved[0].Add(ReadDWORD(32));
            --nSecondCount;
        }

        if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
        {
            return 0;
        }

        int bValid = pCreature->m_pStats->ValidateLevelUp(pLevelStats);
        if (bValid == 0)
        {
            delete pLevelStats;
        }
        nResult = SendServerToPlayerLevelUp_Confirmation(pPlayer->GetPlayerId(),
                                                         bValid);
        if (pCreature->m_pStats->CanLevelUp() != 0)
        {
            pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_LEVEL_UP, NULL);
        }
        if (pCreature->m_bPlayerCreated != 0)
        {
            for (int nNPC = 0; nNPC < CSWPartyTable::MAX_NPCS; nNPC++)
            {
                unsigned long oidNPC = g_pAppManager->GetServerExoApp()
                    ->GetPartyTable()->GetNPCObject(nNPC, 0, 1);
                if (oidNPC != OBJECT_INVALID)
                {
                    CSWSCreature* pNPC = g_pAppManager->GetServerExoApp()
                        ->GetCreatureByGameObjectID(oidNPC);
                    if (pNPC != NULL)
                    {
                        pNPC->HandleAlignmentInfluence();
                    }
                }
            }
        }
    }
    return nResult;
}
// PC 0x005A9160 GOG 0x005A91D0
int CSWSMessage::HandlePlayerToServerPlayModuleCharacterList(
    CSWSPlayer* pPlayer, unsigned char nMinor)
{
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nMinor)
    {
    case 1:
    {
        return HandlePlayerToServerPlayModuleCharacterList_Start(pPlayer);
    }
    case 2:
    {
        return HandlePlayerToServerPlayModuleCharacterList_Stop(pPlayer);
    }
    default:
    {
        return 0;
    }
    }
}
// PC 0x005A8D20 GOG 0x005A8D90
int CSWSMessage::HandlePlayerToServerPlayModuleCharacterList_Start(
    CSWSPlayer* pPlayer)
{
    CExoLinkedList<CSWSClient>* pPlayerList;
    CExoLinkedListNode* pNode;
    CExoLinkedListNode* pNext;
    CSWSPlayer* pOtherPlayer;
    unsigned long oidOther;
    CSWSMessage* pMessage;

    if (!(MessageReadOverflow(1) || MessageReadUnderflow(1) != 0))
    {
        pPlayerList = g_pAppManager->m_pServerExoApp->GetPlayerList();
        pNode = pPlayerList->GetHeadPos();
        for (pNext = pNode; pNext != NULL; pNode = pNext)
        {
            pOtherPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pNode);
            if (pOtherPlayer != NULL && pOtherPlayer->GetGameObject() != NULL)
            {
                oidOther = pOtherPlayer->GetGameObject()->m_nId;
                if (oidOther != CGameObjectArray::INVALIDOBJID)
                {
                    pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
                    if (pMessage != NULL)
                    {
                        pMessage
                            ->SendServerToPlayerPlayModuleCharacterListResponse(
                                pPlayer->m_nPlayerId, oidOther, 1);
                    }
                }
            }
            pPlayerList->GetNext(pNext);
        }
        pPlayer->m_nPlayModuleCharacterListState = 1;
        return 1;
    }
    return 0;
}
// PC 0x005A91A0 GOG 0x005A9210
int CSWSMessage::HandlePlayerToServerJournalMessage(CSWSPlayer* pPlayer,
                                                    unsigned char nMinor)
{
    CSWSPlayerJournalGUI* pJournalGUI;
    CExoString sTag;
    CExoString sText;
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();

    switch (nMinor)
    {
    case JOURNAL_QUEST_SCREEN_OPEN:
        pJournalGUI = pPlayer->m_pJournalGUI;
        pJournalGUI->m_bFullUpdate = 1;
        SendServerToPlayerJournalFullUpdate(pPlayer);
        break;
    case JOURNAL_QUEST_SCREEN_CLOSE:
    {
        pJournalGUI = pPlayer->m_pJournalGUI;
        pJournalGUI->m_bFullUpdate = 0;
        CSWSPlayerJournalGUI* pQuestJournalGUI = pPlayer->m_pJournalGUI;
        pQuestJournalGUI->m_lstQuestUpdates.SetSize(0);
        break;
    }
    case JOURNAL_ADD_WORLD_ENTRY:
    {
        sTag = ReadCExoString(32);
        sText = ReadCExoString(32);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
        {
            return 0;
        }
        if (pModule != NULL)
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            pServerExoApp->GetWorldTimer()->GetWorldTime(&nCalendarDay,
                                                         &nTimeOfDay);
            pModule->AddWorldJournalEntry(pModule->GetWorldJournalIndexUnique(),
                                          sText, sTag, nCalendarDay,
                                          nTimeOfDay);
        }
        break;
    }
    case JOURNAL_DELETE_WORLD_ENTRY:
    {
        int nIndex = ReadINT(32);
        int bStrRef = ReadBOOL();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
        {
            return 0;
        }
        if (pModule != NULL)
        {
            if (bStrRef != 0)
            {
                pModule->DeleteWorldJournalEntryStrref(nIndex);
            }
            else
            {
                pModule->DeleteWorldJournalEntry(nIndex);
            }
        }
        break;
    }
    }
    return 1;
}
// PC 0x005A93F0 GOG 0x005A9460
int CSWSMessage::HandlePlayerToServerMessage(unsigned long nPlayerId,
                                             unsigned char* pMessage,
                                             unsigned long nSize)
{
    CExoString sMessageType;
    unsigned char nMessageId = pMessage[0];
    unsigned char nMajor = pMessage[1];
    unsigned char nMinor = pMessage[2];
    int nResult = 0;

    SetReadMessage(pMessage + 3, nSize - 3, 0xFFFFFFFF, 1);
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMessageId != 112)
    {
        return 0;
    }
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSPlayer* pPlayer =
        pServerExoApp->GetClientObjectByPlayerId(nPlayerId, 1);
    if (pPlayer == NULL)
    {
        return 1;
    }
    sMessageType.Format("unknown Major (0x%.2X)", nMajor);
    switch (nMajor)
    {
    case 1:
        sMessageType = "ServerStatus";
        nResult = HandlePlayerToServerServerStatusMessage(pPlayer, nMinor);
        break;
    case 2:
        sMessageType = "Login";
        nResult = HandlePlayerToServerLoginMessage(pPlayer, nMinor);
        break;
    case 3:
        sMessageType = "Module";
        nResult = HandlePlayerToServerModuleMessage(pPlayer, nMinor);
        break;
    case 6:
        sMessageType = "Input";
        nResult = HandlePlayerToServerInputMessage(pPlayer, nMinor);
        break;
    case 8:
        sMessageType = "Gold";
        nResult = HandlePlayerToServerGoldMessage(pPlayer, nMinor);
        break;
    case 9:
        sMessageType = "Chat";
        nResult = HandlePlayerToServerChatMessage(pPlayer, nMinor);
        break;
    case 11:
        sMessageType = "Chat";
        nResult = 1;
        break;
    case 12:
        sMessageType = "Inventory";
        nResult = HandlePlayerToServerInventoryMessage(pPlayer, nMinor);
        break;
    case 13:
        sMessageType = "GuiInventory";
        nResult = HandlePlayerToServerGuiInventoryMessage(pPlayer, nMinor);
        break;
    case 25:
        sMessageType = "GuiContainer";
        nResult = HandlePlayerToServerGuiContainerMessage(pPlayer, nMinor);
        break;
    case 14:
        sMessageType = "Party";
        nResult = HandlePlayerToServerParty(pPlayer, nMinor);
        break;
    case 15:
        sMessageType = "Cheat";
        nResult = HandlePlayerToServerCheatMessage(pPlayer, nMinor);
        break;
    case 17:
        sMessageType = "CharList";
        nResult = HandlePlayerToServerCharListMessage(pPlayer, nMinor);
        break;
    case 20:
        sMessageType = "Dialog";
        nResult = HandlePlayerToServerDialogMessage(pPlayer, nMinor);
        break;
    case 21:
        sMessageType = "GuiCharacterSheet";
        nResult = HandlePlayerToServerCharacterSheetMessage(pPlayer, nMinor);
        break;
    case 4:
        sMessageType = "Area";
        nResult = HandlePlayerToServerAreaMessage(pPlayer, nMinor);
        break;
    case 22:
        sMessageType = "QuickChat";
        nResult = HandlePlayerToServerQuickChatMessage(pPlayer, nMinor);
        break;
    case 29:
        sMessageType = "LevelUp";
        nResult = HandlePlayerToServerLevelUpMessage(pPlayer, nMinor);
        break;
    case 28:
        sMessageType = "Journal";
        nResult = HandlePlayerToServerJournalMessage(pPlayer, nMinor);
        break;
    case 30:
        sMessageType = "GuiQuickbar";
        nResult = 1;
        break;
    case 32:
        sMessageType = "MapPin";
        nResult = HandlePlayerToServerMapPinMessage(pPlayer, nMinor);
        break;
    case 5:
        sMessageType = "GameObjUpdate";
        nResult = HandlePlayerToServerGameObjectUpdate(pPlayer, nMinor);
        break;
    case 37:
        sMessageType = "Death";
        nResult = HandlePlayerToServerPlayerDeath(pPlayer, nMinor);
        break;
    case 43:
        sMessageType = "Character_Download";
        nResult = HandlePlayerToServerCharacterDownload(pPlayer, nMinor);
        break;
    case 47:
        sMessageType = "ShutDownServer";
        nResult = HandlePlayerToServerShutDownServer(pPlayer, nMinor);
        break;
    case 49:
        sMessageType = "PlayModuleCharacterList";
        nResult = HandlePlayerToServerPlayModuleCharacterList(pPlayer, nMinor);
        break;
    }

    CExoString sError;
    if (MessageReadOverflow(0))
    {
        sError.Format("Server message OVERFLOW in %s (0x%.2X)\nFIX this or "
                      "dream up something nasty to do to the person "
                      "responsible!",
                      sMessageType.CStr(), nMinor);
    }
    if (MessageReadUnderflow(0) != 0)
    {
        sError.Format("Server message UNDERFLOW in %s (0x%.2X)\nFIX this or "
                      "dream up something nasty to do to the person "
                      "responsible!",
                      sMessageType.CStr(), nMinor);
    }
    return nResult;
}

static inline void NwshandleplayermessageTrace1()
{
}

#ifdef _XBOX

int CSWSMessage::HandlePlayerToServerDialogMessage(CSWSPlayer* pPlayer,
                                                   unsigned char nMinor)
{
    unsigned long oidObject;
    unsigned long nReplyIndex;
    int bEscape;
    CServerExoApp* pServerExoApp;
    CGameObject* pObject;
    unsigned long nEntryId;
    unsigned char nResult;

    if (MessageReadOverflow(1) || nMinor != 3) return 0;
    {
        oidObject = ReadOBJECTIDServer();
        nReplyIndex = ReadDWORD(32);
        bEscape = (unsigned char)ReadBYTE(8) != 0;
        nEntryId = ReadDWORD(32);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1)) return 0;
        {
            pServerExoApp = g_pAppManager->m_pServerExoApp;
            nResult = pServerExoApp->GetObjectArray()->GetGameObject(
                oidObject, &pObject);
            if (nResult != CGameObjectArray::SUCCESS) return 0;
            {
                return ((CSWSObject*)pObject)
                    ->ReplyDialog(pPlayer->GetPlayerId(), nReplyIndex, bEscape,
                                  nEntryId, 0);
            }
        }
    }
    return 0;
}
#endif
