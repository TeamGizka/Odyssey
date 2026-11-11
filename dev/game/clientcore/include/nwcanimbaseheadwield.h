#ifndef __NWCANIMBASEHEADWIELD_H__
#define __NWCANIMBASEHEADWIELD_H__

#include "nwcanimbase.h"
class CAurObject;
class CResRef;
class CSWGuiExtent;
#include "nwcanimbasehead.h"
#include "nwcanimbasewield.h"
#include "swguidrawable.h"

class CSWCAnimBaseHeadWield : public virtual CSWCAnimBaseHead,
                              public virtual CSWCAnimBaseWield
{
public:
    CSWCAnimBaseHeadWield();
    virtual ~CSWCAnimBaseHeadWield();

    virtual Gob* GetModel(unsigned char nModel);
    virtual int LoadModel(const CResRef& sModel, unsigned char nModel);
    virtual int ReplaceTexture(unsigned char nModel, const CResRef& sOld,
                               const CResRef& sNew, int nFlags);
    virtual int ReplaceTexture(const CResRef& sOld, const CResRef& sNew,
                               int nFlags);
    virtual int ReplaceTexture(unsigned char nModel, const CResRef& sOld,
                               const CResRef& sNew, unsigned long nColor,
                               unsigned short* pnPalette, int nPaletteSize,
                               unsigned long nFlags);
    virtual int ReplaceTexture(const CResRef& sOld, const CResRef& sNew,
                               unsigned long nColor, unsigned short* pnPalette,
                               int nPaletteSize, unsigned long nFlags);
    virtual int RestoreTexture();
    virtual int RestoreTexture(unsigned char nModel);
    virtual int EnableBumpedOutShell(const CResRef& sShell, float fScale);
    virtual int DisableBumpedOutShell();
    virtual int SetMotionBlur(int bBlur);
    virtual int SetOverlayAnimation(unsigned short nAnimation, int bLoop);
    virtual int SetAnimation(unsigned short nAnimation, float fSpeed,
                             int nFlags, float fTime);
    virtual int SetAnimationOnSubObject(unsigned char nModel,
                                        unsigned short nAnimation,
                                        float fSpeed, int nFlags, float fTime);
    virtual int SetAnimationVisemes(float* pfWeights, float* pfTimes,
                                    int nCount, float fLength);
    virtual void SetAlwaysRender(int bAlwaysRender);
    virtual void SetInCutscene(int bInCutscene);
    virtual void UnloadModel(unsigned char nModel);
    virtual void UnloadModel();
    virtual int Wield(unsigned long nObject, const CExoString& sSlot,
                      unsigned char nModel, float fScale);
    virtual void HideWieldedItems(const CResRef& sLeft, const CResRef& sRight);
    virtual void ShowWieldedItems();
    virtual void SetWieldLimited(int bLimited);
    virtual int SetEnvMap(int nType, int nFlags, const CResRef& sMap);
    virtual int SetEnvMap(unsigned char nModel, int nType, int nFlags,
                          const CResRef& sMap);
    virtual int RestoreEnvMap();
    virtual void TurnOnShadows();
    virtual void TurnOffShadows();
    virtual void UpdateSound(int nEvent);
    virtual int AddAnimationFlag(int nFlag);
    virtual int RemoveAnimationFlag(int nFlag);
    virtual void EnableDistortion();
    virtual void DisableDistortion();
    virtual int SetAnimationInternal(CAurObject* pObject,
                                     unsigned short nAnimation, float fSpeed,
                                     int nFlags, float fTime);
    virtual int SetAnimationOnSubObjectInternal(CAurObject* pObject,
                                                unsigned char nModel,
                                                unsigned short nAnimation,
                                                float fSpeed, int nFlags,
                                                float fTime);
    virtual void HideHeadItem();
    virtual void ShowHeadItem();
    virtual int GetBoundingRect(CSWGuiExtent& extent);
    virtual void SetHeadItem(unsigned long nItem);
};

#endif
