#ifndef AURWINDMANAGER_H
#define AURWINDMANAGER_H

class PartEmitter;
class Vector;
#include "auremitter.h"
#include <aurlist.h>
#include <aurpointer.h>
#include "math3d.h"

class PointSourceWind
{
public:
    PointSourceWind();
    ~PointSourceWind();

#ifdef _XBOX
    PointSourceWind(Vector vPosition, float fParameter0, float fParameter1,
                    float fParameter2);
#else
    PointSourceWind(Vector vPosition, float fParameter0, float fParameter1,  // Xbox 0x002F5C50
                    float fParameter2)
        : m_vPosition(vPosition),
          m_fParameter0(fParameter0),
          m_fParameter1(fParameter1),
          m_fParameter2(fParameter2)
    {
    }
#endif

private:
    friend class WindManager;

    Vector m_vPosition;
    float m_fParameter0;
    float m_fParameter1;
    float m_fParameter2;
};

class WindManager : public SafePointerList<WindManager>
{
public:
    WindManager();
    ~WindManager();

    void fillNoiseField(float fField[16][16]);
    void updateTimer(float fTime);
    void SetMaximumDeviation(float fMaximumDeviation, float fVariation);
    void SetGlobalWind(Vector vWind);

    Vector GetBaseWind() { return m_vGlobalWind; }
    Vector GetGlobalWindNoPoint(const Vector& vPosition, float fScale);
    Vector GetGlobalWind(Vector vPosition, float fScale);
    void RegisterWindyEmitter(PartEmitter* pEmitter);
    void DeregisterWindyEmitter(PartEmitter* pEmitter);
    void AddPointSourceWind(Vector vPosition, float fParameter0,
                           float fParameter1, float fParameter2);
    Vector pointSourceWindVector(Vector vPosition);
    Vector GetGlobalPointWind(const Vector& vPosition, float fTime);

private:

    friend class CAurTriangleBin;
    List<PointSourceWind> m_lstPointSourceWinds;
    Vector m_vGlobalWind;
    Vector m_vWindVelocity;
    float m_fNoiseFields[2][16][16];
    float (*m_pCurrentNoiseField)[16];
    float (*m_pNextNoiseField)[16];
    float m_fMaximumDeviation;
    float m_fMaximumDeviationVariation;
    float m_fNoiseInterpolation;
    float m_fNoiseTimer;
    float m_fNoiseInterval;
    List<PartEmitter*> m_lstWindyEmitters;
};

#endif
