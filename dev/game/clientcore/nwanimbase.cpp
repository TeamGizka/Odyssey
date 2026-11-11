#include "nwcanimbase.h"
// PC 0x00698E00  Xbox 0x000C4900
CSWAnimBase::CSWAnimBase()
{
    m_afValues[0] = 0.0f;
    m_afValues[1] = 0.0f;
}
// PC 0x00698E20  Xbox 0x000C4920
CSWAnimBase::~CSWAnimBase()
{
}
// PC 0x00698E30  Xbox 0x000C4930
int CSWAnimBase::Set(int nValue, float fValue)
{
    switch (nValue)
    {
        case 0:
            m_afValues[0] = fValue;
            return 1;

        case 1:
            m_afValues[1] = fValue;
            return 1;

        case 2:
            m_afValues[2] = fValue;
            return 1;

        case 3:
            m_afValues[3] = fValue;
            return 1;

        default:
            return 0;
    }
}
