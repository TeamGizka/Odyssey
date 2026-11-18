#include "swguimusic.h"

#include "exobase.h"
#include "twodimarray.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "swguioptions.h"
#include "guiingame.h"
#ifndef _XBOX
// PC 0x0074C680 GOG 0x0074C4C0
void CSWGuiTitleMusic::OnPlayPicked(CSWGuiControl* pControl)
{
    if (m_lMusic.num >= 1 && m_pSelectedSong != NULL)
    {
        if (pControl->m_bSelected)
        {
            CMusicItemInfo* pInfo = (CMusicItemInfo*)m_pSelectedSong->GetUserData();
            m_pStreamingSound->SetLooping(m_bLooping);
            if (m_pStreamingSound->IsPlaying())
            {
                StopMusic();
            }
            CResRef cSong(pInfo->m_sFileName);
            m_pStreamingSound->Play(cSong, m_nMusicResType, 0, 1, 0);
            m_lblTrackName.m_cText.m_cParams.SetText(
                m_pSelectedSong->m_cText.m_cParams.m_sText);
        }
        m_bPlaying = 1;
    }
}
// PC 0x0074C730 GOG 0x0074C570
void CSWGuiTitleMusic::OnStopPicked(CSWGuiControl* pControl)
{
    if (m_lMusic.num >= 1 && pControl->m_bSelected &&
        m_pStreamingSound->IsPlaying())
    {
        StopMusic();
    }
}
// PC 0x0074C770 GOG 0x0074C5B0
void CSWGuiTitleMusic::OnNextPicked(CSWGuiControl* pControl)
{
    CSWGuiListBox* pList = &m_lbMusic;
    if (m_pSelectedSong == NULL)
    {
        pList->SetSelectedControl(0, 0);
    }
    else
    {
        int nNumItems = m_lbMusic.GetNumItems();
        pList->SetSelectedControl(m_pSelectedSong, 0);
        int nNext = m_lbMusic.m_nSelectedItem + 1;
        if (nNext >= nNumItems)
        {
            nNext = 0;
        }
        pList->SetSelectedControl(nNext, 0);
    }
    OnSongPicked(pList->GetSelectedControl());
    CMusicItemInfo* pInfo = (CMusicItemInfo*)m_pSelectedSong->GetUserData();
    m_pStreamingSound->SetLooping(m_bLooping);
    if (m_pStreamingSound->IsPlaying())
    {
        StopMusic();
    }
    CResRef cSong(pInfo->m_sFileName);
    m_pStreamingSound->Play(cSong, m_nMusicResType, 0, 1, 0);
    m_lblTrackName.m_cText.m_cParams.SetText(
        m_pSelectedSong->m_cText.m_cParams.m_sText);
    m_bPlaying = 1;
}

// PC 0x0074C200 GOG 0x0074C050
void CSWGuiTitleMusic::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0 &&
        (nEvent == GUIEVENT_CANCEL || nEvent == GUIEVENT_CANCEL_ALTERNATE))
    {
        if (m_pStreamingSound->IsPlaying())
        {
            m_pStreamingSound->Stop();
        }
        g_pAppManager->m_pClientExoApp->SetStartMenuMusicOnMovieEnd(1);
        g_pAppManager->m_pClientExoApp->StartMenuMusic(1, 127, 2);
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        m_pManager->PopModalPanel();
        m_pManager->RemovePanel(this);
    }
    CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif
// PC 0x0074C290
CMusicItemInfo::CMusicItemInfo()
{
}
#ifndef _XBOX
// PC 0x0074C2A0 GOG 0x0074C0E0
void CSWGuiTitleMusic::OnLoopPicked(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
        if (m_bLooping)
        {
            m_pStreamingSound->SetLooping(0);
            m_bLooping = 0;
            m_btnLoop.m_cBorder.m_cParams.SetPulsingAlpha(0, 0, 0);
            m_btnLoop.m_cHilite.m_cParams.SetPulsingAlpha(0, 0, 0);
        }
        else
        {
            m_pStreamingSound->SetLooping(1);
            m_bLooping = 1;
            m_btnLoop.m_cBorder.m_cParams.SetPulsingAlpha(1, 1, 0);
            m_btnLoop.m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
        }
    }
}
// PC 0x0074C330 GOG 0x0074C170
void CSWGuiTitleMusic::StopMusic()
{
    m_bPlaying = 0;
    unsigned long nStartTime =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    m_pStreamingSound->FadeAndStop(500);
    unsigned long nTime;
    do
    {
        g_pExoSound->Render();
        nTime = (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    } while (nTime - nStartTime < 500);
    m_lblTrackName.m_cText.m_cParams.SetText("");
}
// PC 0x0074C400 GOG 0x0074C240
void CSWGuiTitleMusic::OnSongPicked(CSWGuiControl* pControl)
{
    if (m_pSelectedSong != NULL)
    {
        m_pSelectedSong->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        m_pSelectedSong->m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    }
    m_pSelectedSong = pControl->AsButton();
    m_pSelectedSong->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_BONUS;
    m_pSelectedSong->m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_BONUS);
    int nTrack = m_lbMusic.m_nSelectedItem + 1;
    CExoString sTrack;
    sTrack.Format("%d/%d", nTrack, m_nNumSongs);
    m_lblTrackNum.m_cText.m_cParams.SetText(sTrack);
}
// PC 0x0074C510 GOG 0x0074C350
void CSWGuiTitleMusic::OnMoveVolumeSlider(CSWGuiControl* pControl)
{
    int nMax = pControl->AsSlider()->m_nMaxValue;
    int nValue = pControl->AsSlider()->m_nCurValue;
    g_pExoSound->SetMusicVolume((float)nValue / nMax, 0);
    g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMusicVolume = nValue;
}
// PC 0x0074C570 GOG 0x0074C3B0
void CSWGuiTitleMusic::OnExitSlider(CSWGuiControl* pControl)
{
    CSWGuiImageParams* pParams = &pControl->AsSlider()->m_cImage.m_cParams;
    pParams->m_bPulsing = 1;
}
// PC 0x0074C850 GOG 0x0074C690
void CSWGuiTitleMusic::Draw(float fElapsed)
{
    if (m_bPlaying && !m_bLooping && m_pStreamingSound->IsDone())
    {
        OnNextPicked(NULL);
    }
    m_sldVolume.SetCurValue(m_sldVolume.m_nCurValue);
    CSWGuiPanel::Draw(fElapsed);
}
// PC 0x0074C970 GOG 0x0074C7B0
void CSWGuiTitleMusic::PopulateSongList()
{
    C2DA* pMusic = g_pRules->Get2DArrays()->m_pMusic;
    int nRows = pMusic->m_nNumRows;
    CMusicItemInfo cInfo;
    for (int nRow = 0; nRow < nRows; nRow++)
    {
        CExoString sFileName;
        int nStrRef = 0;
        int nPlanet = 0;
        pMusic->GetCExoStringEntry(nRow, "FileName", &sFileName);
        pMusic->GetINTEntry(nRow, "StrrefName", &nStrRef);
        pMusic->GetINTEntry(nRow, "UnlockPlanet", &nPlanet);
        if (nPlanet >= 0 && nPlanet < TITLEMUSIC_NUM_PLANETS && m_bPlanetUnlocked[nPlanet])
        {
            cInfo.m_sFileName = sFileName;
            cInfo.m_nStrRef = nStrRef;
            m_lMusic.Add(cInfo);
        }
    }

    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiExtent cExtent = m_lbMusic.m_cItemExtent;
    CSWGuiControl* pProto = m_lbMusic.m_pProtoItem;
    int nNumSongs = m_lMusic.num;
    for (int nSong = 0; nSong < nNumSongs; nSong++)
    {
        CSWGuiButton* pButton = new CSWGuiButton();
        pButton->Initialize(cExtent, (CSWGuiButton*)pProto);
        pButton->m_cText.m_cParams.SetStrRef(m_lMusic[nSong].m_nStrRef);
        int nIndex = (int)&m_lMusic[nSong];
        pButton->m_nIndex = nIndex;
        cExtent.m_nHeight = 36;
        pButton->SetExtent(cExtent);
        pButton->AddEvent(GUIEVENT_ACTIVATE, this,
                          (CSWGuiHandler)&CSWGuiTitleMusic::OnPlayPicked);
        pButton->AddEvent(GUIEVENT_MOUSE_ENTER, this,
                          (CSWGuiHandler)&CSWGuiTitleMusic::OnSongPicked);
        pButton->SetFocusable(0);
        aControls.Add(pButton);
    }
    m_lbMusic.AddControls(aControls, 1, 0, 0);
}
// PC 0x0074CD00 GOG 0x0074CB40
CSWGuiTitleMusic::CSWGuiTitleMusic(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_pSelectedSong = NULL;
    StartLoadFromLayout("titlemusic_p");
    InitControl(m_lblTitle, "LBL_MUSIC_TITLE", 1);
    InitControl(m_lblBar[0], "LBL_BAR1", 1);
    InitControl(m_lblBar[1], "LBL_BAR2", 1);
    InitControl(m_lblBar[2], "LBL_BAR3", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnPlay, "BTN_PLAY", 1);
    InitControl(m_btnLoop, "BTN_LOOP", 1);
    InitControl(m_btnStop, "BTN_STOP", 1);
    InitControl(m_btnNext, "BTN_NEXT", 1);
    InitControl(m_lbMusic, "LB_MUSIC", 1);
    CSWGuiListBox* pList = &m_lbMusic;
    pList->SetSelectOnLeftClick(1);
    InitControl(m_lblTrackName, "LBL_TRACKNAME", 1);
    InitControl(m_lblUnlocked, "LBL_UNLOCKED", 1);
    InitControl(m_lblTrackNum, "LBL_TRACKNUM", 1);
    InitControl(m_sldVolume, "SLI_VOLUME", 1);
    StopLoadFromLayout();

    m_btnPlay.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiTitleMusic::OnPlayPicked);
    m_btnPlay.SetFocusable(0);
    m_btnPlay.m_nClickSound = CSWGuiControl::GUISOUND_NONE;
    m_btnPlay.m_nFocusSound = CSWGuiControl::GUISOUND_NONE;
    m_btnLoop.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiTitleMusic::OnLoopPicked);
    m_btnLoop.SetFocusable(0);
    m_btnLoop.m_nClickSound = CSWGuiControl::GUISOUND_NONE;
    m_btnLoop.m_nFocusSound = CSWGuiControl::GUISOUND_NONE;
    m_btnStop.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiTitleMusic::OnStopPicked);
    m_btnStop.SetFocusable(0);
    m_btnStop.m_nClickSound = CSWGuiControl::GUISOUND_NONE;
    m_btnStop.m_nFocusSound = CSWGuiControl::GUISOUND_NONE;
    m_btnNext.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiTitleMusic::OnNextPicked);
    m_btnNext.SetFocusable(0);
    m_btnNext.m_nClickSound = CSWGuiControl::GUISOUND_NONE;
    m_btnNext.m_nFocusSound = CSWGuiControl::GUISOUND_NONE;
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnBack.SetFocusable(0);
    m_btnBack.m_nClickSound = CSWGuiControl::GUISOUND_NONE;
    m_btnBack.m_nFocusSound = CSWGuiControl::GUISOUND_NONE;

    m_nVolume = g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMusicVolume;
    m_sldVolume.AddEvent(GUIEVENT_MOUSE_ENTER, this,
                         (CSWGuiHandler)&CSWGuiTitleMusic::OnExitSlider);
    m_sldVolume.AddEvent(1, this,
                         (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldVolume.AddEvent(GUIEVENT_LEFT, this,
                         (CSWGuiHandler)&CSWGuiTitleMusic::OnMoveVolumeSlider);
    m_sldVolume.AddEvent(GUIEVENT_RIGHT, this,
                         (CSWGuiHandler)&CSWGuiTitleMusic::OnMoveVolumeSlider);
    m_sldVolume.AddEvent(GUIEVENT_LEFT_ALTERNATE, this,
                         (CSWGuiHandler)&CSWGuiTitleMusic::OnMoveVolumeSlider);
    m_sldVolume.AddEvent(GUIEVENT_RIGHT_ALTERNATE, this,
                         (CSWGuiHandler)&CSWGuiTitleMusic::OnMoveVolumeSlider);
    m_sldVolume.AddEvent(GUIEVENT_ACTIVATE, this,
                         (CSWGuiHandler)&CSWGuiTitleMusic::OnMoveVolumeSlider);
    m_sldVolume.SetCurValue(m_nVolume);
    OnMoveVolumeSlider(&m_sldVolume);

    CExoIni cIni;
    CExoString sValue;
    cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Game Options", "UnlockedPlanetSongs");
    int nUnlocked = sValue.AsINT();
    for (int nPlanet = 0; nPlanet < TITLEMUSIC_NUM_PLANETS; nPlanet++)
    {
        m_bPlanetUnlocked[nPlanet] = nUnlocked & (1 << nPlanet);
    }
    m_bPlanetUnlocked[0] = 1;

    PopulateSongList();
    m_nNumSongs = m_lbMusic.GetNumItems();
    SetActiveControl(&m_lbMusic, 0);
    m_nMusicResType = 8;
    m_bLooping = 0;
    m_bPlaying = 0;
    m_lblUnlocked.m_cText.m_cParams.SetStrRef(48556);
    m_lblTrackName.m_cText.m_cParams.SetText("");
    m_pStreamingSound = new CExoStreamingSoundSource();
}
// PC 0x0074D4B0 GOG 0x0074D2F0
CSWGuiTitleMusic::~CSWGuiTitleMusic()
{
    if (m_pStreamingSound->IsPlaying())
    {
        m_pStreamingSound->Stop();
    }
    delete m_pStreamingSound;
    m_lMusic.RemoveAll();
    float fVolume = (float)m_nVolume / m_sldVolume.GetMaxValue();
    g_pExoSound->SetMusicVolume(fVolume, 0);
    unsigned char nVolume = (unsigned char)m_nVolume;
    g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nMusicVolume = nVolume;
}
#endif
#ifdef _XBOX
// Xbox 0x0012BDB0
void CSWGuiTitleMusic::StopMusic()
{
    unsigned long nStartTime =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    m_pStreamingSound->FadeAndStop(500);
    unsigned long nTime;
    do
    {
        g_pExoSound->Render();
        nTime = (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    } while (nTime - nStartTime < 500);
}
// Xbox 0x0012BE20
void CSWGuiTitleMusic::OnPanelRemoved()
{
    g_pExoSound->SetMusicVolume((float)m_nSavedVolume * 0.01f, 0);
    unsigned char nVolume = (unsigned char)m_nSavedVolume;
    g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMusicVolume = nVolume;
}
// Xbox 0x0012BE60
void CSWGuiTitleMusic::OnPlayPicked(CSWGuiControl* pControl)
{
    if (m_lMusic.num >= 1 && pControl->m_bSelected)
    {
        CMusicItemInfo* pInfo = (CMusicItemInfo*)m_pSelectedSong->m_nIndex;
        m_pStreamingSound->SetLooping(m_bLooping);
        if (m_pStreamingSound->IsPlaying())
        {
            StopMusic();
        }
        m_pStreamingSound->Play(CResRef(pInfo->m_sFileName), m_nMusicResType, 0, 1, 0);
        m_sCurrentSong = pInfo->m_sFileName;
    }
}
// Xbox 0x0012BEF0
void CSWGuiTitleMusic::OnLoopPicked(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
        if (m_bLooping)
        {
            m_pStreamingSound->SetLooping(0);
            m_bLooping = 0;
            m_btnLoop.m_cBorder.m_cParams.SetPulsingAlpha(0, 0, 0);
            m_btnLoop.m_cHilite.m_cParams.SetPulsingAlpha(0, 0, 0);
        }
        else
        {
            m_pStreamingSound->SetLooping(1);
            m_bLooping = 1;
            m_btnLoop.m_cBorder.m_cParams.SetPulsingAlpha(1, 1, 0);
            m_btnLoop.m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
        }
    }
}
// Xbox 0x0012BF80
void CSWGuiTitleMusic::OnStopPicked(CSWGuiControl* pControl)
{
    if (m_lMusic.num >= 1 && pControl->m_bSelected &&
        m_pStreamingSound->IsPlaying())
    {
        StopMusic();
    }
}
// Xbox 0x0012BFC0
void CSWGuiTitleMusic::SetSongWindow(int nTopSong, int nSelectedSong)
{
    int nShown = m_lMusic.num > NUM_SONG_BUTTONS ? NUM_SONG_BUTTONS : m_lMusic.num;
    m_nTopSong = nTopSong;
    m_nSelectedSong = nSelectedSong;
    int nButton;
    for (nButton = 0; nButton < nShown; nButton++)
    {
        m_abtnSongs[nButton].m_cText.m_cParams.SetStrRef(m_lMusic[nTopSong + nButton].m_nStrRef);
        m_abtnSongs[nButton].m_nIndex = (int)&m_lMusic[nTopSong + nButton];
        m_abtnSongs[nButton].SetEnabledFlag(1);
    }
    for (nButton = nShown; nButton < NUM_SONG_BUTTONS; nButton++)
    {
        m_abtnSongs[nButton].SetEnabledFlag(0);
    }
    if (nSelectedSong != -1)
    {
        if (m_pSelectedSong != NULL)
        {
            m_pSelectedSong->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
            m_pSelectedSong->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
            m_pSelectedSong->m_cBorder.m_cParams.SetPulsingAlpha(0, 0, 0);
            CSWGuiTextParams* pText = &m_pSelectedSong->m_cText.m_cParams;
            pText->m_bHilightText = 0;
        }
        m_pSelectedSong = &m_abtnSongs[nSelectedSong - nTopSong];
        m_pSelectedSong->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
        m_pSelectedSong->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        m_pSelectedSong->m_cBorder.m_cParams.SetPulsingAlpha(1, 1, 1);
        CSWGuiTextParams* pText = &m_pSelectedSong->m_cText.m_cParams;
        pText->m_bHilightText = 1;
    }
    m_lblUpArrow.SetEnabledFlag(m_nTopSong != 0);
    m_lblDownArrow.SetEnabledFlag(m_nTopSong + nShown < m_lMusic.num);
}
// Xbox 0x0012C230
void CSWGuiTitleMusic::SelectPreviousSong()
{
    int nSelected = m_nSelectedSong - 1;
    int nTop = m_nTopSong;
    if (nSelected < 0)
    {
        nSelected = m_lMusic.num - 1;
        nTop = nSelected - (NUM_SONG_BUTTONS - 1) < 0 ? 0 : nSelected - (NUM_SONG_BUTTONS - 1);
        g_pExoInput->ClearEventRepeat(m_pManager->m_nLastInputEvent);
    }
    else if (nSelected < nTop)
    {
        nTop = nSelected;
    }
    SetSongWindow(nTop, nSelected);
}
// Xbox 0x0012C290
void CSWGuiTitleMusic::SelectNextSong(int bNoRefresh)
{
    int nSelected = m_nSelectedSong + 1;
    int nTop = m_nTopSong;
    if (nSelected >= m_lMusic.num)
    {
        nSelected = 0;
        nTop = 0;
        g_pExoInput->ClearEventRepeat(m_pManager->m_nLastInputEvent);
    }
    else if (nSelected - nTop > NUM_SONG_BUTTONS - 1)
    {
        nTop = nSelected - (NUM_SONG_BUTTONS - 1);
    }
    if (!bNoRefresh)
    {
        SetSongWindow(nTop, nSelected);
    }
}
// Xbox 0x0012C2F0
void CSWGuiTitleMusic::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
            if (m_pStreamingSound->IsPlaying())
            {
                m_pStreamingSound->Stop();
            }
            g_pAppManager->m_pClientExoApp->SetStartMenuMusicOnMovieEnd(1);
            g_pAppManager->m_pClientExoApp->StartMenuMusic(1, 127, 2);
            g_pAppManager->m_pClientExoApp->EnableMainMenuIdleTimer();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            m_pManager->RemovePanel(this);
            break;
        case GUIEVENT_UP:
        case GUIEVENT_UP_ALTERNATE:
            SelectNextSong(0);
            break;
        case GUIEVENT_DOWN:
        case GUIEVENT_DOWN_ALTERNATE:
            SelectPreviousSong();
            break;
        }
    }
    CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// Xbox 0x0012C450
void CSWGuiTitleMusic::OnNextPicked(CSWGuiControl* pControl)
{
    if (m_lMusic.num >= 1 && pControl->m_bSelected)
    {
        if (!m_sCurrentSong.IsEmpty())
        {
            for (int nSong = 0; nSong < m_lMusic.num; nSong++)
            {
                SelectNextSong(0);
                if (((CMusicItemInfo*)m_pSelectedSong->m_nIndex)->m_sFileName == m_sCurrentSong)
                {
                    break;
                }
            }
        }
        SelectNextSong(0);
        if (m_pStreamingSound->IsPlaying())
        {
            StopMusic();
        }
        CMusicItemInfo* pInfo = (CMusicItemInfo*)m_pSelectedSong->m_nIndex;
        m_pStreamingSound->SetLooping(m_bLooping);
        m_pStreamingSound->Play(CResRef(pInfo->m_sFileName), m_nMusicResType, 0, 1, 0);
        m_sCurrentSong = pInfo->m_sFileName;
    }
}
// Xbox 0x0012C580
void CSWGuiTitleMusic::PopulateSongList()
{
    C2DA* pMusic = g_pRules->Get2DArrays()->m_pMusic;
    int nRows = pMusic->m_nNumRows;
    CMusicItemInfo cInfo;
    for (int nRow = 0; nRow < nRows; nRow++)
    {
        CExoString sFileName;
        int nStrRef = 0;
        int nPlanet = 0;
        pMusic->GetCExoStringEntry(nRow, "FileName", &sFileName);
        pMusic->GetINTEntry(nRow, "StrrefName", &nStrRef);
        pMusic->GetINTEntry(nRow, "UnlockPlanet", &nPlanet);
        if (nPlanet >= 0 && nPlanet < TITLEMUSIC_NUM_PLANETS && m_bPlanetUnlocked[nPlanet])
        {
            cInfo.m_sFileName = sFileName;
            cInfo.m_nStrRef = nStrRef;
            m_lMusic.Add(cInfo);
        }
    }
}
// Xbox 0x0012C6D0
CSWGuiTitleMusic::CSWGuiTitleMusic(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("titlemusic_x");
    InitControl(m_lblTitle, "LBL_MUSIC_TITLE", 1);
    m_lblTitle.SetEnabledFlag(1);
    InitControl(m_btnPlay, "BTN_PLAY", 1);
    InitControl(m_btnLoop, "BTN_LOOP", 1);
    InitControl(m_btnStop, "BTN_STOP", 1);
    InitControl(m_btnNext, "BTN_NEXT", 1);
    InitControl(m_lblUpArrow, "LBL_UPARROW", 1);
    InitControl(m_lblDownArrow, "LBL_DOWNARROW", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblSelect, "LBL_SELECT", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    m_lblDownArrow.m_cBorder.m_cParams.m_fRotation = 180.0f;
    m_lblUpArrow.SetEnabledFlag(0);
    m_lblDownArrow.SetEnabledFlag(0);

    m_btnPlay.SetEnabledFlag(1);
    m_btnPlay.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiTitleMusic::OnPlayPicked);
    m_btnPlay.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    m_btnPlay.m_nClickSound = CSWGuiControl::GUISOUND_NONE;
    m_btnPlay.m_nFocusSound = CSWGuiControl::GUISOUND_NONE;
    m_btnLoop.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiTitleMusic::OnLoopPicked);
    m_btnLoop.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    m_btnLoop.m_nClickSound = CSWGuiControl::GUISOUND_NONE;
    m_btnLoop.m_nFocusSound = CSWGuiControl::GUISOUND_NONE;
    m_btnStop.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiTitleMusic::OnStopPicked);
    m_btnStop.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    m_btnStop.m_nClickSound = CSWGuiControl::GUISOUND_NONE;
    m_btnStop.m_nFocusSound = CSWGuiControl::GUISOUND_NONE;
    m_btnNext.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiTitleMusic::OnNextPicked);
    m_btnNext.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    m_btnNext.m_nClickSound = CSWGuiControl::GUISOUND_NONE;
    m_btnNext.m_nFocusSound = CSWGuiControl::GUISOUND_NONE;

    m_nSavedVolume = g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMusicVolume;
    g_pExoSound->SetMusicVolume(1.0f, 0);
    g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMusicVolume = 100;

    for (int nSong = 1; nSong <= NUM_SONG_BUTTONS; nSong++)
    {
        CSWGuiButton* pButton = &m_abtnSongs[nSong - 1];
        InitControl(*pButton, CExoString("BTN_SONG") + CExoString(nSong), 1);
        pButton->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        pButton->SetEnabledFlag(1);
    }
    StopLoadFromLayout();

    CExoIni cIni;
    CExoString sValue;
    int nUnlocked = g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nUnlockedSongs;
    for (int nPlanet = 0; nPlanet < TITLEMUSIC_NUM_PLANETS; nPlanet++)
    {
        m_bPlanetUnlocked[nPlanet] = nUnlocked & (1 << nPlanet);
    }
    m_bPlanetUnlocked[0] = 1;

    PopulateSongList();
    SetActiveControl(&m_btnPlay, 0);
    m_pStreamingSound = new CExoStreamingSoundSource();
    m_pSelectedSong = NULL;
    m_nTopSong = 0;
    m_nSelectedSong = 0;
    if (m_lMusic.num < 1)
    {
        m_nSelectedSong = -1;
    }
    SetSongWindow(0, m_nSelectedSong);
    m_nMusicResType = 11;
    m_bLooping = 0;
}
// Xbox 0x0012CF60
CSWGuiTitleMusic::~CSWGuiTitleMusic()
{
    if (m_pStreamingSound->IsPlaying())
    {
        m_pStreamingSound->Stop();
    }
    delete m_pStreamingSound;
    m_lMusic.RemoveAll();
}
#endif
