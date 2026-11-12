#ifndef __SWGUIINGAMEMAP_H__
#define __SWGUIINGAMEMAP_H__

#include "swguicontrol.h"
class CExoLinkedListInternal;
class CSWGuiControl;
class CSWGuiManager;
#include "exobase.h"
#include "swgui.h"
#include "swguidrawable.h"
#include "aurgui.h"

class CSWGuiMapHider : public CSWGuiControl
{
public:
    CSWGuiMapHider();
    virtual ~CSWGuiMapHider();
    virtual void Draw(float fElapsed);
#ifndef _XBOX
    virtual CSWGuiControl* HitCheckMouse(int nX, int nY);
#endif

    void ClearMapNotes();
    CExoString InitializeMapNotes(int nMapMode);
    CExoString GetPrevMapNote();
    void SetMapNote(unsigned long oidNote);
    CExoString GetNextMapNote();

    CAurGUIImageInternal* m_pBlackDot;
    CSWGuiImage* m_pArrow;
    CSWGuiImage* m_pCircle;
#ifdef _XBOX
    CSWGuiImage* m_pCurrentNoteImage;
    CSWGuiImage* m_pNoteImage;
#else
    CSWGuiButton m_btnHider;
    CExoArrayList<CSWGuiButton*> m_aControls;
#endif
    CExoLinkedList<unsigned long> m_lstMapNotes;
    CExoLinkedListNode* m_pCurrentNote;
    unsigned char m_aUnknown240[4];
    int m_nMapMode;
};

class CSWGuiInGameMap : public CSWGuiPanel
{
public:
    CSWGuiInGameMap(CSWGuiManager* pManager);

    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
#ifndef _XBOX
    virtual CSWGuiControl* HitCheckMouse(int nX, int nY);
#endif
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();

    void OnUpArrowPressed(CSWGuiControl* pControl);
    void OnDownArrowPressed(CSWGuiControl* pControl);
    void OnXButton(CSWGuiControl* pControl);
    void ReturnToEbonHawk(CSWGuiControl* pControl);

    static const unsigned long STRREF_MAP_NOTE = 349;

    enum { STRREF_FUNCTION_UNAVAILABLE = 38451,
           STRREF_REJOIN_PARTY = 38452, STRREF_ENEMIES_NEARBY = 38462 };
    void SetMapNote(const CExoString& sNote);
#ifndef _XBOX
    void OnMapNoteClicked(CSWGuiControl* pControl);
#endif

    CSWGuiLabel m_lblMapNote;
    CSWGuiLabel m_lblArea;
    CSWGuiLabel m_lblTitle;
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblReturn;
    CSWGuiLabel m_lblPrtySlct;
    CSWGuiLabel m_lblStart;
    CSWGuiLabel m_lblExit;
    CSWGuiLabel m_lblUp;
    CSWGuiLabel m_lblDown;
    CSWGuiLabel m_lblNoteTitle;
#else
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnExit;
    CSWGuiButton m_btnReturn;
    CSWGuiLabel m_alblBar[5];
    CSWGuiLabel m_lbl1018;
#endif
    CSWGuiMapHider m_cMapHider;
    CSWGuiImage m_imgMap;
    int m_nMessageStrRef;

    unsigned long m_bPartySelection : 1;
    unsigned long m_bReturnButton : 1;
    unsigned long m_bUnknownFlagsRest : 30;
};

class CSWGuiInGameGalaxyMap : public CSWGuiPanel
{
public:
    CSWGuiInGameGalaxyMap(CSWGuiManager* pManager);

    enum { PLANET_COUNT = 16 };
    enum { STRREF_ALREADY_AT_LOCATION = 125629 };
    struct PlanetInfo
    {
        int m_nNameStrRef;
        int m_nDescStrRef;
        CResRef m_resModel;
    };

    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();

    void DisplayPlanet(int nPlanet, int bZoom);
#ifndef _XBOX
    void OnPlanetClicked(CSWGuiControl* pControl);
#endif
    void NextPlanet();
    void PrevPlanet();

#ifdef _XBOX
    CSWGuiLabelHilight m_aPlanetButtons[PLANET_COUNT];
    CSWGuiLabel m_lblPlanetDesc;
    CSWGuiLabel m_lblPlanetName;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblBack;
#else
    CSWGuiButton m_aPlanetButtons[PLANET_COUNT];
    CSWGuiLabel m_lblPlanetDesc;
    CSWGuiLabel m_lblPlanetName;
    CSWGuiLabel m_alblBar[2];
    CSWGuiButton m_btnNext;
    CSWGuiButton m_btnPrev;
#endif
    CSWGui3DSceneView m_cPlanetView;
    CSWGui3DSceneView m_cPlanetBackView;
    PlanetInfo m_aPlanets[PLANET_COUNT];
    int m_nSelectedPlanet;
#ifndef _XBOX
    int m_n28D4;
#endif
};

#endif
