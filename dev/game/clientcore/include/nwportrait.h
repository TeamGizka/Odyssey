#ifndef __NWPORTRAIT_H__
#define __NWPORTRAIT_H__

class CResRef;
#include "exoresman.h"

const unsigned short PORTRAIT_ID_FIRST_RESERVED = 0xFFFE;

const unsigned short PORTRAIT_ID_RESERVED_FFFF = 0xFFFF;
const unsigned short PORTRAIT_ID_RESERVED_FFFE = 0xFFFE;

const unsigned short PORTRAIT_ID_DEFAULT_DOOR = 0x022E;

class CSWPortrait : public CResRef
{
public:
    CSWPortrait();
    ~CSWPortrait();
    CResRef GetPortrait(unsigned char nSize);
    int SetPortraitId(unsigned short nPortraitId);
};

#endif
