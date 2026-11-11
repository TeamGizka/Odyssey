#ifndef __FACTIONMANAGER_H__
#define __FACTIONMANAGER_H__

class CResGFF;
class CResList;
#include "exobase.h"
#include "resgff.h"

class CSWSFaction;

#define INVALID_STANDARD_FACTION -1
#define STANDARD_FACTION_HOSTILE_1 1
#define STANDARD_FACTION_NEUTRAL 5
#define STANDARD_FACTION_INSANE 6

class CFactionManager
{
public:
    CFactionManager();
    ~CFactionManager();

    int GetIsNPCFaction(int nFaction);
    void ResolveFactionName(const CExoString& sName, CSWSFaction* pFaction);
    void DeleteFaction(int nFaction);
    CSWSFaction* GetFaction(int nFaction);
    void SetNPCFactionReputation(int nFaction1, int nFaction2, int nReputation);
    int GetNPCFactionReputation(int nFaction1, int nFaction2);
    int LoadFactions(int bAppend);
    int LoadFactionsFromSaveGame(CResGFF* pRes, CResList* pList);
    int SaveFactions(CResGFF* pRes, CResList* pList);
    int SaveReputations(CResGFF* pRes, CResList* pList);
    int LoadReputations();
    int LoadReputationsFromSaveGame(CResGFF* pRes, CResList* pList);
    void CreateDefaultFactions();

    CSWSFaction* m_pNeutral;
    CSWSFaction* m_pInsane;
    CSWSFaction* m_pFriendly2;
    CSWSFaction* m_pFriendly1;
    CSWSFaction* m_pHostile1;
    CSWSFaction* m_pHostile2;
    CSWSFaction* m_pPlayer;
    int m_nNumFactions;
    CExoArrayList<int>* m_pReputations;
    CExoArrayList<CSWSFaction*>* m_pFactions;
};

#endif
