#include <windows.h>
#include <GL/gl.h>

#include "aurtrigger.h"
#include "aurscene.h"
#include "aurscenemanager.h"
#include "aurrendergl.h"

float parttriggeralpha;

MdlNodeTrigger global_nodetrigger("trigger");
// PC 0x0060E760
void MdlNodeTrigger::InternalParseField(char* sField)
{
}
// PC 0x0063E7C0
Part* MdlNodeTrigger::InternalCreateInstance(Gob* pGob)
{
    return 0;
}
// PC 0x0049D7B0  Xbox 0x002C7560
MdlNodeTrigger::MdlNodeTrigger(char* sName)
    : MdlNode(sName)
{
    m_nNodeType = MdlNode::MDLNODE_TRIGGER_ID;
}
// Xbox 0x002C7580
void AurPartTriggerSetColor(PartTrigger* pTrigger, float* pfColor)
{
    memcpy(&pTrigger->m_fColorRed, pfColor, 4 * sizeof(float));
}
// PC 0x0049D7D0  Xbox 0x002C75B0
#ifdef _XBOX
void AurPartTriggerDelete(CAurScene* pScene, PartTrigger* pTrigger)
#else
void AurPartTriggerDelete(CAurScene* pScene, PartTrigger* pTrigger,
                          bool bDelete)
#endif
{
    if (((Scene*)pScene)->m_pBSPRoot != 0)
    {
        SceneBSPRemovePart((Scene*)pScene, pTrigger);
    }
    else
    {
        SceneRemovePartFromRooms((Scene*)pScene, pTrigger);
    }

#ifndef _XBOX
    if (bDelete && pTrigger != 0)
    {
        delete pTrigger;
    }
#endif
}
// PC 0x00670B20
void AurPartTriggerReleaseMemory(PartTrigger* pTrigger)
{
    delete pTrigger;
}
// PC 0x0049D810  Xbox 0x002C7600
int PartTrigger::HitCheckGeom(const Vector& vStart, Vector& vEnd,
                              Vector* pHit, CHitInfo* pHitInfo, float fRadius)
{
    if (!(pHitInfo->m_nFlags & CHitInfo::HIT_TRIGGER))
    {
        return 0;
    }

    if (!BoxIntersect(vStart, vEnd, m_vMinimum, m_vMaximum))
    {
        return 0;
    }

    for (int i = 0; i < m_lstTriangles.m_nCount; i += 3)
    {
        int nHit = polyhit(&m_lstTriangles.m_pList[i], 3, vStart, vEnd, &vEnd,
                           &pHitInfo->m_vOutputNormal);
        if (nHit)
        {
            if (pHit)
            {
                *pHit = vEnd;
                pHitInfo->m_pOutputHitData = m_pHitData;
                pHitInfo->m_nOutputIndex = CHitInfo::HIT_TRIGGER;
            }
            return nHit;
        }
    }

    return 0;
}
// PC 0x0049DB30  Xbox 0x002C78C0
PartTrigger::PartTrigger(MdlNode* pNode)
    : Part(pNode), m_lstVertices(0), m_lstHighlightTop(0),
      m_lstHighlightBottom(0), m_lstHighlightSide(0),
      m_lstTriangles(0)
{
    m_bHighlighted = false;
    m_bHighlightReverse = false;
    m_fHighlightColorRed = 1.0f;
    m_fHighlightColorGreen = 1.0f;
    m_fHighlightColorBlue = 1.0f;
    m_fHighlightAlpha = 0.5f;
    m_bHighlightColor = false;
    m_fHighlightBlend = 1.0f;
    m_bAdditiveBlend = false;
}
// PC 0x0049D8E0  Xbox 0x002C76D0
PartTrigger::~PartTrigger()
{
    Scene::RemoveFromSceneManager(this);
}

#ifdef _XBOX
// Xbox 0x002C75D0
void PartTrigger::SetHighlightColor(const Vector& vColor, float fAlpha)
{
    m_fHighlightColorRed = vColor.m_fX;
    m_fHighlightColorGreen = vColor.m_fY;
    m_fHighlightColorBlue = vColor.m_fZ;
    m_fHighlightAlpha = fAlpha;
}
// Xbox 0x002C7770
void AurPartTriggerSetHighlightParams(PartTrigger* pTrigger,
                                      const Vector& vColor,
                                      float fAlpha, bool bColor,
                                      float fBlend, bool bAdditive)
{
    pTrigger->SetHighlightColor(vColor, fAlpha);
    pTrigger->m_bHighlightColor = bColor;
    pTrigger->m_fHighlightBlend = fBlend;
    pTrigger->m_bAdditiveBlend = bAdditive;
}
#else
PartTrigger* AurPartTriggerSetHighlightParams(PartTrigger* pTrigger,  // PC 0x0049D9B0
                                              const Vector& vColor,
                                              float fAlpha, bool bColor,
                                              float fBlend, bool bAdditive)
{
    pTrigger->m_fHighlightColorRed = vColor.m_fX;
    pTrigger->m_fHighlightColorGreen = vColor.m_fY;
    pTrigger->m_fHighlightColorBlue = vColor.m_fZ;
    pTrigger->m_fHighlightAlpha = fAlpha;
    pTrigger->m_bHighlightColor = bColor;
    pTrigger->m_fHighlightBlend = fBlend;
    pTrigger->m_bAdditiveBlend = bAdditive;
    return pTrigger;
}
#endif
// PC 0x0049DA00  Xbox 0x002C77B0
void PartTrigger::DrawVolume()
{
    GLRender::DrawFromVerts(Aurora::PRIMITIVE_TRIANGLE_STRIP, m_lstHighlightTop);
    GLRender::DrawFromVerts(Aurora::PRIMITIVE_POLYGON, m_lstHighlightBottom);
    GLRender::DrawFromVerts(Aurora::PRIMITIVE_POLYGON, m_lstHighlightSide);
}
// PC 0x0049DA40  Xbox 0x002C77F0
void PartTrigger::Draw(bool bRenderLast)
{
    float afColor[4];
    afColor[0] = m_fColorRed;
    afColor[1] = m_fColorGreen;
    afColor[2] = m_fColorBlue;
    afColor[3] = m_fAlpha + parttriggeralpha;

    if (afColor[3] == 0.0f)
    {
        return;
    }

    GLRender_PushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT);
    GLRender_Disable(ENABLE_BLEND);
    GLRender_Disable(ENABLE_TEXTURE_2D);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_SetColorv(afColor);
    GLRender::DrawFromVerts(Aurora::PRIMITIVE_LINE_LOOP, m_lstVertices);
    GLRender_Enable(ENABLE_BLEND);
    if (m_bAdditiveBlend)
    {
        GLRender_SetBlendFunc(BLEND_ONE, BLEND_ONE);
    }
    else
    {
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    }
    GLRender::DrawFromVerts(Aurora::PRIMITIVE_TRIANGLES, m_lstTriangles);
    GLRender_PopAttrib();
}
// PC 0x0049DC10  Xbox 0x002C79D0
void PartTrigger::SetHighlighted(bool bHighlighted, float fOffset, bool bReverse)
{
    m_bHighlighted = bHighlighted;
    m_bHighlightReverse = bReverse;
    m_fHighlightOffset = fOffset < 0.0f ? -fOffset : fOffset;

    if (m_lstVertices.m_nCount >= 2 && bHighlighted)
    {
        m_lstHighlightTop.m_nCount = 0;
        m_lstHighlightBottom.m_nCount = 0;
        m_lstHighlightSide.m_nCount = 0;

        for (int i = 0; i < m_lstVertices.m_nCount; i++)
        {
            Vector vVertex = m_lstVertices[i];
            vVertex.m_fZ += m_fHighlightOffset;
            m_lstHighlightTop.Add(vVertex);
            m_lstHighlightBottom.Add(vVertex);
            vVertex.m_fZ -= 2.0f * m_fHighlightOffset;
            m_lstHighlightTop.Add(vVertex);
        }

        int nVertices = m_lstVertices.m_nCount;
        while (nVertices--)
        {
            m_lstHighlightSide.Add(m_lstVertices[nVertices] +
                                   Vector(0.0f, 0.0f, -m_fHighlightOffset));
        }

        m_lstHighlightTop.Add(m_lstHighlightTop[0]);
        m_lstHighlightTop.Add(m_lstHighlightTop[1]);
        m_vMinimum = VectorMin(m_lstHighlightTop);
        m_vMaximum = VectorMax(m_lstHighlightTop);
    }
}
// PC 0x0049DF10  Xbox 0x002C7C00
void PartTrigger::Decompose(List<int> lstIndices)
{
    while (lstIndices.m_nCount > 2)
    {
        float fLeftmost = TRIGGER_DECOMPOSE_LIMIT;
        int nLeftmost = -1;
        for (int i = 0; i < lstIndices.m_nCount; ++i)
        {
            if (fLeftmost > m_lstVertices[lstIndices[i]].m_fX)
            {
                nLeftmost = i;
                fLeftmost = m_lstVertices[lstIndices[i]].m_fX;
            }
        }

        int nPrevious = (nLeftmost + lstIndices.m_nCount - 1) % lstIndices.m_nCount;
        int nNext = (nLeftmost + lstIndices.m_nCount + 1) % lstIndices.m_nCount;
        int nFirst = lstIndices[nPrevious];
        int nCorner = lstIndices[nLeftmost];
        int nLast = lstIndices[nNext];
        List<Vector> lstTriangle(0);
        lstTriangle.Add(m_lstVertices[nFirst]);
        lstTriangle.Add(m_lstVertices[nCorner]);
        lstTriangle.Add(m_lstVertices[nLast]);
        for (int i = 0; i < lstTriangle.m_nCount; ++i)
        {
            lstTriangle[i].m_fZ = 0.0f;
        }

        float fInside = TRIGGER_DECOMPOSE_LIMIT;
        int nInside = -1;
        for (int i = 0; i < lstIndices.m_nCount; ++i)
        {
            int nVertex = lstIndices[i];
            if (nVertex != nFirst && nVertex != nCorner && nVertex != nLast)
            {
                Vector vStart = m_lstVertices[nVertex];
                Vector vEnd = m_lstVertices[nVertex];
                vStart.m_fZ = TRIGGER_PROJECT_TOP;
                vEnd.m_fZ = TRIGGER_DECOMPOSE_BOTTOM;
                Vector vNormal(0.0f, 0.0f, 0.0f);
                int nHit = polyhit(lstTriangle.m_pList, 3, vStart, vEnd, &vEnd, &vNormal);
                if (nHit && vStart.m_fX < fInside)
                {
                    nInside = i;
                    fInside = vStart.m_fX;
                }
            }
        }

        int nSplitVertex = -1;
        if (nInside >= 0)
        {
            nFirst = lstIndices[nInside];
            if (nLeftmost == lstIndices.m_nCount - 1)
            {
                lstIndices.Add(nFirst);
            }
            else
            {
                lstIndices.Insert(nFirst, nLeftmost + 1);
                nSplitVertex = nFirst;
            }
        }
        else
        {
            lstIndices.DelIndex(nLeftmost);
        }

        m_lstTriangles.Add(m_lstVertices[nFirst]);
        m_lstTriangles.Add(m_lstVertices[nCorner]);
        m_lstTriangles.Add(m_lstVertices[nLast]);

        if (nSplitVertex >= 0)
        {
            for (int nSplit = 0; nSplit < lstIndices.m_nCount; ++nSplit)
            {
                if (lstIndices[nSplit] == nSplitVertex)
                {
                    List<int> lstSplit(0);
                    int nRemove = nSplit + 1;
                    int nVertex;
                    do
                    {
                        nVertex = lstIndices[nRemove];
                        lstSplit.Add(nVertex);
                        lstIndices.DelIndex(nRemove);
                    }
                    while (nVertex != nSplitVertex);
                    Decompose(lstSplit);
                    break;
                }
            }
        }
    }
}
// PC 0x0049E610  Xbox 0x002C8430
void AurPartTriggerSetHighlight(PartTrigger* pTrigger, bool bHighlighted,
                               float fOffset, bool bReverse)
{
    pTrigger->SetHighlighted(bHighlighted, fOffset, bReverse);
}
// PC 0x0049E630  Xbox 0x002C8010
PartTrigger* AurPartTriggerCreate(CAurScene* pScene, Vector* pVertices,
                                  int nVertices, int bHitCheck, float* pfColor,
                                  void* pHitData)
{
    PartTrigger* pTrigger = new PartTrigger(&global_nodetrigger);
    CHitInfo* pHitInfo = new CHitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
    for (int i = 0; i < nVertices; ++i)
    {
        if (bHitCheck == 0)
        {
            Vector vStart = pVertices[i];
            vStart.m_fZ = TRIGGER_PROJECT_TOP;
            Vector vEnd = vStart;
            vEnd.m_fZ = TRIGGER_PROJECT_BOTTOM;
            pHitInfo->ClearOutput();
            pHitInfo->m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
            int bHit = pScene->HitCheck(vStart, vEnd, &pVertices[i], pHitInfo);
            if (!bHit)
            {
                Vector vOffset(TRIGGER_PROJECT_OFFSET, TRIGGER_PROJECT_OFFSET, 0.0f);
                pScene->HitCheck(vStart + vOffset, vEnd + vOffset, &pVertices[i], pHitInfo);
            }
            pVertices[i].m_fZ += TRIGGER_PROJECT_OFFSET;
        }
        pTrigger->m_lstVertices.Add(pVertices[i]);
    }

    pTrigger->m_vMaximum = Vector(-TRIGGER_BOUNDS_LIMIT, -TRIGGER_BOUNDS_LIMIT,
                                 -TRIGGER_BOUNDS_LIMIT);
    pTrigger->m_vMinimum = Vector(TRIGGER_BOUNDS_LIMIT, TRIGGER_BOUNDS_LIMIT,
                                 TRIGGER_BOUNDS_LIMIT);
    List<Vector>& lstVertices = pTrigger->m_lstVertices;
    for (int i = 0; i < lstVertices.m_nCount; ++i)
    {
        pTrigger->m_vMinimum = VectorMin(pTrigger->m_vMinimum, lstVertices[i]);
        pTrigger->m_vMaximum = VectorMax(pTrigger->m_vMaximum, lstVertices[i]);
    }
    memcpy(&pTrigger->m_fColorRed, pfColor, 4 * sizeof(float));
    pTrigger->m_pHitData = pHitData;

    List<int> lstIndices(0);
    for (int i = 0; i < nVertices; ++i)
    {
        lstIndices.Add(i);
    }
    pTrigger->Decompose(lstIndices);
    if (((Scene*)pScene)->m_pBSPRoot != 0)
    {
        SceneBSPAddPart((Scene*)pScene, pTrigger, 0);
    }
    else
    {
        SceneAddPartToRooms((Scene*)pScene, pTrigger);
    }
#ifndef _XBOX
    delete pHitInfo;
#endif
    return pTrigger;
}
