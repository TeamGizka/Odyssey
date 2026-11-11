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

    enum { LOAD_MODEL_FLAGS_PLACEABLE = 0x0a };

    CSWCPlaceable();
    virtual ~CSWCPlaceable();
    // PC 0x00641DB0
    virtual CSWCPlaceable* AsSWCPlaceable()
    {
        return this;
    }
    // PC 0x006823C0  Xbox 0x000B7530
    virtual CExoString GetName()
    {
        return m_sName;
    }
    virtual void AIUpdate();
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nFlags);
    virtual void UnloadModel();
    virtual void UnloadModel(unsigned char nModel);
    virtual int SetAnimation(unsigned short nAnimation, int nA, int nB);
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
    void SetAppearance(unsigned char nAppearance);
    void AnimationUpdate();
    void HilightPlaceable(int bHilight);

    CExoString m_sName;
    CExoString m_sTag;
    unsigned char m_nAppearance;

    int m_bPlotObject;
    int m_nUnknown114;
    int m_bLocked;

    int m_bTrap;
    int m_nUnknown120;
    int m_bTrapDetected;
    int m_bUseable;
    int m_nUnknown12C;
    int m_nUnknown130;
    int m_nUnknown134;
    int m_nUnknown138;
    int m_nUnknown13C;
    int m_nUnknown140;
    CExoString m_sModel;
    Gob* m_pLight;
#ifdef _XBOX

    void MenuActionUseItem(unsigned long oidItem, CSWCCreature* pCreature);
    void GetUseItems(CSWCCreature* pCreature, CExoArrayList<CSWGuiInterfaceAction>& lstActions);
#endif
};

#endif
