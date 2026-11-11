#ifndef __SERVEREXOAPPINTERNAL_H__
#define __SERVEREXOAPPINTERNAL_H__


#define SERVER_TEMP_DEATH_SAFE_RADIUS 5.0f
#define SERVER_TEMP_DEATH_MAX_DISTANCE_SQUARED 1600.0f

class C2DA;
class CResGFF;
class CResStruct;
class Vector;
#include "exobase.h"
#include "math3d.h"
#include "nwsvartable.h"
#include "partytables.h"

class CWorldTimer;
class CGameObjectArray;
class CGameObject;
class CResRef;
class CSWSModule;
class CSWSMessage;
class CSWSPlayer;
class CSWSItem;
class CSWSCreature;
class CSWSArea;
class CPathfindInformation;
class CSWSTrigger;
class CSWSPlaceable;
class CSWSAreaOfEffectObject;
class CSWSWaypoint;
class CSWSEncounter;
class CSWSStore;
class CSWSDoor;
class CSWSSoundObject;
class CServerAIMaster;
class CFactionManager;
class CNetLayer;
class CServerInfo;
class CConnectionLib;
class CSWVirtualMachineCommands;




class CExoArrayList;
class CSWSClient;
class CExoString;
class CSWGlobalVariableTable;
class CSWPartyTable;

#ifndef _XBOX
#pragma pack(push, 4)
#endif

class CServerExoAppInternal
{
public:
    static const unsigned __int64 CCLIENTGAMEOBJECTUPDATETIME;
    static const int STRREF_DISCONNECT_SERVERSIDE;
    static const int STRREF_DISCONNECT_CLIENTSIDE;
    static const unsigned __int64 SHUTDOWN_TIMER_CLIENTCHARS;
    static const unsigned __int64 SHUTDOWN_TIMER_SERVERCHARS;
    static const unsigned __int64 SHUTDOWN_TIMER_WARNING1;
    static const unsigned __int64 SHUTDOWN_TIMER_WARNING2;
    static const int STRREF_LOGIN_FAIL_NOTALLOWLOCALVAULT;
    static const int STRREF_LOGIN_FAIL_NOTALLOWSERVERVAULT;
    static const int STRREF_LOGIN_FAIL_NOTALLOWEDTOUSECHAR;


    enum
    {
        DEFAULT_SAVE_SIZE = 3750000,
        DEFAULT_MODULE_SAVE_SIZE = 2500000
    };

    void SetEstimatedSaveSize(const CExoString& sModuleName, unsigned short nModuleType);

    enum
    {
        PAUSE_STATE_PAUSED = 1,
        PAUSE_STATE_COMBAT = 2
    };



    enum
    {
        SHUTDOWN_CLIENT_LOOP_LIMIT = 256,
        SHUTDOWN_REPLY_TIMEOUT = 180000000,
        SHUTDOWN_IMMEDIATE_TIMEOUT = 1000000,


        SHUTDOWN_COMPLETE_TIMEOUT = 1,
        SHUTDOWN_MESSAGE_TIME_UNIT = 100000,


        SHUTDOWN_FIRST_WARNING_TIME = 60000000,
        SHUTDOWN_SECOND_WARNING_TIME = 30000000,
        SHUTDOWN_MICROSECONDS_PER_SECOND = 1000000
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

    union SPauseState
    {
        unsigned char m_nPauseState;
        SPauseStateFlags m_Flags;
    };




    enum
    {
        SERVER_MODE_IDLE = 0,
        SERVER_MODE_LOADING = 1,
        SERVER_MODE_RUNNING = 2
    };

    CServerExoAppInternal();
    ~CServerExoAppInternal();
    static CServerExoAppInternal* s_pServerExoAppInternal;

    CSWSModule* GetModule();
    CExoString GetModuleName();
    int SaveGame(unsigned long nSlot, const CExoString& sName,
                 CSWSPlayer* pPlayer, int bAutoSave);
    void StallEventSaveGame();
    int LoadGame(unsigned long nSlot, const CExoString& sName,
                 const CExoString& sModule, CSWSPlayer* pPlayer);
    int LoadCharacterStart(unsigned char nType, CSWSPlayer* pPlayer,
                           CResRef cResRef, void* pData, unsigned long nSize);
    int ValidatePlayerLogin(void* pPlayerData);
    int LoadCharacterFinish(CSWSPlayer* pPlayer, int bSuccess);
    int LoadPrimaryPlayer(CSWSPlayer* pPlayer);
    CGameObject* GetGameObject(unsigned long oidObject);





    CWorldTimer* GetWorldTimer() { return m_pWorldTimer; }
    CGameObjectArray* GetObjectArray() { return m_pObjectArray; }
    CSWSItem* GetItemByGameObjectID(unsigned long oidObject);
    CSWSCreature* GetCreatureByGameObjectID(unsigned long oidObject);
    CSWSArea* GetAreaByGameObjectID(unsigned long oidObject);
    CSWSTrigger* GetTriggerByGameObjectID(unsigned long oidObject);
    CSWSPlaceable* GetPlaceableByGameObjectID(unsigned long oidObject);
    CSWSStore* GetStoreByGameObjectID(unsigned long oidObject);
    CSWSDoor* GetDoorByGameObjectID(unsigned long oidObject);
    CSWSAreaOfEffectObject* GetAreaOfEffectByGameObjectID(
        unsigned long oidObject);
    CSWSWaypoint* GetWaypointByGameObjectID(unsigned long oidObject);
    CSWSEncounter* GetEncounterByGameObjectID(unsigned long oidObject);
    CSWSSoundObject* GetSoundObjectByGameObjectID(unsigned long oidObject);
    int GetPlayerLanguage(unsigned long nPlayerId);
    int GetModuleLanguage();
    CWorldTimer* GetActiveTimer(unsigned long nTimer);
    int HandleMessage(unsigned long nPlayerId, unsigned char* pMessage,
                      unsigned long nSize, int bGuaranteed);
    int Initialize();
    int CopyModuleToCurrentGame(const CExoString& sSource,
                                const CExoString& sDestination,
                                unsigned short nFlags);
    void Uninitialize();
    int MainLoop();
    void TempDeathUpdate();
    void PlayerListChange(unsigned long nPlayerId, int bAdded, int bGuaranteed);
    int ContinueMessageProcessing();
    void Shutdown(int bExit, int bSave);
    int UnloadModule();
#ifdef _XBOX
    int LoadModule(CExoString sModule, CSWSPlayer* pPlayer);
#else
    int LoadModule(const CExoString& sModule, CSWSPlayer* pPlayer);
#endif
    int RunModule();


    enum
    {
        TEMP_DEATH_CHECK_INTERVAL = 1000,
        TEMP_DEATH_RECOVERY_DELAY = 5 * TEMP_DEATH_CHECK_INTERVAL
    };

    void SendExitingStartNewModuleMessage(int bSuccess);
    int SendEnteringStartNewModuleMessage();
    void SendStartStallEvent(unsigned long nEvent);
    void RunDeathSequence();
    CExoString GetPlayerListString();

    void InitializeNetLayer();
    void StartServices(int nServices);

    void StopServices();
    int ExportAllPlayers();
    int StorePlayerCharacters();
    int ValidateCreateServerCharacter(CSWSPlayer* pPlayer, void* pData, unsigned long nSize);
    int StoreCurrentModule();
    void UpdateClientGameObjects(int bFlag);
    void UpdateClientGameObjectsForPlayer(CSWSPlayer* pPlayer, int bFlag,
                                          unsigned __int64 nTime);
    int SendCharacterQuery(CSWSPlayer* pPlayer);
    CSWSPlayer* GetClientObjectByObjectId(unsigned long oidObject);
    CSWSPlayer* GetClientObjectByPlayerId(unsigned long nPlayerId,
                                          unsigned char nGroup);
    void InitiateModuleForPlayer(void* pData);
    void MovePlayerToArea(void* pData);


    void OnCDChange();
    void OnExit();
    void OnGainFocus();
    void OnLostFocus();
    void OnVideoChange();
    int GetFactionOfObject(unsigned long oidObject, int* pnFaction);
    void TogglePauseState(unsigned char nPauseType);
    void SkipPastDialog(unsigned long oidObject);
    int GetPauseState(unsigned char nPauseType);
    unsigned char GetActivePauseState();
    void SetPauseState(unsigned char nPauseType, int bPause);
    void ReprocessExclusionListActions(unsigned char nPauseType);
    void AddToExclusionList(unsigned long oidObject, unsigned char nType);
    void RemoveFromExclusionList(unsigned long oidObject, unsigned char nType);
    CExoLinkedList<unsigned long>* GetActiveExclusionList();
    int IsOnActiveExclusionList(unsigned long oidObject);
    int ResolvePlayerByFirstName(const CExoString& sFirstName);
    int GetDifficultyOption(int nOption);
    void AddCharListRequest(unsigned long nPlayerId);
    int GetFirstPCObject();
    int GetNextPCObject();
    unsigned long ClientToServerObjectId(unsigned long oidClient);
#ifdef _XBOX
    int DebugValidatePath(unsigned long oidCreature, int bTempWayPoints); int DebugValidatePath_Point(CSWSArea* pArea, Vector vPoint); int DebugValidatePath_Segment(CSWSArea* pArea, CPathfindInformation* pPathfind, Vector vStart, Vector vEnd);
#else
    int DebugValidatePath(unsigned long oidCreature, int bTempWayPoints); int DebugValidatePath_Point(CSWSArea* pArea, const Vector& vPoint); int DebugValidatePath_Segment(CSWSArea* pArea, CPathfindInformation* pPathfind, const Vector& vStart, const Vector& vEnd);
#endif
    int GetPlayerCreatureId();
    void UpDatePlayerPosition(unsigned long oidObject, const Vector& vPosition,
                              const Vector& vOrientation);
    void UpdateSubareasOnMoveTo(unsigned long oidObject, const Vector& vFrom,
                                const Vector& vTo);
    int IsActionsPending(unsigned long oidObject);
    int CreatureAcquireItem(unsigned long oidObject, const CResRef& cResRef);
#ifdef _XBOX
    int TestSafeLocation(unsigned long oidObject, Vector vPosition); int ComputeSafeLocation(Vector vPosition, float fRadius, unsigned long oidObject, int nFlags, Vector* pvSafe); Vector GetNormalOfPoint(Vector vPosition);
#else
    int TestSafeLocation(unsigned long oidObject, const Vector& vPosition); int ComputeSafeLocation(const Vector& vPosition, float fRadius, unsigned long oidObject, int nFlags, Vector* pvSafe); Vector GetNormalOfPoint(const Vector& vPosition);
#endif
    void ClearLastModuleFound();
    void ClearAreaCache();
    int ShrinkLineByCreatures(const Vector& vStart, const Vector& vEnd,
                              Vector& vResult);
    void SetMoveToWorldTime(unsigned long nCalendarDay, unsigned char nHour,
                           unsigned char nMinute, unsigned char nSecond,
                           unsigned short nMillisecond, unsigned short nYear,
                           unsigned short nMonth);
    void GetMoveToWorldTime(unsigned long& nCalendarDay, unsigned char& nHour,
                            unsigned char& nMinute, unsigned char& nSecond,
                            unsigned short& nMillisecond, unsigned short& nYear,
                            unsigned short& nMonth);
    int GetModulePreviouslyVisited(const CExoString& sModule);
    void DoModuleEnterSaveCleanup(const CExoString& sModule, C2DA* p2DA);
    int HasEnoughDiskSpaceForSaveGame();
    int IncludeModuleInSave(const CExoString& sModule);
    int ExecuteLoadModule(const CExoString& sModule);
    int StartNewModule(const CExoString& sModule);
#ifdef _XBOX
    void DoPCAutosave(const CExoString& sModule, class KOTOR_LAUNCH_DATA& cLaunchData, C2DA* p2DA);
    int IsMoveToModuleMinGlobalRim(); CExoString GetLastConversation();
#else
    void DoPCAutosave(const CExoString& sModule, C2DA* p2DA);
#endif
    int IsInjured(unsigned long oidObject);
    int UpdateAutoSaveTimer();
    int AutoSaveOnEnteringModule(const CExoString& sModule, C2DA* p2DA);
    int UpdateMainLoopTimeBudget(unsigned __int64 nTime);
    int UpdateShutdownTimer(unsigned __int64 nTime);
    void UpdateMapData();
    void ForceRunAreaOnExitScripts();
    void DumpServerOptionsToLog();

    void GetServerInfoFromIniFile();

    void DealWithLoadGameError(unsigned long nValue);

    enum { LOAD_GAME_ERROR_COPY_MODULE = 5 };
    unsigned char m_aUnmapped0[0x10004];
    CServerInfo* m_pServerInfo;
    unsigned short m_nServerMode;
    char m_aUnmapped1000A[2];

    CSWVirtualMachineCommands* m_pVirtualMachineCommands;
    CSWSMessage* m_pSWSMessage;

    int m_bForceUpdate;

    unsigned long* m_pServerOptions;

    CExoArrayList<unsigned long> m_lstPendingPlayers;

    unsigned __int64 m_nShutdownTimeout;
    unsigned __int64 m_nShutdownStartTime;
    unsigned __int64 m_nShutdownMessageDelay;
    CNetLayer* m_pNetLayer;
    CServerAIMaster* m_pServerAIMaster;
    CWorldTimer* m_pWorldTimer;
    CWorldTimer* m_pTimestopTimer;
    CWorldTimer* m_pPauseTimer;
    CFactionManager* m_pFactionManager;
    CConnectionLib* m_pConnectionLib;
    CGameObjectArray* m_pObjectArray;
    unsigned long m_oidModule;
    CExoLinkedList<CSWSClient>* m_pPlayerList;
    CExoLinkedList<CSWSClient>* m_pSysAdminList;
    int m_nDebugMode;
    CExoLinkedList<unsigned long>* m_pTimestopExclusionList;
    CExoLinkedList<unsigned long>* m_pPauseExclusionList;
    SPauseState m_PauseState;
    char m_aUnmapped10079[3];
    int m_bDoingStartNewModule;
    int m_bMoveToModulePending;
    CExoString m_sMoveToModule;
    CExoString m_sMoveToModuleStartWaypoint;

    CExoString m_sModuleTransition;
    unsigned long m_nMoveToWorldCalendarDay;
    unsigned char m_nMoveToWorldHour;
    unsigned char m_nMoveToWorldMinute;
    unsigned char m_nMoveToWorldSecond;
    char m_aUnmapped100A3[1];
    unsigned short m_nMoveToWorldMillisecond;
    unsigned short m_nMoveToWorldYear;
    unsigned short m_nMoveToWorldMonth;
    char m_aUnmapped100AA[2];
    unsigned long m_nMoveToPausedTimeOfDay;
    unsigned long m_nMoveToPausedCalendarDay;
    int m_bLoadingModule;
    int m_bAutoSavePending;
    int m_bExportAllCharactersPending;

    int m_bNeedCharacterPending;
    int m_bLoadFromSaveGame;

    unsigned long m_nEstimatedSaveSize;


    unsigned long m_nLastTempDeathUpdate;
    unsigned long m_nTempDeathCheckElapsed;
    unsigned long m_nTempDeathRecoveryElapsed;
    int m_bButtonMashCheck;
    unsigned __int64 m_nMainLoopTimeBudget;
    unsigned __int64 m_nLastMainLoopTime;
    CExoArrayList<unsigned long> m_lstCharListRequests;
    CExoLinkedListNode* m_pPCListPosition;
    CSWGlobalVariableTable m_globalVariableTable;
    CSWPartyTable m_partyTable;
    CExoString m_sLastConversation;
    int m_nActionConversation;
    int m_bIgnoreConversationStartRange;
    unsigned long m_oidLastModuleFound;
    CSWSModule* m_pLastModuleFound;
    unsigned long m_oidAreaCache;
    CSWSArea* m_pAreaCache;



    unsigned long m_nAutoSaveCalendarDay;
    unsigned long m_nAutoSaveTimeOfDay;
    int m_nAutoSaveElapsedTime;
    int m_nAutoSaveLastTime;
    int m_bAutoSaveTimerExpired;
#ifndef _XBOX
    CExoString m_sSaveGameName;
#endif
};

#ifndef _XBOX
#pragma pack(pop)
#endif

class CStatusSummary
{
public:
    unsigned int m_bDisplaySPending : 1;
    unsigned int m_bSoundPending : 1;
    unsigned int m_bItemReceived : 1;
    unsigned int m_bItemLost : 1;
    unsigned int m_bJournal : 1;
    unsigned int m_bLevelUpSound : 1;
    unsigned int m_bNewQuestSound : 1;
    unsigned int m_bCompleteSound : 1;
    unsigned int m_bDarkAlignSound : 1;
    unsigned int m_bLightAlignSound : 1;
    unsigned int m_bCreditsNet : 1;
    unsigned int : 21;
    unsigned char m_nLightShift;
    unsigned char m_nDarkShift;
    char m_aUnmapped6[2];
    int m_nCredits;
    int m_nExperience;
    int m_nStealthExperience;
    int m_nSuppressed;
    int m_nInfluenceGain;
    int m_nInfluenceLost;
    int m_nMaxForcePointsGain;
    int m_nMaxForcePointsLost;
    int SaveToGFF(CResGFF* pRes, CResStruct& stStruct);
    int LoadFromGFF(CResGFF* pGFF, CResStruct& cStruct);
};

#endif
