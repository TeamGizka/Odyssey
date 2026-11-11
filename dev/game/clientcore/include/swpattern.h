#ifndef __SWPATTERN_H__
#define __SWPATTERN_H__

class CSWRumbleMotorData
{
public:
    float* m_pfMagnitudes;
    float* m_pfTimes;
    int m_nNumSamplePoints;
    int m_nCurrentSamplePoint;

    float GetMagnitude(float fTime, int bLoop);

    float GetSampleTime(int nSample)
    {
        if (nSample < 0 || nSample >= m_nNumSamplePoints)
        {
            return 0.0f;
        }
        return m_pfTimes[nSample];
    }
#ifdef _XBOX

    void SetNumSamplePoints(int nSamples);
    void SetSamplePoint(int nSample, float fMagnitude, float fTime)
    {
        if (nSample >= 0 && nSample < m_nNumSamplePoints && m_pfTimes != NULL && m_pfMagnitudes != NULL)
        {
            m_pfMagnitudes[nSample] = fMagnitude;
            m_pfTimes[nSample] = fTime;
        }
    }
#endif
};

class CSWRumblePattern
{
public:
    CSWRumbleMotorData m_aMotorData[2];
    int m_bLoop;

    int GetMagnitudes(float fTime, float& fLeftMagnitude,
                      float& fRightMagnitude);
#ifdef _XBOX
    void LoadPattern(int nRow, class C2DA* p2DA);
#endif
};

#endif
