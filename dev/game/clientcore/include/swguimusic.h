#ifndef __SWGUIMUSIC_H__
#define __SWGUIMUSIC_H__

#include "swguiabilitieschargen.h"

class CExoStreamingSoundSource;

struct CMusicItemInfo
{
    CMusicItemInfo();

    CExoString m_sFileName;
    unsigned long m_nStrRef;
};

#define TITLEMUSIC_NUM_PLANETS 11

class CSWGuiTitleMusic : public CSWGuiPanel
{
public:
    CSWGuiTitleMusic(CSWGuiManager* pManager);
    virtual ~CSWGuiTitleMusic();

#ifdef _XBOX
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelRemoved();

    void PopulateSongList();
    void StopMusic();
    void SetSongWindow(int nTopSong, int nSelectedSong);
    void SelectPreviousSong();
    void SelectNextSong(int bNoRefresh);
    void OnPlayPicked(CSWGuiControl* pControl);
    void OnLoopPicked(CSWGuiControl* pControl);
    void OnStopPicked(CSWGuiControl* pControl);
    void OnNextPicked(CSWGuiControl* pControl);

    enum { NUM_SONG_BUTTONS = 8 };

    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblUpArrow;
    CSWGuiLabel m_lblDownArrow;
    CSWGuiButton m_btnPlay;
    CSWGuiButton m_btnLoop;
    CSWGuiButton m_btnStop;
    CSWGuiButton m_btnNext;
    CSWGuiLabel m_alblBar[4];
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblSelect;
    CSWGuiLabel m_lblBack;
    CSWGuiButton m_abtnSongs[NUM_SONG_BUTTONS];
    CSWGuiButton* m_pSelectedSong;
    CExoArrayList<CMusicItemInfo> m_lMusic;
    int m_nTopSong;
    int m_nSelectedSong;
    CExoStreamingSoundSource* m_pStreamingSound;
    int m_nMusicResType;
    CExoString m_sCurrentSong;
    int m_bPlanetUnlocked[TITLEMUSIC_NUM_PLANETS];
    int m_bLooping;
    int m_nSavedVolume;
#else
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);

    void PopulateSongList();
    void StopMusic();
    void OnSongPicked(CSWGuiControl* pControl);
    void OnPlayPicked(CSWGuiControl* pControl);
    void OnLoopPicked(CSWGuiControl* pControl);
    void OnStopPicked(CSWGuiControl* pControl);
    void OnNextPicked(CSWGuiControl* pControl);
    void OnMoveVolumeSlider(CSWGuiControl* pControl);
    void OnExitSlider(CSWGuiControl* pControl);

    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblBar[3];
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnPlay;
    CSWGuiButton m_btnLoop;
    CSWGuiButton m_btnStop;
    CSWGuiButton m_btnNext;
    CSWGuiLabel m_lblTrackName;
    CSWGuiLabel m_lblUnlocked;
    CSWGuiLabel m_lblTrackNum;
    CSWGuiSlider m_sldVolume;
    CSWGuiListBox m_lbMusic;
    CExoArrayList<CMusicItemInfo> m_lMusic;
    CExoStreamingSoundSource* m_pStreamingSound;
    int m_nMusicResType;
    CExoString m_sCurrentSong;
    CSWGuiButton* m_pSelectedSong;
    int m_bLooping;
    int m_bPlaying;
    int m_nNumSongs;
    int m_nVolume;
    int m_bPlanetUnlocked[TITLEMUSIC_NUM_PLANETS];
#endif
};

#endif
