#ifndef __PARTYTABLES_H__
#define __PARTYTABLES_H__

#include "exobase.h"
#include "math3d.h"

class CItemRepository;
class CSWSJournal;
class CSWSCreature;
class CResGFF;
class CResStruct;

class CSWPartyTable
{
public:

    static const int MAX_NPCS = 12;

    static const int MAX_PUPPETS = 3;

    enum { GALAXY_MAP_NUM_POINTS = 16, GALAXY_MAP_SELECTABLE_SHIFT = 16 };

    static const int PARTY_XP_ROW = 13;

    CSWPartyTable();
    ~CSWPartyTable();

    void ClearTable();
    CItemRepository* GetPartyItems();
    CSWSJournal* GetPartyJournal();
    void ClearInventory();
    int GetIsNPCAvailable(int nNPC);
    CExoString GetPupFilename(int nPUP);
    int GetIsPUPAvailable(int nPUP);
    void SetDisableMap(int bDisable) { m_bDisableMap = bDisable; }
    void SetDisableRegen(int bDisable) { m_bDisableRegen = bDisable; }
    int GetInfluence(int nNPC);
    int SetInfluence(int nNPC, int nInfluence);
    int SetPUPObject(int nPUP, unsigned long oidPUP);
    int GetIsPuppet(int nPUP);
    int KillPUPObject(int nPUP, int bFade);
    int GetPUPID(unsigned long oidObject);
    void ClearPuppets();
    int ClearPuppet(int nPUP);
    CExoString GetPCName() { return m_sPCName; }
    CExoString GetPCFirstName();
    CExoString GetPCLastName();
    unsigned long GetNextPC();
    void SaveMemberByObject(int nNPC, unsigned long oidMember);
    void SavePuppet(int nPUP, int bClearActions);
    void SavePuppetByObject(int nPUP, unsigned long oidPuppet);
    int AddPuppet(int nPUP, unsigned long oidPuppet);
    int AddPUP(int nPUP, CSWSCreature* pCreature);
    int AddPUP(int nPUP, const CExoString& sTemplate);
    unsigned long GetPUPObject(int nPUP, int bLoad, int bResurrect);
    int SpawnPUP(int nPUP, int bUsePosition, const Vector& vPosition,
                 const Vector& vOrientation, int bResurrect);
    CSWSCreature* GetPUPOwner(int nPUP);
    int RemovePuppet(int nPUP);
    int AssignPUP(int nPUP, int nNPC);
    int GetActiveNPC();
    unsigned char GetNumNPCAvailable();
    int SetNPCObject(int nNPC, unsigned long oidNPC);
    int GetIsMember(int nNPC);
    int GetIsMember(unsigned long oidObject);
    int GetIsAvailable(unsigned long oidObject);
    unsigned char GetNPCSelectability(int nNPC);
    int SetNPCSelectability(int nNPC, unsigned char nSelectability);
    int GetNPCID(unsigned long oidObject);
    int AddGameInProgress();
    void RemoveGameInProgress();
    int ClearMembers();
    int ClearMember(int nNPC);
    int GetIsLeader(unsigned long oidObject);
    void SetLeader(unsigned long oidObject);
    void SetPartyFollowState(int nFollowState);
    int SetReturnStrref(int nReturnFlag, unsigned long nStrref,
                        unsigned long nTitleStrref);
    int SetPlanetSelectable(unsigned long nPlanet, int bSelectable);
    int GetPlanetSelectable(unsigned long nPlanet);
    int SetPlanetAvailable(unsigned long nPlanet, int bAvailable);
    int GetPlanetAvailable(unsigned long nPlanet);
    int SetSelectedPlanet(int nPlanet);
    void AutoLevelUpNPCs();
    int KillNPCObject(int nNPC, int bFade);
    int RemoveMember(int nNPC);
    int AddMember(int nNPC, unsigned long oidMember);
    void TransferInventory(CSWSCreature* pCreature);
    int AddNPC(int nNPC, CSWSCreature* pCreature);
    int AddNPC(int nNPC, const CExoString& sTemplate);
    unsigned long GetNPCObject(int nNPC, int bLoad, int bResurrect);
    void DistributeExperience(int nExperience, int bFeedback);
    int SpawnNPC(int nNPC, int bUseSpawnPoint, const Vector& vPosition,
                 const Vector& vOrientation, int bFlag);
    void CreateParty();
    void PositionParty();
    int LoadTableInfo();
    void GivePlotXP(const CExoString& sPlotID, int nPercent);
    void GivePlotXP(unsigned long nPlotIndex, int nPercent);
    int SwitchPlayerCharacter(int nNPC);
    int GetTotalPlayedSeconds();
    void UnstealthParty(int nFlag);
    static int ResetCurrentSessionStartTime();
    int RemoveNPC(int nNPC);
    void SetSoloMode(int bSoloMode, int bUnstealth);
    void UpdateMembers(int bClearActions);
    void SaveMember(int nNPC, int bClearActions);
    void UpdateInventory(int bClearItems);
    void SaveTableInfo(const CExoString& sAlias);
    void Save(const CExoString& sAlias, int bSaveMembers);
    CExoString GetFilename(int nNPC);
    void SaveJournal(CResGFF* pRes, CResStruct* pStruct);
    void LoadJournal(CResGFF* pRes, CResStruct* pStruct);

    int m_nNumMembers;
    int m_nNumPuppets;
    int m_anMembers[2];
    int m_anPuppets[MAX_PUPPETS];
    unsigned long m_aoidNPC[MAX_NPCS];
    int m_abNPCAvailable[MAX_NPCS];
    unsigned char m_anNPCSelectability[MAX_NPCS];
    int m_abPlanetAvailable[GALAXY_MAP_NUM_POINTS];
    int m_abPlanetSelectable[GALAXY_MAP_NUM_POINTS];
    int m_nSelectedPlanet;
    int m_nAIState;
    int m_nFollowState;
    int m_nLeader;
    int m_nControlledNPC;
    int m_anNPCInfluence[MAX_NPCS];
    unsigned long m_aoidPuppet[MAX_PUPPETS];
    int m_abPuppetAvailable[MAX_PUPPETS];
    unsigned char m_anPuppetSelectability[MAX_PUPPETS];
    int m_nGameInProgress;
    int m_nXPPool;
    unsigned long m_nGold;
    int m_nItemComponent;
    int m_nItemChemical;
    int m_nSwoop1;
    int m_nSwoop2;
    int m_nSwoop3;
    int m_n188;
    int m_n18C;
    int m_n190;
    int m_nReturnFlag;
    unsigned long m_nReturnStrref;
    unsigned long m_nReturnTitleStrref;
    int m_bSkipPartyPositioning;
    int m_bInventoryDirty;
    unsigned long m_nPlayedSeconds;
    CItemRepository* m_pPartyItems;
    CSWSJournal* m_pPartyJournal;

    enum { PAZAAK_CARD_COUNT = 23 };
    int m_anPazaakCards[PAZAAK_CARD_COUNT];

    enum { PAZAAK_SIDE_DECK_SIZE = 10 };
    int m_anPazaakSideDeck[PAZAAK_SIDE_DECK_SIZE];
    int m_bSoloMode;
#ifndef _XBOX
    int m_bCheatUsed;
#endif
    CExoString m_sPCName;
    int m_n248;
    int m_nForfeitViolation;
    int m_nForfeitConditions;
    int m_n254;
    int m_bDisableMap;
    int m_bDisableRegen;
};

#endif
