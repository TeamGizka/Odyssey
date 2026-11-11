#ifndef __SWGUIINGAMEGAMEPLAY_H__
#define __SWGUIINGAMEGAMEPLAY_H__

#include "exobase.h"
#include "swgui.h"
#include "swguicontrol.h"
#include "swguioptions.h"

class CSWGuiInGameGameplay : public CSWGuiPanel
{
public:

    enum DescriptionStringRef
    {
        STRREF_INVERT_CAMERA = 48014, STRREF_AUTO_LEVELUP = 42266,
        STRREF_AUTO_SAVE = 38038, STRREF_REVERSE_MOUSE = 48213,
        STRREF_COMBAT_MOVEMENT = 42484, STRREF_MOUSE_OPTIONS = 49071,
        STRREF_KEYMAP_OPTIONS = 49072
    };
    CSWGuiInGameGameplay(CSWGuiManager* pManager);

    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();

    void OnAutoLevelUp(CSWGuiControl* pControl);
    void OnAutoSave(CSWGuiControl* pControl);
    void OnInvertCam(CSWGuiControl* pControl);
    void OnReverse(CSWGuiControl* pControl);
    void OnCombatMovement(CSWGuiControl* pControl);
    void OnKeyMap(CSWGuiControl* pControl);
    void OnMouse(CSWGuiControl* pControl);
    void SetDifficultyText();
    void SetDescription(unsigned long nStrRef);
    void OnEnterDiff(CSWGuiControl* pControl);
    void OnExitDiff(CSWGuiControl* pControl);
    void SetUpOptions();
    void OnEnterSlot(CSWGuiControl* pControl);
    void OnDefault(CSWGuiControl* pControl);
    void OnDifficultyLeft(CSWGuiControl* pControl);
    void OnDifficultyRight(CSWGuiControl* pControl);

#ifdef _XBOX

    struct CFlash { CSWGuiLabel* m_pLabel; float m_fTime; };
    virtual void Draw(float fElapsed); void FlashLabel(CSWGuiLabel* pLabel);
    CSWGuiButton m_btnDifficulty; CSWGuiOptionsCheckbox m_cbAutoLevelUp; CSWGuiOptionsCheckbox m_cbInvertCam;
    CSWGuiOptionsCheckbox m_cbAutoSave; CSWGuiOptionsCheckbox m_cbReverse; CSWGuiOptionsCheckbox m_cbCombatMovement;
    CSWGuiLabel m_lblDiffRight; CSWGuiLabel m_lblDiffLeft; CSWGuiLabel m_lblA; CSWGuiLabel m_lblB; CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblSelect; CSWGuiLabel m_lblBack; CSWGuiLabel m_lblTitle; CSWGuiLabel m_lblRAnalog;
    CSWGuiListBox m_lbxDescription; int m_nDifficulty; CExoArrayList<CFlash> m_aFlashes;
#else
    CSWGuiLabel m_lblDescription; CSWGuiLabel m_lblTitle; CSWGuiListBox m_lbxDescription;
    CSWGuiButton m_btnBack; CSWGuiButton m_btnDefault; CSWGuiButton m_btnKeyMap; CSWGuiButton m_btnMouse;
    CSWGuiButton m_btnDifficulty; CSWGuiButton m_btnDiffRight; CSWGuiButton m_btnDiffLeft;
    CSWGuiOptionsCheckbox m_cbAutoLevelUp; CSWGuiOptionsCheckbox m_cbInvertCam; CSWGuiOptionsCheckbox m_cbAutoSave;
    CSWGuiOptionsCheckbox m_cbReverse; CSWGuiOptionsCheckbox m_cbReverseInGame; CSWGuiOptionsCheckbox m_cbCombatMovement;
    CSWGuiSlider m_sliMouseSen; CSWGuiLabel m_lblMouseSen;
    CSWGuiLabel m_alblBar[4];
    int m_nDifficulty;
#endif
};

#endif
