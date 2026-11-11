#ifndef __LAUNCHDATA_H__
#define __LAUNCHDATA_H__

#include "exobase.h"
#include "serverexoappinternal.h"

struct KOTOR_LAUNCH_OPTIONS_DATA
{
    unsigned int m_bAutoLevelNPCs : 1;
    unsigned int m_bAutoCentre : 1;
    unsigned int m_bAutoSave : 1;
    unsigned int m_bReverse : 1;
    unsigned int m_bCombatMove : 1;
    unsigned int m_bHideUnequippable : 1;
    unsigned int m_bShowTutorial : 1;
    unsigned int m_bShowSubtitles : 1;
    unsigned int m_bShowMiniMap : 1;
    unsigned int m_bRumble : 1;
    unsigned int m_bShowFloaty : 1;
    unsigned int m_bEndRound : 1;
    unsigned int m_bEnemySighted : 1;
    unsigned int m_bPartyKilled : 1;
    unsigned int m_bUseHorzMenu : 1;
    unsigned int m_bUseVertMenu : 1;
    unsigned int m_bTrigger : 1;
    unsigned int m_bShowSummary : 1;
    unsigned char m_nDifficultyOption;
    unsigned char m_nGamma;
    float m_fGammaMin;
    float m_fGammaMax;
    unsigned char m_nVolumeMusic;
    unsigned char m_nVolumeVO;
    unsigned char m_nVolumeFX;
    unsigned char m_nVolumeMovie;
    unsigned char m_nNumMovies;
    unsigned char m_aMoviesShown[11];
};

class KOTOR_LAUNCH_DATA
{
public:
    enum { MAGIC = 0xBADA55 };
    enum
    {
        MODULETRANS_NONE = 0,
        MODULETRANS_MODULE = 1,
        MODULETRANS_LOADGAME = 2
    };

    KOTOR_LAUNCH_DATA();
    // Xbox 0x0001C4F0
    void SetLoadScreen(const CExoString& sLoadScreen)
    {
        EXOXBOXASSERT((unsigned int)sLoadScreen.GetLength() < sizeof(m_szLoadScreen));
        strcpy(m_szLoadScreen, sLoadScreen.CStr());
    }

    void SetXboxSavePath(const CExoString& sXboxSavePath)
    {
        EXOXBOXASSERT((unsigned int)sXboxSavePath.GetLength() < sizeof(m_szXboxSavePath));
        strcpy(m_szXboxSavePath, sXboxSavePath.CStr());
    }
    void SetModuleName(const CExoString& sModuleName)  // Xbox 0x0010F7A0
    {
        EXOXBOXASSERT((unsigned int)sModuleName.GetLength() < sizeof(m_szModuleName));
        strcpy(m_szModuleName, sModuleName.CStr());
    }
    void SetLoadMusic(const CExoString& sLoadMusic)  // Xbox 0x0010F7E0
    {
        EXOXBOXASSERT((unsigned int)sLoadMusic.GetLength() < sizeof(m_szLoadMusic));
        strcpy(m_szLoadMusic, sLoadMusic.CStr());
    }
    void SetStartWaypoint(const CExoString& sStartWaypoint)  // Xbox 0x0010F820
    {
        EXOXBOXASSERT((unsigned int)sStartWaypoint.GetLength() < sizeof(m_szStartWaypoint));
        strcpy(m_szStartWaypoint, sStartWaypoint.CStr());
    }
    void SetMovie1(const CExoString& sMovie)  // Xbox 0x0010F860
    {
        EXOXBOXASSERT((unsigned int)sMovie.GetLength() < sizeof(m_szMovie1));
        strcpy(m_szMovie1, sMovie.CStr());
    }
    void SetMovie2(const CExoString& sMovie)  // Xbox 0x0010F8A0
    {
        EXOXBOXASSERT((unsigned int)sMovie.GetLength() < sizeof(m_szMovie2));
        strcpy(m_szMovie2, sMovie.CStr());
    }
    void SetMovie3(const CExoString& sMovie)  // Xbox 0x0010F8E0
    {
        EXOXBOXASSERT((unsigned int)sMovie.GetLength() < sizeof(m_szMovie3));
        strcpy(m_szMovie3, sMovie.CStr());
    }
    void SetMovie4(const CExoString& sMovie)  // Xbox 0x0010F920
    {
        EXOXBOXASSERT((unsigned int)sMovie.GetLength() < sizeof(m_szMovie4));
        strcpy(m_szMovie4, sMovie.CStr());
    }
    void SetMovie5(const CExoString& sMovie)  // Xbox 0x0010F960
    {
        EXOXBOXASSERT((unsigned int)sMovie.GetLength() < sizeof(m_szMovie5));
        strcpy(m_szMovie5, sMovie.CStr());
    }
    void SetMovie6(const CExoString& sMovie)  // Xbox 0x0010F9A0
    {
        EXOXBOXASSERT((unsigned int)sMovie.GetLength() < sizeof(m_szMovie6));
        strcpy(m_szMovie6, sMovie.CStr());
    }
    void SetTime(unsigned long nYear, unsigned char nMonth, unsigned char nDay, unsigned char nHour,  // Xbox 0x0010F9E0
                 unsigned short nMinute, unsigned short nSecond, unsigned short nMillisecond)
    {
        m_nTimeYear = nYear;
        m_nTimeMonth = nMonth;
        m_nTimeDay = nDay;
        m_nTimeHour = nHour;
        m_nTimeMinute = nMinute;
        m_nTimeSecond = nSecond;
        m_nTimeMillisecond = nMillisecond;
    }
    void SetStatusSummary(const CStatusSummary& cStatusSummary)  // Xbox 0x00214C00
    {
        m_cStatusSummary = cStatusSummary;
    }
    void SetOptionsData(const KOTOR_LAUNCH_OPTIONS_DATA& cOptionsData)  // Xbox 0x0001C530
    {
        m_cOptionsData = cOptionsData;
        m_bOptionsDataValid = 1;
    }

    CExoString GetXboxSavePath() { return CExoString(m_szXboxSavePath); }
    CExoString GetModuleName() { return CExoString(m_szModuleName); }
    CExoString GetLoadScreen() { return CExoString(m_szLoadScreen); }
    CExoString GetLoadMusic() { return CExoString(m_szLoadMusic); }
    CExoString GetStartWaypoint() { return CExoString(m_szStartWaypoint); }
    CExoString GetMovie1() { return CExoString(m_szMovie1); }
    CExoString GetMovie2() { return CExoString(m_szMovie2); }
    CExoString GetMovie3() { return CExoString(m_szMovie3); }
    CExoString GetMovie4() { return CExoString(m_szMovie4); }
    CExoString GetMovie5() { return CExoString(m_szMovie5); }
    CExoString GetMovie6() { return CExoString(m_szMovie6); }

    unsigned long m_nMagic;
    unsigned long m_nModuleTrans;
    unsigned int m_bRenderQA : 1;
    unsigned int m_bLowDiskMode : 1;
    unsigned int m_bMinGlobalRim : 1;
    unsigned int m_bLoadingSave : 1;
    unsigned int m_bLoadAutoSave : 1;
    unsigned char m_nBootFlags;
    unsigned long m_nAutoCalInfo;
    unsigned long m_nPortBinding;
    char m_szModuleName[32];
    char m_szLoadScreen[32];
    char m_szLoadMusic[32];
    char m_szStartWaypoint[64];
    char m_szMovie1[64];
    char m_szMovie2[64];
    char m_szMovie3[64];
    char m_szMovie4[64];
    char m_szMovie5[64];
    char m_szMovie6[64];
    unsigned long m_nTimeYear;
    unsigned char m_nTimeMonth;
    unsigned char m_nTimeDay;
    unsigned char m_nTimeHour;
    unsigned short m_nTimeMinute;
    unsigned short m_nTimeSecond;
    unsigned short m_nTimeMillisecond;
    unsigned long m_nTimePauseDay;
    unsigned long m_nTimePauseTime;
    KOTOR_LAUNCH_OPTIONS_DATA m_cOptionsData;
    unsigned long m_bOptionsDataValid;
    unsigned long m_nSaveSlot;
    char m_szXboxSavePath[64];
    CStatusSummary m_cStatusSummary;
    unsigned long m_nLoadScreenHint;
    unsigned char m_nGamePlayHint;
    unsigned char m_nStoryHint;
    unsigned long m_nHintTimer1;
    unsigned long m_nHintTimer2;
    unsigned long m_nLoadScreenHintUpdateTimer;
    __int64 m_nCurrentSessionStartFILETIME;
};

extern int g_bLowDiskMode;
extern unsigned char g_nBootFlags;
extern __int64 g_nCurrentSessionStartFILETIME;
// Xbox 0x000124A0
inline KOTOR_LAUNCH_DATA::KOTOR_LAUNCH_DATA()
{
    m_nMagic = MAGIC;
    m_bRenderQA = 0;
    m_bLowDiskMode = g_bLowDiskMode;
    m_nBootFlags = g_nBootFlags;
    m_nModuleTrans = MODULETRANS_NONE;
    m_nAutoCalInfo = 0;
    memset(m_szModuleName, 0, sizeof(m_szModuleName));
    memset(m_szLoadScreen, 0, sizeof(m_szLoadScreen));
    memset(m_szLoadMusic, 0, sizeof(m_szLoadMusic));
    memset(m_szStartWaypoint, 0, sizeof(m_szStartWaypoint));
    memset(m_szMovie1, 0, sizeof(m_szMovie1));
    memset(m_szMovie2, 0, sizeof(m_szMovie2));
    memset(m_szMovie3, 0, sizeof(m_szMovie3));
    memset(m_szMovie4, 0, sizeof(m_szMovie4));
    memset(m_szMovie5, 0, sizeof(m_szMovie5));
    memset(m_szMovie6, 0, sizeof(m_szMovie6));
    m_nSaveSlot = 0;
    memset(m_szXboxSavePath, 0, sizeof(m_szXboxSavePath));
    m_bMinGlobalRim = 0;
    m_bLoadingSave = 0;
    m_bLoadAutoSave = 0;
    m_nTimeYear = 0;
    m_nTimeMonth = 0;
    m_nTimeDay = 0;
    m_nTimeHour = 0;
    m_nTimeMinute = 0;
    m_nTimeSecond = 0;
    m_nTimeMillisecond = 0;
    m_nTimePauseDay = 0;
    m_nTimePauseTime = 0;
    memset(&m_cOptionsData, 0, sizeof(m_cOptionsData));
    memset(&m_cStatusSummary, 0, sizeof(m_cStatusSummary));
    m_nLoadScreenHint = 0;
    m_nGamePlayHint = 0;
    m_nStoryHint = 0;
    m_nHintTimer1 = 0;
    m_nHintTimer2 = 0;
    m_bOptionsDataValid = 0;
    m_nCurrentSessionStartFILETIME = g_nCurrentSessionStartFILETIME;
}

#endif
