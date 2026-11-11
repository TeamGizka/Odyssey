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
// PC 0x006DCE80  Xbox 0x00116D60
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
// PC 0x006DCF10
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
        else
        {
            CExoString sCredits("credits");
            Vector vPosition;
            CClientExoApp* pExoApp = g_pAppManager->m_pClientExoApp;
            pExoApp->StartCreditSequence(vPosition, sCredits);
        }
    }
}
// PC 0x006DD910  Xbox 0x00117780
CSWGuiTitleMovies::CSWGuiTitleMovies(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("titlemovie");
    InitControl(m_lbxMovies, "LB_MOVIES", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblPlayMovie, "LBL_PLAYMOVIE", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
#else
    InitControl(m_btnBack, "BTN_BACK", 1);
#endif
    StopLoadFromLayout();

    CSWGuiButton* pProto = m_lbxMovies.m_pProtoItem->AsButton();
    if (pProto != NULL)
    {
        pProto->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
        pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
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
// PC 0x006DCFE0
CSWGuiTitleMovies::~CSWGuiTitleMovies()
{
    for (int nMovie = 0; nMovie < m_lbxMovies.m_lItems.num; ++nMovie)
    {
        delete (CExoString*)m_lbxMovies.GetControl(nMovie)->AsButton()->m_nIndex;
    }
}
// PC 0x006DD100  Xbox 0x00117070
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
    CSWGuiButton* pCredits = new CSWGuiButton;
    pCredits->Initialize(cExtent, pProto);
    pCredits->m_cText.m_cParams.SetText(g_pTlkTable->GetSimpleString(STRREF_CREDITS));
    pCredits->m_nIndex = 0;
    cExtent.m_nHeight = 56;
    pCredits->SetExtent(cExtent);
    pCredits->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiTitleMovies::OnMoviePicked);
    pCredits->AddEvent(GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiTitleMovies::OnMoviePicked);
    lControls.Add(pCredits);
    m_lMovieNames.Add("credits");
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
#ifdef _XBOX
            int nInsert = 0;
#else
            int nInsert = 1;
#endif
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
    m_lbxMovies.AddControls(lControls, 1, 0, 0);
}
