#ifndef __RESARE_H__
#define __RESARE_H__

#include "resgff.h"

class CResARE : public CResGFF
{
public:
    CResARE()  // PC 0x004DF850 GOG 0x004DF970
    {
        strncpy(m_szFileType, "ARE ", 4);
    }
};

#endif
