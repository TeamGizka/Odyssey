#ifndef AURLIGHTMANAGER_H
#define AURLIGHTMANAGER_H

#include <aurlist.h>
#include "aurmodel.h"
#include "math3d.h"

class CAurObjectLightContainer;
class CAurRoom;
class Scene;

#define AUR_MINIMUM_SHADOW_COLOR 0.1f

#define AUR_INACTIVE_LIGHT_PRIORITY 100000.0f

#define AUR_MINIMUM_LIGHT_RADIUS 0.1f
#define AUR_LIGHT_ATTENUATION_SCALE 9.0f

#define AUR_EMITTER_LIGHT_MINIMUM_DISTANCE 0.000001f
#define AUR_EMITTER_LIGHT_DEFAULT_RADIUS 0.01f

#define AUR_PRIMARY_SHADOW_RADIUS 1000.0f

class LightManager
{
public:

    enum { MAX_ENABLED_LIGHTS = 3 };

    enum { PRIMARY_SHADOW_PRIORITY_LIMIT = 6 };

    enum ShadowLightType
    {
        SHADOWLIGHTS_ALL = 0,
        SHADOWLIGHTS_STATIC = 1,
        SHADOWLIGHTS_PRIMARY = 2
    };

    LightManager();
    ~LightManager();

    virtual void PrioritizeLights(const Vector& vPosition, bool bDynamicOnly);
    virtual void EnableLights();
    virtual void EnableLightsDynamicOnly();
    virtual void AddLight(PartLight* pLight);
    virtual void RemoveLight(PartLight* pLight, bool bAll);
    virtual void AddRenderLight(PartLight* pLight);
    virtual void Reset();
    virtual void UpdateLights();
    virtual void EnableHighestPriorityLights(int nMaxLights);

    virtual void EnableHighestPriorityLightsFade(
        int nMaxLights, CAurObjectLightContainer* pContainer);

#ifdef _XBOX
    virtual void DebugPrintLightInfo(int nMaxLights, Vector vPosition);
#else
    virtual void DebugPrintLightInfo(int nMaxLights, Vector vPosition) {}  // PC 0x00693D50 GOG 0x00693AC0
#endif

    virtual void DisableBumpMapLights();
    virtual void EnableBumpMapLights();

    virtual void CollectLightsForGivenRoom(CAurRoom* pRoom);
    virtual void SetupLightsForThisGob(const Vector& vPosition, float fRadius)  // PC 0x004CF540 GOG 0x004CF680
    {
        SetupLightsForThisPositionAndRadius(vPosition, fRadius, m_lstNearbyLights);
        m_bDynamicOnly = 0;
    }
    virtual void SetupLightsForThisRoom(const Vector& vPosition,  // PC 0x004CF570 GOG 0x004CF6B0
                                              float fRadius)
    {
        SetupLightsForThisPositionAndRadius(vPosition, fRadius, m_lstNearbyLights);
    }
    virtual void SetupLightsForThisPositionAndRadius(
        const Vector& vPosition, float fRadius, List<PartLight*>& lstLights);
    virtual void SetupLightsForThisPositionAndRadius(const Vector& vPosition,
                                                     float fRadius);
    // PC 0x006049D0 GOG 0x006049B0
    virtual void CombineLights(const List<PartLight*>& lstLights, List<PartLight*>& lstCombined) {}
    virtual void PrioritizeIllumination(bool bDynamicOnly);
    virtual void PrioritizeShadow();
    virtual void PrioritizeBump();
    virtual void GroupLights() {}  // PC 0x004015A0 GOG 0x004015A0
    virtual void ControlLightsFade(List<PartLight*>& lstFadeIn,
                                  List<PartLight*>& lstFadeOut,
                                  List<PartLight*>& lstLights,
                                  int bDynamicOnly);
    virtual void EnableHeadLight();

    List<PartLight*>* GetBumpMapLights();
    List<PartLight*>* GetLensFlareLights();
    List<PartLight*>* GetShadowLights(int nType);

    Vector GetEmitterLighting(Vector vPosition, List<PartLight*>* pExcludeLights);
    int GetNearestLights(Scene* pScene, Vector vPosition, float fRadius,
                         int bDynamicOnly);
    int DisableThisLight(PartLight* pLight);
    int EnableThisLight(PartLight* pLight);

    void EnablePhongLights(int nMaxLights, int bSkipBumpMapLights);

    enum LightProgramParameter
    {
        LIGHT_PROGRAM_CAMERA_X = 92,
        LIGHT_PROGRAM_CAMERA_Y = 91,
        LIGHT_PROGRAM_CAMERA_Z = 90,
        LIGHT_PROGRAM_ZERO_ONE = 89,
        LIGHT_PROGRAM_CAMERA_POSITION = 88,
        LIGHT_PROGRAM_FIRST_LIGHT = 87,
        LIGHT_PROGRAM_COLOR_SCALE = 15
    };

    enum LightProgramLightOffset
    {
        LIGHT_PROGRAM_POSITION_OFFSET = 5,
        LIGHT_PROGRAM_AMBIENT_OFFSET = 4,
        LIGHT_PROGRAM_DIFFUSE_OFFSET = 3,
        LIGHT_PROGRAM_ATTENUATION_OFFSET = 1
    };

    enum { LIGHT_PROGRAM_TEMP_SIZE = 1024,
           LIGHT_PROGRAM_LARGE_TEMP_SIZE = 4096 };
    static char* SetupVertexProgramLights(char* sProgram, int nLights,
                                          int bWorldSpace);

    void SetUpVertexProgramLightShaderConstants(int nLights, int bWorldSpace);

    static int cm_bLightManagerUpdated;
    static int m_nMaxShadowLights;
    static CAurObject* m_pOnlyGob;
    static int m_nMaxLights;
#ifdef _XBOX
    static void SetMaxLights(int nMaxLights);
    static void SetMaxShadowLights(int nMaxShadowLights);
#endif

    int m_bDynamicOnly;
    List<PartLight*> m_lstLights;
    List<PartLight*> m_lstShadowLights;
    List<PartLight*> m_lstStaticLights;
    List<PartLight*> m_lstRenderLights;
    List<PartLight*> m_lstNearbyLights;
    List<PartLight*> m_lstEnabledLights;
    List<PartLight*> m_lstPrioritizedLights;
    List<PartLight*> m_lstBumpMapLights;
    List<PartLight*> m_lstFadingLights;
    List<PartLight*> m_lstFadingDynamicLights;
    List<PartLight*> m_lstFadedLights;
    List<PartLight*> m_lstFadedDynamicLights;
    List<PartLight*> m_lstShadowCasters;
    PartLight* m_pShadowLight;

    int m_nStateA8;
    List<PartLight*> m_lstShadowCastersStatic;
    List<PartLight*> m_lstShadowCastersBump;
    List<PartLight*> m_lstLensFlareLights;

    List<PartLight*> m_lstExcludedShaderLights;

    List<PartLight*> m_lstPreviousFadeLights;
    List<float> m_lstPreviousFadeIntensities;
    char m_bBumpMapLightsDisabled;
};

int lightcmp(const void* pFirst, const void* pSecond);
void SetLightGL(int nLight, const Vector& vPosition, const Vector& vColor,
                const float& fRadius, const int& bAmbientOnly);
// PC 0x00448F30 GOG 0x00449080
inline char* MdlNode::TranslateNodeName()
{
    if (m_pModel->AsModel())
    {
        return m_pModel->AsModel()->m_lNames[m_nIndex];
    }
    else
    {
        return ((Model*)m_pModel->AsAnimation()->m_pBinaryData)->m_lNames[m_nIndex];
    }
}

#endif
