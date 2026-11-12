#ifndef __SWGUILEVELUPCHARGEN_H__
#define __SWGUILEVELUPCHARGEN_H__

#include "exobase.h"
#include "swguimainchargen.h"

class CSWCCreature;
class CSWGuiInGameCharacter;
class CSWGuiLevelUpPanel;

extern int g_bCreateAllGUIs;
extern int g_bDo3DGUIs;

class CSWGuiLevelUpCharGen;

class CSWGuiLevelUpCharGen : public CSWGuiPanel
{
public:
    static const unsigned long STRREF_LEVEL_LABEL = 74696;

    CSWGuiLevelUpCharGen(CSWGuiManager* pManager, unsigned long nCreature,
                         CSWCCreature* pCreature);
    virtual ~CSWGuiLevelUpCharGen();
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();

    void DetermineClass();
    void SetNameString(const CExoString& sName);
    void AbilitiesChanged();
    void NewAbilityScores(int nStr, int nDex, int nCon, int nInt, int nWis,
                          int nCha, int nPoints);
    void ClearAbilities();
    void ClearSkills();
    void ClearPowers();
    void BeginPlayGame();
    int CopyCreatureStats(CSWCCreature* pFrom, CSWCCreature* pTo);
    void ClearFeats();
    void LevelUpCreature();
    void RemoveSubPanels();
    void HandleCancelButton();
    void CancelLevelUp();
#ifdef _XBOX
    void SetBackPromptEnabled(int bEnabled);
#endif

    CSWCCreature* m_pCreature;
    CSWGuiInGameCharacter* m_pInGameCharacter;
    CSWGuiLevelUpPanel* m_pLevelUpPanel;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblName;
    CSWGuiLabel m_lblClass;
    CSWGuiLabel m_lblLevelValue;
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
#endif
    CSWGui3DSceneView m_viewModel;
    CSWGuiLabel m_lblPortrait;
    int m_nUnknown2540;
    int m_nInputClass;
    CExoString m_sClass;
    CExoString m_sName;
    int m_nUnknown2558;
    unsigned long m_nCreatureId;
};

#endif
