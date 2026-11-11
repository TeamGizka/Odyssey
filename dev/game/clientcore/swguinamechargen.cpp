#include "appmanager.h"
#include "clientexoapp.h"
#include "swguiingame.h"
#include "nwrules.h"
#include "swguicustompanel.h"
#include "swguiquickpanel.h"

#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "nwrace.h"
// PC 0x0076BF00
void CSWGuiNameChargen::SetParentPanel(CSWGuiPanel* pParent, int nParentType)
{
    m_pParent = pParent;
    m_nParentType = nParentType;
}
// PC 0x0076BEB0
void CSWGuiNameChargen::HandleCancelButton()
{
    if (m_pParent != 0)
    {
        m_pManager->PopModalPanel();
        m_pParent->SetVisible(1);
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        if (m_nParentType == 1)
        {
            ((CSWGuiQuickPanel*)m_pParent)->CancelledByChild();
        }
        else if (m_nParentType == 2)
        {
            ((CSWGuiCustomPanel*)m_pParent)->CancelledByChild();
        }
    }
}

#ifdef _XBOX
CSWGuiNameChargen::CSWGuiNameChargen(CSWGuiManager* pManager, CSWCCreature* pCreature)
    : CSWGuiPanel(pManager)
{
    CExoString sName;
    int i;
    m_pCreature = pCreature;
    StartLoadFromLayout("NAME");
    for (i = 0; i < NUM_KEYS; i++)
    {
        sName.Format("BTN_INT%d", i + 1);
        InitControl(m_aIntButtons[i], sName, 1);
        m_aIntButtons[i].AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
        m_aIntButtons[i].AddEvent(GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
        m_aIntButtons[i].m_nIndex = i;
    }
    for (i = 0; i < NUM_KEYS; i++)
    {
        sName.Format("BTN_STD%d", i + 1);
        InitControl(m_aStdButtons[i], sName, 1);
        m_aStdButtons[i].AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
        m_aStdButtons[i].AddEvent(GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
        m_aStdButtons[i].m_nIndex = i;
    }
    InitControl(m_btnSpace, "SPACE_BTN", 1);
    InitControl(m_btnBackspace, "BACKSPACE_BTN", 1);
    InitControl(m_btnDone, "END_BTN", 1);
    InitControl(m_lblBack, "BACK_LBL", 1);
    InitControl(m_lblAccept, "LBL_ACCEPT", 1);
    InitControl(m_lblRandom, "LBL_RANDOM", 1);
    InitControl(m_lblIntl, "LBL_INTL", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblY, "LBL_Y", 1);
    InitControl(m_lblX, "LBL_X", 1);
    InitControl(m_lblNameBox, "NAME_BOX_LBL", 1);
    InitControl(m_lblTitle, "MAIN_TITLE_LBL", 1);
    InitControl(m_lblSubTitle, "SUB_TITLE_LBL", 1);
    StopLoadFromLayout();
    for (i = 0; i < NUM_KEYS; i++)
    {
        if (i < KEYS_PER_ROW)
        {
            if (i < 4)
            {
                m_aStdButtons[i].SetMoveToControl(0, &m_btnBackspace);
                m_aIntButtons[i].SetMoveToControl(0, &m_btnBackspace);
            }
            else if (i < 9)
            {
                m_aStdButtons[i].SetMoveToControl(0, &m_btnSpace);
                m_aIntButtons[i].SetMoveToControl(0, &m_btnSpace);
            }
            else
            {
                m_aStdButtons[i].SetMoveToControl(0, &m_btnDone);
                m_aIntButtons[i].SetMoveToControl(0, &m_btnDone);
            }
        }
        else
        {
            m_aStdButtons[i].SetMoveToControl(0, &m_aStdButtons[i - KEYS_PER_ROW]);
            m_aIntButtons[i].SetMoveToControl(0, &m_aIntButtons[i - KEYS_PER_ROW]);
        }
        if (i > 51)
        {
            if (i > 60)
            {
                m_aStdButtons[i].SetMoveToControl(2, &m_btnDone);
                m_aIntButtons[i].SetMoveToControl(2, &m_btnDone);
            }
            else if (i > 55)
            {
                m_aStdButtons[i].SetMoveToControl(2, &m_btnSpace);
                m_aIntButtons[i].SetMoveToControl(2, &m_btnSpace);
            }
            else
            {
                m_aStdButtons[i].SetMoveToControl(2, &m_btnBackspace);
                m_aIntButtons[i].SetMoveToControl(2, &m_btnBackspace);
            }
        }
        else
        {
            m_aStdButtons[i].SetMoveToControl(2, &m_aStdButtons[i + KEYS_PER_ROW]);
            m_aIntButtons[i].SetMoveToControl(2, &m_aIntButtons[i + KEYS_PER_ROW]);
        }
        if ((unsigned int)i % KEYS_PER_ROW == 0)
        {
            m_aStdButtons[i].SetMoveToControl(1, &m_aStdButtons[i + KEYS_PER_ROW - 1]);
            m_aIntButtons[i].SetMoveToControl(1, &m_aIntButtons[i + KEYS_PER_ROW - 1]);
        }
        else
        {
            m_aStdButtons[i].SetMoveToControl(1, &m_aStdButtons[i - 1]);
            m_aIntButtons[i].SetMoveToControl(1, &m_aIntButtons[i - 1]);
        }
        if ((unsigned int)i % KEYS_PER_ROW == KEYS_PER_ROW - 1)
        {
            m_aStdButtons[i].SetMoveToControl(3, &m_aStdButtons[i - (KEYS_PER_ROW - 1)]);
            m_aIntButtons[i].SetMoveToControl(3, &m_aIntButtons[i - (KEYS_PER_ROW - 1)]);
        }
        else
        {
            m_aStdButtons[i].SetMoveToControl(3, &m_aStdButtons[i + 1]);
            m_aIntButtons[i].SetMoveToControl(3, &m_aIntButtons[i + 1]);
        }
    }
    m_btnSpace.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
    m_btnSpace.AddEvent(GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
    m_btnSpace.m_nIndex = -1;
    m_btnDone.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
    m_btnDone.AddEvent(GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
    m_btnDone.m_nIndex = -1;
    m_btnBackspace.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
    m_btnBackspace.AddEvent(GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
    m_btnBackspace.AddEvent(GUIEVENT_MOUSE_ENTER, this, (CSWGuiHandler)&CSWGuiNameChargen::OnEnterBackspace);
    m_btnBackspace.AddEvent(1, this, (CSWGuiHandler)&CSWGuiNameChargen::OnExitBackspace);
    m_btnBackspace.m_nIndex = -1;
    m_sName = pCreature->GetName();
    m_bRandomName = 0;
    if (m_sName == "")
    {
        HandleRandomName();
    }
    m_lblNameBox.m_cText.m_cParams.SetText(m_sName);
    SetInternational(0);
}
#else
CSWGuiNameChargen::CSWGuiNameChargen(CSWGuiManager* pManager, CSWCCreature* pCreature)  // PC 0x0076C1A0
    : CSWGuiPanel(pManager)
{
    CExoString sName;
    m_pCreature = pCreature;
    StartLoadFromLayout("NAME");
    InitControl(m_btnDone, "END_BTN", 1);
    InitControl(m_editName, "NAME_BOX_EDIT", 1);
    InitControl(m_lblTitle, "MAIN_TITLE_LBL", 1);
    InitControl(m_lblSubTitle, "SUB_TITLE_LBL", 1);
    InitControl(m_btnRandom, "BTN_RANDOM", 1);
    CSWGuiButton* pBackButton = &m_btnBack;
    InitControl(*pBackButton, "BTN_BACK", 1);
    StopLoadFromLayout();
    m_btnDone.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
    m_btnDone.AddEvent(GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
    m_btnRandom.SetFocusable(0);
    m_btnRandom.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnYButtonPressed);
    pBackButton->SetFocusable(0);
    pBackButton->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnDone.SetFocusable(0);
    m_bRandomName = 0;
    m_editName.m_cText.m_nMaxLength = MAX_NAME_LENGTH;
    m_editName.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiNameChargen::HandleDoneButton);
}
// PC 0x0076C490
void CSWGuiNameChargen::OnPanelAdded()
{
    CExoString sName;
    m_editName.SetFocus();
    CSWGuiPanel::OnPanelAdded();
    if (m_nParentType == 1)
    {
        sName = ((CSWGuiCustomPanel*)m_pParent)->GetCurrentName();
    }
    else if (m_nParentType == 2)
    {
        sName = ((CSWGuiCustomPanel*)m_pParent)->GetCurrentName();
    }
    if (sName.GetLength() == 0)
    {
        HandleRandomName();
    }
    else
    {
        m_editName.m_cText.SetText(sName);
    }
}
#endif
// PC 0x0076C580
void CSWGuiNameChargen::HandleInputEvent(int nEvent, int bPressed)
{
    if (bPressed != 0)
    {
        switch (nEvent)
        {
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            HandleCancelButton();
            break;
        case 42:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            HandleRandomName();
#ifdef _XBOX
            SetActiveControl(&m_btnDone, 0);
#endif
            break;
#ifdef _XBOX
        case 41:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            SetInternational(!m_aIntButtons[0].m_bEnabled);
            break;
#endif
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, bPressed);
}
// PC 0x0076BF20
void CSWGuiNameChargen::HandleRandomName()
{
#ifdef _XBOX
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    unsigned char nGender = pStats->m_nGender;
    if (nGender == 1)
    {
        m_sName = m_cNameGen.GetRandomName(CSWRace::CRACE_HUMAN, 6, 15);
    }
    else
    {
        m_sName = m_cNameGen.GetRandomName(CSWRace::CRACE_HUMAN, 5, 15);
    }
    m_lblNameBox.m_cText.m_cParams.SetText(m_sName);
#else
    CExoString sName;
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    unsigned char nGender = pStats->m_nGender;
    if (nGender == 1)
    {
        sName = m_cNameGen.GetRandomName(CSWRace::CRACE_HUMAN, 6, 15);
    }
    else
    {
        sName = m_cNameGen.GetRandomName(CSWRace::CRACE_HUMAN, 5, 15);
    }
    m_editName.m_cText.SetText(sName);
#endif
    m_bRandomName = 1;
}

#ifdef _XBOX
void CSWGuiNameChargen::HandleDoneButton(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl == &m_btnDone)
        {
            if (m_sName.GetLength() == 0)
            {
                CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                CGuiInGame* pGui = pClient->GetInGameGui();
                CSWGuiMessageBox* pMessageBox = pGui->m_pMessageBox;
                pMessageBox->SetAllowCancel(0);
                pMessageBox->SetCallback(0, 0, 0);
                pMessageBox->SetMessage(STRREF_NAME_REQUIRED);
                m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
            else
            {
                while (m_sName[m_sName.GetLength() - 1] == ' ')
                {
                    m_sName = m_sName.Left(m_sName.GetLength() - 1);
                }
                m_pCreature->SetName(m_sName, "");
                m_pManager->PopModalPanel();
                m_pParent->SetVisible(1);
                m_bDeferredRemoval = 0;
                m_nExitState = EXITSTATE_DELETE;
                if (m_nParentType == PARENT_QUICK)
                {
                    ((CSWGuiQuickPanel*)m_pParent)->NameCreationCompleted();
                }
                else if (m_nParentType == PARENT_CUSTOM)
                {
                    ((CSWGuiCustomPanel*)m_pParent)->NameCreationCompleted();
                }
            }
        }
        else
        {
            if (pControl == &m_btnBackspace)
            {
                m_sName = m_sName.Left(m_sName.GetLength() - 1);
                m_bRandomName = 0;
            }
            else if (m_sName.GetLength() < MAX_KEYBOARD_NAME_LENGTH)
            {
                if (pControl == &m_btnSpace)
                {
                    if (m_bRandomName)
                    {
                        m_sName = "";
                        m_bRandomName = 0;
                    }
                    else if (m_sName.GetLength() > 0)
                    {
                        m_sName = m_sName + " ";
                    }
                }
                else
                {
                    if (m_bRandomName)
                    {
                        m_sName = ((CSWGuiButton*)pControl)->m_cText.m_cParams.m_sText;
                        m_bRandomName = 0;
                    }
                    else
                    {
                        m_sName = m_sName + ((CSWGuiButton*)pControl)->m_cText.m_cParams.m_sText;
                    }
                }
            }
            else
            {
                CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                CGuiInGame* pGui = pClient->GetInGameGui();
                CSWGuiMessageBox* pMessageBox = pGui->m_pMessageBox;
                pMessageBox->SetAllowCancel(0);
                pMessageBox->SetMessage(STRREF_NAME_TOO_LONG);
                pMessageBox->SetCallback(0, 0, 0);
                m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
            m_lblNameBox.m_cText.m_cParams.SetText(m_sName);
        }
    }
}
#else
void CSWGuiNameChargen::HandleDoneButton(CSWGuiControl* pControl)  // PC 0x0076C000
{
    CExoString sName;
    sName = m_editName.m_cText.m_sText;
    if (sName.GetLength() == 0)
    {
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        CGuiInGame* pGui = pClient->GetInGameGui();
        CSWGuiMessageBox* pMessageBox = pGui->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetCallback(0, 0, 0);
        pMessageBox->SetMessage(STRREF_NAME_REQUIRED);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
    else
    {
        while (sName[sName.GetLength() - 1] == ' ')
        {
            sName = sName.Left(sName.GetLength() - 1);
        }
        m_pCreature->SetName(sName, "");
        m_pManager->PopModalPanel();
        m_pParent->SetVisible(1);
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        if (m_nParentType == PARENT_QUICK)
        {
            ((CSWGuiQuickPanel*)m_pParent)->NameCreationCompleted();
        }
        else if (m_nParentType == PARENT_CUSTOM)
        {
            ((CSWGuiCustomPanel*)m_pParent)->NameCreationCompleted();
        }
    }
}
#endif

#ifdef _XBOX
#include "exoinput.h"

void CSWGuiNameChargen::OnEnterBackspace(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
        g_pExoInput->SetEventRepeatable(39, 500, 20);
        g_pExoInput->SetEventRepeatable(45, 500, 20);
    }
}

void CSWGuiNameChargen::OnExitBackspace(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
        g_pExoInput->SetEventNonRepeatable(39);
        g_pExoInput->SetEventNonRepeatable(45);
    }
}

void CSWGuiNameChargen::SetInternational(int bInternational)
{
    if (m_aIntButtons[0].m_bEnabled == bInternational)
    {
        return;
    }
    for (int i = 0; i < NUM_KEYS; i++)
    {
        m_aStdButtons[i].m_bEnabled = !bInternational;
        m_aIntButtons[i].m_bEnabled = bInternational;
    }
    if (bInternational)
    {
        if (m_pActiveControl != NULL && m_pActiveControl->m_nIndex != -1)
        {
            SetActiveControl(&m_aIntButtons[m_pActiveControl->m_nIndex], 1);
        }
        m_btnSpace.SetMoveToControl(0, &m_aIntButtons[NUM_KEYS - KEYS_PER_ROW + KEYS_PER_ROW / 2]);
        m_btnSpace.SetMoveToControl(2, &m_aIntButtons[KEYS_PER_ROW / 2]);
        m_btnBackspace.SetMoveToControl(0, &m_aIntButtons[NUM_KEYS - KEYS_PER_ROW]);
        m_btnBackspace.SetMoveToControl(2, &m_aIntButtons[0]);
        m_btnDone.SetMoveToControl(0, &m_aIntButtons[NUM_KEYS - 1]);
        m_btnDone.SetMoveToControl(2, &m_aIntButtons[KEYS_PER_ROW - 1]);
        m_lblIntl.m_cText.m_cParams.SetStrRef(42350);
    }
    else
    {
        if (m_pActiveControl != NULL && m_pActiveControl->m_nIndex != -1)
        {
            SetActiveControl(&m_aStdButtons[m_pActiveControl->m_nIndex], 1);
        }
        m_btnSpace.SetMoveToControl(0, &m_aStdButtons[NUM_KEYS - KEYS_PER_ROW + KEYS_PER_ROW / 2]);
        m_btnSpace.SetMoveToControl(2, &m_aStdButtons[KEYS_PER_ROW / 2]);
        m_btnBackspace.SetMoveToControl(0, &m_aStdButtons[NUM_KEYS - KEYS_PER_ROW]);
        m_btnBackspace.SetMoveToControl(2, &m_aStdButtons[0]);
        m_btnDone.SetMoveToControl(0, &m_aStdButtons[NUM_KEYS - 1]);
        m_btnDone.SetMoveToControl(2, &m_aStdButtons[KEYS_PER_ROW - 1]);
        m_lblIntl.m_cText.m_cParams.SetStrRef(42282);
    }
}

void CSWGuiNameChargen::OnPanelAdded()
{
    SetActiveControl(&m_btnDone, 0);
}
#endif
