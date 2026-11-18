#ifndef __NWCPLACEABLE_H__
#define __NWCPLACEABLE_H__

class CResRef;
class CSWCArea;
class Vector;
#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "nwcobject.h"
#include "nwmessage.h"
#include "nwsobject.h"
#include "aurmodel.h"
#include "nwcmodule.h"
#include "nwcarea.h"
#include "nwsplaceable.h"

class CSWCCreature;
class CSWCAnimBasePlaceable;
class CAurObject;

class CSWCPlaceable : public CSWCObject
{
public:

    enum { LOAD_MODEL_FLAGS_PLACEABLE = 0x0a,
           STRREF_CANNOT_SET_MINES = 47861, STRREF_LOCK = 48180, TUTORIAL_SET_MINE = 2 };
#ifndef _XBOX
#endif

    CSWCPlaceable();
    virtual ~CSWCPlaceable();
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWCPlaceable* AsSWCPlaceable()
    {
        return this;
    }
    // PC 0x006D96B0 GOG 0x006DB220 Xbox 0x0009B430
    virtual CExoString GetName()
    {
        return m_sName;
    }
    virtual void AIUpdate();
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nFlags);
    virtual void UnloadModel();
    virtual void UnloadModel(unsigned char nModel);
    virtual int SetAnimation(unsigned short nAnimation, int nFlags, int nParam);
    virtual int ApplyShadowBlob();
    virtual int IsHostile();
    virtual unsigned char GetReticlePoint(Vector& vPoint);
    virtual float GetCanSeeHeight();

    void AddFireAndForgetAnimation(unsigned short nAnimation, void* pData,
                                  unsigned short nFlags);
    void UsePlaceable(unsigned long oidUser, CSWCCreature* pCreature);
    void BashPlaceable(unsigned long oidUser, CSWCCreature* pCreature);
    int LoadLight();
    void UnloadLight();
    void ActionMenuSecurity(unsigned long oidUser, CSWCCreature* pCreature);
    void GetTargetActions(CSWCCreature* pCreature, int nMenu,
                          CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    CSWSPlaceable* GetServerPlaceable();
    void SetAppearance(int nAppearance);
    int GetAppearance() { return m_nAppearance; }
    void AnimationUpdate();
    void HilightPlaceable(int bHilight);
    void ToggleLockState();
    void ShowFeedbackBubbleText();

    CExoString m_sName;
    CExoString m_sTag;
    int m_nAppearance;

    int m_bPlotObject;
    int m_bLockable;
    int m_bLocked;
    int m_bNotBlastable;

    int m_bTrap;
    int m_bHasDescription;
    int m_bTrapDetected;
    int m_bUseable;
    int m_bSelectable;
    int m_bTrapDisarmable;
    int m_nTrapType;
    int m_nTrapDisarmDC;
    int m_bHilighted;
    int m_nHilightTime;
    CExoString m_sModel;
    Gob* m_pLight;

    void MenuActionUseItem(unsigned long oidItem, CSWCCreature* pCreature);
#ifndef _XBOX
    void MenuActionSetPlaceableMine(unsigned long oidItem, CSWCCreature* pCreature);
#endif
    void GetUseItems(CSWCCreature* pCreature, CExoArrayList<CSWGuiInterfaceAction>& lstActions);
#ifndef _XBOX
    void GetPlaceableInterfaceMines(CSWCCreature* pCreature,
                                    CExoArrayList<CSWGuiInterfaceAction>& lstActions);
#endif
    int ActionMenuExamine(unsigned long oidTarget, CSWCCreature* pCreature);
    int ActionMenuGiveItem(unsigned long oidTarget, CSWCCreature* pCreature);
    int MenuActionUseComputerSpike(unsigned long oidTarget, CSWCCreature* pCreature);
    void SetMineIcon(unsigned char nIcon);
};

#endif
