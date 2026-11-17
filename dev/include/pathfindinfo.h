#ifndef __PATHFINDINFO_H__
#define __PATHFINDINFO_H__

class Vector;
#include "math3d.h"
#include "gameobjectarray.h"

class CSWSCreature;

class CPathfindInfoIntraTileSuccessors
{
public:
    CPathfindInfoIntraTileSuccessors();

    float m_fStartX;
    float m_fStartY;
    float m_fEndX;
    float m_fEndY;
    float m_fDistance;
    int m_nTriangle;
};

class CPathfindInformation
{
public:

    enum { GRID_SEARCH_DEPTH_ALLOWANCE = 36 };
    CPathfindInformation();
    ~CPathfindInformation();

    void Initialize();
    void Shutdown();
    void HandlePathfindError(int nError, char* szMessage);

    static const int PATH_POINT_STARTPOINTS;
    static const int PATH_POINT_GRIDSTART;
    static const int PATH_POINT_GRIDEND;
    static const int PATH_POINT_PATHFIND;
    static const int PATH_POINT_SMOOTH;
    static const int SMOOTHING_START;
    static const int SMOOTHING_STEP_1;
    static const int SMOOTHING_STEP_2;
    static const int SMOOTHING_STEP_3;
    static const int SMOOTHING_STEP_4;

    void DeleteFirstTileFValueAlternatives();
    void CreateFirstTileFValueAlternatives(int nCount);
    void GetFirstTileFValueAlternatives(int nTile, int* pnFValue, int* pnFValue2, CPathfindInfoIntraTileSuccessors** ppSuccessors);
    void SetFirstTileFValueAlternatives(int nTile, int nFValue, int nFValue2, CPathfindInfoIntraTileSuccessors* pSuccessors);
    void FlipStartEndPoints();
    void FlipTempEndPoints();
    void ComputeStepTolerance();

    void SetStartX(float fX) { m_fStartX = fX; }
    void SetStartY(float fY) { m_fStartY = fY; }
    void SetStartZ(float fZ) { m_fStartZ = fZ; }
    void SetEndX(float fX) { m_fEndX = fX; }
    void SetEndY(float fY) { m_fEndY = fY; }
    void SetEndZ(float fZ) { m_fEndZ = fZ; }
    void SetPathPointPath(unsigned long* pPath)  // PC 0x004FBEC0 GOG 0x004F3FA0
    {
        if (m_pPathPointDFSPath != 0)
        {
            delete m_pPathPointDFSPath;
            m_pPathPointDFSPath = 0;
        }
        m_pPathPointDFSPath = pPath;
    }
    void GetPathPointEndpoints(Vector& vStart, Vector& vEnd)  // PC 0x004FBF00 GOG 0x004F3FE0
    {
        vStart = m_vPathPointEndpointStart;
        vEnd = m_vPathPointEndpointEnd;
    }
    void SetPathPointEndpoints(const Vector& vStart, const Vector& vEnd)  // PC 0x004FBF40 GOG 0x004F4020
    {
        m_vPathPointEndpointStart = vStart;
        m_vPathPointEndpointEnd = vEnd;
    }
    void SetPathPointStartPath(int nCount, float* pfPoints)  // PC 0x004FBF80 GOG 0x004F4060
    {
        m_nPathPointStartCount = nCount;
        if (m_pfPathPointStartPoints != 0)
            delete m_pfPathPointStartPoints;
        m_pfPathPointStartPoints = pfPoints;
    }
    void SetPathPointEndPath(int nCount, float* pfPoints)  // PC 0x004FBFC0 GOG 0x004F40A0
    {
        m_nPathPointEndCount = nCount;
        if (m_pfPathPointEndPoints != 0)
            delete m_pfPathPointEndPoints;
        m_pfPathPointEndPoints = pfPoints;
    }
    // PC 0x004FC000 GOG 0x004F40E0
    void SetSmoothFirstPoints(float* pfPoints)
    {
        if (m_pfSmoothFirstPoints == 0 && pfPoints != 0)
        {
            m_pfSmoothFirstPoints = pfPoints;
        }
        else
        {
            if (m_pfSmoothFirstPoints != 0 && m_pfSmoothFirstPoints != pfPoints)
            {
                m_nSmoothFirstCount = 0;
                delete[] m_pfSmoothFirstPoints;
                m_pfSmoothFirstPoints = 0;
            }
            m_pfSmoothFirstPoints = pfPoints;
        }
    }
    void SetSmoothSecondPoints(float* pfPoints)  // PC 0x004FC050 GOG 0x004F4130
    {
        if (m_pfSmoothSecondPoints == 0 && pfPoints != 0)
        {
            m_pfSmoothSecondPoints = pfPoints;
        }
        else
        {
            if (m_pfSmoothSecondPoints != 0 && m_pfSmoothSecondPoints != pfPoints)
            {
                m_nSmoothSecondCount = 0;
                delete[] m_pfSmoothSecondPoints;
                m_pfSmoothSecondPoints = 0;
            }
            m_pfSmoothSecondPoints = pfPoints;
        }
    }
    void SetSmoothFinalPoints(float* pfPoints)  // PC 0x004FC0A0 GOG 0x004F4180
    {
        if (m_pfSmoothFinalPoints == 0 && pfPoints != 0)
        {
            m_pfSmoothFinalPoints = pfPoints;
        }
        else
        {
            if (m_pfSmoothFinalPoints != 0 && m_pfSmoothFinalPoints != pfPoints)
            {
                m_nSmoothFinalCount = 0;
                delete[] m_pfSmoothFinalPoints;
                m_pfSmoothFinalPoints = 0;
            }
            m_pfSmoothFinalPoints = pfPoints;
        }
    }

    float GetStepScale() { return m_fStepScale; }
    float GetHalfStep() { return m_fHalfStep; }

    float GetCreaturePersonalSpace() { return m_fStepScaleY; }

    int GetAreaListCount() { return m_nAreaListCount; }
    unsigned long* GetAreaList() { return m_pAreaList; }

    int GetWayPointCount() { return m_nWayPointCount; }
    int GetCurrentWayPoint() { return m_nCurrentWayPoint; }

    void ResetGridSearchData();
    void ResetWayPointData();
    void ResetPathPointSearchData();
    void ResetIntraTileSearchData();
    void ResetInterTileSearchData();
    void ResetInterAreaPathSearchData();
    void ResetInterAreaSearchData();
    void ResetAtEndOfPath();
    void SetBlockingCreature(unsigned long oidCreature)  // PC 0x004F6030 GOG 0x00505820
    {
        m_oidBlockingCreature = oidCreature;
        if (oidCreature == OBJECT_INVALID)
        {
            m_vBlockingCreaturePosition = Vector();
        }
    }

    int m_nPathFlags;
    float m_fStepScale;
    float m_fStepScaleY;

    float m_fTargetUseRange;
    float m_fStepScaleZ;
    float m_fHalfStep;
    float m_fHitDistance;
    int m_bPathPointGridLeg;
    int m_nField_20;
    int m_bReturnClosest;
    int m_nMovementRate;
    unsigned long m_oidPathfindCreature;
    unsigned long m_oidPathfindTargetCreature;
    int m_oidIgnoreCreature;
    unsigned long m_oidUseTarget;
    int m_nPlotAttempts;
    int m_nField_40;
    float m_fTempStartX;
    float m_fTempStartY;
    float m_fTempStartZ;
    float m_fTempDistance;
    int m_nTempArea;
    int m_bNewPathRequest;
    float m_fStartX;
    float m_fStartY;
    float m_fStartZ;
    float m_fStartDistance;
    int m_nStepTolerance;
    int m_nStartArea;
    float m_fEndX;
    float m_fEndY;
    float m_fEndZ;
    int m_nEndArea;
    int m_bPathReversed;
    int m_bPlotFinished;
    int m_nWayPointCount;
    float* m_pWayPoints;
    int m_nWayPointTempCount;
    float* m_pWayPointsTemp;
    int m_nCurrentWayPoint;
    int m_bAreaPathFound;
    int m_bAreaLegStarted;
    int m_nAreaPathLength;
    int* m_pAreaPath;
    int* m_pAreaPathCost;
    int m_nAreaListCount;
    unsigned long* m_pAreaList;

    int m_oidIgnoreDoor;
    int m_nField_C0;
    int* m_pTileOpen;
    int* m_pTileClosed;
    int m_nField_CC;
    int* m_pTileParent;
    int* m_pTileCost;
    int m_nField_D8;
    int m_nField_DC;
    int m_nField_E0;
    int m_nFirstTileCount;
    int* m_pFirstTileFValue;
    int* m_pFirstTileFValue2;
    CPathfindInfoIntraTileSuccessors** m_ppFirstTileFValueAlternatives;
    int* m_pField_F4;
    int m_bWayPointCut;
    int m_nWayPointCutState;
    char m_nWayPointCutCount;
    int m_nWayPointCutRemaining;
    Vector m_vWayPointOrigin;
    Vector m_vWayPointDirection;
    int m_nField_120;
    int m_nGridDistance;
    int m_nField_128;
    Vector m_vGridOrigin;
    Vector m_vGridSide;
    Vector m_vGridStep;
    Vector m_vGridUp;

    int* m_pGridDFSPath;
    int m_nGridDFSDepth;
    float m_fField_164;
    int m_nGridTransTableDepth;
    int* m_pGridTransTable;
    Vector m_vGridStart;
    Vector m_vGridEnd;

    int m_nGridDFSBestPathLength;
    int* m_pGridDFSBestPath;
    int m_nGridDFSBestDistance;
    int m_bForceGridPath;
    int m_bPartialPathSearch;

    int m_nDirectLineResult;
    char m_bField_1A0;
    int m_nStartPathPoint;
    int m_nEndPathPoint;
    unsigned long* m_pPathPointDFSPath;
    int m_nPathPointDFSDepth;
    void* m_pPathPointTransTable;
    Vector m_vPathPointStart;
    Vector m_vPathPointEnd;
    float m_fPathPointSearchBound;
    float m_fPathPointDFSBestDistance;
    unsigned long* m_pPathPointDFSBestPath;
    int m_nPathPointDFSBestPathLength;
    int m_nPathPointState;
    Vector m_vPathPointEndpointStart;
    Vector m_vPathPointEndpointEnd;
    int m_nPathPointStartCount;
    float* m_pfPathPointStartPoints;
    int m_nPathPointEndCount;
    float* m_pfPathPointEndPoints;
    float m_fPathPointDFSBestScore;
    int m_bIgnoreCreatures;
    int m_nSmoothingStep;
    int m_nSmoothFirstCount;
    float* m_pfSmoothFirstPoints;
    int m_nSmoothSecondCount;
    int m_nSmoothFinalCount;
    float* m_pfSmoothSecondPoints;
    float* m_pfSmoothFinalPoints;
    int m_nSmoothCurrentNode;
    int m_nSmoothIntermediateCount;
    float* m_pfSmoothIntermediatePoints;
    int m_bForceLocationUpdate;
    int m_bStopAtUseRange;
    int m_nAvoidSide;
    // PC 0x00541EB0 GOG 0x00541FD0
    void SetTargetPosition(const Vector& vPosition) { m_vTargetPosition = vPosition; }
    Vector m_vTargetPosition;
    unsigned long m_oidBlockingCreature;
    Vector m_vBlockingCreaturePosition;
    int m_oidFoundEnemyCreature;

    int m_nBumpCount;
    int m_bTimeout;
    int m_nTimeoutDay;
    int m_nTimeoutTime;
};

#define AVOID_CREATURE_POINT_TOLERANCE_SQUARED 0.001f

class CAvoidCreature
{
public:

    enum
    {
        PATH_BLOCKED = 0,
        PATH_BLOCKED_BY_CREATURE = 1,
        PATH_CLEAR = 2
    };
    CAvoidCreature();
    void Reset()  // PC 0x00541ED0 GOG 0x00541FF0
    {
        m_vHexPoints[0] = Vector();
        m_vHexPoints[1] = Vector();
        m_vHexPoints[2] = Vector();
        m_vHexPoints[3] = Vector();
        m_vHexPoints[4] = Vector();
        m_vHexPoints[5] = Vector();
        m_nEntrySegment = -1;
        m_nEntrySide = -1;
        m_vEntryPoint = Vector();
        m_nExitSegment = -1;
        m_nExitSide = -1;
        m_vExitPoint = Vector();
    }

    int GetEntrySegment() { return m_nEntrySegment; }
    int GetExitSegment() { return m_nExitSegment; }
    int InsertPathSegmentBetween(int nFirstSegment, int nLastSegment,
                                 Vector* pNewPoints, int nNewPoints,
                                 CPathfindInformation* pInfo);

    static const float LINEFACTOR;
    static const int AVOIDCREATURE_SUCSESS;
    static const int AVOIDCREATURE_FAILED;
    static const int AVOIDCREATURE_STARTINHEX;
    static const int AVOIDCREATURE_ENDINHEX;

    void ComputeHexPoints(const Vector& vCenter, const Vector& vDirection,
                         float fRadius);
    void Render();
    int SideClearOfObstructions(unsigned long oidCreature, const Vector& vFrom,
                                const Vector& vTo, unsigned long& oidBlocker);
    int IsPathClear(unsigned long oidCreature, Vector* pPoints, int nPoints,
                    unsigned long& oidBlocker);
    int GetClosestPointTo(Vector& vPoint);
    int IsPointInHex(const Vector& vPoint);
    int FindEntryAndExit(CPathfindInformation* pInfo);
    Vector* FindPath_Left(int& nPoints);
    Vector* FindPath_Right(int& nPoints);
    Vector* FindPath(int& nPoints);
    int PlotPathAroundCreature(CSWSCreature* pCreature,
                               CSWSCreature* pAvoidCreature);

    int m_bAvoidLeft;
    Vector m_vHexPoints[6];
    int m_nEntrySegment;
    int m_nEntrySide;
    Vector m_vEntryPoint;
    int m_nExitSegment;
    int m_nExitSide;
    Vector m_vExitPoint;
};

#endif
