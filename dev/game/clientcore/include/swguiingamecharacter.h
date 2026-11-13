#ifndef __SWGUIINGAMECHARACTER_H__
#define __SWGUIINGAMECHARACTER_H__

#include "swgui.h"
class CSWGuiControl;
class CSWGuiLabel;
class CSWGuiLevelUpPanel;
class CSWGuiManager;

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
    // PC 0x006D51E0 GOG 0x006D5150
    SWGUI_PCVIRTUAL void HandleLMouseDown()
    {
        m_bIncrease = 1;
        CSWGuiControl::HandleLMouseDown();
        HandleInputEvent(90710, 1);
        m_fTimeToRepeat = m_fRepeatRate;
    }
    SWGUI_PCVIRTUAL void HandleRMouseDown()  // PC 0x006D5210 GOG 0x006D5180
    {
        m_bIncrease = 0;
        CSWGuiControl::HandleRMouseDown();
        HandleInputEvent(90711, 1);
        m_fTimeToRepeat = m_fRepeatRate;
    }
    virtual void Draw(float fElapsed)  // PC 0x006D5240 GOG 0x006D51B0
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
    // PC 0x004015A0 GOG 0x004015A0
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

    enum
    {
        STRREF_CHANGE_CHARACTER = 38693,
        STRREF_LIGHT_MASTERY_STR = 114161,
        STRREF_LIGHT_MASTERY_CON = 114162,
        STRREF_LIGHT_MASTERY_WIS = 114163,
        STRREF_DARK_MASTERY_DAMAGE = 114164,
        STRREF_DARK_MASTERY_DEX = 114165,
        STRREF_DARK_MASTERY_FORCE_POINTS = 114166
    };

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
#endif
    void SetStats();
    int ShowLevelUpGUI(int bAutomatic);
    void StartAutoLevelup();

#ifdef _XBOX
    CSWGuiLabel m_lblName;
    CSWGuiLabel m_lblClass;
    CSWGuiLabel m_lblClass1;
    CSWGuiLabel m_lblClass2;
    CSWGuiLabel m_lblLevel;
    CSWGuiLabel m_lblLevel1;
    CSWGuiLabel m_lblLevel2;
#else
    CSWGuiLabel m_lblForceMastery;
#endif
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
#ifdef _XBOX
    CSWGuiLabel m_lblGood[EFFECT_ICON_CAPACITY];
#endif
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
    CSWGuiButton m_btnExit;
    CSWGuiSliderReadOnly m_sldAlign;
    CSWGui3DSceneView m_viewCharacter;
    CSWGuiSpeedButton m_btn3DChar;
    CSWGuiLabel m_alblBar[6];
    CSWGuiLabel m_lblXpBack;
    CSWGuiLabel m_lblMoreBack;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblStatsBorder;
    int m_nPartySize;
    CSWCCreature* m_pCreature;
    CSWCCreature* m_pShownCreature;
    int m_nShownAlignment;
#endif
};

#endif
