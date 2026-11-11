#ifndef __NWCANIMBASEHEAD_H__
#define __NWCANIMBASEHEAD_H__

#include "exoresman.h"
#include "nwcanimbase.h"

static inline void NwcanimbaseheadTrace2();

class CSWCAnimBaseHead : public virtual CSWCAnimBaseTW
{
public:
    CSWCAnimBaseHead();
    virtual ~CSWCAnimBaseHead();

    virtual Gob* GetModel(unsigned char nModel);
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel);
    virtual int ReplaceTexture(unsigned char nModel, const CResRef& cOld,
                               const CResRef& cNew, int bSave);
    virtual int ReplaceTexture(const CResRef& cOld, const CResRef& cNew,
                               int bSave);
    virtual int ReplaceTexture(unsigned char nModel, const CResRef& cOld,
                               const CResRef& cNew, unsigned long nCount,
                               unsigned short* pParts, int bSave,
                               unsigned long nStart);
    virtual int ReplaceTexture(const CResRef& cOld, const CResRef& cNew,
                               unsigned long nCount, unsigned short* pParts,
                               int bSave, unsigned long nStart);
    virtual int RestoreTexture();
    virtual int RestoreTexture(unsigned char nModel);
    virtual int EnableBumpedOutShell(const CResRef& cTexture, float fHeight);
    virtual int DisableBumpedOutShell();
    virtual int SetMotionBlur(int bBlur);
    virtual void UnloadModel();
    virtual void UnloadModel(unsigned char nModel);
    virtual int SetEnvMap(int nType, int nFlags, const CResRef& cEnvMap);
    virtual int SetEnvMap(unsigned char nModel, int nType, int nFlags,
                          const CResRef& cEnvMap);
    virtual int RestoreEnvMap();
    virtual void TurnOnShadows();
    virtual void TurnOffShadows();
    virtual void EnableDistortion();
    virtual void DisableDistortion();
    virtual void SetHeadItem(unsigned long nItemId)  // PC 0x00699AD0  Xbox 0x000C5670
    {
        NwcanimbaseheadTrace2();
        m_nHeadItem = nItemId;
    }

    unsigned short m_aParts[10];
    unsigned char m_nPartCount;
    unsigned char m_nPartStart;
    unsigned char m_aPad1A[2];
    CResRef m_cReplacedTexture;
    int m_bTextureReplaced;
    CResRef m_cEnvMap;
    int m_nEnvMapType;

    Gob* m_pHeadModel;
    unsigned long m_nHeadItem;
};

static inline void NwcanimbaseheadTrace2()
{
}

#endif
