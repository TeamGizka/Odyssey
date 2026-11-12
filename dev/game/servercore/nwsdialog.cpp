#include "exobase.h"
#include "exoresman.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "partytables.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "nwscript.h"
#include "nwvmachineinternal.h"
#include "nwsobject.h"
#include "nwsarea.h"
#include "nwsmodule.h"
#include "nwsplayer.h"
#include "nwsplaceable.h"
#include "nwcobject.h"
#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "nwsdialog.h"
#include "nwmessage.h"
#include "nwscreature.h"
#include "nwsparty.h"
#include "nwjournal.h"
#include "worldtimer.h"
#include "swparty.h"
#include "swmain.h"
#include "exotlktable.h"
#include <math.h>
#include <stdio.h>
#include "clientoptions.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"

static inline void NwsdialogTrace1();
// PC 0x005E6980
inline CSWSDialogNode::CSWSDialogNode()
{
}
// PC 0x005E69F0
inline CSWSDialogNode::~CSWSDialogNode()
{
}
// PC 0x005E6A50
inline CSWSDialogReply::CSWSDialogReply()
{
    m_nEntries = 0;
    m_pEntries = NULL;
    m_nReplyIndex = -1;
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
// PC 0x005E56D0
inline CSWSDialogBase::CSWSDialogBase()
    : m_nWaitFlags(0),
      m_nDelay(0),
      m_bSoundActive(0),
      m_nSoundIndex(0),
      m_nQuestEntry(0),
      m_nAnimationCount(0),
      m_pAnimations(NULL),
      m_bSoundExists(0),
      m_nAlienRaceNode(0),
      m_nEmotion(4),
      m_nFacialAnim(0)
{
}
// PC 0x005E57C0
CSWSDialogActiveEntry::CSWSDialogActiveEntry()
    : m_nIndex(0)
{
}
// PC 0x005E5820
CSWSDialogActiveEntry::~CSWSDialogActiveEntry()
{
}
// PC 0x005E6960
inline CSWSDialogLinkEntry::CSWSDialogLinkEntry()
    : m_bDisplayInactive(0)
{
}
// PC 0x005E5870
inline CSWSDialogBase::~CSWSDialogBase()
{
}
// PC 0x005E5900
inline CSWSDialogSpeaker::CSWSDialogSpeaker()
    : m_oidSpeaker(OBJECT_INVALID)
{
}
// PC 0x005E5920
void CSWSDialog::ClearDialogOwnerInObject(unsigned long oidObject)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CGameObject* pObject;

    if (pServer->GetObjectArray()->GetGameObject(oidObject, &pObject) ==
        CGameObjectArray::SUCCESS)
    {
        CSWSObject* pServerObject = (CSWSObject*)pObject;

        if (pServerObject->m_oidDialogOwner == m_oidDialogOwner)
        {
            pServerObject->SetDialogOwner(OBJECT_INVALID);
            if (pServerObject->AsSWSCreature() != NULL)
            {
                pServerObject->AsSWSCreature()->SetActivity(4, 0, 0);
            }
        }
    }
}
// PC 0x005EA490
void CSWSDialog::Cleanup()
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CGameObject* pOwner = pServer->GetGameObject(m_oidDialogOwner);
    if (pOwner != NULL)
    {
        if (pOwner->AsSWSObject() != NULL)
        {
            int nAILevel = m_nOwnerAILevel;
            pOwner->AsSWSObject()->m_nAILevel = nAILevel;
            pOwner->AsSWSObject()->StopSoundPlayingInDialog();
        }
        int nSavedAILevel = pOwner->AsSWSObject()->m_c7C;
        if (nSavedAILevel != -1)
        {
            pOwner->AsSWSObject()->m_nAILevel = nSavedAILevel;
            pOwner->AsSWSObject()->m_c7C = 0xFF;
        }
    }

    ClearDialogOwnerInObject(m_oidDialogOwner);
    if (m_lstPlayers.IsEmpty())
    {
        ClearDialogOwnerInObject(m_oidCurrentPlayer);
    }
    while (!m_lstPlayers.IsEmpty())
    {
        CSWSDialogPlayer* pPlayer = m_lstPlayers.RemoveHead();
        g_pAppManager->m_pClientExoApp->GetInGameGui()->CloseDialog();
        ClearDialogOwnerInObject(pPlayer->m_oidPlayer);
        delete pPlayer;
    }

    if (m_pSpeakers != NULL)
    {
        for (unsigned long nSpeaker = 0; nSpeaker < m_nSpeakers; ++nSpeaker)
        {
            ClearDialogOwnerInObject(m_pSpeakers[nSpeaker].m_oidSpeaker);
        }
        delete [] m_pSpeakers;
        m_pSpeakers = NULL;
        m_nSpeakers = 0;
    }

    if (m_pEntries != NULL)
    {
        for (unsigned long nEntry = 0; nEntry < (unsigned long)m_nEntries; ++nEntry)
        {
            if (m_pEntries[nEntry].m_pReplies != NULL)
            {
                delete [] m_pEntries[nEntry].m_pReplies;
                m_pEntries[nEntry].m_pReplies = NULL;
            }
            if (m_pEntries[nEntry].m_pAnimations != NULL)
            {
                delete [] m_pEntries[nEntry].m_pAnimations;
                m_pEntries[nEntry].m_pAnimations = NULL;
                m_pEntries[nEntry].m_nAnimationCount = 0;
            }
        }
        delete [] m_pEntries;
        m_pEntries = NULL;
        m_nEntries = 0;
    }

    if (m_pReplies != NULL)
    {
        for (unsigned long nReply = 0; nReply < (unsigned long)m_nReplies; ++nReply)
        {
            if (m_pReplies[nReply].m_pEntries != NULL)
            {
                delete [] m_pReplies[nReply].m_pEntries;
                m_pReplies[nReply].m_pEntries = NULL;
            }
            if (m_pReplies[nReply].m_pAnimations != NULL)
            {
                delete [] m_pReplies[nReply].m_pAnimations;
                m_pReplies[nReply].m_pAnimations = NULL;
                m_pReplies[nReply].m_nAnimationCount = 0;
            }
        }
        delete [] m_pReplies;
        m_pReplies = NULL;
        m_nReplies = 0;
    }

    if (m_pStartingEntries != NULL)
    {
        delete [] m_pStartingEntries;
        m_pStartingEntries = NULL;
        m_nStartingEntries = 0;
    }
    m_bEndOfDialog = 0;
    m_nCurrentEntry = 0;
    m_bRepliesSent = 0;
    m_nSelectedReply = DIALOG_INDEX_INVALID;
    m_oidDialogOwner = OBJECT_INVALID;
    m_oidCurrentPlayer = OBJECT_INVALID;
    m_nCurrentPlayerGender = CTlkTable::GENDER_MALE;
    m_bMultiPCDialogViewOnly = 0;
    m_n90 = 0;
}
// PC 0x005E59A0
CSWSDialogStuntModel::CSWSDialogStuntModel()
{
}
// PC 0x0074C290
CSWSDialogAnimation::CSWSDialogAnimation()
{
}
// PC 0x005E59F0
void CSWSDialog::LoadDialogCamera(CSWSDialogCamera& camera, CResGFF* pGFF,
                                  CResStruct& resStruct, int& bSuccess)
{
    int bFieldSuccess;
    {
        CExoString sEmpty = "";
        camera.m_sListener = pGFF->ReadFieldCExoString(&resStruct, "Listener",
                                                       bFieldSuccess, sEmpty);
    }
    camera.m_nCameraAngle =
        pGFF->ReadFieldDWORD(&resStruct, "CameraAngle", bFieldSuccess, 0);
    camera.m_nCameraId =
        pGFF->ReadFieldINT(&resStruct, "CameraID", bFieldSuccess, 0);
    camera.m_fCameraHeightOffset = pGFF->ReadFieldFLOAT(
        &resStruct, "CamHeightOffset", bFieldSuccess, 0.0f);
    camera.m_fTargetHeightOffset = pGFF->ReadFieldFLOAT(
        &resStruct, "TarHeightOffset", bFieldSuccess, 0.0f);
    camera.m_nCameraAnimation =
        pGFF->ReadFieldWORD(&resStruct, "CameraAnimation", bFieldSuccess, 0);
    camera.m_nCameraVideoEffect =
        pGFF->ReadFieldINT(&resStruct, "CamVidEffect", bFieldSuccess, -1);
    camera.m_fCameraFieldOfView =
        pGFF->ReadFieldFLOAT(&resStruct, "CamFieldOfView", bFieldSuccess, 0.0f);

    if (bFieldSuccess == 0 || camera.m_fCameraFieldOfView <= 0.0f)
    {
        camera.m_fCameraFieldOfView = -1.0f;
    }
    if (camera.m_nCameraAngle != 6)
    {
        camera.m_nCameraId = -1;
    }
}
// PC 0x005E5B60
void CSWSDialog::LoadDialogLinkedNode(CSWSDialogActiveEntry& link, CResGFF* pGFF,
                                      CResStruct& resStruct, int& bSuccess,
                                      unsigned long nReplies)
{
    link.m_cActive =
        pGFF->ReadFieldCResRef(&resStruct, "Active", bSuccess, "");
    link.m_nParam1 = pGFF->ReadFieldINT(&resStruct, "Param1", bSuccess, 0);
    link.m_nParam2 = pGFF->ReadFieldINT(&resStruct, "Param2", bSuccess, 0);
    link.m_nParam3 = pGFF->ReadFieldINT(&resStruct, "Param3", bSuccess, 0);
    link.m_nParam4 = pGFF->ReadFieldINT(&resStruct, "Param4", bSuccess, 0);
    link.m_nParam5 = pGFF->ReadFieldINT(&resStruct, "Param5", bSuccess, 0);
    link.m_cActive2 =
        pGFF->ReadFieldCResRef(&resStruct, "Active2", bSuccess, "");
    link.m_nParam1b = pGFF->ReadFieldINT(&resStruct, "Param1b", bSuccess, 0);
    link.m_nParam2b = pGFF->ReadFieldINT(&resStruct, "Param2b", bSuccess, 0);
    link.m_nParam3b = pGFF->ReadFieldINT(&resStruct, "Param3b", bSuccess, 0);
    link.m_nParam4b = pGFF->ReadFieldINT(&resStruct, "Param4b", bSuccess, 0);
    link.m_nParam5b = pGFF->ReadFieldINT(&resStruct, "Param5b", bSuccess, 0);
    link.m_bNot = pGFF->ReadFieldBYTE(&resStruct, "Not", bSuccess, 0);
    link.m_bNot2 = pGFF->ReadFieldBYTE(&resStruct, "Not2", bSuccess, 0);
    link.m_nLogic = pGFF->ReadFieldINT(&resStruct, "Logic", bSuccess, 0);
    link.m_sParamStrA =
        pGFF->ReadFieldCExoString(&resStruct, "ParamStrA", bSuccess, "");
    link.m_sParamStrB =
        pGFF->ReadFieldCExoString(&resStruct, "ParamStrB", bSuccess, "");
    link.m_nIndex = pGFF->ReadFieldDWORD(&resStruct, "Index", bSuccess, 0);
    if (link.m_nIndex > nReplies)
    {
        bSuccess = 0;
    }
}
// PC 0x005E5DE0
int CSWSDialog::CheckScript(CSWSObject* pObject, const CResRef& cScript)
{
    if (cScript == "")
    {
        return 1;
    }
    CExoString sScript;
    unsigned long oidSelf;
    int bResult;

    cScript.CopyToString(sScript);
    oidSelf = pObject->m_nId;
    bResult = g_pVirtualMachine->RunScript(&sScript, oidSelf, 1);
    if (bResult == 1)
    {
        int nType;
        void* pValue;

        if (g_pVirtualMachine->GetRunScriptReturnValue(&nType, &pValue) &&
            nType == VM_STACK_INTEGER)
        {
            bResult = (pValue != NULL);
        }
        else
        {
            bResult = 0;
        }
    }
    return bResult;
}
// PC 0x005E5EC0
void CSWSDialog::RunScript(CSWSObject* pObject, const CResRef& cScript)
{
    if (cScript == "")
    {
        return;
    }
    CExoString sScript;
    unsigned long oidSelf;

    cScript.CopyToString(sScript);
    oidSelf = pObject->m_nId;
    g_pVirtualMachine->RunScript(&sScript, oidSelf, 1);
}
// PC 0x005E5F50
int CSWSDialog::GetStartEntryOneLiner(CSWSObject* pOwner,
                                      CExoLocString& lsText,
                                      CResRef& cSound, CResRef& cScript)
{
    unsigned long nStart;
    for (nStart = 0; nStart < (unsigned long)m_nStartingEntries; ++nStart)
    {
        if (m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_sSpeaker != "")
        {
            continue;
        }

        int bActive1;
        int bHasActive1;
        if (m_pStartingEntries[nStart].m_cActive == "")
        {
            bActive1 = 1;
            bHasActive1 = 0;
        }
        else
        {
            bHasActive1 = 1;
            g_pVirtualMachine->SetScriptParams(
                m_pStartingEntries[nStart].m_nParam1,
                m_pStartingEntries[nStart].m_nParam2,
                m_pStartingEntries[nStart].m_nParam3,
                m_pStartingEntries[nStart].m_nParam4,
                m_pStartingEntries[nStart].m_nParam5,
                m_pStartingEntries[nStart].m_sParamStrA);
            bActive1 = CheckScript(pOwner, m_pStartingEntries[nStart].m_cActive);
            if (m_pStartingEntries[nStart].m_bNot)
            {
                bActive1 = !bActive1;
            }
        }

        int bActive2;
        int bHasActive2;
        if (m_pStartingEntries[nStart].m_cActive2 == "")
        {
            bActive2 = 1;
            bHasActive2 = 0;
        }
        else
        {
            bHasActive2 = 1;
            g_pVirtualMachine->SetScriptParams(
                m_pStartingEntries[nStart].m_nParam1b,
                m_pStartingEntries[nStart].m_nParam2b,
                m_pStartingEntries[nStart].m_nParam3b,
                m_pStartingEntries[nStart].m_nParam4b,
                m_pStartingEntries[nStart].m_nParam5b,
                m_pStartingEntries[nStart].m_sParamStrB);
            bActive2 = CheckScript(pOwner, m_pStartingEntries[nStart].m_cActive2);
            if (m_pStartingEntries[nStart].m_bNot2)
            {
                bActive2 = !bActive2;
            }
        }

        int bActive;
        if (!bHasActive1)
        {
            if (bHasActive2)
            {
                bActive = bActive2;
            }
            else
            {
                bActive = 1;
            }
        }
        else if (!bHasActive2)
        {
            bActive = bActive1;
        }
        else if (m_pStartingEntries[nStart].m_nLogic == CSWSDialogActiveEntry::LOGIC_AND)
        {
            bActive = bActive1 && bActive2;
        }
        else
        {
            bActive = bActive1 || bActive2;
        }

        if (!bActive ||
            m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nReplies != 0)
        {
            continue;
        }
        lsText = m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_lsText;
        cSound = m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_cSoundResRef;
        cScript = m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_cScript;
        return 1;
    }
    return 0;
}
// PC 0x005E61A0
void CSWSDialog::AddJournalEntry(const CExoString& sPlotID,
                                unsigned long nState, unsigned long nPlayerId)
{
    if (sPlotID.IsEmpty())
    {
        return;
    }
    CSWSPlayer* pClient = g_pAppManager->GetServerExoApp()
        ->GetClientObjectByPlayerId(nPlayerId, 0);
    if (pClient == NULL || pClient->AsSWSPlayer() == NULL)
    {
        return;
    }
    CSWSObject* pObject = pClient->AsSWSPlayer()->GetGameObject();
    if (pObject == NULL || pObject->AsSWSCreature() == NULL)
    {
        return;
    }
    CSWSCreature* pCreature = pObject->AsSWSCreature();
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()
        ->GetWorldTime(&nCalendarDay, &nTimeOfDay);
    if (pCreature != NULL &&
        pCreature->GetJournal()->GetState(sPlotID) < (int)nState)
    {
        pCreature->GetJournal()->SetDate(sPlotID, nCalendarDay, 0);
        pCreature->GetJournal()->SetTime(sPlotID, nTimeOfDay, 0);
        pCreature->GetJournal()->SetState(sPlotID, nState, 0);
        int nExperience = pCreature->GetJournal()->GetQuestEntryXP(sPlotID);
        if (nExperience != 0)
        {
            if (g_pAppManager->GetServerExoApp()->GetPartyTable() != NULL)
            {
                g_pAppManager->GetServerExoApp()->GetPartyTable()
                    ->DistributeExperience(nExperience, 1);
            }
            g_pAppManager->GetClientExoApp()->GetInGameGui()
                ->UpdateStatus(GUI_STATUS_XP, nExperience);
        }
    }
    CSWSFaction* pFaction = pCreature->GetFaction();
    if (pFaction != NULL)
    {
        int nMembers = pFaction->m_lstMembers.num;
        unsigned long* pMembers = pFaction->m_lstMembers.element;
        if (pMembers != NULL)
        {
            for (int nMember = 0; nMember < nMembers; nMember++)
            {
                CSWSCreature* pMember = g_pAppManager->GetServerExoApp()
                    ->GetCreatureByGameObjectID(pMembers[nMember]);
                if (pMember != NULL && pMember != pCreature &&
                    pMember->GetJournal()->GetState(sPlotID) < (int)nState)
                {
                    pMember->GetJournal()->SetDate(sPlotID, nCalendarDay, 0);
                    pMember->GetJournal()->SetTime(sPlotID, nTimeOfDay, 0);
                    pMember->GetJournal()->SetState(sPlotID, nState, 0);
                    int nExperience = pMember->GetJournal()->GetQuestEntryXP(sPlotID);
                    if (nExperience != 0)
                    {
                        g_pAppManager->GetClientExoApp()->GetInGameGui()
                            ->UpdateStatus(GUI_STATUS_XP, nExperience);
                    }
                }
            }
        }
    }
}
// PC 0x005E63F0
inline CSWSDialogBase& CSWSDialogBase::operator=(const CSWSDialogBase& dialogBase)
{
    m_sSpeaker = dialogBase.m_sSpeaker;
    m_lsText = dialogBase.m_lsText;
    m_cScript = dialogBase.m_cScript;
    m_nActionParam1 = dialogBase.m_nActionParam1;
    m_nActionParam2 = dialogBase.m_nActionParam2;
    m_nActionParam3 = dialogBase.m_nActionParam3;
    m_nActionParam4 = dialogBase.m_nActionParam4;
    m_nActionParam5 = dialogBase.m_nActionParam5;
    m_cScript2 = dialogBase.m_cScript2;
    m_nActionParam1b = dialogBase.m_nActionParam1b;
    m_nActionParam2b = dialogBase.m_nActionParam2b;
    m_nActionParam3b = dialogBase.m_nActionParam3b;
    m_nActionParam4b = dialogBase.m_nActionParam4b;
    m_nActionParam5b = dialogBase.m_nActionParam5b;
    m_sActionParamStrA = dialogBase.m_sActionParamStrA;
    m_sActionParamStrB = dialogBase.m_sActionParamStrB;
    m_bNodeUnskippable = dialogBase.m_bNodeUnskippable;
    m_nWaitFlags = dialogBase.m_nWaitFlags;
    m_nDelay = dialogBase.m_nDelay;
    m_nSoundDelay = dialogBase.m_nSoundDelay;
    m_bSoundActive = dialogBase.m_bSoundActive;
    m_nSoundIndex = dialogBase.m_nSoundIndex;
    m_cSoundResRef = dialogBase.m_cSoundResRef;
    m_sAnimation = dialogBase.m_sAnimation;
    m_sQuest = dialogBase.m_sQuest;
    m_nQuestEntry = dialogBase.m_nQuestEntry;
    m_nPlotIndex = dialogBase.m_nPlotIndex;
    m_fPlotXPPercentage = dialogBase.m_fPlotXPPercentage;
    m_nAnimationCount = dialogBase.m_nAnimationCount;
    m_pAnimations = dialogBase.m_pAnimations;
    m_nFadeType = dialogBase.m_nFadeType;
    m_vFadeColor = dialogBase.m_vFadeColor;
    m_fFadeLength = dialogBase.m_fFadeLength;
    m_fFadeDelay = dialogBase.m_fFadeDelay;
    m_bSoundExists = dialogBase.m_bSoundExists;
    m_nAlienRaceNode = dialogBase.m_nAlienRaceNode;
    m_nEmotion = dialogBase.m_nEmotion;
    m_nFacialAnim = dialogBase.m_nFacialAnim;
    return *this;
}
// PC 0x005E65A0
void CSWSDialog::InitializePsuedoRandomCameraAngles(CResGFF* pResGFF)
{
    CResRef cResRef;
    CExoString sResRef;
    unsigned short nType;

    pResGFF->GetResRef(cResRef, nType);
    cResRef.CopyToString(sResRef);

    unsigned int nHash = 0;
    for (int nIndex = 0; nIndex < sResRef.GetLength(); nIndex++)
    {
        int nCharacter = (char)sResRef[nIndex];
        int nPower = (unsigned __int64)pow(10.0, (double)nIndex) + 1;
        nHash += nCharacter * nPower;
    }

    m_nPseudoRandomCameraAngle = nHash % PSEUDO_RANDOM_CAMERA_COUNT;
    m_nPseudoRandomCameraStep = nHash % 5 + 1;
    m_bPseudoRandomCameraDirection = nHash & 1;
}
// PC 0x005E66A0
void CSWSDialog::InitializePseudoRandomAlienVOVariation(CResGFF* pResGFF)
{
    CResRef cResRef;
    CExoString sResRef;
    unsigned short nType;

    pResGFF->GetResRef(cResRef, nType);
    cResRef.CopyToString(sResRef);

    unsigned int nHash = 0;
    for (int nIndex = 0; nIndex < sResRef.GetLength(); nIndex++)
    {
        int nCharacter = (char)sResRef[nIndex];
        int nPower = (unsigned __int64)pow(10.0, (double)nIndex) + 1;
        nHash += nCharacter * nPower;
    }

    m_nPseudoRandomAlienVO = nHash % PSEUDO_RANDOM_ALIEN_VO_COUNT;
    m_nPseudoRandomAlienVOStep = nHash % 5 + 1;
    m_bPseudoRandomAlienVODirection = nHash & 1;
}
// PC 0x005E67A0
CExoString CSWSDialog::GetExoStringFromLocString(CExoLocString& locString,
                                                 unsigned char nGender)
{
    unsigned long oidPlayer =
        g_pAppManager->m_pServerExoApp->GetPlayerCreatureId();
    CExoString sString;

    locString.GetString(g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                        &sString, nGender);
    return sString;
}
// PC 0x005E6840
void CSWSDialog::GiveDialogPlotXP(int nPlotIndex, float fPercent)
{
    if (nPlotIndex != CSWSDialogBase::PLOT_INDEX_INVALID && fPercent > 0.0f &&
        g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetPartyTable()->GivePlotXP(
            nPlotIndex, (int)ceil(fPercent * 100.0f));
    }
}
// PC 0x005E68B0
int CSWSDialog::SetDialogOwner(CSWSObject* pOwner)
{
    int nResult = (int)pOwner;
    int nDialogState = pOwner->m_nAILevel;

    m_nOwnerAILevel = nDialogState;
    if (nDialogState < 3)
    {
        pOwner->m_nAILevel = 3;
        nResult = pOwner->m_nId;
        m_oidDialogOwner = nResult;
    }
    else
    {
        m_oidDialogOwner = pOwner->m_nId;
    }
    return nResult;
}
// PC 0x005E6A90
void CSWSDialog::LoadDialogBase(CSWSDialogBase& dialogBase, CResGFF* pGFF,
                                CResStruct& resStruct, unsigned long& nDelay,
                                int& bSuccess)
{
    NwsdialogTrace1();
    int bFieldSuccess = 0;
    int bFadeTypeSuccess = 0;
    CResStruct animationStruct;
    dialogBase.m_lsText = pGFF->ReadFieldCExoLocString(
        &resStruct, "Text", bSuccess, CExoLocString());
    {
        CResRef cEmpty = "";
        dialogBase.m_cScript = pGFF->ReadFieldCResRef(
            &resStruct, "Script", bFieldSuccess, cEmpty);
    }
    dialogBase.m_nActionParam1 = pGFF->ReadFieldINT(&resStruct, "ActionParam1", bFieldSuccess, 0);
    dialogBase.m_nActionParam2 = pGFF->ReadFieldINT(&resStruct, "ActionParam2", bFieldSuccess, 0);
    dialogBase.m_nActionParam3 = pGFF->ReadFieldINT(&resStruct, "ActionParam3", bFieldSuccess, 0);
    dialogBase.m_nActionParam4 = pGFF->ReadFieldINT(&resStruct, "ActionParam4", bFieldSuccess, 0);
    dialogBase.m_nActionParam5 = pGFF->ReadFieldINT(&resStruct, "ActionParam5", bFieldSuccess, 0);
    {
        CResRef cEmpty = "";
        dialogBase.m_cScript2 = pGFF->ReadFieldCResRef(
            &resStruct, "Script2", bFieldSuccess, cEmpty);
    }
    dialogBase.m_nActionParam1b = pGFF->ReadFieldINT(&resStruct, "ActionParam1b", bFieldSuccess, 0);
    dialogBase.m_nActionParam2b = pGFF->ReadFieldINT(&resStruct, "ActionParam2b", bFieldSuccess, 0);
    dialogBase.m_nActionParam3b = pGFF->ReadFieldINT(&resStruct, "ActionParam3b", bFieldSuccess, 0);
    dialogBase.m_nActionParam4b = pGFF->ReadFieldINT(&resStruct, "ActionParam4b", bFieldSuccess, 0);
    dialogBase.m_nActionParam5b = pGFF->ReadFieldINT(&resStruct, "ActionParam5b", bFieldSuccess, 0);
    {
        CExoString sEmpty = "";
        dialogBase.m_sActionParamStrA = pGFF->ReadFieldCExoString(
            &resStruct, "ActionParamStrA", bFieldSuccess, sEmpty);
    }
    {
        CExoString sEmpty = "";
        dialogBase.m_sActionParamStrB = pGFF->ReadFieldCExoString(
            &resStruct, "ActionParamStrB", bFieldSuccess, sEmpty);
    }
    dialogBase.m_bNodeUnskippable = pGFF->ReadFieldINT(
        &resStruct, "NodeUnskippable", bFieldSuccess, 0);
    {
        CExoString sEmpty = "";
        dialogBase.m_sSpeaker = pGFF->ReadFieldCExoString(
            &resStruct, "Speaker", bFieldSuccess, sEmpty);
    }
    dialogBase.m_nWaitFlags = pGFF->ReadFieldDWORD(
        &resStruct, "WaitFlags", bFieldSuccess, 0);
    {
        CExoString sEmpty = "";
        dialogBase.m_sQuest = pGFF->ReadFieldCExoString(
            &resStruct, "Quest", bFieldSuccess, sEmpty);
    }
    dialogBase.m_nQuestEntry = pGFF->ReadFieldDWORD(
        &resStruct, "QuestEntry", bFieldSuccess, 0);
    dialogBase.m_nPlotIndex = pGFF->ReadFieldINT(
        &resStruct, "PlotIndex", bFieldSuccess, 0);
    dialogBase.m_fPlotXPPercentage = pGFF->ReadFieldFLOAT(
        &resStruct, "PlotXPPercentage", bFieldSuccess, 0.0f);
    dialogBase.m_nDelay = pGFF->ReadFieldDWORD(
        &resStruct, "Delay", bFieldSuccess, 0);
    dialogBase.m_nFadeType = pGFF->ReadFieldBYTE(
        &resStruct, "FadeType", bFadeTypeSuccess, CSWSDialogBase::FADE_TYPE_NONE);
    {
        Vector vBlack(0.0f, 0.0f, 0.0f);
        dialogBase.m_vFadeColor = pGFF->ReadFieldVector(
            &resStruct, "FadeColor", bFieldSuccess, vBlack);
    }
    dialogBase.m_fFadeDelay = pGFF->ReadFieldFLOAT(
        &resStruct, "FadeDelay", bFieldSuccess, 0.0f);
    dialogBase.m_fFadeLength = pGFF->ReadFieldFLOAT(
        &resStruct, "FadeLength", bFieldSuccess, 0.0f);
    dialogBase.m_nSoundDelay = CSWSDialogBase::DELAY_DEFAULT;
    dialogBase.m_cSoundResRef = "";
    CResRef cVOResRef = pGFF->ReadFieldCResRef(
        &resStruct, "VO_ResRef", bFieldSuccess, "");
    dialogBase.m_bSoundExists = pGFF->ReadFieldBYTE(
        &resStruct, "SoundExists", bFieldSuccess,
        CSWSDialogBase::SOUND_EXISTS_UNCHECKED);
    if (!dialogBase.m_cSoundResRef.IsValid())
    {
        dialogBase.m_nAlienRaceNode = pGFF->ReadFieldINT(
            &resStruct, "AlienRaceNode", bFieldSuccess, 0);
        dialogBase.m_nEmotion = pGFF->ReadFieldINT(
            &resStruct, "Emotion", bFieldSuccess, 0);
        dialogBase.m_nFacialAnim = pGFF->ReadFieldINT(
            &resStruct, "FacialAnim", bFieldSuccess, 0);
        int nAlienRace;
        if (dialogBase.m_sSpeaker == "" && dialogBase.m_nAlienRaceNode <= 0)
            nAlienRace = m_nAlienRaceOwner;
        else
            nAlienRace = dialogBase.m_nAlienRaceNode;
        if (nAlienRace > 0)
        {
            int nRace = nAlienRace;
            CExoString sEmotion;
            CExoString sAlienVO;
            int bFound = g_pRules->m_p2DArrays->m_pEmotion->GetCExoStringEntry(
                dialogBase.m_nEmotion, CExoString("LABEL"), &sEmotion);
            if (bFound &&
                g_pRules->m_p2DArrays->m_pAlienVO->GetCExoStringEntry(
                    nRace, sEmotion, &sAlienVO))
            {
                dialogBase.m_cSoundResRef = sAlienVO;
                if (dialogBase.m_cSoundResRef.IsValid())
                    dialogBase.m_bSoundExists |= CSWSDialogBase::SOUND_EXISTS_PRESENT |
                                                 CSWSDialogBase::SOUND_EXISTS_SKIP;
            }
        }
    }
    if (!dialogBase.m_cSoundResRef.IsValid())
    {
        dialogBase.m_cSoundResRef = cVOResRef;
        if (!dialogBase.m_cSoundResRef.IsValid())
        {
            dialogBase.m_bSoundExists = 0;
            dialogBase.m_bSoundActive = 1;
        }
    }
    else
    {
        dialogBase.m_bSoundActive = 1;
    }
    if (dialogBase.m_nDelay == CSWSDialogBase::DELAY_DEFAULT)
    {
        if (!dialogBase.m_cSoundResRef.IsValid())
        {
            if (dialogBase.m_nWaitFlags == 0)
            {
                dialogBase.m_cSoundResRef = "";
            }
            else
            {
                dialogBase.m_nSoundDelay = 0;
                dialogBase.m_nDelay = nDelay;
            }
        }
        else
        {
            dialogBase.m_nDelay = nDelay;
            if (dialogBase.m_nWaitFlags == 0)
            {
                dialogBase.m_nWaitFlags = CSWSDialogBase::WAIT_FOR_SOUND;
            }
        }
    }
    else
    {
        dialogBase.m_nWaitFlags |= CSWSDialogBase::WAIT_EXPLICIT_DELAY;
    }
    if (dialogBase.m_nFadeType == CSWSDialogBase::FADE_TYPE_IN ||
        dialogBase.m_nFadeType == CSWSDialogBase::FADE_TYPE_OUT)
    {
        dialogBase.m_fFadeLength = 0.0f;
    }
    if (dialogBase.m_nFadeType == CSWSDialogBase::FADE_TYPE_NONE ||
        bFadeTypeSuccess == 0)
    {
        dialogBase.m_fFadeDelay = 0.0f;
        dialogBase.m_fFadeLength = 0.0f;
        Vector vBlack(0.0f, 0.0f, 0.0f);
        dialogBase.m_nFadeType = CSWSDialogBase::FADE_TYPE_NONE;
        dialogBase.m_vFadeColor = vBlack;
    }
    CResList animationList;
    pGFF->GetList(&animationList, &resStruct, "AnimList");
    dialogBase.m_nAnimationCount = pGFF->GetListCount(&animationList);
    if ((unsigned long)dialogBase.m_nAnimationCount > 0)
    {
        dialogBase.m_pAnimations =
            new CSWSDialogAnimation[dialogBase.m_nAnimationCount];
    }
    for (unsigned long nAnimation = 0;
         nAnimation < (unsigned long)dialogBase.m_nAnimationCount; ++nAnimation)
    {
        pGFF->GetListElement(&animationStruct, &animationList, nAnimation);
        {
            CExoString sEmpty = "";
            dialogBase.m_pAnimations[nAnimation].m_sParticipant =
                pGFF->ReadFieldCExoString(&animationStruct, "Participant",
                                          bSuccess, sEmpty);
        }
        dialogBase.m_pAnimations[nAnimation].m_nAnimation =
            pGFF->ReadFieldWORD(&animationStruct, "Animation", bSuccess, 0);
    }
}
// PC 0x005E73D0
int CSWSDialog::RemovePlayer(unsigned long oidPlayer)
{
    CExoLinkedListNode* pPosition = m_lstPlayers.GetHeadPos();
    int bFirstPlayer = 1;
    CSWSDialogPlayer* pPlayer = m_lstPlayers.GetHead();

    while (pPlayer != NULL)
    {
        if (pPlayer->m_oidPlayer == oidPlayer)
        {
            if (m_lstPlayers.Count() == 1 ||
                (m_bMultiPCDialogViewOnly != 0 && bFirstPlayer != 0))
            {
                return 0;
            }

            m_lstPlayers.Remove(pPosition);
            if (pPlayer->m_oidPlayer == m_oidCurrentPlayer)
            {
                delete pPlayer;
                CSWSDialogPlayer* pNewPlayer = m_lstPlayers.GetHead();
                if (pNewPlayer != NULL)
                {
                    m_oidCurrentPlayer = pNewPlayer->m_oidPlayer;
                    m_nCurrentPlayerGender = pNewPlayer->m_nGender;
                }
            }
            else
            {
                delete pPlayer;
            }
            return 1;
        }
        bFirstPlayer = 0;
        pPlayer = (CSWSDialogPlayer*)m_lstPlayers.GetNext(pPosition);
    }
    return m_lstPlayers.GetHeadPos() != NULL;
}
// PC 0x005E74B0
CSWSObject* CSWSDialog::GetSpeaker(CSWSObject* pOwner, const CExoString& sSpeaker)
{
    unsigned long oidSpeaker;
    unsigned long nSpeaker;
    for (nSpeaker = 0; nSpeaker < m_nSpeakers; ++nSpeaker)
    {
        if (m_pSpeakers[nSpeaker].m_sSpeaker == sSpeaker)
        {
            oidSpeaker = m_pSpeakers[nSpeaker].m_oidSpeaker;
            break;
        }
    }
    if (nSpeaker == m_nSpeakers)
    {
        if (pOwner->m_sTag == sSpeaker)
        {
            oidSpeaker = pOwner->m_nId;
        }
        else
        {
            oidSpeaker = pOwner->GetNearestObjectByName(
                sSpeaker, CSWSArea::MAX_OBJECT_CONVERSATION_DISTANCE);
        }
        CSWSDialogSpeaker* pOldSpeakers = m_pSpeakers;
        m_pSpeakers = new CSWSDialogSpeaker[m_nSpeakers + 1];
        for (nSpeaker = 0; nSpeaker < m_nSpeakers; ++nSpeaker)
        {
            m_pSpeakers[nSpeaker].m_sSpeaker = pOldSpeakers[nSpeaker].m_sSpeaker;
            m_pSpeakers[nSpeaker].m_oidSpeaker = pOldSpeakers[nSpeaker].m_oidSpeaker;
        }
        m_pSpeakers[m_nSpeakers].m_sSpeaker = sSpeaker;
        m_pSpeakers[m_nSpeakers].m_oidSpeaker = oidSpeaker;
        ++m_nSpeakers;
        delete[] pOldSpeakers;
    }
    if (oidSpeaker == OBJECT_INVALID)
    {
        return NULL;
    }
    CGameObject* pObject;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    if (pServer->GetObjectArray()->GetGameObject(
            oidSpeaker, &pObject) != CGameObjectArray::SUCCESS)
    {
        return NULL;
    }
    CSWSObject* pSpeaker = (CSWSObject*)pObject;
    Vector vDistance = pSpeaker->m_vPosition - pOwner->m_vPosition;
    CSWSArea* pSpeakerArea = pSpeaker->GetArea();
    CSWSArea* pOwnerArea = pOwner->GetArea();
    if (pSpeakerArea != pOwnerArea ||
        (vDistance * vDistance > CSWSArea::MAX_OBJECT_CONVERSATION_DISTANCE *
                                 CSWSArea::MAX_OBJECT_CONVERSATION_DISTANCE &&
         pSpeaker->m_n260 == 0))
    {
        if (pSpeaker->m_oidDialogOwner == m_oidDialogOwner)
        {
            ClearDialogOwnerInObject(oidSpeaker);
        }
        return NULL;
    }
    if (pSpeaker->m_oidDialogOwner != OBJECT_INVALID &&
        pSpeaker->m_oidDialogOwner != m_oidDialogOwner)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        if (pServer->GetObjectArray()->GetGameObject(
                pSpeaker->m_oidDialogOwner, &pObject) == CGameObjectArray::SUCCESS &&
            ((CSWSObject*)pObject)->m_oidDialogOwner == pSpeaker->m_oidDialogOwner)
        {
            return NULL;
        }
        pSpeaker->SetDialogOwner(OBJECT_INVALID);
    }
    if (pSpeaker->m_oidDialogOwner == OBJECT_INVALID)
    {
        pSpeaker->SetDialogOwner(m_oidDialogOwner);
        if (pSpeaker->AsSWSCreature() != NULL)
        {
            pSpeaker->AsSWSCreature()->ActionManager(CSWSCreature::ACTION_MANAGER_DIALOG);
            pSpeaker->AsSWSCreature()->SetActivity(4, 1, 0);
        }
    }
    return pSpeaker;
}
// PC 0x005E77E0
float CSWSDialog::SetDialogDelay(CSWSObject* pObject, CExoLocString& lsText,
                                 unsigned long nDelay, int bSound,
                                 unsigned long nFlag)
{
    CExoString sText;
    float fDelay = 0.0f;

    if (m_lstPlayers.IsEmpty() && bSound != 0)
    {
        CSWSMessage::GetLocStringServerTime((unsigned long)-1, lsText, sText,
                                            fDelay, m_nCurrentPlayerGender);
        if (fDelay < 0.15f)
        {
            fDelay = sText.GetLength() * 0.15f * 0.75f;
        }
    }

    CGuiInGame* pGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
    if (pGui->m_nC0 == 0 && nFlag == 0 && fDelay < 0.01f)
    {
        fDelay = 0.01f;
    }

    if ((unsigned int)fDelay < nDelay)
    {
        fDelay = (float)nDelay;
    }
    pObject->SetDialogDelay(fDelay);
    return fDelay;
}
// PC 0x005E7930
int CSWSDialog::SendDialogModels(CSWSObject* pOwner, unsigned long oidPlayer)
{
    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
    CResRef* pModels = new CResRef[m_nStunts];
    unsigned long* pObjectIds = new unsigned long[m_nStunts];

    for (unsigned long i = 0; i < (unsigned long)m_nStunts; i++)
    {
        pObjectIds[i] = OBJECT_INVALID;
        if (m_pStunts[i].m_sParticipant == "PLAYER")
        {
            pObjectIds[i] = pClient->ServerToClientObjectId(
                m_lstPlayers.GetHeadPos() != NULL ? m_oidCurrentPlayer
                                                  : OBJECT_INVALID);
        }
        else if (m_pStunts[i].m_sParticipant == "OWNER" ||
                 m_pStunts[i].m_sParticipant == "")
        {
            pObjectIds[i] = pClient->ServerToClientObjectId(m_oidDialogOwner);
        }
        else
        {
            unsigned long oidStunt = pOwner->GetNearestObjectByName(
                m_pStunts[i].m_sParticipant, 0.0f);
            pObjectIds[i] = pClient->ServerToClientObjectId(oidStunt);
        }

        pModels[i] = m_pStunts[i].m_cStuntModel;
    }

    if (pClient->GetInGameGui()->HandleDialogModels(
            oidPlayer, m_nStunts, pObjectIds, pModels, m_cCameraModel,
            m_cAmbientTrack))
    {
        g_pAppManager->m_pClientExoApp->SetDialogModelsSent(1);
    }

    delete[] pObjectIds;
    delete[] pModels;
    return 1;
}
// PC 0x005E7AF0
unsigned long CSWSDialog::GetDialogModel(int nStunt, int& bNonDependant)
{
    bNonDependant = 0;
    if (nStunt >= 0 && nStunt < m_nStunts)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CGameObject* pGameObject = pServer->GetGameObject(m_oidDialogOwner);
        if (pGameObject != NULL && pGameObject->AsSWSObject() != NULL)
        {
            CSWSObject* pOwner = pGameObject->AsSWSObject();
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
            unsigned long oidModel;
            if (m_pStunts[nStunt].m_sParticipant == "PLAYER")
            {
                oidModel = pClient->ServerToClientObjectId(GetCurrentPlayerId());
            }
            else if (m_pStunts[nStunt].m_sParticipant == "OWNER" ||
                     m_pStunts[nStunt].m_sParticipant == "")
            {
                oidModel = pClient->ServerToClientObjectId(m_oidDialogOwner);
            }
            else
            {
                oidModel = pClient->ServerToClientObjectId(
                    pOwner->GetNearestObjectByName(
                        m_pStunts[nStunt].m_sParticipant, 0.0f));
                bNonDependant = pClient->GetInGameGui()->IsOnDialogNonDependantList(
                    m_pStunts[nStunt].m_sParticipant);
            }
            return oidModel;
        }
    }
    return OBJECT_INVALID;
}
// PC 0x005E7C10
inline CSWSDialogNode& CSWSDialogNode::operator=(const CSWSDialogNode& dialogNode)
{
    CSWSDialogBase::operator=(dialogNode);
    CSWSDialogCamera::operator=(dialogNode);
    return *this;
}
// PC 0x005E7C80
int CSWSDialog::HandleReply(unsigned long nPlayerId, CSWSObject* pOwner,
                            unsigned long nReply, int bEscape,
                            unsigned long nEntry, int bSkip)
{
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    CGameObject* pObject;
    if (!bEscape && (m_nSelectedReply != DIALOG_INDEX_INVALID || nEntry != m_nCurrentEntry))
    {
        return 1;
    }

    CExoLinkedListNode* pPosition = m_lstPlayers.GetHeadPos();
    CSWSDialogPlayer* pPlayer = m_lstPlayers.GetHead();
    while (pPlayer != NULL)
    {
        if (pPlayer->m_nPlayerId == nPlayerId)
        {
            if (bEscape)
            {
                if (g_pAppManager->GetServerExoApp()->GetObjectArray()
                        ->GetGameObject(pPlayer->m_oidPlayer, &pObject) ==
                    CGameObjectArray::SUCCESS)
                {
                    CSWSObject* pPlayerObject = (CSWSObject*)pObject;
                    if (pPlayerObject->AsSWSCreature() != NULL)
                    {
                        pPlayerObject->AsSWSCreature()->SetActivity(
                            CSWSCreature::ACTIVITY_DIALOG, 0, 0);
                    }
                    else
                    {
                        pPlayerObject->StopDialog();
                    }
                }
                return 1;
            }
            m_oidCurrentPlayer = pPlayer->m_oidPlayer;
            m_nCurrentPlayerGender = pPlayer->m_nGender;
            break;
        }
        pPlayer = m_lstPlayers.GetNext(pPosition);
    }
    if (m_lstPlayers.GetHeadPos() != NULL && pPlayer == NULL)
    {
        return 1;
    }
    if (nReply >= (unsigned long)m_pEntries[m_nCurrentEntry].m_nReplies)
    {
        return 0;
    }
    if (g_pAppManager->GetServerExoApp()->GetObjectArray()
            ->GetGameObject(m_oidCurrentPlayer, &pObject) !=
        CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    m_nSelectedReply = nReply;
    unsigned long nReplyIndex = m_pEntries[m_nCurrentEntry].m_pReplies[m_nSelectedReply].m_nIndex;
    if (m_lstPlayers.GetHeadPos() != NULL)
    {
        pPosition = m_lstPlayers.GetHeadPos();
        pPlayer = m_lstPlayers.GetHead();
        while (pPlayer != NULL)
        {
            if (pPlayer->m_nPlayerId != nPlayerId)
            {
                CExoString sListener =
                    m_pReplies[m_pEntries[m_nCurrentEntry].m_pReplies[m_nSelectedReply].m_nIndex].m_sListener;
                if (sListener != "" && sListener.CompareNoCase("OWNER") == 0)
                {
                    if (sListener.CompareNoCase("PLAYER") != 0)
                    {
                        g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                    }
                    else
                    {
                        GetSpeaker(pOwner, sListener);
                    }
                }
                pClient->GetInGameGui()->HandleDialogReplyChosen(
                    pPlayer->m_nPlayerId, nReply, nEntry,
                    GetExoStringFromLocString(m_pReplies[nReplyIndex].m_lsText, m_nCurrentPlayerGender),
                    pClient->ServerToClientObjectId(m_oidCurrentPlayer), m_nCurrentPlayerGender, m_bEndOfDialog);
            }
            AddJournalEntry(m_pReplies[nReplyIndex].m_sQuest,
                            m_pReplies[nReplyIndex].m_nQuestEntry, pPlayer->m_nPlayerId);
            GiveDialogPlotXP(m_pReplies[nReplyIndex].m_nPlotIndex,
                             m_pReplies[nReplyIndex].m_fPlotXPPercentage);
            pPlayer = m_lstPlayers.GetNext(pPosition);
        }
    }
    if (m_lstPlayers.GetHeadPos() != NULL)
    {
        m_n98 = m_oidCurrentPlayer;
    }
    if (!bSkip)
    {
        if (!(m_pReplies[nReplyIndex].m_nWaitFlags & DIALOG_WAIT_FLAG_SOUND))
        {
            if (m_pReplies[nReplyIndex].m_nSoundDelay == CSWSDialogBase::DELAY_DEFAULT)
            {
                if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nC0 == 0)
                {
                    SetDialogDelay(pOwner, m_pReplies[nReplyIndex].m_lsText,
                        (unsigned long)m_pReplies[nReplyIndex].m_lsText.GetSpokenTimeEstimate(
                            g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                            m_nCurrentPlayerGender), m_pReplies[nReplyIndex].m_nEntries != 0, 1);
                }
                else
                {
                    pOwner->SetDialogDelay(0.0f);
                }
            }
            else
            {
                SetDialogDelay(pOwner, m_pReplies[nReplyIndex].m_lsText, 0,
                               m_pReplies[nReplyIndex].m_nEntries != 0, 1);
            }
        }
        else
        {
            SetDialogDelay(pOwner, m_pReplies[nReplyIndex].m_lsText,
                           m_pReplies[nReplyIndex].m_nDelay,
                           m_pReplies[nReplyIndex].m_nEntries != 0, 1);
        }
        if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nC0 == 0)
        {
            pOwner->m_nWaitFlags = m_pReplies[nReplyIndex].m_nWaitFlags;
        }
        else
        {
            pOwner->m_nWaitFlags = 0;
        }
    }
    g_pVirtualMachine->SetScriptParams(
        m_pReplies[nReplyIndex].m_nActionParam1,
        m_pReplies[nReplyIndex].m_nActionParam2,
        m_pReplies[nReplyIndex].m_nActionParam3,
        m_pReplies[nReplyIndex].m_nActionParam4,
        m_pReplies[nReplyIndex].m_nActionParam5,
        m_pReplies[nReplyIndex].m_sActionParamStrA);
    RunScript(pOwner, m_pReplies[nReplyIndex].m_cScript);
    g_pVirtualMachine->SetScriptParams(
        m_pReplies[nReplyIndex].m_nActionParam1b,
        m_pReplies[nReplyIndex].m_nActionParam2b,
        m_pReplies[nReplyIndex].m_nActionParam3b,
        m_pReplies[nReplyIndex].m_nActionParam4b,
        m_pReplies[nReplyIndex].m_nActionParam5b,
        m_pReplies[nReplyIndex].m_sActionParamStrB);
    RunScript(pOwner, m_pReplies[nReplyIndex].m_cScript2);
    unsigned long nNextEntry;
    for (nNextEntry = 0;
         nNextEntry < (unsigned long)m_pReplies[nReplyIndex].m_nEntries;
         ++nNextEntry)
    {
        if (m_pEntries[m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nIndex].m_sSpeaker != "" &&
            pOwner->m_sTag.CompareNoCase(m_pEntries[m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nIndex].m_sSpeaker) == 0 &&
            !GetSpeaker(pOwner, m_pEntries[m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nIndex].m_sSpeaker))
        {
            continue;
        }

        int bActive1;
        int bHasActive1;
        if (m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_cActive == "")
        {
            bActive1 = 1;
            bHasActive1 = 0;
        }
        else
        {
            bHasActive1 = 1;
            g_pVirtualMachine->SetScriptParams(
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nParam1,
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nParam2,
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nParam3,
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nParam4,
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nParam5,
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_sParamStrA);
            bActive1 = CheckScript(pOwner, m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_cActive);
            if (m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_bNot)
            {
                bActive1 = !bActive1;
            }
        }

        int bActive2;
        int bHasActive2;
        if (m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_cActive2 == "")
        {
            bActive2 = 1;
            bHasActive2 = 0;
        }
        else
        {
            bHasActive2 = 1;
            g_pVirtualMachine->SetScriptParams(
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nParam1b,
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nParam2b,
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nParam3b,
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nParam4b,
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nParam5b,
                m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_sParamStrB);
            bActive2 = CheckScript(pOwner, m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_cActive2);
            if (m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_bNot2)
            {
                bActive2 = !bActive2;
            }
        }

        int bActive;
        if (!bHasActive1)
        {
            if (bHasActive2)
            {
                bActive = bActive2;
            }
            else
            {
                bActive = 1;
            }
        }
        else if (!bHasActive2)
        {
            bActive = bActive1;
        }
        else if (m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nLogic == CSWSDialogActiveEntry::LOGIC_AND)
        {
            bActive = bActive1 && bActive2;
        }
        else
        {
            bActive = bActive1 || bActive2;
        }
        if (!bActive)
        {
            continue;
        }
        return pOwner->SendDialogEntry(m_pReplies[nReplyIndex].m_pEntries[nNextEntry].m_nIndex);
    }
    m_bEndOfDialog = 1;
    m_n60 = 1;
    return 0;
}
// PC 0x005E83F0
void CSWSDialog::ClearAllPlayerHostileActions()
{
    unsigned long oidPlayer =
        g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
    unsigned long oidPC = g_pAppManager->GetServerExoApp()
        ->GetClientObjectByObjectId(oidPlayer)->m_oidPCObject;
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    CExoArrayList<unsigned long>* pObjects =
        &g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->m_lstObjectIds;

    for (int nObject = 0; nObject < pObjects->num; nObject++)
    {
        CGameObject* pObject;

        unsigned long oidObject = (*pObjects)[nObject];

        if (pObjectArray->GetGameObject(oidObject, &pObject) ==
                CGameObjectArray::SUCCESS &&
            pObject != NULL && pObject->AsSWSCreature() != NULL &&
            pObject->AsSWSCreature()->GetInParty() == 0)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();

            if (pCreature->GetReputation(oidPC, 0) <=
                CSWSObject::MAX_HOSTILE_REPUTATION)
            {
                pCreature->ClearAllActions(1);
                if (pCreature->GetClientCreature() != NULL)
                {
                    pCreature->GetClientCreature()->ClearAllActions();
                }
            }
        }
    }
}
// PC 0x005E8930
CSWSDialog::CSWSDialog()
{
    m_oidDialogOwner = OBJECT_INVALID;
    m_oidCurrentPlayer = OBJECT_INVALID;
    m_pEntries = NULL;
    m_nEntries = 0;
    m_pReplies = NULL;
    m_nReplies = 0;
    m_pStartingEntries = NULL;
    m_nStartingEntries = 0;
    m_bEndOfDialog = 0;
    m_nCurrentEntry = 0;
    m_bRepliesSent = 0;
    m_nSelectedReply = DIALOG_INDEX_INVALID;
    m_nCurrentPlayerGender = CTlkTable::GENDER_MALE;
    m_oidLastEntrySpeaker = OBJECT_INVALID;
    m_n98 = OBJECT_INVALID;
    m_pSpeakers = NULL;
    m_nSpeakers = 0;
    m_bMultiPCDialogViewOnly = 0;
    m_n90 = 0;
    m_n60 = 0;
    m_n40 = 0;
    m_n44 = 0;
    m_n3C = 0;
    m_n64 = 0;
    m_nPseudoRandomCameraAngle = 0;
    m_nPseudoRandomCameraStep = 1;
    m_bPseudoRandomCameraDirection = 0;
    m_nPseudoRandomCameraAngles[0] = CAMERA_ANGLE_FIRST;
    m_nPseudoRandomCameraAngles[1] = CAMERA_ANGLE_THIRD;
    m_nPseudoRandomCameraAngles[2] = CAMERA_ANGLE_FIRST;
    m_nPseudoRandomCameraAngles[3] = CAMERA_ANGLE_THIRD;
    m_nPseudoRandomCameraAngles[4] = CAMERA_ANGLE_SECOND;
    m_nPseudoRandomCameraAngles[5] = CAMERA_ANGLE_SECOND;
    m_nPseudoRandomCameraAngles[6] = CAMERA_ANGLE_FIRST;
    m_nPseudoRandomCameraAngles[7] = CAMERA_ANGLE_THIRD;
    m_nPseudoRandomCameraAngles[8] = CAMERA_ANGLE_FIRST;
    m_nPseudoRandomCameraAngles[9] = CAMERA_ANGLE_SECOND;
    m_nPseudoRandomCameraAngles[10] = CAMERA_ANGLE_FIRST;
    m_nPseudoRandomCameraAngles[11] = CAMERA_ANGLE_SECOND;
    m_nPseudoRandomCameraAngles[12] = CAMERA_ANGLE_FIRST;
    m_nPseudoRandomCameraAngles[13] = CAMERA_ANGLE_THIRD;
    m_nPseudoRandomCameraAngles[14] = CAMERA_ANGLE_SECOND;
    m_nPseudoRandomCameraAngles[15] = CAMERA_ANGLE_THIRD;
    m_nPseudoRandomCameraAngles[16] = CAMERA_ANGLE_FIRST;
    m_nPseudoRandomCameraAngles[17] = CAMERA_ANGLE_FIRST;
    m_nPseudoRandomCameraAngles[18] = CAMERA_ANGLE_FIRST;
    m_nPseudoRandomAlienVO = 0;
    m_nPseudoRandomAlienVOStep = 1;
    m_bPseudoRandomAlienVODirection = 0;
    m_nPseudoRandomAlienVOVariations[0] = 1;
    m_nPseudoRandomAlienVOVariations[1] = 1;
    m_nPseudoRandomAlienVOVariations[2] = 2;
    m_nPseudoRandomAlienVOVariations[3] = 3;
    m_nPseudoRandomAlienVOVariations[4] = 1;
    m_nPseudoRandomAlienVOVariations[5] = 2;
    m_nPseudoRandomAlienVOVariations[6] = 1;
    m_nPseudoRandomAlienVOVariations[7] = 2;
    m_nPseudoRandomAlienVOVariations[8] = 2;
    m_nPseudoRandomAlienVOVariations[9] = 3;
    m_nPseudoRandomAlienVOVariations[10] = 3;
    m_nPseudoRandomAlienVOVariations[11] = 3;
    m_nPseudoRandomAlienVOVariations[12] = 1;
    m_nPseudoRandomAlienVOVariations[13] = 3;
    m_nPseudoRandomAlienVOVariations[14] = 2;
    m_nPseudoRandomAlienVOVariations[15] = 1;
    m_nPseudoRandomAlienVOVariations[16] = 3;
    m_nPseudoRandomAlienVOVariations[17] = 2;
    m_nPseudoRandomAlienVOVariations[18] = 2;
    m_nF4 = 0;
    m_nF8 = 0;
    m_cAmbientTrack = "";
    m_n10C = 0;
    m_n110 = 0;
    m_n170 = 0;
    m_nAlienRaceOwner = 0;
    m_n178 = 0;
    m_nAnimationRetryDelay = 0;
    m_nLastEntryDay = 0;
    m_nLastEntryTime = 0;
    m_nOwnerAILevel = 2;
}
// PC 0x005EA850
int CSWSDialog::LoadDialog(CResGFF* pGFF, int bLoadReplies)
{
    int bSuccess = 1;
    int bFieldSuccess = 1;
    CResStruct resTop;
    InitializePsuedoRandomCameraAngles(pGFF);
    m_nF4 = 1;
    m_nF8 = 1;
    InitializePseudoRandomAlienVOVariation(pGFF);
    Cleanup();
    pGFF->GetTopLevelStruct(&resTop);

    m_cCameraModel = pGFF->ReadFieldCResRef(&resTop, "CameraModel", bFieldSuccess, "");
    unsigned long nDelayEntry = pGFF->ReadFieldDWORD(&resTop, "DelayEntry", bSuccess, 0);
    unsigned long nDelayReply = pGFF->ReadFieldDWORD(&resTop, "DelayReply", bSuccess, 0);
    m_cEndConversationScript = pGFF->ReadFieldCResRef(&resTop, "EndConversation", bFieldSuccess, "");
    m_cEndConversationAbortedScript = pGFF->ReadFieldCResRef(&resTop, "EndConverAbort", bFieldSuccess, "");
    m_n3C = pGFF->ReadFieldBYTE(&resTop, "Skippable", bFieldSuccess, 1);
    int nConversationType = pGFF->ReadFieldINT(&resTop, "ConversationType", bFieldSuccess, 0);
    unsigned char nComputerType = pGFF->ReadFieldBYTE(&resTop, "ComputerType", bFieldSuccess, 0);
    m_cAmbientTrack = pGFF->ReadFieldCResRef(&resTop, "AmbientTrack", bFieldSuccess, "");
    m_n40 = pGFF->ReadFieldBYTE(&resTop, "UnequipItems", bFieldSuccess, 0);
    m_n44 = pGFF->ReadFieldBYTE(&resTop, "UnequipHItem", bFieldSuccess, 0);
    int bAnimatedCut = pGFF->ReadFieldBYTE(&resTop, "AnimatedCut", bFieldSuccess, 0);
    int bOldHitCheck = pGFF->ReadFieldBYTE(&resTop, "OldHitCheck", bFieldSuccess, 0);
    m_nAlienRaceOwner = pGFF->ReadFieldINT(&resTop, "AlienRaceOwner", bFieldSuccess, 0);
    g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nC0 = bAnimatedCut;
    g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nDialogCameraType = bOldHitCheck;
    if (nConversationType == 2)
    {
        m_n110 = 1;
    }
    if (nConversationType == CONVERSATION_TYPE_COMPUTER)
    {
        m_n170 = nConversationType;
        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetConversationType(CONVERSATION_TYPE_COMPUTER, nComputerType);
    }
    else
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetConversationType(CONVERSATION_TYPE_CINEMATIC, 0);
        if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_n102C == 0)
        {
            g_pAppManager->GetServerExoApp()->GetPartyTable()->UnstealthParty(1);
        }
    }
    if (bAnimatedCut)
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalUnpauseableState(1);
    }

    CResList resList;
    CResList resLinks;
    CResStruct resNode;
    CResStruct resLink;
    pGFF->GetList(&resList, &resTop, "ReplyList");
    m_nReplies = pGFF->GetListCount(&resList);
    pGFF->GetList(&resList, &resTop, "EntryList");
    m_nEntries = pGFF->GetListCount(&resList);
    m_pEntries = new CSWSDialogEntry[m_nEntries];
    unsigned long nNode;
    unsigned long nLink;
    for (nNode = 0; bSuccess && nNode < (unsigned long)m_nEntries; ++nNode)
    {
        pGFF->GetListElement(&resNode, &resList, nNode);
        LoadDialogBase(m_pEntries[nNode], pGFF, resNode, nDelayEntry, bSuccess);
        LoadDialogCamera(m_pEntries[nNode], pGFF, resNode, bSuccess);
        pGFF->GetList(&resLinks, &resNode, "RepliesList");
        m_pEntries[nNode].m_nReplies = pGFF->GetListCount(&resLinks);
        if (bLoadReplies)
        {
            if (m_pEntries[nNode].m_nReplies)
            {
                m_pEntries[nNode].m_pReplies = new CSWSDialogLinkEntry[m_pEntries[nNode].m_nReplies];
            }
            for (nLink = 0; bSuccess && nLink < (unsigned long)m_pEntries[nNode].m_nReplies; ++nLink)
            {
                pGFF->GetListElement(&resLink, &resLinks, nLink);
                LoadDialogLinkedNode(m_pEntries[nNode].m_pReplies[nLink], pGFF, resLink, bSuccess, m_nReplies);
                m_pEntries[nNode].m_pReplies[nLink].m_bDisplayInactive = pGFF->ReadFieldBYTE(&resLink, "DisplayInactive", bFieldSuccess, 0) != 0;
            }
        }
    }
    if (!bSuccess)
    {
        Cleanup();
        return bSuccess;
    }
    if (bLoadReplies)
    {
        pGFF->GetList(&resList, &resTop, "ReplyList");
        m_pReplies = new CSWSDialogReply[m_nReplies];
        for (nNode = 0; bSuccess && nNode < (unsigned long)m_nReplies; ++nNode)
        {
            pGFF->GetListElement(&resNode, &resList, nNode);
            LoadDialogBase(m_pReplies[nNode], pGFF, resNode, nDelayEntry, bSuccess);
            LoadDialogCamera(m_pReplies[nNode], pGFF, resNode, bSuccess);
            pGFF->GetList(&resLinks, &resNode, "EntriesList");
            m_pReplies[nNode].m_nEntries = pGFF->GetListCount(&resLinks);
            if (m_pReplies[nNode].m_nEntries)
            {
                m_pReplies[nNode].m_pEntries = new CSWSDialogActiveEntry[m_pReplies[nNode].m_nEntries];
            }
            for (nLink = 0; bSuccess && nLink < (unsigned long)m_pReplies[nNode].m_nEntries; ++nLink)
            {
                pGFF->GetListElement(&resLink, &resLinks, nLink);
                LoadDialogLinkedNode(m_pReplies[nNode].m_pEntries[nLink], pGFF, resLink, bSuccess, m_nEntries);
            }
        }
        if (!bSuccess)
        {
            Cleanup();
            return bSuccess;
        }
    }
    else
    {
        m_nReplies = 0;
    }
    pGFF->GetList(&resLinks, &resTop, "StartingList");
    m_nStartingEntries = pGFF->GetListCount(&resLinks);
    if (!m_nStartingEntries)
    {
        m_pStartingEntries = NULL;
    }
    else
    {
        m_pStartingEntries = new CSWSDialogActiveEntry[m_nStartingEntries];
    }
    for (nNode = 0; bSuccess && nNode < (unsigned long)m_nStartingEntries; ++nNode)
    {
        pGFF->GetListElement(&resLink, &resLinks, nNode);
        LoadDialogLinkedNode(m_pStartingEntries[nNode], pGFF, resLink, bSuccess, m_nEntries);
    }
    if (!bSuccess)
    {
        Cleanup();
    }
    CResList resStunts;
    CResStruct resStunt;
    pGFF->GetList(&resStunts, &resTop, "StuntList");
    m_nStunts = pGFF->GetListCount(&resStunts);
    if (!m_nStunts)
    {
        m_pStunts = NULL;
    }
    else
    {
        m_pStunts = new CSWSDialogStuntModel[m_nStunts];
    }
    for (unsigned long nStunt = 0; nStunt < (unsigned long)m_nStunts && bSuccess; ++nStunt)
    {
        pGFF->GetListElement(&resStunt, &resStunts, nStunt);
        m_pStunts[nStunt].m_sParticipant = pGFF->ReadFieldCExoString(&resStunt, "Participant", bSuccess, "");
        CResRef cEmpty = "";
        m_pStunts[nStunt].m_cStuntModel = pGFF->ReadFieldCResRef(&resStunt, "StuntModel", bSuccess, cEmpty);
    }
    return bSuccess;
}
// PC 0x005EB2C0
int CSWSDialog::GetStartEntry(CSWSObject* pOwner)
{
    unsigned long nStart;
    for (nStart = 0; nStart < (unsigned long)m_nStartingEntries; ++nStart)
    {
        if (m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_sSpeaker != "" &&
            pOwner->m_sTag.CompareNoCase(m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_sSpeaker) == 0 &&
            !GetSpeaker(pOwner, m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_sSpeaker))
        {
            continue;
        }

        int bActive1;
        int bHasActive1;
        if (m_pStartingEntries[nStart].m_cActive == "")
        {
            bActive1 = 1;
            bHasActive1 = 0;
        }
        else
        {
            bHasActive1 = 1;
            g_pVirtualMachine->SetScriptParams(
                m_pStartingEntries[nStart].m_nParam1,
                m_pStartingEntries[nStart].m_nParam2,
                m_pStartingEntries[nStart].m_nParam3,
                m_pStartingEntries[nStart].m_nParam4,
                m_pStartingEntries[nStart].m_nParam5,
                m_pStartingEntries[nStart].m_sParamStrA);
            bActive1 = CheckScript(pOwner, m_pStartingEntries[nStart].m_cActive);
            if (m_pStartingEntries[nStart].m_bNot)
            {
                bActive1 = !bActive1;
            }
        }

        int bActive2;
        int bHasActive2;
        if (m_pStartingEntries[nStart].m_cActive2 == "")
        {
            bActive2 = 1;
            bHasActive2 = 0;
        }
        else
        {
            bHasActive2 = 1;
            g_pVirtualMachine->SetScriptParams(
                m_pStartingEntries[nStart].m_nParam1b,
                m_pStartingEntries[nStart].m_nParam2b,
                m_pStartingEntries[nStart].m_nParam3b,
                m_pStartingEntries[nStart].m_nParam4b,
                m_pStartingEntries[nStart].m_nParam5b,
                m_pStartingEntries[nStart].m_sParamStrB);
            bActive2 = CheckScript(pOwner, m_pStartingEntries[nStart].m_cActive2);
            if (m_pStartingEntries[nStart].m_bNot2)
            {
                bActive2 = !bActive2;
            }
        }

        int bActive;
        if (!bHasActive1)
        {
            if (bHasActive2)
            {
                bActive = bActive2;
            }
            else
            {
                bActive = 1;
            }
        }
        else if (!bHasActive2)
        {
            bActive = bActive1;
        }
        else if (m_pStartingEntries[nStart].m_nLogic == CSWSDialogActiveEntry::LOGIC_AND)
        {
            bActive = bActive1 && bActive2;
        }
        else
        {
            bActive = bActive1 || bActive2;
        }

        if (!bActive)
        {
            continue;
        }
        if (m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nReplies == 0 && m_n110 == 0 && m_n170 == 0)
        {
            BarkOneLine(m_pStartingEntries[nStart].m_nIndex);
            g_pVirtualMachine->SetScriptParams(
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam1,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam2,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam3,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam4,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam5,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_sActionParamStrA);
            RunScript(pOwner, m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_cScript);
            g_pVirtualMachine->SetScriptParams(
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam1b,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam2b,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam3b,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam4b,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam5b,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_sActionParamStrB);
            RunScript(pOwner, m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_cScript2);
            while (!m_lstPlayers.IsEmpty())
            {
                delete m_lstPlayers.RemoveHead();
            }
            return DIALOG_INDEX_INVALID;
        }
        else if (m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nReplies == 1 &&
                 m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nEntries == 0 &&
                 m_n110 == 0 && m_n170 == 0)
        {
            BarkOneLine(m_pStartingEntries[nStart].m_nIndex);
            g_pVirtualMachine->SetScriptParams(
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam1,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam2,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam3,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam4,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam5,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_sActionParamStrA);
            RunScript(pOwner, m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_cScript);
            g_pVirtualMachine->SetScriptParams(
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam1b,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam2b,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam3b,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam4b,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_nActionParam5b,
                m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_sActionParamStrB);
            RunScript(pOwner, m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_cScript2);
            g_pVirtualMachine->SetScriptParams(
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nActionParam1,
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nActionParam2,
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nActionParam3,
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nActionParam4,
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nActionParam5,
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_sActionParamStrA);
            RunScript(pOwner, m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_cScript);
            g_pVirtualMachine->SetScriptParams(
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nActionParam1b,
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nActionParam2b,
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nActionParam3b,
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nActionParam4b,
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_nActionParam5b,
                m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_sActionParamStrB);
            RunScript(pOwner, m_pReplies[m_pEntries[m_pStartingEntries[nStart].m_nIndex].m_pReplies[0].m_nIndex].m_cScript2);
            while (!m_lstPlayers.IsEmpty())
            {
                delete m_lstPlayers.RemoveHead();
            }
            return DIALOG_INDEX_INVALID;
        }
        return m_pStartingEntries[nStart].m_nIndex;
    }
    return DIALOG_INDEX_INVALID;
}
// PC 0x005E8BA0
void CSWSDialog::BarkOneLine(int nEntry)
{
    CExoString sText;
    CResRef cSound;
    unsigned long oidPlayer =
        g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();

    m_pEntries[nEntry].m_lsText.GetString(
        g_pAppManager->GetClientExoApp()->GetClientLanguage(),
        &sText, m_nCurrentPlayerGender);
    cSound = m_pEntries[nEntry].m_cSoundResRef;

    int bHoldFade = g_pAppManager->GetClientExoApp()->GetInGameGui()
                        ->m_bHoldWorldFadeInForDialog;
    if (bHoldFade)
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()
            ->m_bHoldWorldFadeInForDialog = 0;
        g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(
            1, 0.0f, 1.0f, Vector());
    }

    if (!cSound.IsValid())
    {
        STR_RES strRes;
        unsigned long nStrRef = m_pEntries[nEntry].m_lsText.m_dwStringRef;
        g_pTlkTable->Fetch(nStrRef, strRes, CTlkTable::GENDER_MALE);
        cSound = strRes.m_resSound;
    }
    else if (m_pEntries[nEntry].m_bSoundExists & CSWSDialogBase::SOUND_EXISTS_SKIP)
    {
        CExoString sVariation;
        sVariation.Format("%d", rand() % 3 + 1);
        cSound += sVariation;
    }

    if (sText.CompareNoCase("") == 0)
    {
        CGameObject* pPlayer =
            g_pAppManager->GetServerExoApp()->GetGameObject(oidPlayer);
        if (pPlayer != NULL)
        {
            unsigned long oidSpeaker;
            if (m_pEntries->m_sSpeaker == "")
            {
                oidSpeaker = m_oidDialogOwner;
            }
            else
            {
                oidSpeaker = pPlayer->AsSWSObject()->GetNearestObjectByName(
                    m_pEntries->m_sSpeaker, 0.0f);
            }
            unsigned long oidClient =
                g_pAppManager->GetClientExoApp()->ServerToClientObjectId(
                    oidSpeaker);
            g_pAppManager->GetClientExoApp()->BarkString(oidClient, sText,
                                                         cSound, 0);

            if (g_bLogConversations && oidPlayer != OBJECT_INVALID)
            {
                CSWCObject* pObject =
                    g_pAppManager->GetClientExoApp()->GetGameObject(oidClient);
                CExoString sName;
                if (pObject != NULL && pObject->AsSWCCreature() != NULL)
                {
                    CSWCCreature* pCreature = pObject->AsSWCCreature();
                    if (pCreature != NULL)
                    {
                        sName = pCreature->GetStats()->m_sFirstName;
                        g_pAppManager->GetClientExoApp()->GetInGameGui()
                            ->AppendToMsgBuffer(sName + ": " + sText, 1, 0);
                    }
                }
            }

            CGameObject* pGameObject =
                g_pAppManager->GetServerExoApp()->GetGameObject(
                    m_oidDialogOwner);
            if (pGameObject != NULL)
            {
                CSWSObject* pOwner = pGameObject->AsSWSObject();
                if (pOwner != NULL)
                {
                    m_bEndOfDialog = 1;
                    RunEndConversationScript(pOwner, 1);
                }
            }
        }
    }
}
// PC 0x005E84F0
void CSWSDialog::RunEndConversationScript(CSWSObject* pObject, int bAborted)
{
    CGameObjectArray* pObjectArray;
    CSWSObject* pDialogObject;
    CSWSArea* pArea;
    CGameObject* pGameObject;
    int i;

    if (m_bEndOfDialog != 0 && m_n10C == 0)
    {
        m_n10C = 1;
        RunScript(pObject, m_cEndConversationScript);
        pObjectArray = g_pAppManager->m_pServerExoApp->GetObjectArray();
        pObjectArray->GetGameObject(m_oidDialogOwner, &pGameObject);

        if (pGameObject != NULL)
        {
            pDialogObject = pGameObject->AsSWSObject();
            if (pDialogObject != NULL)
            {
                pArea = pDialogObject->GetArea();
                if (bAborted == 0)
                {
                    i = 0;
                    if (pArea->m_lstObjectIds.num > 0)
                    {
                        do
                        {
                            unsigned long oidObject =
                                pArea->m_lstObjectIds[i];
                            i++;
                            pObjectArray->GetGameObject(oidObject,
                                                        &pGameObject);
                            if (pGameObject != NULL)
                            {
                                if (pGameObject->AsSWSCreature() != NULL)
                                {
                                    pGameObject->AsSWSCreature()
                                        ->OnEndDialog(pDialogObject);
                                }
                                else if (pGameObject->AsSWSPlaceable() != NULL)
                                {
                                    pGameObject->AsSWSPlaceable()->OnEndDialog(
                                        pDialogObject);
                                }
                            }
                        } while (i < pArea->m_lstObjectIds.num);
                    }
                }
                else
                {
                    if (pDialogObject->AsSWSCreature() != NULL)
                    {
                        pDialogObject->AsSWSCreature()->OnEndDialog(
                            pDialogObject);
                        return;
                    }
                    else if (pDialogObject->AsSWSPlaceable() != NULL)
                    {
                        pDialogObject->AsSWSPlaceable()->OnEndDialog(
                            pDialogObject);
                        return;
                    }
                }
            }
        }
        return;
    }

    RunScript(pObject, m_cEndConversationAbortedScript);
}
// PC 0x005E8650
void CSWSDialog::UnequipItemsForDialog(unsigned long oidObject)
{
    if (oidObject == OBJECT_INVALID)
    {
        return;
    }

    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
    unsigned long oidClientObject = pClient->ServerToClientObjectId(oidObject);
    CSWCObject* pObject =
        g_pAppManager->m_pClientExoApp->GetGameObject(oidClientObject);
    if (pObject == NULL || pObject->AsSWCCreature() == NULL)
    {
        return;
    }

    CSWCCreature* pCreature = pObject->AsSWCCreature();
    CExoLinkedListNode* pPosition = m_lstUnequippedItems.GetHeadPos();
    while (pPosition != NULL)
    {
        unsigned long* pId =
            (unsigned long*)m_lstUnequippedItems.GetAtPos(pPosition);
        if (*pId == pCreature->m_nId)
        {
            return;
        }
        m_lstUnequippedItems.GetNext(pPosition);
    }

    unsigned long* pId = new unsigned long;
    if (pId != NULL)
    {
        *pId = pCreature->m_nId;
        m_lstUnequippedItems.AddHead(pId);
        pCreature->HideEquippedItems(CResRef(), CResRef());
    }
}
// PC 0x005E8730
void CSWSDialog::ReEquipItemsForDialog()
{
    unsigned long* pObjectId;
    CSWCObject* pGameObject;
    CSWCObject* pClientObject;
    CSWCCreature* pCreature;
    CExoLinkedListNode* pPosition = m_lstUnequippedItems.GetHeadPos();

    while (pPosition != NULL)
    {
        pObjectId = (unsigned long*)m_lstUnequippedItems.GetAtPos(pPosition);

        if (pObjectId != NULL)
        {
            unsigned long oidObject = *pObjectId;

            pGameObject =
                g_pAppManager->m_pClientExoApp->GetGameObject(oidObject);
            pClientObject = pGameObject;

            if (pGameObject != NULL && pGameObject->AsSWCCreature() != NULL)
            {
                pCreature = pClientObject->AsSWCCreature();
                pCreature->ShowEquippedItems();
            }

            delete pObjectId;
        }

        m_lstUnequippedItems.GetNext(pPosition);
    }
}
// PC 0x005E8F50
int CSWSDialog::SendDialogEntryNode(
    unsigned long nPlayerId, unsigned long oidTarget, unsigned long oidSpeaker,
    unsigned long oidListener, unsigned long oidCamera, unsigned char nGender,
    CSWSObject* pOwner, CSWSDialogEntry& entry, unsigned long& nResult)
{
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    int nAnimations = entry.m_nAnimationCount;
    unsigned long* pAnimationObjects = NULL;
    unsigned short* pAnimations = NULL;
    int* pDependant = NULL;
    int i;
    if (nAnimations != 0)
    {
        pAnimationObjects = new unsigned long[nAnimations];
        pAnimations = new unsigned short[nAnimations];
        pDependant = new int[nAnimations];
    }

    for (i = 0; i < nAnimations; ++i)
    {
        unsigned long oidAnimationObject;
        if (entry.m_pAnimations[i].m_sParticipant == "PLAYER")
        {
            oidAnimationObject = GetCurrentPlayerId();
        }
        else if (entry.m_pAnimations[i].m_sParticipant == "OWNER")
        {
            oidAnimationObject = m_oidDialogOwner;
        }
        else if (entry.m_pAnimations[i].m_sParticipant == "NPC1")
        {
            CSWCCreature* pCharacter = g_pAppManager->GetClientExoApp()
                ->GetSWParty()->GetCharacter(1);
            oidAnimationObject = pCharacter != NULL
                ? g_pAppManager->GetServerExoApp()->ClientToServerObjectId(
                    pCharacter->GetId()) : OBJECT_INVALID;
        }
        else if (entry.m_pAnimations[i].m_sParticipant == "NPC2")
        {
            CSWCCreature* pCharacter = g_pAppManager->GetClientExoApp()
                ->GetSWParty()->GetCharacter(2);
            oidAnimationObject = pCharacter != NULL
                ? g_pAppManager->GetServerExoApp()->ClientToServerObjectId(
                    pCharacter->GetId()) : OBJECT_INVALID;
        }
        else
        {
            oidAnimationObject = pOwner->GetNearestObjectByName(
                entry.m_pAnimations[i].m_sParticipant, 0.0f);
            pDependant[i] = !g_pAppManager->GetClientExoApp()->GetInGameGui()
                ->IsOnDialogNonDependantList(entry.m_pAnimations[i].m_sParticipant);
        }

        if (g_pAppManager->GetServerExoApp()->GetGameObject(oidAnimationObject) != NULL)
        {
            pAnimationObjects[i] = pClient->ServerToClientObjectId(oidAnimationObject);
            pAnimations[i] = entry.m_pAnimations[i].m_nAnimation;
        }
        else
        {
            CExoString sError;
            sError.Format("Error: dialogue can't find object '%s'!",
                          entry.m_pAnimations[i].m_sParticipant);
            pAnimationObjects[i] = OBJECT_INVALID;
            pAnimations[i] = 0;
        }
    }

    if (oidListener != OBJECT_INVALID)
    {
        CGameObject* pServerObject = g_pAppManager->GetServerExoApp()
            ->GetGameObject(oidListener);
        if (pServerObject == NULL || pServerObject->AsSWSObject() == NULL)
        {
            nResult = DIALOG_RESULT_WAIT;
            return 1;
        }
        nResult = DIALOG_RESULT_READY;
        unsigned long oidClient = g_pAppManager->GetClientExoApp()
            ->ServerToClientObjectId(oidListener);
        CSWCObject* pClientObject = g_pAppManager->GetClientExoApp()
            ->GetGameObject(oidClient);
        if (pClientObject == NULL || pClientObject->AsSWCObject() == NULL)
        {
            nResult = DIALOG_RESULT_WAIT;
            CExoString sMessage("Waiting on listening object to spawn clientside: ");
            CExoString sTag(g_pAppManager->GetServerExoApp()
                ->GetGameObject(oidListener)->AsSWSObject()->GetTag().CStr());
            sMessage = sMessage + sTag;
            return 1;
        }
        nResult = DIALOG_RESULT_READY;
    }
    if (oidSpeaker != OBJECT_INVALID)
    {
        CGameObject* pServerObject = g_pAppManager->GetServerExoApp()
            ->GetGameObject(oidSpeaker);
        if (pServerObject == NULL || pServerObject->AsSWSObject() == NULL)
        {
            nResult = DIALOG_RESULT_WAIT;
            return 1;
        }
        nResult = DIALOG_RESULT_READY;
        unsigned long oidClient = g_pAppManager->GetClientExoApp()
            ->ServerToClientObjectId(oidSpeaker);
        CSWCObject* pClientObject = g_pAppManager->GetClientExoApp()
            ->GetGameObject(oidClient);
        if (pClientObject == NULL || pClientObject->AsSWCObject() == NULL)
        {
            nResult = DIALOG_RESULT_WAIT;
            return 1;
        }
        nResult = DIALOG_RESULT_READY;
    }
    if (oidCamera != OBJECT_INVALID)
    {
        CGameObject* pServerObject = g_pAppManager->GetServerExoApp()
            ->GetGameObject(oidCamera);
        if (pServerObject == NULL || pServerObject->AsSWSObject() == NULL)
        {
            nResult = DIALOG_RESULT_WAIT;
            return 1;
        }
        nResult = DIALOG_RESULT_READY;
        unsigned long oidClient = g_pAppManager->GetClientExoApp()
            ->ServerToClientObjectId(oidCamera);
        CSWCObject* pClientObject = g_pAppManager->GetClientExoApp()
            ->GetGameObject(oidClient);
        if (pClientObject == NULL || pClientObject->AsSWCObject() == NULL)
        {
            nResult = DIALOG_RESULT_WAIT;
            return 1;
        }
        nResult = DIALOG_RESULT_READY;
    }

    unsigned long nDay, nTime, nElapsedDay, nElapsedTime;
    int bRetryExpired = 0;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastEntryDay, m_nLastEntryTime, &nElapsedDay, &nElapsedTime);
    m_nLastEntryDay = nDay;
    m_nLastEntryTime = nTime;
    if (m_nAnimationRetryDelay > 0)
    {
        if (m_nAnimationRetryDelay <= nElapsedTime)
        {
            m_nAnimationRetryDelay = 0;
            bRetryExpired = 1;
        }
        else
        {
            m_nAnimationRetryDelay -= nElapsedTime;
        }
    }

    for (i = 0; i < nAnimations; ++i)
    {
        if (pDependant[i])
        {
            unsigned long oidAnimationObject = pAnimationObjects[i];
            unsigned long oidServer = g_pAppManager->GetServerExoApp()
                ->ClientToServerObjectId(oidAnimationObject);
            CGameObject* pServerObject = g_pAppManager->GetServerExoApp()
                ->GetGameObject(oidServer);
            if ((pServerObject != NULL && pServerObject->AsSWSObject() != NULL) ||
                bRetryExpired)
            {
                nResult = DIALOG_RESULT_READY;
                CSWCObject* pClientObject = g_pAppManager->GetClientExoApp()
                    ->GetGameObject(oidAnimationObject);
                if ((pClientObject != NULL && pClientObject->AsSWCObject() != NULL) ||
                    bRetryExpired)
                {
                    nResult = DIALOG_RESULT_READY;
                }
                else
                {
                    nResult = DIALOG_RESULT_WAIT;
                    if (m_nAnimationRetryDelay == 0)
                        m_nAnimationRetryDelay = ANIMATION_RETRY_DELAY;
                    break;
                }
            }
            else
            {
                nResult = DIALOG_RESULT_WAIT;
                if (m_nAnimationRetryDelay == 0)
                    m_nAnimationRetryDelay = ANIMATION_RETRY_DELAY;
                break;
            }
        }
    }
    if (nResult == DIALOG_RESULT_WAIT)
    {
        if (pAnimationObjects != NULL) delete[] pAnimationObjects;
        if (pAnimations != NULL) delete[] pAnimations;
        if (pDependant != NULL) delete[] pDependant;
        return 1;
    }

    int nCameraAngle = m_nPseudoRandomCameraAngles[m_nPseudoRandomCameraAngle];
    if (m_bPseudoRandomCameraDirection)
        m_nPseudoRandomCameraAngle += m_nPseudoRandomCameraStep;
    else
        m_nPseudoRandomCameraAngle -= m_nPseudoRandomCameraStep;
    if (m_nPseudoRandomCameraAngle >= PSEUDO_RANDOM_CAMERA_COUNT)
        m_nPseudoRandomCameraAngle = 0;
    if (m_nPseudoRandomCameraAngle < 0)
        m_nPseudoRandomCameraAngle = PSEUDO_RANDOM_CAMERA_COUNT - 1;
    if (entry.m_nCameraAngle == 0)
    {
        if (m_nF4)
            entry.m_nCameraAngle = CAMERA_ANGLE_SECOND;
        else
            entry.m_nCameraAngle = nCameraAngle;
    }
    m_nF4 = 0;
    int nAlienVOVariation = m_nPseudoRandomAlienVOVariations[m_nPseudoRandomAlienVO];
    if (m_bPseudoRandomAlienVODirection)
        m_nPseudoRandomAlienVO += m_nPseudoRandomAlienVOStep;
    else
        m_nPseudoRandomAlienVO -= m_nPseudoRandomAlienVOStep;
    if (m_nPseudoRandomAlienVO >= PSEUDO_RANDOM_ALIEN_VO_COUNT)
        m_nPseudoRandomAlienVO = 0;
    if (m_nPseudoRandomAlienVO < 0)
        m_nPseudoRandomAlienVO = PSEUDO_RANDOM_ALIEN_VO_COUNT - 1;
    entry.m_nAlienVOVariation = nAlienVOVariation;
    CExoString sText = GetExoStringFromLocString(entry.m_lsText, nGender);
    if (m_n40)
    {
        UnequipItemsForDialog(oidSpeaker);
        UnequipItemsForDialog(oidListener);
        UnequipItemsForDialog(oidCamera);
        for (i = 0; i < nAnimations; ++i)
            UnequipItemsForDialog(g_pAppManager->GetServerExoApp()
                ->ClientToServerObjectId(pAnimationObjects[i]));
    }
    if (m_n44)
    {
        UnequipHItemForDialog(oidSpeaker);
        UnequipHItemForDialog(oidListener);
        UnequipHItemForDialog(oidCamera);
        for (i = 0; i < nAnimations; ++i)
            UnequipHItemForDialog(g_pAppManager->GetServerExoApp()
                ->ClientToServerObjectId(pAnimationObjects[i]));
    }
    int bSoundActive = entry.m_bSoundActive;
    if (g_bDisableSound == 1)
        bSoundActive = 1;
    int bSoundExists = 0;
    if (entry.m_bSoundExists & CSWSDialogBase::SOUND_EXISTS_PRESENT)
        bSoundExists = 1;
    if ((entry.m_bSoundExists & CSWSDialogBase::SOUND_EXISTS_SKIP) || !bSoundExists)
        bSoundActive = 1;
    g_pAppManager->GetClientExoApp()->GetInGameGui()->HandleDialogEntry(
        nPlayerId, pClient->ServerToClientObjectId(oidTarget),
        pClient->ServerToClientObjectId(oidSpeaker),
        pClient->ServerToClientObjectId(oidListener),
        pClient->ServerToClientObjectId(oidCamera), nGender, sText,
        nAnimations, pAnimationObjects, pAnimations, entry.m_nCameraAngle,
        entry.m_nCameraId, entry.m_fCameraHeightOffset, entry.m_fTargetHeightOffset,
        entry.m_nWaitFlags, entry.m_nCameraAnimation, entry.m_fCameraFieldOfView,
        entry.m_nCameraVideoEffect, entry.m_nFadeType, entry.m_vFadeColor,
        entry.m_fFadeDelay, entry.m_fFadeLength, bSoundActive);
    if (pAnimationObjects != NULL) delete[] pAnimationObjects;
    if (pAnimations != NULL) delete[] pAnimations;
    if (pDependant != NULL) delete[] pDependant;
    return 1;
}
// PC 0x005EB8E0
int CSWSDialog::SendDialogReplies(CSWSObject* pOwner, unsigned long nPlayerId)
{

    int bResult = 1;
    int bFirstPlayer = 1;
    if ((unsigned long)m_nCurrentEntry >= (unsigned long)m_nEntries)
    {
        return 0;
    }

    unsigned long nReplies = m_pEntries[m_nCurrentEntry].m_nReplies + 1;
    CSWSDialogReply* pReplies = new CSWSDialogReply[nReplies];
    pReplies[0].m_lsText.m_dwStringRef = -1;
    pReplies[0].m_nReplyIndex = DIALOG_INDEX_INVALID;
    CExoLinkedListNode* pPosition = m_lstPlayers.GetHeadPos();
    CSWSDialogPlayer* pPlayer = m_lstPlayers.GetHead();
    do
    {
        unsigned long oidOldPlayer;
        if (pPlayer != NULL)
        {
            if (nPlayerId != -1 &&
                pPlayer->m_nPlayerId != nPlayerId)
            {
                bFirstPlayer = 0;
                pPlayer = m_lstPlayers.GetNext(pPosition);
                continue;
            }
            oidOldPlayer = m_oidCurrentPlayer;
            m_oidCurrentPlayer = pPlayer->m_oidPlayer;
        }
        else if (nPlayerId != -1)
        {
            break;
        }

        unsigned long nActive = 1;
        unsigned long nInactive = 0;
        unsigned long nReply = 0;
        for (; nReply < (unsigned long)m_pEntries[m_nCurrentEntry].m_nReplies; ++nReply)
        {
            int bActive1;
            int bHasActive1;
            if (m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_cActive == "")
            {
                bActive1 = 1;
                bHasActive1 = 0;
            }
            else
            {
                bHasActive1 = 1;
                g_pVirtualMachine->SetScriptParams(
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nParam1,
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nParam2,
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nParam3,
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nParam4,
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nParam5,
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_sParamStrA);
                bActive1 = CheckScript(pOwner, m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_cActive);
                if (m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_bNot)
                {
                    bActive1 = !bActive1;
                }
            }

            int bActive2;
            int bHasActive2;
            if (m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_cActive2 == "")
            {
                bActive2 = 1;
                bHasActive2 = 0;
            }
            else
            {
                bHasActive2 = 1;
                g_pVirtualMachine->SetScriptParams(
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nParam1b,
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nParam2b,
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nParam3b,
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nParam4b,
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nParam5b,
                    m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_sParamStrB);
                bActive2 = CheckScript(pOwner, m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_cActive2);
                if (m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_bNot2)
                {
                    bActive2 = !bActive2;
                }
            }

            int bActive;
            if (!bHasActive1)
            {
                if (bHasActive2)
                {
                    bActive = bActive2;
                }
                else
                {
                    bActive = 1;
                }
            }
            else if (!bHasActive2)
            {
                bActive = bActive1;
            }
            else if (m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nLogic == CSWSDialogActiveEntry::LOGIC_AND)
            {
                bActive = bActive1 && bActive2;
            }
            else
            {
                bActive = bActive1 || bActive2;
            }
            if (bActive)
            {
                if (m_bMultiPCDialogViewOnly != 0 && bFirstPlayer == 0)
                {
                    pReplies[nReplies - nInactive - 1] =
                        m_pReplies[m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nIndex];
                    pReplies[nReplies - nInactive - 1].m_nReplyIndex = nReply;
                    ++nInactive;
                }
                else
                {
                    pReplies[nActive] =
                        m_pReplies[m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nIndex];
                    pReplies[nActive].m_nReplyIndex = nReply;
                    ++nActive;
                }
            }
            else if (m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_bDisplayInactive)
            {
                pReplies[nReplies - nInactive - 1] =
                    m_pReplies[m_pEntries[m_nCurrentEntry].m_pReplies[nReply].m_nIndex];
                pReplies[nReplies - nInactive - 1].m_nReplyIndex = nReply;
                ++nInactive;
            }
        }
        for (unsigned long i = 0; i < nInactive; ++i)
        {
            pReplies[nActive + i] = pReplies[nReplies - nInactive + i];
        }

        if (pPlayer != NULL)
        {
            m_oidCurrentPlayer = oidOldPlayer;
            if (nActive + nInactive > 1)
            {
                if (nActive == 2 &&
                    m_pReplies[m_pEntries[m_nCurrentEntry].m_pReplies[pReplies[1].m_nReplyIndex].m_nIndex].m_nEntries == 0)
                {
                    m_bEndOfDialog = 1;
                }
                if (nPlayerId == -1)
                    m_nSelectedReply = DIALOG_INDEX_INVALID;
                bResult = SendDialogReplyNode(pPlayer->m_nPlayerId, pPlayer->m_oidPlayer,
                    pPlayer->m_nGender, pReplies + 1, nActive - 1, nInactive,
                    m_nCurrentEntry, m_bEndOfDialog);
            }
            else
            {
                m_bEndOfDialog = 1;
                if (m_bMultiPCDialogViewOnly != 0 && bFirstPlayer == 0)
                {
                    if (nPlayerId == -1)
                        m_nSelectedReply = DIALOG_INDEX_INVALID;
                    bResult = SendDialogReplyNode(pPlayer->m_nPlayerId, pPlayer->m_oidPlayer,
                        pPlayer->m_nGender, pReplies, nActive - 1, nInactive,
                        m_nCurrentEntry, m_bEndOfDialog);
                }
                else
                {
                    if (nPlayerId == -1)
                        m_nSelectedReply = DIALOG_INDEX_INVALID;
                    bResult = SendDialogReplyNode(pPlayer->m_nPlayerId, pPlayer->m_oidPlayer,
                        pPlayer->m_nGender, pReplies, 1, 0, m_nCurrentEntry, m_bEndOfDialog);
                }
            }
            bFirstPlayer = 0;
            pPlayer = m_lstPlayers.GetNext(pPosition);
        }
        else
        {
            if (nActive > 1)
            {
                if (nActive == 2 &&
                    m_pReplies[m_pEntries[m_nCurrentEntry].m_pReplies[pReplies[1].m_nReplyIndex].m_nIndex].m_nEntries == 0)
                {
                    m_bEndOfDialog = 1;
                }
                bResult = pOwner->ReplyDialog(-1,
                    pReplies[1].m_nReplyIndex, 0, m_nCurrentEntry, 0);
            }
            else
            {
                m_bEndOfDialog = 1;
                bResult = pOwner->ReplyDialog(-1,
                    pReplies[0].m_nReplyIndex, 0, m_nCurrentEntry, 0);
            }
        }
    } while (bResult != 0 && pPosition != NULL);
    delete[] pReplies;

    if (pOwner->m_pDialog != NULL)
    {
        if (nPlayerId == -1)
        {
            m_bRepliesSent = 1;
            m_nSelectedReply = DIALOG_INDEX_INVALID;
        }
        else if (m_nSelectedReply != DIALOG_INDEX_INVALID)
        {
            unsigned long oidSpeaker = m_oidCurrentPlayer;
            CExoString sListener(m_pReplies[m_pEntries[m_nCurrentEntry].m_pReplies[m_nSelectedReply].m_nIndex].m_sListener);
            if (sListener != "")
            {
                if (sListener.CompareNoCase("OWNER"))
                    oidSpeaker = pOwner->m_nId;
                else if (sListener.CompareNoCase("PLAYER"))
                    oidSpeaker = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                else
                {
                    CSWSObject* pSpeaker = GetSpeaker(pOwner, sListener);
                    if (pSpeaker != NULL)
                        oidSpeaker = pSpeaker->m_nId;
                }
            }
            CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
            pClient->GetInGameGui()->HandleDialogReplyChosen(nPlayerId, m_nSelectedReply,
                m_nCurrentEntry, GetExoStringFromLocString(
                    m_pReplies[m_pEntries[m_nCurrentEntry].m_pReplies[m_nSelectedReply].m_nIndex].m_lsText,
                    m_nCurrentPlayerGender), pClient->ServerToClientObjectId(oidSpeaker), m_nCurrentPlayerGender, m_bEndOfDialog);
        }
    }
    return bResult;
}
// PC 0x005E98F0
int CSWSDialog::SendDialogReplyNode(unsigned long nPlayerId, unsigned long oidPlayer,
    unsigned char nGender, CSWSDialogReply* pReplies, unsigned long nActive,
    unsigned long nInactive, unsigned long nEntry, int bEndOfDialog)
{
    int bText = 0;
    int bEmpty = 0;
    int nReplies = nActive + nInactive;
    for (int i = 0; i < nReplies; ++i)
    {
        CExoString sText;
        CSWSDialogReply& cReply = pReplies[i];
        cReply.m_lsText.GetString(
            g_pAppManager->GetClientExoApp()->GetClientLanguage(),
            &sText, nGender);
        if (sText == "")
            bEmpty = 1;
        else
            bText = 1;
    }
    if (bText && bEmpty)
    {
        CExoString sError("CONVERSATION ERROR: Last Conversation Node Contains Either an END NODE or CONTINUE NODE.  Please contact a Designer!");
        CGuiInGame* pGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
        pGui->m_bPendingDialogBark = 1;
        pGui->m_sPendingDialogBark = sError;
        return 0;
    }

    CGameObject* pObject;
    g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
        m_oidDialogOwner, &pObject);
    CSWSObject* pOwner = pObject->AsSWSObject();
    CExoString* psText = new CExoString[nReplies];
    unsigned long* pnReplyIndex = new unsigned long[nReplies];
    unsigned long* poidListener = new unsigned long[nReplies];
    unsigned long* pnCameraAngle = new unsigned long[nReplies];
    int* pnCameraId = new int[nReplies];
    float* pfCameraHeight = new float[nReplies];
    float* pfTargetHeight = new float[nReplies];
    unsigned long* pnWaitFlags = new unsigned long[nReplies];
    unsigned short* pnCameraAnimation = new unsigned short[nReplies];
    float* pfFieldOfView = new float[nReplies];
    int* pnVideoEffect = new int[nReplies];
    unsigned char* pnFadeType = new unsigned char[nReplies];
    Vector* pvFadeColor = new Vector[nReplies];
    float* pfFadeDelay = new float[nReplies];
    float* pfFadeLength = new float[nReplies];
    unsigned long* pnAnimations = new unsigned long[nReplies];
    unsigned long** ppoidAnimations = new unsigned long*[nReplies];
    unsigned short** ppnAnimations = new unsigned short*[nReplies];

    int i;
    int nCameraAngle = m_nPseudoRandomCameraAngles[m_nPseudoRandomCameraAngle];
    int bCameraForward = m_bPseudoRandomCameraDirection;
    if (bCameraForward)
        m_nPseudoRandomCameraAngle += m_nPseudoRandomCameraStep;
    else
        m_nPseudoRandomCameraAngle -= m_nPseudoRandomCameraStep;
    if (m_nPseudoRandomCameraAngle >= PSEUDO_RANDOM_CAMERA_COUNT)
        m_nPseudoRandomCameraAngle = 0;
    if (m_nPseudoRandomCameraAngle < 0)
        m_nPseudoRandomCameraAngle = PSEUDO_RANDOM_CAMERA_COUNT - 1;
    EXOLOG("SendDialogReplyNode: player %d, %d active replies, camera angle %d",
        &nPlayerId, &nActive, &nCameraAngle);

    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    for (i = 0; i < nReplies; ++i)
    {
        psText[i] = GetExoStringFromLocString(pReplies[i].m_lsText, nGender);
        pnReplyIndex[i] = pReplies[i].m_nReplyIndex;
        unsigned long oidListener = OBJECT_INVALID;
        if (pOwner != NULL && pReplies[i].m_sListener != "")
        {
            if (pReplies[i].m_sListener.CompareNoCase("OWNER"))
                oidListener = pClient->ServerToClientObjectId(m_oidDialogOwner);
            else if (pReplies[i].m_sListener.CompareNoCase("PLAYER"))
            {

                unsigned long oidPlayerCreature = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayerCreature);
            }
            else
            {
                CSWSObject* pListener = GetSpeaker(pOwner, pReplies[i].m_sListener);
                if (pListener != NULL)
                    oidListener = pClient->ServerToClientObjectId(pListener->GetId());
            }
        }
        int nAnimations = pReplies[i].m_nAnimationCount;
        pnAnimations[i] = nAnimations;
        ppoidAnimations[i] = NULL;
        ppnAnimations[i] = NULL;
        if (nAnimations != 0)
        {
            ppoidAnimations[i] = new unsigned long[nAnimations];
            ppnAnimations[i] = new unsigned short[nAnimations];
        }
        for (int j = 0; j < nAnimations; ++j)
        {
            unsigned long oidAnimation;
            if (pReplies[i].m_pAnimations[j].m_sParticipant == "PLAYER")
            {
                if (m_lstPlayers.GetHeadPos() != NULL)
                    oidAnimation = m_oidCurrentPlayer;
                else
                    oidAnimation = OBJECT_INVALID;
            }
            else if (pReplies[i].m_pAnimations[j].m_sParticipant == "OWNER")
                oidAnimation = m_oidDialogOwner;
            else if (pReplies[i].m_pAnimations[j].m_sParticipant == "NPC1")
            {
                CSWCCreature* pCreature = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(1);
                if (pCreature != NULL)
                    oidAnimation = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(pCreature->GetId());
                else
                    oidAnimation = OBJECT_INVALID;
            }
            else if (pReplies[i].m_pAnimations[j].m_sParticipant == "NPC2")
            {
                CSWCCreature* pCreature = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(2);
                if (pCreature != NULL)
                    oidAnimation = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(pCreature->GetId());
                else
                    oidAnimation = OBJECT_INVALID;
            }
            else
                oidAnimation = pOwner->GetNearestObjectByName(pReplies[i].m_pAnimations[j].m_sParticipant, 0.0f);

            if (g_pAppManager->GetServerExoApp()->GetGameObject(oidAnimation) != NULL)
            {
                ppoidAnimations[i][j] = pClient->ServerToClientObjectId(oidAnimation);
                ppnAnimations[i][j] = pReplies[i].m_pAnimations[j].m_nAnimation;
            }
            else
            {
                CExoString sError;
                sError.Format("Error: dialogue can't find object '%s'!", pReplies[i].m_pAnimations[j].m_sParticipant);
                ppoidAnimations[i][j] = OBJECT_INVALID;
                ppnAnimations[i][j] = 0;
            }
        }
        if (pReplies[i].m_nCameraAngle == 0)
        {
            if (m_nF8)
                pReplies[i].m_nCameraAngle = CAMERA_ANGLE_SECOND;
            else
                pReplies[i].m_nCameraAngle = nCameraAngle;
        }
        poidListener[i] = oidListener;
        pnCameraAngle[i] = pReplies[i].m_nCameraAngle;
        pnCameraId[i] = pReplies[i].m_nCameraId;
        pfCameraHeight[i] = pReplies[i].m_fCameraHeightOffset;
        pfTargetHeight[i] = pReplies[i].m_fTargetHeightOffset;
        pnWaitFlags[i] = pReplies[i].m_nWaitFlags;
        pnCameraAnimation[i] = pReplies[i].m_nCameraAnimation;
        pfFieldOfView[i] = pReplies[i].m_fCameraFieldOfView;
        pnVideoEffect[i] = pReplies[i].m_nCameraVideoEffect;
        pnFadeType[i] = pReplies[i].m_nFadeType;
        pvFadeColor[i] = pReplies[i].m_vFadeColor;
        pfFadeDelay[i] = pReplies[i].m_fFadeDelay;
        pfFadeLength[i] = pReplies[i].m_fFadeLength;
    }
    m_nF8 = 0;
    if (m_n40)
    {
        for (int i = 0; i < nReplies; ++i)
        {
            UnequipItemsForDialog(poidListener[i]);
            for (int j = 0; j < (int)pnAnimations[i]; ++j)
                UnequipItemsForDialog(ppoidAnimations[i][j]);
        }
    }
    if (m_n44)
    {
        for (int i = 0; i < nReplies; ++i)
        {
            UnequipHItemForDialog(poidListener[i]);
            for (int j = 0; j < (int)pnAnimations[i]; ++j)
                UnequipHItemForDialog(ppoidAnimations[i][j]);
        }
    }
    pClient->GetInGameGui()->HandleDialogReplies(nPlayerId,
        pClient->ServerToClientObjectId(oidPlayer), nGender, psText,
        pnReplyIndex, poidListener, pnCameraAngle, pnCameraId, pfCameraHeight,
        pfTargetHeight, pnWaitFlags, pnCameraAnimation, pfFieldOfView,
        pnVideoEffect, pnFadeType, pvFadeColor, pfFadeDelay, pfFadeLength,
        nActive, nInactive, nEntry, pnAnimations, ppoidAnimations,
        ppnAnimations, bEndOfDialog);

    if (pnReplyIndex != NULL)
        delete[] pnReplyIndex;
    if (poidListener != NULL)
        delete[] poidListener;
    if (psText != NULL)
        delete[] psText;
    if (pnCameraAngle != NULL)
        delete[] pnCameraAngle;
    if (pnCameraId != NULL)
        delete[] pnCameraId;
    if (pfCameraHeight != NULL)
        delete[] pfCameraHeight;
    if (pfTargetHeight != NULL)
        delete[] pfTargetHeight;
    if (pnWaitFlags != NULL)
        delete[] pnWaitFlags;
    if (pnCameraAnimation != NULL)
        delete[] pnCameraAnimation;
    if (pfFieldOfView != NULL)
        delete[] pfFieldOfView;
    if (pnVideoEffect != NULL)
        delete[] pnVideoEffect;
    if (pnFadeType != NULL)
        delete[] pnFadeType;
    if (pvFadeColor != NULL)
        delete[] pvFadeColor;
    if (pfFadeDelay != NULL)
        delete[] pfFadeDelay;
    if (pfFadeLength != NULL)
        delete[] pfFadeLength;
    for (int i = 0; i < nReplies; ++i)
    {
        unsigned long* poidAnimation = ppoidAnimations[i];
        unsigned short* pnAnimation = ppnAnimations[i];
        if (poidAnimation != NULL)
            delete[] poidAnimation;
        if (pnAnimation != NULL)
            delete[] pnAnimation;
    }
    if (ppoidAnimations != NULL)
        delete[] ppoidAnimations;
    if (ppnAnimations != NULL)
        delete[] ppnAnimations;
    if (pnAnimations != NULL)
        delete[] pnAnimations;
    return 1;
}
// PC 0x005E87C0
void CSWSDialog::UnequipHItemForDialog(unsigned long oidObject)
{
    if (oidObject == OBJECT_INVALID)
    {
        return;
    }

    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
    unsigned long oidClientObject = pClient->ServerToClientObjectId(oidObject);
    CSWCObject* pObject =
        g_pAppManager->m_pClientExoApp->GetGameObject(oidClientObject);
    if (pObject == NULL || pObject->AsSWCCreature() == NULL)
    {
        return;
    }

    CSWCCreature* pCreature = pObject->AsSWCCreature();
    CExoLinkedListNode* pPosition = m_lstUnequippedHeadItems.GetHeadPos();
    while (pPosition != NULL)
    {
        unsigned long* pId =
            (unsigned long*)m_lstUnequippedHeadItems.GetAtPos(pPosition);
        if (*pId == pCreature->m_nId)
        {
            return;
        }
        m_lstUnequippedHeadItems.GetNext(pPosition);
    }

    unsigned long* pId = new unsigned long;
    if (pId != NULL)
    {
        *pId = pCreature->m_nId;
        m_lstUnequippedHeadItems.AddHead(pId);
        pCreature->HideEquippedHeadItem();
    }
}
// PC 0x005E8880
void CSWSDialog::ReEquipHItemForDialog()
{
    unsigned long* pObjectId;
    CSWCObject* pGameObject;
    CSWCObject* pClientObject;
    CSWCCreature* pCreature;
    CExoLinkedListNode* pPosition = m_lstUnequippedHeadItems.GetHeadPos();

    while (pPosition != NULL)
    {
        pObjectId = (unsigned long*)m_lstUnequippedHeadItems.GetAtPos(pPosition);

        if (pObjectId != NULL)
        {
            unsigned long oidObject = *pObjectId;

            pGameObject =
                g_pAppManager->m_pClientExoApp->GetGameObject(oidObject);
            pClientObject = pGameObject;

            if (pGameObject != NULL && pGameObject->AsSWCCreature() != NULL)
            {
                pCreature = pClientObject->AsSWCCreature();
                pCreature->ShowEquippedHeadItem();
            }

            delete pObjectId;
        }

        m_lstUnequippedHeadItems.GetNext(pPosition);
    }
}
// PC 0x005E8910
CSWSDialogEntry::CSWSDialogEntry()
{
    m_nReplies = 0;
    m_pReplies = NULL;
}
// PC 0x005EC310
int CSWSDialog::SendDialogEntry(CSWSObject* pOwner, unsigned long nPlayerId,
                                unsigned long nEntry, unsigned long& nResult)
{
    CExoLinkedListNode* pPosition;
    unsigned long* pPlayerIds = NULL;
    CSWSObject* pSpeaker;
    if (nPlayerId != -1)
    {
        unsigned long oidListener = OBJECT_INVALID;
        CExoString sListener(m_pEntries[m_nCurrentEntry].m_sListener);
        if (sListener != "")
        {
            if (sListener.CompareNoCase("OWNER"))
                oidListener = pOwner->GetId();
            else if (sListener.CompareNoCase("PLAYER"))
                oidListener = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
            else
            {
                CSWSObject* pListener = GetSpeaker(pOwner, sListener);
                if (pListener != NULL)
                    oidListener = pListener->GetId();
            }
        }
        if (m_pEntries[m_nCurrentEntry].m_sSpeaker != "" &&
            !pOwner->m_sTag.CompareNoCase(m_pEntries[m_nCurrentEntry].m_sSpeaker))
        {
            pSpeaker = GetSpeaker(pOwner, m_pEntries[m_nCurrentEntry].m_sSpeaker);
            if (pSpeaker == NULL)
                return 0;
        }
        else
            pSpeaker = pOwner;
        m_n64 = m_pEntries[m_nCurrentEntry].m_bNodeUnskippable;
        int bResult = SendDialogEntryNode(nPlayerId, pOwner->GetId(),
            pSpeaker->GetId(), oidListener, m_oidCurrentPlayer,
            m_nCurrentPlayerGender, pOwner, m_pEntries[m_nCurrentEntry], nResult);
        if (nResult == DIALOG_RESULT_WAIT)
            return 1;
        if (bResult != 0 && m_bRepliesSent != 0)
            bResult = SendDialogReplies(pOwner, nPlayerId);
        return bResult;
    }

    if (nEntry >= (unsigned long)m_nEntries)
        return 0;
    if (m_pEntries[nEntry].m_sSpeaker != "" &&
        !pOwner->m_sTag.CompareNoCase(m_pEntries[nEntry].m_sSpeaker))
    {
        pSpeaker = GetSpeaker(pOwner, m_pEntries[nEntry].m_sSpeaker);
        if (pSpeaker == NULL)
            return 0;
    }
    else
        pSpeaker = pOwner;
    unsigned long oidListener = OBJECT_INVALID;
    CExoString sListener(m_pEntries[nEntry].m_sListener);
    if (sListener != "")
    {
        if (sListener.CompareNoCase("OWNER"))
            oidListener = pOwner->GetId();
        else if (sListener.CompareNoCase("PLAYER"))
            oidListener = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
        else
        {
            CSWSObject* pListener = GetSpeaker(pOwner, sListener);
            if (pListener != NULL)
                oidListener = pListener->GetId();
        }
    }
    m_bRepliesSent = 0;
    if (m_lstPlayers.GetHeadPos() != NULL)
    {
        pPosition = m_lstPlayers.GetHeadPos();
        CSWSDialogPlayer* pPlayer = m_lstPlayers.GetHead();
        while (pPlayer != NULL)
        {
            SendDialogEntryNode(pPlayer->m_nPlayerId, pOwner->GetId(),
                pSpeaker->GetId(), oidListener, m_oidCurrentPlayer,
                m_nCurrentPlayerGender, pOwner, m_pEntries[nEntry], nResult);
            if (nResult == DIALOG_RESULT_WAIT)
                return 1;
            AddJournalEntry(m_pEntries[nEntry].m_sQuest,
                m_pEntries[nEntry].m_nQuestEntry, pPlayer->m_nPlayerId);
            pPlayer = m_lstPlayers.GetNext(pPosition);
        }
        pPlayerIds = new unsigned long[m_lstPlayers.Count()];
        pPosition = m_lstPlayers.GetHeadPos();
        pPlayer = m_lstPlayers.GetHead();
        int nPlayer = 0;
        while (pPlayer != NULL)
        {
            pPlayerIds[nPlayer++] = pPlayer->m_nPlayerId;
            pPlayer = m_lstPlayers.GetNext(pPosition);
        }
    }
    GiveDialogPlotXP(m_pEntries[nEntry].m_nPlotIndex,
                     m_pEntries[nEntry].m_fPlotXPPercentage);
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    unsigned char nSoundExists = m_pEntries[nEntry].m_bSoundExists;
    if ((nSoundExists & CSWSDialogBase::SOUND_EXISTS_SKIP) &&
        !(nSoundExists & CSWSDialogBase::SOUND_EXISTS_VARIATION))
    {
        CExoString sVariation;
        sVariation.Format("%d", m_pEntries[nEntry].m_nAlienVOVariation);
        m_pEntries[nEntry].m_cSoundResRef += sVariation;
        m_pEntries[nEntry].m_bSoundExists |= CSWSDialogBase::SOUND_EXISTS_VARIATION;
    }
    if (g_bDebugDialogSound)
    {
        EXOLOG("SendDialogEntry: sound mode %d\n", g_pExoSound->GetSoundMode());
    }
    if (g_pExoSound->GetSoundMode() != CExoSound::CEXOSOUND_MODE_GENERIC)
    {
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    }
    if (g_bDebugDialogSound)
    {
        EXOLOG("SendDialogEntry: sound mode %d\n", g_pExoSound->GetSoundMode());
    }
    if (g_bDebugDialogSound)
    {
        char szSound[17];
        m_pEntries[nEntry].m_cSoundResRef.CopyToString(szSound);
        EXOLOG("SendDialogEntry: %s mode %d\n", szSound,
               g_pExoSound->GetSoundMode());
    }
    int bShowText = m_pEntries[nEntry].m_bSoundActive;
    g_pAppManager->GetClientExoApp()->GetInGameGui()->HandleDialogSound(
        pClient->ServerToClientObjectId(pSpeaker->GetId()),
        m_pEntries[nEntry].m_cSoundResRef);
    int bSound;
    char szSubDir[32];
    unsigned long i;
    if (g_pAppManager->GetClientExoApp()->GetModuleResourceLoaded(
            CClientExoApp::MODULE_RESOURCE_VOICE) == 1)
    {
        if (!g_bDisableSound)
        {
            int bSoundExists = 0;
            if (m_pEntries[nEntry].m_bSoundExists & CSWSDialogBase::SOUND_EXISTS_PRESENT)
                bSoundExists = 1;
            bSound = bSoundExists;
        }
        else
        {
            bSound = 0;
        }
    }
    else
    {
        if (!g_bDisableSound)
        {
            bSound = 1;
            CExoString sSound;
            m_pEntries[nEntry].m_cSoundResRef.CopyToString(sSound);
            unsigned long nLength = sSound.GetLength();
            if (nLength > 0)
            {
                char szPrefix[4];
                for (i = 0; i < 3; ++i)
                    szPrefix[i] = sSound[i];
                szPrefix[i] = 0;
                int nPrefix = atoi(szPrefix);
                if (_stricmp(szPrefix, "gbl") != 0 && _stricmp(szPrefix, "avo") != 0 &&
                    _stricmp(szPrefix, "000") != 0 && nPrefix <= 0)
                {
                    sSound.Format("HD0:STREAMVOICE\\%s", sSound.CStr());
                }
                else
                {
                    for (; i < nLength - 3; ++i)
                        szSubDir[i - 3] = sSound[i];
                    szSubDir[i - 3] = 0;
                    sSound.Format("HD0:STREAMVOICE\\%s\\%s\\%s",
                        szPrefix, szSubDir, sSound.CStr());
                }
            }
            else
            {
                sSound = "HD0:STREAMVOICE\\";
            }
            CExoString sFileName;
            sFileName = g_pExoBase->m_pExoAliasList->ResolveFileName(sSound, RESTYPE_BMU);
            FILE* pFile = fopen(sFileName.CStr(), "r");
            if (pFile == NULL)
            {
                sFileName = g_pExoBase->m_pExoAliasList->ResolveFileName(sSound, RESTYPE_WAV);
                pFile = fopen(sFileName.CStr(), "r");
            }
            if (pFile == NULL)
                bSound = 0;
            else
                fclose(pFile);
        }
        else
        {
            bSound = 0;
        }
        if (!bSound && !g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bSubtitles &&
            !bShowText)
        {
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->IsCameraDialog())
            {
                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nDialogScrollWait = 1;
                if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bDialogScrollFinished)
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->ShowDialogMessage();
            }
        }
    }
    if (pPlayerIds != NULL)
        delete[] pPlayerIds;
    if (m_lstPlayers.GetHeadPos() != NULL)
        m_oidLastEntrySpeaker = pSpeaker->GetId();
    if (bSound || (m_pEntries[nEntry].m_nWaitFlags & CSWSDialogBase::WAIT_EXPLICIT_DELAY))
    {
        SetDialogDelay(pOwner, m_pEntries[nEntry].m_lsText,
            m_pEntries[nEntry].m_nDelay, m_pEntries[nEntry].m_nReplies != 0, 0);
    }
    else if (m_pEntries[nEntry].m_nSoundDelay == CSWSDialogBase::DELAY_DEFAULT)
    {
        if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nC0 == 0)
        {
            SetDialogDelay(pOwner, m_pEntries[nEntry].m_lsText,
                (unsigned long)m_pEntries[nEntry].m_lsText.GetSpokenTimeEstimate(
                    g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                    m_nCurrentPlayerGender), m_pEntries[nEntry].m_nReplies != 0, 0);
        }
        else
        {
            pOwner->SetDialogDelay(0.0f);
        }
    }
    else
    {
        SetDialogDelay(pOwner, m_pEntries[nEntry].m_lsText,
            0, m_pEntries[nEntry].m_nReplies != 0, 0);
    }
    unsigned long nWaitFlags = m_pEntries[nEntry].m_nWaitFlags;
    pOwner->m_nWaitFlags = nWaitFlags;
    g_pVirtualMachine->SetScriptParams(
        m_pEntries[nEntry].m_nActionParam1,
        m_pEntries[nEntry].m_nActionParam2,
        m_pEntries[nEntry].m_nActionParam3,
        m_pEntries[nEntry].m_nActionParam4,
        m_pEntries[nEntry].m_nActionParam5,
        m_pEntries[nEntry].m_sActionParamStrA);
    RunScript(pOwner, m_pEntries[nEntry].m_cScript);
    g_pVirtualMachine->SetScriptParams(
        m_pEntries[nEntry].m_nActionParam1b,
        m_pEntries[nEntry].m_nActionParam2b,
        m_pEntries[nEntry].m_nActionParam3b,
        m_pEntries[nEntry].m_nActionParam4b,
        m_pEntries[nEntry].m_nActionParam5b,
        m_pEntries[nEntry].m_sActionParamStrB);
    RunScript(pOwner, m_pEntries[nEntry].m_cScript2);
    m_n64 = m_pEntries[nEntry].m_bNodeUnskippable;
    m_nCurrentEntry = nEntry;
    return pOwner->SendDialogReplies();
}
// PC 0x005ECD70
inline CSWSDialog::~CSWSDialog()
{
    if (m_n178 != NULL)
    {
        if (m_n178->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN) != NULL)
        {
            m_n178->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN)->StopOutOfOrderAnimation("emotions");
        }
        if (m_n178->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD) != NULL)
        {
            m_n178->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD)->StopOutOfOrderAnimation("emotions");
        }
        m_n178 = NULL;
    }
    ReEquipItemsForDialog();
    ReEquipHItemForDialog();
    Cleanup();
}

static inline void NwsdialogTrace1()
{
}
