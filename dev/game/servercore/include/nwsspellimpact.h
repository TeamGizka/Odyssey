#ifndef __NWSSPELLIMPACT_H__
#define __NWSSPELLIMPACT_H__

#include "exobase.h"
#include "math3d.h"
#include "nwsobject.h"

class CResGFF;
class CResStruct;
class CSWSArea;

class CSWSAreaOfEffectObject : public CSWSObject
{
public:

    enum { DEFAULT_SPELL_SAVE_DC = 14 };

    enum { SHAPE_CIRCLE = 0, SHAPE_RECTANGLE = 1 };

    enum { HEARTBEAT_INTERVAL = 6000 };

    enum ScriptIndex
    {
        SCRIPT_HEARTBEAT = 0,
        SCRIPT_USER_DEFINED = 1,
        SCRIPT_ON_ENTER = 2,
        SCRIPT_ON_EXIT = 3
    };
    virtual ~CSWSAreaOfEffectObject();
    virtual CSWSAreaOfEffectObject* AsSWSAreaOfEffectObject() { return this; }  // PC 0x00641DB0
    virtual void AIUpdate();
    virtual void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay);
    virtual unsigned long GetEffectSpellId()  // PC 0x005945D0  Xbox 0x0023B9A0
    {
        return m_EffectSpellId;
    }
    virtual void SetEffectSpellId(unsigned long nSpellId)  // PC 0x005945E0  Xbox 0x0023B9B0
    {
        m_EffectSpellId = nSpellId;
    }

    CSWSAreaOfEffectObject(unsigned long oidObject);
    void SetShape(unsigned char nShape, float fWidth, float fLength);

    float GetRadius()
    {
        return m_fRadius;
    }

    Vector GetPosition();
    void SetDuration(unsigned char nDurationType, float fDuration);
    void LoadAreaEffect(int nAreaEffectId);
    int LoadEffect(CResGFF* pRes, CResStruct* pStruct);
    int SaveEffect(CResGFF* pRes, CResStruct* pStruct);
    unsigned long GetCreator() { return m_oidCreator; }
    void SetCreator(unsigned long oidCreator);
    int LineSegmentIntersectAreaOfEffect(Vector vStart, Vector vEnd);
    int InAreaOfEffect(Vector vPosition);
    void SetTargetObjID(unsigned long oidTarget);
    void UpdateSubAreas(Vector* pvLastPosition);
    void RemoveFromSubAreas(int bNotify);
    void AddToArea(CSWSArea* pArea, float fX, float fY, float fZ,
                  int bLoadingGame);
    void RemoveFromArea();
    void JumpToPoint(CSWSArea* pArea, const Vector& vPosition);

    unsigned short m_nObjectIterator;
    char m_aUnmapped22A[2];
    int m_nAreaEffectId;
    unsigned char m_nShape;
    char m_aUnmapped231[3];
    unsigned long m_EffectSpellId;
    float m_fRadius;
    float m_fWidth;
    float m_fLength;
    Vector* m_pShapeVertices;
    unsigned long m_oidCreator;
    unsigned long m_oidTarget;
    unsigned long m_oidLastEntered;
    unsigned long m_oidLastLeft;
    int m_nSpellSaveDC;
    int m_nSpellLevel;

    CExoString m_asScripts[4];
    unsigned long m_nLastHeartbeatDay;
    unsigned long m_nLastHeartbeatTime;
    unsigned long m_nDuration;
    unsigned char m_nDurationType;
    char m_aUnmapped28D[3];

public:

    void SetLastEntered(unsigned long nLastEntered)
    {
        m_oidLastEntered = nLastEntered;
    }
    unsigned long GetLastEntered()
    {
        return m_oidLastEntered;
    }
    unsigned long GetLastLeft()
    {
        return m_oidLastLeft;
    }
    void SetLastLeft(unsigned long nLastLeft)
    {
        m_oidLastLeft = nLastLeft;
    }
    unsigned char GetShape()
    {
        return m_nShape;
    }
    int GetAreaEffectId()
    {
        return m_nAreaEffectId;
    }
    int GetSpellSaveDC()
    {
        return m_nSpellSaveDC;
    }
    int GetSpellLevel()
    {
        return m_nSpellLevel;
    }
};

#endif
