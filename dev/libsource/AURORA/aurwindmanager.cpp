#include "aurwindmanager.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
// PC 0x004D1E90
void WindManager::SetMaximumDeviation(float fMaximumDeviation, float fVariation)
{
    m_fMaximumDeviation = fMaximumDeviation;
    m_fMaximumDeviationVariation = fVariation;
}
// PC 0x004D1EB0
void WindManager::fillNoiseField(float fField[16][16])
{
    memset(fField, 0, sizeof(float) * 16 * 16);
    for (int nBlockSize = 16; nBlockSize >= 1; nBlockSize /= 2)
    {
        for (int y = 0, yEnd = nBlockSize; yEnd <= 16;
             y = yEnd, yEnd += nBlockSize)
        {
            for (int x = 0, xEnd = nBlockSize; xEnd <= 16;
                 x = xEnd, xEnd += nBlockSize)
            {
                float fNoise = (rand() % (500 / (16 / nBlockSize))) * 0.001f;
                for (int yy = y; yy < yEnd; yy++)
                {
                    for (int xx = x; xx < xEnd; xx++)
                    {
                        fField[yy][xx] += fNoise;
                    }
                }
            }
        }
    }
}
// PC 0x004D1FB0
PointSourceWind::PointSourceWind()
{
    m_vPosition.m_fX = 0.0f;
    m_vPosition.m_fY = 0.0f;
    m_vPosition.m_fZ = 0.0f;

    Vector vPosition;
    vPosition.m_fX = 0.0f;
    vPosition.m_fY = 0.0f;
    vPosition.m_fZ = 0.0f;
    m_vPosition = vPosition;

    m_fParameter0 = 0.0f;
    m_fParameter1 = 0.0f;
    m_fParameter2 = 0.0f;
}
// PC 0x004015A0
PointSourceWind::~PointSourceWind()
{
}
// PC 0x004D1FF0
void WindManager::SetGlobalWind(Vector vWind)
{
    m_vGlobalWind = vWind;

    for (int i = 0; i < m_lstWindyEmitters.m_nCount; i++)
    {
        m_lstWindyEmitters[i]->CalculateRadius();
    }
}
// PC 0x004D2040
Vector WindManager::GetGlobalWindNoPoint(const Vector& vPosition, float fScale)
{
    Vector* pGlobalWind = &m_vGlobalWind;
    if (pGlobalWind->m_fX == 0.0f && pGlobalWind->m_fY == 0.0f &&
        pGlobalWind->m_fZ == 0.0f)
    {
        return m_vGlobalWind;
    }

    int nX = abs((int)vPosition.m_fX * 4) % 16;
    int nY = abs((int)vPosition.m_fY * 4) % 16;
    float fXAngle = 0.0f;
    float fYAngle = 0.0f;

    static Quaternion q;

    float fNoise;
    {
        float (*pCurrent)[16] = m_pCurrentNoiseField;
        float (*pNext)[16] = m_pNextNoiseField;
        float fCurrentWeight = 1.0f - m_fNoiseInterpolation;
        fNoise = fCurrentWeight * pCurrent[nX][nY] +
                 pNext[nX][nY] * m_fNoiseInterpolation;
    }
    float fWindScale = fScale * fNoise;
    Vector vWind = fWindScale * *pGlobalWind;

    if (m_fMaximumDeviation != 0.0f)
    {
        fXAngle =
            (fNoise * m_fMaximumDeviation * 2.0f - m_fMaximumDeviation) * 0.5f;
    }

    if (m_fMaximumDeviationVariation != 0.0f)
    {
        float fVariationNoise =
            (1.0f - m_fNoiseInterpolation) * m_pCurrentNoiseField[nY][nX] +
            m_pNextNoiseField[nY][nX] * m_fNoiseInterpolation;
        float fVariation = fVariationNoise * m_fMaximumDeviationVariation;
        fYAngle = (fVariation * 2.0f - m_fMaximumDeviationVariation) * 0.5f;
    }

    float fCosX = cosineLUT(fXAngle);
    float fSinX = sineLUT(fXAngle);
    float fCosY = cosineLUT(fYAngle);
    float fSinY = sineLUT(fYAngle);
    q.m_fW = fCosY * fCosX;
    q.m_fX = fCosX * fSinY;
    q.m_fY = fSinY * fSinX;
    q.m_fZ = fCosY * fSinX;

    float fXX = q.m_fX * q.m_fX;
    float fYY = q.m_fY * q.m_fY;
    float fZZ = q.m_fZ * q.m_fZ;
    float fXY = q.m_fY * q.m_fX;
    float fZX = q.m_fZ * q.m_fX;
    float fXW = q.m_fX * q.m_fW;
    float fZY = q.m_fZ * q.m_fY;
    float fYW = q.m_fY * q.m_fW;
    float fZW = q.m_fZ * q.m_fW;

    return Vector((1.0f - 2.0f * (fZZ + fYY)) * vWind.m_fX +
                      (2.0f * (fXY - fZW)) * vWind.m_fY +
                      (2.0f * (fYW + fZX)) * vWind.m_fZ,
                  (1.0f - 2.0f * (fZZ + fXX)) * vWind.m_fY +
                      (2.0f * (fZW + fXY)) * vWind.m_fX +
                      (2.0f * (fZY - fXW)) * vWind.m_fZ,
                  (1.0f - 2.0f * (fYY + fXX)) * vWind.m_fZ +
                      (2.0f * (fZX - fYW)) * vWind.m_fX +
                      (2.0f * (fZY + fXW)) * vWind.m_fY);
}
// PC 0x004D2750
Vector WindManager::GetGlobalWind(Vector vPosition, float fScale)
{
    if (m_vGlobalWind.IsZero() && pointSourceWindVector(vPosition).IsZero())
    {
        return m_vGlobalWind;
    }

    int nX = abs((int)vPosition.m_fX * 4) % 16;
    int nY = abs((int)vPosition.m_fY * 4) % 16;
    float fXAngle = 0.0f;
    float fYAngle = 0.0f;

    static Quaternion q;

    float fNoise =
        ((1.0f - m_fNoiseInterpolation) * m_pCurrentNoiseField[nX][nY] +
         m_pNextNoiseField[nX][nY] * m_fNoiseInterpolation);
    Vector vWind = fScale * fNoise * m_vGlobalWind;
    Vector vTotalWind = vWind + pointSourceWindVector(vPosition);

    if (m_fMaximumDeviation != 0.0f)
    {
        fXAngle =
            (fNoise * m_fMaximumDeviation * 2.0f - m_fMaximumDeviation) * 0.5f;
    }

    if (m_fMaximumDeviationVariation != 0.0f)
    {
        float fVariationNoise =
            (1.0f - m_fNoiseInterpolation) * m_pCurrentNoiseField[nY][nX] +
            m_pNextNoiseField[nY][nX] * m_fNoiseInterpolation;
        float fVariation = fVariationNoise * m_fMaximumDeviationVariation;
        fYAngle = (fVariation * 2.0f - m_fMaximumDeviationVariation) * 0.5f;
    }

    float fCosX = cosineLUT(fXAngle);
    float fSinX = sineLUT(fXAngle);
    float fCosY = cosineLUT(fYAngle);
    float fSinY = sineLUT(fYAngle);
    q.m_fW = fCosY * fCosX;
    q.m_fX = fCosX * fSinY;
    q.m_fY = fSinY * fSinX;
    q.m_fZ = fCosY * fSinX;

    float fXX = q.m_fX * q.m_fX;
    float fYY = q.m_fY * q.m_fY;
    float fZZ = q.m_fZ * q.m_fZ;
    float fXY = q.m_fY * q.m_fX;
    float fZX = q.m_fZ * q.m_fX;
    float fXW = q.m_fX * q.m_fW;
    float fZY = q.m_fZ * q.m_fY;
    float fYW = q.m_fY * q.m_fW;
    float fZW = q.m_fZ * q.m_fW;

    return Vector((1.0f - 2.0f * (fZZ + fYY)) * vTotalWind.m_fX +
                      (2.0f * (fXY - fZW)) * vTotalWind.m_fY +
                      (2.0f * (fYW + fZX)) * vTotalWind.m_fZ,
                  (1.0f - 2.0f * (fZZ + fXX)) * vTotalWind.m_fY +
                      (2.0f * (fZW + fXY)) * vTotalWind.m_fX +
                      (2.0f * (fZY - fXW)) * vTotalWind.m_fZ,
                  (1.0f - 2.0f * (fYY + fXX)) * vTotalWind.m_fZ +
                      (2.0f * (fZX - fYW)) * vTotalWind.m_fX +
                      (2.0f * (fZY + fXW)) * vTotalWind.m_fY);
}
// PC 0x004D2B40
Vector WindManager::GetGlobalPointWind(const Vector& vPosition, float)
{
    return pointSourceWindVector(vPosition);
}
// PC 0x004D2370
Vector WindManager::pointSourceWindVector(Vector vPosition)
{
    float fDistance;
    Vector vResult(0.0f, 0.0f, 0.0f);
    Vector vOffset(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < m_lstPointSourceWinds.m_nCount; i++)
    {
        vOffset = vPosition - m_lstPointSourceWinds[i].m_vPosition;
        fDistance = (vOffset.m_fX < 0.0f) ? -vOffset.m_fX : vOffset.m_fX;

        if (((vOffset.m_fY < 0.0f) ? -vOffset.m_fY : vOffset.m_fY) > fDistance)
        {
            if (vOffset.m_fY < 0.0f)
            {
                fDistance = -vOffset.m_fY;
            }
            else
            {
                fDistance = vOffset.m_fY;
            }
        }

#ifdef _XBOX
        if (((vOffset.m_fZ < 0.0f) ? -vOffset.m_fZ : vOffset.m_fZ) > fDistance)
#else
        if (fabs(vOffset.m_fZ) > fDistance)
#endif
        {
            if (vOffset.m_fZ < 0.0f)
            {
                fDistance = -vOffset.m_fZ;
            }
            else
            {
                fDistance = vOffset.m_fZ;
            }
        }

        if (fDistance < m_lstPointSourceWinds[i].m_fParameter0)
        {
            vResult += ((m_lstPointSourceWinds[i].m_fParameter0 - fDistance) /
                        m_lstPointSourceWinds[i].m_fParameter0 *
                        m_lstPointSourceWinds[i].m_fParameter2) *
                       normalize(vOffset);
        }
    }

    return vResult;
}
// PC 0x004D2530
void WindManager::updateTimer(float fTime)
{
    m_fNoiseTimer += fTime;
    if (m_fNoiseTimer > m_fNoiseInterval)
    {
        float (*pNoiseField)[16] = m_pCurrentNoiseField;
        m_pCurrentNoiseField = m_pNextNoiseField;
        m_pNextNoiseField = pNoiseField;
        fillNoiseField(pNoiseField);
        m_fNoiseTimer = 0.0f;
    }

    m_fNoiseInterpolation = m_fNoiseTimer / m_fNoiseInterval;

    m_vWindVelocity = fTime * m_vGlobalWind;

    for (int i = 0; i < m_lstPointSourceWinds.m_nCount; i++)
    {
        m_lstPointSourceWinds[i].m_fParameter1 -= fTime;
        if (m_lstPointSourceWinds[i].m_fParameter1 < 0.0f)
        {
#ifdef _XBOX
            m_lstPointSourceWinds.DelIndex(i);
#else
            int j = i;
            m_lstPointSourceWinds.m_nCount--;
            for (; j < m_lstPointSourceWinds.m_nCount; j++)
            {
                m_lstPointSourceWinds[j] = m_lstPointSourceWinds[j + 1];
            }
#endif
            i--;
        }
    }
}
// PC 0x004D2B70
void WindManager::RegisterWindyEmitter(PartEmitter* pEmitter)
{
    m_lstWindyEmitters.AddUnique(pEmitter);
}
// PC 0x004D2B80
void WindManager::DeregisterWindyEmitter(PartEmitter* pEmitter)
{
    if (m_lstWindyEmitters.Contains(pEmitter))
    {
        m_lstWindyEmitters.Remove(pEmitter);
    }
}
// PC 0x004D2C10
void WindManager::AddPointSourceWind(Vector vPosition, float fParameter0,
                                    float fParameter1, float fParameter2)
{
#ifdef _XBOX
    m_lstPointSourceWinds.Add(PointSourceWind(vPosition, fParameter0, fParameter1, fParameter2));
#else
    PointSourceWind wind(vPosition, fParameter0, fParameter1, fParameter2);
    m_lstPointSourceWinds.Add(wind);
#endif
}
// PC 0x004D2D40
WindManager::WindManager() : m_lstWindyEmitters(0)
{
    m_vGlobalWind = m_vWindVelocity = Vector();

    m_fMaximumDeviation = 0.0f;
    m_fMaximumDeviationVariation = 0.0f;
    m_fNoiseTimer = 0.0f;
    m_fNoiseInterval = 2.0f;

    fillNoiseField(m_fNoiseFields[0]);
    fillNoiseField(m_fNoiseFields[1]);
    m_pCurrentNoiseField = m_fNoiseFields[0];
    m_pNextNoiseField = m_fNoiseFields[1];
}
// PC 0x004D2E10
WindManager::~WindManager()
{
}

#ifdef _XBOX
PointSourceWind::PointSourceWind(Vector vPosition, float fParameter0, float fParameter1,
                                 float fParameter2)
{
    m_vPosition = vPosition;
    m_fParameter0 = fParameter0;
    m_fParameter1 = fParameter1;
    m_fParameter2 = fParameter2;
}
#endif
