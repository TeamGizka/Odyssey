#ifndef __SERVEREXOAPP_H__
#define __SERVEREXOAPP_H__

class CSWSObject;
class Vector;
#include "exobase.h"
#include "math3d.h"
#include "gameobjectarray.h"
#include "netlayer.h"

class CServerExoAppInternal;
class CServerInfo;
class CGameObject;
class CSWPartyTable;
class CGameObjectArray;
class CWorldTimer;
class CResRef;
class CWalkHitInfo;
class CPathfindInformation;
class CSWSModule;
class CSWSMessage;
class CSWSPlayer;
class CSWSItem;
class CSWSCreature;
class CSWSArea;
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
class CConnectionLib;
class CServerInfo;
class CSWGlobalVariableTable;

class CSWSClient;
class CExoString;

inline void ServerExoAppTrace(const void*)
{
}

class CServerExoApp : public CBaseExoApp
{
public:
    CServerExoApp();
    ~CServerExoApp();
    void DestroyExoAppInternal();

    virtual void PlayerListChange(unsigned long nPlayerId, int bAdded,
                                  int bGuaranteed);
    virtual int HandleMessage(unsigned long nPlayerId, unsigned char* pMessage,
                              unsigned long nSize, int bGuaranteed);
    virtual float GetFPS()  // PC 0x004AEC60  Xbox 0x0015EB60
    {
        return 100.0f;
    }
    virtual int ContinueMessageProcessing();
    virtual CNetLayer* GetNetLayer();

    virtual void ShutDownToMainMenu();

    virtual CConnectionLib* GetConnectionLib();
    virtual CExoLocString* GetModuleDescription();
    virtual int GetApplicationId();
    virtual void SetApplicationIdsMatch(int bMatch)  // PC 0x0060E760
    {
    }

    void Initialize();
    void Uninitialize();
    void StartServices(int nServices);
    void StopServices();
    void TogglePauseState(unsigned char nPauseType);
    CGameObject* GetGameObject(unsigned long oidObject);
    unsigned char GetActivePauseState();
    CSWPartyTable* GetPartyTable();
    CSWSModule* GetModule();
    CExoString GetModuleName();
    int SaveGame(unsigned long nSlot, const CExoString& sName,
                 CSWSPlayer* pPlayer, int bAutoSave);
    int LoadGame(unsigned long nSlot, const CExoString& sName,
                 const CExoString& sModule, CSWSPlayer* pPlayer);
    int LoadCharacterStart(unsigned char nType, CSWSPlayer* pPlayer,
                           CResRef cResRef, void* pData, unsigned long nSize);
    int LoadCharacterFinish(CSWSPlayer* pPlayer, int bSuccess);
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
    int MainLoop();
    void Shutdown(int bExit, int bSave);
    int UnloadModule();
    int LoadModule(const CExoString& sModule, CSWSPlayer* pPlayer);
    int RunModule();
    CExoString GetPlayerListString();

    int SendCharacterQuery(CSWSPlayer* pPlayer);
    int StartNewModule(const CExoString& sModule);
    int LoadPrimaryPlayer(CSWSPlayer* pPlayer);
    void OnCDChange();
    void OnExit();
    void OnGainFocus();
    void OnLostFocus();
    void OnVideoChange();
    int IsOnActiveExclusionList(unsigned long oidObject);
    int GetMoveToModulePending();
    CExoString GetMoveToModuleString();
    int ExportAllPlayers();
    CSWSPlayer* GetClientObjectByObjectId(unsigned long oidObject);
    CSWSPlayer* GetClientObjectByPlayerId(unsigned long nPlayerId,
                                          unsigned char nGroup);
    void InitiateModuleForPlayer(void* pData);
    void MovePlayerToArea(void* pData);
    int GetFactionOfObject(unsigned long oidObject, int* pnFaction);
    void SkipPastDialog(unsigned long oidObject);
    int GetPauseState(unsigned char nPauseType);
    void SetPauseState(unsigned char nPauseType, int bPause);
    void AddToExclusionList(unsigned long oidObject, unsigned char nType);
    void RemoveFromExclusionList(unsigned long oidObject, unsigned char nType);
    CExoLinkedList<unsigned long>* GetActiveExclusionList();
    int ResolvePlayerByFirstName(const CExoString& sFirstName);
    int GetDifficultyOption(int nOption);
    void AddCharListRequest(unsigned long nPlayerId);
    int GetFirstPCObject();
    int GetNextPCObject();
    unsigned long ClientToServerObjectId(unsigned long oidClient);
    int GetPlayerCreatureId();
    float ComputeHeight(const CResRef& cAreaResRef, const Vector& vPosition,
                        int nRoomIndex);
    void UpDatePlayerPosition(unsigned long oidObject, const Vector& vPosition,
                              const Vector& vOrientation);
    void UpdateSubareasOnMoveTo(unsigned long oidObject, const Vector& vFrom,
                                const Vector& vTo);
    int IsActionsPending(unsigned long oidObject);
    int GetRoomIndex(const CResRef& cAreaResRef, const Vector& vPosition,
                     int nFlags);
    int CreatureAcquireItem(unsigned long oidObject, const CResRef& cResRef);
    int TestSafeLocation(unsigned long oidObject, const Vector& vPosition);
    int ComputeSafeLocation(const Vector& vPosition, float fRadius,
                            unsigned long oidObject, int nFlags,
                            Vector* pvSafe);
    Vector GetNormalOfPoint(const Vector& vPosition);
    int GetSurfaceMaterial(const CResRef& cAreaResRef, Vector vPosition,
                           int nRoomIndex);
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
    int ExecuteLoadModule(const CExoString& sModule);
    CServerInfo* GetServerInfo();
    void SetMoveToModulePending(int bPending);
    void SetMoveToModuleString(const CExoString& sModule);
    void SetExportAllCharactersPending(int bPending);
    int GetLoadingModule();
    void SetLoadingModule(int bLoading);
    void SetAutoSavePending(int bPending);
    int GetAutoSavePending();
    void SetMoveToModuleStartWaypoint(const CExoString& sWaypoint);
    CExoString GetMoveToModuleStartWaypoint();
    CGameObjectArray* GetObjectArray();
    CServerAIMaster* GetServerAIMaster();
    CExoLinkedList<CSWSClient>* GetPlayerList();
    CSWSMessage* GetSWSMessage();
    CExoLinkedList<CSWSClient>* GetSysAdminList();
    short GetServerMode();
    CWorldTimer* GetWorldTimer();
    CWorldTimer* GetTimestopTimer();
    CWorldTimer* GetPauseTimer();
    void SetDebugMode(int nDebugMode);
    int GetDebugMode();
    int IsInjured(unsigned long oidObject);
    CSWGlobalVariableTable* GetGlobalVariableTable();
    void SetCreatureFollowing(unsigned long oidCreature, unsigned long oidTarget,
                              int bRun, const Vector& vDestination);
    int TestDirectLine(unsigned long oidObject, const CResRef& cAreaResRef,
                       const Vector& vStart, const Vector& vEnd,
                       const float& fDistance, float fWidth, int nFlags,
                       CWalkHitInfo* pWalkHitInfo);
    CPathfindInformation* GetCreaturePathfindInformation(
        unsigned long oidObject);
    void ClearAllActions(unsigned long oidObject);
    void ClearAllQueuedCombatActions(unsigned long oidObject);
    CFactionManager* GetFactionManager();
    void SetLastConversation(const CExoString& sConversation);
    CExoString GetLastConversation();
    void SetActionConversation(int bActionConversation);
    int GetActionConversation();
    void SetIgnoreConversationStartRange(int bIgnore);
    int GetIgnoreConversationStartRange();
    int GetLoadFromSaveGame();
    void SetLoadFromSaveGame(int bLoadFromSaveGame);
    int GetButtonMashCheck();
    void SetButtonMashCheck(int bButtonMashCheck);
    void SetDoingStartNewModule(int bDoingStartNewModule);
    void SetMoveToPausedCalendarDay(unsigned long nCalendarDay);
    unsigned long GetMoveToPausedCalendarDay();
    void SetMoveToPausedTimeOfDay(unsigned long nTimeOfDay);
    unsigned long GetMoveToPausedTimeOfDay();
    void SetSaveGameName(const CExoString& sName);
    // PC 0x004B2820  Xbox 0x0001C420
    CSWSCreature* GetPlayerCreature()
    {
        ServerExoAppTrace(this);
        return GetCreatureByGameObjectID(GetPlayerCreatureId());
    }

    CServerExoAppInternal* GetExoAppInternal()
    {
        return m_pcExoAppInternal;
    }

    void SetReloadModuleWhenEmpty(int bReload);
    CExoString GetBannedListString();
    void AddIPToBannedList(CExoString sIP);
    void AddPlayerNameToBannedList(CExoString sPlayerName);
    void AddCDKeyToBannedList(CExoString sCDKey);
    void RemoveIPFromBannedList(CExoString sIP);
    void RemovePlayerNameFromBannedList(CExoString sPlayerName);
    void RemoveCDKeyFromBannedList(CExoString sCDKey);

    CServerExoAppInternal* m_pcExoAppInternal;
};

extern int RenderPartyFollow;

#endif
