#ifndef __SWPARTY_H__
#define __SWPARTY_H__

#include "math3d.h"
#include "gameobjectarray.h"
class CSWCCreature;
class CSWParty;

#define SWPARTY_TOGETHER_DISTANCE_SQUARED 900.0f

extern int FollowMode;

class CSWPartyMemberData
{
public:

    enum { COMBAT_MESSAGE_APPROACH_TARGET = 42477 };

    enum FollowState
    {
        FOLLOW_UNINITIALIZED = -1,
        FOLLOW_FIND_PATH = 0,
        FOLLOW_OFFSET = 1,
        MOVING_TO_PATH = 2,
        FOLLOW_PATH = 3,
        MOVING_TO_OFFSET = 4,
        FOLLOW_WAITING_FOR_PATH = 5,
        FOLLOW_OFFSET_WAIT_DEST = 6,
        FOLLOW_PATH_WAIT_DEST = 7,
        FOLLOW_TO_OFFSET_WAIT_DEST = 8,
        FOLLOW_TO_PATH_WAIT_DEST = 9,
        FOLLOW_TELEPORT_FORWARD = 10
    };

    CSWPartyMemberData()
    {
        Reset();
        m_nFollowPathIndex = 0;
        m_oidCreature = OBJECT_INVALID;
        m_fFollowDistance = 1.0f;
        m_bWaitingForLeader = 0;
    }

    unsigned long m_oidCreature;
    int m_nCharacterIndex;

    float m_fFollowDistance;

    Vector m_vPosition;
    Vector m_vFollowPosition;
    int m_nFollowPathIndex;
    Vector m_vFormationPosition;
    int m_nFormationPathIndex;
    Vector m_vFormationOffset;

    int m_nStepCount;

    float m_fSpeed;

    Vector m_vFacing;

    float m_fOffsetScale;
    Vector m_vLastPosition;

    int m_bUpdateLastPosition;

    unsigned long m_nFollowPathElapsedTime;

    int m_bWaitingForLeader;

    Vector m_vLeaderPosition;
    unsigned long m_oidCombatTarget;
    unsigned long m_nCombatMessage;
    // PC 0x0050B7D0
    void Reset()
    {
        m_nCharacterIndex = -1;
        m_nFormationPathIndex = 0;
        m_nStepCount = 0;
        m_fSpeed = 0.0f;
        m_bUpdateLastPosition = 0;
        m_vPosition = Vector(0.0f, 0.0f, 0.0f);
        m_vFollowPosition = Vector(0.0f, 0.0f, 0.0f);
        m_vFormationPosition = Vector(0.0f, 0.0f, 0.0f);
        m_vFacing = Vector(0.0f, 1.0f, 0.0f);
        m_fOffsetScale = 1.0f;
        m_oidCombatTarget = OBJECT_INVALID;

        m_nCombatMessage = 111303;
    }
    void SetCombatMessage(unsigned long nMessage);

    enum { COMBAT_MESSAGE_ENGAGED = 111303,
           COMBAT_MESSAGE_CLOSING_TO_ATTACK = 0xA5ED,
           COMBAT_MESSAGE_MOVING_CANCELS_COMBAT = 42476 };
};

typedef Vector CSWPartyFormation[3];

class CSWPartyFollowData
{
public:
    CSWPartyFollowData()
    {
        m_fFacing = 0.0f;
    }

    void Reset(const Vector& vPosition, float fFacing)
    {
        m_vPosition = vPosition;
        m_fFacing = fFacing;
    }

    Vector m_vPosition;
    float m_fFacing;

    Vector m_vFirstMemberPosition;
    Vector m_vSecondMemberPosition;
};

class CSWPartyFollowDataArray
{
public:
    enum { MAX_PATH_POINTS = 100 };
    CSWPartyFollowData m_aPathData[MAX_PATH_POINTS];
    CSWParty* m_pParty;
    int m_nCurrentPathData;
    int m_nPathDataCount;

    int Reset(const Vector& vPosition, float fFacing);
    CSWPartyFollowData* GetPathData(int nIndex);
    int FindClosestPartyMemeber_Current();
    int FindFurthestPartyMemeber_Current();
    float PointOnLineDistanceFromLeader(int nMember, int bSquared);
    float AdvancePointOnPath(float fDistance, int nPathIndex,
                           const Vector& vStart, int nStartIndex,
                           const Vector& vEnd, int nEndIndex, Vector& vPosition,
                           int& nPositionIndex);
    int GetPointBackFromStart(float fDistance, Vector& vPosition,
                              int& nPositionIndex, int nPathIndex);
    CSWPartyFollowDataArray();

    void SmoothFollowPath();
    void ProcessPoint(const Vector& vPosition, float fFacing, float fTime);
};

class CSWParty
{
public:

    enum { LEADER_NEXT = -2 };

    enum { LEADER_PLAYER = -3 };

    enum { FORMATION_NEXT = -9999 };

    enum { MAX_MEMBERS = 3 };

    CSWParty();
    ~CSWParty();
    int IsPartyMember(unsigned long oidCreature);
    CSWCCreature* RemoveCharacter(int nIndex);
    int RemoveCharacter(unsigned long oidCreature);
    CSWCCreature* GetCharacter(int nIndex);
    CSWCCreature* GetPlayerCharacter();
    int GetIndex(unsigned long oidCreature);
    void InvertFormation();
    void SetFormation(int nFormation);
    float GetFollowRange();
    int PartyHasRace(unsigned short nRace);
    int IsPartyTogether();
    void SetLeader(int nIndex);
    void RecaulateFollowPoint();
    Vector GetWorldFormationPosition(const Vector& vPosition,
                                     const Vector& vFacing, int nMember);
    void FollowStateChanged();
    int AddCharacter(unsigned long oidCreature);
    int TestAndResetPath();
    void Update(float fTime);
    void Reset(const Vector& vPosition, float fFacing, int bFlag);
    void ResetAllFollowData(const Vector& vPosition, float fFacing, int bFlag);

    void ResetFollowData(int nMember, const Vector& vPosition)
    {
        m_aPartyMemberData[nMember].m_nCharacterIndex = CSWPartyMemberData::FOLLOW_UNINITIALIZED;
        m_aPartyMemberData[nMember].m_vFollowPosition = vPosition;
        m_aPartyMemberData[nMember].m_nFollowPathIndex = 0;
        m_aPartyMemberData[nMember].m_vFormationPosition = vPosition;
        m_aPartyMemberData[nMember].m_nFormationPathIndex = 0;
    }
    void ClearParty();

    int GetNumCharacters() { return m_nCharacters; }

    CSWPartyMemberData* GetMemberData(int nIndex)
    {
        EXOXBOXASSERT(nIndex >= 0 && nIndex < 3);
#ifndef _XBOX
        if (nIndex < 0)
            return NULL;
#endif
        return &m_aPartyMemberData[nIndex];
    }

    int m_nCharacters;

    int m_nPartyState;

    int m_bApplyLeaderRanges;
    Vector m_vPartyPosition;

    int m_bPartyChanged;
    int m_nPartyFlags;
    float m_fUpdateInterval;
    CSWPartyMemberData m_aPartyMemberData[MAX_MEMBERS];
    int m_nFormation;
    float m_fFormationAngle;
    char m_bFormationInverted;
    CSWPartyFormation m_aRightFormation[3];
    float m_fFormationRange[3];
    CSWPartyFormation m_aLeftFormation[3];
    char m_bLeftFormationAvailable[3];

    Vector m_vLastLeaderPosition;

    float m_fRetreatTimer;
    Vector m_vLeaderFollowPosition;
    Quaternion m_qLeaderFacing;
    CSWPartyFollowDataArray m_cFollowPaths;

    unsigned char m_aUnmapped1288[8];
};

#endif
