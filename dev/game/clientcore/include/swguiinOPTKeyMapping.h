#ifndef __SWGUIINOPTKEYMAPPING_H__
#define __SWGUIINOPTKEYMAPPING_H__

#include "swguicontrol.h"
#include "swgui.h"

class CSWGuiControl;
class CSWGuiExtent;
class CSWGuiManager;

class CSWGuiInGameOptKeyMappings;

class CSWGuiKeyMapButton : public CSWGuiButton
{
public:
    CSWGuiKeyMapButton(CSWGuiInGameOptKeyMappings* pParent);

    static const int KEY_GOOD;
    static const int KEY_ALREADYUSED;
    static const int KEY_INVALID;

    static const int IC_PC;
    static const int IC_MINIGAME;
    static const int IC_GUI;
    static const int IC_DIALOG;
    static const int IC_FREELOOK;
    static const int IC_MOVIE;

    enum KeyMappingControlType
    {
        CONTROL_SINGLE = 0,
        CONTROL_PRIMARY = 1,
        CONTROL_SECONDARY = 2
    };

    CExoString GetActionId(int nAction, int nType);
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    void Draw(float fElapsed);
#ifndef _XBOX
    CSWGuiControl* HitCheckMouse(int nX, int nY);
#endif
    void UpdateColor(); void SetEditing(int bEditing);
    void SetUnchangable(); void SetDescription(const CExoString& sDescription);
    void SetEvents();
    void ResetToDefault();
    void PostChangeToEvent();
    void onControlEntered(CSWGuiControl* pControl);
    void onControlExit(CSWGuiControl* pControl);
    void onEventClicked(CSWGuiControl* pControl);
    int UpdateEvent(unsigned long nKey);

    static const int STRREF_NONE_SET = 48724;

    int m_nDescStrRef;
    CSWGuiButton m_btnKey;
    CSWGuiInGameOptKeyMappings* m_pParent;
    int m_nAction;
    int m_nType;
    int m_bChanged;
    int m_nNewKey;
    int m_nOriginalKey;
    int m_bUnchangable;
    unsigned long m_nFilterFlags;
    int m_nPage;
    int m_nDescriptionStrRef;
    int m_nSortPos;
};

class CSWGuiInGameOptKeyMappings : public CSWGuiPanel
{
public:

    static const int FILTER_MOVE;
    static const int FILTER_GAME;
    static const int FILTER_MINI;

    CSWGuiInGameOptKeyMappings(CSWGuiManager* pManager);
    virtual ~CSWGuiInGameOptKeyMappings();

    virtual void HandleInputEvent(int nEvent, int nState);
    void FixFilterColors(CSWGuiControl* pControl);
    void FillListBox(int nFilter);
    int IsKeyUsedAlready(unsigned long nKey, int nAction, int nType,
                         unsigned long nFilterFlags);
    int OnDefaultClick(CSWGuiControl* pControl);
    void OnAcceptClick(CSWGuiControl* pControl);
    void OnFilterMove(CSWGuiControl* pControl);
    void OnFilterGame(CSWGuiControl* pControl);
    void OnFilterMini(CSWGuiControl* pControl);
    void SetCaptureEvent(CSWGuiKeyMapButton* pButton);
    CSWGuiKeyMapButton* CreateOption(int nAction, CSWGuiButton* pParent,
                                     int nType);
    void FillListWithEvents();
    int UpdateEvent(int nAction, int nType, unsigned long nKey,
                    unsigned long nFilterFlags);
    virtual void Update(float fElapsed);

    void SetCaptureKeyboardKeys(int bCapture);

    CExoArrayList<CSWGuiKeyMapButton*> m_aOptions;
    CSWGuiLabel m_lblTitle;
    CSWGuiButton m_btnDefault;
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnCancel;
    CSWGuiButton m_btnFilterMove;
    CSWGuiButton m_btnFilterGame;
    CSWGuiButton m_btnFilterMini;
    CSWGuiListBox m_lstEvents;
    CSWGuiLabel m_alblBar[3];
    int m_nFilter;
    int m_bCapturing;
    CSWGuiKeyMapButton* m_pCaptureButton;
};

#endif
