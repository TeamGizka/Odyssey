#include "clientoptions.h"
#include "twodimarray.h"
#include "math3d.h"
#include "exobase.h"
#include "nwclass.h"
#include "twodimarrays.h"
#include "swguicustompanel.h"
#include "swparty.h"
#include "partytables.h"
#include "nwcobject.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "aurora.h"
#include <string.h>
#include "nwccreature.h"

CExoString g_sUnlockedMovie;
float creaturewindintensity = 2.5f;
float KeyboardCameraRotationSpeed = -1.0f;
float KeyboardCameraRotationAccl = -1.0f;
float KeyboardCameraRotationDecel = -1.0f;

void AurDisableGrass();
void AurEnableGrass();
void AurSetVSyncState(bool bEnabled);
// PC 0x004015A0
CClientOptions::~CClientOptions()
{
}
// PC 0x0061B6F0  Xbox 0x0005F010
void CClientOptions::SetCameraMode(unsigned char nMode)
{
    m_nCameraMode = nMode;
    if (nMode == 1 || nMode == 0)
    {
        m_nCameraStyle = nMode;
    }
}
// PC 0x0061B710  Xbox 0x0005F030
void CClientOptions::SetFullScreenEnabled(int bEnabled)
{
    if (bEnabled != m_bFullScreen)
    {
        g_nReInitializing = (bEnabled != 0) + 1;
        m_bFullScreen = bEnabled;
    }
}
// PC 0x0061B740  Xbox 0x0005F1C0
void CClientOptions::SetAutoLevelUpNPCs(int bEnabled)
{
    m_bAutoLevelUpNPCs = bEnabled;
    if (m_bAutoLevelUpNPCs)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp != 0)
        {
            pServerExoApp->GetPartyTable()->AutoLevelUpNPCs();
        }
    }
}
// PC 0x0061B780
int CClientOptions::SaveOptions()
{
    CExoIni cIni;
    int i = 0;
    CExoString sValue;
    sValue.Format("%d", m_bCombatMovement);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Combat Movement");
    sValue.Format("%d", m_bReverseMinigameYAxis);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Reverse Minigame YAxis");
    sValue.Format("%d", m_bReverseMouseButtons);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Reverse Mouse Buttons");
    sValue.Format("%d", m_bEnableScreenShot);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "EnableScreenShot");
    sValue.Format("%d", m_bGUIsInScreenShot);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "GUIsInScreenShot");
    sValue.Format("%d", m_bAutoSave);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options", "AutoSave");
    sValue.Format("%d", m_bMouseLook);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options", "Mouse Look");
    sValue.Format("%d", m_bAutoLevelUpNPCs);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Auto Level Up NPCs");
    sValue.Format("%d", m_nDifficulty);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Difficulty Level");
    sValue.Format("%d", m_nMouseSensitivity);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Mouse Sensitivity");
    sValue.Format("%d", m_bMouseTeleporting);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Enable Mouse Teleporting To Buttons");
    sValue.Format("%d", m_bStatusSummary);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Status Summary");
    sValue.Format("%d", m_bFloatingNumbers);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Floating Numbers");
    sValue.Format("%d", m_bMiniMap);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options", "Mini Map");
    sValue.Format("%d", m_bSubtitles);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options", "Subtitles");
    sValue.Format("%d", m_bTutorialPopups);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Tutorial Popups");
    sValue.Format("%d", m_bHideUnequippable);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Hide Unequippable");
    sValue.Format("%d", m_bUseSmallFonts);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Use Small Fonts");
    sValue.Format("%d", m_bHideInGameGUI);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Hide InGame GUI");
    sValue.Format("%d", m_bAutopauseNewTargetSelected);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                       "New Target Selected");
    sValue.Format("%d", m_bAutopauseActionMenu);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                       "Action Menu");
    sValue.Format("%d", m_bAutopausePartyKilled);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                       "Party Killed");
    sValue.Format("%d", m_bAutopauseEnemySighted);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                       "Enemy Sighted");
    sValue.Format("%d", m_bAutopauseMineSighted);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                       "Mine Sighted");
    sValue.Format("%d", m_bAutopauseEndOfCombatRound);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                       "End Of Combat Round");
    sValue.Format("%d", m_nBrightness);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                       "Brightness");
    sValue.Format("%d", m_nShadows);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Graphics Options", "Shadows");
    sValue.Format("%d", m_nSoftShadows);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                       "Soft Shadows");
    sValue.Format("%d", m_nGrass);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Graphics Options", "Grass");
    sValue.Format("%d", m_nTexQual);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                       "Texture Quality");
    sValue.Format("%d", m_nAntiAlias);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                       "Anti Aliasing");
    sValue.Format("%d", m_nFrameBuffer);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                       "Frame Buffer");
    sValue.Format("%d", m_nAnisotropy);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                       "Anisotropy");
    sValue.Format("%d", m_nVSync);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Graphics Options", "V-Sync");
    sValue.Format("%d", m_nMovieVolume);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Sound Options",
                       "Movie Volume");
    sValue.Format("%d", m_nSoundEffectsVolume);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Sound Options",
                       "Sound Effects Volume");
    sValue.Format("%d", m_nVoiceVolume);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Sound Options",
                       "Voiceover Volume");
    sValue.Format("%d", m_nMusicVolume);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Sound Options",
                       "Music Volume");
    CExoString sMovie;
    do
    {
        sMovie.Format("Movie%2d", i);
        sValue.Format("%d", m_aMoviesShown[i]);
        cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Movies Shown",
                           sMovie.CStr());
        i++;
    } while (i < 11);
    sValue.Format("%f", m_fKeyboardCameraDPS);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Keyboard Camera DPS");
    sValue.Format("%f", m_fKeyboardCameraAcceleration);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Keyboard Camera Acceleration");
    sValue.Format("%f", m_fKeyboardCameraDeceleration);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Keyboard Camera Deceleration");
    sValue.Format("%d", m_bEnableTooltips);
    cIni.WriteIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                       "Enable Tooltips");
    return 1;
}
// PC 0x0061D1C0  Xbox 0x0005F5E0
void CClientOptions::ApplySoundOptions()
{
    if (g_pExoSound != 0)
    {
        g_pExoSound->SetMusicVolume(m_nMusicVolume * 0.01f, 0);
        g_pExoSound->SetDialogVolume(m_nVoiceVolume * 0.01f);
        g_pExoSound->SetSoundEffectVolume(m_nSoundEffectsVolume * 0.01f);
    }
}

#ifndef _XBOX
void CClientOptions::SetMovieShown(const CExoString& sMovie, int bShown)  // PC 0x0061D230
{
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pMovies2DA);
    C2DA* pMovies = p2DArrays->m_pMovies2DA;
    int nRow = pMovies->GetRowIndex(sMovie);
    if (nRow < 0)
    {
        char* pDot = strrchr(sMovie.CStr(), '.');
        char* pSlash = strrchr(sMovie.CStr(), '\\');
        if (pDot == 0 || pSlash == 0)
        {
            return;
        }
        unsigned int nLength = strlen(sMovie.CStr());
        int nDot = pDot - sMovie.CStr() + 1;
        int nSlash = pSlash - sMovie.CStr() + 1;
        CExoString sName = sMovie.Right(nLength - nSlash);
        sName = sName.Left(nDot - nSlash - 1);
        p2DArrays = g_pRules->m_p2DArrays;
        pMovies = p2DArrays->m_pMovies2DA;
        nRow = (unsigned char)pMovies->GetRowIndex(sName);
        if (nRow < 0)
        {
            return;
        }
    }
    unsigned int nByte = (unsigned int)nRow >> 3;
    if (nByte >= 11)
    {
        return;
    }
    int nBit = nRow - 8 * nByte;
    if (bShown != 0)
    {
        m_aMoviesShown[nByte] = (1 << nBit) | m_aMoviesShown[nByte];
    }
    else
    {
        m_aMoviesShown[nByte] = ~(1 << nBit) & m_aMoviesShown[nByte];
    }
    g_pAppManager->m_pClientExoApp->SaveOptions();
}
#endif
// PC 0x0061D3D0  Xbox 0x0005F740
int CClientOptions::GetMovieShown(const CResRef& sMovie)
{
    CExoString sName;
    sMovie.CopyToString(sName);
    if (sName.CompareNoCase(g_sUnlockedMovie) != 0)
    {
        return 1;
    }

    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pMovies2DA);
    C2DA* const pMovies = p2DArrays->m_pMovies2DA;
    int nRow = pMovies->GetRowIndex(sName);
    if (nRow < 0)
    {
        return 0;
    }
    unsigned char nByte = nRow / 8;
    if (nByte >= 11)
    {
        return 0;
    }
    unsigned char nBit = nRow - 8 * nByte;
    return (unsigned char)m_aMoviesShown[nByte] & (1 << nBit);
}
// PC 0x0061D4E0
void CClientOptions::SetDefaultGameplayOptions()
{
    m_nDifficulty = 1;
    m_bAutoLevelUpNPCs = 0;
    m_bMouseLook = 0;
    m_bAutoSave = 1;
    m_bReverseMinigameYAxis = 0;
    m_bCombatMovement = 1;
}
// PC 0x0061D500
void CClientOptions::SetDefaultAutopauseOptions()
{
    m_bAutopauseEndOfCombatRound = 0;
    m_bAutopauseEnemySighted = 1;
    m_bAutopauseMineSighted = 1;
    m_bAutopausePartyKilled = 1;
    m_bAutopauseActionMenu = 0;
    m_bAutopauseNewTargetSelected = 1;
}
// PC 0x0061D520
void CClientOptions::SetDefaultAdvancedGraphicsOptions()
{
    m_nFrameBuffer = 1;
    m_nAnisotropy = 1;
    m_nShaderLevel = 1;
    m_nTexQual = 0;
    m_nVSync = 0;
    m_nAntiAlias = 0;
    m_nSoftShadows = 1;
}
// PC 0x0061D540
void CClientOptions::SetDefaultSoundOptions()
{
    m_nMusicVolume = 85;
    m_nVoiceVolume = 85;
    m_nSoundEffectsVolume = 85;
    m_nMovieVolume = 100;
}
// PC 0x0061D550
int CClientOptions::SetDefaultAdvancedSoundOptions()
{
    int nEAXMode;
#ifdef _XBOX
    nEAXMode = 3;
    m_nEAXMode = 3;
#else
    if (g_pExoSound->GetBestEAXAvailable() > 3)
    {
        nEAXMode = 3;
        m_nEAXMode = 3;
    }
    else
    {
        nEAXMode = g_pExoSound->GetBestEAXAvailable();
        m_nEAXMode = nEAXMode;
    }
#endif
    m_nEAXFlags = 0;
    return nEAXMode;
}
// PC 0x0061D590  Xbox 0x0005F8F0
float CClientOptions::GetGammaSetting()
{
    return m_fGammaMax - (m_fGammaMax - m_fGammaMin) * (m_nBrightness * 0.01f);
}
// PC 0x0061D5B0
float CClientOptions::GetMouseSenSetting()
{
    float fPercent = m_nMouseSensitivity * 0.01f;
    return fPercent * 45.0f + 10.0f;
}
// PC 0x0061D5E0
float CClientOptions::GetCameraKeyboardDPS()
{
    if (KeyboardCameraRotationSpeed != -1.0f)
    {
        return KeyboardCameraRotationSpeed;
    }
    return m_fKeyboardCameraDPS;
}
// PC 0x0061D610
float CClientOptions::GetCameraKeyboardAcceleration()
{
    if (KeyboardCameraRotationAccl != -1.0f)
    {
        return KeyboardCameraRotationAccl;
    }
    return m_fKeyboardCameraAcceleration;
}
// PC 0x0061D640
float CClientOptions::GetCameraKeyboardDeceleration()
{
    if (KeyboardCameraRotationDecel != -1.0f)
    {
        return KeyboardCameraRotationDecel;
    }
    return m_fKeyboardCameraDeceleration;
}
// PC 0x0061D670
void CClientOptions::SetEnableHardwareMouseCursor(int bEnabled)
{
    if (g_pAppManager != 0)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        if (pClientExoApp != 0)
        {
            if (bEnabled)
            {
                pClientExoApp->SetMouseMode(0);
            }
            else
            {
                pClientExoApp->SetMouseMode(1);
            }
            m_bHardwareMouseCursor = bEnabled;
        }
    }
}
// PC 0x0061D6B0
void CClientOptions::SetShadows(unsigned long nShadows)
{
    m_nShadows = nShadows;
    if (nShadows != 0)
    {
        AurEnableShadows();
        return;
    }
    AurDisableShadows();
}
// PC 0x0061D6D0
void CClientOptions::SetSoftShadows(unsigned long nSoftShadows)
{
    m_nSoftShadows = nSoftShadows;
    if (nSoftShadows != 0)
    {
        AurEnableSoftShadows();
        return;
    }
    AurDisableSoftShadows();
}
// PC 0x0061D6F0
void CClientOptions::SetGrass(unsigned long nGrass)
{
    m_nGrass = nGrass;
    if (nGrass != 0)
    {
        AurDisableGrass();
    }
    else
    {
        AurEnableGrass();
    }
}

#ifndef _XBOX
void CClientOptions::SetAntiAlias(unsigned char nAntiAlias)  // PC 0x0061D710
{
    m_nAntiAlias = nAntiAlias;
    AurSetAntiAliasingMode(nAntiAlias);
}
#endif
// PC 0x0061D730
int CClientOptions::SetTexQual(unsigned char nTexQual)
{
    m_nTexQual = nTexQual;
    return g_pAppManager->m_pClientExoApp->SetTexturePack(nTexQual);
}

#ifndef _XBOX
void CClientOptions::SetFrameBuffer(unsigned long nFrameBuffer)  // PC 0x0061D750
{
    CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    m_nFrameBuffer = nFrameBuffer;
    if (nFrameBuffer != 0)
    {
        AurEnableFrameBufferEffects();
        if (pParty != 0)
        {
            for (unsigned char i = 0; i < pParty->m_nCharacters; i++)
            {
                CSWCCreature* pCreature = pParty->GetCharacter(i);
                if (pCreature != 0 &&
                    pCreature->HasVisualEffectApplied(8002) != 0 &&
                    (unsigned char)
                            AurCheckFrameBufferModificationsAvailable() != 0)
                {
                    pCreature->DeleteVisualEffect(8002);
                    Vector vOffset;
                    pCreature->AddVisualEffect(8000, 0, 2130706432, 2130706432,
                                               0, 0, vOffset);
                }
            }
        }
    }
    else
    {
        AurDisableFrameBufferEffects();
        if (pParty != 0)
        {
            for (unsigned char i = 0; i < pParty->m_nCharacters; i++)
            {
                CSWCCreature* pCreature = pParty->GetCharacter(i);
                if (pCreature != 0 &&
                    pCreature->HasVisualEffectApplied(8000) != 0 &&
                    (unsigned char)
                            AurCheckFrameBufferModificationsAvailable() != 0)
                {
                    pCreature->DeleteVisualEffect(8000);
                    Vector vOffset;
                    pCreature->AddVisualEffect(8002, 0, 2130706432, 2130706432,
                                               0, 0, vOffset);
                }
            }
        }
    }
}
// PC 0x0061D8C0
void CClientOptions::SetAnisotropy(unsigned long nAnisotropy)
{
    m_nAnisotropy = nAnisotropy;
    SetAnisotropicFilter(nAnisotropy);
}
// PC 0x0061D8E0
void CClientOptions::SetVSync(unsigned long nVSync)
{
    m_nVSync = nVSync;
    if (nVSync != 0)
    {
        AurSetVSyncState(1);
    }
    else
    {
        AurSetVSyncState(0);
    }
}
#endif
// PC 0x0061D910
void CClientOptions::SetUseSmallFonts(int bSmallFonts)
{
    m_bUseSmallFonts = bSmallFonts;
    m_nFontMode = (bSmallFonts == 0) + 1;
    if (g_pGuiMan != 0)
    {
#ifndef _XBOX
        g_pGuiMan->UpdateAllFonts();
#endif
    }
}
// PC 0x0061D950
void CClientOptions::SetDefaultFeedbackOptions()
{
    m_bHideUnequippable = 0;
    m_bTutorialPopups = 1;
    m_bSubtitles = 1;
    m_bMiniMap = 1;
    m_bFloatingNumbers = 1;
    m_bStatusSummary = 1;
    m_bDialogSubtitles = 0;
    m_bTooltipHelp = 1;
    m_bUseSmallFonts = 0;
    m_bHideInGameGUI = 0;
    m_bEnableTooltips = 1;
    SetUseSmallFonts(m_bUseSmallFonts);
}
// PC 0x0061D9A0
int CClientOptions::SetDefaultGraphicsOptions()
{
    m_nBrightness = 57;
    m_fGammaMin = 0.23f;
    m_fGammaMax = 2.0f;
    m_nShadows = 1;
    m_nGrass = 1;
    m_nTexturePack = 1;
    return 1;
}
// PC 0x0061D9D0
void CClientOptions::SetDefaultMouseOptions()
{
    m_bReverseMouseButtons = 0;
    m_nMouseSensitivity = 44;
}
// PC 0x0061D9E0
void CClientOptions::SetDefaultKeyboardCamera()
{
    if (KeyboardCameraRotationSpeed != -1.0f)
    {
        KeyboardCameraRotationSpeed = 200.0f;
    }
    m_fKeyboardCameraDPS = 200.0f;
    if (KeyboardCameraRotationAccl != -1.0f)
    {
        KeyboardCameraRotationAccl = 500.0f;
    }
    m_fKeyboardCameraAcceleration = 500.0f;
    if (KeyboardCameraRotationDecel != -1.0f)
    {
        KeyboardCameraRotationDecel = 2000.0f;
    }
    m_fKeyboardCameraDeceleration = 2000.0f;
}
// PC 0x0061DA60
void CClientOptions::SetDefaultOptions()
{
    SetDefaultGameplayOptions();
    SetDefaultFeedbackOptions();
    SetDefaultAutopauseOptions();
    m_nMusicVolume = 85;
    m_nVoiceVolume = 85;
    m_nSoundEffectsVolume = 85;
    SetDefaultGraphicsOptions();
    SetDefaultAdvancedGraphicsOptions();
    m_nMovieVolume = 100;
    SetDefaultMouseOptions();
    m_bEnableScreenShot = 0;
    m_bGUIsInScreenShot = 1;
    SetDefaultKeyboardCamera();
    memset(m_aMoviesShown, 0, sizeof(m_aMoviesShown));
    m_bEnableCheats = 0;
    m_nCheatFlags = 0;
    m_nDebugFlags = 0;
}
#ifndef _XBOX
CClientOptions::CClientOptions()  // PC 0x0061DB60  Xbox 0x00060270
{
    m_nCameraMode = CClientOptions::CAMERA_MODE_ON_A_STICK;
    m_nCameraStyle = 1;
    m_nAmbientSoundOptions = DEFAULT_AMBIENT_SOUND_OPTIONS;
    m_nGeometryFadeMode = 0;
    m_nGoreLevel = 2;
    m_bVideoOption = 1;
    m_bAutoDetectVideo = 0;
    m_bFullScreen = 0;
    m_bPlayMovies = 1;
    m_bPlaySounds = 1;
    m_nObjectShadowMode = OBJECT_SHADOWS_ALL;
    m_fCameraDistance = 3.0f;
    m_fCameraHeight = 1.0f;
    m_nCombatSpeed = 3;
    m_nGameSpeed = 2;
    m_nWindMode = WIND_PLAYER;
    m_nFontMode = 2;
    m_fCameraFollowRate = 5.0f;
    m_fGuiScale = 1.0f;
    if (g_pGuiMan != 0)
    {
#ifndef _XBOX
        g_pGuiMan->m_fToolTipDelay = 1.0f;
#endif
    }
    m_bEnableTooltips = 1;
    m_bMouseTeleporting = 1;
    SetDefaultOptions();
}
#endif
int CClientOptions::LoadOptions()  // PC 0x0061DBE0
{
    SetDefaultOptions();
    CExoIni cIni;
    CExoString sValue;
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Difficulty Level"))
    {
        m_nDifficulty = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Auto Level Up NPCs"))
    {
        SetAutoLevelUpNPCs(sValue.AsINT());
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Mouse Look"))
    {
        m_bMouseLook = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options", "AutoSave"))
    {
        m_bAutoSave = sValue.AsINT();
        int bAutoSave = m_bAutoSave != 0;
        m_bAutoSave = bAutoSave;
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Reverse Minigame YAxis"))
    {
        m_bReverseMinigameYAxis = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Combat Movement"))
    {
        m_bCombatMovement = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Mouse Sensitivity"))
    {
        m_nMouseSensitivity = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Enable Mouse Teleporting To Buttons"))
    {
        m_bMouseTeleporting = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Hide Unequippable"))
    {
        m_bHideUnequippable = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Tutorial Popups"))
    {
        m_bTutorialPopups = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Subtitles"))
    {
        m_bSubtitles = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options", "Mini Map"))
    {
        m_bMiniMap = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Floating Numbers"))
    {
        m_bFloatingNumbers = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Status Summary"))
    {
        m_bStatusSummary = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Use Small Fonts"))
    {
        SetUseSmallFonts(sValue.AsINT());
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Hide InGame GUI"))
    {
        m_bHideInGameGUI = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                          "End Of Combat Round"))
    {
        m_bAutopauseEndOfCombatRound = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                          "Enemy Sighted"))
    {
        m_bAutopauseEnemySighted = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                          "Mine Sighted"))
    {
        m_bAutopauseMineSighted = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                          "Party Killed"))
    {
        m_bAutopausePartyKilled = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                          "Action Menu"))
    {
        m_bAutopauseActionMenu = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Autopause Options",
                          "New Target Selected"))
    {
        m_bAutopauseNewTargetSelected = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                          "Brightness"))
    {
        m_nBrightness = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                          "Shadows"))
    {
        SetShadows(sValue.AsINT());
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                          "AllowSoftShadows"))
    {
#ifndef _XBOX
        if (sValue.AsINT() == 1)
        {
            AurATIUnlockSoftShadowsForHotFix();
        }
#endif
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                          "Soft Shadows"))
    {
        SetSoftShadows(sValue.AsINT());
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                          "Grass"))
    {
        SetGrass(sValue.AsINT());
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                          "Texture Quality"))
    {
        unsigned char nTexQual = sValue.AsINT();
        m_nTexQual = nTexQual;
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->SetTexturePack(nTexQual);
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                          "Frame Buffer"))
    {
#ifndef _XBOX
        SetFrameBuffer(sValue.AsINT());
#endif
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Graphics Options",
                          "V-Sync"))
    {
#ifndef _XBOX
        SetVSync(sValue.AsINT());
#endif
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Sound Options",
                          "Music Volume"))
    {
        m_nMusicVolume = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Sound Options",
                          "Voiceover Volume"))
    {
        m_nVoiceVolume = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Sound Options",
                          "Sound Effects Volume"))
    {
        m_nSoundEffectsVolume = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Sound Options",
                          "Movie Volume"))
    {
        m_nMovieVolume = sValue.AsINT();
    }
    ApplySoundOptions();
    CExoString sMovie;
    for (int i = 0; i < 11; i++)
    {
        sMovie.Format("Movie%2d", i);
        cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Movies Shown",
                          sMovie.CStr());
        m_aMoviesShown[i] = sValue.AsINT();
    }
    m_bEnableCheats = 0;
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "EnableCheats"))
    {
        m_bEnableCheats = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Keyboard Camera DPS"))
    {
        float fValue = sValue.AsFLOAT();
        if (KeyboardCameraRotationSpeed != -1.0f)
        {
            KeyboardCameraRotationSpeed = fValue;
        }
        m_fKeyboardCameraDPS = fValue;
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Keyboard Camera Acceleration"))
    {
        float fValue = sValue.AsFLOAT();
        if (KeyboardCameraRotationAccl != -1.0f)
        {
            KeyboardCameraRotationAccl = fValue;
        }
        m_fKeyboardCameraAcceleration = fValue;
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Keyboard Camera Deceleration"))
    {
        float fValue = sValue.AsFLOAT();
        if (KeyboardCameraRotationDecel != -1.0f)
        {
            KeyboardCameraRotationDecel = fValue;
        }
        m_fKeyboardCameraDeceleration = fValue;
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Enable Tooltips"))
    {
        m_bEnableTooltips = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "Reverse Mouse Buttons"))
    {
        m_bReverseMouseButtons = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "EnableScreenShot"))
    {
        m_bEnableScreenShot = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor.ini", "Game Options",
                          "GUIsInScreenShot"))
    {
        m_bGUIsInScreenShot = sValue.AsINT();
    }
    return 1;
}

#ifdef _XBOX
#include "LaunchData.h"

struct SXboxGammaRange
{
    float m_fMin;
    float m_fMax;
    float m_fDefault;
};
static SXboxGammaRange s_aXboxGammaRange[3] =
{
    { 0.1f, 2.0f, 1.0f },
    { 0.1f, 2.0f, 1.0f },
    { 0.1f, 2.0f, 1.0f }
};
// Xbox 0x0005F860
void CClientOptions::SetDefaultVideoOptions()
{
    unsigned long nFlags = XGetVideoFlags();
    int nMode;
    if ((nFlags & XC_VIDEO_FLAGS_HDTV_480p) || (nFlags & XC_VIDEO_FLAGS_HDTV_720p) ||
        (nFlags & XC_VIDEO_FLAGS_HDTV_1080i))
    {
        nMode = 2;
    }
    else if ((nFlags & XC_VIDEO_FLAGS_WIDESCREEN) || (nFlags & XC_VIDEO_FLAGS_LETTERBOX))
    {
        nMode = 1;
    }
    else
    {
        nMode = 0;
    }
    float fMin = s_aXboxGammaRange[nMode].m_fMin;
    float fMax = s_aXboxGammaRange[nMode].m_fMax;
    m_nBrightness = (unsigned char)((s_aXboxGammaRange[nMode].m_fDefault - fMin) / (fMax - fMin) * 100.0f);
    m_fGammaMin = fMin;
    m_fGammaMax = fMax;
}
// Xbox 0x00060180
void CClientOptions::SetDefaults()
{
    m_nXboxUnmappedBit32 = 0;
    m_bAutopauseNewTargetSelected = 1;
    m_nDifficulty = 1;
    m_bAutoLevelUpNPCs = 0;
    m_bMouseLook = 0;
    m_bAutoSave = 1;
    m_bReverseMinigameYAxis = 0;
    m_bCombatMovement = 1;
    m_bHideUnequippable = 0;
    m_bTutorialPopups = 1;
    m_bSubtitles = 1;
    m_bMiniMap = 1;
    m_bFloatingNumbers = 1;
    m_nXboxUnmappedBit26 = 1;
    m_bStatusSummary = 1;
    m_bAutopauseEndOfCombatRound = 0;
    m_bAutopauseEnemySighted = 1;
    m_bAutopausePartyKilled = 1;
    m_bAutopauseActionMenu = 0;
    m_nMusicVolume = 85;
    m_nVoiceVolume = 85;
    m_nSoundEffectsVolume = 85;
    m_nMovieVolume = 85;
    SetDefaultVideoOptions();
    memset(m_aMoviesShown, 0, sizeof(m_aMoviesShown));
}
// Xbox 0x0005FAD0
void CClientOptions::LoadFromGFF(CResGFF* pGFF, CResStruct* pStruct)
{
    int bSuccess;

    unsigned char nDefault;
    unsigned char nDifficulty = m_nDifficulty;
    m_nDifficulty = pGFF->ReadFieldBYTE(pStruct, "Difficulty", bSuccess,
                                        nDifficulty);
    m_bAutoLevelUpNPCs =
        pGFF->ReadFieldBYTE(pStruct, "AutoLevel", bSuccess, m_bAutoLevelUpNPCs);
    m_bMouseLook =
        pGFF->ReadFieldBYTE(pStruct, "AutoCentre", bSuccess, m_bMouseLook);
    m_bAutoSave =
        pGFF->ReadFieldBYTE(pStruct, "AutoSave", bSuccess, m_bAutoSave);

    m_bReverseMinigameYAxis =
        pGFF->ReadFieldBYTE(pStruct, "Reverse", bSuccess, m_bMouseLook);
    m_bCombatMovement =
        pGFF->ReadFieldBYTE(pStruct, "CombatMove", bSuccess, m_bCombatMovement);
    m_bHideUnequippable =
        pGFF->ReadFieldBYTE(pStruct, "HideUnequippable", bSuccess, m_bHideUnequippable);
    m_bTutorialPopups =
        pGFF->ReadFieldBYTE(pStruct, "ShowTutorial", bSuccess, m_bTutorialPopups);
    m_bSubtitles =
        pGFF->ReadFieldBYTE(pStruct, "ShowSubtitles", bSuccess, m_bSubtitles);
    m_bMiniMap =
        pGFF->ReadFieldBYTE(pStruct, "ShowMiniMap", bSuccess, m_bMiniMap);
    m_bFloatingNumbers =
        pGFF->ReadFieldBYTE(pStruct, "Rumble", bSuccess, m_bFloatingNumbers);
    m_nXboxUnmappedBit26 =
        pGFF->ReadFieldBYTE(pStruct, "ShowFloaty", bSuccess, m_nXboxUnmappedBit26);
    m_bStatusSummary =
        pGFF->ReadFieldBYTE(pStruct, "ShowSummary", bSuccess, m_bStatusSummary);
    m_bAutopauseEndOfCombatRound =
        pGFF->ReadFieldBYTE(pStruct, "PauseEndRound", bSuccess, m_bAutopauseEndOfCombatRound);
    m_bAutopauseEnemySighted =
        pGFF->ReadFieldBYTE(pStruct, "PauseEnemy", bSuccess, m_bAutopauseEnemySighted);
    m_bAutopausePartyKilled =
        pGFF->ReadFieldBYTE(pStruct, "PausePartyKilled", bSuccess, m_bAutopausePartyKilled);
    m_bAutopauseActionMenu =
        pGFF->ReadFieldBYTE(pStruct, "PauseHorzMenu", bSuccess, m_bAutopauseActionMenu);
    m_nXboxUnmappedBit32 =
        pGFF->ReadFieldBYTE(pStruct, "PauseVertMenu", bSuccess, m_nXboxUnmappedBit32);
    m_bAutopauseNewTargetSelected =
        pGFF->ReadFieldBYTE(pStruct, "PauseTriggers", bSuccess, m_bAutopauseNewTargetSelected);
    nDefault = m_nBrightness;
    m_nBrightness = pGFF->ReadFieldBYTE(pStruct, "Gamma", bSuccess, nDefault);
    nDefault = m_nMusicVolume;
    m_nMusicVolume = pGFF->ReadFieldBYTE(pStruct, "VolumeMusic", bSuccess, nDefault);
    nDefault = m_nVoiceVolume;
    m_nVoiceVolume = pGFF->ReadFieldBYTE(pStruct, "VolumeVO", bSuccess, nDefault);
    nDefault = m_nSoundEffectsVolume;
    m_nSoundEffectsVolume = pGFF->ReadFieldBYTE(pStruct, "VolumeFX", bSuccess, nDefault);
    nDefault = m_nMovieVolume;
    m_nMovieVolume = pGFF->ReadFieldBYTE(pStruct, "VolumeMovie", bSuccess, nDefault);
    if (g_pExoSound != NULL)
    {
        g_pExoSound->SetMusicVolume(m_nMusicVolume * 0.01f, 0);
        g_pExoSound->SetDialogVolume(m_nVoiceVolume * 0.01f);
        g_pExoSound->SetSoundEffectVolume(m_nSoundEffectsVolume * 0.01f);
    }
    pGFF->ReadFieldVOID(pStruct, m_aMoviesShown, sizeof(m_aMoviesShown),
                        "MoviesShown", bSuccess, m_aMoviesShown);
    if (!bSuccess)
    {
        memset(m_aMoviesShown, 0, sizeof(m_aMoviesShown));
    }
}
// Xbox 0x0005F200
void CClientOptions::SaveToGFF(CResGFF* pGFF, CResStruct* pStruct)
{
    pGFF->WriteFieldBYTE(pStruct, m_nDifficulty, "Difficulty");
    pGFF->WriteFieldBYTE(pStruct, m_bAutoLevelUpNPCs, "AutoLevel");
    pGFF->WriteFieldBYTE(pStruct, m_bMouseLook, "AutoCentre");
    pGFF->WriteFieldBYTE(pStruct, m_bAutoSave, "AutoSave");
    pGFF->WriteFieldBYTE(pStruct, m_bReverseMinigameYAxis, "Reverse");
    pGFF->WriteFieldBYTE(pStruct, m_bCombatMovement, "CombatMove");
    pGFF->WriteFieldBYTE(pStruct, m_bHideUnequippable, "HideUnequippable");
    pGFF->WriteFieldBYTE(pStruct, m_bTutorialPopups, "ShowTutorial");
    pGFF->WriteFieldBYTE(pStruct, m_bSubtitles, "ShowSubtitles");
    pGFF->WriteFieldBYTE(pStruct, m_bMiniMap, "ShowMiniMap");
    pGFF->WriteFieldBYTE(pStruct, m_bFloatingNumbers, "Rumble");
    pGFF->WriteFieldBYTE(pStruct, m_nXboxUnmappedBit26, "ShowFloaty");
    pGFF->WriteFieldBYTE(pStruct, m_bStatusSummary, "ShowSummary");
    pGFF->WriteFieldBYTE(pStruct, m_bAutopauseEndOfCombatRound, "PauseEndRound");
    pGFF->WriteFieldBYTE(pStruct, m_bAutopauseEnemySighted, "PauseEnemy");
    pGFF->WriteFieldBYTE(pStruct, m_bAutopausePartyKilled, "PausePartyKilled");
    pGFF->WriteFieldBYTE(pStruct, m_bAutopauseActionMenu, "PauseHorzMenu");
    pGFF->WriteFieldBYTE(pStruct, m_nXboxUnmappedBit32, "PauseVertMenu");
    pGFF->WriteFieldBYTE(pStruct, m_bAutopauseNewTargetSelected, "PauseTriggers");
    pGFF->WriteFieldBYTE(pStruct, m_nBrightness, "Gamma");
    pGFF->WriteFieldBYTE(pStruct, m_nMusicVolume, "VolumeMusic");
    pGFF->WriteFieldBYTE(pStruct, m_nVoiceVolume, "VolumeVO");
    pGFF->WriteFieldBYTE(pStruct, m_nSoundEffectsVolume, "VolumeFX");
    pGFF->WriteFieldBYTE(pStruct, m_nMovieVolume, "VolumeMovie");
    pGFF->WriteFieldVOID(pStruct, m_aMoviesShown, sizeof(m_aMoviesShown),
                         "MoviesShown");
}
// Xbox 0x0005F460
void CClientOptions::GetOptionsData(KOTOR_LAUNCH_OPTIONS_DATA* pData)
{
    pData->m_nDifficultyOption = m_nDifficulty;
    pData->m_bAutoLevelNPCs = m_bAutoLevelUpNPCs;
    pData->m_bAutoCentre = m_bMouseLook;
    pData->m_bAutoSave = m_bAutoSave;
    pData->m_bReverse = m_bReverseMinigameYAxis;
    pData->m_bCombatMove = m_bCombatMovement;
    pData->m_bHideUnequippable = m_bHideUnequippable;
    pData->m_bShowTutorial = m_bTutorialPopups;
    pData->m_bShowSubtitles = m_bSubtitles;
    pData->m_bShowMiniMap = m_bMiniMap;
    pData->m_bRumble = m_bFloatingNumbers;
    pData->m_bShowFloaty = m_nXboxUnmappedBit26;
    pData->m_bShowSummary = m_bStatusSummary;
    pData->m_bEndRound = m_bAutopauseEndOfCombatRound;
    pData->m_bEnemySighted = m_bAutopauseEnemySighted;
    pData->m_bPartyKilled = m_bAutopausePartyKilled;
    pData->m_bUseHorzMenu = m_bAutopauseActionMenu;
    pData->m_bUseVertMenu = m_nXboxUnmappedBit32;
    pData->m_bTrigger = m_bAutopauseNewTargetSelected;
    pData->m_nGamma = m_nBrightness;
    pData->m_nVolumeMusic = m_nMusicVolume;
    pData->m_nVolumeVO = m_nVoiceVolume;
    pData->m_nVolumeFX = m_nSoundEffectsVolume;
    pData->m_nVolumeMovie = m_nMovieVolume;
    memcpy(pData->m_aMoviesShown, m_aMoviesShown, sizeof(m_aMoviesShown));
}
// Xbox 0x0005FF90
void CClientOptions::SetOptionsData(const KOTOR_LAUNCH_OPTIONS_DATA* pData)
{
    m_nDifficulty = pData->m_nDifficultyOption;
    m_bAutoLevelUpNPCs = pData->m_bAutoLevelNPCs;
    m_bMouseLook = pData->m_bAutoCentre;
    m_bAutoSave = pData->m_bAutoSave;
    m_bReverseMinigameYAxis = pData->m_bReverse;
    m_bCombatMovement = pData->m_bCombatMove;
    m_bHideUnequippable = pData->m_bHideUnequippable;
    m_bTutorialPopups = pData->m_bShowTutorial;
    m_bSubtitles = pData->m_bShowSubtitles;
    m_bMiniMap = pData->m_bShowMiniMap;
    m_bFloatingNumbers = pData->m_bRumble;
    m_nXboxUnmappedBit26 = pData->m_bShowFloaty;
    m_bStatusSummary = pData->m_bShowSummary;
    m_bAutopauseEndOfCombatRound = pData->m_bEndRound;
    m_bAutopauseEnemySighted = pData->m_bEnemySighted;
    m_bAutopausePartyKilled = pData->m_bPartyKilled;
    m_bAutopauseActionMenu = pData->m_bUseHorzMenu;
    m_nXboxUnmappedBit32 = pData->m_bUseVertMenu;
    m_bAutopauseNewTargetSelected = pData->m_bTrigger;
    m_nBrightness = pData->m_nGamma;
    m_nMusicVolume = pData->m_nVolumeMusic;
    m_nVoiceVolume = pData->m_nVolumeVO;
    m_nSoundEffectsVolume = pData->m_nVolumeFX;
    m_nMovieVolume = pData->m_nVolumeMovie;
    if (g_pExoSound != NULL)
    {
        g_pExoSound->SetMusicVolume(m_nMusicVolume * 0.01f, 0);
        g_pExoSound->SetDialogVolume(m_nVoiceVolume * 0.01f);
        g_pExoSound->SetSoundEffectVolume(m_nSoundEffectsVolume * 0.01f);
    }
    memcpy(m_aMoviesShown, pData->m_aMoviesShown, sizeof(m_aMoviesShown));
}
// Xbox 0x0005F650
void CClientOptions::SetMovieShown(const CResRef& sMovie, int bShown)
{
    CExoString sName;
    sMovie.CopyToString(sName);
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pMovies2DA);
    C2DA* const pMovies = p2DArrays->m_pMovies2DA;
    int nRow = pMovies->GetRowIndex(sName);
    if (nRow >= 0)
    {
        unsigned char nByte = nRow / 8;
        if (nByte < 11)
        {
            unsigned char nBit = nRow - 8 * nByte;
            if (bShown != 0)
            {
                m_aMoviesShown[nByte] |= (1 << nBit);
            }
            else
            {
                m_aMoviesShown[nByte] &= ~(1 << nBit);
            }
            g_pAppManager->m_pClientExoApp->SaveOptions();
        }
    }
}
#endif

#ifdef _XBOX
#include "nwcmodule.h"
#include "nwcarea.h"
#include "aurrendergl.h"
// Xbox 0x0005EFB0
void CClientOptions::SetGeometryFadeMode(unsigned char nMode)
{
    if (m_nGeometryFadeMode != nMode)
    {
        if (g_pAppManager->m_pClientExoApp->GetModule() != NULL &&
            g_pAppManager->m_pClientExoApp->GetModule()->GetArea() != NULL)
        {
            g_pAppManager->m_pClientExoApp->GetModule()->GetArea()->SetGeometryFadeMode(nMode);
        }
        m_nGeometryFadeMode = nMode;
    }
}
// Xbox 0x0005F060
void CClientOptions::SetNumShadowCastingLights(unsigned char nLights)
{
    if (m_nCombatSpeed != nLights)
    {
        m_nCombatSpeed = nLights;
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        if (pModule != NULL && pModule->GetArea() != NULL)
        {
            pModule->GetArea()->m_nUnknown230 = 1;
        }
    }
}
// Xbox 0x0005F0A0
void CClientOptions::SetVSyncEnabled(int bEnabled)
{
    m_bVSync = (bEnabled != 0);
}
// Xbox 0x0005F0C0
void CClientOptions::SetAnisotropicFiltering(int bEnabled)
{
    m_bAnisotropicFiltering = (bEnabled != 0);
}
// Xbox 0x0005F0E0
void CClientOptions::SetShinyWater(int bEnabled)
{
    m_bShinyWater = bEnabled;
}
// Xbox 0x0005F100
void CClientOptions::SetTruform(int bEnabled)
{
    m_bTruform = bEnabled;
}
// Xbox 0x0005F120
void CClientOptions::SetAntiAliasingMode(unsigned char nMode)
{
    switch (nMode)
    {
    case 0: m_nXboxUnmapped11 = 0; break;
    case 1: m_nXboxUnmapped11 = 1; break;
    case 2: m_nXboxUnmapped11 = 2; break;
    case 3: m_nXboxUnmapped11 = 3; break;
    case 4: m_nXboxUnmapped11 = 4; break;
    default: m_nXboxUnmapped11 = 0; break;
    }
}
// Xbox 0x0005F170
void CClientOptions::SetGrassEnabled(int bEnabled)
{
    enablegrass = (bEnabled != 0);
    m_bGrass = bEnabled;
}
// Xbox 0x0005F1A0
void CClientOptions::SetFastGrass(int bEnabled)
{
    m_bFastGrass = (bEnabled != 0);
}

int CClientOptions::GetShinyWaterSupported()
{
    return 0;
}
// Xbox 0x0005F980
void CClientOptions::SetWindMode(unsigned char nMode)
{
    enum { XBOX_CLIENT_OBJECT_TYPE_CREATURE = 5 };
    if (m_nWindMode != nMode)
    {
        CSWCObject* pObject;
        CSWCCreature* pPlayer = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
        unsigned long oidPlayer;
        if (pPlayer != NULL)
        {
            oidPlayer = pPlayer->m_nId;
        }
        else
        {
            oidPlayer = CGameObjectArray::INVALIDOBJID;
        }
        if (g_pAppManager->m_pClientExoApp->GetModule() != NULL)
        {
            CSWCArea* pArea = g_pAppManager->m_pClientExoApp->GetModule()->GetArea();
            if (pArea != NULL)
            {
                for (int i = 0; i < pArea->m_lstObjectIds.num; i++)
                {
                    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
                    CGameObject* pGameObject = pClientExoApp->GetGameObject(pArea->m_lstObjectIds.element[i]);
                    if (pGameObject != NULL)
                    {
                        pObject = pGameObject->AsSWCObject();
                        if (pObject != NULL && pObject->m_nObjectType == XBOX_CLIENT_OBJECT_TYPE_CREATURE &&
                            pObject->m_nFadeState == CSWCObject::FADE_STATE_VISIBLE &&
                            oidPlayer != pObject->m_nId)
                        {
                            switch (nMode)
                            {
                            case 0:
                            case WIND_PLAYER:
                                pObject->SetWind(0, 0.0f);
                                break;
                            case WIND_ALL:
                                pObject->SetWind(1, creaturewindintensity);
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (pPlayer != NULL && pPlayer->m_nFadeState == CSWCObject::FADE_STATE_VISIBLE)
        {
            switch (nMode)
            {
            case 0:
                pObject->SetWind(0, 0.0f);
                break;
            case WIND_PLAYER:
            case WIND_ALL:
                pObject->SetWind(1, creaturewindintensity);
                break;
            }
        }
        m_nWindMode = nMode;
    }
}
#endif

#ifdef _XBOX
// PC 0x0061DB60  Xbox 0x00060270
CClientOptions::CClientOptions()
{
    m_nCameraStyle = 1;
    m_nCameraMode = CClientOptions::CAMERA_MODE_ON_A_STICK;
    m_nAmbientSoundOptions = DEFAULT_AMBIENT_SOUND_OPTIONS;
    m_nGeometryFadeMode = 0;
    m_nGoreLevel = 2;
    m_nXboxUnmappedBit0 = 1;
    m_nXboxUnmappedBit1 = 0;
    m_bAutoDetectVideo = 1;
    m_bVideoOption = 1;
    m_bFullScreen = 0;
    m_bPlayMovies = 1;
    m_bWindowed = 1;
    m_bPlaySounds = 1;
    m_nObjectShadowMode = OBJECT_SHADOWS_ALL;
    m_fCameraDistance = 3.0f;
    m_fCameraHeight = 1.0f;
    m_nCombatSpeed = 3;
    m_nGameSpeed = 2;
    m_nWindMode = WIND_ALL;
    SetDefaults();
}
#endif
#ifdef _XBOX

#include "aurconsole.h"
#include <stdlib.h>
// Xbox 0x0005F730
void UnlockMovie(char* szArgs)
{
    g_sUnlockedMovie = szArgs;
}
ConsoleFunc ConFUnlockMovie("UnlockMovie", UnlockMovie);
// Xbox 0x0005F910
int setnumshadowlights(char* szArgs)
{
    if (g_pAppManager != NULL && g_pAppManager->m_pClientExoApp != NULL &&
        g_pAppManager->m_pClientExoApp->GetClientOptions() != NULL)
    {
        if (strlen(szArgs) > 0)
        {
            g_pAppManager->GetClientExoApp()->GetClientOptions()->SetNumShadowCastingLights(
                (unsigned char)atoi(szArgs));
        }
    }
    return g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCombatSpeed;
}
ConsoleFunc ConFsetnumshadowlights("setnumshadowlights", setnumshadowlights);
// Xbox 0x000601D0
char* showallmovies(char* szArgs)
{
    CClientOptions* pOptions = g_pAppManager->m_pClientExoApp->GetClientOptions();
    for (int i = 0; i < 11; i++)
    {
        pOptions->m_aMoviesShown[i] = 0xff;
    }
    return "Success";
}
ConsoleFunc ConFshowallmovies("showallmovies", showallmovies);
// Xbox 0x00060200
int setcreaturewindmode(char* szArgs)
{
    if (g_pAppManager != NULL && g_pAppManager->m_pClientExoApp != NULL &&
        g_pAppManager->m_pClientExoApp->GetClientOptions() != NULL)
    {
        if (strlen(szArgs) > 0)
        {
            g_pAppManager->GetClientExoApp()->GetClientOptions()->SetWindMode((unsigned char)atoi(szArgs));
        }
    }
    return g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nWindMode;
}
ConsoleFunc ConFsetcreaturewindmode("setcreaturewindmode", setcreaturewindmode);
#endif
