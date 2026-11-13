#include "swpattern.h"
// PC 0x006E91E0 GOG 0x006E8EA0
CSWRumbleMotorData::~CSWRumbleMotorData()
{
    delete m_pfMagnitudes;
    delete m_pfTimes;
}
// PC 0x006E9200 GOG 0x006E8EC0
float CSWRumbleMotorData::GetMagnitude(float fTime, int bLoop)
{
    if (!m_pfTimes || !m_pfMagnitudes)
    {
        return 0.0f;
    }

    if (bLoop)
    {
        while (fTime > m_pfTimes[m_nNumSamplePoints - 1])
        {
            fTime -= m_pfTimes[m_nNumSamplePoints - 1];
        }
    }
    else if (fTime > m_pfTimes[m_nNumSamplePoints - 1])
    {
        return 0.0f;
    }

    int bFound = 0;
    for (int nSample = m_nCurrentSamplePoint;
         nSample < m_nNumSamplePoints - 1 && !bFound; ++nSample)
    {
        if (fTime >= m_pfTimes[nSample] && fTime < m_pfTimes[nSample + 1])
        {
            bFound = 1;
            m_nCurrentSamplePoint = nSample;
        }
    }

    for (nSample = 0; nSample < m_nCurrentSamplePoint && !bFound; ++nSample)
    {
        if (fTime >= m_pfTimes[nSample] && fTime < m_pfTimes[nSample + 1])
        {
            bFound = 1;
            m_nCurrentSamplePoint = nSample;
        }
    }

    if (!bFound)
    {
        return 0.0f;
    }

    return (fTime - m_pfTimes[m_nCurrentSamplePoint]) /
               (m_pfTimes[m_nCurrentSamplePoint + 1] -
                m_pfTimes[m_nCurrentSamplePoint]) *
               (m_pfMagnitudes[m_nCurrentSamplePoint + 1] -
                m_pfMagnitudes[m_nCurrentSamplePoint]) +
           m_pfMagnitudes[m_nCurrentSamplePoint];
}
// PC 0x006E9320 GOG 0x006E8FE0
int CSWRumblePattern::GetMagnitudes(float fTime, float& fLeftMagnitude,
                                    float& fRightMagnitude)
{
    fLeftMagnitude = m_LeftMotor.GetMagnitude(fTime, m_bLoop);
    fRightMagnitude = m_RightMotor.GetMagnitude(fTime, m_bLoop);

    if (m_bLoop == 0)
    {
        if (!(fTime < m_LeftMotor.GetSampleTime(
                          m_LeftMotor.m_nNumSamplePoints - 1)))
        {
            if (!(fTime < m_RightMotor.GetSampleTime(
                              m_RightMotor.m_nNumSamplePoints - 1)))
            {
                return 0;
            }
        }
    }

    return 1;
}

#ifdef _XBOX
#include "twodimarray.h"

void CSWRumbleMotorData::SetNumSamplePoints(int nSamples)
{
    if (nSamples <= 1)
    {
        m_nNumSamplePoints = 0;
        return;
    }
    m_nNumSamplePoints = nSamples;
    m_pfMagnitudes = new float[nSamples];
    m_pfTimes = new float[m_nNumSamplePoints];
    for (int i = 0; i < m_nNumSamplePoints; i++)
    {
        m_pfMagnitudes[i] = 0.0f;
        m_pfTimes[i] = 0.0f;
    }
}

void CSWRumblePattern::LoadPattern(int nRow, C2DA* p2DA)
{
    if (nRow < p2DA->GetNumRows())
    {
        CExoString sColumn;
        int nSamples;
        float fMagnitude;
        float fTime;
        int i;

        {
            CExoString sLooping("Looping");
            p2DA->GetINTEntry(nRow, sLooping, &m_bLoop);
        }
        {
            CExoString sSamples("LSamples");
            p2DA->GetINTEntry(nRow, sSamples, &nSamples);
        }
        m_LeftMotor.SetNumSamplePoints(nSamples);
        for (i = 0; i < m_LeftMotor.m_nNumSamplePoints; i++)
        {
            sColumn.Format("LMagnitude%d", i + 1);
            p2DA->GetFLOATEntry(nRow, sColumn, &fMagnitude);
            sColumn.Format("LTime%d", i + 1);
            p2DA->GetFLOATEntry(nRow, sColumn, &fTime);
            m_LeftMotor.SetSamplePoint(i, fMagnitude, fTime);
        }

        {
            CExoString sSamples("RSamples");
            p2DA->GetINTEntry(nRow, sSamples, &nSamples);
        }
        m_RightMotor.SetNumSamplePoints(nSamples);
        for (i = 0; i < m_RightMotor.m_nNumSamplePoints; i++)
        {
            sColumn.Format("RMagnitude%d", i + 1);
            p2DA->GetFLOATEntry(nRow, sColumn, &fMagnitude);
            sColumn.Format("RTime%d", i + 1);
            p2DA->GetFLOATEntry(nRow, sColumn, &fTime);
            m_RightMotor.SetSamplePoint(i, fMagnitude, fTime);
        }
    }
}
#endif
