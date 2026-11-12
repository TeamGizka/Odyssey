#ifndef __SWGUIOPTIONS_H__
#define __SWGUIOPTIONS_H__

#include "swguiabilitieschargen.h"

class CSWGuiOptionsGraphics;

class CSWGuiOptionsCheckbox : public CSWGuiButtonToggle
{
public:
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
};

class CSWGuiOptionsMain : public CSWGuiPanel
{
public:
    CSWGuiOptionsMain(CSWGuiManager* pManager);

    void ClosePanel();
    virtual void HandleInputEvent(int nEvent, int nState);

    void SetDescription(unsigned long nStrRef);
    void OnEnterSlot(CSWGuiControl* pControl);
    void OnGameplayOpt(CSWGuiControl* pControl);
    void OnFeedbackOpt(CSWGuiControl* pControl);
    void OnAutopauseOpt(CSWGuiControl* pControl);
#ifndef _XBOX
    void OnGraphicsOpt(CSWGuiControl* pControl);
#endif
    void OnSoundOpt(CSWGuiControl* pControl);
#ifdef _XBOX

    void OnDownload(CSWGuiControl* pControl);

    enum { NUM_OPTIONS = 5 };
    CSWGuiButton m_btnOptions[NUM_OPTIONS];
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblSelect;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblRAnalog;
    CSWGuiListBox m_lbDescription;
#else
    CSWGuiButton m_btnOptions[6];
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblTitle;
    CSWGuiButton m_btnBack;
    CSWGuiLabel m_alblBar[3];
    CSWGuiListBox m_lbDescription;
#endif
};

class CSWGuiOptionsFeedback : public CSWGuiPanel
{
public:

    enum FeedbackOption
    {
        OPTION_HIDE_UNEQUIPPABLE = 0,
        OPTION_TUTORIAL_POPUPS,
        OPTION_SUBTITLES,
        OPTION_MINI_MAP,
        OPTION_FLOATING_NUMBERS,
        OPTION_STATUS_SUMMARY,
        OPTION_HIDE_IN_GAME_GUI,
        OPTION_USE_SMALL_FONTS,
        OPTION_ENABLE_TOOLTIPS,
        OPTION_COUNT
    };

    enum FeedbackStringRef
    {
        STRREF_HIDE_UNEQUIPPABLE_LABEL = 42279,
        STRREF_TUTORIAL_POPUPS_LABEL = 42280,
        STRREF_SUBTITLES_LABEL = 42281,
        STRREF_MINI_MAP_LABEL = 42283,
        STRREF_FLOATING_NUMBERS_LABEL = 42285,
        STRREF_STATUS_SUMMARY_LABEL = 42450,
        STRREF_HIDE_IN_GAME_GUI_LABEL = 49061,
        STRREF_USE_SMALL_FONTS_LABEL = 49062,
        STRREF_ENABLE_TOOLTIPS_LABEL = 49115,

        STRREF_HIDE_UNEQUIPPABLE_DESCRIPTION = 42286,
        STRREF_TUTORIAL_POPUPS_DESCRIPTION = 42287,
        STRREF_SUBTITLES_DESCRIPTION = 42288,
        STRREF_MINI_MAP_DESCRIPTION = 42289,
        STRREF_FLOATING_NUMBERS_DESCRIPTION = 42291,
        STRREF_STATUS_SUMMARY_DESCRIPTION = 42451,
        STRREF_HIDE_IN_GAME_GUI_DESCRIPTION = 49065,
        STRREF_USE_SMALL_FONTS_DESCRIPTION = 49066,
        STRREF_ENABLE_TOOLTIPS_DESCRIPTION = 49114
    };

#ifndef _XBOX
    CSWGuiOptionsFeedback(CSWGuiManager* pManager); virtual ~CSWGuiOptionsFeedback();
#else
    CSWGuiOptionsFeedback(CSWGuiManager* pManager);
#endif

    virtual void HandleInputEvent(int nEvent, int nState);

    void SetDescription(unsigned long nStrRef);
    void SetActiveButton(int nOption);
    void SetUpOptions();
    void PopulateOptionsListbox();
    void OnEnterSlot(CSWGuiControl* pControl);
    void OnDefault(CSWGuiControl* pControl);
    void OnHide(CSWGuiControl* pControl);
    void OnTutorial(CSWGuiControl* pControl);
    void OnSubtitles(CSWGuiControl* pControl);
    void OnMiniMap(CSWGuiControl* pControl);
    void OnFloaty(CSWGuiControl* pControl); void OnSummary(CSWGuiControl* pControl);
    void OnHideInGameGUI(CSWGuiControl* pControl); void OnUseSmallFont(CSWGuiControl* pControl); void OnEnableToolTips(CSWGuiControl* pControl);

#ifdef _XBOX
    CSWGuiOptionsCheckbox m_aCheckboxes[7]; CSWGuiLabel m_lblA; CSWGuiLabel m_lblB; CSWGuiLabel m_lblDescription; CSWGuiLabel m_lblSelect;
    CSWGuiLabel m_lblBack; CSWGuiLabel m_lblTitle; CSWGuiLabel m_lblRAnalog; CSWGuiListBox m_lbDescription; void OnRumble(CSWGuiControl* pControl);
#else
    CSWGuiListBox m_lbOptions; CSWGuiLabel m_lblDescription; CSWGuiLabel m_lblTitle; CSWGuiListBox m_lbDescription; CSWGuiButton m_btnBack; CSWGuiButton m_btnDefault;
    CSWGuiLabel m_alblBar[3];
#endif
};

class CSWGuiOptionsSound : public CSWGuiPanel
{
public:
    CSWGuiOptionsSound(CSWGuiManager* pManager);

    virtual void HandleInputEvent(int nEvent, int nState);

    void SetDescription(unsigned long nStrRef);
    void SetUpOptions();
    void OnMoveVolumeSlider(CSWGuiControl* pControl);
    void OnEnterSlider(CSWGuiControl* pControl);
    void OnExitSlider(CSWGuiControl* pControl);
    void OnDefault(CSWGuiControl* pControl);
    void OnAdvanced(CSWGuiControl* pControl);

#ifdef _XBOX
    CSWGuiSlider m_sldGamma; CSWGuiSlider m_sldMusic; CSWGuiSlider m_sldVoice; CSWGuiSlider m_sldEffects; CSWGuiSlider m_sldMovie;
    CSWGuiLabel m_lblGamma; CSWGuiLabel m_lblMusic; CSWGuiLabel m_lblEffects; CSWGuiLabel m_lblVoice; CSWGuiLabel m_lblMovie;
    CSWGuiLabel m_lblB; CSWGuiLabel m_lblDescription; CSWGuiLabel m_lblBack; CSWGuiLabel m_lblTitle; CSWGuiLabel m_lblRAnalog;
    CSWGuiListBox m_lbDescription; CSWGuiLabel m_lblY; CSWGuiLabel m_lblDefault;
    void OnMoveGammaSlider(CSWGuiControl* pControl);
#else
    CSWGuiSlider m_sldMusic; CSWGuiSlider m_sldVoice;
    CSWGuiSlider m_sldEffects; CSWGuiSlider m_sldMovie;
    CSWGuiLabel m_lblMusic; CSWGuiLabel m_lblEffects; CSWGuiLabel m_lblVoice; CSWGuiLabel m_lblMovie;
    CSWGuiLabel m_lblDescription; CSWGuiLabel m_lblTitle;
    CSWGuiListBox m_lbDescription;
    CSWGuiButton m_btnBack; CSWGuiButton m_btnDefault; CSWGuiButton m_btnAdvanced;
    CSWGuiLabel m_alblBar[4];
#endif
};

class CSWGuiOptionsSoundAdvanced : public CSWGuiPanel
{
public:
    CSWGuiOptionsSoundAdvanced(CSWGuiManager* pManager);

    virtual void HandleInputEvent(int nEvent, int nState);

    void SetUpOptions();
    void SetNewOptions();
    void SetEAXText();
    void SetDescription(unsigned long nStrRef);
    void EnableEAXControls(int bEnable);
    void OnEnterOption(CSWGuiControl* pControl);
    void OnDiscardChanges(CSWGuiControl* pControl);
    void OnEAXLeft(CSWGuiControl* pControl);
    void OnEAXRight(CSWGuiControl* pControl);
    void OnForceSoftware(CSWGuiControl* pControl);
    void OnDefault(CSWGuiControl* pControl);

    CSWGuiOptionsCheckbox m_cbForceSoftware;
    int m_nEAXMode;
    CSWGuiButton m_btnEAX;
    CSWGuiButton m_btnEAXRight;
    CSWGuiButton m_btnEAXLeft;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblTitle;
    CSWGuiListBox m_lbDescription;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnDefault;
    CSWGuiButton m_btnCancel;
    CSWGuiLabel m_alblBar[4];
    int m_bForceSoftware;
    int m_bChanged;
    int m_bDiscard;
};

class CSWGuiOptionsMouse : public CSWGuiPanel
{
public:
    CSWGuiOptionsMouse(CSWGuiManager* pManager);

    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();

    void SetDescription(unsigned long nStrRef);
    void SetUpOptions();
    void OnEnterSlider(CSWGuiControl* pControl);
    void OnEnterRevMouseButtons(CSWGuiControl* pControl);
    void OnMoveMouseSenSlider(CSWGuiControl* pControl);
    void OnExitSlider(CSWGuiControl* pControl);
    void OnRevMouseButtons(CSWGuiControl* pControl);
    void OnDefault(CSWGuiControl* pControl);

    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblTitle;
    CSWGuiListBox m_lbDescription;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnDefault;
    CSWGuiSlider m_sldMouseSen;
    CSWGuiLabel m_lblMouseSen;
    CSWGuiOptionsCheckbox m_cbRevButtons;
    CSWGuiLabel m_alblBar[4];
};

class CSWGuiOptionsResolution : public CSWGuiPanel
{
public:
    CSWGuiOptionsResolution(CSWGuiManager* pManager);

    virtual void HandleInputEvent(int nEvent, int nState);

    void OnResolutionChosen(CSWGuiControl* pControl);

    CSWGuiLabel m_lblResolution;
    CSWGuiListBox m_lbResolutions;
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnBack;
    int m_nUnknown80C;
};

class CSWGuiOptionsGraphics : public CSWGuiPanel
{
public:
    CSWGuiOptionsGraphics(CSWGuiManager* pManager);

    virtual void HandleInputEvent(int nEvent, int nState);

    void SetDescription(unsigned long nStrRef);
    void SetUpOptions();
    void OnMoveGammaSlider(CSWGuiControl* pControl);
    void OnEnterSlider(CSWGuiControl* pControl);
    void OnExitSlider(CSWGuiControl* pControl);
    void OnEnterSlot(CSWGuiControl* pControl);
    void OnDefault(CSWGuiControl* pControl);
    void OnShadows(CSWGuiControl* pControl);
    void OnGrass(CSWGuiControl* pControl);
    void OnResolution(CSWGuiControl* pControl);
    void OnAdvanced(CSWGuiControl* pControl);

    CSWGuiSlider m_sldGamma;
    CSWGuiLabel m_lblGamma;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblTitle;
    CSWGuiListBox m_lbDescription;
    CSWGuiButton m_btnResolution;
    CSWGuiButton m_btnResolutionLeft;
    CSWGuiButton m_btnResolutionRight;
    CSWGuiOptionsCheckbox m_cbShadows;
    CSWGuiOptionsCheckbox m_cbGrass;
    CSWGuiButton m_btnAdvanced;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnDefault;
    CSWGuiLabel m_alblBar[4];
    int m_nUnknown18BC;
    int m_n1E88;
};

class CSWGuiOptionsGraphicsAdvanced : public CSWGuiPanel
{
public:
    CSWGuiOptionsGraphicsAdvanced(CSWGuiManager* pManager,
                                  CSWGuiOptionsGraphics* pGraphics);

    virtual void HandleInputEvent(int nEvent, int nState);

    void SetDescription(unsigned long nStrRef);
    void SetUpOptions();
    void SetNewOptions();
    void SetTexQualText();
    void SetAntiAliasText();
    void SetAnisotropyText();
    void OnEnterSlot(CSWGuiControl* pControl);
    void OnDefault(CSWGuiControl* pControl);
    void OnDiscardChanges(CSWGuiControl* pControl);
    int OnFrameBuffer(CSWGuiControl* pControl);
    void OnSoftShadows(CSWGuiControl* pControl);
    int OnVSync(CSWGuiControl* pControl);
    void OnTexQualLeft(CSWGuiControl* pControl);
    void OnTexQualRight(CSWGuiControl* pControl);
    void OnAntiAliasLeft(CSWGuiControl* pControl);
    void OnAntiAliasRight(CSWGuiControl* pControl);
    void OnAnisotropyLeft(CSWGuiControl* pControl);
    void OnAnisotropyRight(CSWGuiControl* pControl);

    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblTitle;
    CSWGuiListBox m_lbDescription;
    CSWGuiOptionsCheckbox m_cbFrameBuffer;
    CSWGuiOptionsCheckbox m_cbSoftShadows;
    CSWGuiOptionsCheckbox m_cbVSync;
    CSWGuiButton m_btnAnisotropy;
    CSWGuiButton m_btnAnisotropyLeft;
    CSWGuiButton m_btnAnisotropyRight;
    CSWGuiButton m_btnAntiAlias;
    CSWGuiButton m_btnAntiAliasLeft;
    CSWGuiButton m_btnAntiAliasRight;
    CSWGuiButton m_btnTexQual;
    CSWGuiButton m_btnTexQualLeft;
    CSWGuiButton m_btnTexQualRight;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnDefault;
    CSWGuiButton m_btnCancel;
    CSWGuiLabel m_alblBar[4];
    int m_nTexQual;
    int m_nMaxTexQual;
    int m_nAntiAlias;
    int m_nAnisotropy;
    int m_bFrameBuffer;
    int m_bVSync;
    int m_bSoftShadows;
    int m_bDiscard;
    int m_bChanged;
    CSWGuiOptionsGraphics* m_pGraphics;
};

#endif
