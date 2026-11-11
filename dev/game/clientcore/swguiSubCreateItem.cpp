#include "swguiSubCreateItem.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "exoresman.h"
#include "guiingame.h"
#include "resgff.h"
#include <string.h>
#include "swmain.h"

static inline void SwguiSubCreateItemTrace1();
// PC 0x00713230
void CSWGuiCreateItemSubMenu::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 49:
        case 61:
        {
            if (m_nSelection == -99)
            {
                m_nSelection = -1;
            }
            int nSelection = m_nSelection;
            if (nSelection > 0)
            {
                m_nSelection = nSelection - 1;
            }
            else if (nSelection == 0)
            {
                m_nSelection = m_lOptions.num - 1;
            }
            if (m_pManager != NULL)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            break;
        }

        case 50:
        case 62:
        {
            if (m_nSelection == -99)
            {
                m_nSelection = -1;
            }
            int nOptions = m_lOptions.num;
            int nSelection = m_nSelection;
            if (nSelection < nOptions)
            {
                m_nSelection = nSelection + 1;
            }
            else if (nSelection == nOptions)
            {
                m_nSelection = 0;
            }
            if (m_pManager != NULL)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            break;
        }

        case 40:
        case 46:
            ((CGuiInGame*)g_pAppManager->m_pClientExoApp->GetInGameGui())
                ->HideSubItemCreateMenu();
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00713340
CSWGuiButton* CSWGuiCreateItemSubMenu::CreateOption(const CExoString& sText,
                                                    CSWGuiHandler pHandler,
                                                    CSWGuiButton* pPrototype)
{
    CSWGuiButton* pButton = NULL;
    CSWGuiExtent cExtent;
    CSWGuiBorderParams* pBorder;
    CSWGuiBorderParams* pHilite;
    int nBorderDim;
    int nHiliteDim;

    cExtent.m_nLeft = cExtent.m_nTop = cExtent.m_nWidth = cExtent.m_nHeight = 0;
    pButton = new CSWGuiButton;
    pButton->Initialize(cExtent, pPrototype);
    pButton->m_cText.m_cParams.SetText(sText);
    pHilite = &pButton->m_cHilite.m_cParams;
    pBorder = &pButton->m_cBorder.m_cParams;
    nBorderDim = pBorder->GetBorderDim();
    nHiliteDim = pHilite->GetBorderDim();
    if (nHiliteDim > nBorderDim)
    {
        nBorderDim = pHilite->GetBorderDim();
    }
    else
    {
        nBorderDim = pBorder->GetBorderDim();
    }
    cExtent.m_nHeight = pButton->m_cText.GetIdealHeight() + 2 * nBorderDim;
    pButton->SetExtent(cExtent);
    pButton->AddEvent(GUIEVENT_ACTIVATE, this, pHandler);
    pButton->AddEvent(45, this, pHandler);
    return pButton;
}
// PC 0x00713170
void CSWGuiCreateItemSubMenu::Back(CSWGuiControl* pControl)
{
    SwguiSubCreateItemTrace1();
    if (pControl->m_bSelected != 0)
    {
        ((CGuiInGame*)g_pAppManager->m_pClientExoApp->GetInGameGui())
            ->HideSubItemCreateMenu();
        ((CGuiInGame*)g_pAppManager->m_pClientExoApp->GetInGameGui())
            ->ShowItemCreateMenu();
    }
}
// PC 0x007131B0
void CSWGuiCreateItemSubMenu::CreateItem(CSWGuiControl* pControl)
{
    SwguiSubCreateItemTrace1();
    CClientExoApp* pApp;
    CGuiInGame* pGui;
    if (pControl->m_bSelected != 0)
    {
        CResRef cItem;
#ifdef _XBOX
        cItem = (char*)m_lstBlueprintNames[m_nSelection - 1]->CStr();
#else
        int nIndex = pControl->m_nID - 1;
        cItem = (char*)m_lstBlueprintNames[nIndex]->CStr();
#endif
        pApp = g_pAppManager->m_pClientExoApp;
        pApp->CreatureAcquireItem(pApp->GetPlayerCreatureId(), cItem);
        pGui = (CGuiInGame*)g_pAppManager->m_pClientExoApp->GetInGameGui();
        pGui->HideSubItemCreateMenu();
    }
}
// PC 0x00713400
CSWGuiCreateItemSubMenu::CSWGuiCreateItemSubMenu(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_nSelection = 0;
    StartLoadFromLayout("debug");
    InitControl(m_lstOptions, "LB_OPTIONS", 1);
    InitControl(m_lblBuild, "LBL_BUILD", 1);
    StopLoadFromLayout();
    CExoString sBuild = CExoString("Build: ") + g_sBuildNumber;
    m_lblBuild.m_cText.m_cParams.SetText(sBuild);
    CenterPanel();
}
// PC 0x00713650
void CSWGuiCreateItemSubMenu::LoadItems(int nBaseItem)
{
    CSWGuiControl* pPrototype = m_lstOptions.m_pProtoItem;
    int bCreated = 0;
    CExoStringList* pResources;
    CResStruct cTopLevel;
    int bSuccess;
    int j;
    CSWGuiListBox* pListBox;

    m_lstOptionNames.m_nSortMode = 1;
    m_lstBlueprintNames.m_nSortMode = 1;
    pResources = g_pExoResMan->GetResOfType(0x7E9, 0);
    if (pResources != NULL)
    {
        int nCount = pResources->GetCount();
        int i;

        for (i = 0; i < nCount; i++)
        {
            CExoString* pName = pResources->GetString(i);
            CResRef cResource((char*)pName->CStr());
            CResGFF* pGFF =
                (CResGFF*)g_pExoResMan->GetResObject(cResource, RESTYPE_UTI);

            if (pGFF == NULL)
            {
                pGFF = new CResGFF;
                strncpy(pGFF->m_szFileType, "UTI ", 4);
#ifdef _XBOX
                g_pExoResMan->SetResObject((char*)pName->CStr(), RESTYPE_UTI, pGFF);
#else
                CResRef cNewResource((char*)pName->CStr());
                g_pExoResMan->SetResObject(cNewResource, RESTYPE_UTI, pGFF);
#endif
                bCreated = 1;
            }

            if (pGFF != NULL && pGFF->Demand() != 0)
            {
                CResRef cTemplate;
                int nItem;

                pGFF->GetTopLevelStruct(&cTopLevel);
#ifdef _XBOX
                cTemplate = pGFF->ReadFieldCResRef(&cTopLevel, "TemplateResRef",
                                                   bSuccess, "BAD_BLUEPRINT");
#else
                CResRef cDefault("BAD_BLUEPRINT");
                cTemplate = pGFF->ReadFieldCResRef(&cTopLevel, "TemplateResRef",
                                                   bSuccess, cDefault);
#endif
                nItem = pGFF->ReadFieldINT(&cTopLevel, "BaseItem", bSuccess, 0);
                if (bSuccess != 0 && nItem == nBaseItem)
                {
                    CExoString sTemplate;

                    cTemplate.CopyToString(sTemplate);
                    m_lstOptionNames.Add(new CExoString(sTemplate));
                    m_lstBlueprintNames.Add(new CExoString(*pName));
                }

                pGFF->Release();
                g_pExoResMan->ReleaseResObject(pGFF);
                if (bCreated != 0)
                {
                    bCreated = 0;
                    if (pGFF != NULL)
                    {
                        delete pGFF;
                    }
                }
            }
        }
        delete pResources;
    }

    m_lOptions.SetSize(m_lstOptionNames.GetCount() + 1);
    {
        CExoString sBack("Back");
        CSWGuiControl** ppOption = m_lOptions.element;

        *ppOption =
            CreateOption(sBack, (CSWGuiHandler)&CSWGuiCreateItemSubMenu::Back,
                         (CSWGuiButton*)pPrototype);
    }

    for (j = 0; j < m_lstOptionNames.GetCount(); j++)
    {
        CExoString sOption(*m_lstOptionNames.GetString(j));
        CSWGuiControl** ppOption = &m_lOptions.element[j + 1];

        *ppOption = CreateOption(
            sOption, (CSWGuiHandler)&CSWGuiCreateItemSubMenu::CreateItem,
            (CSWGuiButton*)pPrototype);
    }

    pListBox = &m_lstOptions;
    pListBox->AddControls(m_lOptions, 1, 0, 0);
    pListBox->m_bLooping = 1;
    pListBox->SetSelectedControl(0, 0);
    m_nSelection = 0;
    SetActiveControl(pListBox, 0);
}
// PC 0x00713C20
void CSWGuiCreateItemSubMenu::ClearItems()
{
    m_lstOptions.ClearItems();
    m_lOptions.SetSize(0);
    m_lstBlueprintNames.Clear();
    m_lstOptionNames.Clear();
    m_nSelection = -99;
}

static inline void SwguiSubCreateItemTrace1()
{
}
