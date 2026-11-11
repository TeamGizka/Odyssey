#include <windows.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aurcamera.h"
#include "aurlightmanager.h"
#include "aurinternal.h"
#include "aurlightmanager.h"
#include "aurrendergl.h"

static inline void AurlightmanagerTrace1();
#define AURLIGHTMANAGER_TRACE() AurlightmanagerTrace1()

extern int dynamiclightscount;
extern int staticlightscount;
extern Vector headlightambient;
extern Vector headlightdiffuse;

CAurObject* LightManager::m_pOnlyGob = 0;

int LightManager::m_nMaxLights = 3;
int LightManager::m_nMaxShadowLights = 1;
int LightManager::cm_bLightManagerUpdated = 1;
float lightfadespeed = 3.0f;
float lightshadowfadespeed = 3.0f;
int countenabledlights = 0;
int dynamiclightscount = 0;
int staticlightscount = 0;
int containsnegative = 0;

int nosortlights = 0;

int enablebumpselection = 0;
int bumpselection = 0;
int bumplightscount = 0;
float shadowfadespeed = 0.3f;
// PC 0x004CF020
List<PartLight*>* LightManager::GetBumpMapLights()
{
    return &m_lstBumpMapLights;
}
// PC 0x004CF030
List<PartLight*>* LightManager::GetLensFlareLights()
{
    return &m_lstLensFlareLights;
}
// PC 0x004CF040
int lightcmp(const void* pFirst, const void* pSecond)
{
    PartLight* pLight1 = (*(Part* const*)pFirst)->AsPartLight();
    PartLight* pLight2 = (*(Part* const*)pSecond)->AsPartLight();
    MdlNodeLight* pNode1 = (MdlNodeLight*)pLight1->m_pNode;
    MdlNodeLight* pNode2 = (MdlNodeLight*)pLight2->m_pNode;
    int nPriority1 = pLight1->m_nPriority ? pLight1->m_nPriority :
                    pNode1->m_nPriority;
    int nPriority2 = pLight2->m_nPriority ? pLight2->m_nPriority :
                    pNode2->m_nPriority;
    if (equal(pLight1->m_vColor, Vector()))
        return 1;
    else if (equal(pLight2->m_vColor, Vector()))
        return -1;
    else if (nPriority1 < nPriority2)
        return -1;
    else if (nPriority1 > nPriority2)
        return 1;
    else if (pLight1->m_nLightType == PartLight::LIGHTTYPE_STATIC &&
        pLight2->m_nLightType > PartLight::LIGHTTYPE_STATIC)
        return 1;
    if (pLight1->m_nLightType == PartLight::LIGHTTYPE_DYNAMIC)
    {
        if (pLight2->m_nLightType == PartLight::LIGHTTYPE_STATIC ||
            pLight2->m_nLightType == PartLight::LIGHTTYPE_SECONDARY)
            return -1;
    }
    else if (pLight1->m_nLightType == PartLight::LIGHTTYPE_SECONDARY)
    {
        if (pLight2->m_nLightType == PartLight::LIGHTTYPE_STATIC)
            return -1;
    }
    else if (pLight2->m_nLightType == PartLight::LIGHTTYPE_DYNAMIC)
        return 1;
    if (pLight1->m_fPriorityValue < pLight2->m_fPriorityValue)
        return -1;
    if (pLight1->m_fPriorityValue > pLight2->m_fPriorityValue)
        return 1;
    return pLight2 < pLight1 ? 1 : -1;
}
// PC 0x004CF180
void SetLightGL(int nLight, const Vector& vPosition, const Vector& vColor,
                const float& fRadius, const int& bAmbientOnly)
{
    if (bAmbientOnly)
    {
        float* pAmbient = Homogenize(vColor, 1.0f);
        GLRender_SetLightProperties(nLight + 1, LIGHT_AMBIENT, pAmbient);
        GLRender_SetLightProperties(nLight + 1, LIGHT_DIFFUSE, Homogenize(Vector(), 1.0f));
        GLRender_SetLightProperties(nLight + 1, LIGHT_SPECULAR, Homogenize(Vector(), 1.0f));
    }
    else
    {
        float* pAmbient = Homogenize(Vector(), 1.0f);
        GLRender_SetLightProperties(nLight + 1, LIGHT_AMBIENT, pAmbient);
        GLRender_SetLightProperties(nLight + 1, LIGHT_DIFFUSE, Homogenize(vColor, 1.0f));
        GLRender_SetLightProperties(nLight + 1, LIGHT_SPECULAR, Homogenize(vColor, 1.0f));
    }
    GLRender_SetLightProperties(nLight + 1, LIGHT_POSITION, Homogenize(vPosition, 1.0f));

    float fClampedRadius = fRadius > AUR_MINIMUM_LIGHT_RADIUS ?
                          fRadius : AUR_MINIMUM_LIGHT_RADIUS;
    float fAttenuation = AUR_LIGHT_ATTENUATION_SCALE /
                        (fClampedRadius * fClampedRadius +
                         fClampedRadius * fClampedRadius);
    GLRender_SetLightProperties(nLight + 1, LIGHT_QUADRATIC_ATTENUATION, &fAttenuation);
}
// PC 0x004CF340
LightManager::~LightManager()
{
}

Vector LightManager::GetEmitterLighting(Vector vPosition,
                                        List<PartLight*>* pExcludeLights)
{
    Vector vLighting;
    for (int i = 0; i < m_lstFadingLights.m_nCount; i++)
    {
        PartLight* pLight = m_lstFadingLights[i];
        if (pExcludeLights != NULL && pExcludeLights->Contains(pLight))
            continue;

        float fDistance = magnitude(pLight->m_vWorldPosition - vPosition);
        if (fDistance == 0.0f)
            fDistance = AUR_EMITTER_LIGHT_MINIMUM_DISTANCE;
        float fRadius = pLight->m_fRadius;
        if (fRadius == 0.0f)
            fRadius = AUR_EMITTER_LIGHT_DEFAULT_RADIUS;
        float fIntensity = 1.0f - fDistance / fRadius;
        if (fIntensity < 0.0f)
            continue;

        vLighting += pLight->m_vColor * fIntensity * pLight->m_fColorMultiplier;
    }

    if (CAurInternal::m_nHeadLightEnabled)
        vLighting = vLighting + headlightdiffuse + headlightambient;

    if (vLighting.m_fX >= 1.0f)
        vLighting.m_fX = 1.0f;
    if (vLighting.m_fY >= 1.0f)
        vLighting.m_fY = 1.0f;
    if (vLighting.m_fZ >= 1.0f)
        vLighting.m_fZ = 1.0f;
    return vLighting;
}
// PC 0x004CF530
inline void LightManager::Reset()

{
    AURLIGHTMANAGER_TRACE();
    m_lstRenderLights.m_nCount = 0;
}
// PC 0x004CF590
void LightManager::UpdateLights()
{
    Quaternion qOrientation;

    for (int i = 0; i < m_lstLights.m_nCount; i++)
    {
        m_lstLights[i]->GetWorld(m_lstLights[i]->m_vWorldPosition,
                                 qOrientation);
    }
}
// PC 0x004CF5F0
void LightManager::EnableHighestPriorityLights(int nMaxLights)
{
    if (m_lstEnabledLights.m_nCount != 0)
    {
        if (nMaxLights >= m_lstEnabledLights.m_nCount)
            nMaxLights = m_lstEnabledLights.m_nCount;

        int i;
        for (i = 0; i < nMaxLights; i++)
        {
            GLRender_EnableLight(i + 1);
#ifdef _XBOX
            PartLight*& pLight = m_lstEnabledLights[i];
            Vector vColor = pLight->m_vColor * pLight->m_fColorMultiplier;
            SetLightGL(i, pLight->m_vWorldPosition, vColor,
                       pLight->m_fRadius,
                       pLight->m_bAmbientOnly);
#else
            float fMultiplier = m_lstEnabledLights[i]->m_fColorMultiplier;
            Vector vColor(fMultiplier * m_lstEnabledLights[i]->m_vColor.m_fX,
                          fMultiplier * m_lstEnabledLights[i]->m_vColor.m_fY,
                          fMultiplier * m_lstEnabledLights[i]->m_vColor.m_fZ);
            SetLightGL(i, m_lstEnabledLights[i]->m_vWorldPosition, vColor,
                       m_lstEnabledLights[i]->m_fRadius,
                       m_lstEnabledLights[i]->m_bAmbientOnly);
#endif
        }
        for (int j = i; j < MAX_ENABLED_LIGHTS; j++)
            GLRender_DisableLight(j + 1);
    }
}
// PC 0x004CF6B0
void LightManager::PrioritizeLights(const Vector& vPosition, bool bDynamicOnly)
{
    Vector vZero;
    GroupLights();
    for (int i = 0; i < m_lstRenderLights.m_nCount; i++)
    {
        PartLight* pLight = m_lstRenderLights[i];
        Vector vDistance(pLight->m_vWorldPosition.m_fX - vPosition.m_fX,
                         pLight->m_vWorldPosition.m_fY - vPosition.m_fY, 0.0f);
        if (equal(pLight->m_vColor, vZero) || !(pLight->m_fRadius > 0.0f))
            pLight->m_fPriorityValue = AUR_INACTIVE_LIGHT_PRIORITY;
        else
            pLight->m_fPriorityValue = magnitude(vDistance) / pLight->m_fRadius;
    }
    PrioritizeIllumination(bDynamicOnly);
}
// PC 0x004CF770
void LightManager::EnableHeadLight()
{
    AurlightmanagerTrace1();
    GLRender_SetLightProperties(0, LIGHT_AMBIENT, Homogenize(headlightambient, 1.0f));
    GLRender_SetLightProperties(0, LIGHT_DIFFUSE, Homogenize(headlightdiffuse, 1.0f));
    GLRender_SetLightProperties(0, LIGHT_POSITION, Homogenize(CurrentCamera->m_vPosition, 1.0f));
    AurEnableHeadLight();
}
// PC 0x004CF810
void LightManager::EnableLights()
{
    AurlightmanagerTrace1();
    if (enablegetnearestlights == 0)
    {
        if (m_lstEnabledLights.m_nCount > 0)
        {
            dynamiclightscount = m_lstEnabledLights.m_nCount;
        }

        ControlLightsFade(m_lstFadingLights, m_lstFadedLights,
                          m_lstEnabledLights, 0);

        if (CAurInternal::m_nHeadLightEnabled == 1)
        {
            EnableHeadLight();
        }
        else
        {
            AurDisableHeadLight();
        }

        m_bDynamicOnly = 0;
    }
}
// PC 0x004CFD80
char* LightManager::SetupVertexProgramLights(char* sProgram, int nLights,
                                            int bWorldSpace)
{
    if (nLights < 1)
        return "";

    Matrix mCamera;
    Vector vCamera;
    if (CurrentCamera != NULL)
    {
        mCamera = CurrentCamera->m_qOrientation.getmatrix();
        vCamera = CurrentCamera->m_vPosition;
    }
    AurSetVertexProgramParameter4f(LIGHT_PROGRAM_CAMERA_X,
        mCamera.m_vFirst.m_fX, mCamera.m_vSecond.m_fX,
        mCamera.m_vThird.m_fX, vCamera.m_fX);
    AurSetVertexProgramParameter4f(LIGHT_PROGRAM_CAMERA_Y,
        mCamera.m_vFirst.m_fY, mCamera.m_vSecond.m_fY,
        mCamera.m_vThird.m_fY, vCamera.m_fY);
    AurSetVertexProgramParameter4f(LIGHT_PROGRAM_CAMERA_Z,
        mCamera.m_vFirst.m_fZ, mCamera.m_vSecond.m_fZ,
        mCamera.m_vThird.m_fZ, vCamera.m_fZ);

    GLRender_SetVertexProgramParameter4f(LIGHT_PROGRAM_ZERO_ONE, 0.0f, 0.0f, 0.0f, 1.0f);
#ifdef _XBOX
    GLRender_SetVertexProgramParameter4f(LIGHT_PROGRAM_COLOR_SCALE, 1.0f, 1.0f, 1.0f, 0.0f);
#else
    GLRender_SetVertexProgramParameter4f(LIGHT_PROGRAM_COLOR_SCALE, 1.0f, 1.0f, 1.0f, 1.0f);
#endif
    AurSetVertexProgramParameter4f(LIGHT_PROGRAM_CAMERA_POSITION,
        vCamera.m_fX, vCamera.m_fY, vCamera.m_fZ, 0.0f);

    int nParameter = LIGHT_PROGRAM_FIRST_LIGHT;
    static char temp[LIGHT_PROGRAM_LARGE_TEMP_SIZE];
    static char temp2[LIGHT_PROGRAM_TEMP_SIZE];
    static char temp3[LIGHT_PROGRAM_TEMP_SIZE];
#ifdef _XBOX
    sprintf(temp,
        "mov r9.xyzw, c[89].xxxw;\n"
        "dp4 r0.x, r10, c[92];\n"
        "dp4 r0.y, r10, c[91];\n"
        "dp4 r0.z, r10, c[90];\n"
        "dp4 r0.w, r10, c[89];\n"
        "mov r10, r0;\n"
        "dp3 r0.x, r11, c[92];\n"
        "dp3 r0.y, r11, c[91];\n"
        "dp3 r0.z, r11, c[90];\n"
        "mov r11, r0;\n");
#else
    if (usearbvertexprograms)
        sprintf(temp,
            "MOV vFinalColour, program.env[89].xxxw;\n"
            "DP4 vReg0.x, vReg10, program.env[92];\n"
            "DP4 vReg0.y, vReg10, program.env[91];\n"
            "DP4 vReg0.z, vReg10, program.env[90];\n"
            "DP4 vReg0.w, vReg10, program.env[89];\n"
            "MOV vReg10, vReg0;\n"
            "DP3 vReg0.x, vNormalWorld, program.env[92];\n"
            "DP3 vReg0.y, vNormalWorld, program.env[91];\n"
            "DP3 vReg0.z, vNormalWorld, program.env[90];\n"
            "MOV vNormalWorld, vReg0;\n");
    else
        sprintf(temp,
            "MOV R9.xyzw, c[89].xxxw;DP4 R0.x, R10, c[92];"
            "DP4 R0.y, R10, c[91];DP4 R0.z, R10, c[90];"
            "DP4 R0.w, R10, c[89];MOV R10, R0;DP3 R0.x, R11, c[92];"
            "DP3 R0.y, R11, c[91];DP3 R0.z, R11, c[90];MOV R11, R0;");
#endif
    strcat(sProgram, temp);

    for (int i = 0; i < nLights; i++)
    {
        GLRender_SetVertexProgramParameter4f(nParameter, 0.0f, 0.0f, 0.0f, 0.0f);
        nParameter--;
        GLRender_SetVertexProgramParameter4f(nParameter, 0.0f, 0.0f, 0.0f, 1.0f);
        nParameter--;
        GLRender_SetVertexProgramParameter4f(nParameter, 0.0f, 0.0f, 0.0f, 1.0f);
        nParameter--;
        GLRender_SetVertexProgramParameter4f(nParameter, 0.0f, 0.0f, 0.0f, 1.0f);
        nParameter--;
#ifdef _XBOX
        GLRender_SetVertexProgramParameter4f(nParameter, 1.0f, 0.0f, 0.0f, 1.0f);

        nParameter--;
        sprintf(temp,
            "add r0, -r10, c[%d];\n"
            "dp3 r0.w, r0, r0;\n"
            "rsq r1.w, r0.w;\n"
            "mul r2.xyzw, r0.xyzw, r1.wwww;\n"
            "dst r3, r0.w, r1.w;\n"
            "dp3 r3.w, r3, c[%d];\n"
            "rcp r3.w, r3.w;\n"
            "dp3 r0.x, r11, r2;\n"
            "lit r0, r0;\n"
            "mul r0, r0, r3.w;\n",
            nParameter + LIGHT_PROGRAM_POSITION_OFFSET,
            nParameter + LIGHT_PROGRAM_ATTENUATION_OFFSET);
        sprintf(temp2, "mul r1, r0.x, c[%d];\nmad r9.xyzw, r1, c[95], r9;\n",
            nParameter + LIGHT_PROGRAM_AMBIENT_OFFSET);
        sprintf(temp3, "mul r1, r0.y, c[%d];\nmad r9.xyzw, r1, c[94], r9;\n",
            nParameter + LIGHT_PROGRAM_DIFFUSE_OFFSET);
#else
        GLRender_SetVertexProgramParameter4f(nParameter, 1.0f, 1.0f, 1.0f, 1.0f);

        nParameter--;
        if (usearbvertexprograms)
        {
            sprintf(temp,
                "ADD vReg0, -vReg10, program.env[%d];\n"
                "DP3 vReg0.w, vReg0, vReg0;\nRSQ vReg1.w, vReg0.w;\n"
                "MUL vReg2, vReg0, vReg1.w;\nDST vReg3, vReg0.w, vReg1.w;\n"
                "DP3 vReg3.w, vReg3, program.env[%d];\nRCP vReg3.w, vReg3.w;\n"
                "DP3 vReg0.x, vNormalWorld, vReg2;\nLIT vReg0, vReg0;\n"
                "MUL vReg0, vReg0, vReg3.w;\n",
                nParameter + LIGHT_PROGRAM_POSITION_OFFSET,
                nParameter + LIGHT_PROGRAM_ATTENUATION_OFFSET);
            sprintf(temp2,
                "MUL vReg1, vReg0.x, program.env[%d];\n"
                "MAD vFinalColour.xyzw, vReg1, program.env[95], vFinalColour;\n",
                nParameter + LIGHT_PROGRAM_AMBIENT_OFFSET);
            sprintf(temp3,
                "MUL vReg1, vReg0.y, program.env[%d];\n"
                "MAD vFinalColour.xyzw, vReg1, program.env[94], vFinalColour;\n",
                nParameter + LIGHT_PROGRAM_DIFFUSE_OFFSET);
        }
        else
        {
            sprintf(temp,
                "ADD R0, -R10, c[%d];DP3 R0.w, R0, R0;RSQ R1.w, R0.w;"
                "MUL R2, R0, R1.w;DST R3, R0.w, R1.w;DP3 R3.w, R3, c[%d];"
                "RCP R3.w, R3.w;DP3 R0.x, R11, R2;LIT R0, R0;MUL R0, R0, R3.w;",
                nParameter + LIGHT_PROGRAM_POSITION_OFFSET,
                nParameter + LIGHT_PROGRAM_ATTENUATION_OFFSET);
            sprintf(temp2, "MUL R1, R0.x, c[%d];MAD R9.xyzw, R1, c[95], R9;",
                nParameter + LIGHT_PROGRAM_AMBIENT_OFFSET);
            sprintf(temp3, "MUL R1, R0.y, c[%d];MAD R9.xyzw, R1, c[94], R9;",
                nParameter + LIGHT_PROGRAM_DIFFUSE_OFFSET);
        }
#endif
        strcat(sProgram, temp);
        strcat(sProgram, temp2);
        strcat(sProgram, temp3);
    }
#ifdef _XBOX
    strcat(sProgram, "add r9.xyz, r9.xyz, c[93].xyz;\n");
    strcat(sProgram, "mov r9.w, c[94].w;\n");
    strcat(sProgram, "mul oD0.xyzw, r9.xyzw, c[15].xxxx;\n");
#else
    if (usearbvertexprograms)
    {
        strcat(sProgram, "ADD vFinalColour, vFinalColour, program.env[93];\n");
        strcat(sProgram, "MOV vFinalColour.w, program.env[94];\n");
        strcat(sProgram, "MOV result.color, vFinalColour;\n");
    }
    else
    {
        strcat(sProgram, "ADD R9.xyzw, R9.xyzw, c[93].xyzw;");
        strcat(sProgram, "MOV R9.w, c[94].w;");
        strcat(sProgram, "MUL o[COL0].xyzw, R9.xyzw, c[15].xxxx;");
    }
#endif
    return sProgram;
}
// PC 0x004D0250
void LightManager::SetupLightsForThisPositionAndRadius(
    const Vector& vPosition, float fRadius, List<PartLight*>& lstLights)
{
    m_lstRenderLights.m_nCount = 0;
    for (int i = 0; i < lstLights.m_nCount; i++)
    {
        PartLight* pLight = lstLights[i];
        if (pLight->m_fRadius <= 0.0f ||
            (!pLight->m_bFlare && !pLight->m_nLightType))
            continue;
        if (pLight->m_vColor.m_fX == 0.0f && pLight->m_vColor.m_fY == 0.0f &&
            pLight->m_vColor.m_fZ == 0.0f)
            continue;

#ifdef _XBOX
        float fDistX = pLight->m_vWorldPosition.m_fX - vPosition.m_fX;
        fDistX = (fDistX < 0.0f ? -fDistX : fDistX) - pLight->m_fRadius;
        float fDistY = pLight->m_vWorldPosition.m_fY - vPosition.m_fY;
        fDistY = (fDistY < 0.0f ? -fDistY : fDistY) - pLight->m_fRadius;
        if (fDistX < fRadius * 2.0f && fDistY < fRadius * 2.0f)
            AddRenderLight(pLight);
#else
        float fDistX = pLight->m_vWorldPosition.m_fX - vPosition.m_fX;
        if (fDistX < 0.0f)
            fDistX = -fDistX;
        fDistX -= pLight->m_fRadius;
        float fDistY = pLight->m_vWorldPosition.m_fY - vPosition.m_fY;
        if (fDistY < 0.0f)
            fDistY = -fDistY;
        fDistY -= pLight->m_fRadius;
        if (fDistX < fRadius * 2.0f && fDistY < fRadius * 2.0f)
            AddRenderLight(pLight);
#endif
    }
}
// PC 0x004D0350
void LightManager::SetupLightsForThisPositionAndRadius(const Vector& vPosition,
                                                       float fRadius)
{
    m_lstRenderLights.m_nCount = 0;
    for (int i = 0; i < m_lstLights.m_nCount; i++)
    {
        PartLight* pLight = m_lstLights[i];
        if (pLight->m_fRadius <= 0.0f ||
            (!pLight->m_bFlare && !pLight->m_nLightType))
            continue;

#ifdef _XBOX
        float fDistX = pLight->m_vWorldPosition.m_fX - vPosition.m_fX;
        fDistX = (fDistX < 0.0f ? -fDistX : fDistX) - pLight->m_fRadius;
        float fDistY = pLight->m_vWorldPosition.m_fY - vPosition.m_fY;
        fDistY = (fDistY < 0.0f ? -fDistY : fDistY) - pLight->m_fRadius;
        if (fDistX < fRadius && fDistY < fRadius)
            AddRenderLight(pLight);
#else
        float fDistX = pLight->m_vWorldPosition.m_fX - vPosition.m_fX;
        if (fDistX < 0.0f)
            fDistX = -fDistX;
        fDistX -= pLight->m_fRadius;
        float fDistY = pLight->m_vWorldPosition.m_fY - vPosition.m_fY;
        if (fDistY < 0.0f)
            fDistY = -fDistY;
        fDistY -= pLight->m_fRadius;
        if (fDistX < fRadius && fDistY < fRadius)
            AddRenderLight(pLight);
#endif
    }
}
// PC 0x004D0410
int LightManager::GetNearestLights(Scene* pScene, Vector vPosition,
                                    float fRadius, int bDynamicOnly)
{
    if (enablegetnearestlights == 0)
        return 0;

    int nLights = 0;
    for (int i = 0; i < m_lstLights.m_nCount; i++)
    {
        if (nLights >= MAX_ENABLED_LIGHTS)
            break;
        PartLight* pLight = m_lstLights[i];
        if ((bDynamicOnly == 0 || pLight->m_nLightType != PartLight::LIGHTTYPE_STATIC) &&
            (pLight->m_nLightType != PartLight::LIGHTTYPE_STATIC || pLight->m_bFlare != 0))
        {
            Vector vDistance = pLight->m_vWorldPosition - vPosition;
            if (!(magnitude(vDistance) > fRadius + pLight->m_fRadius) &&
                !(pLight->m_fRadius <= 0.0f))
            {
                GLRender_EnableLight(nLights + 1);
                float fMultiplier = pLight->m_fColorMultiplier;
                Vector vColor;
                vColor.m_fX = fMultiplier * pLight->m_vColor.m_fX;
                vColor.m_fY = fMultiplier * pLight->m_vColor.m_fY;
                vColor.m_fZ = fMultiplier * pLight->m_vColor.m_fZ;
                SetLightGL(nLights++, pLight->m_vWorldPosition, vColor,
                           pLight->m_fRadius, pLight->m_bAmbientOnly);
            }
        }
    }
    for (int j = nLights; j < MAX_ENABLED_LIGHTS; j++)
        GLRender_DisableLight(j + 1);
    return nLights;
}
// PC 0x004D0550
LightManager::LightManager()
    : m_lstLights(0),
      m_lstShadowLights(0),
      m_lstStaticLights(0),
      m_lstRenderLights(0),
      m_lstNearbyLights(0),
      m_lstEnabledLights(0),
      m_lstPrioritizedLights(0),
      m_lstBumpMapLights(0),
      m_lstFadingLights(0),
      m_lstFadingDynamicLights(0),
      m_lstFadedLights(0),
      m_lstFadedDynamicLights(0),
      m_lstShadowCasters(0),
      m_lstShadowCastersStatic(0),
      m_lstShadowCastersBump(0),
      m_lstLensFlareLights(0),
      m_lstExcludedShaderLights(0),
      m_lstPreviousFadeLights(0),
      m_lstPreviousFadeIntensities(0)
{
    m_bDynamicOnly = 0;
    m_pShadowLight = NULL;
    m_nStateA8 = 0;
    m_bBumpMapLightsDisabled = 0;
}
// PC 0x004D0690
void LightManager::AddLight(PartLight* pLight)
{
    if (!m_lstLights.Contains(pLight))
    {
        Quaternion qOrientation;
        pLight->GetWorld(pLight->m_vWorldPosition, qOrientation);
        m_lstLights.Add(pLight);
        if (pLight->m_nLightType == PartLight::LIGHTTYPE_STATIC)
            m_lstStaticLights.Add(pLight);
        if (pLight->m_pAurObject &&
            (pLight->m_nLightType != PartLight::LIGHTTYPE_STATIC || pLight->m_bFlare != 0))
            m_lstShadowLights.Add(pLight);
    }
}
// PC 0x004D0780
void LightManager::RemoveLight(PartLight* pLight, bool bAll)
{
    if (m_lstLights.Contains(pLight))
    {
        m_lstLights.Remove(pLight);
        if (pLight->m_nLightType == PartLight::LIGHTTYPE_STATIC)
            m_lstStaticLights.Remove(pLight);
        if (pLight->m_pAurObject &&
            (pLight->m_nLightType != PartLight::LIGHTTYPE_STATIC || pLight->m_bFlare != 0))
            m_lstShadowLights.Remove(pLight);
    }
    if (m_lstLensFlareLights.Contains(pLight))
        m_lstLensFlareLights.Remove(pLight);
    if (bAll)
    {
        if (m_lstEnabledLights.Contains(pLight))
            m_lstEnabledLights.Remove(pLight);
        if (m_lstFadingLights.Contains(pLight))
            m_lstFadingLights.Remove(pLight);
        if (m_lstFadingDynamicLights.Contains(pLight))
            m_lstFadingDynamicLights.Remove(pLight);
        if (m_lstFadedLights.Contains(pLight))
            m_lstFadedLights.Remove(pLight);
        if (m_lstFadedDynamicLights.Contains(pLight))
            m_lstFadedDynamicLights.Remove(pLight);
        if (m_lstShadowCastersStatic.Contains(pLight))
            m_lstShadowCastersStatic.Remove(pLight);
        if (m_lstShadowCastersBump.Contains(pLight))
            m_lstShadowCastersBump.Remove(pLight);
    }
}
// PC 0x004D0940
void LightManager::AddRenderLight(PartLight* pLight)
{
    if (!m_lstRenderLights.Contains(pLight))
        m_lstRenderLights.Add(pLight);
}
// PC 0x004D0990
void LightManager::PrioritizeIllumination(bool bDynamicOnly)
{
    m_lstEnabledLights.m_nCount = 0;
    Vector vZero;
    for (int i = 0; i < m_lstRenderLights.m_nCount; i++)
    {
        if (bDynamicOnly || !(m_lstRenderLights[i]->m_fPriorityValue > 1.0f))
        {
            if (m_lstRenderLights[i]->m_fRadius != 0.0f)
            {
                Vector& vColor = m_lstRenderLights[i]->m_vColor;
                if (!equal(vColor, vZero))
                    m_lstEnabledLights.Add(m_lstRenderLights[i]);
            }
        }
    }
    qsort(m_lstEnabledLights.m_pList, m_lstEnabledLights.m_nCount,
          sizeof(PartLight*), lightcmp);
    m_lstBumpMapLights.m_nCount = 0;
    for (int j = 0; j < m_lstEnabledLights.m_nCount && j < MAX_ENABLED_LIGHTS; j++)
    {
        PartLight* pLight = m_lstEnabledLights[j];
        if (!pLight->m_bAmbientOnly && pLight->m_nLightType)
        {
            m_lstBumpMapLights.Add(pLight);
            break;
        }
    }
}
// PC 0x004D0AD0
void LightManager::PrioritizeShadow()
{
    m_lstPrioritizedLights.m_nCount = 0;
    for (int i = 0; i < m_lstRenderLights.m_nCount; i++)
    {
        PartLight* pLight = m_lstRenderLights[i];
        if (pLight->m_bShadow && !pLight->m_bAmbientOnly &&
            (pLight->m_vColor.m_fX > AUR_MINIMUM_SHADOW_COLOR ||
             pLight->m_vColor.m_fY > AUR_MINIMUM_SHADOW_COLOR ||
             pLight->m_vColor.m_fZ > AUR_MINIMUM_SHADOW_COLOR))
        {
            m_lstPrioritizedLights.Add(pLight);
        }
    }

    qsort(m_lstPrioritizedLights.m_pList, m_lstPrioritizedLights.m_nCount,
          sizeof(PartLight*), lightcmp);
}
// PC 0x004D0B80
void LightManager::PrioritizeBump()
{
    m_lstBumpMapLights.m_nCount = 0;
    for (int i = 0; i < m_lstRenderLights.m_nCount; i++)
    {
        PartLight* pLight = m_lstRenderLights[i];
        if (!pLight->m_bAmbientOnly && pLight->m_nLightType)
        {
            m_lstBumpMapLights.Add(pLight);
        }
    }

    qsort(m_lstBumpMapLights.m_pList, m_lstBumpMapLights.m_nCount,
          sizeof(PartLight*), lightcmp);
    if (m_lstBumpMapLights.m_nCount > 1)
    {
        m_lstBumpMapLights.m_nCount = 1;
    }
}
// PC 0x004D0C10
void LightManager::EnableHighestPriorityLightsFade(
    int nMaxLights, CAurObjectLightContainer* pContainer)
{
    List<PartLight*>& lstLights = pContainer->m_lstLights;
    List<float>& lstIntensities = pContainer->m_lstFadeIntensities;
#ifdef _XBOX
    List<PartLight*> listHighestPriorityLights;
#else
    static List<PartLight*> listHighestPriorityLights(0);
    listHighestPriorityLights.m_nCount = 0;
#endif

    int i;
    for (i = 0; i < nMaxLights; i++)
    {
        if (i >= m_lstEnabledLights.m_nCount)
            break;
        listHighestPriorityLights.Add(m_lstEnabledLights[i]);
    }

    for (i = lstLights.m_nCount; i != 0; )
    {
        PartLight* pLight = lstLights[--i];
        MdlNodeLight* pNode = pLight->m_pNode->AsMdlNodeLight();
        if (listHighestPriorityLights.Contains(pLight))
        {
            if (pNode->m_bFadingLight)
                lstIntensities[i] = min(1.0f,
                    lstIntensities[i] + DeltaT * lightfadespeed);
            else
                lstIntensities[i] = 1.0f;
        }
        else
        {
            if (pNode->m_bFadingLight)
                lstIntensities[i] = max(0.0f,
                    lstIntensities[i] - DeltaT * lightfadespeed);
            else
                lstIntensities[i] = 0.0f;
            if (lstIntensities[i] <= 0.0f)
            {
                lstLights.DelIndex(i);
                lstIntensities.DelIndex(i);
            }
        }
    }

    if (lstLights.m_nCount < nMaxLights &&
        lstLights.m_nCount < listHighestPriorityLights.m_nCount)
    {
        for (i = 0; i < listHighestPriorityLights.m_nCount; i++)
        {
            if (lstLights.m_nCount >= nMaxLights)
                break;
            PartLight* pLight = listHighestPriorityLights[i];
            if (!lstLights.Contains(pLight))
            {
                lstLights.Add(pLight);
                lstIntensities.Add(0.0f);
            }
        }
    }

    for (i = 0; i < m_lstEnabledLights.m_nCount; i++)
    {
        PartLight* pLight = m_lstEnabledLights[i];
#ifdef _XBOX
        int bShadow = pLight->m_bShadow;
        if (bShadow)
#else
        if (pLight->m_bShadow)
#endif
        {
            float fShadowFade;

            if (pContainer->m_pShadowLight == NULL)
            {
                pContainer->m_pShadowLight = pLight;
                fShadowFade = pContainer->m_fShadowFade + lightshadowfadespeed * DeltaT;
            }
            else if (pContainer->m_pShadowLight == pLight)
            {
                fShadowFade = pContainer->m_fShadowFade + lightshadowfadespeed * DeltaT;
            }
            else
            {
                pContainer->m_fShadowFade = max(0.0f,
                    pContainer->m_fShadowFade - lightshadowfadespeed * DeltaT);
                if (pContainer->m_fShadowFade <= 0.0f)
                {
                    pContainer->m_pShadowLight = pLight;
                    fShadowFade = pContainer->m_fShadowFade + lightshadowfadespeed * DeltaT;
                }
                else
                    goto ShadowFadeDone;
            }
            pContainer->m_fShadowFade = min(1.0f, fShadowFade);
            goto ShadowFadeDone;
        }
    }
    if (pContainer->m_pShadowLight)
    {
        pContainer->m_fShadowFade = max(0.0f,
            pContainer->m_fShadowFade - lightshadowfadespeed * DeltaT);
        if (pContainer->m_fShadowFade <= 0.0f)
            pContainer->m_pShadowLight = NULL;
    }

ShadowFadeDone:
    if (lstLights.m_nCount == m_lstPreviousFadeLights.m_nCount)
    {
        bool bSameLights = true;
        for (i = 0; i < lstLights.m_nCount; i++)
        {
            if (lstLights[i] != m_lstPreviousFadeLights[i] ||
                lstIntensities[i] != m_lstPreviousFadeIntensities[i])
            {
                bSameLights = false;
                break;
            }
        }
        if (bSameLights)
        {
            m_lstEnabledLights = lstLights;
            return;
        }
    }
    m_lstPreviousFadeLights = lstLights;
    m_lstPreviousFadeIntensities = lstIntensities;
    cm_bLightManagerUpdated = 1;
    for (i = 0; i < lstLights.m_nCount; i++)
    {
        PartLight* pLight = lstLights[i];
        pLight->m_fMultiplier = lstIntensities[i];
        GLRender_EnableLight(i + 1);
        SetLightGL(i, pLight->m_vWorldPosition,
                   pLight->m_fMultiplier * pLight->m_vColor,
                   pLight->m_fRadius, pLight->m_bAmbientOnly);
    }
    for (i = lstLights.m_nCount; i < MAX_ENABLED_LIGHTS; i++)
        GLRender_DisableLight(i + 1);
    m_lstEnabledLights = lstLights;
}
// PC 0x004D1120
void LightManager::EnableLightsDynamicOnly()
{
    if (enablegetnearestlights == 0)
    {
        List<PartLight*> lstLights;
        for (int i = 0; i < m_lstEnabledLights.m_nCount; i++)
        {
            PartLight* pLight = m_lstEnabledLights[i];
            if (pLight->m_nLightType == PartLight::LIGHTTYPE_DYNAMIC)
                lstLights.Add(pLight);
        }
        staticlightscount = lstLights.m_nCount;
        ControlLightsFade(m_lstFadingDynamicLights, m_lstFadedDynamicLights,
                          lstLights, 1);
        if (CAurInternal::m_nHeadLightEnabled)
            EnableHeadLight();
        else
            AurDisableHeadLight();
        m_bDynamicOnly = 1;
    }
}
// PC 0x004D1220
void LightManager::ControlLightsFade(List<PartLight*>& lstFadeIn,
                                     List<PartLight*>& lstFadeOut,
                                     List<PartLight*>& lstLights,
                                     int bDynamicOnly)
{
    if (nosortlights)
        return;

    int nMaxLights = m_nMaxLights;
    int i;
    if (m_pOnlyGob)
    {
        int nGobLights = 0;
        for (i = 0; i < lstLights.m_nCount; i++)
        {
            if (lstLights[i]->m_pAurObject != m_pOnlyGob)
                break;
            nGobLights++;
        }
        nMaxLights = min(nMaxLights, nGobLights);
    }

    for (i = lstFadeIn.m_nCount; i--;)
    {
        PartLight* pLight = lstFadeIn[i];
        float& fFade = bDynamicOnly ? pLight->m_fDynamicMultiplier
                                   : pLight->m_fMultiplier;
        if (lstLights.Contains(pLight))
        {
            if (lstLights.IndexOf(pLight) >= nMaxLights)
            {
#ifdef _XBOX
                fFade -= DeltaT;
#else
                fFade -= (float)(DeltaT * 1.0);
#endif
                if (fFade < 0.0f)
                {
                    fFade = 0.0f;
                    lstFadeIn.DelIndex(i);
                }
            }
            else if (fFade < 1.0f)
            {
                fFade = min(1.0f, fFade + DeltaT);
            }
        }
        else if (m_lstLights.Contains(pLight))
        {
#ifdef _XBOX
            fFade -= DeltaT;
#else
            fFade -= (float)(DeltaT * 1.0);
#endif
            if (fFade < 0.0f)
            {
                fFade = 0.0f;
                lstFadeIn.DelIndex(i);
            }
        }
        else
        {
            lstFadeIn.DelIndex(i);
        }
    }

    int nNewLights = 0;
    for (i = 0; i < nMaxLights; i++)
    {
        if (i < lstLights.m_nCount)
        {
            PartLight* pLight = lstLights[i];
            if (!((MdlNodeLight*)pLight->m_pNode)->m_bFadingLight &&
                !lstFadeIn.Contains(pLight))
                nNewLights++;
        }
        else
            break;
    }

    int nFreeLights = nMaxLights - lstFadeIn.m_nCount;
    while (nNewLights > nFreeLights)
    {
        bool bFound = false;
        for (i = lstFadeIn.m_nCount - 1; i >= 0; i--)
        {
            PartLight* pLight = lstFadeIn[i];
            int nPriority = lstLights.Contains(pLight)
                ? lstLights.IndexOf(pLight) : nMaxLights + 1;
            if (nPriority > nMaxLights)
            {
                lstFadeIn.DelIndex(i);
                nFreeLights++;
                bFound = true;
                break;
            }
        }
        if (!bFound)
            break;
    }

    for (i = 0; i < nMaxLights; i++)
    {
        if (i >= lstLights.m_nCount || lstFadeIn.m_nCount >= nMaxLights)
            break;
        PartLight* pLight = lstLights[i];
        float& fFade = bDynamicOnly ? pLight->m_fDynamicMultiplier
                                   : pLight->m_fMultiplier;
        if (!lstFadeIn.Contains(pLight))
        {
            lstFadeIn.Add(pLight);
            if (!((MdlNodeLight*)pLight->m_pNode)->m_bFadingLight)
                fFade = 1.0f;
            else
            {

                if (lstFadeOut.Contains(pLight))
                    fFade = 0.0f;
                else
                    fFade = 0.0f;
            }
        }
        fFade = min(1.0f, fFade + DeltaT);
    }

    qsort(lstFadeIn.m_pList, lstFadeIn.m_nCount, sizeof(PartLight*), lightcmp);
    i = 0;
    countenabledlights = min(nMaxLights, lstFadeIn.m_nCount);
    containsnegative = 0;
    for (; i < lstFadeIn.m_nCount; i++)
    {
        float& fFade = bDynamicOnly ? lstFadeIn[i]->m_fDynamicMultiplier
                                   : lstFadeIn[i]->m_fMultiplier;
        Vector vColor = (fFade * lstFadeIn[i]->m_vColor) *
                        lstFadeIn[i]->m_fColorMultiplier;
        if (vColor.m_fX < 0.0f)
            containsnegative = 1;
        GLRender_EnableLight(i + 1);
        SetLightGL(i, lstFadeIn[i]->m_vWorldPosition, vColor,
                   lstFadeIn[i]->m_fRadius, lstFadeIn[i]->m_bAmbientOnly);
    }
    for (int j = i; j < MAX_ENABLED_LIGHTS; j++)
        GLRender_DisableLight(j + 1);
    lstFadeOut = lstLights;
}
// PC 0x004D17B0
List<PartLight*>* LightManager::GetShadowLights(int nType)
{
    m_lstShadowCasters.m_nCount = 0;
    if (nType == SHADOWLIGHTS_PRIMARY)
    {
        PartLight* pShadowLight = NULL;
        int nPriority = PRIMARY_SHADOW_PRIORITY_LIMIT;
        for (int i = 0; i < m_lstLights.m_nCount; i++)
        {
            PartLight* pLight = m_lstLights[i];
            if (pLight->m_bShadow && !pLight->m_bAmbientOnly &&
                (!(pLight->m_fRadius < AUR_PRIMARY_SHADOW_RADIUS) ||
                 !pLight->m_nLightType) &&
                ((MdlNodeLight*)pLight->m_pNode)->m_nPriority < nPriority)
            {
                nPriority = ((MdlNodeLight*)pLight->m_pNode)->m_nPriority;
                pShadowLight = pLight;
            }
        }
        if (pShadowLight)
        {
            m_lstShadowCasters.Add(pShadowLight);
            m_pShadowLight = pShadowLight;
        }
    }
    else
    {
        int nLight;
        for (nLight = 0; m_lstShadowCasters.m_nCount < m_nMaxShadowLights; nLight++)
        {
            if (nLight >= m_lstPrioritizedLights.m_nCount)
                break;
            PartLight* pLight = m_lstPrioritizedLights[nLight];
            if ((nType != SHADOWLIGHTS_STATIC || pLight->m_nLightType) &&
                !(pLight->m_fPriorityValue > 1.0f))
            {
                m_lstShadowCasters.Add(pLight);
                if (nType == SHADOWLIGHTS_STATIC)
                    break;
            }
        }

        List<PartLight*> lstPrevious;
        if (nType == SHADOWLIGHTS_STATIC)
            lstPrevious = m_lstShadowCastersBump;
        else
            lstPrevious = m_lstShadowCastersStatic;

        for (nLight = 0; nLight < m_lstShadowCasters.m_nCount; nLight++)
        {
            PartLight* pLight = m_lstShadowCasters[nLight];
            if (!lstPrevious.Contains(pLight))
                pLight->m_fShadowFade = 0.0f;
        }

        for (nLight = 0; nLight < m_nMaxShadowLights; nLight++)
        {
            if (nLight >= lstPrevious.m_nCount)
                break;
            PartLight* pLight = lstPrevious[nLight];
            if (!m_lstShadowCasters.Contains(pLight))
            {
                pLight->m_fShadowFade = max(0.0f,
                    pLight->m_fShadowFade - 2.0f * DeltaT / shadowfadespeed);
                if (pLight->m_fShadowFade > 0.0f)
                    m_lstShadowCasters.Insert(pLight, 0);
            }
        }
        if (m_lstShadowCasters.m_nCount > m_nMaxShadowLights)
            m_lstShadowCasters.m_nCount = m_nMaxShadowLights;
        for (nLight = 0; nLight < m_lstShadowCasters.m_nCount; nLight++)
        {
            PartLight* pLight = m_lstShadowCasters[nLight];
            pLight->m_fShadowFade = min(1.0f,
                pLight->m_fShadowFade + DeltaT / shadowfadespeed);
        }

        if (nType == SHADOWLIGHTS_STATIC)
            m_lstShadowCastersBump = m_lstShadowCasters;
        else
            m_lstShadowCastersStatic = m_lstShadowCasters;
    }
    return &m_lstShadowCasters;
}
// PC 0x004D1BB0
int LightManager::DisableThisLight(PartLight* pLight)
{
    List<PartLight*> lstLights;
    if (m_bDynamicOnly)
        lstLights = m_lstFadingDynamicLights;
    else
        lstLights = m_lstEnabledLights;
    if (!lstLights.Contains(pLight))
        return 0;
#ifdef _XBOX
    int nLight = lstLights.IndexOf(pLight);
    GLRender_DisableLight(nLight + 1);
#else
    GLRender_DisableLight(lstLights.IndexOf(pLight) + 1);
#endif
    return 1;
}
// PC 0x004D1C90
int LightManager::EnableThisLight(PartLight* pLight)
{
    List<PartLight*> lstLights;
    if (m_bDynamicOnly)
        lstLights = m_lstFadingDynamicLights;
    else
        lstLights = m_lstEnabledLights;
    if (!lstLights.Contains(pLight))
        return 0;
    GLRender_EnableLight(lstLights.IndexOf(pLight) + 1);
    return 1;
}
// PC 0x004CF870
void LightManager::SetUpVertexProgramLightShaderConstants(int nLights,
                                                         int bWorldSpace)
{
    if (CurrentCamera != NULL)
    {
        Matrix mCamera = CurrentCamera->m_qOrientation.getmatrix();
        Vector vCamera = CurrentCamera->m_vPosition;
        AurSetVertexProgramParameter4f(LIGHT_PROGRAM_CAMERA_X,
            mCamera.m_vFirst.m_fX, mCamera.m_vSecond.m_fX,
            mCamera.m_vThird.m_fX, vCamera.m_fX);
        AurSetVertexProgramParameter4f(LIGHT_PROGRAM_CAMERA_Y,
            mCamera.m_vFirst.m_fY, mCamera.m_vSecond.m_fY,
            mCamera.m_vThird.m_fY, vCamera.m_fY);
        AurSetVertexProgramParameter4f(LIGHT_PROGRAM_CAMERA_Z,
            mCamera.m_vFirst.m_fZ, mCamera.m_vSecond.m_fZ,
            mCamera.m_vThird.m_fZ, vCamera.m_fZ);
        GLRender_SetVertexProgramParameter4f(LIGHT_PROGRAM_ZERO_ONE, 0.0f, 0.0f, 0.0f, 1.0f);
        GLRender_SetVertexProgramParameter4f(LIGHT_PROGRAM_COLOR_SCALE, 1.0f, 1.0f, 1.0f, 0.0f);
        AurSetVertexProgramParameter4f(LIGHT_PROGRAM_CAMERA_POSITION,
            CurrentCamera->m_vPosition.m_fX,
            CurrentCamera->m_vPosition.m_fY,
            CurrentCamera->m_vPosition.m_fZ, 1.0f);

        int nParameter = LIGHT_PROGRAM_FIRST_LIGHT;
        if (CAurInternal::m_nHeadLightEnabled)
        {
            nLights--;
            AurSetVertexProgramParameter4f(nParameter--,
                CurrentCamera->m_vPosition.m_fX,
                CurrentCamera->m_vPosition.m_fY,
                CurrentCamera->m_vPosition.m_fZ, 0.0f);
            AurSetVertexProgramParameter4f(nParameter--,
                headlightambient.m_fX, headlightambient.m_fY,
                headlightambient.m_fZ, 1.0f);
            AurSetVertexProgramParameter4f(nParameter--,
                headlightdiffuse.m_fX, headlightdiffuse.m_fY,
                headlightdiffuse.m_fZ, 1.0f);
            AurSetVertexProgramParameter4f(nParameter--,
                0.0f, 0.0f, 0.0f, 1.0f);
            AurSetVertexProgramParameter4f(nParameter--,
                1.0f, 1.0f, 1.0f, 1.0f);
        }

        List<PartLight*> lstLights;
        if (bWorldSpace)
            lstLights = m_lstFadingDynamicLights;
        else
            lstLights = m_lstEnabledLights;
        int nUsed = 0;
        for (int i = 0; i < lstLights.m_nCount; i++)
        {
            if (nUsed >= (m_lstExcludedShaderLights.m_nCount > 0
                          ? nLights - 1 : nLights))
                break;
            if (m_bBumpMapLightsDisabled && m_lstBumpMapLights.Contains(lstLights[i]))
                continue;
            if (m_lstExcludedShaderLights.Contains(lstLights[i]))
                continue;
            nUsed++;
            Vector vAmbient;
            Vector vDiffuse;
            float fMultiplier = bWorldSpace ? 1.0f : lstLights[i]->m_fMultiplier;
#ifdef _XBOX
            if (lstLights[i]->m_bAmbientOnly)
                vAmbient = lstLights[i]->m_vColor * fMultiplier;
            else
                vDiffuse = lstLights[i]->m_vColor * fMultiplier;
#else
            if (lstLights[i]->m_bAmbientOnly)
                vAmbient = fMultiplier * lstLights[i]->m_vColor;
            else
                vDiffuse = fMultiplier * lstLights[i]->m_vColor;
#endif

            AurSetVertexProgramParameter4f(nParameter--,
                lstLights[i]->m_vWorldPosition.m_fX,
                lstLights[i]->m_vWorldPosition.m_fY,
                lstLights[i]->m_vWorldPosition.m_fZ, 0.0f);
            AurSetVertexProgramParameter4f(nParameter--,
                vAmbient.m_fX, vAmbient.m_fY, vAmbient.m_fZ, 1.0f);
            AurSetVertexProgramParameter4f(nParameter--,
                vDiffuse.m_fX, vDiffuse.m_fY, vDiffuse.m_fZ, 1.0f);
            GLRender_SetVertexProgramParameter4f(nParameter, 0.0f, 0.0f, 0.0f, 1.0f);
            nParameter--;
#ifdef _XBOX
            float fRadius = lstLights[i]->m_fRadius;
            GLRender_SetVertexProgramParameter4f(nParameter, 1.0f, 0.0f,
                AUR_LIGHT_ATTENUATION_SCALE / (fRadius * fRadius + fRadius * fRadius), 1.0f);
#else
            float fRadius = lstLights[i]->m_fRadius;
            float fAttenuation = AUR_LIGHT_ATTENUATION_SCALE /
                (fRadius * fRadius + fRadius * fRadius);
            GLRender_SetVertexProgramParameter4f(nParameter, 1.0f, 0.0f, fAttenuation, 1.0f);
#endif

            nParameter--;
        }

        for (int j = nUsed; j < nLights; j++)
        {
            GLRender_SetVertexProgramParameter4f(nParameter, 0.0f, 0.0f, 0.0f, 0.0f);
            nParameter--;
            GLRender_SetVertexProgramParameter4f(nParameter, 0.0f, 0.0f, 0.0f, 1.0f);
            nParameter--;
            GLRender_SetVertexProgramParameter4f(nParameter, 0.0f, 0.0f, 0.0f, 1.0f);
            nParameter--;
            GLRender_SetVertexProgramParameter4f(nParameter, 0.0f, 0.0f, 0.0f, 1.0f);
            nParameter--;
            GLRender_SetVertexProgramParameter4f(nParameter, 1.0f, 1.0f, 1.0f, 1.0f);
            nParameter--;
        }
    }
}
// PC 0x004D1D70
void LightManager::DisableBumpMapLights()
{
#ifdef _XBOX
    List<PartLight*> lstLights(*GetBumpMapLights());
#else
    List<PartLight*> lstLights(m_lstBumpMapLights);
#endif

    for (int i = 0; i < lstLights.m_nCount; i++)
    {
        DisableThisLight(lstLights[i]);
        m_bBumpMapLightsDisabled = 1;
        cm_bLightManagerUpdated = 1;
    }
}
// PC 0x004D1E00
void LightManager::EnableBumpMapLights()
{
#ifdef _XBOX
    List<PartLight*> lstLights(*GetBumpMapLights());
#else
    List<PartLight*> lstLights(m_lstBumpMapLights);
#endif

    for (int i = 0; i < lstLights.m_nCount; i++)
    {
        EnableThisLight(lstLights[i]);
        cm_bLightManagerUpdated = 1;
    }
    m_bBumpMapLightsDisabled = 0;
}

void LightManager::EnablePhongLights(int nMaxLights, int bSkipBumpMapLights)
{
    int i = m_lstExcludedShaderLights.m_nCount;
    while (i != 0)
    {
        DisableThisLight(m_lstExcludedShaderLights[--i]);
    }
    m_lstExcludedShaderLights.m_nCount = 0;

    for (i = 0; i < m_lstFadingDynamicLights.m_nCount; i++)
    {
        PartLight* pLight = m_lstFadingDynamicLights[i];
        if (pLight->m_fRadius > 5000.0f)
        {
            continue;
        }
        if (bSkipBumpMapLights && m_lstBumpMapLights.Contains(pLight))
        {
            continue;
        }
        if (!pLight->m_bAmbientOnly)
        {
            m_lstExcludedShaderLights.Add(pLight);
            EnableThisLight(pLight);
        }
        if (m_lstExcludedShaderLights.m_nCount >= nMaxLights)
        {
            break;
        }
    }
}

Vector headlightdiffuse(0.8f, 0.8f, 0.8f);
Vector headlightambient(0.2f, 0.2f, 0.2f);

static inline void AurlightmanagerTrace1()
{
}

#ifdef _XBOX
void AurPostString(char* szString, int nColumn, int nRow, float fDuration);

static inline float AurLightAbs(float fValue)
{
    if (fValue < 0.0f)
        return 0.0f - fValue;
    return fValue;
}

void LightManager::DebugPrintLightInfo(int nMaxLights, Vector vPosition)
{
    if (m_lstEnabledLights.m_nCount == 0)
    {
        AurPostString("<No active dynamic lights>", 2, 8, 0.25f);
        return;
    }
    AurPostString("        Name   Prior  Rad     Dist(x,y)     Height", 2, 8, 0.25f);
    bool bShadowCasterFound = false;
    for (int i = 0; i < m_lstEnabledLights.m_nCount; i++)
    {
        char sColor[4];
        char sLine[64];
        if (i == 0)
            sprintf(sColor, "R");
        else if (i == 1)
            sprintf(sColor, "G");
        else if (i == 2)
            sprintf(sColor, "B");
        else if (i == 3)
            sprintf(sColor, "Y");
        float fDistanceX = AurLightAbs(m_lstEnabledLights[i]->m_vWorldPosition.m_fX - vPosition.m_fX);
        float fDistanceY = AurLightAbs(m_lstEnabledLights[i]->m_vWorldPosition.m_fY - vPosition.m_fY);
        MdlNodeLight* pNode = (MdlNodeLight*)m_lstEnabledLights[i]->m_pNode;
        float fHeight = m_lstEnabledLights[i]->m_vWorldPosition.m_fZ - (vPosition.m_fZ + 1.75f);
        int nIndex = pNode->m_nIndex;
        Model* pModel = pNode->m_pModel->AsModel() ? pNode->m_pModel->AsModel()
                                                   : (Model*)pNode->m_pModel->AsAnimation()->m_pBinaryData;
        sprintf(sLine, "%s:%s  %d  %6.2f (%6.2f, %6.2f) %6.2f", sColor, pModel->m_lNames[nIndex],
                pNode->m_nPriority, m_lstEnabledLights[i]->m_fRadius, fDistanceX, fDistanceY, fHeight);
        if (!bShadowCasterFound && fHeight >= 0.0f && pNode->m_bShadow)
        {
            strcat(sLine, " Casts shdw");
            bShadowCasterFound = true;
        }
        AurPostString(sLine, 2, i + 9, 0.25f);
    }
}
#endif

#ifdef _XBOX

void AurSetHeadLightProperties(Vector vDiffuse, Vector vAmbient)
{
    headlightdiffuse = vDiffuse;
    headlightambient = vAmbient;
}

char* enableheadlight(char* sArguments)
{
    AurEnableHeadLight();
    return "Success";
}

char* disableheadlight(char* sArguments)
{
    AurDisableHeadLight();
    return "Success";
}

char* setheadlightproperties(char* sArguments)
{
    Vector vDiffuse(0.0f, 0.0f, 0.0f);
    Vector vAmbient(0.0f, 0.0f, 0.0f);

    if (sscanf(sArguments, "%f %f %f %f %f %f", &vDiffuse.m_fX, &vDiffuse.m_fY, &vDiffuse.m_fZ, &vAmbient.m_fX,
               &vAmbient.m_fY, &vAmbient.m_fZ) != 6)
    {
        return "Usage: setheadlightproperties <diffuse rgb> <ambient rgb>";
    }
    AurSetHeadLightProperties(vDiffuse, vAmbient);
    return "Success";
}

void LightManager::SetMaxLights(int nMaxLights)
{
    m_nMaxLights = nMaxLights;
}

void LightManager::SetMaxShadowLights(int nMaxShadowLights)
{
    m_nMaxShadowLights = nMaxShadowLights;
}

char* numbershadowlights(char* sArguments)
{
    int nLights;

    if (sscanf(sArguments, "%d", &nLights) != 1)
    {
        return "usage: numbershadowlights x";
    }
    LightManager::SetMaxShadowLights(nLights);
    return "Success.";
}

char* setmaxlights(char* sArguments)
{
    int nLights;

    if (sscanf(sArguments, "%d", &nLights) != 1)
    {
        return "Usage: setmaxlights <maxlights>";
    }
    LightManager::SetMaxLights(nLights);
    return "Success";
}
#endif
