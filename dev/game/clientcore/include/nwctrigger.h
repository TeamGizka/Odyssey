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
    // PC 0x00641DB0
    virtual CSWCTrigger* AsSWCTrigger()
    {
        return this;
    }
    // PC 0x006912B0  Xbox 0x000B94A0
    virtual CExoString GetName()
    {
        return m_sName;
    }
    virtual void AIUpdate();
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nFlags);
    virtual void UnloadModel();
    virtual void UnloadModel(unsigned char nModel);
    virtual int IsHostile()  // PC 0x006912E0  Xbox 0x000B94D0
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
