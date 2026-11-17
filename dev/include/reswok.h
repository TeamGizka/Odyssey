#ifndef __RESWOK_H__
#define __RESWOK_H__

#include "exobase.h"
#include "exoresman.h"

class CResWOK : public CRes
{
public:
    CResWOK();
    virtual ~CResWOK();

    unsigned char* GetWOKDataPtr();
    unsigned long GetWOKSize();
    int IsLoaded();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    int m_bServiced;
    unsigned long m_nWOKSize;
    unsigned char* m_pWOKData;
};

#endif
