#ifndef __SWGUISAVELOAD_H__
#define __SWGUISAVELOAD_H__

#include "swguicontrol.h"
#include "swgui.h"
#include "serverexoappinternal.h"
class CResGFF;
class CResStruct;
class CSWGuiControl;
class CSWGuiManager;
class CSWGuiSaveLoadEntry;
class CSWGuiLabel_vtbl;
class CSWGuiSaveGameEditBox_vtbl;
class CSWGuiSaveLoadEntry;

extern int g_bEnableLiveContentIgnore;

class CSWGuiSaveLoad : public CSWGuiPanel
{
public:
    CSWGuiSaveLoad(CSWGuiManager* pManager, int bSaveMode, int bExitToMainMenu,
                   int bEndGameMode = 0);
    virtual ~CSWGuiSaveLoad();
    virtual void HandleInputEvent(int nEvent, int nState);

    unsigned long m_bSaveMode : 1;
    unsigned long m_bExitToMainMenu : 1;
    unsigned long m_bFiltersLoaded : 1;
    unsigned long m_nShownHelpFlags : 29;
    int m_nMaxSaveGame;

    CExoString m_sResourceDirectory;
    CSWGuiLabel m_lblPanelName;
    CSWGuiLabel m_lblScreenshot;
    CSWGuiLabel m_lblPlanetName;
    CSWGuiLabel m_lblAreaName;

    CSWGuiLabel m_aPartyMemberLabels[3];
    CSWGuiListBox m_lbSaveGames;
    CSWGuiLabel m_lblPCName;
    CSWGuiLabel m_lblTimePlayed;
#ifdef _XBOX
    CSWGuiLabel m_lblFreeblocks;
    CSWGuiLabel m_lblFreeblocksAmt;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblSaveload;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblDelete;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblFilter;
#else
    CSWGuiButton m_btnSaveLoad;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnDelete;
    CSWGuiButton m_btnFilter;
#endif
    CSWGuiLabel m_alblBar[4];
#ifdef _XBOX
    Vector m_vTextColor;
#endif
    CExoArrayList<CSWGuiSaveLoadEntry*> m_lEntries;
    int m_nCurrentFilter;
    CExoArrayList<CExoString> m_lFilterNames;
    int m_nFilterCount;
    int m_bEndGameMode;
    void SetEndGameMode();
    int LaunchMostRecentSave();
    void HandleFilterPress(CSWGuiControl* pControl);
    void PopulateGameLB();
    void SortGamesByTimeStamp(CExoArrayList<CSWGuiControl*>& lControls);
#ifndef _XBOX
    void SelectValidSaveDirectory();
    void OnSteamCloudSaves(CSWGuiControl* pControl);
    void ShowTooManyCloudSavesDialog();
    void SwapSteamCloudSaves();
#endif
#ifdef _XBOX
    void UpdateFreeBlocks();
    void ShowSaveError(unsigned long nStrRef);
    void OnSaveErrorDone(CSWGuiControl* pControl);
    void OnCorruptGameSelected(CSWGuiControl* pControl);
    void LaunchDashboard(CSWGuiControl* pControl);
    void PromptLaunchDashboard(CSWGuiControl* pControl);
    int LoadPCAutoSave();
    void SaveGameToSlot(CSWGuiControl* pControl);
#endif

#ifndef _XBOX
    void WriteGame(const CExoString& sSaveName);
#endif

    static const unsigned long MAX_SAVE_GAME = 999;
    static const unsigned long STRREF_MAXIMUM_SAVES = 42490;
    void RemoveGame(CSWGuiControl* pControl);
    void ShowGame(CSWGuiControl* pControl);
    int CopyGameToFutureGame(unsigned long nSaveGame,
                             const CExoString& sSaveName);
    void DisplayLoadError(unsigned long nStrRef);
    void LoadPCAutoSave(const CExoString& sModuleName);
    void DeleteGame(CSWGuiControl* pControl);
    int UnpackGame(unsigned long nSaveGame, const CExoString& sSaveName,
                   CSWGuiControl* pControl);
    void LoadGame(CSWGuiControl* pControl);

    static const unsigned long STRREF_MISSING_LIVE_CONTENT = 0;
    static const int MISSING_LIVE_CONTENT_TOKEN = 0;
#ifndef _XBOX
    void PromptForSaveName(CSWGuiControl* pControl);
#endif
    void HandleSaveButton(CSWGuiControl* pControl);
    void SaveNamePanelClose(int bSave, CExoString sSaveName);

    static const unsigned long STRREF_CONFIRM_OVERWRITE = 1591;
    static const unsigned long STRREF_SAVE_SPACE_REQUIRED = 48754;
    static const int SAVE_SPACE_TOKEN_MB = 0;

    static const int SAVE_SPACE_UNITS = 1600;
    static const int OPTIONS_SPACE_UNITS = 2;
    void VerifyLoadGame(CSWGuiControl* pControl);

    static const unsigned long STRREF_CONFIRM_LOAD_GAME = 32155;

    static const unsigned long STRREF_NEW_SLOT = 1590;

    static const unsigned long STRREF_NO_SAVED_GAMES = 42491;
    enum { STRREF_NO_SAVES = 128527, STRREF_TIME_PLAYED = 48755 };
    void ExitPanel();
    void PopulateGameList();

    static const unsigned long STRREF_SAVE = 1587;
    static const unsigned long STRREF_SAVE_GAME = 1588;
    static const unsigned long STRREF_LOAD = 1589;
    static const unsigned long STRREF_LOAD_GAME = 1585;
};

#ifndef _XBOX
class CSWGuiSaveGameEditBox : public CSWGuiEditbox
{
public:
    virtual void HandleKeyPress(int);
};
#endif

#ifndef _XBOX
#pragma pack(push, 4)
#endif
class CSWGuiSaveLoadEntry : public CSWGuiRadioButton
{
public:

    enum SaveSlot
    {
        SAVE_SLOT_QUICK = 0,
        SAVE_SLOT_AUTO = 1,

        SAVE_SLOT_FIRST_NORMAL = 2
    };
    enum SaveTimeToken
    {
        TIME_TOKEN_HOURS = 0,
        TIME_TOKEN_MINUTES = 1
    };
    enum SaveTitleToken
    {
        TITLE_TOKEN_NUMBER = 0,
        TITLE_TOKEN_NAME = 1,
        TITLE_TOKEN_TIME_STAMP = 2
    };
    enum SaveStampToken
    {
        STAMP_TOKEN_MONTH = 0,
        STAMP_TOKEN_DAY = 1,
        STAMP_TOKEN_YEAR = 2,
        STAMP_TOKEN_HOUR = 3,
        STAMP_TOKEN_MINUTE = 4,
        STAMP_TOKEN_SECOND = 5
    };
    enum
    {
        STRREF_AUTO_SAVE = 1593,
        STRREF_QUICK_SAVE = 48752,
        STRREF_SAVE_TITLE = 48753,
        STRREF_TIME_STAMP = 49126
    };

    unsigned long m_bDataLoaded : 1;

    unsigned long m_bBadSave : 1;
    unsigned long m_bNewSlot : 1;
    unsigned long m_bRebootAutoSave : 1;
#ifndef _XBOX
    unsigned long m_bPCAutoSave : 1;
#endif
    unsigned long m_bMissingLiveContent : 1;
    unsigned long m_bEmptySlot : 1;
    unsigned long m_bCheatUsed : 1;
    int m_nSaveGame;
    int GetSaveGame() { return m_nSaveGame; }
    unsigned long m_nTimePlayed;
    unsigned long GetTimePlayed() { return m_nTimePlayed; }

    int m_nSaveSpaceNeeded;
    int GetSaveSpaceNeeded() { return m_nSaveSpaceNeeded; }
    unsigned char m_nGameplayHint;
    unsigned char GetGameplayHint() { return m_nGameplayHint; }
#ifdef _XBOX
    enum { STORY_HINT_AREAS = 10, SAVE_ENTRY_HEIGHT = 32 };
#else
    enum { STORY_HINT_AREAS = 10, SAVE_ENTRY_HEIGHT = 36 };
#endif
    unsigned char m_anStoryHint[STORY_HINT_AREAS];
    unsigned char GetStoryHint(unsigned char nArea) { return m_anStoryHint[nArea]; }
#ifndef _XBOX
    CExoString m_sSaveGameName;
#endif
    CExoString m_sGameName;
    CExoString m_sAreaName;
    CExoString m_sLastModule;
    CExoString m_sSaveDirectory;
    CExoString m_sXboxTitle;
    CExoString m_sScreenshot;

    CExoString m_sMissingLiveContent;

    CResRef m_aPortraits[3];
    unsigned __int64 m_nTimeStamp;
    unsigned __int64 GetTimeStamp() { return m_nTimeStamp; }
    CExoString m_sPCName;
    int m_nSaveNumber;
    int GetSaveNumber() { return m_nSaveNumber; }
    CExoString GetPCName() { return m_sPCName; }

    CExoString GetGameDirectory();
    CResRef GetPlayerPortrait(int nIndex);
    void LoadData(CExoString const&);
    void SetXboxTitle(CExoString const&);
    CExoString FormatTimeToString();
    CSWGuiSaveLoadEntry();
    CSWGuiSaveLoadEntry(CSWGuiSaveLoadEntry* pEntry);
};
#ifndef _XBOX
#pragma pack(pop)
#endif

#ifndef _XBOX
class CSWGuiSaveNamePanel : public CSWGuiPanel
{
public:
    CSWGuiSaveLoad* m_pSaveLoad;
    CSWGuiButton m_btnSave;
    CSWGuiButton m_btnCancel;
    CSWGuiSaveGameEditBox m_edtSaveName;
    CSWGuiLabel m_lblSaveName;
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();
    void SetTextAndPointer(CSWGuiSaveLoad* pSaveLoad, const CExoString& sText);
    void HandleCancelButton(CSWGuiControl*);
    void HandleOkButton(CSWGuiControl*);
    CSWGuiSaveNamePanel(CSWGuiManager*);
    virtual void HandleInputEvent(int nEvent, int nState)  // PC 0x0070A780 GOG 0x0070A4C0
    {
        switch (nEvent)
        {
        case GUIEVENT_ACTIVATE:
        case GUIEVENT_ACCEPT:
            HandleOkButton(NULL);
            break;
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            HandleCancelButton(NULL);
            break;
        }
    }
};
#endif

class KOTOR_AUTOSAVE_PARAMS
{
public:
    CExoString m_sLoadMusic;
    CExoString m_sStartWaypoint;
    CExoString m_sMovie1;
    CExoString m_sMovie2;
    CExoString m_sMovie3;
    CExoString m_sMovie4;
    CExoString m_sMovie5;
    CExoString m_sMovie6;
    int m_nTimeYear;
    unsigned char m_nTimeMonth;
    unsigned char m_nTimeDay;
    unsigned char m_nTimeHour;
    unsigned short m_nTimeMinute;
    unsigned short m_nTimeSecond;
    unsigned short m_nTimeMillisecond;
    int m_nTimePauseDay;
    int m_nTimePauseTime;
    CStatusSummary m_cStatusSummary;
    KOTOR_AUTOSAVE_PARAMS()  // PC 0x004EC380 GOG 0x004EAE70
    {
        Reset();
    }
    int Reset()  // PC 0x004EA110 GOG 0x004E99A0
    {
        m_sStartWaypoint = "";
        m_sLoadMusic = "";
        m_sStartWaypoint = "";
        m_sMovie1 = "";
        m_sMovie2 = "";
        m_sMovie3 = "";
        m_sMovie4 = "";
        m_sMovie5 = "";
        m_sMovie6 = "";
        m_nTimeYear = 0;
        m_nTimeMonth = 0;
        m_nTimeDay = 0;
        m_nTimeHour = 0;
        m_nTimeMinute = 0;
        m_nTimeSecond = 0;
        m_nTimeMillisecond = 0;
        m_nTimePauseDay = 0;
        m_nTimePauseTime = 0;
        memset(&m_cStatusSummary, 0, sizeof(m_cStatusSummary));
        return 0;
    }
    int SaveToGFF(CResGFF* pRes, CResStruct& stStruct);
    void LoadFromGFF(CResGFF* pGFF, CResStruct& cStruct);
};

#endif
