#ifndef __SWGUICLASSSELECTION_H__
#define __SWGUICLASSSELECTION_H__

#include "swguicontrol.h"
#include "swgui.h"
#include "swguidrawable.h"
class CSWCCreature;
class CSWGuiControl;
#include "exobase.h"

extern int g_bDo3DGUIs;

#define CLASSSELECTION_TRANSITION_TIME 0.25f
#define CLASSSELECTION_TRANSITION_IDLE -1.0f

class CSWGuiManager;
class CSWGuiPanel;

class CSWGuiClassType
{
public:
    unsigned char m_nClass;
    unsigned char m_nGender;
    unsigned char m_nSoundSet;
    unsigned char m_bUnknown03;
    unsigned long m_nDescriptionStrRef;
};

class CSWGuiClassSelChar
{
public:
    CSWGuiClassSelChar();
    ~CSWGuiClassSelChar()  // PC 0x0074A600 GOG 0x00749830
    {
    }

    CSWGuiButton m_btnSelect;
    CSWGui3DSceneView m_viewModel;
    CSWCCreature* m_pCreature;
    float m_fTransitionTime;
};

class CSWGuiClassSelection : public CSWGuiPanel
{
public:
    CSWGuiClassSelection(CSWGuiManager* pManager, const CExoString& sModule);
    virtual ~CSWGuiClassSelection();
    virtual void Draw(float fElapsed);
    void HandleCancelButton();
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    void OnSelectButton(CSWGuiControl* pControl);
    void OnEnterButton(CSWGuiControl* pControl);
    void BeginPlayGame();

    static CSWGuiClassType CHAR_TYPES[6];

    enum
    {

        LOAD_STEPS = 16,
        PREVIEW_TOP_NORMAL = 167,
        PREVIEW_TOP_ACTIVE = 157,
        SOUNDSET_PLAYER_MALE = 85,
        SOUNDSET_PLAYER_FEMALE = 83,
        STRREF_MALE = 646,
        STRREF_FEMALE = 647,
        STRREF_JEDI_SENTINEL = 355,
        STRREF_JEDI_GUARDIAN = 353,
        STRREF_JEDI_CONSULAR = 354,
        STRREF_JEDI_CONSULAR_DESCRIPTION = 48031,
        STRREF_JEDI_SENTINEL_DESCRIPTION = 48032,
        STRREF_JEDI_GUARDIAN_DESCRIPTION = 48033
    };

    unsigned char m_aUnmapped64[4];
    CSWCCreature* m_pSelectedCreature;
    CSWGuiClassSelChar m_aCharacters[6];
    CSWGuiLabel m_lblCharGen;
    CSWGuiLabel m_lblInstruction;
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblBack;
#endif
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblClass;
#ifndef _XBOX
    CSWGuiButton m_btnBack;
#endif

    CExoString m_sModule;
};

#endif
