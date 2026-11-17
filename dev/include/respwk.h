#ifndef __RESPWK_H__
#define __RESPWK_H__

#include "exobase.h"
#include "exoresman.h"

class CResPWK : public CRes
{
public:
    CResPWK();
    virtual ~CResPWK();

    unsigned char* GetPWKDataPtr();
    unsigned long GetPWKSize();
    int IsLoaded();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    int m_bServiced;
    unsigned long m_nPWKSize;
    unsigned char* m_pPWKData;
};

#endif
