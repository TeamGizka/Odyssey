#ifndef __NWSPARTY_H__
#define __NWSPARTY_H__

#include "exobase.h"

class CSWSCreature;
class CFactionManager;

class CSWSFaction
{
public:
    CSWSFaction();
    CSWSFaction(unsigned long oidCreature);
    ~CSWSFaction();

    void Initialise();
    CFactionManager* GetFactionManager();
    int GetMemberList(unsigned long** ppMembers);
    int GetMemberCount() { return m_lstMembers.num; }
    int GetCreatureInParty(unsigned long oidCreature);
    int InviteMember(unsigned long oidCreature, unsigned long oidInvitedBy);
    void SendChatMessage(unsigned long nVolume, const CExoString& sMessage);
    int SendFactionUpdateList();
    void SendFactionUpdateAdd(unsigned long oidCreature);
    void SendFactionUpdateRemove(unsigned long oidCreature);
    unsigned long GetWeakestMember(unsigned long oidArea, int bLineOfSight);
    unsigned long GetStrongestMember(unsigned long oidArea, int bLineOfSight);
    unsigned long GetMostDamagedMember(unsigned long oidArea, int bLineOfSight);
    unsigned long GetLeastDamagedMember(unsigned long oidArea,
                                        int bLineOfSight);
    int GetGold();
    int GetAverageReputation(unsigned long oidCreature);
    int GetAverageGoodEvilAlignment();
    int GetAverageLevel();
    int GetAverageXP();
    int GetMostFrequentClass();
    unsigned long GetWorstAC(unsigned long oidArea, int bLineOfSight);
    unsigned long GetBestAC(unsigned long oidArea, int bLineOfSight);
    unsigned long GetLeader();
    int GetSingletonParty();
    unsigned long GetFactionMember(int nMember, int nFaction);
    void RemoveMember(unsigned long oidCreature);
    int TransferLeadership(unsigned long oidCreature, unsigned long oidLeader);
    void AddMember(unsigned long oidCreature, int bSilent);
    int AttemptRemove(unsigned long oidCreature, unsigned long oidLeader);

    CExoArrayList<unsigned long> m_lstMembers;
    int m_nIndex;
    int m_nParentID;
    int m_nLastFactionMember;
    CExoString m_sName;
    int m_bGlobal;
};

#endif
