#include <stdlib.h>

#include "swpazaak.h"
#include "twodimarray.h"
#include "exoresman.h"
#include "exobase.h"
#include "gameobjectarray.h"

int CPazaakCard::s_pnPazaakCardValues[33] =
{
    1, 2, 3, 4, 5, 6,
    -1, -2, -3, -4, -5, -6,
    1, 2, 3, 4, 5, 6,
    1, 1, 0, 0, 1,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10
};
// PC 0x007554D0
CSWPazaak::CSWPazaak()
{
    m_nWager = 0;
    m_oidOpponent = OBJECT_INVALID;
    m_nDeckIndex = -1;
    m_nAIState = AISTATE_DRAW;
}
// PC 0x007558E0
CSWPazaak::EAIState CSWPazaak::PlayAITurn(CPazaakPlayer& cSelf,
                                          CPazaakPlayer& cOpponent,
                                          EAIState eState)
{
    int nCard;
    int nOpponentTotal = cOpponent.GetTotal();
    if (nOpponentTotal > 20)
    {
        nOpponentTotal = -1000000;
        cOpponent.m_bStanding = 1;
    }
    switch (eState)
    {
    case AISTATE_STAND:
    case AISTATE_END_TURN:
        return AISTATE_DRAW;
    case AISTATE_DRAW:
        if (cSelf.m_bStanding != 0)
        {
            return AISTATE_DRAW;
        }
        cSelf.AddGameCard(DrawCard());
        return AISTATE_PLAY_CARD;
    case AISTATE_PLAY_CARD:
    {
        int bOpponentBehind;
        int nSelfTotal = cSelf.GetTotal();
        bOpponentBehind = 1;
        if (cOpponent.m_nWins + 1 >= 3)
        {
            bOpponentBehind = 0;
        }
        if (cOpponent.m_bStanding != 0 &&
            ((nSelfTotal > nOpponentTotal && nSelfTotal <= 20) ||
             cSelf.GetNumCardsPlayed() >= 9))
        {
            cSelf.m_bStanding = 1;
            return AISTATE_STAND;
        }
        int nLowest = FindPotentialSidecardTotal(cSelf, &nCard, 1);
        nCard = -1;
        int nHighest = FindPotentialSidecardTotal(cSelf, &nCard, 0);
        if (nHighest >= 18 && nHighest >= nOpponentTotal &&
            (nHighest >= 20 || ((nHighest >= 19 && nLowest > 11) ||
                                nLowest > 13)) &&
            nCard >= 0)
        {
            if (cSelf.m_aSideDeck[nCard].m_nType == CPazaakCard::CARD_TYPE_FLIP_2_4)
            {
                cSelf.FlipPositiveNegative(2, 4);
            }
            else if (cSelf.m_aSideDeck[nCard].m_nType == CPazaakCard::CARD_TYPE_FLIP_3_6)
            {
                cSelf.FlipPositiveNegative(3, 6);
            }
            cSelf.AddGameCard(cSelf.m_aSideDeck[nCard]);
            CPazaakCard cEmpty;
            cEmpty.m_nType = -1;
            cEmpty.m_bFlipped = 0;
            cEmpty.m_nValue = 0;
            cSelf.m_aSideDeck[nCard] = cEmpty;
            return AISTATE_DECIDE;
        }
        else if (nSelfTotal > 20)
        {
            if (bOpponentBehind && nOpponentTotal >= 18 &&
                nHighest < nOpponentTotal)
            {
                return AISTATE_END_TURN;
            }
            if (nCard >= 0)
            {
                if (cSelf.m_aSideDeck[nCard].m_nType == CPazaakCard::CARD_TYPE_FLIP_2_4)
                {
                    cSelf.FlipPositiveNegative(2, 4);
                }
                else if (cSelf.m_aSideDeck[nCard].m_nType == CPazaakCard::CARD_TYPE_FLIP_3_6)
                {
                    cSelf.FlipPositiveNegative(3, 6);
                }
                cSelf.AddGameCard(cSelf.m_aSideDeck[nCard]);
                CPazaakCard cEmpty;
                cEmpty.m_nType = -1;
                cEmpty.m_bFlipped = 0;
                cEmpty.m_nValue = 0;
                cSelf.m_aSideDeck[nCard] = cEmpty;
                return AISTATE_DECIDE;
            }
        }
    }
    case AISTATE_DECIDE:
    {
        int nSelfTotal = cSelf.GetTotal();
        if (nSelfTotal > 20)
        {
            nSelfTotal = -1000000;
        }
        if (cOpponent.m_bStanding != 0 && nSelfTotal < nOpponentTotal)
        {
            return AISTATE_END_TURN;
        }
        if (nSelfTotal >= 18)
        {
            cSelf.m_bStanding = 1;
            return AISTATE_STAND;
        }
        if (FindPotentialSidecardTotal(cSelf, &nCard, 1) >= 16)
        {
            cSelf.m_bStanding = 1;
            return AISTATE_STAND;
        }
        return AISTATE_END_TURN;
    }
    default:
        return AISTATE_END_TURN;
    }
}
// PC 0x00754900
int CPazaakPlayer::GetTotal()
{
    int nTotal = 0;

    for (int i = 0; i < 9; i++)
    {
        if (m_aBoard[i].m_nType == -1)
        {
            break;
        }
        nTotal += m_aBoard[i].GetCardValue();
    }
    return nTotal;
}
// PC 0x00754930
void CPazaakPlayer::ChooseSidedeck(CPazaakCard* pCards)
{
    int nCount = 10;

    for (int i = 0; i < 4; i++)
    {
        int nChoice = rand() % nCount;
        m_aSideDeck[i] = pCards[nChoice];
        pCards[nChoice] = pCards[nCount - 1];
        CPazaakCard cEmpty;
        cEmpty.m_nType = -1;
        pCards[nCount - 1] = cEmpty;
        nCount--;
    }
}
// PC 0x007549C0
void CPazaakPlayer::AddGameCard(CPazaakCard cCard)
{
    int nSlot;

    if (cCard.m_nType == CPazaakCard::CARD_TYPE_FLIP_2_4 ||
        cCard.m_nType == CPazaakCard::CARD_TYPE_FLIP_3_6)
    {
        cCard.m_nValue = 1;
    }
    CPazaakCard cLastCard;
    for (nSlot = 0; nSlot < 9; nSlot++)
    {
        if (m_aBoard[nSlot].m_nType == -1)
        {
            if (cCard.m_nType == CPazaakCard::CARD_TYPE_DOUBLE)
            {
                cCard.m_nValue = cLastCard.GetCardValue();
            }
            m_aBoard[nSlot] = cCard;
            break;
        }
        cLastCard = m_aBoard[nSlot];
    }
    if (nSlot >= 8 || GetTotal() == 20)
    {
        m_bStanding = 1;
    }
}
// PC 0x00754B00
void CSWPazaak::ClearGameBoard()
{
    for (int i = 0; i < 9; i++)
    {
        CPazaakCard cPlayerCard;
        cPlayerCard.m_nType = -1;
        cPlayerCard.m_bFlipped = 0;
        cPlayerCard.m_nValue = 0;
        m_cPlayer.m_aBoard[i] = cPlayerCard;
        CPazaakCard cOpponentCard;
        cOpponentCard.m_nType = -1;
        cOpponentCard.m_bFlipped = 0;
        cOpponentCard.m_nValue = 0;
        m_cOpponent.m_aBoard[i] = cOpponentCard;
    }
    m_cPlayer.m_bStanding = 0;
    m_cOpponent.m_bStanding = 0;
}
// PC 0x00754C40
void CSWPazaak::ShuffleDeck()
{
    CPazaakCard aCards[40];
    int i;

    for (i = 0; i < 40; i++)
    {
        CPazaakCard cCard;
        cCard.m_nType = (i % 10) + CPazaakCard::CARD_TYPE_DECK_BASE;
        aCards[i] = cCard;
    }
    int nCount = 40;
    for (i = 0; i < 40; i++)
    {
        int nChoice = rand() % nCount;
        m_aDeck[i] = aCards[nChoice];
        aCards[nChoice] = aCards[nCount - 1];
        CPazaakCard cEmpty;
        cEmpty.m_nType = -1;
        aCards[nCount - 1] = cEmpty;
        nCount--;
    }
    m_nDeckIndex = 39;
}
// PC 0x00754D30
CPazaakCard CSWPazaak::DrawCard()
{
    if (m_nDeckIndex < 0)
    {
        ShuffleDeck();
    }
    return m_aDeck[m_nDeckIndex--];
}
// PC 0x00754D80
int CSWPazaak::CanUsePlayerSidedeckCard(int nIndex)
{
    return m_cPlayer.m_aSideDeck[nIndex].m_nType >= 0;
}
// PC 0x00755520
int CSWPazaak::UsePlayerSidedeckCard(int nIndex)
{
    if (m_cPlayer.m_aSideDeck[nIndex].m_nType < 0)
    {
        return 0;
    }
    if (m_cPlayer.m_aSideDeck[nIndex].m_nType == CPazaakCard::CARD_TYPE_FLIP_2_4)
    {
        m_cPlayer.FlipPositiveNegative(2, 4);
    }
    else if (m_cPlayer.m_aSideDeck[nIndex].m_nType == CPazaakCard::CARD_TYPE_FLIP_3_6)
    {
        m_cPlayer.FlipPositiveNegative(3, 6);
    }
    m_cPlayer.AddGameCard(m_cPlayer.m_aSideDeck[nIndex]);
    CPazaakCard cEmpty;
    cEmpty.m_nType = -1;
    cEmpty.m_bFlipped = 0;
    cEmpty.m_nValue = 0;
    m_cPlayer.m_aSideDeck[nIndex] = cEmpty;
    return 1;
}
// PC 0x00755320
int CPazaakPlayer::HasTiebreaker()
{
    for (int i = 0; i < 9; i++)
    {
        if (m_aBoard[i].m_nType == CPazaakCard::CARD_TYPE_TIEBREAKER)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x00755340
void CPazaakPlayer::FlipPositiveNegative(int nValue1, int nValue2)
{
    for (int i = 0; i < 9; i++)
    {
        if (m_aBoard[i].m_nType == -1)
        {
            break;
        }
        if (m_aBoard[i].GetCardValue() == nValue1 ||
            m_aBoard[i].GetCardValue() == nValue2)
        {
            if (m_aBoard[i].IsFlippable())
            {
                m_aBoard[i].m_bFlipped = !m_aBoard[i].m_bFlipped;
            }
            else
            {
                m_aBoard[i].m_nValue = -m_aBoard[i].GetCardValue();
            }
        }
    }
}
// PC 0x00755590
int CSWPazaak::DetermineWinner()
{
    int nPlayerTotal = m_cPlayer.GetTotal();
    int nOpponentTotal = m_cOpponent.GetTotal();

    if (nPlayerTotal > 20 || nOpponentTotal > 20)
    {
        m_cPlayer.m_bStanding = 1;
        m_cOpponent.m_bStanding = 1;
    }
    if (!(m_cPlayer.m_bStanding == 0 || m_cOpponent.m_bStanding == 0))
    {
        if (nPlayerTotal > 20)
        {
            nPlayerTotal = -1000000;
        }
        if (nOpponentTotal > 20)
        {
            nOpponentTotal = -1000000;
        }
        int nPlayerCards = m_cPlayer.GetNumCardsPlayed();
        int nOpponentCards = m_cOpponent.GetNumCardsPlayed();
        if (nPlayerCards == 9)
        {
            if (nOpponentCards != nPlayerCards && nPlayerTotal != -1000000)
            {
                return 1;
            }
        }
        else if (nOpponentCards == 9 && nOpponentTotal != -1000000)
        {
            return 2;
        }
        if (nPlayerTotal > nOpponentTotal)
        {
            return 1;
        }
        if (nPlayerTotal < nOpponentTotal)
        {
            return 2;
        }
        int bPlayerTiebreaker = m_cPlayer.HasTiebreaker();
        int bOpponentTiebreaker = m_cOpponent.HasTiebreaker();
        if (bPlayerTiebreaker && !bOpponentTiebreaker)
        {
            return 1;
        }
        if (!bPlayerTiebreaker && bOpponentTiebreaker)
        {
            return 2;
        }
        return 3;
    }
    return 0;
}
// PC 0x007553B0
int CPazaakPlayer::GetNumCardsPlayed()
{
    int nCards = 0;

    for (int i = 0; i < 9; i++)
    {
        if (m_aBoard[i].m_nType == -1)
        {
            break;
        }
        nCards++;
    }
    return nCards;
}
// PC 0x00755710
int CSWPazaak::FindPotentialSidecardTotal(CPazaakPlayer& cPlayer, int* pnCard,
                                          int bLowest)
{
    int bFlipped = 0;
    int nTotal = cPlayer.GetTotal();
    int nBest = nTotal;
    if (nBest > 20)
    {
        nBest = -1000000;
    }
    int nBestCard = -1;
    int nBestFlipped = 0;
    int bBestFlippable = 0;

    for (int nCard = 0; nCard < 4; nCard++)
    {
        for (int nTry = 2; nTry != 0; nTry--)
        {
            int nNew;
            int nType = cPlayer.m_aSideDeck[nCard].m_nType;
            bFlipped = 0;
            if (nType == CPazaakCard::CARD_TYPE_DOUBLE)
            {
                nNew = FindPotentialDoubleTotal(cPlayer, nCard);
                nTry = 1;
            }
            else if (nType == CPazaakCard::CARD_TYPE_FLIP_2_4 ||
                     nType == CPazaakCard::CARD_TYPE_FLIP_3_6)
            {
                nNew = FindPotentialFlipTotal(cPlayer, nCard);
                nTry = 1;
            }
            else if (nType == CPazaakCard::CARD_TYPE_PLUS_MINUS_1_2)
            {
                nNew = FindPotentialChangeTotal(cPlayer, nCard, bFlipped, bLowest);
                nBestFlipped = bFlipped;
                nTry = 1;
            }
            else
            {
                if ((nType >= CPazaakCard::CARD_TYPE_FLIPPABLE_BASE &&
                     nType < CPazaakCard::CARD_TYPE_TIEBREAKER) ||
                    nType == CPazaakCard::CARD_TYPE_TIEBREAKER)
                {
                    bFlipped = (nTry == 2);
                    cPlayer.m_aSideDeck[nCard].m_bFlipped = bFlipped;
                }
                else
                {
                    nTry = 1;
                }
                nNew = nTotal + cPlayer.m_aSideDeck[nCard].GetCardValue();
            }
            if (nNew > 20)
            {
                nNew = -1000000;
            }
            if ((bLowest == 0 && nNew > nBest) ||
                (bLowest != 0 && (nNew < nBest || nBest == -1000000) && nNew > -1000000) ||
                (nNew == nBest && bBestFlippable != 0))
            {
                nBest = nNew;
                nBestCard = nCard;
                nBestFlipped = bFlipped;
                nType = cPlayer.m_aSideDeck[nCard].m_nType;
                bBestFlippable = (nType >= 12 && nType < 18);
            }
        }
    }
    if (nBestCard >= 0)
    {
        *pnCard = nBestCard;
        int nBestType = cPlayer.m_aSideDeck[nBestCard].m_nType;
        if ((nBestType >= CPazaakCard::CARD_TYPE_FLIPPABLE_BASE &&
             nBestType < CPazaakCard::CARD_TYPE_TIEBREAKER) ||
            nBestType == CPazaakCard::CARD_TYPE_TIEBREAKER ||
            nBestType == CPazaakCard::CARD_TYPE_PLUS_MINUS_1_2)
        {
            cPlayer.m_aSideDeck[nBestCard].m_bFlipped = nBestFlipped;
        }
    }
    return nBest;
}
// PC 0x00754F10
int CSWPazaak::FindPotentialDoubleTotal(CPazaakPlayer& cPlayer, int nCard)
{
    CPazaakCard cLastCard;

    for (int i = 0; i < 9; i++)
    {
        if (cPlayer.m_aBoard[i].m_nType == -1)
        {
            break;
        }
        cLastCard = cPlayer.m_aBoard[i];
    }
    return cPlayer.GetTotal() + cLastCard.GetCardValue();
}
// PC 0x00754DA0
int CSWPazaak::FindPotentialFlipTotal(CPazaakPlayer& cPlayer, int nCard)
{
    int nValue1 = 0;
    int nValue2 = 0;
    CPazaakCard cCard = cPlayer.m_aSideDeck[nCard];
    int i;

    if (cCard.m_nType == CPazaakCard::CARD_TYPE_FLIP_2_4)
    {
        nValue1 = 2;
        nValue2 = 4;
    }
    else if (cCard.m_nType == CPazaakCard::CARD_TYPE_FLIP_3_6)
    {
        nValue1 = 3;
        nValue2 = 6;
    }
    CPazaakCard aBoard[9];
    for (i = 0; i < 9; i++)
    {
        cCard = cPlayer.m_aBoard[i];
        if (cCard.GetCardValue() == nValue1 || cCard.GetCardValue() == nValue2)
        {
            cCard.m_nValue = -cCard.GetCardValue();
        }
        aBoard[i] = cCard;
    }
    int nTotal = 0;
    for (i = 0; i < 9; i++)
    {
        nTotal += aBoard[i].GetCardValue();
    }
    return nTotal;
}
// PC 0x00755080
int CSWPazaak::FindPotentialChangeTotal(CPazaakPlayer& cPlayer, int nCard,
                                        int& bFlipped, int bLowest)
{
    int nTotal = cPlayer.GetTotal();
    int nNew;

    if (bLowest == 0)
    {
        cPlayer.m_aSideDeck[nCard].m_nValue = 2;
        cPlayer.m_aSideDeck[nCard].m_bFlipped = 0;
        nNew = nTotal + cPlayer.m_aSideDeck[nCard].GetCardValue();
        if (nNew > 20)
        {
            cPlayer.m_aSideDeck[nCard].m_nValue = 1;
            nNew = nTotal + cPlayer.m_aSideDeck[nCard].GetCardValue();
            if (nNew > 20)
            {
                cPlayer.m_aSideDeck[nCard].m_bFlipped = 1;
                bFlipped = 1;
                nNew = nTotal + cPlayer.m_aSideDeck[nCard].GetCardValue();
                if (nNew > 20)
                {
                    cPlayer.m_aSideDeck[nCard].m_nValue = 2;
                    nNew = nTotal + cPlayer.m_aSideDeck[nCard].GetCardValue();
                }
            }
        }
    }
    else
    {
        cPlayer.m_aSideDeck[nCard].m_nValue = 2;
        cPlayer.m_aSideDeck[nCard].m_bFlipped = 1;
        bFlipped = 1;
        nNew = nTotal + cPlayer.m_aSideDeck[nCard].GetCardValue();
    }
    return nNew;
}
// PC 0x00755150
void CSWPazaak::LoadInitialSideDeck(int nSideDeck, CPazaakCard* pCards)
{
    C2DA c2DA("PazaakDecks", 0);
    int nCard = 0;

    if (c2DA.Load2DArray() == 0)
    {
        return;
    }

    CExoString sEntry;

    if (nSideDeck < 0)
    {
        int nNumRows = c2DA.m_nNumRows;
        nSideDeck = rand() % nNumRows;
    }
    do
    {
        c2DA.GetCExoStringEntry(nSideDeck, nCard, &sEntry);
        int nType = sEntry[1] - '1';
        switch (sEntry[0])
        {
        case '-':
            nType += CPazaakCard::CARD_TYPE_NEGATIVE_BASE;
            break;
        case '*':
            nType += CPazaakCard::CARD_TYPE_FLIPPABLE_BASE;
            break;
        case '$':
            nType = CPazaakCard::CARD_TYPE_DOUBLE;
            break;
        case 'F':
        case 'f':
            nType = (sEntry[1] == '1') ? CPazaakCard::CARD_TYPE_FLIP_2_4 : CPazaakCard::CARD_TYPE_FLIP_3_6;
            break;
        case 'T':
        case 't':
            nType = CPazaakCard::CARD_TYPE_TIEBREAKER;
            break;
        case 'V':
        case 'v':
            nType = CPazaakCard::CARD_TYPE_PLUS_MINUS_1_2;
            break;
        }
        CPazaakCard cCard;
        cCard.m_nType = nType;
        cCard.m_bFlipped = 0;
        *pCards = cCard;
        nCard++;
        pCards++;
    }
    while (nCard < 10);
    c2DA.Unload2DArray();
}
// PC 0x007553D0
CPazaakPlayer::CPazaakPlayer()
{
    m_bStanding = 0;
    m_nWins = 0;
    for (int i = 0; i < 4; i++)
    {
        CPazaakCard cEmpty;
        cEmpty.m_nType = -1;
        cEmpty.m_bFlipped = 0;
        cEmpty.m_nValue = 0;
        m_aSideDeck[i] = cEmpty;
    }
    for (int j = 0; j < 9; j++)
    {
        CPazaakCard cEmpty;
        cEmpty.m_nType = -1;
        cEmpty.m_bFlipped = 0;
        cEmpty.m_nValue = 0;
        m_aBoard[j] = cEmpty;
    }
}
