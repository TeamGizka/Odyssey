#ifndef __NWCSOUNDOBJECT_H__
#define __NWCSOUNDOBJECT_H__

#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "nwcobject.h"

class CExoSoundSource;
class CExoStreamingSoundSource;
class CSWCMessage;
class CSWCArea;
class CGameObjectArray;

class CSWCSoundObject : public CSWCObject
{
public:
    static const int MAX_VOLUME;

    enum
    {
        TIME_OF_DAY_CUSTOM = 0,
        TIME_OF_DAY_DAY = 1,
        TIME_OF_DAY_NIGHT = 2,
        TIME_OF_DAY_ALWAYS = 3
    };

    CSWCSoundObject();
    virtual ~CSWCSoundObject();
    // PC 0x00641DB0
    virtual CSWCSoundObject* AsSWCSoundObject()
    {
        return this;
    }

    virtual void AIUpdate();
    virtual void SetPosition(const Vector& vPosition)  // PC 0x0066B310  Xbox 0x000BAF30
    {
        CSWCObject::SetPosition(vPosition);
        if (m_bStreaming == 0 && m_pSoundSource != NULL)
        {
            m_pSoundSource->SetPosition(vPosition, 0.0f);
        }
        else if (m_pStreamingSource != NULL)
        {
            m_pStreamingSource->SetPosition(vPosition);
        }
    }
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,  // PC 0x0066B360
                          unsigned char nFlags)
    {
        return 0;
    }
    virtual void UnloadModel(unsigned char nModel)  // PC 0x0060E760
    {
    }
    virtual void UnloadModel()  // PC 0x004015A0
    {
    }

    void LoadFromMessage(CSWCMessage* pMessage);
    int AddToArea(CSWCArea* pArea, CGameObjectArray* pObjectArray);
    void ChangeVolume(int nVolume);
    void ChangePitchVariation(float fVariation);
    void ChangeFixedVariance(float fVariance);
    void ChangePosition(const Vector& vPosition);
    void SetIsActive(int bActive);
    void SetPositional(int bPositional)
    {
        if (m_bStreaming == 0 && m_pSoundSource != NULL)
        {
            m_pSoundSource->Set3D(bPositional);
        }
        else if (m_pStreamingSource != NULL)
        {
            m_pStreamingSource->Set3D(bPositional);
        }
    }
    void SetLooping(int bLooping)
    {
        if (m_bStreaming == 0 && m_pSoundSource != NULL)
        {
            m_pSoundSource->SetLooping(bLooping);
        }
        else if (m_pStreamingSource != NULL)
        {
            m_pStreamingSource->SetLooping(bLooping);
        }
    }
    int GetLooping();
    void SetDistance(float fMin, float fMax)
    {
        if (m_bStreaming == 0 && m_pSoundSource != NULL)
        {
            m_pSoundSource->SetDistance(fMin, fMax);
        }
        else if (m_pStreamingSource != NULL)
        {
            m_pStreamingSource->SetDistance(fMin, fMax);
        }
    }
    void UpdateTimeOfDay();
    unsigned long GetTimeDiff();
    void UpdateVolumeVariation();
    void UpdateIntervalVariation();
    void ResetLastUpdate();
    void Play();
    void Stop();
    void FadeAndStop(unsigned long nFadeTime);
    void ShuffleList();
    CResRef GetNextSound();

    CExoSoundSource* m_pSoundSource;
    CExoStreamingSoundSource* m_pStreamingSource;
    int m_bPlaying;
    int m_nVolume;
    int m_nVolumeVariation;
    int m_nTimeOfDay;
    int m_bActiveNow;
    float m_fHeight;
    float m_fPitchVariation;
    int m_nHourMask;
    float m_fFixedVariance;
    int m_bRestart;
    unsigned char m_nGeneratedType;
    int m_bRandomPosition;
    float m_fRandomRangeX;
    float m_fRandomRangeY;
    CExoArrayList<CResRef>* m_pSounds;
    int m_nCurrentSound;
    int m_nInterval;
    int m_nIntervalVariation;
    int m_nIntervalTimer;
    int m_nSoundTimer;
    int m_bContinuous;
    int m_bRandomOrder;
    int m_bFirstSound;
    unsigned long m_nLastUpdateDay;
    unsigned long m_nLastUpdateTime;
    int m_bStreaming;
};

#endif
