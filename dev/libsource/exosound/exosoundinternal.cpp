#ifndef _XBOX
#include "eax.h"
#endif
#include "exosoundinternal.h"
#include "exobase.h"
#include "exoresman.h"
#include "swmain.h"
#include <ctype.h>
#include "twodimarray.h"


static inline void ExoSoundTrace();
#define EXOSOUND_TRACE() ExoSoundTrace()

int g_bDisableSound;
int g_bSoundPaused;


#ifndef _XBOX
const EAXLISTENERPROPERTIES CExoSoundInternal::CEXOSOUND_PRESETS[24] =
{
    { EAX_ENVIRONMENT_GENERIC, 7.5f, 1.0f, -1000, -100, 0, 1.49f, 0.83f, 1.0f, -2602, 0.007f, { 0.0f, 0.0f, 0.0f }, 200, 0.011f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_PADDEDCELL, 1.4f, 1.0f, -1000, -6000, 0, 0.17f, 0.1f, 1.0f, -1204, 0.001f, { 0.0f, 0.0f, 0.0f }, 207, 0.002f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_ROOM, 1.9f, 1.0f, -1000, -454, 0, 0.4f, 0.83f, 1.0f, -1646, 0.002f, { 0.0f, 0.0f, 0.0f }, 53, 0.003f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_BATHROOM, 1.4f, 1.0f, -1000, -1200, 0, 1.49f, 0.54f, 1.0f, -370, 0.007f, { 0.0f, 0.0f, 0.0f }, 1030, 0.011f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_LIVINGROOM, 2.5f, 1.0f, -1000, -6000, 0, 0.5f, 0.1f, 1.0f, -1376, 0.003f, { 0.0f, 0.0f, 0.0f }, -1104, 0.004f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_STONEROOM, 11.6f, 1.0f, -1000, -300, 0, 2.31f, 0.64f, 1.0f, -711, 0.012f, { 0.0f, 0.0f, 0.0f }, 83, 0.017f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_AUDITORIUM, 21.6f, 1.0f, -1000, -476, 0, 4.32f, 0.59f, 1.0f, -789, 0.02f, { 0.0f, 0.0f, 0.0f }, -289, 0.03f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_CONCERTHALL, 19.6f, 1.0f, -1000, -500, 0, 3.92f, 0.7f, 1.0f, -1230, 0.02f, { 0.0f, 0.0f, 0.0f }, -2, 0.029f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_CAVE, 14.6f, 1.0f, -1000, 0, 0, 2.91f, 1.3f, 1.0f, -602, 0.015f, { 0.0f, 0.0f, 0.0f }, -302, 0.022f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 31 },
    { EAX_ENVIRONMENT_ARENA, 36.2f, 1.0f, -1000, -698, 0, 7.24f, 0.33f, 1.0f, -1166, 0.02f, { 0.0f, 0.0f, 0.0f }, 16, 0.03f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_HANGAR, 50.3f, 1.0f, -1000, -1000, 0, 10.05f, 0.23f, 1.0f, -602, 0.02f, { 0.0f, 0.0f, 0.0f }, 198, 0.03f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_CARPETEDHALLWAY, 1.9f, 1.0f, -1000, -4000, 0, 0.3f, 0.1f, 1.0f, -1831, 0.002f, { 0.0f, 0.0f, 0.0f }, -1630, 0.03f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_HALLWAY, 1.8f, 1.0f, -1000, -300, 0, 1.49f, 0.59f, 1.0f, -1219, 0.007f, { 0.0f, 0.0f, 0.0f }, 441, 0.011f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_STONECORRIDOR, 13.5f, 1.0f, -1000, -237, 0, 2.7f, 0.79f, 1.0f, -1214, 0.013f, { 0.0f, 0.0f, 0.0f }, 395, 0.02f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_ALLEY, 7.5f, 0.3f, -1000, -270, 0, 1.49f, 0.86f, 1.0f, -1204, 0.007f, { 0.0f, 0.0f, 0.0f }, -4, 0.011f, { 0.0f, 0.0f, 0.0f }, 0.125f, 0.95f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_FOREST, 38.0f, 0.3f, -1000, -3300, 0, 1.49f, 0.54f, 1.0f, -2560, 0.162f, { 0.0f, 0.0f, 0.0f }, -229, 0.088f, { 0.0f, 0.0f, 0.0f }, 0.125f, 1.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_CITY, 7.5f, 0.5f, -1000, -800, 0, 1.49f, 0.67f, 1.0f, -2273, 0.007f, { 0.0f, 0.0f, 0.0f }, -1691, 0.011f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_MOUNTAINS, 100.0f, 0.27f, -1000, -2500, 0, 1.49f, 0.21f, 1.0f, -2780, 0.3f, { 0.0f, 0.0f, 0.0f }, -1434, 0.1f, { 0.0f, 0.0f, 0.0f }, 0.25f, 1.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 31 },
    { EAX_ENVIRONMENT_QUARRY, 17.5f, 1.0f, -1000, -1000, 0, 1.49f, 0.83f, 1.0f, -10000, 0.061f, { 0.0f, 0.0f, 0.0f }, 500, 0.025f, { 0.0f, 0.0f, 0.0f }, 0.125f, 0.7f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_PLAIN, 42.5f, 0.21f, -1000, -2000, 0, 1.49f, 0.5f, 1.0f, -2466, 0.179f, { 0.0f, 0.0f, 0.0f }, -1926, 0.1f, { 0.0f, 0.0f, 0.0f }, 0.25f, 1.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_PARKINGLOT, 8.3f, 1.0f, -1000, 0, 0, 1.65f, 1.5f, 1.0f, -1363, 0.008f, { 0.0f, 0.0f, 0.0f }, -1153, 0.012f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 31 },
    { EAX_ENVIRONMENT_SEWERPIPE, 1.7f, 0.8f, -1000, -1000, 0, 2.81f, 0.14f, 1.0f, 429, 0.014f, { 0.0f, 0.0f, 0.0f }, 1023, 0.021f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_UNDERWATER, 1.8f, 1.0f, -1000, -4000, 0, 1.49f, 0.1f, 1.0f, -449, 0.007f, { 0.0f, 0.0f, 0.0f }, 1700, 0.011f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 1.18f, 0.348f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 },
    { EAX_ENVIRONMENT_GENERIC, 7.5f, 1.0f, -1000, -100, 0, 1.49f, 0.83f, 1.0f, -2602, 0.007f, { 0.0f, 0.0f, 0.0f }, 200, 0.011f, { 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 63 }
};
#endif



extern float __stdcall GetObstructionFactor(Vector, Vector);
// PC 0x005D64F0
inline int CExoSoundSourceInternal::GetPlayWhenFree()
{
    return m_bPlayWhenFree;
}
// PC 0x005D6500
inline unsigned long CExoSoundSourceInternal::GetOneShotDelay()
{
    return m_nOneShotDelay;
}
// PC 0x005D6510
inline unsigned long CExoSoundSourceInternal::GetDelayStart()
{
    return m_nDelayStart;
}
// PC 0x005D6520
inline unsigned char CExoSoundSourceInternal::GetVolume()
{
    return m_nVolume;
}

#ifndef _XBOX
inline unsigned long CExoStreamingSoundSourceInternal::GetFadeStartTime()  // PC 0x005D6530
{
    return m_nFadeStartTime;
}
#endif

#ifndef _XBOX
void CExoSoundInternal::SetListenerOrientation(const Vector& vOrientation,  // PC 0x005D6540  Xbox 0x0014FAF0
                                               const Vector& vUp)
{
    if (equal(m_vListenerOrientation, vOrientation) == 0 ||
        equal(m_vListenerUp, vUp) == 0)
    {
        m_vListenerOrientation = vOrientation;
        m_vListenerUp = vUp;
        if (m_h3DProvider != NULL)
        {
            H3DPOBJECT hListener = AIL_open_3D_listener(m_h3DProvider);

            AIL_set_3D_orientation(hListener, vOrientation.m_fX,
                                   vOrientation.m_fY, vOrientation.m_fZ * -1.0f,
                                   vUp.m_fX, vUp.m_fY, vUp.m_fZ * -1.0f);
            AIL_close_3D_listener(hListener);
        }
    }
}
#endif

#ifndef _XBOX
void CExoSoundInternal::SetListenerPosition(const Vector& vPosition)  // PC 0x005D6600  Xbox 0x0014FB80
{
    if (equal(m_vListenerPosition, vPosition) == 0)
    {
        m_vListenerPosition = vPosition;
        if (m_h3DProvider != NULL)
        {
            H3DPOBJECT hListener = AIL_open_3D_listener(m_h3DProvider);

            if (hListener != NULL)
            {
                AIL_set_3D_position(hListener, vPosition.m_fX, vPosition.m_fY,
                                    vPosition.m_fZ * -1.0f);
                AIL_close_3D_listener(hListener);
            }
        }
    }
}
#endif
// PC 0x005D6670
inline CResRef& CExoStreamingSoundSourceInternal::GetResRef()
{
    return m_resRef;
}
// PC 0x005D6680
inline int CExoStreamingSoundSourceInternal::GetResourceType()
{
    return m_nResourceType;
}
// PC 0x005D6690
inline unsigned char CExoSoundPriorityGroup::GetMaxPlaying()
{
    return m_nMaxPlaying;
}
// PC 0x005D66A0
inline unsigned char CExoSoundPriorityGroup::GetPlaying()
{
    return m_nPlaying;
}
// PC 0x005D66B0
inline float CExoSoundInternal::GetDesiredAmbientScale()
{
    return m_fDesiredAmbientScale;
}






#ifndef _XBOX
inline void CExoSoundInternal::SetRolloffFactor(float fFactor)
{
    if (m_h3DProvider != NULL)
    {
        AIL_set_3D_rolloff_factor(m_h3DProvider, fFactor);
    }
}
#endif

#ifndef _XBOX
CExoSoundInternal::CExoSoundInternal()  // PC 0x005D86E0  Xbox 0x00150C60
{
    m_p3DVoices = NULL;
    m_p2DVoices = NULL;
    m_h3DProvider = NULL;
    m_bInitialized = 0;
    m_pPriorityGroups = NULL;
    m_nPriorityGroups = 0;
    m_nNextSourceId = 0;
    LoadPriorityGroups();

    m_fDialogVolume = 1.0f;
    m_fSoundEffectVolume = 1.0f;
    m_fMusicVolume = 1.0f;
    m_fTempDialogVolume = 1.0f;
    m_fTempSoundEffectVolume = 1.0f;
    m_fTempMusicVolume = 1.0f;
    m_nUnMuteFadeTime = 0;
    m_nMuteFadeTime = 0;
    m_nMuteStartTime = 0;
    m_bMuted = 0;
    m_f2D3DBias = 1.0f;
    m_n3DVoices = 0;
    m_n2DVoices = 0;
    m_fReverbLevel = 0.6f;
    m_bStreamingEnvironmentEffects = 1;
    m_bNonStreamingEnvironmentEffects = 1;
    m_nCurrentEnvironment = 0;
    AIL_set_preference(DIG_MIXER_CHANNELS, 64);

    m_nBestEAXAvailable = 0;
    m_fVolumeScale = 1.0f;
    m_fVolumeScaleStart = 1.0f;
    m_fVolumeScaleTarget = 1.0f;
    m_nDesiredEnvironment = 0;
    m_fDesiredAmbientScale = 1.0f;
    m_nLastRenderTime =
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;

    m_pOcclusionPosition = NULL;
    m_nOcclusionIdleFrames = 0;
    m_nMaxOcclusionIdleFrames = 0;
    m_nMaxOcclusionTime = 0;
    m_nMaxObstructionQueries = 0;
    m_nMaxCachedObstructionFrameTime = 0;
    m_nOcclusionTime = 0;
    m_nObstructionQueries = 0;
    m_nOcclusionState = 0;
    m_nOcclusionList = OCCLUSION_LIST_PLAYING;
    m_nOcclusionTimeBudget = OCCLUSION_TIME_BUDGET;
    g_bSoundPaused = 0;
    m_anSoundMode[0] = CExoSound::CEXOSOUND_MODE_GENERIC;
    m_nSoundMode = 0;
    m_bPaused = 0;
    m_p3DOneShotStream = NULL;
}
#endif
// PC 0x005D88F0  Xbox 0x00151220
void CExoSoundInternal::OcclusionRender(unsigned long nFrameTime)
{
    CExoLinkedListNode* pStartPosition = m_pOcclusionPosition;
    int bFirstPass = 1;
    unsigned long nLastTime =
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
    CExoLinkedListNode* pPosition;
    ++m_nOcclusionIdleFrames;
    if (m_pOcclusionPosition == NULL)
    {
        m_pOcclusionPosition = m_lstPlaying.GetHeadPos();
        m_nOcclusionList = OCCLUSION_LIST_PLAYING;
    }
    if (m_pOcclusionPosition == NULL)
    {
        m_nOcclusionIdleFrames = 0;
    }
    if (m_nOcclusionTime > m_nMaxOcclusionTime)
    {
        m_nMaxOcclusionTime = m_nOcclusionTime;
    }
    if (m_nOcclusionTime < m_nOcclusionTimeBudget)
    {
    do
    {
        if (m_nOcclusionList == OCCLUSION_LIST_PLAYING)
        {
            pPosition = m_pOcclusionPosition;
            while (pPosition != NULL)
            {
                if (m_nOcclusionTime >= m_nOcclusionTimeBudget)
                {
                    break;
                }
                m_nOcclusionIdleFrames = 0;
                CExoSoundSourceInternal* pSource = m_lstPlaying.GetAtPos(pPosition);
                if (pSource != NULL && pSource->IsPlaying() &&
                    pSource->m_b3D && !pSource->m_bPlayWhenFree)
                {
                    Vector vSourcePosition = pSource->m_vPosition;
                    Vector vListenerPosition = m_vListenerPosition;
                    unsigned long nObstructionTime =
                        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
                    float fObstruction = 0.0f;
                    if (GetClosestRecentSound(vSourcePosition, vListenerPosition,
                                              nObstructionTime, fObstruction))
                    {
                        if (nFrameTime > m_nMaxCachedObstructionFrameTime)
                        {
                            m_nMaxCachedObstructionFrameTime = nFrameTime;
                        }
                        pSource->m_vObstructionListenerPosition = vListenerPosition;
                        pSource->m_nObstructionTime = nObstructionTime;
                        pSource->m_fObstruction = fObstruction;
                    }
                    else
                    {
                        pSource->m_fObstruction =
                            GetObstructionFactor(vSourcePosition, vListenerPosition);
                        pSource->m_vObstructionListenerPosition = vListenerPosition;
                        pSource->m_nObstructionTime = nObstructionTime;
                        ++m_nObstructionQueries;
                        if (m_nObstructionQueries > m_nMaxObstructionQueries)
                        {
                            m_nMaxObstructionQueries = m_nObstructionQueries;
                        }
                    }
                    g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
                    pSource->DoObstructionWork();
                }
                m_lstPlaying.GetNext(pPosition);
                unsigned long nTime =
                    g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
                m_nOcclusionTime += nTime - nLastTime;
                nLastTime = nTime;
                if (pStartPosition == pPosition)
                {
                    m_pOcclusionPosition = pPosition;
                    m_nObstructionQueries = 0;
                    goto Exit;
                }
            }
            if (pPosition != NULL)
            {
                m_pOcclusionPosition = pPosition;
            }
            else
            {
                m_nOcclusionList = OCCLUSION_LIST_STREAMING;
                m_pOcclusionPosition = m_lstStreaming.GetHeadPos();
            }
            unsigned long nTime =
                g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
            m_nOcclusionTime += nTime - nLastTime;
            nLastTime = nTime;
        }
        if (!bFirstPass && pStartPosition == m_pOcclusionPosition)
        {
            m_nObstructionQueries = 0;
            break;
        }
        if (m_nOcclusionList == OCCLUSION_LIST_STREAMING)
        {
            pPosition = m_pOcclusionPosition;
            while (pPosition != NULL)
            {
                if (m_nOcclusionTime >= m_nOcclusionTimeBudget)
                {
                    break;
                }
                m_nOcclusionIdleFrames = 0;
                CExoStreamingSoundSourceInternal* pSource =
                    m_lstStreaming.GetAtPos(pPosition);
                if (pSource != NULL && pSource->IsPlaying() && pSource->m_b3D)
                {
                    Vector vSourcePosition = pSource->m_vPosition;
                    Vector vListenerPosition = m_vListenerPosition;
                    unsigned long nObstructionTime =
                        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
                    float fObstruction = 0.0f;
                    if (GetClosestRecentSound(vSourcePosition, vListenerPosition,
                                              nObstructionTime, fObstruction))
                    {
                        if (nFrameTime > m_nMaxCachedObstructionFrameTime)
                        {
                            m_nMaxCachedObstructionFrameTime = nFrameTime;
                        }
                        pSource->m_vObstructionListenerPosition = vListenerPosition;
                        pSource->m_nObstructionTime = nObstructionTime;
                        pSource->m_fOcclusion = fObstruction;
                    }
                    else
                    {
                        pSource->m_fOcclusion =
                            GetObstructionFactor(vSourcePosition, vListenerPosition);
                        pSource->m_vObstructionListenerPosition = vListenerPosition;
                        pSource->m_nObstructionTime = nObstructionTime;
                        ++m_nObstructionQueries;
                        if (m_nObstructionQueries > m_nMaxObstructionQueries)
                        {
                            m_nMaxObstructionQueries = m_nObstructionQueries;
                        }
                    }
                    g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
                    pSource->DoObstructionWork();
                }
                m_lstStreaming.GetNext(pPosition);
                unsigned long nTime =
                    g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
                m_nOcclusionTime += nTime - nLastTime;
                nLastTime = nTime;
                if (pStartPosition == pPosition)
                {
                    m_pOcclusionPosition = pPosition;
                    m_nObstructionQueries = 0;
                    goto Exit;
                }
            }
            if (pPosition == NULL)
            {
                m_nOcclusionList = OCCLUSION_LIST_PLAYING;
                m_pOcclusionPosition = m_lstPlaying.GetHeadPos();
            }
            else
            {
                m_pOcclusionPosition = pPosition;
            }
            unsigned long nTime =
                g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
            m_nOcclusionTime += nTime - nLastTime;
            nLastTime = nTime;
        }
        m_nObstructionQueries = 0;
        if (pStartPosition == m_pOcclusionPosition)
        {
            break;
        }
        bFirstPass = 0;
    } while (m_nOcclusionTime < m_nOcclusionTimeBudget);
    }
Exit:
    m_nOcclusionTime = 0;
    if (m_nOcclusionIdleFrames > m_nMaxOcclusionIdleFrames)
    {
        m_nMaxOcclusionIdleFrames = m_nOcclusionIdleFrames;
    }
}
// PC 0x005D66C0
int CExoSoundInternal::GetDesiredEnvironment()
{
    return m_nDesiredEnvironment;
}



#ifndef _XBOX
void CExoSoundInternal::SetEnvironment(unsigned char nEnvironment)  // PC 0x005D66D0
{
    m_nCurrentEnvironment = nEnvironment;
    if (m_h3DProvider != 0)
    {
        if (nEnvironment >= 24)
        {
            if (m_bNonStreamingEnvironmentEffects)
                AIL_set_3D_room_type(m_h3DProvider, EAX_ENVIRONMENT_GENERIC);
            if (m_bStreamingEnvironmentEffects)
            {
                HDIGDRIVER hDriver;
                AIL_quick_handles(&hDriver, NULL, NULL);
                AIL_set_digital_master_room_type(hDriver, ENVIRONMENT_GENERIC);
            }
        }
        else
        {
            if (m_bNonStreamingEnvironmentEffects)
            {
                AIL_set_3D_room_type(m_h3DProvider, CEXOSOUND_PRESETS[nEnvironment].ulEnvironment);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 environment size", &CEXOSOUND_PRESETS[nEnvironment].flEnvironmentSize);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 environment diffusion", &CEXOSOUND_PRESETS[nEnvironment].flEnvironmentDiffusion);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 room", &CEXOSOUND_PRESETS[nEnvironment].lRoom);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 room HF", &CEXOSOUND_PRESETS[nEnvironment].lRoomHF);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX3 room LF ", &CEXOSOUND_PRESETS[nEnvironment].lRoomLF);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX decay time", &CEXOSOUND_PRESETS[nEnvironment].flDecayTime);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 room HF", &CEXOSOUND_PRESETS[nEnvironment].flDecayHFRatio);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX3 room LF", &CEXOSOUND_PRESETS[nEnvironment].flDecayLFRatio);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 reflections", &CEXOSOUND_PRESETS[nEnvironment].lReflections);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 reflections delay", &CEXOSOUND_PRESETS[nEnvironment].flReflectionsDelay);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 reverb", &CEXOSOUND_PRESETS[nEnvironment].lReverb);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 reverb delay", &CEXOSOUND_PRESETS[nEnvironment].flReverbDelay);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX3 echo time", &CEXOSOUND_PRESETS[nEnvironment].flEchoTime);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX3 echo depth", &CEXOSOUND_PRESETS[nEnvironment].flEchoDepth);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX3 modulation time", &CEXOSOUND_PRESETS[nEnvironment].flModulationTime);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX3 modulation depth", &CEXOSOUND_PRESETS[nEnvironment].flModulationDepth);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 air absorption", &CEXOSOUND_PRESETS[nEnvironment].flAirAbsorptionHF);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX3 HF reference", &CEXOSOUND_PRESETS[nEnvironment].flHFReference);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX3 LF reference", &CEXOSOUND_PRESETS[nEnvironment].flLFReference);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 room rolloff", &CEXOSOUND_PRESETS[nEnvironment].flRoomRolloffFactor);
                AIL_set_3D_provider_preference(m_h3DProvider,
                    "EAX2 flags", &CEXOSOUND_PRESETS[nEnvironment].ulFlags);
            }
            if (m_bStreamingEnvironmentEffects)
            {
                HDIGDRIVER hDriver;
                AIL_quick_handles(&hDriver, NULL, NULL);
                AIL_set_digital_master_room_type(hDriver,
                    CEXOSOUND_PRESETS[nEnvironment].ulEnvironment);
            }
        }
    }
}
#endif









#ifndef _XBOX
int ApplySpeakerSettings(int nSpeakerType, int bReleaseDevice)
{
    CExoSoundInternal* pInternal = g_pExoSound->m_pInternal;
    int nResult = 0;
    if (pInternal->m_h3DProvider != 0)
    {
        if (AIL_3D_speaker_type(pInternal->m_h3DProvider) != nSpeakerType)
        {
            AIL_set_3D_speaker_type(pInternal->m_h3DProvider, nSpeakerType);
        }
        nResult = AIL_3D_speaker_type(pInternal->m_h3DProvider);
    }
    HDIGDRIVER hDriver;
    AIL_quick_handles(&hDriver, NULL, NULL);
    if (bReleaseDevice)
    {
        AIL_digital_handle_release(hDriver);
    }
    else
    {
        AIL_digital_handle_reacquire(hDriver);
    }
    return nResult;
}
#endif

#ifndef _XBOX
void GetListenerFactors(float* pfRolloffFactor, float* pfDistanceFactor)
{
    CExoSoundInternal* pInternal = g_pExoSound->m_pInternal;
    if (pInternal->m_h3DProvider != 0)
    {
        *pfRolloffFactor = AIL_3D_rolloff_factor(pInternal->m_h3DProvider);
        *pfDistanceFactor = AIL_3D_distance_factor(pInternal->m_h3DProvider);
    }
}
#endif
// PC 0x005D6940  Xbox 0x001507E0
void CExoSoundInternal::SetMusicVolume(float fVolume, int nTemporary,
                                      int bFade)
{
    if (nTemporary == 0)
    {
        m_fMusicVolume = fVolume;
    }
    m_fTempMusicVolume = fVolume;
    if (bFade == 1)
    {
        CExoSoundPriorityGroup* pGroup =
            m_pPriorityGroups + CExoSoundPriorityGroup::MUSIC_GROUP;

        if (pGroup != NULL)
        {
            pGroup->m_nVolume =
                (unsigned char)min(127.0f, max(0.0f, fVolume * 127.0f));
        }
    }
}
// PC 0x005D69D0  Xbox 0x0014EFF0
void CExoSoundInternal::SetSoundEffectVolume(float fVolume, int nTemporary)
{
    if (nTemporary == 0)
    {
        m_fSoundEffectVolume = fVolume;
    }
    m_fTempSoundEffectVolume = fVolume;
}
// PC 0x005D69F0  Xbox 0x0014F010
void CExoSoundInternal::SetDialogVolume(float fVolume, int nTemporary)
{
    if (nTemporary == 0)
    {
        m_fDialogVolume = fVolume;
    }
    m_fTempDialogVolume = fVolume;
}

#ifndef _XBOX
void CExoSoundInternal::MuteSound()  // PC 0x005D6A10
{
    if (m_bMuted == 0)
    {
        m_nUnMuteFadeTime = 0;
        m_bMuted = 1;
        m_nMuteFadeTime = 500;
        m_nMuteStartTime =
            (unsigned long)g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
            1000;
    }
}
#endif

#ifndef _XBOX
void CExoSoundInternal::UnMuteSound()  // PC 0x005D6A50
{
    if (m_bMuted != 0)
    {
        m_nMuteFadeTime = 0;
        m_bMuted = 0;
        m_nUnMuteFadeTime = 500;
        m_nMuteStartTime =
            (unsigned long)g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
            1000;
    }
}
#endif

// PC 0x006054E0
int CExoSoundInternal::GetMutable(CExoSoundSourceInternal* pSource)
{
    return 1;
}

#ifndef _XBOX
int CExoSoundInternal::GetMutable(CExoStreamingSoundSourceInternal* pSource)  // PC 0x006054E0
{
    return 1;
}
#endif
// PC 0x005D6A90  Xbox 0x00150AA0
int CExoSoundInternal::GetSliderType(CExoSoundSourceInternal* pSource)
{
    unsigned char nGroup = pSource->m_nPriorityGroup;

    if (nGroup != 8 && nGroup != 9 && nGroup != 16 && nGroup != 17 &&
        nGroup != 26)
    {
        if (nGroup != 1 && nGroup != 2)
        {
            return 2;
        }
        return 3;
    }
    return 1;
}

#ifndef _XBOX
int CExoSoundInternal::GetSliderType(CExoStreamingSoundSourceInternal* pSource)  // PC 0x005D6AD0
{
    unsigned char nGroup = pSource->m_nPriorityGroup;

    if (nGroup != 8 && nGroup != 9 && nGroup != 16 && nGroup != 17 &&
        nGroup != 26)
    {
        if (nGroup != 1 && nGroup != 2)
        {
            return 2;
        }
        return 3;
    }
    return 1;
}
#endif

#ifndef _XBOX
float CExoSoundInternal::GetSliderVolume(int nSlider, int nTemporary)  // PC 0x005D6B10  Xbox 0x00150B20
{
    switch (nSlider)
    {
    case 1:
        if (nTemporary != 0)
        {
            return m_fTempDialogVolume;
        }
        return m_fDialogVolume;
    case 2:
        if (nTemporary != 0)
        {
            return m_fTempSoundEffectVolume;
        }
        return m_fSoundEffectVolume;
    case 3:
        if (nTemporary != 0)
        {
            return m_fTempMusicVolume;
        }
        return m_fMusicVolume;
    }
    return 1.0f;
}
#endif
// PC 0x005D6B70  Xbox 0x00150BC0
int CExoSoundInternal::GetPauseable(CExoSoundSourceInternal* pSource)
{
    int nMode = m_anSoundMode[m_nSoundMode];
    unsigned char nGroup;

    if (nMode == CExoSound::CEXOSOUND_MODE_PAUSE)
    {
        nGroup = pSource->m_nPriorityGroup;
        if (nGroup == 1 || nGroup == 2 || nGroup == 4 || nGroup == 11)
        {
            return 0;
        }
    }
    else if (nMode == CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC)
    {
        nGroup = pSource->m_nPriorityGroup;
        if (nGroup == 1 || nGroup == 2 || nGroup == 11)
        {
            return 0;
        }
    }
    return 1;
}
// PC 0x005D6BC0  Xbox 0x00150C10
int CExoSoundInternal::GetPauseable(CExoStreamingSoundSourceInternal* pSource)
{
    int nMode = m_anSoundMode[m_nSoundMode];
    unsigned char nGroup;

    if (nMode == CExoSound::CEXOSOUND_MODE_PAUSE)
    {
        nGroup = pSource->m_nPriorityGroup;
        if (nGroup == 1 || nGroup == 2 || nGroup == 4 || nGroup == 11)
        {
            return 0;
        }
    }
    else if (nMode == CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC)
    {
        nGroup = pSource->m_nPriorityGroup;
        if (nGroup == 1 || nGroup == 2 || nGroup == 11)
        {
            return 0;
        }
    }
    return 1;
}

#ifndef _XBOX
void CExoSoundInternal::ShutDown()  // PC 0x005D6D30  Xbox 0x0014F230
{
    CExoLinkedListNode* pPosition = m_lstStreaming.GetHeadPos();
    CExoStreamingSoundSourceInternal* pStream = m_lstStreaming.GetHead();

    while (pPosition != NULL)
    {
        m_lstStreaming.GetNext(pPosition);
        pStream->Stop();
        if (pPosition != NULL)
        {
            pStream = m_lstStreaming.GetAtPos(pPosition);
        }
    }
    while (m_lstPlaying.GetHeadPos() != NULL)
    {
        CExoSoundSourceInternal* pSource = m_lstPlaying.GetHead();
        if (pSource->m_bPlayWhenFree == 0)
        {
            CExoString sMessage;
            pSource->m_cResRef.CopyToString(sMessage);
            sMessage.Format("CExoSoundSource %s not freed", sMessage.CStr());

#ifdef _DEBUG
            g_pExoBase->m_pExoDebug->Warning(__LINE__, __FILE__, sMessage.CStr());
#endif
        }
        delete pSource;
    }
    if (g_bDisableSound == 0)
    {
        if (m_p3DVoices != NULL)
        {
            for (int nVoice = 0; nVoice < m_n3DVoices; nVoice++)
            {
                if (m_p3DVoices[nVoice].m_hSample != NULL)
                {
                    AIL_release_3D_sample_handle(m_p3DVoices[nVoice].m_hSample);
                    m_p3DVoices[nVoice].m_hSample = NULL;
                }
            }
            delete[] m_p3DVoices;
            m_p3DVoices = NULL;
        }
        if (m_p2DVoices != NULL)
        {
            for (unsigned char nVoice = 0; nVoice < m_n2DVoices; nVoice++)
            {
                if (m_p2DVoices[nVoice].m_hSample != NULL)
                {
                    AIL_release_sample_handle(m_p2DVoices[nVoice].m_hSample);
                    m_p2DVoices[nVoice].m_hSample = NULL;
                }
            }
            delete[] m_p2DVoices;
            m_p2DVoices = NULL;
        }
    }
    if (m_h3DProvider != NULL)
    {
        AIL_close_3D_provider(m_h3DProvider);
        m_h3DProvider = NULL;
        m_bInitialized = 0;
        m_s3DProviderName = "";
    }
    AIL_quick_shutdown();
}
#endif
// PC 0x005D6EF0  Xbox 0x0014F3F0
int CExoSoundInternal::GetClosestRecentSound(
    const Vector& vPosition, Vector& vListenerPosition,
    unsigned long& nTime, float& fObstruction)
{
    Vector vSoundPosition = vPosition;
    Vector vCurrentListenerPosition = vListenerPosition;
    unsigned long nCurrentTime = nTime;
    CExoLinkedListNode* pPosition = m_lstPlaying.GetHeadPos();

    while (pPosition != NULL)
    {
        CExoSoundSourceInternal* pSource = m_lstPlaying.GetAtPos(pPosition);
        if (pSource != NULL && pSource->m_b3D != 0)
        {
            Vector vRecentSoundPosition = pSource->m_vPosition;
            Vector vRecentListenerPosition =
                pSource->m_vObstructionListenerPosition;
            unsigned long nRecentTime = pSource->m_nObstructionTime;
            if (magnitude(vSoundPosition - vRecentSoundPosition) < EXOSOUND_RECENT_SOUND_DISTANCE &&
                magnitude(vCurrentListenerPosition - vRecentListenerPosition) < EXOSOUND_RECENT_SOUND_DISTANCE &&
                nRecentTime - nCurrentTime < RECENT_SOUND_TIME)
            {
                vListenerPosition = vRecentListenerPosition;
                nTime = nRecentTime;
                fObstruction = pSource->m_fObstruction;
                return 1;
            }
        }
        m_lstPlaying.GetNext(pPosition);
    }

    pPosition = m_lstStreaming.GetHeadPos();
    while (pPosition != NULL)
    {
        CExoStreamingSoundSourceInternal* pSource =
            m_lstStreaming.GetAtPos(pPosition);
        if (pSource != NULL && pSource->m_b3D != 0)
        {
            Vector vRecentSoundPosition = pSource->m_vPosition;
            Vector vRecentListenerPosition =
                pSource->m_vObstructionListenerPosition;
            unsigned long nRecentTime = pSource->m_nObstructionTime;
            if (magnitude(vSoundPosition - vRecentSoundPosition) < EXOSOUND_RECENT_SOUND_DISTANCE &&
                magnitude(vCurrentListenerPosition - vRecentListenerPosition) < EXOSOUND_RECENT_SOUND_DISTANCE &&
                nRecentTime - nCurrentTime < RECENT_SOUND_TIME)
            {
                vListenerPosition = vRecentListenerPosition;
                nTime = nRecentTime;
                fObstruction = pSource->m_fOcclusion;
                return 1;
            }
        }
        m_lstStreaming.GetNext(pPosition);
    }
    return 0;
}

#ifndef _XBOX
void CExoSoundInternal::PlaySourceOn3DVoice(CExoSoundSourceInternal* pSource,  // PC 0x005D71C0  Xbox 0x0014F700
                                            C3DVoice* p3DVoice)
{
    int nLoopCount = pSource->m_bLooping == 0;

    p3DVoice->m_pSource = pSource;
    pSource->m_p3DVoice = p3DVoice;

    Vector vPosition = pSource->m_vPosition;
    float fMaxVolumeDist = pSource->m_fMaxVolumeDist;
    float fMinVolumeDist = pSource->m_fMinVolumeDist;

    AIL_set_3D_sample_file(p3DVoice->m_hSample,
                         pSource->m_pRes->GetData());
    AIL_set_3D_sample_loop_count(p3DVoice->m_hSample, nLoopCount);
    pSource->SetVolume(pSource->m_nVolume, 0, 1.0f);
    AIL_set_3D_sample_distances(p3DVoice->m_hSample,
                              fMinVolumeDist, fMaxVolumeDist);
    AIL_set_3D_sample_offset(p3DVoice->m_hSample,
                           pSource->m_nRestartPosition);
    AIL_set_3D_position(p3DVoice->m_hSample, vPosition.m_fX,
                        vPosition.m_fY, vPosition.m_fZ * -1.0f);

    if (pSource->m_fPitchVariance != 0.0f)
    {
        AIL_set_3D_sample_playback_rate(p3DVoice->m_hSample,
                                      pSource->m_nPitchRate);
    }
    else if (pSource->m_fFixedVariance != 0.0f)
    {
        AIL_set_3D_sample_playback_rate(p3DVoice->m_hSample,
                                      pSource->m_nFixedRate);
    }
    else
    {
        AIL_set_3D_sample_playback_rate(p3DVoice->m_hSample,
                                      pSource->m_nSampleRate);
    }

    if (m_bNonStreamingEnvironmentEffects != 0 &&
        AIL_3D_room_type(m_h3DProvider) != EAX_ENVIRONMENT_GENERIC &&
        AIL_3D_room_type(m_h3DProvider) != -1)
    {
        float fReverbLevel = m_fReverbLevel;
        AIL_set_3D_sample_effects_level(p3DVoice->m_hSample, fReverbLevel);
    }
    else
    {
        AIL_set_3D_sample_effects_level(p3DVoice->m_hSample, 0.0f);
    }

    Vector vSourcePosition = pSource->m_vPosition;
    Vector vListenerPosition = m_vListenerPosition;
    unsigned long nTime =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
                        1000);
    float fObstruction = 0.0f;

    if (GetClosestRecentSound(vSourcePosition, vListenerPosition,
                              nTime, fObstruction) != 0)
    {
        pSource->m_vObstructionListenerPosition = vListenerPosition;
        pSource->m_nObstructionTime = nTime;
        pSource->m_fObstruction = fObstruction;
    }
    else
    {
        pSource->m_fObstruction =
            GetObstructionFactor(vSourcePosition, vListenerPosition);
        pSource->m_vObstructionListenerPosition = vListenerPosition;
        pSource->m_nObstructionTime = nTime;
        ++m_nObstructionQueries;
        if (m_nObstructionQueries > m_nMaxObstructionQueries)
        {
            m_nMaxObstructionQueries = m_nObstructionQueries;
        }
    }
    pSource->DoObstructionWork();
    m_nOcclusionTime += (unsigned long)(
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000) - nTime;
    AIL_resume_3D_sample(p3DVoice->m_hSample);
}
#endif

#ifndef _XBOX
void CExoSoundInternal::PlaySourceOn2DVoice(CExoSoundSourceInternal* pSource,  // PC 0x005D7480  Xbox 0x0014F9E0
                                            C2DVoice* p2DVoice)
{
    EXOSOUND_TRACE();
    int nLoopCount = pSource->m_bLooping == 0;

    pSource->m_p2DVoice = p2DVoice;
    p2DVoice->m_pSource = pSource;

    AIL_init_sample(p2DVoice->m_hSample);
    AIL_set_sample_file(p2DVoice->m_hSample,
                        pSource->m_pRes->GetData(), 0);
    AIL_set_sample_loop_count(p2DVoice->m_hSample, nLoopCount);
    AIL_set_sample_position(p2DVoice->m_hSample,
                            pSource->m_nRestartPosition);

    if (pSource->m_fPitchVariance != 0.0f)
    {
        AIL_set_sample_playback_rate(p2DVoice->m_hSample,
                                     pSource->m_nPitchRate);
    }
    else if (pSource->m_fFixedVariance != 0.0f)
    {
        AIL_set_sample_playback_rate(p2DVoice->m_hSample,
                                     pSource->m_nFixedRate);
    }
    else
    {
        AIL_set_sample_playback_rate(p2DVoice->m_hSample,
                                     pSource->m_nSampleRate);
    }
    float fScale = m_fVolumeScale;

    pSource->SetVolume(pSource->m_nVolume, 0, fScale);
    AIL_resume_sample(p2DVoice->m_hSample);
}
#endif
// PC 0x005D7550  Xbox 0x0014FBD0
int CExoSoundInternal::PlayOneShotSound(CResRef const& sResRef,
                                        unsigned char nPriorityGroup,
                                        unsigned long nOneShotDelay,
                                        unsigned char nVolume,
                                        float fFixedVariance,
                                        float fPitchVariance)
{
    int nResult = 0;

    if (sResRef == "")
    {
        return 0;
    }

    CExoSoundSourceInternal* pSource = new CExoSoundSourceInternal(this);

    pSource->SetResRef(sResRef, 0);
    pSource->m_bPlayWhenFree = 1;
    pSource->SetPriorityGroup(nPriorityGroup);

    if (nVolume != 0)
    {
        pSource->SetVolume(nVolume, 0, 1.0f);
    }
    if (fFixedVariance != 0.0f)
    {
        pSource->SetFixedVariance(fFixedVariance);
    }
    if (fPitchVariance != 0.0f)
    {
        pSource->SetPitchVariance(fPitchVariance);
    }
    if (nOneShotDelay != 0)
    {
        pSource->SetOneShotDelay(nOneShotDelay);
    }
    else
    {
        nResult = pSource->Play(0, 0);
        if (pSource->m_bRetryPlay != 0)
        {
            delete pSource;
        }
    }
    return nResult;
}

#ifndef _XBOX
int CExoSoundInternal::Play3DOneShotSound(CResRef const& sResRef,  // PC 0x005D7680  Xbox 0x0014FD00
                                         Vector vPosition,
                                         unsigned char nPriorityGroup,
                                         unsigned long nOneShotDelay,
                                         unsigned char nVolume,
                                         float fFixedVariance,
                                         float fPitchVariance)
{
    int nResult = 0;

    if (sResRef == "")
    {
        return 0;
    }

    const char* pResRef = sResRef.GetResRefStr();
    char cPrefix = toupper(pResRef[0]);
    if ((cPrefix == 'C' || cPrefix == 'P' || cPrefix == 'N') &&
        pResRef[1] == '_')
    {
        if (m_p3DOneShotStream != NULL)
        {
            m_p3DOneShotStream->Stop();
            delete m_p3DOneShotStream;
        }
        m_p3DOneShotStream = new CExoStreamingSoundSource;
        if (m_p3DOneShotStream != NULL)
        {
            m_p3DOneShotStream->SetPriorityGroup(nPriorityGroup);
            m_p3DOneShotStream->Set3D(1);
            m_p3DOneShotStream->SetPosition(vPosition);
            if (nVolume != 0)
            {
                m_p3DOneShotStream->SetVolume(nVolume);
            }
            float fMinVolumeDist =
                m_pPriorityGroups[nPriorityGroup].m_fMinVolumeDist;
            float fMaxVolumeDist =
                m_pPriorityGroups[nPriorityGroup].m_fMaxVolumeDist;
            m_p3DOneShotStream->SetDistance(fMaxVolumeDist, fMinVolumeDist);
            nResult = m_p3DOneShotStream->Play(sResRef, RESTYPE_WAV, 0, 0, 0);
        }
        return nResult;
    }
    else
    {
        CExoSoundSourceInternal* pSource = new CExoSoundSourceInternal(this);
        pSource->SetResRef(sResRef, 0);
        pSource->m_bPlayWhenFree = 1;
        pSource->Set3D(1);
        pSource->SetPriorityGroup(nPriorityGroup);
        pSource->SetPosition(vPosition);
        if (nVolume != 0)
        {
            pSource->SetVolume(nVolume, 0, 1.0f);
        }
        if (fFixedVariance != 0.0f)
        {
            pSource->SetFixedVariance(fFixedVariance);
        }
        if (fPitchVariance != 0.0f)
        {
            pSource->SetPitchVariance(fPitchVariance);
        }
        float fMinVolumeDist =
            m_pPriorityGroups[nPriorityGroup].m_fMinVolumeDist;
        float fMaxVolumeDist =
            m_pPriorityGroups[nPriorityGroup].m_fMaxVolumeDist;
        pSource->m_fMaxVolumeDist = fMaxVolumeDist;
        pSource->m_fMinVolumeDist = fMinVolumeDist;
        if (nOneShotDelay != 0)
        {
            pSource->SetOneShotDelay(nOneShotDelay);
        }
        else
        {
            nResult = pSource->Play(0, 0);
            if (pSource->m_bRetryPlay != 0)
            {
                delete pSource;
            }
        }
    }
    return nResult;
}
#endif

#ifndef _XBOX
CExoLinkedListNode* CExoSoundInternal::RemoveFromPlayingList(  // PC 0x005D78F0  Xbox 0x0014FE70
    CExoSoundSourceInternal* pSource)
{
    CExoLinkedListNode* pPosition = m_lstPlaying.GetHeadPos();
    CExoSoundSourceInternal* pCurrent = m_lstPlaying.GetHead();

    while (pPosition != NULL)
    {
        if (pCurrent == pSource)
        {
            if (pPosition == m_pOcclusionPosition)
            {
                m_lstPlaying.GetNext(m_pOcclusionPosition);
                if (m_pOcclusionPosition == NULL)
                {
                    m_pOcclusionPosition = m_lstStreaming.GetHeadPos();
                    m_nOcclusionList = OCCLUSION_LIST_STREAMING;
                    if (m_pOcclusionPosition == NULL)
                    {
                        m_nOcclusionList = OCCLUSION_LIST_PLAYING;
                        m_pOcclusionPosition = NULL;
                    }
                }
            }
            break;
        }
        pCurrent = m_lstPlaying.GetNext(pPosition);
    }
    pPosition = pSource->m_pListNode;
    m_lstPlaying.GetNext(pPosition);
    m_lstPlaying.Remove(pSource->m_pListNode);
    pSource->m_nListType = CExoSoundSourceInternal::LIST_NONE;
    pSource->m_pListNode = NULL;
    CExoSoundPriorityGroup* pGroup =
        m_pPriorityGroups + pSource->m_nPriorityGroup;

    pGroup->m_nPlaying = pGroup->m_nPlaying - 1;
    return pPosition;
}
#endif

#ifndef _XBOX
CExoLinkedListNode* CExoSoundInternal::RemoveFromNonPlayingLoopingList(  // PC 0x005D79C0  Xbox 0x0014FF50
    CExoSoundSourceInternal* pSource)
{
    CExoLinkedListNode* pNext = pSource->m_pListNode;

    m_lstNonPlayingLooping.GetNext(pNext);
    m_lstNonPlayingLooping.Remove(pSource->m_pListNode);
    pSource->m_nListType = CExoSoundSourceInternal::LIST_NONE;
    pSource->m_pListNode = NULL;
    return pNext;
}
#endif
// PC 0x005D7A00  Xbox 0x0014FFA0
CExoLinkedListNode* CExoSoundInternal::RemoveFromNonPlayingStreamingLoopingList(
    CExoStreamingSoundSourceInternal* pSource)
{
    CExoLinkedListNode* pPosition =
        m_lstNonPlayingStreamingLooping.GetHeadPos();
    CExoStreamingSoundSourceInternal* pCurrent =
        m_lstNonPlayingStreamingLooping.GetHead();

    while (pPosition != NULL)
    {
        if (pSource == pCurrent)
        {
            CExoLinkedListNode* pRemove = pPosition;

            m_lstNonPlayingStreamingLooping.GetNext(pPosition);
            m_lstNonPlayingStreamingLooping.Remove(pRemove);
            return pPosition;
        }
        pCurrent = m_lstNonPlayingStreamingLooping.GetNext(pPosition);
    }
    return NULL;
}

#ifndef _XBOX
CExoLinkedListNode* CExoSoundInternal::RemoveFromDelayedOneShotList(  // PC 0x005D7A70  Xbox 0x00150010
    CExoSoundSourceInternal* pSource)
{
    CExoLinkedListNode* pNext = pSource->m_pListNode;

    m_lstDelayedOneShots.GetNext(pNext);
    m_lstDelayedOneShots.Remove(pSource->m_pListNode);
    pSource->m_nListType = CExoSoundSourceInternal::LIST_NONE;
    pSource->m_pListNode = NULL;
    return pNext;
}
#endif

#ifndef _XBOX
CExoLinkedListNode* CExoSoundInternal::RemoveFromStreamingList(  // PC 0x005D7AB0  Xbox 0x00150060
    CExoStreamingSoundSourceInternal* pSource)
{
    CExoLinkedListNode* pPosition = (CExoLinkedListNode*)pSource->m_bPlaying;

    if (pPosition != NULL)
    {
        if (pPosition == m_pOcclusionPosition)
        {
            m_lstStreaming.GetNext(m_pOcclusionPosition);
            if (m_pOcclusionPosition == NULL)
            {
                m_pOcclusionPosition = m_lstPlaying.GetHeadPos();
                m_nOcclusionList = OCCLUSION_LIST_PLAYING;
                if (m_pOcclusionPosition == NULL)
                {
                    m_nOcclusionList = OCCLUSION_LIST_PLAYING;
                    m_pOcclusionPosition = NULL;
                }
            }
        }
        m_lstStreaming.GetNext(pPosition);
        m_lstStreaming.Remove((CExoLinkedListNode*)pSource->m_bPlaying);
        pSource->m_bPlaying = 0;
        CExoSoundPriorityGroup* pGroup =
            m_pPriorityGroups + pSource->m_nPriorityGroup;

        pGroup->m_nPlaying = pGroup->m_nPlaying - 1;
    }
    return pPosition;
}
#endif
// PC 0x005D7B40  Xbox 0x00150140
void CExoSoundInternal::AddToPlayingList(CExoSoundSourceInternal* pSource)
{
    pSource->m_nListType = 1;
    pSource->m_pListNode = m_lstPlaying.AddHead(pSource);
    CExoSoundPriorityGroup* pGroup =
        m_pPriorityGroups + pSource->m_nPriorityGroup;

    pGroup->m_nPlaying = pGroup->m_nPlaying + 1;
}

#ifndef _XBOX
C3DVoice* CExoSoundInternal::GetFree3DVoice(unsigned char nPriorityGroup)  // PC 0x005D8EB0  Xbox 0x001519B0
{
    unsigned char nPriority = 0;
    CExoSoundPriorityGroup* pGroup = m_pPriorityGroups + nPriorityGroup;
    int nInterrupt;
    int nVoice;

    if (pGroup == NULL)
    {
        return NULL;
    }
    for (nVoice = 0; nVoice < m_n3DVoices; nVoice++)
    {
        if (m_p3DVoices[nVoice].m_bLocked == 0)
        {
            CExoSoundSourceInternal* pVoiceSource =
                m_p3DVoices[nVoice].m_pSource;

            if (pVoiceSource == NULL)
            {
                return &m_p3DVoices[nVoice];
            }
            CExoSoundPriorityGroup* pVoiceGroup =
                m_pPriorityGroups + pVoiceSource->m_nPriorityGroup;

            if (pVoiceGroup != NULL &&
                pVoiceGroup->m_nPriority > pGroup->m_nPriority &&
                pVoiceGroup->m_nPriority > nPriority)
            {
                nInterrupt = nVoice;
                nPriority = pVoiceGroup->m_nPriority;
            }
        }
    }
    if (nPriority > 0)
    {
        CExoSoundSourceInternal* pSource = m_p3DVoices[nInterrupt].m_pSource;

        if (pSource->m_bPlayWhenFree != 0)
        {
            if (pSource->IsPlaying() != 0)
            {
                if (m_lstPendingDeletion.Contains(pSource) == 0)
                {
                    m_lstPendingDeletion.AddHead(pSource);
                }
            }
            else if (m_lstPendingDeletion.Contains(pSource) == 0)
            {
                delete pSource;
            }
        }
        else
        {
            pSource->ShutDownSource();
            if (pSource->m_bLooping != 0)
            {
                AddToNonPlayingLoopingList(pSource);
            }
        }
        return &m_p3DVoices[nInterrupt];
    }
    return NULL;
}
#endif

#ifndef _XBOX
C2DVoice* CExoSoundInternal::GetFree2DVoice(unsigned char nPriorityGroup)  // PC 0x005D8FF0  Xbox 0x00151AF0
{
    unsigned char nPriority = 0;
    CExoSoundPriorityGroup* pGroup = m_pPriorityGroups + nPriorityGroup;
    int nInterrupt;
    int nVoice;

    if (pGroup == NULL)
    {
        return NULL;
    }

    for (nVoice = 0; nVoice < (m_n2DVoices >> 1); nVoice++)
    {
        if (m_p2DVoices[nVoice].m_bLocked == 0)
        {
            CExoSoundSourceInternal* pVoiceSource =
                m_p2DVoices[nVoice].m_pSource;

            if (pVoiceSource == NULL)
            {
                return &m_p2DVoices[nVoice];
            }
            CExoSoundPriorityGroup* pVoiceGroup =
                m_pPriorityGroups + pVoiceSource->m_nPriorityGroup;

            if (pVoiceGroup != NULL &&
                pVoiceGroup->m_nPriority > pGroup->m_nPriority &&
                pVoiceGroup->m_nPriority > nPriority)
            {
                nInterrupt = nVoice;
                nPriority = pVoiceGroup->m_nPriority;
            }
        }
    }
    if (nPriority > 0)
    {
        CExoSoundSourceInternal* pSource = m_p2DVoices[nInterrupt].m_pSource;

        if (pSource->m_bPlayWhenFree != 0)
        {
            if (pSource->IsPlaying() != 0)
            {
                if (m_lstPendingDeletion.Contains(pSource) == 0)
                {
                    m_lstPendingDeletion.AddHead(pSource);
                }
            }
            else if (m_lstPendingDeletion.Contains(pSource) == 0)
            {
                delete pSource;
            }
        }
        else
        {
            pSource->ShutDownSource();
            if (pSource->m_bLooping != 0)
            {
                AddToNonPlayingLoopingList(pSource);
            }
        }
        return &m_p2DVoices[nInterrupt];
    }
    return NULL;
}
#endif
// PC 0x005D7B80  Xbox 0x00150180
CExoLinkedListNode* CExoSoundInternal::AddToNonPlayingLoopingList(
    CExoSoundSourceInternal* pSource)
{
    pSource->m_nListType = CExoSoundSourceInternal::LIST_NONPLAYING_LOOPING;
    pSource->m_pListNode = m_lstNonPlayingLooping.AddHead(pSource);
    return pSource->m_pListNode;
}
// PC 0x005D7BA0  Xbox 0x001501A0
CExoLinkedListNode* CExoSoundInternal::AddToNonPlayingStreamingLoopingList(
    CExoStreamingSoundSourceInternal* pSource)
{
    return m_lstNonPlayingStreamingLooping.AddHead(pSource);
}
// PC 0x005D7BB0  Xbox 0x001501B0
CExoLinkedListNode* CExoSoundInternal::AddToDelayedOneShotList(
    CExoSoundSourceInternal* pSource)
{
    pSource->m_nListType = 3;
    pSource->m_pListNode = m_lstDelayedOneShots.AddHead(pSource);
    return pSource->m_pListNode;
}
// PC 0x005D7BD0  Xbox 0x001501D0
void CExoSoundInternal::LoadPriorityGroups()
{
    int nValue;
    float fMaxVolumeDist;
    float fMinVolumeDist;
    float fVariance;

    C2DA* pTable = new C2DA("PriorityGroups", 0);
    if (!pTable->Load2DArray())
    {
        delete pTable;
        return;
    }
    m_nPriorityGroups = (unsigned char)pTable->GetNumRows();
    m_pPriorityGroups = new CExoSoundPriorityGroup[m_nPriorityGroups];
    for (unsigned char i = 0; i < GetNumPriorityGroups(); i++)
    {
        if (pTable->GetINTEntry(i, "Priority", &nValue))
        {
            CExoSoundPriorityGroup* pGroup = &m_pPriorityGroups[i];
            pGroup->m_nPriority = (unsigned char)nValue;
        }
        if (pTable->GetINTEntry(i, "Volume", &nValue))
        {
            CExoSoundPriorityGroup* pGroup = &m_pPriorityGroups[i];
            pGroup->m_nVolume = (unsigned char)nValue;
        }
        if (pTable->GetINTEntry(i, "MaxPlaying", &nValue))
        {
            CExoSoundPriorityGroup* pGroup = &m_pPriorityGroups[i];
            pGroup->m_nMaxPlaying = (unsigned char)nValue;
        }
        if (pTable->GetINTEntry(i, "Interrupt", &nValue))
        {
            CExoSoundPriorityGroup* pGroup = &m_pPriorityGroups[i];
            pGroup->m_bInterrupt = nValue;
        }
        if (pTable->GetINTEntry(i, "FadeTime", &nValue))
        {
            CExoSoundPriorityGroup* pGroup = &m_pPriorityGroups[i];
            pGroup->m_nFadeTime = (unsigned short)nValue;
        }

        if (pTable->GetFLOATEntry(i, "MaxVolumeDist", &fMaxVolumeDist) &&
            pTable->GetFLOATEntry(i, "MinVolumeDist", &fMinVolumeDist))
        {
            m_pPriorityGroups[i].SetVolumeDistances(fMinVolumeDist, fMaxVolumeDist);
        }
        if (pTable->GetFLOATEntry(i, "PlaybackVariance", &fVariance))
        {
            CExoSoundPriorityGroup* pGroup = &m_pPriorityGroups[i];
            pGroup->m_fPitchVariance = min(1.0f, max(0.0f, fVariance));
        }
    }
    delete pTable;
}



// PC 0x005D6C10  Xbox 0x0014F030
CExoSoundPriorityGroup::CExoSoundPriorityGroup()
{
    m_nPriority = 0;
    m_nVolume = 127;
    m_nMaxPlaying = 255;
    m_bInterrupt = 1;
    m_fMaxVolumeDist = 10.0f;
    m_fMinVolumeDist = 20.0f;
    m_nFadeTime = 0;
    m_nPlaying = 0;
    m_fPitchVariance = 0.0f;
}
// PC 0x005D7FE0  Xbox 0x00150610
void CExoSoundInternal::InterruptPlayingSound(unsigned char nPriorityGroup)
{
    CExoLinkedListNode* pPosition;
    CExoSoundSourceInternal* pSource;
    void* pData;

    pPosition = m_lstPlaying.GetHeadPos();
    pData = m_lstPlaying.GetHead();
    pSource = (CExoSoundSourceInternal*)pData;

    while (pPosition != NULL)
    {
        if (pSource->m_nPriorityGroup == nPriorityGroup)
        {
            if (pSource->m_bPlayWhenFree != 0)
            {
                if (pSource->IsPlaying() != 0)
                {
                    if (m_lstPendingDeletion.Contains(pSource) == 0)
                    {
                        m_lstPendingDeletion.AddHead(pSource);
                        return;
                    }
                }
                else if (m_lstPendingDeletion.Contains(pSource) == 0)
                {
                    delete pSource;
                }
            }
            else
            {
                pSource->ShutDownSource();
                if (pSource->m_bLooping != 0)
                {
                    AddToNonPlayingLoopingList(pSource);
                }
            }
            return;
        }
        pSource = m_lstPlaying.GetNext(pPosition);
    }
}

#ifndef _XBOX
void CExoSoundInternal::InterruptPlayingStream(unsigned char nPriorityGroup)  // PC 0x005D80A0
{
    CExoLinkedListNode* pPosition;
    CExoStreamingSoundSourceInternal* pCurrent;
    void* pData;

    pPosition = m_lstStreaming.GetHeadPos();
    pData = m_lstStreaming.GetHead();
    pCurrent = (CExoStreamingSoundSourceInternal*)pData;

    while (pPosition != NULL)
    {
        if (pCurrent->m_nPriorityGroup == nPriorityGroup)
        {
            pCurrent->Stop();
            if (pCurrent->m_bLooping != 0)
            {
                m_lstNonPlayingStreamingLooping.AddHead(pCurrent);
            }
            break;
        }
        pCurrent = m_lstStreaming.GetNext(pPosition);
    }
}
#endif

#ifndef _XBOX
void CExoSoundInternal::ApplyNewVolume(int nTemporary)  // PC 0x005D8110  Xbox 0x00150860
{
    CExoLinkedListNode* pPosition;
    CExoStreamingSoundSourceInternal* pStream;
    CExoSoundSourceInternal* pSource;
    void* pData;

    pPosition = m_lstStreaming.GetHeadPos();
    pStream = m_lstStreaming.GetHead();
    while (pPosition != NULL)
    {
        if (pStream != NULL)
        {
            pStream->SetVolume(pStream->m_nVolume, nTemporary, 1.0f);
        }
        pStream = m_lstStreaming.GetNext(pPosition);
    }
    pPosition = m_lstPlaying.GetHeadPos();
    pData = m_lstPlaying.GetHead();
    pSource = (CExoSoundSourceInternal*)pData;
    while (pPosition != NULL)
    {
        if (pSource != NULL)
        {
            pSource->SetVolume(pSource->m_nVolume, nTemporary, 1.0f);
            if (pSource->m_b3D != 0)
            {
                if (m_bNonStreamingEnvironmentEffects != 0 &&
                    AIL_3D_room_type(m_h3DProvider) != EAX_ENVIRONMENT_GENERIC &&
                    AIL_3D_room_type(m_h3DProvider) != -1)
                {
                    float fReverbLevel = m_fReverbLevel;

                    AIL_set_3D_sample_effects_level(
                        pSource->m_p3DVoice->m_hSample, fReverbLevel);
                }
                else
                {
                    AIL_set_3D_sample_effects_level(
                        pSource->m_p3DVoice->m_hSample, 0.0f);
                }
            }
        }
        pSource = m_lstPlaying.GetNext(pPosition);
    }
}
#endif
// PC 0x005D8220
void CExoSoundInternal::StopAllOneShots()
{
    CExoLinkedListNode* pPosition = m_lstPlaying.GetHeadPos();
    void* pData = m_lstPlaying.GetHead();
    CExoSoundSourceInternal* pSource = (CExoSoundSourceInternal*)pData;

    while (pPosition != NULL)
    {
        if (pSource != NULL)
        {
            if (pSource->m_bPlayWhenFree != 0)
            {
                pPosition = RemoveFromPlayingList(pSource);
                if (pSource->IsPlaying() != 0)
                {
                    if (m_lstPendingDeletion.Contains(pSource) == 0)
                    {
                        m_lstPendingDeletion.AddHead(pSource);
                    }
                }
                else if (m_lstPendingDeletion.Contains(pSource) == 0)
                {
                    delete pSource;
                }
                if (pPosition != NULL)
                {
                    pSource = m_lstPlaying.GetAtPos(pPosition);
                }
                else
                {
                    pSource = NULL;
                }
            }
            else
            {
                pSource = m_lstPlaying.GetNext(pPosition);
            }
        }
    }
}

#ifndef _XBOX
int CExoSoundInternal::PauseAllSounds(int bPauseAll)  // PC 0x005D82C0
{
    CExoLinkedListNode* pPosition;
    CExoStreamingSoundSourceInternal* pStream;
    CExoSoundSourceInternal* pSource;
    void* pData;

    g_bSoundPaused = 1;
    pPosition = m_lstStreaming.GetHeadPos();
    pData = m_lstStreaming.GetHead();
    pStream = (CExoStreamingSoundSourceInternal*)pData;
    while (pPosition != NULL)
    {
        if (bPauseAll != 0 || GetPauseable(pStream) != 0)
        {
            pStream->Pause();
        }
        pStream = m_lstStreaming.GetNext(pPosition);
    }
    pPosition = m_lstPlaying.GetHeadPos();
    pData = m_lstPlaying.GetHead();
    pSource = (CExoSoundSourceInternal*)pData;
    while (pPosition != NULL)
    {
        if (pSource->IsPaused() == 0 && GetPauseable(pSource) != 0)
        {
            pSource->Pause();
        }
        pSource = m_lstPlaying.GetNext(pPosition);
    }
    return 1;
}
#endif
// PC 0x005D83F0  Xbox 0x00150900
int CExoSoundInternal::ResumeAllSounds()
{
    CExoLinkedListNode* pPosition;
    CExoStreamingSoundSourceInternal* pStream;
    CExoSoundSourceInternal* pSource;

    g_bSoundPaused = 0;
    pPosition = m_lstStreaming.GetHeadPos();
    pStream = m_lstStreaming.GetHead();
    while (pPosition != NULL)
    {
        pStream->Resume();
        pStream = m_lstStreaming.GetNext(pPosition);
    }
    pPosition = m_lstPlaying.GetHeadPos();
    pSource = m_lstPlaying.GetHead();
    while (pPosition != NULL)
    {
        pSource->Resume();
        pSource = m_lstPlaying.GetNext(pPosition);
    }
    return 1;
}

#ifndef _XBOX
int CExoSoundInternal::AddToStreamingList(  // PC 0x005D8470
    CExoStreamingSoundSourceInternal* pSource)
{
    unsigned int nMaximum = m_n2DVoices - (m_n2DVoices >> 1);

    if (m_lstStreaming.Count() >= nMaximum)
    {
        CExoSoundPriorityGroup* pSourceGroup =
            m_pPriorityGroups + pSource->m_nPriorityGroup;
        CExoStreamingSoundSourceInternal* pInterruptSource = NULL;
        unsigned char nPriority = 0;

        if (pSourceGroup != NULL)
        {
            CExoLinkedListNode* pPosition = m_lstStreaming.GetHeadPos();
            CExoStreamingSoundSourceInternal* pCurrent =
                m_lstStreaming.GetHead();
            while (pPosition != NULL)
            {
                if (pCurrent->m_bLooping == 0)
                {
                    pCurrent =
                        (CExoStreamingSoundSourceInternal*)
                            m_lstStreaming.GetNext(pPosition);
                    continue;
                }
                else
                {
                    CExoSoundPriorityGroup* pCurrentGroup =
                        m_pPriorityGroups + pCurrent->m_nPriorityGroup;

                    if (pCurrentGroup != NULL &&
                        pCurrentGroup->m_nPriority >
                            pSourceGroup->m_nPriority &&
                        pCurrentGroup->m_nPriority > nPriority)
                    {
                        pInterruptSource = pCurrent;
                        nPriority = pCurrentGroup->m_nPriority;
                    }
                }
                pCurrent = (CExoStreamingSoundSourceInternal*)
                               m_lstStreaming.GetNext(pPosition);
            }
            if (pInterruptSource != NULL)
            {
                pInterruptSource->Stop();
                if (pInterruptSource->m_bLooping != 0)
                {
                    m_lstNonPlayingStreamingLooping.AddHead(pInterruptSource);
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    pSource->m_bPlaying = (int)m_lstStreaming.AddHead(pSource);
    CExoSoundPriorityGroup* pGroup =
        m_pPriorityGroups + pSource->m_nPriorityGroup;

    pGroup->m_nPlaying = pGroup->m_nPlaying + 1;
    return 1;
}
#endif

#ifndef _XBOX
void CExoSoundInternal::SetSoundMode(int nMode)  // PC 0x005D8560  Xbox 0x00151DB0
{
    if (nMode == CExoSound::CEXOSOUND_MODE_LOSTFOCUS ||
        nMode == CExoSound::CEXOSOUND_MODE_GAINFOCUS)
    {
        if (nMode == CExoSound::CEXOSOUND_MODE_LOSTFOCUS)
        {
            PauseAllSounds(1);
            m_bPaused = 1;
        }
        else
        {
            m_bPaused = 0;
            ResumeAllSounds();
            nMode = m_anSoundMode[m_nSoundMode];
            if (m_nSoundMode != 0)
            {
                m_nSoundMode--;
                SetSoundMode(nMode);
            }
        }
    }
    else
    {
        if (nMode != CExoSound::CEXOSOUND_MODE_GENERIC &&
            nMode == m_anSoundMode[m_nSoundMode])
        {
            return;
        }

        if (m_anSoundMode[m_nSoundMode] == CExoSound::CEXOSOUND_MODE_MUTE)
        {
            UnMuteSound();
        }
        if (m_anSoundMode[m_nSoundMode] == CExoSound::CEXOSOUND_MODE_PAUSE ||
            m_anSoundMode[m_nSoundMode] == CExoSound::CEXOSOUND_MODE_PAUSE_ALL ||
            m_anSoundMode[m_nSoundMode] ==
                CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC)
        {
            ResumeAllSounds();
        }

        if (nMode == CExoSound::CEXOSOUND_MODE_GENERIC)
        {
            m_nSoundMode--;
            if (m_nSoundMode < 0)
            {
                m_nSoundMode = 0;
            }
            nMode = m_anSoundMode[m_nSoundMode];
        }
        else
        {
            m_nSoundMode++;
            if (m_nSoundMode > 9)
            {
                m_nSoundMode = 9;
            }
            m_anSoundMode[m_nSoundMode] = nMode;
        }

        if (nMode == CExoSound::CEXOSOUND_MODE_MUTE)
        {
            MuteSound();
        }
        if (nMode == CExoSound::CEXOSOUND_MODE_PAUSE ||
            nMode == CExoSound::CEXOSOUND_MODE_PAUSE_ALL ||
            nMode == CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC)
        {
            PauseAllSounds(0);
        }
    }
}
#endif

#ifndef _XBOX
void CExoSoundInternal::LoadProviderPreferences()  // PC 0x005D9140
{
    C2DA* pTable = new C2DA("SoundProvider", 0);
    if (pTable->Load2DArray() == 0)
    {
        delete pTable;
        return;
    }

    CExoString sProvider;
    for (unsigned char nProvider = 0;
         nProvider < m_CExoStringList_D8.GetCount(); nProvider++)
    {
        for (unsigned char nRow = 0; nRow < pTable->m_nNumRows; nRow++)
        {
            if (pTable->GetCExoStringEntry(nRow, "Provider", &sProvider))
            {
                if (!(*m_CExoStringList_D8.GetString(nProvider) == sProvider))
                {
                    continue;
                }
                CAllowedSoundProvider* pConfiguration =
                    new CAllowedSoundProvider;
                pConfiguration->m_f2D3DBias = 1.0f;
                pConfiguration->m_nProvider = nProvider;
                pTable->GetINTEntry(nRow, "EAX", &pConfiguration->m_nEAX);
                pTable->GetINTEntry(nRow, "Hardware", &pConfiguration->m_bHardware);
                pTable->GetFLOATEntry(nRow, "2D3DBias", &pConfiguration->m_f2D3DBias);
                m_aProviderConfigurations.Add(pConfiguration);
                m_nBestEAXAvailable = max(m_nBestEAXAvailable, pConfiguration->m_nEAX);
                break;
            }
        }
    }
    pTable->Unload2DArray();
    delete pTable;
}
#endif

#ifndef _XBOX
CExoSoundInternal::~CExoSoundInternal()  // PC 0x005D9400  Xbox 0x00150E90
{
    ShutDown();
    if (m_pPriorityGroups != NULL)
    {
        delete[] m_pPriorityGroups;
        m_pPriorityGroups = NULL;
    }

    m_aProviderConfigurations.num = 0;
    if (m_aProviderConfigurations.element != NULL)
    {
        delete[] m_aProviderConfigurations.element;
        m_aProviderConfigurations.element = NULL;
        m_aProviderConfigurations.array_size = 0;
    }
    m_aProviderConfigurations.num = 0;
}
#endif




#ifndef _XBOX
#pragma optimize("", off)
#endif






#ifndef _XBOX
void CExoSoundInternal::Render()  // PC 0x005D9590  Xbox 0x00151F00
{
    unsigned long nRenderMS = (unsigned long)
        (g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    unsigned long nRenderDeltaMS = nRenderMS - m_nLastRenderTime;
    m_nLastRenderTime = nRenderMS;
    float fElapsedSec = min((float)nRenderDeltaMS / 1000.0f, 0.1f);
    int nRequestedEnvironment = GetDesiredEnvironment();
    float fAmbientScaleDesired = GetDesiredAmbientScale();

    if (m_nCurrentEnvironment != nRequestedEnvironment)
    {
        SetEnvironment((unsigned char)nRequestedEnvironment);
    }
    if (fAmbientScaleDesired != m_fVolumeScale)
    {
        m_fVolumeScale += (fAmbientScaleDesired - m_fVolumeScaleStart) *
            (fElapsedSec / 1.0f);
        if (fAmbientScaleDesired > m_fVolumeScaleStart)
        {
            m_fVolumeScale = min(fAmbientScaleDesired, m_fVolumeScale);
        }
        if (fAmbientScaleDesired < m_fVolumeScaleStart)
        {
            m_fVolumeScale = max(fAmbientScaleDesired, m_fVolumeScale);
        }
    }
    else
    {
        m_fVolumeScaleStart = m_fVolumeScale;
    }
    if (m_fVolumeScaleTarget != fAmbientScaleDesired)
    {
        m_fVolumeScaleStart = m_fVolumeScale;
    }
    m_fVolumeScaleTarget = fAmbientScaleDesired;


    int bRemoved = 0;
    CExoLinkedListNode* pPos = m_lstStreaming.GetHeadPos();
    CExoStreamingSoundSourceInternal* pStreamingSoundSource =
        m_lstStreaming.GetHead();
    while (pPos != NULL)
    {
        if (!pStreamingSoundSource->IsPlaying() && !g_bSoundPaused)
        {
            RemoveFromStreamingList(pStreamingSoundSource);
            break;
        }
        pStreamingSoundSource = m_lstStreaming.GetNext(pPos);
    }

    pPos = m_lstStreaming.GetHeadPos();
    pStreamingSoundSource = m_lstStreaming.GetHead();
    while (pPos != NULL)
    {
        if (pStreamingSoundSource->Get3D() &&
            pStreamingSoundSource->GetLooping() &&
            !pStreamingSoundSource->GetWithinRangeOfListener())
        {
            m_lstStreaming.GetNext(pPos);
            pStreamingSoundSource->Stop();
            AddToNonPlayingStreamingLoopingList(pStreamingSoundSource);
            if (pPos != NULL)
            {
                pStreamingSoundSource = m_lstStreaming.GetAtPos(pPos);
            }
            continue;
        }
        if (pStreamingSoundSource->Get3D())
        {
            pStreamingSoundSource->Update3DStream();
        }
        pStreamingSoundSource = m_lstStreaming.GetNext(pPos);
    }

    pPos = m_lstNonPlayingStreamingLooping.GetHeadPos();
    pStreamingSoundSource = m_lstNonPlayingStreamingLooping.GetHead();
    CExoSoundPriorityGroup* pGroup = NULL;
    while (pPos != NULL)
    {
        pGroup = GetPriorityGroup(pStreamingSoundSource->GetPriorityGroup());
        if (m_lstStreaming.Count() >=
                GetNumber2DVoices() - GetNumber2DVoices() / 2 ||
            (pGroup != NULL && pGroup->GetPlaying() >= pGroup->GetMaxPlaying()))
        {
            pStreamingSoundSource =
                m_lstNonPlayingStreamingLooping.GetNext(pPos);
            continue;
        }
        if (pStreamingSoundSource->GetLooping() &&
            pStreamingSoundSource->GetWithinRangeOfListener())
        {
            pStreamingSoundSource->Update3DStream();
            pStreamingSoundSource->Play(pStreamingSoundSource->GetResRef(),
                pStreamingSoundSource->GetResourceType(), 0, 0, 0);
            pPos = RemoveFromNonPlayingStreamingLoopingList(
                pStreamingSoundSource);
            if (pPos != NULL)
            {
                pStreamingSoundSource =
                    m_lstNonPlayingStreamingLooping.GetAtPos(pPos);
            }
            continue;
        }
        pStreamingSoundSource = m_lstNonPlayingStreamingLooping.GetNext(pPos);
    }

    if (m_h3DProvider == NULL)
    {
        return;
    }
    unsigned long nTimeStamp = (unsigned long)
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
    unsigned long nElapsed;
    unsigned long nOneShotDelayMS;
    unsigned long nStartMS;
    unsigned long nFadeTimeLengthMS;
    unsigned long nFadeStart;
    if (m_nMuteFadeTime != 0)
    {
        nElapsed = nTimeStamp - m_nMuteStartTime;
        if (nElapsed > 0)
        {
            float fFade = min(1.0f, max(0.0f,
                (float)nElapsed / (unsigned long)m_nMuteFadeTime));
            float fDialogVolume = GetDialogVolume(0);
            float fSoundEffectVolume = GetSoundEffectVolume(0);
            float fMusicVolume = GetMusicVolume(0);
            SetDialogVolume(fDialogVolume - fFade * fDialogVolume, 1);
            SetSoundEffectVolume(
                fSoundEffectVolume - fFade * fSoundEffectVolume, 1);
            SetMusicVolume(fMusicVolume - fFade * fMusicVolume, 1, 0);
            ApplyNewVolume(1);
            if (fFade == 1.0f)
            {
                m_nMuteFadeTime = 0;
                m_nMuteStartTime = 0;
                pPos = m_lstStreaming.GetHeadPos();
                pStreamingSoundSource = m_lstStreaming.GetHead();
                while (pPos != NULL)
                {
                    if (pStreamingSoundSource->GetFadeDuration() != 0)
                    {
                        pPos = RemoveFromStreamingList(pStreamingSoundSource);
                        pStreamingSoundSource->Stop();
                        if (pPos != NULL)
                        {
                            pStreamingSoundSource =
                                m_lstStreaming.GetAtPos(pPos);
                        }
                    }
                    else
                    {
                        pStreamingSoundSource = m_lstStreaming.GetNext(pPos);
                    }
                }
            }
        }
    }
    if (m_nUnMuteFadeTime != 0)
    {
        nElapsed = nTimeStamp - m_nMuteStartTime;
        if (nElapsed > 0)
        {
            float fFade = min(1.0f, max(0.0f,
                (float)nElapsed / (unsigned long)m_nUnMuteFadeTime));
            float fDialogVolume = GetDialogVolume(0);
            float fSoundEffectVolume = GetSoundEffectVolume(0);
            float fMusicVolume = GetMusicVolume(0);
            SetDialogVolume(fFade * fDialogVolume, 1);
            SetSoundEffectVolume(fFade * fSoundEffectVolume, 1);
            SetMusicVolume(fFade * fMusicVolume, 1, 0);
            ApplyNewVolume(1);
            if (fFade == 1.0f)
            {
                m_nUnMuteFadeTime = 0;
                m_nMuteStartTime = 0;
            }
        }
    }

    nTimeStamp = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
    pPos = m_lstDelayedOneShots.GetHeadPos();
    CExoSoundSourceInternal* pSoundSource = m_lstDelayedOneShots.GetHead();
    while (pPos != NULL)
    {
        nOneShotDelayMS = pSoundSource->GetOneShotDelay();
        nStartMS = pSoundSource->GetDelayStart();
        if (nTimeStamp >= nStartMS)
        {
            nElapsed = nTimeStamp - nStartMS;
        }
        else
        {
            nElapsed = 0xffffffff - nStartMS + nTimeStamp;
        }
        if (nElapsed != 0)
        {
            if (nElapsed >= nOneShotDelayMS)
            {
                pPos = RemoveFromDelayedOneShotList(pSoundSource);
                pSoundSource->Play(0, 0);
                if (pSoundSource->GetRetryPlay() != 0)
                {
                    delete pSoundSource;
                    pSoundSource = NULL;
                }
                if (pPos != NULL)
                {

                    pSoundSource = m_lstPlaying.GetAtPos(pPos);
                }
            }
            else
            {
                pSoundSource = m_lstDelayedOneShots.GetNext(pPos);
            }
        }
        else
        {
            pSoundSource = m_lstDelayedOneShots.GetNext(pPos);
        }
    }

    if (!m_bMuted)
    {
        pPos = m_lstStreaming.GetHeadPos();
        pStreamingSoundSource = m_lstStreaming.GetHead();
        while (pPos != NULL)
        {
            if (m_fVolumeScale != m_fVolumeScaleStart)
            {
                pStreamingSoundSource->SetVolume(
                    pStreamingSoundSource->GetVolume(), 0, m_fVolumeScale);
            }
            nFadeTimeLengthMS = pStreamingSoundSource->GetFadeDuration();
            nFadeStart = pStreamingSoundSource->GetFadeStartTime();
            if (nFadeTimeLengthMS != 0)
            {
                if (nTimeStamp >= nFadeStart)
                {
                    nElapsed = nTimeStamp - nFadeStart;
                }
                else
                {
                    nElapsed = 0xffffffff - nFadeStart + nTimeStamp;
                }
                if (nElapsed != 0)
                {
                    if (nElapsed >= nFadeTimeLengthMS)
                    {
                        pPos = RemoveFromStreamingList(pStreamingSoundSource);
                        pStreamingSoundSource->Stop();
                        if (pPos != NULL)
                        {
                            pStreamingSoundSource =
                                m_lstStreaming.GetAtPos(pPos);
                        }
                    }
                    else
                    {
                        unsigned char nVolume =
                            pStreamingSoundSource->GetVolume() -
                            (char)(((double)nElapsed / nFadeTimeLengthMS) *
                                pStreamingSoundSource->GetVolume());
                        pStreamingSoundSource->SetVolume(nVolume, 1, 1.0f);
                        pStreamingSoundSource = m_lstStreaming.GetNext(pPos);
                    }
                }
                else
                {
                    pStreamingSoundSource = m_lstStreaming.GetNext(pPos);
                }
            }
            else
            {
                pStreamingSoundSource = m_lstStreaming.GetNext(pPos);
            }
        }
    }

    pPos = m_lstPlaying.GetHeadPos();
    pSoundSource = m_lstPlaying.GetHead();
    while (pPos != NULL)
    {
        if (!pSoundSource->IsPlaying())
        {
            pPos = RemoveFromPlayingList(pSoundSource);
            if (pSoundSource->GetPlayWhenFree())
            {
                if (!m_lstPendingDeletion.Contains(pSoundSource))
                {
                    delete pSoundSource;
                }
                pSoundSource = NULL;
            }
            else if (!pSoundSource->IsPaused())
            {
                pSoundSource->ShutDownSource();
            }
            if (pPos != NULL)
            {
                pSoundSource = m_lstPlaying.GetAtPos(pPos);
            }
        }
        else if (!pSoundSource->GetWithinRangeOfListener())
        {
            pPos = RemoveFromPlayingList(pSoundSource);
            if (pSoundSource->GetPlayWhenFree())
            {
                if (pSoundSource->IsPlaying())
                {
                    if (!m_lstPendingDeletion.Contains(pSoundSource))
                    {
                        m_lstPendingDeletion.AddHead(pSoundSource);
                    }
                }
                else if (!m_lstPendingDeletion.Contains(pSoundSource))
                {
                    delete pSoundSource;
                }
                pSoundSource = NULL;
            }
            else
            {
                pSoundSource->ShutDownSource();
                if (pSoundSource->GetLooping())
                {
                    AddToNonPlayingLoopingList(pSoundSource);
                }
            }
            if (pPos != NULL)
            {
                pSoundSource = m_lstPlaying.GetAtPos(pPos);
            }
        }
        else
        {
            if (m_fVolumeScale != m_fVolumeScaleStart)
            {
                pSoundSource->SetVolume(
                    pSoundSource->GetVolume(), 0, m_fVolumeScale);
            }
            pSoundSource = m_lstPlaying.GetNext(pPos);
        }
    }

    pPos = m_lstNonPlayingLooping.GetHeadPos();
    pSoundSource = m_lstNonPlayingLooping.GetHead();
    while (pPos != NULL)
    {
        m_lstNonPlayingLooping.GetNext(pPos);
        pSoundSource->Play(1, 0);
        if (pPos != NULL)
        {
            pSoundSource = m_lstNonPlayingLooping.GetAtPos(pPos);
        }
    }

    Vector vListenerPositionSaved = m_vListenerPosition;
    Vector vListenerFacingSaved = m_vListenerOrientation;
    Vector vListenerUpVectorSaved = m_vListenerUp;
    m_vListenerPosition = m_vListenerOrientation = m_vListenerUp =
        Vector(0.0f, 0.0f, 0.0f);
    SetListenerPosition(vListenerPositionSaved);
    SetListenerOrientation(vListenerFacingSaved, vListenerUpVectorSaved);

    pPos = m_lstPendingDeletion.GetHeadPos();
    while (pPos != NULL)
    {
        pSoundSource = m_lstPendingDeletion.GetAtPos(pPos);
        if (!pSoundSource->IsPlaying())
        {
            CExoLinkedListNode* pDeletePosition = pPos;
            m_lstPendingDeletion.GetNext(pPos);
            m_lstPendingDeletion.Remove(pDeletePosition);
            delete pSoundSource;
        }
        else
        {
            m_lstPendingDeletion.GetNext(pPos);
        }
    }
    OcclusionRender(nRenderDeltaMS);
}
#endif

#ifndef _XBOX
#pragma optimize("", on)
#endif

#ifndef _XBOX
void CExoSoundInternal::Enumerate3DProviders()  // PC 0x005DA6D0
{
    HPROENUM hEnum = HPROENUM_FIRST;
    CExoString sFilterKey;
    CExoString sFilter;
    CExoIni cIni;
    HPROVIDER aProviders[64];
    char* aNames[64];
    int nProvider = 0;

    m_CExoStringList_D8.m_nSortMode = 1;
    m_CExoStringList_D8.Clear();

    CExoArrayList<HPROVIDER>* pProviderList = &m_a3DProviders;
    pProviderList->num = 0;

    while (AIL_enumerate_3D_providers(&hEnum, &aProviders[nProvider],
                                      &aNames[nProvider]))
    {


        int bFiltered = 0;
        int nFilter = 0;
        while (!bFiltered && nFilter != -1)
        {
            sFilterKey.Format("%s%d", "3D Provider Filter", nFilter);
            nFilter = -1;
        }

        if (!bFiltered &&
            AIL_open_3D_provider(aProviders[nProvider]) == M3D_NOERR)
        {
            AIL_close_3D_provider(aProviders[nProvider]);
            m_CExoStringList_D8.Add(new CExoString(aNames[nProvider]));
            pProviderList->Add(aProviders[nProvider]);
        }
        nProvider++;
    }

    LoadProviderPreferences();
}
#endif

#ifndef _XBOX
int CExoSoundInternal::Initialize(unsigned char n2DVoices,  // PC 0x005DA8A0
                                  unsigned char n3DVoices,
                                  int nEAXMode, int bHardware)
{
    if (g_bDisableSound == 1)
    {
        return 0;
    }

    CExoLinkedList<CExoSoundSourceInternal> lstRestart;
    CExoLinkedListNode* pPosition;
    CExoStreamingSoundSourceInternal* pStream;
    HDIGDRIVER hDriver;

    if (m_h3DProvider != NULL)
    {
        pPosition = m_lstStreaming.GetHeadPos();
        pStream = m_lstStreaming.GetHead();
        while (pPosition != NULL)
        {
            if (pStream->IsPlaying())
            {
                pStream->Pause();
            }
            if (pStream->m_hStream != NULL)
            {
                AIL_close_stream(pStream->m_hStream);
                pStream->m_hStream = NULL;
            }
            pStream = m_lstStreaming.GetNext(pPosition);
        }

        CExoLinkedListNode* pPlayingPosition = m_lstPlaying.GetHeadPos();
        CExoSoundSourceInternal* pSource = m_lstPlaying.GetHead();
        while (pPlayingPosition != NULL)
        {
            if (pSource != NULL)
            {
                pPlayingPosition = RemoveFromPlayingList(pSource);
                if (!pSource->IsPlaying())
                {
                    if (pSource->m_bPlayWhenFree)
                    {
                        if (!m_lstPendingDeletion.Contains(pSource))
                        {
                            delete pSource;
                        }
                    }
                    else
                    {
                        pSource->ShutDownSource();
                    }
                }
                else
                {
                    pSource->StoreRestartSoundPosition();
                    pSource->ShutDownSource();
                    lstRestart.AddHead(pSource);
                }
                if (pPlayingPosition != NULL)
                {
                    pSource = m_lstPlaying.GetAtPos(pPlayingPosition);
                }
            }
        }

        if (m_p3DVoices != NULL)
        {
            for (int nVoice = 0; nVoice < m_n3DVoices; ++nVoice)
            {
                if (m_p3DVoices[nVoice].m_hSample != NULL)
                {
                    AIL_end_3D_sample(m_p3DVoices[nVoice].m_hSample);
                    AIL_release_3D_sample_handle(m_p3DVoices[nVoice].m_hSample);
                    m_p3DVoices[nVoice].m_hSample = NULL;
                }
            }
            delete[] m_p3DVoices;
            m_p3DVoices = NULL;
        }
        if (m_p2DVoices != NULL)
        {
            for (int nVoice = 0; nVoice < m_n2DVoices; ++nVoice)
            {
                if (m_p2DVoices[nVoice].m_hSample != NULL)
                {
                    AIL_end_sample(m_p2DVoices[nVoice].m_hSample);
                    AIL_release_sample_handle(m_p2DVoices[nVoice].m_hSample);
                    m_p2DVoices[nVoice].m_hSample = NULL;
                }
            }
            delete[] m_p2DVoices;
            m_p2DVoices = NULL;
        }
        AIL_close_3D_provider(m_h3DProvider);
        m_h3DProvider = NULL;
    }
    else
    {
        AIL_set_redist_directory("Miles");
        int bStarted = AIL_quick_startup(1, 0, 44100, 16, 2);
        if (bStarted == 0)
        {
            g_bDisableSound = 1;
        }
        Enumerate3DProviders();
    }

    m_h3DProvider = NULL;
    m_bInitialized = 0;
    m_s3DProviderName = "";
    if (bHardware == 0)
        nEAXMode = 0;
    while (m_h3DProvider == NULL)
    {
        for (int nProvider = 0; nProvider < m_aProviderConfigurations.num;
             ++nProvider)
        {
            if (m_aProviderConfigurations[nProvider]->m_bHardware == bHardware &&
                m_aProviderConfigurations[nProvider]->m_nEAX == nEAXMode)
            {
                int nIndex = m_aProviderConfigurations[nProvider]->m_nProvider;
                m_bInitialized = 1;
                m_h3DProvider = m_a3DProviders[nIndex];
                m_s3DProviderName = *m_CExoStringList_D8.GetString(nIndex);
                m_nEAX = nEAXMode;
                m_bHardware = bHardware;
                m_f2D3DBias = m_aProviderConfigurations[nProvider]->m_f2D3DBias;
            }
        }
        if (m_h3DProvider != NULL)
        {
            break;
        }
        if (nEAXMode > 0)
        {
            --nEAXMode;
        }
        else
        {
            if (!bHardware)
            {
                break;
            }
            bHardware = 0;
        }
    }

    if (m_h3DProvider != NULL && AIL_open_3D_provider(m_h3DProvider) == 0)
    {
        m_n3DVoices = n3DVoices;
        m_n3DVoices = m_n3DVoices < MAX_VOICES ? m_n3DVoices : MAX_VOICES;
        m_n3DVoices = m_n3DVoices > MIN_3D_VOICES ? m_n3DVoices : MIN_3D_VOICES;
        m_p3DVoices = new C3DVoice[m_n3DVoices];
        m_n2DVoices = n2DVoices;
        m_n2DVoices = m_n2DVoices < MAX_VOICES ? m_n2DVoices : MAX_VOICES;
        m_n2DVoices = m_n2DVoices > MIN_2D_VOICES ? m_n2DVoices : MIN_2D_VOICES;
        m_p2DVoices = new C2DVoice[m_n2DVoices];
        AIL_quick_handles(&hDriver, NULL, NULL);
        int n3DVoice = 0;
        for (int nVoice = 0;
             nVoice < (m_n3DVoices > m_n2DVoices ? m_n3DVoices : m_n2DVoices);
             ++nVoice, ++n3DVoice)
        {
            if (n3DVoice < m_n3DVoices)
            {
                m_p3DVoices[n3DVoice].m_hSample =
                    AIL_allocate_3D_sample_handle(m_h3DProvider);
                m_p3DVoices[n3DVoice].m_pSource = NULL;
                m_p3DVoices[n3DVoice].m_bLocked = 0;
                m_p3DVoices[n3DVoice].m_nState = 0;
                if (m_p3DVoices[n3DVoice].m_hSample == NULL)
                {
                    m_n3DVoices = n3DVoice;
                }
            }
            if (nVoice < m_n2DVoices)
            {
                m_p2DVoices[nVoice].m_hSample = AIL_allocate_sample_handle(hDriver);
                m_p2DVoices[nVoice].m_pSource = NULL;
                m_p2DVoices[nVoice].m_bLocked = 0;
                m_p2DVoices[nVoice].m_nState = 0;
                if (m_p2DVoices[nVoice].m_hSample != NULL)
                {
                    AIL_init_sample(m_p2DVoices[nVoice].m_hSample);
                }
                else
                {
                    m_n2DVoices = nVoice;
                }
            }
        }
        for (int nVoice = m_n2DVoices / 2; nVoice < m_n2DVoices; ++nVoice)
        {
            if (m_p2DVoices[nVoice].m_hSample != NULL)
            {
                AIL_release_sample_handle(m_p2DVoices[nVoice].m_hSample);
                m_p2DVoices[nVoice].m_hSample = NULL;
            }
        }
        SetRolloffFactor(1.0f);
        SetEnvironment(0);
        while (lstRestart.Count() != 0)
        {
            CExoSoundSourceInternal* pSource = lstRestart.RemoveHead();
            pSource->Play(1, 1);
        }
        pPosition = m_lstStreaming.GetHeadPos();
        pStream = m_lstStreaming.GetHead();
        while (pPosition != NULL)
        {
            pStream->InitializeSource();
            pStream->Resume();
            pStream = m_lstStreaming.GetNext(pPosition);
        }
        return 1;
    }
    AIL_quick_shutdown();
    m_h3DProvider = NULL;
    m_bInitialized = 0;
    m_s3DProviderName = "";
    return 0;
}
#endif

static inline void ExoSoundTrace()
{
}













#ifdef _XBOX


const DSI3DL2LISTENER CExoSoundInternal::CEXOSOUND_PRESETS[24] =
{
    { -3000, -100, 0.0f, 1.49f, 0.83f, -2602, 0.007f, 200, 0.011f, 100.0f, 100.0f, 5000.0f },
    { -3000, -6000, 0.0f, 0.17f, 0.1f, -1204, 0.001f, 207, 0.002f, 100.0f, 100.0f, 5000.0f },
    { -3000, -454, 0.0f, 0.4f, 0.83f, -1646, 0.002f, 53, 0.003f, 100.0f, 100.0f, 5000.0f },
    { -2500, -1200, 0.0f, 1.49f, 0.54f, -370, 0.007f, 1030, 0.011f, 100.0f, 60.0f, 5000.0f },
    { -3000, -6000, 0.0f, 0.5f, 0.1f, -1376, 0.003f, -1104, 0.004f, 100.0f, 100.0f, 5000.0f },
    { -2500, -300, 0.0f, 1.5f, 0.64f, -711, 0.012f, 83, 0.017f, 100.0f, 100.0f, 5000.0f },
    { -2000, -476, 0.0f, 4.32f, 0.59f, -789, 0.02f, -289, 0.03f, 100.0f, 100.0f, 5000.0f },
    { -2000, -500, 0.0f, 3.92f, 0.7f, -1230, 0.02f, -2, 0.029f, 100.0f, 100.0f, 5000.0f },
    { -2000, -1000, 0.0f, 2.91f, 1.3f, -602, 0.015f, -302, 0.022f, 100.0f, 100.0f, 5000.0f },
    { -2000, -698, 0.0f, 7.24f, 0.33f, -1166, 0.02f, 16, 0.03f, 100.0f, 100.0f, 5000.0f },
    { -2000, -1000, 0.0f, 7.0f, 0.23f, -602, 0.02f, 198, 0.03f, 100.0f, 100.0f, 5000.0f },
    { -2500, -4000, 0.0f, 0.3f, 0.1f, -1831, 0.002f, -1630, 0.03f, 100.0f, 100.0f, 5000.0f },
    { -2500, -300, 0.0f, 1.0f, 0.59f, -1219, 0.007f, 441, 0.011f, 80.0f, 100.0f, 5000.0f },
    { -2500, -1000, 0.0f, 2.7f, 0.79f, -1214, 0.013f, 0, 0.02f, 80.0f, 100.0f, 5000.0f },
    { -2500, -270, 0.0f, 1.49f, 0.86f, -1204, 0.007f, -4, 0.011f, 100.0f, 100.0f, 5000.0f },
    { -2500, -3300, 0.0f, 1.49f, 0.54f, -2560, 0.162f, -613, 0.088f, 79.0f, 100.0f, 5000.0f },
    { -2500, -800, 0.0f, 1.49f, 0.67f, -2273, 0.007f, -2217, 0.011f, 50.0f, 100.0f, 5000.0f },
    { -2000, -2500, 0.0f, 1.49f, 0.21f, -2780, 0.3f, -2014, 0.1f, 27.0f, 100.0f, 5000.0f },
    { -2000, -2000, 0.0f, 1.49f, 0.83f, -10000, 0.061f, 500, 0.025f, 100.0f, 100.0f, 5000.0f },
    { -2500, -2000, 0.0f, 1.49f, 0.5f, -2466, 0.179f, -2514, 0.1f, 21.0f, 100.0f, 5000.0f },
    { -2500, -1000, 0.0f, 1.65f, 1.5f, -1363, 0.008f, -1153, 0.012f, 100.0f, 100.0f, 5000.0f },
    { -2000, -1000, 0.0f, 2.81f, 0.14f, 429, 0.014f, 648, 0.021f, 80.0f, 60.0f, 5000.0f },
    { -2000, -4000, 0.0f, 1.49f, 0.1f, -449, 0.007f, 1700, 0.011f, 100.0f, 100.0f, 5000.0f },
    { -10000, -10000, 0.0f, 1.0f, 1.0f, -10000, 0.0f, -10000, 0.0f, 0.0f, 0.0f, 5000.0f }
};


float g_fSoundRenderTime;
#endif

#ifdef _XBOX
void CExoSoundInternal::SetListenerOrientation(const Vector& vOrientation,  // PC 0x005D6540  Xbox 0x0014FAF0
                                               const Vector& vUp)
{
    if (equal(m_vListenerOrientation, vOrientation) == 0 ||
        equal(m_vListenerUp, vUp) == 0)
    {
        m_vListenerOrientation = vOrientation;
        m_vListenerUp = vUp;
        m_pDirectSound->SetOrientation(vOrientation.m_fX, vOrientation.m_fZ, vOrientation.m_fY,
                                       vUp.m_fX, vUp.m_fZ, vUp.m_fY, DS3D_IMMEDIATE);
    }
}
#endif

#ifdef _XBOX
void CExoSoundInternal::SetListenerPosition(const Vector& vPosition)  // PC 0x005D6600  Xbox 0x0014FB80
{
    if (equal(m_vListenerPosition, vPosition) == 0)
    {
        m_vListenerPosition = vPosition;
        m_pDirectSound->SetPosition(vPosition.m_fX, vPosition.m_fZ, vPosition.m_fY, DS3D_IMMEDIATE);
    }
}
#endif

#ifdef _XBOX
// Xbox 0x001507C0
void CExoSoundInternal::SetRollOffFactor(float fFactor)
{
    m_pDirectSound->SetRolloffFactor(fFactor, DS3D_IMMEDIATE);
}




void CExoSoundInternal::SetSpeakerType(char nSpeakerType)
{
}

void CExoSoundInternal::EnableEnvironmentEffects(int bEnabled)
{
    m_bEnvironmentEffects = bEnabled;
}
#endif

#ifdef _XBOX
CExoSoundInternal::CExoSoundInternal()  // PC 0x005D86E0  Xbox 0x00150C60
{
    m_p3DVoices = NULL;
    m_p2DVoices = NULL;
    m_bInitialized = 0;
    m_pPriorityGroups = NULL;
    m_nPriorityGroups = 0;
    m_nNextSourceId = 0;
    LoadPriorityGroups();

    m_fDialogVolume = 1.0f;
    m_fSoundEffectVolume = 1.0f;
    m_fMusicVolume = 1.0f;
    m_fTempDialogVolume = 1.0f;
    m_fTempSoundEffectVolume = 1.0f;
    m_fTempMusicVolume = 1.0f;
    m_f2D3DBias = 1.0f;
    m_nUnMuteFadeTime = 0;
    m_nMuteFadeTime = 0;
    m_nMuteStartTime = 0;
    m_bMuted = 0;
    m_n3DVoices = 0;
    m_n2DVoices = 0;
    m_fReverbLevel = 0.6f;
    m_bEnvironmentEffects = 0;
    m_pStreamManager = new CExoXboxStreamManager(this);

    m_fVolumeScale = 1.0f;
    m_fVolumeScaleStart = 1.0f;
    m_fVolumeScaleTarget = 1.0f;
    m_nDesiredEnvironment = 0;
    m_fDesiredAmbientScale = 1.0f;
    m_nLastRenderTime =
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;

    m_pOcclusionPosition = NULL;
    m_nOcclusionIdleFrames = 0;
    m_nMaxOcclusionIdleFrames = 0;
    m_nMaxOcclusionTime = 0;
    m_nMaxObstructionQueries = 0;
    m_nMaxCachedObstructionFrameTime = 0;
    m_nOcclusionTime = 0;
    m_nObstructionQueries = 0;
    m_nOcclusionState = 0;
    m_nOcclusionList = OCCLUSION_LIST_PLAYING;
    m_nOcclusionTimeBudget = OCCLUSION_TIME_BUDGET;
    g_bSoundPaused = 0;
    m_anSoundMode[0] = CExoSound::CEXOSOUND_MODE_GENERIC;
    m_nSoundMode = 0;
}
#endif

#ifdef _XBOX
inline void CExoSoundInternal::SetEnvironment(unsigned char nEnvironment)  // PC 0x005D66D0
{
    m_nCurrentEnvironment = nEnvironment;
    if (nEnvironment >= 24)
    {
        m_pDirectSound->SetI3DL2Listener(&CEXOSOUND_PRESETS[0], DS3D_DEFERRED);
    }
    else
    {
        m_pDirectSound->SetI3DL2Listener(&CEXOSOUND_PRESETS[nEnvironment], DS3D_DEFERRED);
    }
}
#endif

#ifdef _XBOX
inline void CExoSoundInternal::MuteSound()  // PC 0x005D6A10
{
    if (m_bMuted == 0)
    {
        m_nUnMuteFadeTime = 0;
        m_bMuted = 1;
        m_nMuteFadeTime = 500;
        m_nMuteStartTime =
            (unsigned long)g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
            1000;
    }
}
#endif

#ifdef _XBOX
inline void CExoSoundInternal::UnMuteSound()  // PC 0x005D6A50
{
    if (m_bMuted != 0)
    {
        m_nMuteFadeTime = 0;
        m_bMuted = 0;
        m_nUnMuteFadeTime = 500;
        m_nMuteStartTime =
            (unsigned long)g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
            1000;
    }
}
#endif

#ifdef _XBOX
int CExoSoundInternal::GetMutable(CExoXboxStream* pSource)
{
    return 1;
}
#endif

#ifdef _XBOX
int CExoSoundInternal::GetSliderType(CExoXboxStream* pSource)  // Xbox 0x00150AE0
{
    unsigned char nGroup = pSource->m_nPriorityGroup;

    if (nGroup != 8 && nGroup != 9 && nGroup != 16 && nGroup != 17 &&
        nGroup != 26)
    {
        if (nGroup != 1 && nGroup != 2)
        {
            return 2;
        }
        return 3;
    }
    return 1;
}
#endif

#ifdef _XBOX
float CExoSoundInternal::GetSliderVolume(int nSlider, int nTemporary)  // PC 0x005D6B10  Xbox 0x00150B20
{
    switch (nSlider)
    {
    case 1:
        return nTemporary ? m_fTempDialogVolume : m_fDialogVolume;
    case 2:
        return nTemporary ? m_fTempSoundEffectVolume : m_fSoundEffectVolume;
    case 3:
        return nTemporary ? m_fTempMusicVolume : m_fMusicVolume;
    }
    return 1.0f;
}
#endif

#ifdef _XBOX
void CExoSoundInternal::ShutDown()  // PC 0x005D6D30  Xbox 0x0014F230
{
    m_pStreamManager->ShutDown();
    while (m_lstPlaying.GetHeadPos() != NULL)
    {
        CExoSoundSourceInternal* pSource = m_lstPlaying.GetHead();
        if (pSource->m_bPlayWhenFree == 0)
        {
            CExoString sMessage;
            pSource->m_cResRef.CopyToString(sMessage);
            sMessage.Format("CExoSoundSource %s not freed", sMessage.CStr());
        }
        delete pSource;
    }
    if (g_bDisableSound == 0)
    {
        if (m_p3DVoices != NULL)
        {
            for (int nVoice = 0; nVoice < m_n3DVoices; nVoice++)
            {
                if (m_p3DVoices[nVoice].m_pSource != NULL)
                {
                    m_p3DVoices[nVoice].m_pSource->Stop();
                    m_p3DVoices[nVoice].m_pSource->ReleaseDemand();
                }
            }
            delete[] m_p3DVoices;
            m_p3DVoices = NULL;
        }
        if (m_p2DVoices != NULL)
        {
            for (unsigned char nVoice = 0; nVoice < m_n2DVoices; nVoice++)
            {
                if (m_p2DVoices[nVoice].m_pSource != NULL)
                {
                    m_p2DVoices[nVoice].m_pSource->Stop();
                    m_p2DVoices[nVoice].m_pSource->ReleaseDemand();
                }
            }
            delete[] m_p2DVoices;
            m_p2DVoices = NULL;
        }
    }
}
#endif

#ifdef _XBOX
void CExoSoundInternal::PlaySourceOn3DVoice(CExoSoundSourceInternal* pSource,  // PC 0x005D71C0  Xbox 0x0014F700
                                            C3DVoice* p3DVoice)
{
    int bOneShot = pSource->m_bLooping == 0;

    p3DVoice->m_pSource = pSource;
    pSource->m_p3DVoice = p3DVoice;

    Vector vPosition = pSource->m_vPosition;
    float fMaxVolumeDist = pSource->m_fMaxVolumeDist;
    float fMinVolumeDist = pSource->m_fMinVolumeDist;

    if (pSource->m_nDataSize != 0)
    {
        DSBUFFERDESC cDesc;

        ZeroMemory(&cDesc, sizeof(cDesc));
        cDesc.dwSize = sizeof(cDesc);
        cDesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE;
        cDesc.dwBufferBytes = 0;
        cDesc.lpwfxFormat = &pSource->m_wfxFormat.wfx;
        m_pDirectSound->CreateSoundBuffer(&cDesc, &p3DVoice->m_pBuffer, NULL);
        if (pSource->m_pData != NULL)
        {
            p3DVoice->m_pBuffer->SetBufferData(pSource->m_pData, pSource->m_nDataSize);
        }
    }
    pSource->SetVolume(pSource->m_nVolume, 0, 1.0f);
    p3DVoice->m_pBuffer->SetMinDistance(fMaxVolumeDist, DS3D_DEFERRED);
    p3DVoice->m_pBuffer->SetMaxDistance(fMinVolumeDist, DS3D_DEFERRED);
    p3DVoice->m_pBuffer->SetPosition(vPosition.m_fX, vPosition.m_fZ, vPosition.m_fY, DS3D_DEFERRED);

    if (pSource->m_fPitchVariance != 0.0f)
    {
        p3DVoice->m_pBuffer->SetFrequency(pSource->m_nPitchRate);
    }
    else if (pSource->m_fFixedVariance != 0.0f)
    {
        p3DVoice->m_pBuffer->SetFrequency(pSource->m_nFixedRate);
    }

    Vector vSourcePosition = pSource->m_vPosition;
    Vector vListenerPosition = m_vListenerPosition;
    unsigned long nTime =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
                        1000);
    float fObstruction = 0.0f;

    if (GetClosestRecentSound(vSourcePosition, vListenerPosition,
                              nTime, fObstruction) != 0)
    {
        pSource->m_vObstructionListenerPosition = vListenerPosition;
        pSource->m_nObstructionTime = nTime;
        pSource->m_fObstruction = fObstruction;
    }
    else
    {
        pSource->m_fObstruction =
            GetObstructionFactor(vSourcePosition, vListenerPosition);
        pSource->m_vObstructionListenerPosition = vListenerPosition;
        pSource->m_nObstructionTime = nTime;
        ++m_nObstructionQueries;
        if (m_nObstructionQueries > m_nMaxObstructionQueries)
        {
            m_nMaxObstructionQueries = m_nObstructionQueries;
        }
    }
    pSource->DoObstructionWork();
    m_nOcclusionTime += (unsigned long)(
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000) - nTime;
    m_pDirectSound->CommitDeferredSettings();
    if (pSource->m_pData != NULL)
    {
        p3DVoice->m_pBuffer->Play(0, 0, bOneShot ? 0 : DSBPLAY_LOOPING);
    }
}
#endif

#ifdef _XBOX
void CExoSoundInternal::PlaySourceOn2DVoice(CExoSoundSourceInternal* pSource,  // PC 0x005D7480  Xbox 0x0014F9E0
                                            C2DVoice* p2DVoice)
{
    EXOSOUND_TRACE();
    int bOneShot = pSource->m_bLooping == 0;

    pSource->m_p2DVoice = p2DVoice;
    p2DVoice->m_pSource = pSource;

    if (pSource->m_nDataSize != 0)
    {
        DSBUFFERDESC cDesc;

        ZeroMemory(&cDesc, sizeof(cDesc));
        cDesc.dwSize = sizeof(cDesc);
        cDesc.dwFlags = 0;
        cDesc.dwBufferBytes = 0;
        cDesc.lpwfxFormat = &pSource->m_wfxFormat.wfx;
        m_pDirectSound->CreateSoundBuffer(&cDesc, &p2DVoice->m_pBuffer, NULL);
        if (pSource->m_pData != NULL)
        {
            p2DVoice->m_pBuffer->SetBufferData(pSource->m_pData, pSource->m_nDataSize);
        }
    }
    pSource->SetVolume(pSource->m_nVolume, 0, m_fVolumeScale);

    if (pSource->m_fPitchVariance != 0.0f)
    {
        p2DVoice->m_pBuffer->SetFrequency(pSource->m_nPitchRate);
    }
    else if (pSource->m_fFixedVariance != 0.0f)
    {
        p2DVoice->m_pBuffer->SetFrequency(pSource->m_nFixedRate);
    }
    if (pSource->m_pData != NULL)
    {
        p2DVoice->m_pBuffer->Play(0, 0, bOneShot ? 0 : DSBPLAY_LOOPING);
    }
}
#endif

#ifdef _XBOX
// PC 0x005D7680  Xbox 0x0014FD00
int CExoSoundInternal::Play3DOneShotSound(CResRef const& sResRef,
                                         Vector vPosition,
                                         unsigned char nPriorityGroup,
                                         unsigned long nOneShotDelay,
                                         unsigned char nVolume,
                                         float fFixedVariance,
                                         float fPitchVariance)
{
    int nResult = 0;

    if (sResRef == "")
    {
        return 0;
    }

    CExoSoundSourceInternal* pSource = new CExoSoundSourceInternal(this);
    pSource->SetResRef(sResRef, 0);
    pSource->m_bPlayWhenFree = 1;
    pSource->Set3D(1);
    pSource->SetPriorityGroup(nPriorityGroup);
    pSource->SetPosition(vPosition);
    if (nVolume != 0)
    {
        pSource->SetVolume(nVolume, 0, 1.0f);
    }
    if (fFixedVariance != 0.0f)
    {
        pSource->SetFixedVariance(fFixedVariance);
    }
    if (fPitchVariance != 0.0f)
    {
        pSource->SetPitchVariance(fPitchVariance);
    }
    float fMinVolumeDist =
        m_pPriorityGroups[nPriorityGroup].m_fMinVolumeDist;
    float fMaxVolumeDist =
        m_pPriorityGroups[nPriorityGroup].m_fMaxVolumeDist;
    pSource->m_fMaxVolumeDist = fMaxVolumeDist;
    pSource->m_fMinVolumeDist = fMinVolumeDist;
    if (nOneShotDelay != 0)
    {
        pSource->SetOneShotDelay(nOneShotDelay);
    }
    else
    {
        nResult = pSource->Play(0, 0);
        if (pSource->m_bRetryPlay != 0)
        {
            delete pSource;
        }
    }
    return nResult;
}
#endif

#ifdef _XBOX
CExoLinkedListNode* CExoSoundInternal::RemoveFromPlayingList(  // PC 0x005D78F0  Xbox 0x0014FE70
    CExoSoundSourceInternal* pSource)
{
    CExoLinkedListNode* pPosition = m_lstPlaying.GetHeadPos();
    CExoSoundSourceInternal* pCurrent = m_lstPlaying.GetHead();

    while (pPosition != NULL)
    {
        if (pCurrent == pSource)
        {
            if (pPosition == m_pOcclusionPosition)
            {
                m_lstPlaying.GetNext(m_pOcclusionPosition);
                if (m_pOcclusionPosition == NULL)
                {
                    m_pOcclusionPosition = m_lstStreaming.GetHeadPos();
                    m_nOcclusionList = OCCLUSION_LIST_STREAMING;
                    if (m_pOcclusionPosition == NULL)
                    {
                        m_nOcclusionList = OCCLUSION_LIST_PLAYING;
                        m_pOcclusionPosition = NULL;
                    }
                }
            }
            break;
        }
        pCurrent = m_lstPlaying.GetNext(pPosition);
    }
    pPosition = pSource->m_pListNode;
    m_lstPlaying.GetAtPos(pPosition);
    m_lstPlaying.GetNext(pPosition);
    m_lstPlaying.Remove(pSource->m_pListNode);
    pSource->m_nListType = CExoSoundSourceInternal::LIST_NONE;
    pSource->m_pListNode = NULL;
    CExoSoundPriorityGroup* pGroup =
        m_pPriorityGroups + pSource->m_nPriorityGroup;

    pGroup->m_nPlaying = pGroup->m_nPlaying - 1;
    return pPosition;
}
#endif

#ifdef _XBOX
CExoLinkedListNode* CExoSoundInternal::RemoveFromNonPlayingLoopingList(  // PC 0x005D79C0  Xbox 0x0014FF50
    CExoSoundSourceInternal* pSource)
{
    CExoLinkedListNode* pNext = pSource->m_pListNode;

    m_lstNonPlayingLooping.GetAtPos(pNext);
    m_lstNonPlayingLooping.GetNext(pNext);
    m_lstNonPlayingLooping.Remove(pSource->m_pListNode);
    pSource->m_nListType = CExoSoundSourceInternal::LIST_NONE;
    pSource->m_pListNode = NULL;
    return pNext;
}
#endif

#ifdef _XBOX
CExoLinkedListNode* CExoSoundInternal::RemoveFromDelayedOneShotList(  // PC 0x005D7A70  Xbox 0x00150010
    CExoSoundSourceInternal* pSource)
{
    CExoLinkedListNode* pNext = pSource->m_pListNode;

    m_lstDelayedOneShots.GetAtPos(pNext);
    m_lstDelayedOneShots.GetNext(pNext);
    m_lstDelayedOneShots.Remove(pSource->m_pListNode);
    pSource->m_nListType = CExoSoundSourceInternal::LIST_NONE;
    pSource->m_pListNode = NULL;
    return pNext;
}
#endif

#ifdef _XBOX
// PC 0x005D7AB0  Xbox 0x00150060
CExoLinkedListNode* CExoSoundInternal::RemoveFromStreamingList(
    CExoStreamingSoundSourceInternal* pSource)
{
    CExoLinkedListNode* pPosition = m_lstStreaming.GetHeadPos();
    CExoStreamingSoundSourceInternal* pCurrent = m_lstStreaming.GetHead();

    while (pPosition != NULL)
    {
        if (pCurrent == pSource)
        {
            if (pPosition == m_pOcclusionPosition)
            {
                m_lstStreaming.GetNext(m_pOcclusionPosition);
                if (m_pOcclusionPosition == NULL)
                {
                    m_pOcclusionPosition = m_lstPlaying.GetHeadPos();
                    m_nOcclusionList = OCCLUSION_LIST_PLAYING;
                    if (m_pOcclusionPosition == NULL)
                    {
                        m_nOcclusionList = OCCLUSION_LIST_PLAYING;
                        m_pOcclusionPosition = NULL;
                    }
                }
            }
            break;
        }
        pCurrent = m_lstStreaming.GetNext(pPosition);
    }
    pPosition = m_lstStreaming.GetHeadPos();
    pCurrent = m_lstStreaming.GetHead();
    while (pPosition != NULL)
    {
        if (pCurrent == pSource)
        {
            m_lstStreaming.Remove(pPosition);
            CExoSoundPriorityGroup* pGroup =
                m_pPriorityGroups + pCurrent->m_nPriorityGroup;

            pGroup->m_nPlaying = pGroup->m_nPlaying - 1;
            break;
        }
        pCurrent = m_lstStreaming.GetNext(pPosition);
    }
    return NULL;
}
#endif

#ifdef _XBOX
C3DVoice* CExoSoundInternal::GetFree3DVoice(unsigned char nPriorityGroup)  // PC 0x005D8EB0  Xbox 0x001519B0
{
    unsigned char nPriority = 0;
    CExoSoundPriorityGroup* pGroup = m_pPriorityGroups + nPriorityGroup;
    int nInterrupt;
    int nVoice;

    if (pGroup == NULL)
    {
        return NULL;
    }
    for (nVoice = 0; nVoice < m_n3DVoices; nVoice++)
    {
        if (m_p3DVoices[nVoice].m_bReleasing == 0)
        {
            CExoSoundSourceInternal* pVoiceSource =
                m_p3DVoices[nVoice].m_pSource;

            if (pVoiceSource == NULL)
            {
                return &m_p3DVoices[nVoice];
            }
            CExoSoundPriorityGroup* pVoiceGroup =
                m_pPriorityGroups + pVoiceSource->m_nPriorityGroup;

            if (pVoiceGroup != NULL &&
                pVoiceGroup->m_nPriority > pGroup->m_nPriority &&
                pVoiceGroup->m_nPriority > nPriority)
            {
                nInterrupt = nVoice;
                nPriority = pVoiceGroup->m_nPriority;
            }
        }
    }
    if (nPriority > 0)
    {
        CExoSoundSourceInternal* pSource = m_p3DVoices[nInterrupt].m_pSource;

        if (pSource->m_bPlayWhenFree != 0)
        {
            if (pSource->IsPlaying() != 0)
            {
                if (m_lstPendingDeletion.Contains(pSource) == 0)
                {
                    m_lstPendingDeletion.AddHead(pSource);
                }
            }
            else if (m_lstPendingDeletion.Contains(pSource) == 0)
            {
                delete pSource;
            }
        }
        else
        {
            pSource->ShutDownSource();
            if (pSource->m_bLooping != 0)
            {
                AddToNonPlayingLoopingList(pSource);
            }
        }
        return &m_p3DVoices[nInterrupt];
    }
    return NULL;
}
#endif

#ifdef _XBOX
C2DVoice* CExoSoundInternal::GetFree2DVoice(unsigned char nPriorityGroup)  // PC 0x005D8FF0  Xbox 0x00151AF0
{
    unsigned char nPriority = 0;
    CExoSoundPriorityGroup* pGroup = m_pPriorityGroups + nPriorityGroup;
    int nInterrupt;
    int nVoice;

    if (pGroup == NULL)
    {
        return NULL;
    }

    for (nVoice = 0; nVoice < m_n2DVoices - 2; nVoice++)
    {
        if (m_p2DVoices[nVoice].m_bReleasing == 0)
        {
            CExoSoundSourceInternal* pVoiceSource =
                m_p2DVoices[nVoice].m_pSource;

            if (pVoiceSource == NULL)
            {
                return &m_p2DVoices[nVoice];
            }
            CExoSoundPriorityGroup* pVoiceGroup =
                m_pPriorityGroups + pVoiceSource->m_nPriorityGroup;

            if (pVoiceGroup != NULL &&
                pVoiceGroup->m_nPriority > pGroup->m_nPriority &&
                pVoiceGroup->m_nPriority > nPriority)
            {
                nInterrupt = nVoice;
                nPriority = pVoiceGroup->m_nPriority;
            }
        }
    }
    if (nPriority > 0)
    {
        CExoSoundSourceInternal* pSource = m_p2DVoices[nInterrupt].m_pSource;

        if (pSource->m_bPlayWhenFree != 0)
        {
            if (pSource->IsPlaying() != 0)
            {
                if (m_lstPendingDeletion.Contains(pSource) == 0)
                {
                    m_lstPendingDeletion.AddHead(pSource);
                }
            }
            else if (m_lstPendingDeletion.Contains(pSource) == 0)
            {
                delete pSource;
            }
        }
        else
        {
            pSource->ShutDownSource();
            if (pSource->m_bLooping != 0)
            {
                AddToNonPlayingLoopingList(pSource);
            }
        }
        return &m_p2DVoices[nInterrupt];
    }
    return NULL;
}
#endif

#ifdef _XBOX
void CExoSoundInternal::ApplyNewVolume(int nTemporary)  // PC 0x005D8110  Xbox 0x00150860
{
    CExoLinkedListNode* pPosition;
    CExoStreamingSoundSourceInternal* pStream;
    CExoSoundSourceInternal* pSource;
    void* pData;

    pPosition = m_lstStreaming.GetHeadPos();
    pStream = m_lstStreaming.GetHead();
    while (pPosition != NULL)
    {
        if (pStream != NULL)
        {
            pStream->SetVolume(pStream->m_nVolume, nTemporary, 1.0f);
        }
        pStream = m_lstStreaming.GetNext(pPosition);
    }
    pPosition = m_lstPlaying.GetHeadPos();
    pData = m_lstPlaying.GetHead();
    pSource = (CExoSoundSourceInternal*)pData;
    while (pPosition != NULL)
    {
        if (pSource != NULL)
        {
            pSource->SetVolume(pSource->m_nVolume, nTemporary, 1.0f);
        }
        pSource = m_lstPlaying.GetNext(pPosition);
    }
}
#endif

#ifdef _XBOX
int CExoSoundInternal::PauseAllSounds()  // Xbox 0x00151C80
{
    CExoLinkedListNode* pPosition;
    CExoStreamingSoundSourceInternal* pStream;
    CExoSoundSourceInternal* pSource;
    void* pData;

    g_bSoundPaused = 1;
    pPosition = m_lstStreaming.GetHeadPos();
    pData = m_lstStreaming.GetHead();
    pStream = (CExoStreamingSoundSourceInternal*)pData;
    while (pPosition != NULL)
    {
        if (GetPauseable(pStream) != 0)
        {
            pStream->Pause();
        }
        pStream = m_lstStreaming.GetNext(pPosition);
    }
    pPosition = m_lstPlaying.GetHeadPos();
    pData = m_lstPlaying.GetHead();
    pSource = (CExoSoundSourceInternal*)pData;
    while (pPosition != NULL)
    {
        if (pSource->IsPaused() == 0 && GetPauseable(pSource) != 0)
        {
            pSource->Pause();
        }
        pSource = m_lstPlaying.GetNext(pPosition);
    }
    return 1;
}
#endif

#ifdef _XBOX
void CExoSoundInternal::AddToStreamingList(  // Xbox 0x00150980
    CExoStreamingSoundSourceInternal* pSource)
{
    m_lstStreaming.AddHead(pSource);
    CExoSoundPriorityGroup* pGroup =
        m_pPriorityGroups + pSource->m_nPriorityGroup;

    pGroup->m_nPlaying = pGroup->m_nPlaying + 1;
}



// Xbox 0x001509B0
void CExoSoundInternal::DetachReleasingVoices(CExoSoundSourceInternal* pSource)
{
    CExoLinkedListNode* pPosition;

    pPosition = m_lstReleasing2DVoices.GetHeadPos();
    while (pPosition != NULL)
    {
        C2DVoice* p2DVoice = m_lstReleasing2DVoices.GetAtPos(pPosition);

        if (p2DVoice != NULL && p2DVoice->m_pSource == pSource)
        {
            p2DVoice->m_bSourceGone = 1;
        }
        m_lstReleasing2DVoices.GetNext(pPosition);
    }
    pPosition = m_lstReleasing3DVoices.GetHeadPos();
    while (pPosition != NULL)
    {
        C3DVoice* p3DVoice = m_lstReleasing3DVoices.GetAtPos(pPosition);

        if (p3DVoice != NULL && p3DVoice->m_pSource == pSource)
        {
            p3DVoice->m_bSourceGone = 1;
        }
        m_lstReleasing3DVoices.GetNext(pPosition);
    }
}
// Xbox 0x00150A40
int CExoSoundInternal::IsReleasing2DVoice(C2DVoice* pVoice)
{
    return m_lstReleasing2DVoices.Contains(pVoice);
}
// Xbox 0x00150A50
void CExoSoundInternal::AddReleasing2DVoice(C2DVoice* pVoice)
{
    m_lstReleasing2DVoices.AddHead(pVoice);
    pVoice->m_bReleasing = 1;
}
// Xbox 0x00150A70
int CExoSoundInternal::IsReleasing3DVoice(C3DVoice* pVoice)
{
    return m_lstReleasing3DVoices.Contains(pVoice);
}
// Xbox 0x00150A80
void CExoSoundInternal::AddReleasing3DVoice(C3DVoice* pVoice)
{
    m_lstReleasing3DVoices.AddHead(pVoice);
    pVoice->m_bReleasing = 1;
}
#endif

#ifdef _XBOX
// Xbox 0x00151C40
void CExoSoundInternal::SetEnvironmentEffectLevel(float fLevel)
{
    if (fLevel != -1.0f)
    {
        fLevel = fLevel > 0.0f ? (1.0f > fLevel ? fLevel : 1.0f) : 0.0f;
    }
    m_fReverbLevel = fLevel;
    ApplyNewVolume(0);
}

// PC 0x005D8560  Xbox 0x00151DB0
void CExoSoundInternal::SetSoundMode(int nMode)
{
    if (nMode != CExoSound::CEXOSOUND_MODE_GENERIC &&
        nMode == m_anSoundMode[m_nSoundMode])
    {
        return;
    }

    if (m_anSoundMode[m_nSoundMode] == CExoSound::CEXOSOUND_MODE_MUTE)
    {
        UnMuteSound();
    }
    if (m_anSoundMode[m_nSoundMode] == CExoSound::CEXOSOUND_MODE_PAUSE ||
        m_anSoundMode[m_nSoundMode] == CExoSound::CEXOSOUND_MODE_PAUSE_ALL ||
        m_anSoundMode[m_nSoundMode] ==
            CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC)
    {
        ResumeAllSounds();
    }

    if (nMode == CExoSound::CEXOSOUND_MODE_GENERIC)
    {
        m_nSoundMode--;
        if (m_nSoundMode < 0)
        {
            m_nSoundMode = 0;
        }
        nMode = m_anSoundMode[m_nSoundMode];
    }
    else
    {
        m_nSoundMode++;
        if (m_nSoundMode > 9)
        {
            m_nSoundMode = 9;
        }
        m_anSoundMode[m_nSoundMode] = nMode;
    }

    if (nMode == CExoSound::CEXOSOUND_MODE_MUTE)
    {
        MuteSound();
    }
    if (nMode == CExoSound::CEXOSOUND_MODE_PAUSE ||
        nMode == CExoSound::CEXOSOUND_MODE_PAUSE_ALL ||
        nMode == CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC)
    {
        PauseAllSounds();
    }
}
#endif

#ifdef _XBOX
CExoSoundInternal::~CExoSoundInternal()  // PC 0x005D9400  Xbox 0x00150E90
{
    ShutDown();
    if (m_pPriorityGroups != NULL)
    {
        delete[] m_pPriorityGroups;
        m_pPriorityGroups = NULL;
    }
}
#endif

#ifdef _XBOX


// PC 0x005D9590  Xbox 0x00151F00
void CExoSoundInternal::Render()
{
    unsigned __int64 nStartTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();

    Sleep(1);
    DeleteStoppedStreams();
    if (g_bSoundPaused != 0)
    {
        PauseAllSounds();
    }

    unsigned long nRenderMS = (unsigned long)
        (g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    unsigned long nRenderDeltaMS = nRenderMS - m_nLastRenderTime;
    m_nLastRenderTime = nRenderMS;
    float fElapsedSec = min((float)nRenderDeltaMS / 1000.0f, 0.1f);
    int nRequestedEnvironment = GetDesiredEnvironment();
    float fAmbientScaleDesired = GetDesiredAmbientScale();

    if (m_nCurrentEnvironment != nRequestedEnvironment)
    {
        SetEnvironment((unsigned char)nRequestedEnvironment);
    }
    if (fAmbientScaleDesired != m_fVolumeScale)
    {
        m_fVolumeScale += (fAmbientScaleDesired - m_fVolumeScaleStart) *
            (fElapsedSec / 1.0f);
        if (fAmbientScaleDesired > m_fVolumeScaleStart)
        {
            m_fVolumeScale = min(fAmbientScaleDesired, m_fVolumeScale);
        }
        if (fAmbientScaleDesired < m_fVolumeScaleStart)
        {
            m_fVolumeScale = max(fAmbientScaleDesired, m_fVolumeScale);
        }
    }
    else
    {
        m_fVolumeScaleStart = m_fVolumeScale;
    }
    if (m_fVolumeScaleTarget != fAmbientScaleDesired)
    {
        m_fVolumeScaleStart = m_fVolumeScale;
    }
    m_fVolumeScaleTarget = fAmbientScaleDesired;
    m_pStreamManager->m_fVolumeScale = m_fVolumeScale;
    DirectSoundDoWork();

    CExoLinkedListNode* pPos = m_lstStreaming.GetHeadPos();
    CExoStreamingSoundSourceInternal* pStreamingSoundSource =
        m_lstStreaming.GetHead();
    while (pPos != NULL)
    {
        if (!pStreamingSoundSource->IsPlaying())
        {
            RemoveFromStreamingList(pStreamingSoundSource);
            break;
        }
        pStreamingSoundSource = m_lstStreaming.GetNext(pPos);
    }

    pPos = m_lstStreaming.GetHeadPos();
    pStreamingSoundSource = m_lstStreaming.GetHead();
    while (pPos != NULL)
    {
        if (pStreamingSoundSource->Get3D() &&
            pStreamingSoundSource->GetLooping() &&
            !pStreamingSoundSource->GetWithinRangeOfListener())
        {
            m_lstStreaming.GetNext(pPos);
            pStreamingSoundSource->Stop();
            AddToNonPlayingStreamingLoopingList(pStreamingSoundSource);
            continue;
        }
        pStreamingSoundSource = m_lstStreaming.GetNext(pPos);
    }

    pPos = m_lstNonPlayingStreamingLooping.GetHeadPos();
    pStreamingSoundSource = m_lstNonPlayingStreamingLooping.GetHead();
    while (pPos != NULL)
    {
        if (pStreamingSoundSource->Get3D() &&
            pStreamingSoundSource->GetLooping() &&
            pStreamingSoundSource->GetWithinRangeOfListener())
        {
            pStreamingSoundSource->Play(pStreamingSoundSource->GetResRef(),
                pStreamingSoundSource->GetResourceType(), 0, 0, 0);
            pPos = RemoveFromNonPlayingStreamingLoopingList(
                pStreamingSoundSource);
            if (pPos != NULL)
            {
                pStreamingSoundSource =
                    m_lstNonPlayingStreamingLooping.GetAtPos(pPos);
            }
            continue;
        }
        pStreamingSoundSource = m_lstNonPlayingStreamingLooping.GetNext(pPos);
    }

    unsigned long nTimeStamp = (unsigned long)
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
    unsigned long nElapsed;
    unsigned long nOneShotDelayMS;
    unsigned long nStartMS;
    if (m_nMuteFadeTime != 0)
    {
        nElapsed = nTimeStamp - m_nMuteStartTime;
        if (nElapsed > 0)
        {
            float fFade = min(1.0f, max(0.0f,
                (float)nElapsed / (unsigned long)m_nMuteFadeTime));
            float fDialogVolume = GetDialogVolume(0);
            float fSoundEffectVolume = GetSoundEffectVolume(0);
            float fMusicVolume = GetMusicVolume(0);
            SetDialogVolume(fDialogVolume - fFade * fDialogVolume, 1);
            SetSoundEffectVolume(
                fSoundEffectVolume - fFade * fSoundEffectVolume, 1);
            SetMusicVolume(fMusicVolume - fFade * fMusicVolume, 1, 0);
            ApplyNewVolume(1);
            if (fFade == 1.0f)
            {
                m_nMuteFadeTime = 0;
                m_nMuteStartTime = 0;
            }
        }
    }
    if (m_nUnMuteFadeTime != 0)
    {
        nElapsed = nTimeStamp - m_nMuteStartTime;
        if (nElapsed > 0)
        {
            float fFade = min(1.0f, max(0.0f,
                (float)nElapsed / (unsigned long)m_nUnMuteFadeTime));
            float fDialogVolume = GetDialogVolume(0);
            float fSoundEffectVolume = GetSoundEffectVolume(0);
            float fMusicVolume = GetMusicVolume(0);
            SetDialogVolume(fFade * fDialogVolume, 1);
            SetSoundEffectVolume(fFade * fSoundEffectVolume, 1);
            SetMusicVolume(fFade * fMusicVolume, 1, 0);
            ApplyNewVolume(1);
            if (fFade == 1.0f)
            {
                m_nUnMuteFadeTime = 0;
                m_nMuteStartTime = 0;
            }
        }
    }

    nTimeStamp = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
    pPos = m_lstDelayedOneShots.GetHeadPos();
    CExoSoundSourceInternal* pSoundSource = m_lstDelayedOneShots.GetHead();
    while (pPos != NULL)
    {
        nOneShotDelayMS = pSoundSource->GetOneShotDelay();
        nStartMS = pSoundSource->GetDelayStart();
        if (nTimeStamp >= nStartMS)
        {
            nElapsed = nTimeStamp - nStartMS;
        }
        else
        {
            nElapsed = 0xffffffff - nStartMS + nTimeStamp;
        }
        if (nElapsed != 0)
        {
            if (nElapsed >= nOneShotDelayMS)
            {
                pPos = RemoveFromDelayedOneShotList(pSoundSource);
                pSoundSource->Play(0, 0);
                if (pSoundSource->GetRetryPlay() != 0)
                {
                    delete pSoundSource;
                    pSoundSource = NULL;
                }
                if (pPos != NULL)
                {
                    pSoundSource = m_lstPlaying.GetAtPos(pPos);
                }
            }
            else
            {
                pSoundSource = m_lstDelayedOneShots.GetNext(pPos);
            }
        }
        else
        {
            pSoundSource = m_lstDelayedOneShots.GetNext(pPos);
        }
    }

    pPos = m_lstPlaying.GetHeadPos();
    pSoundSource = m_lstPlaying.GetHead();
    while (pPos != NULL)
    {
        if (!pSoundSource->IsPlaying())
        {
            pPos = RemoveFromPlayingList(pSoundSource);
            if (pSoundSource->GetPlayWhenFree())
            {
                if (!m_lstPendingDeletion.Contains(pSoundSource))
                {
                    delete pSoundSource;
                }
                pSoundSource = NULL;
            }
            else if (!pSoundSource->IsPaused())
            {
                pSoundSource->ShutDownSource();
            }
            if (pPos != NULL)
            {
                pSoundSource = m_lstPlaying.GetAtPos(pPos);
            }
        }
        else if (!pSoundSource->GetWithinRangeOfListener())
        {
            pPos = RemoveFromPlayingList(pSoundSource);
            if (pSoundSource->GetPlayWhenFree())
            {
                if (pSoundSource->IsPlaying())
                {
                    if (!m_lstPendingDeletion.Contains(pSoundSource))
                    {
                        m_lstPendingDeletion.AddHead(pSoundSource);
                    }
                }
                else if (!m_lstPendingDeletion.Contains(pSoundSource))
                {
                    delete pSoundSource;
                }
                pSoundSource = NULL;
            }
            else
            {
                pSoundSource->ShutDownSource();
                if (pSoundSource->GetLooping())
                {
                    AddToNonPlayingLoopingList(pSoundSource);
                }
            }
            if (pPos != NULL)
            {
                pSoundSource = m_lstPlaying.GetAtPos(pPos);
            }
        }
        else
        {
            if (m_fVolumeScale != m_fVolumeScaleStart)
            {
                pSoundSource->SetVolume(
                    pSoundSource->GetVolume(), 0, m_fVolumeScale);
            }
            pSoundSource = m_lstPlaying.GetNext(pPos);
        }
    }

    pPos = m_lstNonPlayingLooping.GetHeadPos();
    pSoundSource = m_lstNonPlayingLooping.GetHead();
    while (pPos != NULL)
    {
        m_lstNonPlayingLooping.GetNext(pPos);
        pSoundSource->Play(1, 0);
        if (pPos != NULL)
        {
            pSoundSource = m_lstNonPlayingLooping.GetAtPos(pPos);
        }
    }

    pPos = m_lstPendingDeletion.GetHeadPos();
    while (pPos != NULL)
    {
        pSoundSource = m_lstPendingDeletion.GetAtPos(pPos);
        if (!pSoundSource->IsPlaying())
        {
            CExoLinkedListNode* pDeletePosition = pPos;
            m_lstPendingDeletion.GetNext(pPos);
            m_lstPendingDeletion.Remove(pDeletePosition);
            delete pSoundSource;
        }
        else
        {
            m_lstPendingDeletion.GetNext(pPos);
        }
    }



    DWORD nStatus = 0;
    pPos = m_lstReleasing2DVoices.GetHeadPos();
    while (pPos != NULL)
    {
        C2DVoice* p2DVoice = m_lstReleasing2DVoices.GetAtPos(pPos);
        if (p2DVoice != NULL)
        {
            p2DVoice->m_pBuffer->GetStatus(&nStatus);
            if (!(nStatus & DSBSTATUS_PLAYING))
            {
                CExoLinkedListNode* pReleasePosition = pPos;
                m_lstReleasing2DVoices.GetNext(pPos);
                m_lstReleasing2DVoices.Remove(pReleasePosition);
                p2DVoice->m_pBuffer->Release();
                if (p2DVoice->m_bSourceGone == 0)
                {
                    p2DVoice->m_pSource->ReleaseDemand();
                }
                p2DVoice->m_pSource = NULL;
                p2DVoice->m_bReleasing = 0;
                p2DVoice->m_bSourceGone = 0;
            }
        }
        else
        {
            m_lstReleasing2DVoices.GetNext(pPos);
        }
    }
    nStatus = 0;
    pPos = m_lstReleasing3DVoices.GetHeadPos();
    while (pPos != NULL)
    {
        C3DVoice* p3DVoice = m_lstReleasing3DVoices.GetAtPos(pPos);
        if (p3DVoice != NULL)
        {
            p3DVoice->m_pBuffer->GetStatus(&nStatus);
            if (!(nStatus & DSBSTATUS_PLAYING))
            {
                CExoLinkedListNode* pReleasePosition = pPos;
                m_lstReleasing3DVoices.GetNext(pPos);
                m_lstReleasing3DVoices.Remove(pReleasePosition);
                p3DVoice->m_pBuffer->Release();
                if (p3DVoice->m_bSourceGone == 0)
                {
                    p3DVoice->m_pSource->ReleaseDemand();
                }
                p3DVoice->m_pSource = NULL;
                p3DVoice->m_bReleasing = 0;
                p3DVoice->m_bSourceGone = 0;
            }
        }
        else
        {
            m_lstReleasing3DVoices.GetNext(pPos);
        }
    }
    m_pDirectSound->CommitDeferredSettings();
    OcclusionRender(nRenderDeltaMS);
    g_fSoundRenderTime = (float)(unsigned long)
        (g_pExoBase->m_pExoTimers->GetHighResolutionTimer() - nStartTime) * 0.001f;
}
#endif

#ifdef _XBOX

// Xbox 0x00150FE0
int CExoSoundInternal::Initialize(const CExoString& s3DProvider, unsigned char n2DVoices, unsigned char n3DVoices)
{
    CExoString sCopy = s3DProvider;
    int nVoice;

    if (g_bDisableSound == 1)
    {
        return 0;
    }
    m_n3DVoices = n3DVoices;
    m_n3DVoices = min(m_n3DVoices, MAX_VOICES);
    m_n3DVoices = max(m_n3DVoices, 8);
    m_p3DVoices = new C3DVoice[m_n3DVoices];
    m_n2DVoices = n2DVoices;
    m_n2DVoices = min(m_n2DVoices, MAX_VOICES);
    m_n2DVoices = max(m_n2DVoices, 8);
    m_p2DVoices = new C2DVoice[m_n2DVoices];
    for (nVoice = 0; nVoice < m_n3DVoices; nVoice++)
    {
        m_p3DVoices[nVoice].m_pSource = NULL;
        m_p3DVoices[nVoice].m_pBuffer = NULL;
        m_p3DVoices[nVoice].m_bReleasing = 0;
        m_p3DVoices[nVoice].m_bSourceGone = 0;
    }
    for (nVoice = 0; nVoice < m_n2DVoices; nVoice++)
    {
        m_p2DVoices[nVoice].m_pSource = NULL;
        m_p2DVoices[nVoice].m_pBuffer = NULL;
        m_p2DVoices[nVoice].m_bReleasing = 0;
        m_p2DVoices[nVoice].m_bSourceGone = 0;
    }
    if (DirectSoundCreate(NULL, &m_pDirectSound, NULL) != DS_OK)
    {
        return 0;
    }
    if (FAILED(LoadEffectsImage("d:\\dsstdfx.bin")))
    {
        CExoString sFileName("dsstdfx");
        g_pExoResMan->ReportMissingFile(sFileName, 0xFFFF, 1);
        return 0;
    }
    DirectSoundUseFullHRTF();
    SetEnvironment(0);
    m_bInitialized = 1;
    m_pDirectSound->SetRolloffFactor(1.0f, DS3D_IMMEDIATE);
    SetEnvironment(m_nCurrentEnvironment);
    return 1;
}
#endif

#ifdef _XBOX


// Xbox 0x0014EE00
long ExoSoundVolumeToMillibels(unsigned char nVolume, long nMinVolume)
{
    float fMinVolume = (float)nMinVolume;
    float fVolume = (float)(unsigned long)(nVolume + 1);
    float fRange = -fMinVolume;
    double fMillibels = log10(fVolume) / log10(128.0) * fRange + fMinVolume;

    return (long)min(0.0, max((double)nMinVolume, fMillibels));
}
// Xbox 0x0014EE90
unsigned char ExoSoundMillibelsToVolume(long nMillibels, long nMinVolume)
{
    float fMinVolume = (float)nMinVolume;
    double fVolume = pow(10.0, log10(128.0) * ((fMinVolume - nMillibels) / fMinVolume)) - 0.9999f;

    return (unsigned char)min(127.0, max(0.0, fVolume));
}


// Xbox 0x0014EF10
DWORD CExoSoundInternal::ParseWaveData(void* pWaveFile, XBOXADPCMWAVEFORMAT* pFormat, void** ppData)
{
    DWORD* pChunk = (DWORD*)pWaveFile;
    WAVEFORMATEX* pFileFormat = NULL;
    DWORD nDataSize = 0;

    ZeroMemory(pFormat, sizeof(XBOXADPCMWAVEFORMAT));
    DWORD nRiff = *pChunk++;
    DWORD nRiffSize = *pChunk++;
    DWORD nWave = *pChunk++;

    if (nRiff != 'FFIR')
    {
        return 0;
    }
    if (nWave != 'EVAW')
    {
        return 0;
    }
    DWORD* pEnd = (DWORD*)((BYTE*)pChunk + nRiffSize - 4);
    while (pChunk < pEnd)
    {
        DWORD nType = *pChunk++;
        DWORD nLength = *pChunk++;

        switch (nType)
        {
        case ' tmf':
            if (pFileFormat == NULL)
            {
                pFileFormat = (WAVEFORMATEX*)pChunk;
            }
            break;
        case 'atad':
            *ppData = pChunk;
            nDataSize = nLength;
            break;
        }
        pChunk = (DWORD*)((BYTE*)pChunk + ((nLength + 1) & ~1));
    }
    pFormat->wfx.cbSize = pFileFormat->cbSize;
    pFormat->wfx.nAvgBytesPerSec = pFileFormat->nAvgBytesPerSec;
    pFormat->wfx.nBlockAlign = pFileFormat->nBlockAlign;
    pFormat->wfx.nChannels = pFileFormat->nChannels;
    pFormat->wfx.nSamplesPerSec = pFileFormat->nSamplesPerSec;
    pFormat->wfx.wBitsPerSample = pFileFormat->wBitsPerSample;
    pFormat->wfx.wFormatTag = pFileFormat->wFormatTag;
    pFormat->wSamplesPerBlock = pFormat->wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM ? 64 : 0;
    return nDataSize;
}

#endif

#ifdef _XBOX

// Xbox 0x001506D0
HRESULT CExoSoundInternal::LoadEffectsImage(const char* pszFileName)
{
    BYTE* pBuffer = NULL;
    HRESULT hr = S_OK;
    DWORD nSize;
    DWORD nRead;
    DWORD nError;
    DSEFFECTIMAGELOC cLocation;
    LPDSEFFECTIMAGEDESC pDescription;
    HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        nError = GetLastError();
        hr = HRESULT_FROM_WIN32(nError);
    }
    if (SUCCEEDED(hr))
    {
        nSize = SetFilePointer(hFile, 0, NULL, FILE_END);
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        pBuffer = new BYTE[nSize];
        if (!ReadFile(hFile, pBuffer, nSize, &nRead, NULL))
        {
            nError = GetLastError();
            hr = HRESULT_FROM_WIN32(nError);
        }
    }
    if (SUCCEEDED(hr))
    {
        cLocation.dwI3DL2ReverbIndex = 0;
        cLocation.dwCrosstalkIndex = 1;
        hr = m_pDirectSound->DownloadEffectsImage(pBuffer, nSize, &cLocation, &pDescription);
    }
    delete[] pBuffer;
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
    }
    return hr;
}

#endif

#ifdef _XBOX

// Xbox 0x0014F390
void CExoSoundInternal::DeleteStoppedStreams()
{
    CExoXboxStream* pStream;

    while (m_pStreamManager->GetNextDeletedStream(&pStream))
    {
        if (pStream->m_nMagic == CExoXboxStream::STREAM_MAGIC && pStream->m_bDeleted != 0)
        {
            delete pStream;
        }
    }
}

#endif
