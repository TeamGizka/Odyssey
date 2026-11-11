#ifndef __SWGUIINGAMETUTORIALBOX_H__
#define __SWGUIINGAMETUTORIALBOX_H__

#include "swgui.h"
class CSWGuiControl;
class CSWGuiManager;

#include "swguiexamine.h"

class CSWGuiTutorialBox : public CSWGuiMessageBox
{
public:
    unsigned long m_oidInstigator;
    unsigned long m_oidTarget;
    unsigned long m_nActionParam;
    int m_bWasPausedByCombat;
    unsigned char m_nTutorialReason;
    unsigned char m_nMessageIndex;
    char gap_996[2];

    CSWGuiTutorialBox(CSWGuiManager* pManager);
    virtual void SetMessage(unsigned long nStrRef);
    int SetTutorialInstigator(unsigned long oidInstigator,
                              unsigned long oidTarget,
                              unsigned long nActionParam);
    int TutorialReasonIsValid(unsigned char nReason);
    void MessageBoxUnpause(CSWGuiControl* pControl);
    void PerformCombatFeat(CSWGuiControl* pControl);
    void PerformForcePower(CSWGuiControl* pControl);
    void PerformUseGrenade(CSWGuiControl* pControl);
    void PerformSetMine(CSWGuiControl* pControl);
    void PerformPlayerFlourishWeapons(CSWGuiControl* pControl);
    void PerformBash(CSWGuiControl* pControl);
    void PerformAttack(CSWGuiControl* pControl);
    void MessageBoxPause(CSWGuiControl* pControl);
    void PerformNonCombatXButton(CSWGuiControl* pControl);
    void PerformCombatXButton(CSWGuiControl* pControl);
    void PerformCombatYButton(CSWGuiControl* pControl);
    void PerformReturnToBase(CSWGuiControl* pControl);
    int SetNextMessage();
    virtual void OnPanelAdded();
    virtual void HandleInputEvent(int nEvent, int nState);
    int SetTutorialReason(unsigned char nReason);
};

#endif
