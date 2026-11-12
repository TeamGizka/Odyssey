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

enum { PAZAAK_TOTAL_BUST = -1000000, PAZAAK_TOTAL_TARGET = 20 };

enum
{
    STRREF_PAZAAK_CONFIRM_SIDEDECK = 32322,
    STRREF_PAZAAK_PLAY_CARD = 32330,
    STRREF_PAZAAK_END_TURN = 32332,
    STRREF_PAZAAK_STAND = 32333,
    STRREF_PAZAAK_SET_WON = 32334,
    STRREF_PAZAAK_SET_LOST = 32335,
    STRREF_PAZAAK_MATCH_WON = 32336,
    STRREF_PAZAAK_MATCH_LOST = 32337,
    STRREF_PAZAAK_SET_TIED = 32338,
    STRREF_PAZAAK_ADD_CARD = 38601,
    STRREF_PAZAAK_REMOVE_CARD = 38602,
    STRREF_PAZAAK_CONTINUE = 38623,
    STRREF_PAZAAK_BACK = 38624,
    STRREF_PAZAAK_QUIT = 38625,
    STRREF_PAZAAK_CANCEL = 38626,
    STRREF_PAZAAK_HELP_SIDEDECK_INTRO = 38627,
    STRREF_PAZAAK_HELP_BASIC_DECK = 38628,
    STRREF_PAZAAK_HELP_SELECT_SIDEDECK = 38629,
    STRREF_PAZAAK_SELECT_TEN_CARDS = 38630,
    STRREF_PAZAAK_HELP_OBJECT = 38631,
    STRREF_PAZAAK_HELP_SETS = 38632,
    STRREF_PAZAAK_HELP_DRAW = 38633,
    STRREF_PAZAAK_HELP_HAND = 38634,
    STRREF_PAZAAK_HELP_PLAY_HAND_CARD = 38635,
    STRREF_PAZAAK_HELP_NEGATIVE_TIP = 38636,
    STRREF_PAZAAK_HELP_TURN_OVER = 38637,
    STRREF_PAZAAK_HELP_STAND_RULES = 38638,
    STRREF_PAZAAK_HELP_CHOOSE_ACTION = 38639,
    STRREF_PAZAAK_HELP_STAND = 38640,
    STRREF_PAZAAK_HELP_END_TURN = 38641,
    STRREF_PAZAAK_HELP_PLAY_CARD = 38642,
    STRREF_PAZAAK_HELP_FLIP_CARDS = 38643,
    STRREF_PAZAAK_HELP_DECLARE_SIGN = 38644,
    STRREF_PAZAAK_HELP_OPPONENT = 38645,
    STRREF_PAZAAK_HELP_TWENTY = 38646,
    STRREF_PAZAAK_HELP_BUST = 38647,
    STRREF_PAZAAK_HELP_MATCH = 38648,
    STRREF_PAZAAK_MATCH_OVER = 38649,
    STRREF_PAZAAK_QUIT_FORFEIT = 42424,
    STRREF_PAZAAK_CONFIRM_FORFEIT = 42425,
    STRREF_PAZAAK_HELP_CARD_PLUS = 128418,
    STRREF_PAZAAK_HELP_CARD_MINUS = 128419,
    STRREF_PAZAAK_HELP_CARD_PLUS_MINUS = 128420,
    STRREF_PAZAAK_HELP_CARD_PLUS_MINUS_1_2 = 128421,
    STRREF_PAZAAK_HELP_CARD_DOUBLE = 128422,
    STRREF_PAZAAK_HELP_CARD_TIEBREAKER = 128423,
    STRREF_PAZAAK_HELP_CARD_FLIP_2_4 = 128424,
    STRREF_PAZAAK_HELP_CARD_FLIP_3_6 = 128425
};

class CSWGuiPazaakCard : public CSWGuiButton
{
public:

    enum { EVENT_CARD_DROP = 98283 };
    CSWGuiPazaakCard();
#ifndef _XBOX
    virtual ~CSWGuiPazaakCard();
#endif
    virtual void Draw(float fElapsed); virtual void HandleInputEvent(int nEvent, int nState);
#ifndef _XBOX
    SWGUI_PCVIRTUAL int GetIsSelectable()  // PC 0x006D4780
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
    // PC 0x006D2FF0
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

    enum { ChosenCardCount = 10, AvailCardCount = 24 };
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
    void OnAvailCardEntered(CSWGuiControl* pControl); void OnChosenCardEntered(CSWGuiControl* pControl);
#ifndef _XBOX
    int AddChosenCard(int nAvail, int nSlot);
#else
    int AddChosenCard(int nAvail);
#endif
    int RemoveChosenCard(int nSlot);
#ifndef _XBOX
    void HandleAvailCardDrop(CSWGuiControl* pControl);
    void HandleChosenCardDrop(CSWGuiControl* pControl);
#endif
    void HandleMoveCard(CSWGuiControl* pControl); void HandleStartDialog(CSWGuiControl* pControl);
    void HandleAcceptButton(CSWGuiControl* pControl);
#ifndef _XBOX
    void UpdateHelpText(int nCardType);
    void ClearSideDeck(CSWGuiControl* pControl);
#endif

    CSWGuiLabel m_lblTitle;
    CSWGuiPazaakCard m_cAvail[AvailCardCount];
    CSWGuiLabel m_lblAvailCount[AvailCardCount];
    CSWGuiPazaakCard m_cChosen[ChosenCardCount];

#ifdef _XBOX
    CSWGuiLabel m_lblYbutton;
    CSWGuiActivatedButton m_btnMove;
    CSWGuiLabel m_lblAbutton;
#endif
    CSWGuiActivatedButton m_btnAtext;
#ifdef _XBOX
    CSWGuiLabel m_lblLbutton;
#endif
    CSWGuiLabel m_lblLtext;
#ifdef _XBOX
    CSWGuiLabel m_lblRbutton;
#else
    CSWGuiLabel m_lblHelp;
    CSWGuiButton m_btnClearCards;
#endif
    CSWGuiLabel m_lblRtext;
    int m_anAvailCount[AvailCardCount];
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
    void HandleChangeHandCard();
    void HandleHandCardRightClick(CSWGuiControl* pControl);
    void HandleFlipButtons(CSWGuiControl* pControl);
    void HandleChangeButtons(CSWGuiControl* pControl);
    void HandleForfeitButton(CSWGuiControl* pControl);
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
    CSWGuiButton m_btnChange[4];
    CSWGuiButton m_btnForfeitGame;
    CSWGuiLabel m_lblFlipIcon;
    CSWGuiLabel m_lblFlipLegend;
    CSWGuiLabel m_lblChangeIcon;
    CSWGuiLabel m_lblChangeLegend;
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
    int m_bPlayerStarts;
};

class CSWGuiWagerPopup : public CSWGuiPanel
{
public:

    enum { STRREF_MAXIMUM_WAGER = 32321, STRREF_CREDITS = 38600 };

    CSWGuiWagerPopup(CSWGuiManager* pManager, CSWGuiPazaakStart* pStart,
                     int nMaxWager);
    virtual void Draw(float fElapsed); virtual void HandleInputEvent(int nEvent, int nState);
    void UpdateWagerText();
    void HandleButtonLess(CSWGuiControl* pControl);
    void HandleButtonMore(CSWGuiControl* pControl);
    void HandleButtonWager(CSWGuiControl* pControl);
    void HandleButtonQuit(CSWGuiControl* pControl)  // PC 0x0040B8A0
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
    CSWGuiLabel m_alblBar[1];
    CSWGuiLabel m_lblBg; int m_nWager; int m_nMaxWager;
#ifdef _XBOX
    struct CFlashLabel { CSWGuiLabel* pLabel; float fTimer; }; CExoArrayList<CFlashLabel> m_lFlashLabels; void FlashLabel(CSWGuiLabel* pLabel);
#else
    CExoArrayList<int> m_lUnknownC9C;
#endif
    CSWGuiPazaakStart* m_pStart;
};

#endif
