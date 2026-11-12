#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "math3d.h"
#include "swparty.h"
#include "nwccreature.h"
#include "nwsobject.h"
#include "nwsitem.h"
#include "serverexoapp.h"
#include "gameobjectarray.h"
#include "nwmessage.h"
#include "nwrace.h"
#include "guiingame.h"
#include "swguiingameequip.h"
#include "nwcitem.h"
#include "nwscombatround.h"
#include "nwitem.h"
#include "clientoptions.h"
#include "gameobjectarray.h"
#include "partytables.h"
#include "serverinfo.h"
#include "swguiingamecharacter.h"
#include "nwitem.h"

#include "nwcarea.h"
#include "nwsarea.h"
#include "nwsarea.h"
#include "nwsplaceable.h"

static inline void SwguiingameequipTrace2();
static inline void SwguiingameequipTrace1();
#define SWGUIINGAMEEQUIP_TRACE() SwguiingameequipTrace1()

static inline CServerExoApp* GetServerExoApp();

const CSWGuiInGameEquip::SlotInfo CSWGuiInGameEquip::SLOT_INFO[NUM_EQUIPMENT_SLOTS] =
{
    {INVENTORY_SLOT_LEFTHAND, "INV_WEAP_L", "weap_l", 31378, 31378},
    {INVENTORY_SLOT_RIGHTHAND, "INV_WEAP_R", "weap_r", 31379, 31379},
    {INVENTORY_SLOT_HEAD, "INV_HEAD", "head", 31375, 41810},
    {INVENTORY_SLOT_RIGHTRING, "INV_ARM_L", "forearm_l", 31376, 41814},
    {INVENTORY_SLOT_NECK, "INV_ARM_R", "forearm_r", 31377, 41814},
    {INVENTORY_SLOT_ARMOUR, "INV_BODY", "armor", 31380, 41813},
    {INVENTORY_SLOT_CLOAK, "INV_HANDS", "hands", 31383, 41811},
    {INVENTORY_SLOT_BELT, "INV_IMPLANT", "implant", 31388, 41811},
    {INVENTORY_SLOT_ARMS, "INV_BELT", "belt", 31382, 41812},
    {0x80000, "INV_WEAP_L2", "weap_l", 31378, 31378},
    {0x40000, "INV_WEAP_R2", "weap_r", 31379, 31379}
};

#ifndef _XBOX
int CSWGuiInGameEquip::GetSlotItemIdIndex(int nSlot)  // PC 0x00713C80
{
    switch (nSlot)
    {
    case INVENTORY_SLOT_LEFTHAND: return 0;
    case INVENTORY_SLOT_RIGHTHAND: return 1;
    case INVENTORY_SLOT_HEAD: return 2;
    case INVENTORY_SLOT_RIGHTRING: return 3;
    case INVENTORY_SLOT_NECK: return 4;
    case INVENTORY_SLOT_ARMOUR: return 5;
    case INVENTORY_SLOT_HANDS: return 6;
    case INVENTORY_SLOT_BELT: return 7;
    case INVENTORY_SLOT_ARMS: return 8;
    default: return INVALID_SLOT_ITEM_INDEX;
    }
}

// PC 0x00714560
CSWSItem* CSWGuiInGameEquip::CopyItem(unsigned long oidItem)
{
    CSWSItem* pCopy = 0;
    CGameObject* pObject = 0;
    CServerExoApp* pServer = GetServerExoApp();
    unsigned long oidServer =
        pServer->ClientToServerObjectId(oidItem);

    pServer = GetServerExoApp();
    if (pServer->GetObjectArray()->GetGameObject(
            oidServer, &pObject) == CGameObjectArray::SUCCESS &&
        pObject->AsSWSItem() != 0)
    {
        pCopy = new CSWSItem(OBJECT_INVALID);
        pCopy->CopyItem(pObject->AsSWSItem());
    }
    return pCopy;
}
// PC 0x00714620
int CSWGuiInGameEquip::ItemExists(unsigned long oidItem)
{
    CServerExoApp* pServer = GetServerExoApp();
    unsigned long oidServer = pServer->ClientToServerObjectId(oidItem);
    CGameObject* pObject;

    pServer = GetServerExoApp();
    if (pServer->GetObjectArray()->GetGameObject(oidServer, &pObject) ==
        CGameObjectArray::SUCCESS)
    {
        return pObject->AsSWSItem() != 0;
    }

    return 0;
}
#endif
void CSWGuiInGameItemEntry::Draw(float fElapsed)  // PC 0x00714810
{
    if (m_bActive)
    {
        CSWGuiButton::m_cHilite.Draw(fElapsed);
        m_cIconHilite.Draw(fElapsed);
    }
    else
    {
        CSWGuiButton::m_cBorder.Draw(fElapsed);
        m_cIconBorder.Draw(fElapsed);
    }
    m_cCountBorder.Draw(fElapsed);
    m_cText.Draw(fElapsed);
    m_cCountText.Draw(fElapsed);
}
// PC 0x00714810
void CSWGuiStoreItemEntry::Draw(float fElapsed)
{
    if (m_bActive)
    {
        CSWGuiButton::m_cHilite.Draw(fElapsed);
        m_cIconHilite.Draw(fElapsed);
    }
    else
    {
        CSWGuiButton::m_cBorder.Draw(fElapsed);
        m_cIconBorder.Draw(fElapsed);
    }
    m_cCountBorder.Draw(fElapsed);
    m_cText.Draw(fElapsed);
    m_cCountText.Draw(fElapsed);
}
// PC 0x00714880
void CSWGuiInGameItemEntry::Initialize(CSWGuiText& cText, CSWGuiBorder& cBorder,
                                       CSWGuiBorder& cHilite, int nWidth,
                                       int bPulsing)
{
    m_cText.m_cParams = cText.m_cParams;
    CSWGuiBorderParams* pParams = &CSWGuiButton::m_cBorder.m_cParams;
    *pParams = cBorder.m_cParams;
    pParams->m_vColor = CGuiInGame::COLOR_BLUE;
    pParams = &CSWGuiButton::m_cHilite.m_cParams;
    *pParams = cHilite.m_cParams;
    pParams->m_vColor = CGuiInGame::COLOR_YELLOW;
    pParams->SetPulsingAlpha(bPulsing, 1, 0);
    pParams = &m_cIconBorder.m_cParams;
    {
#ifndef _XBOX
        CResRef cFill("lbl_hex_3"); CResRef cEdge(""); CResRef cCorner("");
        *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#else
        *pParams = CSWGuiBorderParams("", "", "lbl_hex_3", 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#endif
        pParams->m_vColor = CGuiInGame::COLOR_BLUE;
    }
    pParams = &m_cIconHilite.m_cParams;
    {
#ifndef _XBOX
        CResRef cFill("lbl_hex_3"); CResRef cEdge(""); CResRef cCorner("");
        *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#else
        *pParams = CSWGuiBorderParams("", "", "lbl_hex_3", 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#endif
        pParams->m_vColor = CGuiInGame::COLOR_YELLOW;
        pParams->SetPulsingAlpha(bPulsing, 0, 1);
    }

    {
        CSWGuiExtent cExtent;
        cExtent.m_nLeft = 0;
        cExtent.m_nTop = 0;
        cExtent.m_nWidth = nWidth;
        cExtent.m_nHeight = 56;
        SetExtent(cExtent);
    }
    m_bPulsing = bPulsing;
}
// PC 0x007184A0
CSWGuiInGameItemEntry::CSWGuiInGameItemEntry()
{
    SwguiingameequipTrace2();
    m_bPulsing = 0;
    m_bEquipped = 0;
    m_bUnavailable = 0;
    m_oidItem = OBJECT_INVALID;
    m_nCanUse = 0;
}
// PC 0x00714AB0
void CSWGuiInGameItemEntry::SetItem(unsigned long oidItem, int bEquipped,
                                   int bUnavailable)
{

    CResRef cIcon;
    m_oidItem = oidItem;
    m_bEquipped = bEquipped;
    m_bUnavailable = bUnavailable;
    if (oidItem != OBJECT_INVALID)
    {
        CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->ClientToServerObjectId(oidItem));
        if (pItem != NULL)
        {
            CExoString sName;
            pItem->m_sLocalizedName.GetString(pClient->GetClientLanguage(), &sName, 0);
            if (bEquipped)
                sName = sName + " (" +
                    g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_EQUIPPED) + ")";
            if (bUnavailable)
            {
                m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_PURPLE;
                m_cIconBorder.m_cParams.m_vColor = CGuiInGame::COLOR_PURPLE;
                m_cBorder.m_cParams.m_fHilightAlpha = 0.5f;
                m_cIconBorder.m_cParams.m_fHilightAlpha = 0.5f;
                m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_PURPLE;
                m_cIconHilite.m_cParams.m_vColor = CGuiInGame::COLOR_PURPLE;
            }
            else
            {
                m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
                m_cIconBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
                m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
                m_cIconHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
            }
            m_cText.m_cParams.SetText(sName);
            CExoString sCount;
            int nCount = pItem->m_nStackSize;
            if (nCount > 1)
            {
                if (nCount >= 100)
                {
                    m_cIconBorder.m_cParams.SetFillImage("lbl_hex_7", 0);
                    m_cIconHilite.m_cParams.SetFillImage("lbl_hex_7", 0);
                    sCount.Format("%d", nCount);
                }
                else
                {
                    m_cIconBorder.m_cParams.SetFillImage("lbl_hex_6", 0);
                    m_cIconHilite.m_cParams.SetFillImage("lbl_hex_6", 0);
                    sCount.Format("%d", nCount);
                }
            }
            else
            {
                m_cIconBorder.m_cParams.SetFillImage("lbl_hex_3", 0);
                m_cIconHilite.m_cParams.SetFillImage("lbl_hex_3", 0);
            }
            m_cCountText.m_cParams = CSWGuiTextParams(
                sCount, "fnt_d16x16", CGuiInGame::COLOR_BLUE,
                CSWGuiTextParams::ALIGNMENT_ITEM_COUNT, 1.0f);
            cIcon = pItem->GetIcon();
        }
        else
        {
            m_cIconBorder.m_cParams.SetFillImage("lbl_hex_3", 0);
            m_cIconHilite.m_cParams.SetFillImage("lbl_hex_3", 0);
            m_cText.m_cParams.SetText(g_pAppManager->GetClientExoApp()->GetGUIString(
                CClientExoApp::GUI_STRREF_NONE));
            m_cCountText.m_cParams = CSWGuiTextParams(
                CExoString(), "fnt_d16x16", CGuiInGame::COLOR_BLUE,
                CSWGuiTextParams::ALIGNMENT_ITEM_COUNT, 1.0f);
            cIcon = "inone";
        }
    }
    else
    {
        m_cIconBorder.m_cParams.SetFillImage("lbl_hex_3", 0);
        m_cIconHilite.m_cParams.SetFillImage("lbl_hex_3", 0);
        m_cText.m_cParams.SetText(g_pAppManager->GetClientExoApp()->GetGUIString(
            CClientExoApp::GUI_STRREF_NONE));
        m_cCountText.m_cParams = CSWGuiTextParams(
            CExoString(), "fnt_d16x16", CGuiInGame::COLOR_BLUE,
            CSWGuiTextParams::ALIGNMENT_ITEM_COUNT, 1.0f);
        cIcon = "inone";
    }
    CSWGuiBorderParams* pParams = &m_cCountBorder.m_cParams;
    pParams->m_nFillMode = CSWGuiBorderParams::FILL_MODE_BACKGROUND;
    pParams->SetFillImage(cIcon, 0);
}
// PC 0x007163B0
CSWGuiContainer::CSWGuiContainer(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("container");
    InitControl(m_lblMessage, "LBL_MESSAGE", 1);
    InitControl(m_lblButton, "LBL_BUTTON", 1);
    InitControl(m_lblOk, "LBL_OK", 1);
    InitControl(m_lbItems, "LB_ITEMS", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblX, "LBL_X", 1);
#ifdef _XBOX
    InitControl(m_lblGiveitems, "LBL_GIVEITEMS", 1);
    InitControl(m_lblCancel, "LBL_CANCEL", 1);
    StopLoadFromLayout();

    m_nGiveModeFlags |= GIVE_MODE_ACTIVE;
    m_lbItems.SetScrollOnly(1);
    m_lbItems.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
#else
    InitControl(m_lblCancel, "LBL_CANCEL", 1);
    InitControl(m_btnOk, "BTN_OK", 1);
    InitControl(m_btnGiveitems, "BTN_GIVEITEMS", 1);
    InitControl(m_btnCancel, "BTN_CANCEL", 1);
    StopLoadFromLayout();

    m_nGiveModeFlags = (m_nGiveModeFlags & ~(GIVE_MODE_SETUP | GIVE_MODE_CLEAR)) |
                       GIVE_MODE_ACTIVE;
    m_lbItems.SetScrollOnly(1);
    m_lbItems.SetEnabled(0);
    m_lbItems.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;

    m_btnOk.SetFocusable(0);
    m_btnOk.AddEvent(GUIEVENT_ACTIVATE, this,
                    (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
    m_btnGiveitems.SetFocusable(0);
    m_btnGiveitems.AddEvent(GUIEVENT_ACTIVATE, this,
                           (CSWGuiHandler)&CSWGuiPanel::OnXButtonPressed);
    m_btnCancel.SetFocusable(0);
    m_btnCancel.AddEvent(GUIEVENT_ACTIVATE, this,
                        (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
#endif
    CenterPanel();
}
// PC 0x007153D0
void CSWGuiContainer::GiveItem(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent != 0)
        {
            m_pManager->PlayGuiSound((char)pControl->m_nClickSound);
        }

#ifdef _XBOX

        g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter()->PerformGiveItem(
            static_cast<CSWGuiInGameItemEntry*>(pControl)->m_oidItem, m_oidContainer, 1);
        g_pAppManager->m_pClientExoApp->GetSWCMessage()->
            SendPlayerToServerGuiContainer_Close(m_oidContainer, 0);
        g_pAppManager->m_pClientExoApp->GetInGameGui()->HideContainerGui();
#else
        int nItems = 1;
        if (g_bShiftActionState != 0)
        {
            nItems =
                static_cast<CSWGuiInGameItemEntry*>(pControl)->GetNumItems();
        }

        unsigned long oidContainer = m_oidContainer;
        unsigned long oidItem =
            static_cast<CSWGuiInGameItemEntry*>(pControl)->m_oidItem;
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        pClient->GetSWParty()->GetPlayerCharacter()->PerformGiveItem(
            oidItem, oidContainer, nItems);
        m_nGiveModeFlags |= GIVE_MODE_SETUP | GIVE_MODE_CLEAR;
#endif
    }
}
// PC 0x007151E0
int CSWGuiInGameItemEntry::GetNumItems()
{
    int nItems = 1;
    CExoString sNumItems(m_cText.m_cParams.m_sText);

    if (!sNumItems.IsEmpty())
    {
        nItems = sNumItems.AsINT();
    }

    return nItems;
}
// PC 0x00715260
void CSWGuiInGameItemEntry::SetCanUse(unsigned char nCanUse)
{
    switch (nCanUse)
    {
    case CAN_USE:
    case CAN_USE_EQUIPPED:
        m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
        m_cIconBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
        m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        m_cIconHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        break;
    case CANNOT_USE:
    case CANNOT_USE_WITH_OTHER_HAND:
        m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_RED;
        m_cIconBorder.m_cParams.m_vColor = CGuiInGame::COLOR_RED;
        m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_RED;
        m_cIconHilite.m_cParams.m_vColor = CGuiInGame::COLOR_RED;
        break;
    }

    m_cHilite.m_cParams.SetPulsingAlpha(m_bPulsing, 1, 0);
    m_cIconHilite.m_cParams.SetPulsingAlpha(m_bPulsing, 0, 1);
    m_nCanUse = nCanUse;
}
// PC 0x00715440
void CSWGuiContainer::OnPanelAdded()
{
#ifndef _XBOX
    CSWGuiExtent cPanel = m_cExtent;
    CSWGuiExtent cButton = m_btnOk.m_cExtent;

    g_pGuiMan->StoreCurrentMousePosition();
    g_pGuiMan->MoveMouseToPosition(
        cButton.m_nLeft + cPanel.m_nLeft + cButton.m_nWidth / 2,
        cPanel.m_nTop + cButton.m_nTop + cButton.m_nHeight / 2);
#endif
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x007154C0
void CSWGuiContainer::OnPanelRemoved()
{
#ifndef _XBOX
    g_pGuiMan->RestoreOldMousePosition();
#endif
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x007154E0
void CSWGuiStoreItemEntry::SetExtent(const CSWGuiExtent& cExtent)
{
    if (cExtent.m_nWidth > SWGUI_ITEM_ICON_SIZE)
    {
        m_cExtent = cExtent;
        CSWGuiExtent cItemExtent(m_cExtent.m_nLeft, m_cExtent.m_nTop,
                                SWGUI_ITEM_ICON_SIZE, SWGUI_ITEM_ICON_SIZE);
        m_cIconBorder.SetExtent(cItemExtent);
        m_cIconHilite.SetExtent(cItemExtent);
        m_cCountBorder.SetExtent(cItemExtent);

        CSWGuiExtent cQuantityExtent(cItemExtent);
        cQuantityExtent.m_nWidth =
            m_cCountText.m_cParams.m_sText.GetLength() > SWGUI_ITEM_COUNT_SHORT_DIGITS ? SWGUI_ITEM_COUNT_LONG_WIDTH : SWGUI_ITEM_COUNT_SHORT_WIDTH;
        cQuantityExtent.m_nHeight = SWGUI_ITEM_COUNT_HEIGHT;
        cQuantityExtent.m_nLeft += SWGUI_ITEM_ICON_SIZE - cQuantityExtent.m_nWidth;
        cQuantityExtent.m_nTop += SWGUI_ITEM_ICON_SIZE - SWGUI_ITEM_COUNT_HEIGHT;
        m_cCountText.SetExtent(cQuantityExtent);

        cItemExtent.m_nLeft += SWGUI_ITEM_ICON_SIZE;
        cItemExtent.m_nWidth = m_cExtent.m_nWidth - SWGUI_ITEM_ICON_SIZE;
        m_cBorder.SetExtent(cItemExtent);
        m_cHilite.SetExtent(cItemExtent);
        m_cText.SetExtent(CSWGuiExtent::Intersection(
            m_cBorder.GetInnerExtent(), m_cHilite.GetInnerExtent()));
    }
}
// PC 0x007154E0
void CSWGuiInGameItemEntry::SetExtent(const CSWGuiExtent& cExtent)
{
    if (cExtent.m_nWidth > SWGUI_ITEM_ICON_SIZE)
    {
        m_cExtent = cExtent;
        CSWGuiExtent cItemExtent(m_cExtent.m_nLeft, m_cExtent.m_nTop,
                                SWGUI_ITEM_ICON_SIZE, SWGUI_ITEM_ICON_SIZE);
        m_cIconBorder.SetExtent(cItemExtent);
        m_cIconHilite.SetExtent(cItemExtent);
        m_cCountBorder.SetExtent(cItemExtent);

        CSWGuiExtent cQuantityExtent(cItemExtent);
        cQuantityExtent.m_nWidth =
            m_cCountText.m_cParams.m_sText.GetLength() > SWGUI_ITEM_COUNT_SHORT_DIGITS ? SWGUI_ITEM_COUNT_LONG_WIDTH : SWGUI_ITEM_COUNT_SHORT_WIDTH;
        cQuantityExtent.m_nHeight = SWGUI_ITEM_COUNT_HEIGHT;
        cQuantityExtent.m_nLeft += SWGUI_ITEM_ICON_SIZE - cQuantityExtent.m_nWidth;
        cQuantityExtent.m_nTop += SWGUI_ITEM_ICON_SIZE - SWGUI_ITEM_COUNT_HEIGHT;
        m_cCountText.SetExtent(cQuantityExtent);

        cItemExtent.m_nLeft += SWGUI_ITEM_ICON_SIZE;
        cItemExtent.m_nWidth = m_cExtent.m_nWidth - SWGUI_ITEM_ICON_SIZE;
        m_cBorder.SetExtent(cItemExtent);
        m_cHilite.SetExtent(cItemExtent);
        m_cText.SetExtent(CSWGuiExtent::Intersection(
            m_cBorder.GetInnerExtent(), m_cHilite.GetInnerExtent()));
    }
}
// PC 0x00715660
void CSWGuiStoreItemEntry::Initialize(CSWGuiText& cText, CSWGuiBorder& cBorder,
                                    CSWGuiBorder& cHilite, int nWidth)
{
    m_cText.m_cParams = cText.m_cParams;
    CSWGuiBorderParams* pParams = &CSWGuiButton::m_cBorder.m_cParams;
    *pParams = cBorder.m_cParams;
    pParams->m_vColor = CGuiInGame::COLOR_BLUE;
    pParams = &CSWGuiButton::m_cHilite.m_cParams;
    *pParams = cHilite.m_cParams;
    pParams->m_vColor = CGuiInGame::COLOR_YELLOW;
    pParams->SetPulsingAlpha(1, 1, 0);
    pParams = &m_cIconBorder.m_cParams;
    {
#ifndef _XBOX
        CResRef cFill("lbl_hex_3"); CResRef cEdge(""); CResRef cCorner(""); *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#else
        *pParams = CSWGuiBorderParams("", "", "lbl_hex_3", 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#endif
        pParams->m_vColor = CGuiInGame::COLOR_BLUE;
    }
    pParams = &m_cIconHilite.m_cParams;
    {
#ifndef _XBOX
        CResRef cFill("lbl_hex_3"); CResRef cEdge(""); CResRef cCorner(""); *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#else
        *pParams = CSWGuiBorderParams("", "", "lbl_hex_3", 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#endif
        pParams->m_vColor = CGuiInGame::COLOR_YELLOW;
        pParams->SetPulsingAlpha(1, 0, 1);
    }
    {
        CSWGuiExtent cExtent;
        cExtent.m_nLeft = 0;
        cExtent.m_nTop = 0;
        cExtent.m_nWidth = nWidth;
        cExtent.m_nHeight = 56;
        SetExtent(cExtent);
    }
}
// PC 0x007186F0
void CSWGuiContainer::SetContainer(unsigned long oidContainer, int nType)
{
    m_oidContainer = oidContainer;
    m_nContainerType = nType;

    CExoArrayList<CSWGuiInGameItemEntry*> lItems;
    CSWGuiLabelHilight* pProto = (CSWGuiLabelHilight*)m_lbItems.m_pProtoItem;
    CSWGuiText* pText = &pProto->m_cText;
    CSWGuiBorder* pBorder = &pProto->m_cBorder;
    CSWGuiBorder* pHilite = &pProto->m_cHilite;
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    CItemRepository* pRepository = pServer->GetPlaceableByGameObjectID(
        pServer->ClientToServerObjectId(oidContainer))->m_pcItemRepository;
    lItems.SetSize(pRepository->GetNumItems());
    int nWidth = m_lbItems.GetItemWidth();

    for (unsigned long nItem = 0; nItem < pRepository->GetNumItems(); ++nItem)
    {
        unsigned long oidItem = pClient->ServerToClientObjectId(
            pRepository->ItemListGetItemObjectID(nItem));
        lItems[nItem] = new CSWGuiInGameItemEntry;
        lItems[nItem]->Initialize(*pText, *pBorder, *pHilite, nWidth, 0);
        lItems[nItem]->SetItem(oidItem, 0, 0);
    }
    m_lbItems.AddControls(*(CExoArrayList<CSWGuiControl*>*)&lItems, 1, 0, 0);

    unsigned long nStrRef = m_nContainerType ?
        STRREF_CONTAINER_INVENTORY : STRREF_CONTAINER_EMPTY;
    CExoString sGive = g_pAppManager->GetClientExoApp()->GetGUIString(
        STRREF_GIVE_BUTTON_PREFIX) + " " +
        g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_GIVE_ITEM);
#ifdef _XBOX
    m_lblMessage.m_cText.m_cParams.SetText(
        g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef));
    m_lblOk.m_cText.m_cParams.SetText(
        g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_GET_ITEMS));
    m_lblGiveitems.m_cText.m_cParams.SetText(sGive);
    m_nGiveModeFlags |= GIVE_MODE_ACTIVE;
    m_lbItems.SetScrollOnly(1);
#else
    m_lblMessage.m_cText.m_cParams.SetStrRef(nStrRef);
    m_lblOk.m_cText.m_cParams.SetStrRef(STRREF_GET_ITEMS);
    m_btnOk.m_cText.m_cParams.SetStrRef(STRREF_GET_ITEMS);
    m_btnGiveitems.m_cText.m_cParams.SetText(sGive);
    m_nGiveModeFlags |= GIVE_MODE_ACTIVE;
    m_lbItems.SetScrollOnly(1);
    m_lbItems.SetEnabled(0);
#endif
    SetActiveControl(0, 0);
    SetActiveControl(&m_lbItems, 0);
}

#ifdef _XBOX

#define STRREF_XBOX_GIVE_ITEM_LABEL 47885
void CSWGuiContainer::SetupGiveMode()  // PC 0x00718A50
#else
void CSWGuiContainer::SetupGiveMode(int bRestoreSelection)
#endif
{
    CExoArrayList<CSWGuiInGameItemEntry*> lItems;
    CSWGuiLabelHilight* pProto = (CSWGuiLabelHilight*)m_lbItems.m_pProtoItem;
    CSWGuiText* pText = &pProto->m_cText;
    CSWGuiBorder* pBorder = &pProto->m_cBorder;
    CSWGuiBorder* pHilite = &pProto->m_cHilite;
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    CItemRepository* pRepository =
        g_pAppManager->GetServerExoApp()->GetPartyTable()->GetPartyItems();
    int nWidth = m_lbItems.GetItemWidth();
    int nEntry = 0;

    for (unsigned long nItem = 0; nItem < pRepository->GetNumItems(); ++nItem)
    {
        unsigned long oidItem = pClient->ServerToClientObjectId(
            pRepository->ItemListGetItemObjectID(nItem));
        CSWSItem* pItem = pRepository->ItemListGetItem(nItem);
        if (pItem != NULL && pItem->m_bPlotObject == 0)
        {
            lItems.Add(new CSWGuiInGameItemEntry);
            lItems[nEntry]->Initialize(*pText, *pBorder, *pHilite, nWidth, 1);
            lItems[nEntry]->SetItem(oidItem, 0, 0);
            lItems[nEntry]->AddEvent(GUIEVENT_ACTIVATE, this,
                                    (CSWGuiHandler)&CSWGuiContainer::GiveItem);
            lItems[nEntry]->AddEvent(GUIEVENT_ACCEPT, this,
                                    (CSWGuiHandler)&CSWGuiContainer::GiveItem);
            ++nEntry;
        }
    }

    CExoString sGive = g_pAppManager->GetClientExoApp()->GetGUIString(
        STRREF_GIVE_BUTTON_PREFIX) + " " +
        g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_GET_ITEMS);
#ifdef _XBOX
    m_lblMessage.m_cText.m_cParams.SetText(
        g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_ITEMS_TO_GIVE));
    m_lblOk.m_cText.m_cParams.SetText(
        g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_XBOX_GIVE_ITEM_LABEL));
    m_lblGiveitems.m_cText.m_cParams.SetText(sGive);
    m_nGiveModeFlags &= ~GIVE_MODE_ACTIVE;
    m_lbItems.SetScrollOnly(0);
    m_lbItems.AddControls(*(CExoArrayList<CSWGuiControl*>*)&lItems, 1, 0, 0);
    SetActiveControl(0, 0);
    SetActiveControl(&m_lbItems, 0);
#else
    m_lblMessage.m_cText.m_cParams.SetStrRef(STRREF_ITEMS_TO_GIVE);
    m_lblOk.m_cText.m_cParams.SetStrRef(STRREF_GIVE_ITEM);
    m_btnOk.m_cText.m_cParams.SetStrRef(STRREF_GIVE_ITEM);
    m_btnGiveitems.m_cText.m_cParams.SetText(sGive);
    m_nGiveModeFlags &= ~GIVE_MODE_ACTIVE;
    m_lbItems.SetScrollOnly(0);
    m_lbItems.SetEnabled(1);

    int nSelected = 0;
    if (bRestoreSelection && lItems.num > 0)
    {
        nSelected = min(m_lbItems.m_nSelectedItem, lItems.num - 1);
    }
    m_lbItems.AddControls(*(CExoArrayList<CSWGuiControl*>*)&lItems, 1, 0, 0);
    SetActiveControl(0, 0);
    SetActiveControl(&m_lbItems, 0);
    if (lItems.num > 0)
    {
        m_lbItems.SetSelectedControl(nSelected, 0);
    }
#endif
}

#ifndef _XBOX
void CSWGuiContainer::Update(float fElapsed)  // PC 0x00718DB0
{
    int nResult = m_nGiveModeFlags;

    if ((nResult & GIVE_MODE_CLEAR) != 0)
    {
        nResult &= ~GIVE_MODE_CLEAR;
        m_nGiveModeFlags = nResult;
    }
    else if ((nResult & GIVE_MODE_SETUP) != 0 &&
             (nResult & GIVE_MODE_ACTIVE) == 0)
    {
        SetupGiveMode(1);
        m_nGiveModeFlags &= ~GIVE_MODE_SETUP;
    }
}
#endif
// PC 0x00716780
CSWGuiStoreItemEntry::CSWGuiStoreItemEntry()
{
    m_oidItem = OBJECT_INVALID;
    m_n3B0 = -1;
    m_n3B4 = 0;
}
#ifndef _XBOX
CSWGuiInGameEquip::CSWGuiInGameEquip(CSWGuiManager* pManager)  // PC 0x0071A1B0
    : CSWGuiPanel(pManager)
{
    m_pCreature = NULL;
    m_bDescriptionShown = 0;
    StartLoadFromLayout("equip");
    CExoString sButtonPrefix("BTN_");
    CExoString sLabelPrefix("LBL_");
    for (int nSlot = 0; nSlot < NUM_EQUIPMENT_SLOTS; ++nSlot)
    {
        InitControl(m_aSlotButtons[nSlot],
                    sButtonPrefix + SLOT_INFO[nSlot].sButton, 1);
        InitControl(m_aSlotLabels[nSlot],
                    sLabelPrefix + SLOT_INFO[nSlot].sButton, 1);
        m_aSlotButtons[nSlot].AddEvent(GUIEVENT_ACTIVATE, this,
            (CSWGuiHandler)&CSWGuiInGameEquip::OnSelectSlot);
        m_aSlotButtons[nSlot].AddEvent(GUIEVENT_MOUSE_ENTER, this,
            (CSWGuiHandler)&CSWGuiInGameEquip::OnEnterSlot);
        m_aSlotButtons[nSlot].m_nIndex = nSlot;
        m_aSlotButtons[nSlot].m_cHilite.m_cParams.SetPulsingAlpha(1, 0, 1);
        m_aoidEquippedItems[nSlot] = OBJECT_INVALID;
    }
    InitControl(m_lblAtkl, "LBL_ATKL", 1);
    InitControl(m_lblTohitl, "LBL_TOHITL", 1);
    InitControl(m_lblAtkr, "LBL_ATKR", 1);
    InitControl(m_lblTohitr, "LBL_TOHITR", 1);
    InitControl(m_lblDef, "LBL_DEF", 1);
    InitControl(m_lbItems, "LB_ITEMS", 1);
    InitControl(m_lbDesc, "LB_DESC", 1);
    InitControl(m_lblAttackInfo, "LBL_ATTACK_INFO", 1);
    InitControl(m_lblDefInfo, "LBL_DEF_INFO", 1);
    InitControl(m_lblTohit, "LBL_TOHIT", 1);
    InitControl(m_lblDamage, "LBL_DAMAGE", 1);
    InitControl(m_lblSlotname, "LBL_SLOTNAME", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_btnEquip, "BTN_EQUIP", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_lblCantequip, "LBL_CANTEQUIP", 1);
    StopLoadFromLayout();
    m_lblAtkl.m_cText.m_cParams.SetText("");
    m_lblAtkr.m_cText.m_cParams.SetText("");
    m_lblTohitl.m_cText.m_cParams.SetText("");
    m_lblTohitr.m_cText.m_cParams.SetText("");
    m_lblDef.m_cText.m_cParams.SetText("");
    m_cDescriptionText.Initialize(m_lbDesc.m_pProtoItem->m_cExtent,
                                  (CSWGuiLabel*)m_lbDesc.m_pProtoItem, 1.0f);
    ShowDescription(0);
    m_btnEquip.SetFocusable(0);
    m_btnEquip.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiInGameEquip::OnOKPressed);
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this,
                      (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnEquip.SetShown(0);
    m_lbItems.SetEnabled(0);
    m_lblCantequip.SetShown(0);
    CSWGuiPanel* pPanel = this;
    pPanel->SetActiveControl(&m_aSlotButtons[ARMOUR_ITEM_INDEX], 0);
    m_nPendingSlot = 0;
    m_oidPendingItem = OBJECT_INVALID;
    m_oidDescribed = 0;
    m_pDescribedCopy = NULL;
    m_oidCompared = 0;
    m_pComparedCopy = NULL;
}
// PC 0x00718E80
CSWGuiInGameEquip::~CSWGuiInGameEquip()
{
    m_lbDesc.ClearItems();
    m_lbItems.ClearItems();
    for (int i = 0; i < m_lItemEntries.num; i++)
    {
        if (m_lItemEntries[i] != 0)
        {
            delete m_lItemEntries[i];
        }
        m_lItemEntries[i] = 0;
    }
    m_lItemEntries.RemoveAll();
    if (m_pDescribedCopy != 0)
    {
        delete m_pDescribedCopy;
        m_pDescribedCopy = 0;
    }
    if (m_pComparedCopy != 0)
    {
        delete m_pComparedCopy;
        m_pComparedCopy = 0;
    }
}
// PC 0x0071B040
void CSWGuiInGameEquip::Draw(float fElapsed)
{
    UpdateInventory();
    return CSWGuiPanel::Draw(fElapsed);
}
// PC 0x00719850
void CSWGuiInGameEquip::OnPanelAdded()
{
    CSWParty* pParty;
    CSWCCreature* pCreature;

    m_nCharacter = -1;
    pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    pCreature = pParty->GetPlayerCharacter();
    SetCharacter(pCreature);
    OnEnterSlot(m_pActiveControl);
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
#endif
// PC 0x00715870
void CSWGuiStoreItemEntry::SetColors(Vector vBorderColor, Vector vTextColor,
                                     Vector vHiliteColor, Vector vUnused)
{
    m_cBorder.m_cParams.m_vColor = vBorderColor;
    m_cIconBorder.m_cParams.m_vColor = vBorderColor;
    m_cText.m_cParams.SetDefaultColor(vTextColor);
    m_cCountText.m_cParams.SetDefaultColor(vTextColor);
    m_cCountText.m_cParams.SetColor(vTextColor);
    m_cHilite.m_cParams.m_vColor = vHiliteColor;
    m_cIconHilite.m_cParams.m_vColor = vHiliteColor;
}
void CSWGuiStoreItemEntry::SetItem(unsigned long oidItem, int bUpdate, int nCharacter)  // PC 0x00715900
{
    CResRef cIcon;
    CExoString sName;
    CExoString sCount;

    CExoString sCost;
    CExoString sPrice;
    CServerExoApp* pServer = GetServerExoApp();
    CSWSItem* pItem = pServer->GetItemByGameObjectID(oidItem);
    m_nCharacter = nCharacter;
    m_oidItem = oidItem;
    if (oidItem != OBJECT_INVALID)
    {
        pItem = pServer->GetItemByGameObjectID(oidItem);
        if (pItem != NULL)
        {
            cIcon = pItem->GetIcon();
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
            pItem->m_sLocalizedName.GetString(pClient->GetClientLanguage(), &sName, 0);
            m_cText.m_cParams.SetText(sName);
            int nCount = pItem->m_nStackSize;
            if (nCount > 1)
            {
                if (nCount >= 100)
                {
                    m_cIconBorder.m_cParams.SetFillImage("lbl_hex_7", 0);
                    m_cIconHilite.m_cParams.SetFillImage("lbl_hex_7", 0);
                }
                else
                {
                    m_cIconBorder.m_cParams.SetFillImage("lbl_hex_6", 0);
                    m_cIconHilite.m_cParams.SetFillImage("lbl_hex_6", 0);
                }
                sCount.Format("%d", nCount);
            }
            else
            {
                m_cIconBorder.m_cParams.SetFillImage("lbl_hex_3", 0);
                m_cIconHilite.m_cParams.SetFillImage("lbl_hex_3", 0);
            }
            m_cCountText.m_cParams = CSWGuiTextParams(
                sCount, "fnt_d16x16", CGuiInGame::COLOR_BLUE,
                CSWGuiTextParams::ALIGNMENT_ITEM_COUNT, 1.0f);
        }
        else
        {
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
            m_cText.m_cParams.SetText(pClient->GetGUIString(
                CClientExoApp::GUI_STRREF_NONE));
            cIcon = "inone";
        }
    }
    else
    {
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        m_cText.m_cParams.SetText(pClient->GetGUIString(
            CClientExoApp::GUI_STRREF_NONE));
        cIcon = "inone";
    }
    CSWGuiBorderParams* pParams = &m_cCountBorder.m_cParams;
    pParams->m_nFillMode = CSWGuiBorderParams::FILL_MODE_BACKGROUND;
    pParams->SetFillImage(cIcon, 0);
}
#ifndef _XBOX
void CSWGuiInGameEquip::SetCharacter(CSWCCreature* pCreature)  // PC 0x00715DA0
{
    if (pCreature != m_pCreature)
    {
        m_pCreature = pCreature;
        if (pCreature)
        {
            for (int nSlot = 0; nSlot < NUM_EQUIPMENT_SLOTS; ++nSlot)
            {
                m_aSlotLabels[nSlot].m_cBorder.m_cParams.SetFillImage(
                    GetIcon(nSlot), 0);
            }
        }
    }
}
#endif
#include "nwccreaturestats.h"
// PC 0x00714000
CExoString CSWGuiInGameEquip::GetIcon(int nSlot)
{
    CExoString sIcon;
    unsigned short nRace = m_pCreature->GetStats()->m_nRace;
    sIcon.Format("i%s%s", nRace == CSWRace::CRACE_DROID ? "d" : "",
                 SLOT_INFO[nSlot].sLabel);
    return sIcon;
}
#ifndef _XBOX
void CSWGuiInGameEquip::SetDescription(const CExoString& sDescription)  // PC 0x007140B0
{
    m_cDescriptionText.m_cText.m_cParams.SetText(sDescription);
    m_lbDesc.ClearItems();

    CSWGuiExtent cExtent = m_cDescriptionText.m_cExtent;
    if (sDescription.GetLength() > 0)
    {
        cExtent.m_nHeight = m_cDescriptionText.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_cDescriptionText.m_cText.GetFontHeight();
    }

    m_cDescriptionText.SetExtent(cExtent);

    CSWGuiControl* pControl = &m_cDescriptionText;
    m_lbDesc.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDesc.SetSelectedControl(&m_cDescriptionText, 0);
}
// PC 0x00714220
int CSWGuiInGameEquip::EquipItem(unsigned long oidItem, int nSlot,
                                 int bClearActions)
{
    CGameObject* pObject = 0;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    unsigned long oidServer =
        pServer->ClientToServerObjectId(oidItem);
    pServer = g_pAppManager->m_pServerExoApp;
    if (pServer->GetObjectArray()->GetGameObject(
            oidServer, &pObject) == CGameObjectArray::SUCCESS &&
        pObject->AsSWSItem() != 0)
    {
        CSWSCreature* pCreature = m_pCreature->GetServerCreature();
        if (pCreature != 0 &&
            (g_pRules->m_pBaseItems
                 ->GetBaseItem(pObject->AsSWSItem()->GetBaseItemType())
                 ->GetEquipableSlots() &
             nSlot) != 0)
        {
            if (bClearActions != 0)
            {
                pCreature->ClearAllActions(1);
                pCreature->m_pcCombatRound->RemoveAllActions();
            }

            pCreature->AddEquipItemActions(pObject->AsSWSItem(), nSlot, 0, 1,
                                           1, 0, bClearActions == 0);
            if (pCreature->m_bCombatState != 0)
            {
                unsigned long oidTarget =
                    pCreature->m_pcCombatRound->m_oidUnknown9CC;
                if (oidTarget != OBJECT_INVALID)
                {
                    pCreature->AddAttackActions(oidTarget, 0, 0, 0, 0, 0, 0, 1,
                                                10009, 1500, 1, 0, 0, 4, 0);
                }
            }
            return 1;
        }
    }

    return 0;
}
// PC 0x00716320
int CSWGuiInGameEquip::EquipItem(CSWSItem* pItem, int bFlag1, int bFlag2)
{
    CSWSCreature* pCreature = m_pCreature->GetServerCreature();
    if (pCreature != 0)
    {
        CItemRepository* pRepository = pCreature->GetItemRepository(1);
        for (unsigned int i = 0; i < pRepository->m_lstItems.num; i++)
        {
            CSWSItem* pOther = pRepository->ItemListGetItem(i);
            if (pItem->CompareItem(pOther) != 0)
            {
                unsigned long oidItem = pOther->m_nId;
                return EquipItem(oidItem, bFlag1, bFlag2);
            }
        }
    }

    return 0;
}
// PC 0x00714380
void CSWGuiInGameEquip::UnequipItem(unsigned long oidItem, int bClearActions)
{
    CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(
        g_pAppManager->m_pServerExoApp->ClientToServerObjectId(oidItem));
    if (pItem != 0)
    {
        CSWSCreature* pCreature = m_pCreature->GetServerCreature();
        if (pCreature != 0)
        {
            if (bClearActions != 0)
            {
                pCreature->ClearAllActions(1);
                pCreature->m_pcCombatRound->RemoveAllActions();
            }

            pCreature->AddUnequipActions(pItem, OBJECT_INVALID, 0, 1, 1);
            CSWCItem* pClientItem =
                g_pAppManager->m_pClientExoApp->GetItemByGameObjectID(oidItem);
            if (pClientItem != 0)
            {
                pClientItem->PowerItem(0, 0, 0, 0);
            }
            if (pCreature->m_bCombatState != 0)
            {
                unsigned long oidTarget =
                    pCreature->m_pcCombatRound->m_oidUnknown9CC;
                if (oidTarget != OBJECT_INVALID)
                {
                    pCreature->AddAttackActions(oidTarget, 0, 0, 0, 0, 0, 0, 1,
                                                10009, 1500, 1, 0, 0, 4, 0);
                }
            }
        }
    }
}
// PC 0x00714470
void CSWGuiInGameEquip::ShowCantEquipMessage(int bShow, unsigned long nStrRef)
{
    int bEnabled = bShow == 0;
    m_lblAtkl.SetShown(bEnabled);
    m_lblAtkr.SetShown(bEnabled);
    m_lblDef.SetShown(bEnabled);
    m_lblTohitl.SetShown(bEnabled);
    m_lblTohitr.SetShown(bEnabled);
    m_lblAttackInfo.SetShown(bEnabled);
    m_lblDefInfo.SetShown(bEnabled);
    m_lblTohit.SetShown(bEnabled);
    m_lblDamage.SetShown(bEnabled);
    if (bShow != 0)
    {
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        m_lblCantequip.m_cText.m_cParams.SetText(pClient->GetGUIString(nStrRef));
    }
    m_lblCantequip.m_bEnabled = bShow;
    m_btnEquip.SetEnabled(bEnabled);
    m_btnEquip.m_cText.m_cParams.SetColor(
        bShow ? CGuiInGame::DISABLED_TEXT : CGuiInGame::COLOR_BLUE);
}
// PC 0x00719980
void CSWGuiInGameEquip::OnPanelRemoved()
{
    int i;

    if (m_bDescriptionShown)
    {
        CloseDescription();
    }
    m_lbItems.ClearItems();
    for (i = 0; i < m_lItemEntries.num; i++)
    {
        if (m_lItemEntries[i] != 0)
        {
            delete m_lItemEntries[i];
        }
        m_lItemEntries[i] = 0;
    }
    m_lItemEntries.RemoveAll();
    m_nPendingSlot = 0;
    m_oidPendingItem = OBJECT_INVALID;
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x00716820
void CSWGuiInGameEquip::OnSelectSlot(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
        if (m_pCreature->m_pArea->GetServerArea()->GetPlayerRestrictMode())
        {
            CSWGuiMessageBox* pMessage = g_pAppManager->GetClientExoApp()
                ->GetInGameGui()->m_pMessageBox;
            pMessage->SetAllowCancel(0);
            pMessage->SetCallback(0, 0, 0);
            pMessage->SetMessage(STRREF_ENVIRO_SUIT_RESTRICTION);
            m_pManager->AddPanel(pMessage, SWGUI_PANEL_FLAG_PUSH_MODAL, 1);
            return;
        }
        if (m_nSelectedSlot == INVENTORY_SLOT_ARMOUR &&
            m_pCreature->GetServerCreature()->GetCombatState() == 1 &&
            m_pCreature->GetServerCreature()->GetItemUseState() == 1)
        {
            CSWGuiMessageBox* pMessage = g_pAppManager->GetClientExoApp()
                ->GetInGameGui()->m_pMessageBox;
            pMessage->SetAllowCancel(0);
            pMessage->SetCallback(0, 0, 0);
            int nStrRef = STRREF_ARMOUR_COMBAT_RESTRICTION;
            pMessage->SetMessage(nStrRef);
            CSWGuiManager* pManager = m_pManager;
            if (nStrRef == 0 && pManager == NULL)
                return;
            pManager->AddPanel(pMessage, SWGUI_PANEL_FLAG_PUSH_MODAL, 1);
            return;
        }
        if (m_nSelectedSlot == INVENTORY_SLOT_LEFTHAND &&
            m_aoidEquippedItems[RIGHTHAND_ITEM_INDEX] != OBJECT_INVALID)
        {
            CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
            CSWSItem* pItem = pServer->GetItemByGameObjectID(
                pServer->ClientToServerObjectId(
                    m_aoidEquippedItems[RIGHTHAND_ITEM_INDEX]));
            if (pItem && pItem->GetBaseItem()->m_nWeaponSize ==
                             CSWBaseItem::WEAPON_SIZE_TWO_HANDED)
            {
                CSWGuiMessageBox* pMessage = g_pAppManager->GetClientExoApp()
                    ->GetInGameGui()->m_pMessageBox;
                pMessage->SetAllowCancel(0);
                pMessage->SetCallback(0, 0, 0);
                pMessage->SetMessage(STRREF_TWO_HANDED_RESTRICTION);
                m_pManager->AddPanel(pMessage, SWGUI_PANEL_FLAG_PUSH_MODAL, 1);
                return;
            }
        }
        if (m_lbItems.m_lItems.num == 1)
        {
            CSWGuiMessageBox* pMessage = g_pAppManager->GetClientExoApp()
                ->GetInGameGui()->m_pMessageBox;
            pMessage->SetAllowCancel(0);
            pMessage->SetCallback(0, 0, 0);
            pMessage->SetMessage(STRREF_NO_EQUIPPABLE_ITEMS);
            m_pManager->AddPanel(pMessage, SWGUI_PANEL_FLAG_PUSH_MODAL, 1);
            return;
        }
        m_pDescribedControl = pControl;
        ShowDescription(1);
        SetActiveControl(&m_lbItems, 0);
        if (m_aoidEquippedItems[(int)pControl->GetUserData()] != OBJECT_INVALID)
        {
            m_lbItems.SetSelectedControl(1, 0);
        }
        m_bDescriptionShown = 1;
        g_pAppManager->GetClientExoApp()->ShowTutorialWindow(
            11, OBJECT_INVALID, OBJECT_INVALID, 0);
        m_lbItems.SetEnabled(1);
    }
}
// PC 0x00715F80
void CSWGuiInGameEquip::ShowDescription(int bShow)
{
    ShowCantEquipMessage(0, -1);
    if (bShow)
    {
        m_lbDesc.SetShown(1);
        m_btnBack.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(1581));
        for (int i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
        {
            m_aSlotButtons[i].SetShown(0);
            m_aSlotLabels[i].SetShown(0);
        }
        m_lblSlotname.SetShown(0);
        m_btnEquip.SetShown(1);
        for (int i = 0; i < m_lbItems.m_lItems.num; i++)
        {
            m_lbItems.GetControl(i)->AsButton()->m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
        }
    }
    else
    {
        m_lbDesc.SetShown(0);
        m_btnBack.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(1582));
        for (int i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
        {
            m_aSlotButtons[i].SetShown(1);
            m_aSlotLabels[i].SetShown(1);
        }
        m_lblSlotname.SetShown(1);
        m_btnEquip.SetShown(0);
        for (int i = 0; i < m_lbItems.m_lItems.num; i++)
        {
            m_lbItems.GetControl(i)->AsButton()->m_cBorder.m_cParams.m_fHilightAlpha = 0.0f;
        }
    }
}

// PC 0x00717D30
void CSWGuiInGameEquip::OnItemSelected(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
        if (!m_lbDesc.m_bEnabled)
        {
            pControl->SetActive(0);
            return;
        }

        CSWGuiInGameItemEntry* pEntry = (CSWGuiInGameItemEntry*)pControl;
        unsigned long oidItem = pEntry->m_oidItem;
        unsigned long oidEquipped = m_pCreature->GetEquippedItemID(m_nSelectedSlot);
        if (oidItem != OBJECT_INVALID)
        {
            unsigned long oidServer = g_pAppManager->GetServerExoApp()
                ->ClientToServerObjectId(oidItem);
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()
                ->GetItemByGameObjectID(oidServer);
            if (pItem != 0)
            {
                CExoString sDescription;
                sDescription = pItem->GetPropertyDescription();
                if (sDescription != "")
                {
                    SetDescription(sDescription);
                }
                else
                {
                    SetDescription(g_pAppManager->GetClientExoApp()->GetGUIString(
                        STRREF_NO_DESCRIPTION));
                }
            }
            else if (oidItem == m_oidDescribed && m_pDescribedCopy != 0 &&
                     m_pDescribedCopy->GetPropertyDescription() != "")
            {
                SetDescription(m_pDescribedCopy->GetPropertyDescription());
            }
            else
            {
                SetDescription(g_pAppManager->GetClientExoApp()->GetGUIString(
                    STRREF_NO_DESCRIPTION));
            }
        }
        else
        {
            SetDescription("");
        }

#ifndef _XBOX
        if (m_lbItems.m_bHilightOnOver && m_pCreature->GetServerCreature() != 0)
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            switch (pEntry->m_nCanUse)
            {
            case CSWGuiInGameItemEntry::CAN_USE:
            case CSWGuiInGameItemEntry::CAN_USE_EQUIPPED:
                ShowCantEquipMessage(0, -1);
                if (oidItem != oidEquipped)
                {
                    if (m_oidDescribed == 0)
                    {
                        m_oidDescribed = m_aoidEquippedItems[GetSlotItemIdIndex(m_nSelectedSlot)];
                        if (m_pDescribedCopy != 0)
                        {
                            delete m_pDescribedCopy;
                        }
                        m_pDescribedCopy = CopyItem(m_oidDescribed);
                    }
                    if (oidItem != OBJECT_INVALID)
                    {
                        CSWSItem* pItem;
                        if (m_nSelectedSlot == INVENTORY_SLOT_RIGHTHAND)
                        {
                            CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
                            pItem = pServer->GetItemByGameObjectID(
                                pServer->ClientToServerObjectId(oidItem));
                            if (pItem != 0 &&
                                pItem->GetBaseItem()->m_nWeaponSize == CSWBaseItem::WEAPON_SIZE_TWO_HANDED &&
                                m_aoidEquippedItems[LEFTHAND_ITEM_INDEX] != OBJECT_INVALID)
                            {
                                m_oidCompared = m_aoidEquippedItems[LEFTHAND_ITEM_INDEX];
                                if (m_pComparedCopy != 0)
                                {
                                    delete m_pComparedCopy;
                                }
                                m_pComparedCopy = CopyItem(m_oidCompared);
                            }
                        }
                        m_pManager->PlayGuiSound(SWGUI_SOUND_EQUIP_ITEM);
                        CSWCItem* pClientItem = g_pAppManager->GetClientExoApp()
                            ->GetItemByGameObjectID(oidEquipped);
                        if (pClientItem != 0)
                        {
                            pClientItem->PowerItem(0, 0, 0, 0);
                        }
                        if (ItemExists(oidItem))
                        {
                            EquipItem(oidItem, m_nSelectedSlot, 1);
                        }
                        else if (oidItem == m_oidDescribed && m_pDescribedCopy != 0)
                        {
                            EquipItem(m_pDescribedCopy, m_nSelectedSlot, 1);
                        }
                        if (m_nSelectedSlot == INVENTORY_SLOT_RIGHTHAND && pItem != 0 &&
                            pItem->GetBaseItem()->m_nWeaponSize < CSWBaseItem::WEAPON_SIZE_TWO_HANDED &&
                            m_oidCompared != 0)
                        {
                            if (ItemExists(m_oidCompared))
                            {
                                EquipItem(m_oidCompared, INVENTORY_SLOT_LEFTHAND, 0);
                            }
                            else if (m_pComparedCopy != 0)
                            {
                                EquipItem(m_pComparedCopy, INVENTORY_SLOT_LEFTHAND, 0);
                            }
                            m_oidCompared = 0;
                            if (m_pComparedCopy != 0)
                            {
                                delete m_pComparedCopy;
                                m_pComparedCopy = 0;
                            }
                        }
                    }
                    else
                    {
                        int bUnequip = 1;
                        m_pManager->PlayGuiSound(SWGUI_SOUND_UNEQUIP_ITEM);
                        if (m_nSelectedSlot == INVENTORY_SLOT_RIGHTHAND &&
                            m_aoidEquippedItems[LEFTHAND_ITEM_INDEX] != OBJECT_INVALID)
                        {
                            m_oidCompared = m_aoidEquippedItems[LEFTHAND_ITEM_INDEX];
                            if (m_pComparedCopy != 0)
                            {
                                delete m_pComparedCopy;
                            }
                            m_pComparedCopy = CopyItem(m_oidCompared);
                            UnequipItem(m_aoidEquippedItems[LEFTHAND_ITEM_INDEX], 1);
                            bUnequip = 0;
                        }
                        UnequipItem(oidEquipped, bUnequip);
                    }
                    m_nPendingSlot = m_nSelectedSlot;
                    m_oidPendingItem = oidItem;
                }
                break;

            case CSWGuiInGameItemEntry::CANNOT_USE:
            case CSWGuiInGameItemEntry::CANNOT_USE_WITH_OTHER_HAND:
                ShowCantEquipMessage(1, pEntry->m_nCanUse == CSWGuiInGameItemEntry::CANNOT_USE
                    ? STRREF_ITEM_PREREQUISITES : STRREF_INCOMPATIBLE_WEAPON);
                break;
            }
        }
#endif
    }
}
// PC 0x00719A40
void CSWGuiInGameEquip::OnOKPressed(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0 && m_bDescriptionShown)
    {
        m_oidDescribed = 0;
        m_oidCompared = 0;
        if (m_pDescribedCopy != 0)
        {
            delete m_pDescribedCopy;
            m_pDescribedCopy = 0;
        }
        if (m_pComparedCopy != 0)
        {
            delete m_pComparedCopy;
            m_pComparedCopy = 0;
        }
        CloseDescription();
    }
}
// PC 0x00718450
void CSWGuiInGameEquip::CloseDescription()
{
    ShowDescription(0);
    m_bDescriptionShown = 0;
    SetActiveControl(m_pDescribedControl, 0);
    OnEnterSlot(m_pActiveControl);

    m_lbItems.SetEnabled(0);
}
// PC 0x00716A80
void CSWGuiInGameEquip::OnEnterSlot(CSWGuiControl* pControl)
{

    if (m_pCreature != NULL)
    {
        int nSlotIndex = pControl->m_nIndex;
        int bChanged = 0;
        m_nSelectedSlot = SLOT_INFO[nSlotIndex].nSlot;
        if (m_nSelectedSlot != -1)
        {
            CSWSCreature* pCreature = m_pCreature->GetServerCreature();
            CItemRepository* pRepository = pCreature->GetItemRepository(1);
            CExoArrayList<CSWGuiControl*> lItems;
            CSWGuiInGameItemEntry* pEntry = AddItemEntryToList(0);
            pEntry->SetItem(OBJECT_INVALID, 0, 0);
            pEntry->SetCanUse(CSWGuiInGameItemEntry::CAN_USE);
            pEntry->m_nClickSound = CSWGuiControl::GUISOUND_NONE;
            lItems.Add(pEntry);
            int nEntry = 1;
            if (m_aoidEquippedItems[nSlotIndex] != OBJECT_INVALID)
            {
                pEntry = AddItemEntryToList(nEntry);
                if (pEntry->GetItem() != m_aoidEquippedItems[nSlotIndex] || !pEntry->IsEquipped() ||
                    pEntry->GetCanUse() != CSWGuiInGameItemEntry::CAN_USE_EQUIPPED)
                {
                    bChanged = 1;
                }
                pEntry->SetItem(m_aoidEquippedItems[nSlotIndex], 1, 0);
                pEntry->SetCanUse(CSWGuiInGameItemEntry::CAN_USE_EQUIPPED);
                pEntry->m_nClickSound = CSWGuiControl::GUISOUND_NONE;
                lItems.Add(pEntry);
                ++nEntry;
            }
            int bShowItems = 1;
            if (m_nSelectedSlot == INVENTORY_SLOT_LEFTHAND &&
                m_aoidEquippedItems[RIGHTHAND_ITEM_INDEX] != OBJECT_INVALID)
            {
                CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
                CSWSItem* pItem = pServer->GetItemByGameObjectID(
                    pServer->ClientToServerObjectId(m_aoidEquippedItems[RIGHTHAND_ITEM_INDEX]));
                if (pItem != NULL)
                {
                    bShowItems = pItem->GetBaseItem()->GetWeaponSize() !=
                                 CSWBaseItem::WEAPON_SIZE_TWO_HANDED;
                }
            }
            if (bShowItems)
            {
                for (unsigned long nItem = 0;
                     nItem < (unsigned long)pRepository->GetNumItems(); ++nItem)
                {
                    CSWSItem* pItem = pRepository->ItemListGetItem(nItem);
                    if (pItem == NULL)
                        continue;
                    int nSelectedSlot = m_nSelectedSlot;
                    if (!(nSelectedSlot & pItem->GetBaseItem()->GetEquipableSlots()))
                        continue;
                    unsigned char nDroidOrHuman = pItem->GetBaseItem()->m_nDroidOrHuman;
                    if (nDroidOrHuman != CSWBaseItem::DROID_OR_HUMAN_ANY)
                    switch (nDroidOrHuman)
                    {
                    case CSWBaseItem::DROID_OR_HUMAN_HUMAN:
                    {
                        unsigned short nRace = pCreature->GetStats()->m_nRace;
                        if (nRace != CSWRace::CRACE_HUMAN)
                            continue;
                        break;
                    }
                    case CSWBaseItem::DROID_OR_HUMAN_DROID:
                    {
                        unsigned short nRace = pCreature->GetStats()->m_nRace;
                        if (nRace == CSWRace::CRACE_DROID)
                            break;
                        continue;
                    }
                    }
                    unsigned char nCanUse = CSWGuiInGameItemEntry::CAN_USE;
                    unsigned long nSlot = m_nSelectedSlot;
                    if (!pCreature->CanEquipItem(pItem, &nSlot, 0, 0, 0))
                    {
                        nCanUse = CSWGuiInGameItemEntry::CANNOT_USE;
                    }
                    int bProficient = 1;
                    CClientOptions* pOptions =
                        g_pAppManager->GetClientExoApp()->GetClientOptions();
                    if (pOptions != NULL && pOptions->m_bHideUnequippable)
                    {
                        if (!pCreature->CheckProficiencies(pItem, m_nSelectedSlot))
                            bProficient = 0;
                        nSlot = m_nSelectedSlot;
                        if (!pCreature->CanEquipItem(pItem, &nSlot, 0, 0, 0) ||
                            !bProficient)
                            continue;
                    }
                    CSWCItem* pClientItem = pItem->GetClientItem();
                    if (pClientItem == NULL)
                        continue;
                    int nHandSlot = this > NULL ? m_nSelectedSlot : m_nSelectedSlot;
                    if (nHandSlot == INVENTORY_SLOT_RIGHTHAND ||
                        nHandSlot == INVENTORY_SLOT_LEFTHAND)
                    {
                        CSWCItem* pRight;
                        CSWCItem* pLeft;
                        if (nHandSlot == INVENTORY_SLOT_RIGHTHAND)
                        {
                            pRight = pClientItem;
                            pLeft = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                                m_pCreature->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND));
                        }
                        else
                        {
                            pRight = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                                m_pCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
                            pLeft = pClientItem;
                        }
                        if (pRight != NULL && pLeft != NULL &&
                            (pRight != pClientItem || pItem->GetBaseItem()->m_nWeaponSize !=
                                                       CSWBaseItem::WEAPON_SIZE_TWO_HANDED))
                        {
                            CSWBaseItem* pRightBase = pRight->GetBaseItem();
                            int nWield = (unsigned char)pRightBase->m_nWeaponWield;
                            int nLeftW = (unsigned char)pLeft->GetBaseItem()->m_nWeaponWield;
                            if (nWield != nLeftW ||
                                (nWield != CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2 &&
                                 nWield != CSWBaseItem::WEAPON_WIELD_ONE_HANDED_4))
                            {
                                nCanUse = CSWGuiInGameItemEntry::CANNOT_USE_WITH_OTHER_HAND;
                            }
                        }
                    }
                    pEntry = AddItemEntryToList(nEntry);
                    if (pEntry->GetItem() != pClientItem->GetId() || pEntry->IsEquipped() ||
                        pEntry->GetCanUse() != nCanUse)
                    {
                        bChanged = 1;
                    }
                    pEntry->SetItem(pClientItem->GetId(), 0, 0);
                    pEntry->SetCanUse(nCanUse);
                    pEntry->m_nClickSound = CSWGuiControl::GUISOUND_NONE;
                    lItems.Add(pEntry);
                    ++nEntry;
                }
            }
            if (bChanged || lItems.num != m_lbItems.m_lItems.num)
            {
                m_lbItems.AddControls(lItems, 0, 1, 0);
                m_lbItems.m_cScrollBar.SetCurValue(0);
            }
            unsigned long nStrRef;
            if (m_pCreature->GetStats()->m_nRace == CSWRace::CRACE_DROID)
                nStrRef = SLOT_INFO[nSlotIndex].nDescriptionStrRef;
            else
                nStrRef = SLOT_INFO[nSlotIndex].nNameStrRef;
            m_lblSlotname.m_cText.m_cParams.SetText(
                g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef));
        }
        else
        {
            m_lbItems.ClearItems();
            CExoString sEmpty("");
            m_lblSlotname.m_cText.m_cParams.SetText(sEmpty);
        }
    }
}
// PC 0x00719F10
CSWGuiInGameItemEntry* CSWGuiInGameEquip::AddItemEntryToList(int nItem)
{
    CSWGuiInGameItemEntry* pItemEntry;
    if (nItem == m_lItemEntries.num)
    {
        pItemEntry = new CSWGuiInGameItemEntry;
        m_lItemEntries.Add(pItemEntry);
        CSWGuiButton* pProto = (CSWGuiButton*)m_lbItems.m_pProtoItem;
        pItemEntry->Initialize(pProto->m_cText, pProto->m_cBorder,
                               pProto->m_cHilite,
                               m_lbItems.m_cItemExtent.m_nWidth -
                                   2 * m_lbItems.m_nPadding,
                               1);
        pItemEntry->AddEvent(0, this,
                            (CSWGuiHandler)&CSWGuiInGameEquip::OnItemSelected);
        pItemEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                            (CSWGuiHandler)&CSWGuiInGameEquip::OnOKPressed);
        pItemEntry->m_cBorder.m_cParams.m_fHilightAlpha = 0.0f;
        return pItemEntry;
    }

    pItemEntry = m_lItemEntries[nItem];
    pItemEntry->m_cBorder.m_cParams.m_fHilightAlpha = 0.0f;
    return pItemEntry;
}
#endif
void CSWGuiContainer::HandleInputEvent(int nEvent, int nState)  // PC 0x0071A050
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_ACCEPT:
            if ((m_nGiveModeFlags & GIVE_MODE_ACTIVE) == 0)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            }

        case GUIEVENT_ACTIVATE:
            if ((m_nGiveModeFlags & GIVE_MODE_ACTIVE) != 0)
            {
                int bTakeAll = 0;
                if (m_nContainerType == CONTAINER_TAKE_ALL)
                {
                    bTakeAll = 1;
                }
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                g_pAppManager->GetClientExoApp()->GetSWCMessage()->
                    SendPlayerToServerGuiContainer_Close(m_oidContainer, bTakeAll);
                g_pAppManager->m_pClientExoApp->GetInGameGui()->HideContainerGui();
            }
            break;
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            g_pAppManager->GetClientExoApp()->GetSWCMessage()->
                SendPlayerToServerGuiContainer_Close(m_oidContainer, 0);
            g_pAppManager->m_pClientExoApp->GetInGameGui()->HideContainerGui();
            break;
        case GUIEVENT_TOGGLE_MODE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            if ((m_nGiveModeFlags & GIVE_MODE_ACTIVE) != 0)
            {
#ifdef _XBOX
                SetupGiveMode();
#else
                SetupGiveMode(0);
#endif
            }
            else
            {
                SetContainer(m_oidContainer, m_nContainerType);
            }
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#ifndef _XBOX
void CSWGuiInGameEquip::UpdateInventory()  // PC 0x00717070
{

    CSWSItem* pItem = NULL;
    CSWCItem* pClientItem = NULL;
    CResRef resIcon;
    int bDoubleWeapon = 0;
    int bUnarmed = 0;
    unsigned long oidItem;

    m_aSlotLabels[LEFTHAND_ITEM_INDEX].m_cBorder.m_cParams.SetFillImage(
        GetIcon(LEFTHAND_ITEM_INDEX), 0);
    m_aSlotLabels[LEFTHAND_ITEM_INDEX].m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;

    for (int nSlot = 0; nSlot < NUM_EQUIPMENT_SLOTS; ++nSlot)
    {
        int bChanged = 0;
        if (m_pCreature != NULL)
        {
            oidItem = m_pCreature->GetEquippedItemID(SLOT_INFO[nSlot].nSlot);
            pClientItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
            if (pClientItem != NULL)
                pItem = pClientItem->GetServerItem();
        }

        if (pClientItem != NULL && pItem != NULL)
        {
            m_aoidEquippedItems[nSlot] = oidItem;
            resIcon = pItem->GetIcon();
            int bDouble = 0;
            if (pItem->GetBaseItem()->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
                bDouble = 1;
            CSWGuiLabel* pDamage = NULL;
            CSWGuiLabel* pToHit = NULL;
            int bOffHand = 0;
            switch (nSlot)
            {
            case LEFTHAND_ITEM_INDEX:
                pDamage = &m_lblAtkl;
                pToHit = &m_lblTohitl;
                bOffHand = 1;
            case RIGHTHAND_ITEM_INDEX:
                if (bDouble && !bDoubleWeapon)
                {
                    pDamage = &m_lblAtkl;
                    pToHit = &m_lblTohitl;
                    bOffHand = 1;
                    --nSlot;
                    bDoubleWeapon = 1;
                }
                if (pDamage == NULL)
                {
                    pDamage = &m_lblAtkr;
                    pToHit = &m_lblTohitr;
                    bOffHand = 0;
                }
                {
                    CSWSCreature* pCreature = m_pCreature->GetServerCreature();
                    CExoString sDamage;
                    CExoString sToHit;
                    int nMinimum = 0;
                    int nMaximum = 0;
                    int bDamageBonus = 0;
                    pCreature->GetStats()->GetTotalDamageRange(
                        pCreature, pItem, nMinimum, nMaximum, bOffHand, 1);
                    if (nMinimum)
                        sDamage.Format("%d-%d", nMinimum, nMaximum);
                    int nBaseMinimum = 0;
                    int nBaseMaximum = 0;
                    pCreature->GetStats()->GetTotalDamageRange(
                        pCreature, pItem, nBaseMinimum, nBaseMaximum, bOffHand, 0);
                    if (nMinimum > nBaseMinimum || nMaximum > nBaseMaximum)
                        bDamageBonus = 1;
                    if (nSlot == RIGHTHAND_ITEM_INDEX &&
                        (pItem->GetBaseItem()->m_nWeaponSize == CSWBaseItem::WEAPON_SIZE_TWO_HANDED ||
                         pItem->GetBaseItem()->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON))
                    {
                        m_aSlotLabels[LEFTHAND_ITEM_INDEX].m_cBorder.m_cParams.SetFillImage(resIcon, 0);
                        m_aSlotLabels[LEFTHAND_ITEM_INDEX].m_cBorder.m_cParams.m_fHilightAlpha = 0.25f;
                    }
                    pDamage->m_cText.m_cParams.SetText(sDamage);
                    if (bDamageBonus)
                        pDamage->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_GREEN);
                    else
                        pDamage->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                    int nAttackBonus;
                    if (pItem->GetBaseItem()->m_nRangedWeapon > 0)
                        nAttackBonus = pCreature->GetStats()->GetRangedAttackBonus(bOffHand, 1, 0);
                    else
                        nAttackBonus = pCreature->GetStats()->GetMeleeAttackBonus(bOffHand, 1, 0, bDouble);
                    if (nAttackBonus > 0)
                        sToHit.Format("+%d", nAttackBonus);
                    else
                        sToHit.Format("%d", nAttackBonus);
                    pToHit->m_cText.m_cParams.SetText(sToHit);
                    if (nAttackBonus > pCreature->GetStats()->GetBaseAttackBonus())
                        pToHit->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_GREEN);
                    else
                        pToHit->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                }
                break;
            }
            if (!bDouble || nSlot == RIGHTHAND_ITEM_INDEX)
                m_aSlotLabels[nSlot].m_cBorder.m_cParams.SetFillImage(resIcon, 0);
            bChanged = 1;
        }
        else
        {
            switch (nSlot)
            {
            case RIGHTHAND_ITEM_INDEX:
            {
                bChanged = 1;
                bUnarmed = 1;
                CSWSCreature* pCreature = m_pCreature->GetServerCreature();
                if (pCreature != NULL)
                {
                    int nMinimum = 0;
                    int nMaximum = 0;
                    CExoString sDamage;
                    pCreature->GetStats()->GetTotalDamageRange(
                        pCreature, NULL, nMinimum, nMaximum, 0, 1);
                    if (nMinimum)
                        sDamage.Format("%d-%d", nMinimum, nMaximum);
                    m_lblAtkr.m_cText.m_cParams.SetText(sDamage);
                    CExoString sToHit;
                    int nAttackBonus = pCreature->GetStats()->GetMeleeAttackBonus(0, 1, 0, 0);
                    if (nAttackBonus > 0)
                        sToHit.Format("+%d", nAttackBonus);
                    else
                        sToHit.Format("%d", nAttackBonus);
                    m_lblTohitr.m_cText.m_cParams.SetText(sToHit);
                }
                break;
            }
            }
            if (m_aoidEquippedItems[nSlot] != OBJECT_INVALID || (!bDoubleWeapon && !bUnarmed))
            {
                m_aoidEquippedItems[nSlot] = OBJECT_INVALID;
                switch (nSlot)
                {
                case LEFTHAND_ITEM_INDEX:
                    m_lblAtkl.m_cText.m_cParams.SetText("");
                    m_lblAtkl.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                    m_lblTohitl.m_cText.m_cParams.SetText("");
                    m_lblTohitl.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                    break;
                case RIGHTHAND_ITEM_INDEX:
                    m_lblAtkr.m_cText.m_cParams.SetText("");
                    m_lblAtkr.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                    m_lblTohitr.m_cText.m_cParams.SetText("");
                    m_lblTohitr.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                    break;
                }
                m_aSlotLabels[nSlot].m_cBorder.m_cParams.SetFillImage(GetIcon(nSlot), 0);
                bChanged = 1;
            }
        }
        if (bChanged && m_pActiveControl == &m_aSlotButtons[nSlot])
            OnEnterSlot(m_pActiveControl);
    }

    CExoString sDefense(m_pCreature->GetServerCreature()->GetArmorClass());
    m_lblDef.m_cText.m_cParams.SetText(sDefense);

    if (m_nPendingSlot)
    {
        unsigned long oidPending = m_oidPendingItem;
        int bPending = oidPending != m_pCreature->GetEquippedItemID(m_nPendingSlot);
        if (bPending)
        {
            CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
            CSWSItem* pPending = pServer->GetItemByGameObjectID(pServer->ClientToServerObjectId(oidPending));
            pServer = g_pAppManager->GetServerExoApp();
            CSWSItem* pEquipped = pServer->GetItemByGameObjectID(pServer->ClientToServerObjectId(
                m_pCreature->GetEquippedItemID(m_nPendingSlot)));
            if (pEquipped != NULL)
            {
                if (pPending != NULL)
                    bPending = !pEquipped->CompareItem(pPending);
                else
                    bPending = 0;
            }
            else if (pPending == NULL)
                bPending = 0;
        }
        if (bPending)
        {
            CExoString sPending("--");
            switch (m_nPendingSlot)
            {
            case INVENTORY_SLOT_RIGHTHAND:
                m_lblAtkr.m_cText.m_cParams.SetText(sPending);
                m_lblTohitr.m_cText.m_cParams.SetText(sPending);
                if (m_lblAtkl.m_cText.m_cParams.m_sText != "")
                {
                    m_lblAtkl.m_cText.m_cParams.SetText(sPending);
                    m_lblTohitl.m_cText.m_cParams.SetText(sPending);
                }
                break;
            case INVENTORY_SLOT_ARMOUR:
                m_lblDef.m_cText.m_cParams.SetText(sPending);
                break;
            case INVENTORY_SLOT_LEFTHAND:
                m_lblTohitr.m_cText.m_cParams.SetText(sPending);
                m_lblAtkl.m_cText.m_cParams.SetText(sPending);
                m_lblTohitl.m_cText.m_cParams.SetText(sPending);
                break;
            case INVENTORY_SLOT_HEAD:
            case INVENTORY_SLOT_HANDS:
            case INVENTORY_SLOT_RIGHTRING:
            case INVENTORY_SLOT_NECK:
            case INVENTORY_SLOT_BELT:
            case INVENTORY_SLOT_ARMS:
                m_lblAtkr.m_cText.m_cParams.SetText(sPending);
                m_lblTohitr.m_cText.m_cParams.SetText(sPending);
                if (m_lblAtkl.m_cText.m_cParams.m_sText != "")
                {
                    m_lblAtkl.m_cText.m_cParams.SetText(sPending);
                    m_lblTohitl.m_cText.m_cParams.SetText(sPending);
                }
                m_lblDef.m_cText.m_cParams.SetText(sPending);
                break;
            }
        }
        else
            m_nPendingSlot = 0;
    }
}
// PC 0x00719AA0
void CSWGuiInGameEquip::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
        case GUIEVENT_EXIT:
            if (m_bDescriptionShown)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                if (m_oidDescribed != 0)
                {
                    unsigned long oidEquipped =
                        m_aoidEquippedItems[GetSlotItemIdIndex(m_nSelectedSlot)];
                    if (m_oidDescribed != oidEquipped)
                    {
                        if (m_oidDescribed != OBJECT_INVALID)
                        {
                            if (ItemExists(m_oidDescribed))
                            {
                                EquipItem(m_oidDescribed, m_nSelectedSlot, 1);
                            }
                            else if (m_pDescribedCopy != 0)
                            {
                                EquipItem(m_pDescribedCopy, m_nSelectedSlot, 1);
                            }
                        }
                        else
                        {
                            UnequipItem(oidEquipped, 1);
                        }
                        if (m_nSelectedSlot == INVENTORY_SLOT_RIGHTHAND &&
                            m_oidCompared != 0)
                        {
                            if (ItemExists(m_oidCompared))
                            {
                                EquipItem(m_oidCompared, INVENTORY_SLOT_LEFTHAND, 0);
                            }
                            else if (m_pComparedCopy != 0)
                            {
                                EquipItem(m_pComparedCopy, INVENTORY_SLOT_LEFTHAND, 0);
                            }
                        }
                    }
                }
                m_oidDescribed = 0;
                if (m_pDescribedCopy != 0)
                {
                    delete m_pDescribedCopy;
                    m_pDescribedCopy = 0;
                }
                m_oidCompared = 0;
                if (m_pComparedCopy != 0)
                {
                    delete m_pComparedCopy;
                    m_pComparedCopy = 0;
                }
                CloseDescription();
            }
            else
            {
#ifdef _XBOX
                if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui())
#else
                if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui(0))
#endif
                {
#ifdef _XBOX
                    g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                    g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
                }
            }
            break;

        case GUIEVENT_ACCEPT:
            if (!m_bDescriptionShown)
            {
#ifdef _XBOX
                if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui())
#else
                if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui(0))
#endif
                {
#ifdef _XBOX
                    g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                    g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
                }
            }
            break;

        case GUIEVENT_NEXT_CHARACTER:
            if (!m_bDescriptionShown)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
                g_pAppManager->m_pClientExoApp
                    ->ChangeCharacterToNextLivingPartyMember(0, 0, 1);
                SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()
                                 ->GetPlayerCharacter());
                for (int nSlot = 0; nSlot < NUM_EQUIPMENT_SLOTS; ++nSlot)
                {
                    if (SLOT_INFO[nSlot].nSlot == m_nSelectedSlot)
                    {
                        SetActiveControl(&m_aSlotButtons[nSlot], 0);
                        break;
                    }
                }
                OnEnterSlot(m_pActiveControl);
            }

        case GUIEVENT_SCROLL_DOWN:
            if (m_bDescriptionShown)
            {
                m_lbDesc.HandleInputEvent(GUIEVENT_DOWN, 1);
            }
            break;

        case GUIEVENT_SCROLL_UP:
            if (m_bDescriptionShown)
            {
                m_lbDesc.HandleInputEvent(GUIEVENT_UP, 1);
            }
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

#endif
static inline CServerExoApp* GetServerExoApp()
{
    return g_pAppManager->m_pServerExoApp;
}

static inline void SwguiingameequipTrace2()
{
    SwguiingameequipTrace1();
}

static inline void SwguiingameequipTrace1()
{
}

#ifdef _XBOX
#include "exoinput.h"

inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}

CSWGuiInGameEquip::CSWGuiInGameEquip(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_pCreature = NULL;
    m_bDescriptionShown = 0;
    m_bResetItems = 0;
    CSWGuiButton btnItem;
    StartLoadFromLayout("equip");
    CExoString sButtonPrefix("BTN_");
    CExoString sLabelPrefix("LBL_");
    for (int nSlot = 0; nSlot < NUM_EQUIPMENT_SLOTS; ++nSlot)
    {
        InitControl(m_aSlotButtons[nSlot],
                    sButtonPrefix + SLOT_INFO[nSlot].sButton, 1);
        InitControl(m_aSlotLabels[nSlot],
                    sLabelPrefix + SLOT_INFO[nSlot].sButton, 1);
        m_aSlotButtons[nSlot].AddEvent(GUIEVENT_ACTIVATE, this,
            (CSWGuiHandler)&CSWGuiInGameEquip::OnSelectSlot);
        m_aSlotButtons[nSlot].AddEvent(GUIEVENT_MOUSE_ENTER, this,
            (CSWGuiHandler)&CSWGuiInGameEquip::OnEnterSlot);
        m_aSlotButtons[nSlot].m_nIndex = nSlot;
        m_aSlotButtons[nSlot].m_cHilite.m_cParams.SetPulsingAlpha(1, 0, 1);
        m_aoidEquippedItems[nSlot] = OBJECT_INVALID;
    }
    sLabelPrefix = "LBL_DESCBORDER";
    for (int nBorder = 0; nBorder < NUM_DESC_BORDERS; nBorder++)
    {
        InitControl(m_aDescBorders[nBorder], sLabelPrefix + CExoString(nBorder + 1), 1);
    }
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
    InitControl(m_lblAtkl, "LBL_ATKL", 1);
    InitControl(m_lblTohitl, "LBL_TOHITL", 1);
    InitControl(m_lblAtkr, "LBL_ATKR", 1);
    InitControl(m_lblTohitr, "LBL_TOHITR", 1);
    InitControl(m_lblDef, "LBL_DEF", 1);
    InitControl(m_lblVitality, "LBL_VITALITY", 1);
    InitControl(m_lbDesc, "LB_DESC", 1);
    m_lbDesc.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_lblPortrait, "LBL_PORTRAIT", 1);
    InitControl(m_lblAttackInfo, "LBL_ATTACK_INFO", 1);
    InitControl(m_lblDefInfo, "LBL_DEF_INFO", 1);
    InitControl(m_lblTxtbar, "LBL_TXTBAR", 1);
    InitControl(m_lblPortBord, "LBL_PORT_BORD", 1);
    InitControl(m_lblTohit, "LBL_TOHIT", 1);
    InitControl(m_lblDamage, "LBL_DAMAGE", 1);
    InitControl(m_lblSlotname, "LBL_SLOTNAME", 1);
    InitControl(m_lblEquip, "LBL_EQUIP", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblStart, "LBL_START", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblSelecttitle, "LBL_SELECTTITLE", 1);
    InitControl(m_lblBlack, "LBL_BLACK", 1);
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
            (CSWGuiHandler)&CSWGuiInGameEquip::OnItemSelected);
        m_aItemRows[nRow].AddEvent(CSWGuiControl::EVENT_SELECT, this,
            (CSWGuiHandler)&CSWGuiInGameEquip::OnOKPressed);
        m_aItemRows[nRow].AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
            (CSWGuiHandler)&CSWGuiInGameEquip::OnOKPressed);
        m_aItemRows[nRow].m_nClickSound = CSWGuiControl::GUISOUND_NONE;
        AddControl(&m_aItemRows[nRow]);
    }
    StopLoadFromLayout();
    m_lblAtkl.m_cText.m_cParams.SetText("");
    m_lblAtkr.m_cText.m_cParams.SetText("");
    m_lblTohitl.m_cText.m_cParams.SetText("");
    m_lblTohitr.m_cText.m_cParams.SetText("");
    m_lblDef.m_cText.m_cParams.SetText("");
    for (int nItemRow = 0; nItemRow < NUM_ITEM_ROWS; nItemRow++)
    {
        for (int nDirection = 0; nDirection < 4; nDirection++)
        {
            m_aItemRows[nItemRow].SetMoveToControl(nDirection, NULL);
        }
    }
    m_lblDownarrow.m_cBorder.m_cParams.m_fRotation = 180.0f;
    m_cDescriptionText.Initialize(m_lbDesc.m_pProtoItem->m_cExtent,
                                  (CSWGuiLabel*)m_lbDesc.m_pProtoItem);
    ShowDescription(0);
    CSWGuiPanel* pPanel = this;
    pPanel->SetActiveControl(&m_aSlotButtons[ARMOUR_ITEM_INDEX], 0);
}

CSWGuiInGameEquip::~CSWGuiInGameEquip()
{
    m_lbDesc.ClearItems();
    m_lItems.RemoveAll();
}

void CSWGuiInGameEquip::Draw(float fElapsed)
{
    SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter());
    if (!m_bDescriptionShown)
    {
        UpdateInventory();
    }
    CSWGuiPanel::Draw(fElapsed);
}

void CSWGuiInGameEquip::OnPanelAdded()
{
    SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter());
    m_lblBlack.SetShown(g_pAppManager->m_pClientExoApp->GetSWParty()->GetNumCharacters() > 1);
    OnEnterSlot(m_pActiveControl);
}

void CSWGuiInGameEquip::OnPanelRemoved()
{
    if (m_bDescriptionShown)
    {
        CloseDescription();
    }
    m_lItems.RemoveAll();
}

void CSWGuiInGameEquip::SetCharacter(CSWCCreature* pCreature)
{
    if (pCreature != m_pCreature)
    {
        m_bResetItems = 1;
        m_pCreature = pCreature;
        if (pCreature)
        {
            for (int nSlot = 0; nSlot < NUM_EQUIPMENT_SLOTS; ++nSlot)
            {
                m_aSlotLabels[nSlot].m_cBorder.m_cParams.SetFillImage(
                    GetIcon(nSlot), 0);
            }
        }
        else
        {
            m_lblPortrait.m_cBorder.m_cParams.SetFillImage("", 0);
        }
    }

    if (m_pCreature)
    {
        CResRef* pPortrait = &m_pCreature->GetPortrait(1);
        m_lblPortrait.m_cBorder.m_cParams.SetFillImage(
            *pPortrait, 0);
    }
}

void CSWGuiInGameEquip::SetDescription(const CExoString& sDescription)
{
    m_cDescriptionText.m_cText.m_cParams.SetText(sDescription);
    m_lbDesc.ClearItems();

    CSWGuiExtent cExtent = m_cDescriptionText.m_cExtent;
    if (sDescription.GetLength() > 0)
    {
        cExtent.m_nHeight = m_cDescriptionText.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_cDescriptionText.m_cText.GetFontHeight();
    }

    m_cDescriptionText.SetExtent(cExtent);

    CSWGuiControl* pControl = &m_cDescriptionText;
    m_lbDesc.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDesc.SetSelectedControl(&m_cDescriptionText, 0);
    m_lblRanalog.SetShown(m_lbDesc.IsOversized());
}

void CSWGuiInGameEquip::ShowDescription(int bShow)
{
    if (bShow)
    {
        m_lbDesc.SetShown(1);
        m_lblB.SetShown(1);
        m_lblStart.SetShown(0);
        m_lblBack.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(219));
        for (int i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
        {
            m_aSlotButtons[i].SetShown(0);
            m_aSlotLabels[i].SetShown(0);
        }
        for (int nBorder = 0; nBorder < NUM_DESC_BORDERS; nBorder++)
        {
            m_aDescBorders[nBorder].SetShown(1);
        }
        m_lblAtkl.SetShown(0);
        m_lblAtkr.SetShown(0);
        m_lblDef.SetShown(0);
        m_lblTohitl.SetShown(0);
        m_lblTohitr.SetShown(0);
        m_lblVitality.SetShown(0);
        m_lblAttackInfo.SetShown(0);
        m_lblDefInfo.SetShown(0);
        m_lblTxtbar.SetShown(0);
        m_lblPortBord.SetShown(0);
        m_lblPortrait.SetShown(0);
        m_lblTohit.SetShown(0);
        m_lblDamage.SetShown(0);
        m_lblSlotname.SetShown(0);
        m_lblBlack.SetShown(0);
    }
    else
    {
        m_lbDesc.SetShown(0);
        m_lblB.SetShown(0);
        m_lblRanalog.SetShown(0);
        m_lblStart.SetShown(1);
        m_lblBack.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(36846));
        for (int i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
        {
            m_aSlotButtons[i].SetShown(1);
            m_aSlotLabels[i].SetShown(1);
        }
        for (int nBorder = 0; nBorder < NUM_DESC_BORDERS; nBorder++)
        {
            m_aDescBorders[nBorder].SetShown(0);
        }
        m_lblAtkl.SetShown(1);
        m_lblAtkr.SetShown(1);
        m_lblDef.SetShown(1);
        m_lblTohitl.SetShown(1);
        m_lblTohitr.SetShown(1);
        m_lblVitality.SetShown(1);
        m_lblAttackInfo.SetShown(1);
        m_lblDefInfo.SetShown(1);
        m_lblTxtbar.SetShown(1);
        m_lblPortBord.SetShown(1);
        m_lblPortrait.SetShown(1);
        m_lblTohit.SetShown(1);
        m_lblDamage.SetShown(1);
        m_lblSlotname.SetShown(1);
        m_lblBlack.SetShown(g_pAppManager->GetClientExoApp()->GetSWParty()->GetNumCharacters() > 1);
    }
}

void CSWGuiInGameEquip::OnItemSelected(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
        unsigned long oidItem = ((CSWGuiInGameItemEntry*)pControl)->m_oidItem;
        if (oidItem != OBJECT_INVALID)
        {
            unsigned long oidServer = g_pAppManager->GetServerExoApp()
                ->ClientToServerObjectId(oidItem);
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()
                ->GetItemByGameObjectID(oidServer);
            if (pItem != 0)
            {
                CExoString sDescription;
                sDescription = pItem->GetPropertyDescription();
                if (sDescription != "")
                {
                    SetDescription(sDescription);
                }
                else
                {
                    SetDescription(g_pAppManager->GetClientExoApp()->GetGUIString(
                        STRREF_NO_DESCRIPTION));
                }
            }
            else
            {
                SetDescription(g_pAppManager->GetClientExoApp()->GetGUIString(
                    STRREF_NO_DESCRIPTION));
            }
        }
        else
        {
            SetDescription("");
        }
    }
}

void CSWGuiInGameEquip::CloseDescription()
{
    ShowDescription(0);
    m_bDescriptionShown = 0;
    SetActiveControl(m_pDescribedControl, 0);
    OnEnterSlot(m_pActiveControl);

    m_lblSelecttitle.m_cText.m_cParams.SetText("");
}

void CSWGuiInGameEquip::OnSelectSlot(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
        if (m_pCreature->m_pArea->GetServerArea()->GetPlayerRestrictMode())
        {
            CSWGuiMessageBox* pMessage = g_pAppManager->GetClientExoApp()
                ->GetInGameGui()->m_pMessageBox;
            pMessage->SetAllowCancel(0);
            pMessage->SetCallback(0, 0, 0);
            pMessage->SetMessage(STRREF_ENVIRO_SUIT_RESTRICTION);
            m_pManager->AddPanel(pMessage, SWGUI_PANEL_FLAG_PUSH_MODAL, 1);
            return;
        }
        if (m_nSelectedSlot == INVENTORY_SLOT_ARMOUR &&
            m_pCreature->GetServerCreature()->GetCombatState() == 1 &&
            m_pCreature->GetServerCreature()->GetItemUseState() == 1)
        {
            CSWGuiMessageBox* pMessage = g_pAppManager->GetClientExoApp()
                ->GetInGameGui()->m_pMessageBox;
            pMessage->SetAllowCancel(0);
            pMessage->SetCallback(0, 0, 0);
            int nStrRef = STRREF_ARMOUR_COMBAT_RESTRICTION;
            pMessage->SetMessage(nStrRef);
            CSWGuiManager* pManager = m_pManager;
            if (nStrRef == 0 && pManager == NULL)
                return;
            pManager->AddPanel(pMessage, SWGUI_PANEL_FLAG_PUSH_MODAL, 1);
            return;
        }
        if (m_nSelectedSlot == INVENTORY_SLOT_LEFTHAND &&
            m_aoidEquippedItems[RIGHTHAND_ITEM_INDEX] != OBJECT_INVALID)
        {
            CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
            CSWSItem* pItem = pServer->GetItemByGameObjectID(
                pServer->ClientToServerObjectId(
                    m_aoidEquippedItems[RIGHTHAND_ITEM_INDEX]));
            if (pItem && pItem->GetBaseItem()->m_nWeaponSize ==
                             CSWBaseItem::WEAPON_SIZE_TWO_HANDED)
            {
                CSWGuiMessageBox* pMessage = g_pAppManager->GetClientExoApp()
                    ->GetInGameGui()->m_pMessageBox;
                pMessage->SetAllowCancel(0);
                pMessage->SetCallback(0, 0, 0);
                pMessage->SetMessage(STRREF_TWO_HANDED_RESTRICTION);
                m_pManager->AddPanel(pMessage, SWGUI_PANEL_FLAG_PUSH_MODAL, 1);
                return;
            }
        }
        if (m_lItems.num == 1)
        {
            CSWGuiMessageBox* pMessage = g_pAppManager->GetClientExoApp()
                ->GetInGameGui()->m_pMessageBox;
            pMessage->SetAllowCancel(0);
            pMessage->SetCallback(0, 0, 0);
            pMessage->SetMessage(STRREF_NO_EQUIPPABLE_ITEMS);
            m_pManager->AddPanel(pMessage, SWGUI_PANEL_FLAG_PUSH_MODAL, 1);
            return;
        }
        m_pDescribedControl = pControl;
        ShowDescription(1);
        m_bDescriptionShown = 1;
        m_bResetItems = 1;
        OnEnterSlot(pControl);
        int bEquipped = 0;
        if (m_aoidEquippedItems[(int)pControl->GetUserData()] != OBJECT_INVALID)
        {
            bEquipped = 1;
        }
        SetItemRows(0, bEquipped);
        m_lblSelecttitle.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(
                STRREF_SELECT_ITEM_TO_EQUIP));
        g_pAppManager->GetClientExoApp()->ShowTutorialWindow(
            11, OBJECT_INVALID, OBJECT_INVALID, 0);
    }
}

void CSWGuiInGameEquip::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            if (m_bDescriptionShown)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                CloseDescription();
            }
            else if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui())
            {
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
            }
            break;

        case GUIEVENT_ACCEPT:
            if (!m_bDescriptionShown)
            {
                if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui())
                {
                    g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
                }
            }
            break;

        case 43:
            if (!m_bDescriptionShown)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
                g_pAppManager->m_pClientExoApp
                    ->ChangeCharacterToNextLivingPartyMember(0, 0);
                SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()
                                 ->GetPlayerCharacter());
                OnEnterSlot(m_pActiveControl);
            }

        case GUIEVENT_SCROLL_DOWN:
            if (m_bDescriptionShown)
            {
                m_lbDesc.HandleInputEvent(GUIEVENT_DOWN, 1);
            }
            else
            {
                ScrollItemsDown();
            }
            break;

        case GUIEVENT_SCROLL_UP:
            if (m_bDescriptionShown)
            {
                m_lbDesc.HandleInputEvent(GUIEVENT_UP, 1);
            }
            else
            {
                ScrollItemsUp();
            }
            break;

        case GUIEVENT_DOWN:
        case GUIEVENT_DOWN_ALTERNATE:
            if (m_bDescriptionShown)
            {
                SelectNextItem();
            }
            break;

        case GUIEVENT_UP:
        case GUIEVENT_UP_ALTERNATE:
            if (m_bDescriptionShown)
            {
                SelectPreviousItem();
            }
            break;
        }
    }
    CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

void CSWGuiInGameEquip::SetItemRows(int nTopItem, int nSelectedItem)
{
    int nRows = m_lItems.num > NUM_ITEM_ROWS ? NUM_ITEM_ROWS : m_lItems.num;
    m_nTopItem = nTopItem;
    m_nSelectedItem = nSelectedItem;
    int nRow;
    for (nRow = 0; nRow < nRows; nRow++)
    {
        m_aItemRows[nRow].SetItem(m_lItems[nTopItem + nRow].m_oidItem,
                                  m_lItems[nTopItem + nRow].m_bEquipped, 0);
        m_aItemRows[nRow].SetCanUse(m_lItems[nTopItem + nRow].m_nCanUse);
        m_aItemRows[nRow].SetShown(1);
    }
    for (nRow = nRows; nRow < NUM_ITEM_ROWS; nRow++)
    {
        m_aItemRows[nRow].SetShown(0);
    }
    m_lblUparrow.SetShown(m_nTopItem != 0);
    m_lblDownarrow.SetShown(m_nTopItem + nRows < m_lItems.num);
    if (m_bDescriptionShown && nSelectedItem != -1)
    {
        SetActiveControl(NULL, 0);
        if (m_lItems.num != 0)
        {
            SetActiveControl(&m_aItemRows[nSelectedItem - nTopItem], 1);
        }
    }
}

void CSWGuiInGameEquip::SelectNextItem()
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

void CSWGuiInGameEquip::ScrollItemsDown()
{
    if (m_nTopItem + NUM_ITEM_ROWS < m_lItems.num)
    {
        SetItemRows(m_nTopItem + 1, -1);
    }
}

void CSWGuiInGameEquip::SelectPreviousItem()
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

void CSWGuiInGameEquip::ScrollItemsUp()
{
    if (m_nTopItem > 0)
    {
        SetItemRows(m_nTopItem - 1, -1);
    }
}

void CSWGuiInGameEquip::UpdateInventory()
{

    CSWSItem* pItem = NULL;
    CSWCItem* pClientItem = NULL;
    CResRef resIcon;
    int bDoubleWeapon = 0;
    int bUnarmed = 0;
    unsigned long oidItem;

    m_aSlotLabels[LEFTHAND_ITEM_INDEX].m_cBorder.m_cParams.SetFillImage(
        GetIcon(LEFTHAND_ITEM_INDEX), 0);
    m_aSlotLabels[LEFTHAND_ITEM_INDEX].m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;

    for (int nSlot = 0; nSlot < NUM_EQUIPMENT_SLOTS; ++nSlot)
    {
        int bChanged = 0;
        if (m_pCreature != NULL)
        {
            oidItem = m_pCreature->GetEquippedItemID(SLOT_INFO[nSlot].nSlot);
            pClientItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidItem);
            if (pClientItem != NULL)
                pItem = pClientItem->GetServerItem();
        }

        if (pClientItem != NULL && pItem != NULL)
        {
            m_aoidEquippedItems[nSlot] = oidItem;
            resIcon = pItem->GetIcon();
            int bDouble = 0;
            if (pItem->GetBaseItem()->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON)
                bDouble = 1;
            CSWGuiLabel* pDamage = NULL;
            CSWGuiLabel* pToHit = NULL;
            int bOffHand = 0;
            switch (nSlot)
            {
            case LEFTHAND_ITEM_INDEX:
                pDamage = &m_lblAtkl;
                pToHit = &m_lblTohitl;
                bOffHand = 1;
            case RIGHTHAND_ITEM_INDEX:
                if (bDouble && !bDoubleWeapon)
                {
                    pDamage = &m_lblAtkl;
                    pToHit = &m_lblTohitl;
                    bOffHand = 1;
                    --nSlot;
                    bDoubleWeapon = 1;
                }
                if (pDamage == NULL)
                {
                    pDamage = &m_lblAtkr;
                    pToHit = &m_lblTohitr;
                    bOffHand = 0;
                }
                {
                    CSWSCreature* pCreature = m_pCreature->GetServerCreature();
                    CExoString sDamage;
                    CExoString sToHit;
                    int nMinimum = 0;
                    int nMaximum = 0;
                    int bDamageBonus = 0;
                    pCreature->GetStats()->GetTotalDamageRange(
                        pCreature, pItem, nMinimum, nMaximum, bOffHand, 1);
                    if (nMinimum)
                        sDamage.Format("%d-%d", nMinimum, nMaximum);
                    int nBaseMinimum = 0;
                    int nBaseMaximum = 0;
                    pCreature->GetStats()->GetTotalDamageRange(
                        pCreature, pItem, nBaseMinimum, nBaseMaximum, bOffHand, 0);
                    if (nMinimum > nBaseMinimum || nMaximum > nBaseMaximum)
                        bDamageBonus = 1;
                    if (nSlot == RIGHTHAND_ITEM_INDEX &&
                        (pItem->GetBaseItem()->m_nWeaponSize == CSWBaseItem::WEAPON_SIZE_TWO_HANDED ||
                         pItem->GetBaseItem()->m_nWeaponWield == CSWBaseItem::WIELD_DOUBLE_WEAPON))
                    {
                        m_aSlotLabels[LEFTHAND_ITEM_INDEX].m_cBorder.m_cParams.SetFillImage(resIcon, 0);
                        m_aSlotLabels[LEFTHAND_ITEM_INDEX].m_cBorder.m_cParams.m_fHilightAlpha = 0.25f;
                    }
                    pDamage->m_cText.m_cParams.SetText(sDamage);
                    if (bDamageBonus)
                        pDamage->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_GREEN);
                    else
                        pDamage->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                    int nAttackBonus;
                    if (pItem->GetBaseItem()->m_nRangedWeapon > 0)
                        nAttackBonus = pCreature->GetStats()->GetRangedAttackBonus(bOffHand, 1, 0);
                    else
                        nAttackBonus = pCreature->GetStats()->GetMeleeAttackBonus(bOffHand, 1, 0, bDouble);
                    if (nAttackBonus > 0)
                        sToHit.Format("+%d", nAttackBonus);
                    else
                        sToHit.Format("%d", nAttackBonus);
                    pToHit->m_cText.m_cParams.SetText(sToHit);
                    if (nAttackBonus > pCreature->GetStats()->GetBaseAttackBonus())
                        pToHit->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_GREEN);
                    else
                        pToHit->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                }
                break;
            }
            if (!bDouble || nSlot == RIGHTHAND_ITEM_INDEX)
                m_aSlotLabels[nSlot].m_cBorder.m_cParams.SetFillImage(resIcon, 0);
            bChanged = 1;
        }
        else
        {
            switch (nSlot)
            {
            case RIGHTHAND_ITEM_INDEX:
            {
                bUnarmed = 1;
                bChanged = 1;
                CSWSCreature* pCreature = m_pCreature->GetServerCreature();
                if (pCreature != NULL)
                {
                    int nMinimum = 0;
                    int nMaximum = 0;
                    CExoString sDamage;
                    pCreature->GetStats()->GetTotalDamageRange(
                        pCreature, NULL, nMinimum, nMaximum, 0, 1);
                    if (nMinimum)
                        sDamage.Format("%d-%d", nMinimum, nMaximum);
                    m_lblAtkr.m_cText.m_cParams.SetText(sDamage);
                    CExoString sToHit;
                    int nAttackBonus = pCreature->GetStats()->GetMeleeAttackBonus(0, 1, 0, 0);
                    if (nAttackBonus > 0)
                        sToHit.Format("+%d", nAttackBonus);
                    else
                        sToHit.Format("%d", nAttackBonus);
                    m_lblTohitr.m_cText.m_cParams.SetText(sToHit);
                }
                break;
            }
            }
            if (m_aoidEquippedItems[nSlot] != OBJECT_INVALID || (!bDoubleWeapon && !bUnarmed))
            {
                m_aoidEquippedItems[nSlot] = OBJECT_INVALID;
                switch (nSlot)
                {
                case LEFTHAND_ITEM_INDEX:
                    m_lblAtkl.m_cText.m_cParams.SetText("");
                    m_lblAtkl.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                    m_lblTohitl.m_cText.m_cParams.SetText("");
                    m_lblTohitl.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                    break;
                case RIGHTHAND_ITEM_INDEX:
                    m_lblAtkr.m_cText.m_cParams.SetText("");
                    m_lblAtkr.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                    m_lblTohitr.m_cText.m_cParams.SetText("");
                    m_lblTohitr.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                    break;
                }
                m_aSlotLabels[nSlot].m_cBorder.m_cParams.SetFillImage(GetIcon(nSlot), 0);
                bChanged = 1;
            }
        }
        if (bChanged && m_pActiveControl == &m_aSlotButtons[nSlot])
            OnEnterSlot(m_pActiveControl);
    }

    CExoString sDefense(m_pCreature->GetStats()->GetArmorClass(0));
    m_lblDef.m_cText.m_cParams.SetText(sDefense);
    CExoString sVitality;
    sVitality.Format("%d/%d", m_pCreature->GetStats()->GetCurrentHitPoints(),
                     m_pCreature->GetStats()->GetMaxHitPoints());
    m_lblVitality.m_cText.m_cParams.SetText(sVitality);
    if (m_lblVitality.m_cText.GetIdealHeight() > m_lblVitality.m_cText.GetFontHeight())
    {
        sVitality.Format("%d/ %d", m_pCreature->GetStats()->GetCurrentHitPoints(),
                         m_pCreature->GetStats()->GetMaxHitPoints());
        m_lblVitality.m_cText.m_cParams.SetText(sVitality);
    }
}

void CSWGuiInGameEquip::OnEnterSlot(CSWGuiControl* pControl)
{
    if (m_pCreature != NULL)
    {
        int nSlotIndex = pControl->m_nIndex;
        m_nSelectedSlot = SLOT_INFO[nSlotIndex].nSlot;
        if (m_nSelectedSlot != -1)
        {
            CSWSCreature* pCreature = m_pCreature->GetServerCreature();
            CItemRepository* pRepository = pCreature->GetItemRepository(1);
            CExoArrayList<ItemListEntry> lItems;
            ItemListEntry cEntry;
            cEntry.m_bEquipped = 0;
            cEntry.m_oidItem = OBJECT_INVALID;
            cEntry.m_nCanUse = CSWGuiInGameItemEntry::CAN_USE;
            lItems.Add(cEntry);
            if (m_aoidEquippedItems[nSlotIndex] != OBJECT_INVALID)
            {
                cEntry.m_oidItem = m_aoidEquippedItems[nSlotIndex];
                cEntry.m_nCanUse = CSWGuiInGameItemEntry::CAN_USE_EQUIPPED;
                cEntry.m_bEquipped = 1;
                lItems.Add(cEntry);
            }
            int bShowItems = 1;
            if (m_nSelectedSlot == INVENTORY_SLOT_LEFTHAND &&
                m_aoidEquippedItems[RIGHTHAND_ITEM_INDEX] != OBJECT_INVALID)
            {
                CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
                CSWSItem* pItem = pServer->GetItemByGameObjectID(
                    pServer->ClientToServerObjectId(m_aoidEquippedItems[RIGHTHAND_ITEM_INDEX]));
                if (pItem != NULL)
                {
                    bShowItems = pItem->GetBaseItem()->GetWeaponSize() !=
                                 CSWBaseItem::WEAPON_SIZE_TWO_HANDED;
                }
            }
            if (bShowItems)
            {
                for (unsigned long nItem = 0;
                     nItem < (unsigned long)pRepository->GetNumItems(); ++nItem)
                {
                    CSWSItem* pItem = pRepository->ItemListGetItem(nItem);
                    if (pItem == NULL)
                        continue;
                    int nSelectedSlot = m_nSelectedSlot;
                    if (!(nSelectedSlot & pItem->GetBaseItem()->GetEquipableSlots()))
                        continue;
                    unsigned char nDroidOrHuman = pItem->GetBaseItem()->m_nDroidOrHuman;
                    if (nDroidOrHuman != CSWBaseItem::DROID_OR_HUMAN_ANY)
                    switch (nDroidOrHuman)
                    {
                    case CSWBaseItem::DROID_OR_HUMAN_HUMAN:
                    {
                        unsigned short nRace = pCreature->GetStats()->m_nRace;
                        if (nRace != CSWRace::CRACE_HUMAN)
                            continue;
                        break;
                    }
                    case CSWBaseItem::DROID_OR_HUMAN_DROID:
                    {
                        unsigned short nRace = pCreature->GetStats()->m_nRace;
                        if (nRace == CSWRace::CRACE_DROID)
                            break;
                        continue;
                    }
                    }
                    unsigned char nCanUse = CSWGuiInGameItemEntry::CAN_USE;
                    unsigned long nSlot = m_nSelectedSlot;
                    if (!pCreature->CanEquipItem(pItem, &nSlot, 0, 0, 0))
                    {
                        nCanUse = CSWGuiInGameItemEntry::CANNOT_USE;
                    }
                    int bProficient = 1;
                    CClientOptions* pOptions =
                        g_pAppManager->GetClientExoApp()->GetClientOptions();
                    if (pOptions != NULL && pOptions->m_bHideUnequippable)
                    {
                        if (!pCreature->CheckProficiencies(pItem, m_nSelectedSlot))
                            bProficient = 0;
                        nSlot = m_nSelectedSlot;
                        if (!pCreature->CanEquipItem(pItem, &nSlot, 0, 0, 0) ||
                            !bProficient)
                            continue;
                    }
                    CSWCItem* pClientItem = pItem->GetClientItem();
                    if (pClientItem == NULL)
                        continue;
                    int nHandSlot = this > NULL ? m_nSelectedSlot : m_nSelectedSlot;
                    if (nHandSlot == INVENTORY_SLOT_RIGHTHAND ||
                        nHandSlot == INVENTORY_SLOT_LEFTHAND)
                    {
                        CSWCItem* pRight;
                        CSWCItem* pLeft;
                        if (nHandSlot == INVENTORY_SLOT_RIGHTHAND)
                        {
                            pRight = pClientItem;
                            pLeft = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                                m_pCreature->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND));
                        }
                        else
                        {
                            pRight = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                                m_pCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
                            pLeft = pClientItem;
                        }
                        if (pRight != NULL && pLeft != NULL &&
                            (pRight != pClientItem || pItem->GetBaseItem()->m_nWeaponSize !=
                                                       CSWBaseItem::WEAPON_SIZE_TWO_HANDED))
                        {
                            CSWBaseItem* pRightBase = pRight->GetBaseItem();
                            int nWield = (unsigned char)pRightBase->m_nWeaponWield;
                            int nLeftW = (unsigned char)pLeft->GetBaseItem()->m_nWeaponWield;
                            if (nWield != nLeftW ||
                                (nWield != CSWBaseItem::WEAPON_WIELD_ONE_HANDED_2 &&
                                 nWield != CSWBaseItem::WEAPON_WIELD_ONE_HANDED_4))
                            {
                                nCanUse = CSWGuiInGameItemEntry::CANNOT_USE_WITH_OTHER_HAND;
                            }
                        }
                    }
                    cEntry.m_oidItem = pClientItem->GetId();
                    cEntry.m_nCanUse = nCanUse;
                    cEntry.m_bEquipped = 0;
                    lItems.Add(cEntry);
                }
            }
            int bChanged = 0;
            if (lItems.num != m_lItems.num)
            {
                bChanged = 1;
                m_lItems.num = 0;
                for (unsigned long nEntry = 0; nEntry < (unsigned long)lItems.num; nEntry++)
                {
                    m_lItems.Add(lItems[nEntry]);
                }
            }
            else
            {
                for (unsigned long nEntry = 0; nEntry < (unsigned long)lItems.num; nEntry++)
                {
                    if (lItems[nEntry].m_oidItem != m_lItems[nEntry].m_oidItem)
                    {
                        bChanged = 1;
                    }
                    m_lItems[nEntry] = lItems[nEntry];
                }
            }
            if (bChanged || m_bResetItems)
            {
                SetItemRows(0, -1);
            }
            m_bResetItems = 0;
            int bItems = m_lItems.num > 1;
            m_lblEquip.m_cText.m_cParams.SetColor(
                bItems ? CGuiInGame::COLOR_BLUE : CGuiInGame::DISABLED_TEXT);
            unsigned long nStrRef;
            if (m_pCreature->GetStats()->m_nRace == CSWRace::CRACE_DROID)
                nStrRef = SLOT_INFO[nSlotIndex].nDescriptionStrRef;
            else
                nStrRef = SLOT_INFO[nSlotIndex].nNameStrRef;
            m_lblSlotname.m_cText.m_cParams.SetText(
                g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef));
        }
        else
        {
            SetItemRows(0, 0);
            CExoString sEmpty("");
            m_lblSlotname.m_cText.m_cParams.SetText(sEmpty);
        }
    }
}

void CSWGuiInGameItemEntry::RefreshItem()
{
    SetItem(m_oidItem, m_bEquipped, m_bUnavailable);
}
#endif

#ifdef _XBOX

void CSWGuiInGameEquip::OnOKPressed(CSWGuiControl* pControl)
{
    CSWGuiInGameItemEntry* pEntry = (CSWGuiInGameItemEntry*)pControl;
    if (pEntry->m_bSelected)
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        unsigned long oidItem = pEntry->GetItem();
        unsigned long oidEquipped = m_pCreature->GetEquippedItemID(m_nSelectedSlot);
        unsigned char nCanUse = pEntry->GetCanUse();
        switch (nCanUse)
        {
        case CSWGuiInGameItemEntry::CAN_USE:
        case CSWGuiInGameItemEntry::CAN_USE_EQUIPPED:
            if (oidItem != oidEquipped)
            {
                if (oidItem != OBJECT_INVALID)
                {
                    if (m_nSelectedSlot == INVENTORY_SLOT_RIGHTHAND)
                    {
                        CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
                        CSWSItem* pItem = pServer->GetItemByGameObjectID(pServer->ClientToServerObjectId(oidItem));
                        if (pItem != NULL &&
                            pItem->GetBaseItem()->m_nWeaponSize == CSWBaseItem::WEAPON_SIZE_TWO_HANDED &&
                            m_aoidEquippedItems[LEFTHAND_ITEM_INDEX] != OBJECT_INVALID)
                        {
                            g_pAppManager->GetClientExoApp()->GetSWCMessage()->SendPlayerToServerInventory_Unequip(
                                m_aoidEquippedItems[LEFTHAND_ITEM_INDEX], OBJECT_INVALID);
                        }
                    }
                    m_pManager->PlayGuiSound(SWGUI_SOUND_EQUIP_ITEM);
                    g_pAppManager->GetClientExoApp()->GetSWCMessage()->SendPlayerToServerInventory_Equip(
                        oidItem, m_nSelectedSlot);
                }
                else
                {
                    m_pManager->PlayGuiSound(SWGUI_SOUND_UNEQUIP_ITEM);
                    g_pAppManager->GetClientExoApp()->GetSWCMessage()->SendPlayerToServerInventory_Unequip(
                        oidEquipped, OBJECT_INVALID);
                }
            }
            CloseDescription();
            break;
        case CSWGuiInGameItemEntry::CANNOT_USE:
        case CSWGuiInGameItemEntry::CANNOT_USE_WITH_OTHER_HAND:
        {
            CSWGuiMessageBox* pMessage = g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
            pMessage->SetAllowCancel(0);
            pMessage->SetMessage(nCanUse == CSWGuiInGameItemEntry::CANNOT_USE ? STRREF_ITEM_PREREQUISITES
                                                                                : STRREF_INCOMPATIBLE_WEAPON);
            pMessage->SetCallback(0, 0, 0);
            m_pManager->AddPanel(pMessage, SWGUI_PANEL_FLAG_PUSH_MODAL, 1);
            break;
        }
        }
    }
}
#endif
