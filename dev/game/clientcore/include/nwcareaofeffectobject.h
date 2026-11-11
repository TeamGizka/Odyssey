#ifndef __NWCAREAOFEFFECTOBJECT_H__
#define __NWCAREAOFEFFECTOBJECT_H__

class CResRef;
class Vector;
#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "nwcobject.h"

class CAurObject;
class CExoSoundSource;
class CSWCVisualEffect;

class CSWCAreaOfEffectObject : public CSWCObject
{
public:

    enum { AOE_PER_WALLBLADE = 7 };

    enum EffectShape
    {
        SHAPE_CIRCLE = 'C',
        SHAPE_RECTANGLE = 'R'
    };

    static const unsigned char SOUND_PRIORITY_AREA_EFFECT = 12;

    CSWCAreaOfEffectObject();
    virtual ~CSWCAreaOfEffectObject();
    // PC 0x00698BD0
    virtual CSWCAreaOfEffectObject* AsSWCAreaOfEffectObject()
    {
        return this;
    }

    virtual void AIUpdate();
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,  // PC 0x0069E520
                          unsigned char nFlags)
    {
        return 0;
    }
    virtual void UnloadModel(unsigned char nModel);
    virtual void UnloadModel();
    virtual void SetPosition(const Vector& vPosition);
    virtual int SetEnvMap(unsigned char nModel, int nType, int nFlags,
                          const CResRef& cEnvMap);
    virtual int SetEnvMap(int nType, int nFlags, const CResRef& cEnvMap);

    void ResetTimer(int nIndex);
    int LoadEffect(unsigned long nAreaEffectId);
    int SetAnimation(unsigned short nAnimation, unsigned char nInstance);
    void StopEffect();
    void SetMoveTimer(unsigned long nTime, CAurObject* pObject);
    int SetInstanceRandomPosition(unsigned char nInstance);
    void UpdateInstancePosition();

    unsigned long* m_pnLifetime;
    unsigned long* m_pnDelay;
    unsigned long* m_pnAnimTimer;
    int m_anDuration[3];
    int m_anInstanceCount[3];
    int m_nNumInstances;
    float m_fRadius;
    float m_fWidth;
    float m_fLength;
    unsigned long m_nEndTimer;
    int m_bFinished;
    int m_bStopping;
    unsigned char m_anTypeChance[3];
    char m_cShape;
    int* m_pbAttached;
    CAurScene* m_pScene;
    unsigned short* m_pnAnimation;
    CSWCVisualEffect* m_pVisualEffect;
    int m_bVisualAttached;
    CExoSoundSource* m_pSoundSource;
    CResRef m_cSoundResRef;
    int m_nSoundChance;
    int m_bSnapToGround;
    int m_nAreaEffectId;
    int m_bSoundStarted;
    unsigned long m_oidCreator;
};

#endif
