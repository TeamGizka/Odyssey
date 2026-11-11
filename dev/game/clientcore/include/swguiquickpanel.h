#ifndef __SWGUIQUICKPANEL_H__
#define __SWGUIQUICKPANEL_H__

#include "swgui.h"
class CSWCObject;
class CSWGuiControl;
class CSWGuiMainCharGen;
class CSWGuiManager;
#include "swguiportraitchargen.h"

class CSWCCreature;

class CSWGuiQuickPanel : public CSWGuiPanel
{
public:
    CSWGuiQuickPanel(CSWGuiManager* pManager, CSWCCreature* pCreature);

    void SetState(int nState);
    void OnSelectPlayButton(CSWGuiControl* pControl);
    void OnSelectNameButton(CSWGuiControl* pControl);
    void OnSelectPortraitButton(CSWGuiControl* pControl);
    CExoString GetCurrentName();
    void CancelledByChild();
    void PortraitCreationCompleted();
    void NameCreationCompleted();
    void DoQuickStats();
    void CancelPanel();
    void ConfirmCancel(CSWGuiControl* pControl);
    virtual void OnPanelAdded();
    virtual void HandleInputEvent(int nEvent, int bPressed);
    void OnCancelPressed(CSWGuiControl* pControl);

    CSWCCreature* m_pCreature;
    CSWGuiMainCharGen* m_pMainCharGen;
    CSWGuiLabelHilight m_lblHilight[3];
    CSWGuiLabel m_lblNumber[3];
    CSWGuiButton m_btnStep[3];
#ifndef _XBOX
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnCancel;
#endif
    int m_nState;
};

#endif
