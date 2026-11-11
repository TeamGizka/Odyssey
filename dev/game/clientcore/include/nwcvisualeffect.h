#ifndef __NWCVISUALEFFECT_H__
#define __NWCVISUALEFFECT_H__

class CExoSoundSource;
class CResRef;
class CAurObject;
class CSWCAnimBaseVisualEffect;
class Vector;
#include "exobase.h"
#include "nwcobject.h"

extern float weirdanimspeed;

class CSWCVisualEffect : public CSWCObject
{
public:

    static const unsigned long INVALID_EFFECT = -1;

    enum
    {
        EFFECT_UNHILITED_FIRST = 39,
        EFFECT_UNHILITED_SECOND = 40
    };

    CSWCVisualEffect(int bAddToGameObjectArray);
    virtual ~CSWCVisualEffect();

    virtual void AIUpdate();
    virtual int LoadModel(const CResRef& cResRef, unsigned char nModel,
                          unsigned char nFlags);
    virtual void UnloadModel(unsigned char nModel);
    virtual void UnloadModel();
    virtual void AttachToScene(CAurScene* pScene);
    virtual void SetPosition(const Vector& vPosition);

    int LoadEffect(unsigned long nEffect);
    void SetAnimation(unsigned short nAnimation);
    static void UnloadCachedVFX();
    void DoVFXRumble();

    CExoSoundSource* m_pSoundSource;
    int m_nDuration;
    unsigned long m_nAnimationDelay;
    unsigned long m_nEffect;
    unsigned short m_nAnimation;
    unsigned char m_aUnmapped10E[2];
    int m_bDurationActive;
    int m_bFinished;
    int m_bOneShot;
    int m_bDeleteWhenFinished;
    int m_bGroundEffect;
    int m_bAttachWhenFinished;
    int m_bNoExpirationDelay;
    int m_bShadows;

    static CExoLinkedList<CAurObject> m_lstCachedVFX;
};

#endif
