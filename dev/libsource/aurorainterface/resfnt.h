#ifndef __RESFNT_H__
#define __RESFNT_H__

#include "exoresman.h"

#define RESTYPE_FNT 2005

#define FNT_FONT_INFO_OFFSET 24

class CResFNT : public CRes
{
public:
    CResFNT();
    virtual ~CResFNT();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    int GetTGAAttrib(unsigned long* pnWidth, unsigned long* pnHeight, unsigned char* pnDepth);
    unsigned char* GetTGADataPtr();
    unsigned long* GetFNTDataPtr();

    int ReadColorMappedRLETGA();
    int ReadUnmappedRLETGA();

    int m_bDataOwned;
    int m_bColorMapped;
    int m_bRLE;
    int m_bServiced;
    unsigned long m_nAllocSize;
    unsigned long m_nDataSize;
    unsigned long m_nRLESize;
    unsigned char* m_pColorMap;
    unsigned char* m_pFNTData;
    TGAHeader* m_pHeader;
    unsigned long* m_pFontInfo;
    unsigned char* m_pImageEnd;
};

#endif
