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

// PC 0x0074C200
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
// PC 0x0074C2A0
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
// PC 0x0074C330
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
// PC 0x0074C400
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
// PC 0x0074C510
void CSWGuiTitleMusic::OnMoveVolumeSlider(CSWGuiControl* pControl)
{
    int nMax = pControl->AsSlider()->m_nMaxValue;
    int nValue = pControl->AsSlider()->m_nCurValue;
    g_pExoSound->SetMusicVolume((float)nValue / nMax, 0);
    g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMusicVolume = nValue;
}
// PC 0x0074C570
void CSWGuiTitleMusic::OnExitSlider(CSWGuiControl* pControl)
{
    CSWGuiImageParams* pParams = &pControl->AsSlider()->m_cImage.m_cParams;
    pParams->m_bPulsing = 1;
}
// PC 0x0074C680
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
// PC 0x0074C730
void CSWGuiTitleMusic::OnStopPicked(CSWGuiControl* pControl)
{
    if (m_lMusic.num >= 1 && pControl->m_bSelected &&
        m_pStreamingSound->IsPlaying())
    {
        StopMusic();
    }
}
// PC 0x0074C770
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
// PC 0x0074C850
void CSWGuiTitleMusic::Draw(float fElapsed)
{
    if (m_bPlaying && !m_bLooping && m_pStreamingSound->IsDone())
    {
        OnNextPicked(NULL);
    }
    m_sldVolume.SetCurValue(m_sldVolume.m_nCurValue);
    CSWGuiPanel::Draw(fElapsed);
}
// PC 0x0074C970
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
// PC 0x0074CD00
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
// PC 0x0074D4B0
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
