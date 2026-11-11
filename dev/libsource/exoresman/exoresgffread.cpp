#include "resgff.h"

unsigned long CResGFFFieldIDHash::m_nCRC32[256];
// PC 0x00411690
void CResGFFFieldIDHash::Initialize()
{
    unsigned int i;
    unsigned long nCRC;
    int j;

    for (i = 0; i <= 0xFF; i++)
    {
        nCRC = i;
        for (j = 8; j != 0; j--)
        {
            if ((nCRC & 1) != 0)
            {
                nCRC = (nCRC >> 1) ^ 0xEDB88320;
            }
            else
            {
                nCRC = nCRC >> 1;
            }
        }
        m_nCRC32[i] = nCRC;
    }
}
// PC 0x004116D0
unsigned long CResGFFFieldIDHash::Hash(const char *szLabel, unsigned long nLength)
{
    unsigned long nCRC = 0;
    unsigned long i = 0;
    char ch;

    if (i < nLength)
    {
        do
        {
            ch = szLabel[i];
            if (ch == '\0')
            {
                break;
            }
            i++;
            nCRC = m_nCRC32[(nCRC ^ ch) & 0xFF] ^ (nCRC >> 8);
        } while (i < nLength);
    }

    return nCRC;
}
