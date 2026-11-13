#ifndef __SWMINIGAME_H__
#define __SWMINIGAME_H__

#include "resare.h"
#include "exoresman.h"
#include "exobase.h"
#include "math3d.h"
#include "swplayercontrol.h"
#include <aurpointer.h>

#include "exoresman.h"
#include "aurmodel.h"
#include "aurbehavior.h"
#include <aurpointer.h>

class C2DA;

class CSWMGTargettingParameters
{
public:
    CSWMGTargettingParameters();

    float m_fHorizontalSpread;
    float m_fSensingRadius;
    float m_fVerticalSpread;
    float m_fInaccuracy;
};

class CSWMGBullet
{
public:
    CSWMGBullet();

    int m_nDamage;
    float m_fSpeed;
    float m_fRateOfFire;
    float m_fLifeSpan;
    int m_nTargetType;
    float m_fReserved14;
    CResRef m_resCollisionSound;
};

class CAurPart;
class CSWTrackFollower;

enum
{
    AUR_BEHAVIOR_TRACKANDFIRE = 43690,
    AUR_BEHAVIOR_BULLET = 43691,
    AUR_BEHAVIOR_TRACK = 43692,
    AUR_BEHAVIOR_FIRE = 43693
};

class CSWMGBehaviorFire : public CAurBehaviorAttach
{
public:
    CSWMGBehaviorFire(CAurObject* pObject, CAurObject* pParent,
                      CAurPart* pPart);
    virtual int Fire();
    virtual int Control(float fTimeStep);
    virtual int GetType()  // PC 0x00748D30 GOG 0x00748BD0
    {
        return AUR_BEHAVIOR_FIRE;
    }
    void SetRateOfFire(float fRateOfFire);

    float GetRateOfFire() { return m_fRateOfFire; }

    void SetFireSound(const CResRef& resSound) { m_resFireSound = resSound; }

    CResRef m_resFireSound;
    float m_fRateOfFire;
    float m_fTimeToFire;
};

class CSWMGBehaviorBullet : public CAurBehavior
{
public:
    CSWMGBehaviorBullet(CAurObject* pObject, CSWTrackFollower* pFollower,
                        const CSWMGBullet& bullet);
    virtual int Control(float fTimeStep);
    virtual int GetType()  // PC 0x00748F50 GOG 0x00748DF0
    {
        return AUR_BEHAVIOR_BULLET;
    }

    CSWMGBullet m_bullet;
    CSWTrackFollower* m_pFollower;
};

class CSWMGBehaviorTrack : public CAurBehaviorAttach
{
public:
    CSWMGBehaviorTrack(CAurObject* pObject, CAurObject* pParent,
                       CAurPart* pPart, CAurObject* pModel);
    virtual int Control(float fTimeStep);
    virtual int GetType()  // PC 0x00749890 GOG 0x007496E0
    {
        return AUR_BEHAVIOR_TRACK;
    }

    float GetSpeed() { return m_fSpeed; }

    void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }

    CSWMGTargettingParameters m_params;
    bool m_bTargetting;
    char m_abReserved4D[3];
    float m_fSpeed;
    SafePointer<CAurObject> m_pModel;
    Vector m_vTarget;
};

class CSWMGBehaviorTrackAndFire : public CSWMGBehaviorTrack
{
public:
    CSWMGBehaviorTrackAndFire(CAurObject* pObject, CAurObject* pParent,
                              CAurPart* pPart, CAurObject* pModel);
    void SetRateOfFire(float fRateOfFire);

    float GetRateOfFire() { return m_fRateOfFire; }
    virtual int Control(float fTimeStep);
    virtual int GetType()  // PC 0x00749910 GOG 0x00749780
    {
        return AUR_BEHAVIOR_TRACKANDFIRE;
    }

    void SetFireSound(const CResRef& resSound) { m_resFireSound = resSound; }

    CResRef m_resFireSound;
    float m_fRateOfFire;
    float m_fTimeToFire;
};

class CAurCamera;
class CAurObject;
class CExoStreamingSoundSource;
class CSWCArea;
class CSWCRoom;
class CSWMGObstacle;
class CSWMiniEnemy;
class CSWMiniGameObject;
class CSWMiniPlayer;
class Quaternion;

class SWMGExplodeBullet
{
public:
    SWMGExplodeBullet();

    SafePointer<CAurObject> m_pObject;
    Vector m_vPosition;
    Quaternion m_qOrientation;
};

class SWMGTrackInfo
{
public:
    CResRef m_cResRef;
    Vector m_vPosition;
};
// PC 0x006C5360 GOG 0x0073EC80
inline CSWMGPhysicsState operator+(const CSWMGPhysicsState& stateA,
                                   const CSWMGPhysicsState& stateB)
{
    return CSWMGPhysicsState(stateA.m_vValue + stateB.m_vValue,
                             stateA.m_vRate + stateB.m_vRate);
}
// PC 0x006C53F0 GOG 0x0073ED10
inline CSWMGPhysicsState operator*(const CSWMGPhysicsState& state, float fScale)
{
    return CSWMGPhysicsState(fScale * state.m_vValue,
                             fScale * state.m_vRate);
}
// PC 0x006C5480 GOG 0x006C5190
inline CSWMGPhysicsState operator/(const CSWMGPhysicsState& state, float fScale)
{
    if (fScale != 0.0f)
    {
        return CSWMGPhysicsState(state.m_vValue / fScale,
                                 state.m_vRate / fScale);
    }
    return CSWMGPhysicsState();
}
// PC 0x006C5580 GOG 0x006C5290
inline CSWMGPhysicsState GetDerivative(const Vector& vVelocity,
                                       const Vector& vAcceleration)
{
    return CSWMGPhysicsState(vAcceleration, vVelocity);
}

class CSWMiniGame : public CResHelper<CResARE, RESTYPE_ARE>
{
public:

    enum PhysicsMode
    {
        PHYSICS_VELOCITY = 0,
        PHYSICS_ACCELERATION = 1
    };

    enum BumpAxis
    {
        BUMP_NONE = 0,
        BUMP_X = 1,
        BUMP_Y = 2,
        BUMP_Z = 3
    };

    static const int DOF_VALID_MASK = 7;

    enum AttachMode
    {
        ATTACH_TRACK = 0,
        ATTACH_PLAYER_MODEL = 1,
        ATTACH_CAMERA_HOOK = 2
    };

    CSWMiniGame(CSWCArea* pArea);
    virtual ~CSWMiniGame();

    CSWMiniPlayer* GetPlayer()
    {
        return m_pPlayer;
    }

    void AxisVelocity(const Vector& vAxis, float fScale);
    Vector GetAcceleration(CSWMGPhysicsState& state, const Vector& vTarget);
    void GetClipDist(float* pfNear, float* pfFar);
    void SetClipDist(float fNear, float fFar);
    Vector BumpPlane(const Quaternion& qOrientation);
    int GetAttachType();
    void AxisAcceleration(const Vector& vAxis, float fScale);
    void UpdateAxis(Vector& vAxis, float fScale);
    void UpdateMouse(float fDeltaX, float fDeltaY, float fTimeStep);
#ifdef _XBOX
    void UpdateAxis(float fX, float fZ, float fTimeStep);
#endif
    void Control(float fTimeStep);
    void Go();
    int GetEnemy(int nIndex);
    CSWMiniEnemy* RemoveEnemy(CSWMiniEnemy* pEnemy);
    SWMGTrackInfo* GetProtoTrack(int nIndex);
    SWMGTrackInfo* GetProtoTrack(const CResRef& cResRef);
    CSWMGObstacle* RemoveObstacle(CSWMGObstacle* pObstacle);
    CSWMGObstacle* GetObstacle(int nIndex);
    CAurCamera* SetCamera(CAurCamera* pCamera);
    CSWMiniPlayer* SetPlayer(CSWMiniPlayer* pPlayer);
    CSWMiniPlayer* SetPlayer(CExoArrayList<CResRef>& lstModels,
                             CExoArrayList<unsigned char>& lstRotating,
                             const CResRef& cTrack);
    CSWMiniPlayer* SetPlayer(CExoArrayList<CResRef>& lstModels,
                             CExoArrayList<unsigned char>& lstRotating,
                             const CResRef& cTrack, const CResRef& cCamera,
                             unsigned char bCameraRotate);
    int SetMouseAxisX(int nAxis);
    int SetMouseAxisY(int nAxis);
    void SetCurPos(Vector vPosition);
    CSWMiniEnemy* AddEnemy(CExoArrayList<CResRef>& lstModels,
                           CExoArrayList<unsigned char>& lstRotating,
                           const CResRef& cTrack);
    int ClearEnemies();
    SWMGTrackInfo* AddTrack(const CResRef& cResRef, const Vector& vPosition);
    int ClearTracks();
    CSWMGObstacle* AddObstacle(const CResRef& cResRef, const Vector& vPosition);
    int ClearObstacles();
    int Load();
    void AddBullet(CAurObject* pBullet);
    int HitCheckBullet(CAurObject* pBullet, CAurObject* pTarget,
                       CSWMiniGameObject* pHit);
    void DoHitCheck();
    void Update(float fTimeStep);

    CExoStreamingSoundSource* m_pImpactSound;
    CAurCamera* m_pCam;
    CSWMiniPlayer* m_pPlayer;
    CSWCArea* m_pArea;
    CExoArrayList<unsigned long> m_lstEnemyIds;
    CExoArrayList<SWMGTrackInfo*> m_lstTracks;
    CExoArrayList<unsigned long> m_lstObstacleIds;
    CExoArrayList<SafePointer<CAurObject>*> m_lstBullets;
    CExoArrayList<SWMGExplodeBullet*> m_lstEffects;
    float m_fNearClip;
    float m_fFarClip;
    float m_fViewAngle;
    float m_fAxisScale;
    int m_nBumpPlane;
    int m_nDOF;
    int m_nAttachType;
    int m_nMouseAxisX;
    int m_nMouseAxisY;
    float m_fSteerValue;
    int m_bSteerCentering;
    unsigned char m_bUseInertia;
    unsigned char m_bDoBumping;
    CResRef m_cAmbientSoundResRef;
    CSWCRoom* m_pCurrentRoom;
    int m_nPhysicsMode;
    Vector m_vPosition;
    float m_fLateralAccel;
    CSWMGPhysicsState m_CurrentState;
    CSWMGPhysicsState m_NextState;
    float m_fF0;
    float m_fF4;
    float m_fF8;
    C2DA* m_pFC;
    int m_anSwoopUpgrades[6];
    float m_fJumpSpeed;
};

#include <aurpointer.h>
#include "exobase.h"
#include "math3d.h"
#include "aurbehavior.h"

class CResGFF;
class CResStruct;
class CSWMGObstacle;
class CSWMiniEnemy;
class CSWMiniGameObject;
class CSWMiniPlayer;
class CSWTrackFollower;
class Gob;

enum SWMiniGameObjectType
{
    SWMG_OBJECT_ANY = 0,
    SWMG_OBJECT_PLAYER = 1,
    SWMG_OBJECT_ENEMY = 2,
    SWMG_OBJECT_OBSTACLE = 3,
    SWMG_OBJECT_FOLLOWER = 4,
    SWMG_OBJECT_TRIGGER = 5
};

class CSWMiniGameObject
{
public:

    enum ScriptEvent
    {
        SCRIPT_ANIM_EVENT = 3
    };
    CSWMiniGameObject(unsigned long nType);
    ~CSWMiniGameObject();

    virtual void PlayAnimation(char* szAnimation, float fSpeed, int nFlags) = 0;
    virtual CExoString GetName() = 0;
    virtual void LoadScripts(CResGFF* pGFF, CResStruct* pStruct,
                             CResStruct& structScripts);
    virtual CResRef GetScriptName(int nScript) = 0;
    virtual void SetScriptName(int nScript, const CResRef& resRef) = 0;
    virtual CSWTrackFollower* AsFollower()  // PC 0x004065A0 GOG 0x00406570
    {
        return 0;
    }
    virtual CSWMiniPlayer* AsPlayer()  // PC 0x004065A0 GOG 0x00406570
    {
        return 0;
    }
    virtual CSWMiniEnemy* AsEnemy()  // PC 0x004065A0 GOG 0x00406570
    {
        return 0;
    }
    virtual CSWMGObstacle* AsObstacle()  // PC 0x004065A0 GOG 0x00406570
    {
        return 0;
    }
    virtual void Update(float fTime);

    void* GetLastBulletHitShooter();
    void* GetLastFollowerHit();
    // PC 0x006C6060 GOG 0x006C5D70
    void SetBullet(const CSWMGBullet& bullet)
    {
        m_bullet = bullet;
    }

    void OnHitBullet(int bScript);
    void OnHitFollower(int bScript);
    void OnCreate();

    unsigned long m_nType;
    unsigned long m_nObjectId;
    unsigned char m_bCreated;
    CExoString m_sName;

    SafePointer<CAurObject> m_pModel;
    CSWMGBullet m_bullet;

    CExoString m_sLastBulletHitPart;
    unsigned long m_nLastBulletHitShooter;
    unsigned long m_nLastFollowerHit;
    Vector m_vSoundPosition;
};

class CSWMiniGameObjectArray
{
public:
    CSWMiniGameObjectArray();

    void* GetMiniGameObject(unsigned long nObjectId, unsigned char nType);
    CSWMiniGameObject* GetMiniGameObjectByName(const CExoString& sName);

    enum { MAX_OBJECTS = 255, INVALID_OBJECT_ID = 255 };
    unsigned long FindNextEmptyIndex();

    unsigned long m_nCurrentObject;
    CSWMiniGameObject* m_apObjects[MAX_OBJECTS];
};

class CAurCamera;
class CAurObject;
class Gob;
class CExoSoundSource;
class CExoStreamingSoundSource;
class CExoString;
class CResGFF;
class CResRef;
class CResStruct;
class CSWCArea;
class CSWMGPhysicsState;
class CSWMGTargettingGunBank;
class CSWMGTargettingParameters;
class CSWMiniGameObject;
class Vector;
class Quaternion;
#include "exoresman.h"
#include "exobase.h"
#include "aurmodel.h"

class CSWMGBullet;
class CSWMGGunBank;
class CSWMGObstacle;
class CSWMiniPlayer;
class CSWMiniEnemy;

class CSWMGSound
{
public:
    CSWMGSound()
    {
        m_nVolume = 0;
        m_nFrequency = 0;
        m_bFrequencyIsRandom = 0;
    }

    int m_nVolume;
    int m_nFrequency;
    int m_bFrequencyIsRandom;
};

class SWMGFollowerModel
{
public:
    Gob* m_pModel;
    unsigned char m_bAttached;
    char m_abPadding[3];
};

class SWMGKeyInfo
{
public:
    SWMGKeyInfo()
    {
    }

    float m_fTime;
    Vector m_vPosition;
    Quaternion m_qOrientation;
    unsigned char m_bPosition;
    unsigned char m_bOrientation;
};

class CSWTrackFollower : public CSWMiniGameObject
{
public:

    static const int INVALID_GUN_BANK = -1;

    static const int NUM_SOUND_SOURCES = 1;

    enum { NUM_SCRIPTS = 13 };

    enum SoundSlot
    {
        SOUND_ENGINE = 0,
        SOUND_DEATH = 1,
        SOUND_COUNT = 2
    };

    enum SoundPriorityGroup
    {
        SOUND_PRIORITY_PLAYER = 23,
        SOUND_PRIORITY_ENEMY = 24
    };

    static const int LOOP_FOREVER = -1;

    static const int LOOP_UNSPECIFIED = -10;
    enum FollowerScriptEvent
    {

        SCRIPT_DEATH = 6,
        SCRIPT_TRACK_LOOP = 9,
        SCRIPT_ACCELERATE = 10,
        SCRIPT_BRAKE = 11
    };

    static const unsigned char DEATH_SOUND_VOLUME = 100;

    enum TargetTypeFlags
    {
        TARGET_PLAYER = 1,
        TARGET_ENEMY = 2
    };
    virtual void PlayAnimation(char* szAnimation, float fSpeed, int nFlags);

    virtual CExoString GetName();
    virtual void LoadScripts(CResGFF* pGFF, CResStruct* pStruct,
                             CResStruct& structScripts);
    virtual CResRef GetScriptName(int nScript);
    virtual void SetScriptName(int nScript, const CResRef& resRef);
    void OnAccelerate(int bValue);
    void OnBrake(int bValue);
    virtual CSWTrackFollower* AsFollower()  // PC 0x00698BD0 GOG 0x00698980
    {
        return this;
    }
    virtual void Update(float fTime);

    virtual ~CSWTrackFollower();
    virtual int Load(CResGFF* pGFF, CResStruct* pStruct);
    virtual int LoadGun(CResGFF* pGFF, CResStruct* pStruct) = 0;
    virtual int AddModel(CAurObject* pModel, unsigned char bAttach);
    virtual int SetLooping(int nLooping) = 0;
    virtual int TargetType() = 0;

    CSWTrackFollower(CSWMiniGame* pMiniGame, unsigned long nType);

    float GetSpeed()
    {
        return m_fSphereRadius;
    }
    float GetTrackSpeed()
    {
        return m_fTrackSpeed;
    }
    // PC 0x006C8CF0 GOG 0x006C89F0
    void SetBullet(CSWMGBullet* pBullet)
    {
        m_lastBulletFired = *pBullet;
    }
    float SetSpeed(float fSpeed);
    float SetSphereRadius(float fRadius);
    int SetHitPoints(int nHitPoints);
    float SetInvincibility(float fTime);
    int GetIsInvulnerable();
    int SetMaxHitPoints(int nHitPoints);
    int GetSoundVolume(int nSound);
    void SetSoundFrequency(int nSound, int nFrequency);
    int GetSoundFrequencyIsRandom(int nSound);
    void SetSoundFrequencyIsRandom(int nSound, int bRandom);
    int GetSoundFrequency(int nSound);
    void SetSoundVolume(int nSound, int nVolume);
    int GetLastObstacleHit();

    CSWMiniGame* GetMiniGame()
    {
        return m_pMiniGame;
    }

    CSWMGTargettingGunBank* GetGunBank(int nGunBank);
    Gob* GetModel(int nIndex);
    CAurObject* RemoveModel(CAurObject* pModel);
    void UpdateGunBanks();
    int DoBulletHitCheck(CAurObject* pBullet);
    Vector GetPosition();
    int ForceSetTrack(CAurObject* pTrack, int bPlayAnimation);
    int SetTrack(CAurObject* pTrack, int bPlayAnimation);
    void Stop();
    void Hit();
    void AdjustHitPoints(int nHitPoints);
    void OnHitObstacle(int nReserved);
    void OnDamage(int nReserved);
    int DoObstacleHitCheck(CSWMGObstacle* pObstacle);
    void OnDeath(int nReserved);
    void RemoveGunBank(CSWMGGunBank* pGunBank);
    void Go();
    int AttachGuns(CAurObject* pObject);
    void AttachGuns();
    CExoSoundSource* SetSoundName(int nSound, const CResRef& resRef);
    void LoadSounds(CResGFF* pGFF, CResStruct* pStruct,
                    CResStruct& structSounds);

    CSWMiniGame* m_pMiniGame;
    Gob* m_pTrack;
    CExoArrayList<SWMGFollowerModel> m_lstModels;
    CExoArrayList<CSWMGGunBank*> m_lstGunBanks;
    int m_nNumLoops;
    float m_fSphereRadius;

    struct
    {
        unsigned int m_bTrackPlaying : 1;
        unsigned int m_bDying : 1;
        unsigned int m_nUnusedFlags : 30;
    };

    int m_nHitPoints;
    int m_nMaxHPs;
    int m_nBumpDamage;
    float m_fTrackSpeed;
    float m_fInvincibilityRemaining;
    float m_fInvincePeriod;
    CResRef m_aScripts[NUM_SCRIPTS];
    CExoSoundSource* m_apSoundSources[NUM_SOUND_SOURCES];
    CResRef m_aOtherSoundNames[1];
    CSWMGSound m_aSounds[2];
    int m_nLastObstacleHit;

    CSWMGBullet m_lastBulletFired;
    int m_nDamage;
};

class CSWMGObstacle : public CSWMiniGameObject
{
public:
    SafePointer<CAurObject> m_pModel;
    CResRef m_aScripts[5];
    virtual void PlayAnimation(char* szAnimation, float fSpeed, int nFlags)  // PC 0x006C4CF0 GOG 0x006C4B20
    {
        if (m_pModel != 0)
        {
            ((Gob*)(CAurObject*)m_pModel)->PlayAnimation(szAnimation, fSpeed, nFlags,
                                                          0.0f);
        }
    }
    virtual CExoString GetName();
    virtual void LoadScripts(CResGFF*, CResStruct*, CResStruct&);
    virtual CResRef GetScriptName(int);
    virtual void SetScriptName(int, CResRef const&);
    virtual CSWMGObstacle* AsObstacle()  // PC 0x00698BD0 GOG 0x00698980
    {
        return this;
    }
    int Load(CResGFF*, CResStruct*);
    CSWMGObstacle(CSWMiniGame*, CAurObject*);
    ~CSWMGObstacle();
};

class CSWMiniEnemy : public CSWTrackFollower
{
public:
    CSWMiniEnemy(CSWMiniGame* pMiniGame)
        : CSWTrackFollower(pMiniGame, SWMG_OBJECT_ENEMY)
    {
    }
    // PC 0x00698BD0 GOG 0x00698980
    virtual CSWMiniEnemy* AsEnemy()
    {
        return this;
    }

    unsigned char m_bTrigger;
    virtual int SetLooping(int);
    CSWMGGunBank* CreateGunBank(int, CResRef const&, CResRef const&, CSWMGBullet const&,
                      CSWMGTargettingParameters const&, CResRef const&);
    virtual int LoadGun(CResGFF*, CResStruct*);
    virtual int Load(CResGFF*, CResStruct*);
    virtual int TargetType();
};

class CSWMiniPlayer : public CSWTrackFollower
{
public:

    CExoArrayList<SWMGKeyInfo> m_lstPositionKeys;
    CExoArrayList<SWMGKeyInfo> m_lstOrientationKeys;

    CExoArrayList<SWMGKeyInfo*> m_lstKeyAddresses;
    Vector m_vOffset;
    int m_nBankPosition;

    unsigned char m_bSpeedOverride;
    char gap_1D5[3];
    float m_fMinimumSpeed;
    float m_fMaximumSpeed;
    float m_fAPS;
    Vector m_vTunnelMinimum;
    Vector m_vTunnelMaximum;
    Vector m_vTunnelWrap;
    Vector m_vOrigin;
    Vector m_vTarget;
    Gob* m_pCameraHook;
    unsigned char m_bCameraHookAttached;
    char gap_225[3];

    Vector m_vInertia;
    Vector m_vInitialHook;

    Quaternion m_qInitialHook;
    Gob* GetCameraHook()
    {
        return m_pCameraHook;
    }
    void KeepInTunnel(Vector& vPosition);
    Vector SetOffset(const Vector& vOffset);
    void AdjustPosition(int nPosition, int bRelative);
    virtual int SetLooping(int);
    float SetMinSpeed(float);
    float SetMaxSpeed(float);
    float SetAPS(float);
    Vector SetOrigin(const Vector& vOrigin);
    void SetCameraHook(CAurObject*, unsigned char);
    Vector DoInertia(const Vector& vPrevious, const Vector& vCurrent,
                     const Quaternion& qOrientation, float fTime);
    int DoFollowerHitCheck(CExoArrayList<unsigned long>& lstEnemyIds,
                           int bRunScripts);
    void Control(float, CSWMGPhysicsState&, CSWMGPhysicsState&);
    void Fire();
    CSWMiniPlayer(CSWMiniGame*);
    ~CSWMiniPlayer();
    virtual CSWMiniPlayer* AsPlayer()  // PC 0x00698BD0 GOG 0x00698980
    {
        return this;
    }
    virtual int TargetType()  // PC 0x0065D0F0 GOG 0x0065CF00
    {
        return TARGET_PLAYER;
    }
    CSWMGGunBank* CreateGunBank(int, CResRef const&, CResRef const&,
                                CSWMGBullet const&, CResRef const&);
    virtual int LoadGun(CResGFF*, CResStruct*);
    virtual int Load(CResGFF*, CResStruct*);
};

class CAurObject;
class CResRef;

#include "exobase.h"
#include "exoresman.h"
#include "aurbehavior.h"
#include "nwscript.h"

class CAurPart;
class CSWTrackFollower;

extern CVirtualMachine* g_pVirtualMachine;
CAurObject* NewCAurObject(char* sModel, char* sName, FILE* pModelFile,
                          FILE* pVertexFile);
void FireGunCallback(CAurObject* pObject, char* sEvent, void* pData);
void GunDie(CAurObject* pObject, char* sEvent, void* pData);

class CSWMGGunBank
{
public:
    CSWMGGunBank(CSWTrackFollower* pFollower, const CResRef& sBank,
                 const CResRef& sGun, const CSWMGBullet& bullet);
    ~CSWMGGunBank();

    virtual CAurObject* AddGun(CAurObject* pObject, CAurPart* pPart);
    virtual void Die();
    virtual int IsTargetting()  // PC 0x004065A0 GOG 0x00406570
    {
        return 0;
    }

    void OnFire();

    CSWTrackFollower* GetFollower() { return m_pFollower; }
    void SetBankModel(const CResRef& sBank);
    float SetBulletSpeed(float fSpeed);
    float SetBulletLifeSpan(float fLifeSpan);
    int SetBulletTargetType(int nTargetType);
    float SetRateOfFire(float fRateOfFire);
    void Hit();
    CAurObject* RemoveGun(CAurObject* pGun);
    void Fire();
    void SetGunModel(const CResRef& sGun);
    int ClearGuns();

    CResRef m_resBank;
    CResRef m_resGun;
    CResRef m_resFireSound;
    CExoArrayList<CAurObject*> m_lstGuns;
    CSWMGBullet m_bullet;
    CSWTrackFollower* m_pFollower;
};

class CSWMGTargettingGunBank : public CSWMGGunBank
{
public:
    CSWMGTargettingGunBank(CSWTrackFollower* pFollower, const CResRef& sBank,
                           const CResRef& sGun, const CSWMGBullet& bullet,
                           const CSWMGTargettingParameters& params,
                           CSWTrackFollower* pTarget);

    virtual CAurObject* AddGun(CAurObject* pObject, CAurPart* pPart);
    virtual void Die();
    virtual int IsTargetting()  // PC 0x0065D0F0 GOG 0x0065CF00
    {
        return 1;
    }

    float SetSensingRadius(float fRadius);
    float SetVerticalSpread(float fSpread);
    float SetHorizontalSpread(float fSpread);
    float SetInaccuracy(float fInaccuracy);

    CSWMGTargettingParameters m_params;
    CSWTrackFollower* m_pTarget;
};

#endif
