#ifndef __NWCAREA_H__
#define __NWCAREA_H__



#define ROOM_SOUND_CALLBACK_DELAY 10000.0f

class CAurCamera;
class CGuiAreaLoadscreen;
class CSWBehaviorAttachSound;
class CExoLinkedListInternal;
class CSWCAmbientSound;
class CSWCMapPin;
class CSWCRoom;
class CSWMiniGame;
class CSWSArea;
class CWalkHitInfo;
class Vector;
#include "exoresman.h"
#include "math3d.h"
#include <aurlist.h>
#include "exobase.h"
#include "nwarea.h"
#include "nwsobject.h"

class CAurScene;
class CAurObject;
class Gob;



struct CSWCPlaceableObjectLight
{
    unsigned long m_oidPlaceable;
    Gob* m_pLight;
};

void PlayRoomSoundEvent(CAurObject* pObject, char* sEvent, void* pData);

class CSWCArea : public CSWArea, public CGameObject
{
public:


    enum { GRID_COORDINATE_INVALID = 0xFF };




    enum { WEATHER_FADE_DURATION = 6000 };
    enum { LIGHTNING_DELAY_RANGE = 5000 };
    enum { WIND_GUST_DELAY_RANGE = 5000 };


    static const int MAX_INTERTILE_PATH_DEPTH;

    CSWCArea();
    virtual ~CSWCArea();
    // PC 0x006524E0
    virtual CSWCArea* AsSWCArea()
    {
        return this;
    }

    virtual int SetWind(unsigned char nWind);
    virtual int SetGlobalLightDiffuse(int nLight, Vector vColor);
    virtual int SetGlobalLightAmbient(int nLight, Vector vColor);
    virtual int SetWeather(unsigned char nWeather, float fStrength);
    virtual int SetFog(float fNear, float fFar, const Vector& vColor);
    int SetFogColor(const Vector& vColor);
    void AttachmentFromServerArea(CSWSArea* pArea);
    void RemovePlaceableObjectLight(unsigned long oidPlaceable, int bFlag);
    void UpdateShadowingLights();
    int GetIsDark();
    void SetAsNight();
    void SetAsDay();
    void SetAsDawn();
    void SetAsDusk();
    CSWMiniGame* RestartMiniGame();
    void ForceBrightSunAndMoon();
    void UpdateSoundObjectsTimeOfDay();
    Gob* GetPlaceableObjectLight(unsigned long oidPlaceable);
    int AddPlaceableObjectLight(unsigned long oidPlaceable,
                                unsigned char nAppearance, Vector vPosition);
    int AddPlaceableObjectStatic(unsigned long oidPlaceable,
                                 unsigned char nAppearance, Vector vPosition,
                                 Vector vOrientation);
    void ClearAllMapPins();
    void UpdateDayNightCycle(float fTimeStep);
    void ClearTilePathNodes();
    void EmptyCAurObjectToHitCheckIgnoreList();
    void SetLightningDone();
    void UnloadArea();
    int LoadArea(CAurCamera* pCamera, CGuiAreaLoadscreen* pLoadScreen,
                 int bTransition);
    void HiliteAreaObjects(int bHilite);
    void Update(float fTimeDelta);
    CSWSArea* GetServerArea();
    int GetObjectsAtPositionVisible(const Vector& vPosition);
    CSWCMapPin* GetMapPin(unsigned long nReference, int nType);
    void AddMapPin(CSWCMapPin* pMapPin);
    void AddCorpse(unsigned long oidCorpse);
    int TestDirectLine(unsigned long oidObject, const Vector& vStart,
                       const Vector& vEnd, const float& fDistance, float fWidth,
                       int nFlags, CWalkHitInfo* pWalkHitInfo);
    void StartWeather(unsigned char nWeather);
    void StopWeather(unsigned char nWeather);
    void DetachFromServerArea();
    void StartSounds();
    int ParentCorpseExists(unsigned long oidParent);
    int GetSurfaceMaterial(Vector vPosition);
    int ApplyFireAndForgetVisualEffect(unsigned short nEffect,
                                       Vector vPosition);
    int GetIsObjectCorpse(unsigned long oidObject);
    int ClearLineOfSightOneWay(Vector vStart, Vector vEnd, Vector* pHit);
    void ChangeTriggerDisplay(int bDisplay);
    void DisplayAllTriggers(int bDisplay);
    CSWCRoom* GetRoom(int nRoom);
    CSWCRoom* GetRoom(const Vector& vPosition);
    CSWCRoom* GetRoom(const CExoString& sRoomName);
    int GetRoomIndex(const Vector& vPosition, int nFlags);

    CResRef m_cAreaResRef;

    CExoArrayList<unsigned long> m_lstRenderTriggerIds;
    Vector m_vNightAmbientColor;
    Vector m_vNightDiffuseColor;
    Vector m_vMoonColor;
    Vector m_vDayAmbientColor;
    Vector m_vDayDiffuseColor;
    Vector m_vSunColor;
    Vector m_vUnknown170;


    CExoString m_sAreaName;
    CAurScene* m_pScene;
    Gob* m_pShadowLight;
    Gob* m_apWeatherEffects[6];
    Gob* m_pLightningEffect;

    int m_nID;
    int m_nCreatorID;
    unsigned long m_nVersion;
    int m_bDawning;
    int m_bDusking;
    int m_nUnknown1BC;
    int m_bLightingChanged;
    CExoArrayList<CSWCMapPin*> m_lstMapPins;
    CExoArrayList<unsigned long> m_lstSoundObjectIds;
    CSWCAmbientSound* m_pAmbientSound;
#ifdef _XBOX
    CSWCAmbientSound* GetAmbientSound() { return m_pAmbientSound; }

    void SetGeometryFadeMode(unsigned char nMode);
#endif
    float m_fLightningTimer;


    float m_fLightningFlashTime;
    int m_nUnknown1E8;
    unsigned long m_nWeatherDuration;
    unsigned char m_nWeatherType;
    unsigned char m_nNewWeatherType;
    unsigned char m_aUnknown1F2[0x2];

    float m_fWeatherStrength;
    float m_fUnknown1F8;
    unsigned long m_nCalendarDay;
    unsigned long m_nTimeOfDay;
    CExoLinkedList<CAurObject> m_lstHitCheckIgnoreObjects;
    int m_nUnknown208;
    CExoArrayList<unsigned long> m_lstObjectIds;
    CExoArrayList<CSWCPlaceableObjectLight*> m_lstPlaceableLights;
    CExoArrayList<unsigned long> m_lstDeferredDeleteIds;
    int m_nUnknown230;
    int m_bIsDark;


    List<Vector> m_lstTilePathNodes;
    List<int> m_lstTilePathNodeIds;
    void* m_pAreaGrid;
    unsigned char m_nGridX;
    unsigned char m_nGridY;
    unsigned char m_nGridZ;
    unsigned char m_aUnknown257[0x1];
    int m_nGridCellCount;
    int m_nRoomCount;
    CSWCRoom* m_pRooms;
    CSWMiniGame* m_pMiniGame;
    unsigned long m_anCorpseObjectIds[4];
    unsigned char m_nCorpseReadIndex;
    unsigned char m_nCorpseWriteIndex;
    unsigned char m_aUnknown27A[0x2];
    CSWSArea* m_pServerArea;
#ifndef _XBOX
    CExoLinkedList<CSWBehaviorAttachSound> m_lstRoomSounds;
    CExoArrayList<CAurObject*> m_a288;
#endif
};

#endif
