#ifndef __NWSPLAYER_H__
#define __NWSPLAYER_H__

class CResRef;
class CSWSCreature;
class CSWSObject;
class CSWSPlayerLastUpdateObject;
class CSWSPlayerLUOInventory;
class CSWCreatureStatsUpdate;
class CSWSPlayerCharSheetGUI;
class CLastUpdateObject;
class CLastUpdatePartyObject;
#include "exobase.h"
#include "exoresman.h"
#include "gameobjectarray.h"
#include "nwsobject.h"
#include "nwsclient.h"
#include "nwupdatestats.h"
#include "nwsplayerluo.h"

class CSWSPlayerInventoryGUI
{
public:
    CSWSPlayerInventoryGUI();
    ~CSWSPlayerInventoryGUI()
    {
        delete m_pLastUpdateObject;
    }
    void SetOpen(int bOpen);

    CSWSPlayerLUOInventory* m_pLastUpdateObject;
    int m_bOpen;
};

class CSWSPlayerJournalQuestUpdates
{
public:
    enum
    {
        QUEST_NAME = 0x0001,
        QUEST_TEXT = 0x0002,
        QUEST_DATE = 0x0004,
        QUEST_TIME = 0x0008,
        QUEST_STATE = 0x0010,
        QUEST_PRIORITY = 0x0020,
        QUEST_PICTURE = 0x0040,
        QUEST_COMPLETED = 0x0080,
        QUEST_QUIET = 0x0100,
        QUEST_PLOTINDEX = 0x0200,
        QUEST_PLANETID = 0x0400,
        QUEST_XP = 0x0800,
        QUEST_REMOVED = 0x4000,
        QUEST_NEW = 0x8000,
        QUEST_CLEAR_REMOVED = 0x4000
    };

    unsigned short m_nQuest;
    CExoString m_sTag;
};

class CSWSPlayerJournalGUI
{
public:
    CSWSPlayerJournalGUI()
    {
        m_bUpdateRequired = 1;
        m_bFullUpdate = 0;
    }

    int m_bFullUpdate;

    int GetFullUpdate() { return m_bFullUpdate; }
    CExoArrayList<CSWSPlayerJournalQuestUpdates> m_lstQuestUpdates;
    int m_bUpdateRequired;
};

class CSWSPlayerContainerGUI
{
public:
    CSWSPlayerContainerGUI()
    {
        m_n04 = 0;
        m_oidContainer = CGameObjectArray::INVALIDOBJID;
    }

    unsigned long m_oidContainer;
    int m_n04;
};

#define CHARACTER_TYPE_LOCAL 1
#define CHARACTER_TYPE_SERVER 3
#define CHARACTER_TYPE_SERVER_PLAYER 4

class CSWSPlayer : public CSWSClient
{
public:

    enum AreaTransition
    {
        AREA_TRANSITION_DEFAULT = -1,
        AREA_TRANSITION_RANDOM = 0,
        AREA_TRANSITION_USER_DEFINED = 1
    };

    enum AreaLoadState
    {
        AREA_LOAD_PENDING = 1,
        AREA_LOAD_COMPLETE = 2
    };

    enum { GAME_OBJECT_UPDATE_INTERVAL = 200000 };

    enum { CHARACTER_MESSAGE_INITIAL_SIZE = 4096 };

    int PackCreatureIntoMessage();
    virtual ~CSWSPlayer();
    virtual CSWSPlayer* AsSWSPlayer()  // PC 0x00698BD0 GOG 0x00698980
    {
        return this;
    }

    CSWSPlayer(unsigned long nPlayerId);
    int GetIsAllowedToSave();
    void SetAreaTransitionBMP(int nState, const CExoString& sBMP);
    int GetAreaTransitionBMP() { return m_nAreaTransitionState; }
    int GetCharacterInfoFromIFO(unsigned char& nClassCount, int* pnClasses,
                                unsigned char* pnClassLevels,
                                unsigned long& nExperience);
    CSWSPlayerLastUpdateObject* CreateNewPlayerLastUpdateObject();
    CSWSObject* GetGameObject();
    unsigned long GetPCObject();
    CSWSPlayerInventoryGUI* GetInventoryGUIInfo() { return m_pInventoryGUI; }
    CSWSPlayerContainerGUI* GetContainerGUIInfo() { return m_pContainerGUI; }
    int SaveServerCharacter();
    int LoadCreatureData(CResRef cResRef, CSWSCreature* pCreature);
    int LoadLocalCharacter();
    int LoadCharacterFromIFO(unsigned long nPlayerIndex, int bFlag1,
                             int bFlag2);
    int LoadServerCharacter(CResRef cResRef);
    void ClearPlayerOnDestroyGame();
    CExoString GetPlayerName();
    int PermittedToDisplayCharacterSheet(unsigned long oidCreature);
    CLastUpdateObject* GetLastUpdateObject(unsigned long oidObject);
    void ClearPlayerLastUpdateObject();
    void SetGameObject(CSWSObject* pObject);

    CExoLinkedList<CLastUpdateObject>* m_pActiveObjectsLastUpdate;
    CExoLinkedList<CLastUpdatePartyObject>* m_pPartyObjectsLastUpdate;
    int m_nAreaTransitionState;
    CExoString m_sAreaTransitionBMP;

    int m_bFloatyEffects;
    char m_nAreaLoadState;
    char m_a25[3];
    unsigned long m_oidNWSObject;

#ifdef _XBOX

    int m_nXboxAlign2C;
#endif
    unsigned long m_anLastGameObjectUpdateTime[2];
    unsigned long m_oidLastControlledObject;
    unsigned long m_oidPCObject;
    int m_bIsAllowedToSave;
    int m_bModuleCharacterLoaded;
    int m_nModulePlayerIndex;
    int m_nPlayModuleCharacterListState;
    CSWSPlayerJournalGUI* m_pJournalGUI;
    CSWSPlayerCharSheetGUI* m_pCharSheetGUI;
    CSWSPlayerInventoryGUI* m_pInventoryGUI;
    CSWSPlayerContainerGUI* m_pContainerGUI;
    CSWSPlayerLastUpdateObject* m_pLastUpdateObject;
    int m_n60;
    char m_nCharacterType;
    CResRef m_cServerCharacter;
    char m_aUnmapped75[7];
    int m_n7C;
    int m_nPlayerIndex;

public:

    int GetFloatyEffects()
    {
        return m_bFloatyEffects;
    }
    void SetFloatyEffects(int nFloatyEffects)
    {
        m_bFloatyEffects = nFloatyEffects;
    }
    void SetCharacterType(char nCharacterType)
    {
        m_nCharacterType = nCharacterType;
    }
    char GetCharacterType()
    {
        return m_nCharacterType;
    }
    void SetPCObject(unsigned long nPCObject)
    {
        m_oidPCObject = nPCObject;
    }
};

class CSWSPlayerCharSheetGUI
{
public:

    enum UpdateType
    {
        UPDATE_NONE = -1, UPDATE_STATS = 0, UPDATE_SKILLS = 1, UPDATE_FEATS = 2
    };
    enum UpdateMask
    {
        UPDATE_ABILITIES = 0x01, UPDATE_FORTITUDE = 0x02,
        UPDATE_WILL = 0x04, UPDATE_REFLEX = 0x08,
        UPDATE_EXPERIENCE = 0x10, UPDATE_ATTACK_BONUS = 0x20,
        UPDATE_COMBAT_INFORMATION = 0x40, UPDATE_SKILL_RANKS = 0x80,
        UPDATE_FEAT_LISTS = 0x200, UPDATE_ARMOR_CLASS = 0x400
    };
    unsigned long m_oidCreature;
    char m_nUpdateType;
    char m_a05[3];
    CSWCreatureStatsUpdate* m_pStatsUpdate;
    int m_nUpdateRequired;
    CSWSPlayerCharSheetGUI()  // PC 0x0059D5F0 GOG 0x0059D660
    {
        m_oidCreature = OBJECT_INVALID;
        m_pStatsUpdate = new CSWCreatureStatsUpdate;
        m_nUpdateType = -1;
        m_nUpdateRequired = 1;
    }
    ~CSWSPlayerCharSheetGUI()
    {
        delete m_pStatsUpdate;
    }
    void SetCreatureDisplayed(unsigned long oidCreature);
    int ComputeCharacterSheetUpdateRequired(CSWSPlayer*);
};

#endif
