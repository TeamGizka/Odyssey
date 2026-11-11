#include "swgui.h"
#include "resgff.h"

CSWGuiDynamic::CSWGuiDynamic(CSWGuiManager* pManager, const CResRef& cLayout)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout(cLayout);

    CResGFF* pGFF = m_pLayout;
    CExoString sTag;
    int nControls = pGFF->GetListCount(&m_lLayoutControls);

    for (int i = 0; i < nControls; i++)
    {
        CResStruct cControl;
        int bSuccess;

        pGFF->GetListElement(&cControl, &m_lLayoutControls, i);
        unsigned long nType =
            pGFF->ReadFieldDWORD(&cControl, "CONTROLTYPE", bSuccess, 0);
        sTag = pGFF->ReadFieldCExoString(&cControl, "TAG", bSuccess, "");

        CSWGuiControl* pControl = NULL;
        switch (nType)
        {
        case 4:
            pControl = new CSWGuiLabel;
            break;
        case 5:
            pControl = new CSWGuiLabelHilight;
            break;
        case 6:
            pControl = new CSWGuiButton;
            break;
        case 7:
            pControl = new CSWGuiButtonToggle;
            break;
        case 8:
            pControl = new CSWGuiSlider;
            break;
        case 9:
            pControl = new CSWGuiScrollBar;
            break;
        case 10:
            pControl = new CSWGuiProgressBar;
            break;
        case 11:
            pControl = new CSWGuiListBox;
            break;
        }

        if (pControl != NULL)
        {
            InitControl(*pControl, sTag, 1);
        }
    }

    StopLoadFromLayout();
}

CSWGuiDynamic::~CSWGuiDynamic()
{
    int nControls = m_lControls.num;

    for (int i = 0; i < nControls; i++)
    {
        if (m_lControls[i] != NULL)
        {
            delete m_lControls[i];
        }
    }
    m_lControls.SetSize(0);
}
