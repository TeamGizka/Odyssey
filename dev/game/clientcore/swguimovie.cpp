#include "appmanager.h"
#include "clientexoapp.h"
#include "swguimainmenu.h"
#include "guiingame.h"
#include "exotlktable.h"
#include "clientoptions.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"

#ifdef _XBOX
static inline C2DA* SwguimovieGetMovies2DA(CTwoDimArrays* pArrays)
{
    EXOXBOXASSERT(pArrays->m_pMovies2DA);
    return pArrays->m_pMovies2DA;
}
#endif
// PC 0x0074B400
void CSWGuiTitleMovies::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            g_pAppManager->m_pClientExoApp->SetStartMenuMusicOnMovieEnd(1);
            g_pAppManager->m_pClientExoApp->StartMenuMusic(1, 127, 2);
#ifndef _XBOX
            g_pAppManager->m_pClientExoApp->EnableMainMenuIdleTimer();
#endif
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
#ifdef _XBOX
            m_pManager->RemovePanel(this);
#else
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
#endif
            break;

        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        }
    }

    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#ifndef _XBOX
void CSWGuiTitleMovies::OnMoviePicked(CSWGuiControl* pControl)  // PC 0x0074B4A0
{
    if (pControl->m_bSelected != 0)
    {
        CExoString* psMovie = (CExoString*)pControl->AsButton()->m_nIndex;
        if (psMovie != NULL)
        {
            int nMovieResult = 0;
            g_pAppManager->m_pClientExoApp->PlayMovie(*psMovie, nMovieResult,
                                                      1, 1);
        }
        else
        {
            CExoString sCredits("credits");
            Vector vPosition;
            CClientExoApp* pExoApp = g_pAppManager->m_pClientExoApp;
            pExoApp->StartCreditSequence(vPosition, sCredits);
        }
    }
}
#endif
CSWGuiTitleMovies::CSWGuiTitleMovies(CSWGuiManager* pManager)  // PC 0x0074BE70
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("titlemovie");
#else
    StartLoadFromLayout("titlemovie_p");
#endif
    InitControl(m_lbxMovies, "LB_MOVIES", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblPlayMovie, "LBL_PLAYMOVIE", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
#else
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_lblUnlocked, "LBL_UNLOCKED", 1);
    InitControl(m_lblUnlockedValue, "LBL_UNLOCKED_VALUE", 1);
#endif
    StopLoadFromLayout();
#ifndef _XBOX
    m_lblUnlocked.SetEnabledFlag(1);
    m_lblUnlockedValue.SetEnabledFlag(1);
#endif

    CSWGuiButton* pProto = m_lbxMovies.m_pProtoItem->AsButton();
    if (pProto != NULL)
    {
#ifdef _XBOX
        pProto->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
        pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
#else
        pProto->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
        pProto->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
        pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 1);
#endif
    }

#ifndef _XBOX
    CSWGuiButton* pBack = &m_btnBack;
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this,
                      (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
#endif
    PopulateMovies();
    SetActiveControl(&m_lbxMovies, 0);
}
// PC 0x0074B570
CSWGuiTitleMovies::~CSWGuiTitleMovies()
{
    for (int nMovie = 0; nMovie < m_lbxMovies.m_lItems.num; ++nMovie)
    {
        delete (CExoString*)m_lbxMovies.GetControl(nMovie)->AsButton()->m_nIndex;
    }
}
// PC 0x0074B720
void CSWGuiTitleMovies::PopulateMovies()
{
    CExoString sMovie;
    CExoArrayList<CExoString> lMovies;
    CExoArrayList<CSWGuiControl*> lControls;
    int nOrder = 0;
    CSWGuiExtent cExtent = m_lbxMovies.m_cItemExtent;

#ifdef _XBOX
    g_pExoBase->GetDirectoryList(&lMovies, "MOVIES:", 13, 0, 0);
#else
    g_pExoBase->GetDirectoryList(&lMovies, "MOVIES:", RESTYPE_BIK, 0, 0);
#endif
    CExoString sAlias;
    for (int nPackage = 1; nPackage < CTlkTable::NUM_CONTENT_PKGS; nPackage++)
    {
        sAlias.Format("live%d", nPackage);
        if (g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, 0).GetLength() > 0)
        {
            sAlias = sAlias + ":movies";
#ifdef _XBOX
            g_pExoBase->GetDirectoryList(&lMovies, sAlias, 13, 0, 0);
#else
            g_pExoBase->GetDirectoryList(&lMovies, sAlias, RESTYPE_BIK, 0, 0);
#endif
        }
    }

    EXOXBOXASSERT(m_lbxMovies.m_nCONTROLTYPE == 6);
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbxMovies.m_pProtoItem;
    int nMovies = lMovies.num;
    CExoString sName;
#ifndef _XBOX
    int nUnlocked = 0;
#endif

    for (int nMovie = 0; nMovie < nMovies; nMovie++)
    {
        sMovie = lMovies[nMovie];
        sMovie = sMovie.SubString(0, sMovie.GetLength() - 4);
        int nAlwaysShow;
        {
            CExoString sColumn("AlwaysShow");
#ifdef _XBOX
            CTwoDimArrays* pXbox2DArrays = g_pRules->Get2DArrays();
            EXOXBOXASSERT(pXbox2DArrays->m_pMovies2DA);
            C2DA* pMovies = pXbox2DArrays->m_pMovies2DA;
#else
            C2DA* pMovies = g_pRules->Get2DArrays()->m_pMovies2DA;
#endif
            pMovies->GetINTEntry(sMovie, sColumn, &nAlwaysShow);
        }
        if (nAlwaysShow != 1)
        {
#ifdef _XBOX
            if (!g_pAppManager->GetClientExoApp()->GetClientOptions()->GetMovieShown(sMovie))
                continue;
#else
            CResRef cMovie(sMovie);
            if (!g_pAppManager->GetClientExoApp()->GetClientOptions()->GetMovieShown(cMovie))
                continue;
#endif
        }
#ifndef _XBOX
        nUnlocked++;
#endif
        {
            CSWGuiButton* pButton = new CSWGuiButton;
            pButton->Initialize(cExtent, pProto);
            unsigned long nStrrefName = 0;
            {
                CExoString sColumn("StrrefName");
#ifdef _XBOX
                CTwoDimArrays* pXbox2DArrays = g_pRules->Get2DArrays();
                EXOXBOXASSERT(pXbox2DArrays->m_pMovies2DA);
                C2DA* pMovies = pXbox2DArrays->m_pMovies2DA;
#else
                C2DA* pMovies = g_pRules->Get2DArrays()->m_pMovies2DA;
#endif
                pMovies->GetINTEntry(sMovie, sColumn, (int*)&nStrrefName);
            }
            if (nStrrefName != 0)
                sName = g_pTlkTable->GetSimpleString(nStrrefName);
            else
                sName = sMovie;
            pButton->m_cText.m_cParams.SetText(sName);
            pButton->m_nIndex = (int)new CExoString(sMovie);
            cExtent.m_nHeight = 56;
            pButton->SetExtent(cExtent);
            pButton->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiTitleMovies::OnMoviePicked);
            pButton->AddEvent(GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiTitleMovies::OnMoviePicked);
            {
                CExoString sColumn("Order");
#ifdef _XBOX
                CTwoDimArrays* pXbox2DArrays = g_pRules->Get2DArrays();
                EXOXBOXASSERT(pXbox2DArrays->m_pMovies2DA);
                C2DA* pMovies = pXbox2DArrays->m_pMovies2DA;
#else
                C2DA* pMovies = g_pRules->Get2DArrays()->m_pMovies2DA;
#endif
                pMovies->GetINTEntry(sMovie, sColumn, &nOrder);
            }
            int nInsert = 0;
            for (; nInsert < lControls.num; nInsert++)
            {
                int nOtherOrder = 0;
                {
                    CExoString sColumn("Order");
#ifdef _XBOX
                    CTwoDimArrays* pXbox2DArrays = g_pRules->Get2DArrays();
                    EXOXBOXASSERT(pXbox2DArrays->m_pMovies2DA);
                    C2DA* pMovies = pXbox2DArrays->m_pMovies2DA;
#else
                    C2DA* pMovies = g_pRules->Get2DArrays()->m_pMovies2DA;
#endif
                    pMovies->GetINTEntry(m_lMovieNames[nInsert], sColumn, &nOtherOrder);
                }
                if (nOtherOrder > nOrder)
                    break;
            }
            lControls.Insert(pButton, nInsert);
            m_lMovieNames.Insert(sMovie, nInsert);
        }
    }
#ifndef _XBOX
    CExoString sUnlocked;
    sUnlocked.Format("%d / %d", nUnlocked, nMovies);
    m_lblUnlockedValue.m_cText.m_cParams.SetText(sUnlocked);
#endif
    m_lbxMovies.AddControls(lControls, 1, 0, 0);
}

#ifdef _XBOX

void CSWGuiTitleMovies::OnMoviePicked(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CExoString* psMovie = (CExoString*)pControl->AsButton()->m_nIndex;
        if (psMovie != NULL)
        {
            int nMovieResult = 0;
            g_pAppManager->m_pClientExoApp->PlayMovie(*psMovie, nMovieResult,
                                                      0, 1);
        }
    }
}
#endif
