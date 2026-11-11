#include <stdio.h>
#include <stdlib.h>
#include "guiingame.h"
#include "nwfeat.h"
#include "swmain.h"
#include "swparty.h"

#include "exobase.h"
#include "appmanager.h"
#include "exotlktable.h"
#include "nwtlktable.h"
#include "clientexoapp.h"
#include "nwmessage.h"
#include "aurcamera.h"
#include "clientoptions.h"
#include "nwcsoundobject.h"
#include "gameobjectarray.h"
#include "clientaimaster.h"
#include "nwcambientsound.h"
#include "nwcarea.h"
#include "nwcmappin.h"
#include "nwcmodule.h"
#include "nwcareaofeffectobject.h"
#include "nwcjournal.h"
#include "nwcitem.h"
#include "nwctrigger.h"
#include "nwcplaceable.h"
#include "nwcdoor.h"
#include "nwcprojectile.h"
#include "nwsobject.h"
#include "nwsitem.h"
#include "nwsplayer.h"
#include "swguiingame.h"
#include "swguicustompanel.h"
#include "nwccreature.h"
#include "nwrules.h"
#include "nwitem.h"
#include "nwcanimbase.h"
#include "clientexoappinternal.h"
#include "nwccreaturestats.h"
#include "soundset.h"
#include "gameobjectarray.h"
#include "netlayer.h"
#include "nwcanimbase.h"
#include "serverexoapp.h"
#include "nwsplaceable.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "nwspell.h"
#include "nwcspellvisual.h"
#include "nwccreatureappearance.h"
#include "nwscript.h"
#include "nwreentrantserver.h"
#include "serverinfo.h"
#include "partytables.h"
#include "nwscreature.h"

static inline void NwchandlemessageTrace3();
static inline void NwchandlemessageTrace2();
static inline void NwchandlemessageTrace1();
#define NWCHANDLEMESSAGE_TRACE() NwchandlemessageTrace3()

static inline CClientExoApp* ClientExoAppLate3(CAppManager* p);
static inline CClientExoApp* ClientExoAppLate2(CAppManager* p);
static inline CClientExoApp* ClientExoAppLate1(CAppManager* p);

int enablemodelrequest = 1;
int g_bLogConversations = 0;
// PC 0x0064B4D0  Xbox 0x00070B50
void CSWCProjectile::SetDesiredPosition(const Vector& vPosition)
{
    m_vDesiredPosition = vPosition;
}
// PC 0x0064B4F0  Xbox 0x0016E480
void CSWCProjectile::SetSourcePosition(const Vector& vPosition)
{
    m_vSourcePosition = vPosition;
}
// PC 0x0064B510  Xbox 0x00070B70
CSWCMessage::CSWCMessage()
{
}
// PC 0x0064B530  Xbox 0x00070B90
CSWCMessage::~CSWCMessage()
{
}
// PC 0x00654CC0  Xbox 0x0007A4C0
int CSWCMessage::HandleServerToPlayerChat(unsigned char nMinor)
{
    NwchandlemessageTrace1();
    Vector vPosition;
    CExoString sMessage;
    CExoString sBubbleMessage;
    CResRef cVoice;
    CResRef cLip;
    CExoString sPrefix;
    CExoString sSpeakerName;
    STR_RES strRes;
    CSWCCreature* pCreature = NULL;
    CSWCObject* pObject = NULL;
    CGameObject* pGameObject = NULL;
    float fDuration = 0.0f;
    CClientExoApp* pClientApp = g_pAppManager->GetClientExoApp();
    unsigned long oidSpeaker = OBJECT_INVALID;
    int bShowBubble = 1;
    int bAllowMissingObject = 0;
    float fScale;
    char szPlayerId[16];

    if (MessageReadOverflow(1))
        return 0;

    if (nMinor != CHAT_SERVER_TELL && nMinor != CHAT_SERVER_STRREF)
        oidSpeaker = ReadOBJECTIDClient();
    else if (nMinor == CHAT_SERVER_STRREF)
        oidSpeaker = ReadOBJECTIDClient();

    if (nMinor == CHAT_MULTILANG_LISTED_RECIPIENT || nMinor == CHAT_MULTILANG)
    {
        fScale = 0.75f;
        bAllowMissingObject = 1;
        unsigned long oidTokenObject = ReadOBJECTIDClient();
        sMessage = ReadCExoLocStringClient(cLip, &fDuration);
        cVoice = ReadCResRef(16);
        if (!cVoice.IsValid())
            cVoice = cLip;
        unsigned long oidSoundObject = ReadOBJECTIDClient();
        CGameObject* pSoundObject = pClientApp->GetGameObject(oidSoundObject);
        if (pSoundObject != NULL && pSoundObject->AsSWCObject() != NULL)
        {
            pObject = pSoundObject->AsSWCObject();
            if (pObject->m_pVocalization != NULL &&
                pObject->m_nVocalizationFlag != 0)
                pObject->m_pVocalization->Stop();
        }
        int bSuppressEmpty = ReadINT(32);
        if (sMessage == "" && bSuppressEmpty == 1)
            return 1;
        pGameObject = pClientApp->GetGameObject(oidTokenObject);
        sBubbleMessage = sMessage;
        g_pSWTlkTable->ParseStr(sBubbleMessage, pGameObject, 0, 1);
        g_pSWTlkTable->ParseStr(sMessage, pGameObject, 0, 0);
    }
    else
    {
        fScale = 1.0f;
        if (nMinor == CHAT_STRREF_TALK || nMinor == CHAT_STRREF_SHOUT ||
            nMinor == CHAT_STRREF_WHISPER || nMinor == CHAT_SERVER_STRREF)
        {
            STR_RES strMessage;
            unsigned long nStrRef = ReadDWORD(32);
            if (nStrRef != -1 &&
                g_pTlkTable->Fetch(nStrRef, strMessage, CTlkTable::GENDER_MALE))
                sMessage = strMessage.m_sString;
            sMessage == "";
        }
        else
            sMessage = ReadCExoString(32);
        sBubbleMessage = sMessage;
    }

    sPrefix = "";
    if (nMinor == CHAT_SHOUT || nMinor == CHAT_TELL || nMinor == CHAT_PARTY)
    {
        bAllowMissingObject = 1;
        ReadFLOAT(1.0f, 32);
        ReadFLOAT(1.0f, 32);
        ReadFLOAT(1.0f, 32);
        sPrefix = ReadCExoLocStringClient(CResRef(), NULL);
        sPrefix = sPrefix + " " + ReadCExoLocStringClient(CResRef(), NULL);
    }
    if (MessageReadOverflow(1) || MessageReadUnderflow(1))
        return 0;

    if (nMinor != CHAT_SERVER_TELL && nMinor != CHAT_SERVER_STRREF)
    {
        pGameObject = pClientApp->GetGameObject(oidSpeaker);
        if (pGameObject == NULL && !bAllowMissingObject)
            return 0;
    }
    if (pGameObject != NULL)
    {
        pCreature = pGameObject->AsSWCCreature();
        pObject = pGameObject->AsSWCObject();
        if (pCreature == NULL && pObject == NULL && !bAllowMissingObject)
            return 0;
    }
    CSWCPlayer* pPlayer = g_pAppManager->GetClientExoApp()
        ->GetPlayerByGameObjectID(oidSpeaker);
    if (sPrefix == "" && nMinor != CHAT_SERVER_TELL &&
        nMinor != CHAT_SERVER_STRREF)
    {
        if (pCreature != NULL)
            sPrefix = pCreature->GetStats()->GetFullName();
        else if (pObject == NULL ||
            !g_pAppManager->GetClientExoApp()->GetObjectName(pObject->GetId(), sPrefix))
        {
            STR_RES strUnknownSpeaker;

            g_pTlkTable->Fetch(1360, strUnknownSpeaker, CTlkTable::GENDER_MALE);
            sPrefix = strUnknownSpeaker.m_sString;
        }
    }
    CGuiInGame* pGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
    if (pGui == NULL)
        return 0;
    if (!pGui->m_bItemCreateMenuAvailable)
        return 0;
    sSpeakerName = sPrefix;
    sPrefix = sPrefix;
    unsigned long nMessageMask;
    switch (nMinor)
    {
    case CHAT_MULTILANG_LISTED_RECIPIENT:
        bShowBubble = 0;
    case CHAT_TALK:
    case CHAT_STRREF_TALK:
    case CHAT_MULTILANG:
        sPrefix = sPrefix + ": ";
        nMessageMask = CGuiInGame::MESSAGE_MASK_CHAT_TALK;
        break;
    case CHAT_SHOUT:
    case CHAT_STRREF_SHOUT:
        g_pTlkTable->Fetch(0, strRes, CTlkTable::GENDER_MALE);
        sPrefix = sPrefix + ": " + strRes.m_sString.CStr();
        nMessageMask = CGuiInGame::MESSAGE_MASK_CHAT_SHOUT;
        break;
    case CHAT_WHISPER:
    case CHAT_STRREF_WHISPER:
        g_pTlkTable->Fetch(0, strRes, CTlkTable::GENDER_MALE);
        sPrefix = sPrefix + ": " + strRes.m_sString.CStr();
        nMessageMask = CGuiInGame::MESSAGE_MASK_CHAT_WHISPER;
        break;
    case CHAT_TELL:
        g_pTlkTable->Fetch(0, strRes, CTlkTable::GENDER_MALE);
        sPrefix = sPrefix + ": " + strRes.m_sString.CStr();
        nMessageMask = CGuiInGame::MESSAGE_MASK_CHAT_TELL;
        break;
    case CHAT_SERVER_TELL:
    case CHAT_SERVER_STRREF:
        return 1;
    case CHAT_PARTY:
        g_pTlkTable->Fetch(0, strRes, CTlkTable::GENDER_MALE);
        sPrefix = sPrefix + ": " + strRes.m_sString.CStr();
        nMessageMask = CGuiInGame::MESSAGE_MASK_CHAT_PARTY;
        break;
    case CHAT_SOUND:
        break;
    default:
        return 0;
    }

    if (pObject != NULL)
    {
        vPosition = pObject->m_vPosition;
        vPosition.m_fZ += 2.0f;
    }
    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    if (pModule == NULL)
        return 0;
    CAurCamera* pCamera = pModule->m_pModuleCamera;
    if (pCamera == NULL)
        return 0;
    vPosition = pCamera->MapToScreen(vPosition, false);
    if (cVoice.IsValid())
    {
        if (pObject != NULL)
        {
            pObject->NewVocalization(cVoice, 1);
            if (pObject->m_pVocalization != NULL)
            {
                pObject->m_pVocalization->Set3D(0);
                pObject->m_pVocalization->SetPosition(pObject->m_vPosition, 0.0f);
                if (pCreature != NULL &&
                    g_pAppManager->GetClientExoApp()->GetPlayerCreature() == pCreature)
                    pObject->m_pVocalization->SetPriorityGroup(
                        CExoStreamingSoundSource::PRIORITY_GROUP_PLAYER_VOICE);
                else
                    pObject->m_pVocalization->SetPriorityGroup(
                        CExoStreamingSoundSource::PRIORITY_GROUP_DIALOG_VOICE);
                pObject->m_pVocalization->Play();
                pObject->LipSync(cVoice);
            }
        }
    }
    if (pObject != NULL)
        pGui->m_oidDialogSoundObject = pObject->GetId();

    if (nMinor == CHAT_SOUND)
    {
        if (pObject != NULL)
        {
            CResRef cSound(sMessage);
            g_pExoSound->Play3DOneShotSound(cSound,
                pObject->m_vPosition, 0.0f, CExoSound::PRIORITY_GROUP_CHAT_SOUND,
                0, 0, 0.0f, 0.0f);
        }
        else
        {
            CResRef cSound(sMessage);
            g_pExoSound->PlayOneShotSound(cSound,
                CExoSound::PRIORITY_GROUP_CHAT_SOUND, 0, 0, 0.0f, 0.0f);
        }
    }
    else
    {
        if (g_bLogConversations)
        {
            if (pPlayer != NULL)
            {
                if (pCreature != NULL)
                    pGui->AppendToMsgBuffer(sPrefix + sMessage, nMessageMask, 0);
                else if (pObject != NULL)
                {
                    if (pObject->GetPortrait(1).GetResRef()[0] == 0)
                        pGui->AppendToMsgBuffer(sPrefix + sMessage, nMessageMask, 0);
                    else
                        pGui->AppendToMsgBuffer(sPrefix + sMessage, nMessageMask, 0);
                }
                else
                    pGui->AppendToMsgBuffer(sPrefix + sMessage, nMessageMask, 0);
            }
            else if (pCreature != NULL)
                pGui->AppendToMsgBuffer(sPrefix + sMessage, nMessageMask, 0);
            else if (pObject != NULL)
            {
                if (pObject->GetPortrait(1).GetResRef()[0] == 0)
                    pGui->AppendToMsgBuffer(sPrefix + sMessage, nMessageMask, 0);
                else
                    pGui->AppendToMsgBuffer(sPrefix + sMessage, nMessageMask, 0);
            }
            else
                pGui->AppendToMsgBuffer(sPrefix + sMessage, nMessageMask, 0);
        }
        if (bShowBubble)
        {
            CSWCCreature* pPlayerCreature = g_pAppManager->GetClientExoApp()->GetPlayerCreature();
            if (pPlayerCreature != NULL && pPlayer != NULL)
            {
                CSWCPlayer* pLocalPlayer = g_pAppManager->GetClientExoApp()
                    ->GetPlayerByGameObjectID(pPlayerCreature->GetId());
                sPrefix = "";
                if (pLocalPlayer == NULL || pPlayer->GetPlayerId() != pLocalPlayer->GetPlayerId())
                {
                    sprintf(szPlayerId, "%u", pPlayer->GetPlayerId());
                    sPrefix = "[";
                    sPrefix = sPrefix + sSpeakerName;
                    sPrefix = sPrefix + "]: ";
                }
            }
            if (pObject != NULL)
            {
                int nObjectType = pObject->m_nObjectType;
                switch (nObjectType)
                {
                case SW_OBJECT_TYPE_TRIGGER:
                case SW_OBJECT_TYPE_PLACEABLE:
                case SW_OBJECT_TYPE_DOOR:
                    sPrefix = CExoString("[") + sSpeakerName + "]: ";
                    break;
                default:
                    sPrefix = "";
                    break;
                }
                pObject->SetPopUpBubbleText(sPrefix + sBubbleMessage, 1, fScale, fDuration);
                pObject->m_nIdleDay = 1;
            }
        }
    }
    return 1;
}
// PC 0x00655E40  Xbox 0x0007B670
int CSWCMessage::HandleServerToPlayerCreatureUpdate_Appearance()
{

    CExoString sFirstName;
    CExoString sLastName;
    unsigned long oidObject = ReadOBJECTIDClient();
    unsigned short nUpdateMask = ReadWORD(16);

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetClientExoApp()->GetGameObjectArray();
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidObject);
    if (pCreature == NULL)
    {
        return 0;
    }
    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    if (pModule == NULL)
    {
        return 0;
    }
    CSWCArea* pArea = pModule->m_pInfo;
    if (pArea == NULL)
    {
        return 0;
    }

    if (nUpdateMask & 0x400)
    {
        sFirstName = ReadCExoLocStringClient(CResRef(), NULL);
        sLastName = ReadCExoLocStringClient(CResRef(), NULL);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        pCreature->SetName(sFirstName, sLastName);
    }
    if (MessageReadOverflow(1))
    {
        return 0;
    }

    CCAppearanceInfo sAppearance = *pCreature->GetAppearance();
    int bReAttach = 0;
    unsigned char nPartMask = 0;
    if (nUpdateMask & 0x1)
    {
        unsigned short nAppearance = ReadWORD(16);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (nAppearance != sAppearance.m_nAppearance)
        {
            if (pCreature->m_pLoopingSound != NULL)
            {
                pCreature->DeleteLoopingCreatureSound();
            }
            int nSoundAppType;
            g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(
                nAppearance, "soundapptype", &nSoundAppType);
            CExoString sLooping;
            if (g_pRules->Get2DArrays()->GetAppearanceSounds()->GetCExoStringEntry(
                    nSoundAppType, "Looping", &sLooping))
            {
                pCreature->SetNewLoopingCreatureSound(sLooping);
            }
        }
        sAppearance.m_nAppearance = nAppearance;
    }
    if (nUpdateMask & 0x2)
    {
        sAppearance.m_nHeadVariation = ReadBYTE(8);
    }
    if (nUpdateMask & 0x4)
    {
        sAppearance.m_nGender = ReadBYTE(8);
    }
    if (nUpdateMask & 0x80)
    {
        sAppearance.m_nHead = ReadBYTE(8);
    }
    if (nUpdateMask & 0x4000)
    {
        sAppearance.m_nUnknown30 = ReadBOOL();
    }
    if (nUpdateMask & 0x8000)
    {
        sAppearance.m_nUnknown34 = ReadBYTE(8);
    }
    if (nUpdateMask & 0x800)
    {
        sAppearance.m_nTail = ReadBYTE(8);
    }
    if (nUpdateMask & 0x1000)
    {
        sAppearance.m_nWing = ReadBYTE(8);
    }
    if (nUpdateMask & 0x8)
    {
        sAppearance.m_nUnknown1D = ReadBYTE(8);
    }
    if (nUpdateMask & 0x10)
    {
        sAppearance.m_nUnknown1E = ReadBYTE(8);
    }
    if (nUpdateMask & 0x20)
    {
        sAppearance.m_nUnknown1F = ReadBYTE(8);
    }
    if (nUpdateMask & 0x40)
    {
        sAppearance.m_nUnknown20 = ReadBYTE(8);
    }
    if (nUpdateMask & 0x2000)
    {
        pCreature->SetSoundSet(ReadWORD(16));
    }

    CSWCCreatureAppearance* pAppearance = pCreature->GetAppearance();
    unsigned short nCurrentAppearance = pAppearance->m_nAppearance;
    if (sAppearance.m_nAppearance != nCurrentAppearance ||
        sAppearance.m_nHeadVariation != pAppearance->m_nHeadVariation ||
        sAppearance.m_nGender != pAppearance->m_nGender)
    {
        nPartMask = 0xEF;
        bReAttach = 1;
    }
    if (sAppearance.m_nHead != pAppearance->m_nHead ||
        sAppearance.m_nUnknown34 != pAppearance->m_nUnknown34 ||
        sAppearance.m_nUnknown30 != pAppearance->m_nUnknown30)
    {
        nPartMask |= CSWCCreature::APPEARANCE_PART_HEAD;
    }
    if (sAppearance.m_nTail != pAppearance->m_nTail)
    {
        nPartMask |= CSWCCreature::APPEARANCE_PART_TAIL;
    }
    if (sAppearance.m_nWing != pAppearance->m_nWing)
    {
        nPartMask |= CSWCCreature::APPEARANCE_PART_WING;
    }

    int bHasModel = pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 0) != NULL;
    CSWCItem* pLeftItem = NULL;
    CSWCItem* pRightItem = NULL;
    int bLeftChanged = 0;
    int bRightChanged = 0;
    int bLeftLastInstant = 0;
    int bRightLastInstant = 0;
    if (nUpdateMask & 0x200)
    {
        unsigned char nItems = ReadBYTE(8);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        for (unsigned char nItem = 0; nItem < nItems; nItem++)
        {
            unsigned char nAction = ReadCHAR(8);
            oidObject = ReadOBJECTIDClient();
            unsigned long nSlot = ReadDWORD(32);
            if (nSlot == INVENTORY_SLOT_RIGHTHAND)
            {
                bRightLastInstant = ReadBOOL();
            }
            else if (nSlot == INVENTORY_SLOT_LEFTHAND)
            {
                bLeftLastInstant = ReadBOOL();
            }
            if (MessageReadOverflow(1))
            {
                return 0;
            }

            if (nAction == 'A')
            {
                unsigned long nBaseItem;
                unsigned char nModelVariation;
                unsigned char nBodyVariation;
                unsigned char nTextureVariation;
                HandleServerToPlayerItemAppearance(&nBaseItem, &nModelVariation,
                                                  &nBodyVariation, &nTextureVariation);
                CSWCItem* pItem =
                    g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidObject);
                if (pItem == NULL)
                {
                    if (sAppearance.m_nGender == 0)
                    {
                        pItem = CreateItem(nBaseItem, nModelVariation, 'M');
                    }
                    else
                    {
                        pItem = CreateItem(nBaseItem, nModelVariation, 'F');
                    }
                    if (pItem == NULL)
                    {
                        continue;
                    }
                    pItem->SetId(oidObject);
                    if (pObjectArray->AddExternalObject(oidObject, (CGameObject*)(CSWCObject*)pItem, 1) !=
                        CGameObjectArray::SUCCESS)
                    {
                        continue;
                    }
                }
                else if (g_pRules->GetBaseItemArray()->GetBaseItem(pItem->GetBaseItemType()) == NULL)
                {
                    continue;
                }
                HandleServerToPlayerActiveItemProperties(oidObject);
                CSWBaseItem* pBaseItem =
                    g_pRules->GetBaseItemArray()->GetBaseItem(pItem->GetBaseItemType());
                if (pBaseItem->m_bUnknown18 == BASEITEM_MODEL_TYPE_BODY_VARIATION)
                {
                    sAppearance.m_nBodyVariation = nBodyVariation;
                    sAppearance.m_nTextureVar = nTextureVariation;
                }
                if (nSlot == INVENTORY_SLOT_HEAD || nSlot == INVENTORY_SLOT_RIGHTHAND ||
                    nSlot == INVENTORY_SLOT_LEFTHAND)
                {
                    if (pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) == NULL)
                    {
                        pItem->LoadModel(pItem->m_cModelResRef,
                                         CSWCAnimBase::MODEL_SLOT_MAIN, 0);
                        if (pBaseItem->m_nPartEnvMap == 1)
                        {
                            CResRef cEnvMap("default");
                            pItem->SetEnvMap(CSWCAnimBase::MODEL_SLOT_MAIN, 1, cEnvMap);
                        }
                    }
                }
                CAurScene* pScene = pArea->m_pScene;
                pItem->AttachToScene(pScene);
                if (nSlot == INVENTORY_SLOT_ARMOUR)
                {
                    pItem->AttachToScene(NULL);
                    pCreature->GetArmourClass();
                    sAppearance.m_bUseBodyVariation = 1;
                    sAppearance.m_anArmorModelPart[0] = pItem->m_aUnknown04[0];
                    sAppearance.m_anArmorModelPart[1] = pItem->m_aUnknown04[1];
                    sAppearance.m_anArmorModelPart[2] = pItem->m_aUnknown04[2];
                    sAppearance.m_anArmorModelPart[3] = pItem->m_aUnknown04[3];
                    sAppearance.m_anArmorModelPart[4] = pItem->m_aUnknown04[4];
                    sAppearance.m_anArmorModelPart[5] = pItem->m_aUnknown04[5];
                    nPartMask |= CSWCCreature::APPEARANCE_PART_BODY;
                    pCreature->EquipToInventorySlot(INVENTORY_SLOT_ARMOUR, pItem);
                }
                else if (nSlot == INVENTORY_SLOT_HEAD)
                {
                    pItem->AttachToScene(NULL);
                    sAppearance.m_nUnmapped10 = oidObject;
                    nPartMask |= CSWCCreature::APPEARANCE_PART_HEAD;
                    pCreature->EquipToInventorySlot(INVENTORY_SLOT_HEAD, pItem);
                }
                else if (nSlot == INVENTORY_SLOT_LEFTHAND ||
                         nSlot == INVENTORY_SLOT_RIGHTHAND)
                {
                    if (nSlot == INVENTORY_SLOT_LEFTHAND)
                    {
                        pLeftItem = pItem;
                        bLeftChanged = 1;
                    }
                    else if (nSlot == INVENTORY_SLOT_RIGHTHAND)
                    {
                        pRightItem = pItem;
                        bRightChanged = 1;
                    }
                }
            }
            else if (nAction == 'D')
            {
                if (nSlot == INVENTORY_SLOT_ARMOUR)
                {
                    memset(sAppearance.m_anArmorModelPart, 0,
                           sizeof(sAppearance.m_anArmorModelPart));
                    pCreature->GetArmourClass();
                    sAppearance.m_bUseBodyVariation = 0;
                    sAppearance.m_nBodyVariation = 0;
                    nPartMask |= CSWCCreature::APPEARANCE_PART_BODY;
                }
                else if (nSlot == INVENTORY_SLOT_HEAD)
                {
                    sAppearance.m_nUnmapped10 = CGameObjectArray::INVALIDOBJID;
                    nPartMask |= CSWCCreature::APPEARANCE_PART_HEAD;
                }
                else if (nSlot == INVENTORY_SLOT_LEFTHAND)
                {
                    pLeftItem = NULL;
                    bLeftChanged = 1;
                }
                else if (nSlot == INVENTORY_SLOT_RIGHTHAND)
                {
                    pRightItem = NULL;
                    bRightChanged = 1;
                }
                if (nSlot != INVENTORY_SLOT_LEFTHAND && nSlot != INVENTORY_SLOT_RIGHTHAND)
                {
                    pCreature->EquipToInventorySlot(nSlot, NULL);
                }
            }
        }
    }

    if (nPartMask != 0)
    {
        if (enablemodelrequest)
        {
            if (g_pAppManager->GetClientExoApp()->GetAreaNotReadyToDisplay())
            {
                if (!pCreature->SetAppearance(&sAppearance, 0xEF,
                                              !bLeftChanged && !bRightChanged))
                {
                    EXOXBOXASSERT(0);
                    return 0;
                }
            }
            else
            {
                if (!pCreature->RequestAppearance(&sAppearance, nPartMask))
                {
                    return 0;
                }
                if (g_pAppManager->GetClientExoApp()->GetPauseState(
                        CClientExoAppInternal::PAUSE_TYPE_PAUSED_MASK) ||
                    g_pAppManager->GetClientExoApp()->GetPauseState(
                        CClientExoAppInternal::PAUSE_TYPE_COMBAT_MASK))
                {
                    pCreature->m_nCameraFollowDelay = 1;
                }
                else
                {
                    pCreature->m_nCameraFollowDelay = 500;
                }
            }
        }
        else
        {
            if (!pCreature->SetAppearance(&sAppearance, nPartMask,
                                          !bLeftChanged && !bRightChanged))
            {
                return 0;
            }
        }
    }

    if (enablemodelrequest)
    {
        int bUpdateAppearance = 1;
        if (pCreature->m_nCameraFollowDelay > 0)
        {
            nCurrentAppearance = pCreature->GetAppearance()->m_nAppearance;
            if (sAppearance.m_nAppearance == nCurrentAppearance)
            {
                bUpdateAppearance = 0;
            }
        }
        if (bUpdateAppearance)
        {
            unsigned short nPreviousAppearance = pCreature->GetAppearance()->m_nAppearance;
            pCreature->m_nAppearance = nPreviousAppearance;
            pCreature->GetAppearance()->m_nAppearance = sAppearance.m_nAppearance;
        }
    }

    if (bLeftChanged)
    {
        if (pLeftItem != NULL)
        {
            int bInLeftHand = 0;
            Gob* pLeftModel = pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_LEFTHAND, 1);
            if (pLeftModel == pLeftItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1))
            {
                bInLeftHand = 1;
            }
            Gob* pRightModel = pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_RIGHTHAND, 1);
            if (pRightModel != pLeftItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) &&
                !bInLeftHand)
            {
                pLeftItem->AttachToScene(NULL);
            }
            pCreature->SetLastInstant(INVENTORY_SLOT_LEFTHAND, bLeftLastInstant);
            if (!bHasModel)
            {
                pLeftItem->SetEquippedBy(pCreature);
                pCreature->SetEquippedItemID(INVENTORY_SLOT_LEFTHAND, pLeftItem->GetId());
                pCreature->UpdateWeaponClass();
                SwitchWeaponEvent(NULL, "Loading", pCreature);
            }
            else
            {
                pCreature->EquipToInventorySlot(INVENTORY_SLOT_LEFTHAND, pLeftItem);
            }
        }
        else
        {
            pCreature->SetLastInstant(INVENTORY_SLOT_LEFTHAND, bLeftLastInstant);
            pCreature->EquipToInventorySlot(INVENTORY_SLOT_LEFTHAND, NULL);
        }
    }
    if (bRightChanged)
    {
        if (pRightItem != NULL)
        {
            int bInLeftHand = 0;
            Gob* pLeftModel = pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_LEFTHAND, 1);
            if (pLeftModel == pRightItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1))
            {
                bInLeftHand = 1;
            }
            Gob* pRightModel = pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_RIGHTHAND, 1);
            if (pRightModel != pRightItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) &&
                !bInLeftHand)
            {
                pRightItem->AttachToScene(NULL);
            }
            pCreature->SetLastInstant(INVENTORY_SLOT_RIGHTHAND, bRightLastInstant);
            if (!bHasModel)
            {
                pRightItem->SetEquippedBy(pCreature);
                pCreature->SetEquippedItemID(INVENTORY_SLOT_RIGHTHAND, pRightItem->GetId());
                pCreature->UpdateWeaponClass();
                SwitchWeaponEvent(NULL, "Loading", pCreature);
            }
            else
            {
                pCreature->EquipToInventorySlot(INVENTORY_SLOT_RIGHTHAND, pRightItem);
            }
            pCreature->SetLastInstant(INVENTORY_SLOT_RIGHTHAND, 0);
        }
        else
        {
            pCreature->SetLastInstant(INVENTORY_SLOT_RIGHTHAND, bRightLastInstant);
            pCreature->EquipToInventorySlot(INVENTORY_SLOT_RIGHTHAND, NULL);
        }
    }
    if (bLeftChanged || bRightChanged)
    {
        pCreature->DetachVisualEffects();
        pCreature->ReAttachVisualEffects();
    }
    pCreature->UpdateShadowBlob();

    int bAttached;
    if (bReAttach)
    {
        CAurScene* pScene = pArea->m_pScene;
        pCreature->AttachToScene(pScene);
        bAttached = 1;
    }
    else
    {
        bAttached = 0;
    }

    if (g_pAppManager->GetClientExoApp()->GetPlayerCreature() == pCreature)
    {
        if (enablemodelrequest)
        {
            if (pCreature->m_nCameraFollowDelay > 0)
            {
                pCreature->m_bCameraFollowPending = 1;
            }
        }
        else if (bAttached)
        {
            if (pModule->GetCamera() == NULL)
            {
                return 0;
            }
            pModule->SetCameraFollow(pCreature->GetObjectId(), 0);
        }
    }
    else if (g_pAppManager->GetClientExoApp()->GetPlayerCreatureId() == pCreature->GetObjectId())
    {
        CClientExoApp* pClientExoApp = g_pAppManager->GetClientExoApp();
        CSWCPlayer* pPlayer =
            pClientExoApp->GetPlayerByGameObjectID(pClientExoApp->GetPlayerCreatureId());
        if (pPlayer != NULL)
        {
            pPlayer->m_oidControlledObject = pCreature->GetObjectId();
        }
        g_pAppManager->GetClientExoApp()->SetPlayerCreature(pCreature->GetObjectId());
        CSWCModule* pPlayerModule = g_pAppManager->GetClientExoApp()->GetModule();
        if (pPlayerModule == NULL)
        {
            return 0;
        }
        if (pPlayerModule->GetCamera() == NULL)
        {
            return 0;
        }
        g_pAppManager->GetClientExoApp()->GetPlayerByGameObjectID(oidObject);
        g_pAppManager->GetClientExoApp()->AttachPlayerGameObjectToArea();
        if (enablemodelrequest)
        {
            if (pCreature->m_nCameraFollowDelay > 0)
            {
                pCreature->m_bCameraFollowPending = 1;
            }
        }
        else
        {
            pPlayerModule->SetCameraFollow(pCreature->GetObjectId(), 0);
        }
    }

    if (!bHasModel)
    {
        unsigned short nAnimation = pCreature->m_nServerUpdateValue;
        if (pCreature->AnimationLooping(nAnimation))
        {
            pCreature->SetLoopingAnimation(nAnimation);
        }
        pCreature->SetAnimation(nAnimation, 0, 0);
    }
    return 1;
}
// PC 0x00656BA0  Xbox 0x0007C430
int CSWCMessage::HandleServerToPlayerSound(unsigned char nMinor)
{
    switch (nMinor)
    {
    case 2:
        return HandleServerToPlayerSoundObject_Play();
    case 3:
        return HandleServerToPlayerSoundObject_Stop();
    case 5:
        return HandleServerToPlayerSoundObject_ChangePosition();
    }
    return 0;
}
// PC 0x00656BD0  Xbox 0x0007C460
int CSWCMessage::HandleServerToPlayerUpdate_Add()
{
    unsigned char nObjectType = ReadBYTE(8);

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        return HandleServerToPlayerCreatureUpdate_Add();
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {
        return HandleServerToPlayerItemUpdate_Add(0, NULL);
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
    {
        return HandleServerToPlayerTriggerUpdate_Add();
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        return HandleServerToPlayerDoorUpdate_Add();
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT)
    {
        return HandleServerToPlayerAreaOfEffectUpdate_Add();
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        return HandleServerToPlayerPlaceableUpdate_Add();
    }
    return 0;
}
// PC 0x00656C60  Xbox 0x0007C4F0
int CSWCMessage::HandleServerToPlayerUpdate_Appearance()
{
    unsigned char nObjectType = ReadBYTE(8);

    if (MessageReadOverflow(1) ||
        nObjectType != CGameObject::OBJECT_TYPE_CREATURE)
    {
        return 0;
    }
    return HandleServerToPlayerCreatureUpdate_Appearance();
}
// PC 0x00656C90  Xbox 0x0007C520
int CSWCMessage::HandleServerToPlayerUpdate_Delete()
{
    unsigned char nObjectType = ReadBYTE(8);

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        return HandleServerToPlayerCreatureUpdate_Delete();
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {
        return HandleServerToPlayerItemUpdate_Delete();
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
    {
        return HandleServerToPlayerTriggerUpdate_Delete();
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        return HandleServerToPlayerDoorUpdate_Delete();
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT)
    {
        return HandleServerToPlayerAreaOfEffectUpdate_Delete();
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        return HandleServerToPlayerPlaceableUpdate_Delete();
    }
    return 0;
}
// PC 0x00656D20  Xbox 0x0007C5B0
int CSWCMessage::HandleServerToPlayerCCMessage(unsigned char nMinor)
{
    CExoString sMessage;
    STR_RES cString;
    CExoString sObjectName;
    switch (nMinor)
    {
    case CC_MESSAGE_OBJECT_TEXT:
    {
        unsigned long oidObject = ReadOBJECTIDClient();
        unsigned long nString = ReadDWORD(32);
        if (MessageReadOverflow(1))
            return 0;
        CExoString sText = g_pAppManager->GetClientExoApp()->GetGUIString(nString);
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObjectName))
            g_pTlkTable->SetCustomToken(0, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sObjectName);
        }
        g_pTlkTable->SetCustomToken(1, sText);
        sMessage = g_pSWTlkTable->Fetch(1401, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        return 1;
    }
    case CC_MESSAGE_DAMAGE_WITHOUT_ATTACKER:
    {
        int bFirst = 1;
        CExoString sDamage;
        unsigned long oidObject = ReadOBJECTIDClient();
        int anDamage[15];
        int nTotal = 0;
        int i;
        for (i = 0; i < 15; ++i)
        {
            int bPresent = ReadBOOL();
            if (MessageReadOverflow(1))
                return 0;
            anDamage[i] = -1;
            if (bPresent)
            {
                anDamage[i] = ReadWORD(14);
                if (MessageReadOverflow(1))
                    return 0;
                nTotal += anDamage[i];
            }
            if (i == 14)
                nTotal = anDamage[14];
        }
        if (MessageReadOverflow(1))
            return 0;
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObjectName))
            g_pTlkTable->SetCustomToken(0, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sObjectName);
        }
        g_pTlkTable->SetCustomToken(1, CExoString(nTotal));
        sMessage = g_pSWTlkTable->Fetch(1402, NULL, 0, 0) + " (";
        if (anDamage[0] > -1 || anDamage[1] > -1 || anDamage[2] > -1)
        {
            int nPhysical = 0;
            if (anDamage[0] > -1)
                nPhysical = anDamage[0];
            if (anDamage[1] > -1)
                nPhysical += anDamage[1];
            if (anDamage[2] > -1)
                nPhysical += anDamage[2];
            g_pTlkTable->SetCustomToken(0, CExoString(nPhysical));
            sDamage = g_pSWTlkTable->Fetch(1423, NULL, 0, 0);
            if (!bFirst)
                sMessage = sMessage + " " + sDamage;
            else
            {
                sMessage = sMessage + sDamage;
                bFirst = 0;
            }
        }
        for (i = 0; i < 15; ++i)
        {
            if (i == 3)
            {
                if (anDamage[3] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[3]));
                    sDamage = g_pSWTlkTable->Fetch(1422, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 4)
            {
                if (anDamage[4] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[4]));
                    sDamage = g_pSWTlkTable->Fetch(1440, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 5)
            {
                if (anDamage[5] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[5]));
                    sDamage = g_pSWTlkTable->Fetch(1441, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 6)
            {
                if (anDamage[6] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[6]));
                    sDamage = g_pSWTlkTable->Fetch(1442, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 7)
            {
                if (anDamage[7] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[7]));
                    sDamage = g_pSWTlkTable->Fetch(1443, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 8)
            {
                if (anDamage[8] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[8]));
                    sDamage = g_pSWTlkTable->Fetch(1444, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 9)
            {
                if (anDamage[9] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[9]));
                    sDamage = g_pSWTlkTable->Fetch(1445, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 10)
            {
                if (anDamage[10] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[10]));
                    sDamage = g_pSWTlkTable->Fetch(1446, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 11)
            {
                if (anDamage[11] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[11]));
                    sDamage = g_pSWTlkTable->Fetch(1447, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 12)
            {
                if (anDamage[12] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[12]));
                    sDamage = g_pSWTlkTable->Fetch(1448, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 13)
            {
                if (anDamage[13] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[13]));
                    sDamage = g_pSWTlkTable->Fetch(41902, NULL, 0, 0);
                }
                else
                    continue;
            }
            else
                continue;
            if (!bFirst)
                sMessage = sMessage + " " + sDamage;
            else
            {
                bFirst = 0;
                sMessage = sMessage + sDamage;
            }
        }
        sMessage = sMessage + ")";
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        return 1;
    }
    case CC_MESSAGE_DAMAGE:
    {
        int bFirst = 1;
        CExoString sAttacker;
        CExoString sUnknownAttacker;
        CExoString sDamage;
        unsigned long oidAttacker = ReadOBJECTIDClient();
        unsigned long oidObject = ReadOBJECTIDClient();
        int anDamage[15];
        int nTotal = 0;
        int i;
        for (i = 0; i < 15; ++i)
        {
            int bPresent = ReadBOOL();
            if (MessageReadOverflow(1))
                return 0;
            anDamage[i] = -1;
            if (bPresent)
                anDamage[i] = ReadWORD(14);
            if (i == 14)
                nTotal = anDamage[14];
        }
        if (MessageReadOverflow(1))
            return 0;
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObjectName))
        {
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidAttacker, sAttacker))
                g_pTlkTable->SetCustomToken(0, sAttacker);
            else
            {
                sUnknownAttacker = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sUnknownAttacker);
            }
            g_pTlkTable->SetCustomToken(1, sObjectName);
        }
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidAttacker, sAttacker))
                g_pTlkTable->SetCustomToken(0, sAttacker);
            else
            {
                sUnknownAttacker = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sUnknownAttacker);
            }
            g_pTlkTable->SetCustomToken(1, cString.m_sString);
        }
        g_pTlkTable->SetCustomToken(2, CExoString(nTotal));
        sMessage = g_pSWTlkTable->Fetch(1403, NULL, 0, 0) + " (";
        if (anDamage[0] > -1 || anDamage[1] > -1 || anDamage[2] > -1)
        {
            int nPhysical = 0;
            if (anDamage[0] > -1)
                nPhysical = anDamage[0];
            if (anDamage[1] > -1)
                nPhysical += anDamage[1];
            if (anDamage[2] > -1)
                nPhysical += anDamage[2];
            g_pTlkTable->SetCustomToken(0, CExoString(nPhysical));
            sDamage = g_pSWTlkTable->Fetch(1423, NULL, 0, 0);
            if (!bFirst)
                sMessage = sMessage + " " + sDamage;
            else
            {
                sMessage = sMessage + sDamage;
                bFirst = 0;
            }
        }
        for (i = 0; i < 15; ++i)
        {
            if (i == 3)
            {
                if (anDamage[3] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[3]));
                    sDamage = g_pSWTlkTable->Fetch(1422, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 4)
            {
                if (anDamage[4] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[4]));
                    sDamage = g_pSWTlkTable->Fetch(1440, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 5)
            {
                if (anDamage[5] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[5]));
                    sDamage = g_pSWTlkTable->Fetch(1441, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 6)
            {
                if (anDamage[6] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[6]));
                    sDamage = g_pSWTlkTable->Fetch(1442, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 7)
            {
                if (anDamage[7] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[7]));
                    sDamage = g_pSWTlkTable->Fetch(1443, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 8)
            {
                if (anDamage[8] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[8]));
                    sDamage = g_pSWTlkTable->Fetch(1444, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 9)
            {
                if (anDamage[9] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[9]));
                    sDamage = g_pSWTlkTable->Fetch(1445, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 10)
            {
                if (anDamage[10] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[10]));
                    sDamage = g_pSWTlkTable->Fetch(1446, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 11)
            {
                if (anDamage[11] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[11]));
                    sDamage = g_pSWTlkTable->Fetch(1447, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 12)
            {
                if (anDamage[12] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[12]));
                    sDamage = g_pSWTlkTable->Fetch(1448, NULL, 0, 0);
                }
                else
                    continue;
            }
            else if (i == 13)
            {
                if (anDamage[13] > -1)
                {
                    g_pTlkTable->SetCustomToken(0, CExoString(anDamage[13]));
                    sDamage = g_pSWTlkTable->Fetch(41902, NULL, 0, 0);
                }
                else
                    continue;
            }
            else
                continue;
            if (!bFirst)
                sMessage = sMessage + " " + sDamage;
            else
            {
                bFirst = 0;
                sMessage = sMessage + sDamage;
            }
        }
        sMessage = sMessage + ")";
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        int bDetails = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        if (bDetails)
        {
            CExoString sDetails;
            sDetails = ReadCExoString(32);
            if (MessageReadOverflow(1))
                return 0;
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sDetails, 0x80, 0);
        }
        return 1;
    }

    case CC_MESSAGE_MELEE_ATTACK:
    {
        STR_RES cAttackString;
        CExoString sSign;
        CExoString sAttacker;
        CExoString sResult;
        CExoString sUnknownAttacker;
        CExoString sOpposedRoll;
        unsigned long oidAttacker = ReadOBJECTIDClient();
        unsigned long oidTarget = ReadOBJECTIDClient();
        if (MessageReadOverflow(1))
            return 0;
        int nModifier = ReadCHAR(8);
        unsigned char nRoll = ReadBYTE(5);
        unsigned char nResult = ReadBYTE(4);
        unsigned char nAttackType = ReadBYTE(3);
        int bFlag0 = ReadBOOL();
        int bFlag1 = ReadBOOL();
        int bFlag2 = ReadBOOL();
        int bFlag3 = ReadBOOL();
        int bOpposedRoll = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        unsigned long nOpposedRoll;
        if (bOpposedRoll == 1)
            nOpposedRoll = (unsigned char)ReadBYTE(5);
        unsigned char nSpecialResult = (nResult == 17 || nResult == 16) ? ReadBYTE(7) : 0;
        if (MessageReadOverflow(1))
            return 0;
        int nTotal = nModifier + nRoll;
        if (nModifier >= 0)
            sSign = "+";
        else
        {
            sSign = "-";
            nModifier = -nModifier;
        }
        if (bOpposedRoll == 1)
        {
            unsigned long nOpposedTotal;
            if (sSign == "-")
                nOpposedTotal = nOpposedRoll - nModifier;
            else
                nOpposedTotal = nOpposedRoll + nModifier;
            g_pTlkTable->SetCustomToken(0, CExoString(nOpposedRoll));
            sOpposedRoll = g_pSWTlkTable->Fetch(1417, NULL, 0, 0);
            sOpposedRoll.Format("%s %s %d = %d", sOpposedRoll.CStr(), sSign.CStr(), nModifier, nOpposedTotal);
        }
        if (nResult == 1)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1371);
        else if (nResult == 2)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1390);
        else if (nResult == 8)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(0);
        else if (nResult == 4)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1373);
        else if (nResult == 3)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1416);
        else if (nResult == 17)
        {
            g_pTlkTable->SetCustomToken(0, nSpecialResult);
            sResult = g_pSWTlkTable->Fetch(1418, NULL, 0, 0);
        }
        else if (nResult == 16)
        {
            g_pTlkTable->SetCustomToken(0, nSpecialResult);
            sResult = g_pSWTlkTable->Fetch(1419, NULL, 0, 0);
        }
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidTarget, sObjectName))
        {
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidAttacker, sAttacker))
                g_pTlkTable->SetCustomToken(0, sAttacker);
            else
            {
                sUnknownAttacker = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sUnknownAttacker);
            }
        }
        else
        {
            CAppManager* pAppManager = g_pAppManager;
            sObjectName = pAppManager->m_pClientExoApp->GetGUIString(0);
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidAttacker, sAttacker))
                g_pTlkTable->SetCustomToken(0, sAttacker);
            else
            {
                sUnknownAttacker = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sUnknownAttacker);
            }
        }
        g_pTlkTable->SetCustomToken(1, sObjectName);
        g_pTlkTable->SetCustomToken(2, sResult);
        g_pTlkTable->SetCustomToken(3, CExoString(nRoll));
        g_pTlkTable->SetCustomToken(4, sSign);
        g_pTlkTable->SetCustomToken(5, CExoString(nModifier));
        g_pTlkTable->SetCustomToken(6, CExoString(nTotal));
        if (bOpposedRoll == 1)
            g_pTlkTable->SetCustomToken(7, sOpposedRoll);
        else
            g_pTlkTable->SetCustomToken(7, "");
        sMessage = g_pSWTlkTable->Fetch(1404, NULL, 0, 0);
        if (bFlag3 == 1)
            sMessage = g_pAppManager->GetClientExoApp()->GetGUIString(0) + " : " + sMessage;
        if (bFlag1 == 1)
            sMessage = g_pAppManager->GetClientExoApp()->GetGUIString(1391) + " : " + sMessage;
        if (nAttackType != 0)
        {
            CExoString sAttackType;
            if (nAttackType == 2)
                sAttackType = g_pAppManager->GetClientExoApp()->GetGUIString(178);
            else if (nAttackType == 3)
                sAttackType = g_pAppManager->GetClientExoApp()->GetGUIString(167);
            else if (nAttackType == 5)
                sAttackType = g_pAppManager->GetClientExoApp()->GetGUIString(160);
            else if (nAttackType == 6)
                sAttackType = g_pAppManager->GetClientExoApp()->GetGUIString(180);
            sMessage = sAttackType + CExoString(" : ") + sMessage;
        }
        if (bFlag2 == 1)
            sMessage = g_pAppManager->GetClientExoApp()->GetGUIString(0) + " : " + sMessage;
        if (bFlag0 == 1)
            sMessage = g_pAppManager->GetClientExoApp()->GetGUIString(0) + " : " + sMessage;
        int bExtraString = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        if (bExtraString != 0)
        {
            CExoString sExtra;
            sExtra = ReadCExoString(32);
            if (MessageReadOverflow(1))
                return 0;
        }
        return 1;
    }

    case CC_MESSAGE_RANGED_ATTACK:
    {
        CExoString sSign;
        CExoString sAttacker;
        CExoString sFeat;
        CExoString sResult;
        CExoString sUnknownAttacker;
        CExoString sOpposedRoll;
        unsigned long oidAttacker = ReadOBJECTIDClient();
        unsigned long oidTarget = ReadOBJECTIDClient();
        int nModifier = ReadCHAR(8);
        unsigned char nRoll = ReadBYTE(5);
        unsigned char nResult = ReadBYTE(4);
        unsigned short nFeat = ReadWORD(16);
        unsigned char nAttackType = ReadBYTE(3);
        int bFlag0 = ReadBOOL();
        int bFlag1 = ReadBOOL();
        int bOpposedRoll = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        unsigned long nOpposedRoll;
        if (bOpposedRoll == 1)
            nOpposedRoll = (unsigned char)ReadBYTE(5);
        unsigned char nSpecialResult = (nResult == 17 || nResult == 16) ? ReadBYTE(7) : 0;
        if (MessageReadOverflow(1))
            return 0;
        int nTotal = nModifier + nRoll;
        if (nModifier >= 0)
            sSign = "+";
        else
        {
            sSign = "-";
            nModifier = -nModifier;
        }
        if (bOpposedRoll == 1)
        {
            unsigned long nOpposedTotal = nOpposedRoll + nModifier;
            g_pTlkTable->SetCustomToken(0, CExoString(nOpposedRoll));
            sOpposedRoll = g_pSWTlkTable->Fetch(1417, NULL, 0, 0);
            sOpposedRoll.Format("%s %s %d = %d", sOpposedRoll.CStr(), sSign.CStr(), nModifier, nOpposedTotal);
        }
        if (nFeat != 0)
        {
            CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
            if (pFeat != NULL)
            {
                unsigned long nStrRef = pFeat->m_nNameStrRef;
                sFeat = g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef);
            }
        }
        if (nResult == 1)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1371);
        else if (nResult == 2)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1390);
        else if (nResult == 8)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(0);
        else if (nResult == 4)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1373);
        else if (nResult == 5)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1413);
        else if (nResult == 6)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1414);
        else if (nResult == 17)
        {
            g_pTlkTable->SetCustomToken(0, nSpecialResult);
            sResult = g_pSWTlkTable->Fetch(1418, NULL, 0, 0);
        }
        else if (nResult == 16)
        {
            g_pTlkTable->SetCustomToken(0, nSpecialResult);
            sResult = g_pSWTlkTable->Fetch(1419, NULL, 0, 0);
        }
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidTarget, sObjectName))
        {
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidAttacker, sAttacker))
                g_pTlkTable->SetCustomToken(0, sAttacker);
            else
            {
                sUnknownAttacker = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sUnknownAttacker);
            }
        }
        else
        {
            CAppManager* pAppManager = g_pAppManager;
            sObjectName = pAppManager->m_pClientExoApp->GetGUIString(0);
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidAttacker, sAttacker))
                g_pTlkTable->SetCustomToken(0, sAttacker);
            else
            {
                sUnknownAttacker = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sUnknownAttacker);
            }
        }
        g_pTlkTable->SetCustomToken(2, sObjectName);
        g_pTlkTable->SetCustomToken(1, sFeat);
        g_pTlkTable->SetCustomToken(3, sResult);
        g_pTlkTable->SetCustomToken(4, CExoString(nRoll));
        g_pTlkTable->SetCustomToken(5, sSign);
        g_pTlkTable->SetCustomToken(6, CExoString(nModifier));
        g_pTlkTable->SetCustomToken(7, CExoString(nTotal));
        if (bOpposedRoll == 1)
            g_pTlkTable->SetCustomToken(8, sOpposedRoll);
        else
            g_pTlkTable->SetCustomToken(8, "");
        sMessage = g_pSWTlkTable->Fetch(1405, NULL, 0, 0);
        if (bFlag1 != 0)
            sMessage = g_pAppManager->GetClientExoApp()->GetGUIString(1391) + " : " + sMessage;
        if (nAttackType != 0 && nAttackType != 1)
        {
            CExoString sAttackType;
            if (nAttackType == 2)
                sAttackType = g_pAppManager->GetClientExoApp()->GetGUIString(178);
            else if (nAttackType == 3)
                sAttackType = g_pAppManager->GetClientExoApp()->GetGUIString(167);
            else if (nAttackType == 5)
                sAttackType = g_pAppManager->GetClientExoApp()->GetGUIString(160);
            else if (nAttackType == 6)
                sAttackType = g_pAppManager->GetClientExoApp()->GetGUIString(180);
            sMessage = sAttackType + CExoString(" : ") + sMessage;
        }
        if (bFlag0 == 1)
            sMessage = g_pAppManager->GetClientExoApp()->GetGUIString(0) + " : " + sMessage;
        int bExtraString = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        if (bExtraString != 0)
        {
            CExoString sExtra;
            sExtra = ReadCExoString(32);
            if (MessageReadOverflow(1))
                return 0;
        }
        return 1;
    }
    case CC_MESSAGE_FORCE_RESISTANCE:
    {
        CExoString sResult;
        int bShowResult = TRUE;
        unsigned long oidObject = ReadOBJECTIDClient();
        int nResult = ReadBYTE(8);
        if (MessageReadOverflow(1))
            return 0;
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObjectName))
            g_pTlkTable->SetCustomToken(0, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sObjectName);
        }
        if (nResult == 0)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1393);
        else if (nResult == 1)
        {
            sMessage = g_pSWTlkTable->Fetch(38598, NULL, 0, 0);
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
            return 1;
        }
        else
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1400);
        if (bShowResult)
            g_pTlkTable->SetCustomToken(1, sResult);
        else
            g_pTlkTable->SetCustomToken(1, "");
        sMessage = g_pSWTlkTable->Fetch(38597, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        return 1;
    }
    case CC_MESSAGE_SPELL_RESISTANCE_CHECK:
    {
        unsigned long oidCaster = ReadOBJECTIDClient();
        unsigned long oidObject = ReadOBJECTIDClient();
        int nRoll = ReadBYTE(8);
        int nResistance = ReadBYTE(8);
        int nResult = ReadBYTE(8);
        if (MessageReadOverflow(1))
            return 0;
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObjectName))
            g_pTlkTable->SetCustomToken(0, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sObjectName);
        }
        g_pTlkTable->SetCustomToken(1, CExoString(nRoll));
        g_pTlkTable->SetCustomToken(2, CExoString(nResistance));
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidCaster, sObjectName))
            g_pTlkTable->SetCustomToken(3, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(3, sObjectName);
        }
        g_pTlkTable->SetCustomToken(4, CExoString(nResult));
        sMessage = g_pSWTlkTable->Fetch(42019, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        return 1;
    }
    case CC_MESSAGE_SAVING_THROW:
    {
        unsigned char nSave = ReadBYTE(8);
        ReadBYTE(8);
        unsigned char nRoll = ReadBYTE(5);
        char nModifier = ReadCHAR(8);
        char nBase = ReadCHAR(8);
        unsigned char nDifficulty = ReadBYTE(7);
        unsigned long oidObject = ReadOBJECTIDClient();
        if (MessageReadOverflow(1))
            return 0;
        CExoString sModifiers;
        if (nBase)
        {
            g_pTlkTable->SetCustomToken(0, nBase);
            sModifiers = sModifiers + g_pSWTlkTable->Fetch(42387, NULL, 0, 0);
        }
        if (nModifier)
        {
            g_pTlkTable->SetCustomToken(0, nModifier);
            sModifiers = sModifiers + g_pSWTlkTable->Fetch(42388, NULL, 0, 0);
        }

        CExoString sUnusedString1;
        CExoString sSave;
        CExoString sUnusedString2;
        CExoString sResult;
        CExoString sCreature;
        CExoString sUnusedString3;
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sCreature))
            g_pTlkTable->SetCustomToken(0, sCreature);
        else
        {
            sCreature = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sCreature);
        }
        switch (nSave)
        {
        case 1: sSave = g_pAppManager->GetClientExoApp()->GetGUIString(1375); break;
        case 2: sSave = g_pAppManager->GetClientExoApp()->GetGUIString(1374); break;
        case 3: sSave = g_pAppManager->GetClientExoApp()->GetGUIString(1376); break;
        }
        g_pTlkTable->SetCustomToken(1, sSave);
        if (nRoll + nModifier + nBase >= nDifficulty)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1392);
        else
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1393);
        g_pTlkTable->SetCustomToken(2, sResult);
        g_pTlkTable->SetCustomToken(3, CExoString(nRoll + nBase + nModifier));
        g_pTlkTable->SetCustomToken(4, CExoString((int)nRoll));
        g_pTlkTable->SetCustomToken(5, sModifiers);
        g_pTlkTable->SetCustomToken(6, nDifficulty);
        sMessage = g_pSWTlkTable->Fetch(1406, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        return 1;
    }
    case CC_MESSAGE_DEATH:
    {
        CExoString sObject;
        CExoString sKiller;
        STR_RES cKiller;
        STR_RES cObject;
        unsigned long oidKiller = ReadOBJECTIDClient();
        unsigned long oidObject = ReadOBJECTIDClient();
        int nExperience = ReadINT(32);
        if (MessageReadOverflow(1))
            return 0;
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObject))
            g_pTlkTable->SetCustomToken(0, sObject);
        else
        {
            sObject = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sObject);
        }
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidKiller, sKiller))
            g_pTlkTable->SetCustomToken(1, sKiller);
        else
        {
            sKiller = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(1, sKiller);
        }
        g_pTlkTable->SetCustomToken(2, CExoString(nExperience));
        sMessage = g_pSWTlkTable->Fetch(1407, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        return 1;
    }
    case CC_MESSAGE_SPELL_DATA:
    {

        CExoString sUnusedString;
        CExoString sSpell;
        unsigned long oidCaster = ReadOBJECTIDClient();
        unsigned long oidTarget = ReadOBJECTIDClient();
        int nSpell = ReadINT(32);
        int bCustomName = ReadBOOL();
        CExoString sCustomName;
        if (bCustomName)
            sCustomName = ReadCExoLocStringClient(CResRef(), NULL);
        int nBaseCost = ReadINT(32);
        int nCost = ReadINT(32);
        ReadINT(32);
        if (MessageReadOverflow(1))
            return 1;
        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
        if (pSpell != NULL)
        {
            if (pSpell->m_nAltMessage)
            {
                if (g_pAppManager->GetClientExoApp()->GetObjectName(oidCaster, sObjectName))
                    g_pTlkTable->SetCustomToken(0, sObjectName);
                else
                {
                    sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                    g_pTlkTable->SetCustomToken(0, sObjectName);
                }
                int nAltMessage = pSpell->m_nAltMessage;
                sMessage = g_pSWTlkTable->Fetch(nAltMessage, NULL, 0, 0);
            }
            else
            {
                if (!bCustomName)
                    sSpell = g_pAppManager->GetClientExoApp()->GetGUIString(pSpell->GetNameStrRef());
                else
                    sSpell = sCustomName;
                if (g_pAppManager->GetClientExoApp()->GetObjectName(oidCaster, sObjectName))
                    g_pTlkTable->SetCustomToken(0, sObjectName);
                else
                {
                    sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                    g_pTlkTable->SetCustomToken(0, sObjectName);
                }
                g_pTlkTable->SetCustomToken(1, sSpell);
                sMessage = g_pSWTlkTable->Fetch(32292, NULL, 0, 0);
            }
            int nModifier = nCost - nBaseCost;
            if (nCost)
            {
                CExoString sCost;
                if (nModifier >= 0)
                    sCost.Format(" %d (%d + %d ", nCost, nBaseCost, nModifier);
                else
                    sCost.Format(" %d (%d - %d ", nCost, nBaseCost, abs(nModifier));
                sMessage = sMessage + " " +
                    g_pTlkTable->GetSimpleString(42006) + sCost +
                    g_pTlkTable->GetSimpleString(42008) + ")";
            }
        }
        g_pAppManager->GetClientExoApp()->GetObjectName(oidTarget, sObjectName);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 1);
        return 1;
    }
    case CC_MESSAGE_SKILL:
    {
        CExoString sSign;

        CExoString sUnusedString;
        CExoString sSkill;
        CExoString sReason;
        CExoString sResult;
        unsigned long oidObject = ReadOBJECTIDClient();
        unsigned long oidTarget = ReadOBJECTIDClient();
        unsigned long nSkillString = ReadDWORD(32);
        int nRoll = ReadBYTE(5);
        int nModifier = ReadCHAR(8);
        int nDifficulty = ReadBYTE(7);
        ReadBOOL();
        int nResult = ReadBYTE(8);
        unsigned long nReason = ReadDWORD(32);
        if (MessageReadOverflow(1))
            return 0;
        sSkill = g_pAppManager->GetClientExoApp()->GetGUIString(nSkillString);
        sReason = g_pAppManager->GetClientExoApp()->GetGUIString(nReason);
        if (nResult != 5)
        {
            switch (nResult)
            {
            case 1: case 4:
                sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1392);
                break;
            case 0: case 2: case 3:
                sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1393);
                break;
            }
            int nTotal = nModifier + nRoll;
            if (nModifier < 0)
            {
                sSign = "-";
                nModifier = -nModifier;
            }
            else
                sSign = "+";
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pTlkTable->SetCustomToken(1, sResult);
            g_pTlkTable->SetCustomToken(2, sSkill);
            g_pTlkTable->SetCustomToken(3, CExoString(nTotal));
            g_pTlkTable->SetCustomToken(4, CExoString(nRoll));
            g_pTlkTable->SetCustomToken(5, sSign);
            g_pTlkTable->SetCustomToken(6, sReason);
            g_pTlkTable->SetCustomToken(7, CExoString(nModifier));
            g_pTlkTable->SetCustomToken(8, CExoString(nDifficulty));
            sMessage = g_pSWTlkTable->Fetch(1408, NULL, 0, 0);
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        }
        CGameObject* pGameObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidTarget);
        CSWCObject* pTarget = pGameObject != NULL ? pGameObject->AsSWCObject() : NULL;
        if (pTarget != NULL)
        {
            switch (nResult)
            {
            case 0:
                switch (nSkillString)
                {
                case 329: case 1529: case 1531:
                    sMessage.Format("%s *%s*", sSkill.CStr(), sResult.CStr());
                    if (sMessage.GetLength() > 20)
                        sMessage.Format("*%s*", sResult.CStr());
                    pTarget->SetFeedbackInfo(sMessage);
                    break;
                }
                break;
            case 1:
                switch (nSkillString)
                {
                case 1529: case 1531:
                    sMessage.Format("%s *%s*", sSkill.CStr(), sResult.CStr());
                    if (sMessage.GetLength() > 20)
                        sMessage.Format("*%s*", sResult.CStr());
                    pTarget->SetFeedbackInfo(sMessage);
                    break;
                }
                break;
            case 3:
                switch (nSkillString)
                {
                case 329: case 1529: case 1531:
                {
                    CExoString sFailure = g_pAppManager->GetClientExoApp()->GetGUIString(1519);
                    sMessage.Format("%s *%s*", sSkill.CStr(), sFailure.CStr());
                    if (sMessage.GetLength() > 20)
                        sMessage.Format("*%s*", sFailure.CStr());
                    pTarget->SetFeedbackInfo(sMessage);
                    break;
                }
                }
                break;
            case 5:
                if (nSkillString == 329)
                {
                    CExoString sFailure = g_pAppManager->GetClientExoApp()->GetGUIString(1370);
                    sMessage.Format("%s *%s*", sSkill.CStr(), sFailure.CStr());
                    if (sMessage.GetLength() > 20)
                        sMessage.Format("*%s*", sFailure.CStr());
                    pTarget->SetFeedbackInfo(sMessage);
                }
                break;
            }
        }
        return 1;
    }
    case CC_MESSAGE_TOUCH_ATTACK:
    {
        CExoString sSign;
        CExoString sAttacker;
        CExoString sResult;
        CExoString sTouch;
        CExoString sUnknownAttacker;
        unsigned long oidAttacker = ReadOBJECTIDClient();
        unsigned long oidTarget = ReadOBJECTIDClient();
        int nModifier = ReadCHAR(8);
        int nRoll = ReadBYTE(5);
        int nResult = ReadBYTE(4);
        int bRanged = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        int bConcealment = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        int nConcealment;
        int bTouchRanged = 0;
        if (bConcealment == 1)
            nConcealment = ReadBYTE(7);
        else
            nConcealment = 0;
        if (bRanged == 1)
            bTouchRanged = 1;
        if (bTouchRanged)
            sTouch = g_pAppManager->GetClientExoApp()->GetGUIString(1410);
        else
            sTouch = g_pAppManager->GetClientExoApp()->GetGUIString(1411);
        if (nResult == 1)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1371);
        else if (nResult == 2)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1390);
        else if (nResult == 4)
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(1373);
        else if (nResult == 17)
        {
            g_pTlkTable->SetCustomToken(0, nConcealment);
            sResult = g_pSWTlkTable->Fetch(1418, NULL, 0, 0);
        }
        else if (nResult == 16)
        {
            g_pTlkTable->SetCustomToken(0, nConcealment);
            sResult = g_pSWTlkTable->Fetch(1419, NULL, 0, 0);
        }
        int nTotal = nRoll + nModifier;
        if (nModifier < 0)
        {
            sSign = "-";
            nModifier = -nModifier;
        }
        else
            sSign = "+";
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidTarget, sObjectName))
        {
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidAttacker, sAttacker))
                g_pTlkTable->SetCustomToken(0, sAttacker);
            else
            {
                sUnknownAttacker = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sUnknownAttacker);
            }
        }
        else
        {
            CAppManager* pAppManager = g_pAppManager;
            sObjectName = pAppManager->m_pClientExoApp->GetGUIString(0);
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidAttacker, sAttacker))
                g_pTlkTable->SetCustomToken(0, sAttacker);
            else
            {
                sUnknownAttacker = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sUnknownAttacker);
            }
        }
        g_pTlkTable->SetCustomToken(2, sObjectName);
        g_pTlkTable->SetCustomToken(1, sTouch);
        g_pTlkTable->SetCustomToken(3, sResult);
        g_pTlkTable->SetCustomToken(4, CExoString(nRoll));
        g_pTlkTable->SetCustomToken(5, sSign);
        g_pTlkTable->SetCustomToken(6, CExoString(nModifier));
        g_pTlkTable->SetCustomToken(7, CExoString(nTotal));
        g_pTlkTable->SetCustomToken(8, "");
        sMessage = g_pSWTlkTable->Fetch(1405, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        return 1;
    }
    case CC_MESSAGE_INITIATIVE:
    {
        CExoString sSign;
        unsigned long oidObject = ReadOBJECTIDClient();
        int nRoll = ReadBYTE(5);
        int nModifier = ReadCHAR(8);
        if (MessageReadOverflow(1))
            return 0;
        int nTotal = nModifier + nRoll;
        if (nModifier < 0)
        {
            sSign = "-";
            nModifier = -nModifier;
        }
        else
            sSign = "+";
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObjectName))
            g_pTlkTable->SetCustomToken(0, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sObjectName);
        }
        g_pTlkTable->SetCustomToken(1, CExoString(nTotal));
        g_pTlkTable->SetCustomToken(2, CExoString(nRoll));
        g_pTlkTable->SetCustomToken(3, sSign);
        g_pTlkTable->SetCustomToken(4, CExoString(nModifier));
        g_pTlkTable->SetCustomToken(5, CExoString(nTotal));
        sMessage = g_pSWTlkTable->Fetch(1412, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        return 1;
    }
    case CC_MESSAGE_FEEDBACK:
    {
        int nFeedback = ReadWORD(16);
        if (MessageReadOverflow(1))
            return 0;
        switch (nFeedback)
        {
        case CSWSCreature::FEEDBACK_SKILL_COOLDOWN:
        {
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_PARTY_LEADER_CHANGED:
        case CSWSCreature::FEEDBACK_UNKNOWN_032:
        case CSWSCreature::FEEDBACK_UNKNOWN_034:
        case CSWSCreature::FEEDBACK_UNKNOWN_035:
        case CSWSCreature::FEEDBACK_UNKNOWN_036:
        case CSWSCreature::FEEDBACK_UNKNOWN_037:
        case CSWSCreature::FEEDBACK_PARTY_MEMBER_JOINED:
        case CSWSCreature::FEEDBACK_UNKNOWN_039:
        case CSWSCreature::FEEDBACK_UNKNOWN_040:
        case CSWSCreature::FEEDBACK_UNKNOWN_041:
        case CSWSCreature::FEEDBACK_UNKNOWN_042:
        case CSWSCreature::FEEDBACK_INVITATION_EXPIRED:
        case CSWSCreature::FEEDBACK_PARTY_MEMBER_LEFT:
        case CSWSCreature::FEEDBACK_UNKNOWN_046:
        case CSWSCreature::FEEDBACK_UNKNOWN_048:
        case CSWSCreature::FEEDBACK_DISEASE_IMMUNITY:
        case CSWSCreature::FEEDBACK_CRITICAL_HITS_IMMUNITY:
        case CSWSCreature::FEEDBACK_DEATH_IMMUNITY:
        case CSWSCreature::FEEDBACK_FEAR_IMMUNITY:
        case CSWSCreature::FEEDBACK_KNOCKDOWN_IMMUNITY:
        case CSWSCreature::FEEDBACK_PARALYSIS_IMMUNITY:
        case CSWSCreature::FEEDBACK_NEGATIVE_LEVEL_IMMUNITY:
        case CSWSCreature::FEEDBACK_MIND_SPELLS_IMMUNITY:
        case CSWSCreature::FEEDBACK_POISON_IMMUNITY:
        case CSWSCreature::FEEDBACK_SNEAK_ATTACK_IMMUNITY:
        case CSWSCreature::FEEDBACK_SLEEP_IMMUNITY:
        case CSWSCreature::FEEDBACK_DAZED_IMMUNITY:
        case CSWSCreature::FEEDBACK_CONFUSED_IMMUNITY:
        case CSWSCreature::FEEDBACK_STUN_IMMUNITY:
        case CSWSCreature::FEEDBACK_BLINDNESS_IMMUNITY:
        case CSWSCreature::FEEDBACK_DEAFNESS_IMMUNITY:
        case CSWSCreature::FEEDBACK_CURSED_IMMUNITY:
        case CSWSCreature::FEEDBACK_ENTANGLE_IMMUNITY:
        case CSWSCreature::FEEDBACK_SILENCE_IMMUNITY:
        case CSWSCreature::FEEDBACK_SLOW_IMMUNITY:
        case CSWSCreature::FEEDBACK_UNKNOWN_202:
        case CSWSCreature::FEEDBACK_HORRIFIED_IMMUNITY:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_COUNT:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_COUNTER_PAIR:
        {
            int nValue0 = ReadINT(32);
            int nValue1 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetInteger(0, nValue0);
            pMessage->SetInteger(1, nValue1);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_DAMAGE_IMMUNITY:
        {
            STR_RES cImmunityString;
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            int nValue1 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            pMessage->SetInteger(1, nValue1);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_DAMAGE_RESISTED:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_DAMAGE_RESISTANCE_REMAINING:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            int nValue1 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            pMessage->SetInteger(1, nValue1);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_DAMAGE_REDUCTION:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_DAMAGE_REDUCTION_REMAINING:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            int nValue1 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            pMessage->SetInteger(1, nValue1);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_SPELL_IMMUNITY:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_COUNTER:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_COUNTER_REMAINING:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            int nValue1 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            pMessage->SetInteger(1, nValue1);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_ATTRIBUTE_REDUCED:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            int nValue1 = ReadINT(32);
            float fValue0 = ReadFLOAT(1.0f, 32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            pMessage->SetInteger(1, nValue1);
            pMessage->SetFloat(0, fValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_FLOATY_DATA:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_FLOATY_STRING:
        {
            CExoString sValue0;
            unsigned long oidObject0 = ReadOBJECTIDClient();
            sValue0 = ReadCExoString(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetString(0, sValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_CUSTOM_STRING:
        {
            CExoString sValue0;
            sValue0 = ReadCExoString(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetString(0, sValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_EXPERIENCE_POINTS_RECEIVED:
        {
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_SCRIPT_REPORT:
        {
            CExoString sValue0;
            CExoString sValue1;
            unsigned long oidObject0 = ReadOBJECTIDClient();
            sValue0 = ReadCExoString(32);
            sValue1 = ReadCExoString(32);
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetString(0, sValue0);
            pMessage->SetString(1, sValue1);
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_ACTION_LIST_OVERFLOW:
        case CSWSCreature::FEEDBACK_ACTION_LIST_ERASED:
        {
            CExoString sValue1;
            unsigned long oidObject0 = ReadOBJECTIDClient();
            sValue1 = ReadCExoString(32);
            int nValue0 = ReadINT(32);
            int nValue1 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetString(1, sValue1);
            pMessage->SetInteger(0, nValue0);
            pMessage->SetInteger(1, nValue1);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_EFFECT_LIST_OVERFLOW:
        case CSWSCreature::FEEDBACK_EFFECT_LIST_ERASED:
        {
            CExoString sValue1;
            unsigned long oidObject0 = ReadOBJECTIDClient();
            sValue1 = ReadCExoString(32);
            int nValue0 = ReadINT(32);
            int nValue1 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetString(1, sValue1);
            pMessage->SetInteger(0, nValue0);
            pMessage->SetInteger(1, nValue1);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_AI_UPDATE_TIME_OVERFLOW:
        {
            CExoString sValue1;
            CExoString sValue2;
            unsigned long oidObject0 = ReadOBJECTIDClient();
            sValue1 = ReadCExoString(32);
            int nValue0 = ReadINT(32);
            int nValue1 = ReadINT(32);
            sValue2 = ReadCExoString(32);
            int nValue2 = ReadINT(32);
            int nValue3 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetString(1, sValue1);
            pMessage->SetString(2, sValue2);
            pMessage->SetInteger(0, nValue0);
            pMessage->SetInteger(1, nValue1);
            pMessage->SetInteger(2, nValue2);
            pMessage->SetInteger(3, nValue3);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_ALIGNMENT_SHIFT:
        {
            int nValue0 = ReadINT(32);
            int nValue1 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetInteger(0, nValue0);
            pMessage->SetInteger(1, nValue1);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_GOLD_ADDED:
        case CSWSCreature::FEEDBACK_GOLD_REMOVED:
        {
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_RECEIVED:
        case CSWSCreature::FEEDBACK_ITEM_LOST:
        {
            int bCustomName = ReadBOOL();
            if (MessageReadOverflow(1))
                return 0;
            unsigned char nBaseItem;
            if (bCustomName == 1)
                sObjectName = ReadCExoLocStringClient(CResRef(), NULL);
            else
                nBaseItem = ReadBYTE(8);
            if (MessageReadOverflow(1))
                return 0;
            if (bCustomName == 0)
                sObjectName = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->GetNameText();
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetString(0, sObjectName);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_ITEM_TEXT:
        case CSWSCreature::FEEDBACK_OBJECT_ITEM_NOTICE:
        {
            unsigned long oidObject = ReadOBJECTIDClient();
            int bCustomName = ReadBOOL();
            if (MessageReadOverflow(1))
                return 0;
            unsigned char nBaseItem;
            if (bCustomName == 1)
                sObjectName = ReadCExoLocStringClient(CResRef(), NULL);
            else
                nBaseItem = ReadBYTE(8);
            if (MessageReadOverflow(1))
                return 0;
            if (bCustomName == 0)
                sObjectName = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->GetNameText();
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject);
            pMessage->SetString(0, sObjectName);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_HEALED:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_FORCE_POINTS_REGAINED:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_EXPERIENCE_POINTS_GAINED:
        case CSWSCreature::FEEDBACK_EXPERIENCE_POINTS_LOST:
        {
            int nValue0 = ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_LOCKED:
        {
            unsigned long oidObject0 = ReadOBJECTIDClient();
            if (MessageReadOverflow(1))
                return 0;
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetObjectID(0, oidObject0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        case CSWSCreature::FEEDBACK_FORCE_POWER_NAME:
        {
            int nValue0 = ReadINT(32);
            CSWCCMessageData* pMessage = new CSWCCMessageData;
            pMessage->SetInteger(0, nValue0);
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, pMessage, "");
            break;
        }
        default:
            g_pAppManager->GetClientExoApp()->AddFeedbackMessage(nFeedback, NULL, "");
            break;
        }
        return 1;
    }
    case CC_MESSAGE_DISPEL_MAGIC:
    {
        unsigned long oidObject = ReadOBJECTIDClient();
        unsigned char nCount = ReadBYTE(8);
        if (MessageReadOverflow(1))
            return 0;
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObjectName))
            g_pTlkTable->SetCustomToken(0, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sObjectName);
        }
        sMessage = g_pSWTlkTable->Fetch(0, NULL, 0, 0) +
            " : " + sObjectName + " : ";
        for (unsigned char i = 0; i < nCount; ++i)
        {
            int nSpell = ReadWORD(9);
            if (MessageReadOverflow(1))
                return 0;
            if (i > 0)
                sMessage = sMessage + ", ";
            CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
            unsigned long nNameStrRef = pSpell->m_nNameStrRef;
            sMessage = sMessage + g_pAppManager->GetClientExoApp()->GetGUIString(nNameStrRef);
        }
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        return 1;
    }
    case CC_MESSAGE_STEALTH_DETECTION:
    {
        CExoString sObserver;
        CExoString sCreature;
        unsigned long oidObserver = ReadOBJECTIDClient();
        unsigned long oidCreature = ReadOBJECTIDClient();
        int bDisplay = ReadBOOL();
        short nDetectionModifier = 0;
        short nDetectionBonus1 = 0;
        short nDetectionBonus2 = 0;
        short nDetectionBonus3 = 0;
        short nDetectionBonus4 = 0;
        short nDetectionBonus5 = 0;
        short nDetectionRoll = 0;
        short nStealthModifier = 0;
        short nStealthBonus = 0;
        short nStealthRoll = 0;
        nDetectionRoll = ReadSHORT(16);
        nStealthRoll = ReadSHORT(16);
        nDetectionModifier = ReadSHORT(16);
        nStealthModifier = ReadSHORT(16);
        nDetectionBonus1 = ReadSHORT(16);
        nStealthBonus = ReadSHORT(16);
        nDetectionBonus2 = ReadSHORT(16);
        nDetectionBonus3 = ReadSHORT(16);
        nDetectionBonus4 = ReadSHORT(16);
        nDetectionBonus5 = ReadSHORT(16);
        if (MessageReadOverflow(1))
            return 0;
        CExoString sUnused;
        g_pAppManager->GetClientExoApp()->GetObjectName(oidObserver, sObserver);
        g_pAppManager->GetClientExoApp()->GetObjectName(oidCreature, sCreature);
        if (bDisplay)
        {
            short nDetection = nDetectionRoll + nDetectionModifier + nDetectionBonus1 + nDetectionBonus2 +
                nDetectionBonus3 + nDetectionBonus4 + nDetectionBonus5;
            short nStealth = nStealthRoll + nStealthModifier + nStealthBonus;
            g_pTlkTable->SetCustomToken(0, sObserver);
            g_pTlkTable->SetCustomToken(1, sCreature);
            g_pTlkTable->SetCustomToken(2, CExoString(nDetection));
            g_pTlkTable->SetCustomToken(3, CExoString(nStealth));
            sMessage = g_pSWTlkTable->Fetch(38023, NULL, 0, 0);
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 1);
            g_pTlkTable->SetCustomToken(0, sObserver);
            g_pTlkTable->SetCustomToken(1, CExoString(nDetection));
            g_pTlkTable->SetCustomToken(2, CExoString(nDetectionRoll));
            g_pTlkTable->SetCustomToken(3, CExoString(nDetectionModifier));
            sMessage = g_pSWTlkTable->Fetch(42123, NULL, 0, 0);
            if (nDetectionBonus1)
            {
                g_pTlkTable->SetCustomToken(0, CExoString(nDetectionBonus1));
                sMessage = sMessage + " + " + g_pSWTlkTable->Fetch(42125, NULL, 0, 0);
            }
            if (nDetectionBonus2)
            {
                g_pTlkTable->SetCustomToken(0, CExoString(nDetectionBonus2));
                sMessage = sMessage + " + " + g_pSWTlkTable->Fetch(42126, NULL, 0, 0);
            }
            if (nDetectionBonus3)
            {
                g_pTlkTable->SetCustomToken(0, CExoString(nDetectionBonus3));
                sMessage = sMessage + " + " + g_pSWTlkTable->Fetch(42127, NULL, 0, 0);
            }
            if (nDetectionBonus4)
            {
                g_pTlkTable->SetCustomToken(0, CExoString(nDetectionBonus4));
                sMessage = sMessage + " + " + g_pSWTlkTable->Fetch(42128, NULL, 0, 0);
            }
            if (nDetectionBonus5)
            {
                g_pTlkTable->SetCustomToken(0, CExoString(nDetectionBonus5));
                sMessage = sMessage + " + " + g_pSWTlkTable->Fetch(42129, NULL, 0, 0);
            }
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
            g_pTlkTable->SetCustomToken(0, sCreature);
            g_pTlkTable->SetCustomToken(1, CExoString(nStealth));
            g_pTlkTable->SetCustomToken(2, CExoString(nStealthRoll));
            g_pTlkTable->SetCustomToken(3, CExoString(nStealthModifier));
            sMessage = g_pSWTlkTable->Fetch(42122, NULL, 0, 0);

            if (nDetectionBonus1)
            {
                g_pTlkTable->SetCustomToken(0, CExoString(nStealthBonus));
                sMessage = sMessage + " + " + g_pSWTlkTable->Fetch(42125, NULL, 0, 0);
            }
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        }
        return 1;
    }
    case CC_MESSAGE_MINE_DETECTION:
    {
        CExoString sCreature;
        CExoString sMine;
        unsigned long oidCreature = ReadOBJECTIDClient();
        unsigned long oidMine = ReadOBJECTIDClient();
        short nRoll = ReadSHORT(16);
        short nModifier = ReadSHORT(16);
        short nDifficulty = ReadSHORT(16);
        if (MessageReadOverflow(1))
            return 0;

        CExoString sUnusedString;
        short nTotal = nModifier + nRoll;
        g_pAppManager->GetClientExoApp()->GetObjectName(oidCreature, sCreature);
        g_pAppManager->GetClientExoApp()->GetObjectName(oidMine, sMine);
        g_pTlkTable->SetCustomToken(0, sCreature);
        g_pTlkTable->SetCustomToken(1, sMine);
        g_pTlkTable->SetCustomToken(2, CExoString(nTotal));
        g_pTlkTable->SetCustomToken(3, CExoString(nDifficulty));
        sMessage = g_pSWTlkTable->Fetch(42132, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 1);
        g_pTlkTable->SetCustomToken(0, sCreature);
        g_pTlkTable->SetCustomToken(1, CExoString(nTotal));
        g_pTlkTable->SetCustomToken(2, CExoString(nRoll));
        g_pTlkTable->SetCustomToken(3, CExoString(nModifier));
        sMessage = g_pSWTlkTable->Fetch(42123, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        return 1;
    }
    case CC_MESSAGE_FORCE_POINTS_SPENT:
    {
        CExoString sUnused = ReadCExoString(32);
        int nBaseCost = ReadINT(32);
        int nCost = ReadINT(32);
        ReadINT(32);
        int nModifier = nCost - nBaseCost;
        if (nCost)
        {
            CExoString sCost;
            if (nModifier >= 0)
                sCost.Format(" %d (%d + %d ", nCost, nBaseCost, nModifier);
            else
                sCost.Format(" %d (%d - %d ", nCost, nBaseCost, abs(nModifier));
            sMessage = g_pTlkTable->GetSimpleString(42006) + sCost +
                g_pTlkTable->GetSimpleString(42008) + ")";
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        }
        return 1;
    }
    case CC_MESSAGE_ATTACK_SUMMARY:
    {
        CExoString sAttack;
        unsigned long oidAttacker = ReadOBJECTIDClient();
        unsigned long oidTarget = ReadOBJECTIDClient();
        int nResult = ReadINT(32);
        unsigned short nFeat = ReadWORD(16);
        int nRoll = ReadINT(32);
        int nModifier = ReadINT(32);
        int nDamage = ReadINT(32);
        int bCritical = ReadINT(32);
        int bSneak = ReadINT(32);
        int bSpecial = ReadINT(32);
        int nEffect = ReadINT(32);
        int bEffectResult = ReadBOOL();
        int bAutomaticHit = ReadBOOL();
        int bAutomaticMiss = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidAttacker, sObjectName))
            g_pTlkTable->SetCustomToken(0, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sObjectName);
        }
        if (nResult == 1 || nResult == 2 || nResult == 3)
            sAttack = g_pAppManager->GetClientExoApp()->GetGUIString(42043);
        else
            sAttack = g_pAppManager->GetClientExoApp()->GetGUIString(42044);
        g_pTlkTable->SetCustomToken(1, sAttack);
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidTarget, sObjectName))
            g_pTlkTable->SetCustomToken(2, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(2, sObjectName);
        }
        sMessage = g_pSWTlkTable->Fetch(42042, NULL, 0, 0) + ". ";
        if (nFeat)
        {
            CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
            unsigned long nStrRef = pFeat->m_nNameStrRef;
            g_pTlkTable->SetCustomToken(0, g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef));
            sMessage = sMessage + g_pSWTlkTable->Fetch(42046, NULL, 0, 0) + ". ";
        }
        if (nResult == 1 || nResult == 2 || nResult == 3)
            sAttack = g_pAppManager->GetClientExoApp()->GetGUIString(42133);
        else
            sAttack = g_pAppManager->GetClientExoApp()->GetGUIString(42134);
        g_pTlkTable->SetCustomToken(0, sAttack);
        g_pTlkTable->SetCustomToken(1, CExoString(nRoll));
        g_pTlkTable->SetCustomToken(2, CExoString(nModifier));
        if (nDamage < 0)
            nDamage = 0;
        g_pTlkTable->SetCustomToken(3, CExoString(nDamage));
        sMessage = sMessage + g_pSWTlkTable->Fetch(42119, NULL, 0, 0);
        if (bCritical)
            sMessage = sMessage + g_pSWTlkTable->Fetch(1511, NULL, 0, 0) + " ";
        if (bSneak)
            sMessage = sMessage + g_pSWTlkTable->Fetch(1459, NULL, 0, 0) + " ";
        if (bSpecial)
            sMessage = sMessage + g_pSWTlkTable->Fetch(42303, NULL, 0, 0) + " ";
        if (bAutomaticHit)
            sMessage = sMessage + g_pSWTlkTable->Fetch(42390, NULL, 0, 0) + " ";
        else if (bAutomaticMiss)
            sMessage = sMessage + g_pSWTlkTable->Fetch(42391, NULL, 0, 0) + " ";
        if (nResult == 8)
            sMessage = sMessage + g_pSWTlkTable->Fetch(42411, NULL, 0, 0) + " ";
        else if (nResult == 9)
            sMessage = sMessage + g_pSWTlkTable->Fetch(42421, NULL, 0, 0) + " ";
        if (nEffect)
        {
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidTarget, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            unsigned long nEffectString = 0;
            switch (nEffect)
            {
            case 1: nEffectString = bEffectResult ? 42072 : 42031; break;
            case 4: nEffectString = bEffectResult ? 42073 : 42032; break;
            case 7: nEffectString = bEffectResult ? 42074 : 42033; break;
            case 9: nEffectString = bEffectResult ? 42075 : 42034; break;
            case 2: nEffectString = bEffectResult ? 42076 : 42035; break;
            case 3: nEffectString = bEffectResult ? 42077 : 42036; break;
            case 5: nEffectString = bEffectResult ? 42078 : 42037; break;
            case 6: nEffectString = bEffectResult ? 42079 : 42038; break;
            case 10: nEffectString = bEffectResult ? 42080 : 42039; break;
            case 8: nEffectString = bEffectResult ? 42081 : 42040; break;
            }
            sAttack = g_pAppManager->GetClientExoApp()->GetGUIString(nEffectString);
            g_pTlkTable->SetCustomToken(1, sAttack);
            sMessage = sMessage + g_pSWTlkTable->Fetch(42030, NULL, 0, 0);
        }
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 1);
        return 1;
    }
    case CC_MESSAGE_ATTACK_BREAKDOWN:
    {
        CExoString sHand;
        int nAttack = ReadINT(32);
        int nBase = ReadINT(32);
        ReadINT(32);
        int nAbility = ReadINT(32);
        int nAlternateAbility = ReadINT(32);
        int nFeat = ReadINT(32);
        int nFeatBonus = ReadINT(32);
        int bOffHand = ReadINT(32);
        int nModifier1 = ReadINT(32);
        int nModifier2 = ReadINT(32);
        ReadINT(32);
        int nFeatBonus2 = ReadINT(32);
        int nFeat2 = ReadINT(32);
        int nModifier3 = ReadINT(32);
        int nModifier4 = ReadINT(32);
        int nModifier5 = ReadINT(32);
        int nModifier6 = ReadINT(32);
        int nRoll = ReadINT(32);
        int nModifier7 = ReadINT(32);
        int bAutomaticHit = ReadINT(32);
        int bAutomaticMiss = ReadINT(32);
        int nHandString = (bOffHand != 0) + 42314;
        sHand = g_pSWTlkTable->Fetch(nHandString, NULL, 0, 0);
        g_pTlkTable->SetCustomToken(0, sHand);
        g_pTlkTable->SetCustomToken(1, nAttack);
        sMessage = sMessage + g_pSWTlkTable->Fetch(42146, NULL, 0, 0);
        g_pTlkTable->SetCustomToken(0, nBase);
        sMessage = sMessage + g_pSWTlkTable->Fetch(42316, NULL, 0, 0);
        if (bAutomaticHit)
            sMessage = sMessage + " " + g_pSWTlkTable->Fetch(42390, NULL, 0, 0);
        else if (bAutomaticMiss)
            sMessage = sMessage + " " + g_pSWTlkTable->Fetch(42391, NULL, 0, 0);
        else
        {
            g_pTlkTable->SetCustomToken(0, nRoll);
            sMessage = sMessage + g_pSWTlkTable->Fetch(42392, NULL, 0, 0);
            if (nModifier7)
            {
                g_pTlkTable->SetCustomToken(0, nModifier2 + nModifier7);
                sMessage = sMessage + g_pSWTlkTable->Fetch(42333, NULL, 0, 0);
            }
            if (nModifier1)
            {
                g_pTlkTable->SetCustomToken(0, nModifier1);
                sMessage = sMessage + g_pSWTlkTable->Fetch(42334, NULL, 0, 0);
            }
            if (nFeat && nFeatBonus)
            {
                CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
                unsigned long nStrRef = pFeat->m_nNameStrRef;
                g_pTlkTable->SetCustomToken(0, g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef));
                g_pTlkTable->SetCustomToken(1, nFeatBonus);
                sMessage = sMessage + g_pSWTlkTable->Fetch(42318, NULL, 0, 0);
            }
            if (nFeatBonus2)
            {
                CSWFeat* pFeat = g_pRules->GetFeat(nFeat2);
                unsigned long nStrRef = pFeat->m_nNameStrRef;
                g_pTlkTable->SetCustomToken(0, g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef));
                g_pTlkTable->SetCustomToken(1, nFeatBonus2);
                sMessage = sMessage + g_pSWTlkTable->Fetch(42318, NULL, 0, 0);
            }
            if (nModifier3)
            {
                g_pTlkTable->SetCustomToken(0, nModifier3);
                sMessage = sMessage + g_pSWTlkTable->Fetch(42330, NULL, 0, 0);
            }
            if (nModifier4)
            {
                g_pTlkTable->SetCustomToken(0, nModifier4);
                sMessage = sMessage + g_pSWTlkTable->Fetch(42317, NULL, 0, 0);
            }

            CExoString sUnusedString;
            int nAbilityBonus;
            unsigned long nAbilityString;
            if (nAlternateAbility)
            {
                nAbilityBonus = nAlternateAbility;
                nAbilityString = 42375;
            }
            else
            {
                nAbilityBonus = nAbility;
                nAbilityString = 42154;
            }
            if (nAbilityBonus)
            {
                g_pTlkTable->SetCustomToken(0, nAbilityBonus);
                sMessage = sMessage + g_pSWTlkTable->Fetch(nAbilityString, NULL, 0, 0);
            }
            if (nModifier5)
            {
                g_pTlkTable->SetCustomToken(0, nModifier5);
                sMessage = sMessage + g_pSWTlkTable->Fetch(42331, NULL, 0, 0);
            }
            if (nModifier6)
            {
                g_pTlkTable->SetCustomToken(0, nModifier6);
                sMessage = sMessage + g_pSWTlkTable->Fetch(42332, NULL, 0, 0);
            }
        }
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        break;
    }
    case CC_MESSAGE_THREAT_BREAKDOWN:
    {
        int nRoll = ReadINT(32);
        int nModifier = ReadINT(32);
        int bDisplay = ReadINT(32);
        int nThreat = ReadINT(32);
        int nCritical = ReadINT(32);
        ReadINT(32);
        int bSuccess = ReadINT(32);
        if (bDisplay)
        {
            g_pTlkTable->SetCustomToken(0, CExoString(nRoll));
            g_pTlkTable->SetCustomToken(1, CExoString(nModifier));
            CExoString sResult;
            sResult = g_pAppManager->GetClientExoApp()->GetGUIString(bSuccess ? 1392 : 1393);
            g_pTlkTable->SetCustomToken(2, sResult);
            g_pTlkTable->SetCustomToken(3, CExoString(nThreat));
            g_pTlkTable->SetCustomToken(4, CExoString(nCritical));
            sMessage = g_pSWTlkTable->Fetch(42148, NULL, 0, 0);
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        }
        break;
    }
    case CC_MESSAGE_DEFENSE_BREAKDOWN:
    {
        int nDefense = ReadINT(32);
        int nBase = ReadINT(32);
        int nModifier1 = ReadINT(32);
        ReadINT(32);
        int nModifier2 = ReadINT(32);
        int nModifier3 = ReadINT(32);
        int nModifier4 = ReadINT(32);
        int nModifier5 = ReadINT(32);
        int nModifier6 = ReadINT(32);
        if (!nDefense)
            break;
        g_pTlkTable->SetCustomToken(0, nDefense);
        sMessage = g_pSWTlkTable->Fetch(42149, NULL, 0, 0);
        if (nBase)
        {
            g_pTlkTable->SetCustomToken(0, nBase);
            sMessage = sMessage + g_pSWTlkTable->Fetch(42338, NULL, 0, 0);
        }
        if (nModifier1)
        {
            g_pTlkTable->SetCustomToken(0, nModifier1);
            sMessage = sMessage + g_pSWTlkTable->Fetch(42339, NULL, 0, 0);
        }
        if (nModifier2)
        {
            g_pTlkTable->SetCustomToken(0, nModifier2);
            sMessage = sMessage + g_pSWTlkTable->Fetch(42340, NULL, 0, 0);
        }
        if (nModifier3)
        {
            g_pTlkTable->SetCustomToken(0, nModifier3);
            sMessage = sMessage + g_pSWTlkTable->Fetch(42341, NULL, 0, 0);
        }
        if (nModifier4)
        {
            g_pTlkTable->SetCustomToken(0, nModifier4);
            sMessage = sMessage + g_pSWTlkTable->Fetch(42342, NULL, 0, 0);
        }
        if (nModifier5)
        {
            g_pTlkTable->SetCustomToken(0, nModifier5);
            sMessage = sMessage + g_pSWTlkTable->Fetch(42343, NULL, 0, 0);
        }
        if (nModifier6)
        {
            g_pTlkTable->SetCustomToken(0, nModifier6);
            sMessage = sMessage + g_pSWTlkTable->Fetch(42427, NULL, 0, 0);
        }
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        break;
    }
    case CC_MESSAGE_DAMAGE_BREAKDOWN:
    {
        int anDamage[15] = {0};
        int bFirst = 1;
        CExoString sDetails;
        int i;
        for (i = 0; i < 14; ++i)
        {
            if (i == 0 || i == 1 || i == 2)
            {
                int nPhysical = ReadINT(32);
                if (nPhysical > 0)
                    anDamage[2] += nPhysical;
            }
            else
                anDamage[i] = ReadINT(32);
        }
        int nTotal = ReadINT(32);
        ReadINT(32);
        int nModifier1 = ReadINT(32);
        ReadINT(32);
        int nModifier2 = ReadINT(32);
        ReadINT(32);
        int nModifier3 = ReadINT(32);
        int nModifier4 = ReadINT(32);
        int nModifier5 = ReadINT(32);
        int nReduction1 = ReadINT(32);
        int nReduction2 = ReadINT(32);
        ReadINT(32);
        ReadINT(32);
        ReadINT(32);
        for (i = 0; i < 14; ++i)
        {
            unsigned long nString;
            int nDamage = anDamage[i];
            switch (i)
            {
            case 0: case 1: case 2: nString = 1423; break;
            case 3: nString = 1422; break;
            case 4: nString = 1440; break;
            case 5: nString = 1441; break;
            case 6: nString = 1442; break;
            case 7: nString = 1443; break;
            case 8: nString = 1444; break;
            case 9: nString = 1445; break;
            case 10: nString = 1446; break;
            case 11: nString = 1447; break;
            case 12: nString = 1448; break;
            case 13: nString = 41902; break;
            default: nString = 0; break;
            }
            if (nDamage > 0)
            {
                g_pTlkTable->SetCustomToken(0, nDamage);
                CExoString sDamage = g_pSWTlkTable->Fetch(nString, NULL, 0, 0);
                if (i == 12 || i == 0 || i == 1 || i == 2)
                {
                    if (!bFirst)
                        sDetails = sDamage + " + " + sDetails;
                    else
                    {
                        bFirst = 0;
                        sDetails = sDetails + sDamage;
                    }
                }
                else
                {
                    if (!bFirst)
                        sDetails = sDetails + " + " + sDamage;
                    else
                    {
                        bFirst = 0;
                        sDetails = sDetails + sDamage;
                    }
                }
            }
        }
        if (nTotal > 0)
        {
            if (nModifier1)
            {
                STR_RES cModifier;
                g_pTlkTable->SetCustomToken(0, nModifier1);
                sDetails = sDetails + " " + g_pSWTlkTable->Fetch(42154, NULL, 0, 0);
            }
            if (nModifier4)
            {
                g_pTlkTable->SetCustomToken(0, nModifier4);
                sDetails = sDetails + " " + g_pSWTlkTable->Fetch(42363, NULL, 0, 0);
            }
            if (nModifier2 > 0)
            {
                STR_RES cModifier;
                g_pTlkTable->SetCustomToken(0, nModifier2);
                sDetails = sDetails + " + " + g_pSWTlkTable->Fetch(42155, NULL, 0, 0);
            }
            if (nModifier3 > 0)
            {
                STR_RES cModifier;
                g_pTlkTable->SetCustomToken(0, nModifier3);
                sDetails = sDetails + " + " + g_pSWTlkTable->Fetch(42156, NULL, 0, 0);
            }
            if (nReduction1)
            {
                STR_RES cModifier;
                g_pTlkTable->SetCustomToken(0, nReduction1);
                sDetails = sDetails + " - " + g_pSWTlkTable->Fetch(42433, NULL, 0, 0);
            }
            if (nReduction2)
            {
                STR_RES cModifier;
                g_pTlkTable->SetCustomToken(0, nReduction2);
                sDetails = sDetails + " - " + g_pSWTlkTable->Fetch(42434, NULL, 0, 0);
            }
            CExoString sTotal;
            if (nModifier5 > 0)
            {
                g_pTlkTable->SetCustomToken(0, nModifier5);
                sTotal = g_pSWTlkTable->Fetch(42386, NULL, 0, 0);
            }
            sTotal = sTotal + CExoString(nTotal);
            g_pTlkTable->SetCustomToken(0, sTotal);
            g_pTlkTable->SetCustomToken(1, sDetails);
            sMessage = g_pSWTlkTable->Fetch(42150, NULL, 0, 0);
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        }
        break;
    }
    case CC_MESSAGE_EFFECT_APPLICATION_BREAKDOWN:
    {
        unsigned long oidObject = ReadOBJECTIDClient();
        ReadINT(32);
        int nEffect = ReadINT(32);
        ReadINT(32);
        ReadINT(32);
        ReadINT(32);
        ReadINT(32);
        ReadINT(32);
        ReadINT(32);
        int nResult = ReadINT(32);
        if (nResult == -1 || nResult == 1)
            break;
        CExoString sPrefix;
        sPrefix = g_pSWTlkTable->Fetch(42157, NULL, 0, 0);
        if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObjectName))
            g_pTlkTable->SetCustomToken(0, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sObjectName);
        }
        unsigned long nResultString;
        switch (nResult)
        {
        default: nResultString = 42158; break;
        case 0: nResultString = 42158; break;
        case 1: nResultString = 42159; break;
        case 2: nResultString = 42160; break;
        }
        unsigned long nEffectString = 0;
        if (nEffect)
        {
            switch (nEffect)
            {
            case 1: nEffectString = 42031; break;
            case 4: nEffectString = 42032; break;
            case 7: nEffectString = 42033; break;
            case 9: nEffectString = 42034; break;
            case 2: nEffectString = 42035; break;
            case 3: nEffectString = 42036; break;
            case 5: nEffectString = 42037; break;
            case 6: nEffectString = 42038; break;
            case 10: nEffectString = 42039; break;
            case 8: nEffectString = 42040; break;
            }
            CExoString sEffect = g_pSWTlkTable->Fetch(nEffectString, NULL, 0, 0);
            g_pTlkTable->SetCustomToken(1, sEffect);
        }
        sMessage = sPrefix + g_pSWTlkTable->Fetch(nResultString, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        break;
    }
    case CC_MESSAGE_DEFLECT_BREAKDOWN:
    {
        unsigned long oidObject = ReadOBJECTIDClient();
        ReadINT(32);
        int nDifficulty = ReadINT(32);
        int nRoll = ReadINT(32);
        int nBase = ReadINT(32);
        int nFeatBonus = ReadINT(32);
        int nFeat = ReadINT(32);
        int nModifier1 = ReadINT(32);
        int nModifier2 = ReadINT(32);
        int nModifier3 = ReadINT(32);
        if (nRoll)
        {
            CExoString sDetails;
            GetSingleCustomTokenStringPart(nBase, 42316, sDetails);
            if (nFeatBonus)
            {
                g_pTlkTable->SetCustomToken(0, g_pRules->GetFeat(nFeat)->GetNameText());
                g_pTlkTable->SetCustomToken(1, nFeatBonus);
                sDetails = sDetails + g_pSWTlkTable->Fetch(42318, NULL, 0, 0);
            }
            GetSingleCustomTokenStringPart(nModifier1, 42419, sDetails);
            GetSingleCustomTokenStringPart(nModifier2, 42420, sDetails);
            GetSingleCustomTokenStringPart(nModifier3, 42620, sDetails);
            if (g_pAppManager->GetClientExoApp()->GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pTlkTable->SetCustomToken(1, nRoll);
            g_pTlkTable->SetCustomToken(2, sDetails);
            g_pTlkTable->SetCustomToken(3, nDifficulty);
            sMessage = g_pSWTlkTable->Fetch(42417, NULL, 0, 0);
            g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        }
        break;
    }
    case CC_MESSAGE_POISONED:
    {
        if (g_pAppManager->GetClientExoApp()->GetObjectName(ReadOBJECTIDClient(), sObjectName))
            g_pTlkTable->SetCustomToken(0, sObjectName);
        else
        {
            sObjectName = g_pAppManager->GetClientExoApp()->GetGUIString(0);
            g_pTlkTable->SetCustomToken(0, sObjectName);
        }
        int nPoison = ReadINT(32);
        g_pTlkTable->SetCustomToken(1, g_pTlkTable->GetSimpleString(nPoison));
        sMessage = g_pSWTlkTable->Fetch(47873, NULL, 0, 0);
        g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
        break;
    }
    }
    return 0;
}
// PC 0x006631C0  Xbox 0x00088910
int CSWCMessage::HandleServerToPlayerSafeProjectile(unsigned char nMinor)
{
    int bSuccess;

    bSuccess = 0;
    if (nMinor == 1)
    {
        bSuccess = HandleServerToPlayerSafeProjectileSpell();
    }
    else if (nMinor == 2)
    {
        bSuccess = HandleServerToPlayerSafeProjectileProjectile();
    }
    return bSuccess;
}
// PC 0x006631E0  Xbox 0x00088930
int CSWCMessage::HandleServerToPlayerGuiInfoPopupMessage(unsigned char nMinor)
{
    NwchandlemessageTrace1();
    switch (nMinor)
    {
    case 2:
        return 1;
    case 3:
        return 0;
    case 4:
        return 0;
    case 5:
        return 0;
    case 6:
        return 0;
    }
    return 0;
}
// PC 0x00663220  Xbox 0x00088970
int CSWCMessage::HandleServerToPlayerCheatMessage(unsigned char nMinor)
{
    switch (nMinor)
    {
    case CHEAT_DO_NASTY:
        return HandleServerToPlayerCheat_DoNasty();
    case CHEAT_PONY_RIDE:
        return HandleServerToPlayerCheat_PonyRide();
    case CHEAT_DEBUG_MODE:
        return HandleServerToPlayerCheat_DebugMode();
    }
    return 0;
}
// PC 0x006632A0  Xbox 0x000889F0
int CSWCMessage::HandleServerToPlayerJournalMessage_AddQuest()
{
    NwchandlemessageTrace1();
    CSWCJournal* pJournal = g_pAppManager->m_pClientExoApp->GetQuestJournal();
    SJournalEntry entry;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    entry.m_lsName.AddString(
        g_pAppManager->m_pClientExoApp->GetClientLanguage(),
        ReadCExoLocStringClient(CResRef(), NULL), 0);
    entry.m_lsText.AddString(
        g_pAppManager->m_pClientExoApp->GetClientLanguage(),
        ReadCExoLocStringClient(CResRef(), NULL), 0);
    entry.m_nDate = ReadDWORD(32);
    entry.m_nTime = ReadDWORD(32);
    entry.m_sPlotID = ReadCExoString(32);
    entry.m_nState = ReadINT(32);
    entry.m_nPriority = ReadDWORD(32);
    entry.m_nPicture = ReadWORD(16);
    entry.m_bCompleted = ReadBOOL();
    entry.m_bNew = 1;
    entry.m_nPlotIndex = ReadINT(32);
    entry.m_nPlanetID = ReadINT(32);
    entry.m_nXP = ReadINT(32);
    pJournal->Add(entry, 0x8000);
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    return 1;
}
// PC 0x0064B540  Xbox 0x00070BA0
CSWCItem* CSWCMessage::CreateItem(unsigned long nBaseItem,
                                  unsigned char nVariation, char cPrefix)
{
    CSWCItem* pItem = new CSWCItem(nBaseItem);
    if (pItem == NULL)
    {
        return NULL;
    }
    CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem);
    if (pBaseItem == NULL)
    {
        return NULL;
    }
    CResRef cResRef;
    cResRef = pBaseItem->GetIconResRef(nVariation, cPrefix);
    if (!g_pExoResMan->Exists(cResRef, RESTYPE_TGA, (unsigned long*)NULL) &&
        !g_pExoResMan->Exists(cResRef, RESTYPE_TPC, (unsigned long*)NULL))
    {
        cResRef = CResRef(pBaseItem->m_resDefaultIcon);
        if (!g_pExoResMan->Exists(cResRef, RESTYPE_TGA, (unsigned long*)NULL) ||
            !g_pExoResMan->Exists(cResRef, RESTYPE_TPC, (unsigned long*)NULL))
        {
            CExoString sMessage;
            CExoString sIcon;
            CExoString sDefaultIcon;
            pBaseItem->GetIconResRef(nVariation, cPrefix).CopyToString(sIcon);
            cResRef.CopyToString(sDefaultIcon);
            sMessage.Format("Icon %s nor the default icon %s could be loaded.",
                            sIcon.CStr(), sDefaultIcon.CStr());
        }
    }
    pItem->m_cIconResRef = cResRef;
    pItem->m_nModelVariation = nVariation;
    pItem->m_nBodyVariation = cPrefix;
    cResRef = pBaseItem->GetModelResRef(nVariation, cPrefix);
    if (!g_pExoResMan->Exists(cResRef, RESTYPE_MDL, (unsigned long*)NULL))
    {
        cResRef = CResRef(pBaseItem->m_resDefaultModel);
        if (!g_pExoResMan->Exists(cResRef, RESTYPE_MDL, (unsigned long*)NULL))
        {
            CExoString sMessage;
            CExoString sModel;
            CExoString sDefaultModel;
            pBaseItem->GetModelResRef(nVariation, cPrefix).CopyToString(sModel);
            cResRef.CopyToString(sDefaultModel);
            sMessage.Format(
                "Model %s nor the default model %s could be loaded.",
                sModel.CStr(), sDefaultModel.CStr());
        }
    }
    pItem->m_cModelResRef = cResRef;
    return pItem;
}
// PC 0x0064B8E0  Xbox 0x00070F40
int CSWCMessage::HandleServerToPlayerCamera(unsigned char nMinor)
{
    unsigned char nFlags;
    float fYaw;
    float fDist;
    float fPitch;
    CAurCamera* pCamera;
    CClientOptions* pClientOptions;
    unsigned char nCameraMode;
    char szCommand[64];

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMinor == 1)
    {
        nFlags = ReadBYTE(8);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        nMinor = nFlags & 1;
        if (nMinor != 0)
        {
            fYaw = ReadFLOAT(1.0f, 32);
        }
        if ((nFlags & 2) != 0)
        {
            fDist = ReadFLOAT(1.0f, 32);
        }
        if ((nFlags & 4) != 0)
        {
            fPitch = ReadFLOAT(1.0f, 32);
        }
        if ((nFlags & 8) != 0)
        {
            ReadBYTE(8);
        }
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (g_pAppManager->m_pClientExoApp->GetModule() != NULL)
        {
            pCamera =
                g_pAppManager->m_pClientExoApp->GetModule()->m_pModuleCamera;
        }
        if (pCamera != NULL)
        {
            if (nMinor != 0)
            {
                sprintf(szCommand, "yaw %f", (fYaw - HALF_PI) * RAD_TO_DEG);
                pCamera->BehaviorMessage(szCommand);
            }
            if ((nFlags & 2) != 0)
            {
                sprintf(szCommand, "dist %f", fDist);
                pCamera->BehaviorMessage(szCommand);
            }
            if ((nFlags & 4) != 0)
            {
                sprintf(szCommand, "pitch %f", fPitch);
                pCamera->BehaviorMessage(szCommand);
                return 0;
            }
        }
    }
    else if (nMinor == 2)
    {
        nCameraMode = ReadBYTE(8);
        if (!MessageReadOverflow(1))
        {
            pClientOptions = g_pAppManager->m_pClientExoApp->GetClientOptions();
            if (pClientOptions != NULL)
            {
                pClientOptions->SetCameraMode(nCameraMode);
            }
        }
    }
    return 0;
}
// PC 0x0064BAB0  Xbox 0x00071130
int CSWCMessage::HandleServerToPlayerArea(unsigned char nMinor)
{
    if (MessageReadOverflow(1))
        return 0;
    switch (nMinor)
    {
    case AREA_MESSAGE_WEATHER:
    {
        unsigned char nWeather = ReadBYTE(8);
        int bEnabled = ReadBOOL();
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        if (pModule && pModule->m_pInfo)
        {
            if (bEnabled)
                pModule->m_pInfo->StartWeather(nWeather);
            else
                pModule->m_pInfo->StopWeather(nWeather);
            return 1;
        }
        break;
    }
    case AREA_MESSAGE_VISUAL_EFFECT:
    {
        unsigned short nEffect = ReadWORD(16);
        Vector vPosition;
        vPosition.m_fX = ReadFLOAT(1.0f, 32);
        vPosition.m_fY = ReadFLOAT(1.0f, 32);
        vPosition.m_fZ = ReadFLOAT(1.0f, 32);
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        if (pModule)
        {
            pModule->m_pInfo->ApplyFireAndForgetVisualEffect(nEffect, vPosition);
            return 1;
        }
        break;
    }
    case AREA_MESSAGE_TRANSITION:
    {
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        if (!pModule)
        {
            ClearReadMessage();
            return 0;
        }
        CSWCArea* pArea = pModule->m_pInfo;
        CAurCamera* pCamera = pModule->m_pModuleCamera;
        if (!pArea || !pCamera)
        {
            ClearReadMessage();
            return 0;
        }
        CGameObject* pRemovedObject;
        g_pAppManager->GetClientExoApp()->GetGameObjectArray()->Delete(
            pArea->GetId(), &pRemovedObject);
        CExoString sCustomImage;
        unsigned char nImage = ReadBYTE(8);
        if (nImage == AREA_LOADSCREEN_CUSTOM)
            sCustomImage = ReadCExoString(32);
        CGuiAreaLoadscreen* pLoadScreen;
        {
            CExoString sImage;
            if (nImage == AREA_LOADSCREEN_RANDOM)
            {
                int nRows = g_pRules->Get2DArrays()->GetAreaTransition()->GetNumRows();
                nImage = rand() % (nRows - AREA_LOADSCREEN_FIRST_TABLE_ROW) +
                    AREA_LOADSCREEN_FIRST_TABLE_ROW;
            }
            if (nImage == AREA_LOADSCREEN_CUSTOM)
            {
                CClientExoAppInternal* pInternal =
                    g_pAppManager->GetClientExoApp()->m_pcExoAppInternal;
                pLoadScreen = pInternal->StartLoadBar(2, sCustomImage.CStr(), 1);
            }
            else
            {
                g_pRules->Get2DArrays()->GetAreaTransition()->GetCExoStringEntry(
                    nImage, "BMPResRef", &sImage);
                CClientExoAppInternal* pInternal =
                    g_pAppManager->GetClientExoApp()->m_pcExoAppInternal;
                pLoadScreen = pInternal->StartLoadBar(2, sImage.CStr(), 1);
            }
            g_pAppManager->m_pClientExoApp->GetModule()->Update(0.00000001f);
            SetClearColor("0 0 0");
            g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
            g_pAppManager->m_pClientExoApp->GetWorldTimer()->
                GetSnapshotTimeDifference();
        }
        g_pAppManager->m_pClientExoApp->EmptyHitcheckIgnoreList();
        UpdateScreen(0.05f, 0, 0);
        pArea->UnloadArea();
        float fX = ReadFLOAT(1.0f, 32);
        float fY = ReadFLOAT(1.0f, 32);
        float fZ = ReadFLOAT(1.0f, 32);
        float fYaw = ReadFLOAT(1.0f, 32);
        ReadBOOL();
        if (MessageReadOverflow(1))
        {
            ClearReadMessage();
            return 0;
        }
        if (!pArea->LoadArea(pCamera, pLoadScreen, 1))
        {
            ClearReadMessage();
            return 0;
        }
        CSWCCreature* pPlayer = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
        if (pPlayer)
        {
            Vector vPosition(fX, fY, fZ);
            pPlayer->SetPosition(vPosition);
            pPlayer->m_vDesiredPosition = vPosition;
            Vector vOrientation(cos(fYaw), sin(fYaw), 0.0f);
            pPlayer->SetOrientation(vOrientation);
            pPlayer->SetWayPoints(0, 0);
        }
        return 1;
    }
    }
    return 0;
}
// PC 0x0064BF30  Xbox 0x000715C0
int CSWCMessage::HandleServerToPlayerSetCustomToken(unsigned char nMinor)
{
    CExoString sToken;
    unsigned long nToken;
    unsigned long nCount;
    unsigned long i;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMinor == 1)
    {
        nToken = ReadINT(32);
        sToken = ReadCExoString(32);
        g_pTlkTable->SetCustomToken(nToken, sToken);
    }
    else
    {
        if (nMinor != 2)
        {
            return 0;
        }
        nCount = ReadDWORD(32);
        for (i = 0; i < nCount; i++)
        {
            nToken = ReadINT(32);
            sToken = ReadCExoString(32);
            g_pTlkTable->SetCustomToken(nToken, sToken);
        }
    }
    if (!MessageReadOverflow(1) && MessageReadUnderflow(1) == 0)
    {
        return 1;
    }
    return 0;
}
// PC 0x0064C0A0  Xbox 0x00071730
int CSWCMessage::HandleServerToPlayerCreatureUpdate_Add()
{

    Vector vOrientation;
    CSWCCreature* pPlayerCreature =
        g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    int bExisting = 0;
    unsigned long oidCreature = ReadOBJECTIDClient();
    unsigned char bInParty = ReadBYTE(8);
    float fX = ReadFLOAT(1.0f, 32);
    float fY = ReadFLOAT(1.0f, 32);
    float fZ = ReadFLOAT(1.0f, 32);
    vOrientation.m_fX = ReadFLOAT(1.0f, 32);
    vOrientation.m_fY = ReadFLOAT(1.0f, 32);
    vOrientation.m_fZ = ReadFLOAT(1.0f, 32);
    unsigned short nServerUpdateValue = ReadWORD(16);

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    CSWCCreature* pCreature = pClientExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        bExisting = 1;
        pCreature->m_bFadeLocked = 0;
        if (pPlayerCreature != NULL)
        {
            pPlayerCreature->UpdateRelativePerception(oidCreature);
        }
        CSWCArea* pCreatureArea = pCreature->m_pArea;
        if (pCreatureArea->GetObjectsAtPositionVisible(pCreature->m_vPosition) == 1)
        {
            pCreature->SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 0, 0, 0);
        }
        else
        {
            pCreature->SetDesiredFadeState(CSWCObject::FADE_STATE_HIDDEN, 0, 0, 0);
        }
        pCreature->m_bVisibilityUpdated = 1;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    CSWCArea* pArea = pClientExoApp->GetModule()->m_pInfo;
    if (bExisting == 0)
    {
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pCreature = pClientExoApp->GetCreatureByGameObjectID(oidCreature);
        if (pCreature == NULL)
        {
            pCreature = new CSWCCreature;
            if (pCreature == NULL)
            {
                return 0;
            }
            pCreature->SetId(oidCreature);
            pCreature->SetArea(pArea);
            pCreature->SetInParty(bInParty == 1);
            Vector vPosition(fX, fY, fZ);
            pCreature->SetPosition(vPosition);
            pCreature->SetOrientation(vOrientation);
            pCreature->SetMoveOrientation(vOrientation);
            pCreature->SetDesiredOrientation(vOrientation);
            pCreature->m_vDesiredPosition = vPosition;
            pClientExoApp = g_pAppManager->m_pClientExoApp;
            if (pClientExoApp->GetGameObjectArray()->AddExternalObject(
                    oidCreature, (CGameObject*)pCreature, 1) != CGameObjectArray::SUCCESS)
            {
                return 0;
            }
            CClientAIMaster* pAIMaster =
                g_pAppManager->m_pClientExoApp->GetClientAIMaster();
            pAIMaster->AddObject(pCreature);
        }
        if (pPlayerCreature != NULL)
        {
            pPlayerCreature->UpdateRelativePerception(oidCreature);
        }
        pCreature->SetFadeState(CSWCObject::FADE_STATE_VISIBLE);
        pCreature->SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 0, 0, 0);
    }
    pCreature->AttachToScene(pArea->m_pScene);
    pCreature->m_nServerUpdateValue = nServerUpdateValue;
    pCreature->m_bFadeLocked = 0;
    return 1;
}
// PC 0x0064C3C0  Xbox 0x00071A50
int CSWCMessage::HandleServerToPlayerCreatureUpdate_VisEffect()
{

    Vector vSourcePosition;
    Vector vPosition;
    float fDuration = 0.0f;
    unsigned long oidDummy;
    unsigned long oidObject = ReadOBJECTIDClient();
    unsigned short nEffect = ReadWORD(16);
    if (MessageReadOverflow(1))
        return 0;

    vPosition.m_fX = ReadFLOAT(1.0f, 32);
    vPosition.m_fY = ReadFLOAT(1.0f, 32);
    vPosition.m_fZ = ReadFLOAT(1.0f, 32);
    if (CLoopingVisualEffect::GetIsBeam(nEffect))
    {
        unsigned long oidSource = ReadOBJECTIDClient();
        unsigned long oidTarget = ReadOBJECTIDClient();
        unsigned char nSourceNode = ReadBYTE(8);
        unsigned char nTargetNode = ReadBYTE(8);
        vSourcePosition.m_fX = ReadFLOAT(1.0f, 32);
        vSourcePosition.m_fY = ReadFLOAT(1.0f, 32);
        vSourcePosition.m_fZ = ReadFLOAT(1.0f, 32);
        if (oidObject == OBJECT_INVALID)
            fDuration = ReadFLOAT(1.0f, 32);
        if (MessageReadOverflow(1))
            return 0;

        CSWCObject* pSource = g_pAppManager->GetClientExoApp()->GetGameObject(oidSource);
        if (pSource == NULL ||
            (pSource->m_nObjectType != CGameObject::OBJECT_TYPE_CREATURE &&
             pSource->m_nObjectType != CGameObject::OBJECT_TYPE_PLACEABLE &&
             pSource->m_nObjectType != CGameObject::OBJECT_TYPE_DOOR))
        {
            CSWCCreature* pDummy = new CSWCCreature;
            if (pDummy == NULL)
                return 0;
            pDummy->m_nCameraFollowDelay = 0;
            if (pDummy->LoadModel("c_dummy", CSWCObject::MODEL_SLOT_DEFAULT, 0))
            {
                CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
                if (pArea != NULL)
                {
                    if (!g_pAppManager->GetClientExoApp()->GetGameObjectArray()->AddInternalObject(oidDummy, pDummy, 0))
                        return 0;
                    pDummy->SetId(oidDummy);
                    pDummy->SetArea(pArea);
                    CClientAIMaster* pAIMaster = g_pAppManager->GetClientExoApp()->GetClientAIMaster();
                    pAIMaster->AddObject(pDummy);
                    CAurScene* pScene = pArea->m_pScene;
                    pDummy->AttachToScene(pScene);
                    pDummy->SetPosition(vSourcePosition);
                    pDummy->m_vDesiredPosition = vSourcePosition;
                    pDummy->GetAppearance()->m_nAppearance = CCAppearanceInfo::APPEARANCE_SAFE_PROJECTILE_DUMMY;
                }
            }
            if (fDuration == 0.0f)
                pDummy->SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 1, VISUAL_EFFECT_BEAM_DUMMY_DELAY, 0);
            else
                pDummy->SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 1, (unsigned long)(fDuration * 1000.0f), 0);
            pDummy->m_bFadeLocked = 1;
        }

        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidObject);
        if (pObject != NULL &&
            (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE ||
             pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE ||
             pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR))
        {
            pObject = pObject->AsSWCObject();
        }
        else
        {
            CSWCCreature* pDummy = new CSWCCreature;
            if (pDummy == NULL)
                return 0;
            pDummy->m_nCameraFollowDelay = 0;
            if (pDummy->LoadModel("c_dummy", CSWCObject::MODEL_SLOT_DEFAULT, 0))
            {
                CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
                if (pArea != NULL)
                {
                    if (g_pAppManager->GetClientExoApp()->GetGameObjectArray()->AddInternalObject(oidDummy, pDummy, 0) != CGameObjectArray::SUCCESS)
                        return 0;
                    pDummy->SetId(oidDummy);
                    pDummy->SetArea(pArea);
                    CClientAIMaster* pAIMaster = g_pAppManager->GetClientExoApp()->GetClientAIMaster();
                    pAIMaster->AddObject(pDummy);
                    pDummy->AttachToScene(pArea->m_pScene);
                    pDummy->SetPosition(vPosition);
                    pDummy->m_vDesiredPosition = vPosition;
                    pDummy->GetAppearance()->m_nAppearance = CCAppearanceInfo::APPEARANCE_SAFE_PROJECTILE_DUMMY;
                }
            }
            if (fDuration == 0.0f)
                pDummy->SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 1, VISUAL_EFFECT_BEAM_DUMMY_DELAY, 0);
            else
                pDummy->SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 1, (unsigned long)(fDuration * 1000.0f), 0);
            pDummy->m_bFadeLocked = 1;
            pObject = pDummy;
        }
        if (fDuration == 0.0f)
        {
            if (oidObject == CGameObjectArray::INVALIDOBJID)
                pObject->AddVisualEffect(nEffect, 1, oidSource, oidTarget, nSourceNode, nTargetNode, vPosition);
            else
                pObject->AddVisualEffect(nEffect, 1, oidSource, oidTarget, nSourceNode, nTargetNode, Vector());
        }
        else
        {
            if (oidObject == CGameObjectArray::INVALIDOBJID)
                pObject->AddVisualEffect(nEffect, 0, oidSource, oidTarget, nSourceNode, nTargetNode, vPosition);
            else
                pObject->AddVisualEffect(nEffect, 0, oidSource, oidTarget, nSourceNode, nTargetNode, Vector());
        }
    }
    else
    {
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            return 0;
        if (oidObject == OBJECT_INVALID)
        {
            CSWCCreature* pDummy = new CSWCCreature;
            if (pDummy == NULL)
                return 0;
            pDummy->m_nCameraFollowDelay = 0;
            CResRef cDummy("c_dummy");
            if (pDummy->LoadModel(cDummy, CSWCObject::MODEL_SLOT_DEFAULT, 0))
            {
                CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
                if (pArea != NULL)
                {
                    if (g_pAppManager->GetClientExoApp()->GetGameObjectArray()->AddInternalObject(oidDummy, pDummy, 0) != CGameObjectArray::SUCCESS)
                        return 0;
                    pDummy->SetId(oidDummy);
                    pDummy->SetArea(pArea);
                    CClientAIMaster* pAIMaster = g_pAppManager->GetClientExoApp()->GetClientAIMaster();
                    pAIMaster->AddObject(pDummy);
                    pDummy->AttachToScene(pArea->m_pScene);
                    pDummy->SetPosition(vPosition);
                    pDummy->m_vDesiredPosition = vPosition;
                    pDummy->GetAppearance()->m_nAppearance = CCAppearanceInfo::APPEARANCE_SAFE_PROJECTILE_DUMMY;
                }
            }
            pDummy->SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 1, VISUAL_EFFECT_DUMMY_DELAY, 0);
            pDummy->m_bFadeLocked = 1;
            pDummy->AddVisualEffect(nEffect, 1, OBJECT_INVALID, OBJECT_INVALID, 0, 0, vPosition);
        }
        else
        {
            CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidObject);
            if (pObject == NULL)
                return 0;
            pObject->AddVisualEffect(nEffect, 1, OBJECT_INVALID, OBJECT_INVALID, 0, 0, vPosition);
        }
    }
    return 1;
}
// PC 0x0064CAF0  Xbox 0x000721E0
int CSWCMessage::HandleServerToPlayerCreatureUpdate_ControlObject(
    unsigned long oidObject)
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
#ifndef _XBOX
    pApp->SetCharacterChangeInProgress(0);
#endif
    CSWCCreature* pOldCreature = pApp->GetPlayerCreature();
    pApp->SetPlayerCreature(oidObject);
    if (pOldCreature != NULL)
    {
        unsigned long oidOldCreature = pOldCreature->m_nId;
        CSWCPlayer* pPlayer = pApp->GetPlayerByGameObjectID(oidOldCreature);
        if (pPlayer != NULL)
            pPlayer->m_oidControlledObject = oidObject;
    }
    CSWCCreature* pCreature = pApp->GetCreatureByGameObjectID(oidObject);
    if (pCreature == NULL)
    {
        return 0;
    }
    CSWCModule* pModule = pApp->GetModule();
    if (pModule == NULL)
    {
        return 0;
    }
    if (pModule->m_pModuleCamera == NULL)
    {
        return 0;
    }
    if (pOldCreature != pCreature)
    {
        pApp->GetPlayerByGameObjectID(oidObject);
        pCreature->m_pStats->GetMaxHitPoints();
        pApp->AttachPlayerGameObjectToArea();
        if (enablemodelrequest)
        {
            pCreature->m_bCameraFollowPending = 1;
            pCreature->m_bCameraFollowSnap = 1;
        }
        else
        {
            pModule->SetCameraFollow(pCreature->m_nId, 1);
        }
        if (pApp->GetModule()->m_pInfo != NULL &&
            (pApp->GetModule()->m_pInfo->m_nFlags & CSWArea::AREA_FLAG_HEAT_SHIMMER))
        {
            Vector vOffset;
            pCreature->AddVisualEffect(VFX_ARD_HEAT_SHIMMER, 0,
                                      OBJECT_INVALID, OBJECT_INVALID,
                                      0, 0, vOffset);
            if (pOldCreature != NULL)
            {
                pOldCreature->DeleteVisualEffect(VFX_ARD_HEAT_SHIMMER);
            }
        }
        SendPlayerToServerMessage(CONTROL_OBJECT_ACK_MAJOR,
                                 CONTROL_OBJECT_ACK_MINOR, NULL, 0);
    }
    else
    {
        if (enablemodelrequest)
        {
            pCreature->m_bCameraFollowPending = 1;
            pCreature->m_bCameraFollowSnap = 1;
        }
        else
        {
            pModule->SetCameraFollow(pCreature->m_nId, 1);
        }
    }
    return 1;
}
// PC 0x0064CC90  Xbox 0x00072370
int CSWCMessage::HandleServerToPlayerCreatureUpdate_Delete()
{

    unsigned long oidCreature = ReadOBJECTIDClient();
    int bInstant = ReadBOOL();
    int bHidden = ReadBOOL();
    unsigned long nFadeDuration = ReadDWORD(32);
    int bDead = ReadBOOL();

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    unsigned long nFadeDelay = bInstant ? 0 : 45000;
    CSWCCreature* pCreature = g_pAppManager->GetClientExoApp()
        ->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        CSWSCreature* pServerCreature = pCreature->GetServerCreature();
        if (!bDead)
        {
            if (bHidden)
            {
                pCreature->SetFadeState(CSWCObject::FADE_STATE_HIDDEN);
                pCreature->SetDesiredFadeState(CSWCObject::FADE_STATE_HIDDEN,
                                               1, nFadeDelay, 0);
            }
            else
            {
                pCreature->SetFadeState(CSWCObject::FADE_STATE_VISIBLE);
                pCreature->SetDesiredFadeState(CSWCObject::FADE_STATE_HIDDEN,
                                               1, nFadeDelay, nFadeDuration);
            }
        }
        else if (pCreature->m_pArea != NULL)
        {
            int nBodyBag;
            int bFound;
            {
                CExoString sColumn("Body_Bag");
                unsigned short nAppearance =
                    pCreature->m_pAppearance->m_nAppearance;
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pAppearance = p2DArrays->m_pAppearance;
                bFound = pAppearance->GetINTEntry(nAppearance, sColumn,
                                                  &nBodyBag);
            }
            int bCorpse;
            if (bFound)
            {
                CExoString sColumn("Corpse");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pBodyBag = p2DArrays->m_pBodyBag;
                bFound = pBodyBag->GetINTEntry(nBodyBag, sColumn, &bCorpse);
            }
            if (!bFound || !bCorpse)
            {
                pCreature->m_pArea->AddCorpse(pCreature->m_nId);
            }
        }
        pCreature->KillAllVisualEffects();
        pCreature->SetCombatState(0);
        pCreature->m_bInvisibilityVisualApplied = 0;
        if (pServerCreature != NULL || pCreature->m_pArea == NULL)
        {
            pCreature->m_bFadeLocked = 1;
        }
    }
    return 1;
}
// PC 0x006634A0  Xbox 0x00088BF0
int CSWCMessage::HandleServerToPlayerExportReply()
{

    CExoFile* pFile = NULL;
    CExoString sFilename;
    STR_RES strRes;
    CExoString sText;
    CExoString sFirstName;
    CExoString sLastName;

    sFirstName = ReadCExoLocStringClient(CResRef(), NULL);
    sLastName = ReadCExoLocStringClient(CResRef(), NULL);
    unsigned long nSize = ReadDWORD(32);
    void* pData = ReadVOIDPtr(nSize);

    if (MessageReadOverflow(1) || MessageReadUnderflow(1))
        goto ExportFailed;
    {
        sFilename = "LOCALVAULT:";
        sFilename = sFilename + g_pAppManager->GetClientExoApp()
            ->GetPlayerCreature()->GetSaveCreatureFilename(sFirstName,
                                                          sLastName);
        pFile = new CExoFile(sFilename, RESTYPE_BIC, "wb");
    }
    if (pFile == NULL)
        goto ExportFailed;
    {
        pFile->Write(pData, nSize, 1);
        g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
            FEEDBACK_MESSAGE_CHARACTER_EXPORT_SUCCESS, NULL, "");
    }
    goto ExportDone;
ExportFailed:
    {
        g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
            FEEDBACK_MESSAGE_CHARACTER_EXPORT_FAILED, NULL, "");
    }
ExportDone:
    if (pFile != NULL)
        delete pFile;

    if (g_pAppManager->GetClientExoApp()->m_pcExoAppInternal
            ->m_nCharacterExportState ==
            CClientExoAppInternal::CHARACTER_EXPORT_PENDING)
    {
        CClientExoAppInternal* pInternal =
            g_pAppManager->GetClientExoApp()->m_pcExoAppInternal;
        pInternal->m_nCharacterExportState =
            CClientExoAppInternal::CHARACTER_EXPORT_IDLE;
    }
    return 1;
}
// PC 0x006637C0  Xbox 0x00088F60
int CSWCMessage::HandleServerToPlayerGuiQuickbar(unsigned char nMinor)
{
    unsigned char nButton;

    switch (nMinor)
    {
    case 1:
        return HandleServerToPlayerGuiQuickbar_SetButton(0, 1);
    case 2:
        nButton = ReadBYTE(8);
        return HandleServerToPlayerGuiQuickbar_SetButton(nButton, 0);
    }
    return 0;
}
// PC 0x00663810  Xbox 0x00088FB0
int CSWCMessage::HandleServerToPlayerMapPin(unsigned char nMinor)
{
    switch (nMinor)
    {
    case 1:
        return HandleServerToPlayerMapPinEnabled();
    case 4:
        return HandleServerToPlayerMapPinReferenceNumber();
    }
    return 0;
}
// PC 0x00663840  Xbox 0x00088FE0
int CSWCMessage::HandleServerToPlayerDebugInfo_Creature(unsigned long oidObject)
{
    CExoString asEquippedItems[CSWInventory::TOTAL_SLOT_COUNT];
    CExoString sMessage;
    CExoString sLABEL;
    CExoString sTag;
    CExoString sFaction;
    CResRef cConversation;
    CExoString sAreaTag;
    char szBuffer[64];

    sTag = ReadCExoString(32);
    sFaction = ReadCExoString(32);
    cConversation = ReadCResRef(16);
    short nCurrentHP = ReadSHORT(16);
    short nMaxHP = ReadSHORT(16);
    short nArmorClass = ReadSHORT(16);
    int nAILevel = ReadINT(32);
    float fChallengeRating = ReadFLOAT(1.0f, 32);
    sAreaTag = ReadCExoString(32);
    unsigned long nEquipmentMask = ReadDWORD(32);
    int nEquippedItems = 0;
    unsigned long i;
    for (i = 0; i < CSWInventory::TOTAL_SLOT_COUNT; i++)
    {
        if (nEquipmentMask & (1 << i))
        {
            asEquippedItems[i] = ReadCExoString(32);
            nEquippedItems++;
        }
    }
    unsigned long nRepositoryItems = ReadDWORD(32);
    CExoString* psRepositoryItems = NULL;
    if (nRepositoryItems > 0)
    {
        psRepositoryItems = new CExoString[nRepositoryItems];
        if (psRepositoryItems == NULL)
            return 0;
    }
    for (i = 0; i < nRepositoryItems; i++)
        psRepositoryItems[i] = ReadCExoString(32);
    if (MessageReadOverflow(1))
        return 0;

    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidObject);
    if (pCreature == NULL)
        return 0;
    CSWCCreatureAppearance* pAppearance = pCreature->GetAppearance();
    CSWCCreatureStats* pStats = pCreature->GetStats();
    CSWCArea* pArea = pCreature->m_pArea;
    if (pAppearance == NULL || pStats == NULL || pArea == NULL)
        return 0;
    CGuiInGame* pGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
    if (pGui == NULL)
        return 0;

    pGui->AppendToMsgBuffer("******************",
        CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    sMessage = "FirstName: ";
    sMessage = sMessage + pStats->m_sFirstName;
    sMessage = sMessage + "    LastName: ";
    sMessage = sMessage + pStats->m_sLastName;
    sMessage = sMessage + "    Tag: ";
    sMessage = sMessage + sTag;
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("FirstName: %s\n", pStats->m_sFirstName.CStr());
    EXOLOG("LastName: %s\n", pStats->m_sLastName.CStr());
    EXOLOG("Tag: %s\n", sTag.CStr());

    sMessage = "Conversation File: ";
    sMessage = sMessage + (char*)cConversation.GetResRef();
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("Conversation File: %s\n", cConversation.GetResRef());
    {
        CExoString sColumn = "LABEL";
        C2DA* pAppearanceTable = g_pRules->Get2DArrays()->m_pAppearance;
        pAppearanceTable->GetCExoStringEntry(
            pAppearance->m_nAppearance, sColumn, &sLABEL);
    }
    sMessage = "Appearance: ";
    sMessage = sMessage + sLABEL;
    sMessage = sMessage + "    Race: ";
    sMessage = sMessage + pStats->GetRaceString();
    sMessage = sMessage + "    Class: ";
    sMessage = sMessage + pStats->GetClassesString(' ');
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("Appearance: %s\n", sLABEL.CStr());
    EXOLOG("Race: %s\n", pStats->GetRaceString().CStr());
    EXOLOG("Class: %s\n", pStats->GetClassesString(' ').CStr());

    sMessage = "Faction: ";
    sMessage = sMessage + sFaction;
    sMessage = sMessage + "    AI Level: ";
    sMessage = sMessage + CExoString(nAILevel);
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("Faction: %s\n", sFaction.CStr());
    EXOLOG("AI Level: %s\n", CExoString(nAILevel));

    sMessage = "CurrentHP: ";
    sMessage = sMessage + CExoString(nCurrentHP);
    sMessage = sMessage + "    MaxHP: ";
    sMessage = sMessage + CExoString(nMaxHP);
    sMessage = sMessage + "    AC: ";
    sMessage = sMessage + CExoString(nArmorClass);
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("CurrentHP: %s\n", CExoString(pStats->GetCurrentHitPoints()).CStr());
    EXOLOG("MaxHP: %s\n", CExoString(pStats->GetMaxHitPoints()).CStr());
    EXOLOG("AC: %s\n", CExoString(pStats->GetArmorClass(0)).CStr());

    Vector vPosition = pCreature->m_vPosition;
    sprintf(szBuffer, "%.2f, %.2f, %.2f", vPosition.m_fX,
        vPosition.m_fY, vPosition.m_fZ);
    sMessage = "Area Name: ";
    sMessage = sMessage + pArea->m_sAreaName;
    sMessage = sMessage + "    Area Tag: ";
    sMessage = sMessage + sAreaTag;
    sMessage = sMessage + "    Pos: ";
    sMessage = sMessage + szBuffer;
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("%s\n", pArea->m_sAreaName.CStr());

    sMessage = "EquippedItems: ";
    sMessage = sMessage + CExoString(nEquippedItems);
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("EquippedItems: %s\n", CExoString(nEquippedItems).CStr());
    if (nEquipmentMask & INVENTORY_SLOT_HEAD)
    {
        sMessage = "    Head: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_HEAD];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("Head: %s\n", asEquippedItems[CSWInventory::SLOT_HEAD].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_ARMOUR)
    {
        sMessage = "    Chest: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_ARMOUR];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("Chest: %s\n", asEquippedItems[CSWInventory::SLOT_ARMOUR].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_BOOTS)
    {
        sMessage = "    Boots: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_BOOTS];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("Boots: %s\n", asEquippedItems[CSWInventory::SLOT_BOOTS].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_HANDS)
    {
        sMessage = "    Arms: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_HANDS];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("Arms: %s\n", asEquippedItems[CSWInventory::SLOT_HANDS].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_RIGHTHAND)
    {
        sMessage = "    RightHand: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_RIGHTHAND];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("RightHand: %s\n", asEquippedItems[CSWInventory::SLOT_RIGHTHAND].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_LEFTHAND)
    {
        sMessage = "    LeftHand: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_LEFTHAND];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("LeftHand: %s\n", asEquippedItems[CSWInventory::SLOT_LEFTHAND].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_LEFTRING)
    {
        sMessage = "    Cloak: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_LEFTRING];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("Cloak: %s\n", asEquippedItems[CSWInventory::SLOT_LEFTRING].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_NECK)
    {
        sMessage = "    RightForeArm: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_NECK];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("RightForeArm: %s\n", asEquippedItems[CSWInventory::SLOT_NECK].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_RIGHTRING)
    {
        sMessage = "    LeftForeArm: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_RIGHTRING];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("LeftForeArm: %s\n", asEquippedItems[CSWInventory::SLOT_RIGHTRING].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_BELT)
    {
        sMessage = "    Neck: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_BELT];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("Neck: %s\n", asEquippedItems[CSWInventory::SLOT_BELT].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_ARMS)
    {
        sMessage = "    Belt: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_ARMS];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("Belt: %s\n", asEquippedItems[CSWInventory::SLOT_ARMS].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_CREATUREWEAPON1)
    {
        sMessage = "    CWeapon1: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_CREATUREWEAPON1];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("CWeapon1: %s\n", asEquippedItems[CSWInventory::SLOT_CREATUREWEAPON1].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_CREATUREWEAPON2)
    {
        sMessage = "    CWeapon2: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_CREATUREWEAPON2];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("CWeapon2: %s\n", asEquippedItems[CSWInventory::SLOT_CREATUREWEAPON2].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_CREATUREWEAPON3)
    {
        sMessage = "    CWeapon3: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_CREATUREWEAPON3];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("CWeapon3: %s\n", asEquippedItems[CSWInventory::SLOT_CREATUREWEAPON3].CStr());
    }
    if (nEquipmentMask & INVENTORY_SLOT_CREATUREARMOUR)
    {
        sMessage = "    CArmor: ";
        sMessage = sMessage + asEquippedItems[CSWInventory::SLOT_CREATUREARMOUR];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("CArmor: %s\n", asEquippedItems[CSWInventory::SLOT_CREATUREARMOUR].CStr());
    }

    sMessage = "RepositoryItems: ";
    sMessage = sMessage + CExoString(nRepositoryItems);
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("RepositoryItems: %s\n", CExoString(nRepositoryItems).CStr());
    for (i = 0; i < nRepositoryItems; i++)
    {
        sMessage = "    ";
        sMessage = sMessage + psRepositoryItems[i];
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        EXOLOG("%s\n", psRepositoryItems[i].CStr());
    }
    sMessage = "KnownSpells: ";
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    for (unsigned char nClass = 0; nClass < pStats->GetNumClasses(); nClass++)
    {
        CSWClass* pClass = &g_pRules->m_pClasses[pStats->GetClass(nClass)];
        if (pClass != NULL)
        {
            sMessage = "    Class: ";
            sMessage = sMessage + pClass->GetNameText();
            pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
            EXOLOG("Class: %s\n", pClass->GetNameText().CStr());
        for (unsigned char nLevel = 0;
             nLevel < CSWCCreatureStats_ClassInfo::SPELL_LEVEL_COUNT; nLevel++)
            {
                unsigned short nSpells = pStats->GetNumberKnownSpells(nClass, nLevel);
            if (nSpells >= 1)
                {
                    sMessage = "        Level: ";
                    sMessage = sMessage + CExoString(nLevel);
                    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
                    EXOLOG("Level: %s\n", CExoString(nLevel).CStr());
                    if (CSWClass::IsJedi(pStats->GetClass(nClass)))
                    {
                        sMessage = "        SpellsPerDayLeft: ";
                        sMessage = sMessage + CExoString(pStats->GetSpellsPerDayLeft(nClass, nLevel));
                        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
                        EXOLOG("SpellsPerDayLeft: %s\n", CExoString(pStats->GetSpellsPerDayLeft(nClass, nLevel)).CStr());
                    }
                    for (i = 0; i < nSpells; i++)
                    {
                        unsigned long nKnownSpell = pStats->GetKnownSpell(nClass, nLevel, i);
                        if (g_pRules->m_pSpellArray->GetSpell(nKnownSpell) != NULL)
                        {
                            sMessage = "            ";
                            sMessage = sMessage + g_pRules->m_pSpellArray->GetSpell(nKnownSpell)->GetSpellNameText();
                            pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
                            EXOLOG("%s\n", g_pRules->m_pSpellArray->GetSpell(nKnownSpell)->GetSpellNameText().CStr());
                        }
                    }
                }
            }
        }
    }
    sprintf(szBuffer, "%.2f", fChallengeRating);
    sMessage = "ChallengeRating: ";
    sMessage = sMessage + szBuffer;
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    pGui->AppendToMsgBuffer("******************",
        CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    if (psRepositoryItems != NULL)
        delete[] psRepositoryItems;
    return 1;
}
// PC 0x00665510  Xbox 0x0008ACB0
int CSWCMessage::HandleServerToPlayerAmbient(unsigned char nMinor)
{
    int bSuccess;

    switch (nMinor)
    {
    case 1:
        bSuccess = HandleServerToPlayerAmbientMusicPlay();
        break;
    case 2:
        bSuccess = HandleServerToPlayerAmbientMusicSetDelay();
        break;
    case 3:
        bSuccess = HandleServerToPlayerAmbientMusicChangeTrack();
        break;
    case 4:
        bSuccess = HandleServerToPlayerAmbientBattleMusicPlay();
        break;
    case 5:
        bSuccess = HandleServerToPlayerAmbientBattleMusicChange();
        break;
    case 6:
        bSuccess = HandleServerToPlayerAmbientSoundLoopPlay();
        break;
    case 7:
        bSuccess = HandleServerToPlayerAmbientSoundLoopChange();
        break;
    case 8:
        bSuccess = HandleServerToPlayerAmbientSoundVolume();
        break;
    default:
        bSuccess = 0;
        break;
    }
    return bSuccess;
}
// PC 0x00665590  Xbox 0x0008AD30
int CSWCMessage::HandleServerToPlayerGuiTimingEvent(unsigned char nMinor)
{
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nMinor)
    {
    case 1:
        HandleServerToPlayerGuiTimingEvent_Info();
        break;
    }
    return !MessageReadOverflow(1) && MessageReadUnderflow(1) == 0;
}
// PC 0x006655E0  Xbox 0x0008AE70
int CSWCMessage::HandleServerToPlayerCreatureUpdate_Update(
    unsigned long oidObject, unsigned long nUpdateMask)
{
    CResRef cPortrait;
    Vector vPosition;
    Vector vOrientation;
    unsigned short nAnimation = 0;
    int bCustomSpeed = 0;
    unsigned long oidPlayer;
    float fSpeed = 1.0f;
    unsigned char nAttackCount = 0;
    Vector vTargetPosition;
    unsigned long aoidAttackObjects[3];
    unsigned short anAttackDamageAmount[3];
    unsigned short anAttackCriticalMultiplier[3];
    unsigned short anAttackRoll[3];
    unsigned char anAttackType[3];
    unsigned short anAttackFlags[3];
    unsigned short anAttackDamage[3];
    int abAttackVisual[3];
    int abAttackImpact[3];
    unsigned char anAttackWeapon[3];
    unsigned long oidLockTarget;
    unsigned long nSpell;
    unsigned long nSpellFlags;
    unsigned long oidSpellTarget;
    unsigned char nSpellTargetType;
    unsigned short nWayPointCount;

    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidObject);
    CSWCCreatureStats* pStats;
    if (pCreature != NULL)
        pStats = pCreature->GetStats();
    else
        pStats = NULL;

    int bPosition = nUpdateMask & CREATURE_UPDATE_POSITION;
    if (bPosition)
    {
        vPosition.m_fX = ReadFLOAT(1.0f, 32);
        vPosition.m_fY = ReadFLOAT(1.0f, 32);
        vPosition.m_fZ = ReadFLOAT(1.0f, 32);
    }
    else if (pCreature != NULL)
        vPosition = pCreature->m_vPosition;

    int bOrientation = nUpdateMask & CREATURE_UPDATE_ORIENTATION;
    if (bOrientation)
    {
        vOrientation.m_fX = ReadFLOAT(1.0f, 32);
        vOrientation.m_fY = ReadFLOAT(1.0f, 32);
        vOrientation.m_fZ = ReadFLOAT(1.0f, 32);
        if (ReadBYTE(8))
            oidLockTarget = ReadOBJECTIDClient();
        else
            oidLockTarget = CGameObjectArray::INVALIDOBJID;
        if (vOrientation.m_fZ * vOrientation.m_fZ +
            vOrientation.m_fY * vOrientation.m_fY +
            vOrientation.m_fX * vOrientation.m_fX < 0.00001f)
            vOrientation.m_fX = 1.0f;
    }
    else if (pCreature != NULL)
        vOrientation = pCreature->m_vOrientation;

    if (nUpdateMask & CREATURE_UPDATE_PORTRAIT)
    {
        CExoString sPrefix;
        CExoString sPortrait;
        unsigned short nPortrait = ReadWORD(16);
        if (MessageReadOverflow(1))
            return 0;
        if (pCreature != NULL)
            pCreature->SetPortraitId(nPortrait);
        if (nPortrait >= PORTRAIT_ID_FIRST_RESERVED)
        {
            cPortrait = ReadCResRef(16);
            if (MessageReadOverflow(1))
                return 0;
            if (pCreature != NULL)
            {
                CSWCCreatureStats::CheckCustomPortrait(&cPortrait,
                    nPortrait == PORTRAIT_ID_RESERVED_FFFE);
                pCreature->SetPortrait(cPortrait);
            }
        }
        else
        {
            sPrefix = "po_";
            {
                CExoString sColumn("BaseResRef");
                C2DA* pPortraits = g_pRules->Get2DArrays()->m_pPortrait;
                pPortraits->GetCExoStringEntry(nPortrait, sColumn, &sPortrait);
            }
            sPortrait = sPrefix + sPortrait;
            cPortrait = sPortrait.CStr();
        }
    }

    if (pCreature != NULL)
    {
        nAnimation = pCreature->GetCurrentAnimation();
    }
    int bAnimation = nUpdateMask & CREATURE_UPDATE_ANIMATION;
    if (bAnimation)
    {
        float fAnimationSpeed = ReadFLOAT(1.0f, 32);
        if (MessageReadOverflow(1))
            return 0;
        if (pCreature != NULL)
            pCreature->m_fAnimationSpeed = fAnimationSpeed;
    }
    int bSetAnimation = 1;
    if (bAnimation)
    {
        nAnimation = ReadWORD(16);
        if (nAnimation == 10009 || nAnimation == CREATURE_ANIMATION_MELEE_SEQUENCE)
        {
            nAttackCount = ReadBYTE(2);
            for (int i = 0; i < nAttackCount; ++i)
            {
                aoidAttackObjects[i] = ReadOBJECTIDClient();
                anAttackDamageAmount[i] = ReadWORD(16);
                anAttackCriticalMultiplier[i] = ReadWORD(16);
                anAttackRoll[i] = ReadWORD(16);
                anAttackType[i] = ReadBYTE(4);
                anAttackFlags[i] = ReadWORD(16);
                anAttackDamage[i] = ReadWORD(9);
                abAttackVisual[i] = ReadBOOL();
                abAttackImpact[i] = ReadBOOL();
                anAttackWeapon[i] = ReadBYTE(4);
            }
            if (MessageReadOverflow(1))
                return 0;
        }
        if (nAnimation == CREATURE_ANIMATION_CONJURE_1 ||
            nAnimation == CREATURE_ANIMATION_CONJURE_2 ||
            nAnimation == CREATURE_ANIMATION_CONJURE_3 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_1 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_2 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_3 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_4 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_5 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_11)
        {
            nSpell = ReadDWORD(32);
            unsigned long oidConjuringTarget = ReadOBJECTIDClient();
            if (MessageReadOverflow(1))
                return 0;
            if (pCreature != NULL)
            {
                pCreature->m_nConjuringSpell = nSpell;
                pCreature->m_oidUnknown2C8 = oidConjuringTarget;
            }
        }
        if (nAnimation == CREATURE_ANIMATION_SPELLCAST_1 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_2 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_3 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_4 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_5 ||
            nAnimation == CREATURE_ANIMATION_SPELLCAST_11)
        {
            nSpellTargetType = ReadBYTE(8);
            if (nSpellTargetType == 1)
                oidSpellTarget = ReadOBJECTIDClient();
            else if (nSpellTargetType == 2)
            {
                oidSpellTarget = OBJECT_INVALID;
                float fX = ReadFLOAT(1.0f, 32);
                float fY = ReadFLOAT(1.0f, 32);
                float fZ = ReadFLOAT(1.0f, 32);
                vTargetPosition = Vector(fX, fY, fZ);
            }
            nSpellFlags = ReadDWORD(32);
            if (MessageReadOverflow(1))
                return 0;
        }
    }
    else if (pCreature != NULL)
    {
        bSetAnimation = 0;
        nAnimation = pCreature->GetCurrentAnimation();
    }

    if ((nUpdateMask & CREATURE_UPDATE_VISUAL_EFFECTS) &&
        !HandleServerToPlayerUpdateVisualEffects(pCreature))
        return 0;

    unsigned char nWayPointSequence;
    int bSkipMovement = 0;
    if (nUpdateMask & CREATURE_UPDATE_ANIMATION)
    {
        nWayPointSequence = ReadBYTE(8);
        if (pCreature != NULL)
            pCreature->m_nUnknown1F0 = nWayPointSequence;
        else
            bSkipMovement = 1;
    }
    else
    {
        nWayPointSequence = 0;
        if (pCreature == NULL)
            bSkipMovement = 1;
    }

    unsigned short nOldAnimation;
    if (!bSkipMovement && pCreature != NULL)
    {
        if (bOrientation)
        {
            pCreature->SetDesiredOrientation(vOrientation);
            if (pCreature->m_nUnknown214 &&
                pCreature->GetObjectId() != g_pAppManager->GetClientExoApp()->GetPlayerCreatureId())
                pCreature->SetMoveOrientation(vOrientation);
            pCreature->SetLockOrientationToObject(oidLockTarget, 0);
        }
        nOldAnimation = pCreature->GetCurrentAnimation();
        if (!pCreature->AnimationStationary(nOldAnimation) &&
            pCreature->AnimationStationary(nAnimation) == 1 &&
            pCreature->AnimationLooping(nAnimation) == 1 &&
            nAnimation != CREATURE_ANIMATION_AMBIENT_4 &&
            nAnimation != CREATURE_ANIMATION_AMBIENT_3 &&
            nAnimation != CREATURE_ANIMATION_AMBIENT_14 &&
            nAnimation != CREATURE_ANIMATION_DIALOG_SPEAK &&
            nAnimation != CREATURE_ANIMATION_DIALOG_SPEAK_INJURED &&
            nAnimation != CREATURE_ANIMATION_DIALOG_LISTEN &&
            nAnimation != CREATURE_ANIMATION_DIALOG_LISTEN_INJURED &&
            nAnimation != 10039 && nAnimation != 10040 &&
            nAnimation != 10041 && nAnimation != 10042)
        {
            pCreature->SetLoopingAnimation(nAnimation);
            Vector* pEnd = pCreature->GetEndWayPoint();
            if (pEnd != NULL)
            {
                if (!bPosition)
                    bSkipMovement = 1;
                else
                {
                    Vector vDelta = *pEnd - vPosition;
                    if (vDelta.m_fY * vDelta.m_fY + vDelta.m_fX * vDelta.m_fX <= 0.00001f)
                        bSkipMovement = 1;
                }
            }
        }
    }

    if (!bSkipMovement)
    {
        if ((pCreature != NULL &&
             pCreature->AnimationStationary(nOldAnimation) == 1 &&
             pCreature->AnimationStationary(nAnimation) == 0 &&
             pCreature->m_vPosition * pCreature->m_vPosition <= 0.00001f) ||
            pCreature->m_bVisibilityUpdated == 1)
        {
            if (bPosition)
            {
                pCreature->SetPosition(vPosition);
                pCreature->m_vDesiredPosition = vPosition;
            }
            pCreature->m_bVisibilityUpdated = 0;
        }
        if (!pCreature->AnimationFireAndForget(nAnimation))
        {
            if (bSetAnimation)
                pCreature->SetAnimation(nAnimation, 0, 0);
        }
        else
        {
            if (bAnimation)
            {
                if (nAnimation == 10009 || nAnimation == CREATURE_ANIMATION_MELEE_SEQUENCE)
                {
                    CSWCCombatAttackData* pFirst = new CSWCCombatAttackData;
                    CSWCCombatAttackData* pAttack = pFirst;
                    for (int i = 0; i < nAttackCount; ++i)
                    {
                        pAttack->m_oidCreature = oidObject;
                        pAttack->m_nAttackRoll = anAttackRoll[i];
                        pAttack->m_nAttackResult = anAttackType[i];
                        pAttack->m_nAttackFlags = anAttackFlags[i];
                        pAttack->m_oidAttackObject = aoidAttackObjects[i];
                        pAttack->m_nDamageAmount = anAttackDamageAmount[i];
                        pAttack->m_nCriticalMultiplier = anAttackCriticalMultiplier[i];
                        pAttack->m_nDamage = anAttackDamage[i];
                        pAttack->m_bHit = 1;
                        pAttack->m_nImpactVisual = abAttackVisual[i];
                        pAttack->m_nImpactX = abAttackImpact[i];
                        pAttack->m_nImpactY = anAttackWeapon[i];
                        if (i < nAttackCount - 1)
                        {
                            pAttack->m_pNextAttack = new CSWCCombatAttackData;
                            pAttack = pAttack->m_pNextAttack;
                        }
                    }
                    pCreature->AddFireForgetAnimation(nAnimation, pFirst);
                }
                else if (nAnimation == CREATURE_ANIMATION_SPELLCAST_1 ||
                         nAnimation == CREATURE_ANIMATION_SPELLCAST_2 ||
                         nAnimation == CREATURE_ANIMATION_SPELLCAST_3 ||
                         nAnimation == CREATURE_ANIMATION_SPELLCAST_4 ||
                         nAnimation == CREATURE_ANIMATION_SPELLCAST_5 ||
                         nAnimation == CREATURE_ANIMATION_SPELLCAST_11)
                {
                    if (nAnimation == CREATURE_ANIMATION_SPELLCAST_1)
                        pCreature->SetAnimation(CREATURE_ANIMATION_SPELLCAST_6, 0, 0);
                    else if (nAnimation == CREATURE_ANIMATION_SPELLCAST_2)
                        pCreature->SetAnimation(CREATURE_ANIMATION_SPELLCAST_7, 0, 0);
                    else if (nAnimation == CREATURE_ANIMATION_SPELLCAST_3)
                        pCreature->SetAnimation(CREATURE_ANIMATION_SPELLCAST_8, 0, 0);
                    else if (nAnimation == CREATURE_ANIMATION_SPELLCAST_4)
                        pCreature->SetAnimation(CREATURE_ANIMATION_SPELLCAST_9, 0, 0);
                    else if (nAnimation == CREATURE_ANIMATION_SPELLCAST_5)
                        pCreature->SetAnimation(CREATURE_ANIMATION_SPELLCAST_10, 0, 0);
                    else if (nAnimation == CREATURE_ANIMATION_SPELLCAST_11)
                        pCreature->SetAnimation(CREATURE_ANIMATION_SPELLCAST_11, 0, 0);
                    CSWCCombatAttackData* pSpell = new CSWCCombatAttackData;
                    pSpell->m_nAttackState = nSpellTargetType;
                    pSpell->m_oidAttackObject = oidSpellTarget;
                    pSpell->m_vImpactPosition = vTargetPosition;
                    pSpell->m_nAttackIndex = nSpell;
                    pSpell->m_bHit = 4;
                    pSpell->m_nWeaponIndex = nSpellFlags;
                    pCreature->AddFireForgetAnimation(nAnimation, pSpell);
                }
            }
        }

        if (!pCreature->AnimationStationary(nAnimation))
        {
            if (bAnimation)
            {
                nWayPointCount = ReadWORD(16);
                if (nWayPointCount > 0)
                {
                    bCustomSpeed = ReadBOOL();
                    if (bCustomSpeed)
                        fSpeed = ReadFLOAT(1.0f, 32);
                }
            }
            else
            {
                unsigned short nOldWayPointCount;
                Vector* pOldWayPoints;
                unsigned short nCurrentWayPoint;
                pCreature->GetWayPoints(&nOldWayPointCount, &pOldWayPoints, &nCurrentWayPoint);
                nWayPointCount = nOldWayPointCount;
            }
            if (MessageReadOverflow(1))
                return 0;
        }
        else
        {
            if (bAnimation)
            {
                nWayPointCount = ReadWORD(16);
                if (nWayPointCount > 0)
                {
                    bCustomSpeed = ReadBOOL();
                    if (bCustomSpeed)
                        fSpeed = ReadFLOAT(1.0f, 32);
                }
            }
            if ((!bCustomSpeed && !pCreature->m_nUnknown214) ||
                !nWayPointSequence || pCreature->m_nUnknown1EF == nWayPointSequence)
            {
                pCreature->SetWayPoints(0, NULL);
                nWayPointCount = 0;
            }
        }
        if (nWayPointCount > 0)
        {
            pCreature->m_nUnknown214 = bCustomSpeed || pCreature->m_nUnknown214;
            pCreature->m_fUnknown1FC = fSpeed;
        }
        Vector vGroundNormal;
        if (!pCreature->AnimationStationary(nAnimation) && nWayPointCount > 0)
        {
            if (bAnimation)
            {
                const unsigned short nReceivedWayPointCount = nWayPointCount;
                Vector* pWayPoints = new Vector[nReceivedWayPointCount];
                if (pWayPoints == NULL)
                    return 0;
                for (int i = 0; i < nReceivedWayPointCount; ++i)
                {
                    pWayPoints[i].m_fX = ReadFLOAT(1.0f, 32);
                    pWayPoints[i].m_fY = ReadFLOAT(1.0f, 32);
                    pWayPoints[i].m_fZ = ReadFLOAT(1.0f, 32);
                    if (MessageReadOverflow(1))
                        return 0;
                }
                int bEvaluateWayPoints = 1;
                Vector* pEnd = pCreature->GetEndWayPoint();
                if (pEnd != NULL)
                {
                    Vector vLast = pWayPoints[nReceivedWayPointCount - 1];
                    Vector vDifference = *pEnd - vLast;
                    if (vDifference.m_fY * vDifference.m_fY +
                        vDifference.m_fX * vDifference.m_fX <= 0.00001f)
                    {
                        bEvaluateWayPoints = 0;
                        Vector vDifference2;
                        vDifference2 = pCreature->m_vPosition - vLast;
                        if (vDifference2.m_fY * vDifference2.m_fY +
                            vDifference2.m_fX * vDifference2.m_fX <= 0.00001f)
                            pCreature->SetAnimation(pCreature->GetLoopingAnimation(), 0, 0);
                    }
                }
                if (bEvaluateWayPoints)
                    pCreature->EvaluateNewWayPoints(nReceivedWayPointCount, pWayPoints,
                                                    nWayPointSequence, vPosition);
                delete[] pWayPoints;
            }
            if (bOrientation && oidLockTarget == CGameObjectArray::INVALIDOBJID)
                pCreature->SetLockOrientationToObject(oidLockTarget, 0);
        }
        else if (!pCreature->m_nUnknown214 || !nWayPointSequence ||
                 pCreature->m_nUnknown1EF == nWayPointSequence)
        {
            if (pCreature->m_pArea != NULL)
                pCreature->ComputeZValue(pCreature->m_pArea, &vPosition,
                                         vPosition.m_fZ, &vGroundNormal);
            if (bPosition)
            {
                Vector vDifference = pCreature->m_vDesiredPosition - vPosition;
                if (vDifference * vDifference > 0.00001f)
                {
                    pCreature->m_vDesiredPosition = vPosition;
                    pCreature->ResetDesiredPositionTimer();
                    pCreature->SetGroundNormal(vGroundNormal);
                }
                vDifference = pCreature->m_vPosition - vPosition;
                if (pCreature->m_vPosition * pCreature->m_vPosition <= 0.00001f ||
                    vDifference * vDifference >= 36.0f)
                {
                    pCreature->SetPosition(vPosition);
                    pCreature->SetGroundNormal(vGroundNormal);
                    if (bOrientation)
                        pCreature->SetOrientation(vOrientation);
                }
            }
        }
    }
    else
    {
        if (bAnimation)
        {
            nWayPointCount = ReadWORD(16);
            if (nWayPointCount > 0)
            {
                bCustomSpeed = ReadBOOL();
                if (bCustomSpeed)
                    fSpeed = ReadFLOAT(1.0f, 32);
                if (pCreature != NULL)
                {
                    pCreature->m_nUnknown214 = bCustomSpeed || pCreature->m_nUnknown214;
                    pCreature->m_fUnknown1FC = fSpeed;
                }
            }
        }
        if ((nUpdateMask & CREATURE_UPDATE_ANIMATION) && nWayPointCount > 0)
            for (int i = nWayPointCount; i > 0; --i)
            {
                ReadFLOAT(1.0f, 32);
                ReadFLOAT(1.0f, 32);
                ReadFLOAT(1.0f, 32);
            }
    }

    if (nUpdateMask & CREATURE_UPDATE_COMBAT_STATE)
    {
        unsigned short nOverlayState = ReadWORD(16);
        unsigned char nReadyState = ReadBYTE(8);
        unsigned short nMovementFlags = ReadWORD(16);
        unsigned char nHostile = ReadBYTE(8);
        unsigned char nStealth = ReadBYTE(8);
        int bSuppressAnimation = ReadBOOL();
        int bCombatState = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        unsigned long oidTarget = nReadyState == 2 ? ReadOBJECTIDClient() : OBJECT_INVALID;
        if (MessageReadOverflow(1))
            return 0;
        if (pCreature != NULL)
        {
            if (nStealth == 1)
            {
                if (pCreature->m_nStealthState == 0)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(85, pMessage, "");
                    if (pCreature->GetObjectId() == g_pAppManager->GetClientExoApp()->GetPlayerCreatureId())
                    {
                        CResRef cSound("sdr_invisible");
                        g_pExoSound->Play3DOneShotSound(cSound, pCreature->m_vPosition, 0.0f,
                            CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
                    }
                }
                pCreature->SetStealthState(1, bSuppressAnimation);
            }
            else if (nStealth == 0)
            {
                if (pCreature->m_nStealthState == 1)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(86, pMessage, "");
                    if (pCreature->GetObjectId() == g_pAppManager->GetClientExoApp()->GetPlayerCreatureId())
                    {
                        CResRef cSound("sdr_invisible");
                        g_pExoSound->Play3DOneShotSound(cSound, pCreature->m_vPosition, 0.0f,
                            CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
                    }
                }
                pCreature->SetStealthState(0, bSuppressAnimation);
            }
        }
        if (!nOverlayState)
        {
            if (pCreature != NULL)
            {
                if (!g_pAppManager->GetClientExoApp()->GetPlayerByGameObjectID(pCreature->GetObjectId()) &&
                    !pCreature->m_bDeadSelectable)
                    g_pAppManager->GetClientExoApp()->AddObjectToHitcheckIgnoreList(pCreature->GetObjectId());
                pCreature->ComputeWeaponOverlays(0, 0);
                unsigned long oidItem = pCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND);
                CSWCItem* pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
                if (pItem != NULL)
                    pItem->PowerItem(0, 0, 1, 0);
                oidItem = pCreature->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND);
                pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
                if (pItem != NULL)
                    pItem->PowerItem(0, 0, 1, 0);
                if (g_pAppManager->GetClientExoApp()->GetSWParty()->IsPartyMember(pCreature->GetObjectId()) &&
                    g_pAppManager->GetClientExoApp()->GetSWParty()->GetNumCharacters() > 1)
                    g_pAppManager->GetClientExoApp()->ShowTutorialWindow(15, OBJECT_INVALID, OBJECT_INVALID, 0);
            }
        }
        else
        {
            if (pCreature != NULL)
                g_pAppManager->GetClientExoApp()->RemoveObjectFromHitcheckIgnoreList(pCreature->GetObjectId());
        }
        if (pCreature != NULL)
        {
            pCreature->m_nWeaponOverlayState = nOverlayState;
            pCreature->m_nReadyAnimationState = nReadyState;
            pCreature->m_nMovementFlags = nMovementFlags;
            pCreature->m_nHostile = nHostile;
            pCreature->m_oidCombatTarget = oidTarget;
            pCreature->SetCombatState(bCombatState);
            if (bCombatState && pCreature->m_bInParty &&
                g_pAppManager->GetClientExoApp()->GetClientOptions()->GetCameraMode() == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                g_pAppManager->GetClientExoApp()->GetModule()->RestoreCamera();
                g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GAME, 1);
            }
            g_pAppManager->GetClientExoApp()->GetInGameGui();
        }
    }
    if (nUpdateMask & CREATURE_UPDATE_SPEED)
    {
        float fWalkSpeed = ReadFLOAT(1.0f, 32);
        float fDriveSpeed = ReadFLOAT(1.0f, 32);
        if (MessageReadOverflow(1))
            return 0;
        if (pCreature != NULL)
        {
            pCreature->m_fWalkSpeed = fWalkSpeed;
            pCreature->m_fDriveSpeed = fDriveSpeed;
            pCreature->GetAppearance()->m_fDriveMaxSpeed = fDriveSpeed * 0.001f;
            if (pCreature->GetAppearance()->GetRunLooksAwfulRate() > fDriveSpeed)
                pCreature->SetRunLooksAwful(1);
            else
                pCreature->SetRunLooksAwful(0);
        }
    }
    if (nUpdateMask & CREATURE_UPDATE_UNUSED_OBJECT)
    {
        ReadOBJECTIDClient();
        if (MessageReadOverflow(1))
            return 0;
    }
    if (nUpdateMask & CREATURE_UPDATE_UNUSED_200)
    {
        ReadINT(10);
        ReadINT(10);
        if (MessageReadOverflow(1))
            return 0;
    }
    if (nUpdateMask & CREATURE_UPDATE_HIT_POINTS)
    {
        short nCurrent = ReadSHORT(16);
        short nBase = ReadSHORT(16);
        short nBonus = ReadSHORT(16);
        short nMax = ReadSHORT(16);
        if (MessageReadOverflow(1))
            return 0;
        if (pCreature != NULL)
        {
            pCreature->GetStats()->m_nHitPointsBase = nBase;
            pCreature->GetStats()->m_nHitPointBonus = nBonus;
            pCreature->GetStats()->m_nCurrentHitPoints = nCurrent;
            pCreature->GetStats()->m_nMaxHitPoints = nMax;
        }
    }
    if (nUpdateMask & CREATURE_UPDATE_FORCE_POINTS)
    {
        short nCurrent = ReadSHORT(16);
        short nBase = ReadSHORT(16);
        short nBonus = ReadSHORT(16);
        short nMax = ReadSHORT(16);
        if (MessageReadOverflow(1))
            return 0;
        CSWCCreature* pUpdatedCreature = g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidObject);
        if (pUpdatedCreature != NULL)
        {
            pUpdatedCreature->GetStats()->m_nForcePointsBase = nBase;
            pUpdatedCreature->GetStats()->m_nForcePointBonus = nBonus;
            pUpdatedCreature->GetStats()->m_nCurrentForcePoints = nCurrent;
            pUpdatedCreature->GetStats()->m_nMaxForcePoints = nMax;
        }
    }
    if (nUpdateMask & CREATURE_UPDATE_NAMES)
    {
        CExoString sFirstName = ReadCExoLocStringClient(CResRef(), NULL);
        CExoString sLastName = ReadCExoLocStringClient(CResRef(), NULL);
        if (MessageReadOverflow(1))
            return 0;
    }
    if (nUpdateMask & CREATURE_UPDATE_PARTY_STATUS)
    {
        int bStatus1 = ReadBOOL();
        int bStatus2 = ReadBOOL();
        int bStatus3 = ReadBOOL();
        int bStatus4 = ReadBOOL();
        int bStatus5 = ReadBOOL();
        unsigned long oidStatus = OBJECT_INVALID;
        CExoString sStatus;
        CExoString sUnusedStatus;
        if (bStatus5)
        {
            oidStatus = ReadOBJECTIDClient();
            sStatus = ReadCExoLocStringClient(CResRef(), NULL);
            sUnusedStatus = ReadCExoLocStringClient(CResRef(), NULL);
        }
        ReadBOOL();
        ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        if (pCreature != NULL)
        {
            if (pCreature->GetPartyStatus() == NULL)
                pCreature->m_pPartyStatus = new CSWCCreaturePartyStatus;
            pCreature->GetPartyStatus()->m_nUnknown00 = bStatus1;
            pCreature->GetPartyStatus()->m_nUnknown04 = bStatus2;
            pCreature->GetPartyStatus()->m_nUnknown08 = bStatus3;
            pCreature->GetPartyStatus()->m_nUnknown0C = bStatus4;
            pCreature->GetPartyStatus()->m_nUnknown10 = bStatus5;
            if (bStatus5)
            {
                pCreature->GetPartyStatus()->m_nUnknown14 = oidStatus;
                pCreature->GetPartyStatus()->m_sUnknown18 = sStatus;
            }
        }
    }
    if (nUpdateMask & CREATURE_UPDATE_PERCEPTION)
    {
        CExoArrayList<CSWVisibilityNode*>& lstVisibility = pCreature->m_lstVisibilityList;
        CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetPlayerCreature();
        if (pPlayer == NULL)
            oidPlayer = OBJECT_INVALID;
        else
            oidPlayer = pPlayer->m_nId;
        int bSeen = ReadBOOL();
        int bHeard = ReadBOOL();
        int bInvisible = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        if (pCreature != NULL)
        {
            CSWVisibilityNode* pNode;
            int bFound = 0;
            int nVisibilityCount = lstVisibility.num;
            for (int i = 0; i < nVisibilityCount; ++i)
            {
                pNode = lstVisibility.element[i];
                unsigned long oidNode = pNode->m_oidObject;
                if (oidNode == oidPlayer)
                {
                    bFound = 1;
                    break;
                }
            }
            if (oidPlayer == oidObject)
            {
                if (!bFound)
                {
                    pNode = new CSWVisibilityNode;
                    pNode->m_oidObject = oidPlayer;
                    pNode->m_bSeen = 1;
                    pNode->m_bHeard = 1;
                    pNode->m_bInvisible = bInvisible;
                    lstVisibility.Add(pNode);
                    pCreature->UpdateRelativePerception(pNode);
                }
                else
                {
                    pNode->m_bSeen = 1;
                    pNode->m_bHeard = 1;
                    pNode->m_bInvisible = bInvisible;
                    pCreature->UpdateRelativePerception(pNode);
                }
            }
            else if (!bFound)
            {
                while (lstVisibility.num > 0)
                {
                    CSWVisibilityNode* pOldNode =
                        lstVisibility.element[0];
                    if (pOldNode != NULL)
                        delete pOldNode;
                    lstVisibility.DelIndex(0);
                }
                if (oidPlayer != OBJECT_INVALID)
                {
                    pNode = new CSWVisibilityNode;
                    pNode->m_oidObject = oidPlayer;
                    pNode->m_bSeen = bSeen;
                    pNode->m_bHeard = bHeard;
                    pNode->m_bInvisible = bInvisible;
                    lstVisibility.Add(pNode);
                }
            }
            else
            {
                pNode->m_bSeen = bSeen;
                pNode->m_bHeard = bHeard;
                pNode->m_bInvisible = bInvisible;
            }
        }
    }
    if (nUpdateMask & CREATURE_UPDATE_STATES)
    {
        unsigned char nStates = ReadBYTE(8);
        if (MessageReadOverflow(1))
            return 0;
        if (pCreature != NULL)
        {
            if (nStates & 1)
            {
                if (pCreature->m_nUnknown190 == 0)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(83, pMessage, "");
                }
                pCreature->m_nUnknown190 = 1;
            }
            else if (!(nStates & 1))
            {
                if (pCreature->m_nUnknown190 == 1)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(84, pMessage, "");
                }
                pCreature->m_nUnknown190 = 0;
            }
            if (nStates & 2)
            {
                if (pCreature->m_nStealthState == 0)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(85, pMessage, "");
                }
                pCreature->SetStealthState(1, 0);
            }
            else if (!(nStates & 2))
            {
                if (pCreature->m_nStealthState == 1)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(86, pMessage, "");
                }
                pCreature->SetStealthState(0, 0);
            }
            if (nStates & 4)
            {
                if (pCreature->m_nUnknown198 == 0)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(87, pMessage, "");
                }
                pCreature->m_nUnknown198 = 1;
            }
            else if (!(nStates & 4))
            {
                if (pCreature->m_nUnknown198 == 1)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(88, pMessage, "");
                }
                pCreature->m_nUnknown198 = 0;
            }
            if (nStates & 8)
            {
                if (pCreature->m_nUnknown19C == 0)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(89, pMessage, "");
                }
                pCreature->m_nUnknown19C = 1;
            }
            else if (!(nStates & 8))
            {
                if (pCreature->m_nUnknown19C == 1)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(90, pMessage, "");
                }
                pCreature->m_nUnknown19C = 0;
            }
            if (nStates & 16)
            {
                if (pCreature->m_nUnknown1A0 == 0)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(91, pMessage, "");
                }
                pCreature->m_nUnknown1A0 = 1;
            }
            else if (!(nStates & 16))
            {
                if (pCreature->m_nUnknown1A0 == 1)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(92, pMessage, "");
                }
                pCreature->m_nUnknown1A0 = 0;
            }
            if (nStates & 64)
            {
                if (pCreature->m_nUnknown1A8 == 0)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(166, pMessage, "");
                }
                pCreature->m_nUnknown1A8 = 1;
            }
            else if (!(nStates & 64))
            {
                if (pCreature->m_nUnknown1A8 == 1)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(167, pMessage, "");
                }
                pCreature->m_nUnknown1A8 = 0;
            }
            if (nStates & 32)
            {
                if (pCreature->m_nUnknown1AC == 0)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(168, pMessage, "");
                }
                pCreature->m_nUnknown1AC = 1;
            }
            else if (!(nStates & 32))
            {
                if (pCreature->m_nUnknown1AC == 1)
                {
                    CSWCCMessageData* pMessage = new CSWCCMessageData;
                    pMessage->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(169, pMessage, "");
                }
                pCreature->m_nUnknown1AC = 0;
            }
        }
    }
    if (nUpdateMask & CREATURE_UPDATE_STATS_BYTE)
    {
        unsigned char nValue = ReadBYTE(8);
        if (MessageReadOverflow(1))
            return 0;
        if (pCreature != NULL)
            pCreature->GetStats()->m_aUnknown52[0] = nValue;
    }
    if (nUpdateMask & CREATURE_UPDATE_CLASSES)
    {
        CExoString sSubRace;
        unsigned short nRace = ReadWORD(16);
        sSubRace = ReadCExoString(32);
        unsigned char nUnknown = ReadBYTE(8);
        unsigned char nGender = ReadBYTE(8);
        short nAlignment = ReadSHORT(16);
        int bDeadSelectable = ReadBOOL();
        unsigned char nClasses = ReadBYTE(8);
        if (MessageReadOverflow(1))
            return 0;
        if (pCreature != NULL)
        {
            pStats->m_nRace = nRace;
            pStats->m_sSubRace = sSubRace;
            pStats->m_nSubraceIndex = nUnknown;
            pStats->m_nGender = nGender;
            pCreature->SetAlignmentGoodEvil(nAlignment);
            pCreature->UpdateFloatyText();
            pCreature->m_bDeadSelectable = bDeadSelectable;
            if (bDeadSelectable == 1)
                g_pAppManager->GetClientExoApp()->RemoveObjectFromHitcheckIgnoreList(pCreature->GetObjectId());
            else if (!g_pAppManager->GetClientExoApp()->GetPlayerByGameObjectID(pCreature->GetObjectId()) &&
                     !pCreature->m_nWeaponOverlayState)
                g_pAppManager->GetClientExoApp()->AddObjectToHitcheckIgnoreList(pCreature->GetObjectId());
            pStats->m_nClassCount = nClasses;
        }
        for (int i = 0; i < nClasses; ++i)
        {
            unsigned char nClass = ReadBYTE(8);
            unsigned char nLevel = ReadBYTE(8);
            if (MessageReadOverflow(1))
                return 0;
            if (pCreature != NULL)
            {
                pStats->SetClass(i, nClass);
                pStats->SetClassLevel(i, nLevel);
            }
        }
    }
    if ((nUpdateMask & CREATURE_UPDATE_SCENE) && pCreature != NULL)
    {
        CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
        if (pModule != NULL)
        {
            CSWCArea* pArea = pModule->GetArea();
            if (pArea != NULL && pCreature->m_oidArea == pArea->m_nId)
            {
                Gob* pModel = pCreature->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 0);
                if (pModel != NULL)
                {
                    CAurScene* pScene = pArea->m_pScene;
                    if (pModel->GetScene() != pScene &&
                        g_pAppManager->GetClientExoApp()->ReattachCreatureToScene(pCreature->GetObjectId()))
                        pCreature->AttachToScene(pArea->m_pScene);
                }
            }
        }
    }
    return 1;
}
// PC 0x00667CC0  Xbox 0x0008D740
int CSWCMessage::HandleServerToPlayerUpdate_CharacterSheet()
{
    unsigned long oidCreature = ReadOBJECTIDClient();
    CGameObject* pObject;
    if (g_pAppManager->GetClientExoApp()->GetGameObjectArray()->GetGameObject(
            oidCreature, &pObject) != CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    CSWCCreature* pCreature = pObject->AsSWCCreature();
    CSWCCreatureStats* pStats = pCreature->GetStats();
    unsigned long nUpdateMask = ReadDWORD(32);
    if (MessageReadOverflow(1))
        return 0;

    if (nUpdateMask & CHARACTER_SHEET_ABILITIES)
    {
        unsigned char nStrength = ReadBYTE(8);
        unsigned char nDexterity = ReadBYTE(8);
        unsigned char nConstitution = ReadBYTE(8);
        unsigned char nIntelligence = ReadBYTE(8);
        unsigned char nWisdom = ReadBYTE(8);
        unsigned char nCharisma = ReadBYTE(8);
        unsigned char nStrengthBase = ReadCHAR(8);
        unsigned char nDexterityBase = ReadCHAR(8);
        unsigned char nConstitutionBase = ReadCHAR(8);
        unsigned char nIntelligenceBase = ReadCHAR(8);
        unsigned char nWisdomBase = ReadCHAR(8);
        unsigned char nCharismaBase = ReadCHAR(8);
        unsigned char nStrengthBonus = ReadCHAR(8);
        unsigned char nDexterityBonus = ReadCHAR(8);
        unsigned char nConstitutionBonus = ReadCHAR(8);
        unsigned char nIntelligenceBonus = ReadCHAR(8);
        unsigned char nWisdomBonus = ReadCHAR(8);
        unsigned char nCharismaBonus = ReadCHAR(8);
        unsigned char nMaxDexterityBonus = ReadCHAR(8);
        if (MessageReadOverflow(1))
            return 0;
        pStats->m_nStrength = nStrength;
        pStats->m_nDexterity = nDexterity;
        pStats->m_nConstitution = nConstitution;
        pStats->m_nIntelligence = nIntelligence;
        pStats->m_nWisdom = nWisdom;
        pStats->m_nCharisma = nCharisma;
        pStats->m_aUnknown41[0] = nStrengthBase;
        pStats->m_aUnknown41[1] = nDexterityBase;
        pStats->m_aUnknown41[2] = nConstitutionBase;
        pStats->m_aUnknown41[3] = nIntelligenceBase;
        pStats->m_aUnknown41[4] = nWisdomBase;
        pStats->m_aUnknown41[5] = nCharismaBase;
        pStats->m_aAbilities[0] = nStrengthBonus;
        pStats->m_aAbilities[1] = nDexterityBonus;
        pStats->m_aAbilities[2] = nConstitutionBonus;
        pStats->m_aAbilities[3] = nIntelligenceBonus;
        pStats->m_aAbilities[4] = nWisdomBonus;
        pStats->m_aAbilities[5] = nCharismaBonus;
        pStats->m_nDexterityBonus = nMaxDexterityBonus;
    }
    if (nUpdateMask & CHARACTER_SHEET_FORTITUDE)
    {
        char nFortitude = ReadCHAR(8);
        if (MessageReadOverflow(1))
            return 0;
        pStats->m_nFortitudeSave = nFortitude;
    }
    if (nUpdateMask & CHARACTER_SHEET_WILL)
    {
        char nWill = ReadCHAR(8);
        if (MessageReadOverflow(1))
            return 0;
        pStats->m_nWillSave = nWill;
    }
    if (nUpdateMask & CHARACTER_SHEET_REFLEX)
    {
        char nReflex = ReadCHAR(8);
        if (MessageReadOverflow(1))
            return 0;
        pStats->m_nReflexSave = nReflex;
    }
    if (nUpdateMask & CHARACTER_SHEET_EXPERIENCE)
    {
        unsigned long nExperience = ReadDWORD(32);
        if (MessageReadOverflow(1))
            return 0;
        pStats->m_nExperience = nExperience;
    }
    if (nUpdateMask & CHARACTER_SHEET_ATTACK)
    {
        unsigned char nBaseAttackBonus = ReadBYTE(8);
        int bAttack = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        pStats->m_nUnknown64 = nBaseAttackBonus;
        pStats->m_nUnknown60 = bAttack;
    }
    if (nUpdateMask & CHARACTER_SHEET_COMBAT)
    {
        CCombatInformation* pInfo = new CCombatInformation;
        unsigned char nNumAttacks = ReadBYTE(3);
        int nOnHandAttackMod = ReadCHAR(8);
        int nOnHandDamageMod = ReadCHAR(8);
        unsigned char nForceResistance = ReadCHAR(8);
        ReadBYTE(7);
        unsigned char nArmorCheckPenalty = ReadBYTE(5);
        unsigned char nUnarmedDamageDice = ReadBYTE(5);
        unsigned char nUnarmedDamageDie = ReadBYTE(5);
        unsigned char nCriticalRange = ReadBYTE(4);
        unsigned char nCriticalMultiplier = ReadBYTE(3);
        if (MessageReadOverflow(1))
            return 0;
        pInfo->m_nNumAttacks = nNumAttacks;
        pInfo->m_nOnHandAttackMod = nOnHandAttackMod;
        pInfo->m_nOnHandDamageMod = nOnHandDamageMod;
        pInfo->m_nForceResistance = nForceResistance;
        pInfo->m_nArmorCheckPenalty = nArmorCheckPenalty;
        pInfo->m_nUnarmedDamageDice = nUnarmedDamageDice;
        pInfo->m_nUnarmedDamageDie = nUnarmedDamageDie;
        pInfo->m_nOnHandCriticalRange = nCriticalRange;
        pInfo->m_nOnHandCriticalMultiplier = nCriticalMultiplier;
        int bOffHandWeaponEquipped = ReadBOOL();
        if (MessageReadOverflow(1))
            return 0;
        pInfo->m_nOffHandWeaponEquipped = bOffHandWeaponEquipped;
        if (bOffHandWeaponEquipped == 1)
        {
            int nOffHandAttackMod = ReadCHAR(8);
            int nOffHandDamageMod = ReadCHAR(8);
            nCriticalRange = ReadBYTE(4);
            nCriticalMultiplier = ReadBYTE(3);
            if (MessageReadOverflow(1))
                return 0;
            pInfo->m_nOffHandAttackMod = nOffHandAttackMod;
            pInfo->m_nOffHandDamageMod = nOffHandDamageMod;
            pInfo->m_nOffHandCriticalRange = nCriticalRange;
            pInfo->m_nOffHandCriticalMultiplier = nCriticalMultiplier;
        }
        char nModifier;
        unsigned char nModifierType, nWeaponWield;
        unsigned char nVersusRace, nVersusGoodEvil;
        unsigned char nNodes = ReadBYTE(8);
        int nNode;
        for (nNode = 0; nNode < nNodes; ++nNode)
        {
            nModifier = ReadCHAR(8);
            nWeaponWield = ReadBYTE(3);
            nVersusRace = ReadBOOL() == 1 ? ReadBYTE(8) : g_pRules->m_nNumRaces;
            nVersusGoodEvil = ReadBOOL() == 1 ? ReadBYTE(3) : 0;
            if (MessageReadOverflow(1))
                return 0;
            CCombatInformationNode* pNode = new CCombatInformationNode;
            pNode->m_nModifier = nModifier;
            pNode->m_nWeaponWield = nWeaponWield;
            pNode->m_nVersusRace = nVersusRace;
            pNode->m_nVersusGoodEvil = nVersusGoodEvil;
            pInfo->m_lstAttacks.Add(pNode);
        }
        nNodes = ReadBYTE(8);
        for (nNode = 0; nNode < nNodes; ++nNode)
        {
            nModifier = ReadCHAR(8);
            nModifierType = ReadBYTE(4);
            nWeaponWield = ReadBYTE(3);
            nVersusRace = ReadBOOL() == 1 ? ReadBYTE(8) : g_pRules->m_nNumRaces;
            nVersusGoodEvil = ReadBOOL() == 1 ? ReadBYTE(3) : 0;
            if (MessageReadOverflow(1))
                return 0;
            CCombatInformationNode* pNode = new CCombatInformationNode;
            pNode->m_nModifier = nModifier;
            pNode->m_nModifierType = nModifierType;
            pNode->m_nWeaponWield = nWeaponWield;
            pNode->m_nVersusRace = nVersusRace;
            pNode->m_nVersusGoodEvil = nVersusGoodEvil;
            pInfo->m_lstDamage.Add(pNode);
        }
        pStats->SetCombatInformation(pInfo);
    }
    if (nUpdateMask & CHARACTER_SHEET_DEFENSE)
        pStats->m_nDefenseBase = ReadSHORT(16);
    if (nUpdateMask & CHARACTER_SHEET_SKILLS)
    {
        unsigned long nSkillMask = ReadDWORD(g_pRules->m_nNumSkills);
        if (MessageReadOverflow(1))
            return 0;
        for (unsigned short nSkill = 0; nSkill < g_pRules->m_nNumSkills; ++nSkill)
        {
            if (nSkillMask & (1 << nSkill))
            {
                char nRank = ReadCHAR(8);
                unsigned short bCanUse = ReadBOOL();
                if (MessageReadOverflow(1))
                    return 0;
                pStats->SetSkillRank(nSkill, nRank);
                SetGuiSkillRank(pCreature, 1, bCanUse, nSkill);
            }
        }
    }
    if (nUpdateMask & CHARACTER_SHEET_FEATS)
    {
        pStats->ClearFeats();
        unsigned short nFeats = ReadWORD(16);
        if (MessageReadOverflow(1))
            return 0;
        for (unsigned short nFeatIndex = 0; nFeatIndex < nFeats; ++nFeatIndex)
        {
            unsigned short nFeat = ReadWORD(16);
            if (MessageReadOverflow(1))
                return 0;
            pStats->m_lstFeats.AddUnique(nFeat);
        }
        unsigned short nBonusFeats = ReadWORD(16);
        if (MessageReadOverflow(1))
            return 0;
        for (unsigned short nBonusIndex = 0; nBonusIndex < nBonusFeats; ++nBonusIndex)
        {
            unsigned short nFeat = ReadWORD(16);
            if (MessageReadOverflow(1))
                return 0;
            pStats->m_lstBonusFeats.AddUnique(nFeat);
        }
    }
    return 1;
}
// PC 0x0064CE90  Xbox 0x00072590
void CSWCMessage::SetGuiSkillRank(CSWCCreature* pCreature,
                                unsigned short nSkillFlags,
                                unsigned short nSkillMask,
                                unsigned short nSkill)
{

    if (nSkillFlags & nSkillMask)
    {
        if (pCreature->GetStats()->GetSkillRankBase(nSkill) <= 0)
        {
            pCreature->GetStats()->CSWCCreatureStats::SetSkillRankBase(nSkill, 1);
        }
    }
    else
    {
        if (pCreature->GetStats()->GetSkillRankBase(nSkill) > 0)
        {
            pCreature->GetStats()->CSWCCreatureStats::SetSkillRankBase(nSkill, 0);
        }
    }
}
// PC 0x0064CF30  Xbox 0x00072630
int CSWCMessage::HandleServerToPlayerDoorUpdate_Add()
{

    CExoString sModel;
    CExoString sDescription;
    CExoString sTransitionDestination;
    int bVisibleModel = 1;
    CGameObjectArray* pGameObjectArray =
        g_pAppManager->GetClientExoApp()->GetGameObjectArray();
    CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
    unsigned long oidDoor = ReadOBJECTIDClient();
    int bStatic = ReadBOOL();
    unsigned char nAppearanceType = ReadBYTE(8);
    unsigned char nGenericType;
    if (nAppearanceType == 0)
    {
        nGenericType = ReadBYTE(8);
    }
    sDescription = ReadCExoLocStringClient(CResRef(), NULL);
    ReadBOOL();
    unsigned short nAnimation = ReadWORD(16);
    int bLockable = ReadBOOL();
    int bNoVisibleModel = ReadBOOL();
    int bLocked = ReadBOOL();
    int bAreaTransition = ReadBOOL();
    sTransitionDestination = ReadCExoLocStringClient(CResRef(), NULL);
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nAppearanceType != 0)
    {
        {
            CExoString sColumn("Model");
            C2DA* pDoorTypes = g_pRules->Get2DArrays()->m_pDoorTypes;
            pDoorTypes->GetCExoStringEntry(nAppearanceType, sColumn, &sModel);
        }
        {
            CExoString sColumn("VisibleModel");
            C2DA* pDoorTypes = g_pRules->Get2DArrays()->m_pDoorTypes;
            pDoorTypes->GetINTEntry(nAppearanceType, sColumn, &bVisibleModel);
        }
    }
    else
    {
        CExoString sColumn("ModelName");
        C2DA* pGenericDoors = g_pRules->Get2DArrays()->m_pGenericDoors;
        pGenericDoors->GetCExoStringEntry(nGenericType, sColumn, &sModel);
    }
    CSWCDoor* pDoor =
        g_pAppManager->GetClientExoApp()->GetDoorByGameObjectID(oidDoor);
    if (pDoor == NULL)
    {
        pDoor = new CSWCDoor;
        pDoor->LoadModel(sModel, CSWCObject::MODEL_SLOT_DEFAULT,
                         CSWCDoor::NETWORK_MODEL_LOAD_FLAGS);
        pDoor->SetId(oidDoor);
        pDoor->SetArea(pArea);
        if (pGameObjectArray->AddExternalObject(oidDoor, pDoor, 1) !=
            CGameObjectArray::SUCCESS)
        {
            return 0;
        }
        CClientAIMaster* pClientAIMaster =
            g_pAppManager->GetClientExoApp()->GetClientAIMaster();
        pClientAIMaster->AddObject(pDoor);
    }
    else
    {
        pDoor->SetObjectVisibilityOnSetArea(pArea);
    }
    CAurScene* pScene = pArea->m_pScene;
    pDoor->AttachToScene(pScene);
    pDoor->m_sDescription = sDescription;
    if (nAnimation == DOOR_ANIMATION_CLOSED)
    {
        pDoor->SetState(CSWCDoor::DOOR_STATE_CLOSED);
    }
    else if (nAnimation == DOOR_ANIMATION_OPEN1)
    {
        pDoor->SetState(CSWCDoor::DOOR_STATE_OPEN1);
    }
    else if (nAnimation == DOOR_ANIMATION_OPEN2)
    {
        pDoor->SetState(CSWCDoor::DOOR_STATE_OPEN2);
    }
    else if (nAnimation == DOOR_ANIMATION_DESTROYED)
    {
        pDoor->SetState(CSWCDoor::DOOR_STATE_DESTROYED);
    }
    pDoor->SetAnimationWithoutTrigger(nAnimation);
    pDoor->m_bLockable = bLockable;
    pDoor->m_bNoVisibleModel = bNoVisibleModel;
    pDoor->m_bLocked = bLocked;
    pDoor->SetIsAreaTransition(bAreaTransition);
    pDoor->m_nAppearanceType = nAppearanceType;
    pDoor->m_nGenericType = nGenericType;
    if (bVisibleModel == 0)
    {
        pDoor->m_bLocked = 1;
        pDoor->SetState(CSWCDoor::DOOR_STATE_DESTROYED);
    }
    pDoor->m_bStatic = bStatic;
    if (bStatic)
    {
        g_pAppManager->GetClientExoApp()->AddObjectToHitcheckIgnoreList(pDoor->GetId());
    }
    pDoor->m_sTransitionDestination = sTransitionDestination;
    return 1;
}
// PC 0x0064D3A0  Xbox 0x00072AA0
int CSWCMessage::HandleServerToPlayerDoorUpdate_Delete()
{
    unsigned long oidDoor = ReadOBJECTIDClient();
    CGameObjectArray* pGameObjectArray;
    CGameObject* pGameObject;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    pGameObjectArray = g_pAppManager->m_pClientExoApp->GetGameObjectArray();
    if (pGameObjectArray->Delete(oidDoor, &pGameObject) !=
        CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    if (pGameObject == NULL)
    {
        return 0;
    }
    if (pGameObject->AsSWCDoor() != NULL)
    {
        g_pAppManager->m_pClientExoApp->GetClientAIMaster();
        delete pGameObject->AsSWCDoor();
    }
    return 1;
}
// PC 0x0064D420  Xbox 0x00072B20
int CSWCMessage::HandleServerToPlayerDoorUpdate_Update(
    unsigned long oidObject, unsigned long nUpdateMask)
{
    CSWCDoor* pDoor =
        g_pAppManager->GetClientExoApp()->GetDoorByGameObjectID(oidObject);
    if (pDoor == NULL)
    {
        return 0;
    }
    if (nUpdateMask & DOOR_UPDATE_FLAGS)
    {
        int bTrap = ReadBOOL();
        int bTrapDetected = ReadBOOL();
        int bLockable = ReadBOOL();
        int bLocked = ReadBOOL();
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        pDoor->m_bTrap = bTrap;
        pDoor->m_bLockable = bLockable;
        pDoor->m_bLocked = bLocked;
        if (bTrapDetected)
        {
            if (!pDoor->m_bHilighted)
            {
                pDoor->HilightDoor(1);
            }
            pDoor->m_bTrapDetected = 1;
        }
        else
        {
            if (pDoor->m_bHilighted)
            {
                pDoor->HilightDoor(0);
            }
            pDoor->m_bTrapDetected = 0;
        }
    }
    return 1;
}
// PC 0x0064D500  Xbox 0x00072C00
int CSWCMessage::HandleServerToPlayerPlaceableUpdate_Add()
{

    CGameObjectArray* pObjectArray =
        g_pAppManager->GetClientExoApp()->GetGameObjectArray();
    unsigned long oidPlaceable = ReadOBJECTIDClient();
    CExoString sName = ReadCExoLocStringClient(CResRef(), NULL);
    int bTrapDetected = ReadBOOL();
    unsigned char nAppearance = ReadBYTE(8);
    unsigned short nAnimation = ReadWORD(16);
    int nFlag110 = ReadBOOL();
    int nFlag128 = ReadBOOL();
    int nFlag130 = ReadBOOL();
    int nFlag114 = ReadBOOL();
    int nFlag118 = ReadBOOL();
    int nFlag12C = ReadBOOL();
    int nFlag120 = ReadBOOL();
    if (MessageReadOverflow(1))
    {
        return 0;
    }

    CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
    CSWCPlaceable* pPlaceable = g_pAppManager->GetClientExoApp()
        ->GetPlaceableByGameObjectID(oidPlaceable);
    if (pPlaceable == NULL)
    {
        CExoString sModelName;
        if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nGoreLevel == 0)
        {
            bool bNoLowGore;
            {
                CExoString sColumn("LowGore");
                C2DA* pTable = g_pRules->Get2DArrays()->m_pPlaceables;
                bNoLowGore = !pTable->GetCExoStringEntry(nAppearance, sColumn, &sModelName);
            }
            if (bNoLowGore)
            {
                CExoString sColumn("ModelName");
                C2DA* pTable = g_pRules->Get2DArrays()->m_pPlaceables;
                pTable->GetCExoStringEntry(nAppearance, sColumn, &sModelName);
            }
        }
        else
        {
            CExoString sColumn("ModelName");
            C2DA* pTable = g_pRules->Get2DArrays()->m_pPlaceables;
            pTable->GetCExoStringEntry(nAppearance, sColumn, &sModelName);
        }

        if (g_pAppManager->GetServerExoApp() == NULL)
        {
            return 0;
        }
        pPlaceable = new CSWCPlaceable;
        if (pPlaceable == NULL)
        {
            return 0;
        }
        CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
        CSWSPlaceable* pServerPlaceable = pServer->GetPlaceableByGameObjectID(
            pServer->ClientToServerObjectId(oidPlaceable));
        CSWCCreature* pCorpse = NULL;
        if (pServerPlaceable != NULL && pServerPlaceable->m_bIsCorpse)
        {
            unsigned long oidCorpse = pServerPlaceable->m_oidBodyBagCreature;
            oidCorpse = g_pAppManager->GetClientExoApp()
                ->ServerToClientObjectId(oidCorpse);
            pCorpse = g_pAppManager->GetClientExoApp()
                ->GetCreatureByGameObjectID(oidCorpse);
        }
        if (pCorpse != NULL && pCorpse->GetAnimationBase() != NULL)
        {
            pPlaceable->AcquireAnimationBase(pCorpse);
            pPlaceable->SetLoopingAnimation(pCorpse->GetLoopingAnimation());
            CClientAIMaster* pAIMaster =
                g_pAppManager->GetClientExoApp()->GetClientAIMaster();
            pAIMaster->m_aiList.RemoveObject(pCorpse);
            CGameObject* pObject;
            if (g_pAppManager->GetClientExoApp()->GetGameObjectArray()
                    ->Delete(pCorpse->GetId(), &pObject) == CGameObjectArray::SUCCESS &&
                pObject != NULL)
            {
                delete pCorpse;
            }
        }
        else
        {
            CResRef resModel(sModelName);
            pPlaceable->LoadModel(resModel,
                CSWCObject::MODEL_SLOT_DEFAULT,
                CSWCPlaceable::LOAD_MODEL_FLAGS_PLACEABLE);
        }
        pPlaceable->SetAppearance(nAppearance);
        pPlaceable->SetId(oidPlaceable);
        pPlaceable->SetArea(pArea);
        if (pObjectArray->AddExternalObject(oidPlaceable, pPlaceable, 1) !=
            CGameObjectArray::SUCCESS)
        {
            return 0;
        }
        CClientAIMaster* pAIMaster =
            g_pAppManager->GetClientExoApp()->GetClientAIMaster();
        pAIMaster->AddObject(pPlaceable);
        pPlaceable->LoadLight();
        if (pServerPlaceable != NULL && pServerPlaceable->m_bIsBodyBag &&
            !pServerPlaceable->m_bIsBodyBagVisible && pArea != NULL &&
            pArea->ParentCorpseExists(pPlaceable->GetId()))
        {
            pPlaceable->SetDesiredFadeState(CSWCObject::FADE_STATE_HIDDEN, 0, 0, 0);
            pPlaceable->SetFadeState(CSWCObject::FADE_STATE_HIDDEN);
        }
    }
    else
    {
        pPlaceable->m_bFadeLocked = 0;
        pPlaceable->SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 0, 0, 0);
        pPlaceable->SetObjectVisibilityOnSetArea(pArea);
    }
    pPlaceable->m_sName = sName;
    CAurScene* pScene = pArea->m_pScene;
    pPlaceable->AttachToScene(pScene);
    pPlaceable->m_bPlotObject = nFlag110;
    pPlaceable->m_bUseable = nFlag128;
    pPlaceable->m_nUnknown130 = nFlag130;
    pPlaceable->m_nUnknown114 = nFlag114;
    pPlaceable->m_bLocked = nFlag118;
    pPlaceable->m_nUnknown12C = nFlag12C;
    pPlaceable->m_nUnknown120 = nFlag120;
    pPlaceable->m_bTrapDetected = bTrapDetected;
    pPlaceable->SetAnimation(nAnimation, 0, 0);
    if (pPlaceable->m_bUseable)
    {
        g_pAppManager->GetClientExoApp()
            ->RemoveObjectFromHitcheckIgnoreList(pPlaceable->GetId());
    }
    else
    {
        g_pAppManager->GetClientExoApp()
            ->AddObjectToHitcheckIgnoreList(pPlaceable->GetId());
    }
    {
        CResRef resEnvMap("DEFAULT");
        pPlaceable->SetEnvMap(1, 1, resEnvMap);
    }
    return 1;
}
// PC 0x0064DA20  Xbox 0x00073110
int CSWCMessage::HandleServerToPlayerPlaceableUpdate_Delete()
{
    unsigned long oidPlaceable = ReadOBJECTIDClient();
    int bInstant = ReadBOOL();
    int bImmediate = ReadBOOL();
    unsigned long nFadeTime = ReadDWORD(32);
    CSWCObject* pObject;
    CSWCPlaceable* pPlaceable;
    unsigned long nFadeFlags;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    pObject = g_pAppManager->m_pClientExoApp->GetGameObject(oidPlaceable);
    if (pObject == NULL)
    {
        return 0;
    }
    nFadeFlags = bInstant != 0 ? 0 : 0xAFC8;
    if (pObject->AsSWCPlaceable() != NULL)
    {
        pPlaceable = pObject->AsSWCPlaceable();
        if (bImmediate != 0)
        {
            pPlaceable->SetFadeState(CSWCObject::FADE_STATE_HIDDEN);
            pPlaceable->SetDesiredFadeState(CSWCObject::FADE_STATE_HIDDEN, 1, nFadeFlags, 0);
        }
        else
        {
            pPlaceable->SetDesiredFadeState(CSWCObject::FADE_STATE_HIDDEN, 1, nFadeFlags, nFadeTime);
        }
        pPlaceable->m_bFadeLocked = 1;
    }
    return 1;
}
// PC 0x0064DAE0  Xbox 0x000731D0
int CSWCMessage::HandleServerToPlayerPlaceableUpdate_Update(
    unsigned long oidObject, unsigned long nUpdateMask)
{

    CSWCPlaceable* pPlaceable = g_pAppManager->GetClientExoApp()
        ->GetPlaceableByGameObjectID(oidObject);
    if (nUpdateMask & PLACEABLE_UPDATE_FLAGS)
    {
        int bTrapDetected = ReadBOOL();
        int bTrap = ReadBOOL();
        int nFlag118 = ReadBOOL();
        int nFlag110 = ReadBOOL();
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (pPlaceable != NULL)
        {
            pPlaceable->m_bTrapDetected = bTrapDetected;
            pPlaceable->m_bLocked = nFlag118;
            pPlaceable->m_bPlotObject = nFlag110;
            if (bTrap)
            {
                if (!pPlaceable->m_nUnknown13C)
                {
                    pPlaceable->HilightPlaceable(1);
                }
                pPlaceable->m_bTrap = 1;
            }
            else
            {
                if (pPlaceable->m_nUnknown13C)
                {
                    pPlaceable->HilightPlaceable(0);
                }
                pPlaceable->m_bTrap = 0;
            }
        }
    }
    return 1;
}
// PC 0x0064DBC0  Xbox 0x000732B0
int CSWCMessage::HandleServerToPlayerUpdateVisualEffects(CSWCObject* pObject)
{
    unsigned short nCount = ReadWORD(16);
    unsigned char nUpdate;
    unsigned short nEffect;
    unsigned long oidCreator;
    unsigned char nBeamFlag;
    unsigned short nRemaining = nCount;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    for (; nRemaining > 0; nRemaining--)
    {
        nUpdate = ReadBYTE(8);
        nEffect = ReadWORD(16);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (CLoopingVisualEffect::GetIsBeam(nEffect) != 0)
        {
            oidCreator = ReadOBJECTIDClient();
            nBeamFlag = ReadBYTE(8);
        }
        else
        {
            oidCreator = 2130706432;
            nBeamFlag = 0;
        }
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (nUpdate == 'A')
        {
            if (pObject != NULL)
            {
                Vector vOffset;
                pObject->AddVisualEffect(nEffect, 0, oidCreator, 2130706432,
                                         nBeamFlag, 0, vOffset);
            }
        }
        else if (nUpdate == 'D' && pObject != NULL)
        {
            pObject->DeleteVisualEffect(nEffect);
        }
    }
    return 1;
}
// PC 0x0064DCF0  Xbox 0x000733E0
int CSWCMessage::HandleServerToPlayerUpdate_WorkRemaining()
{
    unsigned char nStep = ReadBYTE(8);
    unsigned char nStepCount = ReadBYTE(8);
    unsigned char nTotalSteps;
    CClientExoApp* pApp;
    CSWCModule* pModule;

    EXOXBOXASSERT(nStepCount > 0);
    if (nStepCount < 1)
    {
        nTotalSteps = 1;
    }
    else
    {
        nTotalSteps = nStepCount;
    }
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    pModule = g_pAppManager->m_pClientExoApp->GetModule();
    pApp = g_pAppManager->m_pClientExoApp;
    pApp->SetLoadBarProgress(
        (int)((float)pApp->GetLoadStep(3) +
              ((float)pApp->GetLoadStep(2) +
               ((float)pApp->GetLoadStep(1) +
                ((float)pApp->GetLoadStep(0) +
                 (float)pApp->GetLoadStep(4) *
                     ((float)nStep / (float)nTotalSteps))))),
        1);
    UpdateScreen(0.033333335f, 0, 0);
    if (nStep == nTotalSteps)
    {
        CExoString sAreaName;
        CResRef resArea = pModule->m_resReserved28;
        resArea.CopyToString(sAreaName);
        SendPlayerToServerArea_AreaLoaded();
    }
    return 1;
}
// PC 0x0064DEA0  Xbox 0x000735A0
int CSWCMessage::HandleServerToPlayerGuiInventory(unsigned char nMinor)
{
    int bSuccess = 1;

    if (nMinor == 1)
    {
        ReadBYTE(8);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
        {
            return 0;
        }
    }
    return bSuccess;
}
// PC 0x0064DEE0  Xbox 0x000735E0
int CSWCMessage::HandleServerToPlayerInventory(unsigned char nMinor)
{

    CGameObject* pObject = NULL;
    unsigned long oidItem = ReadOBJECTIDClient();
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetClientExoApp()->GetGameObjectArray();
    if (pObjectArray == NULL)
    {
        return 0;
    }

    switch (nMinor)
    {
    case INVENTORY_DROP:
    case INVENTORY_DROP_CANCEL:
        if (MessageReadUnderflow(1))
        {
            return 0;
        }
        if (pObjectArray->GetGameObject(oidItem, &pObject) == CGameObjectArray::SUCCESS)
        {
            CSWCItem* pItem = pObject->AsSWCItem();
            if (pItem != NULL)
            {
                if (nMinor != INVENTORY_DROP_CANCEL)
                {
                    pItem->PlayDropSound();
                    CSWCCreature* pEquippedBy = pItem->GetEquippedBy();
                    if (pEquippedBy != NULL)
                    {
                        SwitchWeaponEvent(NULL, NULL, pEquippedBy);
                    }
                }
                pItem->m_bAttached = 0;
                if (nMinor == INVENTORY_DROP &&
                    g_pAppManager->m_pClientExoApp->GetPlayerCreature()->GetEquippedItemID(pItem->m_nEquipSlot) == pItem->m_nId)
                {
                    unsigned long nSlot = pItem->m_nEquipSlot;
                    if (nSlot != INVENTORY_SLOT_HEAD)
                    {
                        g_pAppManager->m_pClientExoApp->GetPlayerCreature()->EquipToInventorySlot(nSlot, NULL);
                    }
                }
            }
        }
        return 1;

    case INVENTORY_EQUIP:
    case INVENTORY_EQUIP_CANCEL:
        {
            unsigned long nSlot = ReadDWORD(32);
            int bInstant;
            if (nMinor == INVENTORY_EQUIP)
            {
                bInstant = ReadBOOL();
            }
            if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            {
                return 0;
            }
            if (nMinor == INVENTORY_EQUIP &&
                g_pAppManager->m_pClientExoApp->GetPlayerCreature() != NULL)
            {
                g_pAppManager->m_pClientExoApp->GetPlayerCreature()->SetLastInstant(nSlot, bInstant);
                if (pObjectArray->GetGameObject(oidItem, &pObject) == CGameObjectArray::SUCCESS)
                {
                    CSWCItem* pItem = pObject->AsSWCItem();
                    if (pItem != NULL &&
                        g_pAppManager->m_pClientExoApp->GetPlayerCreature()->GetEquippedItemID(pItem->m_nEquipSlot) != pItem->m_nId &&
                        nSlot != INVENTORY_SLOT_HEAD)
                    {
                        g_pAppManager->m_pClientExoApp->GetPlayerCreature()->EquipToInventorySlot(nSlot, pItem);
                    }
                }
            }
        }
        return 1;

    case INVENTORY_PICKUP:
    case INVENTORY_PICKUP_CANCEL:
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
        {
            return 0;
        }
        if (pObjectArray->GetGameObject(oidItem, &pObject) == CGameObjectArray::SUCCESS && pObject != NULL)
        {
            CSWCItem* pItem = pObject->AsSWCItem();
            if (pItem != NULL)
            {
                if (nMinor == INVENTORY_PICKUP_CANCEL)
                {
                    pItem->m_bAttached = 0;
                }
                else
                {
                    pItem->PlayDropSound();
                    pItem->AttachToScene(NULL);
                    if (pItem->m_nAttachSlot != INVENTORY_SLOT_INVALID)
                    {
                        CSWCMessage* pMessage = g_pAppManager->m_pClientExoApp->GetSWCMessage();
                        if (pMessage != NULL)
                        {
                            pMessage->SendPlayerToServerInventory_Equip(pItem->GetId(), pItem->m_nAttachSlot);
                        }
                    }
                    else
                    {
                        pItem->m_bAttached = 0;
                    }
                    pItem->m_nAttachSlot = INVENTORY_SLOT_INVALID;
                }
            }
        }
        return 1;

    case INVENTORY_LEARN_SCROLL_SOUND:
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
        {
            return 0;
        }
        if (pObjectArray->GetGameObject(oidItem, &pObject) == CGameObjectArray::SUCCESS && pObject != NULL)
        {
            CSWCItem* pItem = pObject->AsSWCItem();
            if (pItem != NULL)
            {
                pItem->PlayDropSound();
            }
        }
        return 1;

    case INVENTORY_UNEQUIP:
    case INVENTORY_UNEQUIP_CANCEL:
        {
            int bInstant;
            if (nMinor == INVENTORY_UNEQUIP)
            {
                bInstant = ReadBOOL();
            }
            if (MessageReadOverflow(1) || MessageReadUnderflow(1))
            {
                return 0;
            }
            if (pObjectArray->GetGameObject(oidItem, &pObject) == CGameObjectArray::SUCCESS && pObject != NULL)
            {
                CSWCItem* pItem = pObject->AsSWCItem();
                if (pItem != NULL)
                {
                    if (nMinor != INVENTORY_UNEQUIP_CANCEL && !bInstant)
                    {
                        pItem->PlayDropSound();
                    }
                    pItem->m_bAttached = 0;
                    if (nMinor == INVENTORY_UNEQUIP &&
                        g_pAppManager->m_pClientExoApp->GetPlayerCreature()->GetEquippedItemID(pItem->m_nEquipSlot) == pItem->m_nId)
                    {
                        if (pItem->m_nEquipSlot != INVENTORY_SLOT_HEAD)
                        {
                            g_pAppManager->m_pClientExoApp->GetPlayerCreature()->SetLastInstant(pItem->m_nEquipSlot, bInstant);
                            g_pAppManager->m_pClientExoApp->GetPlayerCreature()->EquipToInventorySlot(pItem->m_nEquipSlot, NULL);
                        }
                        if (pItem->m_nEquipSlot != INVENTORY_SLOT_INVALID)
                        {
                            pItem->m_nEquipSlot = INVENTORY_SLOT_INVALID;
                        }
                    }
                }
            }
        }
        return 1;

    case INVENTORY_LEARN_SCROLL:
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
        {
            return 0;
        }
        pObject = g_pAppManager->m_pClientExoApp->GetGameObject(oidItem);
        if (pObject != NULL)
        {
            CSWCItem* pItem = pObject->AsSWCItem();
            if (pItem != NULL)
            {
                pItem->m_bAttached = 0;
            }
        }
        return 1;

    default:
        return 0;
    }
}
// PC 0x0064E3D0  Xbox 0x00073AD0
int CSWCMessage::HandleServerToPlayerActiveItemProperties(unsigned long oidItem)
{

    CExoString sName;
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    CSWCItem* pItem = pClientExoApp->GetItemByGameObjectID(oidItem);
    if (pItem != NULL &&
        pItem->GetBaseItem()->m_nItemType >= CSWBaseItem::ITEM_TYPE_ARMOR_FIRST &&
        pItem->GetBaseItem()->m_nItemType <= CSWBaseItem::ITEM_TYPE_ARMOR_LAST)
    {
        short nArmorClass = ReadSHORT(16);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        pItem->m_nArmorClass = nArmorClass;
    }
    sName = ReadCExoLocStringClient(CResRef(), NULL);
    int bIdentified = ReadBOOL();
    unsigned long nCost = ReadDWORD(32);
    unsigned long nStackSize = ReadDWORD(32);
    int bCanUse = ReadBOOL();
    int bStolen = ReadBOOL();
    unsigned char nNumProperties = ReadBYTE(8);
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nNumProperties > 0)
    {
        if (pItem != NULL)
        {
            pItem->SetNumActiveProperties(nNumProperties);
        }
        for (int i = 0; i < nNumProperties; i++)
        {
            unsigned short nPropertyType = ReadWORD(16);
            unsigned short nSubtype = ReadWORD(16);
            unsigned short nCostValue = ReadWORD(16);
            unsigned char nParam1 = ReadBYTE(8);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            if (pItem != NULL)
            {
                pItem->GetActiveProperty(i)->m_nPropertyType = nPropertyType;
                pItem->GetActiveProperty(i)->m_nSubtype = nSubtype;
                pItem->GetActiveProperty(i)->m_nCostValue = nCostValue;
                pItem->GetActiveProperty(i)->m_nParam1 = nParam1;
                pItem->GetActiveProperty(i)->m_bUseable = 1;
                pItem->GetActiveProperty(i)->m_nUsesPerDay = 0;
                pItem->GetActiveProperty(i)->m_nUpgradeType = CSWItemProperty::UPGRADE_TYPE_NONE;
                if (nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM &&
                    pItem->m_nActivePropertyIndex == CSWCItem::INVALID_ACTIVE_PROPERTY_INDEX)
                {
                    pItem->m_nActivePropertyIndex = i;
                }
            }
        }
    }
    if (pItem != NULL)
    {
        pItem->m_bIdentified = bIdentified;
        pItem->m_nCost = nCost;
        pItem->SetStackSize(nStackSize);
        pItem->m_sName = sName;
        pItem->m_bCanUse = bCanUse;
        pItem->m_bStolen = bStolen;
    }
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetInGameGui();
    unsigned char nActiveMask = ReadBYTE(8);
    unsigned char nUsesMask = ReadBYTE(8);
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    for (int i = 0; i < 8; i++)
    {
        if ((nUsesMask & (1 << i)) != 0)
        {
            unsigned char nUsesLeft = ReadBYTE(8);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            if (pItem != NULL)
            {
                pItem->SetItemPropertyUsesLeft(i, nUsesLeft);
                CSWItemProperty* pProperty = pItem->GetActiveProperty(i);
                if (pProperty != NULL)
                {
                    pProperty->m_bUseable = (nActiveMask & (1 << i)) != 0;
                }
            }
        }
    }
    return 1;
}
// PC 0x0064E730  Xbox 0x00073E50
int CSWCMessage::HandleServerToPlayerItemAppearance(
    unsigned long* pnBaseItem, unsigned char* pnModelVariation,
    unsigned char* pnBodyVariation, unsigned char* pnTextureVariation)
{
    *pnBaseItem = ReadDWORD(32);
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    *pnModelVariation = ReadBYTE(8);
    unsigned long nBaseItem = *pnBaseItem;
    if (g_pRules->m_pBaseItems->GetBaseItem(nBaseItem)->m_bUnknown18 == 1)
    {
        *pnBodyVariation = ReadBYTE(8);
        *pnTextureVariation = ReadBYTE(8);
    }
    return 1;
}
// PC 0x0064E7B0  Xbox 0x00073ED0
int CSWCMessage::HandleServerToPlayerItemUpdate_Add(int nMode, CSWCItem** ppItem)
{

    char cGender = 'M';
    unsigned long oidItem = ReadOBJECTIDClient();
    unsigned long nBaseItem;
    unsigned char nModelVariation;
    unsigned char nBodyVariation;
    unsigned char nTextureVariation;
    CGameObject* pObject;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    HandleServerToPlayerItemAppearance(&nBaseItem, &nModelVariation,
                                     &nBodyVariation, &nTextureVariation);
    CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetClientExoApp()->GetGameObjectArray();
    if (pObjectArray->GetGameObject(oidItem, &pObject) == CGameObjectArray::SUCCESS)
    {
        CSWCItem* pItem = pObject->AsSWCItem();
        pItem->m_bInContainer = 0;
        CSWBaseItem* pBaseItem =
            g_pRules->GetBaseItemArray()->GetBaseItem(pItem->GetBaseItemType());
        if (pBaseItem != NULL &&
            pBaseItem->m_bUnknown18 == BASEITEM_MODEL_TYPE_BODY_VARIATION)
        {
            g_pAppManager->GetClientExoApp()->GetPlayerCreature();
        }
        if (nMode == 0)
        {
            if (pItem->m_oidVisibleOwner == OBJECT_INVALID)
            {
                pItem->SetObjectVisibilityOnSetArea(pArea);
                pItem->AttachToScene(pArea->m_pScene);
                CClientAIMaster* pAIMaster =
                    g_pAppManager->GetClientExoApp()->GetClientAIMaster();
                pAIMaster->AddObject(pItem);
                HandleServerToPlayerActiveItemProperties(oidItem);
                return 1;
            }
            else
            {
                CSWCCreature* pEquippedBy = pItem->GetEquippedBy();
                if (pEquippedBy != NULL)
                {
                    if (pEquippedBy->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND) == oidItem)
                        pEquippedBy->EquipToInventorySlot(INVENTORY_SLOT_RIGHTHAND, NULL);
                    if (pEquippedBy->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND) == oidItem)
                        pEquippedBy->EquipToInventorySlot(INVENTORY_SLOT_LEFTHAND, NULL);
                    if (pEquippedBy->GetEquippedItemID(INVENTORY_SLOT_HEAD) == oidItem)
                        pEquippedBy->EquipToInventorySlot(INVENTORY_SLOT_HEAD, NULL);
                }
                else
                {
                    CClientAIMaster* pAIMaster =
                        g_pAppManager->GetClientExoApp()->GetClientAIMaster();
                    pAIMaster->AddObject(pItem);
                }
                pItem->SetEquippedBy(NULL);
                CGameObject* pPreviousOwner;
                unsigned long oidPreviousOwner = pItem->m_oidVisibleOwner;
                if (pObjectArray->GetGameObject(oidPreviousOwner, &pPreviousOwner) ==
                        CGameObjectArray::SUCCESS && pPreviousOwner->AsSWCCreature() != NULL)
                {
                    SwitchWeaponEvent(NULL, NULL, pPreviousOwner->AsSWCCreature());
                }
                pItem->SetArea(NULL);
                pItem->AttachToScene(pArea->m_pScene);
            }
            HandleServerToPlayerActiveItemProperties(oidItem);
            return 1;
        }
        else if (nMode == 1)
        {
            pItem->SetArea(NULL);
            *ppItem = pItem;
            HandleServerToPlayerActiveItemProperties(oidItem);
            return 1;
        }
    }
    CSWCCreature* pPlayerCreature =
        g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    if (pPlayerCreature != NULL && pPlayerCreature->GetAppearance() != NULL)
    {
        cGender = pPlayerCreature->GetAppearance()->GetGenderChar();
    }
    CSWCItem* pItem = CreateItem(nBaseItem, nModelVariation, cGender);
    if (pItem == NULL)
    {
        return 0;
    }
    pItem->SetId(oidItem);
    if (nMode == 1)
    {
        pItem->AttachToScene(NULL);
        pItem->SetArea(NULL);
        *ppItem = pItem;
    }
    else
    {
        pItem->AttachToScene(pArea->m_pScene);
        pItem->SetArea(pArea);
        CClientAIMaster* pAIMaster =
            g_pAppManager->GetClientExoApp()->GetClientAIMaster();
        pAIMaster->AddObject(pItem);
    }
    if (pObjectArray->AddExternalObject(oidItem, pItem, 1) != CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    HandleServerToPlayerActiveItemProperties(oidItem);
    return 1;
}
// PC 0x0064EAC0  Xbox 0x000741E0
int CSWCMessage::HandleServerToPlayerItemUpdate_Delete()
{
    unsigned long oidItem = ReadOBJECTIDClient();
    int bDelete = ReadBOOL();
    CGameObjectArray* pGameObjectArray;
    CGameObject* pGameObject;
    CSWCItem* pItem;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    pGameObjectArray = g_pAppManager->m_pClientExoApp->GetGameObjectArray();
    if (bDelete == 1)
    {
        if (pGameObjectArray->Delete(oidItem, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject != NULL)
        {
            if (pGameObject->AsSWCItem() != NULL)
            {
                pItem = pGameObject->AsSWCItem();
                if (pItem != NULL)
                {
                    delete pItem;
                    return 1;
                }
            }
            return 1;
        }
        return 0;
    }
    if (pGameObjectArray->GetGameObject(oidItem, &pGameObject) !=
        CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    pItem = pGameObject->AsSWCItem();
    if (pItem == NULL)
    {
        return 0;
    }
    pItem->SetArea(NULL);
    if (pItem->GetEquippedBy() == NULL)
    {
        pItem->AttachToScene(NULL);
    }
    return 1;
}
// PC 0x0064EB90  Xbox 0x000742B0
int CSWCMessage::HandleServerToPlayerItemUpdate_Update(
    unsigned long oidObject, unsigned long nUpdateMask)
{
    CResRef cResRef;
    CSWCItem* pItem;
    CSWCArea* pArea;
    CClientExoApp* pApp;

    pApp = g_pAppManager->m_pClientExoApp;
    pItem = pApp->GetItemByGameObjectID(oidObject);
    if (pItem == NULL)
    {
        return 0;
    }
    pArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
    pItem->AttachToScene(pArea->m_pScene);
    pItem->SetArea(pArea);
    return 1;
}
// PC 0x0064EC00  Xbox 0x00074340
int CSWCMessage::HandleServerToPlayerLogin(unsigned char nMinor)
{
    CExoString sPlayerName;
    CExoString sPassword;
    int anPlayerIds[LOGIN_PLAYER_CAPACITY];
    unsigned char acPlayerTypes[LOGIN_PLAYER_CAPACITY];

    if (MessageReadOverflow(1))
        return 0;

    switch (nMinor)
    {
    case LOGIN_ACCEPTED:
        if (MessageReadUnderflow(1))
            return 0;
        g_bLoggedIn = 1;
        return 1;
    case LOGIN_ACKNOWLEDGED:
        if (MessageReadUnderflow(1))
            return 0;
        return 1;
    case LOGIN_PLAYER_RECORDS:
        {
            unsigned char nPlayers = ReadBYTE(8);
            for (unsigned char i = 0; i < nPlayers; ++i)
            {
                anPlayerIds[i] = ReadINT(32);
                acPlayerTypes[i] = ReadBYTE(8);
            }
            unsigned long nPlayerId = ReadDWORD(32);
            return 1;
        }
    case LOGIN_END_LOAD_BAR:
        g_pAppManager->m_pClientExoApp->EndLoadBar();
        return 1;
    case LOGIN_CHARACTER_REPLY:
        {
            unsigned long nCharacterId = ReadDWORD(32);
            if (MessageReadOverflow(1) || MessageReadUnderflow(1))
                return 0;
            return 1;
        }
    }
    return 0;
}
// PC 0x0064EDD0  Xbox 0x00074510
int CSWCMessage::HandleServerToPlayerLoadBar(unsigned char nMinor)
{

    if (MessageReadOverflow(1))
        return 0;

    switch (nMinor)
    {
    case LOAD_BAR_START_STALL_EVENT:
        {
            unsigned long nEvent = ReadDWORD(32);
            if (MessageReadOverflow(1) || MessageReadUnderflow(1))
                return 0;
            int bSuppressLoadBar = g_pAppManager->GetClientExoApp()
                ->m_pcExoAppInternal->m_nUnknown188;
            if (bSuppressLoadBar != 0 && nEvent == LOADBAR_STALL_EVENT_SAVE_GAME)
            {
                g_pAppManager->GetClientExoApp()->m_pcExoAppInternal
                    ->m_nUnknown240 = 1;
                return 1;
            }
            if (g_pAppManager->m_pServerExoApp != NULL)
                return 1;
            if (nEvent == LOADBAR_STALL_EVENT_SAVE_GAME)
            {
                CExoString sFeedback("");
                g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                    FEEDBACK_MESSAGE_SAVE_GAME_STARTED, NULL, sFeedback);
            }
            g_pAppManager->m_pReentrantServerStats->StartStallEvent(nEvent);
            g_pAppManager->m_pReentrantServerStats->m_nProgress = 0;
            g_pAppManager->m_pReentrantServerStats->m_nProgressTotal = 0;
            return 1;
        }
    case LOAD_BAR_UPDATE_STALL_EVENT:
        {
            unsigned long nProgress = ReadDWORD(32);
            unsigned long nTotal = ReadDWORD(32);
            if (MessageReadOverflow(1) || MessageReadUnderflow(1))
                return 0;
            if (g_pAppManager->m_pClientExoApp->m_pcExoAppInternal
                    ->m_nUnknown240 == 1)
                return 1;
            if (g_pAppManager->m_pServerExoApp != NULL)
                return 1;
            g_pAppManager->m_pReentrantServerStats->m_nProgress = nProgress;
            if (nTotal != STALL_EVENT_PROGRESS_UNCHANGED)
                g_pAppManager->m_pReentrantServerStats->m_nProgressTotal = nTotal;
            return 1;
        }
    case LOAD_BAR_END_STALL_EVENT:
        {
            unsigned long nEvent = ReadDWORD(32);
            unsigned long bShutDown = ReadDWORD(3);
            if (MessageReadOverflow(1) || MessageReadUnderflow(1))
                return 0;
            if (g_pAppManager->GetClientExoApp()->GetExoAppInternal()
                    ->m_nUnknown240 == 1)
            {
                g_pAppManager->GetClientExoApp()->GetExoAppInternal()
                    ->m_nUnknown240 = 0;
                return 1;
            }
            g_pAppManager->GetClientExoApp()->GetExoAppInternal()
                ->m_bShutDownToMainMenuPending = bShutDown;
            if (g_pAppManager->m_pServerExoApp != NULL)
                return 1;
            if (nEvent == LOADBAR_STALL_EVENT_SAVE_GAME)
            {
                CExoString sFeedback("");
                g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                    FEEDBACK_MESSAGE_SAVE_GAME_FINISHED, NULL, sFeedback);
            }
            g_pAppManager->m_pReentrantServerStats->m_bOperationEndPending = 1;
            return 1;
        }
    }
    return 0;
}
// PC 0x0064F050  Xbox 0x000747C0
int CSWCMessage::HandleServerToPlayerSoundObject_Play()
{
    unsigned long oidObject = ReadOBJECTIDClient();
    CSWCSoundObject* pSoundObject;

    if (!(MessageReadOverflow(1) || MessageReadUnderflow(1) != 0))
    {
        pSoundObject =
            g_pAppManager->m_pClientExoApp->GetSoundObjectByGameObjectID(
                oidObject);
        if (pSoundObject != NULL)
        {
            pSoundObject->Play();
        }
        return 1;
    }
    return 0;
}
// PC 0x0064F0A0  Xbox 0x00074810
int CSWCMessage::HandleServerToPlayerSoundObject_Stop()
{
    unsigned long oidObject = ReadOBJECTIDClient();
    CSWCSoundObject* pSoundObject;

    if (!(MessageReadOverflow(1) || MessageReadUnderflow(1) != 0))
    {
        pSoundObject =
            g_pAppManager->m_pClientExoApp->GetSoundObjectByGameObjectID(
                oidObject);
        if (pSoundObject != NULL)
        {
            pSoundObject->Stop();
        }
        return 1;
    }
    return 0;
}
// PC 0x0064F0F0  Xbox 0x00074860
int CSWCMessage::HandleServerToPlayerSoundObject_ChangePosition()
{
    Vector vPosition;
    unsigned long oidObject = ReadOBJECTIDClient();
    CSWCSoundObject* pSoundObject;

    vPosition.m_fX = ReadFLOAT(1.0f, 32);
    vPosition.m_fY = ReadFLOAT(1.0f, 32);
    vPosition.m_fZ = ReadFLOAT(1.0f, 32);
    if (!(MessageReadOverflow(1) || MessageReadUnderflow(1) != 0))
    {
        pSoundObject =
            g_pAppManager->m_pClientExoApp->GetSoundObjectByGameObjectID(
                oidObject);
        if (pSoundObject != NULL)
        {
            pSoundObject->ChangePosition(vPosition);
        }
        return 1;
    }
    return 0;
}
// PC 0x0064F1A0  Xbox 0x00074910
int CSWCMessage::HandleServerToPlayerParty(unsigned char nMinor)
{
    if (MessageReadOverflow(1))
        return 0;

    switch (nMinor)
    {
    case 9:
        ReadOBJECTIDClient();
        return 1;
    case 1:
    case 3:
        {
            CGameObjectArray* pObjectArray = g_pAppManager->GetClientExoApp()
                ->m_pcExoAppInternal->m_pGameObjectArray;
            int nCount = ReadINT(32);
            for (; nCount > 0; --nCount)
            {
                unsigned long oidObject = ReadOBJECTIDClient();
                CGameObject* pObject;
                if (pObjectArray->GetGameObject(oidObject, &pObject) ==
                        CGameObjectArray::SUCCESS)
                    pObject->AsSWCCreature();
            }
        }
        break;
    case 4:
        {
            int nCount = ReadINT(32);
            for (; nCount > 0; --nCount)
                ReadOBJECTIDClient();
        }
        break;
    default:
        return 0;
    }
    return 1;
}
// PC 0x0064F280  Xbox 0x00074A00
int CSWCMessage::HandleServerToPlayerServerStatus(unsigned char nMinor)
{
    CExoString sCommand;

    if (MessageReadOverflow(1) || MessageReadUnderflow(1))
        return 0;

    g_pAppManager->m_pClientExoApp->GetPlayModulePanel();
    switch (nMinor)
    {
    case SERVER_STATUS_RUN_REQUEST:
        g_pAppManager->m_pClientExoApp->ServerModeChange(
            CClientExoApp::SERVER_MODE_RUN_REQUESTED);
        sCommand.Format("%c%s.%s", MODULE_COMMAND_PREFIX, "Module", "Run");
        g_pAppManager->m_pClientExoApp->GetNetLayer()->SendMessageToPlayer(
            CNetLayer::PLAYERID_SERVER, (unsigned char*)sCommand.CStr(),
            sCommand.GetLength(), 0);
        return 1;
    case SERVER_STATUS_RESET:
        g_pAppManager->m_pClientExoApp->ServerModeChange(
            CClientExoApp::SERVER_MODE_RESET);
        return 1;
    case SERVER_STATUS_CHARACTER_LOGIN:
        g_pAppManager->m_pClientExoApp->ServerModeChange(
            CClientExoApp::SERVER_MODE_CHARACTER_LOGIN);
        if (g_pAppManager->m_pClientExoApp->GetLoadFromIFO())
        {
            g_pAppManager->m_pClientExoApp->SetLoadFromIFO(0);
        }
        else if (g_pAppManager->m_pClientExoApp->GetPlayerCharacterSet())
        {
            CResRef cCharacter(
                g_pAppManager->m_pClientExoApp->GetPlayerCharacterName());
            SendPlayerToServerLogin_LoginLocalCharacter(cCharacter, 0);
        }
        return 1;
    default:
        return 0;
    }
    return 0;
}
// PC 0x0064F430  Xbox 0x00074BB0
int CSWCMessage::HandleServerToPlayerTriggerUpdate_Update(
    unsigned long oidObject, unsigned long nUpdateMask)
{
    CSWCTrigger* pTrigger;
    int bHighlight;
    CClientExoApp* pApp;

    pApp = g_pAppManager->m_pClientExoApp;
    pTrigger = pApp->GetTriggerByGameObjectID(oidObject);
    if (pTrigger == NULL)
    {
        return 0;
    }
    if ((nUpdateMask & 0x10) != 0 && pTrigger->m_bTrap != 0)
    {
        bHighlight = ReadBOOL();
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (bHighlight != 0)
        {
            pTrigger->UpdateTriggerColor(0);
            return 1;
        }
        pTrigger->UpdateTriggerColor(1);
    }
    return 1;
}
// PC 0x0064F4B0  Xbox 0x00074C30
int CSWCMessage::HandleServerToPlayerTriggerUpdate_Add()
{

    Vector vVertex;
    CExoString sName;
    CExoString sTag;
    int bHostile = 0;
    unsigned long oidTrigger = ReadOBJECTIDClient();
    sName = ReadCExoLocStringClient(CResRef(), NULL);
    int bTrap = ReadBOOL();
    int bAreaTransition = ReadBOOL();
    unsigned char nCursor = ReadBYTE(8);
    unsigned char nTrapType;
    if (bTrap != 0)
    {
        bHostile = ReadBOOL();
        nTrapType = ReadBYTE(8);
    }
    float fHighlightFade = ReadFLOAT(1.0f, 32);
    sTag = ReadCExoLocStringClient(CResRef(), NULL);
    unsigned char nVertices = ReadBYTE(8);
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetClientExoApp()->GetGameObjectArray();
    CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
    CSWCTrigger* pTrigger = new CSWCTrigger;
    if (bTrap != 0)
    {
        CExoString sModel;
        bool bMissingModel;
        {
            CExoString sColumn("Model");
            C2DA* pTraps = g_pRules->Get2DArrays()->m_pTraps;
            bMissingModel = !pTraps->GetCExoStringEntry(nTrapType, sColumn, &sModel);
        }
        if (bMissingModel)
        {
            sModel = "v_mnfrag";
        }
        pTrigger->LoadModel(sModel, CSWCObject::MODEL_SLOT_DEFAULT,
                            CSWCTrigger::LOAD_MODEL_FLAGS_TRIGGER);
    }
    else
    {
        pTrigger->LoadModel("EMPTY", CSWCObject::MODEL_SLOT_DEFAULT,
                            CSWCTrigger::LOAD_MODEL_FLAGS_TRIGGER);
    }
    pTrigger->SetId(oidTrigger);
    pTrigger->SetArea(pArea);
    if (pObjectArray->AddExternalObject(oidTrigger, pTrigger, 1) !=
        CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    CClientAIMaster* pAIMaster =
        g_pAppManager->GetClientExoApp()->GetClientAIMaster();
    pAIMaster->AddObject(pTrigger);
    pTrigger->m_sName = sName;
    if (bTrap != 0)
    {
        pTrigger->m_bTrap = 1;
        pTrigger->m_cTriggerType = nTrapType;
    }
    else if (bAreaTransition != 0)
    {
        pTrigger->m_bAreaTransition = 1;
    }
    pTrigger->m_bHostile = bHostile;
    pTrigger->SetCursor(nCursor);
    int nCurrentVertices = pTrigger->m_nNumVertices;
    pTrigger->m_fHighlightFade = fHighlightFade;
    if (nCurrentVertices < nVertices)
    {
        pTrigger->m_pvVertices = new Vector[nVertices];
        pTrigger->m_nNumVertices = nVertices;
    }
    for (int i = 0; i < nVertices; ++i)
    {
        vVertex.m_fX = ReadFLOAT(1.0f, 32);
        vVertex.m_fY = ReadFLOAT(1.0f, 32);
        vVertex.m_fZ = ReadFLOAT(1.0f, 32) + 0.05f;
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        Vector* pVertex = &pTrigger->m_pvVertices[i];
        *pVertex = vVertex;
    }
    if (bTrap != 0)
    {
        CAurScene* pScene = pArea->m_pScene;
        pTrigger->AttachToScene(pScene);
    }
    else if (bAreaTransition != 0)
    {
        pTrigger->AddToRenderList(CSWCTrigger::RENDER_AREA_TRANSITION);
        pTrigger->DisplayTrigger(1);
    }
    else
    {
        pTrigger->AddToRenderList(CSWCTrigger::RENDER_TRIGGER);
    }
    pTrigger->SpecialDisplayTrigger(RenderQATriggers);
    pTrigger->m_sTag = sTag;
    return 1;
}
// PC 0x0064F960  Xbox 0x000750E0
int CSWCMessage::HandleServerToPlayerTriggerUpdate_Delete()
{

    unsigned long oidTrigger = ReadOBJECTIDClient();
    unsigned char nMode = ReadBYTE(8);
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    CSWCTrigger* pTrigger =
        g_pAppManager->GetClientExoApp()->GetTriggerByGameObjectID(oidTrigger);
    if (pTrigger == NULL)
    {
        return 0;
    }
    pTrigger->RemoveFromRenderList();
    if (nMode == TRIGGER_DELETE_EXPLODE)
    {
        CExoString sExplosionSound;
        unsigned char nTriggerType = pTrigger->m_cTriggerType;
        if (nTriggerType != 255)
        {
            int bFound;
            {
                CExoString sColumn("ExplosionSound");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pTraps = p2DArrays->m_pTraps;
                bFound = pTraps->GetCExoStringEntry(nTriggerType, sColumn,
                                                    &sExplosionSound);
            }
            if (bFound)
            {
                CResRef cSound(sExplosionSound);
                g_pExoSound->Play3DOneShotSound(cSound, pTrigger->m_vPosition,
                    0.0f, CExoSound::PRIORITY_GROUP_TRIGGER, 0, 0, 0.0f, 0.0f);
            }
        }
        pTrigger->AddFireAndForgetAnimation(CSWCTrigger::TRIGGER_ANIMATION_EXPLODE, NULL, 0);
    }
    else if (nMode == TRIGGER_DELETE_DISARM)
    {
        pTrigger->AddFireAndForgetAnimation(CSWCTrigger::TRIGGER_ANIMATION_DISARM, NULL, 0);
    }
    else
    {
        CGameObject* pObject;
        CGameObjectArray* pGameObjectArray =
            g_pAppManager->GetClientExoApp()->GetGameObjectArray();
        if (pGameObjectArray->Delete(oidTrigger, &pObject) != CGameObjectArray::SUCCESS)
        {
            return 0;
        }
        if (pObject == NULL)
        {
            return 0;
        }
        delete pObject->AsSWCTrigger();
    }
    return 1;
}
// PC 0x0064FB20  Xbox 0x000752C0
int CSWCMessage::HandleServerToPlayerUpdate_GuiQuickbarItemUseCount()
{
    unsigned char nCount = ReadBYTE(8);
    unsigned long oidItem;
    unsigned char nProperty;
    unsigned short nUses;
    CSWCItem* pItem;
    CSWItemProperty* pProperty;
    int i;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    for (i = 0; i < nCount; i++)
    {
        ReadBYTE(8);
        ReadBYTE(8);
        oidItem = ReadOBJECTIDClient();
        nProperty = ReadBYTE(8);
        nUses = ReadWORD(16);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(oidItem);
        if (pItem != NULL)
        {
            pProperty = pItem->GetActiveProperty(nProperty);
            if (pProperty == NULL || pProperty->m_nCostValue == CSWItemProperty::ACTIVATE_ITEM_SINGLE_USE)
            {
                pItem->SetStackSize(nUses);
            }
            else
            {
                pItem->SetItemPropertyUsesLeft(nProperty, nUses);
            }
        }
    }
    return 1;
}
#ifndef _XBOX
int CSWCMessage::HandleServerToPlayerUpdate_GuiInventory()  // PC 0x0064FC10  Xbox 0x000753B0
{

    unsigned char nMode = ReadCHAR(8);
    if (MessageReadOverflow(1))
    {
        return 0;
    }

    int bChangedPlayer = 0;
    unsigned long oidPlayer =
        g_pAppManager->m_pClientExoApp->GetPlayerCreatureId();
    if (g_pAppManager->m_pServerExoApp->GetServerInfo()->m_bPartyPicking &&
        g_pAppManager->m_pClientExoApp->GetInGameGui()->GetPartyAccessPanelUp(1))
    {
        int nNPC = g_pAppManager->m_pClientExoApp->GetInGameGui()->GetCurrentEquipScreenNPC();
        unsigned long oidNPC =
            g_pAppManager->m_pServerExoApp->GetPartyTable()->GetNPCObject(nNPC, 0, 1);
        if (g_pAppManager->m_pClientExoApp->DontUseThisSetPlayerCreature(oidNPC))
        {
            bChangedPlayer = 1;
        }
    }

    if (nMode == GUI_INVENTORY_RECORD_ADD)
    {
        unsigned long nSlot = ReadDWORD(32);
        if (MessageReadOverflow(1))
        {
            if (bChangedPlayer)
                g_pAppManager->m_pClientExoApp->DontUseThisSetPlayerCreature(oidPlayer);
            return 0;
        }
        CSWCItem* pItem;
        if (!HandleServerToPlayerItemUpdate_Add(1, &pItem))
        {
            if (bChangedPlayer)
                g_pAppManager->m_pClientExoApp->DontUseThisSetPlayerCreature(oidPlayer);
            return 0;
        }
        pItem->m_nEquipSlot = nSlot;
        pItem->m_nInventorySlot = INVENTORY_SLOT_INVALID;
        CSWCCreature* pCreature =
            g_pAppManager->m_pClientExoApp->GetPlayerCreature();
        if (pCreature != NULL)
        {
            if (pCreature->GetEquippedItemID(nSlot) != pItem->GetId())
                pCreature->EquipToInventorySlot(nSlot, pItem);
        }
        if (bChangedPlayer)
            g_pAppManager->m_pClientExoApp->DontUseThisSetPlayerCreature(oidPlayer);
        return 1;
    }
    else if (nMode == GUI_INVENTORY_RECORD_DELETE)
    {
        ReadDWORD(32);
        if (MessageReadOverflow(1))
        {
            if (bChangedPlayer)
                g_pAppManager->m_pClientExoApp->DontUseThisSetPlayerCreature(oidPlayer);
            return 0;
        }
        if (bChangedPlayer)
            g_pAppManager->m_pClientExoApp->DontUseThisSetPlayerCreature(oidPlayer);
        return 1;
    }
    if (bChangedPlayer)
        g_pAppManager->m_pClientExoApp->DontUseThisSetPlayerCreature(oidPlayer);
    return 0;
}
#endif
int CSWCMessage::HandleServerToPlayerUpdate_GuiRepository(int bBank)  // PC 0x0064FDF0  Xbox 0x00075480
{
    unsigned char nMode = ReadCHAR(8);
    unsigned long oidItem;
    int nStackSize;
    int nCost;
    CSWCItem* pItem;
    CSWCItem* pNewItem;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMode == 65)
    {
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        HandleServerToPlayerItemUpdate_Add(1, &pNewItem);
        return 1;
    }
    if (nMode == 85)
    {
        oidItem = ReadOBJECTIDClient();
        nStackSize = ReadDWORD(32);
        nCost = ReadDWORD(32);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(oidItem);
        if (pItem != NULL)
        {
            pItem->SetStackSize(nStackSize);
            pItem->m_nCost = nCost;
        }
        return 1;
    }
    if (nMode == 68)
    {
        oidItem = ReadOBJECTIDClient();
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(oidItem);
        return 1;
    }
    return 0;
}
// PC 0x0064FEF0  Xbox 0x00075580
int CSWCMessage::HandleServerToPlayerUpdate_GuiContainer()
{
    unsigned char nMode = ReadCHAR(8);
    unsigned char nSlot;
    unsigned char nContainerType;
    unsigned long oidItem;
    int nStackSize;
    int nCost;
    CSWCItem* pItem;
    CSWCItem* pNewItem;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMode == 'P')
    {
        ReadBYTE(8);
    }
    else if (nMode == 'A')
    {
        nSlot = ReadBYTE(8);
        nContainerType = ReadBYTE(8);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        HandleServerToPlayerItemUpdate_Add(1, &pNewItem);
        pItem = pNewItem;
        pItem->m_nContainerSlot = nSlot;
        pItem->m_nContainerType = nContainerType;
        pNewItem->m_bInContainer = 1;
        return 1;
    }
    else if (nMode == 'U')
    {
        oidItem = ReadOBJECTIDClient();
        nStackSize = ReadDWORD(32);
        nCost = ReadDWORD(32);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
        if (pItem != NULL)
        {
            pItem->SetStackSize(nStackSize);
            pItem->m_nCost = nCost;
        }
        return 1;
    }
    else if (nMode == 'D')
    {
        oidItem = ReadOBJECTIDClient();
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
        return 1;
    }
    else if (nMode == 'M')
    {
        nSlot = ReadBYTE(8);
        nContainerType = ReadBYTE(8);
        oidItem = ReadOBJECTIDClient();
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
        if (pItem == NULL)
        {
            return 0;
        }
        pItem->m_nContainerSlot = nSlot;
        pItem->m_nContainerType = nContainerType;
        return 1;
    }
    return 0;
}
// PC 0x006500B0  Xbox 0x00075740
int CSWCMessage::GetSingleCustomTokenStringPart(int nToken,
                                                unsigned long nStrRef,
                                                CExoString& sResult)
{
    int bSuccess = 0;
    CExoString sText;

    if (nToken != 0)
    {
        {
            CExoString sToken(nToken);
            g_pTlkTable->SetCustomToken(0, sToken);
        }
        sText = g_pSWTlkTable->Fetch(nStrRef, NULL, 0, 0);
        sResult = sResult + sText;
        bSuccess = 1;
    }
    return bSuccess;
}
// PC 0x006501B0  Xbox 0x00075840
int CSWCMessage::HandleServerToPlayerSafeProjectileProjectile()
{

    int bResult = 0;
    unsigned long oidSource = ReadOBJECTIDClient();
    unsigned long oidTarget = ReadOBJECTIDClient();
    Vector vStart = ReadVector();
    Vector vEnd = ReadVector();
    unsigned long nTime = ReadINT(32);
    unsigned char nAttackResult = ReadBYTE(8);
    unsigned long nWeaponHand = ReadINT(32);
    unsigned char nAmmunitionType = ReadBYTE(8);
    unsigned char nVariation = ReadBYTE(8);
    unsigned long nSourceDuration = ReadINT(32);

    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidSource);
    if (pCreature != NULL)
    {
        Vector vPosition = vStart;
        int bHookFound = 0;
        unsigned long nSlot = 0;
        CExoString sHook;
        if (nWeaponHand == SAFE_PROJECTILE_HAND_LEFT)
        {
            if (pCreature->GetModel(255, 1)->GetPart("lbullet") != NULL)
            {
                bHookFound = 1;
                sHook = "lbullet";
            }
            else
            {
                nSlot = INVENTORY_SLOT_LEFTHAND;
                sHook = "bullethook";
            }
        }
        else if (nWeaponHand == SAFE_PROJECTILE_HAND_RIGHT)
        {
            if (pCreature->GetModel(255, 1)->GetPart("rbullet") != NULL)
            {
                bHookFound = 1;
                sHook = "rbullet";
            }
            else
            {
                nSlot = INVENTORY_SLOT_RIGHTHAND;
                sHook = "bullethook";
            }
        }
        else if (nWeaponHand == SAFE_PROJECTILE_HAND_CREATURE)
        {
            sHook = "impact";
        }

        Quaternion qOrientation;
        Gob* pModel = NULL;
        if (bHookFound)
        {
            pModel = pCreature->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
            pModel->GetPartPosition(sHook.CStr(), &vPosition, &qOrientation);
        }
        else
        {
            if (nWeaponHand == SAFE_PROJECTILE_HAND_CREATURE)
            {
                pModel = pCreature->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
            }
            else
            {
                CGameObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(
                    pCreature->GetEquippedItemID(nSlot));
                if (pObject != NULL && pObject->AsSWCItem() != NULL)
                {
                    pModel = pObject->AsSWCItem()->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
                }
            }
            if (pModel != NULL)
            {
                pModel->GetPartPosition(sHook.CStr(), &vPosition, &qOrientation);
            }
        }

        if (nAmmunitionType != 0xFF)
        {
            CExoString sModelColumn;
            CExoString sModel;
            CExoString sShotSoundColumn;
            CExoString sShotSound;
            CExoString sImpactSoundColumn;
            CExoString sImpactSound;
            CExoString sMuzzleFlash;
#ifdef _XBOX
            CTwoDimArrays* pXbox2DArrays = g_pRules->Get2DArrays();
            EXOXBOXASSERT(pXbox2DArrays->m_pAmmunitionTypes);
            C2DA* pAmmunitionTypes = pXbox2DArrays->m_pAmmunitionTypes;
#else
            C2DA* pAmmunitionTypes = g_pRules->Get2DArrays()->m_pAmmunitionTypes;
#endif
            CSWCProjectile* pProjectile = new CSWCProjectile;

            sModelColumn.Format("Model%d", nVariation);
            pAmmunitionTypes->GetCExoStringEntry(nAmmunitionType, sModelColumn,
                                                 &sModel);
            if (!pProjectile->LoadModel(sModel, 255, 0))
            {
                delete pProjectile;
                return 0;
            }
            pProjectile->SetEnvMap(255, 1, 0, "DEFAULT");
            pProjectile->SetPosition(vPosition);
            pProjectile->SetOrientation(normalize(vEnd - vStart));
            pProjectile->m_nOrientationType =
                CSWCProjectile::PROJECTILE_ORIENTATION_MOVEMENT;
            pProjectile->SetDesiredPositionTimer(nTime);
            pProjectile->m_nProjectileDuration = 0;
            pProjectile->SetAnimation(CSWCProjectile::PROJECTILE_ANIMATION_TRAVEL);
            sImpactSoundColumn.Format("ImpactSound%d", nVariation);
            pAmmunitionTypes->GetCExoStringEntry(nAmmunitionType,
                                                 sImpactSoundColumn,
                                                 &sImpactSound);
            {
                CResRef cImpactSound(sImpactSound.CStr());
                pProjectile->m_cProjectileSound = cImpactSound;
            }

            switch (nAttackResult)
            {
            case ATTACK_RESULT_HIT_SUCCESSFUL:
            case ATTACK_RESULT_CRITICAL_HIT:
            case ATTACK_RESULT_AUTOMATIC_HIT:
            case ATTACK_RESULT_ATTACK_RESISTED:
                pProjectile->SetDesiredTargetObject(oidTarget, "impact");
                pProjectile->m_nImpactType = CSWCProjectile::IMPACT_UNATTACHED_1;
                pProjectile->SetSpawnCombatVisualOnImpact(
                    1, nAttackResult == ATTACK_RESULT_CRITICAL_HIT, 0);
                pProjectile->SetProjectileType(
                    CSWCProjectile::PROJECTILE_PATH_TYPE_HOMING);
                break;
            case ATTACK_RESULT_PARRIED:
            case ATTACK_RESULT_DEFLECTED:
            case ATTACK_RESULT_WIDE_MISS:
                pProjectile->m_nSourceTargetId = oidSource;
                pProjectile->m_nImpactType = CSWCProjectile::IMPACT_UNATTACHED_1;
                pProjectile->m_nSourceDuration = nSourceDuration;
                pProjectile->SetDesiredTargetObject(oidTarget, "impact_bolt");
                if (nAttackResult == ATTACK_RESULT_PARRIED)
                {
                    pProjectile->SetSourcePosition(vEnd);
                    pProjectile->SetProjectileType(
                        CSWCProjectile::PROJECTILE_PATH_TYPE_PARRIED);
                }
                else if (nAttackResult == ATTACK_RESULT_WIDE_MISS)
                {
                    float fRadius = 0.0f;
                    CSWCCreature* pTarget =
                        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
                            oidTarget);
                    if (pTarget != NULL)
                    {
                        CSWCArea* pArea = pTarget->m_pArea;
                        pProjectile->SetArea(pArea);
                        int nAppearance = pTarget->GetAppearance()->m_nAppearance;
                        int nForceShield = pTarget->GetForceShield();
                        int nShield = 0;
                        int nShieldAppearance = 0;
                        CExoString sColumn;
                        while (nShieldAppearance != nAppearance && ++nShield <= 4)
                        {
                            sColumn.Format("Appearance_0%d", nShield);
                            C2DA* pForceShields = g_pRules->Get2DArrays()->m_pForceShields;
                            pForceShields->GetINTEntry(nForceShield, sColumn,
                                                       &nShieldAppearance);
                        }
                        sColumn.Format("Radius_0%d", nShield);
                        C2DA* pForceShields = g_pRules->Get2DArrays()->m_pForceShields;
                        pForceShields->GetFLOATEntry(nForceShield, sColumn, &fRadius);
                    }
                    pProjectile->m_fSourceDistance = fRadius;
                    pProjectile->SetSourcePosition(vEnd);
                    pProjectile->SetSpawnCombatVisualOnImpact(1, 0, 0);
                    pProjectile->SetProjectileType(
                        CSWCProjectile::PROJECTILE_PATH_TYPE_REFLECTED);
                }
                else
                {
                    pProjectile->m_nImpactType = CSWCProjectile::IMPACT_UNATTACHED_1;
                    pProjectile->SetSourcePosition(pCreature->m_vPosition);
                    pProjectile->SetSpawnCombatVisualOnImpact(1, 0, 0);
                    pProjectile->SetProjectileType(
                        CSWCProjectile::PROJECTILE_PATH_TYPE_DEFLECTED);
                }
                break;
            default:
                pProjectile->SetDesiredTargetObject(OBJECT_INVALID, "");
                pProjectile->SetDesiredPosition(vEnd);
                pProjectile->SetProjectileType(
                    CSWCProjectile::PROJECTILE_PATH_TYPE_HOMING);
                break;
            }

            CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
            if (pModule != NULL && pModule->m_pInfo != NULL)
            {
                CAurScene* pScene = pModule->m_pInfo->m_pScene;
                pProjectile->AttachToScene(pScene);
                pAmmunitionTypes->GetCExoStringEntry(
                    nAmmunitionType, "MuzzleFlash", &sMuzzleFlash);
                pProjectile->CreateMuzzleFlash(sMuzzleFlash, pModel, sHook);
            }
            sShotSoundColumn.Format("ShotSound%d", nVariation);
            pAmmunitionTypes->GetCExoStringEntry(nAmmunitionType,
                                                 sShotSoundColumn, &sShotSound);
            g_pExoSound->Play3DOneShotSound(sShotSound.CStr(),
                                            vPosition, 0.0f, 15, 0, 0, 0.0f,
                                            0.0f);
            bResult = 1;
        }
    }
    return bResult;
}
// PC 0x00650B90  Xbox 0x00076250
int CSWCMessage::HandleServerToPlayerSafeProjectileSpell()
{

    Vector vStart;
    Vector vEnd;
    Vector vOrientation;
    CSWCObject* pSource = NULL;
    CSWCObject* pTarget = NULL;
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    unsigned long oidSource = ReadOBJECTIDClient();
    unsigned long oidTarget = ReadOBJECTIDClient();
    vStart.m_fX = ReadFLOAT(1.0f, 32);
    vStart.m_fY = ReadFLOAT(1.0f, 32);
    vStart.m_fZ = ReadFLOAT(1.0f, 32);
    vEnd.m_fX = ReadFLOAT(1.0f, 32);
    vEnd.m_fY = ReadFLOAT(1.0f, 32);
    vEnd.m_fZ = ReadFLOAT(1.0f, 32);
    unsigned long nProjectileTime = ReadDWORD(32);
    unsigned char nSpellType = ReadBYTE(8);
    ReadDWORD(32);
    ReadINT(32);
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    unsigned long nSpellId;
    unsigned long nPathType;
    if (nSpellType == CSWSObject::SPELL_PROJECTILE_TYPE_NORMAL)
    {
        nSpellId = ReadDWORD(32);
    }
    else if (nSpellType == CSWSObject::SPELL_PROJECTILE_TYPE_CUSTOM_PATH)
    {
        nSpellId = ReadDWORD(32);
        nPathType = ReadBYTE(8);
    }
    else
    {
        nPathType = ReadBYTE(8);
        ReadBYTE(8);
        ReadBYTE(8);
    }
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    CGameObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidSource);
    if (pObject != NULL)
    {
        pSource = pObject->AsSWCObject();
    }
    pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidTarget);
    if (pObject != NULL)
    {
        pTarget = pObject->AsSWCObject();
    }
    Vector vDirection = vEnd - vStart;
    if (vDirection * vDirection > SAFE_PROJECTILE_MIN_DIRECTION_SQUARED)
    {
        vOrientation = normalize(vEnd - vStart);
    }
    else
    {
        vOrientation = Vector(1.0f, 0.0f, 0.0f);
    }
    unsigned long oidDummy;
    if (oidTarget != CGameObjectArray::INVALIDOBJID && pTarget == NULL)
    {
        CSWCCreature* pCreature = new CSWCCreature;
        if (pCreature == NULL)
        {
            return 0;
        }
        pCreature->m_nCameraFollowDelay = 0;
        CResRef cDummy("c_dummy");
        if (pCreature->LoadModel(cDummy, CSWCObject::MODEL_SLOT_DEFAULT, 0) &&
            g_pAppManager->GetClientExoApp()->GetPlayerCreature() != NULL)
        {
            CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetPlayerCreature()->m_pArea;
            if (pArea != NULL)
            {
                if (g_pAppManager->GetClientExoApp()->GetGameObjectArray()->AddInternalObject(
                        oidDummy, pCreature, 0) != CGameObjectArray::SUCCESS)
                {
                    return 0;
                }
                pCreature->SetId(oidDummy);
                pCreature->SetArea(pArea);
                CClientAIMaster* pAIMaster =
                    g_pAppManager->GetClientExoApp()->GetClientAIMaster();
                pAIMaster->AddObject(pCreature);
                CAurScene* pScene = pArea->m_pScene;
                pCreature->AttachToScene(pScene);
                pCreature->SetPosition(vEnd);
                pCreature->m_vDesiredPosition = vEnd;
                pCreature->GetAppearance()->m_nAppearance = CCAppearanceInfo::APPEARANCE_SAFE_PROJECTILE_DUMMY;
                oidTarget = oidDummy;
            }
        }
        pCreature->SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 1, nProjectileTime + 500, 0);
        pCreature->m_bFadeLocked = 1;
        pTarget = pCreature->AsSWCObject();
    }
    if (nSpellType == CSWSObject::SPELL_PROJECTILE_TYPE_NORMAL ||
        nSpellType == CSWSObject::SPELL_PROJECTILE_TYPE_CUSTOM_PATH)
    {
        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
        if (pSpell != NULL)
        {
            Quaternion qOrientation;
            Vector vProjectileStart;
            CResRef cSound;
            vProjectileStart = vStart;
            vProjectileStart.m_fZ += 1.0f;
            if (nSpellType == CSWSObject::SPELL_PROJECTILE_TYPE_NORMAL)
            {
                nPathType = pSpell->m_nProjectileType;
            }
            if (pSource != NULL)
            {
                if (pSource->AsSWCCreature() != NULL)
                {
                    CSWCSpellCastVisual* pCastVisual = pSource->AsSWCCreature()->m_pSpellCastVisual;
                    pCastVisual->LoadCastVisual(
                        pSpell->m_resCastHeadVisual, pSpell->m_resCastHandVisual,
                        pSpell->m_resCastGroundVisual, pSource->AsSWCCreature());
                    pCastVisual = pSource->AsSWCCreature()->m_pSpellCastVisual;
                    pCastVisual->m_bTimerRunning = 1;
                }
                if (pSource->GetModel(255, 1) != NULL)
                {
                    if (pSpell->m_nProjectileSpawnPoint == CSWSpell::PROJECTILE_SPAWN_HAND)
                    {
                        pSource->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPartPosition("handconjure", &vProjectileStart, &qOrientation);
                    }
                    else if (pSpell->m_nProjectileSpawnPoint == CSWSpell::PROJECTILE_SPAWN_HEAD)
                    {
                        pSource->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPartPosition("headconjure", &vProjectileStart, &qOrientation);
                    }
                    else if (pSpell->m_nProjectileSpawnPoint == CSWSpell::PROJECTILE_SPAWN_RIGHT_HAND)
                    {
                        pSource->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPartPosition("rhand", &vProjectileStart, &qOrientation);
                    }
                    else
                    {
                        pSource->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPartPosition("root", &vProjectileStart, &qOrientation);
                    }
                }
            }
            cSound = pSpell->m_resCastSound;
            if (cSound.GetResRef()[0] != 0)
            {
                if (pSource != NULL)
                {
                    g_pExoSound->Play3DOneShotSound(cSound, pSource->m_vPosition,
                        1.5f, 13, 0, 0, 0.0f, 0.0f);
                }
                else
                {
                    g_pExoSound->Play3DOneShotSound(cSound, vStart,
                        1.5f, 13, 0, 0, 0.0f, 0.0f);
                }
            }
            if (pSpell->m_nProjectile)
            {
                CSWCProjectile* pProjectile = new CSWCProjectile;
                pProjectile->SetProjectileSpellId(nSpellId);
                if (pProjectile->LoadModel(pSpell->m_resProjectileModel, 255, 0))
                {
                    pProjectile->SetPosition(Vector(vProjectileStart.m_fX, vProjectileStart.m_fY, vProjectileStart.m_fZ));
                    if (nPathType != CSWCProjectile::PROJECTILE_PATH_TYPE_LINKED)
                    {
                        pProjectile->SetOrientation(Vector(0.0f, 1.0f, 1.0f));
                    }
                    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
                    if (pModule != NULL && pModule->m_pInfo != NULL)
                    {
                        CAurScene* pScene = pModule->m_pInfo->m_pScene;
                        pProjectile->AttachToScene(pScene);
                    }
                    pProjectile->SetDesiredPositionTimer(nProjectileTime);
                    pProjectile->SetAnimation(CSWCProjectile::PROJECTILE_ANIMATION_TRAVEL);
                    pProjectile->SetDesiredPosition(vEnd);
                    pProjectile->SetProjectileType(nPathType);
                    if (nPathType == CSWCProjectile::PROJECTILE_PATH_TYPE_BURST_UP)
                    {
                        for (int i = 4; i != 0; --i)
                        {
                            CSWCProjectile* pExtra = new CSWCProjectile;
                            pExtra->SetProjectileSpellId(nSpellId);
                            if (pExtra->LoadModel(pSpell->m_resProjectileModel, 255, 0))
                            {
                                pExtra->SetPosition(Vector(vProjectileStart.m_fX, vProjectileStart.m_fY, vProjectileStart.m_fZ));
                                pExtra->SetOrientation(vOrientation);
                                pModule = g_pAppManager->GetClientExoApp()->GetModule();
                                if (pModule != NULL && pModule->m_pInfo != NULL)
                                {
                                    CAurScene* pScene = pModule->m_pInfo->m_pScene;
                                    pExtra->AttachToScene(pScene);
                                }
                                pExtra->SetDesiredPositionTimer(nProjectileTime);
                                pExtra->SetAnimation(CSWCProjectile::PROJECTILE_ANIMATION_TRAVEL);
                                pExtra->SetDesiredPosition(vEnd);
                                pExtra->SetDesiredTargetObject(oidTarget, "");
                                pExtra->SetProjectileType(CSWCProjectile::PROJECTILE_PATH_TYPE_BURST_UP);
                            }
                            else
                            {
                                delete pExtra;
                            }
                        }
                    }
                }
                else
                {
                    delete pProjectile;
                }
            }
        }
    }
    return 1;
}
// PC 0x006514C0  Xbox 0x00076BB0
int CSWCMessage::HandleServerToPlayerCombatRound(unsigned char nMinor)
{
    CExoString sText;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMinor == 1)
    {
        ReadBYTE(8);
    }
    return 1;
}
// PC 0x00651560  Xbox 0x00076C50
int CSWCMessage::HandleServerToPlayerActiveItemPropertyUpdate(
    unsigned char nMinor)
{
    unsigned long oidItem = ReadOBJECTIDClient();
    unsigned char nActiveMask = ReadBYTE(8);
    unsigned char nUsesMask = ReadBYTE(8);
    unsigned char aUsesLeft[8];
    unsigned char nUsesLeft;
    CSWCItem* pItem;
    CSWItemProperty* pProperty;
    int i;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMinor == 1)
    {
        for (i = 0; i < 8; i++)
        {
            if (((1 << i) & nUsesMask) != 0)
            {
                nUsesLeft = ReadBYTE(8);
                if (MessageReadOverflow(1))
                {
                    return 0;
                }
                aUsesLeft[i] = nUsesLeft;
            }
        }
        pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(oidItem);
        if (pItem != NULL)
        {
            for (i = 0; i < pItem->m_nNumActiveProperties; i++)
            {
                pProperty = pItem->GetActiveProperty(i);
                if (pProperty != NULL)
                {
                    pProperty->m_bUseable = (nActiveMask & (1 << i)) != 0;
                }
            }
            for (i = 0; i < 64; i++)
            {
                if (((1 << i) & nUsesMask) != 0)
                {
                    pItem->SetItemPropertyUsesLeft(i, aUsesLeft[i]);
                }
            }
        }
    }
    return 1;
}
// PC 0x00651680  Xbox 0x00076D90
int CSWCMessage::HandleServerToPlayerAreaOfEffectUpdate_Add()
{
    unsigned long oidObject = ReadOBJECTIDClient();
    unsigned long nAreaEffectId = ReadINT(32);
    unsigned long oidCreator = ReadOBJECTIDClient();
    CSWCAreaOfEffectObject* pAreaOfEffect;
    CSWCArea* pArea;
    CGameObjectArray* pGameObjectArray;
    CClientAIMaster* pAIMaster;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    pAreaOfEffect = new CSWCAreaOfEffectObject;
    if (pAreaOfEffect == NULL)
    {
        return 0;
    }
    if (pAreaOfEffect->LoadEffect(nAreaEffectId) == 0)
    {
        return 0;
    }
    pArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
    pAreaOfEffect->SetId(oidObject);
    pAreaOfEffect->SetArea(pArea);
    pAreaOfEffect->m_oidCreator = oidCreator;
    pAreaOfEffect->m_pScene = pArea->m_pScene;
    pGameObjectArray = g_pAppManager->m_pClientExoApp->GetGameObjectArray();
    if (pGameObjectArray->AddExternalObject(oidObject,
                                            (CGameObject*)pAreaOfEffect,
                                            1) != CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    pAIMaster = g_pAppManager->m_pClientExoApp->GetClientAIMaster();
    pAIMaster->AddObject(pAreaOfEffect);
    return 1;
}
// PC 0x006517B0  Xbox 0x00076EC0
int CSWCMessage::HandleServerToPlayerAreaOfEffectUpdate_Delete()
{
    unsigned long oidObject = ReadOBJECTIDClient();
    CSWCAreaOfEffectObject* pAreaOfEffect;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    pAreaOfEffect =
        g_pAppManager->m_pClientExoApp->GetAreaOfEffectByGameObjectID(
            oidObject);
    if (pAreaOfEffect != NULL)
    {
        pAreaOfEffect->StopEffect();
    }
    return 1;
}
// PC 0x006517F0  Xbox 0x00076F00
int CSWCMessage::HandleServerToPlayerAreaOfEffectUpdate_Update(
    unsigned long oidObject, unsigned long nUpdateMask)
{
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;

    return pClientExoApp->GetAreaOfEffectByGameObjectID(oidObject) != NULL;
}
// PC 0x00651810  Xbox 0x00076F20
int CSWCMessage::HandleServerToPlayerGuiContainerObjectMessage(
    unsigned char nMinor)
{
    unsigned long oidContainer = ReadOBJECTIDClient();
    int nGuiType = ReadINT(32);

    if (!(MessageReadOverflow(1) || MessageReadUnderflow(1) != 0))
    {
        if (nMinor == 1 && oidContainer != OBJECT_INVALID)
        {
            g_pAppManager->m_pClientExoApp->GetInGameGui()->ShowContainerGui(
                oidContainer, nGuiType);
        }
        return 1;
    }
    return 0;
}
// PC 0x00651880  Xbox 0x00076F90
int CSWCMessage::HandleServerToPlayerGenericObjectUpdate_Update(
    unsigned long oidObject, unsigned long nUpdateMask)
{

    CResRef cPortrait;
    Vector vPosition;
    Vector vOrientation;
    CSWCObject* pObject = NULL;
    CSWCObject* pGameObject;
    unsigned short nPortraitId;
    float fAnimationSpeed;
    unsigned short nAnimation;

    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    pGameObject = pApp->GetGameObject(oidObject);
    if (nUpdateMask & GENERIC_OBJECT_UPDATE_POSITION)
    {
        vPosition.m_fX = ReadFLOAT(1.0f, 32);
        vPosition.m_fY = ReadFLOAT(1.0f, 32);
        vPosition.m_fZ = ReadFLOAT(1.0f, 32);
    }
    if (nUpdateMask & GENERIC_OBJECT_UPDATE_ORIENTATION)
    {
        vOrientation.m_fX = ReadFLOAT(1.0f, 32);
        vOrientation.m_fY = ReadFLOAT(1.0f, 32);
        vOrientation.m_fZ = ReadFLOAT(1.0f, 32);
    }
    if (nUpdateMask & GENERIC_OBJECT_UPDATE_PORTRAIT)
    {
        nPortraitId = ReadWORD(16);
        if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
        {
            cPortrait = ReadCResRef(16);
        }
    }
    if (nUpdateMask & GENERIC_OBJECT_UPDATE_ANIMATION)
    {
        fAnimationSpeed = ReadFLOAT(1.0f, 32);
    }
    if (nUpdateMask & GENERIC_OBJECT_UPDATE_ANIMATION)
    {
        nAnimation = ReadWORD(16);
    }
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (pGameObject != NULL && pGameObject->AsSWCObject() != NULL)
    {
        pObject = pGameObject->AsSWCObject();
        if (nUpdateMask & GENERIC_OBJECT_UPDATE_POSITION)
        {
            pObject->SetPosition(vPosition);
        }
        if (nUpdateMask & GENERIC_OBJECT_UPDATE_ORIENTATION)
        {
            pObject->SetOrientation(vOrientation);
        }
        if (nUpdateMask & GENERIC_OBJECT_UPDATE_PORTRAIT)
        {
            pObject->SetPortraitId(nPortraitId);
            if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
            {
                pObject->SetPortrait(cPortrait);
            }
        }
        if (nUpdateMask & GENERIC_OBJECT_UPDATE_ANIMATION)
        {
            pObject->m_fAnimationSpeed = fAnimationSpeed;
            if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
            {
                if (nAnimation == DOOR_ANIMATION_CLOSED)
                {
                    pObject->AsSWCDoor()->SetState(CSWCDoor::DOOR_STATE_CLOSED);
                }
                else if (nAnimation == DOOR_ANIMATION_OPEN1)
                {
                    pObject->AsSWCDoor()->SetState(CSWCDoor::DOOR_STATE_OPEN1);
                }
                else if (nAnimation == DOOR_ANIMATION_OPEN2)
                {
                    pObject->AsSWCDoor()->SetState(CSWCDoor::DOOR_STATE_OPEN2);
                }
            }
            pObject->SetAnimation(nAnimation, 0, 0);
        }
    }
    if (nUpdateMask & GENERIC_OBJECT_UPDATE_VISUAL_EFFECTS)
    {
        if (HandleServerToPlayerUpdateVisualEffects(pObject) == 0)
        {
            return 0;
        }
    }
    return 1;
}
// PC 0x00651AF0  Xbox 0x00077220
int CSWCMessage::HandleServerToPlayerCreatureUpdate_StripEquippedItems()
{
    unsigned long oidObject = ReadOBJECTIDClient();
    CSWCCreature* pCreature;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    pCreature =
        g_pAppManager->m_pClientExoApp->GetCreatureByGameObjectID(oidObject);
    if (pCreature == NULL)
    {
        return 0;
    }
    pCreature->StripEquippedItems();
    return 1;
}
// PC 0x00651B30  Xbox 0x00077260
int CSWCMessage::HandleServerToPlayerItemUpdate_DestroyItem()
{
    unsigned long oidItem = ReadOBJECTIDClient();
    CSWCItem* pItem;
    CGameObjectArray* pGameObjectArray;
    CGameObject* pGameObject;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    pItem = g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(oidItem);
    if (pItem == NULL)
    {
        return 0;
    }
    pGameObjectArray = g_pAppManager->m_pClientExoApp->GetGameObjectArray();
    if (pGameObjectArray->Delete(pItem->GetId(), &pGameObject) !=
        CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    delete pItem;
    return 1;
}
// PC 0x00651BA0  Xbox 0x000772E0
int CSWCMessage::HandleServerToPlayerVoiceChatMessage(unsigned char nMinor)
{
    unsigned long oidCreature;
    unsigned char nVoiceChat;
    CSWCCreature* pCreature;

    if (nMinor == 1)
    {
        oidCreature = ReadOBJECTIDClient();
        nVoiceChat = ReadBYTE(8);
        if (!MessageReadOverflow(1) && MessageReadUnderflow(1) == 0)
        {
            pCreature =
                g_pAppManager->m_pClientExoApp->GetCreatureByGameObjectID(
                    oidCreature);
            if (pCreature != NULL)
            {
                pCreature->PlaySoundSetSound(nVoiceChat, 1);
            }
            return 1;
        }
        return 0;
    }
    return 1;
}
// PC 0x00651C10  Xbox 0x00077350
int CSWCMessage::HandleServerToPlayerCheat_DoNasty()
{

    unsigned long oidCreature = ReadOBJECTIDClient();
    if (MessageReadOverflow(1))
    {
        return 0;
    }

    CGameObject* pObject;
    if (g_pAppManager->GetClientExoApp()->GetGameObjectArray()
            ->GetGameObject(oidCreature, &pObject) == CGameObjectArray::SUCCESS
        && pObject->AsSWCCreature() != NULL)
    {
        CSWCCreature* pCreature = pObject->AsSWCCreature();
        if (pCreature->m_pBloodEffect != NULL)
        {
            delete pCreature->m_pBloodEffect;
        }
        pCreature->m_pBloodEffect = (Gob*)NewCAurObject(
            "VWP_L_Blood_nast", "blood", NULL, NULL);
        if (pCreature->m_pBloodEffect == NULL)
        {
            return 0;
        }
        pCreature->PlaySoundSetSound(SOUNDSET_NASTY, 1);

        Quaternion qRoot;
        Vector vRoot;
        pCreature->m_pBloodEffect->GetPartLocalPosition("root", &vRoot, &qRoot);
        pCreature->m_pBloodEffect->SetPartLocalPosition("root", vRoot,
            qRoot * Quaternion(Vector(1.0f, 0.0f, 0.0f),
                               SWCCREATURE_NASTY_ROOT_ROTATION));
        pCreature->m_pBloodEffect->GetPartPosition("root", &vRoot, &qRoot);
        vRoot.m_fZ += SWCCREATURE_NASTY_EFFECT_HEIGHT;
        vRoot += pCreature->m_vPosition;
        pCreature->m_pBloodEffect->SetPosition(vRoot);
        pCreature->m_pBloodEffect->SetOrientation(
            YawPitchRoll(Yaw(-pCreature->m_vOrientation),
                         Pitch(-pCreature->m_vOrientation), 0.0f));
        pCreature->m_pBloodEffect->AttachToScene((Scene*)pCreature->m_pArea->m_pScene);
        pCreature->m_pBloodEffect->PlayAnimation("impact", 1.0f,
                                                Gob::ANIMATION_FLAG_NO_LOOP, 0.0f);
    }
    return 1;
}
// PC 0x00651ED0  Xbox 0x00077700
int CSWCMessage::HandleServerToPlayerJournalMessage_RemoveQuest()
{
    CExoString sTag;
    CSWCJournal* pJournal = g_pAppManager->m_pClientExoApp->GetQuestJournal();

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    sTag = ReadCExoString(32);
    pJournal->Remove(sTag);
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    return 1;
}
// PC 0x00651FB0  Xbox 0x000777E0
int CSWCMessage::HandleServerToPlayerJournalMessage_SetQuestPicture()
{
    CSWCJournal* pJournal = g_pAppManager->m_pClientExoApp->GetQuestJournal();
    CExoString sTag;
    int nPicture;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    sTag = ReadCExoString(32);
    nPicture = ReadINT(32);
    pJournal->SetJournalPicture(sTag, nPicture);
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    return 1;
}
// PC 0x006520A0  Xbox 0x000778D0
int CSWCMessage::HandleServerToPlayerJournalMessage_AddWorld()
{
    CExoString sName;
    CExoString sText;
    CSWCWorldJournal* pJournal =
        g_pAppManager->m_pClientExoApp->GetWorldJournal();
    unsigned long nId;
    unsigned long nTime;
    unsigned long nDay;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    nId = ReadINT(32);
    sName = ReadCExoString(32);
    sText = ReadCExoString(32);
    nTime = ReadDWORD(32);
    nDay = ReadDWORD(32);
    pJournal->AddEntry(nId, sName, sText, nTime, nDay);
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    return 1;
}
// PC 0x00652210  Xbox 0x00077A40
int CSWCMessage::HandleServerToPlayerJournalMessage_AddWorldStrref()
{
    CSWCWorldJournal* pJournal =
        g_pAppManager->m_pClientExoApp->GetWorldJournal();
    unsigned long nStrRef;
    unsigned long nTime;
    unsigned long nDay;
    unsigned long nType;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    nStrRef = ReadDWORD(32);
    nTime = ReadDWORD(32);
    nDay = ReadDWORD(32);
    nType = ReadDWORD(32);
    pJournal->AddEntryStrRef(nStrRef, nTime, nDay, nType);
    return MessageReadUnderflow(1) == 0;
}
// PC 0x00652290  Xbox 0x00077AC0
int CSWCMessage::HandleServerToPlayerJournalMessage_DeleteWorld()
{
    CSWCWorldJournal* pJournal =
        g_pAppManager->m_pClientExoApp->GetWorldJournal();
    unsigned long nId;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    nId = ReadINT(32);
    pJournal->DeleteEntry(nId);
    return MessageReadUnderflow(1) == 0;
}
// PC 0x006522E0  Xbox 0x00077B10
int CSWCMessage::HandleServerToPlayerJournalMessage_DeleteWorldStrref()
{
    CSWCWorldJournal* pJournal =
        g_pAppManager->m_pClientExoApp->GetWorldJournal();
    unsigned long nStrRef;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    nStrRef = ReadDWORD(32);
    pJournal->DeleteEntryStrRef(nStrRef);
    return MessageReadUnderflow(1) == 0;
}
// PC 0x006684F0  Xbox 0x0008DF70
int CSWCMessage::HandleServerToPlayerModule(unsigned char nMinor)
{

    if (!MessageReadOverflow(1))
    switch (nMinor)
    {
    case CSWSMessage::MODULE_LOAD:
    {
        CSWCModule* pModule = ClientExoAppLate3(g_pAppManager)->GetModule();
        if (!g_bLoggedIn)
        {
            ClearReadMessage();
            return 1;
        }
        if (!pModule)
            pModule = ClientExoAppLate3(g_pAppManager)->CreateModule();
        pModule->UnloadModule();
        if (!pModule->LoadModule())
            break;
        ClientExoAppLate3(g_pAppManager)->InitializeInGameGUI();
        ClientExoAppLate3(g_pAppManager)->GetInGameGui()->ClearMapDisplays();
        return SendPlayerToServerModule_ModuleLoaded();
    }
    case CSWSMessage::MODULE_UPDATE_TIME:
        return HandleServerToPlayerModuleUpdate_Time();
    case CSWSMessage::MODULE_START_NEW:
        return HandleServerToPlayerModule_StartStartNewModule();
    case CSWSMessage::MODULE_END_NEW:
        return HandleServerToPlayerModule_EndStartNewModule();
    case CSWSMessage::MODULE_EXPORT_REPLY:
        return HandleServerToPlayerExportReply();
    case CSWSMessage::MODULE_UNLOAD:
        ClientExoAppLate3(g_pAppManager)->UnloadModule();
    case CSWSMessage::MODULE_SUCCESS:
        return 1;
    case CSWSMessage::MODULE_SAVE_STATUS:
        return HandleServerToPlayerSaveGameStatus();
    case CSWSMessage::MODULE_END_GAME:
        return HandleServerToPlayerEndGame();
    }
    return 0;
}
// PC 0x00668640  Xbox 0x0008E0C0
int CSWCMessage::HandleServerToPlayerPlayerList(unsigned char nMinor)
{
    CExoLinkedListNode* pNode;
    CExoString sPlayerName;
    CExoString sUnused;
    CExoString sFirstName;
    CExoString sLastName;
    CResRef cPortrait;

    if (MessageReadOverflow(1))
        return 0;

    CExoLinkedList<CSWCPlayer>* pPlayerList =
        g_pAppManager->GetClientExoApp()->GetPlayerList();
    g_pAppManager->GetClientExoApp()->GetInGameGui();

    unsigned char nPlayers;
    switch (nMinor)
    {
    case PLAYER_LIST_ALL:
        while (pPlayerList->GetHeadPos() != NULL)
            delete (CSWCPlayer*)pPlayerList->RemoveHead();
        nPlayers = ReadBYTE(8);
        break;
    case PLAYER_LIST_ADD:
        nPlayers = 1;
        break;
    case PLAYER_LIST_DELETE:
        {
            unsigned long nPlayerId = ReadDWORD(32);
            if (MessageReadOverflow(1) || MessageReadUnderflow(1))
                return 0;

            pNode = pPlayerList->GetHeadPos();
            CSWCPlayer* pPlayer = pPlayerList->GetHead();
            while (pPlayer != NULL)
            {
                if (pPlayer->m_nPlayerId == nPlayerId)
                {
                    pPlayerList->Remove(pNode);
                    delete pPlayer;
                    return 1;
                }
                pPlayer = pPlayerList->GetNext(pNode);
            }
            return 0;
        }
    default:
        return 0;
    }

    if (MessageReadOverflow(1))
        return 0;

    unsigned short nIndex = 0;
    for (; nIndex < (unsigned short)nPlayers; ++nIndex)
    {
        unsigned long nPlayerId = ReadDWORD(32);
        unsigned long oidControlledObject = ReadOBJECTIDClient();
        sPlayerName = ReadCExoString(32);
        int bPrimaryPC = ReadBOOL();
        unsigned long oidPrimaryPC;
        unsigned short nPortraitId;
        if (bPrimaryPC == 1)
        {
            oidPrimaryPC = ReadOBJECTIDClient();
            sFirstName = ReadCExoLocStringClient(CResRef(), NULL);
            sLastName = ReadCExoLocStringClient(CResRef(), NULL);
            if (!sLastName.IsEmpty())
                sFirstName = sFirstName + " " + sLastName;
            nPortraitId = ReadWORD(16);
            if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
                cPortrait = ReadCResRef(16);
        }

        int bNewPlayer = 0;
        if (MessageReadOverflow(1))
            return 0;

        CSWCPlayer* pPlayer;
        if (g_pAppManager->GetClientExoApp()->GetPlayerByPlayerID(nPlayerId) == NULL)
        {
            bNewPlayer = 1;
            pPlayer = new CSWCPlayer(nPlayerId);
            if (pPlayer == NULL)
                return 0;
        }
        else
        {
            pPlayer = g_pAppManager->GetClientExoApp()->GetPlayerByPlayerID(nPlayerId);
            if (pPlayer == NULL)
                return 0;
        }

        pPlayer->m_oidControlledObject = oidControlledObject;
        pPlayer->m_sPlayerName = sPlayerName;
        if (bPrimaryPC == 1)
        {
            pPlayer->m_sPrimaryPCName = sFirstName;
            pPlayer->m_oidPrimaryPC = oidPrimaryPC;
            pPlayer->SetPrimaryPCPortraitId(nPortraitId);
            if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
            {
                CSWCCreatureStats::CheckCustomPortrait(&cPortrait,
                    nPortraitId == PORTRAIT_ID_RESERVED_FFFE);
                pPlayer->SetPrimaryPCPortrait(cPortrait);
            }
        }
        if (bNewPlayer == 1)
            pPlayerList->AddHead(pPlayer);
    }

    if (MessageReadUnderflow(1))
        return 0;
    return 1;
}
// PC 0x00668BB0  Xbox 0x0008E640
int CSWCMessage::HandleServerToPlayerUpdate_PlayerInfo()
{

    unsigned long oidCreature = ReadOBJECTIDClient();
    unsigned long nUpdateMask = ReadDWORD(32);

    if (MessageReadOverflow(1))
    {
        return 0;
    }

    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidCreature);
    CSWCCreatureStats* pStats = NULL;
    if (pCreature != NULL)
    {
        pStats = pCreature->m_pStats;
    }

    if (nUpdateMask & 0x1000)
    {
        pStats->ClearFeats();
        pStats->ClearKnownSpells();
        pCreature->DeleteSpellLikeAbilityList();
        if (pCreature != NULL)
        {
            pCreature->ClearVisibilityList();
        }
        g_pAppManager->m_pClientExoApp->GetInGameGui();
    }

    if (nUpdateMask & 0x1)
    {
        short nDefense = ReadSHORT(16);
        unsigned long nGold = ReadDWORD(32);
        int nUnknown1B4 = ReadINT(32);
        int bLevelUp = ReadBOOL();
        unsigned long nExperience = ReadDWORD(32);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (pCreature != NULL)
        {
            pCreature->GetStats()->m_nExperience = nExperience;
            pCreature->GetStats()->m_nDefenseBase = nDefense;
            pCreature->m_nGold = nGold;
            pCreature->m_nUnknown1B4 = nUnknown1B4;
        }
        g_pAppManager->m_pClientExoApp->GetPlayerCreatureId();
        CGuiInGame* pGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
        if (pGui != NULL)
        {
            pGui->SetLevelUpMode(bLevelUp);
        }
        if (bLevelUp == 1)
        {
            unsigned short nSkillPoints = ReadWORD(16);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            if (pCreature != NULL)
            {
                pCreature->GetStats()->m_nSkillPoints = nSkillPoints;
            }
            for (unsigned short nSkill = 0; nSkill < g_pRules->m_nNumSkills;
                 nSkill++)
            {
                char nRank = ReadCHAR(8);
                if (MessageReadOverflow(1))
                {
                    return 0;
                }
                if (pCreature != NULL)
                {
                    pCreature->GetStats()->CSWCCreatureStats::SetSkillRankBase(
                        nSkill, nRank);
                }
            }
            unsigned char nStartingPackage = ReadBYTE(8);
            ReadBYTE(8);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            if (pCreature != NULL)
            {
                pCreature->GetStats()->m_nStartingPackage = nStartingPackage;
            }
        }
    }

    if (nUpdateMask & 0x2)
    {
        unsigned short nSkillFlags = ReadWORD(16);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (pCreature != NULL)
        {
            SetGuiSkillRank(pCreature, nSkillFlags, 0x2,
                            CSWRules::SKILL_DEMOLITIONS);
            SetGuiSkillRank(pCreature, nSkillFlags, 0x20,
                            CSWRules::SKILL_SECURITY);
            SetGuiSkillRank(pCreature, nSkillFlags, 0x10,
                            CSWRules::SKILL_STEALTH);
        }
    }

    if (nUpdateMask & 0x8)
    {
        unsigned short nFlags = ReadWORD(16);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (pCreature != NULL)
        {
            if (nFlags & 0x1)
            {
                if (pCreature->m_nUnknown190 == 0)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        83, pData, "");
                }
                pCreature->m_nUnknown190 = 1;
            }
            else
            {
                if (pCreature->m_nUnknown190 == 1)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        84, pData, "");
                }
                pCreature->m_nUnknown190 = 0;
            }

            if (nFlags & 0x4)
            {
                if (pCreature->m_nUnknown198 == 0)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        87, pData, "");
                }
                pCreature->m_nUnknown198 = 1;
            }
            else
            {
                if (pCreature->m_nUnknown198 == 1)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        88, pData, "");
                }
                pCreature->m_nUnknown198 = 0;
            }

            if (nFlags & 0x8)
            {
                if (pCreature->m_nUnknown19C == 0)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        89, pData, "");
                }
                pCreature->m_nUnknown19C = 1;
            }
            else
            {
                if (pCreature->m_nUnknown19C == 1)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        90, pData, "");
                }
                pCreature->m_nUnknown19C = 0;
            }

            if (nFlags & 0x10)
            {
                if (pCreature->m_nUnknown1A0 == 0)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        91, pData, "");
                }
                pCreature->m_nUnknown1A0 = 1;
            }
            else
            {
                if (pCreature->m_nUnknown1A0 == 1)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        92, pData, "");
                }
                pCreature->m_nUnknown1A0 = 0;
            }

            if (nFlags & 0x40)
            {
                if (pCreature->m_nUnknown1A8 == 0)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        166, pData, "");
                }
                pCreature->m_nUnknown1A8 = 1;
            }
            else
            {
                if (pCreature->m_nUnknown1A8 == 1)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        167, pData, "");
                }
                pCreature->m_nUnknown1A8 = 0;
            }

            if (nFlags & 0x20)
            {
                if (pCreature->m_nUnknown1AC == 0)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        168, pData, "");
                }
                pCreature->m_nUnknown1AC = 1;
            }
            else
            {
                if (pCreature->m_nUnknown1AC == 1)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData();
                    pData->SetObjectID(0, pCreature->GetId());
                    g_pAppManager->GetClientExoApp()->AddFeedbackMessage(
                        169, pData, "");
                }
                pCreature->m_nUnknown1AC = 0;
            }

            pCreature->m_nUnknown1B8 = (nFlags >> 7) & 1;
            if (nFlags & 0x100)
            {
                pCreature->SetSpecialWalkAnimation(1);
            }
            else
            {
                pCreature->SetSpecialWalkAnimation(0);
            }
            pCreature->m_nUnknown370 = (nFlags >> 10) & 1;
            pCreature->m_nUnknown374 = (nFlags >> 11) & 1;
            pCreature->m_nUnknown378 = (nFlags >> 12) & 1;
            g_pAppManager->m_pClientExoApp->GetInGameGui();
            g_pAppManager->m_pClientExoApp->GetInGameGui();
        }
    }

    if (nUpdateMask & 0x80)
    {
        unsigned char nCount = ReadBYTE(8);
        for (unsigned char i = 0; i < nCount; i++)
        {
            ReadBYTE(8);
            ReadWORD(16);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
        }
        if (g_pAppManager->m_pClientExoApp->GetInGameGui() != NULL)
        {
            g_pAppManager->m_pClientExoApp->GetPlayerCreatureId();
        }
    }

    if (nUpdateMask & 0x10)
    {
        for (unsigned char nClass = 0; nClass < 2; nClass++)
        {
            unsigned short nRemoved = ReadWORD(16);
            for (; nRemoved != 0; nRemoved--)
            {
                unsigned long nSpell = ReadDWORD(32);
                if (pCreature != NULL)
                {
                    pStats->RemoveKnownSpell(nClass, nSpell);
                }
            }
            unsigned short nAdded = ReadWORD(16);
            for (; nAdded != 0; nAdded--)
            {
                unsigned long nSpell = ReadDWORD(32);
                if (pCreature != NULL)
                {
                    pStats->AddKnownSpell(nClass, nSpell);
                }
            }
        }
        if (g_pAppManager->m_pClientExoApp->GetInGameGui() != NULL)
        {
            g_pAppManager->m_pClientExoApp->GetPlayerCreatureId();
        }
    }

    if (nUpdateMask & 0x20)
    {
        for (unsigned char nClass = 0; nClass < 2; nClass++)
        {
            unsigned short nLevels = ReadWORD(16);
            for (; nLevels != 0; nLevels--)
            {
                ReadBYTE(8);
                ReadBYTE(8);
                if (MessageReadOverflow(1))
                {
                    return 0;
                }
            }
            unsigned short nMemorized = ReadWORD(16);
            for (; nMemorized != 0; nMemorized--)
            {
                ReadDWORD(32);
                ReadBYTE(8);
                ReadBYTE(8);
                ReadBOOL();
                ReadBOOL();
                ReadBYTE(8);
                if (MessageReadOverflow(1))
                {
                    return 0;
                }
            }
        }
    }

    if (nUpdateMask & 0x40)
    {
        unsigned char nCount = ReadBYTE(8);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        for (unsigned char i = 0; i < nCount; i++)
        {
            ReadBYTE(8);
            unsigned short nMask = ReadWORD(16);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            for (unsigned char j = 0; j < 1; j++)
            {
                if (nMask & (1 << j))
                {
                    ReadBYTE(8);
                    if (MessageReadOverflow(1))
                    {
                        return 0;
                    }
                }
            }
        }
    }

    if (nUpdateMask & 0x4)
    {
        unsigned short nFeats = ReadWORD(16);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        for (int i = 0; i < nFeats; i++)
        {
            unsigned char nType = ReadCHAR(8);
            unsigned short nFeat = ReadWORD(16);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            if (nType == 'B')
            {
                if (pCreature != NULL)
                {
                    pStats->m_lstBonusFeats.AddUnique(nFeat);
                }
            }
            else if (nType == 'X')
            {
                if (pCreature != NULL)
                {
                    pStats->m_lstBonusFeats.Remove(nFeat);
                }
            }
            else if (nType == 'A')
            {
                if (pCreature != NULL)
                {
                    pStats->m_lstFeats.AddUnique(nFeat);
                }
            }
            else
            {
                if (pCreature != NULL)
                {
                    pStats->m_lstFeats.Remove(nFeat);
                }
            }
        }
        nFeats = ReadWORD(16);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        for (int j = 0; j < nFeats; j++)
        {
            unsigned short nFeat = ReadWORD(16);
            unsigned char nUses = ReadBYTE(8);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            if (pCreature != NULL)
            {
                pStats->SetFeatRemainingUses(nFeat, nUses);
            }
        }
    }

    if (nUpdateMask & 0x100)
    {
        unsigned char nEffects = ReadBYTE(8);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        for (int i = 0; i < nEffects; i++)
        {
            unsigned char nType = ReadCHAR(8);
            if (nType == 'D')
            {
                ReadWORD(16);
                ReadWORD(16);
                if (MessageReadOverflow(1))
                {
                    return 0;
                }
            }
            else if (nType == 'S')
            {
                ReadWORD(16);
                ReadWORD(16);
                ReadOBJECTIDClient();
                if (MessageReadOverflow(1))
                {
                    return 0;
                }
            }
            else if (nType == 'A')
            {

                unsigned short nAction = ReadWORD(16);
                ReadWORD(16);
                if (nAction != ACTION_MOVETOPOINT && nAction != ACTION_DROPITEM)
                {
                    ReadOBJECTIDClient();
                }
                if (nAction == ACTION_MOVETOPOINT ||
                    nAction == ACTION_CASTSPELL ||
                    nAction == ACTION_ITEMCASTSPELL ||
                    nAction == ACTION_DROPITEM || nAction == ACTION_SETTRAP)
                {
                    ReadFLOAT(1.0f, 32);
                    ReadFLOAT(1.0f, 32);
                    ReadFLOAT(1.0f, 32);
                }
                if (nAction == ACTION_CASTSPELL)
                {
                    g_pRules->m_pSpellArray->GetSpell(ReadDWORD(32));
                }
                else if (nAction == ACTION_ITEMCASTSPELL)
                {
                    g_pRules->m_pSpellArray->GetSpell(ReadDWORD(32));
                }
            }
            else if (nType == 'U')
            {
                ReadWORD(16);
                ReadWORD(16);
                ReadOBJECTIDClient();
            }
        }
    }

    if (nUpdateMask & 0x2000)
    {
        if (pCreature == NULL)
        {
            return 0;
        }
        CExoArrayList<CSWVisibilityNode*>* pVisibility = &pCreature->m_lstVisibilityList;
        int nCount = ReadDWORD(32);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        int i;
        for (i = 0; i < nCount; i++)
        {
            unsigned long oidObject = ReadOBJECTIDClient();
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            for (int j = pVisibility->num - 1; j >= 0; j--)
            {
                CSWVisibilityNode* pNode = (*pVisibility)[j];
                if (pNode->m_oidObject == oidObject)
                {
                    pVisibility->DelIndex(j);
                    if (pNode != NULL)
                    {
                        delete pNode;
                    }
                    break;
                }
            }
        }

        nCount = ReadDWORD(32);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        for (i = 0; i < nCount; i++)
        {
            unsigned long oidObject = ReadOBJECTIDClient();
            int bSeen = ReadBOOL();
            int bHeard = ReadBOOL();
            int bInvisible = ReadBOOL();
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            CSWVisibilityNode* pNode;
            int bFound = 0;
            for (int n = 0; n < pVisibility->num; n++)
            {
                pNode = (*pVisibility)[n];
                if (pNode->m_oidObject == oidObject)
                {
                    bFound = 1;
                    break;
                }
            }
            if (!bFound)
            {
                pNode = new CSWVisibilityNode;
                pNode->m_oidObject = oidObject;
                if (bSeen == 1)
                {
                    pNode->m_bSeen = 1;
                    pNode->m_bHeard = 1;
                }
                else
                {
                    pNode->m_bHeard = bHeard;
                }
                pNode->m_bInvisible = bInvisible;
                pVisibility->Add(pNode);
            }
            else
            {

                if (bSeen == 1)
                {
                    pNode->m_bSeen = 1;
                    pNode->m_bHeard = 1;
                }
                else
                {
                    pNode->m_bSeen = bHeard;
                    pNode->m_bHeard = bSeen;
                }
                pNode->m_bInvisible = bInvisible;
            }
            pCreature->UpdateRelativePerception(pNode);
        }
    }

    if (nUpdateMask & 0x800)
    {
        int bAbilities = ReadBOOL();
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        if (bAbilities == 1)
        {
            unsigned char nSTRBase = ReadBYTE(8);
            unsigned char nDEXBase = ReadBYTE(8);
            unsigned char nCONBase = ReadBYTE(8);
            unsigned char nINTBase = ReadBYTE(8);
            unsigned char nWISBase = ReadBYTE(8);
            unsigned char nCHABase = ReadBYTE(8);
            unsigned char nSTR = ReadBYTE(8);
            unsigned char nDEX = ReadBYTE(8);
            unsigned char nCON = ReadBYTE(8);
            unsigned char nINT = ReadBYTE(8);
            unsigned char nWIS = ReadBYTE(8);
            unsigned char nCHA = ReadBYTE(8);
            char nWillSave = ReadCHAR(8);
            char nReflexSave = ReadCHAR(8);
            char nFortitudeSave = ReadCHAR(8);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            if (pCreature != NULL)
            {
                pCreature->GetStats()->m_aAbilities[0] = nSTRBase;
                pCreature->GetStats()->m_aAbilities[1] = nDEXBase;
                pCreature->GetStats()->m_aAbilities[2] = nCONBase;
                pCreature->GetStats()->m_aAbilities[3] = nINTBase;
                pCreature->GetStats()->m_aAbilities[4] = nWISBase;
                pCreature->GetStats()->m_aAbilities[5] = nCHABase;
                pCreature->GetStats()->m_nStrength = nSTR;
                pCreature->GetStats()->m_nDexterity = nDEX;
                pCreature->GetStats()->m_nConstitution = nCON;
                pCreature->GetStats()->m_nIntelligence = nINT;
                pCreature->GetStats()->m_nWisdom = nWIS;
                pCreature->GetStats()->m_nCharisma = nCHA;
                pCreature->GetStats()->m_nWillSave = nWillSave;
                pCreature->GetStats()->m_nReflexSave = nReflexSave;
                pCreature->GetStats()->m_nFortitudeSave = nFortitudeSave;
            }
        }
    }

    if (nUpdateMask & 0x8000)
    {
        for (int nSkill = 0; nSkill < g_pRules->m_nNumSkills; nSkill++)
        {
            pCreature->GetStats()->CSWCCreatureStats::SetSkillRankBase(
                nSkill, ReadCHAR(8));
            pCreature->GetStats()->SetSkillRank(nSkill, ReadCHAR(8));
        }
    }

    if (nUpdateMask & 0x10000)
    {
        if (ReadWORD(16) >= PORTRAIT_ID_FIRST_RESERVED)
        {
            pCreature->SetPortrait(ReadCResRef(16));
        }
    }

    if (nUpdateMask & 0x200)
    {
        pCreature->GetStats()->m_nHitPointsBase = ReadSHORT(16);
        pCreature->GetStats()->m_nCurrentHitPoints = ReadSHORT(16);
        pCreature->GetStats()->m_nHitPointBonus = ReadSHORT(16);
        pCreature->GetStats()->m_nMaxHitPoints = ReadSHORT(16);
    }

    if (nUpdateMask & 0x20000)
    {
        pCreature->GetStats()->m_nForcePointsBase = ReadSHORT(16);
        pCreature->GetStats()->m_nCurrentForcePoints = ReadSHORT(16);
        pCreature->GetStats()->m_nForcePointBonus = ReadSHORT(16);
        pCreature->GetStats()->m_nMaxForcePoints = ReadSHORT(16);
    }

    if (nUpdateMask & 0x4000)
    {
        unsigned short nCount = ReadWORD(16);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        for (; nCount > 0; nCount--)
        {
            unsigned char nType = ReadBYTE(8);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            if (nType == 'S')
            {
                if (pCreature->m_pSpellLikeAbilities == NULL)
                {
                    pCreature->CreateNewSpellLikeAbilityList();
                }
                unsigned short nSize = ReadWORD(16);
                if (MessageReadOverflow(1))
                {
                    return 0;
                }
                pCreature->m_pSpellLikeAbilities->SetSize(nSize);
            }
            else if (nType == 'U')
            {
                unsigned short nIndex = ReadWORD(16);
                int nCasterLevel = ReadBOOL();
                unsigned char bReady = ReadBYTE(8);
                unsigned short nSpell = ReadWORD(16);
                if (MessageReadOverflow(1))
                {
                    return 0;
                }
                (*pCreature->m_pSpellLikeAbilities)[nIndex].m_nCasterLevel =
                    nCasterLevel;
                (*pCreature->m_pSpellLikeAbilities)[nIndex].m_bReady = bReady;
                (*pCreature->m_pSpellLikeAbilities)[nIndex].m_nSpell = nSpell;
            }
        }
    }

    if (g_pAppManager->m_pClientExoApp->GetPlayerCreatureId() != oidCreature &&
        g_pAppManager->m_pClientExoApp->GetMiniGame() == NULL)
    {
        HandleServerToPlayerCreatureUpdate_ControlObject(oidCreature);
    }
    return 1;
}
// PC 0x0066A1F0  Xbox 0x0008FC80
int CSWCMessage::HandleServerToPlayerUpdate_GuiElements()
{
    unsigned char nType = ReadBYTE(8);

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nType)
    {
    case 'I':
        return HandleServerToPlayerUpdate_GuiInventory();
    case 'S':
        return HandleServerToPlayerUpdate_CharacterSheet();
    case 'C':
        return HandleServerToPlayerUpdate_GuiContainer();
    case 'B':
        return HandleServerToPlayerUpdate_GuiRepository(1);
    case 'A':
    case 'R':
        return HandleServerToPlayerUpdate_GuiRepository(0);
    case 'Q':
        return HandleServerToPlayerUpdate_GuiQuickbarItemUseCount();
    }
    return 0;
}
// PC 0x0066A2A0  Xbox 0x0008FD30
int CSWCMessage::HandleServerToPlayerUpdate_Update()
{
    NwchandlemessageTrace2();
    unsigned char nObjectType = ReadBYTE(8);
    unsigned long oidObject = ReadOBJECTIDClient();
    unsigned long nUpdateMask = ReadDWORD(32);

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        return HandleServerToPlayerCreatureUpdate_Update(oidObject,
                                                         nUpdateMask);
    }
    if (HandleServerToPlayerGenericObjectUpdate_Update(oidObject,
                                                       nUpdateMask) == 0)
    {
        return 0;
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {
        return HandleServerToPlayerItemUpdate_Update(oidObject, nUpdateMask);
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
    {
        return HandleServerToPlayerTriggerUpdate_Update(oidObject, nUpdateMask);
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        return HandleServerToPlayerDoorUpdate_Update(oidObject, nUpdateMask);
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT)
    {
        return HandleServerToPlayerAreaOfEffectUpdate_Update(oidObject,
                                                             nUpdateMask);
    }
    if (nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        return HandleServerToPlayerPlaceableUpdate_Update(oidObject,
                                                          nUpdateMask);
    }
    return 0;
}
// PC 0x0066A370  Xbox 0x0008FE00
int CSWCMessage::HandleServerToPlayerJournalMessage(unsigned char nMinor)
{
    NWCHANDLEMESSAGE_TRACE();
    switch (nMinor)
    {
    case 6:
        return HandleServerToPlayerJournalMessage_AddQuest();
    case 7:
        return HandleServerToPlayerJournalMessage_RemoveQuest();
    case 8:
        return HandleServerToPlayerJournalMessage_SetQuestPicture();
    case 1:
        return HandleServerToPlayerJournalMessage_AddWorld();
    case 2:
        return HandleServerToPlayerJournalMessage_AddWorldStrref();
    case 3:
        return HandleServerToPlayerJournalMessage_DeleteWorld();
    case 4:
        return HandleServerToPlayerJournalMessage_DeleteWorldStrref();
    case 5:
        return HandleServerToPlayerJournalMessage_DeleteWorldAll();
    case 9:
        return HandleServerToPlayerJournalMessage_FullQuestUpdate();
    case 12:
        return HandleServerToPlayerJournalMessage_Updated();
    }
    return 0;
}
// PC 0x00652330  Xbox 0x00077B60
int CSWCMessage::HandleServerToPlayerJournalMessage_DeleteWorldAll()
{
    CSWCWorldJournal* pJournal =
        g_pAppManager->m_pClientExoApp->GetWorldJournal();

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    ReadBYTE(8);
    pJournal->DeleteAllEntries();
    return MessageReadUnderflow(1) == 0;
}
// PC 0x00652380  Xbox 0x00077BB0
int CSWCMessage::HandleServerToPlayerJournalMessage_FullQuestUpdate()
{

    CSWCJournal* pJournal =
        g_pAppManager->GetClientExoApp()->GetQuestJournal();
    SJournalEntry entry;

    if (MessageReadOverflow(1))
    {
        pJournal->CallFullUpdateComplete();
        return 0;
    }
    if (ReadBOOL())
    {
        pJournal->DisableSort();
        int bFullUpdate = ReadBOOL();
        if (bFullUpdate)
        {
            pJournal->ClearAll();
        }
        int nEntries = ReadINT(32);
        for (; nEntries > 0; --nEntries)
        {
            unsigned short nFlags = ReadWORD(16);
            entry.m_sPlotID = ReadCExoString(32);
            if (!(nFlags & CSWSPlayerJournalQuestUpdates::QUEST_REMOVED))
            {
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NAME))
                    entry.m_lsName.AddString(
                        g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                        ReadCExoLocStringClient(CResRef(), NULL), 0);
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_TEXT))
                    entry.m_lsText.AddString(
                        g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                        ReadCExoLocStringClient(CResRef(), NULL), 0);
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_DATE))
                    entry.m_nDate = ReadDWORD(32);
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_TIME))
                    entry.m_nTime = ReadDWORD(32);
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_STATE))
                    entry.m_nState = ReadINT(32);
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_PRIORITY))
                    entry.m_nPriority = ReadDWORD(32);
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_PICTURE))
                    entry.m_nPicture = ReadWORD(16);
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_COMPLETED))
                    entry.m_bCompleted = ReadBOOL();
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_PLOTINDEX))
                    entry.m_nPlotIndex = ReadINT(32);
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_PLANETID))
                    entry.m_nPlanetID = ReadINT(32);
                if (bFullUpdate || (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_NEW) ||
                    (nFlags & CSWSPlayerJournalQuestUpdates::QUEST_XP))
                    entry.m_nXP = ReadINT(32);
                if (!(nFlags & CSWSPlayerJournalQuestUpdates::QUEST_QUIET))
                    entry.m_bNew = 1;
                else
                    entry.m_bNew = 0;
                pJournal->Add(entry, nFlags);
            }
            else
            {
                pJournal->Remove(entry.m_sPlotID);
            }
        }
        pJournal->EnableSort();
        pJournal->CallFullUpdateComplete();
    }
    else
    {
        pJournal->CallFullUpdateComplete();
    }
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    return 1;
}
// PC 0x006526E0  Xbox 0x00077F00
int CSWCMessage::HandleServerToPlayerJournalMessage_Updated()
{
    CSWCWorldJournal* pJournal =
        g_pAppManager->m_pClientExoApp->GetWorldJournal();
    CExoString sText;
    int bFromServer;
    int bActive;
    CClientExoApp* pClientExoApp;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    sText = ReadCExoLocStringClient(CResRef(), NULL);
    bFromServer = ReadBOOL();
    bActive = ReadBOOL();
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->JournalUpdated(bFromServer, bActive, sText);
    if (MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    return 1;
}
// PC 0x006527E0  Xbox 0x00078000
int CSWCMessage::HandleServerToPlayerLevelUpMessage(unsigned char nMinor)
{
    CGuiInGame* pInGameGui;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nMinor)
    {
    case 1:
        pInGameGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
        if (pInGameGui != NULL)
        {
            pInGameGui->SetLevelUpMode(0);
        }
        break;
    }
    return !MessageReadOverflow(1) && MessageReadUnderflow(1) == 0;
}
// PC 0x00652840  Xbox 0x00078060
int CSWCMessage::HandleServerToPlayerModule_StartStartNewModule()
{
    g_pAppManager->m_pClientExoApp->SetInStartNewModule(1);
    return 1;
}
// PC 0x00652860  Xbox 0x00078080
int CSWCMessage::HandleServerToPlayerModule_EndStartNewModule()
{
    int bSuccess = ReadBOOL();

    g_pAppManager->m_pClientExoApp->SetInStartNewModule(0);
    if (!(MessageReadOverflow(1) || MessageReadUnderflow(1) != 0))
    {
        if (bSuccess == 1)
        {
            SendPlayerToServerLogin_LoginIFOCharacter();
        }
        return 1;
    }
    return 0;
}
// PC 0x006528B0  Xbox 0x000780D0
int CSWCMessage::HandleServerToPlayerModuleUpdate_Time()
{
    unsigned char nFlags = ReadBYTE(8);
    unsigned char bTimeOfDay = nFlags & 1;
    unsigned char nTimeOfDayState;
    unsigned long nTimeOfDayTime;
    unsigned char bTimeB0;
    int nTimeB0;
    unsigned char bTimeAC;
    int nTimeAC;
    unsigned char bTimeA8;
    int nTimeA8;
    unsigned char bTimeA4;
    int nTimeA4;
    CSWCModule* pModule;

    if (bTimeOfDay != 0)
    {
        nTimeOfDayState = ReadBYTE(8);
        if (nTimeOfDayState == 3 || nTimeOfDayState == 4)
        {
            nTimeOfDayTime = ReadDWORD(32);
        }
        else
        {
            nTimeOfDayTime = 0;
        }
    }
    bTimeB0 = nFlags & 2;
    if (bTimeB0 != 0)
    {
        nTimeB0 = ReadBYTE(8);
    }
    bTimeAC = nFlags & 4;
    if (bTimeAC != 0)
    {
        nTimeAC = ReadBYTE(8);
    }
    bTimeA8 = nFlags & 8;
    if (bTimeA8 != 0)
    {
        nTimeA8 = ReadBYTE(8);
    }
    bTimeA4 = nFlags & 0x10;
    if (bTimeA4 != 0)
    {
        nTimeA4 = ReadDWORD(32);
    }
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    pModule = g_pAppManager->m_pClientExoApp->GetModule();
    if (pModule != NULL)
    {
        if (bTimeOfDay != 0)
        {
            pModule->SetTimeOfDayState(nTimeOfDayState, nTimeOfDayTime);
        }
        if (bTimeB0 != 0)
        {
            pModule->m_nTimeB0 = nTimeB0;
        }
        if (bTimeAC != 0)
        {
            pModule->m_nTimeAC = nTimeAC;
        }
        if (bTimeA8 != 0)
        {
            pModule->m_nTimeA8 = nTimeA8;
        }
        if (bTimeA4 != 0)
        {
            pModule->m_nTimeA4 = nTimeA4;
        }
    }
    return 1;
}
// PC 0x006529F0  Xbox 0x00078210
int CSWCMessage::HandleServerToPlayerSaveGameStatus()
{

    STR_RES strRes;
    CExoString sMessage;

    ReadBYTE(8);
    if (!MessageReadOverflow(1) && MessageReadUnderflow(1) == 0)
    {
        return 1;
    }
    return 0;
}
// PC 0x00652AD0  Xbox 0x000782F0
int CSWCMessage::HandleServerToPlayerEndGame()
{
    CExoString sMessage = ReadCExoString(32);

    return 1;
}
// PC 0x00652B00  Xbox 0x00078310
int CSWCMessage::HandleServerToPlayerGuiQuickbar_SetButton(unsigned char nButton,
                                                         int bClear)
{
    unsigned char nFirstButton;
    unsigned char nEndButton;
    unsigned char nCurrentButton;

    if (bClear)
    {
        nFirstButton = 0;
        nEndButton = QUICKBAR_BUTTON_COUNT;
    }
    else
    {
        nFirstButton = nButton;
        nEndButton = nButton + 1;
    }

    for (nCurrentButton = nFirstButton; nCurrentButton < nEndButton;
         ++nCurrentButton)
    {
        unsigned char nAction = ReadBYTE(8);
        switch (nAction)
        {
        case QUICKBAR_ACTION_COMPOSITE:
            ReadBYTE(8);
            ReadDWORD(32);
            ReadBYTE(8);
            ReadBYTE(8);
            if (MessageReadOverflow(1))
                return 0;
            break;
        case QUICKBAR_ACTION_DWORD_BYTE:
            ReadDWORD(32);
            ReadBYTE(8);
            if (MessageReadOverflow(1))
                return 0;
            break;
        case QUICKBAR_ACTION_STRINGS:
            {
                CExoString sCommand;
                CExoString sLabel;
                sCommand = ReadCExoString(32);
                sLabel = ReadCExoString(32);
                if (MessageReadOverflow(1))
                    return 0;
            }
            break;
        case QUICKBAR_ACTION_INTEGER_3:
            ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            break;
        case QUICKBAR_ACTION_INTEGER_4:
            ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            break;
        case QUICKBAR_ACTION_INTEGER_8:
            ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            break;
        case QUICKBAR_ACTION_INTEGER_10:
            ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            break;
        case QUICKBAR_ACTION_INTEGER_37:
            ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            break;
        case QUICKBAR_ACTION_INTEGER_42:
            ReadINT(32);
            if (MessageReadOverflow(1))
                return 0;
            break;
        }
    }
    return 1;
}
// PC 0x00652D00  Xbox 0x00078510
int CSWCMessage::HandleServerToPlayerMapPinEnabled()
{
    unsigned long oidPin;
    int bEnabled;
    CSWCArea* pArea;
    CSWCMapPin* pMapPin;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    oidPin = ReadOBJECTIDClient();
    bEnabled = ReadBOOL();
    pArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
    pMapPin = pArea->GetMapPin(oidPin, 0);
    if (pMapPin != NULL)
    {
        pMapPin->m_bEnabled = bEnabled;
    }
    return MessageReadUnderflow(1) == 0;
}
// PC 0x00652D60  Xbox 0x00078570
int CSWCMessage::HandleServerToPlayerMapPinReferenceNumber()
{

    Vector vPosition;
    CExoString sText;

    if (MessageReadOverflow(1))
        return 0;

    vPosition.m_fX = ReadFLOAT(1.0f, 32);
    vPosition.m_fY = ReadFLOAT(1.0f, 32);
    vPosition.m_fZ = ReadFLOAT(1.0f, 32);
    sText = ReadCExoString(32);
    unsigned long nReference = ReadDWORD(32);
    if (MessageReadUnderflow(1))
        return 0;

    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    CSWCArea* pArea = pModule->m_pInfo;
    CSWCMapPin* pMapPin = new CSWCMapPin;
    pMapPin->SetPosition(vPosition);
    pMapPin->m_sText = sText;
    pMapPin->m_nReference = nReference;
    pMapPin->m_nType = CSWCMapPin::PIN_TYPE_REFERENCE_NUMBER;
    pMapPin->m_bEnabled = 1;
    pArea->AddMapPin(pMapPin);
    return 1;
}
// PC 0x00652F20  Xbox 0x00078720
int CSWCMessage::HandleServerToPlayerDebugInfo_Item(unsigned long oidItem)
{

    STR_RES strRes;
    CExoString sBaseType;
    CExoString sMessage;
    CExoString sTag;

    sTag = ReadCExoString(32);
    int nPassiveProperties = ReadINT(32);
    unsigned short* pnPassiveProperties = NULL;
    if (nPassiveProperties > 0)
    {
        pnPassiveProperties = new unsigned short[nPassiveProperties];
        if (pnPassiveProperties == NULL)
            return 0;
    }
    for (int i = 0; i < nPassiveProperties; ++i)
        pnPassiveProperties[i] = ReadWORD(16);

    if (MessageReadOverflow(1))
        return 0;

    C2DA* pItemProps = new C2DA("ITEMPROPS", 0);
    if (pItemProps == NULL)
        return 0;
    if (!pItemProps->Load2DArray())
        return 0;
    CSWCItem* pItem =
        g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
    if (pItem == NULL)
        return 0;
    CGuiInGame* pGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
    if (pGui == NULL)
        return 0;

    pGui->AppendToMsgBuffer("******************", CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    sBaseType = g_pRules->m_pBaseItems
        ->GetBaseItem(pItem->GetBaseItemType())->GetNameText();
    sMessage = "Name: ";
    sMessage = sMessage + pItem->GetName(0);
    sMessage = sMessage + "    Tag: ";
    sMessage = sMessage + sTag;
    sMessage = sMessage + "    BaseType: ";
    sMessage = sMessage + sBaseType;
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);

    pItem->GetName(0).CStr();
    sTag.CStr();
    sBaseType.CStr();

    sMessage = "Weight: ";
    sMessage = sMessage + CExoString(0);
    sMessage = sMessage + "    AC: ";
    sMessage = sMessage + CExoString(pItem->GetArmorClass());
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    CExoString(0).CStr();
    CExoString(pItem->GetArmorClass()).CStr();

    sMessage = "ActiveProperties: ";
    sMessage = sMessage + CExoString(pItem->GetNumActiveProperties());
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    CExoString(pItem->GetNumActiveProperties()).CStr();
    int nStringRef;
    for (int nProperty = 0; nProperty < pItem->GetNumActiveProperties(); ++nProperty)
    {
        CSWItemProperty* pProperty = pItem->GetActiveProperty(nProperty);
        if (pProperty != NULL)
        {
            {
                CExoString sColumn = "STRINGREF";
                unsigned short nPropertyType = pProperty->m_nPropertyType;
                pItemProps->GetINTEntry(nPropertyType, sColumn, &nStringRef);
            }
            g_pTlkTable->Fetch(nStringRef, strRes, CTlkTable::GENDER_MALE);
            sMessage = "    ";
            sMessage = sMessage + strRes.m_sString;
            pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
            strRes.m_sString.CStr();
        }
    }

    sMessage = "PassiveProperties: ";
    sMessage = sMessage + CExoString(nPassiveProperties);
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    CExoString(nPassiveProperties).CStr();
    for (int nPassive = 0; nPassive < nPassiveProperties; ++nPassive)
    {
        pItemProps->GetINTEntry(pnPassiveProperties[nPassive], "STRINGREF", &nStringRef);
        g_pTlkTable->Fetch(nStringRef, strRes, CTlkTable::GENDER_MALE);
        sMessage = "    ";
        sMessage = sMessage + strRes.m_sString;
        pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
        strRes.m_sString.CStr();
    }
    pGui->AppendToMsgBuffer("******************", CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    pItemProps->Unload2DArray();
    delete pItemProps;
    if (pnPassiveProperties != NULL)
        delete[] pnPassiveProperties;
    return 1;
}
// PC 0x00653850  Xbox 0x00079050
int CSWCMessage::HandleServerToPlayerDebugInfo_Placeable(unsigned long oidObject)
{
    CResRef cTemplate;
    CExoString sMessage;
    CExoString sLABEL;
    CExoString sName;
    CExoString sTag;
    CExoString sDebug;
    CExoString sFaction;

    sName = ReadCExoLocStringClient(CResRef(), NULL);
    sTag = ReadCExoString(32);
    unsigned char nHardness = ReadBYTE(8);
    short nHitPoints = ReadSHORT(16);
    unsigned char nFortitudeSave = ReadBYTE(8);
    unsigned char nReflexSave = ReadBYTE(8);
    unsigned char nWillSave = ReadBYTE(8);
    cTemplate = ReadCResRef(16);
    sFaction = ReadCExoString(32);

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    CSWCPlaceable* pPlaceable = g_pAppManager->GetClientExoApp()
        ->GetPlaceableByGameObjectID(oidObject);
    if (pPlaceable == NULL)
    {
        return 0;
    }
    CGuiInGame* pGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
    if (pGui == NULL)
    {
        return 0;
    }

    pGui->AppendToMsgBuffer("******************",
                           CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    {
        CExoString sColumn = "LABEL";
        unsigned char nAppearance = pPlaceable->m_nAppearance;
        g_pRules->Get2DArrays()->m_pPlaceables->GetCExoStringEntry(
            nAppearance, sColumn, &sLABEL);
    }
    sMessage = "Name: ";
    sMessage = sMessage + sName;
    sMessage = sMessage + "    Tag: ";
    sMessage = sMessage + sTag;
    sMessage = sMessage + "    Appearance: ";
    sMessage = sMessage + sLABEL;
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("Name: %s\n", sName.CStr());
    EXOLOG("Tag: %s\n", sTag.CStr());
    EXOLOG("Appearance: %s\n", sLABEL.CStr());

    sMessage = "Hardness: ";
    sMessage = sMessage + CExoString(nHardness);
    sMessage = sMessage + "    HP: ";
    sMessage = sMessage + CExoString(nHitPoints);
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("Hardness: %s\n", CExoString(nHardness).CStr());
    EXOLOG("HP: %s\n", CExoString(nHitPoints).CStr());

    sMessage = "FortitudeSave: ";
    sMessage = sMessage + CExoString(nFortitudeSave);
    sMessage = sMessage + "    ReflexSave: ";
    sMessage = sMessage + CExoString(nReflexSave);
    sMessage = sMessage + "    WillSave: ";
    sMessage = sMessage + CExoString(nWillSave);
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("FortitudeSave: %s\n", CExoString(nFortitudeSave).CStr());
    EXOLOG("ReflexSave: %s\n", CExoString(nReflexSave).CStr());
    EXOLOG("WillSave: %s\n", CExoString(nWillSave).CStr());

    cTemplate.CopyToString(sLABEL);
    sMessage = "TemplateResRef: ";
    sMessage = sMessage + sLABEL;
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("TemplateResRef: %s\n", sLABEL.CStr());

    sMessage = "Faction: ";
    sMessage = sMessage + sFaction;
    pGui->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    EXOLOG("Faction: %s\n", sFaction.CStr());
    pGui->AppendToMsgBuffer("******************",
                           CGuiInGame::MESSAGE_MASK_DEBUG_INFO, 0);
    return 1;
}
// PC 0x00654230  Xbox 0x00079A30
int CSWCMessage::HandleServerToPlayerPopUpGUIPanel(unsigned char nMinor)
{
    switch (nMinor)
    {
    case 3:
        ReadCExoString(32);
        return 1;
    case 2:
        g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->DestroyDeathMenuGUI();
        return 1;
    case 1:
    {
        CExoString sText;

        if (!MessageReadOverflow(1))
        {
            ReadBOOL();
            ReadBOOL();
            ReadINT(32);
            sText = ReadCExoString(32);
            if (!MessageReadOverflow(1) && MessageReadUnderflow(1) == 0)
            {
                return 1;
            }
        }
        return 0;
    }
    }
    return 1;
}
// PC 0x00654380  Xbox 0x00079B80
int CSWCMessage::HandleServerToPlayerAmbientMusicPlay()
{
    int bPlay;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    bPlay = ReadBOOL();
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    g_pAppManager->m_pClientExoApp->GetModule()
        ->m_pInfo->m_pAmbientSound->PlayMusic(bPlay);
    return 1;
}
// PC 0x006543E0  Xbox 0x00079BE0
int CSWCMessage::HandleServerToPlayerAmbientMusicSetDelay()
{
    int nDelay;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    nDelay = ReadINT(32);
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    g_pAppManager->m_pClientExoApp->GetModule()
        ->m_pInfo->m_pAmbientSound->SetMusicDelay(nDelay);
    return 1;
}
// PC 0x00654450  Xbox 0x00079C50
int CSWCMessage::HandleServerToPlayerAmbientMusicChangeTrack()
{
    int bDay;
    int nTrack;
    CSWCModule* pModule;
    CSWCArea* pArea;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    bDay = ReadBOOL();
    nTrack = ReadINT(32);
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    pModule = g_pAppManager->GetClientExoApp()->GetModule();
    pArea = pModule->m_pInfo;
    if (bDay)
    {
        pArea->m_pAmbientSound->SetMusicDayTrack(nTrack);
    }
    else
    {
        pArea->m_pAmbientSound->SetMusicNightTrack(nTrack);
    }
    return 1;
}
// PC 0x006544D0  Xbox 0x00079CD0
int CSWCMessage::HandleServerToPlayerAmbientBattleMusicPlay()
{
    int bPlay;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    bPlay = ReadBOOL();
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    g_pAppManager->m_pClientExoApp->GetModule()
        ->m_pInfo->m_pAmbientSound->PlayBattleMusic(bPlay);
    return 1;
}
// PC 0x00654530  Xbox 0x00079D30
int CSWCMessage::HandleServerToPlayerAmbientBattleMusicChange()
{
    int nTrack;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    nTrack = ReadINT(32);
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    g_pAppManager->m_pClientExoApp->GetModule()
        ->m_pInfo->m_pAmbientSound->SetBattleMusicTrack(nTrack);
    return 1;
}
// PC 0x006545A0  Xbox 0x00079DA0
int CSWCMessage::HandleServerToPlayerAmbientSoundLoopPlay()
{
    int bPlay;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    bPlay = ReadBOOL();
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    g_pAppManager->m_pClientExoApp->GetModule()
        ->m_pInfo->m_pAmbientSound->PlayAmbientSound(bPlay);
    return 1;
}
// PC 0x00654600  Xbox 0x00079E00
int CSWCMessage::HandleServerToPlayerAmbientSoundLoopChange()
{
    int bDay;
    int nTrack;
    CSWCModule* pModule;
    CSWCArea* pArea;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    bDay = ReadBOOL();
    nTrack = ReadINT(32);
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    pModule = g_pAppManager->GetClientExoApp()->GetModule();
    pArea = pModule->m_pInfo;
    if (bDay)
    {
        pArea->m_pAmbientSound->SetAmbientDayTrack(nTrack);
    }
    else
    {
        pArea->m_pAmbientSound->SetAmbientNightTrack(nTrack);
    }
    return 1;
}
// PC 0x00654680  Xbox 0x00079E80
int CSWCMessage::HandleServerToPlayerAmbientSoundVolume()
{

    int bDay;
    int nVolume;
    CSWCArea* pArea;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    bDay = ReadBOOL();
    nVolume = ReadINT(32);
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
    if (bDay)
    {
        pArea->m_pAmbientSound->SetAmbientDayVolume(nVolume);
    }
    else
    {
        pArea->m_pAmbientSound->SetAmbientNightVolume(nVolume);
    }
    return 1;
}
// PC 0x00654700  Xbox 0x00079F00
int CSWCMessage::HandleServerToPlayerCharacterDownload(unsigned char nMinor)
{

    CExoString sCharacterFile;
    CExoString sFirstName;
    CExoString sLastName;
    unsigned long nDataSize;
    unsigned char* pCharacterData;
    int nCharacterFileId;
    CClientExoApp* pClientExoApp;

    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nMinor)
    {
    case CHARACTER_DOWNLOAD_REPLY:
        sFirstName = ReadCExoLocStringClient(CResRef(), NULL);
        sLastName = ReadCExoLocStringClient(CResRef(), NULL);
        nDataSize = ReadDWORD(32);
        pCharacterData = (unsigned char*)ReadVOIDPtr(nDataSize);
        g_pAppManager->m_pClientExoApp->SetCharacterFile(
            nDataSize, pCharacterData, 0);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
        {
            return 0;
        }
        break;
    case CHARACTER_DOWNLOAD_FILE:
        sCharacterFile = ReadCExoString(32);
        nCharacterFileId = ReadBOOL();
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->SetCharacterFile(
            sCharacterFile.GetLength(), (unsigned char*)sCharacterFile.CStr(),
            nCharacterFileId);
        break;
    default:
        return 0;
    }
    return 1;
}

int CSWCMessage::HandleServerToPlayerCheat_PonyRide()
{
    ReadOBJECTIDClient();
    ReadBOOL();
    return !MessageReadOverflow(1);
}

int CSWCMessage::HandleServerToPlayerCheat_DebugMode()
{
    int bDebugMode = ReadBOOL();
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    g_pAppManager->GetClientExoApp()->SetDebugMode(bDebugMode);
    return 1;
}
// PC 0x00654900  Xbox 0x0007A100
int CSWCMessage::HandleServerToPlayerSaveLoad(unsigned char nMinor)
{
    int bSuccess = 1;
    unsigned char nMode;
    unsigned long nFlag;

    switch (nMinor)
    {
    case 1:
        nMode = ReadBYTE(8);
        nFlag = ReadDWORD(4);
        if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
        {
            return 0;
        }
        if (nFlag != 0 && nMode != 2)
        {
            g_pAppManager->m_pClientExoApp->m_pcExoAppInternal
                ->ShutDownLoadGameLoadBar();
        }
        break;
    }
    return bSuccess;
}
// PC 0x00654970  Xbox 0x0007A170
int CSWCMessage::HandlePlayerToServerShutDownServer(unsigned char nMinor)
{

    STR_RES strRes;

    if (MessageReadOverflow(1))
    {
        return 0;
    }

    ReadDWORD(32);
    if (MessageReadOverflow(1) || MessageReadUnderflow(1) != 0)
    {
        return 0;
    }
    return 1;
}
// PC 0x00654A30  Xbox 0x0007A230
int CSWCMessage::HandleServerToPlayerGuiTimingEvent_Info()
{
    if (ReadBOOL())
    {
        ReadDWORD(32);
        ReadBYTE(8);
    }
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bPlayMovies)
    {
        CGuiInGame* pGui =
            g_pAppManager->m_pClientExoApp->GetInGameGui();
        if (pGui == NULL || pGui->m_bItemCreateMenuAvailable == 0)
        {
            return 0;
        }
    }
    return 1;
}
// PC 0x00654AA0  Xbox 0x0007A2A0
int CSWCMessage::HandleServerToPlayerGUICharacterSheet(unsigned char nMinor)
{
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nMinor)
    {
    case 2:
        ReadOBJECTIDClient();
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        g_pAppManager->m_pClientExoApp->GetInGameGui();
        break;
    }
    return !MessageReadOverflow(1) && MessageReadUnderflow(1) == 0;
}
// PC 0x00654B10  Xbox 0x0007A310
int CSWCMessage::HandleServerToPlayerItemScript_Add()
{
    unsigned long oidItem = ReadOBJECTIDClient();
    if (MessageReadOverflow(1))
    {
        return 0;
    }

    unsigned long nBaseItem;
    unsigned char nModelVariation;
    unsigned char nBodyVariation;
    unsigned char nTextureVariation;
    HandleServerToPlayerItemAppearance(&nBaseItem, &nModelVariation,
                                      &nBodyVariation, &nTextureVariation);
    CSWCArea* pArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pClientExoApp->GetGameObjectArray();
    CGameObject* pObject;
    CSWCItem* pItem;
    if (pObjectArray->GetGameObject(oidItem, &pObject) != CGameObjectArray::SUCCESS)
    {
        pItem = CreateItem(nBaseItem, nModelVariation, 'M');
        if (pItem == NULL)
        {
            return 0;
        }
        pItem->SetId(oidItem);
        pItem->SetArea(pArea);
        if (pObjectArray->AddExternalObject(oidItem, (CGameObject*)(CSWCObject*)pItem, 1) !=
            CGameObjectArray::SUCCESS)
        {
            return 0;
        }
    }
    else
    {
        pItem = pObject->AsSWCItem();
    }

    pItem->m_bInContainer = 0;
    pItem->SetObjectVisibilityOnSetArea(pArea);
    pItem->AttachToScene(pArea->m_pScene);
    CClientAIMaster* pAIMaster = g_pAppManager->m_pClientExoApp->GetClientAIMaster();
    pAIMaster->AddObject(pItem);
    HandleServerToPlayerActiveItemProperties(oidItem);
    if (!pItem->LoadModel(pItem->m_cModelResRef,
                          CSWCAnimBase::MODEL_SLOT_MAIN, 0))
    {
        return 0;
    }
    int nBaseItemType = pItem->m_nBaseItem;
    if (g_pRules->m_pBaseItems->GetBaseItem(nBaseItemType)->m_nPartEnvMap)
    {
        CResRef cEnvMap("default");
        pItem->SetEnvMap(CSWCAnimBase::MODEL_SLOT_MAIN, 1, cEnvMap);
    }
    return 1;
}

#include "guiingame.h"
// PC 0x0066A410  Xbox 0x0008FEA0
int CSWCMessage::HandleServerToPlayerDebugInfo(unsigned char nMinor)
{
    unsigned long oidObject = ReadOBJECTIDClient();

    if (MessageReadOverflow(1))
    {
        return 0;
    }

    CSWCObject* pObject =
        ClientExoAppLate3(g_pAppManager)->GetGameObject(oidObject);
    if (pObject != NULL)
    {
        switch (pObject->m_nObjectType)
        {
        case SW_OBJECT_TYPE_CREATURE:
            return HandleServerToPlayerDebugInfo_Creature(oidObject);
        case SW_OBJECT_TYPE_ITEM:
            return HandleServerToPlayerDebugInfo_Item(oidObject);
        case SW_OBJECT_TYPE_PLACEABLE:
            return HandleServerToPlayerDebugInfo_Placeable(oidObject);
        }
    }
    else if (ClientExoAppLate3(g_pAppManager)->GetModule()->m_pInfo->m_nId ==
             oidObject)
    {
        return 1;
    }
    return 0;
}
// PC 0x0066A4A0  Xbox 0x0008FF30
int CSWCMessage::HandleServerToPlayerGameObjectUpdate(unsigned char nMinor)
{
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    switch (nMinor)
    {
    case GAME_OBJECT_UPDATE_RECORDS:
        while (MessageMoreDataToRead())
        {
            unsigned char nUpdate = ReadBYTE(8);
            if (MessageReadOverflow(1))
            {
                return 0;
            }
            switch (nUpdate)
            {
            case GAME_OBJECT_RECORD_ADD:
                HandleServerToPlayerUpdate_Add();
                break;
            case GAME_OBJECT_RECORD_UPDATE:
                HandleServerToPlayerUpdate_Update();
                break;
            case GAME_OBJECT_RECORD_DELETE:
                HandleServerToPlayerUpdate_Delete();
                break;
            case GAME_OBJECT_RECORD_APPEARANCE:
                HandleServerToPlayerUpdate_Appearance();
                break;
            case GAME_OBJECT_RECORD_PLAYER_INFO:
                HandleServerToPlayerUpdate_PlayerInfo();
                break;
            case GAME_OBJECT_RECORD_GUI_ELEMENTS:
                HandleServerToPlayerUpdate_GuiElements();
                break;
            case GAME_OBJECT_RECORD_WORK_REMAINING:
                HandleServerToPlayerUpdate_WorkRemaining();
                break;
            default:
                EXOXBOXASSERT(0);
                return 0;
            }
        }
        return 1;
    case GAME_OBJECT_UPDATE_CONTROL_OBJECT:
    {
        unsigned long oidObject = ReadOBJECTIDClient();
        if (MessageReadOverflow(1) || MessageReadUnderflow(1))
        {
            return 0;
        }
        return HandleServerToPlayerCreatureUpdate_ControlObject(oidObject);
    }
    case GAME_OBJECT_UPDATE_VISUAL_EFFECT:
        return HandleServerToPlayerCreatureUpdate_VisEffect();
    case GAME_OBJECT_UPDATE_STRIP_EQUIPPED_ITEMS:
        return HandleServerToPlayerCreatureUpdate_StripEquippedItems();
    case GAME_OBJECT_UPDATE_DESTROY_ITEM:
        return HandleServerToPlayerItemUpdate_DestroyItem();
    case GAME_OBJECT_UPDATE_ADD_ITEM_SCRIPT:
        return HandleServerToPlayerItemScript_Add();
    default:
        return 0;
    }
}

#include "twodimarray.h"

#include "twodimarrays.h"
// PC 0x0066A640  Xbox 0x000900C0
int CSWCMessage::HandleServerToPlayerMessage(unsigned char* pMessage,
                                            unsigned long nSize)
{

    CExoString sMajor;
    unsigned char nMessageType = pMessage[0];
    unsigned char nMajor = pMessage[1];
    unsigned char nMinor = pMessage[2];
    int bResult = 0;

    SetReadMessage(pMessage + MESSAGE_HEADER_SIZE, nSize - MESSAGE_HEADER_SIZE,
                   RECIPIENT_UNSPECIFIED, 1);
    if (MessageReadOverflow(1))
    {
        return 0;
    }
    if (nMessageType != MESSAGE_PLAYER)
    {
        return 0;
    }

    sMajor.Format("unknown Major (0x%.2X)", nMajor);
    switch (nMajor)
    {
    case MESSAGE_AREA:
        sMajor = "Area";
        bResult = HandleServerToPlayerArea(nMinor);
        break;
    case MESSAGE_CAMERA:
        sMajor = "Camera";
        bResult = HandleServerToPlayerCamera(nMinor);
        break;
    case MESSAGE_DIALOG:
        sMajor = "Dialog";
        bResult = 1;
        break;
    case MESSAGE_CHAT:
        sMajor = "Chat";
        bResult = HandleServerToPlayerChat(nMinor);
        break;
    case MESSAGE_CUSTOM_TOKEN:
        sMajor = "CustomToken";
        bResult = HandleServerToPlayerSetCustomToken(nMinor);
        break;
    case MESSAGE_LOGIN:
        sMajor = "Login";
        bResult = HandleServerToPlayerLogin(nMinor);
        break;
    case MESSAGE_LOAD_BAR:
        sMajor = "LoadBar";
        bResult = HandleServerToPlayerLoadBar(nMinor);
        break;
    case MESSAGE_MODULE:
        sMajor = "Module";
        bResult = HandleServerToPlayerModule(nMinor);
        break;
    case MESSAGE_SERVER_STATUS:
        sMajor = "ServerStatus";
        bResult = HandleServerToPlayerServerStatus(nMinor);
        break;
    case MESSAGE_PLAYER_LIST:
        sMajor = "PlayerList";
        bResult = HandleServerToPlayerPlayerList(nMinor);
        break;
    case MESSAGE_GAME_OBJECT_UPDATE:
        sMajor = "GameObjUpdate";
        bResult = HandleServerToPlayerGameObjectUpdate(nMinor);
        break;
    case MESSAGE_INVENTORY:
        sMajor = "Inventory";
        bResult = HandleServerToPlayerInventory(nMinor);
        break;
    case MESSAGE_PARTY:
        sMajor = "Party";
        bResult = HandleServerToPlayerParty(nMinor);
        break;
    case MESSAGE_GUI_INVENTORY:
        sMajor = "GuiInventory";
        bResult = HandleServerToPlayerGuiInventory(nMinor);
        break;
    case MESSAGE_CHARACTER_LIST:
        sMajor = "CharList";
        bResult = 0;
        break;
    case MESSAGE_CC:
        sMajor = "ClientSideMessage";
        bResult = HandleServerToPlayerCCMessage(nMinor);
        break;
    case MESSAGE_QUICK_CHAT:
        sMajor = "QuickChat";
        bResult = 1;
        break;
    case MESSAGE_SOUND:
        sMajor = "Sound";
        bResult = HandleServerToPlayerSound(nMinor);
        break;
    case MESSAGE_COMBAT_ROUND:
        sMajor = "Combat_Round";
        bResult = HandleServerToPlayerCombatRound(nMinor);
        break;
    case MESSAGE_SAFE_PROJECTILE:
        sMajor = "SafeProjectile";
        bResult = HandleServerToPlayerSafeProjectile(nMinor);
        break;
    case MESSAGE_ITEM_PROPERTY:
        sMajor = "Item_Property";
        bResult = HandleServerToPlayerActiveItemPropertyUpdate(nMinor);
        break;
    case MESSAGE_CHEAT:
        sMajor = "Cheat";
        bResult = HandleServerToPlayerCheatMessage(nMinor);
        break;
    case MESSAGE_VOICE_CHAT:
        sMajor = "VoiceChat";
        bResult = HandleServerToPlayerVoiceChatMessage(nMinor);
        break;
    case MESSAGE_GUI_CONTAINER:
        sMajor = "GuiContainer";
        bResult = HandleServerToPlayerGuiContainerObjectMessage(nMinor);
        break;
    case MESSAGE_EXAMINE_GUI:
        sMajor = "GuiInfoPopup";
        bResult = HandleServerToPlayerGuiInfoPopupMessage(nMinor);
        break;
    case MESSAGE_JOURNAL:
        sMajor = "Journal";
        bResult = HandleServerToPlayerJournalMessage(nMinor);
        break;
    case MESSAGE_LEVEL_UP:
        sMajor = "LevelUp";
        bResult = HandleServerToPlayerLevelUpMessage(nMinor);
        break;
    case MESSAGE_GUI_QUICKBAR:
        sMajor = "GuiQuickbar";
        bResult = HandleServerToPlayerGuiQuickbar(nMinor);
        break;
    case MESSAGE_MAP_PIN:
        sMajor = "MapPin";
        bResult = HandleServerToPlayerMapPin(nMinor);
        break;
    case MESSAGE_DEBUG_INFO:
        sMajor = "DebugInfo";
        bResult = HandleServerToPlayerDebugInfo(nMinor);
        break;
    case MESSAGE_POP_UP_GUI:
        sMajor = "PopUpGUIPanel";
        bResult = HandleServerToPlayerPopUpGUIPanel(nMinor);
        break;
    case MESSAGE_AMBIENT:
        sMajor = "Ambient";
        bResult = HandleServerToPlayerAmbient(nMinor);
        break;
    case MESSAGE_CHARACTER_DOWNLOAD:
        sMajor = "Character_Download";
        bResult = HandleServerToPlayerCharacterDownload(nMinor);
        break;
    case MESSAGE_SAVE_LOAD:
        sMajor = "SaveLoad";
        bResult = HandleServerToPlayerSaveLoad(nMinor);
        break;
    case MESSAGE_SHUTDOWN_SERVER:
        sMajor = "ShutDownServer";
        bResult = HandlePlayerToServerShutDownServer(nMinor);
        break;
    case MESSAGE_GUI_TIMING_EVENT:
        sMajor = "LevelUp";
        bResult = HandleServerToPlayerGuiTimingEvent(nMinor);
        break;
    case MESSAGE_GUI_CHARACTER_SHEET:
        sMajor = "GUICharacterSheet";
        bResult = HandleServerToPlayerGUICharacterSheet(nMinor);
        break;
    case MESSAGE_PLAY_MODULE_CHARACTER_LIST:
        sMajor = "PlayModuleCharacterList";
        bResult = 1;
        break;
    }

    CExoString sError;
    if (MessageReadOverflow(0))
    {
        sError.Format("Client message OVERFLOW in %s (0x%.2X)\n"
                      "FIX this or FLOG the person responsible!",
                      sMajor.CStr(), nMinor);
    }
    if (MessageReadUnderflow(0))
    {
        sError.Format("Client message UNDERFLOW in %s (0x%.2X)\n"
                      "FIX this or FLOG the person responsible!",
                      sMajor.CStr(), nMinor);
    }
    return bResult;
}

static inline CClientExoApp* ClientExoAppLate3(CAppManager* p)
{
    return ClientExoAppLate2(p);
}

static inline CClientExoApp* ClientExoAppLate1(CAppManager* p)
{
    return p->m_pClientExoApp;
}

static inline void NwchandlemessageTrace2()
{
    NwchandlemessageTrace1();
}

static inline void NwchandlemessageTrace1()
{
}

static inline CClientExoApp* ClientExoAppLate2(CAppManager* p)
{
    return ClientExoAppLate1(p);
}

static inline void NwchandlemessageTrace3()
{
    NwchandlemessageTrace2();
}

#ifdef _XBOX
// PC 0x0064FC10  Xbox 0x000753B0
int CSWCMessage::HandleServerToPlayerUpdate_GuiInventory()
{
    unsigned char nMode = ReadBYTE(8);
    if (MessageReadOverflow(1))
    {
        return 0;
    }

    if (nMode == GUI_INVENTORY_RECORD_ADD)
    {
        unsigned long nSlot = ReadDWORD(32);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        CSWCItem* pItem;
        if (!HandleServerToPlayerItemUpdate_Add(1, &pItem))
        {
            return 0;
        }
        pItem->m_nEquipSlot = nSlot;
        pItem->m_nInventorySlot = INVENTORY_SLOT_INVALID;
        CSWCCreature* pCreature =
            g_pAppManager->m_pClientExoApp->GetPlayerCreature();
        if (pCreature != NULL)
        {
            if (pCreature->GetEquippedItemID(nSlot) != pItem->GetId())
                pCreature->EquipToInventorySlot(nSlot, pItem);
        }
        return 1;
    }
    else if (nMode == GUI_INVENTORY_RECORD_DELETE)
    {
        ReadDWORD(32);
        if (MessageReadOverflow(1))
        {
            return 0;
        }
        return 1;
    }
    return 0;
}
#endif

