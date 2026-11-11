#ifndef MATH3D_H
#define MATH3D_H



#define PI 3.1415927f
#define HALF_PI 1.5707964f
#define TWO_PI 6.2831855f
#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.29578f

template <class T> class List;


static inline void roundToInt(int& iResult, float fValue)
{
    __asm
    {
        fld fValue
        mov edx, iResult
        frndint
        fistp dword ptr [edx]
    }
}


// PC 0x00425430
inline float AbsoluteValue(float fValue)
{
    return fValue < 0.0f ? -fValue : fValue;
}

class Matrix;

class Vector
{
public:
    Vector()  // PC 0x0040A290
    {
        m_fX = 0.0f;
        m_fY = 0.0f;
        m_fZ = 0.0f;
    }
    // PC 0x00404B50
    Vector(float fX, float fY, float fZ)
        : m_fX(fX),
          m_fY(fY),
          m_fZ(fZ)
    {
    }
    // PC 0x00480900
    int IsZero() const
    {
        return m_fX == 0.0f && m_fY == 0.0f && m_fZ == 0.0f;
    }

    Vector& operator+=(const Vector& other);
    Vector& operator-=(const Vector& other);
    Vector& operator*=(float fScale);
    Vector& operator/=(float fScale);
    void Normalize();


    Vector operator-() const
    {
        return Vector(-m_fX, -m_fY, -m_fZ);
    }


    float GetX() const { return m_fX; }

    float GetZ() const;
    float m_fX;
    float m_fY;
    float m_fZ;
};

class Quaternion
{
public:
    Quaternion()
    {
        m_fX = m_fY = m_fZ = 0.0f;
        m_fW = 1.0f;
    }

    Quaternion(Vector vAxis, float fAngle);
    // PC 0x004634E0
    Vector xdir() const
    {
        return Vector(1.0f - 2.0f * (m_fY * m_fY + m_fZ * m_fZ),
                      2.0f * (m_fX * m_fY + m_fW * m_fZ),
                      2.0f * (m_fX * m_fZ - m_fW * m_fY));
    }

    Vector ydir() const;
    Vector zdir() const  // PC 0x0044FEE0
    {
        return Vector(2.0f * (m_fX * m_fZ + m_fY * m_fW),
                      2.0f * (m_fZ * m_fY - m_fW * m_fX),
                      1.0f - 2.0f * (m_fX * m_fX + m_fY * m_fY));
    }
    Vector axis() const;
    Matrix getmatrix() const;

    Quaternion(float fW, float fX, float fY, float fZ)
        : m_fW(fW),
          m_fX(fX),
          m_fY(fY),
          m_fZ(fZ)
    {
    }

    float m_fW;
    float m_fX;
    float m_fY;
    float m_fZ;
};


class Vector4
{
public:
    Vector4()
    {
        m_fX = 0.0f;
        m_fY = 0.0f;
        m_fZ = 0.0f;
        m_fW = 1.0f;
    }

    float m_fX;
    float m_fY;
    float m_fZ;
    float m_fW;
};

class Plane
{
public:
    Plane()
        : m_fDistance(0.0f)
    {
    }

    Plane(Vector vNormal, float fDistance)
        : m_vNormal(vNormal),
          m_fDistance(fDistance)
    {
    }

    void Transform(const Vector& vPosition, const Quaternion& qOrientation);

    Vector m_vNormal;
    float m_fDistance;
};

extern Vector zaxis;
extern int quaternionidentityoptimization;

float magnitude(const Vector& vector);
int SphereIntersect(const Vector& vStart, const Vector& vEnd,
                    const Vector& vCenter, float fRadius);
float DistanceBetweenLines(const Vector& vFirstPoint, const Vector& vFirstDirection,
                           const Vector& vSecondPoint, const Vector& vSecondDirection,
                           Vector* pvFirstClosest, Vector* pvSecondClosest);
Vector cmul(const Vector& left, const Vector& right);
Vector normalize(const Vector& vector);
Quaternion normalize(Quaternion quaternion);
float DegToRad(float fDegrees);
double Round(float fValue, float fRoundTo);
Matrix Transpose(const Matrix& matrix);
Vector operator*(const Matrix& matrix, const Vector& vector);
Quaternion operator-(const Quaternion& quaternion);
Quaternion operator*(const Quaternion& left, const Quaternion& right);
Quaternion operator*(const Quaternion& quaternion, float fScale);
Vector operator*(const Quaternion& quaternion, const Vector& vector);
Quaternion operator+(const Quaternion& left, const Quaternion& right);
Quaternion slerp(Quaternion first, const Quaternion& second, float fPosition);
Quaternion YawPitchRoll(float fYaw, float fPitch, float fRoll);
float Roll(Quaternion quaternion);
void MakeMatrix4x4(float* pMatrix, const Vector& vTranslation,
                   const Quaternion& quaternion);
Quaternion RotationArc(Vector vFirst, Vector vSecond);
Quaternion pointto(const Vector& vector);
float InterpolateBezier(float fStart, float fEnd, float fControl1,
                        float fControl2, float fPosition);
Vector InterpolateBezier(const Vector& vStart, const Vector& vEnd,
                         const Vector& vControl1, const Vector& vControl2,
                         float fPosition);
Vector VectorMin(Vector vFirst, Vector vSecond);
Vector VectorMax(Vector vFirst, Vector vSecond);
Vector VectorMin(List<Vector>& vectors);
Vector VectorMax(List<Vector>& vectors);
Vector cross(const Vector& vFirst, const Vector& vSecond);
Vector NormalOf(const Vector& vFirst, const Vector& vSecond,
                const Vector& vThird);
float SphereAbovePlane(const Vector& vCenter, float fRadius,
                       const Plane& plane);
float PlaneHeightAtPoint(const Vector& vNormal, float fDistance,
                         const Vector& vPoint);
float Yaw(const Vector& vector);
float Pitch(const Vector& vector);
float Yaw(const Quaternion& quaternion);
float Pitch(const Quaternion& quaternion);
int BoxIntersect(const Vector& vStart, const Vector& vEnd,
                 const Vector& vMinimum, const Vector& vMaximum, Vector* pvHit);
int BoxIntersect(const Vector& vStart, const Vector& vEnd,
                 const Vector& vMinimum, const Vector& vMaximum);
int polyhit(const Vector* pVertices, int nVertices, const Vector& vStart,
            const Vector& vEnd, Vector* pHit, Vector* pNormal);

#define POLYHIT_EDGE_TOLERANCE 0.00001f

int polyhit(const Vector* pVertices, int nVertices, const Vector& vNormal,
            const float& fDistance, const Vector& vStart, const Vector& vEnd,
            Vector* pHit);
float BoxAbovePlane(const Vector& vMinimum, const Vector& vMaximum,
                    const Plane& plane);
Vector planeproject(const Vector& vNormal, float fDistance,
                    const Vector& vPoint);
Vector lineproject(const Vector& vStart, const Vector& vEnd,
                   const Vector& vPoint);
Vector planelineintersection(const Vector& vNormal, float fDistance,
                             const Vector& vStart, const Vector& vEnd);
Vector segmentproject(const Vector& vStart, const Vector& vEnd,
                      const Vector& vPoint);
int IntersectCircle2d(const Vector& vCenter, float fRadius,
                      const Vector& vStart, const Vector& vEnd, Vector& vHit1,
                      Vector& vHit2, bool bSegment);
int equal(const Vector& vFirst, const Vector& vSecond);
int IntersectLineSegments2d(const Vector& vStart, const Vector& vEnd,
                            const Vector& vSideStart, const Vector& vSideEnd,
                            Vector& vIntersection);
int operator!=(const Vector& vFirst, const Vector& vSecond);
float* Homogenize(const Vector& vector, const float& fW);
float cosineLUT(float fAngle);
float sineLUT(float fAngle);
// PC 0x00463530
inline Vector Quaternion::ydir() const
{
    return Vector(2.0f * (m_fX * m_fY - m_fW * m_fZ),
                  1.0f - 2.0f * (m_fX * m_fX + m_fZ * m_fZ),
                  2.0f * (m_fY * m_fZ + m_fX * m_fW));
}
// PC 0x00425450
inline Vector operator+(const Vector& left, const Vector& right)
{
    return Vector(left.m_fX + right.m_fX, left.m_fY + right.m_fY,
                  left.m_fZ + right.m_fZ);
}
// PC 0x00425480
inline Vector operator-(const Vector& left, const Vector& right)
{
    return Vector(left.m_fX - right.m_fX, left.m_fY - right.m_fY,
                  left.m_fZ - right.m_fZ);
}
// PC 0x004254B0
inline Vector operator*(const Vector& vector, float fScale)
{
    return Vector(fScale * vector.m_fX, fScale * vector.m_fY,
                  fScale * vector.m_fZ);
}
// PC 0x00448D40
inline Vector operator*(float fScale, const Vector& vector)
{
    return Vector(fScale * vector.m_fX, fScale * vector.m_fY,
                  fScale * vector.m_fZ);
}
// PC 0x00448D70
inline Vector operator/(const Vector& vector, float fScale)
{
    if (fScale != 0.0f)
    {
        float fReciprocal = 1.0f / fScale;
        return Vector(fReciprocal * vector.m_fX, fReciprocal * vector.m_fY,
                      fReciprocal * vector.m_fZ);
    }
    return vector;
}
// PC 0x004254E0
inline float operator*(const Vector& left, const Vector& right)
{
    return left.m_fX * right.m_fX + left.m_fY * right.m_fY +
           left.m_fZ * right.m_fZ;
}
// PC 0x00485EA0
inline Vector Interpolate(const Vector& vFirst, const Vector& vSecond, float fPosition)
{
    return vFirst * (1.0f - fPosition) + vSecond * fPosition;
}

class Matrix
{
public:
    Matrix()  // PC 0x00448DD0
    {
        m_vFirst = Vector(1.0f, 0.0f, 0.0f);
        m_vSecond = Vector(0.0f, 1.0f, 0.0f);
        m_vThird = Vector(0.0f, 0.0f, 1.0f);
    }
    // PC 0x00448E80
    Matrix(Vector vFirst, Vector vSecond, Vector vThird)
    {
        m_vFirst = vFirst;
        m_vSecond = vSecond;
        m_vThird = vThird;
    }

    Vector m_vFirst;
    Vector m_vSecond;
    Vector m_vThird;
};
// PC 0x00463580
inline Matrix Quaternion::getmatrix() const
{
    return Matrix(xdir(), ydir(), zdir());
}


#include "aurlist.h"
#include "math3d.h"

class nodedata;

class BSPNode
{
public:
    BSPNode(Vector vNormal, float fDistance);
    ~BSPNode();

    Plane m_Plane;
    BSPNode* m_pNegative;
    BSPNode* m_pPositive;
    int m_bLeaf;
    Vector m_vMinimum;
    Vector m_vMaximum;
    List<Plane> m_lstNegativePlanes;
    List<Plane> m_lstPositivePlanes;
    List<Vector> m_lstVertices;
    nodedata* m_pData;
};

typedef void (*BSPTraverseCallback)(BSPNode*, void*);
typedef void (*BSPVolumeCallback)(BSPNode*, const Vector&, float);
typedef int (*BSPSearchCallback)(BSPNode*, void*);
typedef int (*BSPHitCallback)(BSPNode*, Vector, Vector, Vector*);
typedef void (*BSPPlaneCallback)(BSPNode*, void*, List<Plane>&);


enum BSPFaceSide
{
    BSP_FACE_NEGATIVE = 1,
    BSP_FACE_POSITIVE = 2,
    BSP_FACE_SPLIT = 3
};

void BSPTraverse(BSPNode* pNode, BSPTraverseCallback pCallback, void* pData);
void BSPTraverse(BSPNode* pNode, const Vector& vCenter, float fRadiusSquared,
                 BSPVolumeCallback pCallback);
int RFaceSplitTest(List<Vector>& vertices, const Plane& plane);
BSPNode* BSPFitSphere(BSPNode* pNode, const Vector& vCenter, float fRadius);
BSPNode* BSPFitBox(BSPNode* pNode, const Vector& vMinimum,
                   const Vector& vMaximum);
void BSPSphereCheck(BSPNode* pNode, const Vector& vCenter, float fRadius,
                    List<BSPNode*>& nodes);
BSPNode* BSPSearch(BSPNode* pNode, Vector vPoint, BSPSearchCallback pCallback,
                   void* pData);
int BSPHitCheck(BSPNode* pNode, Vector vStart, Vector vEnd, Vector* pHit,
                BSPHitCallback pCallback);
void BSPTraverse(BSPNode* pNode, Vector vPoint,
                 void (*pCallback)(BSPNode*, void*, List<Plane>&), void* pData);
void BSPTraverseVolumeApprox(BSPNode* pNode, const Vector& vPoint,
                              List<Plane>& planes,
                              void (*pCallback)(BSPNode*, void*, List<Plane>&),
                              void* pData);

#endif
