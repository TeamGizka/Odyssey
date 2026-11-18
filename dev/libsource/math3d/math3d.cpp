#include "math3d.h"
#include <math.h>
#ifdef _XBOX
#include <stdlib.h>
#endif

int quaternionidentityoptimization = 1;

unsigned int topMask[5] = {0xffff0000, 0xff00ff00, 0xf0f0f0f0, 0xcccccccc,
                           0xaaaaaaaa};

Vector xaxis(1.0f, 0.0f, 0.0f);
Vector yaxis(0.0f, 1.0f, 0.0f);
Vector zaxis(0.0f, 0.0f, 1.0f);

// Dead code: not in the PC executable
float clampf(float fValue)
{
    if (fValue < 0.0f)
    {
        return 0.0f;
    }
    if (fValue > 1.0f)
    {
        return 1.0f;
    }
    return fValue;
}
// PC 0x004D6960 GOG 0x004D6AA0 Xbox 0x0028E660
float DegToRad(float fDegrees)
{
    return fDegrees * DEG_TO_RAD;
}

#define SEGMENT_SMALL_NUM 0.00000001
#define SEGMENT_SMALL_NUM_F 0.00000001f
#define SEGMENT_DOT(a, b) ((a).m_fX * (b).m_fX + (a).m_fY * (b).m_fY + (a).m_fZ * (b).m_fZ)
inline float SegmentAbs(float x) { return x < 0 ? -x : x; }  // Xbox 0x0028E6E0
#define SEGMENT_MAX(a, b) ((a) > (b) ? (a) : (b))
// PC 0x004D6970 GOG 0x004D6AB0 Xbox 0x0028FD90
double Round(float fValue, float fRoundTo)
{
    return floor(fValue / fRoundTo + 0.5f) * fRoundTo;
}

// Dead code: not in the PC executable
int argmin(float* pValues, int nValues)
{
    int nMinimum = 0;
    for (int i = 1; i < nValues; i++)
    {
        if (pValues[i] < pValues[nMinimum])
        {
            nMinimum = i;
        }
    }
    return nMinimum;
}
// PC 0x004D69A0 GOG 0x004D6AE0 Xbox 0x0028E670
float SphereAbovePlane(const Vector& vCenter, float fRadius, const Plane& plane)
{
    return vCenter.m_fX * plane.m_vNormal.m_fX +
           vCenter.m_fY * plane.m_vNormal.m_fY +
           vCenter.m_fZ * plane.m_vNormal.m_fZ + plane.m_fDistance - fRadius;
}

static inline float interpolateLinear(float fStart, float fEnd, float fPosition)
{
    return fStart * (1.0f - fPosition) + fEnd * fPosition;
}

// PC 0x004D69D0 GOG 0x004D6B10 Xbox 0x0028E6A0
float PlaneHeightAtPoint(const Vector& vNormal, float fDistance,
                         const Vector& vPoint)
{
    if (vNormal.m_fZ == 0.0f)
    {
        return 0.0f;
    }
    return -(vNormal.m_fX * vPoint.m_fX + vNormal.m_fY * vPoint.m_fY +
             fDistance) /
           vNormal.m_fZ;
}

// Dead code: not in the PC executable
Vector planeproject(const Vector& vNormal, float fDistance,
                    const Vector& vPoint)
{
    return vPoint -
           vNormal * (vPoint.m_fX * vNormal.m_fX + vPoint.m_fY * vNormal.m_fY +
                      vPoint.m_fZ * vNormal.m_fZ + fDistance);
}
// PC 0x004D6A10 GOG 0x004D6B50 Xbox 0x002908C0
Vector operator*(const Matrix& matrix, const Vector& vector)
{
    return Vector(matrix.m_vFirst.m_fX * vector.m_fX +
                      matrix.m_vSecond.m_fX * vector.m_fY +
                      matrix.m_vThird.m_fX * vector.m_fZ,
                  matrix.m_vFirst.m_fY * vector.m_fX +
                      matrix.m_vSecond.m_fY * vector.m_fY +
                      matrix.m_vThird.m_fY * vector.m_fZ,
                  matrix.m_vFirst.m_fZ * vector.m_fX +
                      matrix.m_vSecond.m_fZ * vector.m_fY +
                      matrix.m_vThird.m_fZ * vector.m_fZ);
}
// PC 0x004D6A70 GOG 0x004D6BB0 Xbox 0x00291050
float Yaw(const Quaternion& quaternion)
{
    static Vector v(0.0f, 0.0f, 0.0f);
    v = Vector(2.0f * (quaternion.m_fY * quaternion.m_fX -
                                quaternion.m_fZ * quaternion.m_fW),
                        1.0f - 2.0f * (quaternion.m_fZ * quaternion.m_fZ +
                                       quaternion.m_fX * quaternion.m_fX),
                        2.0f * (quaternion.m_fY * quaternion.m_fZ +
                                quaternion.m_fW * quaternion.m_fX));
#ifdef _XBOX
    return Yaw(v);
#else
    if (v.m_fY == 0.0 && v.m_fX == 0.0)
    {
        return 0.0f;
    }
    return (float)atan2(-v.m_fX, v.m_fY) * RAD_TO_DEG;
#endif
}
// PC 0x004D6B50 GOG 0x004D6C90 Xbox 0x0028E7C0
#ifdef _XBOX
inline
#endif
float Yaw(const Vector& vector)
{
#ifdef _XBOX
    float fYaw;
    if (vector.m_fY == 0.0 && vector.m_fX == 0.0)
    {
        fYaw = 0.0f;
    }
    else
    {
        fYaw = (float)atan2(-vector.m_fX, vector.m_fY) * RAD_TO_DEG;
    }
    return fYaw;
#else
    if (vector.m_fY == 0.0 && vector.m_fX == 0.0)
    {
        return 0.0f;
    }
    return (float)atan2(-vector.m_fX, vector.m_fY) * RAD_TO_DEG;
#endif
}
// PC 0x004D6B90 GOG 0x004D6CD0 Xbox 0x0028E810
int polyhit(const Vector* pVertices, int nVertices, const Vector& vNormal,
            const float& fDistance, const Vector& vStart, const Vector& vEnd,
            Vector* pHit)
{

    float fStart = vNormal * vStart + fDistance;
    float fEnd = vNormal * vEnd + fDistance;
    if (fStart < 0.0f || fEnd > 0.0f || fStart == fEnd)
    {
        return 0;
    }

    static Vector the_point(0.0f, 0.0f, 0.0f);
    static Vector side(0.0f, 0.0f, 0.0f);
    float fPosition = fStart / (fStart - fEnd);
    the_point = (1.0f - fPosition) * vStart + fPosition * vEnd;
    int nHit = 1;
    for (int i = 0; nHit && i < nVertices; i++)
    {
        side = cross(pVertices[(i + 1) % nVertices] - pVertices[i],
                     the_point - pVertices[i]);
        nHit = side * vNormal >= -POLYHIT_EDGE_TOLERANCE;
    }
    if (nHit && pHit)
    {
        *pHit = the_point;
    }
    return nHit;
}

// Dead code: not in the PC executable
int BoxInside(const Vector& vPoint, const Vector& vMinimum, const Vector& vMaximum)
{
    return vPoint.m_fX >= vMinimum.m_fX && vPoint.m_fX <= vMaximum.m_fX &&
           vPoint.m_fY >= vMinimum.m_fY && vPoint.m_fY <= vMaximum.m_fY &&
           vPoint.m_fZ >= vMinimum.m_fZ && vPoint.m_fZ <= vMaximum.m_fZ;
}
// PC 0x004D6E10 GOG 0x004D6F50
#ifdef _XBOX
inline
#endif
void SetOpCode(const Vector& vPoint, const Vector& vMinimum,
               const Vector& vMaximum, unsigned char& nOutCode)
{
    nOutCode = 0;
    if (vPoint.m_fX < vMinimum.m_fX)
        nOutCode = 0x20;
    else if (vPoint.m_fX > vMaximum.m_fX)
        nOutCode = 0x10;
    if (vPoint.m_fY < vMinimum.m_fY)
        nOutCode |= 0x08;
    else if (vPoint.m_fY > vMaximum.m_fY)
        nOutCode |= 0x04;
    if (vPoint.m_fZ < vMinimum.m_fZ)
        nOutCode |= 0x02;
    else if (vPoint.m_fZ > vMaximum.m_fZ)
        nOutCode |= 0x01;
}
// PC 0x004D6EA0 GOG 0x004D6FE0 Xbox 0x0028EAC0
int BoxIntersect(Vector const& vStart, Vector const& vEnd,
                 Vector const& vMinimum, Vector const& vMaximum, Vector* vHit)
{
    unsigned char nCode1;
    SetOpCode(vStart, vMinimum, vMaximum, nCode1);
    if (nCode1 == 0)
        return -1;

    unsigned char nCode2;
    SetOpCode(vEnd, vMinimum, vMaximum, nCode2);
    if ((nCode2 & nCode1) != 0)
        return 0;

    if (nCode1 & 0x20)
    {
        float fT = (vMinimum.m_fX - vStart.m_fX) / (vEnd.m_fX - vStart.m_fX);
        float fY = (1.0f - fT) * vStart.m_fY + fT * vEnd.m_fY;
        float fZ = (1.0f - fT) * vStart.m_fZ + fT * vEnd.m_fZ;
        if (fY >= vMinimum.m_fY && fY <= vMaximum.m_fY && fZ >= vMinimum.m_fZ &&
            fZ <= vMaximum.m_fZ)
        {
            vHit->m_fX = vMinimum.m_fX;
            vHit->m_fY = fY;
            vHit->m_fZ = fZ;
            return 1;
        }
    }
    else if (nCode1 & 0x10)
    {
        float fT = (vMaximum.m_fX - vStart.m_fX) / (vEnd.m_fX - vStart.m_fX);
        float fY = (1.0f - fT) * vStart.m_fY + fT * vEnd.m_fY;
        float fZ = (1.0f - fT) * vStart.m_fZ + fT * vEnd.m_fZ;
        if (fY >= vMinimum.m_fY && fY <= vMaximum.m_fY && fZ >= vMinimum.m_fZ &&
            fZ <= vMaximum.m_fZ)
        {
            vHit->m_fX = vMaximum.m_fX;
            vHit->m_fY = fY;
            vHit->m_fZ = fZ;
            return 2;
        }
    }

    if (nCode1 & 8)
    {
        float fT = (vMinimum.m_fY - vStart.m_fY) / (vEnd.m_fY - vStart.m_fY);
        float fX = (1.0f - fT) * vStart.m_fX + fT * vEnd.m_fX;
        float fZ = (1.0f - fT) * vStart.m_fZ + fT * vEnd.m_fZ;
        if (fX >= vMinimum.m_fX && fX <= vMaximum.m_fX && fZ >= vMinimum.m_fZ &&
            fZ <= vMaximum.m_fZ)
        {
            vHit->m_fX = fX;
            vHit->m_fY = vMinimum.m_fY;
            vHit->m_fZ = fZ;
            return 3;
        }
    }
    else if (nCode1 & 4)
    {
        float fT = (vMaximum.m_fY - vStart.m_fY) / (vEnd.m_fY - vStart.m_fY);
        float fX = (1.0f - fT) * vStart.m_fX + fT * vEnd.m_fX;
        float fZ = (1.0f - fT) * vStart.m_fZ + fT * vEnd.m_fZ;
        if (fX >= vMinimum.m_fX && fX <= vMaximum.m_fX && fZ >= vMinimum.m_fZ &&
            fZ <= vMaximum.m_fZ)
        {
            vHit->m_fX = fX;
            vHit->m_fY = vMaximum.m_fY;
            vHit->m_fZ = fZ;
            return 4;
        }
    }

    if (nCode1 & 2)
    {
        float fT = (vMinimum.m_fZ - vStart.m_fZ) / (vEnd.m_fZ - vStart.m_fZ);
        float fX = (1.0f - fT) * vStart.m_fX + fT * vEnd.m_fX;
        float fY = (1.0f - fT) * vStart.m_fY + fT * vEnd.m_fY;
        if (fY >= vMinimum.m_fY && fY <= vMaximum.m_fY && fX >= vMinimum.m_fX &&
            fX <= vMaximum.m_fX)
        {
            vHit->m_fX = fX;
            vHit->m_fY = fY;
            vHit->m_fZ = vMinimum.m_fZ;
            return 5;
        }
    }
    else if (nCode1 & 1)
    {
        float fT = (vMaximum.m_fZ - vStart.m_fZ) / (vEnd.m_fZ - vStart.m_fZ);
        float fX = (1.0f - fT) * vStart.m_fX + fT * vEnd.m_fX;
        float fY = fT * vEnd.m_fY + (1.0f - fT) * vStart.m_fY;
        if (fY >= vMinimum.m_fY && fY <= vMaximum.m_fY && fX >= vMinimum.m_fX &&
            fX <= vMaximum.m_fX)
        {
            vHit->m_fX = fX;
            vHit->m_fY = fY;
            vHit->m_fZ = vMaximum.m_fZ;
            return 6;
        }
    }

    return 0;
}
// PC 0x004D72C0 GOG 0x004D7400 Xbox 0x0028EF10
int BoxIntersect(Vector const& vStart, Vector const& vEnd,
                 Vector const& vMinimum, Vector const& vMaximum)
{
    unsigned char nCode1;
    SetOpCode(vStart, vMinimum, vMaximum, nCode1);
    if (nCode1 == 0)
        return -1;

    unsigned char nCode2;
    SetOpCode(vEnd, vMinimum, vMaximum, nCode2);
    if ((nCode2 & nCode1) != 0)
        return 0;

    if (nCode1 & 0x20)
    {
        float fT = (vMinimum.m_fX - vStart.m_fX) / (vEnd.m_fX - vStart.m_fX);
        float fY = (1.0f - fT) * vStart.m_fY + fT * vEnd.m_fY;
        float fZ = (1.0f - fT) * vStart.m_fZ + fT * vEnd.m_fZ;
        if (fY >= vMinimum.m_fY && fY <= vMaximum.m_fY && fZ >= vMinimum.m_fZ &&
            fZ <= vMaximum.m_fZ)
        {
            return 1;
        }
    }
    else if (nCode1 & 0x10)
    {
        float fT = (vMaximum.m_fX - vStart.m_fX) / (vEnd.m_fX - vStart.m_fX);
        float fY = (1.0f - fT) * vStart.m_fY + fT * vEnd.m_fY;
        float fZ = (1.0f - fT) * vStart.m_fZ + fT * vEnd.m_fZ;
        if (fY >= vMinimum.m_fY && fY <= vMaximum.m_fY && fZ >= vMinimum.m_fZ &&
            fZ <= vMaximum.m_fZ)
        {
            return 2;
        }
    }

    if (nCode1 & 8)
    {
        float fT = (vMinimum.m_fY - vStart.m_fY) / (vEnd.m_fY - vStart.m_fY);
        float fX = (1.0f - fT) * vStart.m_fX + fT * vEnd.m_fX;
        float fZ = (1.0f - fT) * vStart.m_fZ + fT * vEnd.m_fZ;
        if (fX >= vMinimum.m_fX && fX <= vMaximum.m_fX && fZ >= vMinimum.m_fZ &&
            fZ <= vMaximum.m_fZ)
        {
            return 3;
        }
    }
    else if (nCode1 & 4)
    {
        float fT = (vMaximum.m_fY - vStart.m_fY) / (vEnd.m_fY - vStart.m_fY);
        float fX = (1.0f - fT) * vStart.m_fX + fT * vEnd.m_fX;
        float fZ = (1.0f - fT) * vStart.m_fZ + fT * vEnd.m_fZ;
        if (fX >= vMinimum.m_fX && fX <= vMaximum.m_fX && fZ >= vMinimum.m_fZ &&
            fZ <= vMaximum.m_fZ)
        {
            return 4;
        }
    }

    if (nCode1 & 2)
    {
        float fT = (vMinimum.m_fZ - vStart.m_fZ) / (vEnd.m_fZ - vStart.m_fZ);
        float fX = (1.0f - fT) * vStart.m_fX + fT * vEnd.m_fX;
        float fY = (1.0f - fT) * vStart.m_fY + fT * vEnd.m_fY;
        if (fY >= vMinimum.m_fY && fY <= vMaximum.m_fY && fX >= vMinimum.m_fX &&
            fX <= vMaximum.m_fX)
        {
            return 5;
        }
    }
    else if (nCode1 & 1)
    {
        float fT = (vMaximum.m_fZ - vStart.m_fZ) / (vEnd.m_fZ - vStart.m_fZ);
        float fX = (1.0f - fT) * vStart.m_fX + fT * vEnd.m_fX;
        float fY = fT * vEnd.m_fY + (1.0f - fT) * vStart.m_fY;
        if (fY >= vMinimum.m_fY && fY <= vMaximum.m_fY && fX >= vMinimum.m_fX &&
            fX <= vMaximum.m_fX)
        {
            return 6;
        }
    }

    return 0;
}
// PC 0x004D7630 GOG 0x004D7770 Xbox 0x0028F2C0
#ifdef _XBOX
inline
#endif
int equal(const Vector& vFirst, const Vector& vSecond)
{
    return vFirst.m_fX == vSecond.m_fX && vFirst.m_fY == vSecond.m_fY &&
           vFirst.m_fZ == vSecond.m_fZ;
}
// PC 0x004D7670 GOG 0x004D77B0 Xbox 0x0028F300
float ClosestDistanceBetweenSegments(const Vector& vFirstStart, const Vector& vFirstEnd,
                                     const Vector& vSecondStart, const Vector& vSecondEnd)
{
    float sc = 0.0f;
    Vector u = vFirstEnd - vFirstStart;
    Vector v = vSecondEnd - vSecondStart;
    Vector w = vFirstStart - vSecondStart;
#ifdef _XBOX
    float a = u * u;
    float b = u * v;
    float c = v * v;
    float d = u * w;
    float e = v * w;
#else
    float a = SEGMENT_DOT(u, u);
    float b = SEGMENT_DOT(u, v);
    float c = SEGMENT_DOT(v, v);
    float d = SEGMENT_DOT(u, w);
    float e = SEGMENT_DOT(v, w);
#endif
    float D = a * c - b * b;
    float sN, sD = D;
    float tc, tN, tD = D;

    if (D < SEGMENT_SMALL_NUM)
    {
        sN = 0.0f;
        sD = 1.0f;
        tN = e;
        tD = c;
    }
    else
    {
        sN = b * e - c * d;
        tN = a * e - b * d;
        if (sN < 0.0)
        {
            sN = 0.0f;
            tN = e;
            tD = c;
        }
        else if (sN > sD)
        {
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0)
    {
        tN = 0.0f;
        if (-d < 0.0)
            sN = 0.0f;
        else if (-d > a)
            sN = sD;
        else
        {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD)
    {
        tN = tD;
        if (-d + b < 0.0)
            sN = 0.0f;
        else if (-d + b > a)
            sN = sD;
        else
        {
            sN = -d + b;
            sD = a;
        }
    }

    sc = fabs(sN) < SEGMENT_SMALL_NUM_F ? 0.0f : sN / sD;
    tc = fabs(tN) < SEGMENT_SMALL_NUM_F ? 0.0f : tN / tD;

    Vector dP = w + (sc * u) - (tc * v);
    return SEGMENT_MAX(SegmentAbs(dP.m_fX),
                       SEGMENT_MAX(SegmentAbs(dP.m_fY), SegmentAbs(dP.m_fZ)));
}

// Dead code: not in the PC executable
int IsPowerOf2(int nValue)
{
    return (nValue & (nValue - 1)) == 0;
}

// Dead code: not in the PC executable
Quaternion VirtualTrackBall(const Vector& vProjectionCenter, const Vector& vRotationCenter,
                            const Vector& vFirstDirection, const Vector& vSecondDirection)
{
    Vector vNormal = vRotationCenter - vProjectionCenter;
    float fFudgeFactor = 1.0f / (magnitude(vNormal) * 0.25f);
    vNormal = normalize(vNormal);
    float fDistance = -(vNormal * vRotationCenter);

    Vector vFirst = planelineintersection(vNormal, fDistance, vProjectionCenter,
                                          vProjectionCenter + vFirstDirection);
    vFirst = vFirst - vRotationCenter;
    vFirst = vFirst * fFudgeFactor;
    float fMagnitude = magnitude(vFirst);
    if (fMagnitude > 1.0f)
    {
        vFirst /= fMagnitude;
    }
    else
    {
        vFirst = vFirst - vNormal * (float)sqrt(1.0f - fMagnitude * fMagnitude);
    }

    Vector vSecond = planelineintersection(vNormal, fDistance, vProjectionCenter,
                                           vProjectionCenter + vSecondDirection);
    vSecond = vSecond - vRotationCenter;
    vSecond = vSecond * fFudgeFactor;
    fMagnitude = magnitude(vSecond);
    if (fMagnitude > 1.0f)
    {
        vSecond /= fMagnitude;
    }
    else
    {
        vSecond = vSecond - vNormal * (float)sqrt(1.0f - fMagnitude * fMagnitude);
    }

    return RotationArc(vFirst, vSecond);
}
// PC 0x004D7A90 GOG 0x004D7BD0 Xbox 0x0028F7A0
void MakeMatrix4x4(float* pMatrix, const Vector& vTranslation,
                   const Quaternion& quaternion)
{
    float fXX = quaternion.m_fX * quaternion.m_fX;
    float fYY = quaternion.m_fY * quaternion.m_fY;
    float fZZ = quaternion.m_fZ * quaternion.m_fZ;
    float fXY = quaternion.m_fX * quaternion.m_fY;
    float fXZ = quaternion.m_fX * quaternion.m_fZ;
    float fXW = quaternion.m_fX * quaternion.m_fW;
    float fYZ = quaternion.m_fY * quaternion.m_fZ;
    float fYW = quaternion.m_fY * quaternion.m_fW;
    float fZW = quaternion.m_fZ * quaternion.m_fW;

#ifdef _XBOX
    pMatrix[0] = 1.0f - 2.0f * (fZZ + fYY);
    pMatrix[1] = 2.0f * (fZW + fXY);
    pMatrix[2] = 2.0f * (fXZ - fYW);
    pMatrix[3] = 0.0f;
    pMatrix[4] = 2.0f * (fXY - fZW);
    pMatrix[5] = 1.0f - 2.0f * (fZZ + fXX);
    pMatrix[6] = 2.0f * (fYZ + fXW);
    pMatrix[7] = 0.0f;
    pMatrix[8] = 2.0f * (fYW + fXZ);
    pMatrix[9] = 2.0f * (fYZ - fXW);
    pMatrix[10] = 1.0f - 2.0f * (fYY + fXX);
    pMatrix[11] = 0.0f;
#else
    pMatrix[3] = 0.0f;
    pMatrix[7] = 0.0f;
    pMatrix[11] = 0.0f;
    pMatrix[0] = 1.0f - 2.0f * (fZZ + fYY);
    pMatrix[1] = 2.0f * (fZW + fXY);
    pMatrix[2] = 2.0f * (fXZ - fYW);
    pMatrix[4] = 2.0f * (fXY - fZW);
    pMatrix[5] = 1.0f - 2.0f * (fZZ + fXX);
    pMatrix[6] = 2.0f * (fYZ + fXW);
    pMatrix[8] = 2.0f * (fYW + fXZ);
    pMatrix[9] = 2.0f * (fYZ - fXW);
    pMatrix[10] = 1.0f - 2.0f * (fYY + fXX);
#endif
    pMatrix[12] = vTranslation.m_fX;
    pMatrix[13] = vTranslation.m_fY;
    pMatrix[14] = vTranslation.m_fZ;
    pMatrix[15] = 1.0f;
}
// PC 0x004D7B80 GOG 0x004D7CC0 Xbox 0x0028F910
float* Homogenize(const Vector& vector, const float& fW)
{
    static float a[4];
#ifdef _XBOX
    *(Vector*)a = vector;
#else
    a[0] = vector.m_fX;
    a[1] = vector.m_fY;
    a[2] = vector.m_fZ;
#endif
    a[3] = fW;
    return a;
}
// PC 0x004D7BB0 GOG 0x004D7CF0 Xbox 0x0028F950
float InterpolateBezier(float fStart, float fEnd, float fControl1,
                        float fControl2, float fPosition)
{
    float fInverse = 1.0f - fPosition;
    float afPoints[3];

    afPoints[0] = fInverse * fStart + fPosition * fControl1;
    afPoints[1] = fInverse * fControl1 + fPosition * fControl2;
    afPoints[2] = fInverse * fControl2 + fPosition * fEnd;

    afPoints[0] = fInverse * afPoints[0] + fPosition * afPoints[1];
    afPoints[1] = fInverse * afPoints[1] + fPosition * afPoints[2];

    float fResult = fInverse * afPoints[0] + fPosition * afPoints[1];
    return fResult;
}

// Dead code: not in the PC executable
int FloorLogBase2(unsigned long nValue)  // Xbox 0x0028F9F0
{
    int nLog = 0;
    int nShift = 16;
    for (int i = 0; i < 5; i++)
    {
        if (nValue & topMask[i])
        {
            nValue &= topMask[i];
            nLog += nShift;
        }
        nShift /= 2;
    }
    return nLog;
}
// PC 0x004D7C20 GOG 0x004D7D60 Xbox 0x0028FA40
int IntersectCircle2d(const Vector& vCenter, float fRadius,
                      const Vector& vStart, const Vector& vEnd, Vector& vHit1,
                      Vector& vHit2, bool bSegment)
{
    Vector vDelta = vEnd - vStart;
    Vector vOffset = vStart - vCenter;
    float fA = vDelta * vDelta;
    float fB = 2.0f * (vOffset * vDelta);
    float fC = vOffset * vOffset - fRadius * fRadius;
    float fDiscriminant = fB * fB - 4.0f * fA * fC;

    if (fDiscriminant == 0.0f)
    {
        float fT = -fB / (2.0f * fA);
        if (fT >= 0.0f && fT <= 1.0f)
        {
            vHit1 = vStart + fT * vDelta;
            return 1;
        }
    }
    else if (fDiscriminant > 0.0f)
    {
        int nHits = 2;
        float fRoot = (float)sqrt(fDiscriminant);
        float fT = (-fB + fRoot) / (2.0f * fA);
        if (fT >= 0.0f && fT <= 1.0f)
        {
            vHit1 = vStart + fT * vDelta;
        }
        else
        {
            nHits = 1;
        }
        fT = (-fB - fRoot) / (2.0f * fA);
        if (fT >= 0.0f && fT <= 1.0f)
        {
            if (nHits == 2)
            {
                vHit2 = vStart + vDelta * fT;
            }
            else
            {
                vHit1 = vStart + vDelta * fT;
            }
        }
        else
        {
            nHits--;
        }
        return nHits;
    }
    return 0;
}
// PC 0x004D7EC0 GOG 0x004D8000 Xbox 0x0028FD10
float cosineLUT(float fAngle)
{
    static const float lookUpTable[32] = {
        1.0f,         0.98078525f,  0.9238795f,   0.8314696f,   0.70710677f,
        0.55557024f,  0.38268343f,  0.19509032f,  0.0f,         -0.19509032f,
        -0.38268343f, -0.55557024f, -0.70710677f, -0.8314696f,  -0.9238795f,
        -0.98078525f, -1.0f,        -0.98078525f, -0.9238795f,  -0.8314696f,
        -0.70710677f, -0.55557024f, -0.38268343f, -0.19509032f, 0.0f,
        0.19509032f,  0.38268343f,  0.55557024f,  0.70710677f,  0.8314696f,
        0.9238795f,   0.98078525f,
    };
    int nIndex;
    roundToInt(nIndex, fAngle * 5.092958f);
    return lookUpTable[nIndex & 31];
}
// PC 0x004D7F00 GOG 0x004D8040 Xbox 0x0028FD50
float sineLUT(float fAngle)
{
    static const float lookUpTable[32] = {
        0.0f,         0.19509032f,  0.38268343f,  0.55557024f,  0.70710677f,
        0.8314696f,   0.9238795f,   0.98078525f,  1.0f,         0.98078525f,
        0.9238795f,   0.8314696f,   0.70710677f,  0.55557024f,  0.38268343f,
        0.19509032f,  0.0f,         -0.19509032f, -0.38268343f, -0.55557024f,
        -0.70710677f, -0.8314696f,  -0.9238795f,  -0.98078525f, -1.0f,
        -0.98078525f, -0.9238795f,  -0.8314696f,  -0.70710677f, -0.55557024f,
        -0.38268343f, -0.19509032f,
    };
    int nIndex;
    roundToInt(nIndex, fAngle * 5.092958f);
    return lookUpTable[nIndex & 31];
}
// PC 0x004D7F40 GOG 0x004D8080 Xbox 0x0028FDC0
Vector VectorMin(Vector vFirst, Vector vSecond)
{
    return Vector(vFirst.m_fX < vSecond.m_fX ? vFirst.m_fX : vSecond.m_fX,
                  vFirst.m_fY < vSecond.m_fY ? vFirst.m_fY : vSecond.m_fY,
                  vFirst.m_fZ < vSecond.m_fZ ? vFirst.m_fZ : vSecond.m_fZ);
}
// PC 0x004D7FB0 GOG 0x004D80F0 Xbox 0x0028FE20
Vector VectorMax(Vector vFirst, Vector vSecond)
{
    return Vector(vFirst.m_fX > vSecond.m_fX ? vFirst.m_fX : vSecond.m_fX,
                  vFirst.m_fY > vSecond.m_fY ? vFirst.m_fY : vSecond.m_fY,
                  vFirst.m_fZ > vSecond.m_fZ ? vFirst.m_fZ : vSecond.m_fZ);
}
// PC 0x004D8020 GOG 0x004D8160 Xbox 0x0028FE80
float BoxAbovePlane(const Vector& vMinimum, const Vector& vMaximum,
                    const Plane& plane)
{
    float fZ = plane.m_vNormal.m_fZ > 0.0f ? vMinimum.m_fZ : vMaximum.m_fZ;
    float fY = plane.m_vNormal.m_fY > 0.0f ? vMinimum.m_fY : vMaximum.m_fY;
    float fX = plane.m_vNormal.m_fX > 0.0f ? vMinimum.m_fX : vMaximum.m_fX;
    return fY * plane.m_vNormal.m_fY + fX * plane.m_vNormal.m_fX +
           fZ * plane.m_vNormal.m_fZ + plane.m_fDistance;
}
// PC 0x004D8090 GOG 0x004D81D0 Xbox 0x0028FF00
Vector VectorMax(List<Vector>& vectors)
{
    Vector vResult = vectors.m_pList[0];
    for (int i = 0; i < vectors.m_nCount; i++)
    {
        if (!(vResult.m_fX > vectors.m_pList[i].m_fX))
        {
            vResult.m_fX = vectors.m_pList[i].m_fX;
        }
        if (!(vResult.m_fY > vectors.m_pList[i].m_fY))
        {
            vResult.m_fY = vectors.m_pList[i].m_fY;
        }
        if (!(vResult.m_fZ > vectors.m_pList[i].m_fZ))
        {
            vResult.m_fZ = vectors.m_pList[i].m_fZ;
        }
    }
    return vResult;
}

// Dead code: not in the PC executable
int VectorMaxDir(List<Vector>& vectors, Vector vDirection)
{
    int nMaximum = 0;
    for (int i = 0; i < vectors.m_nCount; i++)
    {
        if (vectors.m_pList[i] * vDirection > vectors.m_pList[nMaximum] * vDirection)
        {
            nMaximum = i;
        }
    }
    return nMaximum;
}
// PC 0x004D8130 GOG 0x004D8270 Xbox 0x00290090
Vector VectorMin(List<Vector>& vectors)
{
    Vector vResult = vectors.m_pList[0];
    for (int i = 0; i < vectors.m_nCount; i++)
    {
        if (!(vResult.m_fX < vectors.m_pList[i].m_fX))
        {
            vResult.m_fX = vectors.m_pList[i].m_fX;
        }
        if (!(vResult.m_fY < vectors.m_pList[i].m_fY))
        {
            vResult.m_fY = vectors.m_pList[i].m_fY;
        }
        if (!(vResult.m_fZ < vectors.m_pList[i].m_fZ))
        {
            vResult.m_fZ = vectors.m_pList[i].m_fZ;
        }
    }
    return vResult;
}

// Dead code: not in the PC executable
void Scale(List<Vector>& vectors, float fScale)
{
    for (int i = 0; i < vectors.m_nCount; i++)
    {
        vectors.m_pList[i] = vectors.m_pList[i] * fScale;
    }
}

// Dead code: not in the PC executable
float ComputeArea(List<Vector>& vectors)
{
    float fArea = 0.0f;
    for (int i = 0; i < vectors.m_nCount; i++)
    {
        Vector vFirst = vectors.m_pList[0];
        Vector vSecond = vectors.m_pList[(i + 1) % vectors.m_nCount];
        Vector vThird = vectors.m_pList[(i + 2) % vectors.m_nCount];
        fArea += magnitude(cross(vSecond - vFirst, vThird - vSecond)) * 0.5f;
    }
    return fArea;
}
// PC 0x004D9690 GOG 0x004D97D0 Xbox 0x002922F0
Vector InterpolateBezier(const Vector& vStart, const Vector& vEnd,
                         const Vector& vControl1, const Vector& vControl2,
                         float fPosition)
{
    float fInverse = 1.0f - fPosition;
    Vector avPoints[3];

    avPoints[0] = fInverse * vStart + fPosition * vControl1;
    avPoints[1] = fInverse * vControl1 + fPosition * vControl2;
    avPoints[2] = fInverse * vControl2 + fPosition * vEnd;

    avPoints[0] = fInverse * avPoints[0] + fPosition * avPoints[1];
    avPoints[1] = fInverse * avPoints[1] + fPosition * avPoints[2];

    Vector vResult = fInverse * avPoints[0] + fPosition * avPoints[1];
    return vResult;
}
// PC 0x004D98C0 GOG 0x004D9A00 Xbox 0x002925B0
int IntersectLineSegments2d(const Vector& vStart1, const Vector& vEnd1,
                            const Vector& vStart2, const Vector& vEnd2,
                            Vector& vHit)
{

#ifdef _XBOX
    float fDot = ((vEnd2.m_fX - vStart2.m_fX) * (vEnd1.m_fX - vStart1.m_fX) + (vEnd2.m_fY - vStart2.m_fY) * (vEnd1.m_fY - vStart1.m_fY));
    float fLength1 = SqrtEstimateNR_ASM((vEnd1.m_fY - vStart1.m_fY) * (vEnd1.m_fY - vStart1.m_fY) + (vEnd1.m_fX - vStart1.m_fX) * (vEnd1.m_fX - vStart1.m_fX));
    float fLength2 = SqrtEstimateNR_ASM((vEnd2.m_fX - vStart2.m_fX) * (vEnd2.m_fX - vStart2.m_fX) + (vEnd2.m_fY - vStart2.m_fY) * (vEnd2.m_fY - vStart2.m_fY));
    float fCosine = fDot / fLength1;
    if (fCosine /
        fLength2 == 1.0)
    {
        return 0;
    }
#else
    double fSquareY1 = (vEnd1.m_fY - vStart1.m_fY) * (vEnd1.m_fY - vStart1.m_fY);
    float fLength1 = (float)sqrt(fSquareY1 + (vEnd1.m_fX - vStart1.m_fX) * (vEnd1.m_fX - vStart1.m_fX));
    float fLength2 = sqrt((vEnd2.m_fX - vStart2.m_fX) * (vEnd2.m_fX - vStart2.m_fX) + (vEnd2.m_fY - vStart2.m_fY) * (vEnd2.m_fY - vStart2.m_fY));
    float fCosine = ((vEnd2.m_fX - vStart2.m_fX) * (vEnd1.m_fX - vStart1.m_fX) + (vEnd2.m_fY - vStart2.m_fY) * (vEnd1.m_fY - vStart1.m_fY)) / fLength1;
    if (fCosine /
        fLength2 == 1.0)
    {
        return 0;
    }
#endif

    bool bHit = false;
    if (vStart1.m_fX == vEnd1.m_fX)
    {
        float fSlope2 = (vEnd2.m_fY - vStart2.m_fY) / (vEnd2.m_fX - vStart2.m_fX);
        vHit.m_fX = vStart1.m_fX;
        float fY = vStart2.m_fY - fSlope2 * vStart2.m_fX +
                   fSlope2 * vStart1.m_fX;
        vHit.m_fY = fY;
        if ((vEnd2.m_fY >= vStart2.m_fY && fY >= vStart2.m_fY && fY <= vEnd2.m_fY) ||
            (vStart2.m_fY >= vEnd2.m_fY && fY >= vEnd2.m_fY && fY <= vStart2.m_fY))
        {
            bHit = true;
            if ((vEnd1.m_fY >= vStart1.m_fY && !(fY >= vStart1.m_fY && fY <= vEnd1.m_fY)) ||
                (vStart1.m_fY >= vEnd1.m_fY && !(fY >= vEnd1.m_fY && fY <= vStart1.m_fY)) ||
                (vEnd2.m_fX >= vStart2.m_fX && !(vHit.m_fX >= vStart2.m_fX && vHit.m_fX <= vEnd2.m_fX)) ||
                (vStart2.m_fX >= vEnd2.m_fX && !(vHit.m_fX >= vEnd2.m_fX && vHit.m_fX <= vStart2.m_fX)))
                bHit = false;
        }
    }
    else if (vStart2.m_fX == vEnd2.m_fX)
    {
        float fSlope1 = (vEnd1.m_fY - vStart1.m_fY) / (vEnd1.m_fX - vStart1.m_fX);
        vHit.m_fX = vStart2.m_fX;
        float fY = vStart1.m_fY - fSlope1 * vStart1.m_fX + fSlope1 * vStart2.m_fX;
        vHit.m_fY = fY;
        if ((vEnd1.m_fY >= vStart1.m_fY && fY >= vStart1.m_fY && fY <= vEnd1.m_fY) ||
            (vStart1.m_fY >= vEnd1.m_fY && fY >= vEnd1.m_fY && fY <= vStart1.m_fY))
        {
            bHit = true;
            if ((vEnd2.m_fY >= vStart2.m_fY && !(fY >= vStart2.m_fY && fY <= vEnd2.m_fY)) ||
                (vStart2.m_fY >= vEnd2.m_fY && !(fY >= vEnd2.m_fY && fY <= vStart2.m_fY)) ||
                (vEnd1.m_fX >= vStart1.m_fX && !(vHit.m_fX >= vStart1.m_fX && vHit.m_fX <= vEnd1.m_fX)) ||
                (vStart1.m_fX >= vEnd1.m_fX && !(vHit.m_fX >= vEnd1.m_fX && vHit.m_fX <= vStart1.m_fX)))
                bHit = false;
        }
    }
    else
    {
        float fSlope1 = (vEnd1.m_fY - vStart1.m_fY) / (vEnd1.m_fX - vStart1.m_fX);
        float fSlope2 = (vEnd2.m_fY - vStart2.m_fY) / (vEnd2.m_fX - vStart2.m_fX);
        if (fSlope2 - fSlope1 == 0.0)
            return 0;

        Vector vFirst = vStart1;
        Vector vSecond = vEnd1;
        Vector vThird = vStart2;
        Vector vFourth = vEnd2;
        vThird.m_fZ = 0.0f;
        vFourth.m_fZ = 0.0f;
        float fNormalY = vFirst.m_fX - vSecond.m_fX;
        Vector vNormal = Vector(vSecond.m_fY - vFirst.m_fY,
                                fNormalY, 0.0f) / fLength1;
        float fDistance = vNormal.m_fZ * 0.0f -
                          vNormal.m_fY * vFirst.m_fY -
                          vNormal.m_fX * vFirst.m_fX;
        vHit = planelineintersection(vNormal, fDistance, vThird, vFourth);
        if (AbsoluteValue(fSlope2) > 1.0f)
        {
            if ((vEnd2.m_fY >= vStart2.m_fY && vHit.m_fY >= vStart2.m_fY && vHit.m_fY <= vEnd2.m_fY) ||
                (vStart2.m_fY >= vEnd2.m_fY && vHit.m_fY >= vEnd2.m_fY && vHit.m_fY <= vStart2.m_fY))
                bHit = true;
        }
        else
        {
            if ((vEnd2.m_fX >= vStart2.m_fX && vHit.m_fX >= vStart2.m_fX && vHit.m_fX <= vEnd2.m_fX) ||
                (vStart2.m_fX >= vEnd2.m_fX && vHit.m_fX >= vEnd2.m_fX && vHit.m_fX <= vStart2.m_fX))
                bHit = true;
        }
        if (bHit)
        {
            if (AbsoluteValue(fSlope1) > 1.0f)
            {
                if ((vEnd1.m_fY >= vStart1.m_fY && !(vHit.m_fY >= vStart1.m_fY && vHit.m_fY <= vEnd1.m_fY)) ||
                    (vStart1.m_fY >= vEnd1.m_fY && !(vHit.m_fY >= vEnd1.m_fY && vHit.m_fY <= vStart1.m_fY)))
                    bHit = false;
            }
            else
            {
                if ((vEnd1.m_fX >= vStart1.m_fX && !(vHit.m_fX >= vStart1.m_fX && vHit.m_fX <= vEnd1.m_fX)) ||
                    (vStart1.m_fX >= vEnd1.m_fX && !(vHit.m_fX >= vEnd1.m_fX && vHit.m_fX <= vStart1.m_fX)))
                    bHit = false;
            }
        }
    }

    if (bHit)
    {
        float fT;
        if (AbsoluteValue(vStart2.m_fX - vEnd2.m_fX) >= AbsoluteValue(vStart2.m_fY - vEnd2.m_fY))
            fT = (vHit.m_fX - vStart2.m_fX) / (vEnd2.m_fX - vStart2.m_fX);
        else
            fT = (vHit.m_fY - vStart2.m_fY) / (vEnd2.m_fY - vStart2.m_fY);
        vHit.m_fZ = (vEnd2.m_fZ - vStart2.m_fZ) * fT + vStart2.m_fZ;
    }
    return bHit != false;
}
// PC 0x004D9EE0 GOG 0x004DA020 Xbox 0x00292C90
Quaternion::Quaternion(Vector vAxis, float fAngle)
{
    if (fAngle == 0.0f)
    {
        m_fW = 1.0f;
        m_fX = m_fY = m_fZ = 0.0f;
        return;
    }

    vAxis = normalize(vAxis);
    float fHalfAngle = fAngle * 0.5f;
    m_fW = (float)cos(fHalfAngle);
    vAxis = vAxis * (float)sin(fHalfAngle);
    m_fX = vAxis.m_fX;
    m_fY = vAxis.m_fY;
    m_fZ = vAxis.m_fZ;
}
// PC 0x004D9F80 GOG 0x004DA0C0 Xbox 0x00292DC0
Vector NormalOf(const Vector& vFirst, const Vector& vSecond,
                const Vector& vThird)
{
    Vector vNormal = cross(vSecond - vFirst, vThird - vSecond);
    float fMagnitude = magnitude(vNormal);
    if (fMagnitude == 0.0f)
    {
        return Vector(1.0f, 0.0f, 0.0f);
    }
    return vNormal * (1.0f / fMagnitude);
}

// PC 0x004DA050 GOG 0x004DA190 Xbox 0x00292EC0
Quaternion YawPitchRoll(float fYaw, float fPitch, float fRoll)
{
    fYaw *= DEG_TO_RAD;
    fPitch *= DEG_TO_RAD;
    fRoll *= DEG_TO_RAD;
    return Quaternion(zaxis, fYaw) *
           Quaternion(xaxis, fPitch) *
           Quaternion(yaxis, fRoll);
}
// PC 0x004DA210 GOG 0x004DA350 Xbox 0x00293130
float Roll(Quaternion quaternion)
{
    quaternion =
        Quaternion(zaxis, Yaw(quaternion) * -DEG_TO_RAD) * quaternion;
    quaternion =
        Quaternion(xaxis, Pitch(quaternion) * -DEG_TO_RAD) * quaternion;
    return (float)atan2(-quaternion.xdir().m_fZ, quaternion.xdir().m_fX) *
           RAD_TO_DEG;
}
// PC 0x004DA410 GOG 0x004DA550 Xbox 0x002933F0
Quaternion pointto(const Vector& vector)
{
    static Vector v(0.0f, 0.0f, 0.0f);
    static Vector a(0.0f, 0.0f, 0.0f);
    v = normalize(vector);

    Vector vUp(0.0f, 0.0f, 1.0f);
    a = cross(vUp, v);
    float fSin = magnitude(a);
    if (fSin > 1.0f)
    {
        fSin = 1.0f;
    }
    else if (fSin < -1.0f)
    {
        fSin = -1.0f;
    }
    else if (fSin == 0.0f)
    {
        float fAngle = 0.0f;
        if (!(v.m_fZ > 0.0f))
        {
            fAngle = PI;
        }
        return Quaternion(xaxis, fAngle);
    }

    float fAngle = (float)asin(fSin);
    if (vUp * v < 0.0f)
    {
        fAngle = PI - fAngle;
    }
    return Quaternion(normalize(a), fAngle);
}

#ifdef _XBOX
int logbase2(unsigned int nValue)  // Xbox 0x0028F9F0
{
    int nLog = 0;
    for (int i = 0; i < 5; i++)
    {
        if (nValue & topMask[i])
        {
            nValue &= topMask[i];
            nLog += 16 >> i;
        }
    }
    return nLog;
}
#endif
// PC 0x004D81D0 GOG 0x004D8310 Xbox 0x00290240
float magnitude(const Vector& vector)
{
#ifdef _XBOX
    return SqrtEstimateNR_ASM(vector.m_fX * vector.m_fX + vector.m_fY * vector.m_fY +
                              vector.m_fZ * vector.m_fZ);
#else
    return (float)sqrt(vector.m_fX * vector.m_fX + vector.m_fY * vector.m_fY +
                       vector.m_fZ * vector.m_fZ);
#endif
}

// Dead code: not in the PC executable
Vector Orth(const Vector& vector)
{
    Vector vOrth = cross(vector, Vector(1.0f, 0.0f, 0.0f));
    if (magnitude(vOrth) < 0.01)
    {
        vOrth = cross(vector, Vector(0.0f, 1.0f, 0.0f));
    }
    vOrth = normalize(vOrth);
    return vOrth;
}
// PC 0x004D8200 GOG 0x004D8340 Xbox 0x002902C0
void Vector::Normalize()
{
    float fMagnitude = magnitude(*this);
    if (fMagnitude < 1e-9)
    {
        m_fX = 1.0f;
        m_fY = 0.0f;
        m_fZ = 0.0f;
        return;
    }

    float fReciprocal = 1.0f / fMagnitude;
    m_fX *= fReciprocal;
    m_fY *= fReciprocal;
    m_fZ *= fReciprocal;
}
// PC 0x004D8250 GOG 0x004D8390 Xbox 0x00290330
#ifdef _XBOX
inline
#endif
Vector normalize(const Vector& vector)
{
    float fMagnitude = magnitude(vector);
    if (fMagnitude < 1e-9)
    {
        return Vector(1.0f, 0.0f, 0.0f);
    }

    float fReciprocal = 1.0f / fMagnitude;
    return Vector(vector.m_fX * fReciprocal, vector.m_fY * fReciprocal,
                  vector.m_fZ * fReciprocal);
}

// Dead code: not in the PC executable
Vector Orth(const Vector& vFirst, const Vector& vSecond)
{
    Vector vOrth = cross(vFirst, vSecond);
    vOrth = normalize(vOrth);
    return vOrth;
}

// Dead code: not in the PC executable
int bitcount(unsigned char nValue)
{
    int nCount = 0;
    for (int i = 0; i < 8; i++)
    {
        if ((nValue >> i) & 1)
        {
            nCount++;
        }
    }
    return nCount;
}

// PC 0x004D82B0 GOG 0x004D83F0 Xbox 0x002903A0
Vector planelineintersection(const Vector& vNormal, float fDistance,
                             const Vector& vStart, const Vector& vEnd)
{

    static Vector dif(0.0f, 0.0f, 0.0f);
    dif = vEnd - vStart;
#ifdef _XBOX
    float fDenominator = dif * vNormal;
    float fProjection = 0.0f;
    if (fDenominator != 0.0f)
    {
        fProjection = -(vStart * vNormal + fDistance) / fDenominator;
    }
#else
    float fDenominator = dif.m_fX * vNormal.m_fX;
    fDenominator += dif.m_fZ * vNormal.m_fZ;
    fDenominator += dif.m_fY * vNormal.m_fY;
    float fProjection = 0.0f;
    if (fDenominator != 0.0f)
    {
        fProjection =
            -(vStart.m_fX * vNormal.m_fX + vStart.m_fY * vNormal.m_fY +
              vStart.m_fZ * vNormal.m_fZ + fDistance) /
            fDenominator;
    }
#endif
    return vStart + dif * fProjection;
}

// Dead code: not in the PC executable
Vector planesegmentintersection(const Vector& vNormal, float fDistance,
                                const Vector& vStart, const Vector& vEnd)
{

    static Vector dif(0.0f, 0.0f, 0.0f);
    dif = vEnd - vStart;
    float fDenominator = dif.m_fX * vNormal.m_fX;
    fDenominator += dif.m_fZ * vNormal.m_fZ;
    fDenominator += dif.m_fY * vNormal.m_fY;
    float fProjection = 0.0f;
    if (fDenominator != 0.0f)
    {
        fProjection =
            -(vStart.m_fX * vNormal.m_fX + vStart.m_fY * vNormal.m_fY +
              vStart.m_fZ * vNormal.m_fZ + fDistance) /
            fDenominator;
        if (1.0f < fProjection)
        {
            fProjection = 1.0f;
        }
        else if (0.0f > fProjection)
        {
            fProjection = 0.0f;
        }
    }
    return vStart + dif * fProjection;
}
// PC 0x004D83C0 GOG 0x004D8500 Xbox 0x002904E0
Vector lineproject(const Vector& vStart, const Vector& vEnd,
                   const Vector& vPoint)
{

    static Vector w(0.0f, 0.0f, 0.0f);
    w = vEnd - vStart;
#ifdef _XBOX
    float fProjection = w * w;
    if (fProjection != 0.0f)
    {
        fProjection = ((vPoint - vStart) * w) / fProjection;
    }
#else
    float fProjection = w.m_fX * w.m_fX;
    fProjection += w.m_fZ * w.m_fZ;
    fProjection += w.m_fY * w.m_fY;
    if (fProjection != 0.0f)
    {
        const Vector& vOffset = vPoint - vStart;
        float fDot = vOffset.m_fZ * w.m_fZ;
        fDot += vOffset.m_fX * w.m_fX;
        fDot += vOffset.m_fY * w.m_fY;
        fProjection = fDot / fProjection;
    }
#endif
    return vStart + w * fProjection;
}
// PC 0x004D84C0 GOG 0x004D8600 Xbox 0x00290630
Vector segmentproject(const Vector& vStart, const Vector& vEnd,
                      const Vector& vPoint)
{
    if (equal(vStart, vEnd))
    {
        return vStart;
    }

    static Vector w(0.0f, 0.0f, 0.0f);
    w = vEnd - vStart;
#ifdef _XBOX
    float fProjection = w * w;
    if (fProjection != 0.0f)
    {
        fProjection = ((vPoint - vStart) * w) / fProjection;
    }
    if (1.0f < fProjection)
    {
        fProjection = 1.0f;
    }
    else if (0.0f > fProjection)
    {
        fProjection = 0.0f;
    }
#else
    float fProjection = w.m_fX * w.m_fX;
    fProjection += w.m_fZ * w.m_fZ;
    fProjection += w.m_fY * w.m_fY;
    if (fProjection != 0.0f)
    {
        const Vector& vOffset = vPoint - vStart;
        float fDot = vOffset.m_fZ * w.m_fZ;
        fDot += vOffset.m_fX * w.m_fX;
        fDot += vOffset.m_fY * w.m_fY;
        fProjection = fDot / fProjection;
    }
    if (1.0f < fProjection)
    {
        fProjection = 1.0f;
    }
    else if (0.0f > fProjection)
    {
        fProjection = 0.0f;
    }
#endif
    return vStart + w * fProjection;
}

// Dead code: not in the PC executable
float Determinant(Matrix matrix)
{
    return matrix.m_vFirst.m_fX * matrix.m_vSecond.m_fY * matrix.m_vThird.m_fZ +
           matrix.m_vSecond.m_fX * matrix.m_vThird.m_fY * matrix.m_vFirst.m_fZ +
           matrix.m_vThird.m_fX * matrix.m_vFirst.m_fY * matrix.m_vSecond.m_fZ -
           matrix.m_vFirst.m_fX * matrix.m_vThird.m_fY * matrix.m_vSecond.m_fZ -
           matrix.m_vSecond.m_fX * matrix.m_vFirst.m_fY * matrix.m_vThird.m_fZ -
           matrix.m_vThird.m_fX * matrix.m_vSecond.m_fY * matrix.m_vFirst.m_fZ;
}

// Dead code: not in the PC executable
Matrix Inverse(Matrix matrix)
{
    static Matrix b;
    float fDeterminant = Determinant(matrix);
    float (*aafMatrix)[3] = (float (*)[3])&matrix;
    float (*aafInverse)[3] = (float (*)[3])&b;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            aafInverse[j][i] = (aafMatrix[(i + 1) % 3][(j + 1) % 3] * aafMatrix[(i + 2) % 3][(j + 2) % 3] -
                                aafMatrix[(i + 1) % 3][(j + 2) % 3] * aafMatrix[(i + 2) % 3][(j + 1) % 3]) /
                               fDeterminant;
        }
    }
    return b;
}

// Dead code: not in the PC executable
Matrix inverse2(Matrix matrix)
{
    matrix.m_vThird = Vector(0.0f, 0.0f, 1.0f);
    matrix.m_vSecond.m_fZ = 0.0f;
    matrix.m_vFirst.m_fZ = 0.0f;
    float fDeterminant = Determinant(matrix);
    float fTemp = matrix.m_vFirst.m_fX;
    matrix.m_vFirst.m_fX = matrix.m_vSecond.m_fY;
    matrix.m_vSecond.m_fY = fTemp;
    matrix.m_vFirst.m_fY = -matrix.m_vFirst.m_fY;
    matrix.m_vSecond.m_fX = -matrix.m_vSecond.m_fX;
    float fInverse = 1.0f / fDeterminant;
    matrix.m_vFirst = matrix.m_vFirst * fInverse;
    matrix.m_vSecond = matrix.m_vSecond * fInverse;
    return matrix;
}
// PC 0x004D8620 GOG 0x004D8760 Xbox 0x002907E0
Matrix Transpose(const Matrix& matrix)
{
    return Matrix(Vector(matrix.m_vFirst.m_fX, matrix.m_vSecond.m_fX,
                         matrix.m_vThird.m_fX),
                  Vector(matrix.m_vFirst.m_fY, matrix.m_vSecond.m_fY,
                         matrix.m_vThird.m_fY),
                  Vector(matrix.m_vFirst.m_fZ, matrix.m_vSecond.m_fZ,
                         matrix.m_vThird.m_fZ));
}

inline float norm(const Vector& vector)
{
    return SEGMENT_MAX(AbsoluteValue(vector.m_fX),
                       SEGMENT_MAX(AbsoluteValue(vector.m_fY), AbsoluteValue(vector.m_fZ)));
}

// Dead code: not in the PC executable
float norm(const Matrix& matrix)
{
    return SEGMENT_MAX(norm(matrix.m_vFirst),
                       SEGMENT_MAX(norm(matrix.m_vSecond), norm(matrix.m_vThird)));
}

// Dead code: not in the PC executable
Matrix operator*(const Matrix& left, const Matrix& right)
{
    return Matrix(left * right.m_vFirst, left * right.m_vSecond, left * right.m_vThird);
}

// Dead code: not in the PC executable
float* Augment(const Matrix& matrix)
{
    static float a[16];
    for (int i = 0; i < 16; i++)
    {
        a[i] = 0.0f;
    }
    a[15] = 1.0f;
    a[0] = matrix.m_vFirst.m_fX;
    a[4] = matrix.m_vFirst.m_fY;
    a[8] = matrix.m_vFirst.m_fZ;
    a[1] = matrix.m_vSecond.m_fX;
    a[5] = matrix.m_vSecond.m_fY;
    a[9] = matrix.m_vSecond.m_fZ;
    a[2] = matrix.m_vThird.m_fX;
    a[6] = matrix.m_vThird.m_fY;
    a[10] = matrix.m_vThird.m_fZ;
    return a;
}
// PC 0x004D86D0 GOG 0x004D8810 Xbox 0x00290950
void Plane::Transform(const Vector& vPosition, const Quaternion& qOrientation)
{
    static Vector newnormal;
    static Vector origin;

    newnormal = -qOrientation * m_vNormal;
    origin = -qOrientation * (-m_vNormal * m_fDistance - vPosition);
    m_vNormal = newnormal;
    m_fDistance = -(origin * newnormal);
}

// Dead code: not in the PC executable
Vector operator*(const Vector& vector, const Quaternion& quaternion)
{
    return Vector();
}

// PC 0x004D8A90 GOG 0x004D8BD0 Xbox 0x00290DA0
Quaternion normalize(Quaternion quaternion)
{
#ifdef _XBOX
    float fScale = ReciprocalSqrtEstimateNR_ASM(
        quaternion.m_fW * quaternion.m_fW + quaternion.m_fX * quaternion.m_fX +
        quaternion.m_fY * quaternion.m_fY + quaternion.m_fZ * quaternion.m_fZ);
    if (fScale == 1.0f || fScale == -1.0f)
    {
        quaternion.m_fW = 1.0f;
        quaternion.m_fX = 0.0f;
        quaternion.m_fY = 0.0f;
        quaternion.m_fZ = 0.0f;
        return quaternion;
    }
    return quaternion * fScale;
#else
    float fMagnitude = (float)sqrt(
        quaternion.m_fW * quaternion.m_fW + quaternion.m_fX * quaternion.m_fX +
        quaternion.m_fY * quaternion.m_fY + quaternion.m_fZ * quaternion.m_fZ);
    if (fMagnitude == 0.0f)
    {
        quaternion.m_fW = 1.0f;
        quaternion.m_fX = 0.0f;
        quaternion.m_fY = 0.0f;
        quaternion.m_fZ = 0.0f;
        return quaternion;
    }
    return quaternion * (1.0f / fMagnitude);
#endif
}
// PC 0x004D8B40 GOG 0x004D8C80 Xbox 0x00290EB0
Quaternion slerp(Quaternion first, const Quaternion& second, float fPosition)
{
    float fDot = dot(first, second);
    if (fDot < 0.0f)
    {
        first.m_fX = -first.m_fX;
        first.m_fY = -first.m_fY;
        first.m_fZ = -first.m_fZ;
        first.m_fW = -first.m_fW;
    }

    float fCosine = (fDot < 0.0f) ? -fDot : fDot;

    if (fCosine >= 1.0)
    {
        return first;
    }
    float fAngle = (float)acos(fCosine);
    if (fAngle == 0.0f)
    {
        return first;
    }

    float fSine = (float)sin(fAngle);
    Quaternion result =
        first * (float)(sin(fAngle - fAngle * fPosition) / fSine) +
        second * (float)(sin(fAngle * fPosition) / fSine);
    return result;
}

// Dead code: not in the PC executable
Quaternion Matrix::getquaternion() const
{
    Quaternion quaternion;
    Vector vFirst = m_vFirst;
    Vector vSecond = m_vSecond;
    Vector vThird = m_vThird;
    int anNext[3] = {1, 2, 0};
    float aafMatrix[3][3];

    for (int i = 0; i < 3; i++)
    {
        aafMatrix[0][i] = (&vFirst.m_fX)[i];
        aafMatrix[1][i] = (&vSecond.m_fX)[i];
        aafMatrix[2][i] = (&vThird.m_fX)[i];
    }

    float fTrace = aafMatrix[0][0] + aafMatrix[1][1] + aafMatrix[2][2];
    if (fTrace > 0.0f)
    {
        float fScale = (float)sqrt(fTrace + 1.0);
        quaternion.m_fW = fScale * 0.5f;
        fScale = 0.5f / fScale;
        quaternion.m_fX = (aafMatrix[1][2] - aafMatrix[2][1]) * fScale;
        quaternion.m_fY = (aafMatrix[2][0] - aafMatrix[0][2]) * fScale;
        quaternion.m_fZ = (aafMatrix[0][1] - aafMatrix[1][0]) * fScale;
    }
    else
    {
        float afQuaternion[4];
        int i = 0;
        if (aafMatrix[1][1] > aafMatrix[0][0])
        {
            i = 1;
        }
        if (aafMatrix[2][2] > aafMatrix[i][i])
        {
            i = 2;
        }
        int j = anNext[i];
        int k = anNext[j];

        float fScale = (float)sqrt((aafMatrix[i][i] - (aafMatrix[j][j] + aafMatrix[k][k])) + 1.0);
        afQuaternion[i] = fScale * 0.5f;
        if (fScale != 0.0f)
        {
            fScale = 0.5f / fScale;
        }
        afQuaternion[3] = (aafMatrix[j][k] - aafMatrix[k][j]) * fScale;
        afQuaternion[j] = (aafMatrix[i][j] + aafMatrix[j][i]) * fScale;
        afQuaternion[k] = (aafMatrix[i][k] + aafMatrix[k][i]) * fScale;

        quaternion.m_fX = afQuaternion[0];
        quaternion.m_fY = afQuaternion[1];
        quaternion.m_fZ = afQuaternion[2];
        quaternion.m_fW = afQuaternion[3];
    }

    return quaternion;
}

// Dead code: not in the PC executable
int isnormalized(const Vector& vector)
{
    float fLengthSquared = vector * vector;
    return fLengthSquared >= 0.99 && fLengthSquared <= 1.01;
}
// PC 0x004D8CA0 GOG 0x004D8DE0 Xbox 0x00291180
float Pitch(const Quaternion& quaternion)
{
    static Vector v(0.0f, 0.0f, 0.0f);
    v = Vector(2.0f * (quaternion.m_fY * quaternion.m_fX -
                                quaternion.m_fZ * quaternion.m_fW),
                        1.0f - 2.0f * (quaternion.m_fZ * quaternion.m_fZ +
                                       quaternion.m_fX * quaternion.m_fX),
                        2.0f * (quaternion.m_fY * quaternion.m_fZ +
                                quaternion.m_fW * quaternion.m_fX));
    float fX = v.m_fX;
    float fY = v.m_fY;
#ifdef _XBOX
    return (float)atan2(v.m_fZ, SqrtEstimateNR_ASM(fX * fX + fY * fY)) * RAD_TO_DEG;
#else
    float fZ = v.m_fZ;
    return (float)atan2(fZ, (float)sqrt(fX * fX + fY * fY)) * RAD_TO_DEG;
#endif
}
// PC 0x004D8D70 GOG 0x004D8EB0 Xbox 0x002912E0
float Pitch(const Vector& vector)
{
    float fX = vector.m_fX;
    float fY = vector.m_fY;
#ifdef _XBOX
    return (float)atan2(vector.m_fZ, SqrtEstimateNR_ASM(fX * fX + fY * fY)) * RAD_TO_DEG;
#else
    float fZ = vector.m_fZ;
    return (float)atan2(fZ, (float)sqrt(fX * fX + fY * fY)) * RAD_TO_DEG;
#endif
}
// PC 0x004D8DA0 GOG 0x004D8EE0 Xbox 0x00291360
Quaternion RotationArc(Vector vFirst, Vector vSecond)
{
    static Quaternion q;
    vFirst.Normalize();
    vSecond.Normalize();
    Vector vCross = cross(vFirst, vSecond);
    float fDot = vFirst * vSecond;
    if (fDot <= -1.0f)
    {
        return Quaternion(0.0f, 1.0f, 0.0f, 0.0f);
    }

#ifdef _XBOX
    float fScale = SqrtEstimateNR_ASM((fDot + 1.0f) * 2.0f);
#else
    float fScale = (float)sqrt((fDot + 1.0f) * 2.0f);
#endif
    q.m_fX = vCross.m_fX * (1.0f / fScale);
    q.m_fY = vCross.m_fY * (1.0f / fScale);
    q.m_fZ = (1.0f / fScale) * vCross.m_fZ;
    q.m_fW = fScale * 0.5f;
    return q;
}
// PC 0x004D8EF0 GOG 0x004D9030 Xbox 0x00291590
int polyhit(const Vector* pVertices, int nVertices, const Vector& vStart,
            const Vector& vEnd, Vector* pHit, Vector* pNormal)
{

    Vector vSum(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < nVertices - 2; i++)
    {
        int nNext = (i + 1) % nVertices;
        int nAfter = (i + 2) % nVertices;
        vSum = vSum + cross(pVertices[nNext] - pVertices[i],
                            pVertices[nAfter] - pVertices[nNext]);
    }
#ifdef _XBOX
    float fMagnitude = SqrtEstimateNR_ASM(vSum * vSum);
#else
    float fMagnitude = magnitude(vSum);
#endif
    if (fMagnitude < 1e-8f)
    {
        return 0;
    }
    float fReciprocal = 1.0f / fMagnitude;
    Vector vNormal = vSum * fReciprocal;
    float fDistance = -(vNormal * pVertices[0]);
    float fStart, fEnd;
    if ((fStart = vNormal * vStart + fDistance) < 0.0f ||
        (fEnd = vNormal * vEnd + fDistance) > 0.0f || fStart == fEnd)
    {
        return 0;
    }
    static Vector the_point(0.0f, 0.0f, 0.0f);
    float fPosition = fStart / (fStart - fEnd);
    the_point = (1.0f - fPosition) * vStart + fPosition * vEnd;
    int nHit = 1;
    for (i = 0; nHit && i < nVertices; i++)
    {
        Vector vFirst = pVertices[i];
        Vector vSecond = pVertices[(i + 1) % nVertices];
        Vector vSide;
        vSide = cross(vSecond - vFirst, the_point - vFirst);
        nHit = vSide * vNormal >= 0.0;
    }
    if (nHit)
    {
        if (pNormal)
        {
            *pNormal = vNormal;
        }
        if (pHit)
        {
            *pHit = the_point;
        }
    }
    return nHit;
}
// PC 0x004D92C0 GOG 0x004D9400 Xbox 0x00291D90
int SphereIntersect(const Vector& vStart, const Vector& vEnd,
                    const Vector& vCenter, float fRadius)
{
    Vector vDirection = vEnd - vStart;
    float fLengthSquared = vDirection * vDirection;
    if (fLengthSquared < 1e-9)
    {
        return 0;
    }

    float fPosition = ((vCenter - vStart) * vDirection) / fLengthSquared;
#ifdef _XBOX
    fPosition = __min(1.0f, __max(0.0f, fPosition));
#else
    if (!(0.0f > fPosition))
    {
        if (1.0f < fPosition)
        {
            fPosition = 1.0f;
        }
    }
    else
    {
        fPosition = 0.0f;
    }
#endif

    Vector vClosest = vStart + vDirection * fPosition;
    if (magnitude(vClosest - vCenter) < fRadius)
    {
        return 1;
    }
    return 0;
}
// PC 0x004D93F0 GOG 0x004D9530 Xbox 0x00291EB0
float DistanceBetweenLines(const Vector& vFirstPoint, const Vector& vFirstDirection,
                           const Vector& vSecondPoint, const Vector& vSecondDirection,
                           Vector* pvFirstClosest, Vector* pvSecondClosest)
{
    static Vector cp;
    cp = cross(vFirstDirection, vSecondDirection);
    cp.Normalize();

    float fSignedDistance = -(cp * vFirstPoint) - -(cp * vSecondPoint);
    float fDistance = fSignedDistance < 0.0f ? -fSignedDistance : fSignedDistance;

    if (pvFirstClosest != 0)
    {
        Vector vNormal = normalize(cross(vSecondDirection, cp));
        float fPlaneDistance = -(vNormal * vSecondPoint);
        *pvFirstClosest = planelineintersection(vNormal, fPlaneDistance,
                                               vFirstPoint, vFirstPoint + vFirstDirection);
    }
    if (pvSecondClosest != 0)
    {
        Vector vNormal = normalize(cross(vFirstDirection, cp));
        float fPlaneDistance = -(vNormal * vFirstPoint);
        *pvSecondClosest = planelineintersection(vNormal, fPlaneDistance,
                                                vSecondPoint, vSecondPoint + vSecondDirection);
    }
    return fDistance;
}
