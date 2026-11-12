#ifndef __CLIENTEXOAPP_H__
#define __CLIENTEXOAPP_H__

#define GUI_STRREF_ITEM_RANGE 38565

#define GUI_STRREF_ITEM_ON_HIT 38583
#define GUI_STRREF_CHANCE 38574
#define GUI_STRREF_SECONDS 47878
#define GUI_STRREF_DC 48756

#define GUI_STRREF_ITEM_DEFENCE_BONUS 38593
#define GUI_STRREF_ITEM_MAX_DEXTERITY_BONUS 42144
#define GUI_STRREF_VERSUS 1395

#define GUI_STRREF_DAMAGE_PHYSICAL 38552

#define GUI_STRREF_ITEM_SINGLE_USE 783
#define GUI_STRREF_ITEM_USES 41899
#define GUI_STRREF_UNLIMITED 41900
#define GUI_STRREF_ITEM_SAVES 41935
#define GUI_STRREF_ITEM_SKILLS 41936
#define GUI_STRREF_ITEM_FORCE_RESIST 41930
#define GUI_STRREF_ITEM_VULNERABILITY 41898
#define GUI_STRREF_ITEM_RESTRICTED_TO 41934

#define GUI_STRREF_ITEM_ATTACK_MODIFIER 38566

const unsigned short FEEDBACK_MESSAGE_JOURNAL_UPDATED = 184;

const unsigned short FEEDBACK_MESSAGE_CHARACTER_EXPORT_SUCCESS = 216;
const unsigned short FEEDBACK_MESSAGE_CHARACTER_EXPORT_FAILED = 217;

const unsigned short FEEDBACK_MESSAGE_SAVE_GAME_STARTED = 225;
const unsigned short FEEDBACK_MESSAGE_SAVE_GAME_FINISHED = 226;

class CClientAIMaster;
class CExoFile;
class CExoStreamingSoundSource;
class CGuiInGame;
class CResRef;
class CSoundSetList;
class CSWCCMessageData;
class CSWCPlaceable;
class CSWCSoundObject;
class CSWCTrigger;
class CSWMiniGame;
class CSWMiniGameObjectArray;
class CSWSDialog;
class CServerInfo;
class Vector;
class CAurCamera;
#include "exobase.h"
#include "gameobjectarray.h"
#include "netlayer.h"

class CSWCArea;
class CSWCDoor;
class CSWCProjectile;

class CClientExoAppInternal;

enum ClientInputClass
{
    INPUT_CLASS_GAME = 0,

    INPUT_CLASS_MINIGAME = 1,
    INPUT_CLASS_GUI = 2,

    INPUT_CLASS_DIALOG = 3,
    INPUT_CLASS_FREE_LOOK = 4
};

enum ClientMouseHideFlags
{

    MOUSE_HIDE_MINIGAME = 1,
    MOUSE_HIDE_CAMERA_ROTATE = 0x20
};

#define MOUSE_HIDE_ALL 0xffff
class CNetLayer;
class CSWCPlayer;
class CSWGuiInGame;
class CSWGuiManager;
class CSWCCreature;
class CSWCArea;
class CWorldTimer;
class CSWCModule;
class CSWCObject;
class CAurObject;
template <class T> class List;
class CGameObjectArray;
class CClientOptions;
class CSWParty;
class CSWCItem;
class CSWCModule;
class CSWCJournal;
class CSWCMessage;
class CSWCAreaOfEffectObject;
class CSWCWorldJournal;

class CExoLinkedList;
class CExoString;

class CConnectionLib;
class CExoImeUI;

extern float g_fFPS;

class CClientExoApp : public CBaseExoApp
{
public:

    enum ServerMode
    {
        SERVER_MODE_RESET = 0,
        SERVER_MODE_RUN_REQUESTED = 1,
        SERVER_MODE_CHARACTER_LOGIN = 2
    };

    static const unsigned char TUTORIAL_FORCE_POWER = 3;
    static const unsigned char TUTORIAL_HOSTILE_FORCE_POWER = 4;

    static const unsigned char TUTORIAL_JOURNAL = 14;

    static const unsigned long TUTORIAL_BASH = 33;

    static const unsigned char TUTORIAL_ATTACK = 34;
    static const unsigned char TUTORIAL_ATTACK_MASH = 35;
    static const unsigned long ATTACK_MASH_TUTORIAL_DELAY = 3000;

    static const unsigned long TUTORIAL_RETURN_TO_EBON_HAWK = 39;

    static const unsigned char TUTORIAL_PARTY_SELECTION = 41;

    static const unsigned long TUTORIAL_EQUIPMENT = 11;

    CClientExoApp();
    ~CClientExoApp();

    virtual void PlayerListChange(unsigned long nPlayerId, int bAdded,
                                  int bGuaranteed);
    virtual int HandleMessage(unsigned long nPlayerId, unsigned char* pMessage,
                              unsigned long nSize, int bGuaranteed);
    virtual float GetFPS() { return g_fFPS; }  // PC 0x00639650
    virtual int ContinueMessageProcessing();
    virtual CNetLayer* GetNetLayer();
    virtual void ShutDownToMainMenu();
    virtual CConnectionLib* GetConnectionLib();
    virtual CExoLocString* GetModuleDescription();
    virtual int GetApplicationId();

    CSWCModule* GetModule();
    CSWCMessage* GetSWCMessage();
    CGuiInGame* GetInGameGui();
    CSWGuiManager* GetSWGuiManager();
    CSWCCreature* GetCreatureByGameObjectID(unsigned long nId);
    CSWCAreaOfEffectObject* GetAreaOfEffectByGameObjectID(
        unsigned long oidObject);
    CSWCSoundObject* GetSoundObjectByGameObjectID(unsigned long oidObject);
    CSWCWorldJournal* GetWorldJournal();
    int GetInStartNewModule();
    void SetInStartNewModule(int bInStartNewModule);
    CSWCDoor* GetDoorByGameObjectID(unsigned long oidObject);
    CSWCPlaceable* GetPlaceableByGameObjectID(unsigned long oidObject);
    void SetPausedByCombat(int nA, unsigned char nB, int nC);

    enum AutoPauseReason
    {

        AUTOPAUSE_ENEMY_SIGHTED = 1,
        AUTOPAUSE_END_OF_COMBAT_ROUND = 5,

        AUTOPAUSE_PARTY_MEMBER_KILLED = 9,
        AUTOPAUSE_MINE_SIGHTED = 11
    };
    static const unsigned char TUTORIAL_ENEMY_SIGHTED = 21;
    int GetPausedByCombat();
    CSWMiniGame* GetMiniGame();
    void SetAutoPaused(int bAuto, unsigned char nB);
    int GetCombatMode();
    int GetAutoPaused();
    void PlayerFlourishWeapons();
    CWorldTimer* GetWorldTimer();
    CWorldTimer* GetPauseTimer();
    CSWCObject* GetGameObject(unsigned long oidObject);
    unsigned long ServerToClientObjectId(unsigned long oidServer);
    void BarkString(unsigned long oidObject, const CExoString& sText,
                   const CResRef& cSound, int nFlags);
    void StopSoundPlayingInDialog();
    int IsSoundPlayingInDialog();
    CGameObjectArray* GetGameObjectArray();
    CSWMiniGameObjectArray* GetMGOArray();
    int GetHitCheckMaterialMask();
    CClientOptions* GetClientOptions();
    int GetInputClass();
    int GetInFreeLook();
    CExoString GetDisplayCharacterForKey(int nKey);
    int GetPazaakResults(int* pnResult, int* pnUnused);
    void SetEndGamePending(int bPending, float fDelay, int nC);
    void EnableVideoEffect(int nEffect);
    void DisableVideoEffect();
    int GetVideoEffectType();
    CSWParty* GetSWParty();
    int GetPlayerMovementInput();
    CSWCCreature* GetPlayerCreature();
    unsigned long GetPlayerCreatureId();

    enum { GUI_STRREF_ITEM_PROPERTY_HEADING = 0x609, GUI_STRREF_NONE = 363,
           GUI_STRREF_ATTACK = 375, GUI_STRREF_DAMAGE = 31385,
           GUI_STRREF_PHYSICAL = 38552,
           GUI_STRREF_FEATS_REQUIRED = 38536,
           GUI_STRREF_ITEM_CRITICAL_THREAT = 38571,
           GUI_STRREF_NO_DESCRIPTION = 32172 };

    enum { GUI_STRING_SECURITY = 329, GUI_STRING_BASH = 368 };

    enum
    {
        GUI_STRING_OPEN = 365,
        GUI_STRING_USE = 366,
        GUI_STRING_DISABLE_MINE = 370,
        GUI_STRING_DIALOG = 371,
        GUI_STRING_RECOVER_MINE = 1531,
        GUI_STRING_NO_ACTION = 32236,
        GUI_STRING_SEARCH = 48182
    };
    CExoString GetGUIString(unsigned long nStrRef);
    void GetGUIString(CExoString& sText, unsigned long nStrRef);
    void GetGUIString(CExoString* pText, unsigned long nStrRef);
    int GetCharacterChangeInProgress();

    enum { TUTORIAL_MAP = 13 };
    int ShowTutorialWindow(unsigned char nTutorial, unsigned long nX,
                           unsigned long nY, unsigned long nActionParam);
    int SaveOptions();
    int LoadOptions(int bSetAntiAliasing);
#ifdef _XBOX

    void SetMinGlobalRim(int bMinGlobalRim);
#endif
    int SetTexturePack(unsigned char nTexturePack);
    void SetMouseMode(unsigned char nMode);
    CSWCJournal* GetQuestJournal();
    int GetClientLanguage();
    void ExitProgram();
    void BusyWaitMenuMusic();
    CExoString GetClientName();
    CExoString& SetPlayerCharacterName(const CExoString& sName);
    void DisableInput();
    CExoString GetLoadMusicByModuleName(const CExoString& sModule);
    void SetLoadMusic(const CExoString& sMusic);
    void SetLoadScreenByModuleName(const CExoString& sModule);
    void ShowLoadScreen(int bFade, int bPlayMusic, int bSaving);
    int GetNextLoadScreenHintSTRREF(const CExoString& sModule,
                                    unsigned char nB);
    void SetLoadScreenHint(unsigned long nHint);
    unsigned long GetLoadScreenHint();
    void ShowMouse(int nFlags);
    void HideMouse(int nFlags);
    int GetLastTarget();
    CAurCamera* GetModuleCamera();
    int ChangeCharacterToNextLivingPartyMember(int nA, int nB,
                                               unsigned char nC);
#ifdef _XBOX
    int ChangeCharacterToNextLivingPartyMember(int nA, int nB);
#endif
#ifdef _XBOX

    void SetInputClass(int nClass);
    void SetInputClass(int nClass, int) { SetInputClass(nClass); }
#else
    void SetInputClass(int nClass, int bHideInterface);
#endif
    CSWCItem* GetItemByGameObjectID(unsigned long nId);
    int CreatureAcquireItem(unsigned long oidObject, const CResRef& cResRef);
    void EndPazaakGame(int nResult, int nWager, int bPlayAgain,
                      unsigned long oidOpponent);
    int SetVideoMode(unsigned long nVideoMode);
    int IsValidResolution(unsigned long nWidth, unsigned long nHeight);
    int IsMoviePlaying();
    int GetCreditSequenceInProgress();
    void SetCreditSequenceInProgress(int bInProgress);
    void SetInputActive(int nA, int nB);
    int GetCaptureEvents();
    void SetCaptureEvents(const CExoString& sFileName);
    CExoFile* GetCaptureEventFile();
    void TurnOffKeyboardModal();
    int GetLoadBarState();
    void EndLoadBar();
    void AddFeedbackMessage(unsigned short nType, CSWCCMessageData* pMessage,
                            const CExoString& sString);
    int GetCaptureEventPlayBack();

    void SetCaptureEventPlayBack(const CExoString& sFileName);
    void SetCaptureEventPlayBack(int nRate);
    int IsClientImeUI();
    CExoImeUI* GetImeUI();
    void CancelMovie();
    void StopMovie();
    void QueryExitProgram();
    void RestartMoviesIfNecessary();
    int IsClientInitialized();
    void SetEventDescriptions();
    int Initialize();
    int Uninitialize();
#ifndef _XBOX
    int StartServices(int nServices, const CExoString& sArg2,
                      const CExoString& sArg3, const CExoString& sArg4,
                      unsigned long nArg5);
#endif
    void StopServices();
    void SetLoadStep(int nStep, unsigned char cValue);
    void SetLoadBarProgress(int nProgress, int bUpdateHint);
    void BeginIntro();
    void DisplayMainMenu(int bUnknown);
#ifdef _XBOX

    void StartServices(int nServices, const CExoString& sArg2, const CExoString& sArg3, const CExoString& sArg4, unsigned long nArg5);
    void DisplayStartScreen();
    void ShutDownLegalScreen();
#endif
    void StartMenuMusic(int bCultTheme, int nVolume, int nPriorityGroup);
    void StopMenuMusic();
    void StopLoadMusic();
    CExoStreamingSoundSource* AquireLoadMusic(const CResRef& cResRef);
#ifdef _XBOX

    void HideLoadScreen(int bFade);
    void HideLoadScreen(int bFade, int bUpdateScreen) { HideLoadScreen(bFade); }
#else
    void HideLoadScreen(int bFade, int bStopLoadMusic, int bUpdateScreen = 1);
#endif
    int MainLoop();
    void EnableMouseAtEndOfLoadModuleMovie();

    enum { RUMBLE_TABLE_FOOTSTEPSOUNDS = 0, RUMBLE_TABLE_VISUALEFFECTS = 1 };
    void LookUpAndPerformRumbleWithCutOff(int nEntry, unsigned char nTable,
                                          const Vector& vPosition);
    void JournalUpdated(int nType, int nIndex, const CExoString& sName);
    int GetObjectName(unsigned long oidObject, CExoString& sName);
    void SetCharacterFile(unsigned long nDataSize, unsigned char* pCharacterData,
                          int nCharacterFileId);
    void UnloadModule();
    CSWCModule* CreateModule();
    CWorldTimer* GetAnimationTimer();
    CSWCPlayer* GetPlayerByGameObjectID(unsigned long oidObject);
    CSWCArea* GetAreaByGameObjectID(unsigned long oidObject);
    CSWCTrigger* GetTriggerByGameObjectID(unsigned long oidObject);
    CSWCProjectile* GetProjectileByGameObjectID(unsigned long oidObject);
    int GetPauseState(unsigned char nPauseType);
    void SetPauseState(unsigned char nPauseState, int bPause,
                      CExoLinkedList<unsigned long>* pExclusionList);
    CWorldTimer* GetActiveTimer(unsigned long nTimer);
#ifdef _XBOX
    int SendSaveGameRequest(unsigned long nPlayerId,
                            const CExoString& sSaveName);
#else
    int SendSaveGameRequest(unsigned long nPlayerId,
                            const CExoString& sSaveName,
                            const CExoString& sSaveDir);
#endif
    int SendLoadGameRequest(unsigned long nPlayerId,
                            const CExoString& sSaveName,
                            const CExoString& sSaveDir);
    int GetPlayModulePanel();
    CClientAIMaster* GetClientAIMaster();
    CExoLinkedList<CSWCPlayer>* GetPlayerList();
    CSWCPlayer* GetPlayerByPlayerID(unsigned long nPlayerId);
    CServerInfo* GetServerInfo();
    void AttachPlayerGameObjectToArea();
    int InitializeInGameGUI();
    void ServerModeChange(unsigned char nServerMode);
    void AddObjectToHitcheckIgnoreList(unsigned long oidObject);
    void AddObjectModelDirectlyToIgnoreList(unsigned long oidObject,
                                           List<CAurObject*>* pIgnoreList);
    void RemoveObjectFromHitcheckIgnoreList(unsigned long oidObject);
    void EmptyHitcheckIgnoreList();
    int SetPlayerCreature(unsigned long oidObject);
    virtual void SetApplicationIdsMatch(int bMatch);
    CSoundSetList* GetSoundSetList();
    CResRef GetItemDropSound(unsigned char nType);
    void SetStartMenuMusicOnMovieEnd(int bValue);
    unsigned char GetActivePauseState();
    int IsPlayerCreature(CSWCCreature* pCreature);
    CExoString& GetPlayerCharacterName();
    int GetPlayerCharacterSet();
    int GetAreaNotReadyToDisplay();
    void SetAreaNotReadyToDisplay(int bNotReady);
    int PlayLoadModuleMovies();
    int PlayMovie(const CExoString& sMovie, int& nA, int nB, int nC,
                  int nSoundModeFlags = 0);
    int AddMoveToModuleMovie(const CExoString& sMovie);
    CExoArrayList<CExoString>* GetMoveToModuleMovies();
    void IndicateObject(CSWCObject* pObject, int nFlags);
    int ReattachCreatureToScene(unsigned long oidObject);
    void ResetDriveAcceleration();
    void EnableInput();
    int DismissInGameGUI();
    void ChangeCharacter(int nCharacter, int bPlaySound);
    int SetGameOver(int bGameOver);
    void SetGuiGameSlow(int bSlow);
    void RunDeathSequence();
    int GetGameOver();
    void SetLastPartyMemberTempKilled(unsigned long oidObject);
    void SetCanSendDialog(int bCanSend);
    int GetCanSendDialog(unsigned long nA, unsigned long nB,
                         CSWSDialog* pDialog);
    void SetDialogSent(int bSent);
    int GetDialogSent();
    void SetDialogModelsSent(int bSent);
    int GetDialogModelsSent();
    int GetClientRangedAnimation(unsigned long nA, int nB, int nC, int nD);
    int GetClientMeleeAnimation(unsigned long nA, unsigned long nB,
                                unsigned short nC, int nD, int nE, int nF);
    void SetLoadFromIFO(int bValue);
    int GetLoadFromIFO();
    void SetLastTarget(unsigned long oidObject, int nB);
    void SetRunScript(const CExoString& sScript);
    void UpdateRoom(const Vector& vPosition);
    void StartPazaakGame(int nSideDeck, const CExoString& sEndScript, int nWager, int bTutorial,
                        unsigned long oidOpponent);
    void SetFutureLeader(unsigned long oidObject);
    unsigned long GetFutureLeader();
    int GetEndGamePending();
    int PlayRumblePattern(int nPattern);
    int StopRumblePattern(int nPattern);
    void TogglePartyFollow();
    void SetCombatMode(int nMode);
    void AddFloatyText(unsigned long oidObject, unsigned char nType, int nValue);

    enum { FLOATY_TEXT_DAMAGE = 0, FLOATY_TEXT_MISS = 2, FLOATY_TEXT_XP = 3,
           FLOATY_TEXT_LEVEL = 4 };
    void DelayNextEnemyAutoPause();
    int ClearCachedCreatures(CSWCCreature* pCreature);
    void SetAttackMashTutorialTimer(unsigned long nTimer);
    unsigned long GetAttackMashTutorialTimer();
    int GetLoadingSaveGame();
    void SetLoadingSaveGame(int bLoading);
    unsigned char GetLoadStep(int nStep);
    void ClearEnemySighted();
    CExoString& GetLiveContentName(int nIndex);
    int GetLiveContentExists(int nIndex);
    int GetLiveContentCorrupt(int nIndex);
    unsigned long GetLoadScreenHintUpdateTimer();
    void SetLoadScreenHintUpdateTimer(unsigned long nTimer);
    unsigned char GetGamePlayHint();
    void SetGamePlayHint(unsigned char nHint);
    unsigned char GetStoryHint(unsigned char nArea = 0);
    void SetStoryHint(unsigned char nHint, unsigned char nArea = 0);
    void SetModuleResourceLoaded(unsigned char nMask);

    enum { MODULE_RESOURCE_VOICE = 8 };
    void ResetModuleResourcesLoaded();
    int GetModuleResourceLoaded(unsigned char nMask);
    int GetTargetChanging();
    int GetTogglePauseAtEndOfFrame();
    void BusyWaitMuteSound();
#ifdef _XBOX
    void SetMissingFilePending(int bPending);
#endif
    int GetPazaakIsRunning();
    void SetCharacterChangeInProgress(int bInProgress);
    void FadeAndStopStreamingSoundSource(CExoStreamingSoundSource* pSource,
                                         unsigned long nB);
    void StartCreditSequence(const Vector& vColor, const CExoString& sMusic);
    void StopCreditSequence();
    void SetLastClickedOnTarget(unsigned long oidObject);
    unsigned long GetLastClickedOnTarget();
    int AddMovieToMovieQueue(const CExoString& sMovie, int nFlags);
    int PlayMovieQueue(int nFlags);
    void PauseAllSoundsForMovieLostFocus();
    void ResumeAllStreamingMusicForMovie();
    int GetSoundPausedByCombat();
    void SetSoundPausedByCombat(int nPaused);
    void ClearMainMenuPointer();
    void EnableMainMenuIdleTimer();
    int DontUseThisSetPlayerCreature(unsigned long oidObject);
    void HandleExitGameButton();
    void RemoveMoveToModuleMovies();
    CClientExoAppInternal* GetExoAppInternal()
    {
        return m_pcExoAppInternal;
    }
    CClientExoAppInternal* m_pcExoAppInternal;
    int SetDebugMode(int nDebugMode);
};

char* SetFramesPerSecond(char* szFrameRate);
extern int g_nSetMovieFrameRate;
extern CAurObject* g_pDialogIconModel;
extern CAurObject* g_pGroundRing;
extern int g_nHSMode;

extern int g_bLogConversations;
extern int g_bShowFogOfWar;

extern unsigned long LockRenderDev;
int LoadScreenUpdate(int nStep, int nSteps, int nStart, int nRange);

#endif
