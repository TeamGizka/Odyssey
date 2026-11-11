#ifndef __NWSENCOUNTER_H__
#define __NWSENCOUNTER_H__

#include "exobase.h"
#include "exoresman.h"
#include "nwsobject.h"

class CResGFF;
class CResList;
class CResStruct;
class CSWSArea;

#define ENCOUNTER_SPAWN_COST_TOLERANCE 0.001f

#define ENCOUNTER_SPAWN_POOL_ALLOWANCE_DIVISOR 10.0f

#define ENCOUNTER_ENEMY_SEARCH_RADIUS 30.0f

#define ENCOUNTER_SAFE_SPAWN_RADIUS 20.0f
#define ENCOUNTER_NON_VISIBLE_SPAWN_RADIUS 40.0f
#define ENCOUNTER_SPAWN_ORIENTATION_OFFSET HALF_PI

#define ENCOUNTER_DEFAULT_AREA_HALF_EXTENT 7.5f

class CEncounterListEntry
{
public:
    CEncounterListEntry()  // PC 0x0058FF80  Xbox 0x0023E790
    {
        m_cResRef = "";
        m_fChallengeRating = 0.0f;
        m_fSpawnPoolCost = 0.0f;
        m_bSingleSpawn = 0;
        m_bSpawned = 0;
        m_bSelected = 0;
    }
    ~CEncounterListEntry() {}  // PC 0x004015A0
    CEncounterListEntry& operator=(const CEncounterListEntry& other);

    CResRef m_cResRef;
    float m_fChallengeRating;
    float m_fSpawnPoolCost;
    int m_bSpawned;
    int m_bSelected;
    int m_bSingleSpawn;
};

class CEncounterSpawnListEntry
{
public:
    CEncounterSpawnListEntry()
    {
        m_cResRef = "";
        m_fChallengeRating = 0.0f;
        m_nSpawnCount = 0;
        m_nTotalSpawned = 0;
        m_nSpawnLimit = 0;
        m_nSpawned = 0;
    }

    CResRef m_cResRef;
    float m_fChallengeRating;
    int m_nSpawnCount;
    int m_nSpawnLimit;
    int m_nSpawned;
    int m_nTotalSpawned;
};

class CEncounterSpawnPoint
{
public:
    CEncounterSpawnPoint();

    Vector m_vPosition;
    float m_fOrientation;
};

class CSWSEncounter : public CSWSObject
{
public:

    enum { SPAWN_OPTION_ONCE = 0, SPAWN_OPTION_CONTINUOUS = 1 };

    enum { RESPAWNS_UNLIMITED = -1 };

    static const unsigned long HEARTBEAT_INTERVAL = 6000;

    CSWSEncounter(unsigned int oidEncounter);

    CSWSEncounter(unsigned long oidEncounter);
    virtual ~CSWSEncounter();
    virtual CSWSEncounter* AsSWSEncounter() { return this; }  // PC 0x00641DB0  Xbox 0x00241E90
    virtual void AIUpdate();

    int LineSegmentIntersectActivateArea(const Vector& vStart,
                                         const Vector& vEnd);
    int GetInActivateArea(const Vector& vPosition);
    int GetInActivateAreaList(unsigned long oidObject);
    void AddToActivateAreaList(unsigned long oidObject);
    int RemoveFromActivateAreaList(unsigned long oidObject);
    void LoadEncounterSpawnPoints(CResGFF* pRes, CResList& lstSpawnPoints);
    int LoadEncounterGeometry(CResGFF* pRes, CResList& lstGeometry);
    void ReadEncounterScriptsFromGff(CResGFF* pRes, CResStruct* pStruct);
    void SetDifficulty(int nDifficulty);
    void PadOutCreatureCount(CEncounterListEntry* pEntries, int& nCount);
    void UpdateActivateAreaList(unsigned long oidObject);
    void BuildInitialSpawnList(CEncounterListEntry* pEntries, int& nCount);
    int SaveEncounter(CResGFF* pRes, CResStruct* pStruct);
    float CalculatePointsFromCR(float fChallengeRating);
    void TallyEnemyRadiusPoints();
    void SpawnCreatureFromSpawnList();
    void ClearSpawnList();
    void RemoveFromArea();
    void AddCreaturesToSpawnList(CEncounterListEntry* pEntries, int& nCount,
                                int nIndex, CEncounterListEntry cEntry);
    int ReadEncounterFromGff(CResGFF* pRes, CResStruct* pStruct,
                             int bLoadPosition, Vector* pvPosition);
    void CalculateSpawnPool(float fMultiplier);
    void LoadFractionalCRData();
    void AddToArea(CSWSArea* pArea, float fX, float fY, float fZ, int nFlags);
    void BuildSpawnList();
    int LoadEncounter(CResGFF* pRes, CResStruct* pStruct);
    int LoadFromTemplate(const CResRef& cResRef, int bLoadPosition,
                         Vector* pvPosition);
    void SpawnIfAppropriate(unsigned long oidObject, float fMultiplier);
    void SetActive(int bActive);
    virtual void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay);
    void RemoveFromActiveCreatureCount(float fChallengeRating, int bAddToPool);
    virtual CExoLocString* GetFirstName()  // PC 0x00593510  Xbox 0x00241EA0
    {
        return &m_lsFirstName;
    }

    unsigned short m_nActivateObjectArrayIndex;
    unsigned long m_nFaction;
    CExoLocString m_lsFirstName;
    int m_bActive;
    int m_bReset;
    int m_nResetTime;
    int m_nSpawnOption;
    int m_nDifficulty;
    int m_nDifficultyIndex;
    int m_nRecommendedCreatures;
    int m_nMaxCreatures;
    CExoArrayList<CEncounterListEntry*> m_lstSpawnList;
    int m_nNumberSpawned;
    unsigned long m_nHeartbeatDay;
    unsigned long m_nHeartbeatTime;
    unsigned long m_nLastSpawnDay;
    unsigned long m_nLastSpawnTime;
    int m_bStarted;
    int m_bExhausted;
    int m_nAreaListMaxSize;
    unsigned long* m_pActivateAreaList;
    int m_nActivateAreaListSize;
    int m_bSpawnTriggered;
    Vector* m_pActivateAreaVertices;
    int m_nActivateAreaVertexDataSize;
    unsigned long* m_pActivateAreaIndices;
    int m_nActivateAreaVertexCount;
    float m_fMinX;
    float m_fMaxX;
    float m_fMinY;
    float m_fMaxY;
    CEncounterListEntry* m_pSpawnEntries;
    int m_nSpawnEntryCount;
    CEncounterSpawnPoint* m_pSpawnPoints;
    int m_nSpawnPointCount;
    int m_nSpawnLevel;
    int m_nRespawns;
    int m_nCurrentSpawns;

    float m_fCreaturePoints;
    float m_fMinSpawnPoolCost;
    float m_fAreaPoints;
    float m_fSpawnPool;
    float m_fSpawnPoolActive;
    unsigned long m_oidLastEntered;
    unsigned long m_oidLastLeft;
    CExoString m_asScripts[5];
    CExoArrayList<float>* m_plstFractionalCR;
    int m_nCustomScriptId;
    int m_bPlayerOnly;

    static CExoArrayList<float> m_sPowerLookup;

public:

    unsigned long GetLastEntered()
    {
        return m_oidLastEntered;
    }
    unsigned long GetLastLeft()
    {
        return m_oidLastLeft;
    }
    int GetActive()
    {
        return m_bActive;
    }
    int GetDifficulty()
    {
        return m_nDifficulty;
    }
    int GetDifficultyIndex()
    {
        return m_nDifficultyIndex;
    }
    void SetLastEntered(unsigned long nLastEntered)
    {
        m_oidLastEntered = nLastEntered;
    }
    void SetLastLeft(unsigned long nLastLeft)
    {
        m_oidLastLeft = nLastLeft;
    }
    unsigned short GetActivateObjectArrayIndex()
    {
        return m_nActivateObjectArrayIndex;
    }
    void SetActivateObjectArrayIndex(unsigned short nActivateObjectArrayIndex)
    {
        m_nActivateObjectArrayIndex = nActivateObjectArrayIndex;
    }
};

#endif
