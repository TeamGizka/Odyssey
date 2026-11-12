#ifndef __SWPAZAAK_H__
#define __SWPAZAAK_H__

struct CPazaakCard
{

    enum ECardTypeBase
    {
        CARD_TYPE_NEGATIVE_BASE = 6,
        CARD_TYPE_FLIPPABLE_BASE = 12,
        CARD_TYPE_TIEBREAKER = 18,
        CARD_TYPE_DOUBLE = 19,
        CARD_TYPE_FLIP_2_4 = 20,
        CARD_TYPE_FLIP_3_6 = 21,
        CARD_TYPE_PLUS_MINUS_1_2 = 22,
        CARD_TYPE_DECK_BASE = 23
    };

    CPazaakCard()
    {
        m_bFlipped = 0;
        m_nValue = 0;
    }

    CPazaakCard(int nType)
    {
        m_nType = nType;
        m_bFlipped = 0;
        m_nValue = 0;
    }

    int IsFlippable()
    {
        return (m_nType >= CARD_TYPE_FLIPPABLE_BASE && m_nType < CARD_TYPE_TIEBREAKER) ||
               m_nType == CARD_TYPE_TIEBREAKER;
    }

    int GetCardValue()
    {
        if (m_nType < 0)
        {
            return 0;
        }
        if (IsFlippable() && m_bFlipped != 0)
        {
            return -s_pnPazaakCardValues[m_nType];
        }
        if (m_nType == CARD_TYPE_DOUBLE)
        {
            return m_nValue;
        }
        if (m_nType == CARD_TYPE_PLUS_MINUS_1_2)
        {
            if (m_nValue == 0)
            {
                m_nValue = 1;
            }
            if (m_bFlipped != 0)
            {
                return -m_nValue;
            }
            return m_nValue;
        }
        if (m_nType != CARD_TYPE_FLIP_2_4 && m_nType != CARD_TYPE_FLIP_3_6 && m_nValue != 0)
        {
            return m_nValue;
        }
        return s_pnPazaakCardValues[m_nType];
    }

    static int s_pnPazaakCardValues[];

    int m_nType;
    int m_bFlipped;
    int m_nValue;
};

class CPazaakPlayer
{
public:
    CPazaakPlayer();
    int GetTotal();
    void AddGameCard(CPazaakCard cCard);
    void ChooseSidedeck(CPazaakCard* pCards);
    int HasTiebreaker();
    void FlipPositiveNegative(int nValue1, int nValue2);
    int GetNumCardsPlayed();

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
    void DrawPlayerCard();

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
    int FindPotentialDoubleTotal(CPazaakPlayer& cPlayer, int nCard);
    int FindPotentialFlipTotal(CPazaakPlayer& cPlayer, int nCard);
    int FindPotentialChangeTotal(CPazaakPlayer& cPlayer, int nCard, int& bFlipped,
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
