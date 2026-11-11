#include "exobase.h"

#include "exoresman.h"
#include "exotlktable.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "swguiingame.h"

#include "swguiingame.h"
#include "swguiabilitieschargen.h"
#include "guiingame.h"
#include "nwitem.h"
#include "partytables.h"
#include "swparty.h"
#include "nwccreaturestats.h"
#include "nwcitem.h"
#include "swguiingamecharacter.h"
#include "swguiingameequip.h"

static inline void SwguiingameinventoryTrace2();
static inline void SwguiingameinventoryTrace1();

const int CSWGuiInGameInventory::m_nFilterName[6] =
{
    41822, 42165, 41818, 41821, 41819, 41820
};
#ifndef _XBOX
void CSWGuiInGameInventory::OnControlSelected(CSWGuiControl* pControl)  // PC 0x006B25E0  Xbox 0x000E80E0
{
    if (pControl->m_bSelected != 0)
    {
        CSWGuiControl* pSelected = m_lbxItems.GetSelectedControl();
        if (pSelected != 0)
        {
            unsigned long oidItem =
                static_cast<CSWGuiInGameItemEntry*>(pSelected)->m_oidItem;
            if (oidItem != OBJECT_INVALID)
            {
                CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                CSWSCreature* pCreature = pServer->GetPlayerCreature();
                if (pCreature->m_bCombatState != 0 &&
                    pCreature->m_nItemUseState == 1 &&
                    pCreature->m_nUseItemTimer != 0)
                {
                    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                    CGuiInGame* pGui = pClient->GetInGameGui();
                    CSWGuiMessageBox* pMessageBox = pGui->m_pMessageBox;
                    pMessageBox->SetAllowCancel(0);
                    pMessageBox->SetMessage(STRREF_ITEM_USED_THIS_ROUND);
                    pMessageBox->SetCallback(0, 0, 0);
                    m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                }
                else
                {
                    pCreature->m_bSafeProjectileBroadcast = 0;
                    pCreature->InstantUseItem(oidItem);
                    m_bUpdateItems = 1;
                    m_bRestoreSelection = 1;
                    m_oidSelectedItem = oidItem;
                    m_nSavedTopItem = m_lbxItems.m_nTopVisibleItem;
                    m_nSavedSelected = m_lbxItems.m_nSelectedItem;
                }
            }
        }
    }
}
#endif
void CSWGuiInGameInventory::CantEquip(CSWGuiControl* pControl)  // PC 0x006B26E0  Xbox 0x000E82B0
{
    if (pControl->m_bSelected != 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(42485);
        pMessageBox->SetCallback(0, 0, 0);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}
// PC 0x006B2740  Xbox 0x000E8310
void CSWGuiInGameInventory::NotUseable(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(42486);
        pMessageBox->SetCallback(0, 0, 0);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}
#ifndef _XBOX
void CSWGuiInGameInventory::NotUseableOnHawk(CSWGuiControl* pControl)  // PC 0x006B27A0
{
    if (pControl->m_bSelected != 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(49145);
        pMessageBox->SetCallback(0, 0, 0);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}
#endif
void CSWGuiInGameInventory::FullHealth(CSWGuiControl* pControl)  // PC 0x006B2800  Xbox 0x000E8370
{
    if (pControl->m_bSelected != 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(42499);
        pMessageBox->SetCallback(0, 0, 0);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}
#ifndef _XBOX
void CSWGuiInGameInventory::SquadFullHealth(CSWGuiControl* pControl)  // PC 0x006B2860
{
    if (pControl->m_bSelected != 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(48494);
        pMessageBox->SetCallback(0, 0, 0);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}

#include "nwccreature.h"

static inline void SwguiingameinventorySync3();
static inline void SwguiingameinventorySync2();
static inline void SwguiingameinventorySync1();
// PC 0x006B28C0  Xbox 0x000E83D0
void CSWGuiInGameInventory::UpdatePlayerStats()
{
    CSWCCreature* pCreature;

    if (m_nCharacter != -1 &&
        ((CSWServerInfo*)g_pAppManager->m_pServerExoApp->GetServerInfo())
                ->m_bPartyPicking != 0)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        pCreature = pClient->GetCreatureByGameObjectID(
            pClient->ServerToClientObjectId(
                pServer->GetPartyTable()->GetNPCObject(m_nCharacter, 0, 1)));
    }
    else
    {
        pCreature =
            g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
    }

    if (pCreature != 0)
    {
        CResRef* pPortrait = &pCreature->GetPortrait(1);
        m_lblPort.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
        {
            CExoString sDefense(pCreature->GetStats()->GetArmorClass(0));
            m_lblDef.m_cText.m_cParams.SetText(sDefense);
        }

        CExoString sVitality;
        short nCurrentHitPoints = pCreature->GetStats()->m_nCurrentHitPoints;
        sVitality.Format("%d/%d", nCurrentHitPoints,
                         pCreature->GetStats()->GetMaxHitPoints());
        m_lblVit.m_cText.m_cParams.SetText(sVitality);
        if (m_lblVit.m_cText.GetIdealHeight() > m_lblVit.m_cText.GetFontHeight())
        {
            short nCurrentHitPoints = pCreature->GetStats()->m_nCurrentHitPoints;
            sVitality.Format("%d/ %d", nCurrentHitPoints,
                             pCreature->GetStats()->GetMaxHitPoints());
            m_lblVit.m_cText.m_cParams.SetText(sVitality);
        }
    }
}
// PC 0x006B2A80  Xbox 0x000E8540
void CSWGuiInGameInventory::SetNextFilter()
{
    int nFilter =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nInventoryFilter + 1;
    if (nFilter >= 6)
    {
        nFilter = 0;
    }
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nInventoryFilter =
        nFilter;

    int nNextFilter = nFilter + 1;
    if (nNextFilter >= 6)
    {
        nNextFilter = 0;
    }

    CExoString sText = g_pTlkTable->GetSimpleString(0xA577) + " " +
                       g_pTlkTable->GetSimpleString(m_nFilterName[nNextFilter]);
    m_btnQuestItems.m_cText.m_cParams.SetText(sText);

    sText = g_pTlkTable->GetSimpleString(0x7DAB) + " - " +
            g_pTlkTable->GetSimpleString(m_nFilterName[nFilter]);
    m_lblInv.m_cText.m_cParams.SetText(sText);
}
// PC 0x006B2C90  Xbox 0x000E8750
int CSWGuiInGameInventory::CheckFilter(CSWSItem* pItem, CSWSCreature* pCreature,
                                       int nSlot)
{
    int bResult = 0;
    if (pItem == 0)
    {
        return 0;
    }
    switch (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nInventoryFilter)
    {
    case 0:
        bResult = 1;
        break;
    case 1:
        bResult = pItem->m_bNewItem;
        break;
    case 2:
        bResult = pItem->m_bPlotObject;
        break;
    case 3:
        if (pItem->GetBaseItem() != 0)
        {
            bResult = pItem->GetBaseItem()->m_nEquipableSlots != 0;
        }
        break;
    case 4:
        if (pItem->GetBaseItem() != 0)
        {
            if (pItem->m_bPlotObject == 0 &&
                pItem->GetBaseItem()->m_nEquipableSlots == 0)
            {
                bResult = 1;
            }
            else
            {
                bResult = 0;
            }
        }
        break;
    case 5:
        if (pCreature->GetClientCreature() != 0)
        {
            if (pCreature->GetClientCreature()->IsLeaderUsableItem(pItem, nSlot,
                                                                   0xFF) != 0 &&
                pCreature->CanUseItem(pItem, 0) != 0)
            {
                bResult = 1;
            }
            else
            {
                bResult = 0;
            }
        }
        break;
    }
    return bResult;
}
// PC 0x006B2DC0
void CSWGuiInGameInventory::UpdatePortraits()
{
    CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();

    if (pParty->m_nCharacters > 1)
    {
        m_btnChange1.m_bEnabled = 1;
        {
            CResRef* pPortrait = &pParty->GetCharacter(1)->GetPortrait(1);
            m_btnChange1.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
        }
        {
            CResRef* pPortrait = &pParty->GetCharacter(1)->GetPortrait(1);
            m_btnChange1.m_cHilite.m_cParams.SetFillImage(*pPortrait, 0);
        }
    }
    else
    {
        m_btnChange1.m_bEnabled = 0;
    }

    if (pParty->m_nCharacters > 2)
    {
        m_btnChange2.m_bEnabled = 1;
        {
            CResRef* pPortrait = &pParty->GetCharacter(2)->GetPortrait(1);
            m_btnChange2.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
        }
        {
            CResRef* pPortrait = &pParty->GetCharacter(2)->GetPortrait(1);
            m_btnChange2.m_cHilite.m_cParams.SetFillImage(*pPortrait, 0);
        }
    }
    else
    {
        m_btnChange2.m_bEnabled = 0;
    }
}
// PC 0x006B2EC0
void CSWGuiInGameInventory::OnChangeCharacter(CSWGuiControl* pControl)
{
    int nChange = 1;

    if (pControl->m_bSelected != 0)
    {
        CSWServerInfo* pServerInfo =
            (CSWServerInfo*)g_pAppManager->m_pServerExoApp->GetServerInfo();
        if (pServerInfo->m_bPartyPicking != 0)
        {
            char nCharacter = (char)pControl->m_nIndex;
            CSWCCreature* pCreature;
            if (nCharacter == -1)
            {
                pCreature = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
            }
            else
            {
                CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                unsigned long oidServer =
                    g_pAppManager->m_pServerExoApp->GetPartyTable()
                        ->GetNPCObject(nCharacter, 0, 1);
                pCreature = pClient->GetCreatureByGameObjectID(
                    pClient->ServerToClientObjectId(oidServer));
            }

            if (pCreature != 0)
            {
                if (m_btnSwitchLeft.m_bEnabled != 0)
                {
                    if (pControl == &m_btnChange1)
                    {
                        m_btnChange1.m_nIndex = m_btnChange2.m_nIndex;
                    }
                }
                else
                {
                    m_btnChange1.m_nIndex = -2;
                }
                m_nCharacter = nCharacter;
            }
        }
        else
        {
            if (pControl == &m_btnChange2)
            {
                nChange = 2;
            }
            if (g_pAppManager->m_pClientExoApp
                    ->ChangeCharacterToNextLivingPartyMember(0, 0, nChange) ==
                0)
            {
                return;
            }
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
        m_bUpdateItems = 1;
        UpdatePortraits();
    }
}
// PC 0x006B2FD0
void CSWGuiInGameInventory::OnSwitchRight(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        char nFirst = m_btnChange1.m_nIndex;
        int bFound = 0;
        if (nFirst != -2)
        {
            m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                m_btnChange1.m_cBorder.m_cParams.m_cFillImage, 0);
            m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                m_btnChange1.m_cHilite.m_cParams.m_cFillImage, 0);
            m_btnChange2.m_nIndex = m_btnChange1.m_nIndex;
            int nNPC = nFirst - 1;
            while (nNPC >= 0 && bFound == 0)
            {
                if (nNPC != m_nCharacter)
                {
                    if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetIsNPCAvailable(nNPC) != 0)
                    {
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        unsigned long oidNPC =
                            g_pAppManager->m_pServerExoApp->GetPartyTable()
                                ->GetNPCObject(nNPC, 0, 1);
                        CSWCCreature* pCreature =
                            pClient->GetCreatureByGameObjectID(
                                pClient->ServerToClientObjectId(oidNPC));
                        if (pCreature != 0)
                        {
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            m_btnChange1.m_nIndex = nNPC;
                            bFound = 1;
                        }
                    }
                }
                --nNPC;
            }
            if (bFound == 0)
            {
                if (m_nCharacter == -1 ||
                    (char)m_btnChange2.m_nIndex == -1)
                {
                    for (int nHigh = 8; nHigh > nFirst && bFound == 0; --nHigh)
                    {
                        if (nHigh != m_nCharacter)
                        {
                            if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                                    ->GetIsNPCAvailable(nHigh) != 0)
                            {
                                CClientExoApp* pClient =
                                    g_pAppManager->m_pClientExoApp;
                                unsigned long oidNPC =
                                    g_pAppManager->m_pServerExoApp
                                        ->GetPartyTable()
                                        ->GetNPCObject(nHigh, 0, 1);
                                CSWCCreature* pCreature =
                                    pClient->GetCreatureByGameObjectID(
                                        pClient->ServerToClientObjectId(
                                            oidNPC));
                                if (pCreature != 0)
                                {
                                    {
                                        CResRef* pPortrait =
                                            &pCreature->GetPortrait(1);
                                        m_btnChange1.m_cBorder.m_cParams
                                            .SetFillImage(*pPortrait, 0);
                                    }
                                    {
                                        CResRef* pPortrait =
                                            &pCreature->GetPortrait(1);
                                        m_btnChange1.m_cHilite.m_cParams
                                            .SetFillImage(*pPortrait, 0);
                                    }
                                    m_btnChange1.m_nIndex = nHigh;
                                    bFound = 1;
                                }
                            }
                        }
                    }
                }
                else
                {
                    {
                        CResRef* pPortrait =
                            &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                                 ->GetPortrait(1);
                        m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    {
                        CResRef* pPortrait =
                            &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                                 ->GetPortrait(1);
                        m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    m_btnChange1.m_nIndex = -1;
                }
            }
        }
    }
}
// PC 0x006B3250
void CSWGuiInGameInventory::OnSwitchLeft(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        char nSecond = m_btnChange2.m_nIndex;
        int bFound = 0;
        if (nSecond != -2)
        {
            m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                m_btnChange2.m_cBorder.m_cParams.m_cFillImage, 0);
            m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                m_btnChange2.m_cHilite.m_cParams.m_cFillImage, 0);
            m_btnChange1.m_nIndex = m_btnChange2.m_nIndex;
            int nNPC = nSecond + 1;
            while (nNPC < 9 && bFound == 0)
            {
                if (nNPC != m_nCharacter)
                {
                    if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetIsNPCAvailable(nNPC) != 0)
                    {
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        unsigned long oidNPC =
                            g_pAppManager->m_pServerExoApp->GetPartyTable()
                                ->GetNPCObject(nNPC, 0, 1);
                        CSWCCreature* pCreature =
                            pClient->GetCreatureByGameObjectID(
                                pClient->ServerToClientObjectId(oidNPC));
                        if (pCreature != 0)
                        {
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            m_btnChange2.m_nIndex = nNPC;
                            bFound = 1;
                        }
                    }
                }
                ++nNPC;
            }
            if (bFound == 0)
            {
                if (m_nCharacter != -1)
                {
                    {
                        CResRef* pPortrait =
                            &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                                 ->GetPortrait(1);
                        m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    {
                        CResRef* pPortrait =
                            &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                                 ->GetPortrait(1);
                        m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    m_btnChange2.m_nIndex = -1;
                }
                else
                {
                    for (int nLow = 0; nLow < nSecond && bFound == 0; ++nLow)
                    {
                        if (nLow != m_nCharacter)
                        {
                            if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                                    ->GetIsNPCAvailable(nLow) != 0)
                            {
                                CClientExoApp* pClient =
                                    g_pAppManager->m_pClientExoApp;
                                unsigned long oidNPC =
                                    g_pAppManager->m_pServerExoApp
                                        ->GetPartyTable()
                                        ->GetNPCObject(nLow, 0, 1);
                                CSWCCreature* pCreature =
                                    pClient->GetCreatureByGameObjectID(
                                        pClient->ServerToClientObjectId(
                                            oidNPC));
                                if (pCreature != 0)
                                {
                                    {
                                        CResRef* pPortrait =
                                            &pCreature->GetPortrait(1);
                                        m_btnChange2.m_cBorder.m_cParams
                                            .SetFillImage(*pPortrait, 0);
                                    }
                                    {
                                        CResRef* pPortrait =
                                            &pCreature->GetPortrait(1);
                                        m_btnChange2.m_cHilite.m_cParams
                                            .SetFillImage(*pPortrait, 0);
                                    }
                                    m_btnChange2.m_nIndex = nLow;
                                    bFound = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
// PC 0x006B34C0  Xbox 0x000E9570
CSWGuiInGameInventory::CSWGuiInGameInventory(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_bUpdateItems = 0;
    m_bRestoreSelection = 0;
    m_oidSelectedItem = OBJECT_INVALID;
    m_nCharacter = -1;
    StartLoadFromLayout("inventory");
    InitControl(m_lbxDescription, "LB_DESCRIPTION", 1);
    InitControl(m_lbxItems, "LB_ITEMS", 1);
    InitControl(m_lblInv, "LBL_INV", 1);
    InitControl(m_lblCredits, "LBL_CREDITS", 1);
    InitControl(m_lblCreditsValue, "LBL_CREDITS_VALUE", 1);
    InitControl(m_lblVit, "LBL_VIT", 1);
    InitControl(m_lblDef, "LBL_DEF", 1);
    InitControl(m_lblBgStats, "LBL_BGSTATS", 1);
    InitControl(m_lblBgPort, "LBL_BGPORT", 1);
    InitControl(m_lblPort, "LBL_PORT", 1);
    InitControl(m_btnExit, "BTN_EXIT", 1);
    InitControl(m_btnUseItem, "BTN_USEITEM", 1);
    InitControl(m_btnQuestItems, "BTN_QUESTITEMS", 1);
    InitControl(m_btnChange1, "BTN_CHANGE1", 1);
    InitControl(m_btnChange2, "BTN_CHANGE2", 1);
    StopLoadFromLayout();

    m_btnSwitchRight.SetFocusable(0);
    m_btnSwitchRight.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                              (CSWGuiHandler)&CSWGuiInGameInventory::OnSwitchRight);
    m_btnSwitchLeft.SetFocusable(0);
    m_btnSwitchLeft.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                             (CSWGuiHandler)&CSWGuiInGameInventory::OnSwitchLeft);
    m_btnSwitchLeft.m_cBorder.m_cParams.m_fRotation = 180.0f;
    m_btnSwitchLeft.m_cHilite.m_cParams.m_fRotation = 180.0f;

    m_lblUnknown64.Initialize(m_lbxDescription.m_pProtoItem->m_cExtent,
                              (CSWGuiLabel*)m_lbxDescription.m_pProtoItem,
                              1.0f);
    CSWGuiListBox* pItems = &m_lbxItems;
    pItems->m_bShowScrollBar = 1;
    SetActiveControl(pItems, 0);

    int nNextFilter =
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nInventoryFilter + 1;
    if (nNextFilter >= 6)
    {
        nNextFilter = 0;
    }

    CExoString sText = g_pTlkTable->GetSimpleString(42359) + " " +
                       g_pTlkTable->GetSimpleString(m_nFilterName[nNextFilter]);
    m_btnQuestItems.m_cText.m_cParams.SetText(sText);

    sText = g_pTlkTable->GetSimpleString(32171) + " - " +
            g_pTlkTable->GetSimpleString(m_nFilterName[
                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nInventoryFilter]);
    m_lblInv.m_cText.m_cParams.SetText(sText);

    m_btnExit.SetFocusable(0);
    m_btnExit.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                       (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnUseItem.SetFocusable(0);
    m_btnUseItem.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                          (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
    m_btnQuestItems.SetFocusable(0);
    m_btnQuestItems.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                             (CSWGuiHandler)&CSWGuiPanel::OnXButtonPressed);
    m_btnChange1.SetFocusable(0);
    m_btnChange1.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                          (CSWGuiHandler)&CSWGuiInGameInventory::OnChangeCharacter);
    m_btnChange2.SetFocusable(0);
    m_btnChange2.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                          (CSWGuiHandler)&CSWGuiInGameInventory::OnChangeCharacter);

    m_btnChange1.SetToolTip(38693, GUIEVENT_NEXT_CHARACTER);
#ifndef _XBOX
    m_btnChange2.m_nToolTipStrRef = 38693;
#endif
}
// PC 0x006B3D10  Xbox 0x000E8E80
void CSWGuiInGameInventory::OnControlEntered(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        unsigned long oidItem = static_cast<CSWGuiInGameItemEntry*>(pControl)->m_oidItem;
        if (oidItem != OBJECT_INVALID)
        {
            CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
            CExoString sDescription = pItem->GetPropertyDescription();
            m_lblUnknown64.m_cText.m_cParams.SetText(sDescription);
            if (m_nCharacter != -1 &&
                ((CSWServerInfo*)g_pAppManager->m_pServerExoApp->GetServerInfo())->m_bPartyPicking)
                m_bUpdateUseItem = 0;
            else
                m_bUpdateUseItem = pControl->m_nIndex;
            if (pItem->m_bNewItem)
                m_aNewItems.AddUnique(oidItem);
        }
        else
        {
            m_bUpdateUseItem = 0;
            CExoString sDescription("Error: Invalid item");
            m_lblUnknown64.m_cText.m_cParams.SetText(sDescription);
        }
        m_lbxDescription.ClearItems();
        CSWGuiExtent cExtent = m_lblUnknown64.m_cExtent;
        if (m_lblUnknown64.m_cText.m_cParams.m_sText.GetLength() > 0)
            cExtent.m_nHeight = m_lblUnknown64.m_cText.GetIdealHeight();
        else
            cExtent.m_nHeight = m_lblUnknown64.m_cText.GetFontHeight();
        m_lblUnknown64.SetExtent(cExtent);
        CSWGuiControl* pDescription = &m_lblUnknown64;
        m_lbxDescription.AddControls(&pDescription, 1, 0, 0, 0);
        m_lbxDescription.SetSelectedControl(&m_lblUnknown64, 0);
    }
}
// PC 0x006B3ED0  Xbox 0x000E9080
void CSWGuiInGameInventory::HandleInputEvent(int nEvent, int nState)
{
    SwguiingameinventoryTrace2();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 58:
            m_lbxDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case 57:
            m_lbxDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case 206:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            g_pAppManager->m_pClientExoApp
                ->ChangeCharacterToNextLivingPartyMember(0, 0, 1);
            m_bUpdateItems = 1;
            UpdatePortraits();
            break;
        case 40:
        case 45:
        case 46:
        case 223:
            if (((CGuiInGame*)g_pAppManager->m_pClientExoApp->GetInGameGui())
#ifdef _XBOX
                    ->HideSWInGameGui() != 0)
#else
                    ->HideSWInGameGui(0) != 0)
#endif
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            }
            break;
        case 41:
            SetNextFilter();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            m_bUpdateItems = 1;
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x006B40C0  Xbox 0x000E9260
CSWGuiInGameInventory::~CSWGuiInGameInventory()
{
    SwguiingameinventoryTrace2();
    m_lbxDescription.ClearItems();
    m_lbxItems.ClearItems();
    for (int i = 0; i < m_aItemEntries.num; i++)
    {
        if (m_aItemEntries.element[i] != 0)
        {
            delete m_aItemEntries.element[i];
        }
        m_aItemEntries.element[i] = 0;
    }
    m_aItemEntries.num = 0;
}
// PC 0x006B4430  Xbox 0x000E9510
void CSWGuiInGameInventory::OnPanelRemoved()
{
    SwguiingameinventoryTrace2();
    m_lbxItems.ClearItems();
    for (int i = 0; i < m_aItemEntries.num; i++)
    {
        if (m_aItemEntries[i] != 0)
        {
            delete m_aItemEntries[i];
        }
        m_aItemEntries[i] = 0;
    }
    m_aItemEntries.num = 0;
    if (m_aItemEntries.element != 0)
    {
        delete m_aItemEntries.element;
        m_aItemEntries.element = 0;
        m_aItemEntries.array_size = 0;
    }
    m_aItemEntries.num = 0;
    g_pAppManager->m_pServerExoApp->GetPartyTable()
        ->GetPartyItems()
        ->ClearNewItems(&m_aNewItems);
}
// PC 0x006B44D0
void CSWGuiInGameInventory::CreateItemEntry(
    CSWCCreature* pCreature, int& nItem,
    CExoArrayList<CSWGuiControl*>& lControls, CSWSItem* pItem, int bSquadItem,
    int bEquipped)
{
    CSWGuiInGameItemEntry* pItemEntry;
    if (nItem == m_aItemEntries.num)
    {
        pItemEntry = new CSWGuiInGameItemEntry;
        m_aItemEntries.Add(pItemEntry);
        CSWGuiButton* pProto = m_lbxItems.m_pProtoItem->AsButton();
        pItemEntry->Initialize(pProto->m_cText, pProto->m_cBorder,
                               pProto->m_cHilite,
                               m_lbxItems.m_cItemExtent.m_nWidth -
                                   2 * m_lbxItems.m_nPadding,
                               1);
        pItemEntry->AddEvent(0, this,
                            (CSWGuiHandler)&CSWGuiInGameInventory::OnControlEntered);
    }
    else
    {
        pItemEntry = (CSWGuiInGameItemEntry*)m_aItemEntries[nItem];
    }

    if (m_nCharacter != -1 &&
        ((CSWServerInfo*)g_pAppManager->m_pServerExoApp->GetServerInfo())
                ->m_bPartyPicking != 0)
    {
        pItemEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                            (CSWGuiHandler)&CSWGuiInGameInventory::NotUseableOnHawk);
        pItemEntry->m_nIndex = 0;
    }
    else if (pCreature->IsLeaderUsableItem(pItem, bSquadItem, 0xFF) != 0)
    {
        int nItemType = pItem->GetBaseItem()->m_nItemType;
        if ((nItemType == CSWBaseItem::ITEM_TYPE_REPAIR_KIT ||
             nItemType == CSWBaseItem::ITEM_TYPE_MEDPAC) &&
            pCreature->m_pStats->GetCurrentHitPoints() >=
                pCreature->m_pStats->GetMaxHitPoints())
        {
            CSWSCreature* pServerCreature = pCreature->GetServerCreature();
            if (pServerCreature == 0 || pServerCreature->m_nPoisonState == 0)
            {
                pItemEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                                    (CSWGuiHandler)&CSWGuiInGameInventory::FullHealth);
                pItemEntry->m_nIndex = 0;
            }
        }
        else if (nItemType == CSWBaseItem::ITEM_TYPE_SQUAD_MEDPAC)
        {
            int bNeedsHealing = 0;
            if (pCreature->m_pStats->GetCurrentHitPoints() <
                pCreature->m_pStats->GetMaxHitPoints())
                bNeedsHealing = 1;
            CSWCCreature* pPartyCreature =
                g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(1);
            if (pPartyCreature != 0 && pPartyCreature->GetServerCreature() != 0)
            {
                if (pPartyCreature->m_pStats->GetCurrentHitPoints() <
                    pPartyCreature->m_pStats->GetMaxHitPoints())
                    bNeedsHealing |= 1;
            }
            pPartyCreature =
                g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(2);
            if (pPartyCreature != 0 && pPartyCreature->GetServerCreature() != 0)
            {
                if (pPartyCreature->m_pStats->GetCurrentHitPoints() <
                    pPartyCreature->m_pStats->GetMaxHitPoints())
                    bNeedsHealing |= 1;
            }
            CSWSCreature* pServerCreature = pCreature->GetServerCreature();
            if (bNeedsHealing == 0 || pServerCreature == 0)
            {
                pItemEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                                    (CSWGuiHandler)&CSWGuiInGameInventory::SquadFullHealth);
                pItemEntry->m_nIndex = 0;
            }
        }
        else
        {
            pItemEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                                (CSWGuiHandler)&CSWGuiInGameInventory::OnControlSelected);
            pItemEntry->m_nIndex = 1;
        }
    }
    else
    {
        if (pItem->GetBaseItem()->GetEquipableSlots() != 0)
        {
            pItemEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                                (CSWGuiHandler)&CSWGuiInGameInventory::CantEquip);
            pItemEntry->m_nIndex = 0;
        }
        else
        {
            pItemEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                                (CSWGuiHandler)&CSWGuiInGameInventory::NotUseable);
            pItemEntry->m_nIndex = 0;
        }
    }
    pItemEntry->SetItem(pItem->m_nId, bEquipped, pItem->m_bNewItem);
    pItemEntry->SetActive(0);
    lControls.Add(pItemEntry);
    ++nItem;
}
// PC 0x006B4810  Xbox 0x000E8A90
void CSWGuiInGameInventory::PopulateItemListBox()
{
    CSWCCreature* pCreature;
    if (m_nCharacter != -1 &&
        ((CSWServerInfo*)g_pAppManager->m_pServerExoApp->GetServerInfo())
                ->m_bPartyPicking != 0)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        pCreature = pClient->GetCreatureByGameObjectID(
            pClient->ServerToClientObjectId(
                pServer->GetPartyTable()->GetNPCObject(m_nCharacter, 0, 1)));
    }
    else
    {
        pCreature = g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
    }
    CSWSCreature* pServerCreature = pCreature->GetServerCreature();
    CItemRepository* pRepository = pServerCreature->GetItemRepository(1);
    m_lblCreditsValue.m_cText.m_cParams.SetText(
        CExoString(pServerCreature->GetGold()));

    CExoArrayList<CSWGuiControl*> lControls;
    int nItem = 0;
    int nSelectedItem = -1;
    for (unsigned long nSlot = 0; nSlot < 14; ++nSlot)
    {
        unsigned long nInventorySlot = 1 << nSlot;
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        CSWCItem* pClientItem = pClient->GetItemByGameObjectID(
            pCreature->GetEquippedItemID(nInventorySlot));
        if (pClientItem != NULL)
        {
            CSWSItem* pItem = pClientItem->GetServerItem();
            int bSquadItem = nInventorySlot == INVENTORY_SLOT_RIGHTRING ||
                nInventorySlot == INVENTORY_SLOT_NECK ||
                nInventorySlot == INVENTORY_SLOT_ARMS;
            if (pItem != NULL && !pItem->m_bDeleting &&
                CheckFilter(pItem, pServerCreature, bSquadItem) != 0)
            {
                if (m_oidSelectedItem == pItem->GetId())
                {
                    nSelectedItem = nItem;
                    m_oidSelectedItem = OBJECT_INVALID;
                }
                CreateItemEntry(pCreature, nItem, lControls, pItem, bSquadItem, 1);
            }
        }
    }
    for (unsigned long i = 0; i < pRepository->GetNumItems(); ++i)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != NULL && !pItem->m_bDeleting &&
            CheckFilter(pItem, pServerCreature, 0) != 0)
        {
            if (m_oidSelectedItem == pItem->GetId())
            {
                nSelectedItem = nItem;
                m_oidSelectedItem = OBJECT_INVALID;
            }
            CreateItemEntry(pCreature, nItem, lControls, pItem, 0, 0);
        }
    }

    m_lbxItems.AddControls(lControls, 0, 1, 0);
    if (lControls.num > 0)
    {
        if (m_bRestoreSelection)
        {
            m_lbxItems.SetTopVisible(m_nSavedTopItem);
            m_lbxItems.SetSelectedControl(min((int)m_nSavedSelected, lControls.num - 1), 0);
            m_bRestoreSelection = 0;
        }
        else
        {
            m_lbxItems.SetSelectedControl(nSelectedItem == -1 ? 0 : nSelectedItem, 0);
        }
    }
    else
    {
        m_btnUseItem.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        m_btnUseItem.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_ICON_COLOR;
        m_lbxItems.SetSelectedControl(-1, 0);
        CExoString sEmpty("");
        m_lblUnknown64.m_cText.m_cParams.SetText(sEmpty);
    }
    m_bUpdateItems = 0;
}
// PC 0x006B4BB0  Xbox 0x000E9470
void CSWGuiInGameInventory::Draw(float fElapsed)
{
    SwguiingameinventorySync3();
    if (m_bUpdateItems)
    {
        PopulateItemListBox();
    }
    UpdatePlayerStats();
    if (m_bUpdateUseItem)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature =
            pServer->GetPlayerCreature();
        if (pCreature->m_nUseItemTimer != 0)
        {
            m_btnUseItem.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        }
        else
        {
            m_btnUseItem.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
        }
    }
    else
    {
        m_btnUseItem.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
    }
    return CSWGuiPanel::Draw(fElapsed);
}
// PC 0x006B4C30  Xbox 0x000E94A0
void CSWGuiInGameInventory::OnPanelAdded()
{
    SwguiingameinventorySync3();
    m_nCharacter = -1;
    m_btnChange1.m_nIndex = -2;
    m_btnChange2.m_nIndex = -2;
    UpdatePortraits();
    PopulateItemListBox();
    m_btnExit.HandleMouseOver(0);
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    g_pAppManager->m_pClientExoApp->ShowTutorialWindow(20, OBJECT_INVALID,
                                                       OBJECT_INVALID, 0);
}

static inline void SwguiingameinventoryTrace2()
{
    SwguiingameinventoryTrace1();
}

static inline void SwguiingameinventoryTrace1()
{
}

static inline void SwguiingameinventorySync3()
{
    SwguiingameinventorySync2();
}

static inline void SwguiingameinventorySync2()
{
    SwguiingameinventorySync1();
}

static inline void SwguiingameinventorySync1()
{
}
#endif

#ifdef _XBOX
#include "nwccreature.h"
#include "exoinput.h"
// PC 0x006B34C0  Xbox 0x000E9570
CSWGuiInGameInventory::CSWGuiInGameInventory(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    CSWGuiButton btnItem;
    m_bUpdateItems = 0;
    m_oidSelectedItem = OBJECT_INVALID;
    StartLoadFromLayout("inventory");
    InitControl(m_lbxDescription, "LB_DESCRIPTION", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblX, "LBL_X", 1);
    InitControl(m_lblInv, "LBL_INV", 1);
    InitControl(m_lblUseItem, "LBL_USEITEM", 1);
    InitControl(m_lblFilter, "LBL_FILTER", 1);
    InitControl(m_lblCredits, "LBL_CREDITS", 1);
    InitControl(m_lblCreditsValue, "LBL_CREDITS_VALUE", 1);
    InitControl(m_lblVit, "LBL_VIT", 1);
    InitControl(m_lblDef, "LBL_DEF", 1);
    InitControl(m_lblBgStats, "LBL_BGSTATS", 1);
    InitControl(m_lblBgPort, "LBL_BGPORT", 1);
    InitControl(m_lblPort, "LBL_PORT", 1);
    InitControl(m_lblStart, "LBL_START", 1);
    InitControl(m_lblExit, "LBL_EXIT", 1);
    InitControl(m_lblBlack, "LBL_BLACK", 1);
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
    InitControl(m_lblDownarrow, "LBL_DOWNARROW", 1);
    InitControl(m_lblUparrow, "LBL_UPARROW", 1);
    for (int nRow = 0; nRow < NUM_ITEM_ROWS; nRow++)
    {
        InitControl(btnItem, CExoString("BTN_ITEM") + CExoString(nRow + 1), 0);
        m_aItemRows[nRow].Initialize(btnItem.m_cText, btnItem.m_cBorder,
                                     btnItem.m_cHilite,
                                     btnItem.m_cExtent.m_nWidth, 1);
        m_aItemRows[nRow].SetExtent(btnItem.m_cExtent);
        m_aItemRows[nRow].AddEvent(GUIEVENT_MOUSE_ENTER, this,
            (CSWGuiHandler)&CSWGuiInGameInventory::OnControlEntered);
        AddControl(&m_aItemRows[nRow]);
    }
    StopLoadFromLayout();
    for (int nItemRow = 0; nItemRow < NUM_ITEM_ROWS; nItemRow++)
    {
        for (int nDirection = 0; nDirection < 4; nDirection++)
        {
            m_aItemRows[nItemRow].SetMoveToControl(nDirection, NULL);
        }
    }
    m_lblDownarrow.m_cBorder.m_cParams.m_fRotation = 180.0f;
    m_lblUnknown64.Initialize(m_lbxDescription.m_pProtoItem->m_cExtent,
                              (CSWGuiLabel*)m_lbxDescription.m_pProtoItem);

    int nNextFilter =
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nInventoryFilter + 1;
    if (nNextFilter >= 6)
    {
        nNextFilter = 0;
    }

    CExoString sText = g_pTlkTable->GetSimpleString(42359) + " " +
                       g_pTlkTable->GetSimpleString(m_nFilterName[nNextFilter]);
    m_lblFilter.m_cText.m_cParams.SetText(sText);

    sText = g_pTlkTable->GetSimpleString(32171) + " - " +
            g_pTlkTable->GetSimpleString(m_nFilterName[
                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nInventoryFilter]);
    m_lblInv.m_cText.m_cParams.SetText(sText);
}
// PC 0x006B40C0  Xbox 0x000E9260
CSWGuiInGameInventory::~CSWGuiInGameInventory()
{
    m_lbxDescription.ClearItems();
    m_lItems.RemoveAll();
}
// PC 0x006B4BB0  Xbox 0x000E9470
void CSWGuiInGameInventory::Draw(float fElapsed)
{
    if (m_bUpdateItems)
    {
        PopulateItemListBox();
    }
    UpdatePlayerStats();
    CSWGuiPanel::Draw(fElapsed);
}
// PC 0x006B4C30  Xbox 0x000E94A0
void CSWGuiInGameInventory::OnPanelAdded()
{
    m_lblBlack.SetShown(g_pAppManager->m_pClientExoApp->GetSWParty()->GetNumCharacters() > 1);
    m_lblRanalog.SetShown(0);
    PopulateItemListBox();
    g_pAppManager->m_pClientExoApp->ShowTutorialWindow(20, OBJECT_INVALID,
                                                       OBJECT_INVALID, 0);
}
// PC 0x006B4430  Xbox 0x000E9510
void CSWGuiInGameInventory::OnPanelRemoved()
{
    m_lItems.RemoveAll();
    g_pAppManager->m_pServerExoApp->GetPartyTable()
        ->GetPartyItems()
        ->ClearNewItems(&m_aNewItems);
}
// PC 0x006B3ED0  Xbox 0x000E9080
void CSWGuiInGameInventory::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_SCROLL_DOWN:
            m_lbxDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case GUIEVENT_SCROLL_UP:
            m_lbxDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case 43:
            g_pAppManager->m_pClientExoApp
                ->ChangeCharacterToNextLivingPartyMember(0, 0);
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            m_bUpdateItems = 1;
            break;
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui() != 0)
            {
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
            }
            break;
        case GUIEVENT_TOGGLE_MODE:
            SetNextFilter();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            m_bUpdateItems = 1;
            break;
        case GUIEVENT_DOWN:
        case GUIEVENT_DOWN_ALTERNATE:
            if (m_pActiveControl != NULL)
            {
                SelectNextItem();
            }
            break;
        case GUIEVENT_UP:
        case GUIEVENT_UP_ALTERNATE:
            if (m_pActiveControl != NULL)
            {
                SelectPreviousItem();
            }
            break;
        }
    }
    CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// Xbox 0x000E7EA0
void CSWGuiInGameInventory::SetItemRows(int nTopItem, int nSelectedItem)
{
    int nRows = m_lItems.num > NUM_ITEM_ROWS ? NUM_ITEM_ROWS : m_lItems.num;
    m_nTopItem = nTopItem;
    m_nSelectedItem = nSelectedItem;
    int nRow;
    for (nRow = 0; nRow < nRows; nRow++)
    {
        m_aItemRows[nRow].SetItem(m_lItems[nTopItem + nRow].m_oidItem,
                                  m_lItems[nTopItem + nRow].m_bEquipped,
                                  m_lItems[nTopItem + nRow].m_bNewItem);
        m_aItemRows[nRow].AddEvent(GUIEVENT_ACTIVATE, this,
                                   m_lItems[nTopItem + nRow].m_pfnSelect);
        m_aItemRows[nRow].m_nIndex = m_lItems[nTopItem + nRow].m_bUsable;
        m_aItemRows[nRow].SetShown(1);
    }
    for (nRow = nRows; nRow < NUM_ITEM_ROWS; nRow++)
    {
        m_aItemRows[nRow].SetShown(0);
    }
    m_lblUparrow.SetShown(m_nTopItem != 0);
    m_lblDownarrow.SetShown(m_nTopItem + nRows < m_lItems.num);
    SetActiveControl(NULL, 0);
    if (m_lItems.num != 0)
    {
        SetActiveControl(&m_aItemRows[nSelectedItem - nTopItem], 1);
    }
}
// Xbox 0x000E8020
void CSWGuiInGameInventory::SelectNextItem()
{
    int nSelected = m_nSelectedItem + 1;
    int nTop = m_nTopItem;
    if (nSelected >= m_lItems.num)
    {
        nSelected = 0;
        nTop = 0;
        g_pExoInput->ClearEventRepeat(m_pManager->m_nLastInputEvent);
    }
    else if (nSelected - nTop > NUM_ITEM_ROWS - 1)
    {
        nTop = nSelected - (NUM_ITEM_ROWS - 1);
    }
    SetItemRows(nTop, nSelected);
}
// Xbox 0x000E8080
void CSWGuiInGameInventory::SelectPreviousItem()
{
    int nSelected = m_nSelectedItem - 1;
    int nTop = m_nTopItem;
    if (nSelected < 0)
    {
        nSelected = m_lItems.num - 1;
        nTop = nSelected - (NUM_ITEM_ROWS - 1) < 0 ? 0 : nSelected - (NUM_ITEM_ROWS - 1);
        g_pExoInput->ClearEventRepeat(m_pManager->m_nLastInputEvent);
    }
    else if (nSelected < nTop)
    {
        nTop = nSelected;
    }
    SetItemRows(nTop, nSelected);
}
// Xbox 0x000E89A0
void CSWGuiInGameInventory::CreateItemEntry(CSWCCreature* pCreature, CSWSItem* pItem,
                                            ItemListEntry* pEntry, int bSquadItem,
                                            int bEquipped)
{
    pEntry->m_oidItem = pItem->m_nId;
    pEntry->m_bNewItem = pItem->m_bNewItem;
    pEntry->m_bEquipped = bEquipped;
    if (pCreature->IsLeaderUsableItem(pItem, bSquadItem) != 0)
    {
        int nItemType = pItem->GetBaseItem()->m_nItemType;
        if ((nItemType == CSWBaseItem::ITEM_TYPE_REPAIR_KIT ||
             nItemType == CSWBaseItem::ITEM_TYPE_MEDPAC) &&
            pCreature->m_pStats->GetCurrentHitPoints() >=
                pCreature->m_pStats->GetMaxHitPoints())
        {
            CSWSCreature* pServerCreature = pCreature->GetServerCreature();
            if (pServerCreature == 0 || pServerCreature->m_nPoisonState == 0)
            {
                pEntry->m_pfnSelect = (CSWGuiHandler)&CSWGuiInGameInventory::FullHealth;
                pEntry->m_bUsable = 0;
            }
        }
        else
        {
            pEntry->m_pfnSelect = (CSWGuiHandler)&CSWGuiInGameInventory::OnControlSelected;
            pEntry->m_bUsable = 1;
        }
    }
    else if (pItem->GetBaseItem()->GetEquipableSlots() != 0)
    {
        pEntry->m_pfnSelect = (CSWGuiHandler)&CSWGuiInGameInventory::CantEquip;
        pEntry->m_bUsable = 0;
    }
    else
    {
        pEntry->m_pfnSelect = (CSWGuiHandler)&CSWGuiInGameInventory::NotUseable;
        pEntry->m_bUsable = 0;
    }
}
// PC 0x006B2A80  Xbox 0x000E8540
void CSWGuiInGameInventory::SetNextFilter()
{
    int nFilter =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nInventoryFilter + 1;
    if (nFilter >= 6)
    {
        nFilter = 0;
    }
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nInventoryFilter =
        nFilter;

    int nNextFilter = nFilter + 1;
    if (nNextFilter >= 6)
    {
        nNextFilter = 0;
    }

    CExoString sText = g_pTlkTable->GetSimpleString(0xA577) + " " +
                       g_pTlkTable->GetSimpleString(m_nFilterName[nNextFilter]);
    m_lblFilter.m_cText.m_cParams.SetText(sText);

    sText = g_pTlkTable->GetSimpleString(0x7DAB) + " - " +
            g_pTlkTable->GetSimpleString(m_nFilterName[nFilter]);
    m_lblInv.m_cText.m_cParams.SetText(sText);
}
// PC 0x006B28C0  Xbox 0x000E83D0
void CSWGuiInGameInventory::UpdatePlayerStats()
{
    CSWCCreature* pCreature =
        g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);

    if (pCreature != 0)
    {
        CResRef* pPortrait = &pCreature->GetPortrait(1);
        m_lblPort.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
        {
            CExoString sDefense(pCreature->GetStats()->GetArmorClass(0));
            m_lblDef.m_cText.m_cParams.SetText(sDefense);
        }

        CExoString sVitality;
        short nCurrentHitPoints = pCreature->GetStats()->m_nCurrentHitPoints;
        sVitality.Format("%d/%d", nCurrentHitPoints,
                         pCreature->GetStats()->GetMaxHitPoints());
        m_lblVit.m_cText.m_cParams.SetText(sVitality);
        if (m_lblVit.m_cText.GetIdealHeight() > m_lblVit.m_cText.GetFontHeight())
        {
            short nCurrentHitPoints = pCreature->GetStats()->m_nCurrentHitPoints;
            sVitality.Format("%d/ %d", nCurrentHitPoints,
                             pCreature->GetStats()->GetMaxHitPoints());
            m_lblVit.m_cText.m_cParams.SetText(sVitality);
        }
    }
}
// PC 0x006B2C90  Xbox 0x000E8750
int CSWGuiInGameInventory::CheckFilter(CSWSItem* pItem, CSWSCreature* pCreature,
                                       int nSlot)
{
    int bResult = 0;
    if (pItem == 0)
    {
        return 0;
    }
    switch (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nInventoryFilter)
    {
    case 0:
        bResult = 1;
        break;
    case 1:
        bResult = pItem->m_bNewItem;
        break;
    case 2:
        bResult = pItem->m_bPlotObject;
        break;
    case 3:
        if (pItem->GetBaseItem() != 0)
        {
            bResult = pItem->GetBaseItem()->m_nEquipableSlots != 0;
        }
        break;
    case 4:
        if (pItem->GetBaseItem() != 0)
        {
            if (pItem->m_bPlotObject == 0 &&
                pItem->GetBaseItem()->m_nEquipableSlots == 0)
            {
                bResult = 1;
            }
            else
            {
                bResult = 0;
            }
        }
        break;
    case 5:
        if (pCreature->GetClientCreature() != 0)
        {
            if (pCreature->GetClientCreature()->IsLeaderUsableItem(pItem,
                                                                   nSlot) != 0 &&
                pCreature->CanUseItem(pItem, 0) != 0)
            {
                bResult = 1;
            }
            else
            {
                bResult = 0;
            }
        }
        break;
    }
    return bResult;
}
// PC 0x0041A290  Xbox 0x0004CEC0
inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
// PC 0x006B3D10  Xbox 0x000E8E80
void CSWGuiInGameInventory::OnControlEntered(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        int bUseItem = 0;
        unsigned long oidItem = static_cast<CSWGuiInGameItemEntry*>(pControl)->m_oidItem;
        if (oidItem != OBJECT_INVALID)
        {
            CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
            CExoString sDescription = pItem->GetPropertyDescription();
            m_lblUnknown64.m_cText.m_cParams.SetText(sDescription);
            bUseItem = pControl->m_nIndex;
            if (pItem->m_bNewItem)
                m_aNewItems.AddUnique(oidItem);
        }
        else
        {
            CExoString sDescription("Error: Invalid item");
            m_lblUnknown64.m_cText.m_cParams.SetText(sDescription);
        }
        m_lbxDescription.ClearItems();
        CSWGuiExtent cExtent = m_lblUnknown64.m_cExtent;
        if (m_lblUnknown64.m_cText.m_cParams.m_sText.GetLength() > 0)
            cExtent.m_nHeight = m_lblUnknown64.m_cText.GetIdealHeight();
        else
            cExtent.m_nHeight = m_lblUnknown64.m_cText.GetFontHeight();
        m_lblUnknown64.SetExtent(cExtent);
        if (bUseItem)
        {
            if (g_pAppManager->m_pServerExoApp->GetPlayerCreature()->m_nUseItemTimer == 0)
                m_lblUseItem.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
            else
                m_lblUseItem.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        }
        else
        {
            m_lblUseItem.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        }
        CSWGuiControl* pDescription = &m_lblUnknown64;
        m_lbxDescription.AddControls(&pDescription, 1, 0, 0, 0);
        m_lbxDescription.SetSelectedControl(&m_lblUnknown64, 0);
        m_lblRanalog.SetShown(m_lbxDescription.IsOversized());
    }
}
// PC 0x006B25E0  Xbox 0x000E80E0
void CSWGuiInGameInventory::OnControlSelected(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        unsigned long oidItem =
            static_cast<CSWGuiInGameItemEntry*>(pControl)->m_oidItem;
        if (oidItem != OBJECT_INVALID)
        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetPlayerCreature();
            if (pCreature->m_bCombatState != 0 &&
                pCreature->GetItemUseState() == 1 &&
                pCreature->m_nUseItemTimer != 0)
            {
                CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                CGuiInGame* pGui = pClient->GetInGameGui();
                CSWGuiMessageBox* pMessageBox = pGui->m_pMessageBox;
                pMessageBox->SetAllowCancel(0);
                pMessageBox->SetMessage(STRREF_ITEM_USED_THIS_ROUND);
                pMessageBox->SetCallback(0, 0, 0);
                m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
            else
            {
                pCreature->m_bSafeProjectileBroadcast = 0;
                pCreature->InstantUseItem(oidItem);
                CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
                if (pItem != NULL && !pItem->m_bDeleting)
                {
                    static_cast<CSWGuiInGameItemEntry*>(pControl)->RefreshItem();
                }
                else
                {
                    m_lItems.DelIndex(m_nSelectedItem);
                    int nSelected = m_nSelectedItem;
                    int nTop = m_nTopItem;
                    if (m_nSelectedItem == m_lItems.num)
                    {
                        nSelected = m_nSelectedItem - 1 < 0 ? 0 : m_nSelectedItem - 1;
                        nTop = nSelected - (NUM_ITEM_ROWS - 1) < 0 ? 0 : nSelected - (NUM_ITEM_ROWS - 1);
                    }
                    SetItemRows(nTop, nSelected);
                    if (m_lItems.num == 0)
                    {
                        m_lblUseItem.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
                        SetActiveControl(NULL, 0);
                        m_lblUnknown64.m_cText.m_cParams.SetText("");
                    }
                }
            }
        }
    }
}
// PC 0x006B4810  Xbox 0x000E8A90
void CSWGuiInGameInventory::PopulateItemListBox()
{
    CSWCCreature* pCreature = g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
    CSWSCreature* pServerCreature = pCreature->GetServerCreature();
    CItemRepository* pRepository = pServerCreature->GetItemRepository(1);
    m_lblCreditsValue.m_cText.m_cParams.SetText(
        CExoString(pServerCreature->GetGold()));

    CExoString sName;
    CExoArrayList<CExoString> lNames;
    ItemListEntry cEntry;
    m_lItems.num = 0;
    for (unsigned long nSlot = 0; nSlot < 14; ++nSlot)
    {
        unsigned long nInventorySlot = 1 << nSlot;
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        CSWCItem* pClientItem = pClient->GetItemByGameObjectID(
            pCreature->GetEquippedItemID(nInventorySlot));
        if (pClientItem != NULL)
        {
            CSWSItem* pItem = pClientItem->GetServerItem();
            int bSquadItem = nInventorySlot == INVENTORY_SLOT_RIGHTRING ||
                nInventorySlot == INVENTORY_SLOT_NECK ||
                nInventorySlot == INVENTORY_SLOT_ARMS;
            if (pItem != NULL && !pItem->m_bDeleting &&
                CheckFilter(pItem, pServerCreature, bSquadItem) != 0)
            {
                CreateItemEntry(pCreature, pItem, &cEntry, bSquadItem, 1);
                pItem->m_sLocalizedName.GetString(
                    g_pAppManager->GetClientExoApp()->GetClientLanguage(), &sName, 0);
                int nIndex;
                for (nIndex = 0; nIndex < m_lItems.num; nIndex++)
                {
                    if (sName < lNames[nIndex])
                    {
                        m_lItems.Insert(cEntry, nIndex);
                        lNames.Insert(sName, nIndex);
                        break;
                    }
                }
                if (nIndex == m_lItems.num)
                {
                    m_lItems.Add(cEntry);
                    lNames.Add(sName);
                }
            }
        }
    }
    for (unsigned long i = 0; i < pRepository->GetNumItems(); ++i)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != NULL && !pItem->m_bDeleting &&
            CheckFilter(pItem, pServerCreature, 0) != 0)
        {
            CreateItemEntry(pCreature, pItem, &cEntry, 0, 0);
            m_lItems.Add(cEntry);
        }
    }

    SetItemRows(0, 0);
    if (m_lItems.num == 0)
    {
        m_lblUseItem.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        SetActiveControl(NULL, 0);
        CExoString sEmpty("");
        m_lblUnknown64.m_cText.m_cParams.SetText(sEmpty);
    }
    m_bUpdateItems = 0;
}
#endif
