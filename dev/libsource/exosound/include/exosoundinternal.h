#ifndef __EXOSOUNDINTERNAL_H__
#define __EXOSOUNDINTERNAL_H__

#include "exobase.h"
#include "math3d.h"
#ifdef _XBOX
#include <xtl.h>
#include <dsound.h>
#else
#include "mss.h"
#endif
#include <stdio.h>
#include "exoresman.h"
class CExoLinkedListInternal;
class CExoLinkedListNode;
class CExoSoundSourceInternal;
class CExoStreamingSoundSourceInternal;
class CExoStreamingSoundSource;

struct _EAXLISTENERPROPERTIES;


class CAllowedSoundProvider
{
public:
    int m_nProvider;
    float m_f2D3DBias;
    int m_bHardware;
    int m_nEAX;
};
class CResRef;
class Vector;




#define EXOSOUND_RECENT_SOUND_DISTANCE 0.5f


class CExoSoundPriorityGroup
{
public:

    static const int MUSIC_GROUP = 2;

    CExoSoundPriorityGroup();

    // PC 0x004015A0
    ~CExoSoundPriorityGroup() {}



    unsigned char GetMaxPlaying();
    unsigned char GetPlaying();
    void SetVolumeDistances(float fMin, float fMax)
    {
        m_fMaxVolumeDist = fMax;
        m_fMinVolumeDist = fMin;
    }

    int m_bInterrupt;
    unsigned char m_nMaxPlaying;
    unsigned char m_nPlaying;
    unsigned char m_nPriority;
    unsigned char m_nVolume;
    float m_fMinVolumeDist;
    float m_fMaxVolumeDist;
    float m_fPitchVariance;
    unsigned short m_nFadeTime;
};

#ifdef _XBOX


class C2DVoice
{
public:
    CExoSoundSourceInternal* m_pSource;
    LPDIRECTSOUNDBUFFER m_pBuffer;


    int m_bReleasing;

    int m_bSourceGone;
};

class C3DVoice
{
public:
    CExoSoundSourceInternal* m_pSource;
    LPDIRECTSOUNDBUFFER m_pBuffer;
    int m_bReleasing;
    int m_bSourceGone;
};

class CExoXboxStream;
class CExoXboxStreamManager;


#define EXOSOUND_MIN_VOLUME (-6400)

#define EXOSOUND_RESTYPE_WMA 11

#define EXOSOUND_MIN_FADE_TIME 100




long ExoSoundVolumeToMillibels(unsigned char nVolume, long nMinVolume);
unsigned char ExoSoundMillibelsToVolume(long nMillibels, long nMinVolume);



inline void ExoSoundCopyWaveFormat(XBOXADPCMWAVEFORMAT* pFormat, const WAVEFORMATEX* pFileFormat)
{
    pFormat->wfx.cbSize = pFileFormat->cbSize;
    pFormat->wfx.nAvgBytesPerSec = pFileFormat->nAvgBytesPerSec;
    pFormat->wfx.nBlockAlign = pFileFormat->nBlockAlign;
    pFormat->wfx.nChannels = pFileFormat->nChannels;
    pFormat->wfx.nSamplesPerSec = pFileFormat->nSamplesPerSec;
    pFormat->wfx.wBitsPerSample = pFileFormat->wBitsPerSample;
    pFormat->wfx.wFormatTag = pFileFormat->wFormatTag;
    pFormat->wSamplesPerBlock = pFileFormat->wFormatTag == WAVE_FORMAT_XBOX_ADPCM ? 64 : 0;
}
#else
class C2DVoice
{
public:
    CExoSoundSourceInternal* m_pSource;
    HSAMPLE m_hSample;


    int m_bLocked;

    int m_nState;
};

class C3DVoice
{
public:
    CExoSoundSourceInternal* m_pSource;
    H3DSAMPLE m_hSample;


    int m_bLocked;

    int m_nState;
};
#endif

class CExoSoundInternal
{
public:

#ifdef _XBOX

    static const DSI3DL2LISTENER CEXOSOUND_PRESETS[24];
#else
    static const _EAXLISTENERPROPERTIES CEXOSOUND_PRESETS[24];
#endif


    static const unsigned long RECENT_SOUND_TIME = 175;


    static const int MAX_VOICES = 64;
    static const int MIN_3D_VOICES = 16;
    static const int MIN_2D_VOICES = 24;



    enum OcclusionList
    {
        OCCLUSION_LIST_PLAYING = 1,
        OCCLUSION_LIST_STREAMING = 2
    };

    static const unsigned long OCCLUSION_TIME_BUDGET = 3;

#ifdef _XBOX


    int m_bEnvironmentEffects;
    int m_bInitialized;
    int m_bMuted;
    unsigned char m_nCurrentEnvironment;
    float m_fVolumeScale;
    float m_fVolumeScaleStart;
    float m_fVolumeScaleTarget;
    int m_nDesiredEnvironment;
    float m_fDesiredAmbientScale;
    unsigned char m_n2DVoices;
    unsigned char m_n3DVoices;
    unsigned char m_nPriorityGroups;
    C2DVoice* m_p2DVoices;
    C3DVoice* m_p3DVoices;
    CExoLinkedList<CExoSoundSourceInternal> m_lstDelayedOneShots;
    CExoLinkedList<CExoSoundSourceInternal> m_lstNonPlayingLooping;
    CExoLinkedList<CExoStreamingSoundSourceInternal>
        m_lstNonPlayingStreamingLooping;
    CExoLinkedList<CExoSoundSourceInternal> m_lstPlaying;
    CExoLinkedList<CExoStreamingSoundSourceInternal> m_lstStreaming;
    CExoLinkedList<CExoSoundSourceInternal> m_lstPendingDeletion;

    CExoLinkedList<C2DVoice> m_lstReleasing2DVoices;
    CExoLinkedList<C3DVoice> m_lstReleasing3DVoices;
    CExoSoundPriorityGroup* m_pPriorityGroups;
    unsigned long m_nMuteStartTime;
    int m_nMuteFadeTime;
    int m_nNextSourceId;
    int m_nUnMuteFadeTime;
    float m_f2D3DBias;
    float m_fDialogVolume;
    float m_fReverbLevel;
    float m_fTempMusicVolume;
    float m_fTempSoundEffectVolume;
    float m_fTempDialogVolume;
    float m_fMusicVolume;
    float m_fSoundEffectVolume;
    Vector m_vListenerOrientation;
    Vector m_vListenerUp;
    Vector m_vListenerPosition;
    CExoLinkedListNode* m_pOcclusionPosition;
    int m_nOcclusionList;
    unsigned long m_nOcclusionIdleFrames;
    unsigned long m_nMaxOcclusionIdleFrames;
    unsigned long m_nMaxOcclusionTime;
    unsigned long m_nMaxObstructionQueries;
    unsigned long m_nMaxCachedObstructionFrameTime;
    unsigned long m_nOcclusionTimeBudget;
    unsigned long m_nOcclusionTime;
    unsigned long m_nObstructionQueries;
    int m_nOcclusionState;

    unsigned char m_aXboxUnmappedD4[0x200];

    CExoXboxStreamManager* m_pStreamManager;
    unsigned char m_aXboxUnmapped2D8[0x208];
    LPDIRECTSOUND m_pDirectSound;
    unsigned char m_aXboxUnmapped4E4[0xC];
    unsigned long m_nLastRenderTime;
    int m_anSoundMode[10];
    int m_nSoundMode;
#else
    int m_bStreamingEnvironmentEffects;
    int m_bNonStreamingEnvironmentEffects;
    int m_bInitialized;
    int m_bMuted;

    unsigned char m_nCurrentEnvironment;
    char gap_11[3];
    float m_fVolumeScale;
    float m_fVolumeScaleStart;
    float m_fVolumeScaleTarget;
    int m_nDesiredEnvironment;
    float m_fDesiredAmbientScale;
    unsigned char m_n2DVoices;
    unsigned char m_n3DVoices;

    unsigned char m_nPriorityGroups;
    char gap_2B[1];
    C2DVoice* m_p2DVoices;
    C3DVoice* m_p3DVoices;
    CExoLinkedList<CExoSoundSourceInternal> m_lstDelayedOneShots;
    CExoLinkedList<CExoSoundSourceInternal> m_lstNonPlayingLooping;
    CExoLinkedList<CExoStreamingSoundSourceInternal>
        m_lstNonPlayingStreamingLooping;
    CExoLinkedList<CExoSoundSourceInternal> m_lstPlaying;
    CExoLinkedList<CExoStreamingSoundSourceInternal> m_lstStreaming;

    CExoLinkedList<CExoSoundSourceInternal> m_lstPendingDeletion;
    CExoSoundPriorityGroup* m_pPriorityGroups;
    unsigned long m_nMuteStartTime;
    int m_nMuteFadeTime;
    int m_nNextSourceId;
    int m_nUnMuteFadeTime;
    float m_f2D3DBias;
    float m_fDialogVolume;
    float m_fReverbLevel;
    float m_fTempMusicVolume;
    float m_fTempSoundEffectVolume;
    float m_fTempDialogVolume;
    float m_fMusicVolume;
    float m_fSoundEffectVolume;
    Vector m_vListenerOrientation;
    Vector m_vListenerUp;
    Vector m_vListenerPosition;
    CExoLinkedListNode* m_pOcclusionPosition;
    int m_nOcclusionList;

    unsigned long m_nOcclusionIdleFrames;
    unsigned long m_nMaxOcclusionIdleFrames;
    unsigned long m_nMaxOcclusionTime;
    unsigned long m_nMaxObstructionQueries;
    unsigned long m_nMaxCachedObstructionFrameTime;
    unsigned long m_nOcclusionTimeBudget;
    unsigned long m_nOcclusionTime;
    unsigned long m_nObstructionQueries;

    int m_nOcclusionState;

    CExoString m_s3DProviderName;
    CExoStringList m_CExoStringList_D8;

    CExoArrayList<HPROVIDER> m_a3DProviders;
    HPROVIDER m_h3DProvider;

    int m_bHardware;
    int m_nEAX;
    int m_nBestEAXAvailable;

    CExoArrayList<CAllowedSoundProvider*> m_aProviderConfigurations;

    unsigned long m_nLastRenderTime;
    int m_anSoundMode[10];
    int m_nSoundMode;
    int m_bPaused;


    CExoStreamingSoundSource* m_p3DOneShotStream;
#endif
    void SetListenerOrientation(Vector const&, Vector const&);
    Vector GetListenerOrientation();
    void SetListenerPosition(const Vector&);


    int GetDesiredEnvironment();



    float GetDesiredAmbientScale();


    void SetRolloffFactor(float fFactor);
    unsigned char GetNumber2DVoices();
    unsigned char GetNumber3DVoices();
    void Set2D3DBias(float fBias);
    float Get2D3DBias();
    Vector GetListenerPosition();
    void EnableStreamingEnvironmentEffects(int bEnabled);
    void EnableNonStreamingEnvironmentEffects(int bEnabled);
    int GetEAX();
    int GetBestEAXAvailable();
    void SetDesiredEnvironment(int nEnvironment);
    void SetDesiredAmbientScale(float fScale);
    int GetSoundMode();
    int GetLostFocusPaused();
    unsigned char GetNumPriorityGroups() { return m_nPriorityGroups; }
    CExoSoundPriorityGroup* GetPriorityGroup(unsigned char nGroup);
    float GetMusicVolume(int bTemporary);
    float GetSoundEffectVolume(int bTemporary);
    float GetDialogVolume(int bTemporary);


    void SetEnvironment(unsigned char);
    void SetMusicVolume(float, int, int);
    void SetSoundEffectVolume(float, int);
    void SetDialogVolume(float, int);
    void MuteSound();
    void UnMuteSound();
    int GetMutable(CExoSoundSourceInternal*);
#ifdef _XBOX
    int GetMutable(CExoXboxStream*);
#else
    int GetMutable(CExoStreamingSoundSourceInternal*);
#endif
    int GetSliderType(CExoSoundSourceInternal*);
#ifdef _XBOX
    int GetSliderType(CExoXboxStream*);
#else
    int GetSliderType(CExoStreamingSoundSourceInternal*);
#endif
    float GetSliderVolume(int, int);
    int GetPauseable(CExoSoundSourceInternal*);
    int GetPauseable(CExoStreamingSoundSourceInternal*);
    void ShutDown();
    int GetClosestRecentSound(Vector const&, Vector&, unsigned long&, float&);
    void PlaySourceOn3DVoice(CExoSoundSourceInternal*, C3DVoice*);
    void PlaySourceOn2DVoice(CExoSoundSourceInternal*, C2DVoice*);
    int PlayOneShotSound(CResRef const&, unsigned char, unsigned long,
                         unsigned char, float, float);
    int Play3DOneShotSound(CResRef const&, Vector, unsigned char, unsigned long,
                           unsigned char, float, float);
    CExoLinkedListNode* RemoveFromPlayingList(CExoSoundSourceInternal*);
    CExoLinkedListNode* RemoveFromNonPlayingLoopingList(
        CExoSoundSourceInternal*);
    CExoLinkedListNode* RemoveFromNonPlayingStreamingLoopingList(
        CExoStreamingSoundSourceInternal*);
    CExoLinkedListNode* RemoveFromDelayedOneShotList(CExoSoundSourceInternal*);
    CExoLinkedListNode* RemoveFromStreamingList(
        CExoStreamingSoundSourceInternal*);
    void AddToPlayingList(CExoSoundSourceInternal*);
    CExoLinkedListNode* AddToNonPlayingLoopingList(CExoSoundSourceInternal*);
    CExoLinkedListNode* AddToNonPlayingStreamingLoopingList(
        CExoStreamingSoundSourceInternal*);
    CExoLinkedListNode* AddToDelayedOneShotList(CExoSoundSourceInternal*);
    void LoadPriorityGroups();
    void InterruptPlayingSound(unsigned char);
    void InterruptPlayingStream(unsigned char);
    void ApplyNewVolume(int);
    void StopAllOneShots();
#ifdef _XBOX
    int PauseAllSounds();
#else
    int PauseAllSounds(int);
#endif
    int ResumeAllSounds();
#ifdef _XBOX
    void AddToStreamingList(CExoStreamingSoundSourceInternal*);
#else
    int AddToStreamingList(CExoStreamingSoundSourceInternal*);
#endif
    void SetSoundMode(int);
    CExoSoundInternal();

    void OcclusionRender(unsigned long);
    C3DVoice* GetFree3DVoice(unsigned char);
    C2DVoice* GetFree2DVoice(unsigned char);
    ~CExoSoundInternal();
    void Render();

    void LoadProviderPreferences();
    void Enumerate3DProviders();
#ifdef _XBOX


    int Initialize(const CExoString& s3DProvider, unsigned char n2DVoices, unsigned char n3DVoices);

    HRESULT LoadEffectsImage(const char* pszFileName);
    DWORD ParseWaveData(void* pWaveFile, XBOXADPCMWAVEFORMAT* pFormat, void** ppData);
    void DeleteStoppedStreams();
    void DetachReleasingVoices(CExoSoundSourceInternal*);
    int IsReleasing2DVoice(C2DVoice* pVoice);
    void AddReleasing2DVoice(C2DVoice* pVoice);
    int IsReleasing3DVoice(C3DVoice* pVoice);
    void AddReleasing3DVoice(C3DVoice* pVoice);


    void SetEnvironmentEffectLevel(float fLevel);
    void SetRollOffFactor(float fFactor);
    void SetSpeakerType(char nSpeakerType);
    void EnableEnvironmentEffects(int bEnabled);
    LPDIRECTSOUND GetDirectSound();
#else
    int Initialize(unsigned char, unsigned char, int, int);
#endif
};




class CExoLinkedListNode;

class CExoSoundSourceInternal : public CResHelper<CResWave, RESTYPE_WAV>
{
public:


    enum SourceListType
    {
        LIST_NONE = 0,
        LIST_NONPLAYING_LOOPING = 2
    };

    virtual ~CExoSoundSourceInternal();
    int IsPaused();
    int Demand();
    int ReleaseDemand();
    void DoObstructionWork();
    CExoSoundSourceInternal(CExoSoundInternal*);
    int GetWithinRangeOfListener();
    int IsPlaying();
    void SetLooping(int);
    int IsScalable();
    void Pause();
    void Resume();
    void ShutDownSource();
    void Stop();
    void SetPosition(Vector const&);
    void Set3D(int);
    void SetOneShotDelay(unsigned long);
    void StoreRestartSoundPosition();
    void CalculatePitchVarianceFrequency();
    void CalculateFixedVarianceFrequency();

    unsigned int Play(int, int);
    void SetVolume(unsigned char, int, float);
    int SetFixedVariance(float);
    void SetPitchVariance(float);
    void SetPriorityGroup(unsigned char);




    int GetPlayWhenFree();

    int GetLooping();
    void SetDistance(float fMaxDistance, float fMinDistance);
    unsigned long GetOneShotDelay();
    unsigned long GetDelayStart();
    unsigned char GetVolume();
    // PC 0x007846A0
    int GetRetryPlay() { return m_bRetryPlay; }



    int m_b3D;
#ifdef _XBOX
    int m_nUnknown20;
#else
    unsigned char m_aUnmapped20[4];
#endif
    int m_bLooping;
    int m_bPlayWhenFree;
    int m_bRetryPlay;
    int m_bDemanded;
    unsigned char m_nListType;

    unsigned char m_nPriorityGroup;
    C2DVoice* m_p2DVoice;
    C3DVoice* m_p3DVoice;
    int m_n40;
    int m_n44;
    CExoLinkedListNode* m_pListNode;
    CExoSoundInternal* m_pSound;
    unsigned long m_nSampleRate;
#ifdef _XBOX

    unsigned long m_nDataSize;
#else
    unsigned char m_aUnmapped4C[4];
#endif
    unsigned long m_nFixedRate;
    unsigned long m_nPitchRate;
    int m_nDuration;
    unsigned long m_nOneShotDelay;
    int m_nSourceId;
    unsigned long m_nDelayStart;
    float m_fFixedVariance;

    float m_fMinVolumeDist;
    float m_fMaxVolumeDist;
    float m_fPitchVariance;
    Vector m_vPosition;
#ifdef _XBOX
    unsigned char m_nGroupVolume;


    void* m_pData;
    DWORD m_nRestartPosition;
    int m_nVolume;
    XBOXADPCMWAVEFORMAT m_wfxFormat;
    Vector m_vObstructionListenerPosition;
    unsigned long m_nObstructionTime;
    float m_fObstruction;
#else
    unsigned char m_nGroupVolume;
    unsigned char m_nVolume;
    int m_nRestartPosition;


    Vector m_vObstructionListenerPosition;
    unsigned long m_nObstructionTime;


    float m_fObstruction;
#endif
};


#ifdef _XBOX
class CExoSoundInternal;








struct CExoXboxStreamPacket
{
    DWORD m_nStatus;
    DWORD m_nCompletedSize;
    DWORD m_nState;
    DWORD m_nLastPacket;
};

class CExoXboxStream
{
public:
    enum { NUM_PACKETS = 3 };
    enum
    {
        PACKET_FREE = 0xADEADCAB,
        PACKET_READ = 0xDEADDEAD
    };
    enum { STREAM_MAGIC = 0xBAADD00D };
    enum { WMA_LOOKAHEAD_SIZE = 0x10000 };

    CExoXboxStream(CExoSoundInternal* pSoundInternal);
    ~CExoXboxStream();

    int OpenWaveFile(XBOXADPCMWAVEFORMAT* pFormat, int bCached);
    int OpenWMAFile(XBOXADPCMWAVEFORMAT* pFormat);
    DWORD GetPacketSize(XBOXADPCMWAVEFORMAT* pFormat, int bWMA);
    int GetPendingPackets(DWORD* pnPending);
    HRESULT ReadPacket(DWORD nPacket);
    HRESULT SubmitPacket(DWORD nPacket);
    void SetLooping(int bLooping);
    void Set3D(int b3D);
    int CreateStream(XBOXADPCMWAVEFORMAT* pFormat, int bWMA);
    HRESULT Process();
    int IsPlaying();
    int IsActive();
    void Stop(int bKeepPlaying);
    void FadeAndStop(unsigned long nFadeTime);
    void Pause();
    void Resume();
    void SetPosition(const Vector& vPosition);
    void SetObstruction(float fObstruction);
    void CalculatePitchVarianceFrequency();
    void SetVolume(unsigned char nVolume, int nSliderMode, float fScale, int bDirect);
    void SetPriorityGroup(unsigned char nPriorityGroup);
    int SetFixedVariance(float fVariance);
    void SetPitchVariance(float fVariance);
    int Play(const CExoString& sFileName, unsigned short nResourceType, int bVoice, int bMusic, int bCached,
             int bPaused);

    int m_bPlaying;
    int m_bPlayFailed;
    DWORD m_nMagic;
    int m_bStopped;
    unsigned char m_nPriorityVolume;
    int m_nUnknown14;
    int m_bStarting;
    int m_bLooping;
    unsigned char m_nUnmapped20;
    unsigned char m_nPriorityGroup;
    CResRef m_resRef;
    int m_nUnmapped34;
    CExoSoundInternal* m_pSoundInternal;
    CExoString m_sFileName;
    unsigned long m_nFadeDuration;
    int m_nSourceId;
    unsigned long m_nFadeStartTime;
    unsigned char m_aUnmapped50[0xC];
    int m_nUnknown5C;
    int m_nUnmapped60;
    int m_bReverbEnabled;
    int m_b3D;
    int m_bPaused;
    int m_bEndOfFile;
    int m_bFinished;
    CExoXboxStreamPacket m_aPackets[NUM_PACKETS];
    DWORD m_nBlockAlign;
    DWORD m_nFileLength;
    DWORD m_nUnknownB0;
    DWORD m_nPacketSize;
    DWORD m_nPacketsRead;
    DWORD m_nPacketsSubmitted;
    HANDLE m_hFile;
    LPDIRECTSOUNDSTREAM m_pStream;
    int m_nVolume;
    void* m_pBuffer;
    unsigned short m_nResourceType;
    Vector m_vPosition;
    XFileMediaObject* m_pFileObject;
    float m_fMaxVolumeDist;
    float m_fMinVolumeDist;
    int m_bDeleted;
    DWORD m_nSampleRate;
    DWORD m_nFixedRate;
    DWORD m_nPitchRate;
    float m_fFixedVariance;
    float m_fPitchVariance;
    int m_bHasStartedStreaming;
};


class CExoXboxStreamCommand
{
public:
    enum
    {
        COMMAND_NONE = 0,
        COMMAND_STOP_ALL = 1,
        COMMAND_PLAY = 10,
        COMMAND_DELETE = 11,
        COMMAND_SET_LOOPING = 12,
        COMMAND_SET_3D = 13,
        COMMAND_SET_VOLUME = 14,
        COMMAND_STOP = 15,
        COMMAND_PAUSE = 16,
        COMMAND_RESUME = 17,
        COMMAND_SET_POSITION = 18,
        COMMAND_FADE_AND_STOP = 19,
        COMMAND_SET_OBSTRUCTION = 20,
        COMMAND_SET_PRIORITY_GROUP = 21,
        COMMAND_SET_PITCH_VARIANCE = 22,
        COMMAND_SET_FIXED_VARIANCE = 23
    };

    CExoXboxStreamCommand();

    ~CExoXboxStreamCommand();

    int m_nCommand;
    CExoXboxStream* m_pStream;
    CExoString m_sFileName;
    float m_fParam1;
    float m_fParam2;
    float m_fParam3;
    float m_fParam4;
};




class CExoXboxStreamManager
{
public:
    enum { QUEUE_SIZE = 1024 };

    enum { UPDATE_INTERVAL = 33 };
    enum { THREAD_STACK_SIZE = 0x10000 };

    CExoXboxStreamManager(CExoSoundInternal* pSoundInternal);

    int QueueCommand(CExoXboxStreamCommand cCommand);
    int GetNextCommand(CExoXboxStreamCommand& cCommand);
    int QueueDeletedStream(CExoXboxStream* pStream);
    int GetNextDeletedStream(CExoXboxStream** ppStream);
    void ShutDown();
    void RemoveStream(CExoXboxStream* pStream);
    void ProcessCommand(CExoXboxStreamCommand cCommand);
    void Update();
    void Initialize();

    int m_nUnmapped00;
    CExoLinkedList<CExoXboxStream> m_lstStreams;
    CExoSoundInternal* m_pSoundInternal;
    HANDLE m_hEvent;
    HANDLE m_hThread;
    float m_fVolumeScale;
    float m_fLastVolumeScale;
    float m_fUnknown1C;
    unsigned long m_nStartTime;
    CExoXboxStreamCommand m_aCommands[QUEUE_SIZE];
    CExoXboxStream* m_apDeletedStreams[QUEUE_SIZE];
    DWORD m_nCommandHead;
    DWORD m_nCommandTail;
    DWORD m_nDeletedHead;
    DWORD m_nDeletedTail;
};



class CExoStreamingSoundSourceInternal
{
public:
    CExoXboxStream* m_pStream;
    CResRef m_resRef;
    CExoString m_sFileName;
    CExoSoundInternal* m_pSoundInternal;
    int m_nSourceId;
    unsigned short m_nResourceType;
    int m_bLooping;
    unsigned char m_nPriorityGroup;
    int m_b3D;
    int m_nUnknown34;
    int m_nVolume;
    Vector m_vPosition;
    float m_fMaxVolumeDist;
    float m_fMinVolumeDist;
    Vector m_vObstructionListenerPosition;
    unsigned long m_nObstructionTime;
    float m_fOcclusion;
    unsigned long m_nFadeDuration;

    unsigned char GetPriorityGroup();
    int Get3D();
    int GetLooping();
    CResRef GetFilePlaying();
    void Set3D(int b3D);
    void SetDistance(float fMaxVolumeDist, float fMinVolumeDist);
    void SetPosition(const Vector& vPosition);
    CResRef& GetResRef();
    int GetResourceType();
    unsigned long GetFadeDuration() { return m_nFadeDuration; }
    unsigned char GetVolume(); CExoSoundInternal* GetSoundInternal() { return m_pSoundInternal; }
    CExoStreamingSoundSourceInternal(CExoSoundInternal* pSoundInternal);
    ~CExoStreamingSoundSourceInternal();
    int GetPlayFailed();
    int IsPlaying();
    int GetHasStartedStreaming();
    void SetLooping(int);
    void Stop();
    void Pause();
    void FadeAndStop(unsigned long);
    int GetWithinRangeOfListener();
    void DoObstructionWork();
    void SetVolume(unsigned char, int, float);
    int Play(CResRef const&, unsigned short, int, int, int);
    void SetPriorityGroup(unsigned char);
    void Resume();
    void SetReverbEnabled(int bEnabled);
    void SetPitchVariance(float fVariance);
    void SetFixedVariance(float fVariance);
};

#else
class CExoSoundInternal;

class CExoStreamingSoundSourceInternal
{
public:

    enum { LEGACY_WAVE_HEADER_SIZE = 470 };

    HSTREAM m_hStream;
    int m_n4;
    int m_bLooping;
    unsigned char m_nPriorityGroup;
    CResRef m_resRef;
    int m_bPlaying;
    CExoSoundInternal* m_pSoundInternal;
    CExoString m_sFileName;
    unsigned long m_nFadeDuration;
    int m_nSourceId;
    unsigned long m_nFadeStartTime;
    FILE* m_pFile;
    char m_aUnmapped3C[4];
    int m_nPausePosition;
    unsigned short m_nResourceType;
    unsigned char m_nVolume;
    char m_aUnmapped47[1];
    float m_fMinDistance;
    float m_fMaxDistance;
    unsigned char m_nPriorityVolume;
    char m_aUnmapped51[3];
    float m_fVolume;
    int m_b3D;
    Vector m_vPosition;
    int m_bReverbEnabled;
    int m_bReverbChanged;


    Vector m_vObstructionListenerPosition;
    unsigned long m_nObstructionTime;
    float m_fOcclusion;
    int m_n88;
    int SetReverbEnabled(int bEnabled);


    unsigned char GetPriorityGroup();
    int Get3D();
    int GetLooping();
    CResRef GetFilePlaying();
    void Set3D(int b3D);
    void SetDistance(float fMinDistance, float fMaxDistance);
    void SetPosition(Vector vPosition);
    CResRef& GetResRef();
    int GetResourceType();
    unsigned long GetFadeDuration() { return m_nFadeDuration; }  // PC 0x007846A0
    unsigned long GetFadeStartTime();
    unsigned char GetVolume();
    CExoStreamingSoundSourceInternal(CExoSoundInternal* pSoundInternal);
    ~CExoStreamingSoundSourceInternal();
    void InitializeSource();
    int GetPlayFailed();
    int IsPlaying();
    void SetLooping(int);
    int IsScalable();
    void Stop();
    void Pause();
    void FadeAndStop(unsigned long);
    int GetWithinRangeOfListener();
    void Update3DStream();
    void DoObstructionWork();
    void SetVolume(unsigned char, int, float);
    int Play(CResRef const&, unsigned short, int, int, int);
    void SetPriorityGroup(unsigned char);
    void Resume();
};
#endif

extern int g_bSoundPaused;





template void CResHelper<CResWave, RESTYPE_WAV>::SetResRef(const CResRef&, int);

#ifdef _XBOX


inline int CExoStreamingSoundSourceInternal::GetLooping()
{
    return m_bLooping;
}

inline int CExoStreamingSoundSourceInternal::Get3D() { return m_b3D; }

inline int CExoSoundSourceInternal::GetLooping() { return m_bLooping; }


inline float CExoSoundInternal::GetMusicVolume(int bTemporary)
{
    return bTemporary ? m_fTempMusicVolume : m_fMusicVolume;
}

inline float CExoSoundInternal::GetSoundEffectVolume(int bTemporary)
{
    return bTemporary ? m_fTempSoundEffectVolume : m_fSoundEffectVolume;
}

inline float CExoSoundInternal::GetDialogVolume(int bTemporary)
{
    return bTemporary ? m_fTempDialogVolume : m_fDialogVolume;
}
#endif

#endif
