#ifndef __SWGUICUSTOMPANEL_H__
#define __SWGUICUSTOMPANEL_H__

class CSWCObject;
class CSWGuiControl;
class CSWGuiManager;
#include "swguifeatschargen.h"
#include "swguimainchargen.h"
#include "swguicontrol.h"
#include "swgui.h"
#include "nwccreaturestats.h"
#include "exobase.h"
#include "math3d.h"
#include "swguiabilitieschargen.h"
#include "guiingame.h"
#include "nwrules.h"

class CSWGuiPanel;
class CSWCCreature;

class SFeedbackMessage;

class SDialogMessage;

class CSWGuiNameChargen : public CSWGuiPanel
{
public:

    enum { PARENT_QUICK = 1, PARENT_CUSTOM = 2 };
    enum { MAX_NAME_LENGTH = 18 };
    static const unsigned long STRREF_NAME_REQUIRED = 42360;
    CSWGuiNameChargen(CSWGuiManager* pManager, CSWCCreature* pCreature);
    void SetParentPanel(CSWGuiPanel* pParent, int nParentType);
    void HandleCancelButton();
    void HandleRandomName();
    void HandleDoneButton(CSWGuiControl* pControl);
    virtual void HandleInputEvent(int nEvent, int bPressed);
#ifdef _XBOX
    void SetInternational(int bInternational);
    void OnEnterBackspace(CSWGuiControl* pControl);
    void OnExitBackspace(CSWGuiControl* pControl);
#else
    virtual void OnPanelAdded();
#endif

    CSWCCreature* m_pCreature;
    CSWGuiPanel* m_pParent;
#ifdef _XBOX
    enum { NUM_KEYS = 65, KEYS_PER_ROW = 13, MAX_KEYBOARD_NAME_LENGTH = 16 };
    static const unsigned long STRREF_NAME_TOO_LONG = 42361;
    CSWGuiButton m_aStdButtons[NUM_KEYS];
    CSWGuiButton m_aIntButtons[NUM_KEYS];
    CSWGuiButton m_btnSpace;
    CSWGuiButton m_btnBackspace;
    CSWGuiButton m_btnDone;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblRandom;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblNameBox;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblSubTitle;
    CSWGuiLabel m_lblIntl;
#else
    CSWGuiButton m_btnDone;
    CSWGuiEditbox m_editName;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblSubTitle;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnRandom;
#endif
    unsigned char m_nParentType;
    unsigned long m_bRandomName : 1;
    unsigned long m_nNameFlagsUnused : 31;
#ifdef _XBOX
    CExoString m_sName;
#endif
    CSWNameGen m_cNameGen;
};

class CSWGuiCustomPanel : public CSWGuiPanel
{
public:
    CSWGuiCustomPanel(CSWGuiManager* pManager, CSWCCreature* pCreature);
    void SetParentPanel(CSWGuiPanel* pParent);
    void CancelledByChild();
    void PortraitCreationCompleted();

    CSWCCreature* m_pCreature;
    CSWGuiPanel* m_pParentPanel;
    CSWGuiLabelHilight m_aHilights[6];
    CSWGuiLabel m_aNumbers[6];
    CSWGuiButton m_aSteps[6];
    CSWGuiLabel m_lblBackground;
#ifndef _XBOX
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnCancel;
#endif
    int m_nState;
    void AbilityCreationCompleted();
    void SetState(int nState);
    void OnSelectPortraitButton(CSWGuiControl* pControl);
    void OnSelectNameButton(CSWGuiControl* pControl);
    void OnSelectFeatsButton(CSWGuiControl* pControl);
    void OnSelectSkillsButton(CSWGuiControl* pControl);
    void OnSelectAbilitiesButton(CSWGuiControl* pControl);
    void OnSelectPlayButton(CSWGuiControl* pControl);
    void NameCreationCompleted();
    void SkillCreationCompleted();
    void FeatCreationCompleted();
    void ClearAbilities();
    void CancelPanel();
    void ConfirmCancel(CSWGuiControl* pControl);
    void OnCancelPressed(CSWGuiControl* pControl);
    CExoString GetCurrentName();
    virtual void HandleInputEvent(int nEvent, int nState);
};

#endif
