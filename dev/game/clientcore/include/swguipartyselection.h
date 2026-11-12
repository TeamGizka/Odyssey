#ifndef __SWGUIPARTYSELECTION_H__
#define __SWGUIPARTYSELECTION_H__

#include "swgui.h"

#ifdef _XBOX

typedef CSWGuiButtonToggle CSWGuiPartySelectionButton;
#else
class CSWGuiPartySelectionButton : public CSWGuiRadioButton
{
public:
    CSWGuiPartySelectionButton();
    virtual ~CSWGuiPartySelectionButton();

    void SetSelected(int bSelected);

    void SetPartyData(void* pData)
    {
        m_nIndex = (int)pData;
    }

    int m_bPartySelected;
};
#endif

class CSWGuiPartySelectionData
{
public:
    ~CSWGuiPartySelectionData();

    CSWGuiObject* m_p00;
    CSWGuiPartySelectionButton m_btnNPC;
    CSWGuiLabel m_lblNotAvailable;
    CSWGuiLabel m_lblCharacter;
    unsigned long m_bSelectable : 1;
    unsigned long m_bSpawned : 1;
    unsigned long m_bForced : 1;
    unsigned long m_nFlagsUnused : 29;
    int m_nPartyIndex;
    int m_nNPC;
    int m_bHidden;
};

class CSWGuiPartySelection : public CSWGuiPanel
{
public:
    CSWGuiPartySelection(CSWGuiManager* pManager);

    enum { MAX_SELECTED_NPCS = 2 };

    enum
    {
        NPC_ATTON = 0,
        NPC_BAO_DUR = 1,
        NPC_CANDEROUS = 2,
        NPC_G0T0 = 3,
        NPC_HANDMAIDEN = 4,
        NPC_HK_47 = 5,
        NPC_KREIA = 6,
        NPC_MIRA = 7,
        NPC_T3_M4 = 8,
        NPC_VISAS = 9,
        NPC_HANHARR = 10,
        NPC_DISCIPLE = 11
    };

    enum
    {
        STRREF_AVAILABLE_SLOTS = 1564,
        STRREF_LEVEL = 32154,
        STRREF_CONFIRM_PARTY = 38328,
        STRREF_CONFIRM_SMALL_PARTY = 38329,
        STRREF_ADD = 38455,
        STRREF_REMOVE = 38456,
        STRREF_UNABLE_TO_JOIN = 42376,
        STRREF_CANNOT_CANCEL = 42405,
        STRREF_MUST_BE_MEMBER = 42406,
        STRREF_UNABLE_TO_LEAVE = 108425,
        STRREF_CANNOT_CHANGE_PARTY = 108426,
        STRREF_CANNOT_CHANGE_COMBAT = 126692,
        STRREF_CANNOT_CHANGE_UNCONSCIOUS = 126693
    };
#ifdef _XBOX

    enum { RESTYPE_TXB = 3006 };
#endif

    enum LabelIndex
    {
        LABEL_TITLE = 0,
        LABEL_ADD_REMOVE = 2,
        LABEL_AVAILABLE = 3,
        LABEL_BEVEL_L = 4,
        LABEL_BEVEL_R = 5,
        LABEL_BEVEL_M = 6
    };

    virtual void HandleInputEvent(int nEvent, int bPressed);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();

    void SetForcedNPC(int nFirstNPC, int nSecondNPC);
    void UpdateCount();
#ifndef _XBOX
    void UpdateVisibleCharacters();
#endif
    void AcceptParty(int bCancelled);
    void ConfirmPartySelection(CSWGuiControl* pControl);
    void SetAddRemoveLabel(int bAdd);
    void OnToggled(CSWGuiControl* pControl);
    void OnDone(CSWGuiControl* pControl);
    void OnEnter(CSWGuiControl* pControl);
    // PC 0x005EE1C0
    void OnExit(CSWGuiControl*)
    {
    }
#ifdef _XBOX
    void OnDoneEnter(CSWGuiControl* pControl);
#endif

    int m_nInputClass;
    int m_nSelectedCount;
    int m_bForcedNPC;
    int m_bPlaySound;
    int m_n78;
    CExoString m_sRunScript;
    CSWGuiPartySelectionData m_aPartyMembers[12];
    CSWGuiLabel m_lblCount;
#ifdef _XBOX
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblB;
#endif
    CSWGuiButton m_btnDone;
    CSWGuiLabel m_lblNPCName;
    CSWGuiLabel m_lblNPCLevel;
#ifdef _XBOX
    CSWGuiLabel m_lbl3D;
#else
    CSWGui3DSceneView m_lbl3D;
#endif
    CSWGuiLabel m_aLabels[7];
#ifndef _XBOX
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnAccept;
    CSWGuiControl* m_pSelectedControl;
    CSWGuiLabel m_alblBar[5];
    CSWGuiLabel m_lblNameBack;
    int m_bCanChangeParty;
    unsigned long m_nCannotChangeStrRef;
#endif
};

#endif
