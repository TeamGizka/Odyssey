#ifndef __NWCTRIGGER_H__
#define __NWCTRIGGER_H__

#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "nwcobject.h"

class CSWCCreature;
class CSWGuiInterfaceAction;
class CSWSArea;
class PartTrigger;

class CSWCTrigger : public CSWCObject
{
public:

    enum { LOAD_MODEL_FLAGS_TRIGGER = 9 };
    enum RenderMode { RENDER_TRIGGER = 1, RENDER_AREA_TRANSITION = 2 };

    enum { STRREF_MINE = 1421 };

    enum TriggerAnimation
    {
        TRIGGER_ANIMATION_EXPLODE = 0x15c,
        TRIGGER_ANIMATION_DISARM = 0x15d
    };

    CSWCTrigger();
    virtual ~CSWCTrigger();
    // PC 0x00698BD0
    virtual CSWCTrigger* AsSWCTrigger()
    {
        return this;
    }
    // PC 0x006EA750
    virtual CExoString GetName()
    {
        return m_sName;
    }
    virtual void AIUpdate();
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nFlags);
    virtual void UnloadModel();
    virtual void UnloadModel(unsigned char nModel);
    virtual int IsHostile()  // PC 0x006EA780
    {
        return m_bHostile;
    }
    virtual unsigned char GetReticlePoint(Vector& vPoint);

    void AddFireAndForgetAnimation(unsigned short nAnimation, void* pData,
                                   unsigned short nFlags);
    void UpdateTriggerColor(int nMode);
    void DisplayTrigger(int bDisplay);
    void SetCursor(unsigned char nTrigger);
    void UpdatePosition();
    unsigned char GetCenter(Vector& vCenter);
    void ActionMenuDisableMine(unsigned long nAction, CSWCCreature* pCreature);
    void ActionMenuRecoverMine(unsigned long nAction, CSWCCreature* pCreature);
    enum TrapType
    {
        TRAP_FLASH_STUN_MINOR = 0,
        TRAP_FLASH_STUN_AVERAGE = 1,
        TRAP_FLASH_STUN_DEADLY = 2,
        TRAP_FRAGMENTATION_MINE_MINOR = 3,
        TRAP_FRAGMENTATION_MINE_AVERAGE = 4,
        TRAP_FRAGMENTATION_MINE_DEADLY = 5,
        TRAP_PLASMA_MINOR = 6,
        TRAP_PLASMA_AVERAGE = 7,
        TRAP_PLASMA_DEADLY = 8,
        TRAP_POISON_GAS_MINOR = 9,
        TRAP_POISON_GAS_AVERAGE = 10,
        TRAP_POISON_GAS_DEADLY = 11,
        TRAP_SONIC_MINOR = 14,
        TRAP_SONIC_AVERAGE = 15,
        TRAP_SONIC_DEADLY = 16,
        TRAP_FLASH_STUN_STRONG = 17,
        TRAP_FLASH_STUN_DEVASTATING = 18,
        TRAP_FRAGMENTATION_MINE_STRONG = 19,
        TRAP_FRAGMENTATION_MINE_DEVASTATING = 20,
        TRAP_PLASMA_STRONG = 21,
        TRAP_PLASMA_DEVASTATING = 22,
        TRAP_POISON_GAS_STRONG = 23,
        TRAP_POISON_GAS_DEVASTATING = 24,
        TRAP_SONIC_STRONG = 25,
        TRAP_SONIC_DEVASTATING = 26
    };
    enum MineStrength
    {
        MINE_STRENGTH_NONE = -1,
        MINE_STRENGTH_MINOR = 0,
        MINE_STRENGTH_AVERAGE = 1,
        MINE_STRENGTH_STRONG = 2,
        MINE_STRENGTH_DEADLY = 3,
        MINE_STRENGTH_DEVASTATING = 4
    };
    int GetMineStrength(int nTrapType);
    void GetMineForcePowers(CSWCCreature* pCreature, int nSpell,
                            CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    CSWSTrigger* GetServerTrigger();
    void GetTargetActions(CSWCCreature* pCreature, int bFlag,
                          CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    void AnimationUpdate();
    int LineSegmentIntersectTrigger(const Vector& vStart, const Vector& vEnd,
                                    Vector* pvHit);
    void AddToRenderList(int nMode);
    void RemoveFromRenderList();
    void SpecialDisplayTrigger(int bDisplay);
    void UpdateAreaTransitionDisplay();

    int m_nInitialValue;
    CExoString m_sName;
    int m_bTrap;
    int m_bAreaTransition;
    int m_bDisplayed;
    int m_bHostile;
    float m_fHighlightFade;
    unsigned char m_aUnknown11C[0x10];
    short m_nCursor;
    int m_nNumVertices;
    Vector* m_pvVertices;
    PartTrigger* m_pPartTrigger;
    unsigned long m_nRemoveTimer;
    CExoString m_sTag;
    char m_cTriggerType;
};

#endif
