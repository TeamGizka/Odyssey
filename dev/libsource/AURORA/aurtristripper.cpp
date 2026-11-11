#include <stdio.h>
#include <set>

#include "aurlist.h"
#include "aurtristripper.h"

const unsigned int nStripSplitThreshold = 1000;
// PC 0x0047BB80  Xbox 0x002D2270
void NvStripifier::UpdateCacheFace(VertexCache* pCache, NvFaceInfo* pFace)
{
    if (!pCache->InCache(pFace->m_nVertex0))
    {
        pCache->AddEntry(pFace->m_nVertex0);
    }
    if (!pCache->InCache(pFace->m_nVertex1))
    {
        pCache->AddEntry(pFace->m_nVertex1);
    }
    if (!pCache->InCache(pFace->m_nVertex2))
    {
        pCache->AddEntry(pFace->m_nVertex2);
    }
}
// PC 0x0047C230  Xbox 0x002D2C20
NvEdgeInfo* NvStripifier::FindEdgeInfo(NvEdgeInfoList& lstEdges, int nVertex0,
                                       int nVertex1)
{
    NvEdgeInfo* pEdge = lstEdges[nVertex0];

    while (pEdge != 0)
    {
        if (pEdge->m_nVertex0 == nVertex0)
        {
            if (pEdge->m_nVertex1 == nVertex1)
            {
                return pEdge;
            }

            pEdge = pEdge->m_pNextVertex0;
        }
        else
        {
            if (pEdge->m_nVertex0 == nVertex1)
            {
                return pEdge;
            }

            pEdge = pEdge->m_pNextVertex1;
        }
    }

    return 0;
}
// PC 0x0047C270  Xbox 0x002D2C90
bool NvStripifier::AlreadyExists(NvFaceInfo* pFace, NvFaceInfoList& lstFaces)
{
    for (unsigned int i = 0; i < lstFaces.size(); i++)
    {
        NvFaceInfo* pOther = lstFaces[i];
        if (pOther->m_nVertex0 == pFace->m_nVertex0 &&
            pOther->m_nVertex1 == pFace->m_nVertex1 &&
            pOther->m_nVertex2 == pFace->m_nVertex2)
        {
            return true;
        }
    }

    return false;
}
// PC 0x0047C2C0
int NvStripifier::FindStartPoint(NvFaceInfoList& lstFaces,
                                 NvEdgeInfoList& lstEdges)
{
    for (int i = 0; i < lstFaces.size(); i++)
    {
        NvFaceInfo* pFace = lstFaces[i];
        int nCount = 0;

        NvEdgeInfo* pEdge =
            FindEdgeInfo(lstEdges, pFace->m_nVertex0, pFace->m_nVertex1);
        NvFaceInfo* pOther =
            (pEdge->m_pFace0 == pFace ? pEdge->m_pFace1 : pEdge->m_pFace0);
        if (pOther == 0)
        {
            nCount++;
        }

        pEdge = FindEdgeInfo(lstEdges, pFace->m_nVertex1, pFace->m_nVertex2);
        pOther = (pEdge->m_pFace0 == pFace ? pEdge->m_pFace1 : pEdge->m_pFace0);
        if (pOther == 0)
        {
            nCount++;
        }

        pEdge = FindEdgeInfo(lstEdges, pFace->m_nVertex2, pFace->m_nVertex0);
        pOther = (pEdge->m_pFace0 == pFace ? pEdge->m_pFace1 : pEdge->m_pFace0);
        if (pOther == 0)
        {
            nCount++;
        }

        if (nCount > 1)
        {
            return i;
        }
    }

    return -1;
}
// PC 0x0047BC40  Xbox 0x002D2330
int NvStripifier::CalcNumHitsFace(VertexCache* pCache, NvFaceInfo* pFace)
{
    int nNumHits = 0;
    if (pCache->InCache(pFace->m_nVertex0))
    {
        nNumHits++;
    }
    if (pCache->InCache(pFace->m_nVertex1))
    {
        nNumHits++;
    }
    if (pCache->InCache(pFace->m_nVertex2))
    {
        nNumHits++;
    }
    return nNumHits;
}
// PC 0x0047C400  Xbox 0x002D2DB0
NvFaceInfo* NvStripifier::FindGoodResetPoint(NvFaceInfoList& lstFaces,
                                           NvEdgeInfoList& lstEdges)
{
    NvFaceInfo* pResult = 0;
    int nNumFaces = lstFaces.size();
    int nStartPoint;
    if (m_bFirstTimeResetPoint)
    {
        nStartPoint = FindStartPoint(lstFaces, lstEdges);
        m_bFirstTimeResetPoint = false;
    }
    else
    {
        nStartPoint = (int)((nNumFaces - 1.0f) * m_fMeshJump);
    }
    if (nStartPoint == -1)
    {
        nStartPoint = (int)((nNumFaces - 1.0f) * m_fMeshJump);
    }
    int nIndex = nStartPoint;
    do
    {
        if (lstFaces[nIndex]->m_nStripId < 0)
        {
            pResult = lstFaces[nIndex];
            break;
        }
        if (++nIndex >= nNumFaces)
        {
            nIndex = 0;
        }
    }
    while (nIndex != nStartPoint);
    m_fMeshJump += 0.1f;
    if (m_fMeshJump > 1.0f)
    {
        m_fMeshJump = 0.05f;
    }
    return pResult;
}
// PC 0x0047C4C0
inline int NvStripifier::GetNextIndex(const NvIndexList& lstIndices, NvFaceInfo* pFace)
{
    int nNumIndices = lstIndices.size();
    int nVertex0 = lstIndices[nNumIndices - 2];
    int nVertex1 = lstIndices[nNumIndices - 1];

    int nFaceVertex0 = pFace->m_nVertex0;
    int nFaceVertex1 = pFace->m_nVertex1;
    int nFaceVertex2 = pFace->m_nVertex2;

    if (nFaceVertex0 != nVertex0 && nFaceVertex0 != nVertex1)
    {
        if ((nFaceVertex1 != nVertex0 && nFaceVertex1 != nVertex1) ||
            (nFaceVertex2 != nVertex0 && nFaceVertex2 != nVertex1))
        {
            printf("GetNextIndex: Triangle doesn't have all of its vertices\n");
            printf(
                "GetNextIndex: Duplicate triangle probably got us derailed\n");
        }

        return nFaceVertex0;
    }

    if (nFaceVertex1 != nVertex0 && nFaceVertex1 != nVertex1)
    {
        if ((nFaceVertex0 != nVertex0 && nFaceVertex0 != nVertex1) ||
            (nFaceVertex2 != nVertex0 && nFaceVertex2 != nVertex1))
        {
            printf("GetNextIndex: Triangle doesn't have all of its vertices\n");
            printf(
                "GetNextIndex: Duplicate triangle probably got us derailed\n");
        }

        return nFaceVertex1;
    }

    if (nFaceVertex2 != nVertex0 && nFaceVertex2 != nVertex1)
    {
        if ((nFaceVertex0 != nVertex0 && nFaceVertex0 != nVertex1) ||
            (nFaceVertex1 != nVertex0 && nFaceVertex1 != nVertex1))
        {
            printf("GetNextIndex: Triangle doesn't have all of its vertices\n");
            printf(
                "GetNextIndex: Duplicate triangle probably got us derailed\n");
        }

        return nFaceVertex2;
    }

    printf("GetNextIndex: Duplicate triangle sent\n");
    return -1;
}
// PC 0x0047C5B0  Xbox 0x002D2EE0
void NvStripifier::UpdateCacheStrip(VertexCache* pCache, NvStripInfo* pStrip)
{
    for (unsigned int i = 0; i < pStrip->m_lstFaces.size(); i++)
    {
        if (!pCache->InCache(pStrip->m_lstFaces[i]->m_nVertex0))
        {
            pCache->AddEntry(pStrip->m_lstFaces[i]->m_nVertex0);
        }
        if (!pCache->InCache(pStrip->m_lstFaces[i]->m_nVertex1))
        {
            pCache->AddEntry(pStrip->m_lstFaces[i]->m_nVertex1);
        }
        if (!pCache->InCache(pStrip->m_lstFaces[i]->m_nVertex2))
        {
            pCache->AddEntry(pStrip->m_lstFaces[i]->m_nVertex2);
        }
    }
}
// PC 0x0047C6D0  Xbox 0x002D3010
float NvStripifier::CalcNumHitsStrip(VertexCache* pCache, NvStripInfo* pStrip)
{
    int nNumHits = 0;
    int nNumFaces = 0;
    for (unsigned int i = 0; i < pStrip->m_lstFaces.size(); i++)
    {
        if (pCache->InCache(pStrip->m_lstFaces[i]->m_nVertex0))
        {
            nNumHits++;
        }
        if (pCache->InCache(pStrip->m_lstFaces[i]->m_nVertex1))
        {
            nNumHits++;
        }
        if (pCache->InCache(pStrip->m_lstFaces[i]->m_nVertex2))
        {
            nNumHits++;
        }
        nNumFaces++;
    }
    return (float)nNumHits / (float)nNumFaces;
}
// PC 0x0047C7A0  Xbox 0x002D30F0
int NvStripifier::NumNeighbors(NvFaceInfo* pFace, NvEdgeInfoList& lstEdges)
{
    int nNumNeighbors = 0;
    NvEdgeInfo* pEdge;
    NvFaceInfo* pOther;

    pEdge = FindEdgeInfo(lstEdges, pFace->m_nVertex0, pFace->m_nVertex1);
    pOther = (pEdge->m_pFace0 == pFace ? pEdge->m_pFace1 : pEdge->m_pFace0);
    if (pOther != 0)
    {
        nNumNeighbors++;
    }

    pEdge = FindEdgeInfo(lstEdges, pFace->m_nVertex1, pFace->m_nVertex2);
    pOther = (pEdge->m_pFace0 == pFace ? pEdge->m_pFace1 : pEdge->m_pFace0);
    if (pOther != 0)
    {
        nNumNeighbors++;
    }

    pEdge = FindEdgeInfo(lstEdges, pFace->m_nVertex2, pFace->m_nVertex0);
    pOther = (pEdge->m_pFace0 == pFace ? pEdge->m_pFace1 : pEdge->m_pFace0);
    if (pOther != 0)
    {
        nNumNeighbors++;
    }

    return nNumNeighbors;
}
// PC 0x0047C830  Xbox 0x002D2A20
float NvStripifier::AvgStripSize(const NvStripInfoList& lstStrips)
{
    int nSizeAccum = 0;
    int nNumStrips = lstStrips.size();

    for (int i = 0; i < nNumStrips; i++)
    {
        NvStripInfo* pStrip = lstStrips[i];
        nSizeAccum += pStrip->m_lstFaces.size();
    }

    return ((float)nSizeAccum) / ((float)nNumStrips);
}

#include <aurlist.h>
// PC 0x0047C8A0  Xbox 0x002D3160
bool NvStripInfo::Unique(NvFaceInfoList& lstFaces, NvFaceInfo* pFace)
{
    bool bVertex0, bVertex1, bVertex2;
    bVertex0 = bVertex1 = bVertex2 = false;
    for (unsigned int i = 0; i < lstFaces.size(); i++)
    {
        if (!bVertex0)
        {
            if (lstFaces[i]->m_nVertex0 == pFace->m_nVertex0 ||
                lstFaces[i]->m_nVertex1 == pFace->m_nVertex0 ||
                lstFaces[i]->m_nVertex2 == pFace->m_nVertex0)
            {
                bVertex0 = true;
            }
        }
        if (!bVertex1)
        {
            if (lstFaces[i]->m_nVertex0 == pFace->m_nVertex1 ||
                lstFaces[i]->m_nVertex1 == pFace->m_nVertex1 ||
                lstFaces[i]->m_nVertex2 == pFace->m_nVertex1)
            {
                bVertex1 = true;
            }
        }
        if (!bVertex2)
        {
            if (lstFaces[i]->m_nVertex0 == pFace->m_nVertex2 ||
                lstFaces[i]->m_nVertex1 == pFace->m_nVertex2 ||
                lstFaces[i]->m_nVertex2 == pFace->m_nVertex2)
            {
                bVertex2 = true;
            }
        }
        if (bVertex0 && bVertex1 && bVertex2)
        {
            return false;
        }
    }
    return true;
}
// PC 0x0047CCE0
bool NvStripifier::FindTraversal(NvFaceInfoList& lstFaces,
                                 NvEdgeInfoList& lstEdges,
                                 NvStripInfo* pStrip,
                                 NvStripStartInfo& startInfo)
{
    int nVertex = (pStrip->m_startInfo.m_bToVertex1
                       ? pStrip->m_startInfo.m_pStartEdge->m_nVertex1
                       : pStrip->m_startInfo.m_pStartEdge->m_nVertex0);
    NvFaceInfo* pUntouchedFace = 0;
    NvEdgeInfo* pEdgeIter = lstEdges[nVertex];
    while (pEdgeIter != 0)
    {
        NvFaceInfo* pFace0 = pEdgeIter->m_pFace0;
        NvFaceInfo* pFace1 = pEdgeIter->m_pFace1;
        if ((pFace0 != 0 && !pStrip->IsInStrip(pFace0)) && pFace1 != 0 &&
            !pStrip->IsMarked(pFace1))
        {
            pUntouchedFace = pFace1;
            break;
        }
        if ((pFace1 != 0 && !pStrip->IsInStrip(pFace1)) && pFace0 != 0 &&
            !pStrip->IsMarked(pFace0))
        {
            pUntouchedFace = pFace0;
            break;
        }
        pEdgeIter = (pEdgeIter->m_nVertex0 == nVertex ? pEdgeIter->m_pNextVertex0
                                                      : pEdgeIter->m_pNextVertex1);
    }

    startInfo.m_pStartFace = pUntouchedFace;
    startInfo.m_pStartEdge = pEdgeIter;
    if (pEdgeIter != 0)
    {
        if (pStrip->SharesEdge(startInfo.m_pStartFace, lstEdges))
        {
            startInfo.m_bToVertex1 = (pEdgeIter->m_nVertex0 == nVertex);
        }
        else
        {
            startInfo.m_bToVertex1 = (pEdgeIter->m_nVertex1 == nVertex);
        }
    }
    return (startInfo.m_pStartFace != 0);
}
// PC 0x0047C960  Xbox 0x002D3220
bool NvStripInfo::SharesEdge(const NvFaceInfo* pFace, NvEdgeInfoList& lstEdges)
{
    int nVertex1 = pFace->m_nVertex1;
    int nVertex0 = pFace->m_nVertex0;
    NvEdgeInfo* pEdge = NvStripifier::FindEdgeInfo(lstEdges, nVertex0, nVertex1);
    if (IsInStrip(pEdge->m_pFace0) || IsInStrip(pEdge->m_pFace1))
    {
        return true;
    }
    int nVertex2 = pFace->m_nVertex2;
    pEdge = NvStripifier::FindEdgeInfo(lstEdges, nVertex1, nVertex2);
    if (IsInStrip(pEdge->m_pFace0) || IsInStrip(pEdge->m_pFace1))
    {
        return true;
    }
    pEdge = NvStripifier::FindEdgeInfo(lstEdges, nVertex2, nVertex0);
    if (IsInStrip(pEdge->m_pFace0) || IsInStrip(pEdge->m_pFace1))
    {
        return true;
    }
    return false;
}
// PC 0x0047E720  Xbox 0x002D5160
void NvStripifier::CreateStrips(NvStripInfoList& lstStrips,
                                NvIndexList& lstIndices,
                                NvIndexList& lstStripLengths)
{
    int nNumStrips = lstStrips.size();
    if (nNumStrips != 0)
    {
        lstStripLengths.resize(nNumStrips);

        int nStartingLoc = 0;
        for (int s = 0; s < nNumStrips; s++)
        {
            NvStripInfo* pStrip = lstStrips[s];
            int nStripFaceCount = pStrip->m_lstFaces.size();

            NvFaceInfo firstFace(pStrip->m_lstFaces[0]->m_nVertex0,
                                 pStrip->m_lstFaces[0]->m_nVertex1,
                                 pStrip->m_lstFaces[0]->m_nVertex2);

            if (nStripFaceCount > 1)
            {
                int nUnique =
                    GetUniqueVertexInB(pStrip->m_lstFaces[1], &firstFace);
                if (nUnique == firstFace.m_nVertex1)
                {
                    int nTemp = firstFace.m_nVertex0;
                    firstFace.m_nVertex0 = firstFace.m_nVertex1;
                    firstFace.m_nVertex1 = nTemp;
                }
                else if (nUnique == firstFace.m_nVertex2)
                {
                    int nTemp = firstFace.m_nVertex0;
                    firstFace.m_nVertex0 = firstFace.m_nVertex2;
                    firstFace.m_nVertex2 = nTemp;
                }

                if (nStripFaceCount > 2)
                {
                    int nShared =
                        GetSharedVertex(pStrip->m_lstFaces[2], &firstFace);
                    if (nShared == firstFace.m_nVertex1)
                    {
                        int nTemp = firstFace.m_nVertex1;
                        firstFace.m_nVertex1 = firstFace.m_nVertex2;
                        firstFace.m_nVertex2 = nTemp;
                    }
                }
            }

            if (!IsCW(pStrip->m_lstFaces[0], firstFace.m_nVertex0,
                      firstFace.m_nVertex1))
            {
                lstIndices.push_back(firstFace.m_nVertex0);
            }

            lstIndices.push_back(firstFace.m_nVertex0);
            lstIndices.push_back(firstFace.m_nVertex1);
            lstIndices.push_back(firstFace.m_nVertex2);

            NvFaceInfo lastFace = firstFace;
            for (int j = 1; j < nStripFaceCount; j++)
            {
                int nUnique =
                    GetUniqueVertexInB(&lastFace, pStrip->m_lstFaces[j]);
                if (nUnique != -1)
                {
                    lstIndices.push_back(nUnique);
                    int nTemp = lastFace.m_nVertex0;
                    lastFace.m_nVertex0 = lastFace.m_nVertex1;
                    lastFace.m_nVertex1 = lastFace.m_nVertex2;
                    lastFace.m_nVertex2 = nUnique;
                }
            }

            lstStripLengths[s] = lstIndices.size() - nStartingLoc;
            nStartingLoc = lstIndices.size();
        }
    }
}

bool bNonManifoldFlag;
// PC 0x0047ED60  Xbox 0x002D49C0
void NvStripifier::BuildStripifyInfo(NvFaceInfoList& lstFaces,
                                    NvEdgeInfoList& lstEdges)
{
    int nNumIndices = m_lstIndices.size();
    lstFaces.reserve(nNumIndices);
    lstEdges.resize(nNumIndices);
    for (int i = 0; i < nNumIndices; i++)
    {
        lstEdges[i] = 0;
    }

    int nNumFaces = nNumIndices / 3;
    int nIndex = 0;
    for (int nFace = 0; nFace < nNumFaces; nFace++)
    {
        int nVertex0 = m_lstIndices[nIndex];
        nIndex++;
        int nVertex1 = m_lstIndices[nIndex];
        nIndex++;
        int nVertex2 = m_lstIndices[nIndex];
        nIndex++;
        NvFaceInfo* pFace = new NvFaceInfo(nVertex0, nVertex1, nVertex2);
        if (!AlreadyExists(pFace, lstFaces))
        {
            lstFaces.push_back(pFace);
            NvEdgeInfo* pEdge = FindEdgeInfo(lstEdges, nVertex0, nVertex1);
            if (pEdge == 0)
            {
                pEdge = new NvEdgeInfo(nVertex0, nVertex1);
                pEdge->m_pNextVertex0 = lstEdges[nVertex0];
                pEdge->m_pNextVertex1 = lstEdges[nVertex1];
                lstEdges[nVertex0] = pEdge;
                lstEdges[nVertex1] = pEdge;
                pEdge->m_pFace0 = pFace;
            }
            else
            {
                if (pEdge->m_pFace1 != 0)
                {
                    printf("BuildStripifyInfo: > 2 triangles on an edge... uncertain consequences\n");
                    bNonManifoldFlag = true;
                }
                else
                {
                    pEdge->m_pFace1 = pFace;
                }
            }
            pEdge = FindEdgeInfo(lstEdges, nVertex1, nVertex2);
            if (pEdge == 0)
            {
                pEdge = new NvEdgeInfo(nVertex1, nVertex2);
                pEdge->m_pNextVertex0 = lstEdges[nVertex1];
                pEdge->m_pNextVertex1 = lstEdges[nVertex2];
                lstEdges[nVertex1] = pEdge;
                lstEdges[nVertex2] = pEdge;
                pEdge->m_pFace0 = pFace;
            }
            else
            {
                if (pEdge->m_pFace1 != 0)
                {
                    printf("BuildStripifyInfo: > 2 triangles on an edge... uncertain consequences\n");
                }
                else
                {
                    pEdge->m_pFace1 = pFace;
                }
            }
            pEdge = FindEdgeInfo(lstEdges, nVertex2, nVertex0);
            if (pEdge == 0)
            {
                pEdge = new NvEdgeInfo(nVertex2, nVertex0);
                pEdge->m_pNextVertex0 = lstEdges[nVertex2];
                pEdge->m_pNextVertex1 = lstEdges[nVertex0];
                lstEdges[nVertex2] = pEdge;
                lstEdges[nVertex0] = pEdge;
                pEdge->m_pFace0 = pFace;
            }
            else
            {
                if (pEdge->m_pFace1 != 0)
                {
                    printf("BuildStripifyInfo: > 2 triangles on an edge... uncertain consequences\n");
                }
                else
                {
                    pEdge->m_pFace1 = pFace;
                }
            }
        }
    }
}
// PC 0x0047F0D0  Xbox 0x002D4CA0
void NvStripifier::CommitStrips(NvStripInfoList& lstStrips,
                                const NvStripInfoList& lstNewStrips,
                                MyFaceList& lstFaces)
{
    int nNumStrips = lstNewStrips.size();
    for (int i = 0; i < nNumStrips; i++)
    {
        NvStripInfo* pStrip = lstNewStrips[i];
        pStrip->m_nExperimentId = -1;
        lstStrips.push_back(pStrip);

        NvFaceInfoList& lstStripFaces = lstNewStrips[i]->m_lstFaces;
        int nNumFaces = lstStripFaces.size();
        NvFaceInfo* pFirstFace = lstStripFaces[0];
        if (pFirstFace->m_nVertex0 == 2302 &&
            pFirstFace->m_nVertex1 == 3215 &&
            pFirstFace->m_nVertex2 == 2603)
        {
            printf("BLEH");
        }

        for (int j = 0; j < nNumFaces; j++)
        {
            NvFaceInfo* pFace = lstStripFaces[j];
            if (pStrip->m_nExperimentId >= 0)
            {
                pFace->m_nExperimentId = pStrip->m_nExperimentId;
                pFace->m_nTestStripId = pStrip->m_nStripId;
            }
            else
            {
                pFace->m_nExperimentId = -1;
                pFace->m_nStripId = pStrip->m_nStripId;
            }
        }
    }
}

List<NvFaceInfo*> facestodelete;
// PC 0x0047F220  Xbox 0x002D4DD0
void NvStripifier::RemoveSmallStrips(NvStripInfoList& lstStrips,
                                     NvStripInfoList& lstLargeStrips,
                                     NvFaceInfoList& lstFaces)
{
    lstFaces.clear();
    lstLargeStrips.clear();
    NvFaceInfoList lstTempFaces;
    int i;

    for (i = 0; i < lstStrips.size(); i++)
    {
        int nNumFaces = lstStrips[i]->m_lstFaces.size();
        if (nNumFaces < m_nMinStripLength)
        {
            for (int j = 0; j < lstStrips[i]->m_lstFaces.size(); j++)
            {
                lstTempFaces.push_back(lstStrips[i]->m_lstFaces[j]);
            }

            delete lstStrips[i];
        }
        else
        {
            lstLargeStrips.push_back(lstStrips[i]);
        }
    }

    bool* pVisited = 0;
    unsigned int nNumTempFaces = lstTempFaces.size();
    if (nNumTempFaces > 0)
    {
        pVisited = new bool[nNumTempFaces];
        memset(pVisited, 0, nNumTempFaces * sizeof(bool));
    }

    VertexCache* pCache = new VertexCache(m_nCacheSize);

    int nBestNumHits = -1;
    int nNumHits;
    int nBestIndex;

    while (1)
    {
        nBestNumHits = -1;

        for (i = 0; i < lstTempFaces.size(); i++)
        {
            if (pVisited[i])
            {
                continue;
            }

            nNumHits = CalcNumHitsFace(pCache, lstTempFaces[i]);
            if (nNumHits > nBestNumHits)
            {
                nBestNumHits = nNumHits;
                nBestIndex = i;
            }
        }

        if (nBestNumHits == -1.0f)
        {
            break;
        }
        pVisited[nBestIndex] = true;
        UpdateCacheFace(pCache, lstTempFaces[nBestIndex]);
        lstFaces.push_back(lstTempFaces[nBestIndex]);
    }

    delete pCache;
    delete[] pVisited;
}
// PC 0x0047F560  Xbox 0x002D55C0
void NvStripifier::SplitUpStripsAndOptimize(NvStripInfoList& lstStrips,
                                            NvStripInfoList& lstOptimizedStrips,
                                            NvEdgeInfoList& lstEdges,
                                            NvFaceInfoList& lstFaces)
{
    NvStripInfoList lstTempStrips;

    int i;
    for (i = 0; i < lstStrips.size(); i++)
    {
        NvStripInfo* pCurrentStrip;
        NvStripStartInfo startInfo(0, 0, false);

        if (lstStrips[i]->m_lstFaces.size() > nStripSplitThreshold)
        {
            unsigned int nNumTimes = lstStrips[i]->m_lstFaces.size() / nStripSplitThreshold;
            unsigned int nNumLeftover = lstStrips[i]->m_lstFaces.size() % nStripSplitThreshold;

            unsigned int j;
            for (j = 0; j < nNumTimes; j++)
            {
                pCurrentStrip = new NvStripInfo(startInfo, 0, -1);
                for (unsigned int nFaceCtr = j * nStripSplitThreshold;
                     nFaceCtr < nStripSplitThreshold + (j * nStripSplitThreshold); nFaceCtr++)
                {
                    pCurrentStrip->m_lstFaces.push_back(
                        lstStrips[i]->m_lstFaces[nFaceCtr]);
                }
                lstTempStrips.push_back(pCurrentStrip);
            }

            int nLeftOff = j * nStripSplitThreshold;
            if (nNumLeftover != 0)
            {
                pCurrentStrip = new NvStripInfo(startInfo, 0, -1);
                for (unsigned int k = 0; k < nNumLeftover; k++)
                {
                    pCurrentStrip->m_lstFaces.push_back(
                        lstStrips[i]->m_lstFaces[nLeftOff++]);
                }
                lstTempStrips.push_back(pCurrentStrip);
            }
        }
        else
        {
            pCurrentStrip = new NvStripInfo(startInfo, 0, -1);
            for (int j = 0; j < lstStrips[i]->m_lstFaces.size(); j++)
            {
                pCurrentStrip->m_lstFaces.push_back(lstStrips[i]->m_lstFaces[j]);
            }
            lstTempStrips.push_back(pCurrentStrip);
        }
    }

    NvStripInfoList lstTempStrips2;
    RemoveSmallStrips(lstTempStrips, lstTempStrips2, lstFaces);

    lstOptimizedStrips.clear();
    if (lstTempStrips2.size() != 0)
    {
        VertexCache* pCache = new VertexCache(m_nCacheSize);

        float fBestNumHits = -1.0f;
        float fNumHits;
        int nBestIndex;

        int nFirstIndex = 0;
        float fMinCost = 10000.0f;

        for (i = 0; i < lstTempStrips2.size(); i++)
        {
            int nNumNeighbors = 0;
            for (int j = 0; j < lstTempStrips2[i]->m_lstFaces.size(); j++)
            {
                nNumNeighbors += NumNeighbors(lstTempStrips2[i]->m_lstFaces[j],
                                                 lstEdges);
            }

            float fCurrCost = (float)nNumNeighbors /
                              (float)lstTempStrips2[i]->m_lstFaces.size();
            if (fCurrCost < fMinCost)
            {
                fMinCost = fCurrCost;
                nFirstIndex = i;
            }
        }

        UpdateCacheStrip(pCache, lstTempStrips2[nFirstIndex]);
        lstOptimizedStrips.push_back(lstTempStrips2[nFirstIndex]);
        lstTempStrips2[nFirstIndex]->m_bVisited = true;

        while (1)
        {
            fBestNumHits = -1.0f;
            for (i = 0; i < lstTempStrips2.size(); i++)
            {
                if (lstTempStrips2[i]->m_bVisited)
                {
                    continue;
                }
                fNumHits = CalcNumHitsStrip(pCache, lstTempStrips2[i]);
                if (fNumHits > fBestNumHits)
                {
                    fBestNumHits = fNumHits;
                    nBestIndex = i;
                }
            }

            if (fBestNumHits == -1.0f)
            {
                break;
            }
            lstTempStrips2[nBestIndex]->m_bVisited = true;
            UpdateCacheStrip(pCache, lstTempStrips2[nBestIndex]);
            lstOptimizedStrips.push_back(lstTempStrips2[nBestIndex]);
        }

        delete pCache;
    }
}

int numsamples = 10;

int numedges = 6;
// PC 0x00480490  Xbox 0x002D6350
void NvStripifier::FindAllStrips(NvStripInfoList& lstStrips,
                                 NvFaceInfoList& lstFaces,
                                 NvEdgeInfoList& lstEdges, int nSamples,
                                 MyFaceList& lstOutputFaces)
{
    int nExperimentId = 0;
    int nStripId = 0;
    bool bDone = false;

    int nLoopCtr = 0;

    while (!bDone)
    {
        nLoopCtr++;

        NvStripInfoList* pExperiments =
            new NvStripInfoList[nSamples * numedges];
        int nExperimentIndex = 0;
        std::set<NvFaceInfo*> setResetPoints;
        for (int i = 0; i < nSamples; i++)
        {
            NvFaceInfo* pNextFace = FindGoodResetPoint(lstFaces, lstEdges);
            if (pNextFace == 0)
            {
                bDone = true;
                break;
            }
            else if (setResetPoints.find(pNextFace) != setResetPoints.end())
            {
                continue;
            }

            setResetPoints.insert(pNextFace);

            NvEdgeInfo* pEdge01 = FindEdgeInfo(lstEdges, pNextFace->m_nVertex0,
                                               pNextFace->m_nVertex1);
            NvStripInfo* pStrip01 =
                new NvStripInfo(NvStripStartInfo(pNextFace, pEdge01, true),
                                nStripId++, nExperimentId++);
            pExperiments[nExperimentIndex++].push_back(pStrip01);

            NvEdgeInfo* pEdge10 = FindEdgeInfo(lstEdges, pNextFace->m_nVertex0,
                                               pNextFace->m_nVertex1);
            NvStripInfo* pStrip10 =
                new NvStripInfo(NvStripStartInfo(pNextFace, pEdge10, false),
                                nStripId++, nExperimentId++);
            pExperiments[nExperimentIndex++].push_back(pStrip10);

            NvEdgeInfo* pEdge12 = FindEdgeInfo(lstEdges, pNextFace->m_nVertex1,
                                               pNextFace->m_nVertex2);
            NvStripInfo* pStrip12 =
                new NvStripInfo(NvStripStartInfo(pNextFace, pEdge12, true),
                                nStripId++, nExperimentId++);
            pExperiments[nExperimentIndex++].push_back(pStrip12);

            NvEdgeInfo* pEdge21 = FindEdgeInfo(lstEdges, pNextFace->m_nVertex1,
                                               pNextFace->m_nVertex2);
            NvStripInfo* pStrip21 =
                new NvStripInfo(NvStripStartInfo(pNextFace, pEdge21, false),
                                nStripId++, nExperimentId++);
            pExperiments[nExperimentIndex++].push_back(pStrip21);

            NvEdgeInfo* pEdge20 = FindEdgeInfo(lstEdges, pNextFace->m_nVertex2,
                                               pNextFace->m_nVertex0);
            NvStripInfo* pStrip20 =
                new NvStripInfo(NvStripStartInfo(pNextFace, pEdge20, true),
                                nStripId++, nExperimentId++);
            pExperiments[nExperimentIndex++].push_back(pStrip20);

            NvEdgeInfo* pEdge02 = FindEdgeInfo(lstEdges, pNextFace->m_nVertex2,
                                               pNextFace->m_nVertex0);
            NvStripInfo* pStrip02 =
                new NvStripInfo(NvStripStartInfo(pNextFace, pEdge02, false),
                                nStripId++, nExperimentId++);
            pExperiments[nExperimentIndex++].push_back(pStrip02);
        }

        int nNumExperiments = nExperimentIndex;
        for (i = 0; i < nNumExperiments; i++)
        {
            pExperiments[i][0]->Build(lstEdges, lstFaces);
            int nStripExperimentId = pExperiments[i][0]->m_nExperimentId;

            NvStripInfo* pStripIter = pExperiments[i][0];
            NvStripStartInfo startInfo(0, 0, false);
            while (FindTraversal(lstFaces, lstEdges, pStripIter, startInfo))
            {
                pStripIter = new NvStripInfo(startInfo, nStripId++,
                                             nStripExperimentId);
                pStripIter->Build(lstEdges, lstFaces);
                pExperiments[i].push_back(pStripIter);
            }
        }

        int nBestIndex = 0;
        double dBestValue = 0;
        for (i = 0; i < nNumExperiments; i++)
        {
            const float fAvgStripSizeWeight = 1.0f;
            const float fNumTrisWeight = 1.0f;
            float fAvgStripSize = AvgStripSize(pExperiments[i]);
            float fNumStrips = (float)pExperiments[i].size();
            float fValue = fAvgStripSize * fAvgStripSizeWeight +
                           (fAvgStripSize * fNumStrips * fNumTrisWeight);

            if (fValue > dBestValue)
            {
                dBestValue = fValue;
                nBestIndex = i;
            }
        }

        CommitStrips(lstStrips, pExperiments[nBestIndex], lstOutputFaces);

        for (i = 0; i < nNumExperiments; i++)
        {
            if (i != nBestIndex)
            {
                int nNumStrips = pExperiments[i].size();
                for (int j = 0; j < nNumStrips; j++)
                {
                    delete pExperiments[i][j];
                }
            }
        }

        delete[] pExperiments;
    }
}
// PC 0x0047FD70
void NvStripInfo::Build(NvEdgeInfoList& lstEdges, NvFaceInfoList& lstFaces)
{
    static NvIndexList scratchIndices;
    scratchIndices.resize(0);

    NvFaceInfoList lstForward;
    NvFaceInfoList lstBackward;
    lstForward.push_back(m_startInfo.m_pStartFace);
    NvFaceInfo* pStartFace = m_startInfo.m_pStartFace;
    if (m_nExperimentId >= 0)
    {
        pStartFace->m_nExperimentId = m_nExperimentId;
        pStartFace->m_nTestStripId = m_nStripId;
    }
    else
    {
        pStartFace->m_nExperimentId = -1;
        pStartFace->m_nStripId = m_nStripId;
    }

    int nVertex0 = m_startInfo.m_bToVertex1 ?
        m_startInfo.m_pStartEdge->m_nVertex0 : m_startInfo.m_pStartEdge->m_nVertex1;
    int nVertex1 = m_startInfo.m_bToVertex1 ?
        m_startInfo.m_pStartEdge->m_nVertex1 : m_startInfo.m_pStartEdge->m_nVertex0;
    scratchIndices.push_back(nVertex0);
    scratchIndices.push_back(nVertex1);
    int nVertex2 = NvStripifier::GetNextIndex(scratchIndices, m_startInfo.m_pStartFace);
    scratchIndices.push_back(nVertex2);

    int nPrevious = nVertex1;
    int nNext = nVertex2;
    NvEdgeInfo* pEdge = NvStripifier::FindEdgeInfo(lstEdges, nPrevious, nNext);
    NvFaceInfo* pFace = pEdge->m_pFace0 == m_startInfo.m_pStartFace ?
        pEdge->m_pFace1 : pEdge->m_pFace0;
    while (pFace != 0 && !IsMarked(pFace) && Unique(lstForward, pFace))
    {
        lstForward.push_back(pFace);
        if (m_nExperimentId >= 0)
        {
            pFace->m_nExperimentId = m_nExperimentId;
            pFace->m_nTestStripId = m_nStripId;
        }
        else
        {
            pFace->m_nExperimentId = -1;
            pFace->m_nStripId = m_nStripId;
        }
        nPrevious = nNext;
        nNext = NvStripifier::GetNextIndex(scratchIndices, pFace);
        scratchIndices.push_back(nNext);
        pEdge = NvStripifier::FindEdgeInfo(lstEdges, nPrevious, nNext);
        pFace = pEdge->m_pFace0 == pFace ? pEdge->m_pFace1 : pEdge->m_pFace0;
    }

    NvFaceInfoList lstAllFaces;
    for (unsigned int i = 0; i < lstForward.size(); i++)
    {
        lstAllFaces.push_back(lstForward[i]);
    }
    scratchIndices.resize(0);
    scratchIndices.push_back(nVertex2);
    scratchIndices.push_back(nVertex1);
    scratchIndices.push_back(nVertex0);
    nPrevious = nVertex1;
    nNext = nVertex0;
    pStartFace = m_startInfo.m_pStartFace;
    pEdge = NvStripifier::FindEdgeInfo(lstEdges, nPrevious, nNext);
    pFace = pEdge->m_pFace0 == pStartFace ?
        pEdge->m_pFace1 : pEdge->m_pFace0;
    while (pFace != 0 && !IsMarked(pFace) && Unique(lstAllFaces, pFace))
    {
        lstBackward.push_back(pFace);
        lstAllFaces.push_back(pFace);
        if (m_nExperimentId >= 0)
        {
            pFace->m_nExperimentId = m_nExperimentId;
            pFace->m_nTestStripId = m_nStripId;
        }
        else
        {
            pFace->m_nExperimentId = -1;
            pFace->m_nStripId = m_nStripId;
        }
        nPrevious = nNext;
        nNext = NvStripifier::GetNextIndex(scratchIndices, pFace);
        scratchIndices.push_back(nNext);
        pEdge = NvStripifier::FindEdgeInfo(lstEdges, nPrevious, nNext);
        pFace = pEdge->m_pFace0 == pFace ? pEdge->m_pFace1 : pEdge->m_pFace0;
    }
    Combine(lstForward, lstBackward);
}
// PC 0x0047EB30
void NvStripInfo::Combine(const NvFaceInfoList& lstForward,
                          const NvFaceInfoList& lstBackward)
{
    int nNumFaces = lstBackward.size();
    int i;
    for (i = nNumFaces - 1; i >= 0; i--)
    {
        m_lstFaces.push_back(lstBackward[i]);
    }

    nNumFaces = lstForward.size();
    for (i = 0; i < nNumFaces; i++)
    {
        m_lstFaces.push_back(lstForward[i]);
    }
}
// PC 0x00480EF0  Xbox 0x002D6C60
void NvStripifier::Stripify(const NvIndexList& lstIndices, int nCacheSize,
                            int nMinStripLength, MyFaceList& lstFaces,
                            NvStripInfoList& lstStrips,
                            NvFaceInfoList& lstFaceInfos)
{
    m_fMeshJump = 0.0f;
    m_bFirstTimeResetPoint = true;
    m_nCacheSize = nCacheSize;
    m_nMinStripLength = nMinStripLength;
    m_lstIndices = lstIndices;

    NvFaceInfoList lstAllFaceInfos;
    NvEdgeInfoList lstAllEdgeInfos;
    BuildStripifyInfo(lstAllFaceInfos, lstAllEdgeInfos);

    NvStripInfoList lstAllStrips;
    FindAllStrips(lstAllStrips, lstAllFaceInfos, lstAllEdgeInfos, numsamples,
                  lstFaces);

    SplitUpStripsAndOptimize(lstAllStrips, lstStrips, lstAllEdgeInfos,
                             lstFaceInfos);

    int i;
    for (i = 0; i < lstAllStrips.size(); i++)
    {
        delete lstAllStrips[i];
    }

    for (i = 0; i < lstAllEdgeInfos.size(); i++)
    {
        NvEdgeInfo* pInfo = lstAllEdgeInfos[i];
        while (pInfo != 0)
        {
            NvEdgeInfo* pNext = (pInfo->m_nVertex0 == i ? pInfo->m_pNextVertex0
                                                        : pInfo->m_pNextVertex1);
            pInfo->Unref();
            pInfo = pNext;
        }
    }

    facestodelete.m_nCount = 0;
    for (i = 0; i < lstAllFaceInfos.size(); i++)
    {
        facestodelete.Add(lstAllFaceInfos[i]);
    }
}

#define CACHESIZE_GEFORCE1_2 16
#define MIN_STRIP_LENGTH 2
// PC 0x00481140  Xbox 0x002D6E30
void AurStripify(List<int>& lstIndices, List<List<int> >& lstStrips)
{
    MyFaceList lstFaces;
    NvStripInfoList lstStripInfos;
    NvIndexList lstStripIndices;
    NvIndexList lstStripLengths;
    NvFaceInfoList lstFaceInfos;
    NvStripifier* pStripifier = new NvStripifier;

    for (int i = 0; i < lstIndices.m_nCount; i += 3)
    {
        MyFace face;
        face.m_nVertex0 = lstIndices[i];
        face.m_nVertex1 = lstIndices[i + 1];
        face.m_nVertex2 = lstIndices[i + 2];
        lstFaces.push_back(face);
    }

    NvIndexList lstInputIndices;
    lstInputIndices.reserve(3 * lstFaces.size());
    unsigned int i;
    for (i = 0; i < lstFaces.size(); i++)
    {
        lstInputIndices.push_back((unsigned short)lstFaces[i].m_nVertex0);
        lstInputIndices.push_back((unsigned short)lstFaces[i].m_nVertex1);
        lstInputIndices.push_back((unsigned short)lstFaces[i].m_nVertex2);
    }

    pStripifier->Stripify(lstInputIndices, CACHESIZE_GEFORCE1_2, MIN_STRIP_LENGTH,
        lstFaces, lstStripInfos, lstFaceInfos);
    pStripifier->CreateStrips(lstStripInfos, lstStripIndices, lstStripLengths);
    delete pStripifier;

    lstStrips.SetSize(lstStripLengths.size() + 1);
    int nIndex = 0;
    for (i = 0; i < lstStripLengths.size(); i++)
    {
        for (int j = 0; j < lstStripLengths[i]; j++)
            lstStrips[i].Add(lstStripIndices[nIndex++]);
    }

    int nTriangleList = lstStripLengths.size();
    for (i = 0; i < lstFaceInfos.size(); i++)
    {
        lstStrips[nTriangleList].Add(lstFaceInfos[i]->m_nVertex0);
        lstStrips[nTriangleList].Add(lstFaceInfos[i]->m_nVertex1);
        lstStrips[nTriangleList].Add(lstFaceInfos[i]->m_nVertex2);
    }

    i = facestodelete.m_nCount;
    while (i != 0)
        delete facestodelete[--i];
    for (i = 0; i < lstStripInfos.size(); i++)
        delete lstStripInfos[i];
}
