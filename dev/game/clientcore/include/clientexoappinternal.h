#ifndef __CLIENTEXOAPPINTERNAL_H__
#define __CLIENTEXOAPPINTERNAL_H__

#include "exoimeui.h"

#define CLIENT_MOUSE_HIDE_LOAD_SCREEN 2

#define CLIENT_MOUSE_HIDE_EVIL_ENDING 128

#define CLIENT_RUMBLE_DURATION 600000

#define CLIENT_RUMBLE_DEFAULT_CUTOFF 10.0f

#define CLIENT_CAMERA_ZOOM_THRESHOLD 0.0001f

#define CLIENT_LOAD_HINT_PROGRESS_LIMIT 95

#define CLIENT_DEATH_SEQUENCE_DURATION 4.0f
#define CLIENT_DEATH_SEQUENCE_MIN_SPEED 0.2f

class CExoImeUI;
class CExoLinkedListInternal;
class CExoMoviePlayer;
void ChangeResolutionForMovie();
void RestoreResolutionFromMovie();
class CExoStreamingSoundSource;
class CGameObjectArray;
class CNetLayer;
class CResRef;
class CSoundSetList;
class CSWCCMessageData;
class CSWCCreature;
class CSWCJournal;
class CSWCDungeonMaster;
class CSWCPlayer;
class CSWCMessage;
class CSWCModule;
class CSWCObject;
class CAurCamera;
class CAurObject;
template <class T> class List;
class CSWCWorldJournal;
class CSWGuiControl;
class CSWGuiLoadScreen;
class CSWGuiMainMenu;
class CSWGuiPanel;
class CSWParty;
class CSWPlayerControl;
class CSWSDialog;
class CWorldTimer;
class Vector;
#include "exobase.h"
#include "clientexoapp.h"
#include "exoresman.h"
#include "nwportrait.h"
#include "worldtimer.h"
#include "math3d.h"

#include "exotlktable.h"

struct CGuiStringStruct
{
    unsigned long m_nStrRef;
    CExoString m_sString;
};

struct CLiveContentStruct
{
    CExoString m_sName;
    int m_bCorrupt;
    int m_bExists;
};

struct CSWRumbleInstance
{
    int m_nPattern;
    float m_fTime;
    int m_bFirstUpdate;
};

struct CHostileHilite
{
    unsigned long m_oidObject;
    float m_fTimeRemaining;
};

class CGameObject;
class CClientOptions;
class CGuiInGame;
class CSWGuiManager;
class CSWGuiInterfaceAction;
class CExoInput;
class CSWRumblePattern;

class CClientAIMaster;
class CExoFile;
class CServerInfo;
class CSWCArea;
class CSWCAreaOfEffectObject;
class CSWCDoor;
class CSWCItem;
class CSWMiniGameObjectArray;
class CSWCPlaceable;
class CSWCProjectile;
class CSWCSoundObject;
class CSWCTrigger;
extern CExoInput* g_pExoInput;
extern int g_bLeftMouseDown;
extern int g_bMouseCameraRotateToggle;
extern float g_fMouseSensitivity;
extern int g_bShiftActionState;
extern CSWGuiManager* g_pGuiMan;
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern float CameraPersonalSpace;
extern int RenderQA;
extern int RenderPosition;

#define CLIENT_OBJECT_LOOK_DURATION 10.0f
#define CLIENT_HOSTILE_HILITE_DURATION 0.25f
#define CLIENT_PASSIVE_SELECTION_RANGE 30.0f
#define CLIENT_SIGHTED_LINGER_TIME 10.0f
#define CLIENT_TARGET_INVISIBLE_LIMIT 1.0f
#define CLIENT_TARGET_CHANGE_DELAY 1.5f
enum { CLIENT_DEBUG_PATH_SQUARES = 200 };
const unsigned long CLIENT_RENDER_LOCK_PLAYER = (unsigned long)-9999;

class CClientMsgSysAdmin;
class CGuiAreaLoadscreen;

struct CNearestStruct
{

    enum
    {
        CAN_SEE_UNKNOWN = -1
    };

    unsigned long m_oidObject;
    char m_nCanSee;
    unsigned char m_aUnmapped05[3];

    unsigned long m_bInFront : 1;
    unsigned long m_nUnused : 31;
};
int TestNearestStruct(void* pObjectId, CNearestStruct cNearest);

class CConnectionLib;
class CResRef;

#define CLIENT_DIALOG_SEND_WAIT_INTERVAL 1000
#define CLIENT_DIALOG_SEND_RETRY_BACKDATE 2000

struct CPlayBackEventNode
{
    CPlayBackEventNode()  // PC 0x0066FC10
    {
        nTime = 0;
        cType = 0;
        nKey = 0;
        nInputEvent = 0;
        nInputValue = 0;
        nPlayerId = 0;
        bGuaranteed = 0;
        pMessage = NULL;
        nMessageSize = 0;
        fX = 0.0f;
        fY = 0.0f;
        fZ = 0.0f;
    }

    enum EventType
    {
        KEY_DOWN = 'K', CHARACTER = 'C', CONNECTION = 'L',
        MESSAGE = 'M', INPUT = 'I', INPUT_END = 'Z',
        CAMERA = 'V', POSITION = 'Y', PLAYER_POSITION = 'X'
    };
    int nTime;
    char cType;
    unsigned short nKey;
    int nInputEvent;
    int nInputValue;
    unsigned long nPlayerId;
    int bGuaranteed;
    unsigned char* pMessage;
    unsigned long nMessageSize;
    float fX;
    float fY;
    float fZ;
    unsigned long oidObject;
};

struct CQueuedAnim
{
    CExoString sAnimation;
    float fDuration;
    int bLooping;
    unsigned short nAnimation;
};

class CClientExoAppInternal
{
public:

    enum CharacterExportState
    {
        CHARACTER_EXPORT_IDLE = 0,
        CHARACTER_EXPORT_PENDING = 1
    };

    static const unsigned long VIDEO_STARTUP_WIDTH = 640;
    static const unsigned long VIDEO_STARTUP_HEIGHT = 480;
    static const unsigned long VIDEO_FALLBACK_WIDTH = 800;
    static const unsigned long VIDEO_FALLBACK_HEIGHT = 600;
    static const unsigned char VIDEO_BITS_PER_PIXEL = 32;
    static const unsigned long VIDEO_REFRESH_RATE = 60;

    static const unsigned long VIDEO_MAX_STANDARD_REFRESH_RATE = 85;

    static const unsigned long VIDEO_XGA_WIDTH = 1024;
    static const unsigned long VIDEO_XGA_HEIGHT = 768;
    static const unsigned long VIDEO_SXGA_WIDTH = 1280;
    static const unsigned long VIDEO_SXGA_HEIGHT = 1024;
    static const unsigned long VIDEO_SXGA_4_3_HEIGHT = 960;
    static const unsigned long VIDEO_UXGA_WIDTH = 1600;
    static const unsigned long VIDEO_UXGA_HEIGHT = 1200;
    enum VideoModeSelection
    {
        VIDEO_MODE_WINDOWED = 1, VIDEO_MODE_FULLSCREEN = 2
    };

    enum { VIDEO_WINDOW_WIDTH_LIMIT = 639, VIDEO_WINDOW_HEIGHT_LIMIT = 479 };

    enum { PLAYBACK_RECORD_LENGTH = 80, PLAYBACK_COMMAND_LENGTH = 64,
           SCREENSHOT_NAME_LENGTH = 16 };
    void PlayBackInputEvents();
#ifdef _XBOX
    void HandleInputEvent(int nEvent, int nValue);
#else
    int HandleInputEvent(int nEvent, int nValue);
#endif

    enum InputActionEvent
    {
        INPUT_EVENT_FREE_LOOK = 1, INPUT_EVENT_PAUSE = 2,
        INPUT_EVENT_MOVE_FORWARD = 3, INPUT_EVENT_MOVE_BACK = 4,
        INPUT_EVENT_PREVIOUS_TARGET = 5, INPUT_EVENT_NEXT_TARGET = 6,
        INPUT_EVENT_NEXT_CHARACTER = 9, INPUT_EVENT_SOLO_MODE = 10,
        INPUT_EVENT_EXIT = 11, INPUT_EVENT_RESTORE_CAMERA = 15,
        INPUT_EVENT_FIRE = 25,
        INPUT_EVENT_MOUSE_X = 65, INPUT_EVENT_MOUSE_Y = 66,
        INPUT_EVENT_MOUSE_LEFT = 67, INPUT_EVENT_MOUSE_RIGHT = 68,
        INPUT_EVENT_MOUSE_MIDDLE = 69,
        INPUT_EVENT_MOUSE_WHEEL = 70, INPUT_EVENT_CAMERA_RESET = 72,
        INPUT_EVENT_CAMERA_LEFT = 73, INPUT_EVENT_CAMERA_RIGHT = 74,
        INPUT_EVENT_CAMERA_UP = 75, INPUT_EVENT_CAMERA_DOWN = 76,
        INPUT_EVENT_SCREENSHOT = 80, INPUT_EVENT_GUI_ACCEPT = 181,
        INPUT_EVENT_ACTION_UP = 200, INPUT_EVENT_ACTION_DOWN = 201,
        INPUT_EVENT_SELECT_NEXT = 204, INPUT_EVENT_SELECT_PREVIOUS = 205,
        INPUT_EVENT_SOLO_TOGGLE = 207, INPUT_EVENT_FREE_LOOK_TOGGLE = 208,
        INPUT_EVENT_EQUIP = 209, INPUT_EVENT_INVENTORY = 210,
        INPUT_EVENT_CHARACTER = 211, INPUT_EVENT_ABILITIES = 212,
        INPUT_EVENT_MESSAGES = 213, INPUT_EVENT_JOURNAL = 214,
        INPUT_EVENT_MAP = 215, INPUT_EVENT_OPTIONS = 216,
        INPUT_EVENT_MINIGAME_FIRE = 217, INPUT_EVENT_QUICK_SAVE = 218,
        INPUT_EVENT_ROTATE_LEFT = 219, INPUT_EVENT_ROTATE_RIGHT = 220,
        INPUT_EVENT_SHIFT_LEFT = 221, INPUT_EVENT_SHIFT_RIGHT = 222,
        INPUT_EVENT_PAUSE_TOGGLE = 224, INPUT_EVENT_TOOLTIPS = 225,
        INPUT_EVENT_DEFAULT_ACTION = 239, INPUT_EVENT_CANCEL_COMBAT = 240,
        INPUT_EVENT_COMBAT_PAUSE = 241, INPUT_EVENT_FLOURISH = 242,
        INPUT_EVENT_CLEAR_ACTION = 245,

        INPUT_EVENT_RESERVED_246 = 246, INPUT_EVENT_RESERVED_247 = 247,
        INPUT_EVENT_QUEUE_ACTION = 250,
        INPUT_EVENT_CLEAR_ACTION_ALTERNATE = 251,
        INPUT_EVENT_RESERVED_252 = 252,
        INPUT_EVENT_PAUSE_ALTERNATE = 253, INPUT_EVENT_QUICK_LOAD = 263,
        INPUT_EVENT_STEALTH = 264
    };

    enum { PAUSE_REASON_INPUT = 4, AUTOPAUSE_NEW_TARGET = 8,
           TUTORIAL_PAUSE = 6, TUTORIAL_OPTIONS = 7,
           STRREF_PLAYER_DEAD = 42628, QUICK_LOAD_COOLDOWN = 1000 };
    enum
    {
        PAUSE_STATE_PAUSED = 1,
        PAUSE_STATE_COMBAT = 2
    };

    enum
    {
        PAUSE_TYPE_PAUSED_MASK = 0x01,
        PAUSE_TYPE_COMBAT_MASK = 0x02
    };

    struct SPauseStateFlags
    {
        unsigned char m_bPaused : 1;
        unsigned char m_bPausedByCombat : 1;
        unsigned char m_nUnused : 6;
    };

    struct SPauseFlagBits
    {
        unsigned long m_bLastTargetChanged : 1;
        unsigned long m_nUnused1 : 1;
        unsigned long m_bTogglePauseAtEndOfFrame : 1;
#ifdef _XBOX
        unsigned long m_bAutoPaused : 1;
        unsigned long m_nUnused : 28;
#else
        unsigned long m_nUnused : 29;
#endif
    };

    union SPauseFlags
    {
        int m_nPauseFlags;
        SPauseFlagBits m_Bits;
    };

    struct SAutoPauseFlagBits
    {
        unsigned long m_bAutoPaused : 1;
        unsigned long m_nUnused : 31;
    };

    union SAutoPauseFlags
    {
        int m_nAutoPauseFlags;
        SAutoPauseFlagBits m_Bits;
    };

    union SPauseState
    {
        unsigned char m_nPauseState;
        SPauseStateFlags m_Flags;
    };

    CGameObject* GetGameObject(unsigned long oidObject);

    void DisableInput();
    void EnableInput();
    int PerformXMouseMoveAction(int nX);
    int PerformYMouseMoveAction(int nY);
    void HandleMouseClickInWorld();
#ifndef _XBOX
    void PerformLButtonDownAction(int nArg);
#endif
#ifndef _XBOX
    void PerformLButtonUpAction();
#endif
    void PerformRButtonDownAction();
    int PerformRButtonUpAction();
    void SetMouseCursor(int nCursor, int nArg2, int nArg3);
    void RestoreMousePositionTo(int nX, int nY);
    void DoScreenShot();
    void HideMouse(int nFlags);
    void ShowMouse(int nFlags);
    int DismissInGameGUI();
#ifdef _XBOX

    void SetInputClass(int nClass);
    void SetInputClass(int nClass, int) { SetInputClass(nClass); }
#else
    void SetInputClass(int nClass, int bHideInterface);
#endif
    int ActionIconToMouseCursor(int nAction, int nArg2);
    CResRef ActionIconToCursorResref(int nAction, int nArg2);
    void HandleMouseOver(unsigned long oidObject, const Vector& vPosition,
                         int bForceUpdate);

    void GetDefaultActions();
    int SetDebugMode(int nDebugMode);
    int ServerToClientObjectId(unsigned long oidServerObject);

    void BeginIntro();
    CSWCJournal* GetQuestJournal();
    void CreateQuestJournal();
    void CreateWorldJournal();
    void DestroyWorldJournal();
    void DestroyQuestJournal();
    CSWCWorldJournal* GetWorldJournal();
    void JournalUpdated(int nType, int nIndex, const CExoString& sName);
    int GetObjectName(unsigned long oidObject, CExoString& sName);
    void AddFeedbackMessage(unsigned short nType, CSWCCMessageData* pMessageData,
                            const CExoString& sMessage);
    void SetCharacterFile(unsigned long nDataSize, unsigned char* pCharacterData,
                         int nCharacterFileId);
    void DisplayMainMenu(int bUnknown);
#ifdef _XBOX
    void DisplayStartScreen();
    void ShutDownLegalScreen();
#endif
    void UnloadModule();
    void CleanUpModuleServices();
    CSWCModule* CreateModule();
    void ExitProgram();
    void HandleExitGameButton(CSWGuiControl* pControl);
    CExoFile* GetCaptureEventFile();
    int GetCaptureEvents();
    int GetCaptureEventPlayBack();
    CExoString GetClientName();
    int GetClientLanguage();
    CWorldTimer* GetWorldTimer() { return m_pWorldTimer; }
    CClientOptions* GetClientOptions();
    CWorldTimer* GetAnimationTimer();
    int GetConnectionLib();
    CSWCModule* GetModule();
    CSWCCreature* GetPlayerCreature();
    int GetPlayerCreatureId();

    int GetPlayerMovementInput();
    CAurCamera* GetModuleCamera();
    CGameObjectArray* GetGameObjectArray();
    CSWCPlayer* GetPlayerByGameObjectID(unsigned long oidObject);
    CSWCItem* GetItemByGameObjectID(unsigned long nId);
    CSWCCreature* GetCreatureByGameObjectID(unsigned long nId);
    CSWCArea* GetAreaByGameObjectID(unsigned long oidObject);
    CSWCTrigger* GetTriggerByGameObjectID(unsigned long oidObject);
    CSWCProjectile* GetProjectileByGameObjectID(unsigned long oidObject);
    CSWCPlaceable* GetPlaceableByGameObjectID(unsigned long oidObject);
    CSWCDoor* GetDoorByGameObjectID(unsigned long oidObject);
    CSWCAreaOfEffectObject* GetAreaOfEffectByGameObjectID(
        unsigned long oidObject);
    CSWCSoundObject* GetSoundObjectByGameObjectID(unsigned long oidObject);
    void SetInStartNewModule(int bInStartNewModule);
    int GetPauseState(unsigned char nPauseType);
    int GetControlledCharacterDead();
    CWorldTimer* GetActiveTimer(unsigned long nTimer);
    void AddToPauseExclusionList(unsigned long oidObject);
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
    int GetPlayerList();
    CSWCPlayer* GetPlayerByPlayerID(unsigned long nPlayerId);
    CNetLayer* GetNetLayer();
    CSWCMessage* GetSWCMessage();
    CServerInfo* GetServerInfo();
    void AttachPlayerGameObjectToArea();
    int InitializeInGameGUI();

    void InitializeClientOptions();
#ifdef _XBOX
    int AddPatchFiles(int bRemoveFirst);
#endif
    void InitializeSoundOptions();
    void ServerModeChange(unsigned char nServerMode);
    void AddObjectToHitcheckIgnoreList(unsigned long oidObject);
    void AddCreatureModelDirectlyToIgnoreList(unsigned long oidCreature,
                                              List<CAurObject*>* pIgnoreList);
    void AddObjectModelDirectlyToIgnoreList(unsigned long oidObject,
                                           List<CAurObject*>* pIgnoreList);
    void AddSimpleObjectModelDirectlyToIgnoreList(unsigned long oidObject,
                                                 List<CAurObject*>* pIgnoreList);
    void AddItemModelDirectlyToIgnoreList(unsigned long oidItem,
                                          List<CAurObject*>* pIgnoreList);
    void RemoveObjectFromHitcheckIgnoreList(unsigned long oidObject);
    void EmptyHitcheckIgnoreList();
    void SetHitcheckIgnoreList(List<CAurObject*>* pIgnoreList);
    int SetPlayerCreature(unsigned long oidObject);
    int HandleMessage(unsigned long nPlayerId, unsigned char* pMessage,
                      unsigned long nSize, int bGuaranteed);
    int ContinueMessageProcessing();
    int Initialize();
    int IsClientInitialized();
    void ClearCameraInput();
    int MainLoop();
    void ProcessInput(float fElapsed);
    void UpdateSoundEngine();
    CGuiAreaLoadscreen* StartLoadBar(int nType, char* sImage, int bShow);
    void DoPassiveSelection(float fElapsed);
    int Uninitialize();
    int ReadVideoModeSettings(unsigned long* pnWidth, unsigned long* pnHeight,
                              unsigned char* pnBitsPerPixel,
                              unsigned long* pnRefreshRate);
    unsigned long GetNearestVideoMode(unsigned long nWidth, unsigned long nHeight,
                                      unsigned char nBitsPerPixel, unsigned long nRefreshRate);
    void ReadAndSetVideoMode(int bStartup, int bFullScreen);
    void UpdateCamera(float fDeltaTime);

    enum { CAMERA_TURN_INPUT = 0x11C, CAMERA_SCROLL_PIXELS = 40,
           CAMERA_EDGE_MIN_PIXELS = 2, CAPTURE_EVENTS_RECORD = 1 };
    void AddDownloadedResources();
#ifndef _XBOX
    int StartServices(int nServices, const CExoString& sArg2,
                      const CExoString& sArg3, const CExoString& sArg4,
                      unsigned long nArg5);
#else
    void StartServices(int nServices, const CExoString& sArg2, const CExoString& sArg3, const CExoString& sArg4, unsigned long nArg5);
#endif
    void SetEventDescriptions();
    void StopServices();

    enum InputContext
    {
        INPUT_CONTEXT_PC = 0, INPUT_CONTEXT_MINIGAME = 1,
        INPUT_CONTEXT_PC_GUI = 2, INPUT_CONTEXT_DIALOG = 3,
        INPUT_CONTEXT_FREE_LOOK = 4, INPUT_CONTEXT_MOVIE = 5
    };
    enum InputEvent
    {
        INPUT_KEY_RETURN = 181, INPUT_KEY_UP = 182, INPUT_KEY_DOWN = 183,
        INPUT_KEY_LEFT = 184, INPUT_KEY_RIGHT = 185, INPUT_KEY_NUMPADENTER = 187,
        INPUT_SCREENSHOT = 80,
        INPUT_JOYSTICK_Y = 7, INPUT_JOYSTICK_Y_BUTTON = 4,
        INPUT_JOYSTICK_X = 8, INPUT_JOYSTICK_X_BUTTON = 3,
        INPUT_JOYSTICK_SLIDER0 = 12, INPUT_JOYSTICK_SLIDER1 = 13,
        INPUT_JOYSTICK_BUTTON8 = 11,
        INPUT_MINIGAME_MOUSE_X = 23, INPUT_MINIGAME_MOUSE_Y = 24,
        INPUT_MINIGAME_MOUSE_BUTTON0 = 25,
        INPUT_MOUSE_X = 65, INPUT_MOUSE_Y = 66,
        INPUT_MOUSE_BUTTON0 = 67, INPUT_MOUSE_BUTTON1 = 68,
        INPUT_MOUSE_BUTTON2 = 69, INPUT_MOUSE_Z = 70,
        INPUT_ACTION_FIRST = 200, INPUT_ACTION_LAST = 265,
        INPUT_PAIRED_ACTION_FIRST = 280, INPUT_PAIRED_ACTION_LAST = 286
    };
    void ShutDownToMainMenu();
    void ShutDownLoadGameLoadBar();
    void SetApplicationIdsMatch(int bMatch);
    CSWParty* GetSWParty();
    int GetHitCheckMaterialMask();
    int SetVideoMode(unsigned long nVideoMode);
    int SetVideoMode(unsigned long nVideoMode, unsigned char nModeSelection);
    int SetTexturePack(unsigned char nTexturePack);
    int SetTexturePack(unsigned char nTexturePack, int nUnknown);
    int GetSoundSetList();
    void LoadItemDropSoundList();
    int GetItemDropSound(unsigned char nType);
    int GetLoadBarState();
    void EndLoadBar();
    void RestartMoviesIfNecessary();
    void StartMenuMusic(int bCultTheme, int nVolume, int nPriorityGroup);

    enum { MENU_MUSIC_VOLUME = 127, MENU_MUSIC_PRIORITY_GROUP = 2 };
    void StopMenuMusic();
    void StopLoadMusic();
    void StartLoadMusic();
    void SetStartMenuMusicOnMovieEnd(int bValue);
    unsigned char GetActivePauseState();
    void QueryExitProgram();
    void SetInputActive(int nA, int nB);
    void TurnOffKeyboardModal();
    int IsPlayerCreature(CSWCCreature* pCreature);
    void ShowLoadScreen(int bFade, int bPlayMusic, int bSaving);
#ifdef _XBOX
    void HideLoadScreen(int bFade);
#else
    void HideLoadScreen(int bFade, int bUpdateScreen);
#endif
    int GetLoadScreenVisible();
    CExoString& SetPlayerCharacterName(const CExoString& sName);
    CExoString& GetPlayerCharacterName();
    int GetPlayerCharacterSet();
    int GetAreaNotReadyToDisplay() { return m_nAreaNotReadyToDisplay; }
    void SetAreaNotReadyToDisplay(int bNotReady);
    CSWMiniGameObjectArray* GetMGOArray();
    int PlayLoadModuleMovies();
    int PlayMovie(const CExoString& sMovie, int& nA, int nB, int nC);
    void StallServerCode();
#ifdef _XBOX
    int WaitForMovie();
#else
    int WaitForMovie(int nFlags);
#endif
    void StopMovie();
    int IsOnExclusionList(unsigned long oidObject);
    void InitializeMoviePlayer();
    void ShutDownMoviePlayer();
    int IsMoviePlaying();
    int AddMovieToExoArrayList(const CExoString& sMovie,
                               CExoArrayList<CExoString>& lstMovies);
    int PlayMoviesInExoArrayList(CExoArrayList<CExoString>& lstMovies,
                                 int& nResult, int nWait, int nMode,
                                 CExoArrayList<int>* lstSkippable);
    int AddMoveToModuleMovie(const CExoString& sMovie);
    int GetMoveToModuleMovies();
    void IndicateObject(CSWCObject* pObject, int nFlags);

    void ShowObject(CSWCObject* pObject, int bLookAt, int bUnknown1,
                   int bUnknown2);
    CSWCObject* SelectNearestObject(int bReverse, unsigned char nFilter);
    void ResetDriveAcceleration();
    void BarkString(unsigned long oidObject, const CExoString& sText,
                   const CResRef& cSound, int nFlags);
    void StopSoundPlayingInDialog();
    int IsSoundPlayingInDialog();
    int GetPausedByCombat();
    void SetPausedByCombat(int nA, unsigned char nB, int nC);
    void ChangeCharacter(int nCharacter, int bPlaySound);
    int SetGameOver(int bGameOver);
    void SetGameSpeed(float fSpeed);
    void RunDeathSequence();
    void UpdateDeathSequence();
    int GetGameOver();
    void SetLastPartyMemberTempKilled(unsigned long oidObject);
    void SetCanSendDialog(int bCanSend);
    void SetCaptureEventPlayBackRate(int nRate);
    void SetCaptureEvents(const CExoString& sFileName);
    void SetCaptureEventPlayBack(const CExoString& sFileName);
    void ReadPlayBackInputEvents();
    void HurryDeathSequence();
    int GetCanSendDialog(unsigned long nA, unsigned long nB,
                         CSWSDialog* pDialog);
    void SetDialogSent(int bSent);
    void SetDialogModelsSent(int bSent);
    int GetClientRangedAnimation(unsigned long nA, int nB, int nC, int nD);
    int GetClientMeleeAnimation(unsigned long nA, unsigned long nB,
                                unsigned short nC, int nD, int nE, int nF);
    void SetLoadScreenByModuleName(const CExoString& sModule);
#ifdef _XBOX

    void MountDownloadedContent(int bRemove, int bShowProgress);
    void AddCorruptContentMessage();
#endif
#ifdef _XBOX
    void SetLoadMusic(const CExoString& sMusic) { m_sLoadMusic = sMusic; }
#else
    int SetLoadMusic(const CExoString& sMusic);
#endif
    void SetLoadBarProgress(int nProgress, int bUpdateHint);
    void UpdateLoadingScreenHint();
    void SetLoadFromIFO(int bValue);
    int GetLoadFromIFO();
    void SetLastTarget(unsigned long oidObject, int bChanged);
    int SetRunScript(const CExoString& sScript);
    void UpdateRoom(const Vector& vPosition);
    void StartPazaakGame(int nSideDeck, const CExoString& sEndScript, int nWager, int bTutorial,
                        unsigned long oidOpponent);
    void EndPazaakGame(int nResult, int nWager, int bPlayAgain,
                      unsigned long oidOpponent);
    int GetPazaakResults(int* pnA, int* pnB);
    void SetFutureLeader(unsigned long oidObject);
    int CrownFutureLeader();
    void SetEndGamePending(int bPending, float fDelay, int nC);
    int PlayRumblePattern(int nPattern);
    int StopRumblePattern(int nPattern);
    void UpdateRumble(float fDeltaTime);
    void TogglePartyFollow();
    int GetCombatMode();
    void SetCombatMode(int nMode);
    void UpdateCombatMode();
    void PlayerFlourishWeapons();

    enum FloatyTextType
    {
        FLOATY_TEXT_DAMAGE = 0, FLOATY_TEXT_HEAL = 1, FLOATY_TEXT_MISS = 2,
        FLOATY_TEXT_XP = 3, FLOATY_TEXT_LEVEL = 4,
        FLOATY_TEXT_SNEAK_ATTACK = 5, FLOATY_TEXT_SPOTTED = 6
    };
    enum FloatyTextStringRef
    {
        STRREF_FLOATY_MISS = 1373, STRREF_FLOATY_XP = 38551,
        STRREF_FLOATY_LEVEL = 32154, STRREF_FLOATY_SNEAK_ATTACK = 1391,
        STRREF_FLOATY_SPOTTED = 42403
    };
    void AddFloatyText(unsigned long oidObject, unsigned char nType, int nValue);
    CExoString GetGUIString(unsigned long nStrRef);
    void SetAutoPaused(int bAuto, unsigned char nB);
    int GetAutoPaused();
    void SetLoadScreenHint(unsigned long nHint);
    unsigned long GetLoadScreenHint();
    void EnableVideoEffect(int nEffect);
    void DisableVideoEffect();
    void DelayNextEnemyAutoPause();
    int ShowTutorialWindow(unsigned char nTutorial, unsigned long nX,
                           unsigned long nY, unsigned long nActionParam);
    int ClearCachedCreatures(CSWCCreature* pCreature);
    void LookUpAndPerformRumbleWithCutOff(int nEntry, unsigned char nTable,
                                          const Vector& vPosition);
    int SaveOptions();
    int LoadOptions();
    void DoQuickSave();
    int ChangeCharacterToNextLivingPartyMember(int nA, int nB,
                                               unsigned char nC);
#ifdef _XBOX
    int ChangeCharacterToNextLivingPartyMember(int nA, int nB);
#endif
    void HideMovieFadePanel();
    void PauseAllSoundsForMovieLostFocus();
    void ResumeAllStreamingMusicForMovie();
    void SetAttackMashTutorialTimer(unsigned long nTimer);
    int GetAttackMashTutorialTimer();
    int GetLoadingSaveGame();
    void SetLoadingSaveGame(int bLoading);
    void SetLoadStep(int nStep, unsigned char cValue);
    int GetLoadStep(int nStep);
    ~CClientExoAppInternal();
    CClientExoAppInternal();

    enum { CAPTURE_PLAYBACK_DEFAULT_RATE = 30 };
    static const unsigned long VIDEO_MODE_UNSET = 0xFFFFFFFF;
    static const unsigned char TEXTURE_PACK_UNSET = 0xFF;

    enum { CLIENT_VIDEO_EFFECT_DISABLED = -2, AUTO_PAUSE_TYPE_UNSET = 0xFF,
           TUTORIAL_REASON_UNSET = 0xFF };
    CExoString GetLoadMusicByModuleName(const CExoString& sModule);
    void SetPauseState(unsigned char nPauseState, int bPause,
                      CExoLinkedList<unsigned long>* pExclusionList);
    void ClearPauseExclusionList();
    void ClearEnemySighted();
    int GetLiveContentName(int nIndex);
    void SetLoadScreenHintUpdateTimer(unsigned long nTimer);

    enum { LOAD_SCREEN_HINT_INTERVAL = 10000 };
    int GetGamePlayHint();
    void SetGamePlayHint(unsigned char nHint);
    int GetStoryHint();
    void SetStoryHint(unsigned char nHint);

    enum { LOAD_SCREEN_HINT_GAMEPLAY = 1, LOAD_SCREEN_HINT_STORY = 2 };
    int GetNextLoadScreenHintSTRREF(const CExoString& sModule,
                                    unsigned char nHintType);
    void SetModuleResourceLoaded(unsigned char nMask);
    void ResetModuleResourcesLoaded();
    int GetModuleResourceLoaded(unsigned char nMask);
    int GetTargetChanging();
    int GetTogglePauseAtEndOfFrame();
    void BusyWaitMenuMusic();
    void BusyWaitMuteSound();
    int GetPazaakIsRunning();
    void SetMouseMode(unsigned char nMode);
#ifndef _XBOX
    void SetToolTipInstantMode(int bInstant);
#endif
    void CancelMovie();
    int GetCharacterChangeInProgress();
    void SetCharacterChangeInProgress(int bInProgress);
    void FadeAndStopStreamingSoundSource(CExoStreamingSoundSource* pSource,
                                         unsigned long nB);
    void StartCreditSequence(const Vector& vColor, const CExoString& sMusic);
    void StopCreditSequence();
    void PlayCharacterSelectSound(CSWCCreature* pCreature);
    int GetCreditSequenceInProgress();
    void EnableMouseAtEndOfLoadModuleMovie();
    void SetEvilEndingHackInProgress(int bInProgress);
    int AddMovieToMovieQueue(const CExoString& sMovie, int nFlags);
    int PlayMovieQueue(int nFlags);
    int GetInFreeLook();
    int IsValidResolution(unsigned long nWidth, unsigned long nHeight);
    CExoString GetDisplayCharacterForKey(int nKey);
    int GetSoundPausedByCombat();
    int DontUseThisSetPlayerCreature(unsigned long oidObject);
    unsigned long GetLastAttachedCreature();
    void RemoveMoveToModuleMovies();
    void UpdateHostileHilite(float fElapsedTime);

    CSWMiniGameObjectArray* m_pMiniGameObjectArray;
    CClientOptions* m_pClientOptions;
    CServerInfo* m_pServerInfo;
    CClientAIMaster* m_pClientAIMaster;
    CNetLayer* m_pUnknown10;
    CGameObjectArray* m_pGameObjectArray;
    CSWCModule* m_pModule;
    CExoLinkedList<CSWCPlayer> m_lstPlayerList;
    int m_nPlayerCreatureId;
    CWorldTimer* m_pWorldTimer;
    CWorldTimer* m_pCombatTimer;
    CWorldTimer* m_pPauseTimer;
    CWorldTimer* m_pAnimationTimer;
    CConnectionLib* m_pConnectionLib;
    CSWCJournal* m_pQuestJournal;
    CSWCWorldJournal* m_pWorldJournal;
    CGuiInGame* m_pGuiInGame;
    int m_nUnknown44;
    int m_nUnknown48;
    int m_nUnknown4C;
    int m_nPlayModulePanel;
    int m_nUnknown54;
    unsigned long m_oidUnknown58;
    unsigned char m_aUnmapped5C[4];
    int m_nUnknown60;
    unsigned char m_aUnmapped64[4];
    int m_nUnknown68;
    int m_nUnknown6C;

    struct CPazaakData
    {
        CPazaakData()
        {
            m_bIsRunning = 0;
            m_nResult = 0;
            m_nResultUnused = 0;
            m_nSideDeck = 0;
        }
        int m_bIsRunning;
        CExoString m_sEndScript;
        int m_nResultUnused;
        int m_nResult;
        int m_nSideDeck;
    } m_PazaakState;
    CExoFile* m_pCaptureEventFile;
    int m_nCaptureEvents;
    int m_nCaptureEventPlayBack;
    int m_nCaptureEventPlayBackRate;
    CExoLinkedList<CPlayBackEventNode> m_lstPlayBackEvents;
    int m_nInputClass;
    unsigned char m_aUnmappedA0[0x4];
    int m_nHitCheckMaterialMask;
    unsigned char m_bPlayBackInputEnd;
    unsigned char m_aUnmappedA9[3];

    float m_fCameraZoom;
    float m_fCameraTilt;
    float m_fCameraTurn;
    Vector m_vUnknownB8;
    Vector m_vUnknownC4;
    int m_nCameraScrollX;
    int m_nCameraScrollY;
    unsigned char m_aUnmappedD8[4];
    int m_bClientImeUI;
    CExoImeUI* m_pImeUI;
    int m_nXboxUnmappedE4;
    int m_nUnknownE4;
    int m_nUnknownE8;
    int m_nUnknownEC;
    int m_nUnknownF0;
    int m_nUnknownF4;
    int m_nUnknownF8;
    int m_nUnknownFC;
    int m_anKeyboardModal[7];
#ifdef _XBOX
    float m_fXboxUnmapped120;
    float m_fXboxUnmapped124;
    float m_fXboxUnmapped128;
    float m_fXboxUnmapped12C;
#else
    int m_nUnknown11C;
    int m_nUnknown120;
    int m_nUnknown124;
    int m_nUnknown128;
#endif
    CExoMoviePlayer* m_pMoviePlayer;
    unsigned char m_aUnmapped130[0xC];
    CExoString m_sClientName;
    int m_nClientLanguage;
    unsigned short m_nServerMode;
    unsigned char m_aUnmapped14A[0x2];
    CSWCMessage* m_pSWCMessage;
    int m_bScreenShot;
    char m_szScreenShot[SCREENSHOT_NAME_LENGTH];
    CExoLinkedList<CQueuedAnim> m_lstPlayerAnimations;

    float m_fPlayerAnimationTime;
    unsigned long m_oidUnknown170;
    int m_nUnknown174;
    int m_nUnknown178;
    unsigned char m_nUnknown17C;
    unsigned char m_aUnmapped17D[11];
    int m_nUnknown188;
    int m_nLoadBarState;
    CExoLinkedList<unsigned long> m_lstHitcheckIgnore;
    int m_nDebugMode;
    unsigned char m_aUnmapped198[4];
    int m_nUnknown19C;

    unsigned long m_nVideoMode;
    unsigned char m_nTexturePack;
    unsigned char m_aUnmapped1A5[3];
    int m_bEndGamePending;
    int m_nEndGamePendingUnknown;
    int m_nUnknown1B0;
    unsigned char* m_pCharacterFile;
    unsigned long m_nCharacterFileSize;
    int m_nCharacterFileId;
    CClientMsgSysAdmin* m_pUnknown1C0;
    CSoundSetList* m_pSoundSetList;
    CResRef* m_pItemDropSounds;
    SPauseState m_PauseState;
    unsigned char m_aUnmapped1CD[0x3];
    CExoLinkedList<unsigned long>* m_pPauseExclusionList;
    int m_nUnknown1D4;
    CExoArrayList<CExoString> m_aTextureNames;
    CExoString m_sTexturePack;
    int m_bStartMenuMusicOnMovieEnd;
    int m_nUnknown1F0;
    unsigned char m_aUnmapped1F4[4];
    int m_bMovieDisabled;
    int m_bSkipIntro;
    int m_bEnableMouseAtEndOfLoadModuleMovie;
    int m_bClientInitialized;
    int m_bApplicationIdsMatch;
    Vector m_vUnknown20C;
    unsigned char m_aUnmapped218[8];
    int m_bInStartNewModule;
    unsigned long m_oidUnknown224;
    int m_bCanSendDialog;
    int m_nCanSendDialogTime;
    int m_bDialogSent;
    int m_bDialogModelsSent;
    CExoStreamingSoundSource* m_pMenuMusic;
    CExoStreamingSoundSource* m_pLoadMusic;
    int m_nUnknown240;
    int m_nUnknown244;
    int m_bModuleServicesActive;
    int m_nUnknown24C;
    int m_bExitGame;
    int m_bExitGamePending;
    int m_nUnknown258;
    int m_nUnknown25C;
#ifdef _XBOX
    unsigned char m_nXboxUnmapped264;
#endif

    int m_nCharacterExportState;
    int m_bShutDownToMainMenuPending;
    int m_nUnknown268;
    int m_nUnknown26C;
    CSWParty* m_pSWParty;
    CSWGuiManager* m_pSWGuiManager;
    CSWGuiLoadScreen* m_pGuiLoadScreen;
    CSWGuiPanel* m_pUnknown27C;
    CSWGuiMainMenu* m_pMainMenu;
    CSWGuiPanel* m_pStartupPanel;
    int m_nAreaNotReadyToDisplay;
    unsigned long m_oidReattachedCreature;
    int m_bPlayerCharacterNameSet;
    CExoString m_sPlayerCharacterName;
    int m_nUnknown29C;
    CSWPlayerControl* m_pPlayerControl;

    float m_fDriveActionDelay;

    CExoArrayList<CNearestStruct> m_aNearestObjects;
    int m_nLastTarget;
#ifndef _XBOX
    unsigned long m_oidLastClickedOnTarget;
#endif
    int m_bInputDisabled;
    int m_nGameOver;
    int m_bGameOverPending;

    unsigned long m_nDeathSequenceLastTime;
    float m_fDeathSequenceElapsed;
    float m_fDeathSequenceScale;
    unsigned long m_oidLastPartyMemberTempKilled;
    CExoArrayList<CExoString> m_aMoveToModuleMovies;
#ifndef _XBOX
    CExoArrayList<CExoString> m_aMovieQueue;
    CExoArrayList<int> m_aMovieQueueFlags;
#endif
    CExoString m_sLoadScreenImage;
    CExoString m_sLoadMusic;
    int m_bShowLoadScreen;
    int m_nLoadFromIFO;
    CExoArrayList<CHostileHilite> m_aHostileHilites;
    int m_nCombatMode;
    int m_bEnemySighted;
#ifndef _XBOX
    int m_bMineSighted;
#endif
    int m_bRunScript;
    CExoString m_sRunScript;
    unsigned long m_oidFutureLeader;
#ifdef _XBOX

    int m_bMinGlobalRim;
#endif
    int m_nLoadingSaveGame;
    CExoArrayList<CSWRumblePattern> m_aRumblePatterns;
    CExoArrayList<CSWRumbleInstance> m_aRumbleInstances;
    CExoArrayList<CGuiStringStruct> m_aGuiStrings;
    unsigned long m_oidUnknown364;
    float m_fTargetInvisibleTime;

    float m_fMovingTargetTime;

    float m_fStatusSummaryDelay;
    float m_fEndGamePendingDelay;
    float m_fTargetChanging;
    SPauseFlags m_PauseFlags;
#ifndef _XBOX
    int m_bTogglePause;
    SAutoPauseFlags m_AutoPauseFlags;
#endif
    unsigned char m_nTogglePauseType;
    unsigned char m_aUnmapped389[0x3];
    float m_fEnemyAutoPauseDelay;
    float m_fEnemyAutoPauseTimer;

    float m_fCombatAutoPauseTimer;
#ifndef _XBOX
    unsigned char m_nDelayedAutoPauseType;
    unsigned char m_aUnmapped399[3];
#endif
    float m_fEnemyAutoPauseCooldown;
#ifndef _XBOX
    float m_fMineSightedTime;
#endif
    int m_nUnknown3A4;
    int m_nVideoEffect;
    unsigned long m_nTutorialX;
    unsigned long m_nTutorialY;
    unsigned long m_nTutorialZ;
    unsigned char m_nTutorialReason;
    unsigned char m_aUnmapped3B9[0x3];
    unsigned long m_oidCachedCreature;
    CSWCCreature* m_pCachedCreature;
    CExoStreamingSoundSource* m_pEvilEndingHackSound;
#ifndef _XBOX
    int m_nMouseX;
    int m_nMouseY;
    int m_nUnknown3D0;
    int m_nUnknown3D4;
    int m_nMouseHidden;
    float m_fMouseHideTime;
#endif
    unsigned long m_nAttackMashTutorialTimer;
    unsigned char m_aLoadSteps[8];

    enum { INPUT_HISTORY_LENGTH = 10 };
    unsigned long m_anInputHistory[INPUT_HISTORY_LENGTH];
    unsigned char m_nInputHistoryCount;
    unsigned char m_aInputHistoryAlignment[3];
    CLiveContentStruct m_aLiveContentNames[7];
    unsigned long m_nLoadScreenHintUpdateTimer;
    unsigned long m_nLoadScreenStartTime;
    unsigned char m_nGamePlayHint;
    unsigned char m_nStoryHint;
    unsigned char m_aUnmapped492[10];
    int m_bMainMenuTransition;
    unsigned char m_nModuleResourcesLoaded;
    unsigned char m_aUnmapped499[3];
    int m_nUnknown49C;
    int m_nUnknown4A0;
#ifndef _XBOX
    int m_bCharacterChangeInProgress;
    unsigned long m_oidMouseOverTarget;
    Vector m_vMouseOverPosition;
    int m_bCreditSequenceInProgress;
    int m_bEvilEndingHackInProgress;
    int m_nSoundPausedByCombat;

    CExoArrayList<CSWGuiInterfaceAction> m_aDefaultActions;
    unsigned char m_n4D8;
    unsigned char m_bAllowHighMonitorFrequency;
#else

    int m_nXboxUnmapped468;
    unsigned char m_aXboxUnmapped46C[4];

    int m_nMouseX;
    int m_nMouseY;
    int m_nMouseHidden;
    float m_fMouseHideTime;

#endif
};

class CSWCPlayer
{
public:
    virtual CSWCDungeonMaster* AsSWCDungeonMaster() { return 0; }  // PC 0x004065A0
    unsigned long m_nPlayerId;
    int m_nUnknown8;
    CExoString m_sPlayerName;
    CSWPortrait m_cPrimaryPCPortrait;
    short m_nPrimaryPCPortraitId;
    char m_aUnmapped26[2];

    unsigned long m_oidControlledObject;

    unsigned long m_oidPrimaryPC;
    unsigned long GetPrimaryPC() { return m_oidPrimaryPC; }
    CExoString m_sPrimaryPCName;
    void SetPrimaryPCPortrait(CResRef cPortrait);
    void SetPrimaryPCPortraitId(unsigned short nPortraitId);
    unsigned long GetPlayerId() { return m_nPlayerId; }
    CSWCPlayer(unsigned long nPlayerId)  // PC 0x0069FDF0
        : m_sPlayerName(),
          m_cPrimaryPCPortrait(),
          m_sPrimaryPCName()
    {
        m_nPlayerId = nPlayerId;
        m_nUnknown8 = 0;
        m_nPrimaryPCPortraitId = -1;
    }
};

#include "exobase.h"

class CClientMsgSysAdmin
{
public:
    int GetTokenPair(unsigned char* pData, unsigned long nSize,
                     CExoString& sCategory, CExoString& sName,
                     CExoString& sValue);
    int SendSysAdminToServerMessage(const CExoString& sMessage);
    int HandleServerToSysAdminMessage(unsigned char* pData,
                                      unsigned long nSize);
};

#endif
