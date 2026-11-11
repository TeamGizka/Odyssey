#ifndef AURTRISTRIPPER_H
#define AURTRISTRIPPER_H

#include <list>
#include <vector>

#include "aurvertexcache.h"

template <class T> class List;

struct MyFace
{
    int m_nVertex0;
    int m_nVertex1;
    int m_nVertex2;

    int m_nStripId;
    int m_nTestStripId;
    int m_nExperimentId;
};

class NvFaceInfo
{
public:
    NvFaceInfo(int nVertex0, int nVertex1, int nVertex2)
    {
        m_nVertex0 = nVertex0;
        m_nVertex1 = nVertex1;
        m_nVertex2 = nVertex2;
        m_nStripId = -1;
        m_nTestStripId = -1;
        m_nExperimentId = -1;
    }

    int m_nVertex0;
    int m_nVertex1;
    int m_nVertex2;
    int m_nStripId;
    int m_nTestStripId;
    int m_nExperimentId;
};

class NvEdgeInfo
{
public:
    NvEdgeInfo(int nVertex0, int nVertex1)
    {
        m_nVertex0 = nVertex0;
        m_nVertex1 = nVertex1;
        m_pFace0 = 0;
        m_pFace1 = 0;
        m_pNextVertex0 = 0;
        m_pNextVertex1 = 0;
        m_nReferenceCount = 2;
    }

    void Unref()
    {
        if (--m_nReferenceCount == 0)
        {
            delete this;
        }
    }

    unsigned int m_nReferenceCount;
    NvFaceInfo* m_pFace0;
    NvFaceInfo* m_pFace1;
    int m_nVertex0;
    int m_nVertex1;
    NvEdgeInfo* m_pNextVertex0;
    NvEdgeInfo* m_pNextVertex1;
};

class NvStripStartInfo
{
public:
    NvStripStartInfo(NvFaceInfo* pStartFace, NvEdgeInfo* pStartEdge,
                     bool bToVertex1)
    {
        m_pStartFace = pStartFace;
        m_pStartEdge = pStartEdge;
        m_bToVertex1 = bToVertex1;
    }

    NvFaceInfo* m_pStartFace;
    NvEdgeInfo* m_pStartEdge;
    bool m_bToVertex1;
};

typedef std::vector<NvFaceInfo*> NvFaceInfoList;
typedef std::vector<NvEdgeInfo*> NvEdgeInfoList;
typedef std::vector<unsigned short> NvIndexList;
typedef std::vector<MyFace> MyFaceList;

class NvStripInfo
{
public:
    bool Unique(NvFaceInfoList& lstFaces, NvFaceInfo* pFace);
    bool SharesEdge(const NvFaceInfo* pFace, NvEdgeInfoList& lstEdges);
    void Combine(const NvFaceInfoList& lstForward,
                 const NvFaceInfoList& lstBackward);
    void Build(NvEdgeInfoList& lstEdges, NvFaceInfoList& lstFaces);
    bool IsInStrip(const NvFaceInfo* pFace) const  // PC 0x0047BA70  Xbox 0x002D2040
    {
        if (pFace == 0)
        {
            return false;
        }
        if (m_nExperimentId >= 0)
        {
            return pFace->m_nTestStripId == m_nStripId;
        }
        return pFace->m_nStripId == m_nStripId;
    }
    bool IsMarked(NvFaceInfo* pFace)
    {
        return (pFace->m_nStripId >= 0) ||
               (m_nExperimentId >= 0 && pFace->m_nExperimentId == m_nExperimentId);
    }
    void MarkTriangle(NvFaceInfo* pFace)
    {
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
    }
    NvStripInfo(const NvStripStartInfo& startInfo, int nStripId,
                int nExperimentId = -1)
        : m_startInfo(startInfo)
    {
        m_nStripId = nStripId;
        m_nExperimentId = nExperimentId;
        m_bVisited = false;
    }

    NvStripStartInfo m_startInfo;
    NvFaceInfoList m_lstFaces;
    int m_nStripId;
    int m_nExperimentId;
    bool m_bVisited;
};

typedef std::vector<NvStripInfo*> NvStripInfoList;

class NvStripifier
{
public:
    void CreateStrips(NvStripInfoList& lstStrips, NvIndexList& lstIndices,
                      NvIndexList& lstStripLengths);
    void BuildStripifyInfo(NvFaceInfoList& lstFaces, NvEdgeInfoList& lstEdges);
protected:
    bool AlreadyExists(NvFaceInfo* pFace, NvFaceInfoList& lstFaces);
    NvFaceInfo* FindGoodResetPoint(NvFaceInfoList& lstFaces,
                                  NvEdgeInfoList& lstEdges);
    void UpdateCacheFace(VertexCache* pCache, NvFaceInfo* pFace);
    int CalcNumHitsFace(VertexCache* pCache, NvFaceInfo* pFace);
    void UpdateCacheStrip(VertexCache* pCache, NvStripInfo* pStrip);
    float CalcNumHitsStrip(VertexCache* pCache, NvStripInfo* pStrip);
    bool FindTraversal(NvFaceInfoList& lstFaces, NvEdgeInfoList& lstEdges,
                       NvStripInfo* pStrip, NvStripStartInfo& startInfo);
public:
    void CommitStrips(NvStripInfoList& lstStrips,
                     const NvStripInfoList& lstNewStrips, MyFaceList& lstFaces);
    void SplitUpStripsAndOptimize(NvStripInfoList& lstStrips,
                                  NvStripInfoList& lstOptimizedStrips,
                                  NvEdgeInfoList& lstEdges,
                                  NvFaceInfoList& lstFaces);
    void Stripify(const NvIndexList& lstIndices, int nCacheSize,
                  int nMinStripLength, MyFaceList& lstFaces,
                  NvStripInfoList& lstStrips, NvFaceInfoList& lstFaceInfos);
#ifndef _XBOX
    static int GetUniqueVertexInB(NvFaceInfo* pFaceA, NvFaceInfo* pFaceB)  // Xbox 0x002D1FA0
    {
        if (pFaceB->m_nVertex0 != pFaceA->m_nVertex0 &&
            pFaceB->m_nVertex0 != pFaceA->m_nVertex1 &&
            pFaceB->m_nVertex0 != pFaceA->m_nVertex2)
        {
            return pFaceB->m_nVertex0;
        }
        if (pFaceB->m_nVertex1 != pFaceA->m_nVertex0 &&
            pFaceB->m_nVertex1 != pFaceA->m_nVertex1 &&
            pFaceB->m_nVertex1 != pFaceA->m_nVertex2)
        {
            return pFaceB->m_nVertex1;
        }
        if (pFaceB->m_nVertex2 != pFaceA->m_nVertex0 &&
            pFaceB->m_nVertex2 != pFaceA->m_nVertex1 &&
            pFaceB->m_nVertex2 != pFaceA->m_nVertex2)
        {
            return pFaceB->m_nVertex2;
        }
        return -1;
    }
    // Xbox 0x002D1FF0
    static int GetSharedVertex(NvFaceInfo* pFaceA, NvFaceInfo* pFaceB)
    {
        int nVertex = pFaceB->m_nVertex0;

        if (nVertex == pFaceA->m_nVertex0 || nVertex == pFaceA->m_nVertex1 ||
            nVertex == pFaceA->m_nVertex2)
        {
            return nVertex;
        }

        nVertex = pFaceB->m_nVertex1;
        if (nVertex == pFaceA->m_nVertex0 || nVertex == pFaceA->m_nVertex1 ||
            nVertex == pFaceA->m_nVertex2)
        {
            return nVertex;
        }

        nVertex = pFaceB->m_nVertex2;
        if (nVertex == pFaceA->m_nVertex0 || nVertex == pFaceA->m_nVertex1 ||
            nVertex == pFaceA->m_nVertex2)
        {
            return nVertex;
        }

        return -1;
    }

    static bool IsCW(NvFaceInfo* pFace, int nVertex0, int nVertex1)
    {
        bool bIsCW = false;
        if (pFace->m_nVertex0 == nVertex0)
        {
            bIsCW = (pFace->m_nVertex1 == nVertex1); }
        else if (pFace->m_nVertex1 == nVertex0)
        {
            bIsCW = (pFace->m_nVertex2 == nVertex1); }
        else
        {
            bIsCW = (pFace->m_nVertex0 == nVertex1); }
        return bIsCW;
    }
#else
    static int GetUniqueVertexInB(NvFaceInfo* pFaceA, NvFaceInfo* pFaceB); static int GetSharedVertex(NvFaceInfo* pFaceA, NvFaceInfo* pFaceB); bool IsCW(NvFaceInfo* pFace, int nVertex0, int nVertex1);
#endif

    static int GetNextIndex(const NvIndexList& lstIndices, NvFaceInfo* pFace);
    static NvEdgeInfo* FindEdgeInfo(NvEdgeInfoList& lstEdges, int nVertex0,
                                    int nVertex1);
#ifndef _XBOX
    static NvFaceInfo* FindOtherFace(NvEdgeInfoList& lstEdges, int nVertex0, int nVertex1, NvFaceInfo* pFace)
    {
        NvEdgeInfo* pEdge = FindEdgeInfo(lstEdges, nVertex0, nVertex1); return (pEdge->m_pFace0 == pFace ? pEdge->m_pFace1 : pEdge->m_pFace0);
    }
#else
    static NvFaceInfo* FindOtherFace(NvEdgeInfoList& lstEdges, int nVertex0, int nVertex1, NvFaceInfo* pFace);
#endif
    int FindStartPoint(NvFaceInfoList& lstFaces, NvEdgeInfoList& lstEdges);
    int NumNeighbors(NvFaceInfo* pFace, NvEdgeInfoList& lstEdges);
    float AvgStripSize(const NvStripInfoList& lstStrips);
    void FindAllStrips(NvStripInfoList& lstStrips, NvFaceInfoList& lstFaces,
                       NvEdgeInfoList& lstEdges, int nSamples,
                       MyFaceList& lstOutputFaces);
    void RemoveSmallStrips(NvStripInfoList& lstStrips,
                           NvStripInfoList& lstLargeStrips,
                           NvFaceInfoList& lstFaces);

    NvIndexList m_lstIndices;
    int m_nCacheSize;
    int m_nMinStripLength;
    float m_fMeshJump;
    bool m_bFirstTimeResetPoint;
#ifdef _XBOX
public: NvStripifier();
#endif
};

void AurStripify(List<int>& lstIndices, List<List<int> >& lstStrips);

#endif
