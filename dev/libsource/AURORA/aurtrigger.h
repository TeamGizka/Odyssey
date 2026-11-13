#ifndef AURTRIGGER_H
#define AURTRIGGER_H

#include "auremitter.h"
#include <aurlist.h>

class CHitInfo;
class CAurScene;

#define TRIGGER_PROJECT_TOP 1000.0f
#define TRIGGER_PROJECT_BOTTOM -11.0f
#define TRIGGER_PROJECT_OFFSET 0.01f
#define TRIGGER_BOUNDS_LIMIT 100000.0f

#define TRIGGER_DECOMPOSE_LIMIT 1000000.0f
#define TRIGGER_DECOMPOSE_BOTTOM -1000.0f

#define TRIGGER_HIGHLIGHT_PHASE_RATE TWO_PI
enum { TRIGGER_HIGHLIGHT_STENCIL_MASK = 0x0F };

class MdlNodeTrigger : public MdlNode
{
public:
    MdlNodeTrigger(char* sName);
    Part* InternalCreateInstance(Gob* pGob);
    void InternalParseField(char* szField);
    MdlNode* InternalConJoin();
};

class PartTrigger : public Part
{
public:
    float m_fColorRed;
    float m_fColorGreen;
    float m_fColorBlue;
    float m_fAlpha;
    Vector m_vMinimum;
    Vector m_vMaximum;
    List<Vector> m_lstVertices;
    List<Vector> m_lstHighlightTop;
    List<Vector> m_lstHighlightBottom;
    List<Vector> m_lstHighlightSide;
    void* m_pHitData;
    bool m_bHighlighted;
    char m_abReservedA9[3];
    float m_fHighlightOffset;
    bool m_bHighlightReverse;
    char m_abReservedB1[3];

    float m_fHighlightColorRed;
    float m_fHighlightColorGreen;
    float m_fHighlightColorBlue;
    float m_fHighlightAlpha;
    bool m_bHighlightColor;
    char m_abReservedC5[3];
    float m_fHighlightBlend;
    bool m_bAdditiveBlend;
    char m_abReservedCD[3];
    List<Vector> m_lstTriangles;

    PartTrigger(MdlNode* pNode);
    virtual ~PartTrigger();
    virtual int HitCheckGeom(const Vector& vStart, Vector& vEnd, Vector* pHit,
                             CHitInfo* pHitInfo, float fRadius);
    virtual PartTrigger* AsPartTrigger() { return this; }  // PC 0x00698BD0 GOG 0x00698980
    virtual void Draw(bool bRenderLast);
    virtual void DrawVolume();

    void SetHighlighted(bool bHighlighted, float fOffset, bool bReverse);
    void Decompose(List<int> lstIndices);
#ifdef _XBOX
    void SetHighlightColor(const Vector& vColor, float fAlpha);
#endif
};

extern MdlNodeTrigger global_nodetrigger;

void AurPartTriggerSetColor(PartTrigger* pTrigger, float* pfColor);
void AurPartTriggerReleaseMemory(PartTrigger* pTrigger);
#ifdef _XBOX

void AurPartTriggerDelete(CAurScene* pScene, PartTrigger* pTrigger);
inline void AurPartTriggerDelete(CAurScene* pScene, PartTrigger* pTrigger, bool bDelete)
{
    AurPartTriggerDelete(pScene, pTrigger);
}
#else
void AurPartTriggerDelete(CAurScene* pScene, PartTrigger* pTrigger,
                          bool bDelete);
#endif
#ifdef _XBOX
void AurPartTriggerSetHighlightParams(PartTrigger* pTrigger,
                                      const Vector& vColor,
                                      float fAlpha, bool bColor,
                                      float fBlend, bool bAdditive);
#else
PartTrigger* AurPartTriggerSetHighlightParams(PartTrigger* pTrigger,
                                              const Vector& vColor,
                                              float fAlpha, bool bColor,
                                              float fBlend, bool bAdditive);
#endif
void AurPartTriggerSetHighlight(PartTrigger* pTrigger, bool bHighlighted,
                               float fOffset, bool bReverse);
PartTrigger* AurPartTriggerCreate(CAurScene* pScene, Vector* pVertices,
                                  int nVertices, int bHitCheck, float* pfColor,
                                  void* pHitData);

#endif
