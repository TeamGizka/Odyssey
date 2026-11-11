#ifndef __SWPAZAAK_H__
#define __SWPAZAAK_H__

struct CPazaakCard
{

    enum ECardTypeBase
    {
        CARD_TYPE_NEGATIVE_BASE = 6,
        CARD_TYPE_FLIPPABLE_BASE = 12
    };

    CPazaakCard()
    {
        m_bFlipped = 0;
    }

    CPazaakCard(int nType)
    {
        m_nType = nType;
        m_bFlipped = 0;
    }

    int GetCardValue()
    {
        if (m_nType < 0)
        {
            return 0;
        }
        if (m_nType >= 12 && m_nType < 18 && m_bFlipped != 0)
        {
            return -s_pnPazaakCardValues[m_nType];
        }
        return s_pnPazaakCardValues[m_nType];
    }

    static int s_pnPazaakCardValues[];

    int m_nType;
    int m_bFlipped;
};

class CPazaakPlayer
{
public:
    CPazaakPlayer();
    int GetTotal();
    int AddGameCard(CPazaakCard cCard);
    void ChooseSidedeck(CPazaakCard* pCards);

    CPazaakCard m_aSideDeck[4];
    CPazaakCard m_aBoard[9];
    int m_bStanding;
    int m_nWins;
};

class CSWPazaak
{
public:
    enum EAIState
    {
        AISTATE_DRAW = 0,
        AISTATE_PLAY_CARD = 1,
        AISTATE_DECIDE = 2,
        AISTATE_STAND = 3,
        AISTATE_END_TURN = 4
    };

    CSWPazaak();
    void LoadInitialSideDeck(int nSideDeck, CPazaakCard* pCards);
    void ShuffleDeck();
    CPazaakCard DrawCard();
    int DrawPlayerCard();

    unsigned long GetOpponent()
    {
        return m_oidOpponent;
    }

    int GetPlayerTotal()
    {
        return m_cPlayer.GetTotal();
    }

    int GetOpponentTotal()
    {
        return m_cOpponent.GetTotal();
    }

    void ClearGameBoard();
    int CanUsePlayerSidedeckCard(int nIndex);
    int UsePlayerSidedeckCard(int nIndex);
    int DetermineWinner();
    int FindPotentialSidecardTotal(CPazaakPlayer& cPlayer, int* pnCard,
                                   int bLowest);
    EAIState PlayAITurn(CPazaakPlayer& cSelf, CPazaakPlayer& cOpponent,
                        EAIState eState);

    int PlayOpponentTurn();

    int m_nWager;
    unsigned long m_oidOpponent;
    CPazaakPlayer m_cPlayer;
    CPazaakPlayer m_cOpponent;
    CPazaakCard m_aDeck[40];
    int m_nDeckIndex;
    EAIState m_nAIState;
};

#endif
