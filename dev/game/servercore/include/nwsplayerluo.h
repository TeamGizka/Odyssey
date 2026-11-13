#ifndef __NWSPLAYERLUO_H__
#define __NWSPLAYERLUO_H__

class CExoLinkedListInternal;
class CExoLinkedListNode;
class CSWVisibilityNode;
class CLoopingVisualEffect;
#include "exobase.h"
#include "exoresman.h"
#include "nwscreature.h"
#include "nwscreaturestats.h"

class CSWSPlayerLUOSortedObjectList
{
public:
    CSWSPlayerLUOSortedObjectList()  // PC 0x0058B7C0 GOG 0x0058B840
        : m_oidObject(OBJECT_INVALID), m_fDistance(0.0f), m_nCategory(0)
    {
    }

    enum { ALLOCATION_INCREMENT = 32 };
    unsigned long m_oidObject;
    float m_fDistance;
    unsigned long m_nCategory;
};

class CSWSPlayerLUOInventoryItem
{
public:
    unsigned long m_oidItem;

    unsigned long m_nStackSize;
    unsigned long m_nCost;
};

class CSWSPlayerLUOInventory
{
public:
    CSWSPlayerLUOInventory();
    ~CSWSPlayerLUOInventory();

    void ClearSlots();
    void ClearRepository();
    void ClearContainer();
    void ClearStore();
    CSWSPlayerLUOInventoryItem* ItemListGetUpdateItem(CExoLinkedListNode* pNode,
                                                      unsigned char nList);
    void ItemListSetEnd(CExoLinkedListNode*& pNode, unsigned char nList);
    void ItemListGetPrev(CExoLinkedListNode*& pNode, unsigned char nList);
    void ItemListRemove(CExoLinkedListNode* pNode, unsigned char nList);
    void ItemListAddHead(unsigned long oidItem, unsigned char nList,
                         unsigned long nStackSize, unsigned long nCost);

    unsigned long m_oidSlot[20];
    int m_nSlotIndex[20];
    CExoLinkedList<CSWSPlayerLUOInventoryItem> m_lstRepository;
    unsigned long m_oidRepository;
    CExoLinkedList<CSWSPlayerLUOInventoryItem> m_lstContainer;
    CExoLinkedList<CSWSPlayerLUOInventoryItem> m_lstOverflow;
};

class CSWSPlayerLastUpdateObject
{
public:
    CSWSPlayerLastUpdateObject();
    ~CSWSPlayerLastUpdateObject();

    void ClearKnownSpellUsesLeft();

    void ClearActionQueue();
    unsigned char GetKnownSpellUsesLeft(unsigned char nClass,
                                        unsigned char nLevel);
    void SetKnownSpellUsesLeft(unsigned char nClass, unsigned char nLevel,
                               unsigned char nUsesLeft);
    unsigned long GetKnownSpell(unsigned char nClass, unsigned char nLevel,
                                unsigned char nIndex);
    void ClearVisibilityList();
    void ClearKnownSpells();
    void ClearFeats();
    void AddKnownSpell(unsigned char nClass, unsigned char nLevel,
                       unsigned long nSpell);
    void ClearSpellAddDeleteLists();

    int m_nComponent;
    int m_nChemical;
    unsigned long m_nGold;
    short m_nArmorClass;
    short m_nEncumbranceState;
    int m_bCanLevelUp;
    int m_nLevel;
    unsigned long m_nExperience;
    unsigned short m_nGuiSkills;
    char m_nUnknown16[2];

    CExoArrayList<unsigned short> m_lstFeats;
    CExoArrayList<unsigned short> m_lstBonusFeats;
    CExoArrayList<CFeatUseListEntry*> m_lstFeatUses;
    unsigned short m_nPlayerState;
    char m_nUnknown3E[2];
    CSWActionNode m_aActions[10];
    CExoArrayList<CSWVisibilityNode*> m_lstVisibility;
    CExoArrayList<unsigned long> m_aKnownSpells[2];
    CExoArrayList<unsigned long> m_aSpellsRemoved[2];
    CExoArrayList<unsigned long> m_aSpellsAdded[2];
    unsigned char m_nKnownSpellUsesLeft[2];
    char m_nUnknown1AE[2];

    CExoArrayList<CSWSStats_SpellLikeAbility>* m_pExtraSpells;
};

class CLastUpdateObjectRecord
{
public:

    enum { NUM_RECORDS = 36, UNSET_VALUE = 255 };

    unsigned long m_oidObject;
    unsigned char m_nUnknown4;
    int m_nUnknown8;
    unsigned short m_nUnknownC;
};

class CLastUpdateObject
{
public:

    static const int UNSET_SHORT_CACHE = -1;

    CLastUpdateObject();
    ~CLastUpdateObject();

    unsigned long m_oidAppearanceLeftHand;
    unsigned long m_oidAppearanceRightHand;
    unsigned long m_oidAppearanceArmour;
    unsigned long m_oidAppearanceHead;
    unsigned short m_nAppearance;
    unsigned char m_nAppearancePart0;
    unsigned char m_nAppearancePart1;
    unsigned char m_nAppearancePart2;
    unsigned char m_nAppearancePart3;
    unsigned char m_nAppearancePart4;
    unsigned char m_nAppearancePart5;
    unsigned char m_nAppearancePart6;
    unsigned char m_nAppearancePart7;
    unsigned char m_nAppearancePart8;
    unsigned long m_nAppearanceState;
    unsigned char m_nAppearanceVariant;
    char m_nUnknown21[3];
    int m_nUnknown24;

    unsigned char m_nObjectType;
    char m_nUnknown29;
    unsigned short m_nUnknown2A;
    unsigned long m_oidObject;
    Vector m_vUnknown30;
    int m_nUnknown3C;

    Vector m_vOrientation;
    int m_nAnimation;
    float m_fUnknown50;
    Vector m_vUnknown54;

    unsigned char m_nClientSidePathType;

    char m_nUnknown61;
    unsigned short m_nAIStateFlags;
    unsigned char m_nAIState;
    char m_nUnknown65;
    unsigned short m_nActivity;
    int m_bCombatState;
    unsigned char m_nStealthMode;
    char m_nUnknown6D[3];
    unsigned long m_oidAITarget;
    unsigned long m_oidLockOrientationToObject;
    unsigned long m_oidAttackTarget;
    CResRef m_resRef;

    unsigned short m_nPortraitId;
    unsigned short GetPortraitId() { return m_nPortraitId; }
    char m_nUnknown8E[2];
    CExoLocString m_lsName;
    CExoLocString m_lsDescription;
    float m_fWalkRate;
    float m_fRunRate;
    CExoArrayList<CLoopingVisualEffect*> m_lstLoopingVisualEffects;
    unsigned char m_nAssociateState;
    unsigned char m_nDamageLevel;
    char m_nUnknownB6[4];

    short m_nCurrentHitPoints;
    short m_nBaseMaxHitPoints;
    short m_nTemporaryHitPoints;
    short m_nMaxHitPoints;
    char m_nUnknownC2[2];
    int m_bAIStateFlag0040;
    int m_bPartyLeader;
    int m_bSingletonParty;
    int m_bInvitedToParty;
    CExoArrayList<unsigned long> m_lstTrapDetectedBy;
    int m_bTrapFlag;
    int m_bTrapFlagged;
    int GetTrapFlagged() { return m_bTrapFlagged; }
    int m_nFaction;
    int m_bLocked;
    int GetLocked() { return m_bLocked; }
    int m_bPlotObject;
    unsigned char m_nAIStateReputation;
    char m_nUnknownF5[3];
    int m_bSeen;
    int m_bHeard;
    int m_bInvisible;
    unsigned short m_nRace;
    char m_nUnknown106[2];
    CExoString m_sTag;
    unsigned char m_nSubraceIndex;
    unsigned char m_nGender;
    unsigned char m_nClass[2];
    unsigned char m_nClassLevel[2];
    char m_nUnknown116[2];
    short m_nGoodEvilAlignment;
    short m_n11A;
    int m_n11C;
    CExoString m_s120;
    int m_nObjectF4;
    int m_nAreaWaitState;

    unsigned char m_nStrengthBase;
    unsigned char m_nDexterityBase;
    unsigned char m_nConstitutionBase;
    unsigned char m_nIntelligenceBase;
    unsigned char m_nWisdomBase;
    unsigned char m_nCharismaBase;
    unsigned char m_nStrength;
    unsigned char m_nDexterity;
    unsigned char m_nConstitution;
    unsigned char m_nIntelligence;
    unsigned char m_nWisdom;
    unsigned char m_nCharisma;
    short m_nCurrentForcePoints;
    short m_nForcePoints;
    short m_nTemporaryForcePoints;
    short m_nMaxForcePoints;
    unsigned char m_nSkillBase[8];
    unsigned char m_nSkill[8];
    int m_bDeleteNoFade;
    int m_bKeepCorpse;
    unsigned long m_nFadeOutTime;
    CLastUpdateObjectRecord m_aRecords[CLastUpdateObjectRecord::NUM_RECORDS];
};

class CLastUpdatePartyObject
{
public:
    CResRef m_cPortrait;
    unsigned short m_nPortraitId;
    char m_nUnknown12[2];
    int m_bUpdated;
    unsigned long m_oidObject;
    unsigned short m_nAIStateFlags;
    unsigned char m_nAIState;
    char m_nUnknown1F;
    unsigned short m_nActivity;
    char m_nUnknown22[2];
    unsigned long m_oidAITarget;
    unsigned short m_nCurrentHitPoints;
    unsigned short m_nBaseMaxHitPoints;
    unsigned short m_nTemporaryHitPoints;
    unsigned short m_nMaxHitPoints;
    unsigned char m_nAIStateReputation;
    char m_nUnknown31[3];
    int m_bCombatState;
    unsigned char m_nStealthMode;
    char m_nUnknown39[3];
    unsigned long m_oidArea;
    Vector m_vPosition;
    int m_nAreaWaitState;
    int m_bAIStateFlag0040;
    int m_bPartyLeader;
    int m_bSingletonParty;
    int m_bInvitedToParty;
    int m_nPoisonState;
    int m_bDiseased;
    unsigned short m_nCurrentForcePoints;
    unsigned short m_nForcePoints;
    unsigned short m_nTemporaryForcePoints;
    unsigned short m_nMaxForcePoints;
};

#endif
