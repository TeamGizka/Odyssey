#ifndef __SWGUIPORTRAITCHARGEN_H__
#define __SWGUIPORTRAITCHARGEN_H__

#include "swguidrawable.h"
#include "swguicontrol.h"
class CSWCObject;
class CSWGuiManager;
class CSWGuiQuickPanel;
#include "swguiquickorcustompanel.h"
#include "swgui.h"
#include "swguimainchargen.h"
#include "swgui.h"

class CSWCCreature;

extern int g_bDo3DGUIs;

class CSWGuiPortraitCharGen : public CSWGuiPanel
{
public:
    CSWGuiPortraitCharGen(CSWGuiManager* pManager, CSWCCreature* pCreature);
    virtual void Draw(float fTimeStep);
    virtual void HandleInputEvent(int nEvent, int bPressed);
    virtual void OnPanelAdded();

    void SetParentPanel(CSWGuiPanel* pParent, int nParentType);
    void OnSelectDisplayButton();
    void UpdatePortraitButton();
    void HandleCancelButton();
    void HandleLeftButton();
    void HandleRightButton();
#ifdef _XBOX
    void ShowTimedControl(CSWGuiControl* pControl);
#else
    void OnLeftArrowPressed(CSWGuiControl* pControl);
    void OnRightArrowPressed(CSWGuiControl* pControl);
#endif
    void GetPortraitsList(int nGender);

    CSWCCreature* m_pCreature;
    CSWGuiPanel* m_pParent;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblSubTitle;
#ifdef _XBOX
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblPortrait;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblBevel[5];
    CSWGuiLabelHilight m_lblhArrR;
    CSWGuiLabelHilight m_lblhArrL;
    CSWGui3DSceneView m_viewHead;
#else
    CSWGuiLabel m_lblPortrait;
    CSWGuiLabel m_lblBevel[5];
    CSWGui3DSceneView m_viewHead;
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnRight;
    CSWGuiButton m_btnLeft;
    CSWGuiLabel m_alblBar[8];
#endif
    CExoArrayList<int> m_lstPortraits;
    CExoArrayList<int> m_lstAppearances;
    CExoArrayList<CSWGuiTimedControl> m_lstTimedControls;
    float m_fBlinkTimer;
    int m_nPortraitIndex;
    int m_nOriginalPortrait;
    unsigned char m_nParentType;
};

#endif
