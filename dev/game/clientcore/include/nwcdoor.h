#ifndef __NWCDOOR_H__
#define __NWCDOOR_H__

#include "exobase.h"
#include "exoresman.h"
#include "nwcobject.h"

class CSWCCreature;
class CSWGuiInterfaceAction;
class CSWSDoor;

class CSWCDoor : public CSWCObject
{
public:

    enum DoorState
    {
        DOOR_STATE_CLOSED = 0,
        DOOR_STATE_OPEN1 = 1,
        DOOR_STATE_OPEN2 = 2,

        DOOR_STATE_DESTROYED = 3
    };

    static const int MODEL_CALLBACK_DELAY = 9999;

    enum { NETWORK_MODEL_LOAD_FLAGS = 5 };

    enum
    {
        INITIAL_APPEARANCE_TYPE = 255,
        INITIAL_GENERIC_TYPE = 255
    };

    CSWCDoor();
    virtual ~CSWCDoor();
    // PC 0x00698BD0
    virtual CSWCDoor* AsSWCDoor()
    {
        return this;
    }
    // PC 0x006D96B0
    virtual CExoString GetName()
    {
        return m_sDescription;
    }
    virtual void AIUpdate();
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nFlags);
    virtual void UnloadModel();
    virtual void UnloadModel(unsigned char nModel);
    // PC 0x0066FC40
    int GetIsOpen()
    {
        return ((CSWCObject*)this)->GetCurrentAnimation() == DOOR_ANIMATION_OPEN1 ||
               ((CSWCObject*)this)->GetCurrentAnimation() == DOOR_ANIMATION_OPEN2;
    }
    void SetIsAreaTransition(int bAreaTransition);
    int SetAnimationWithoutTrigger(unsigned short nAnimation);
    void SetState(unsigned char nState);
    void ToggleDoorState(unsigned long oidDoor, CSWCCreature* pCreature);
    void AddFireAndForgetAnimation(unsigned short nAnimation, void* pData,
                                   unsigned short nFlags);
    void MenuActionSecurity(unsigned long oidDoor, CSWCCreature* pCreature);
    void MenuActionBash(unsigned long oidDoor, CSWCCreature* pCreature);
    void GetTargetActions(CSWCCreature* pCreature, int nMenu,
                         CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    CSWSDoor* GetServerDoor();
    void AnimationUpdate();
    void UpdateAreaTransitionDisplay();
    void HilightDoor(int bHilight);
    // PC 0x007343D0
    CExoString GetTransitionDestination()
    {
        return m_sTransitionDestination;
    }

    CExoString m_sDescription;

    int m_bLocked;
    int m_bLockable;

    int m_bNoVisibleModel;

    int m_bTrapDetected;
    int m_bTrap;
    int m_bUseable;
    unsigned char m_nState;

    int m_bHilighted;
    unsigned char m_nAppearanceType;
    unsigned char m_nGenericType;

    int m_nDoorData;
    int m_bAreaTransition;

    CExoString m_sModel;
    int m_bStatic;
    CExoString m_sTransitionDestination;
#ifdef _XBOX

    void GetUseItems(CSWCCreature* pCreature, CExoArrayList<CSWGuiInterfaceAction>& lstActions);
#endif
};

#endif
