#ifndef __CLIENTOPTIONS_H__
#define __CLIENTOPTIONS_H__

class CResRef;
#ifdef _XBOX
class CResGFF;
class CResStruct;
#endif
#include "exobase.h"
#include "exoresman.h"

class CSWGuiManager;

extern float creaturewindintensity;

class CClientOptions
{
public:
    CClientOptions();
    ~CClientOptions();

    enum { GORE_LEVEL_FULL = 2 };

    static const int AMBIENT_SOUND_OPTION_MASK = 3;

    static const int DEFAULT_AMBIENT_SOUND_OPTIONS = 16;

    enum { CAMERA_MODE_WORLD_CLICK_DISABLED = 5 };

    enum CameraMode
    {
        CAMERA_MODE_CHASE = 0, CAMERA_MODE_BG = 1, CAMERA_MODE_EQ = 2,

        CAMERA_MODE_ON_A_STICK = 3,
        CAMERA_MODE_DIALOG = 4,
        CAMERA_MODE_COMBAT_FACING = 6,

        CAMERA_MODE_NAVIGATE = 7
    };

    void SetCameraMode(unsigned char nMode);
    unsigned char GetCameraMode() { return m_nCameraMode; }

    void SetEAXMode(unsigned char nMode) { m_nEAXMode = nMode; }

    enum { TEXTURE_QUALITY_COUNT = 4 };
    void SetFullScreenEnabled(int bEnabled);
    void SetAutoLevelUpNPCs(int bEnabled);
    int SaveOptions();
    int LoadOptions();
    void ApplySoundOptions();
#ifdef _XBOX
    void SetMovieShown(const CResRef& sMovie, int bShown);
#else
    void SetMovieShown(const CExoString& sMovie, int bShown);
#endif
    int GetMovieShown(const CResRef& sMovie);
    void SetDefaultGameplayOptions();
    void SetDefaultAutopauseOptions();
    void SetDefaultAdvancedGraphicsOptions();
    void SetDefaultSoundOptions();
    int SetDefaultAdvancedSoundOptions();
    float GetGammaSetting();
    float GetMouseSenSetting();
    float GetCameraKeyboardDPS();
    float GetCameraKeyboardAcceleration();
    float GetCameraKeyboardDeceleration();
    void SetEnableHardwareMouseCursor(int bEnabled);
    void SetShadows(unsigned long nShadows);
    void SetSoftShadows(unsigned long nSoftShadows);
    void SetGrass(unsigned long nGrass);
    void SetAntiAlias(unsigned char nAntiAlias);
    int SetTexQual(unsigned char nTexQual);
    void SetFrameBuffer(unsigned long nFrameBuffer);
    void SetAnisotropy(unsigned long nAnisotropy);
    void SetVSync(unsigned long nVSync);
    void SetUseSmallFonts(int bSmallFonts);
    void SetDefaultFeedbackOptions();
    int SetDefaultGraphicsOptions();
    void SetDefaultMouseOptions();
    void SetDefaultKeyboardCamera();
    void SetDefaultOptions();
#ifdef _XBOX

    void SetDefaults();
    void LoadFromGFF(CResGFF* pGFF, CResStruct* pStruct);
    void SaveToGFF(CResGFF* pGFF, CResStruct* pStruct);

    void SetOptionsData(const struct KOTOR_LAUNCH_OPTIONS_DATA* pData);

    void GetOptionsData(struct KOTOR_LAUNCH_OPTIONS_DATA* pData);

    void SetDefaultVideoOptions();

    void SetGeometryFadeMode(unsigned char nMode);
    void SetNumShadowCastingLights(unsigned char nLights);
    void SetVSyncEnabled(int bEnabled);
    void SetAnisotropicFiltering(int bEnabled);
    void SetShinyWater(int bEnabled);
    void SetTruform(int bEnabled);
    void SetAntiAliasingMode(unsigned char nMode);
    void SetGrassEnabled(int bEnabled);
    void SetFastGrass(int bEnabled);
    void SetWindMode(unsigned char nMode);
    void SetGoreLevel(unsigned char nLevel) { m_nGoreLevel = nLevel; }
    void SetZoomSpeed(float fSpeed) { m_fCameraDistance = fSpeed; }
    void SetCameraTurnSpeed(float fSpeed) { m_fCameraHeight = fSpeed; }
    void SetVisualEffectsHigh(int bEnabled) { m_bHighQualityPersistentVFX = bEnabled; }
    void SetScreenEdgeCameraTurn(int bEnabled) { m_bPlaySounds = bEnabled; }
    int GetShinyWaterSupported();
#endif

#ifdef _XBOX

    unsigned int m_nXboxUnmappedBit0 : 1;
    unsigned int m_nXboxUnmappedBit1 : 1;
    unsigned int m_bAutoDetectVideo : 1;
    unsigned int m_bHardwareMouseCursor : 1;
    unsigned int m_bVideoOption : 1;
    unsigned int m_bFullScreen : 1;
    unsigned int m_bPlayMovies : 1;
    unsigned int m_bWindowed : 1;
    unsigned int m_bPlaySounds : 1;

    unsigned int m_bVSync : 1;
    unsigned int m_bTruform : 1;
    unsigned int m_bAnisotropicFiltering : 1;
    unsigned int m_bShinyWater : 1;
    unsigned int m_bGrass : 1;
    unsigned int m_bFastGrass : 1;
    unsigned int m_bHighQualityPersistentVFX : 1;
    unsigned int m_bAutoLevelUpNPCs : 1;
    unsigned int m_bMouseLook : 1;
    unsigned int m_bAutoSave : 1;
    unsigned int m_bReverseMinigameYAxis : 1;
    unsigned int m_bCombatMovement : 1;
    unsigned int m_bHideUnequippable : 1;
    unsigned int m_bTutorialPopups : 1;
    unsigned int m_bSubtitles : 1;
    unsigned int m_bMiniMap : 1;
    unsigned int m_bFloatingNumbers : 1;
    unsigned int m_nXboxUnmappedBit26 : 1;
    unsigned int m_bStatusSummary : 1;
    unsigned int m_bAutopauseEndOfCombatRound : 1;
    unsigned int m_bAutopauseEnemySighted : 1;
    unsigned int m_bAutopausePartyKilled : 1;
    unsigned int m_bAutopauseActionMenu : 1;
    unsigned int m_nXboxUnmappedBit32 : 1;
    unsigned int m_bAutopauseNewTargetSelected : 1;
    unsigned int m_nAutopauseBits : 30;
    unsigned char m_nGeometryFadeMode;
    enum ObjectShadowMode
    {
        OBJECT_SHADOWS_PLAYER = 1,
        OBJECT_SHADOWS_ALL = 2
    };
    unsigned char m_nObjectShadowMode;
    unsigned char m_nCameraMode;
    unsigned char m_nCameraStyle;
    unsigned char m_nCombatSpeed;
    unsigned char m_nGameSpeed;
    enum WindMode { WIND_PLAYER = 1, WIND_ALL = 2 };
    unsigned char m_nWindMode;
    unsigned char m_nGoreLevel;
    unsigned char m_nAmbientSoundOptions;
    unsigned char m_nXboxUnmapped11;
    unsigned char m_nDifficulty;
    unsigned char m_nBrightness;
    float m_fGammaMin;
    float m_fGammaMax;
    unsigned char m_nMusicVolume;
    unsigned char m_nVoiceVolume;
    unsigned char m_nSoundEffectsVolume;
    unsigned char m_nMovieVolume;
    unsigned char m_aMoviesShown[11];
    float m_fCameraDistance;
    float m_fCameraHeight;

    unsigned int m_bDialogSubtitles : 1;
    unsigned int m_bTooltipHelp : 1;
    unsigned int m_bUseSmallFonts : 1;
    unsigned int m_bHideInGameGUI : 1;
    unsigned int m_bEnableTooltips : 1;
    unsigned int m_bAutopauseMineSighted : 1;
    unsigned int m_bGUIsInScreenShot : 1;
    float m_fGuiScale;
    int m_bMouseTeleporting;
    unsigned long m_nShadows;
    unsigned long m_nSoftShadows;
    unsigned long m_nGrass;
    unsigned long m_nFrameBuffer;
    unsigned long m_nAnisotropy;
    int m_nShaderLevel;
    unsigned char m_nTexQual;
    unsigned char m_nTexturePack;
    unsigned long m_nVSync;
    unsigned char m_nAntiAlias;
    unsigned char m_nEAXMode;
    int m_nEAXFlags;
    unsigned char m_nMouseSensitivity;
    int m_bReverseMouseButtons;
    unsigned char m_bEnableScreenShot;
    int m_nInputFlags;
    unsigned char m_nFontMode;
    float m_fCameraFollowRate;
    int m_nCheatFlags;
    int m_nDebugFlags;
    int m_bEnableCheats;
    float m_fKeyboardCameraDPS;
    float m_fKeyboardCameraAcceleration;
    float m_fKeyboardCameraDeceleration;
#else
    unsigned int m_bAutoDetectVideo : 1;
    unsigned int m_bHardwareMouseCursor : 1;
    unsigned int m_bVideoOption : 1;
    unsigned int m_bFullScreen : 1;
    unsigned int m_bPlayMovies : 1;
    unsigned int m_bWindowed : 1;
    unsigned int m_bPlaySounds : 1;
    unsigned int m_bHighQualityPersistentVFX : 1;
    unsigned int m_nDisplayBits : 24;
    unsigned char m_nDifficulty;
    unsigned int m_bAutoLevelUpNPCs : 1;
    unsigned int m_bMouseLook : 1;
    unsigned int m_bAutoSave : 1;
    unsigned int m_bReverseMinigameYAxis : 1;
    unsigned int m_bCombatMovement : 1;
    unsigned int m_bReverseIngame : 1;
    unsigned int m_nGameplayBits : 26;
    float m_fGuiScale;
    int m_bMouseTeleporting;
    unsigned char m_n14;
    unsigned char m_n15;
    unsigned char m_n16;
    unsigned short m_n18;
    unsigned char m_n1A;
    unsigned char m_n1B;
    unsigned int m_bHideUnequippable : 1;
    unsigned int m_bTutorialPopups : 1;
    unsigned int m_bSubtitles : 1;
    unsigned int m_bMiniMap : 1;
    unsigned int m_bFloatingNumbers : 1;
    unsigned int m_bStatusSummary : 1;
    unsigned int m_bDialogSubtitles : 1;
    unsigned int m_bTooltipHelp : 1;
    unsigned int m_bUseSmallFonts : 1;
    unsigned int m_bHideInGameGUI : 1;
    unsigned int m_bEnableTooltips : 1;
    unsigned int m_bAutopauseEndOfCombatRound : 1;
    unsigned int m_bAutopauseEnemySighted : 1;
    unsigned int m_bAutopauseMineSighted : 1;
    unsigned int m_bAutopausePartyKilled : 1;
    unsigned int m_bAutopauseActionMenu : 1;
    unsigned int m_bAutopauseNewTargetSelected : 1;
    unsigned int m_nAutopauseBits : 15;
    unsigned char m_nBrightness;
    float m_fGammaMin;
    float m_fGammaMax;
    unsigned long m_nGrass;
    unsigned long m_nSoftShadows;
    unsigned long m_nShadows;
    unsigned long m_nFrameBuffer;
    unsigned long m_nAnisotropy;
    int m_nShaderLevel;
    unsigned char m_nTexQual;
    unsigned char m_nTexturePack;
    unsigned long m_nVSync;
    unsigned char m_nAntiAlias;
    unsigned char m_nMusicVolume;
    unsigned char m_nVoiceVolume;
    unsigned char m_nSoundEffectsVolume;
    unsigned char m_nMovieVolume;
    unsigned char m_nEAXMode;
    int m_nEAXFlags;
    unsigned char m_nMouseSensitivity;
    int m_bReverseMouseButtons;
    unsigned char m_bEnableScreenShot;
    unsigned char m_aMoviesShown[11];

    unsigned char m_nGeometryFadeMode;

    enum ObjectShadowMode
    {
        OBJECT_SHADOWS_PLAYER = 1,
        OBJECT_SHADOWS_ALL = 2
    };
    unsigned char m_nObjectShadowMode;
    unsigned char m_nCameraMode;
    unsigned char m_nCameraStyle;
    unsigned char m_nCombatSpeed;
    unsigned char m_nGameSpeed;

    enum WindMode { WIND_PLAYER = 1, WIND_ALL = 2 };
    unsigned char m_nWindMode;

    unsigned char m_nGoreLevel;

    unsigned char m_nAmbientSoundOptions;
    unsigned char m_bGUIsInScreenShot;
    float m_fCameraDistance;
    float m_fCameraHeight;
    unsigned char m_nFontMode;
    float m_fCameraFollowRate;
    int m_nCheatFlags;
    int m_nDebugFlags;
    int m_bEnableCheats;
    float m_fKeyboardCameraDPS;
    float m_fKeyboardCameraAcceleration;
    float m_fKeyboardCameraDeceleration;
#endif
};

extern CSWGuiManager* g_pGuiMan;
extern int g_nReInitializing;
extern CExoString g_sUnlockedMovie;
extern float KeyboardCameraRotationSpeed;
extern float KeyboardCameraRotationAccl;
extern float KeyboardCameraRotationDecel;

#endif
