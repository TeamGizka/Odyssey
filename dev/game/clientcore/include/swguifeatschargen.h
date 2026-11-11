#ifndef __SWGUIFEATSCHARGEN_H__
#define __SWGUIFEATSCHARGEN_H__

#include "swguicontrol.h"
#include "swgui.h"
class CSWGuiControl;
class CSWGuiManager;
class CSWGuiLevelUpPanel;
class CSWCCreature;
#include "exobase.h"
#include "swguiskillflowchart.h"

class CSWGuiFeatsCharGen : public CSWGuiPanel
{
public:

    enum FeatState
    {
        FEAT_AVAILABLE = 0,
        FEAT_CHOSEN = 1,
        FEAT_LOCKED = 2,
        FEAT_GRANTED = 3,
        FEAT_UNAVAILABLE = 4
    };

    enum StringRef
    {
        STRREF_FEAT = 42487,
        STRREF_ADD = 38455,
        STRREF_REMOVE = 38456,
        STRREF_LEVEL_UP = 1071,
        STRREF_BONUS_FEATS = 1316,
        STRREF_NO_FEAT_CHOICES_REMAINING = 42530,
        STRREF_FEAT_AUTOMATIC_THIS_LEVEL = 42183,
        STRREF_FEAT_ALREADY_OWNED = 42182,
        STRREF_FEAT_PREREQUISITES_NOT_MET = 42184
    };

    CSWGuiFeatsCharGen(CSWGuiManager* pManager, CSWCCreature* pCreature,
                       int bLevelUp, int bBonusFeatSelection);

    CSWCCreature* m_pCreature;

    CSWGuiPanel* m_pParentPanel;

    CSWGuiLabel m_lblMainTitle;
    CSWGuiLabel m_lblSubTitle;
    CSWGuiLabel m_lblRemainingBox;
    CSWGuiLabel m_lblStdRemainingSelections;
    CSWGuiLabel m_lblStdSelectionsRemaining;
    CSWGuiLabel m_lblDesc;
    CSWGuiLabel m_lblSelectionsRemaining;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblDescription;
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblRecommended;
    CSWGuiLabel m_lblSelect;
    CSWGuiLabel m_lblRanalog;
#endif
    CSWGuiLabel m_lblName;
#ifndef _XBOX
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnRecommended;
    CSWGuiButton m_btnSelect;
#endif
    CSWGuiListBox m_lbFeats;
    CSWGuiListBox m_lbDesc;
    CExoArrayList<unsigned short> m_lGranted;
    CExoArrayList<unsigned short> m_lLocked;
    CExoArrayList<unsigned short> m_lAvailable;
    CExoArrayList<unsigned short> m_lChosen;

    static const unsigned short FEAT_INVALID = 0xffff;
    unsigned short m_nSelectedFeat;

    unsigned char m_nGeneralFeats;
    unsigned char m_nBonusFeats;
    unsigned char m_nGeneralFeatChoices;
    unsigned char m_nBonusFeatChoices;
    unsigned char m_nFeatsRemaining;
    unsigned char m_nUnmapped19F3;
    unsigned long m_bLevelUp : 1;
    unsigned long m_bBonusFeatSelection : 1;
    unsigned long m_bShowGrantedTutorial : 1;
    unsigned long : 29;
    CExoString m_sFeatUnavailableText;
    CExoString m_sFeatLockedText;
    CSWGuiSkillFlowChart m_cChart;
#ifndef _XBOX
    unsigned char m_nFeatState;
#endif
    virtual ~CSWGuiFeatsCharGen();
#ifndef _XBOX
    virtual void Draw(float fElapsed);
#endif
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    void ShowGranted(CSWGuiControl* pControl);
    void HandleCancelButton();
    void DetermineFeat(unsigned short nFeat, unsigned char& nState);
    void SetDescription(unsigned long nStrRef);
    void OnEnterFeat(unsigned short nFeat);
    void BuildButtons();
    void BuildAvailableList();
#ifndef _XBOX
    void OnFeatSelectionChanged(CSWGuiControl* pControl);
#endif
    void InitiateFeats();
    void AddChosenFeat(unsigned short nFeat);

    int GetNumChosenFeats() { return m_lChosen.num; }
    void RemoveChosenFeat(unsigned short nFeat);
    void OnRecommendButton();
    void OnFeatPicked(unsigned short nFeat);
#ifndef _XBOX
    void OnDoubleClick(CSWGuiControl* pControl);
#endif
    void OnAcceptButton();
};

#endif
