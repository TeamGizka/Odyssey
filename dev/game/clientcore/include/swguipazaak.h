#ifndef __SWGUIPAZAAK_H__
#define __SWGUIPAZAAK_H__

#include "swguicontrol.h"
#include "swgui.h"
struct CPazaakCard;
class CSWGuiControl;
class CSWGuiManager;
class CSWPazaak;
#include "exobase.h"
#include "aurlist.h"
#include "swpazaak.h"
#include "swguiingamecharacter.h"
#include "swparty.h"

class CResGFF;
class CResStruct;
class CSWGuiPazaakStart;

extern CSWGuiManager* g_pGuiMan;
extern int g_bPazaakShowNPCSideDeck;

class CSWGuiPazaakCard : public CSWGuiButton
{
public:

    enum { EVENT_CARD_DROP = 98283 };

    CSWGuiPazaakCard();
    virtual ~CSWGuiPazaakCard();
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
#ifndef _XBOX
    SWGUI_PCVIRTUAL int GetIsSelectable()  // PC 0x0067DE60
    {
#ifndef _XBOX
        return m_bSelectable & m_bHilightOnOver;
#else
        return m_bSelectable;
#endif
    }
#endif
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
#ifndef _XBOX
    SWGUI_PCVIRTUAL int HandleMouseCapturedMovement(int nX, int nY);
    SWGUI_PCVIRTUAL void DrawDragDrop(int nX, int nY);
    SWGUI_PCVIRTUAL void HandleLMouseDown();
    SWGUI_PCVIRTUAL void HandleLMouseUp();
#endif

    void SetCard(CPazaakCard cCard);

    void SetDimmed(int bDimmed)
    {
        m_cBorder.m_cParams.m_vColor =
            bDimmed ? Vector(0.667f, 0.667f, 0.667f) : Vector(1.0f, 1.0f, 1.0f);
    }

    CSWGuiLabel m_lblCard;
    CPazaakCard m_cCard;
#ifndef _XBOX
    int m_nDownX;
    int m_nDownY;
    int m_bDraggable;
    int m_bSelectable;
#endif
};

#ifdef _XBOX
class CSWGuiActivatedButton : public CSWGuiLabel
#else
class CSWGuiActivatedButton : public CSWGuiButton
#endif
{
public:
    CSWGuiActivatedButton();

    virtual void Draw(float fElapsed);
#ifndef _XBOX
    SWGUI_PCVIRTUAL void SetEnabled(int bEnabled);
#else
    void SetActivated(int bActivated);
#endif
    // PC 0x0067CA50
    void StartFlash(const Vector& vColor)
    {
        m_fFlashTimer = 0.8f;
        m_vFlashColor = vColor;
    }

    float m_fFlashTimer;
#ifdef _XBOX
    int m_bActivated;
#endif
    Vector m_vFlashColor;
};

class CSWGuiTutorial : public CSWGuiMessageBox
{
public:
    CSWGuiTutorial(CSWGuiManager* pManager);
    virtual void HandleInputEvent(int nEvent, int nState);

    void PlaceRightOf(CSWGuiControl* pControl);
    void PlaceAbove(CSWGuiControl* pControl);
    void ShowHelp(unsigned long nHelpStrRef, unsigned long nTitleStrRef,
                  unsigned long nButtonStrRef);
    void ShowMultipleHelp(const unsigned long* pStrRefs, int nCount);

    int GetEnabled()
    {
        return m_bEnabled;
    }

    List<unsigned long> m_lHelp;
    int m_nHelpIndex;
    int m_bEnabled;
#ifndef _XBOX
    CSWGuiControl* m_pPlacedAbove;
#endif
};

class CSWGuiPazaakStart : public CSWGuiPanel
{
public:

    enum { ChosenCardCount = 10 };
    CSWGuiPazaakStart(CSWGuiManager* pManager, int nWager, int nSideDeck,
                      int bTutorial, unsigned long oidOpponent);
    virtual ~CSWGuiPazaakStart();
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();

    void HandleWagerExit(int nWager);
    void HandleQuitDialog(CSWGuiControl* pControl);
    int IsAvailCardActive();
    int ClearSideDeck();
    void ShowDisabledButtons();
    void OnAvailCardEntered(CSWGuiControl* pControl);
    void OnChosenCardEntered(CSWGuiControl* pControl);
    int AddChosenCard(int nAvail, int nSlot);
    int RemoveChosenCard(int nSlot);
#ifndef _XBOX
    void HandleAvailCardDrop(CSWGuiControl* pControl);
#endif
#ifndef _XBOX
    void HandleChosenCardDrop(CSWGuiControl* pControl);
#endif
    void HandleMoveCard(CSWGuiControl* pControl);
    void HandleStartDialog(CSWGuiControl* pControl);
    void HandleAcceptButton(CSWGuiControl* pControl);

    CSWGuiLabel m_lblTitle;
    CSWGuiPazaakCard m_cAvail[18];
    CSWGuiLabel m_lblAvailCount[18];
    CSWGuiPazaakCard m_cChosen[ChosenCardCount];

#ifdef _XBOX
    CSWGuiLabel m_lblYbutton;
#endif
    CSWGuiActivatedButton m_btnMove;
#ifdef _XBOX
    CSWGuiLabel m_lblAbutton;
#endif
    CSWGuiActivatedButton m_btnAtext;
#ifdef _XBOX
    CSWGuiLabel m_lblLbutton;
#endif
    CSWGuiLabel m_lblLtext;
#ifdef _XBOX
    CSWGuiLabel m_lblRbutton;
#endif
    CSWGuiLabel m_lblRtext;
    int m_anAvailCount[18];
    CPazaakCard m_aChosen[ChosenCardCount];
    CSWGuiPanel* m_pWagerPopup;
#ifdef _XBOX
    int m_bChosenActive;
#endif
    CSWGuiControl* m_pChosenCards;
#ifndef _XBOX
    CSWGuiControl* m_pAvailCards;
#endif
    int m_bSideDeckFull;
    CSWPazaak* m_pPazaak;
    CSWGuiMessageBox m_cMessageBox;
    CSWGuiTutorial m_cTutorial;
};

class CSWGuiPazaakGame : public CSWGuiPanel
{
public:

    enum GameState
    {
        STATE_START_GAME = 0,
        STATE_PLAYER_SIDE_CARD_PLAYED = 4,
        STATE_END_PLAYER_TURN = 5,
        STATE_EXIT_GAME = 12
    };

    CSWGuiPazaakGame(CSWGuiManager* pManager, CSWPazaak* pPazaak, int nUnknown,
                     unsigned long nUnknown2);
    virtual ~CSWGuiPazaakGame();
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();

    int RefreshDisplay();
    void ReconnectSidedeckButtons();
    void HandleQuitDialog(CSWGuiControl* pControl);
    int SetHandEnabled(int bEnabled);
    void CancelHandAction();
    void HandleXButton(CSWGuiControl* pControl);
    void HandleBlackButton(CSWGuiControl* pControl);
    void HandleFlipHandCard();
    void HandleHandCardRightClick(CSWGuiControl* pControl);
    void HandleFlipButtons(CSWGuiControl* pControl);
    void StartNewGame();
    void HandleEndDialog(CSWGuiControl* pControl);
    void HandleTutorialClose(CSWGuiControl* pControl);
    void HandleContinue(CSWGuiControl* pControl);
    void HandleStand(CSWGuiControl* pControl);
    void HandlePlayHandCard(int nIndex);
#ifndef _XBOX
    void HandleHandCardDrop(CSWGuiControl* pControl);
#endif
    void HandleHandCardDoubleClick(CSWGuiControl* pControl);
    void HandleHandcardAction(CSWGuiControl* pControl);
#ifdef _XBOX
    void OnHandCardEntered(CSWGuiControl* pControl);
#endif
    int DoGameSequence();

    CSWGuiLabel m_lblPlayerName;
    CSWGuiLabel m_lblNpcName;
    CSWGuiLabel m_lblPlayerTotal;
    CSWGuiLabel m_lblNpcTotal;
    CSWGuiLabel m_lblPlayerSideDeck;
    CSWGuiLabel m_lblNpcSideDeck;
    CSWGuiLabel m_lblPlayerTurn;
    CSWGuiLabel m_lblNpcTurn;
    CSWGuiLabel m_lblPlayerScore[3];
    CSWGuiLabel m_lblNpcScore[3];
    CSWGuiPazaakCard m_cPlayerBoard[9];
    CSWGuiPazaakCard m_cPlayerHand[4];
    CSWGuiPazaakCard m_cNpcBoard[9];
    CSWGuiPazaakCard m_cNpcHand[4];

#ifndef _XBOX
    CSWGuiButton m_btnFlip[4];
    CSWGuiLabel m_lblFlipIcon;
    CSWGuiLabel m_lblFlipLegend;
#else
    CSWGuiActivatedButton m_lblXbutton;
#endif
    CSWGuiActivatedButton m_btnContinue;
#ifdef _XBOX
    CSWGuiActivatedButton m_lblYbutton;
#endif
    CSWGuiActivatedButton m_btnStand;
#ifdef _XBOX
    CSWGuiActivatedButton m_lblAbutton;
#endif
    CSWGuiActivatedButton m_btnX;
#ifdef _XBOX
    CSWGuiActivatedButton m_lblBbutton;
#endif
    CSWGuiActivatedButton m_btnBlack;
    CSWGuiMessageBox m_cMessageBox;
    CSWGuiTutorial m_cTutorial;
    char* m_szStartTurnSound;
    char* m_szDrawMainSound;
    char* m_szPlaySideSound;
    char* m_szWarnBustSound;
    char* m_szEndSound;
    CSWPazaak* m_pPazaak;
    CSWPazaak* GetPazaak();
    int m_nState;
    float m_fTimer;
    unsigned long m_bUnknown86DC0 : 1;
    unsigned long m_bShownFirstCardHelp : 1;
    unsigned long m_bShownOpponentTurnHelp : 1;
    unsigned long m_bShownGameStartHelp : 1;
    unsigned long m_bShownTwentyHelp : 1;
    unsigned long m_nUnknown86DC5 : 27;
#ifndef _XBOX
    int m_nPlayedCard;
    int m_nHandAction;
#endif
};

class CSWGuiWagerPopup : public CSWGuiPanel
{
public:

    enum { STRREF_MAXIMUM_WAGER = 32321, STRREF_CREDITS = 38600 };

    CSWGuiWagerPopup(CSWGuiManager* pManager, CSWGuiPazaakStart* pStart,
                     int nMaxWager);
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);

    void UpdateWagerText();
    void HandleButtonLess(CSWGuiControl* pControl);
    void HandleButtonMore(CSWGuiControl* pControl);
    void HandleButtonWager(CSWGuiControl* pControl);
    void HandleButtonQuit(CSWGuiControl* pControl)  // PC 0x0040B650
    {
        HandleInputEvent(GUIEVENT_CANCEL, 1);
    }

    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblMaximum;
    CSWGuiLabel m_lblWagerval;
#ifdef _XBOX
    CSWGuiLabel m_lblLess;
    CSWGuiLabel m_lblMore;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblWager;
    CSWGuiLabel m_lblQuit;
#else
    CSWGuiSpeedButton m_btnLess;
    CSWGuiSpeedButton m_btnMore;
    CSWGuiButton m_btnWager;
    CSWGuiButton m_btnQuit;
#endif
    CSWGuiLabel m_lblBg;
    int m_nWager;
    int m_nMaxWager;
    CExoArrayList<int> m_lUnknownC9C;
    CSWGuiPazaakStart* m_pStart;
};

#endif
