#ifndef __SWGUIMAINCHARGEN_H__
#define __SWGUIMAINCHARGEN_H__

#include "swguidrawable.h"
#include "swgui.h"
class CSWGuiManager;
#include "exobase.h"
#include "swgui.h"

class CSWCCreature;
class CSWGuiClassSelection;
class CSWGuiQuickOrCustomPanel;
class CSWGuiQuickPanel;
class CSWGuiCustomPanel;

class CSWGuiMainCharGen : public CSWGuiPanel
{
public:
    CSWGuiMainCharGen(CSWGuiManager* pManager, CSWCCreature* pCreature,
                      CSWGuiClassSelection* pClassSelection);
    virtual void Draw(float fTimeStep);
    virtual void OnPanelAdded();

    void SetVisiblePanel(int nPanel);
    void RemoveSubPanels();
    void DetermineClass();
    void SetNameString(const CExoString& sName);
    void PortraitChanged();
    void AbilitiesChanged();
    void ClearAbilities();
#ifndef _XBOX
    void ClearFeats();
#endif
    void NameChanged();
    void BeginPlayGame();

    CSWCCreature* m_pCreature;
    CSWGuiClassSelection* m_pClassSelection;
    CSWGuiQuickOrCustomPanel* m_pQuickOrCustom;
    CSWGuiQuickPanel* m_pQuick;
    CSWGuiCustomPanel* m_pCustom;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblName;
    CSWGuiLabel m_lblClass;
    CSWGuiLabel m_lblStrength;
    CSWGuiLabel m_lblDexterity;
    CSWGuiLabel m_lblConstitution;
    CSWGuiLabel m_lblIntelligence;
    CSWGuiLabel m_lblWisdom;
    CSWGuiLabel m_lblCharisma;
    CSWGuiLabel m_lblStrengthAbility;
    CSWGuiLabel m_lblDexterityAbility;
    CSWGuiLabel m_lblConstitutionAbility;
    CSWGuiLabel m_lblIntelligenceAbility;
    CSWGuiLabel m_lblWisdomAbility;
    CSWGuiLabel m_lblCharismaAbility;
    CSWGuiLabel m_lblVitality;
    CSWGuiLabel m_lblDefense;
    CSWGuiLabel m_lblNewWill;
    CSWGuiLabel m_lblNewReflex;
    CSWGuiLabel m_lblNewFortitude;
    CSWGuiLabel m_lblPortrait;
    CSWGuiLabel m_lblBevelLeft;
    CSWGuiLabel m_lblBevelMiddle;
    CSWGuiLabel m_lblBevelRight;
    CSWGuiLabel m_lblFortitude;
    CSWGuiLabel m_lblReflex;
    CSWGuiLabel m_lblWill;
#ifndef _XBOX
    CSWGuiLabel m_lblStatsBack;
    CSWGuiLabel m_lblStatsBorder;
    CSWGuiLabel m_lblPortBorder;
    CSWGuiLabel m_lblVitName;
    CSWGuiLabel m_lblDefName;
    CSWGuiLabel m_lblBar;
#endif
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblAccept;
    virtual void HandleInputEvent(int nEvent, int nState);
#endif
    CSWGui3DSceneView m_viewModel;
    int m_nVisiblePanel;
    CExoString m_sClass;
    CExoString m_sName;
};

#ifdef _XBOX
inline void CSWGuiMainCharGen::SetNameString(const CExoString& sName)
{
    m_sName = sName;
    m_lblName.m_cText.m_cParams.SetText(m_sName);
    m_lblClass.m_cText.m_cParams.SetText(m_sClass);
}
#endif

#endif
