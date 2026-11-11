#include "exoresman.h"

#include <string.h>
#include <stdio.h>

#include "math3d.h"
// PC 0x005A4C50
int CLYT::GetRoomCount()
{
    return m_nRoomCount;
}
// PC 0x005D64F0  Xbox 0x00148C30
int CLYT::GetTrackCount()
{
    return m_nTrackCount;
}
// PC 0x00710E50
int CLYT::GetObstacleCount()
{
    return m_nObstacleCount;
}
// PC 0x005DE450  Xbox 0x001498C0
int CLYT::UnloadLayout()
{
    int nResult = m_bLayoutLoaded;
    if (nResult != 0)
    {
        nResult = m_pRes->Release();
        m_bLayoutLoaded = 0;
    }
    return nResult;
}
// PC 0x005DE470  Xbox 0x00148B70
CResRef CLYT::GetRoomName(int nRoom)
{
    return m_pRoomNames[nRoom];
}
// PC 0x005DE4A0  Xbox 0x00148BA0
Vector CLYT::GetRoomPosition(int nRoom)
{
    return m_pRoomPositions[nRoom];
}
// PC 0x005DE4D0  Xbox 0x00148BD0
CResRef CLYT::GetTrackName(int nTrack)
{
    return m_pTrackNames[nTrack];
}
// PC 0x005DE500  Xbox 0x00148C00
Vector CLYT::GetTrackPosition(int nTrack)
{
    return m_pTrackPositions[nTrack];
}
// PC 0x005DE530  Xbox 0x00148C40
CResRef CLYT::GetObstacleName(int nObstacle)
{
    return m_pObstacleNames[nObstacle];
}
// PC 0x005DE560  Xbox 0x00148C70
Vector CLYT::GetObstaclePosition(int nObstacle)
{
    return m_pObstaclePositions[nObstacle];
}
// PC 0x005DE770  Xbox 0x00148E80
CLYT::CLYT()
{
    m_nRoomCount = 0;
    m_pRoomNames = 0;
    m_pRoomPositions = 0;
    m_nTrackCount = 0;
    m_pTrackNames = 0;
    m_pTrackPositions = 0;
    m_nObstacleCount = 0;
    m_pObstacleNames = 0;
    m_pObstaclePositions = 0;
    m_nDoorHookCount = 0;
    m_pDoorHookNames = 0;
    m_pDoorHookTargets = 0;
    m_pDoorHookPositions = 0;
    m_pDoorHookOrientations = 0;
    m_pDoorHookIds = 0;
}
// PC 0x005DE7E0  Xbox 0x00148EF0
CLYT::~CLYT()
{
    UnloadLayout();
    delete[] m_pRoomNames;
    m_pRoomNames = 0;
    delete[] m_pRoomPositions;
    m_pRoomPositions = 0;
    delete[] m_pTrackNames;
    m_pTrackNames = 0;
    delete[] m_pTrackPositions;
    m_pTrackPositions = 0;
    delete[] m_pObstacleNames;
    m_pObstacleNames = 0;
    delete[] m_pObstaclePositions;
    m_pObstaclePositions = 0;
    delete[] m_pDoorHookNames;
    m_pDoorHookNames = 0;
    delete[] m_pDoorHookTargets;
    m_pDoorHookTargets = 0;
    delete[] m_pDoorHookPositions;
    m_pDoorHookPositions = 0;
    delete[] m_pDoorHookOrientations;
    m_pDoorHookOrientations = 0;
    delete[] m_pDoorHookIds;
    m_pDoorHookIds = 0;
}
// PC 0x005DF140  Xbox 0x00149850
int CLYT::LoadLayout(const CResRef& cResRef)
{
    CExoString sLayout;
    cResRef.CopyToString(sLayout);
    return LoadLayout(sLayout);
}
// PC 0x005DE900  Xbox 0x00149010
int CLYT::LoadLayout(const CExoString& sLayout)
{
    char* pBuffer;
    char* pLine;
    char szRoomName[20];
    int i;
    {
        CResRef cResRef(sLayout);
        SetResRef(cResRef, 0);
    }

    if (m_pRes->Demand() == 0)
    {
        return 0;
    }

    unsigned char* pData = m_pRes->GetLYTDataPtr();
    unsigned int nSize = m_pRes->GetLYTSize();
    pBuffer = new char[nSize + 4];
    pLine = new char[nSize + 4];
    memset(pBuffer, 0, nSize + 4);
    memcpy(pBuffer, pData, nSize);

    char* pCurrent = pBuffer;
    int bFoundLayout = 0;
    sscanf(pBuffer, "%[^\r\n]", pLine);
    do
    {
        if (strcmp("beginlayout", pLine) == 0)
        {
            bFoundLayout = 1;
        }
        pCurrent += strlen(pLine) + 2;
        sscanf(pCurrent, "%[^\r\n]", pLine);
    } while (bFoundLayout == 0);

    sscanf(pLine, "%*s%d", &m_nRoomCount);
    pCurrent += strlen(pLine) + 2;
    if (m_nRoomCount > 0)
    {
        m_pRoomNames = new CResRef[m_nRoomCount];
        m_pRoomPositions = new Vector[m_nRoomCount];
        for (i = 0; i < m_nRoomCount; i++)
        {
            sscanf(pCurrent, "%[^\r\n]", pLine);
            sscanf(pLine, "%s%f%f%f", szRoomName, &m_pRoomPositions[i].m_fX,
                   &m_pRoomPositions[i].m_fY, &m_pRoomPositions[i].m_fZ);
            m_pRoomNames[i] = szRoomName;
            pCurrent += strlen(pLine) + 2;
        }
    }

    sscanf(pCurrent, "%[^\r\n]", pLine);
    if (strcmp(pLine, "donelayout") == 0)
    {
        m_bLayoutLoaded = 1;
        delete[] pLine;
        delete[] pBuffer;
        return m_bLayoutLoaded;
    }
    sscanf(pLine, "%*s%d", &m_nTrackCount);
    pCurrent += strlen(pLine) + 2;
    if (m_nTrackCount > 0)
    {
        m_pTrackNames = new CResRef[m_nTrackCount];
        m_pTrackPositions = new Vector[m_nTrackCount];
        for (i = 0; i < m_nTrackCount; i++)
        {
            sscanf(pCurrent, "%[^\r\n]", pLine);
            sscanf(pLine, "%s%f%f%f", szRoomName,
                   &m_pTrackPositions[i].m_fX, &m_pTrackPositions[i].m_fY,
                   &m_pTrackPositions[i].m_fZ);
            m_pTrackNames[i] = szRoomName;
            pCurrent += strlen(pLine) + 2;
        }
    }

    sscanf(pCurrent, "%[^\r\n]", pLine);
    if (strcmp(pLine, "donelayout") == 0)
    {
        m_bLayoutLoaded = 1;
        delete[] pLine;
        delete[] pBuffer;
        return m_bLayoutLoaded;
    }
    sscanf(pLine, "%*s%d", &m_nObstacleCount);
    pCurrent += strlen(pLine) + 2;
    if (m_nObstacleCount > 0)
    {
        m_pObstacleNames = new CResRef[m_nObstacleCount];
        m_pObstaclePositions = new Vector[m_nObstacleCount];
        for (i = 0; i < m_nObstacleCount; i++)
        {
            sscanf(pCurrent, "%[^\r\n]", pLine);
            sscanf(pLine, "%s%f%f%f", szRoomName,
                   &m_pObstaclePositions[i].m_fX,
                   &m_pObstaclePositions[i].m_fY,
                   &m_pObstaclePositions[i].m_fZ);
            m_pObstacleNames[i] = szRoomName;
            pCurrent += strlen(pLine) + 2;
        }
    }

    sscanf(pCurrent, "%[^\r\n]", pLine);
    if (strcmp(pLine, "donelayout") == 0)
    {
        m_bLayoutLoaded = 1;
        delete[] pLine;
        delete[] pBuffer;
        return m_bLayoutLoaded;
    }
    sscanf(pLine, "%*s%d", &m_nDoorHookCount);
    pCurrent += strlen(pLine) + 2;
    if (m_nDoorHookCount > 0)
    {
        char szDoorHookName[20];
        char szDoorHookTarget[20];
        m_pDoorHookNames = new CExoString[m_nDoorHookCount];
        m_pDoorHookTargets = new CExoString[m_nDoorHookCount];
        m_pDoorHookPositions = new Vector[m_nDoorHookCount];
        m_pDoorHookOrientations = new Quaternion[m_nDoorHookCount];
        m_pDoorHookIds = new int[m_nDoorHookCount];
        for (i = 0; i < m_nDoorHookCount; i++)
        {
            sscanf(pCurrent, "%[^\r\n]", pLine);
            sscanf(pLine, "%s%s%d%f%f%f%f%f%f%f", szDoorHookName,
                   szDoorHookTarget, &m_pDoorHookIds[i],
                   &m_pDoorHookPositions[i].m_fX,
                   &m_pDoorHookPositions[i].m_fY,
                   &m_pDoorHookPositions[i].m_fZ,
                   &m_pDoorHookOrientations[i].m_fW,
                   &m_pDoorHookOrientations[i].m_fX,
                   &m_pDoorHookOrientations[i].m_fY,
                   &m_pDoorHookOrientations[i].m_fZ);
            m_pDoorHookNames[i] = szDoorHookName;
            m_pDoorHookTargets[i] = szDoorHookTarget;
            pCurrent += strlen(pLine) + 2;
        }
    }
    m_bLayoutLoaded = 1;
    delete[] pLine;
    delete[] pBuffer;
    return m_bLayoutLoaded;
}
