#include "exoinput.h"
#include "exoinputinternal.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "exotlktable.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "nwsobject.h"
#include "swparty.h"
#include "guiingame.h"
#include "swguiinOPTKeyMapping.h"

const int CSWGuiInGameOptKeyMappings::FILTER_MOVE = 0;
const int CSWGuiInGameOptKeyMappings::FILTER_GAME = 1;
const int CSWGuiInGameOptKeyMappings::FILTER_MINI = 2;
const int CSWGuiKeyMapButton::KEY_GOOD = 0;
const int CSWGuiKeyMapButton::KEY_ALREADYUSED = 1;
const int CSWGuiKeyMapButton::KEY_INVALID = 2;
const int CSWGuiKeyMapButton::IC_PC = 0x01;
const int CSWGuiKeyMapButton::IC_MINIGAME = 0x02;
const int CSWGuiKeyMapButton::IC_GUI = 0x04;
const int CSWGuiKeyMapButton::IC_DIALOG = 0x08;
const int CSWGuiKeyMapButton::IC_FREELOOK = 0x10;
const int CSWGuiKeyMapButton::IC_MOVIE = 0x20;
// PC 0x0075E110
CExoString CSWGuiKeyMapButton::GetActionId(int nAction, int nType)
{
    CExoString sAction;

    if (nType == 0)
    {
        CExoString sNumber(nAction);
        sAction = CExoString("Action") + sNumber;
    }
    else if (nType == 1)
    {
        sAction.Format("%s%dB", "Action", nAction);
    }
    else if (nType == 2)
    {
        sAction.Format("%s%dA", "Action", nAction);
    }
    return sAction;
}
// PC 0x0075EA20
void CSWGuiKeyMapButton::SetExtent(const CSWGuiExtent& cExtent)
{
    CSWGuiExtent cLabelExtent = cExtent;
    cLabelExtent.m_nWidth -= 197;
    m_cExtent = cLabelExtent;
    m_cBorder.SetExtent(cLabelExtent);
    m_cHilite.SetExtent(cLabelExtent);
    m_cText.SetExtent(CSWGuiExtent::Intersection(
        m_cBorder.GetInnerExtent(), m_cHilite.GetInnerExtent()));

    CSWGuiExtent cKeyExtent = cExtent;
    cKeyExtent.m_nLeft = cLabelExtent.m_nLeft + cLabelExtent.m_nWidth + 2;
    cKeyExtent.m_nWidth = 195;
    m_btnKey.SetExtent(cKeyExtent);
}
// PC 0x0075EB20
void CSWGuiKeyMapButton::Draw(float fElapsed)
{
    CSWGuiButton::Draw(fElapsed);
    m_btnKey.Draw(fElapsed);
}
// PC 0x0075EB50
CSWGuiControl* CSWGuiKeyMapButton::HitCheckMouse(int nX, int nY)
{
    CSWGuiControl* nResult = CSWGuiButton::HitCheckMouse(nX, nY);
    if (nResult == 0)
    {
        return m_btnKey.HitCheckMouse(nX, nY);
    }
    return nResult;
}
// PC 0x0075DEE0
void CSWGuiInGameOptKeyMappings::SetCaptureKeyboardKeys(int bCapture)
{
    m_bCapturing = bCapture;
    g_pExoInput->SetCaptureKeyboardKeys(bCapture);
}
// PC 0x0075F920
void CSWGuiInGameOptKeyMappings::FillListWithEvents()
{
    CSWGuiButton* pProto;
    m_aOptions.num = 0;
    CSWGuiListBox* pListBox = &m_lstEvents;
    pProto = (CSWGuiButton*)pListBox->m_pProtoItem;
    CSWGuiKeyMapButton* pOption;
    int nAction;

    for (nAction = 0; nAction < 7; nAction++)
    {
        pOption = CreateOption(nAction + 280, pProto, 2);
        if (pOption)
        {
            m_aOptions.Add(pOption);
        }
        pOption = CreateOption(nAction + 280, pProto, 1);
        if (pOption)
        {
            m_aOptions.Add(pOption);
        }
    }

    for (nAction = 0; nAction < 70; nAction++)
    {
        pOption = CreateOption(nAction + 200, pProto, 0);
        if (pOption)
        {
            m_aOptions.Add(pOption);
        }
    }
}

void CSWGuiKeyMapButton::SetEditing(int bEditing)
{
    if (bEditing)
    {
        m_btnKey.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
    }
    else
    {
        UpdateColor();
    }
    m_btnKey.m_cBorder.m_cParams.SetPulsingAlpha(bEditing, bEditing, bEditing);
}
// PC 0x005EE1C0
void CSWGuiKeyMapButton::onControlExit(CSWGuiControl* pControl)
{
}
// PC 0x0075F5B0
int CSWGuiInGameOptKeyMappings::UpdateEvent(int nAction, int nType,
                                            unsigned long nKey,
                                            unsigned long nFilterFlags)
{
    CExoString sKey;
    sKey.Format("KEY%d", nKey - CExoInput::KEYBOARD_START);
    int nBindable = 0;
    bool bBindable;
    {
        CExoString sColumn("Bindable");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pBindableKey;
        bool bKeyBindable;
        bBindable = pTable->GetINTEntry(sKey, sColumn, &nBindable) &&
                    (bKeyBindable = nBindable != 0);
    }
    if (bBindable)
    {
        if (IsKeyUsedAlready(nKey, nAction, nType, nFilterFlags))
        {
            return CSWGuiKeyMapButton::KEY_ALREADYUSED;
        }
        return CSWGuiKeyMapButton::KEY_GOOD;
    }
    return CSWGuiKeyMapButton::KEY_INVALID;
}
// PC 0x0075F860
int CSWGuiKeyMapButton::UpdateEvent(unsigned long nKey)
{
    unsigned long nFilterFlags = m_nFilterFlags;
    int nResult =
        m_pParent->UpdateEvent(m_nAction, m_nType, nKey, nFilterFlags);
    if (nResult == KEY_GOOD)
    {
        m_bChanged = m_nOriginalKey != (int)nKey;
        m_nNewKey = nKey;
        SetDescription(GetActionId(m_nAction, m_nType));
        UpdateColor();
    }
    return nResult;
}
// PC 0x0075FB00
CSWGuiInGameOptKeyMappings::CSWGuiInGameOptKeyMappings(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager),
      m_bCapturing(0)
{
    g_pExoInput->SetCaptureKeyboardKeys(0);
    m_nFilter = FILTER_MOVE;
    m_pCaptureButton = 0;

    StartLoadFromLayout("OPTKeyMapping");
    InitControl(m_lblTitle, "LBL_Title", 1);
    InitControl(m_btnDefault, "BTN_Default", 1);
    InitControl(m_btnAccept, "BTN_Accept", 1);
    InitControl(m_btnCancel, "BTN_Cancel", 1);
    InitControl(m_btnFilterMove, "BTN_Filter_Move", 1);
    InitControl(m_btnFilterGame, "BTN_Filter_Game", 1);
    InitControl(m_btnFilterMini, "BTN_Filter_Mini", 1);
    InitControl(m_lstEvents, "LST_EventList", 1);
    StopLoadFromLayout();

    m_btnFilterMove.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
    FillListWithEvents();
    FillListBox(m_nFilter);

    m_btnDefault.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                         (CSWGuiHandler)&CSWGuiInGameOptKeyMappings::OnDefaultClick);
    m_btnAccept.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                        (CSWGuiHandler)&CSWGuiInGameOptKeyMappings::OnAcceptClick);
    m_btnCancel.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                        (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);

    m_btnFilterMove.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                            (CSWGuiHandler)&CSWGuiInGameOptKeyMappings::OnFilterMove);
    m_btnFilterMove.AddEvent(CSWGuiControl::EVENT_EXIT, this,
                            (CSWGuiHandler)&CSWGuiInGameOptKeyMappings::FixFilterColors);
    m_btnFilterMove.SetFocusable(0);
    m_btnFilterGame.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                            (CSWGuiHandler)&CSWGuiInGameOptKeyMappings::OnFilterGame);
    m_btnFilterGame.AddEvent(CSWGuiControl::EVENT_EXIT, this,
                            (CSWGuiHandler)&CSWGuiInGameOptKeyMappings::FixFilterColors);
    m_btnFilterGame.SetFocusable(0);
    m_btnFilterMini.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                            (CSWGuiHandler)&CSWGuiInGameOptKeyMappings::OnFilterMini);
    m_btnFilterMini.AddEvent(CSWGuiControl::EVENT_EXIT, this,
                            (CSWGuiHandler)&CSWGuiInGameOptKeyMappings::FixFilterColors);
    m_btnFilterMini.SetFocusable(0);

    SetActiveControl(&m_lstEvents, 0);
}
// PC 0x0075FA70
void CSWGuiInGameOptKeyMappings::Update(float fElapsed)
{
    (void)fElapsed;
    if (m_bCapturing == 1)
    {
        int nKey = g_pExoInput->GetLastCapturedKeyboardKey();
        if (nKey != 0 && m_pCaptureButton != 0)
        {
            if (nKey == CExoInput::KEYBOARD_ESC)
            {
                SetCaptureEvent(0);
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            }
            else
            {
                switch (m_pCaptureButton->UpdateEvent(nKey))
                {
                case CSWGuiKeyMapButton::KEY_GOOD:
                    SetCaptureKeyboardKeys(0);
                    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                    SetCaptureEvent(0);
                    break;
                case CSWGuiKeyMapButton::KEY_ALREADYUSED:
                    SetCaptureKeyboardKeys(1);
                    break;
                case CSWGuiKeyMapButton::KEY_INVALID:
                    SetCaptureKeyboardKeys(1);
                    break;
                }
            }
        }
    }
}
// PC 0x0075EB80
CSWGuiInGameOptKeyMappings::~CSWGuiInGameOptKeyMappings()
{
    m_lstEvents.ClearItems();
    while (m_aOptions.num != 0)
    {
        int nCount = m_aOptions.num;
        int nIndex = nCount - 1;
        delete m_aOptions.element[nIndex];
        m_aOptions.element[nIndex] = 0;
        m_aOptions.DelIndex(nIndex);
    }
}
// PC 0x0075DF00
void CSWGuiInGameOptKeyMappings::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0 && (nEvent == 40 || nEvent == 46))
    {
        m_bCapturing = 0;
        g_pExoInput->SetCaptureKeyboardKeys(0);
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        m_pManager->PopModalPanel();
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        return;
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0075DF70
void CSWGuiInGameOptKeyMappings::FixFilterColors(CSWGuiControl* pControl)
{
    switch (m_nFilter)
    {
    case FILTER_MOVE:
        m_btnFilterMove.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
        if (pControl == 0)
        {
            m_btnFilterGame.m_cText.m_cParams.SetColor(CGuiInGame::STANDARD_BLUE);
            m_btnFilterMini.m_cText.m_cParams.SetColor(CGuiInGame::STANDARD_BLUE);
        }
        break;

    case FILTER_GAME:
        m_btnFilterGame.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
        if (pControl == 0)
        {
            m_btnFilterMove.m_cText.m_cParams.SetColor(CGuiInGame::STANDARD_BLUE);
            m_btnFilterMini.m_cText.m_cParams.SetColor(CGuiInGame::STANDARD_BLUE);
        }
        break;

    case FILTER_MINI:
        m_btnFilterMini.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
        if (pControl == 0)
        {
            m_btnFilterMove.m_cText.m_cParams.SetColor(CGuiInGame::STANDARD_BLUE);
            m_btnFilterGame.m_cText.m_cParams.SetColor(CGuiInGame::STANDARD_BLUE);
        }
        break;
    }
}
// PC 0x0075F3A0
void CSWGuiInGameOptKeyMappings::FillListBox(int nFilter)
{
    CExoArrayList<CSWGuiControl*> aControls;

    for (int i = 0; i < m_aOptions.num; i++)
    {
        CSWGuiKeyMapButton* pOption = m_aOptions[i];
        if (pOption->m_nPage == nFilter)
        {
            int nSortPos = pOption->m_nSortPos;
            int nInsert = -1;
            for (int j = 0; j < aControls.num; j++)
            {
                nInsert = j + 1;
                if (((CSWGuiKeyMapButton*)aControls[j])->m_nSortPos > nSortPos)
                {
                    nInsert = j;
                    break;
                }
            }
            if (nInsert == -1)
            {
                aControls.Add(pOption);
            }
            else
            {
                aControls.Insert(pOption, nInsert);
            }
        }
    }
    CSWGuiListBox* pListBox = &m_lstEvents;
    pListBox->ClearItems();
    pListBox->AddControls(aControls, 0, 1, 0);
    pListBox->m_bLooping = 0;
}
// PC 0x0075ECE0
int CSWGuiInGameOptKeyMappings::IsKeyUsedAlready(unsigned long nKey,
                                                 int nAction, int nType,
                                                 unsigned long nFilterFlags)
{
    for (int i = 0; i < m_aOptions.num; i++)
    {
        CSWGuiKeyMapButton* pOption = m_aOptions[i];
        if ((pOption->m_nAction != nAction || pOption->m_nType != nType) &&
            (pOption->m_nFilterFlags & nFilterFlags) != 0 &&
            (unsigned long)(pOption->m_bChanged
                                ? pOption->m_nNewKey
                                : pOption->m_nOriginalKey) == nKey)
        {
            pOption->m_bChanged = 1;
            pOption->m_nNewKey = 0;
            int nControlType = pOption->m_nType;
            pOption->SetDescription(pOption->GetActionId(pOption->m_nAction, nControlType));
            pOption->UpdateColor();
        }
    }
    return 0;
}
// PC 0x0075EDD0
int CSWGuiInGameOptKeyMappings::OnDefaultClick(CSWGuiControl* pControl)
{
    int nResult;
    int i;
    CSWGuiKeyMapButton* pOption;

    nResult = m_aOptions.num;
    for (i = 0; i < nResult; i++)
    {
        pOption = m_aOptions[i];
        if (pOption->m_bUnchangable == 0)
        {
            pOption->ResetToDefault();
        }
        nResult = m_aOptions.num;
    }
    return nResult;
}
// PC 0x0075EE10
void CSWGuiInGameOptKeyMappings::OnAcceptClick(CSWGuiControl* pControl)
{
    for (int i = 0; i < m_aOptions.num; i++)
    {
        CSWGuiKeyMapButton* pOption = m_aOptions[i];
        if (pOption->m_bUnchangable == 0 && pOption->m_bChanged != 0)
        {
            pOption->PostChangeToEvent();
        }
    }
    m_bCapturing = 0;
    g_pExoInput->SetCaptureKeyboardKeys(0);
    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    m_pManager->PopModalPanel();
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
}
// PC 0x0075F700
void CSWGuiInGameOptKeyMappings::OnFilterMove(CSWGuiControl* pControl)
{
    m_nFilter = 0;
    FixFilterColors(0);
    FillListBox(m_nFilter);
}
// PC 0x0075F750
void CSWGuiInGameOptKeyMappings::OnFilterGame(CSWGuiControl* pControl)
{
    m_nFilter = 1;
    FixFilterColors(0);
    FillListBox(m_nFilter);
}
// PC 0x0075F7A0
void CSWGuiInGameOptKeyMappings::OnFilterMini(CSWGuiControl* pControl)
{
    m_nFilter = 2;
    FixFilterColors(0);
    FillListBox(m_nFilter);
}
// PC 0x0075EE90
void CSWGuiInGameOptKeyMappings::SetCaptureEvent(CSWGuiKeyMapButton* pButton)
{
    if (pButton != m_pCaptureButton)
    {
        if (m_pCaptureButton != 0)
        {
            m_pCaptureButton->SetEditing(0);
        }
        m_lstEvents.SetSelectOnLeftClick(0);
        SetCaptureKeyboardKeys(0);
    }

    if (pButton != 0)
    {
        m_pCaptureButton = pButton;
        pButton->SetEditing(1);
        m_lstEvents.SetSelectOnLeftClick(1);
        SetCaptureKeyboardKeys(1);
    }
}
// PC 0x0075EF60
CSWGuiKeyMapButton::CSWGuiKeyMapButton(CSWGuiInGameOptKeyMappings* pParent)
{
    m_pParent = pParent;
    m_nAction = 0;
    m_bChanged = 0;
    m_nNewKey = 0;
    m_nOriginalKey = -1;
    m_bUnchangable = 0;
    m_nPage = 0;
    m_nFilterFlags = 0;
    m_nSortPos = -1;
    m_btnKey.m_pOwner = pParent;
#ifndef _XBOX
    m_btnKey.m_pParent = this;
#endif
    AddChildControl(m_btnKey.AsButton());
}
// PC 0x0075E020
void CSWGuiKeyMapButton::UpdateColor()
{
    if (m_bChanged)
    {
        m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_PURPLE;
        m_btnKey.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_PURPLE;
    }
    else
    {
        m_cBorder.m_cParams.m_vColor = CGuiInGame::STANDARD_BLUE;
        m_btnKey.m_cBorder.m_cParams.m_vColor = CGuiInGame::STANDARD_BLUE;
    }
}
// PC 0x0075F010
void CSWGuiKeyMapButton::onControlEntered(CSWGuiControl* pControl)
{
    (void)pControl;
    m_pParent->SetCaptureEvent(0);
}
// PC 0x0075F030
void CSWGuiKeyMapButton::onEventClicked(CSWGuiControl* pControl)
{
    if (m_bUnchangable == 0 && pControl->m_bSelected != 0)
    {
        m_pParent->SetCaptureEvent(this);
    }
}

void CSWGuiKeyMapButton::SetEvents()
{
    AddEvent(CSWGuiControl::EVENT_SELECT, this,
             (CSWGuiHandler)&CSWGuiKeyMapButton::onEventClicked);
    AddEvent(CSWGuiControl::EVENT_ENTER, this,
             (CSWGuiHandler)&CSWGuiKeyMapButton::onControlEntered);
    AddEvent(CSWGuiControl::EVENT_EXIT, this,
             (CSWGuiHandler)&CSWGuiKeyMapButton::onControlExit);
}
// PC 0x0075F080
CSWGuiKeyMapButton* CSWGuiInGameOptKeyMappings::CreateOption(
    int nAction, CSWGuiButton* pParent, int nType)
{
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = cExtent.m_nTop = cExtent.m_nWidth = cExtent.m_nHeight = 0;
    CSWGuiKeyMapButton* pOption = new CSWGuiKeyMapButton(this);
    pOption->m_nAction = nAction;
    pOption->m_nType = nType;
    pOption->Initialize(cExtent, pParent);
    pOption->m_btnKey.Initialize(cExtent, pParent);
    pOption->m_btnKey.m_cText.m_cParams.SetText(CExoString(nAction));

    CExoString sAction = pOption->GetActionId(nAction, nType);
    int nReMappable;
    int nForceDisplay;
    int nDisabled = 0;
    int nSortPos = -1;
    {
        CExoString sColumn("ReMappable");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pKeymap;
        pTable->GetINTEntry(sAction, sColumn, &nReMappable);
    }
    {
        CExoString sColumn("ForceDisplay");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pKeymap;
        pTable->GetINTEntry(sAction, sColumn, &nForceDisplay);
    }
    {
        CExoString sColumn("Disabled");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pKeymap;
        pTable->GetINTEntry(sAction, sColumn, &nDisabled);
    }
    {
        CExoString sColumn("SortPos");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pKeymap;
        pTable->GetINTEntry(sAction, sColumn, &nSortPos);
    }
    pOption->m_nSortPos = nSortPos;
    if ((!nReMappable && !nForceDisplay) || nDisabled == 1)
    {
        delete pOption;
        return 0;
    }

    pOption->SetDescription(sAction);
    int nBorderDim = pOption->m_cHilite.m_cParams.GetBorderDim() > pOption->m_cBorder.m_cParams.GetBorderDim()
        ? pOption->m_cHilite.m_cParams.GetBorderDim() : pOption->m_cBorder.m_cParams.GetBorderDim();
    cExtent.m_nHeight = 2 * (pOption->m_cText.GetIdealHeight() + nBorderDim);
    pOption->SetExtent(cExtent);
    pOption->SetEvents();
    if (nForceDisplay)
    {
        pOption->SetUnchangable();
    }
    return pOption;
}
// PC 0x0075E0B0
inline void CSWGuiKeyMapButton::SetUnchangable()
{
    m_bUnchangable = 1;
    m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_ICON_COLOR;
    m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
    m_cHilite.m_cParams.m_vColor = CGuiInGame::DISABLED_ICON_COLOR;
}
// PC 0x0075E240
void CSWGuiKeyMapButton::SetDescription(const CExoString& sDescription)
{
    int nActionStrRef;
    {
        CExoString sColumn("ActionStrRef");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pKeymap;
        pTable->GetINTEntry(sDescription, sColumn, &nActionStrRef);
    }
    {
        CExoString sColumn("DescStrRef");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pKeymap;
        pTable->GetINTEntry(sDescription, sColumn, &m_nDescStrRef);
    }
#ifndef _XBOX
    m_nToolTipStrRef = m_nDescStrRef;
#endif
#ifndef _XBOX
    m_btnKey.m_nToolTipStrRef = m_nDescStrRef;
#endif
    {
        CExoString sColumn("Page");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pTable = p2DArrays->m_pKeymap;
        pTable->GetINTEntry(sDescription, sColumn, &m_nPage);
    }

    int nInputClass = 0;
    m_nFilterFlags = 0;
    bool bInputClass;
    {
        CTwoDimArrays* p2DArrays;
        C2DA* pTable;
        bInputClass = (pTable = (p2DArrays = g_pRules->m_p2DArrays)->m_pKeymap)->GetINTEntry(
            sDescription, "ICPC", &nInputClass) && nInputClass == 1;
    }
    if (bInputClass)
    {
        m_nFilterFlags |= IC_PC;
    }
    {
        CTwoDimArrays* p2DArrays;
        C2DA* pTable;
        bInputClass = (pTable = (p2DArrays = g_pRules->m_p2DArrays)->m_pKeymap)->GetINTEntry(
            sDescription, "ICMiniGame", &nInputClass) && nInputClass == 1;
    }
    if (bInputClass)
    {
        m_nFilterFlags |= IC_MINIGAME;
    }
    {
        CTwoDimArrays* p2DArrays;
        C2DA* pTable;
        bInputClass = (pTable = (p2DArrays = g_pRules->m_p2DArrays)->m_pKeymap)->GetINTEntry(
            sDescription, "ICPCGUI", &nInputClass) && nInputClass == 1;
    }
    if (bInputClass)
    {
        m_nFilterFlags |= IC_GUI;
    }
    {
        CTwoDimArrays* p2DArrays;
        C2DA* pTable;
        bInputClass = (pTable = (p2DArrays = g_pRules->m_p2DArrays)->m_pKeymap)->GetINTEntry(
            sDescription, "ICDialog", &nInputClass) && nInputClass == 1;
    }
    if (bInputClass)
    {
        m_nFilterFlags |= IC_DIALOG;
    }
    {
        CTwoDimArrays* p2DArrays;
        C2DA* pTable;
        bInputClass = (pTable = (p2DArrays = g_pRules->m_p2DArrays)->m_pKeymap)->GetINTEntry(
            sDescription, "ICFreeLook", &nInputClass) && nInputClass == 1;
    }
    if (bInputClass)
    {
        m_nFilterFlags |= IC_FREELOOK;
    }
    {
        CTwoDimArrays* p2DArrays;
        C2DA* pTable;
        bInputClass = (pTable = (p2DArrays = g_pRules->m_p2DArrays)->m_pKeymap)->GetINTEntry(
            sDescription, "ICMovie", &nInputClass) && nInputClass == 1;
    }
    if (bInputClass)
    {
        m_nFilterFlags |= IC_MOVIE;
    }

    CExoString sText = g_pTlkTable->GetSimpleString(nActionStrRef);
    m_cText.m_cParams.SetText(sText);
    sText = g_pTlkTable->GetSimpleString(STRREF_NONE_SET);
    CExoInputEventDesc* pEvent = g_pExoInput->GetEvent(
        m_nAction, CExoInput::EVENTCLASS_ANY);
    if (pEvent != 0)
    {
        if (pEvent->Is2Button() && m_nType > CONTROL_PRIMARY)
        {
            int nKey = ((CExoInputeventDesc2ButtonAxis*)pEvent)->m_nControl2;
            if (m_bChanged)
            {
                nKey = m_nNewKey;
            }
            else
            {
                m_nOriginalKey = nKey;
            }
            if (nKey != 0)
            {
                sText = g_pAppManager->GetClientExoApp()->GetDisplayCharacterForKey(nKey);
            }
        }
        else
        {
            int nKey = pEvent->m_nControl;
            if (m_bChanged)
            {
                nKey = m_nNewKey;
            }
            else
            {
                m_nOriginalKey = nKey;
            }
            if (nKey != 0)
            {
                sText = g_pAppManager->GetClientExoApp()->GetDisplayCharacterForKey(nKey);
            }
        }
    }
    m_btnKey.m_cText.m_cParams.SetText(sText);
}
// PC 0x0075E730
void CSWGuiKeyMapButton::ResetToDefault()
{
    int nDefaultKey;
    CTwoDimArrays* p2DArrays;
    C2DA* pTable;
    if (!(pTable = (p2DArrays = g_pRules->m_p2DArrays)->m_pKeymap)->GetINTEntry(
            GetActionId(m_nAction, m_nType), "Language0", &nDefaultKey))
    {
        return;
    }
    m_bChanged = m_nOriginalKey != nDefaultKey;
    m_nNewKey = nDefaultKey;
    SetDescription(GetActionId(m_nAction, m_nType));
    if (m_bChanged)
    {
        m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_PURPLE;
        m_btnKey.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_PURPLE;
    }
    else
    {
        m_cBorder.m_cParams.m_vColor = CGuiInGame::STANDARD_BLUE;
        m_btnKey.m_cBorder.m_cParams.m_vColor = CGuiInGame::STANDARD_BLUE;
    }
}
// PC 0x0075E8D0
void CSWGuiKeyMapButton::PostChangeToEvent()
{
    CExoInputEventDesc* pEvent = g_pExoInput->GetEvent(m_nAction, CExoInput::EVENTCLASS_ANY);
    if (pEvent != 0)
    {
        if (pEvent->Is2Button() && m_nType > CONTROL_PRIMARY)
        {
            int nControl = m_nNewKey;
            ((CExoInputeventDesc2ButtonAxis*)pEvent)->m_nControl2 = nControl;
        }
        else
        {
            int nControl = m_nNewKey;
            pEvent->m_nControl = nControl;
        }
    }

    CExoIni cIni;
    CExoString sKey;
    sKey.Format("%d", m_nNewKey);
    cIni.WriteIniEntry(sKey, ".\\swkotor.ini", "Keymapping",
                       GetActionId(m_nAction, m_nType));
}
