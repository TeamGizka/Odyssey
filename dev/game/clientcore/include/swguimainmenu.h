#ifndef __SWGUIMAINMENU_H__
#define __SWGUIMAINMENU_H__

#include "swguidrawable.h"
#include "swguicontrol.h"
#include "swgui.h"
class CSWGuiControl;
class CSWGuiManager;
class CSWCCreature;

class CSWGuiMainMenu : public CSWGuiPanel
{
public:
    class CErrorMessageData
    {
    public:
#ifdef _XBOX
        CErrorMessageData()
        {
            m_bHideButtons = 0;
            m_bAllowCancel = 0;
            m_pCallbackObject = 0;
            m_pfnCallback = 0;
        }
        CExoString m_sMessage;
        int m_bHideButtons;
        int m_bAllowCancel;
        CSWGuiObject* m_pCallbackObject;
        CSWGuiHandler m_pfnCallback;
#else
        CExoString m_sMessage;
        unsigned char m_aUnmapped08[0x10];
#endif
    };

    CSWGuiMainMenu(CSWGuiManager* pManager);
    virtual ~CSWGuiMainMenu();

    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();

    void LoadFromLayout(CSWGuiManager* pManager);
    void OnNewGamePicked(CSWGuiControl* pControl);
    void OnLoadSaveGame(CSWGuiControl* pControl);
    void OnMoviesPicked(CSWGuiControl* pControl);
    void OnMusicPicked(CSWGuiControl* pControl);
    void OnOptionsPicked(CSWGuiControl* pControl);
    void OnEnterButton(CSWGuiControl* pControl);
    void OnExitButton(CSWGuiControl* pControl);
    void LaunchDashboard(CSWGuiControl* pControl);
    void OnQuitButtonPressed(CSWGuiControl* pControl);
    void ClearListBox();
    void DeleteModuleList();
#ifdef _XBOX
    void AddErrorMessage(const CErrorMessageData& cMessage);

    void ShowErrorMessage();
    void OnErrorMessageDone(CSWGuiControl* pControl);
#endif
    void OnModulePicked(CSWGuiControl* pControl);
    void PopulateModules();
    void OnLoadGamePicked();
    void OnWarpButtonPressed(CSWGuiControl* pControl);

#ifdef _XBOX
    float m_fIdleTime;
#else
    int m_nSelectedModule;
#endif
    CSWGuiListBox m_lbModules;
    CExoArrayList<CExoString> m_lstModules;
    int m_nMenuState;
    CExoArrayList<CErrorMessageData> m_lstErrorMessages;
    CSWGui3DSceneView m_view3D;
    CSWGuiButton m_btnNewGame;
    CSWGuiButton m_btnLoadGame;
    CSWGuiButton m_btnMovies;
    CSWGuiButton m_btnMusic;
    CSWGuiButton m_btnOptions;
    CSWGuiButton m_btnCredits;
#ifdef _XBOX
    CSWGuiLabel m_lblWarp;
    CSWGuiLabel m_lblY;
#endif
    CSWGuiLabel m_lblNewContent;
    CSWGuiLabel m_alblBackdrop[2];
#ifdef _XBOX
    CSWGuiLabel m_lblObsidianLogo;
    CSWGuiLabel m_lblObsidianBuild;
#else
    CSWGuiLabel m_lblBioWare;
    CSWGuiLabel m_lblLucasArts;
    CSWGuiButton m_btnExit;
    CSWGuiButton m_btnWarp;
#endif
    int m_bReady;
    int m_nDrawCount;
#ifdef _XBOX
    Vector m_vButtonTextColor;
#endif
    CSWCCreature* m_pCreature;
    int m_bIdleTimerDisabled;
};

#include "swgui.h"
class CSWGuiManager;

class CSWGuiTitleMovies : public CSWGuiPanel
{
public:

    enum { STRREF_CREDITS = 47918 };

    CSWGuiTitleMovies(CSWGuiManager* pManager);
    virtual ~CSWGuiTitleMovies();
    virtual void HandleInputEvent(int nEvent, int nState);

    void OnMoviePicked(CSWGuiControl* pControl);
    void PopulateMovies();

    CSWGuiListBox m_lbxMovies;
    CSWGuiLabel m_lblTitle;
#ifdef _XBOX
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblPlayMovie;
    CSWGuiLabel m_lblUnlocked;
    CSWGuiLabel m_lblUnlockedValue;
    CSWGuiLabel m_alblBar[1];
#else
    CSWGuiButton m_btnBack;
    CSWGuiLabel m_alblBar[3];
    CSWGuiLabel m_lblUnlocked;
    CSWGuiLabel m_lblUnlockedValue;
#endif
    CExoArrayList<CExoString> m_lMovieNames;
};

#endif
