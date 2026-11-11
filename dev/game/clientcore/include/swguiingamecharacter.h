#ifndef __SWGUIINGAMECHARACTER_H__
#define __SWGUIINGAMECHARACTER_H__

#include "swgui.h"
class CSWGuiControl;
class CSWGuiLabel;
class CSWGuiLevelUpPanel;
class CSWGuiManager;
class CSWGuiScriptSelect;

#include "swguicontrol.h"
#include "swguidrawable.h"
#include "math3d.h"
class CSWGuiLevelUpCharGen;

extern CSWGuiManager* g_pGuiMan;

class CSWGuiSpeedButton : public CSWGuiButton
{
public:

    enum { EVENT_REPEAT_LEFT = 90710, EVENT_REPEAT_RIGHT = 90711 };

    CSWGuiSpeedButton()
    {
        SetRepeat(0.2f, 0.5f);
    }
    // PC 0x0067E800
    SWGUI_PCVIRTUAL void HandleLMouseDown()
    {
        m_bIncrease = 1;
        CSWGuiControl::HandleLMouseDown();
        HandleInputEvent(90710, 1);
        m_fTimeToRepeat = m_fRepeatRate;
    }
    SWGUI_PCVIRTUAL void HandleRMouseDown()  // PC 0x0067E830
    {
        m_bIncrease = 0;
        CSWGuiControl::HandleRMouseDown();
        HandleInputEvent(90711, 1);
        m_fTimeToRepeat = m_fRepeatRate;
    }
    virtual void Draw(float fElapsed)  // PC 0x0067E860
    {
        CSWGuiButton::Draw(fElapsed);
#ifndef _XBOX
        if (g_pGuiMan->m_pMouseCapture == this)
        {
            m_fTimeToRepeat -= fElapsed;
            while (m_fTimeToRepeat < 0.0f)
            {
                m_fTimeToRepeat += m_fRepeatDelay;
                if (m_bIncrease)
                {
                    HandleInputEvent(EVENT_REPEAT_LEFT, 1);
                }
                else
                {
                    HandleInputEvent(EVENT_REPEAT_RIGHT, 1);
                }
            }
        }
#endif
    }

    void SetRepeat(float fDelay, float fRate)
    {
        SetRepeatDelay(fDelay);
        SetRepeatRate(fRate);
    }
    void SetRepeatDelay(float fDelay)
    {
        m_fRepeatDelay = fDelay;
    }
    void SetRepeatRate(float fRate)
    {
        m_fRepeatRate = fRate;
    }

    float m_fRepeatDelay;
    float m_fRepeatRate;
    float m_fTimeToRepeat;
    unsigned long m_bIncrease : 1;
    unsigned long m_nUnmapped1D0Rest : 31;
};

extern int g_bDo3DGUIs;

class CSWGuiBlackenedLabel : public CSWGuiLabel
{
public:
    virtual void Draw(float fElapsed);
};

class CSWGuiSliderReadOnly : public CSWGuiSlider
{
public:
    // PC 0x004015A0
    SWGUI_PCVIRTUAL void HandleLMouseDown()
    {
    }
};

class CAurScene;
class CAurObject;

class CSWServerInfo
{
public:
    unsigned char m_aUnknown00[0x104];
    unsigned long m_bPartyPicking : 1;
    unsigned long m_nUnknown104Rest : 31;
};

class CSWCCreature;

class CSWGuiInGameCharacter : public CSWGuiPanel
{
public:

    enum
    {
        EVENT_SCRIPT_SELECT = 41,
        EVENT_AUTO_LEVELUP = 42,
        EVENT_ROTATE_LEFT = 59,
        EVENT_ROTATE_RIGHT = 60,
#ifdef _XBOX
        EVENT_NEXT_CHARACTER = 43
#else
        EVENT_NEXT_CHARACTER = 206,
        EVENT_EXIT = 223
#endif
    };

    enum
    {
        CHARACTER_INDEX_NONE = -2,
        CHARACTER_INDEX_PLAYER = -1
    };

    enum { STRREF_CHANGE_CHARACTER = 38693 };

    enum { EFFECT_ICON_CAPACITY = 10, ALIGNMENT_SLIDER_MAX = 100 };

    CSWGuiInGameCharacter(CSWGuiManager* pManager);
    virtual ~CSWGuiInGameCharacter();
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();

    void SetModText(CSWGuiLabel& lblMod, int nMod);
    void SetLabelBonusPenaltyColour(CSWGuiLabel& lblStat, int nBonus);
    void DoAutoLevelup(CSWGuiControl* pControl);

    void AttachModel();
    void RotateCharacter(int bLeft);
#ifndef _XBOX
    void RotateCharacterLeft(int nUnused);
    void RotateCharacterRight(int nUnused);
    void UpdatePortraits();
    void OnChangeCharacter(CSWGuiControl* pControl);
    void OnSwitchLeft(CSWGuiControl* pControl);
    void OnSwitchRight(CSWGuiControl* pControl);
#endif
    void SetStats();
    int ShowLevelUpGUI(int bAutomatic);
    void StartAutoLevelup();

    CSWGuiLabel m_lblName;
    CSWGuiLabel m_lblClass;
    CSWGuiLabel m_lblClass1;
    CSWGuiLabel m_lblClass2;
    CSWGuiLabel m_lblLevel;
    CSWGuiLabel m_lblLevel1;
    CSWGuiLabel m_lblLevel2;
    CSWGuiLabel m_lblFortitude;
    CSWGuiLabel m_lblReflex;
    CSWGuiLabel m_lblWill;
    CSWGuiLabel m_lblFortitudeStat;
    CSWGuiLabel m_lblReflexStat;
    CSWGuiLabel m_lblWillStat;
    CSWGuiLabel m_lblExperience;
    CSWGuiLabel m_lblExperienceStat;
    CSWGuiLabel m_lblNextLevel;
    CSWGuiLabel m_lblNeededXP;
    CSWGuiLabel m_lblDefenseStat;
    CSWGuiLabel m_lblForceStat;
    CSWGuiLabel m_lblVitalityStat;
    CSWGuiLabel m_lblDefense;
    CSWGuiLabel m_lblForce;
    CSWGuiLabel m_lblVitality;
    CSWGuiLabel m_lblStr;
    CSWGuiLabel m_lblStrength;
    CSWGuiLabel m_lblStrMod;
    CSWGuiLabel m_lblWis;
    CSWGuiLabel m_lblWisdom;
    CSWGuiLabel m_lblWisMod;
    CSWGuiLabel m_lblCha;
    CSWGuiLabel m_lblCharisma;
    CSWGuiLabel m_lblChaMod;
    CSWGuiLabel m_lblInt;
    CSWGuiLabel m_lblIntelligence;
    CSWGuiLabel m_lblIntMod;
    CSWGuiLabel m_lblCon;
    CSWGuiLabel m_lblConstitution;
    CSWGuiLabel m_lblConMod;
    CSWGuiLabel m_lblDex;
    CSWGuiLabel m_lblDexterity;
    CSWGuiLabel m_lblDexMod;
#ifdef _XBOX
    CSWGuiLabel m_lblXScripts;
    CSWGuiBlackenedLabel m_lblALevelUp;
    CSWGuiBlackenedLabel m_lblAuto;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblBevel;
    CSWGuiLabel m_lblStart;
    CSWGuiLabel m_lblExit;
    CSWGuiLabel m_lblBlack;
    CSWGuiBlackenedLabel m_lblChangeChar;
    CSWGuiLabel m_lblBevel2;
#else
    CSWGuiLabel m_lblBevel;
    CSWGuiLabel m_lblBevel2;
#endif
    CSWGuiLabel m_lblGood[EFFECT_ICON_CAPACITY];
    CSWGuiLabel m_lblMore;
    CSWGuiLabel m_lblAdorn;
    CSWGuiBlackenedLabel m_lblLight;
    CSWGuiBlackenedLabel m_lblDark;
#ifdef _XBOX
    CSWGuiSlider m_sldAlign;
    CSWGui3DSceneView m_viewCharacter;
    int m_nPartySize;
    CSWCCreature* m_pCreature;
    CSWCCreature* m_pShownCreature;
    int m_nShownAlignment;
#else
    CSWGuiButton m_btnAuto;
    CSWGuiButton m_btnLevelUp;
    CSWGuiButton m_btnChange1;
    CSWGuiButton m_btnChange2;
    CSWGuiButton m_btnCharLeft;
    CSWGuiButton m_btnCharRight;
    CSWGuiButton m_btnExit;
    CSWGuiButton m_btnScripts;
    CSWGuiSliderReadOnly m_sldAlign;
    CSWGui3DSceneView m_viewCharacter;
    CSWGuiSpeedButton m_btn3DChar;
    int m_nPartySize;
    CSWCCreature* m_pCreature;
    CSWCCreature* m_pShownCreature;
    int m_nShownAlignment;
    char m_nSelectedNPC;
#endif
    CSWGuiScriptSelect* m_pScriptSelect;
};

class CSWGuiButtonToggle;
class CSWGuiControl;
class CSWGuiManager;
class CSWGuiObject;
#include "exobase.h"
#include "swgui.h"
#include "swguicontrol.h"

class CSWGuiScriptCreatureStats
{
public:
    unsigned char m_aUnknown00[0x13A];
    unsigned short m_nAIState;
};

class CSWGuiScriptCreature
{
public:
#ifdef _XBOX
    unsigned char m_aUnknown00[0xA84];
#else
    unsigned char m_aUnknown00[0xA74];
#endif
    CSWGuiScriptCreatureStats* m_pStats;
};

class CSWGuiScriptSelect : public CSWGuiPanel
{
public:

    struct SCRIPT_INFO
    {
        unsigned long m_nDescriptionStrRef;
        int m_nAIState;
    };

    CSWGuiScriptSelect(CSWGuiManager* pManager);
    virtual ~CSWGuiScriptSelect();

    void SetCreatureID(unsigned long oidCreature);
    CExoArrayList<SCRIPT_INFO> m_aScripts;
    CSWGuiListBox m_lbxScripts;
    CSWGuiListBox m_lbxDescription;
    CSWGuiLabel m_lblDescription;
#ifdef _XBOX

    CSWGuiLabel m_lblAImg;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblBImg;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblRAnalog;
#else
    CSWGuiLabel m_lblTitle;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnAccept;
#endif
    int m_nUnknownC38;
    unsigned long m_oidCreature;
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    void SetDescription(unsigned long nStrRef);
    void OnScriptSelected(CSWGuiControl* pControl);
    CSWGuiControl* CreateOption(unsigned long nStrRef, PFNGUIEVENT pHandler,
                                CSWGuiButtonToggle* pProto);
    void OnEnter(CSWGuiControl* pControl);
};

#endif
