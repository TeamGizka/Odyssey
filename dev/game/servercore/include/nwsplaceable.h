#ifndef __NWSPLACEABLE_H__
#define __NWSPLACEABLE_H__

class CItemRepository;
class CResGFF;
class CResRef;
class CResStruct;
class CSWCObject;
class CSWCPlaceable;
class CSWSArea;
class CSWSItem;
class CSWSObjectActionNode;
class CSWSTrigger;
class CWalkHitInfo;
class Quaternion;
class Vector;
#include "exobase.h"
#include "math3d.h"
#include "nwsobject.h"
#include "SWCollisionMesh.h"

class CSWSPlaceable : public CSWSObject
{
public:

    enum { HEARTBEAT_INTERVAL = 6000 };

    enum
    {
        BODY_BAG_LOW_HARDNESS_APPEARANCE = 217,
        BODY_BAG_DESCRIPTION_STRREF = 38612,
        BODY_BAG_HIT_POINTS = 15,
        BODY_BAG_DISARM_DC = 15,
        BODY_BAG_OPEN_LOCK_DC = 18,
        BODY_BAG_FORTITUDE_SAVE = 16
    };

    enum
    {
        SCRIPT_ONCLOSED,
        SCRIPT_ONDAMAGED,
        SCRIPT_ONDEATH,
        SCRIPT_ONDISARM,
        SCRIPT_ONHEARTBEAT,
        SCRIPT_ONINVDISTURBED,
        SCRIPT_ONLOCK,
        SCRIPT_ONMELEEATTACKED,
        SCRIPT_ONOPEN,
        SCRIPT_ONSPELLCASTAT,
        SCRIPT_ONTRAPTRIGGERED,
        SCRIPT_ONUNLOCK,
        SCRIPT_ONUSED,
        SCRIPT_ONUSERDEFINED,
        SCRIPT_ONDIALOG,
        SCRIPT_ONENDDIALOGUE,
        SCRIPT_ONFAILTOOPEN,
        NUM_SCRIPTS
    };

    CSWSPlaceable(unsigned long oidSelf);
    CExoString* GetScriptName(int nScript)
    {
        if (nScript < 0 || nScript >= NUM_SCRIPTS)
            return NULL;
        return &m_sScripts[nScript];
    }

    enum AnimationState
    {
        ANIMATION_STATE_BASE = 0,
        ANIMATION_STATE_OPEN,
        ANIMATION_STATE_CLOSED,
        ANIMATION_STATE_DESTROYED,
        ANIMATION_STATE_ACTIVATE,
        ANIMATION_STATE_DEACTIVATE
    };

    int GetFactionId() { return m_nFaction; }
    void SetFactionId(int nFaction) { m_nFaction = nFaction; }
    unsigned char GetTrapType() { return m_nTrapType; }
    unsigned char GetOpenLockDC();
    unsigned char GetDetectDC();
    unsigned char GetDisarmDC();
    void SetFlagged(unsigned char bFlagged) { m_bTrapFlagged = bFlagged; }
    unsigned char GetFlagged() { return m_bTrapFlagged; }
    int GetTrapFlag() { return m_nTrapFlag; }
    CExoArrayList<unsigned long>* GetCreatureList() { return &m_lstTrapDetectedBy; }
    unsigned char GetBodyBag() { return m_nBodyBag; }
    int GetGroundPile() { return m_bGroundPile; }
    int GetIsBodyBag() { return m_bIsBodyBag; }
    int GetIsCorpse() { return m_bIsCorpse; }
    void SetLocked(int bLocked) { m_bLocked = bLocked; }
    virtual ~CSWSPlaceable();
    virtual CSWSPlaceable* AsSWSPlaceable()  // PC 0x00698BD0
    {
        return this;
    }
    virtual int GetDead();
    virtual void AIUpdate();
    virtual void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay);
    virtual CResRef GetDialogResref()  // PC 0x00587F50
    {
        return m_cConversation;
    }
    virtual CExoLocString* GetFirstName()  // PC 0x00587F40
    {
        return &m_lsLocName;
    }
    virtual void DoDamage(int nDamage);
    virtual unsigned long GetEffectSpellId()  // PC 0x00587F80
    {
        return m_EffectSpellId;
    }
    virtual void SetEffectSpellId(unsigned long nSpellId)
    {
        m_EffectSpellId = nSpellId;
    }

    void PostProcess();
    void RemoveFromArea();
    int RemoveItem(CSWSItem* pItem);
    int AcquireItem(CSWSItem** ppItem, unsigned long oidPossessor,
                    int bDisplayFeedback);
    CSWCPlaceable* GetClientPlaceable();
    Vector GetNearestActionPoint(const Vector& vPosition);
    int AddCastSpellActions(unsigned long nSpellId, int nMultiClass,
                            const Vector& vTarget, unsigned long oidTarget,
                            int bFake, unsigned char nProjectilePathType);
    int AIActionCastSpell(CSWSObjectActionNode* pNode);
    void SetLightIsOn(int bLightIsOn);
    int GetLightIsOn();
    unsigned char GetBodyBagAppearance();
    int NoNonWalkPolys(const Vector& vStart, const Vector& vEnd,
                       const float& fDistance, const float& fWidth,
                       const float& fHeight, CWalkHitInfo* pWalkHitInfo);
    void SetOrientation(const Quaternion& qOrientation);
    void OnEndDialog(CSWSObject* pObject);
    int GetItemCount(int bDropableOnly);
    int LoadPlaceable(CResGFF* pRes, CResStruct* pStruct, int bLoadState);
    int LoadBodyBag(unsigned char nAppearance);
    int SavePlaceable(CResGFF* pRes, CResStruct* pStruct);
    void AddToArea(CSWSArea* pArea, const Vector& vPosition, int bLoadingGame);
    void OpenInventory(unsigned long oidOpener, int bAnimate);
    void CloseInventory(unsigned long oidCloser, int bUpdatePlayer);
    void ClosePlaceableForAllPlayers();
    int LoadFromTemplate(const CResRef& cResRef, int bLoadState);
    int AcquireItemsFromObject(unsigned long oidSource, int bDropableOnly);

    CExoLocString m_lsLocName;
    unsigned long m_nAppearance;
    CExoLocString m_lsDescription;
    int m_nFaction;
    CResRef m_cConversation;
    int m_n250;
    int m_bGroundPile;
    unsigned char m_nHardness;
    char m_aUnmapped259[0x3];
    float m_fBearing;
    int m_bLocked;
    CExoString m_sKeyName;
    int m_bKeyRequired;
    int m_bAutoRemoveKey;
    unsigned char m_nOpenLockDC;
    unsigned char m_nCloseLockDC;
    unsigned char m_nOpenLockDiff;
    char m_nOpenLockDiffMod;
    int m_nOwnerDemolitionsSkill;
    unsigned char m_bUseTweakColor;
    char m_aUnmapped2BD[0x3];
    unsigned long m_nTweakColor;
    unsigned char m_nTrapDetectDC;
    char m_aUnmapped277[0x1];
    int m_nTrapFlag;
    unsigned char m_nDisarmDC;
    char m_aUnmapped27D[0x3];
    int m_bTrapDisarmable;
    int m_bTrapDetectable;
    int m_bTrapOneShot;

    int m_bTrapFlagged;
    unsigned char m_nTrapType;
    char m_aUnmapped291[0x3];
    CExoString m_sScripts[NUM_SCRIPTS];
    unsigned char m_nFortSave;
    unsigned char m_nWillSave;
    unsigned char m_nReflexSave;
    char m_aUnmapped317[0x1];
    CExoArrayList<unsigned long> m_lstTrapDetectedBy;
    int m_bHasInventory;
    int m_bUseable;
    int m_n32C;
    int m_bLockable;
    int m_bDieWhenEmpty;
    int m_bOpen;
    int m_bPreciseUse;
    int m_bHostile;
    int m_nStaticBoundingBox;
    unsigned long m_nHeartbeatDay;
    unsigned long m_nHeartbeatTime;
    unsigned long m_oidLastOpenedBy;
    unsigned long m_oidLastClosedBy;
    unsigned long m_oidLastUsedBy;
    unsigned long m_oidLastTriggeredBy;
    unsigned long m_oidLastDisarmedBy;
    unsigned long m_oidLastLockedBy;
    unsigned long m_oidLastUnlockedBy;
    CItemRepository* m_pcItemRepository;

    unsigned short m_nInventoryIndex;
    unsigned short m_nInventoryContainerIndex;
    unsigned long m_oidInventoryContainer;

    unsigned short GetInventoryIndex() { return m_nInventoryIndex; }
    void SetInventoryIndex(unsigned short nIndex) { m_nInventoryIndex = nIndex; }
    unsigned short GetInventoryContainerIndex() { return m_nInventoryContainerIndex; }
    void SetInventoryContainerIndex(unsigned short nIndex) { m_nInventoryContainerIndex = nIndex; }
    unsigned long GetInventoryContainer() { return m_oidInventoryContainer; }
    void SetInventoryContainer(unsigned long oidContainer) { m_oidInventoryContainer = oidContainer; }
    CResRef m_cTemplateResRef;
    unsigned long m_EffectSpellId;
    int m_bLightIsOn;
    int m_n390;
    unsigned char m_nBodyBag;
    char m_aUnmapped395[0x3];
    int m_bStatic;

    int m_bDynamic;
    CSWPlaceableSurfaceMesh m_cSurfaceMesh;
    Quaternion m_qOrientation;
    int m_n438;
    int m_n43C;
    int m_bIsBodyBag;
    int m_bIsBodyBagVisible;
    unsigned long m_oidBodyBagCreature;
    int m_bIsCorpse;

    int m_bInventoryOpening;
    CSWSTrigger* m_pTrapTrigger;
};

#endif
