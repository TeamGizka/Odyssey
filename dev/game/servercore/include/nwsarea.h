#ifndef __NWSAREA_H__
#define __NWSAREA_H__

#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "nwarea.h"
#include "nwsobject.h"
#include "nwsvartable.h"
#include "swminigame.h"

template <class T> class CExoArrayList;
class CExoString;
class CERFFile;
class CGameEffect;
class CLYT;
class CPathfindInformation;
class CResARE;
class CResGFF;
class CResRef;
class CResStruct;
class CSWCArea;
class CSWSAmbientSound;
class CSWSPathPointSuccessors;
class CSWSObject;
class CSWSRoom;
class CSWSPlayer;
class CWalkHitInfo;

struct CSWSAreaVersionInfo
{
    int m_nID;
    int m_nCreatorID;
    unsigned long m_nVersion;
};

struct NWAreaExpansion_st
{
    CExoLocString m_sExpansionName;
    int m_nExpansionID;
};

#define MAX_GRID_DFS_SEARCH_NODES 64

struct CSWSAreaGridTransEntry
{
    int m_nX;
    int m_nY;
    int m_nScore;
    int m_nTieBreak;
};

struct CSWSAreaGridSuccessors

{
    int m_nCost;
    int m_nX;
    int m_nY;
    int m_nZ;
};

struct CSWSAreaPathPointTransEntry
{
    float m_fX;
    float m_fY;
    float m_fScore;
    float m_fTieBreak;
};

struct CSWSAreaPathPoint
{
    float m_fX;
    float m_fY;
    int m_nConnectionCount;
    int m_nFirstConnection;
};

class CSWSPathPointSuccessors
{
public:
    unsigned long m_nPathPoint;
    float m_fDistance;
    float m_fDistanceToGoal;
};

extern int g_nAreaGridWidth;
extern int g_nAreaGridHeight;

extern int DumpRoomBBox;
extern int RenderTest;

extern int SmoothTime;

extern int bDumpData;
extern int g_bRenderClipWalkable;

#define AREA_DIRECT_LINE_HEIGHT_MARGIN 0.1f
#define AREA_DIRECT_LINE_OBJECT_HEIGHT_RANGE 1000.0f

#define AREA_DOOR_CROSSING_HEIGHT 0.5f

#define AREA_PATH_POINT_TEST_HEIGHT 0.03f

#define AREA_LINE_OF_SIGHT_RADIUS 0.001f

#define AREA_SOUND_STATIC_OBSTRUCTION 0.4f
#define AREA_SOUND_DOOR_OBSTRUCTION 0.55f
#define AREA_SOUND_OBSTRUCTION_STEP 0.1f
#define AREA_SOUND_OBSTRUCTION_TIME_LIMIT 100

#define AREA_SAFE_LOCATION_HORIZONTAL_RADIUS 0.01f
#define AREA_SAFE_LOCATION_VERTICAL_RADIUS 0.1f

#define AREA_SAFE_DIRECTION_PROBE_RADIUS 0.01f
#define AREA_SAFE_DIRECTION_HEIGHT_MARGIN 0.1f
#define AREA_SAFE_DIRECTION_STEP_ALLOWANCE 0.01

#define AREA_SAFE_LOCATION_INITIAL_RADIUS 1.0f

#define AREA_NON_VISIBLE_PLAYER_LIMIT 96
#define AREA_NON_VISIBLE_SEARCH_RADIUS 20.0f
#define AREA_NON_VISIBLE_SIGHT_DISTANCE 30.0f

#define AREA_NON_VISIBLE_INVALID_COORDINATE -1.0f

#define AREA_CREATURE_LINE_SEARCH_MARGIN 6.0f
#define AREA_CREATURE_LINE_RADIUS_MARGIN 0.01f

#define AREA_SHRINK_LINE_SEARCH_MARGIN 3.0f
#define AREA_SHRINK_LINE_HIT_BACKOFF 0.001f
#define AREA_SHRINK_LINE_HEIGHT_BIAS 0.01f

#define AREA_BUDGE_SEARCH_MARGIN 3.0f
#define AREA_BUDGE_SAFE_LOCATION_RADIUS 10.0f

#define AREA_CREATURE_LOAD_SAFE_LOCATION_RADIUS 20.0f

#define AREA_PATH_POINT_SEARCH_STEP 10.0f
#define AREA_PATH_POINT_EXCLUSION_MARGIN 0.01f

#define AREA_PATH_POINT_HEIGHT_MARGIN 1000.0f

#define AREA_PATH_POINT_SAFE_RADIUS 2.0f
#define AREA_PATH_POINT_RESTART_TOLERANCE_SQUARED 0.0001f
#define AREA_PATH_POINT_SEARCH_BOUND_FRACTION 0.25f
#define AREA_PATH_POINT_INITIAL_SEARCH_BOUND 10.0f

#define AREA_SMOOTH_LINEAR_SEARCH_LIMIT 4
#define AREA_SMOOTH_SUBDIVISION_THRESHOLD 1.1

#define AREA_SMOOTH_RESTART_RADIUS_MARGIN 0.001f

#define AREA_AWAY_ANGLE_STEP 0.7853981633974483f
#define AREA_AWAY_MINIMUM_FRACTION 0.005f
#define AREA_AWAY_REVERSE_WEIGHT 256.0f
#define AREA_AWAY_REVERSE_DIAGONAL_WEIGHT 64.0f
#define AREA_AWAY_SIDE_WEIGHT 16.0f
#define AREA_AWAY_FORWARD_DIAGONAL_WEIGHT 4.0f

#define AREA_GRID_DIRECT_LINE_SCALE 1.2f
#define AREA_GRID_MINIMUM_FRACTION 0.005f

class CSWSArea : public CSWArea,
                 public CResHelper<CResARE, RESTYPE_ARE>,
                 public CGameObject
{
public:

    enum { AWAY_FORWARD = 0, AWAY_FORWARD_RIGHT = 1, AWAY_RIGHT = 2,
           AWAY_BACK_RIGHT = 3, AWAY_BACK = 4, AWAY_BACK_LEFT = 5,
           AWAY_LEFT = 6, AWAY_FORWARD_LEFT = 7, AWAY_DIRECTION_COUNT = 8 };

    enum { PATH_POINT_INVALID = 0xFFFFFFFF, PATH_POINT_SEARCH_PASSES = 50 };

    enum { GRID_DIRECT_LINE_BUDGET = 4, GRID_DIRECT_LINE_COST_INTERVAL = 6,
           GRID_DIRECT_LINE_COST_PHASE = 4 };

    enum { PATH_POINT_DFS_MAX_EXPANSIONS = 4096, PATH_POINT_DFS_RENDER_LINES = 500 };

    enum { GFF_STRUCT_DOOR = 8 };

    enum { HEARTBEAT_INTERVAL = 6000, WEATHER_CHECK_INTERVAL = 300000 };

    enum AreaScript
    {
        SCRIPT_ON_HEARTBEAT = 0,
        SCRIPT_ON_USER_DEFINED = 1,
        SCRIPT_ON_ENTER = 2,
        SCRIPT_ON_EXIT = 3,
        SCRIPT_COUNT = 4
    };
    static const int MAX_X_TILES;
    static const int MAX_Y_TILES;
    static const float MAX_OBJECT_VISIBLE_DISTANCE;
    static const float MAX_OBJECT_CONVERSATION_DISTANCE;
    static const float MAX_OBJECT_CHATTALK_DISTANCE;

    static const float MAX_OBJECT_CHATWHISPER_DISTANCE;

    static const float MAX_OBJECT_START_CONVERSATION_DISTANCE;

    static const float MAX_OBJECT_CHATTALKSILENT_DISTANCE;
    static const float MAX_OBJECT_CHATSHOUTSILENT_DISTANCE;

    virtual ~CSWSArea();
    virtual int GetIsIDInExcludeList(unsigned long oidObject,
                                     CExoArrayList<CGameObject*>* pExcludeList);
    virtual CSWSArea* NWAreaAsSWSArea() { return this; }  // PC 0x00698BD0
    virtual int HandleTransparentDoors(
        const Vector& vStart, const Vector& vEnd, float fDistance, float fWidth,
        float fHeight, unsigned long oidObject, unsigned long& oidHitObject, int bFlag,
        CExoArrayList<CGameObject*>* pExcludeList, int bCheckObjects);
    virtual CSWSArea* AsSWSArea() { return this; }  // PC 0x004E88F0

    float ComputePathDistance(int nPoints, float* pfPoints);
    int CountVisibleToPlayers(const Vector& vPosition, int nMaxCount);
    int EvaluateOverlappingTargets(CPathfindInformation* pPathfind,
                                   const Vector& vPosition,
                                   unsigned long oidTarget,
                                   const Vector& vTargetPosition,
                                   float fDistance, int nFlags, int bCheckSize,
                                   unsigned long* poidOverlap);
    int SetHitInfoForCreature(const Vector& vCenter, float fRadius,
                              const Vector& vStart, const Vector& vEnd,
                              CWalkHitInfo* pWalkHitInfo);
    int CountAreaTransitionTriggers(unsigned long* poidTriggers, int nCount,
                                    unsigned long* poidExclude);
    int NoCreaturesOnLine(const Vector& vStart, const Vector& vEnd,
                          CPathfindInformation* pPathfind, int nFlags,
                          int nCount, unsigned long* poidIgnore, int bFlag,
                          CWalkHitInfo* pWalkHitInfo);
    int GridDFSGenerateSuccessors(int nCost, int nRemainingCost,
                                  int nX, int nY, int nZ,
                                  CSWSAreaGridSuccessors** ppSuccessors);
    int ClosestPathPoint(const Vector& vPosition, unsigned long& nPathPoint);
    int PathPointDFSGenerateSuccessors(float fDistance, int nDepth,
                                       unsigned long nPathPoint,
                                       CSWSPathPointSuccessors** ppSuccessors);
    Vector ComputeAwayVector(const Vector& vPosition, const Vector& vAway,
                          float fDistance, CPathfindInformation* pPathfind);
    int ComputeBestCorner(float fX1, float fY1, float fX2, float fY2, float fX3,
                          float fY3, float fX4, float fY4, float* pfX1,
                          float* pfY1, float* pfX2, float* pfY2);
    int TestSafeLocationPoint(const Vector& vPosition,
                              CPathfindInformation* pPathfind);
    int ComputeNonVisibleLocation(const Vector& vPosition,
                                  CPathfindInformation* pPathfind, int nFlags,
                                  Vector* pvLocation, float fDistance);
    int IntersectingLineSegment(const Vector& vStart, const Vector& vEnd,
                                CExoArrayList<unsigned long>* pList);
    int InSubAreas(const Vector& vPosition,
                   CExoArrayList<unsigned long>* pList);
    int SmoothCornerOptimize(int nPoints, float* pfPoints, int* pnResultCount,
                             float** ppfResult);
    void SmoothSelectNodes(int nPoints, float* pfPoints, int* pnResultCount,
                           float** ppfResult, int nFlags, int nPass);
    int SmoothSelectNodesRestartable(int nPoints, float* pfPoints,
                                     int* pnResultCount, float** ppfResult,
                                     int nFlags, int nPass, float fTolerance);
    int GridDFSearch(int nDepth, int nCost, int nRemainingCost, int nX,
                     int nY, int nZ);
    int PathPointDFSearch(int nStart, float fX, float fY, unsigned long nFlags,
                          unsigned long nGoal);
    int ComputeSafeLocationInDirection(const Vector& vPosition,
                                       const Vector& vDirection,
                                       float fDistance,
                                       CPathfindInformation* pPathfind,
                                       int nFlags, Vector* pvLocation);
    int SmoothSelectionRestartable(int nPoints, float* pfPoints,
                                   int* pnResultCount, float** ppfResult);
    void SmoothSelection(int nPoints, float* pfPoints, int* pnResultCount,
                         float** ppfResult);
    int SmoothPointsOnPath();
    int SmoothPointsOnPathRestartable();
    int PlotGridPath(CPathfindInformation* pPathfind, unsigned __int64 nFlags);
    int PlotSoundPath(CPathfindInformation* pPathfind);
    int PlotPathPointPath(CPathfindInformation* pPathfind,
                          unsigned __int64 nFlags);
    int NoNoneWalkPolysInStaticObject(unsigned long oidObject,
                                      const Vector& vStart, const Vector& vEnd,
                                      float fDistance, float fWidth,
                                      float fHeight, unsigned long oidSource,
                                      int bCheckDoors,
                                      CWalkHitInfo* pWalkHitInfo);
    void GridDFSTransTableInitialize();
    int GridDFSTransTableGet(int nX, int nY, int* pnScore, int* pnTieBreak);
    void GridDFSTransTablePut(int nX, int nY, int nScore, int nTieBreak);
    void PathPointDFSTransTableInitialize();
    int PathPointDFSTransTableGet(float fX, float fY, float* pfScore,
                                  float* pfTieBreak);
    void PathPointDFSTransTablePut(float fX, float fY, float fScore,
                                   float fTieBreak);
    int GetRoomIndex(const Vector& vPosition, int nFlags);
    int GetRoomForceRating(const Vector& vPosition);
    unsigned char GetRoomDisableWeather(const Vector& vPosition);
    int TestDirectLine(const Vector& vStart, const Vector& vEnd,
                       const float& fDistance, float fWidth, int nFlags,
                       CWalkHitInfo* pWalkHitInfo);

    enum DirectLineResult
    {
        DIRECT_LINE_NO_ROOM = 0,
        DIRECT_LINE_CLEAR = 1,
        DIRECT_LINE_BLOCKED_ROOM = -1,
        DIRECT_LINE_BLOCKED_OBJECT = -2,
        DIRECT_LINE_BLOCKED_CREATURE = -3
    };
    float ComputeHeight(const Vector& vPosition, int nRoomIndex,
                        CSWSRoom** ppRoom, int* pnRoomIndex = NULL);
    class CSWSRoom* GetRoom(const Vector& vPosition, int* pnFace,
                            int* pnRoomIndex = NULL);
    class CSWSRoom* GetRoom(int nRoomIndex);
    int ComputeSafeLocation(const Vector& vPosition, float fRadius,
                            CPathfindInformation* pPathfind, int bFlag,
                            Vector* pSafePosition);
    static class CSWSRoom* LoadRooms(CLYT* pLayout);
    int LoadCreatures(CResGFF* pRes, CResStruct* pStruct, int bLoadingGame,
                      int bUseTemplates);
    int LoadItems(CResGFF* pRes, CResStruct* pStruct, int nAreaId,
                  int bLoadingGame);
    int LoadEncounters(CResGFF* pRes, CResStruct* pStruct, int nAreaId,
                       int bLoadingGame);
    int LoadWaypoints(CResGFF* pRes, CResStruct* pStruct, int nAreaId,
                      int bLoadingGame);
    int LoadSounds(CResGFF* pRes, CResStruct* pStruct, int nAreaId,
                   int bLoadingGame);
    int LoadStores(CResGFF* pRes, CResStruct* pStruct, int nAreaId,
                   int bLoadingGame);
    int LoadAreaEffects(CResGFF* pRes, CResStruct* pStruct, int nAreaId,
                        int bLoadingGame);
    int LoadMaps(CResGFF* pRes, CResStruct* pStruct);
    int LoadPlaceableCameras(CResGFF* pRes, CResStruct* pStruct);
    int SaveProperties(CResGFF* pRes, CResStruct* pStruct);
    void SaveMaps(CResGFF* pRes, CResStruct* pStruct);

    enum
    {
        GFF_STRUCT_ITEM = 0,

        GFF_STRUCT_TRIGGER = 1,
        GFF_STRUCT_PATH_POINT = 2,
        GFF_STRUCT_PATH_CONNECTION = 3,
        GFF_STRUCT_CREATURE = 4,
        GFF_STRUCT_SOUND = 6,
        GFF_STRUCT_PLACEABLE = 9,
        GFF_STRUCT_PLACEABLE_CAMERA = 14
    };

    static const int GFF_CAMERA_DEFAULT_FIELD_OF_VIEW = 55;
    void SavePlaceableCameras(CResGFF* pRes, CResStruct* pStruct);
    int GetSurfaceMaterial(Vector vPosition);
    int PositionWalkable(Vector vPosition);

    int TestPathPoint(float fPersonalSpace, int bVerbose);
    int GetDoorCrossed(const Vector& vStart, const Vector& vEnd);
    void InitializeRoomAudioParams(int nRoomCount);
    CSWCArea* GetClientArea();
    int AttachmentFromClientArea(CSWCArea* pArea);
    void DetachFromClientArea();
    int SetMaxStealthXP(unsigned long nExperience);
    int SetCurrentStealthXP(unsigned long nExperience);
    unsigned char GetNextTransitionPendingID();
    void SetFogColor(const Vector& vColor);
    void SetTransitionPending(int bPending, unsigned char nId);

    int GetPlayerRestrictMode() { return m_nPlayerRestrictMode; }
    void SetPlayerRestrictMode(int nRestrictMode);
    unsigned long GetDirtyARGB(int nIndex) { return m_nDirtyARGB[nIndex]; }
    int GetDirtySize(int nIndex) { return m_nDirtySize[nIndex]; }
    int GetDirtyFormula(int nIndex) { return m_nDirtyFormula[nIndex]; }
    int GetDirtyFunc(int nIndex) { return m_nDirtyFunc[nIndex]; }
    int GetFirstObjectIndiceByX(int* pnIndex, float fX);
    int UpdatePositionInObjectsArray(CGameObject* pObject);
    int GetFirstObjectInArea(unsigned long& oidObject);
    int GetNextObjectInArea(unsigned long& oidObject);
    void IncreaseAILevelPriority();
    void DecreaseAILevelPriority();
    int LoadRoomInfo(CResStruct* pStruct);
    int LoadProperties(CResGFF* pRes, CResStruct* pStruct);
    void SaveCreatures(CResGFF* pRes, CResStruct* pStruct,
                       CExoArrayList<unsigned long>& lstObjectIds);
    void SaveItems(CResGFF* pRes, CResStruct* pStruct,
                   CExoArrayList<unsigned long>& lstObjectIds);
    void SaveDoors(CResGFF* pRes, CResStruct* pStruct,
                   CExoArrayList<unsigned long>& lstObjectIds);
    void SaveTriggers(CResGFF* pRes, CResStruct* pStruct,
                      CExoArrayList<unsigned long>& lstObjectIds);
    void SaveEncounters(CResGFF* pRes, CResStruct* pStruct,
                        CExoArrayList<unsigned long>& lstObjectIds);
    void SaveWaypoints(CResGFF* pRes, CResStruct* pStruct,
                       CExoArrayList<unsigned long>& lstObjectIds);
    void SaveSounds(CResGFF* pRes, CResStruct* pStruct,
                    CExoArrayList<unsigned long>& lstObjectIds);
    void SaveStores(CResGFF* pRes, CResStruct* pStruct,
                    CExoArrayList<unsigned long>& lstObjectIds);
    void SavePlaceables(CResGFF* pRes, CResStruct* pStruct,
                        CExoArrayList<unsigned long>& lstObjectIds);
    void SaveAreaEffects(CResGFF* pRes, CResStruct* pStruct,
                         CExoArrayList<unsigned long>& lstObjectIds);
    void PlayVisualEffect(unsigned short nEffect, const Vector& vPosition);
    void SetCurrentWeather(unsigned char nWeather, int bEnabled);
    void BudgeCreatures(const Vector& vCentre, const Vector& vMin,
                        const Vector& vMax, unsigned long oidObject,
                        int bUseActionPoint);
    int LoadPathPoints(CResStruct* pStruct);
    int NoNonWalkPolys(const Vector& vStart, const Vector& vEnd,
                       float fDistance, float fWidth, float fHeight,
                       unsigned long oidObject, CWalkHitInfo* pWalkHitInfo,
                       int bCheckDoors);
    int NoNonWalkPolysInDoors(const Vector& vStart, const Vector& vEnd,
                               float fMinHeight, float fMaxHeight, float fRadius,
                               unsigned long oidObject, unsigned long& oidHitObject,
                               CExoArrayList<CGameObject*>* pExcludeList,
                               int bFlag, int bCheckTransparentDoors);
    int NoNonWalkPolysDetailed(const Vector& vStart, const Vector& vEnd,
                               float fDistance, float fWidth, float fHeight,
                               unsigned long oidObject, unsigned long& oidHitObject,
                               int bFlag, CExoArrayList<CGameObject*>* pObjects,
                               int bCheckObjects, CWalkHitInfo* pWalkHitInfo,
                               int bCheckDoors);
    void ClearObjects();
    void AwardStealthXP(unsigned long oidObject);
    int EnforceNoPCHeadCloning(int nHead);
    unsigned char GetNumMinesSetByPlayerParty();
    int GetPlayerCanSetMines();
    int AIUpdate();
    void IncrementPlayersInArea();
    void DecrementPlayersInArea();

    void SetTag(const CExoString& sTag) { m_sTag = sTag.LowerCase(); }
    int LoadAreaHeader(CResStruct* pStruct);
    int LoadDoors(CResGFF* pRes, CResStruct* pStruct, int nAreaId,
                  int bLoadingGame);
    int LoadTriggers(CResGFF* pRes, CResStruct* pStruct, int bLoadingGame,
                     int bUseTemplates);
    int LoadPlaceables(CResGFF* pRes, CResStruct* pStruct, int bLoadingGame,
                       int bUseTemplates);
    int PackAreaIntoMessage(int nPlayerId, int nAreaId, int nFlags,
                            CSWSPlayer* pPlayer);
    void UnloadArea();
    int ClearLineOfSight(const Vector& vStart, const Vector& vEnd,
                         Vector* pvHit, unsigned long* poidHit,
                         unsigned long oidObject, unsigned long oidTarget,
                         int bFlag1, int bFlag2);
    void ApplyEffect(CGameEffect* pEffect, const Vector& vPosition,
                    const Vector& vOrientation);
    CSWSArea(const CResRef& cResRef, int nAreaId, unsigned long oidArea);
    void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                      void* pEventData, unsigned long nCalendarDay,
                      unsigned long nTimeOfDay);

    enum { TRANSITION_FADE_DELAY = 500 };
    int RemoveObjectFromArea(unsigned long oidObject);
    int LoadGIT(int bLoadingGame);
    void SaveGIT(CERFFile* pERFFile, const CExoString& sName,
                CExoArrayList<unsigned long>& lstPlayers);
    int AddObjectToArea(unsigned long oidObject, int bLoadingGame);

    void AddSubArea(CSWSObject* pObject)
    {
        m_lstTriggerIds.Add(pObject->GetId());
    }

    void RemoveSubArea(CGameObject* pObject)
    {
        m_lstTriggerIds.Remove(pObject->GetId());
    }
    int LoadArea(int bLoadingGame);
    int ShrinkLineByCreatures(const Vector& vStart, const Vector& vEnd,
                              Vector& vResult);

    unsigned short GetLoadScreenID() { return m_nLoadScreenID; }

    int m_nPlayersInArea;
    CExoArrayList<unsigned long> m_lstTrapObjectIds;

    int m_nObjectByNameIndex;
    unsigned long m_nLastHeartbeatDay;
    unsigned long m_nLastHeartbeatTime;
    CExoLinkedList<NWAreaExpansion_st> m_lstExpansions;
    CExoString m_sComments;
    CExoLocString m_lsName;
    CExoString m_sTag;
    CSWSAreaVersionInfo* m_pVersionInfo;
    CExoString m_sScripts[SCRIPT_COUNT];
    int m_nChanceFog;
    int m_nModSpotCheck;
    int m_nModListenCheck;
    CExoArrayList<unsigned long> m_lstObjectIds;
    int m_nObjectIterator;
    CExoArrayList<unsigned long> m_lstTriggerIds;
    CPathfindInformation* m_pPathfindInformation;
    CPathfindInformation* m_pAreaPathfindInformation;
    CSWSAreaGridTransEntry* m_pGridTransTable;
    char m_aUnmapped1B8[0x14];
    int m_bGridSearchAborted;
    int m_nGridSearchNodes;
    unsigned long m_oidEnteringObject;
    unsigned long m_oidExitingObject;

    int m_nCustomScriptEventId;
    int m_bTransitionCounted;
    CSWSScriptVarTable m_lScriptVars;
    CSWVarTable m_lVarTable;
    CSWSAmbientSound* m_pAmbientSound;

    unsigned char m_nCurrentWeather;
    int m_bWeatherStarted;

    unsigned long m_nWeatherCheckTimer;

    unsigned long m_nLastAIUpdateDay;
    unsigned long m_nLastAIUpdateTime;

    unsigned char m_nWeatherOverride;

    unsigned char m_nActiveWeather;
    unsigned char m_nLightingScheme;

    unsigned char m_nGridMaskBytes;
    int m_bHeaderLoaded;
    unsigned short m_nLoadScreenID;
    int m_nRoomCount;
    CSWSRoom* m_pRooms;
    int m_bMiniGame;
    int m_nPathPointCount;
    CSWSAreaPathPoint* m_pPathPoints;
    int m_nPathConnectionCount;
    int* m_pPathConnections;

    int m_bPathPointDFSAborted;
    char m_aUnmapped24C[0x4];
    CSWSAreaPathPointTransEntry* m_pPathPointTransTable;
    int m_nPathPointDFSExpansions;
    int m_nPathPointPathCapacity;
    CExoString* m_psRoomAudio;
    int* m_pnRoomAudio;
    float* m_pfRoomAudio;
    int m_nRoomAudioCount;
    int m_nRoomAudioEntryCount;
    int* m_pnRoomForceRating;
    unsigned char* m_pbRoomDisableWeather;
    unsigned char* m_pnRoomAudioIndex;
    CExoArrayList<CExoString> m_lstRoomAudioNames;
    CExoArrayList<int> m_lstRoomAudioLooping;
    CExoArrayList<CExoString> m_lstRoomAudioModelParts;
    CExoArrayList<CExoString> m_lstRoomAudioOmenEvents;
    CExoArrayList<CResRef> m_lstRoomAudioSounds;
    int m_bUnescapable;
    int m_bDisableTransit;
    int m_nPlayerRestrictMode;
    unsigned long m_nStealthXPMax;
    unsigned long m_nStealthXPCurrent;
    unsigned long m_nStealthXPLoss;
    int m_bStealthXPEnabled;
    int m_bTransitionPending;
    char m_nNextTransitionPendingId;
    unsigned char m_nCurrentTransitionPendingId;
    char m_aUnmapped2CA[0x2];
    unsigned long m_nDirtyARGB[3];
    int m_nDirtySize[3];
    int m_nDirtyFormula[3];
    int m_nDirtyFunc[3];

    CPathfindInformation* m_pActivePathfindInformation;
    CSWCArea* m_pClientArea;

public:

    CExoString GetTag()
    {
        return m_sTag;
    }
    void SetObjectByNameIndex(int nObjectByNameIndex)
    {
        m_nObjectByNameIndex = nObjectByNameIndex;
    }
    int GetObjectByNameIndex()
    {
        return m_nObjectByNameIndex;
    }
    int GetCustomScriptEventId()
    {
        return m_nCustomScriptEventId;
    }
    void SetCustomScriptEventId(int nCustomScriptEventId)
    {
        m_nCustomScriptEventId = nCustomScriptEventId;
    }
    CSWSAmbientSound* GetAmbientSound()
    {
        return m_pAmbientSound;
    }
    unsigned char GetCurrentWeather()
    {
        return m_nCurrentWeather;
    }
};

#endif
