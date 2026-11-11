#ifndef __NWAREA_H__
#define __NWAREA_H__

#include "exoresman.h"
#include "math3d.h"

template <class T> class CExoArrayList;
class CGameObject;


#define AREA_DEFAULT_FOG_DISTANCE 10000.0f
#define AREA_DEFAULT_ALPHA_TEST 0.2f

#define AREA_INVALID_OBJECT_ID 0x7F000000

class CSWArea
{
public:


    enum AreaFlags
    {


        AREA_FLAG_SUPPRESS_WEATHER = 1,
        AREA_FLAG_HEAT_SHIMMER = 8
    };

    static const int MAX_INTER_TILE_SUCCESSORS;
    static const float TILE_SIZE;
    static const float MAX_TILE_EPSILON;
    static const float MIN_TILE_EPSILON;
    static const float EPSILON;
    static const float SPLINE_COSINE_LIMIT;

    enum WeatherType
    {
        WEATHER_CLEAR = 0,
        WEATHER_RAIN = 1,
        WEATHER_SNOW = 2,

        WEATHER_AUTOMATIC = 0xFF
    };



    enum WindType { WIND_NONE = 0, WIND_LIGHT = 1, WIND_HEAVY = 2 };

    CSWArea();
    virtual ~CSWArea();
    virtual int GetIsIDInExcludeList(unsigned long oidObject,  // PC 0x00409EB0
                                      CExoArrayList<CGameObject*>* pExcludeList)
    {
        return 0;
    }
    virtual int SetFog(float fNear, float fFar, Vector vColor);
    virtual int SetIsNight(int bIsNight);
    virtual int SetWind(unsigned char nWind);



    int AddStaticObject(unsigned long nId, const Vector& vOrientation,
                        const Vector& vPosition, int nVertices,
                        Vector* pVertices, int nTriangles, int* pTriangles);
    void GrowStaticObjectArray();
    void GrowStaticObjectVertices(int nAdditional);
    void GrowStaticObjectTriangles(int nAdditional);
    int InitializeStaticObjects();
    int IntersectLineSegments(float fX1, float fY1, float fX2, float fY2,
                              float fX3, float fY3, float fX4, float fY4,
                              float* pfX, float* pfY);
    void RemoveStaticObject(int nIndex);
    int GetIsNight();
    int AddStaticBoundingBox(unsigned long nId, const Vector& vMin,
                             const Vector& vMax);
    int RemoveStaticBoundingBox(unsigned long nId);

    // PC 0x0063BFA0  Xbox 0x00095F80
    CResRef GetDefaultEnvMap() { return m_cDefaultEnvMap; }
    int GetWidth() { return m_nWidth; }
    int GetHeight() { return m_nHeight; }

    int m_nFlags;

    int m_nWidth;
    int m_nHeight;
    int m_n10;
    int m_n14;
    unsigned long* m_p18;



    int* m_p1C;
    int* m_p20;
    int* m_p24;
    int* m_p28;
    int m_n2C;
    int m_n30;
    int* m_p34;
    int m_n38;
    int m_n3C;
    Vector* m_p40;
    int m_nBoundingBoxAlloc;
    int m_nBoundingBoxCount;
    Vector* m_pBoundingBoxes;
    unsigned long* m_pBoundingBoxIds;
    Vector m_vFogColor;
    float m_fFogNear;
    float m_fFogFar;
    unsigned long m_nMoonAmbientColor;
    unsigned long m_nMoonDiffuseColor;
    unsigned long m_nMoonFogColor;
    float m_fMoonFogNear;
    float m_fMoonFogFar;
    unsigned char m_bMoonFogOn;
    unsigned char m_aUnused7D[3];
    int m_bMoonShadows;
    unsigned long m_nSunAmbientColor;
    unsigned long m_nSunDiffuseColor;
    unsigned long m_nSunFogColor;
    float m_fSunFogNear;
    float m_fSunFogFar;
    char m_bSunFogOn;
    unsigned char m_aUnused99[3];
    int m_bSunShadows;
    int m_bDayNightCycle;
    int m_bIsNight;


    unsigned char m_nChanceRain;
    unsigned char m_nChanceSnow;
    unsigned char m_nChanceLightning;
    unsigned char m_nWind;
    unsigned long m_nDynAmbientColor;
    int m_bNoRest;
    unsigned char m_nShadowOpacity;
    int m_nTurnRateRow;
    CResRef m_cDefaultEnvMap;

    unsigned long m_nGrassAmbient;
    unsigned long m_nGrassDiffuse;
    float m_fGrassDensity;
    float m_fGrassQuadSize;

    enum GrassProbability { GRASS_PROB_LL, GRASS_PROB_LR, GRASS_PROB_UL, GRASS_PROB_UR,
                            GRASS_PROB_COUNT };
    float m_fGrassProbability[GRASS_PROB_COUNT];
    CResRef m_cGrassTexName;
    float m_fAlphaTest;

public:

    int GetFlags()
    {
        return m_nFlags;
    }
    unsigned char GetShadowOpacity()
    {
        return m_nShadowOpacity;
    }
    void SetMoonFogColor(unsigned long nMoonFogColor)
    {
        m_nMoonFogColor = nMoonFogColor;
    }
    void SetSunFogColor(unsigned long nSunFogColor)
    {
        m_nSunFogColor = nSunFogColor;
    }
    unsigned long GetMoonFogColor()
    {
        return m_nMoonFogColor;
    }
    unsigned long GetSunFogColor()
    {
        return m_nSunFogColor;
    }
};

#endif
