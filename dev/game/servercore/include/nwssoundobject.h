#ifndef __NWSSOUNDOBJECT_H__
#define __NWSSOUNDOBJECT_H__

#include "exobase.h"
#include "exoresman.h"
#include "nwsobject.h"

class CSWCSoundObject;

class CSWSSoundObject : public CSWSObject
{
public:
    CSWSSoundObject(unsigned long oidObject);
    virtual ~CSWSSoundObject();

    virtual void AIUpdate();
    // PC 0x00641DB0
    virtual CSWSSoundObject* AsSWSSoundObject() { return this; }

    virtual void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay);
    void Save(CResGFF* pRes, CResStruct* pStruct);
    void RemoveFromArea();
    void AddToArea(CSWSArea* pArea, int bLoadingGame);
    void PackIntoMessage(CSWSMessage* pMessage);
    void Play();
    void Stop();
    CSWCSoundObject* GetClientSoundObject();
    void FadeAndStop(unsigned long nFadeTime);
    void ChangeVolume(int nVolume);
    void ChangePosition(Vector vPosition);
    int Load(CResGFF* pRes, CResStruct* pStruct);
    int LoadFromTemplate(const CResRef& resTemplate);
    void ChangePitchVariation(float fVariation);
    void ChangeFixedVariance(float fVariance);

    int m_bActive;
    int m_bPositional;
    int m_bLooping;
    int m_nVolume;
    int m_nVolumeVariation;
    int m_nTimes;
    float m_fPitchVariation;
    unsigned long m_nHours;
    float m_fFixedVariance;
    unsigned char m_nGeneratedType;
    int m_bRandomPosition;
    float m_fRandomRangeX;
    float m_fRandomRangeY;
    CExoArrayList<CResRef> m_lstSounds;
    unsigned long m_nInterval;
    unsigned long m_nIntervalVariation;
    float m_fMinDistance;
    float m_fMaxDistance;
    int m_bContinuous;
    int m_bRandomOrder;

public:

    void SetPositional(int nPositional)
    {
        m_bPositional = nPositional;
    }
    void SetLooping(int nLooping)
    {
        m_bLooping = nLooping;
    }
    void SetVolume(int nVolume)
    {
        m_nVolume = nVolume;
    }
    void SetVolumeVariation(int nVolumeVariation)
    {
        m_nVolumeVariation = nVolumeVariation;
    }
    void SetPitchVariation(float nPitchVariation)
    {
        m_fPitchVariation = nPitchVariation;
    }
    void SetHours(unsigned long nHours)
    {
        m_nHours = nHours;
    }
    void SetInterval(unsigned long nInterval)
    {
        m_nInterval = nInterval;
    }
    void SetMinDistance(float nMinDistance)
    {
        m_fMinDistance = nMinDistance;
    }
    void SetMaxDistance(float nMaxDistance)
    {
        m_fMaxDistance = nMaxDistance;
    }
    void SetContinuous(int nContinuous)
    {
        m_bContinuous = nContinuous;
    }
    void SetRandomPosition(int nRandomPosition)
    {
        m_bRandomPosition = nRandomPosition;
    }
    int GetPositional()
    {
        return m_bPositional;
    }
    int GetLooping()
    {
        return m_bLooping;
    }
    int GetVolume()
    {
        return m_nVolume;
    }
    int GetVolumeVariation()
    {
        return m_nVolumeVariation;
    }
    float GetPitchVariation()
    {
        return m_fPitchVariation;
    }
    unsigned long GetHours()
    {
        return m_nHours;
    }
    unsigned long GetInterval()
    {
        return m_nInterval;
    }
    float GetMinDistance()
    {
        return m_fMinDistance;
    }
    float GetMaxDistance()
    {
        return m_fMaxDistance;
    }
    int GetContinuous()
    {
        return m_bContinuous;
    }
    int GetRandomPosition()
    {
        return m_bRandomPosition;
    }
};

#endif
