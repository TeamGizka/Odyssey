#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "exobase.h"
#include "nwcambientsound.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serverexoapp.h"
#include "math3d.h"
#include "clientaimaster.h"
#include "guiingame.h"
#include "nwcmappin.h"
#include "nwcmodule.h"
#include "nwcplaceable.h"
#include "nwctrigger.h"
#include "nwsplaceable.h"
#include "nwcsoundobject.h"
#include "nwcanimbase.h"
#include "nwsarea.h"
#include "nwsmodule.h"
#include "nwcvisualeffect.h"
#include "nwccreature.h"
#include "nwclass.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "worldtimer.h"
#include "aurscene.h"
#include "aurcamera.h"
#include "swcroom.h"
#include "nwcarea.h"
#include "gameobjectarray.h"
#include "swmain.h"
#include "swminigame.h"
#include "nwmessage.h"
#include "aurgrass.h"
#include "aurtexman.h"
#include "nwrules.h"

const int CSWCArea::MAX_INTERTILE_PATH_DEPTH = 5;


static inline void NwcareaTrace1();

float g_fOldShadowAlpha = 0.0f;

int RyanStartTextureSize = 0;
int RyanEndTextureSize = 0;
int RyanLevelTextureSize = 0;
int RyanTotalLevelVertices = 0;
int RyanLevelVertices = 0;
int RyanTotalLevelVertSize = 0;
int RyanLevelVertSize = 0;
int RyanStartParts = 0;
int RyanEndParts = 0;
int RyanLevelTotalParts = 0;



int RyanUnused1 = 0;
int RyanUnused2 = 0;
extern int countpart;


float darkvaluemultiplier = 0.35f;
float lightvaluemultiplier = 0.5f;
// PC 0x00604B50  Xbox 0x00034BF0
void PlayRoomSoundEvent(CAurObject* pObject, char* sEvent, void* pData)
{
    if (pData != NULL)
    {
        static_cast<CSWBehaviorAttachSound*>(pData)->PlaySoundA();
    }
}

static CSWCRoom* AllocateRoomArray(int nCount)
{
    return new CSWCRoom[nCount];
}
// PC 0x00604BC0  Xbox 0x00034C60
int CSWCArea::GetIsObjectCorpse(unsigned long oidObject)
{
    for (int i = 0; i < 4; i++)
    {
        if (m_anCorpseObjectIds[i] == oidObject)
        {
            return TRUE;
        }
    }
    return FALSE;
}
// PC 0x00604BF0  Xbox 0x00034C90
int CSWCArea::GetSurfaceMaterial(Vector vPosition)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    if (pServerExoApp != NULL)
    {
        CSWSModule* pModule = pServerExoApp->GetModule();
        if (pModule != NULL)
        {
            CSWSArea* pArea = pModule->GetArea();
            if (pArea != NULL)
            {
                return pArea->GetSurfaceMaterial(vPosition);
            }
        }
    }
    return 0;
}
// PC 0x00604C40  Xbox 0x00034CE0
int CSWCArea::TestDirectLine(unsigned long oidObject, const Vector& vStart,
                             const Vector& vEnd, const float& fDistance,
                             float fWidth, int nFlags,
                             CWalkHitInfo* pWalkHitInfo)
{
    if (g_pAppManager->m_pServerExoApp != NULL)
    {
        return g_pAppManager->m_pServerExoApp->TestDirectLine(
            oidObject, m_cAreaResRef, vStart, vEnd, fDistance, fWidth, nFlags,
            pWalkHitInfo);
    }
    return FALSE;
}
// PC 0x00604C90  Xbox 0x00034D20
int CSWCArea::ApplyFireAndForgetVisualEffect(unsigned short nEffect,
                                             Vector vPosition)
{
    CSWCVisualEffect* pEffect = new CSWCVisualEffect(TRUE);
    if (pEffect == NULL)
    {
        return FALSE;
    }
    if (pEffect->LoadEffect(nEffect) == 0)
    {
        delete pEffect;
        return FALSE;
    }
    pEffect->AttachToScene(m_pScene);
    pEffect->m_bDeleteWhenFinished = TRUE;
    pEffect->m_bOneShot = TRUE;
    pEffect->SetPosition(vPosition);
    pEffect->SetAnimation(1);
    pEffect->DoVFXRumble();
    return TRUE;
}
// PC 0x00604D70  Xbox 0x00034E00
void CSWCArea::StartSounds()
{
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    g_pAppManager->m_pClientExoApp->StopMenuMusic();
    g_pAppManager->m_pClientExoApp->StopLoadMusic();
    m_pAmbientSound->StartSounds();
}
// PC 0x00604DB0  Xbox 0x00034E40
int CSWCArea::SetWeather(unsigned char nWeather, float fStrength)
{

    char* const sRainModel = "fx_rain";
    char* const sSnowModel = "fx_snow";
    char* const sLightningModel = "fx_lightning01";
    char* const sWeatherObject = "weather";
    char* const sLightningObject = "weatherlight";
    CExoString sAnimation;
    float fTime;
    float fLength;

    if (fStrength != 0.0f)
    {
        if (m_nWeatherType != nWeather)
        {
            for (unsigned char i = 0; i < 6; i++)
            {
                if (m_apWeatherEffects[i] != NULL)
                {
                    delete m_apWeatherEffects[i];
                    m_apWeatherEffects[i] = NULL;
                }
            }
            switch (nWeather)
            {
            case WEATHER_CLEAR:
                break;
            case WEATHER_RAIN:
                for (unsigned char i = 0; i < 6; i++)
                {
                    m_apWeatherEffects[i] = static_cast<Gob*>(
                        NewCAurObject(sRainModel, sWeatherObject, NULL, NULL));
                    if (m_apWeatherEffects[i] == NULL)
                    {
                        return FALSE;
                    }
                }
                break;
            case WEATHER_SNOW:
                for (unsigned char i = 0; i < 6; i++)
                {
                    m_apWeatherEffects[i] = static_cast<Gob*>(
                        NewCAurObject(sSnowModel, sWeatherObject, NULL, NULL));
                    if (m_apWeatherEffects[i] == NULL)
                    {
                        return FALSE;
                    }
                }
                break;
            default:
                return FALSE;
            }
            for (unsigned char i = 0; i < 6; i++)
            {
                if (m_apWeatherEffects[i] != NULL)
                {
                    m_apWeatherEffects[i]->AttachToScene((Scene*)m_pScene);
                }
            }
            m_nWeatherType = nWeather;
        }
        if (m_nChanceLightning > 0)
        {
            if (m_pLightningEffect != NULL)
            {
                delete m_pLightningEffect;
                m_pLightningEffect = NULL;
            }
            switch (m_nWeatherType)
            {
            case WEATHER_RAIN:
                m_pLightningEffect = static_cast<Gob*>(NewCAurObject(
                    sLightningModel, sLightningObject, NULL, NULL));
                break;
            case WEATHER_SNOW:
                m_pLightningEffect = static_cast<Gob*>(NewCAurObject(
                    sLightningModel, sLightningObject, NULL, NULL));
                break;
            default:
                return FALSE;
            }
            if (m_pLightningEffect != NULL)
            {
                m_pLightningEffect->AttachToScene((Scene*)m_pScene);
            }
        }
        else if (m_pLightningEffect != NULL)
        {
            delete m_pLightningEffect;
            m_pLightningEffect = NULL;
        }
    }
    else
    {
        for (unsigned char i = 0; i < 6; i++)
        {
            if (m_apWeatherEffects[i] != NULL)
            {
                delete m_apWeatherEffects[i];
                m_apWeatherEffects[i] = NULL;
            }
        }
        if (m_pLightningEffect != NULL)
        {
            delete m_pLightningEffect;
            m_pLightningEffect = NULL;
        }
        m_nWeatherType = WEATHER_CLEAR;
    }
    switch (nWeather)
    {
    case WEATHER_RAIN:
        sAnimation = "rain";
        break;
    case WEATHER_SNOW:
        sAnimation = "snow";
        break;
    }
    for (unsigned char i = 0; i < 6; i++)
    {
        if (m_apWeatherEffects[i] != NULL)
        {
            m_apWeatherEffects[i]->PlayAnimation(sAnimation.CStr(), 0.0f,
                                                 Gob::ANIMATION_FLAG_STOP,
                                                 0.0f);
            m_apWeatherEffects[i]->PlayAnimation(sAnimation.CStr(),
                                                 fStrength * 0.01f, 0, 0.0f);
            m_apWeatherEffects[i]->GetAnimation((char*)sAnimation.CStr(),
                                                &fTime, &fLength);
            m_apWeatherEffects[i]->AnimateExternal(fLength);
            m_apWeatherEffects[i]->PlayAnimation(sAnimation.CStr(), 0.0f, 0,
                                                 0.0f);
        }
    }
    return TRUE;
}
// PC 0x00605090  Xbox 0x00035140
int CSWCArea::SetFogColor(const Vector& vColor)
{
    if (m_pScene == NULL)
    {
        return FALSE;
    }
    m_pScene->SetFogColor(vColor);
    CExoString sColor;
    sColor.Format("%f %f %f", vColor.m_fX, vColor.m_fY, vColor.m_fZ);
    SetClearColor((char*)sColor.CStr());
    m_vFogColor = vColor;
    return TRUE;
}
// PC 0x00605160  Xbox 0x00035210
int CSWCArea::SetGlobalLightDiffuse(int nLight, Vector vColor)
{
    char szCommand[128];

    if (m_pShadowLight == NULL)
    {
        return FALSE;
    }
    if (nLight == 0)
    {
        m_pShadowLight->AttachToScene(NULL);
    }
    else
    {
        sprintf(szCommand, "sun gidy_sun_diff color %f %f %f", vColor.m_fX,
                vColor.m_fY, vColor.m_fZ);
        AurControlPart(szCommand);
    }
    return TRUE;
}
// PC 0x006051F0  Xbox 0x000352C0
int CSWCArea::SetGlobalLightAmbient(int nLight, Vector vColor)
{
    char szCommand[128];

    if (m_pScene == NULL)
    {
        return FALSE;
    }
    sprintf(szCommand, "sun gidy_sun_amb color %f %f %f", vColor.m_fX,
            vColor.m_fY, vColor.m_fZ);
    AurControlPart(szCommand);
    return TRUE;
}
// PC 0x00605260  Xbox 0x00035350
int CSWCArea::SetWind(unsigned char nWind)
{
    float fMaximumDeviation = 0.0f;
    float fVariation = 0.0f;
    Vector vDirection(1.0f, 1.0f, 0.0f);
    vDirection = normalize(vDirection);
    BOOL bWind = FALSE;
    float fStrength;

    switch (nWind)
    {
    case WIND_NONE:
        break;
    case WIND_LIGHT:
        bWind = TRUE;
        fStrength = 1.0f;
        fMaximumDeviation = 100.0f;
        fVariation = 3.0f;
        break;
    case WIND_HEAVY:
        bWind = TRUE;
        fStrength = 2.0f;
        fMaximumDeviation = 150.0f;
        fVariation = 5.0f;
        break;
    default:
        return FALSE;
    }
    AurSetGlobalWind(vDirection * (bWind ? fStrength : 0.0f));
    AurSetGlobalWindVariation(fMaximumDeviation, fVariation);
    m_nWind = nWind;
    return TRUE;
}

// PC 0x00605370  Xbox 0x00035480
int CSWCArea::ClearLineOfSightOneWay(Vector vStart, Vector vEnd, Vector* pHit)
{
    return 1;
}






void CSWCArea::ChangeTriggerDisplay(int bDisplay)
{
    for (int i = m_lstRenderTriggerIds.num - 1; i >= 0; i--)
    {
        CSWCTrigger* pTrigger =
            g_pAppManager->m_pClientExoApp->GetTriggerByGameObjectID(
                m_lstRenderTriggerIds.element[i]);
        if (pTrigger != NULL && !pTrigger->m_bAreaTransition &&
            !pTrigger->m_bTrap)
        {
            pTrigger->DisplayTrigger(bDisplay);
        }
    }
}

void CSWCArea::DisplayAllTriggers(int bDisplay)
{
    for (int i = m_lstRenderTriggerIds.num - 1; i >= 0; i--)
    {
        CSWCTrigger* pTrigger =
            g_pAppManager->m_pClientExoApp->GetTriggerByGameObjectID(
                m_lstRenderTriggerIds.element[i]);
        pTrigger->SpecialDisplayTrigger(bDisplay);
    }
}
// PC 0x00605380  Xbox 0x00035490
void CSWCArea::StartWeather(unsigned char nWeather)
{
    m_nNewWeatherType = nWeather;
    m_nWeatherDuration = WEATHER_FADE_DURATION;
}
// PC 0x006053A0  Xbox 0x000354B0
void CSWCArea::StopWeather(unsigned char nWeather)
{
    m_nNewWeatherType = WEATHER_CLEAR;
    m_nWeatherDuration = WEATHER_FADE_DURATION;
}
// PC 0x006053C0  Xbox 0x000354D0
void CSWCArea::SetLightningDone()
{
    shadowalpha = g_fOldShadowAlpha;
    int nRandom = rand();
    m_fLightningTimer = (nRandom % LIGHTNING_DELAY_RANGE) * 0.001f + 5.0f;
    if (m_pLightningEffect != NULL)
    {
        m_pLightningEffect->AttachToScene(NULL);
    }
}
// PC 0x00605410  Xbox 0x00035570
void CSWCArea::UpdateShadowingLights()
{
    if (m_pShadowLight != NULL)
    {
        CSWCCreature* pCreature =
            g_pAppManager->m_pClientExoApp->GetPlayerCreature();
        if (pCreature != NULL)
        {
            pCreature->GetCreatureLight(TRUE);
        }
    }
}
// PC 0x00605440  Xbox 0x000355A0
int CSWCArea::GetIsDark()
{
    if (m_pScene == NULL)
    {
        return FALSE;
    }
    CSWCCreature* pCreature =
        g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    if (pCreature != NULL)
    {
        pCreature->GetCreatureLight(FALSE);
    }
    Vector vLight(1.0f, 1.0f, 1.0f);
    float fLightSquared = vLight.m_fX * vLight.m_fX +
                          vLight.m_fY * vLight.m_fY +
                          vLight.m_fZ * vLight.m_fZ;
    float fDark = darkvaluemultiplier * 1.732;
    int bIsDark;
    if (fLightSquared <= fDark * fDark)
    {
        bIsDark = TRUE;
    }
    else
    {
        float fBright = lightvaluemultiplier * 1.732;
        if (fLightSquared >= fBright * fBright)
        {
            bIsDark = FALSE;
        }
        else
        {
            bIsDark = m_bIsDark;
        }
    }
    m_bIsDark = bIsDark;
    return bIsDark;
}
// PC 0x006054E0  Xbox 0x000356A0
int CSWCArea::GetObjectsAtPositionVisible(const Vector& vPosition)
{
    return 1;
}
// PC 0x006054F0  Xbox 0x000356B0
CSWCRoom* CSWCArea::GetRoom(int nRoom)
{
    CSWCRoom* pRoom = NULL;
    if (nRoom != -1)
    {
        pRoom = &m_pRooms[nRoom];
    }
    return pRoom;
}
// PC 0x00605510  Xbox 0x000356D0
CSWCRoom* CSWCArea::GetRoom(const CExoString& sRoomName)
{
    CSWCRoom* pRoom = NULL;
    for (int i = 0; i < m_nRoomCount; i++)
    {
        CSWCRoom* pCurrentRoom = &m_pRooms[i];
        CResRef cResRef = pCurrentRoom->m_cResRef;
        CExoString sResRef;
        cResRef.CopyToString(sResRef);
        if (sRoomName.CompareNoCase(sResRef.CStr()))
        {
            pRoom = &m_pRooms[i];
            break;
        }
    }
    return pRoom;
}
// PC 0x00605870  Xbox 0x00035AC0
int CSWCArea::SetFog(float fNear, float fFar, const Vector& vColor)
{
    NwcareaTrace1();
    if (m_pScene == NULL)
    {
        return FALSE;
    }
    m_pScene->SetFogRange(fNear, fFar);
    m_fFogNear = fNear;
    m_fFogFar = fFar;
    SetFogColor(vColor);
    return TRUE;
}
// PC 0x006058C0  Xbox 0x00035B10
void CSWCArea::SetAsDay()
{
    NwcareaTrace1();
    if (m_bDayNightCycle == 0 && m_bIsNight != 0)
    {
        SetAsNight();
        return;
    }
    projectionalpha = 0.05f;
    shadowalpha = m_nShadowOpacity * 0.01f;
    m_bLightingChanged = TRUE;
    m_bDawning = FALSE;
    m_bDusking = FALSE;
    m_bIsNight = FALSE;
    SetFog(m_fSunFogNear, m_fSunFogFar, m_vSunColor);
    SetGlobalLightAmbient(1, m_vDayAmbientColor);
    SetGlobalLightDiffuse(1, m_vDayDiffuseColor);
    UpdateShadowingLights();
}
// PC 0x006059C0  Xbox 0x00035C10
void CSWCArea::SetAsNight()
{
    if (m_bDayNightCycle == 0 && m_bIsNight == 0)
    {
        SetAsDay();
        return;
    }
    projectionalpha = 0.0f;
    shadowalpha = m_nShadowOpacity * 0.01f;
    m_bLightingChanged = TRUE;
    m_bDawning = FALSE;
    m_bDusking = FALSE;
    m_bIsNight = TRUE;
    SetFog(m_fMoonFogNear, m_fMoonFogFar, m_vMoonColor);
    SetGlobalLightAmbient(1, m_vNightAmbientColor);
    SetGlobalLightDiffuse(1, m_vNightDiffuseColor);
    UpdateShadowingLights();
}
// PC 0x00605AC0  Xbox 0x00035D10
CSWCMapPin* CSWCArea::GetMapPin(unsigned long nReference, int nType)
{
    NwcareaTrace1();
    int nCount = m_lstMapPins.num;
    for (int i = 0; i < nCount; i++)
    {
        CSWCMapPin* pMapPin = m_lstMapPins[i];
        if (pMapPin != NULL && pMapPin->m_nReference == nReference &&
            pMapPin->m_nType == nType)
        {
            return pMapPin;
        }
    }
    return NULL;
}
// PC 0x00605B10  Xbox 0x00035D60
void CSWCArea::UpdateSoundObjectsTimeOfDay()
{
    NwcareaTrace1();
    int nCount = m_lstSoundObjectIds.num;
    for (int i = 0; i < nCount; i++)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCSoundObject* pSoundObject =
            pClientExoApp->GetSoundObjectByGameObjectID(
                m_lstSoundObjectIds[i]);
        if (pSoundObject != NULL)
        {
            pSoundObject->UpdateTimeOfDay();
        }
    }
}
// PC 0x00605B50  Xbox 0x00035DA0
void CSWCArea::RemovePlaceableObjectLight(unsigned long oidPlaceable, int bFlag)
{
    NwcareaTrace1();
    int nCount = m_lstPlaceableLights.num;
    for (int i = 0; i < nCount; i++)
    {
        CSWCPlaceableObjectLight* pLight = m_lstPlaceableLights[i];
        if (pLight != NULL && pLight->m_oidPlaceable == oidPlaceable)
        {
            if (bFlag != 0)
            {
                Gob* pGob = m_lstPlaceableLights[i]->m_pLight;
                if (pGob != NULL)
                {
                    delete pGob;
                }
            }
            delete m_lstPlaceableLights[i];
            int nRemaining = m_lstPlaceableLights.num - 1;
            int j = i;
            m_lstPlaceableLights.num = nRemaining;
            for (; j < m_lstPlaceableLights.num; j++)
            {
                m_lstPlaceableLights[j] =
                    m_lstPlaceableLights[j + 1];
            }
            return;
        }
    }
}
// PC 0x00605BF0  Xbox 0x00035E40
Gob* CSWCArea::GetPlaceableObjectLight(unsigned long oidPlaceable)
{
    NwcareaTrace1();
    int nCount = m_lstPlaceableLights.num;
    for (int i = 0; i < nCount; i++)
    {
        if (m_lstPlaceableLights[i] != NULL &&
            m_lstPlaceableLights[i]->m_oidPlaceable == oidPlaceable)
        {
            return m_lstPlaceableLights[i]->m_pLight;
        }
    }
    return NULL;
}
// PC 0x00605C40  Xbox 0x00035E90
void CSWCArea::HiliteAreaObjects(int bHilite)
{
    NwcareaTrace1();
    for (int i = 0; i < m_lstObjectIds.num; i++)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CGameObject* pGameObject =
            pClientExoApp->GetGameObject(m_lstObjectIds[i]);
        if (pGameObject == NULL)
        {
            continue;
        }
        CSWCObject* pObject = pGameObject->AsSWCObject();
        if (pObject == NULL || pObject->m_nFadeState == 0)
        {
            continue;
        }
        switch (pObject->m_nObjectType)
        {
        case SW_OBJECT_TYPE_CREATURE:
        {
            CSWCCreature* pCreature = pObject->AsSWCCreature();
            if (bHilite != 0)
            {
                if (pCreature ==
                    g_pAppManager->m_pClientExoApp->GetPlayerCreature())
                {
                    pObject->Hilite(Vector(1.0f, 1.0f, 1.0f), 2, 0.0f, 1);
                }
                else if (pCreature->m_nHostile == 1)
                {
                    pObject->Hilite(Vector(0.3f, 1.0f, 0.3f), 2, 0.0f, 1);
                }
                else if (pCreature->m_nHostile == 0)
                {
                    pObject->Hilite(Vector(0.4f, 0.4f, 1.0f), 2, 0.0f, 1);
                }
                else
                {
                    pObject->Hilite(Vector(1.0f, 0.1f, 0.1f), 2, 0.0f, 1);
                }
            }
            else
            {
                pCreature->SetPopUpBubbleDecayTime(0.0f);
                pObject->UnHilite(2);
            }
            break;
        }
        case SW_OBJECT_TYPE_ITEM:
            if (bHilite != 0)
            {
                pObject->Hilite(Vector(0.3f, 1.0f, 1.0f), 2, 0.0f, 1);
            }
            else
            {
                pObject->UnHilite(2);
            }
            break;
        case SW_OBJECT_TYPE_DOOR:
            if (bHilite != 0)
            {
                pObject->Hilite(Vector(0.3f, 1.0f, 1.0f), 2, 0.0f, 1);
            }
            else
            {
                pObject->UnHilite(2);
            }
            break;
        case SW_OBJECT_TYPE_PLACEABLE:
            if (bHilite != 0)
            {
                pObject->Hilite(Vector(0.3f, 1.0f, 1.0f), 2, 0.0f, 1);
            }
            else
            {
                pObject->UnHilite(2);
            }
            break;
        }
    }
}
// PC 0x00605E50
CSWMiniGame* CSWCArea::RestartMiniGame()
{
    if (m_pMiniGame != NULL)
    {
        CSWMiniGame* pMiniGame = new CSWMiniGame(this);
        int nTracks = m_pMiniGame->m_lstTracks.num;
        for (int i = 0; i < nTracks; ++i)
        {
            SWMGTrackInfo* pTrack = m_pMiniGame->GetProtoTrack(i);
            pMiniGame->AddTrack(pTrack->m_cResRef, pTrack->m_vPosition);
        }
        int nObstacles = m_pMiniGame->m_lstObstacleIds.num;
        for (int j = 0; j < nObstacles; ++j)
        {
            CSWMGObstacle* pObstacle = m_pMiniGame->GetObstacle(j);
            Gob* pModel = (Gob*)(CAurObject*)pObstacle->m_pModel;
            CResRef cModel(pModel->GetModelName());
            pMiniGame->AddObstacle(cModel, pModel->GetPosition());
        }
        if (!pMiniGame->Load())
        {
            return NULL;
        }
        pMiniGame->SetCamera(m_pMiniGame->m_pCam);
        delete m_pMiniGame;
        m_pMiniGame = pMiniGame;
        m_pMiniGame->Go();
    }
    return m_pMiniGame;
}
// PC 0x00605FA0  Xbox 0x000360E0
CSWCRoom* CSWCArea::GetRoom(const Vector& vPosition)
{
    CSWCRoom* pRoom = NULL;
    int nRoom = GetRoomIndex(vPosition, 0);
    if (nRoom != -1)
    {
        pRoom = &m_pRooms[nRoom];
    }
    return pRoom;
}
// PC 0x00605610  Xbox 0x000357D0
int CSWCArea::GetRoomIndex(const Vector& vPosition, int nFlags)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    return pServerExoApp->GetRoomIndex(m_cAreaResRef, vPosition, nFlags);
}
// PC 0x00605640  Xbox 0x00035800
CSWSArea* CSWCArea::GetServerArea()
{
    if (m_pServerArea == NULL)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        unsigned long nServerId = m_nId;
        nServerId = pServerExoApp->ClientToServerObjectId(nServerId);
        m_pServerArea = pServerExoApp->GetAreaByGameObjectID(nServerId);
        if (m_pServerArea != NULL)
        {
            m_pServerArea->AttachmentFromClientArea(this);
        }
    }
    return m_pServerArea;
}
// PC 0x00605690  Xbox 0x00035850
void CSWCArea::AttachmentFromServerArea(CSWSArea* pArea)
{
    m_pServerArea = pArea;
}
// PC 0x006056A0  Xbox 0x00035860
void CSWCArea::DetachFromServerArea()
{
    m_pServerArea = NULL;
}
// PC 0x006056B0  Xbox 0x00035870
void CSWCArea::AddCorpse(unsigned long oidCorpse)
{
    m_anCorpseObjectIds[m_nCorpseWriteIndex] = oidCorpse;
    m_nCorpseWriteIndex++;
    if (m_nCorpseWriteIndex >= 4)
    {
        m_nCorpseWriteIndex = 0;
    }
    if (m_nCorpseReadIndex == m_nCorpseWriteIndex)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pCreature = pClientExoApp->GetCreatureByGameObjectID(
            m_anCorpseObjectIds[m_nCorpseReadIndex]);
        if (pCreature != NULL)
        {
            unsigned long oidParent = pCreature->m_oidParentCorpse;
            CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
            CSWCPlaceable* pPlaceable =
                pApp->GetPlaceableByGameObjectID(oidParent);
            if (pPlaceable != NULL)
            {
                CSWSPlaceable* pServerPlaceable =
                    pPlaceable->GetServerPlaceable();
                if (pServerPlaceable != NULL)
                {
                    pServerPlaceable->m_bIsBodyBagVisible = TRUE;
                }
                pPlaceable->SetDesiredFadeState(CSWCObject::FADE_STATE_VISIBLE, 0, 0, 0);
            }
            pCreature->SetFadeState(CSWCObject::FADE_STATE_VISIBLE);
            pCreature->SetDesiredFadeState(CSWCObject::FADE_STATE_HIDDEN, 1, 0xAFC8, 0x3E8);
            pCreature->m_bFadeLocked = TRUE;
        }
        m_anCorpseObjectIds[m_nCorpseReadIndex] = OBJECT_INVALID;
        m_nCorpseReadIndex++;
        if (m_nCorpseReadIndex >= 4)
        {
            m_nCorpseReadIndex = 0;
        }
    }
}
// PC 0x006057B0  Xbox 0x00035970
int CSWCArea::ParentCorpseExists(unsigned long oidParent)
{
    for (unsigned char i = 0; i < 4; i++)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pCreature =
            pClientExoApp->GetCreatureByGameObjectID(m_anCorpseObjectIds[i]);
        if (pCreature != NULL && pCreature->m_oidParentCorpse == oidParent)
        {
            return TRUE;
        }
    }
    return FALSE;
}
// PC 0x00605FE0
void CSWCArea::ForceBrightSunAndMoon()
{
    Vector vBlack(0.0f, 0.0f, 0.0f);
    m_vNightAmbientColor = vBlack;
    m_vDayAmbientColor = vBlack;
    Vector vWhite(1.0f, 1.0f, 1.0f);
    m_vNightDiffuseColor = vWhite;
    m_vDayDiffuseColor = vWhite;
    m_bSunFogOn = 0;
    if (m_bIsNight != 0)
    {
        SetAsNight();
    }
    else
    {
        SetAsDay();
    }
}
// PC 0x00606090  Xbox 0x00036190
CSWCArea::CSWCArea()
    : CGameObject(CGameObject::OBJECT_TYPE_AREA, OBJECT_INVALID)
{
    m_sAreaName = "";
    m_pScene = 0;
    m_pShadowLight = 0;
    for (unsigned char i = 0; i < 6; i++)
    {
        m_apWeatherEffects[i] = 0;
    }
    InitializeStaticObjects();
    m_pLightningEffect = 0;
    m_vNightAmbientColor = Vector();
    m_vNightDiffuseColor = Vector();
    m_vMoonColor = Vector();
    m_vDayAmbientColor = Vector();
    m_vDayDiffuseColor = Vector();
    m_vSunColor = Vector();
    m_bDawning = 0;
    m_bDusking = 0;
    m_bIsNight = 0;
    m_vUnknown170 = Vector();
    m_nUnknown1BC = 0;
    m_bLightingChanged = 1;
    m_pAmbientSound = new CSWCAmbientSound(this);
    m_fLightningTimer = 0.0f;
    m_fLightningFlashTime = 0.0f;
    m_nUnknown1E8 = 0;
    m_nWeatherDuration = 0;
    m_nWeatherType = WEATHER_CLEAR;
    m_nNewWeatherType = WEATHER_CLEAR;
    m_fWeatherStrength = 0.0f;
    m_fUnknown1F8 = 0.0f;
    unsigned long nDay;
    unsigned long nTime;
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    m_nCalendarDay = nDay;
    m_nTimeOfDay = nTime;
    m_nUnknown208 = 0;
    m_nUnknown230 = 0;
    m_bIsDark = 0;
    m_pAreaGrid = 0;
    m_nGridX = GRID_COORDINATE_INVALID;
    m_nGridY = GRID_COORDINATE_INVALID;
    m_nGridZ = GRID_COORDINATE_INVALID;
    m_nGridCellCount = 0;
    m_nRoomCount = 0;
    m_pRooms = 0;
    m_pMiniGame = 0;
    m_nCorpseReadIndex = 0;
    m_nCorpseWriteIndex = 1;
    m_pServerArea = 0;
}
// PC 0x006075B0  Xbox 0x00037980
void CSWCArea::ClearTilePathNodes()
{
    m_lstTilePathNodeIds.SetSize(0);
    m_lstTilePathNodes.SetSize(0);
}
// PC 0x00607610  Xbox 0x00037B20
int CSWCArea::LoadArea(CAurCamera* pCamera, CGuiAreaLoadscreen* pLoadScreen,
                       int bLoad)
{
    CSWCMessage* pMessage = g_pAppManager->GetClientExoApp()->GetSWCMessage();
    if (pMessage == NULL)
        return FALSE;

    m_nId = pMessage->ReadOBJECTIDClient();
    CResRef cArea;
    cArea = pMessage->ReadCResRef(16);
    m_cAreaResRef = cArea;
    m_sAreaName = pMessage->ReadCExoLocStringClient(CResRef(), NULL);
    m_nID = pMessage->ReadINT(32);
    m_nCreatorID = pMessage->ReadINT(32);
    m_nVersion = pMessage->ReadINT(32);
    m_nFlags = pMessage->ReadINT(32);
    m_nTurnRateRow = pMessage->ReadINT(32);
    m_cDefaultEnvMap = pMessage->ReadCResRef(16);
    if (pMessage->ReadBYTE(8))
    {
        m_pMiniGame = new CSWMiniGame(this);
        AurDisableInterleavedAnimations();
    }
    else
        AurEnableInterleavedAnimations();

    m_nMoonAmbientColor = pMessage->ReadDWORD(32);
    m_nMoonDiffuseColor = pMessage->ReadDWORD(32);
    m_bMoonFogOn = pMessage->ReadBYTE(8);
    m_nMoonFogColor = pMessage->ReadDWORD(32);
    m_fMoonFogNear = pMessage->ReadFLOAT(1.0f, 32);
    m_fMoonFogFar = pMessage->ReadFLOAT(1.0f, 32);
    m_bMoonShadows = pMessage->ReadBOOL();
    m_nSunAmbientColor = pMessage->ReadDWORD(32);
    m_nSunDiffuseColor = pMessage->ReadDWORD(32);
    m_bSunFogOn = pMessage->ReadBYTE(8);
    m_nSunFogColor = pMessage->ReadDWORD(32);
    m_fSunFogNear = pMessage->ReadFLOAT(1.0f, 32);
    m_fSunFogFar = pMessage->ReadFLOAT(1.0f, 32);
    m_bSunShadows = pMessage->ReadBOOL();
    m_bDayNightCycle = pMessage->ReadBYTE(8);
    m_bIsNight = pMessage->ReadBYTE(8);
    m_nDynAmbientColor = pMessage->ReadDWORD(32);
    m_bNoRest = pMessage->ReadBYTE(8);
    m_nShadowOpacity = pMessage->ReadBYTE(8);
    m_pAmbientSound->LoadFromMessage(pMessage);

    CSWCModule* pModule = g_pAppManager->GetClientExoApp()->GetModule();
    unsigned char nTimeState = pMessage->ReadBYTE(8);
    unsigned long nTime = pMessage->ReadINT(32);
    int nHour = pMessage->ReadBYTE(8);
    int nDay = pMessage->ReadBYTE(8);
    int nMonth = pMessage->ReadBYTE(8);
    int nYear = pMessage->ReadINT(32);
    unsigned char nDawnHour = pMessage->ReadBYTE(8);
    unsigned char nDuskHour = pMessage->ReadBYTE(8);

    m_cGrassTexName = pMessage->ReadCResRef(16);
    m_fGrassDensity = pMessage->ReadFLOAT(1.0f, 32);
    m_fGrassQuadSize = pMessage->ReadFLOAT(1.0f, 32);
    m_nGrassDiffuse = pMessage->ReadDWORD(32);
    m_nGrassAmbient = pMessage->ReadDWORD(32);
    for (int nProbability = 0; nProbability < GRASS_PROB_COUNT; nProbability++)
        m_fGrassProbability[nProbability] = pMessage->ReadFLOAT(1.0f, 32);

    CAurGrassInfo grassInfo;
    m_cGrassTexName.CopyToString(grassInfo.m_szTextureName);
    grassInfo.m_fDensity = m_fGrassDensity;
    grassInfo.m_fQuadSize = m_fGrassQuadSize;
    grassInfo.m_vAmbient.m_fX = (m_nGrassAmbient & 0xFF) * (1.0f / 255.0f);
    grassInfo.m_vAmbient.m_fY = ((m_nGrassAmbient >> 8) & 0xFF) * (1.0f / 255.0f);
    grassInfo.m_vAmbient.m_fZ = ((m_nGrassAmbient >> 16) & 0xFF) * (1.0f / 255.0f);
    grassInfo.m_vDiffuse.m_fX = (m_nGrassDiffuse & 0xFF) * (1.0f / 255.0f);
    grassInfo.m_vDiffuse.m_fY = ((m_nGrassDiffuse >> 8) & 0xFF) * (1.0f / 255.0f);
    grassInfo.m_vDiffuse.m_fZ = ((m_nGrassDiffuse >> 16) & 0xFF) * (1.0f / 255.0f);
    memcpy(grassInfo.m_afProbability, m_fGrassProbability, sizeof(m_fGrassProbability));
    int bGrass;
    for (int nMaterial = 0; nMaterial < 32; nMaterial++)
    {
        {
            CExoString sColumn("Grass");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pSurfaceMaterial = p2DArrays->m_pSurfaceMaterial;
            pSurfaceMaterial->GetINTEntry(nMaterial, sColumn, &bGrass);
        }
        if (bGrass)
            AurGrassAddInfo(nMaterial, grassInfo);
    }
    m_nChanceLightning = pMessage->ReadBYTE(8);
    m_nWind = pMessage->ReadBYTE(8);
    unsigned char nWeather = pMessage->ReadBYTE(8);
    int bWeather = pMessage->ReadBOOL();
    m_fAlphaTest = pMessage->ReadFLOAT(1.0f, 32);

    CExoString* pasRoomNames = NULL;
    int* panEnvAudio = NULL;
    float* pafAmbientScale = NULL;
    int nAudioRooms = pMessage->ReadINT(32);
    if (nAudioRooms)
    {
        pasRoomNames = new CExoString[nAudioRooms];
        panEnvAudio = new int[nAudioRooms];
        pafAmbientScale = new float[nAudioRooms];
    }
    for (int nAudioRoom = 0; nAudioRoom < nAudioRooms; nAudioRoom++)
    {
        pasRoomNames[nAudioRoom] = pMessage->ReadCExoString(32);
        panEnvAudio[nAudioRoom] = pMessage->ReadINT(32);
        pafAmbientScale[nAudioRoom] = pMessage->ReadFLOAT(1.0f, 32);
    }
    int nPartSounds = pMessage->ReadINT(32);
    CExoString* pasSoundRooms = new CExoString[nPartSounds];
    int* pabLooping = new int[nPartSounds];
    CExoString* pasModelParts = new CExoString[nPartSounds];
    CExoString* pasOmenEvents = new CExoString[nPartSounds];
    CResRef* pacSounds = new CResRef[nPartSounds];
    int i;
    for (i = 0; i < nPartSounds; i++)
    {
        pasSoundRooms[i] = pMessage->ReadCExoString(32);
        pabLooping[i] = pMessage->ReadBOOL();
        pasModelParts[i] = pMessage->ReadCExoString(32);
        pasOmenEvents[i] = pMessage->ReadCExoString(32);
        pacSounds[i] = pMessage->ReadCResRef(16);
    }
    pMessage->ReadINT(32);
    m_nWidth = pMessage->ReadINT(32);
    m_nHeight = pMessage->ReadINT(32);
    if (pMessage->MessageReadOverflow(1))
        return FALSE;

    if (bWeather)
        StartWeather(nWeather);
    else
        StartWeather(WEATHER_CLEAR);
    m_pScene = NewCAurScene("mainscene");
    if (m_pScene == NULL)
        return FALSE;
    m_pScene->InitBSP(m_nWidth, m_nHeight, 5, true, 10.0f);
    CLYT* pLayout = new CLYT;
    pLayout->LoadLayout(m_cAreaResRef);
    m_nRoomCount = pLayout->GetRoomCount();
    if (m_nRoomCount)
        m_pRooms = new CSWCRoom[m_nRoomCount];
    AurTextureMakeCurrent();
    RyanStartTextureSize = usedtexturememory;
    RyanTotalLevelVertices = 0;
    RyanTotalLevelVertSize = 0;
    RyanStartParts = countpart;
    int nSceneRoom = 0;
    for (i = 0; i < m_nRoomCount; i++)
    {
        if (i % 5 == 0 || i == m_nRoomCount - 1)
        {
            CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
            pClient->SetLoadBarProgress(
                (int)((float)pClient->GetLoadStep(3) +
                      ((float)pClient->GetLoadStep(1) +
                       ((float)pClient->GetLoadStep(0) +
                        (float)pClient->GetLoadStep(2) *
                            ((float)(i + 1) / (float)m_nRoomCount)))),
                TRUE);
            UpdateScreen(1.0f / 30.0f, FALSE, FALSE);
        }
        m_pRooms[i].SetRoomInfo(pLayout->GetRoomName(i),
                                   pLayout->GetRoomPosition(i));
        m_pRooms[i].SetRoomEnvAudioProps(nAudioRooms, pasRoomNames,
                                             panEnvAudio, pafAmbientScale);
        if (m_pRooms[i].m_bNoWalkMesh)
            m_pRooms[i].m_nRoomIndex = -1;
        else
            m_pRooms[i].m_nRoomIndex = nSceneRoom++;
        Gob* pRoom = m_pRooms[i].SpawnRoom(m_pScene);
        if (pRoom)
        {
            pRoom->PlayAnimation("animloop1", 1.0f,
                AnimRun::FLAG_CURRENT_ANIMATION_PENDING, 0.0f);
            pRoom->PlayAnimation("animloop2", 1.0f,
                AnimRun::FLAG_CURRENT_ANIMATION_PENDING, 0.0f);
            pRoom->PlayAnimation("animloop3", 1.0f,
                AnimRun::FLAG_CURRENT_ANIMATION_PENDING, 0.0f);
        }
        CSWCRoom* pCurrentRoom = &m_pRooms[i];
        CResRef cResRef = pCurrentRoom->m_cResRef;
        CExoString sRoom(cResRef.GetResRefStr());
        for (int nAttachSound = 0; nAttachSound < nPartSounds; nAttachSound++)
        {
            if (pasSoundRooms[nAttachSound] == sRoom && pRoom)
            {
                Gob* pSoundDummy = static_cast<Gob*>(NewCAurObject("MGB_null",
                                                       "sounddummy", NULL, NULL));
                CAurPart* pPart = pRoom->GetPart(pasModelParts[nAttachSound].CStr());
                if (pPart)
                {
                    CSWBehaviorAttachSound* pBehavior =
                        new CSWBehaviorAttachSound(pSoundDummy, pRoom, pPart,
                            pacSounds[nAttachSound].GetResRefStr());
                    pSoundDummy->SetBehavior(pBehavior);
                    pSoundDummy->AttachToScene((Scene*)m_pScene);
                    pSoundDummy->EnableAlwaysRender();
                    if (!pasOmenEvents[nAttachSound].IsEmpty())
                        pRoom->RegisterCallBack(PlayRoomSoundEvent,
                            pasOmenEvents[nAttachSound].CStr(), pBehavior,
                            9999.0f, false);
                    else
                        pBehavior->PlaySoundA();
                    m_lstRoomSounds.AddHead(pBehavior);
                }
            }
        }
    }
    delete[] pasSoundRooms;
    delete[] pabLooping;
    delete[] pasModelParts;
    delete[] pasOmenEvents;
    delete[] pacSounds;
    AurTextureMakeCurrent();
    RyanEndParts = countpart;
    RyanEndTextureSize = usedtexturememory;
    RyanLevelTextureSize = RyanEndTextureSize - RyanStartTextureSize;
    RyanLevelTotalParts = RyanEndParts - RyanStartParts;
    RyanLevelVertices = RyanTotalLevelVertices;
    RyanLevelVertSize = RyanTotalLevelVertSize;
    delete[] pasRoomNames;
    if (panEnvAudio)
        delete[] panEnvAudio;
    if (pafAmbientScale)
        delete[] pafAmbientScale;

    if (m_pMiniGame)
    {
        char szName[17];
        int nTracks = pLayout->GetTrackCount();
        if (nTracks > 0)
        {
            for (i = 0; i < nTracks; i++)
            {
                pLayout->GetTrackName(i).CopyToString(szName);
                CResRef cTrack(szName);
                m_pMiniGame->AddTrack(cTrack, pLayout->GetTrackPosition(i));
            }
        }
        int nObstacles = pLayout->GetObstacleCount();
        if (nObstacles > 0)
        {
            for (i = 0; i < nObstacles; i++)
            {
                pLayout->GetObstacleName(i).CopyToString(szName);
                CResRef cObstacle(szName);
                m_pMiniGame->AddObstacle(cObstacle,
                                         pLayout->GetObstaclePosition(i));
            }
        }
        if (!m_pMiniGame->Load())
        {
            delete pLayout;
            delete[] m_pRooms;
            m_pRooms = NULL;
            return FALSE;
        }
    }
    pLayout->UnloadLayout();
    delete pLayout;
    if (m_pScene->LoadVisibility(m_cAreaResRef.GetResRefStr()))
    {
        m_pScene->SetCurrentRoom(0);
        m_pScene->EnableVisibilityGraph();
    }
    else
        m_pScene->DisableVisibilityGraph();
    AurTextureSuspendUpdates();
    g_pAppManager->GetClientExoApp()->GetInGameGui();
    Vector vPosition;
    int nPins = pMessage->ReadINT(32);
    for (int nPin = 0; nPin < nPins; nPin++)
    {
        CSWCMapPin* pPin = new CSWCMapPin;
        pPin->m_nReference = pMessage->ReadOBJECTIDClient();
        vPosition.m_fX = pMessage->ReadFLOAT(1.0f, 32);
        vPosition.m_fY = pMessage->ReadFLOAT(1.0f, 32);
        vPosition.m_fZ = pMessage->ReadFLOAT(1.0f, 32);
        pPin->SetPosition(vPosition);
        pPin->m_bEnabled = pMessage->ReadBOOL();
        pPin->m_sText = pMessage->ReadCExoLocStringClient(CResRef(), NULL);
        pPin->m_nType = CSWCMapPin::PIN_TYPE_OBJECT_ID;
        AddMapPin(pPin);
    }
    int nReferencePins = pMessage->ReadINT(32);
    for (int nReferencePin = 0; nReferencePin < nReferencePins; nReferencePin++)
    {
        CSWCMapPin* pPin = new CSWCMapPin;
        pPin->m_nReference = pMessage->ReadINT(32);
        pPin->m_sText = pMessage->ReadCExoString(32);
        vPosition.m_fX = pMessage->ReadFLOAT(1.0f, 32);
        vPosition.m_fY = pMessage->ReadFLOAT(1.0f, 32);
        vPosition.m_fZ = pMessage->ReadFLOAT(1.0f, 32);
        pPin->SetPosition(vPosition);
        pPin->m_bEnabled = TRUE;
        pPin->m_nType = CSWCMapPin::PIN_TYPE_REFERENCE_NUMBER;
        AddMapPin(pPin);
    }
    CGameObjectArray* pObjects = g_pAppManager->GetClientExoApp()->GetGameObjectArray();
    unsigned short nSoundObjects = pMessage->ReadWORD(16);
    for (unsigned short nSoundObject = 0; nSoundObject < nSoundObjects;
         nSoundObject++)
    {
        CSWCSoundObject* pSound = new CSWCSoundObject;
        pSound->LoadFromMessage(pMessage);
        pSound->AddToArea(this, pObjects);
    }
    unsigned long oidPlaceable;
    unsigned char nAppearance;
    Vector vPlaceablePosition;
    Vector vPlaceableOrientation;
    unsigned short nLights = pMessage->ReadWORD(16);
    for (i = 0; i < nLights; i++)
    {
        oidPlaceable = pMessage->ReadOBJECTIDClient();
        nAppearance = pMessage->ReadBYTE(8);
        vPlaceablePosition.m_fX = pMessage->ReadFLOAT(1.0f, 32);
        vPlaceablePosition.m_fY = pMessage->ReadFLOAT(1.0f, 32);
        vPlaceablePosition.m_fZ = pMessage->ReadFLOAT(1.0f, 32);
        if (pMessage->MessageReadOverflow(1))
            break;
        AddPlaceableObjectLight(oidPlaceable, nAppearance, vPlaceablePosition);
    }
    unsigned short nStatics = pMessage->ReadWORD(16);
    for (i = 0; i < nStatics; i++)
    {
        oidPlaceable = pMessage->ReadOBJECTIDClient();
        nAppearance = pMessage->ReadBYTE(8);
        vPlaceablePosition.m_fX = pMessage->ReadFLOAT(1.0f, 32);
        vPlaceablePosition.m_fY = pMessage->ReadFLOAT(1.0f, 32);
        vPlaceablePosition.m_fZ = pMessage->ReadFLOAT(1.0f, 32);
        vPlaceableOrientation.m_fX = pMessage->ReadFLOAT(1.0f, 32);
        vPlaceableOrientation.m_fY = pMessage->ReadFLOAT(1.0f, 32);
        vPlaceableOrientation.m_fZ = pMessage->ReadFLOAT(1.0f, 32);
        if (pMessage->MessageReadOverflow(1))
            break;
        AddPlaceableObjectStatic(oidPlaceable, nAppearance,
                                 vPlaceablePosition, vPlaceableOrientation);
    }
    AurTextureResumeUpdates();
    if (pMessage->MessageReadOverflow(1) || pMessage->MessageReadUnderflow(1))
        goto LoadFailed;
    m_pShadowLight = static_cast<Gob*>(NewCAurObject("gidy_sun", "sun", NULL, NULL));
    if (!m_pShadowLight)
        goto LoadFailed;
    m_pShadowLight->SetPosition(Vector(4000.0f, 4500.0f, 7000.0f));
    m_pShadowLight->AttachToScene((Scene*)m_pScene);
    m_pScene->SetFog((unsigned char)m_bSunFogOn);
    projectionalpha = 0.05f;
    m_vMoonColor.m_fX = (m_nMoonFogColor & 0xFF) * (1.0f / 255.0f);
    m_vMoonColor.m_fY = ((m_nMoonFogColor >> 8) & 0xFF) * (1.0f / 255.0f);
    m_vMoonColor.m_fZ = ((m_nMoonFogColor >> 16) & 0xFF) * (1.0f / 255.0f);
    m_vNightAmbientColor.m_fX = (m_nMoonAmbientColor & 0xFF) * (1.0f / 255.0f);
    m_vNightAmbientColor.m_fY = ((m_nMoonAmbientColor >> 8) & 0xFF) * (1.0f / 255.0f);
    m_vNightAmbientColor.m_fZ = ((m_nMoonAmbientColor >> 16) & 0xFF) * (1.0f / 255.0f);
    m_vNightDiffuseColor.m_fX = (m_nMoonDiffuseColor & 0xFF) * (1.0f / 255.0f);
    m_vNightDiffuseColor.m_fY = ((m_nMoonDiffuseColor >> 8) & 0xFF) * (1.0f / 255.0f);
    m_vNightDiffuseColor.m_fZ = ((m_nMoonDiffuseColor >> 16) & 0xFF) * (1.0f / 255.0f);
    m_vSunColor.m_fX = (m_nSunFogColor & 0xFF) * (1.0f / 255.0f);
    m_vSunColor.m_fY = ((m_nSunFogColor >> 8) & 0xFF) * (1.0f / 255.0f);
    m_vSunColor.m_fZ = ((m_nSunFogColor >> 16) & 0xFF) * (1.0f / 255.0f);
    m_vDayAmbientColor.m_fX = (m_nSunAmbientColor & 0xFF) * (1.0f / 255.0f);
    m_vDayAmbientColor.m_fY = ((m_nSunAmbientColor >> 8) & 0xFF) * (1.0f / 255.0f);
    m_vDayAmbientColor.m_fZ = ((m_nSunAmbientColor >> 16) & 0xFF) * (1.0f / 255.0f);
    m_vDayDiffuseColor.m_fX = (m_nSunDiffuseColor & 0xFF) * (1.0f / 255.0f);
    m_vDayDiffuseColor.m_fY = ((m_nSunDiffuseColor >> 8) & 0xFF) * (1.0f / 255.0f);
    m_vDayDiffuseColor.m_fZ = ((m_nSunDiffuseColor >> 16) & 0xFF) * (1.0f / 255.0f);
    if (m_bIsNight)
        SetAsNight();
    else
        SetAsDay();
    m_vUnknown170.m_fX = (m_nDynAmbientColor & 0xFF) * (1.0f / 255.0f);
    m_vUnknown170.m_fY = ((m_nDynAmbientColor >> 8) & 0xFF) * (1.0f / 255.0f);
    m_vUnknown170.m_fZ = ((m_nDynAmbientColor >> 16) & 0xFF) * (1.0f / 255.0f);
    m_pScene->SetAmbient(m_vUnknown170);
    m_pScene->FinishSceneGeometry();
    m_pScene->ComputeStaticLighting();
    pCamera->AttachToScene(m_pScene);
    SetWind(m_nWind);
    pModule->SetTimeOfDayState(nTimeState, 0);
    pModule->m_fTimeOfDay = nTime * 0.001f;
    pModule->m_nTimeA4 = nYear;
    pModule->m_nTimeA8 = nMonth;
    pModule->m_nTimeAC = nDay;
    pModule->m_nTimeB0 = nHour;
    pModule->m_nDawnHour = nDawnHour;
    pModule->m_nDuskHour = nDuskHour;
    UpdateSoundObjectsTimeOfDay();
    m_pAmbientSound->UpdateTimeOfDay();
    {
        CClientOptions* pOptions = g_pAppManager->GetClientExoApp()->GetClientOptions();
        if (m_pScene)
        {
            int nFade;
            switch (pOptions->m_nGeometryFadeMode)
            {
            case 0: nFade = CAurScene::ENABLE_FADE; break;
            case 1: nFade = CAurScene::DISABLE_FADE; break;
            case 2: nFade = CAurScene::ALWAYS_FADE; break;
            }
            m_pScene->SetGeomFadeProperties(nFade);
        }
    }
    if (g_pAppManager->GetClientExoApp()->GetPlayerCreature())
        g_pAppManager->GetClientExoApp()->GetPlayerCreature()->RefreshVisualEffects(0);
    for (int nObject = 0; nObject < m_lstObjectIds.num; nObject++)
    {
        CSWCCreature* pCreature = g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
            m_lstObjectIds.element[nObject]);
        if (pCreature)
            pCreature->UpdateAreaId();
    }
    return TRUE;

LoadFailed:
    delete m_pScene;
    m_pScene = NULL;
    return FALSE;
}
// PC 0x00609410  Xbox 0x00039970
void CSWCArea::UnloadArea()
{
    if (g_pAppManager->m_pClientExoApp->GetInGameGui() != NULL)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->FadeAndStopDialogAmbientTrack();
    }
    AurGrassClearInfo();
    ClearTilePathNodes();
    m_lstRenderTriggerIds.SetSize(0);
    for (int nEffect = 0; nEffect < 6; nEffect++)
    {
        if (m_apWeatherEffects[nEffect] != NULL)
        {
            delete m_apWeatherEffects[nEffect];
            m_apWeatherEffects[nEffect] = NULL;
        }
    }
    CGameObjectArray* pGameObjectArray =
        g_pAppManager->m_pClientExoApp->GetGameObjectArray();
    CClientAIMaster* pClientAIMaster =
        g_pAppManager->m_pClientExoApp->GetClientAIMaster();
    int nSoundObjects = m_lstSoundObjectIds.num;
    for (int i = 0; i < nSoundObjects; i++)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCSoundObject* pSoundObject =
            pClientExoApp->GetSoundObjectByGameObjectID(
                m_lstSoundObjectIds.element[i]);
        if (pSoundObject != NULL)
        {
            pSoundObject->Stop();
            pClientAIMaster->RemoveObject(pSoundObject);
            pGameObjectArray->Delete(m_lstSoundObjectIds.element[i]);
            delete pSoundObject;
        }
    }
    m_lstSoundObjectIds.SetSize(0);
    if (m_pAmbientSound != NULL)
    {
        m_pAmbientSound->StopSounds(0);
    }
    if (m_pLightningEffect != NULL)
    {
        delete m_pLightningEffect;
        m_pLightningEffect = NULL;
    }
    m_fLightningTimer = 0.0f;
    m_fLightningFlashTime = 0.0f;
    m_nUnknown1E8 = 0;
    m_nWeatherDuration = 0;
    m_nWeatherType = WEATHER_CLEAR;
    m_nNewWeatherType = WEATHER_CLEAR;
    m_fWeatherStrength = 0.0f;
    m_fUnknown1F8 = 0.0f;
    ClearAllMapPins();
    if (m_pShadowLight != NULL)
    {
        delete m_pShadowLight;
        m_pShadowLight = NULL;
    }
    if (m_pScene != NULL)
    {
        delete m_pScene;
        m_pScene = NULL;
    }
    while (m_lstRoomSounds.GetHeadPos() != NULL)
    {
        CSWBehaviorAttachSound* pBehavior = m_lstRoomSounds.RemoveHead();
        if (pBehavior != NULL)
        {
            delete pBehavior;
        }
    }
    EmptyCAurObjectToHitCheckIgnoreList();
    for (int j = m_lstPlaceableLights.num - 1; j >= 0; j--)
    {
        CSWCPlaceableObjectLight* pLight = m_lstPlaceableLights.element[j];
        if (pLight != NULL)
        {
            if (pLight->m_pLight != NULL)
            {
                delete pLight->m_pLight;
            }
            delete m_lstPlaceableLights.element[j];
            m_lstPlaceableLights.DelIndex(j);
        }
    }
    for (int k = m_lstDeferredDeleteIds.num - 1; k >= 0; k--)
    {
        CGameObject* pGameObject;
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        if (pClientExoApp->GetGameObjectArray()->Delete(
                m_lstDeferredDeleteIds.element[k], &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject != NULL &&
            reinterpret_cast<CSWCObject*>(pGameObject)->AsSWCPlaceable() !=
                NULL &&
            pGameObject != NULL)
        {
            delete reinterpret_cast<CSWCObject*>(pGameObject);
        }
        m_lstDeferredDeleteIds.DelIndex(k);
    }
    if (m_pAreaGrid != NULL)
    {
        delete m_pAreaGrid;
        m_pAreaGrid = NULL;
        m_nGridX = GRID_COORDINATE_INVALID;
        m_nGridY = GRID_COORDINATE_INVALID;
        m_nGridZ = GRID_COORDINATE_INVALID;
        m_nGridCellCount = 0;
    }
    for (int m = 0; m < 4; m++)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pCreature =
            pClientExoApp->GetCreatureByGameObjectID(m_anCorpseObjectIds[m]);
        if (pCreature != NULL)
        {
            CGameObject* pGameObject;
            CClientAIMaster* pAIMaster =
                g_pAppManager->m_pClientExoApp->GetClientAIMaster();
            pAIMaster->m_aiList.RemoveObject(pCreature);
            pClientExoApp = g_pAppManager->m_pClientExoApp;
            if (pClientExoApp->GetGameObjectArray()->Delete(
                    m_anCorpseObjectIds[m], &pGameObject) ==
                    CGameObjectArray::SUCCESS &&
                pGameObject != NULL)
            {
                if (pGameObject->AsSWCCreature() == pCreature)
                {
                    delete pCreature;
                }
            }
        }
        m_anCorpseObjectIds[m] = OBJECT_INVALID;
    }
    m_nCorpseReadIndex = 0;
    m_nCorpseWriteIndex = 1;
}
// PC 0x006097F0  Xbox 0x00039D00
void CSWCArea::Update(float fTimeDelta)
{
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    unsigned long nDayDifference;
    unsigned long nTimeDifference;

    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(
        &nCalendarDay, &nTimeOfDay);
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nCalendarDay, nTimeOfDay, m_nCalendarDay, m_nTimeOfDay,
        &nDayDifference, &nTimeDifference);
    m_nCalendarDay = nCalendarDay;
    m_nTimeOfDay = nTimeOfDay;
    float fTimeStep = nTimeDifference / 1000.0f;

    if (m_pScene == NULL)
    {
        return;
    }

    CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    CAurCamera* pCamera = g_pAppManager->GetClientExoApp()->GetModuleCamera();
    if (pPlayer != NULL && pCamera != NULL)
    {
        Vector vPosition = pPlayer->m_vPosition;
        vPosition.m_fZ += 15.0f;
        Vector vDirection;
        Vector vRight;
        vDirection = pCamera->GetOrientation().zdir();
        vDirection.m_fZ = 0.0f;
        vDirection = normalize(vDirection);
        vRight = cross(vDirection, Vector(0.0f, 0.0f, 1.0f));
        vRight = normalize(vRight) * 10.0f;
        Vector vForward = vDirection * 10.0f;
        vRight.m_fZ = 0.0f;
        vForward.m_fZ = 0.0f;
        vPosition -= vRight;

        for (short j = 0, nEffect = 0; j < 2; j++)
        {
            for (short i = 0; i < 3; i++, nEffect++)
            {
                if (m_apWeatherEffects[nEffect] != NULL)
                {
                    m_apWeatherEffects[nEffect]->SetOrientation(
                        YawPitchRoll(Yaw(vDirection), 0.0f, 0.0f));
                    if (i == 1 && j == 0)
                    {
                        m_apWeatherEffects[nEffect]->SetPosition(
                            vPosition + vRight * i + vForward * j);
                    }
                    else
                    {
                        m_apWeatherEffects[nEffect]->SetPosition(
                            vPosition + vRight * i + vForward * j -
                            Vector(0.0f, 0.0f, 5.0f));
                    }
                }
            }
        }

        if (m_pLightningEffect != NULL)
        {
            int bStartFlash = TRUE;
            if (m_fLightningFlashTime != 0.0f)
            {
                if (m_fLightningFlashTime <= fTimeStep)
                {
                    m_fLightningFlashTime = 0.0f;
                    SetLightningDone();
                }
                else
                {
                    bStartFlash = FALSE;
                    m_fLightningFlashTime -= fTimeStep;
                }
            }
            if (m_fLightningTimer != 0.0f)
            {
                if (m_fLightningTimer <= fTimeStep)
                {
                    m_fLightningTimer = 0.0f;
                }
                else
                {
                    bStartFlash = FALSE;
                    m_fLightningTimer -= fTimeStep;
                }
            }
            if (bStartFlash)
            {
                m_fLightningTimer =
                    (rand() % LIGHTNING_DELAY_RANGE) / 1000.0f + 5.0f;
                if (rand() % 100 < m_nChanceLightning && rand() % 2)
                {
                    char szAnimation[16];
                    float fAnimationTime;
                    float fAnimationLength;
                    vPosition = pPlayer->m_vPosition;
                    vPosition.m_fZ += 10.0f;
                    if (rand() % 2)
                    {
                        vPosition.m_fX += (rand() % 100) / 10.0f;
                    }
                    else
                    {
                        vPosition.m_fX -= (rand() % 100) / 10.0f;
                    }
                    if (rand() % 2)
                    {
                        vPosition.m_fY += (rand() % 100) / 10.0f;
                    }
                    else
                    {
                        vPosition.m_fY -= (rand() % 100) / 10.0f;
                    }
                    m_pLightningEffect->AttachToScene((Scene*)m_pScene);
                    m_pLightningEffect->SetPosition(vPosition);
                    sprintf(szAnimation, "flash0%i", rand() % 3 + 1);
                    m_pLightningEffect->GetAnimation(szAnimation,
                                                     &fAnimationTime,
                                                     &fAnimationLength);
                    m_fLightningFlashTime = fAnimationLength;
                    m_pLightningEffect->PlayAnimation(szAnimation, 1.0f, 3,
                                                      0.0f);
                    g_fOldShadowAlpha = shadowalpha;
                    shadowalpha *= 0.4f;
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                }
            }
        }
    }

    if (m_nWind == 1 || m_nWind == 2)
    {
        if (m_fUnknown1F8 <= fTimeStep)
        {
            int nGust;
            switch (m_nWind)
            {
            case 1:
                nGust = rand();
                break;
            case 2:
                nGust = rand();
                break;
            }
            m_fUnknown1F8 = (rand() % WIND_GUST_DELAY_RANGE) / 1000.0f + 5.0f;
        }
        else
        {
            m_fUnknown1F8 -= fTimeStep;
        }
    }

    if (m_bDayNightCycle)
    {
        UpdateDayNightCycle(fTimeStep);
    }
    if (m_bIsNight && m_nUnknown230)
    {
        m_nUnknown230 = 0;
    }

    if (m_nWeatherDuration > 0)
    {
        if (m_nWeatherDuration > nTimeDifference)
        {
            m_fWeatherStrength =
                ((WEATHER_FADE_DURATION - m_nWeatherDuration) / 6000.0f) *
                100.0f;
            if (m_nNewWeatherType == WEATHER_RAIN)
            {
                SetWeather(WEATHER_RAIN, m_fWeatherStrength);
            }
            else if (m_nNewWeatherType == WEATHER_SNOW)
            {
                SetWeather(WEATHER_SNOW, m_fWeatherStrength);
            }
            else if (m_nNewWeatherType == WEATHER_CLEAR)
            {
                m_fWeatherStrength = (m_nWeatherDuration / 6000.0f) * 100.0f;
                SetWeather(m_nWeatherType, m_fWeatherStrength);
            }
            m_nWeatherDuration -= nTimeDifference;
        }
        else
        {
            m_nWeatherDuration = 0;
            SetWeather(m_nNewWeatherType, 100.0f);
        }
    }

    m_pAmbientSound->Update();
    g_pAppManager->GetClientExoApp()->GetAnimationTimer()->SetSnapshotTime();
    m_pScene->Animate(
        (float)(__int64)g_pAppManager->GetClientExoApp()->GetAnimationTimer()
            ->GetSnapshotTimeDifference() /
        1000000.0f);
    if (m_pMiniGame != NULL)
    {
        m_pMiniGame->Update(fTimeDelta);
    }
}


// PC 0x00606490  Xbox 0x00036650
void CSWCArea::UpdateDayNightCycle(float fTimeStep)
{
    Vector vAmbient;
    Vector vDiffuse;
    Vector vFog;
    float fFogNear;
    float fFogFar;
    char sCommand[128];

    if (!m_bDawning && !m_bDusking)
    {
        return;
    }
    CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
    if (pModule == NULL)
    {
        return;
    }
    float fDuration = pModule->m_fTimeOfDayHours;
    float fTime = pModule->m_fTimeOfDay;
    if (!m_nUnknown1BC)
    {

        if (m_bDawning)
        {
            for (int i = 0; i < m_nWidth * m_nHeight; ++i)
            {
            }
        }
        else if (m_bDusking)
        {
            for (int i = 0; i < m_nWidth * m_nHeight; ++i)
            {
            }
        }
        m_nUnknown1BC = TRUE;
    }
    if (m_bDawning)
    {
        if (fTime >= fDuration)
        {
            projectionalpha = 0.05f;
            shadowalpha = m_nShadowOpacity * 0.01f;
            m_nUnknown1BC = FALSE;
            m_bDawning = FALSE;
            vAmbient = m_vDayAmbientColor;
            vDiffuse = m_vDayDiffuseColor;
            vFog = m_vSunColor;
            fFogNear = m_fSunFogNear;
            fFogFar = m_fSunFogFar;
            UpdateShadowingLights();
            m_bIsNight = FALSE;
        }
        else
        {
            float fFraction = fTime / fDuration;
            projectionalpha = 0.05f * fFraction;
            Vector vAmbientDelta = m_vDayAmbientColor - m_vNightAmbientColor;
            Vector vDiffuseDelta = m_vDayDiffuseColor - m_vNightDiffuseColor;
            Vector vFogDelta = m_vSunColor - m_vMoonColor;
            vAmbient = vAmbientDelta * fFraction + m_vNightAmbientColor;
            vDiffuse = vDiffuseDelta * fFraction + m_vNightDiffuseColor;
            vFog = vFogDelta * fFraction + m_vMoonColor;
            fFogNear = (m_fSunFogNear - m_fMoonFogNear) * fFraction + m_fMoonFogNear;
            fFogFar = (m_fSunFogFar - m_fMoonFogFar) * fFraction + m_fMoonFogFar;
        }
    }
    else if (m_bDusking)
    {
        if (fTime >= fDuration)
        {
            projectionalpha = 0.0f;
            shadowalpha = m_nShadowOpacity * 0.01f;
            m_nUnknown1BC = FALSE;
            m_bDusking = FALSE;
            vAmbient = m_vNightAmbientColor;
            vDiffuse = m_vNightDiffuseColor;
            vFog = m_vMoonColor;
            fFogNear = m_fFogNear;
            fFogFar = m_fFogFar;
            UpdateShadowingLights();
            m_bIsNight = TRUE;
        }
        else
        {
            float fFraction = fTime / fDuration;
            projectionalpha = 0.05f - 0.05f * fFraction;
            Vector vAmbientDelta = m_vNightAmbientColor - m_vDayAmbientColor;
            Vector vDiffuseDelta = m_vNightDiffuseColor - m_vDayDiffuseColor;
            Vector vFogDelta = m_vMoonColor - m_vSunColor;
            vAmbient = vAmbientDelta * fFraction + m_vDayAmbientColor;
            vDiffuse = vDiffuseDelta * fFraction + m_vDayDiffuseColor;
            vFog = vFogDelta * fFraction + m_vSunColor;
            fFogNear = (m_fMoonFogNear - m_fSunFogNear) * fFraction + m_fSunFogNear;
            fFogFar = (m_fMoonFogFar - m_fSunFogFar) * fFraction + m_fSunFogFar;
        }
    }

    float fHalfDuration = fDuration * 0.5f;
    if (fTime >= fHalfDuration && m_bLightingChanged)
    {
        if (m_bDusking)
        {
            m_bIsNight = TRUE;
        }
        else if (m_bDawning)
        {
            m_bIsNight = FALSE;
        }
        UpdateShadowingLights();
        m_bLightingChanged = FALSE;
    }
    if (fTime < fDuration)
    {
        if (m_bLightingChanged)
        {
            float fShadowAlpha = m_nShadowOpacity * 0.01f;
            float fShadowDelta = -fShadowAlpha;
            shadowalpha = (fTime / fHalfDuration) * fShadowDelta + fShadowAlpha;
        }
        else
        {
            shadowalpha = (fTime / fHalfDuration - 1.0f) * (m_nShadowOpacity * 0.01f);
        }
    }
    sprintf(sCommand, "sun gidy_sun_amb color %f %f %f",
            vAmbient.m_fX, vAmbient.m_fY, vAmbient.m_fZ);
    AurControlPart(sCommand);
    sprintf(sCommand, "sun gidy_sun_diff color %f %f %f",
            vDiffuse.m_fX, vDiffuse.m_fY, vDiffuse.m_fZ);
    AurControlPart(sCommand);
    m_fFogNear = fFogNear;
    m_fFogFar = fFogFar;
    m_pScene->SetFogRange(fFogNear, fFogFar);
    m_pScene->SetFogColor(vFog);
    sprintf(sCommand, "%f %f %f", vFog.m_fX, vFog.m_fY, vFog.m_fZ);
    SetClearColor(sCommand);
    UpdateSoundObjectsTimeOfDay();
    m_pAmbientSound->UpdateTimeOfDay();
}
// PC 0x00606C50  Xbox 0x00037020
void CSWCArea::SetAsDusk()
{
    if (m_bDayNightCycle == 0)
    {
        if (m_bIsNight != 0)
        {
            SetAsNight();
        }
        else
        {
            SetAsDay();
        }
    }
    else
    {
        projectionalpha = 0.05f;
        shadowalpha = m_nShadowOpacity * 0.01f;
        m_bLightingChanged = TRUE;
        m_bDusking = TRUE;
        m_bDawning = FALSE;
        m_bIsNight = FALSE;
        UpdateShadowingLights();
    }
}
// PC 0x00606CF0  Xbox 0x000370C0
void CSWCArea::SetAsDawn()
{
    if (m_bDayNightCycle == 0)
    {
        if (m_bIsNight != 0)
        {
            SetAsNight();
        }
        else
        {
            SetAsDay();
        }
    }
    else
    {
        projectionalpha = 0.0f;
        shadowalpha = m_nShadowOpacity * 0.01f;
        m_bLightingChanged = TRUE;
        m_bDawning = TRUE;
        m_bDusking = FALSE;
        m_bIsNight = TRUE;
        UpdateShadowingLights();
    }
}
// PC 0x00606D90  Xbox 0x00037160
void CSWCArea::AddMapPin(CSWCMapPin* pMapPin)
{
    m_lstMapPins.Add(pMapPin);
}
// PC 0x00606DD0  Xbox 0x000371A0
void CSWCArea::ClearAllMapPins()
{
    int nCount = m_lstMapPins.num;
    for (int i = 0; i < nCount; i++)
    {
        CSWCMapPin* pMapPin = m_lstMapPins.element[i];
        if (pMapPin != NULL)
        {
            delete pMapPin;
        }
    }
    m_lstMapPins.SetSize(0);
}
// PC 0x00606E30  Xbox 0x00037200
int CSWCArea::AddPlaceableObjectLight(unsigned long oidPlaceable,
                                      unsigned char nAppearance,
                                      Vector vPosition)
{
    Quaternion qOrientation;
    Vector vColor;
    Vector vOffset;
    Vector vRootPosition;
    int nLightColor;
    bool bFailed;
    {
        CExoString sColumn("LightColor");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        bFailed = !pPlaceables->GetINTEntry(nAppearance, sColumn,
                                            &nLightColor);
    }
    if (bFailed)
    {
        return TRUE;
    }

    int bFound = FALSE;
    CSWCPlaceableObjectLight* pLight;
    int nCount = m_lstPlaceableLights.num;
    for (int i = 0; i < nCount; i++)
    {
        if (m_lstPlaceableLights.element[i] != NULL &&
            m_lstPlaceableLights.element[i]->m_oidPlaceable == oidPlaceable)
        {
            pLight = m_lstPlaceableLights.element[i];
            bFound = TRUE;
            break;
        }
    }
    if (!bFound)
    {
        pLight = new CSWCPlaceableObjectLight;
        if (pLight == NULL)
        {
            return FALSE;
        }
    }
    pLight->m_pLight = static_cast<Gob*>(
        NewCAurObject("fx_placeable01", "placeablelight", NULL, NULL));
    if (pLight->m_pLight == NULL)
    {
        delete pLight;
        return FALSE;
    }
    pLight->m_oidPlaceable = oidPlaceable;
    {
        CExoString sColumn("RED");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pLightColor = p2DArrays->m_pLightColor;
        pLightColor->GetFLOATEntry(nLightColor, sColumn, &vColor.m_fX);
    }
    {
        CExoString sColumn("GREEN");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pLightColor = p2DArrays->m_pLightColor;
        pLightColor->GetFLOATEntry(nLightColor, sColumn, &vColor.m_fY);
    }
    {
        CExoString sColumn("BLUE");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pLightColor = p2DArrays->m_pLightColor;
        pLightColor->GetFLOATEntry(nLightColor, sColumn, &vColor.m_fZ);
    }
    {
        CExoString sColumn("LightOffsetX");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        bFailed = !pPlaceables->GetFLOATEntry(nAppearance, sColumn,
                                              &vOffset.m_fX);
    }
    if (bFailed)
    {
        vOffset.m_fX = 0.0f;
    }
    {
        CExoString sColumn("LightOffsetY");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        bFailed = !pPlaceables->GetFLOATEntry(nAppearance, sColumn,
                                              &vOffset.m_fY);
    }
    if (bFailed)
    {
        vOffset.m_fY = 0.0f;
    }
    {
        CExoString sColumn("LightOffsetZ");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pPlaceables = p2DArrays->m_pPlaceables;
        bFailed = !pPlaceables->GetFLOATEntry(nAppearance, sColumn,
                                              &vOffset.m_fZ);
    }
    if (bFailed)
    {
        vOffset.m_fZ = 3.0f;
    }

    pLight->m_pLight->GetPartLocalPosition("root", &vRootPosition,
                                           &qOrientation);
    pLight->m_pLight->SetPartLocalPosition("root", vRootPosition + vOffset,
                                           qOrientation);
    pLight->m_pLight->SetPosition(vPosition);
    pLight->m_pLight->AttachToScene((Scene*)m_pScene);
    if (!bFound)
    {
        m_lstPlaceableLights.Add(pLight);
    }
    return TRUE;
}
// PC 0x006072D0  Xbox 0x000376A0
int CSWCArea::AddPlaceableObjectStatic(unsigned long oidPlaceable,
                                      unsigned char nAppearance,
                                      Vector vPosition, Vector vOrientation)
{
    CGameObjectArray* pGameObjectArray =
        g_pAppManager->m_pClientExoApp->GetGameObjectArray();
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    CSWCPlaceable* pPlaceable =
        pClientExoApp->GetPlaceableByGameObjectID(oidPlaceable);
    if (pPlaceable == NULL)
    {
        CExoString sModel;
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nGoreLevel ==
            0)
        {
            bool bMissing;
            {
                CExoString sColumn("LowGore");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pPlaceables = p2DArrays->m_pPlaceables;
                bMissing = pPlaceables->GetCExoStringEntry(
                    nAppearance, sColumn, &sModel) == 0;
            }
            if (bMissing)
            {
                CExoString sColumn("ModelName");
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pPlaceables = p2DArrays->m_pPlaceables;
                pPlaceables->GetCExoStringEntry(nAppearance, sColumn, &sModel);
            }
        }
        else
        {
            CExoString sColumn("ModelName");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPlaceables = p2DArrays->m_pPlaceables;
            pPlaceables->GetCExoStringEntry(nAppearance, sColumn, &sModel);
        }
        pPlaceable = new CSWCPlaceable;
        if (pPlaceable == NULL)
        {
            return FALSE;
        }
        pPlaceable->SetAppearance(nAppearance);
        pPlaceable->SetId(oidPlaceable);
        pPlaceable->SetArea(this);
        if (pGameObjectArray->AddExternalObject(
                oidPlaceable, (CGameObject*)pPlaceable, TRUE) !=
            CGameObjectArray::SUCCESS)
        {
            return FALSE;
        }
        pPlaceable->SetPosition(vPosition);
        pPlaceable->SetOrientation(vOrientation);
        Quaternion qOrientation = YawPitchRoll(Yaw(vOrientation), 0.0f, 0.0f);
    }
    pPlaceable->m_bPlotObject = TRUE;
    pPlaceable->m_bUseable = 0;
    pPlaceable->m_nUnknown130 = 0;
    pPlaceable->m_nUnknown114 = 0;
    pPlaceable->m_bLocked = 0;
    pPlaceable->m_nUnknown12C = 0;
    pPlaceable->m_nUnknown120 = 0;
    pPlaceable->m_bTrapDetected = 0;
    pPlaceable->SetAnimation(0, 0, 0);
    g_pAppManager->m_pClientExoApp->RemoveObjectFromHitcheckIgnoreList(
        oidPlaceable);
    m_lstDeferredDeleteIds.Add(oidPlaceable);
    return TRUE;
}

void CSWCArea::EmptyCAurObjectToHitCheckIgnoreList()
{
    while (m_lstHitCheckIgnoreObjects.GetHeadPos() != NULL)
    {
        m_lstHitCheckIgnoreObjects.RemoveHead();
    }
}
// PC 0x0060A0F0  Xbox 0x0003A6B0
CSWCArea::~CSWCArea()
{
    UnloadArea();
    delete m_pAmbientSound;
    m_pAmbientSound = 0;
    delete[] m_pRooms;
    m_pRooms = 0;
    delete m_pMiniGame;
    if (m_pServerArea != 0)
    {
        m_pServerArea->DetachFromClientArea();
        m_pServerArea = 0;
    }
}

static inline void NwcareaTrace1()
{
}

#ifdef _XBOX
#include "aurconsole.h"



// Xbox 0x00034B60
char* setweatherdensity(char* sArguments)
{
    int nDensity;
    int nWeather;
    int nUnused;
    if (sscanf(sArguments, "%i %i %i", &nWeather, &nDensity, &nUnused) != 3)
    {
        return "Failure";
    }
    if (g_pAppManager != NULL && g_pAppManager->m_pClientExoApp != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule() != NULL)
    {
        CSWCArea* pArea = g_pAppManager->m_pClientExoApp->GetModule()->GetArea();
        if (pArea != NULL)
        {
            pArea->SetWeather(nWeather, (float)nDensity);
        }
    }
    return "Success";
}
ConsoleFunc ConFsetweatherdensity("setweatherdensity", setweatherdensity);
#endif
