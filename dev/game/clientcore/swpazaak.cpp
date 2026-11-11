#include <stdlib.h>

#include "swpazaak.h"
#include "twodimarray.h"
#include "exoresman.h"
#include "exobase.h"
#include "gameobjectarray.h"

int CPazaakCard::s_pnPazaakCardValues[28] =
{
    1, 2, 3, 4, 5, 6,
    -1, -2, -3, -4, -5, -6,
    1, 2, 3, 4, 5, 6,
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
        if (cOpponent.m_bStanding != 0 && nSelfTotal > nOpponentTotal &&
            nSelfTotal <= 20)
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
            cSelf.AddGameCard(cSelf.m_aSideDeck[nCard]);
            CPazaakCard cEmpty;
            cEmpty.m_nType = -1;
            cEmpty.m_bFlipped = 0;
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
                cSelf.AddGameCard(cSelf.m_aSideDeck[nCard]);
                CPazaakCard cEmpty;
                cEmpty.m_nType = -1;
                cEmpty.m_bFlipped = 0;
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
    CPazaakCard* pSlot = m_aSideDeck;
    CPazaakCard* pLast = &pCards[9];

    for (int i = 4; i != 0; i--)
    {
        int nChoice = rand() % nCount;
        *pSlot = pCards[nChoice];
        pCards[nChoice] = *pLast;
        CPazaakCard cEmpty;
        cEmpty.m_nType = -1;
        *pLast = cEmpty;
        pLast--;
        pSlot++;
        nCount--;
    }
}
// PC 0x007549C0
int CPazaakPlayer::AddGameCard(CPazaakCard cCard)
{
    int nSlot;

    for (nSlot = 0; nSlot < 9; nSlot++)
    {
        if (m_aBoard[nSlot].m_nType == -1)
        {
            m_aBoard[nSlot] = cCard;
            break;
        }
    }
    if (nSlot >= 8 || (nSlot = GetTotal()) == 20)
    {
        m_bStanding = 1;
    }
    return nSlot;
}

void CSWPazaak::ClearGameBoard()
{
    for (int i = 0; i < 9; i++)
    {
        CPazaakCard cPlayerCard;
        cPlayerCard.m_nType = -1;
        cPlayerCard.m_bFlipped = 0;
        m_cPlayer.m_aBoard[i] = cPlayerCard;
        CPazaakCard cOpponentCard;
        cOpponentCard.m_nType = -1;
        cOpponentCard.m_bFlipped = 0;
        m_cOpponent.m_aBoard[i] = cOpponentCard;
    }
    m_cPlayer.m_bStanding = 0;
    m_cOpponent.m_bStanding = 0;
}

void CSWPazaak::ShuffleDeck()
{
    CPazaakCard aCards[40];
    int i;

    for (i = 0; i < 40; i++)
    {
        CPazaakCard cCard;
        cCard.m_nType = (i % 10) + 18;
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
        cEmpty.m_bFlipped = 0;
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
    m_cPlayer.AddGameCard(m_cPlayer.m_aSideDeck[nIndex]);
    CPazaakCard cEmpty;
    cEmpty.m_nType = -1;
    cEmpty.m_bFlipped = 0;
    m_cPlayer.m_aSideDeck[nIndex] = cEmpty;
    return 1;
}

int CSWPazaak::DetermineWinner()
{
    int nPlayerTotal = m_cPlayer.GetTotal();
    int nOpponentTotal = m_cOpponent.GetTotal();
    int nWinner = 1;

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
        if (nPlayerTotal <= nOpponentTotal)
        {
            return (nPlayerTotal >= nOpponentTotal) + 2;
        }
        return nWinner;
    }
    return 0;
}

int CSWPazaak::FindPotentialSidecardTotal(CPazaakPlayer& cPlayer, int* pnCard,
                                          int bLowest)
{
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
        int nType = cPlayer.m_aSideDeck[nCard].m_nType;
        for (int nTry = 2; nTry != 0; nTry--)
        {
            int bFlipped = 0;
            if (nType >= 12 && nType < 18)
            {
                bFlipped = (nTry == 2);
                cPlayer.m_aSideDeck[nCard].m_bFlipped = bFlipped;
            }
            else
            {
                nTry = 1;
            }
            int nNew = nTotal + cPlayer.m_aSideDeck[nCard].GetCardValue();
            if (nNew > 20)
            {
                nNew = -1000000;
            }
            if ((bLowest == 0 && nNew > nBest) ||
                (bLowest != 0 && nNew < nBest && nNew > -1000000) ||
                (nNew == nBest && bBestFlippable != 0))
            {
                nBest = nNew;
                nBestCard = nCard;
                nBestFlipped = bFlipped;
                bBestFlippable = (nType >= 12 && nType < 18);
            }
        }
    }
    if (nBestCard >= 0)
    {
        *pnCard = nBestCard;
        cPlayer.m_aSideDeck[nBestCard].m_bFlipped = nBestFlipped;
    }
    return nBest;
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
        case '*':
            nType += CPazaakCard::CARD_TYPE_FLIPPABLE_BASE;
            break;
        case '-':
            nType += CPazaakCard::CARD_TYPE_NEGATIVE_BASE;
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
        m_aSideDeck[i] = cEmpty;
    }
    for (int j = 0; j < 9; j++)
    {
        CPazaakCard cEmpty;
        cEmpty.m_nType = -1;
        cEmpty.m_bFlipped = 0;
        m_aBoard[j] = cEmpty;
    }
}
