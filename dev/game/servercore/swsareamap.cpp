#include "exobase.h"
#include "swsareamap.h"
// PC 0x00573A00
CSWSAreaMap::CSWSAreaMap()
{
    m_pnExplored = NULL;
    m_fScaleX = 0.0f;
    m_fScaleY = 0.0f;
    m_fOffsetX = 0.0f;
    m_fOffsetY = 0.0f;
    m_nExploredSize = 0;
    m_nWidth = 0;
    m_nHeight = 0;
    for (int i = 0; i < 3; i++)
    {
        m_nPartyMapX[i] = -1;
        m_nPartyMapY[i] = -1;
        m_fPartyOrientation[i] = 0.0f;
    }
}
// PC 0x00573A50
CSWSAreaMap::~CSWSAreaMap()
{
    if (m_pnExplored != NULL)
    {
        delete[] m_pnExplored;
        m_pnExplored = NULL;
        m_nExploredSize = 0;
    }
}
// PC 0x00573A80
void CSWSAreaMap::Initialize(int bEnabled, int nMapWidth, unsigned long nNorthAxis,
                             float fWorldX1, float fWorldY1, float fWorldX2,
                             float fWorldY2, int nMapX1, int nMapY1,
                             int nMapX2, int nMapY2, int nZoom)
{
    if (bEnabled != 0)
    {
        if (nMapWidth > 88 || nMapWidth == 0)
        {
            nMapWidth = 88;
        }
        m_nWidth = nMapWidth;
        int nMapHeight = (int)(nMapWidth * 0.58181816f);
        m_nHeight = nMapHeight;
        if (nMapHeight < 1)
        {
            nMapHeight = 1;
        }
        m_nOrientation = nNorthAxis;
        m_nHeight = nMapHeight;
        m_fInverseZoom = 1.0f / nZoom;
        if (m_pnExplored == NULL)
        {
            m_nExploredSize = ((nMapWidth + 1) * (nMapHeight + 1)) / 32 + 1;
            m_pnExplored = new unsigned long[m_nExploredSize];
            memset(m_pnExplored, 0, m_nExploredSize * 4);
            if (m_nOrientation == ORIENTATION_COUNTERCLOCKWISE)
            {
                float fTemp = fWorldX1;
                fWorldX1 = fWorldY1;
                fWorldY1 = fTemp;
                fWorldX1 *= -1.0f;
                fTemp = fWorldX2;
                fWorldX2 = fWorldY2;
                fWorldY2 = fTemp;
                fWorldX2 *= -1.0f;
            }
            if (m_nOrientation == ORIENTATION_HALF_TURN)
            {
                fWorldX1 = fWorldX1 * -1.0f;
                fWorldY1 = fWorldY1 * -1.0f;
                fWorldX2 = fWorldX2 * -1.0f;
                fWorldY2 = fWorldY2 * -1.0f;
            }
            if (m_nOrientation == ORIENTATION_CLOCKWISE)
            {
                float fTemp = fWorldX1;
                fWorldX1 = fWorldY1;
                fWorldY1 = fTemp * -1.0f;
                fTemp = fWorldX2;
                fWorldX2 = fWorldY2;
                fWorldY2 = fTemp * -1.0f;
            }
            m_fScaleX = (fWorldX2 - fWorldX1) / (nMapX2 - nMapX1);
            m_fScaleY = (fWorldY2 - fWorldY1) / (nMapY2 - nMapY1);
            m_fOffsetX = fWorldX1 - nMapX1 * m_fScaleX;
            m_fOffsetY = fWorldY1 - nMapY1 * m_fScaleY;
        }
#ifdef _XBOX
        else
        {
            EXOXBOXASSERT(0);
        }
#endif
    }
}
// PC 0x00573C90
int CSWSAreaMap::GetMapPixelFromWorldCoord(Vector vPosition, int& nMapX,
                                           int& nMapY)
{
    float fX = vPosition.m_fX;
    float fY = vPosition.m_fY;
    if (m_nOrientation == ORIENTATION_COUNTERCLOCKWISE)
    {
        fY = vPosition.m_fX;
        fX = vPosition.m_fY * -1.0f;
    }
    if (m_nOrientation == ORIENTATION_HALF_TURN)
    {
        fX = fX * -1.0f;
        fY = fY * -1.0f;
    }
    if (m_nOrientation == ORIENTATION_CLOCKWISE)
    {
        float fTemp = fX;
        fX = fY;
        fY = fTemp * -1.0f;
    }
    nMapX = (int)((fX - m_fOffsetX) / m_fScaleX + 0.5f);
    nMapY = (int)((fY - m_fOffsetY) / m_fScaleY + 0.5f);
    if (nMapX >= 0 && nMapX <= 440 && nMapY >= 0 && nMapY <= 256)
    {
        return 1;
    }
    nMapX = -1;
    nMapY = -1;
    return 0;
}
// PC 0x00573D60
float CSWSAreaMap::GetMapRotateCCWFromWorldOrientation(Vector vOrientation)
{
    float fAngle = Yaw(vOrientation);
    switch (m_nOrientation)
    {
    case 1:
        fAngle += 180.0f; break;
    case 2:
        fAngle += 90.0f; break;
    case 3:
        fAngle += 270.0f; break;
    }
    return fAngle;
}
// PC 0x00573DA0
int CSWSAreaMap::GetGridPixelFromMapPixel(int nMapX, int nMapY, int& nGridX,
                                          int& nGridY)
{
    if (nMapX <= 440 && nMapX >= 0 && nMapY <= 256 && nMapY >= 0)
    {
        float fPixelsPerRow = 256.0f / m_nHeight;
        nGridX = (int)(nMapX / (440.0f / m_nWidth) + 0.5f);
        nGridY = (int)(nMapY / fPixelsPerRow + 0.5f);
        return 1;
    }
    nGridX = -1;
    nGridY = -1;
    return 0;
}
// PC 0x00574040
void CSWSAreaMap::SetEntireMapExplored()
{
    if (m_pnExplored != NULL)
    {
        memset(m_pnExplored, 0xFF, m_nExploredSize * 4);
    }
}
// PC 0x00573C20
void CSWSAreaMap::LoadSavedAreaMapData(unsigned long* pData,
                                       unsigned long nSize, int nWidth,
                                       int nHeight)
{
    if (m_nExploredSize > 0 && nWidth == m_nWidth && nHeight == m_nHeight &&
        nSize == m_nExploredSize)
    {
        memcpy(m_pnExplored, pData, nSize * 4);
    }
}
// PC 0x00574060
void CSWSAreaMap::SetMapPointExplored(int nMapX, int nMapY)
{
    if (nMapX <= 440 && nMapX >= 0 && nMapY <= 256 && nMapY >= 0)
    {
        int nGridX = 0;
        int nGridY = 0;
        if (GetGridPixelFromMapPixel(nMapX, nMapY, nGridX, nGridY))
        {
            int nIndex = nGridX + nGridY * (m_nWidth + 1);
            if (nIndex / 32 <= (int)m_nExploredSize)
            {
                m_pnExplored[nIndex / 32] |= 1 << (nIndex % 32);
            }
        }
    }
}
// PC 0x00573C60
unsigned long* CSWSAreaMap::GetMapData(unsigned long& nSize, int& nWidth,
                                       int& nHeight)
{
    if (m_nExploredSize <= 0)
    {
        return NULL;
    }
    nSize = m_nExploredSize;
    nWidth = m_nWidth;
    nHeight = m_nHeight;
    return m_pnExplored;
}
// PC 0x00573E30
void CSWSAreaMap::SetPartyMemberWorldLocation(int nMember, Vector vPosition)
{
    if (m_nExploredSize > 0 && nMember < 3)
    {
        int nMapX = 0;
        int nMapY = 0;
        if (GetMapPixelFromWorldCoord(vPosition, nMapX, nMapY))
        {
            m_nPartyMapX[nMember] = nMapX;
            m_nPartyMapY[nMember] = nMapY;
        }
    }
}
// PC 0x00573EA0
void CSWSAreaMap::SetPartyMemberWorldOrientation(int nMember,
                                                 Vector vOrientation)
{
    if (m_nExploredSize > 0 && nMember < 3)
    {
        m_fPartyOrientation[nMember] =
            GetMapRotateCCWFromWorldOrientation(vOrientation);
    }
}
// PC 0x00573F20
int CSWSAreaMap::GetPartyMemberMapLocation(int nMember, int& nMapX, int& nMapY)
{
    if (m_nExploredSize <= 0)
    {
        return 0;
    }
    if (nMember >= 3)
    {
        return 0;
    }
    int nX = m_nPartyMapX[nMember];
    if (nX == -1)
    {
        return 0;
    }
    nMapX = nX;
    nMapY = m_nPartyMapY[nMember];
    return 1;
}
// PC 0x00573F60
float CSWSAreaMap::GetPartyMemberMapOrientation(int nMember)
{
    if (m_nExploredSize <= 0)
    {
        return 0.0f;
    }
    if (nMember >= 3)
    {
        return 0.0f;
    }
    return m_fPartyOrientation[nMember];
}
// PC 0x00573F80
int CSWSAreaMap::IsWorldPointExplored(Vector vPosition)
{
    unsigned long nSize = m_nExploredSize;
    if (!(nSize > 0))
    {
        return 0;
    }
    int nMapX;
    int nMapY;
    int nGridX;
    int nGridY = 0;
    if (!GetMapPixelFromWorldCoord(vPosition, nMapX, nMapY))
    {
        return 0;
    }
    if (!GetGridPixelFromMapPixel(nMapX, nMapY, nGridX, nGridY))
    {
        return 0;
    }
    int nIndex = nGridX + nGridY * (m_nWidth + 1);
    if (nIndex / 32 > (int)nSize)
    {
        return 0;
    }
    if ((m_pnExplored[nIndex / 32] & (1 << (nIndex % 32))) == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
// PC 0x005740F0
void CSWSAreaMap::SetWorldPointExplored(Vector vPosition, int nRadius)
{
    if (m_nExploredSize > 0)
    {
        int nMapX = 0;
        int nMapY = 0;
        if (GetMapPixelFromWorldCoord(vPosition, nMapX, nMapY))
        {
            for (int i = -nRadius; i <= nRadius; i++)
            {
                int nSpan = nRadius - abs(i);
                for (int j = -nSpan; j <= nSpan; j++)
                {
                    int nX = nMapX + i * (440 / m_nWidth);
                    int nY = nMapY + j * (256 / m_nHeight);
                    if (nX < 0)
                    {
                        nX = 0;
                    }
                    if (nY < 0)
                    {
                        nY = 0;
                    }
                    if (nX > 440)
                    {
                        nX = 440;
                    }
                    if (nY > 256)
                    {
                        nY = 256;
                    }
                    SetMapPointExplored(nX, nY);
                }
            }
        }
    }
}
