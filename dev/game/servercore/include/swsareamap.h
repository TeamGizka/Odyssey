#ifndef __SWSAREAMAP_H__
#define __SWSAREAMAP_H__

class Vector;
#include "math3d.h"

class CSWSAreaMap
{
public:

    static const int MAP_PIXEL_WIDTH = 588;
    static const int MAP_PIXEL_HEIGHT = 294;

    static const int EXPLORED_BITS_PER_WORD = 32;

    enum MapOrientation
    {
        ORIENTATION_HALF_TURN = 1,
        ORIENTATION_CLOCKWISE = 2,
        ORIENTATION_COUNTERCLOCKWISE = 3
    };

    CSWSAreaMap();
    ~CSWSAreaMap();

    void Initialize(int bEnabled, int nMapWidth, unsigned long nNorthAxis,
                    float fWorldX1, float fWorldY1, float fWorldX2,
                    float fWorldY2, int nMapX1, int nMapY1, int nMapX2,
                    int nMapY2, int nZoom);
    int GetMapPixelFromWorldCoord(Vector vPosition, int& nMapX, int& nMapY);
    float GetMapRotateCCWFromWorldOrientation(Vector vOrientation);
    int GetGridPixelFromMapPixel(int nMapX, int nMapY, int& nGridX,
                                 int& nGridY);
    void SetEntireMapExplored();
    void LoadSavedAreaMapData(unsigned long* pData, unsigned long nSize,
                              int nWidth, int nHeight);
    void SetMapPointExplored(int nMapX, int nMapY);
    unsigned long* GetMapData(unsigned long& nSize, int& nWidth, int& nHeight);
    void SetPartyMemberWorldLocation(int nMember, Vector vPosition);
    void SetPartyMemberWorldOrientation(int nMember, Vector vOrientation);
    int GetPartyMemberMapLocation(int nMember, int& nMapX, int& nMapY);
    float GetPartyMemberMapOrientation(int nMember);
    int IsWorldPointExplored(Vector vPosition);
    void SetWorldPointExplored(Vector vPosition, int nRadius);
    int GetOrientation() { return m_nOrientation; }

    unsigned long* m_pnExplored;
    unsigned long m_nExploredSize;
    int m_nWidth;
    int m_nHeight;
    int m_nOrientation;

    float m_fInverseZoom;
    float m_fScaleX;
    float m_fScaleY;
    float m_fOffsetX;
    float m_fOffsetY;
    int m_nPartyMapX[3];
    int m_nPartyMapY[3];
    float m_fPartyOrientation[3];
};

#endif
