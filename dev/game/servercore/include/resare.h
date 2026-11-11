#ifndef __RESARE_H__
#define __RESARE_H__

#include "resgff.h"

class CResARE : public CResGFF
{
public:
    CResARE()  // PC 0x00504830  Xbox 0x000BBB70
    {
        strncpy(m_szFileType, "ARE ", 4);
    }
};

#endif
