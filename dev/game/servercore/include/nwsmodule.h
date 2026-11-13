#ifndef __NWSMODULE_H__
#define __NWSMODULE_H__

#include "resifo.h"
#include "math3d.h"
#include "exoresman.h"
#include "nwjournal.h"
#include "nwsobject.h"
class CERFFile;
class CExoLinkedListInternal;
class CPathfindInformation;
class CRes;
class CResStruct;
class CSWSCreature;
class CSWSPlayer;
class NWPlayerCharacterList_st;
#include "exobase.h"
#include "swsareamap.h"

class CSWSArea;

extern int RenderAi;

#define NWSMODULE_DOOR_GOAL_DISTANCE_MARGIN 0.5f

#define NWSMODULE_PATH_GOAL_MARGIN 0.001f
#define NWSMODULE_PATH_LENGTH_MIN 0.0001f
#define NWSMODULE_PARTIAL_PATH_SEARCH_MIN 0.005f

class CSWSTagNode
{
public:
    char m_sTag[36];
    unsigned long m_oidObject;
};

class NWPlayerListItem_st
{
public:
    CExoString m_sPlayerName;
    CExoLocString m_lsFirstName;
    CExoLocString m_lsLastName;
    int m_nPlayerType;
};

class CSWSModuleEntryInfo
{
public:
    CResRef m_cEntryArea;
    float m_fEntryX;
    float m_fEntryY;
    float m_fEntryZ;
    float m_fEntryDirX;
    float m_fEntryDirY;
};

class CSWSModuleInfo
{
public:
    unsigned char m_aModuleID[32];
    int m_nCreatorID;
    unsigned long m_nVersion;
};

class NWModuleExpansion_st
{
public:
    CExoLocString m_lsExpansionName;
    int m_nExpansionID;
};

class NWModuleCutScene_st
{
public:
    CResRef m_cCutSceneName;
    unsigned long m_nCutSceneID;
};

struct NWPlayerCharacterListClass_st
{
    int m_nClass;
    unsigned char m_nClassLevel;
};

class NWPlayerCharacterList_st
{
public:

    enum CharacterListStatus
    {
        CHARACTER_LIST_MODULE_PLAYER = 14,
        CHARACTER_LIST_PLAYER_VAULT = 17,
        CHARACTER_LIST_SERVER_VAULT = 2
    };
    CExoLocString m_lsFirstName;
    CExoLocString m_lsLastName;
    unsigned char m_bCharacterExists;
    CResRef m_cCharacter;
    unsigned short m_nPortraitId;
    CResRef m_cPortrait;
    CExoArrayList<NWPlayerCharacterListClass_st> m_ClassList;
    int operator==(NWPlayerCharacterList_st& other)
    {
        return m_lsFirstName == other.m_lsFirstName &&
               m_lsLastName == other.m_lsLastName;
    }
    ~NWPlayerCharacterList_st();
    NWPlayerCharacterList_st()  // PC 0x005079C0 GOG 0x004FFA70
    {
        m_bCharacterExists = 0;
        m_nPortraitId = -1;
    }
};

class CSWSModule : public CResHelper<CResIFO, RESTYPE_IFO>,
                   public CGameObject
{
public:

    enum { GFF_STRUCT_EXPANSION = 0, GFF_STRUCT_CUTSCENE = 1,
           GFF_STRUCT_LIMBO_CREATURE = 4, GFF_STRUCT_AREA = 6,
           GFF_STRUCT_TOKEN = 7 };

    enum { GFF_STRUCT_PLAYER = 0xBEAD };

    enum { PATH_MAX_ATTEMPTS = 100, PATH_PARTIAL_SEARCH_FAILED = -4 };

    enum LoadModuleResult
    {
        LOAD_MODULE_SUCCESS = 0,
        LOAD_MODULE_RESOURCE_FAILED = 1,
        LOAD_MODULE_OUT_OF_MEMORY = 3
    };

    enum TimePeriod
    {
        TIME_DAY = 1, TIME_NIGHT = 2, TIME_DAWN = 3, TIME_DUSK = 4
    };
    enum TimeUpdateMask
    {
        TIME_UPDATE_PERIOD = 0x01, TIME_UPDATE_HOUR = 0x02,
        TIME_UPDATE_DAY = 0x04, TIME_UPDATE_MONTH = 0x08,
        TIME_UPDATE_YEAR = 0x10
    };
    CSWSModule(const CExoString& sModuleName, int bRequest, int bUnknown);
    virtual ~CSWSModule();
    virtual CSWSModule* AsSWSModule() { return this; }  // PC 0x00507D40 GOG 0x004FFDF0

    enum ScriptType
    {
        SCRIPT_HEARTBEAT, SCRIPT_USER_DEFINED, SCRIPT_MODULE_LOAD,
        SCRIPT_MODULE_START, SCRIPT_CLIENT_ENTER, SCRIPT_CLIENT_LEAVE,
        SCRIPT_ACTIVATE_ITEM, SCRIPT_ACQUIRE_ITEM, SCRIPT_UNACQUIRE_ITEM,
        SCRIPT_PLAYER_DEATH, SCRIPT_PLAYER_DYING, SCRIPT_SPAWN_BUTTON_DOWN,
        SCRIPT_PLAYER_REST, SCRIPT_PLAYER_LEVEL_UP, SCRIPT_EQUIP_ITEM,
        NUM_SCRIPTS
    };

    void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                      void* pEventData, unsigned long nCalendarDay,
                      unsigned long nTimeOfDay);
    int AIUpdate();
    void ClearAreaVisitedFlags();
    int ComputeInterAreaPath(CPathfindInformation* pPathfind);
    int GenerateInterAreaDFSSuccessors(int nArea,
                                       CPathfindInformation* pPathfind,
                                       unsigned long** ppSuccessors);
    int InterAreaDFS(int nArea, int nDepth, CPathfindInformation* pPathfind);
    int PlotInterAreaPath(CPathfindInformation* pPathfind,
                          unsigned long nFlags);
    int PlotPathInArea(CPathfindInformation* pPathfind, unsigned long nFlags);
    int PlotPath(CPathfindInformation* pPathfind, unsigned long nFlags);
    void SetIntraAreaGoal(CPathfindInformation* pPathfind);
    int DoUpdate();
    void UpdateTime(unsigned long nDay, unsigned long nTime,
                    unsigned long nDeltaTime);
    void TimeStopSanityCheck();
    int FindTagPositionInTable(char* sTag);
    int AddObjectToLookupTable(const CExoString& sTag, unsigned long oidObject);
    int RemoveObjectFromLookupTable(const CExoString& sTag,
                                    unsigned long oidObject);
    unsigned long FindObjectByTagOrdinal(const CExoString& sTag,
                                         unsigned long nNth);
    unsigned long FindObjectByTagTypeOrdinal(const CExoString& sTag,
                                             int nObjectType,
                                             unsigned long nNth);
    int GetPrimaryPlayerIndex();
    int GetPlayerIndexInPlayerList(CSWSPlayer* pPlayer);
    CSWSArea* GetArea();
    int GetWorldJournalIndexUnique();
    void AddWorldJournalEntry(int nIndex, const CExoString& sText,
                              const CExoString& sTag,
                              unsigned long nCalendarDay,
                              unsigned long nTimeOfDay);
    void AddWorldJournalEntryStrref(unsigned long nIndex, unsigned long nStrRef,
                                    unsigned long nCalendarDay,
                                    unsigned long nTimeOfDay);
    void DeleteWorldJournalAllEntries();
    void DeleteWorldJournalEntryStrref(unsigned long nStrRef);
    void DeleteWorldJournalEntry(int nIndex);
    void SetTag(const CExoString& sTag)  // PC 0x005040C0 GOG 0x004FC1A0
    {
        m_sTag = sTag.LowerCase();
    }
    unsigned long FindObjectByTagOrdinal(const CExoString& sTag, int nOrdinal);
    void ClearAreaCache();
    int GetTime(unsigned long* pnYear, unsigned long* pnMonth,
                unsigned long* pnDay, unsigned long* pnTimeOfDay,
                unsigned char* pnDawnDusk, unsigned long* pnTimeOfDay2);
    CExoString GetModuleResourceName();

    CExoString& GetModuleName() { return m_sModuleName; }
    void SaveModuleStart(CExoString& sPath, CExoString& sName);
    int SaveModuleFAC();
    int SaveModuleIFOStart(CResGFF* pRes, CResStruct* pStruct);
    void SavePrimaryPlayerInfo(CSWSCreature* pCreature,
                               const CExoString& sPath);
    int SaveModuleInProgress();

    int LoadModuleInProgress(int nAreasLoaded, int nAreasToLoad);
    int LoadModuleFinish();
    void PostProcess();
    int LoadModuleStart(const CExoString& sModule, int bLoadModuleTime);
    void AddModuleResources(const CExoString& sModuleName);
    void RemoveModuleResources(const CExoString& sModuleName);
    int SaveModuleFinish(CExoString& sPath, CExoString& sName);
    int SavePlayers(CResGFF* pRes, CResStruct* pStruct,
                    const CExoString& sPath,
                    CExoArrayList<unsigned long>& lstPlayers);
    int SaveModuleIFOFinish(CResGFF* pRes, CResStruct* pStruct,
                            CERFFile* pERFFile, CExoString& sPath,
                            CExoArrayList<unsigned long>& lstPlayers);

    int SaveStatic(CERFFile* pERFFile, const CExoString& sFileType,
                   unsigned short nResType, int bIsGFF);
    void PackModuleIntoMessage(unsigned long nPlayerId);
    void PackPlayerCharacterListIntoMessage(CSWSPlayer* pPlayer,
        CExoArrayList<NWPlayerCharacterList_st*>& lstCharacters);
    void AddObjectToLimbo(unsigned long oidObject);
    
    void SaveLimboCreatures(CResGFF* pRes, CResStruct* pStruct);
    int LoadLimboCreatures(CResGFF* pRes, CResStruct* pStruct,
                           int bIsSaveGame);
    void RemoveObjectFromLimbo(unsigned long oidObject);
    int IsObjectInLimbo(unsigned long oidObject);
    unsigned long GetWaypoint(const CExoString& sTag);

    CExoLinkedList<NWModuleExpansion_st> m_lstModuleExpansion;
    CExoLinkedList<NWModuleCutScene_st> m_lstModuleCutScene;

    CResRef m_cAreaResRef;
    unsigned long m_oidArea;
    int m_bAIUpdate;
    CExoLinkedList<NWPlayerListItem_st> m_lstPlayers;
    CExoLocString m_sDescription;
    CSWSModuleInfo* m_pModuleInfo;
    CSWSModuleEntryInfo* m_pEntryInfo;
    CExoString m_sModuleName;
    CExoLocString m_lsName;
    CResRef m_cHak;
    CResRef m_cStartMovie;

    CSWSScriptVarTable m_ScriptVarTable;

    CSWVarTable m_lVarTable;
    CExoString m_sScripts[NUM_SCRIPTS];
    unsigned long m_nHeartbeatDay;
    unsigned long m_nHeartbeatTime;
    CExoArrayList<CSWSTagNode> m_lstTagTable;

    CExoArrayList<unsigned long> m_lstInvisibleCreatures;
    int m_nUserDefinedEvent;
    unsigned long m_oidClientEnter;
    unsigned long m_oidClientLeave;
    unsigned long m_oidAcquiredItem;
    unsigned long m_oidItemAcquirer;
    unsigned long m_oidUnacquiredItem;
    unsigned long m_oidItemUnacquirer;
    unsigned long m_oidRestingPlayer;
    unsigned char m_bRestCancelled;
    char m_aUnmapped169[3];
    unsigned long m_oidDeadPlayer;
    unsigned long m_oidDyingPlayer;
    unsigned long m_oidRespawningPlayer;
    unsigned long m_oidActivatedItem;
    unsigned long m_oidItemActivator;
    unsigned long m_oidActivatedTarget;
    unsigned long m_oidActivatedTarget2;
    Vector m_vActivatedLocation;
    unsigned long m_oidLevelingPlayer;
    unsigned long m_n198;
    unsigned long m_oidEquippedItem;
    unsigned char m_nMinutesPerHour;
    unsigned char m_nDawnHour;
    unsigned char m_nDuskHour;

    unsigned char m_nStartMonth;
    unsigned char m_nStartDay;
    unsigned char m_nStartHour;
    unsigned char m_nXPScale;
    char m_aUnmapped1A7[1];
    unsigned long m_nYear;
    unsigned long m_nMonth;
    unsigned long m_nDay;
    unsigned long m_nTime;
    unsigned long m_nTime2;
    unsigned char m_nTimeDawnDusk;
    char m_aUnmapped1BD[3];
    unsigned long m_nLastUpdateDay;
    unsigned long m_nLastUpdateTime;

    int m_bIsSaveGame;
    CExoArrayList<CWorldJournalEntry> m_lstWorldJournal;
    int m_n1D8;
    CExoArrayList<unsigned long> m_lLimbo;
    CERFFile* m_pERFFile;
    CResStruct* m_pERFStruct;
    CResGFF* m_pModuleGFF;
    CExoArrayList<unsigned long>* m_plstPlayers;
    CExoString m_sTag;
    int m_bIsNWMFile;
    CExoString m_sSaveName;

    int m_nResourceCount;
    unsigned char m_n210;
    char m_aUnmapped211[3];
    int m_n214;

    CSWSAreaMap* GetAreaMap() { return m_pAreaMap; }

    CSWSAreaMap* m_pAreaMap;
    unsigned long m_oidAreaCached;
    CSWSArea* m_pAreaCached;

public:

    CSWSScriptVarTable* GetScriptVarTable()
    {
        return &m_ScriptVarTable;
    }
    int GetIsSaveGame()
    {
        return m_bIsSaveGame;
    }
    void SetIsSaveGame(int nIsSaveGame)
    {
        m_bIsSaveGame = nIsSaveGame;
    }
    unsigned char GetXPScale()
    {
        return m_nXPScale;
    }
    void SetXPScale(unsigned char nXPScale)
    {
        m_nXPScale = nXPScale;
    }
    unsigned char GetDawnHour()
    {
        return m_nDawnHour;
    }
    unsigned char GetDuskHour()
    {
        return m_nDuskHour;
    }
    CExoString GetTag()
    {
        return m_sTag;
    }
    int GetIsNWMFile()
    {
        return m_bIsNWMFile;
    }
    void SetIsNWMFile(int nIsNWMFile)
    {
        m_bIsNWMFile = nIsNWMFile;
    }
    CResRef* GetStartMovie()
    {
        return &m_cStartMovie;
    }
    unsigned char GetMinutesPerHour()
    {
        return m_nMinutesPerHour;
    }
#ifdef _XBOX
    void TestPathPoints(char* szArgs);
#endif
};

#endif
