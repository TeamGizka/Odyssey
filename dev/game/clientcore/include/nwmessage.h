#ifndef __NWMESSAGE_H__
#define __NWMESSAGE_H__

class CClientExoAppInternal;
class CResRef;
class Vector;
#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"

#define SAFE_PROJECTILE_MIN_DIRECTION_SQUARED 0.00001f

class CResGFF;
class CResStruct;

class CSWMessage
{
public:
    static const int DEFAULT_WRITE_BUFFER_SIZE;

    enum CCMessageSelector
    {

        CC_MESSAGE_OBJECT_TEXT = 1,

        CC_MESSAGE_DAMAGE_WITHOUT_ATTACKER = 2,
        CC_MESSAGE_DAMAGE = 3,

        CC_MESSAGE_DEATH = 4,

        CC_MESSAGE_MELEE_ATTACK = 5,
        CC_MESSAGE_RANGED_ATTACK = 6,

        CC_MESSAGE_SAVING_THROW = 7,

        CC_MESSAGE_SPELL_DATA = 8,
        CC_MESSAGE_SKILL = 9,

        CC_MESSAGE_FORCE_RESISTANCE = 10,
        CC_MESSAGE_FEEDBACK = 11,

        CC_MESSAGE_FORCE_POINTS_SPENT = 12,
        CC_MESSAGE_TOUCH_ATTACK = 13,
        CC_MESSAGE_INITIATIVE = 14,

        CC_MESSAGE_DISPEL_MAGIC = 15,

        CC_MESSAGE_STEALTH_DETECTION = 16,
        CC_MESSAGE_SPELL_RESISTANCE_CHECK = 17,

        CC_MESSAGE_ATTACK_SUMMARY = 18,

        CC_MESSAGE_MINE_DETECTION = 19,
        CC_MESSAGE_ATTACK_BREAKDOWN = 20,
        CC_MESSAGE_THREAT_BREAKDOWN = 21,
        CC_MESSAGE_DEFENSE_BREAKDOWN = 22,
        CC_MESSAGE_DAMAGE_BREAKDOWN = 23,
        CC_MESSAGE_EFFECT_APPLICATION_BREAKDOWN = 25,
        CC_MESSAGE_DEFLECT_BREAKDOWN = 26,

        CC_MESSAGE_POISONED = 27
    };

    enum LoadBarSelector
    {
        LOAD_BAR_START_STALL_EVENT = 1,
        LOAD_BAR_UPDATE_STALL_EVENT = 2,
        LOAD_BAR_END_STALL_EVENT = 3
    };
    enum { STALL_EVENT_PROGRESS_UNCHANGED = 0xFFFFFFFF };
    enum GuiInventoryRecord
    {
        GUI_INVENTORY_RECORD_ADD = 'A',
        GUI_INVENTORY_RECORD_DELETE = 'D'
    };
    enum ChatAdditionalSelector { CHAT_SOUND = 7, CHAT_SERVER_STRREF = 15 };

    enum AreaMessageSelector
    {
        AREA_MESSAGE_TRANSITION = 1,
        AREA_MESSAGE_VISUAL_EFFECT = 2,
        AREA_MESSAGE_WEATHER = 4
    };
    enum AreaLoadscreenSelector
    {
        AREA_LOADSCREEN_RANDOM = 0,
        AREA_LOADSCREEN_CUSTOM = 1,
        AREA_LOADSCREEN_FIRST_TABLE_ROW = 2
    };

    enum VisualEffectDummyDelay
    {
        VISUAL_EFFECT_BEAM_DUMMY_DELAY = 6000,
        VISUAL_EFFECT_DUMMY_DELAY = 10000
    };

    enum LoadBarStallEvent
    {

        LOADBAR_STALL_EVENT_LOAD_MODULE = 1,
        LOADBAR_STALL_EVENT_SAVE_GAME = 2,
        LOADBAR_STALL_EVENT_LOAD_GAME = 3
    };
    enum MultiLangChatType { CHAT_TYPE_MULTILANG = 8 };
    enum ChatTellSelector { CHAT_TELL = 4 };
    enum ChatShoutSelector { CHAT_SHOUT = 2 };
    enum ChatDispatchSelector
    {
        CHAT_TALK = 1,
        CHAT_WHISPER = 3,
        CHAT_SERVER_TELL = 5,
        CHAT_PARTY = 6,
        CHAT_DIALOG = 13,
        CHAT_DIALOG_SHOUT = 14
    };
    enum MultiLangChatSelector
    {
        CHAT_MULTILANG = 11,
        CHAT_MULTILANG_LISTED_RECIPIENT = 12
    };
    enum StrRefChatType
    {
        CHAT_STRREF_TALK = 8,
        CHAT_STRREF_SHOUT = 9,
        CHAT_STRREF_WHISPER = 10
    };

    enum GameObjectMessageSelector { MESSAGE_GAME_OBJECT_UPDATE = 5 };
    enum ChatMessageSelector { MESSAGE_CHAT = 9 };
    enum CCMessageMajorSelector { MESSAGE_CC = 18 };
    enum PlayModuleMessageSelector { MESSAGE_PLAY_MODULE_CHARACTER_LIST = 49 };
    enum ExamineMessageSelector { MESSAGE_EXAMINE_GUI = 27 };
    enum CustomTokenMessageSelector { MESSAGE_CUSTOM_TOKEN = 50 };
    enum MessageMajorSelector
    {
        MESSAGE_LOGIN = 2,
        MESSAGE_MODULE = 3,
        MESSAGE_AREA = 4,
        MESSAGE_INVENTORY = 12,
        MESSAGE_GUI_INVENTORY = 13,
        MESSAGE_PARTY = 14,
        MESSAGE_CHEAT = 15,
        MESSAGE_CAMERA = 16,
        MESSAGE_CHARACTER_LIST = 17,
        MESSAGE_COMBAT_ROUND = 19,
        MESSAGE_DIALOG = 20,
        MESSAGE_GUI_CHARACTER_SHEET = 21,
        MESSAGE_QUICK_CHAT = 22,
        MESSAGE_SOUND = 23,
        MESSAGE_ITEM_PROPERTY = 24,
        MESSAGE_GUI_CONTAINER = 25,
        MESSAGE_VOICE_CHAT = 26,
        MESSAGE_JOURNAL = 28,
        MESSAGE_LEVEL_UP = 29,
        MESSAGE_GUI_QUICKBAR = 30,
        MESSAGE_MAP_PIN = 32,
        MESSAGE_SAFE_PROJECTILE = 34,
        MESSAGE_AMBIENT = 40,
        MESSAGE_LOAD_BAR = 44,
        MESSAGE_SAVE_LOAD = 45,
        MESSAGE_SHUTDOWN_SERVER = 47,
        MESSAGE_GUI_TIMING_EVENT = 48
    };

    enum MessageHeader { MESSAGE_PLAYER = 'P', MESSAGE_HEADER_SIZE = 3 };

    enum CharListSelector { CHAR_LIST_REPLY = 2 };

    enum GuiUpdateCategory
    {
        GUI_UPDATE_INVENTORY = 'I',
        GUI_UPDATE_CONTAINER = 'C',
        GUI_UPDATE_REPOSITORY = 'R'
    };
    enum CreatureModeMask
    {
        CREATURE_MODE_DETECT = 0x01, CREATURE_MODE_STEALTH = 0x02,
        CREATURE_MODE_PARRY = 0x04, CREATURE_MODE_POWER_ATTACK = 0x08,
        CREATURE_MODE_IMPROVED_POWER_ATTACK = 0x10, CREATURE_MODE_FLURRY = 0x20,
        CREATURE_MODE_RAPID_SHOT = 0x40, CREATURE_MODE_COUNTERSPELL = 0x80
    };

    enum CharacterSheetUpdateMask
    {
        CHARACTER_SHEET_ABILITIES = 0x0001,
        CHARACTER_SHEET_FORTITUDE = 0x0002,
        CHARACTER_SHEET_WILL = 0x0004,
        CHARACTER_SHEET_REFLEX = 0x0008,
        CHARACTER_SHEET_EXPERIENCE = 0x0010,
        CHARACTER_SHEET_ATTACK = 0x0020,
        CHARACTER_SHEET_COMBAT = 0x0040,
        CHARACTER_SHEET_SKILLS = 0x0080,
        CHARACTER_SHEET_FEATS = 0x0200,
        CHARACTER_SHEET_DEFENSE = 0x0400
    };

    enum MessageRecipient { RECIPIENT_UNSPECIFIED = 0xFFFFFFFF };

    enum PlayerListMessageSelector { MESSAGE_PLAYER_LIST = 10 };
    enum PlayerListSelector { PLAYER_LIST_ALL = 1, PLAYER_LIST_ADD = 2, PLAYER_LIST_DELETE = 3 };

    enum InventorySelector
    {
        INVENTORY_EQUIP = 1,
        INVENTORY_EQUIP_CANCEL = 2,
        INVENTORY_DROP = 3,
        INVENTORY_DROP_CANCEL = 4,
        INVENTORY_PICKUP = 5,
        INVENTORY_PICKUP_CANCEL = 6,
        INVENTORY_UNEQUIP = 7,
        INVENTORY_UNEQUIP_CANCEL = 8,
        INVENTORY_LEARN_SCROLL = 14,
        INVENTORY_LEARN_SCROLL_SOUND = 15
    };

    enum GameObjectUpdateRecord
    {
        GAME_OBJECT_RECORD_ADD = 'A',
        GAME_OBJECT_RECORD_UPDATE = 'U',
        GAME_OBJECT_RECORD_DELETE = 'D',
        GAME_OBJECT_RECORD_APPEARANCE = 'P',
        GAME_OBJECT_RECORD_PLAYER_INFO = 'I',
        GAME_OBJECT_RECORD_GUI_ELEMENTS = 'G',
        GAME_OBJECT_RECORD_WORK_REMAINING = 'W'
    };

    enum PlayerGameObjectUpdate
    {
        PLAYER_OBJECT_ADD_ITEM = 'A', PLAYER_OBJECT_SUMMON_CREATURE = 'C',
        PLAYER_OBJECT_DELETE = 'D', PLAYER_OBJECT_APPEARANCE_PART1 = 'G',
        PLAYER_OBJECT_APPEARANCE_PART6 = 'H', PLAYER_OBJECT_APPEARANCE_PART = 'L',
        PLAYER_OBJECT_EFFECT_M = 'M', PLAYER_OBJECT_APPEARANCE = 'P',
        PLAYER_OBJECT_APPEARANCE_TYPE = 'R', PLAYER_OBJECT_APPEARANCE_PART0 = 'T'
    };
    enum PlayerGameObjectUpdateMinor { PLAYER_OBJECT_UPDATE = 1 };

    enum PlayerAppearancePart
    {
        PLAYER_APPEARANCE_PART2 = 0, PLAYER_APPEARANCE_PART3 = 1,
        PLAYER_APPEARANCE_PART4 = 6, PLAYER_APPEARANCE_PART5 = 7
    };

    enum DebugInfoMessageSelector { MESSAGE_DEBUG_INFO = 33 };

    enum DebugInfoSelector { DEBUG_INFO_CREATURE = 1, DEBUG_INFO_PLACEABLE = 3 };

    enum TriggerDeleteSelector
    {
        TRIGGER_DELETE_EXPLODE = 1,
        TRIGGER_DELETE_DISARM = 2,

        TRIGGER_DELETE_NORMAL = 3
    };

    enum CharacterDownloadMessageSelector { MESSAGE_CHARACTER_DOWNLOAD = 43 };
    enum CharacterDownloadSelector
    {
        CHARACTER_DOWNLOAD_REPLY = 2,
        CHARACTER_DOWNLOAD_FILE = 3
    };

    enum ServerStatusMessageSelector { MESSAGE_SERVER_STATUS = 1 };

    enum ServerStatusSelector
    {
        SERVER_STATUS_RUN_REQUEST = 1,
        SERVER_STATUS_RESET = 2,
        SERVER_STATUS_CHARACTER_LOGIN = 3
    };

    enum PopUpGUIMessageSelector { MESSAGE_POP_UP_GUI = 36 };
    enum PopUpGUISelector
    {
        POP_UP_GUI_PLAYER_DEATH = 1,
        POP_UP_GUI_CUSTOM = 3
    };

    CSWMessage();
    virtual ~CSWMessage();

    void ClearReadMessage();
    CResRef ReadCResRef(int nLength);
    void* ReadVOIDPtr(int nBytes);
    int MessageReadOverflow(int nBytes);
    int MessageReadUnderflow(int nBytes);
    int MessageMoreDataToRead();
    void ExtendWriteBuffer(unsigned long nExtraBytes);
    void WriteBYTE(unsigned char cValue, int nBits);
    void WriteCHAR(char cValue, int nBits);
    void WriteWORD(unsigned short nValue, int nBits);
    void WriteSHORT(short nValue, int nBits);
    void WriteINT(int nValue, int nBits);
    void WriteDWORD(unsigned long nValue, int nBits);
    void WriteCResRef(CResRef cResRef, int nLength);
    void WriteVOIDPtr(void* pValue, int nBytes);
    int GetWriteMessage(unsigned char** ppBuffer, unsigned long* pnSize);
    void SetReadMessage(unsigned char* pBuffer, unsigned long nSize,
                        unsigned long nRecipient, int nMsgType);
    int ReadBOOL();
    unsigned char ReadBYTE(int nBits);
    char ReadCHAR(int nBits);
    unsigned short ReadWORD(int nBits);
    short ReadSHORT(int nBits);
    unsigned long ReadDWORD(int nBits);
    int ReadINT(int nBits);
    float ReadFLOAT(float fDefault, int nBits);
    CExoString ReadCExoString(int nBits);
    void CreateWriteMessage(unsigned long nMsgLen, unsigned long nRecipient,
                            int nMsgType);
    void WriteBOOL(int bValue);
    void WriteFLOAT(float fValue, float fDefault, int nBits);
    void WriteCExoString(const CExoString& sValue, int nBits);
    Vector ReadVector();
    void WriteVector(const Vector& vValue);

    int PeekAtWriteMessageSize();

    unsigned char ReadBit(unsigned char nBit);
    unsigned __int64 ReadBits(int nBits);
    __int64 ReadSigned(int nBits);
    unsigned __int64 ReadUnsigned(int nBits);
    void WriteBit(unsigned char bValue);
    void WriteBits(unsigned __int64 nValue, int nBits);
    void WriteSigned(__int64 nValue, int nBits);
    void WriteUnsigned(unsigned __int64 nValue, int nBits);
    unsigned char* m_pWriteBuffer;
    unsigned long m_nWriteBufferSize;
    unsigned long m_nWriteIndex;
    unsigned char* m_pUnknown10;
    unsigned long m_nUnknown14;
    unsigned long m_nUnknown18;
    unsigned long m_nTotalBytesWritten;
    unsigned long m_nUnknown20;
    int m_bWritingMessage;
    unsigned char* m_pReadBuffer;
    unsigned long m_nReadBufferSize;
    unsigned long m_nReadIndex;
    unsigned long m_nUnknown34;
    unsigned long m_nUnknown38;
    unsigned long m_nUnknown3C;
    unsigned long m_nUnknown40;
    unsigned char m_bUnknown44;
    unsigned char m_bUnknown45;
    unsigned char m_aUnknown46[2];
    int m_nMsgType;
    unsigned long m_nRecipient;
};

class CSWCCMessageData
{
public:

    enum InvitationObjectID { INVITATION_INVITER = 0 };

    enum ResistanceFeedbackInteger
    {
        RESISTANCE_FEEDBACK_MESSAGE = 0,
        RESISTANCE_FEEDBACK_ABSORBED = 1,
        RESISTANCE_FEEDBACK_REMAINING = 2
    };
    enum ResistanceFeedbackObjectID { RESISTANCE_FEEDBACK_TARGET = 0 };

    enum SkillDataObjectID { SKILL_DATA_CREATURE = 0, SKILL_DATA_TARGET = 1 };
    enum SkillDataInteger
    {
        SKILL_DATA_NAME = 0, SKILL_DATA_ROLL = 1, SKILL_DATA_RANK = 2,
        SKILL_DATA_DC = 3, SKILL_DATA_TAKE_20 = 4, SKILL_DATA_RESULT = 5,
        SKILL_DATA_DESCRIPTION = 6
    };
    enum SkillDataResult
    {
        SKILL_RESULT_FAILED = 0, SKILL_RESULT_SUCCESS = 1,
        SKILL_RESULT_CRITICAL_FAILURE = 2, SKILL_RESULT_RETRY = 3,
        SKILL_RESULT_OWN_MINE = 4, SKILL_RESULT_IMPOSSIBLE = 5
    };
    enum
    {
        SKILL_DATA_DEMOLITIONS_STRREF = 324,
        SKILL_DATA_SECURITY_STRREF = 329,
        SKILL_DATA_TREAT_INJURY_STRREF = 330,
        SKILL_DATA_DISARM_STRREF = 1529, SKILL_DATA_SET_STRREF = 1530,
        SKILL_DATA_RECOVER_STRREF = 1531, SKILL_DATA_EXAMINE_STRREF = 1532
    };

    enum SpellDataObjectID { SPELL_DATA_CASTER = 0, SPELL_DATA_ITEM = 1,
                             SPELL_DATA_TARGET = 2 };

    enum TouchAttackObjectID { TOUCH_ATTACK_ATTACKER = 0, TOUCH_ATTACK_TARGET = 1 };
    enum TouchAttackInteger
    {
        TOUCH_ATTACK_BONUS = 0, TOUCH_ATTACK_ROLL = 1,
        TOUCH_ATTACK_RESULT = 2, TOUCH_ATTACK_RANGED = 3,
        TOUCH_ATTACK_CONCEALMENT = 4
    };
    enum TouchAttackResult
    {
        TOUCH_ATTACK_MISS = 0, TOUCH_ATTACK_HIT = 1,
        TOUCH_ATTACK_CRITICAL = 2, TOUCH_ATTACK_MESSAGE_MISS = 4
    };
    enum SpellDataInteger { SPELL_DATA_SPELL = 0, SPELL_DATA_CAST = 2,
                            SPELL_DATA_BASE_ITEM = 3 };

    enum SpellDataCast { SPELL_DATA_CAST_STARTED = 1 };

    enum FloatingTextInteger
    {
        FLOATING_TEXT_STR_REF = 0,
        FLOATING_TEXT_FEEDBACK = 9
    };
    enum FloatingTextString { FLOATING_TEXT_STRING = 0 };
    enum FloatingTextObjectID { FLOATING_TEXT_CREATURE = 0 };

    enum AttackSummaryInteger
    {
        SUMMARY_ATTACK_RESULT = 0,
        SUMMARY_ATTACK_TYPE = 1,
        SUMMARY_DEFENSE_TOTAL = 2,
        SUMMARY_ATTACK_FLAGS = 3,
        SUMMARY_BASE_DAMAGE = 4,
        SUMMARY_DAMAGE_DETAIL = 5,
        SUMMARY_SNEAK_ATTACK = 6,
        SUMMARY_COUP_DE_GRACE = 7,
        SUMMARY_HIT_DETAIL = 8,
        SUMMARY_HIT_MODIFIER = 9,
        SUMMARY_DEFENSE_ROLL = 10,
        SUMMARY_DEFENSE_MODIFIER = 11
    };

    enum AttackBreakdownInteger
    {
        ATTACK_TOTAL = 0,
        ATTACK_ROLL = 1,
        ATTACK_BONUS = 2,
        ATTACK_MODIFIER1 = 3,
        ATTACK_MODIFIER2 = 4,
        ATTACK_TYPE = 5,
        ATTACK_MODIFIER3 = 6,
        ATTACK_DETAIL = 7,
        ATTACK_MODIFIER4 = 8,
        ATTACK_MODIFIER5 = 9,
        ATTACK_DETAIL1 = 10,
        ATTACK_MODIFIER6 = 11,
        ATTACK_DETAIL2 = 12,
        ATTACK_MODIFIER7 = 13,
        ATTACK_MODIFIER8 = 14,
        ATTACK_MODIFIER9 = 15,
        ATTACK_MODIFIER10 = 16,
        ATTACK_DETAIL3 = 17,
        ATTACK_MODIFIER11 = 18,
        ATTACK_DEFENSE_ROLL = 19,
        ATTACK_DEFENSE_MODIFIER = 20
    };
    enum ThreatBreakdownInteger
    {
        THREAT_ATTACK_ROLL = 0,
        THREAT_ROLL = 1,
        THREAT_DETAIL = 2,
        THREAT_TOTAL = 3,
        THREAT_DEFENSE = 4,
        THREAT_DETAIL1 = 5,
        THREAT_MODIFIER = 6
    };
    enum DefenseBreakdownInteger
    {
        DEFENSE_MODIFIER0 = 0,
        DEFENSE_MODIFIER1 = 1,
        DEFENSE_MODIFIER2 = 2,
        DEFENSE_MODIFIER3 = 3,
        DEFENSE_MODIFIER4 = 4,
        DEFENSE_MODIFIER5 = 5,
        DEFENSE_MODIFIER6 = 6,
        DEFENSE_MODIFIER7 = 7,
        DEFENSE_MODIFIER8 = 8
    };
    enum EffectApplicationBreakdownInteger
    {
        EFFECT_SAVE_TYPE = 0,
        EFFECT_STATE = 1,
        EFFECT_VERSUS_TYPE = 2,
        EFFECT_SAVE_ROLL = 3,
        EFFECT_SAVE_EFFECT_BONUS = 4,
        EFFECT_SAVE_BONUS = 5,
        EFFECT_SAVE_TOTAL = 6,
        EFFECT_SAVE_DC = 7,
        EFFECT_SAVE_RESULT = 8
    };
    enum DeflectBreakdownInteger
    {
        DEFLECT_DETAIL0 = 0,
        DEFLECT_DETAIL1 = 1,
        DEFLECT_DETAIL2 = 2,
        DEFLECT_DETAIL3 = 3,
        DEFLECT_DETAIL4 = 4,
        DEFLECT_DETAIL5 = 5,
        DEFLECT_DETAIL6 = 6,
        DEFLECT_DETAIL7 = 7,
        DEFLECT_DETAIL8 = 8
    };

    enum BreakdownObjectID { BREAKDOWN_REACT_OBJECT = 0 };
    enum AttackSummaryObjectID
    {
        SUMMARY_ATTACKER = 0,
        SUMMARY_TARGET = 1
    };

    enum SavingThrowDataType { DATA_TYPE_SAVING_THROW = 1 };

    enum FeedbackDataType { DATA_TYPE_FEEDBACK = 3 };
    enum FeedbackInteger { FEEDBACK_MESSAGE = 0 };
    enum SavingThrowInteger
    {
        SAVING_THROW_SAVE_TYPE = 0,
        SAVING_THROW_VERSUS_TYPE = 1,
        SAVING_THROW_ROLL = 2,
        SAVING_THROW_EFFECT_BONUS = 3,
        SAVING_THROW_BASE_BONUS = 4,
        SAVING_THROW_DC = 5,
        SAVING_THROW_CREATURE = 6
    };
    CSWCCMessageData();
    ~CSWCCMessageData();

    int GetInteger(int nIndex);
    float GetFloat(int nIndex);
    unsigned long GetObjectID(int nIndex);
    CExoString GetString(int nIndex);
    int SaveData(CResGFF* pRes, CResStruct* pStruct);
    void SetString(int nIndex, const CExoString& sValue);
    int LoadData(CResGFF* pRes, CResStruct* pStruct);
    void CopyTo(CSWCCMessageData* pData);

    void SetInteger(int nIndex, int nValue);
    void SetFloat(int nIndex, float fValue);
    void SetObjectID(int nIndex, unsigned long oidValue);

    unsigned char m_nType;
    CExoArrayList<int> m_lstIntegers;
    CExoArrayList<float> m_lstFloats;
    CExoArrayList<CExoString> m_lstStrings;
    CExoArrayList<unsigned long> m_lstObjectIDs;
};

class CExoLocString;
class CGameObject;
class CGameObjectArray;
class CItemRepository;
class CLoopingVisualEffect;
class CLastUpdateObject;
class CLastUpdatePartyObject;
class CResRef;
class CSWCCMessageData;
class CSWSArea;
class CSWSAreaOfEffectObject;
class CSWSCombatAttackData;
class CSWSCreature;
class CSWSDoor;
class CSWSItem;
class CSWSObject;
class CSWSPlaceable;
class CSWSPlayerLUOInventory;
class CSWSPlayerLastUpdateObject;
class CSWSPlayerLUOSortedObjectList;
class CSWSTrigger;
class Vector;

class CSWSPlayer;

class CSWSMessage : public CSWMessage
{
public:
    static const int GUI_PANEL_PLAYER_DEATH;
    static const int GUI_PANEL_PARTY_INVITATION_RECEIVED;

    enum LoginCharacterRequest
    {
        LOGIN_LOCAL_CHARACTER = 1,
        LOGIN_SERVER_CHARACTER = 2,
        LOGIN_MODULE_CHARACTER = 14,
        LOGIN_DEFAULT_MODULE_CHARACTER = 15,
        LOGIN_SERVER_PLAYER_CHARACTER = 17,
        LOGIN_LOCAL_CHARACTER_DATA = 19
    };

    enum SaveLoadStatus
    {
        SAVELOAD_STATUS_LOAD_FAILED = 1,
        SAVELOAD_STATUS_SAVE_FAILED = 2
    };
    enum SaveLoadFailure
    {
        SAVELOAD_FAILURE_DISK_SPACE = 1,

        SAVELOAD_FAILURE_MODULE_NOT_FOUND = 3
    };

    enum ModuleSaveGameStatus { SAVEGAME_STATUS_STARTED = 0, SAVEGAME_STATUS_COMPLETE = 1 };

    enum GameObjectUpdateSelector
    {

        GAME_OBJECT_UPDATE_RECORDS = 1,
        GAME_OBJECT_UPDATE_VISUAL_EFFECT = 3
    };

    int SendServerToPlayerGameObjUpdateVisEffect(CSWSPlayer* pPlayer,
        unsigned short nVisualEffectId, unsigned long oidTarget,
        unsigned long oidCreator, unsigned long oidBeamTarget,
        unsigned char nBeamType, unsigned char nBeamMode,
        Vector vTargetPosition, float fDuration);

    int SendServerToPlayerChatMultiLangMessage(unsigned char nType,
        unsigned long oidSpeaker, CExoLocString& lsMessage,
        unsigned long oidTarget, unsigned char nGender,
        unsigned long* pnRecipients, unsigned long nRecipientCount,
        int bExcludeUnlisted, const CResRef& resSound,
        unsigned long oidSound, int nSoundVolume);

    int SendServerToPlayerChatMultiLang_Helper(unsigned long nPlayerId,
        unsigned char nType, unsigned long oidSpeaker, CExoLocString& lsMessage,
        unsigned long oidTarget, unsigned char nGender, int bListedRecipient,
        const CResRef& resSound, unsigned long oidSound, int nSoundVolume);

    enum ModuleMessageSelector
    {
        MODULE_LOADED = 2,
        MODULE_SAVE_GAME = 4,
        MODULE_LOAD_GAME = 5,
        MODULE_RUN = 7,

        MODULE_LOAD = 1,
        MODULE_UPDATE_TIME = 3,
        MODULE_SUCCESS = 6,
        MODULE_UNLOAD = 8,
        MODULE_EXPORT_REPLY = 10,
        MODULE_START_NEW = 11,
        MODULE_END_NEW = 12,
        MODULE_SAVE_STATUS = 13,
        MODULE_END_GAME = 14
    };

    enum JournalMessageSelector
    {
        JOURNAL_QUEST_SCREEN_OPEN = 10,
        JOURNAL_QUEST_SCREEN_CLOSE = 11,
        JOURNAL_ADD_WORLD_ENTRY = 13,
        JOURNAL_DELETE_WORLD_ENTRY = 14
    };

    enum FeedbackIntegerParameter
    {

        FEEDBACK_INTEGER_EXPERIENCE = 0,
        FEEDBACK_INTEGER_MESSAGE = 9
    };

    enum FeedbackMessageSelector { FEEDBACK_MESSAGE_EXPERIENCE_GAINED = 143 };

    enum PlayModuleCharacterListSelector { PLAY_MODULE_CHARACTER_LIST_RESPONSE = 3 };

    enum ExamineGuiSelector
    {

        EXAMINE_GUI_ITEM_DATA = 2,
        EXAMINE_GUI_CREATURE_DATA = 3,

        EXAMINE_GUI_DOOR_DATA = 6
    };

    enum ExamineMineDifficulty
    {
        EXAMINE_MINE_EASY = 0,
        EXAMINE_MINE_MODERATE = 1,
        EXAMINE_MINE_DIFFICULT = 2,
        EXAMINE_MINE_VERY_DIFFICULT = 3,
        EXAMINE_MINE_IMPOSSIBLE = 4
    };

    CSWSMessage();
    virtual ~CSWSMessage();

    void AddTriggerGeometryToMessage(CSWSTrigger* pTrigger);

    void WriteGameObjUpdate_MinorGUIPanels(CSWSPlayer* pPlayer);
    int WriteGameObjUpdate_PartyAIState(CSWSPlayer* pPlayer);
    void WriteGameObjUpdate_CharacterSheet(CSWSPlayer* pPlayer,
                                          unsigned long nUpdateRequired);

    int SendServerToPlayerCCMessage(unsigned long nPlayerId,
                                    unsigned char nMessage,
                                    CSWCCMessageData* pMessageData,
                                    CSWSCombatAttackData* pAttackData);

    int HandlePlayerToServerQuickChatMessage(CSWSPlayer* pPlayer,
                                             unsigned char nMinor);
    int HandlePlayerToServerGuiInventoryMessage(CSWSPlayer* pPlayer,
                                                unsigned char nMinor);
    int HandlePlayerToServerInputCancelGuiTimingEvent(CSWSPlayer* pPlayer);
    int HandlePlayerToServerInputDriveControl(CSWSPlayer* pPlayer);
    int HandlePlayerToServerInputAbortDriveControl(CSWSPlayer* pPlayer);
    int HandlePlayerToServerShutDownServer(CSWSPlayer* pPlayer,
                                           unsigned char nMinor);
    int HandlePlayerToServerPlayModuleCharacterList_Stop(CSWSPlayer* pPlayer);
    int HandlePlayerToServerMapPinMessage(CSWSPlayer* pPlayer,
                                          unsigned char nMinor);
    int HandlePlayerToServerMapPinSetMapPinAt(CSWSPlayer* pPlayer);
    int HandlePlayerToServerMapPinDestroyMapPin(CSWSPlayer* pPlayer);
    int HandlePlayerToServerMapPinChangePin(CSWSPlayer* pPlayer);
    int SendServerToPlayerMapPinAdded(CSWSPlayer* pPlayer, Vector vPosition,
                                      const CExoString& sNote,
                                      unsigned long nMapPin);
    int SendServerToPlayerDebugInfo_Creature(CSWSPlayer* pPlayer,
                                             unsigned long oidObject);
    int SendServerToPlayerDebugInfo_Area(CSWSPlayer* pPlayer,
                                         unsigned long oidObject);
    int SendServerToPlayerDebugInfo_Placeable(CSWSPlayer* pPlayer,
                                             unsigned long oidObject);
    int SendServerToAllPlayersCreatureUpdate_StripEquippedItems(
        unsigned long oidCreature);
    int SendServerToPlayerDebugInfo_Item(CSWSPlayer* pPlayer,
                                         unsigned long oidObject);
    int HandlePlayerToServerDialogMessage(CSWSPlayer* pPlayer,
                                          unsigned char nMinor);
    int HandlePlayerToServerServerStatusMessage(CSWSPlayer* pPlayer,
                                                unsigned char nMinor);
    int HandlePlayerToServerCharacterDownload(CSWSPlayer* pPlayer,
                                              unsigned char nMinor);
    int HandlePlayerToServerPlayModuleCharacterList(CSWSPlayer* pPlayer,
                                                    unsigned char nMinor);
    int HandlePlayerToServerPlayModuleCharacterList_Start(CSWSPlayer* pPlayer);
    int SendServerToPlayerServerStatus_Status(unsigned long nPlayerId);
    int SendServerToPlayerCharacterDownloadFail(CSWSPlayer* pPlayer);
    int SendServerToPlayerCharacterDownloadReply(CSWSPlayer* pPlayer);
    int HandlePlayerToServerAreaMessage(CSWSPlayer* pPlayer,
                                        unsigned char nMinor);
    int SendServerToPlayerPlayerList_Add(unsigned long nPlayerId,
                                         CSWSPlayer* pPlayer);
    int SendServerToPlayerPlayerList_All(CSWSPlayer* pPlayer);
    int SendServerToPlayerModule_SetPauseState(unsigned char nPauseType,
                                               int bPause);
    int HandlePlayerToServerCharListMessage(CSWSPlayer* pPlayer,
                                            unsigned char nMinor);
    int HandlePlayerToServerLoginMessage(CSWSPlayer* pPlayer,
                                         unsigned char nMinor);
    int HandlePlayerToServerModuleMessage(CSWSPlayer* pPlayer,
                                          unsigned char nMinor);
    int HandlePlayerToServerCheatMessage(CSWSPlayer* pPlayer,
                                         unsigned char nMinor);
    int HandlePlayerToServerGameObjectUpdate(CSWSPlayer* pPlayer,
                                             unsigned char nMinor);
    int HandlePlayerToServerInputMessage(CSWSPlayer* pPlayer,
                                         unsigned char nMinor);
    int HandlePlayerToServerInputWalkToWaypoint(CSWSPlayer* pPlayer);
    int HandlePlayerToServerInventoryMessage(CSWSPlayer* pPlayer,
                                             unsigned char nMinor);
    int HandlePlayerToServerParty(CSWSPlayer* pPlayer, unsigned char nMinor);
    int HandlePlayerToServerJournalMessage(CSWSPlayer* pPlayer,
                                           unsigned char nMinor);
    int SendServerToPlayerJournalFullUpdate(CSWSPlayer* pPlayer);
    int SendServerToPlayerLevelUp_Confirmation(unsigned long nPlayerId,
                                               int bSuccess);
    int SendServerToPlayerParty_List(unsigned long nPlayerId, int nCount,
                                     unsigned long* pMembers,
                                     unsigned char nMinor,
                                     unsigned long oidExclude);
    int SendServerToPlayerChat_Party(unsigned long nPlayerId,
                                     unsigned long nVolume,
                                     const CExoString& sMessage);
    int SendServerToPlayerChat_Talk(unsigned long nPlayerId,
                                    unsigned long oidSpeaker,
                                    const CExoString& sMessage);
    int SendServerToPlayerChat_ServerTell(unsigned long nPlayerId,
                                          const CExoString& sMessage);
    int SendServerToPlayerChat_Tell(unsigned long nPlayerId,
                                    unsigned long oidSpeaker,
                                    const CExoString& sMessage);
    int SendServerToPlayerChat_Shout(unsigned long nPlayerId,
                                     unsigned long oidSpeaker,
                                     const CExoString& sMessage);
    int SendServerToPlayerChat_Whisper(unsigned long nPlayerId,
                                       unsigned long oidSpeaker,
                                       const CExoString& sMessage);
    int SendServerToPlayerJournalAddQuest(
        CSWSPlayer* pPlayer, const CExoString& sQuestTag, int nState,
        unsigned long nDate, unsigned short nTime, int bPicture,
        unsigned long nNameStrRef, unsigned long nTextStrRef,
        CExoLocString& lsName, CExoLocString& lsText, int nPriority,
        int nPercentComplete, int bComplete);
    int SendServerToPlayerJournalRemoveQuest(CSWSPlayer* pPlayer,
                                             const CExoString& sQuestTag);

    enum CustomTokenSelector
    {
        CUSTOM_TOKEN_SINGLE = 1,
        CUSTOM_TOKEN_LIST = 2
    };
    int SendServerToPlayerSetCustomToken(unsigned long nPlayerId, int nToken,
                                         const CExoString& sValue);
    int SendServerToPlayerAIActionPlaySound(unsigned long nPlayerId,
                                            unsigned long oidSpeaker,
                                            const CExoString& sSound);
    int SendServerToPlayerSaveLoad_Status(CSWSPlayer* pPlayer,
                                          unsigned char nStatus,
                                          unsigned long nValue);
    int SendServerToPlayerArea_VisualEffect(CSWSPlayer* pPlayer,
                                            unsigned short nEffect,
                                            Vector vPosition);
    int SendServerToPlayerSoundObject_Play(CSWSPlayer* pPlayer,
                                           unsigned long oidSound);
    int SendServerToPlayerSoundObject_Stop(CSWSPlayer* pPlayer,
                                           unsigned long oidSound);
    int SendServerToPlayerSoundObject_ChangePosition(CSWSPlayer* pPlayer,
                                                     unsigned long oidSound,
                                                     Vector vPosition);
    int SendServerToPlayerSafeProjectileProjectile(
        CSWSPlayer* pPlayer, unsigned long oidSource, unsigned long oidTarget,
        Vector vStart, Vector vEnd, unsigned long nProjectileType,
        unsigned char nProjectileFlags, int nDelay, unsigned char nSpellType,
        unsigned char nSpellLevel, int nSpellId);
    int SendServerToPlayerSafeProjectileSpell(
        CSWSPlayer* pPlayer, unsigned long oidSource, unsigned long oidTarget,
        Vector vStart, Vector vEnd, unsigned long nProjectileType,
        unsigned char nSpellType, unsigned long nSpellId,
        unsigned char nPathType, unsigned char nSpellLevel,
        unsigned char nCasterLevel, unsigned long nDelay, int nVisualEffect);
    int SendServerToPlayerCheatDebugMode(int bDebugMode);
    int SendServerToPlayerModule_SaveGameStatus(unsigned char nStatus);
    int SendServerToPlayerLoadBar_StartStallEvent(unsigned long nEvent);
    int SendServerToPlayerLoadBar_UpdateStallEvent(unsigned long nEvent,
                                                   unsigned long nProgress);
    int SendServerToPlayerLoadBar_EndStallEvent(unsigned long nEvent,
                                                unsigned long nResult);
    int HandlePlayerToServerLevelUpMessage(CSWSPlayer* pPlayer,
                                           unsigned char nMinor);
    int HandlePlayerToServerPlayerDeath(CSWSPlayer* pPlayer,
                                        unsigned char nMinor);
    int HandlePlayerToServerMessage(unsigned long nPlayerId,
                                    unsigned char* pMessage,
                                    unsigned long nSize);
    int HandleServerAdminToServerMessage(unsigned long nPlayerId,
                                         unsigned char* pMessage,
                                         unsigned long nSize);
    int ParseGetString(unsigned char* pString, unsigned long nLength,
                       CExoString& sString, unsigned long nMaxLength);
    int ParseToken(const char* szToken, unsigned char** ppString,
                   unsigned long& nLength, int bPeriod);
    int SendServerToPlayerModule_Loading(CSWSPlayer* pPlayer);
    int SendServerToPlayerArea_ClientArea(CSWSPlayer* pPlayer, CSWSArea* pArea,
                                          float fX, float fY, float fZ,
                                          const Vector& vOrientation,
                                          int bPlayerIsNewToModule);
    int SendServerToPlayerSetCustomTokenList(unsigned long nPlayerId);
    int SendServerToPlayerModule_StartStartNewModule();
    int SendServerToPlayerLogin_NeedCharacter(unsigned long nPlayerId);
    int SendServerToPlayerLogin_Confirm(unsigned long nPlayerId);
    int SendServerToPlayerUpdateCharResponse(CSWSPlayer* pPlayer,
                                             unsigned char nResponse,
                                             CResRef resFile);

    enum UpdateCharacterSource
    {
        UPDATE_CHARACTER_LOCAL = 1,
        UPDATE_CHARACTER_MODULE = 4,
        UPDATE_CHARACTER_SERVER_VAULT = 5
    };
    enum UpdateCharacterSelector { UPDATE_CHARACTER_RESPONSE = 4 };
    int SendServerToPlayerCharList(CSWSPlayer* pPlayer);
    int SendServerToPlayerPlayModuleCharacterListResponse(
        unsigned long nPlayerId, unsigned long oidPlayer, int bStart);
    int HandlePlayerToServerGuiContainerMessage(CSWSPlayer* pPlayer,
                                                unsigned char nMinor);
    int HandlePlayerToServerCharacterSheetMessage(CSWSPlayer* pPlayer,
                                                  unsigned char nMinor);
    int HandlePlayerToServerGoldMessage(CSWSPlayer* pPlayer,
                                        unsigned char nMinor);
    int SendServerToPlayerGUICharacterSheet_NotPermitted(
        unsigned long nPlayerId, unsigned long oidCreature);
    int SendServerToPlayerItemScriptAdd(unsigned long nPlayerId,
                                        CSWSArea* pArea, CSWSItem* pItem,
                                        Vector vPosition);
    void AssignCreatureLists(CExoArrayList<unsigned long>* pSource,
                             CExoArrayList<unsigned long>* pDestination);
    int CompareCreatureLists(CExoArrayList<unsigned long>* pFirst,
                             CExoArrayList<unsigned long>* pSecond);
    int CompareVisualEffectLists(CExoArrayList<CLoopingVisualEffect*>* pFirst,
                                 CExoArrayList<CLoopingVisualEffect*>* pSecond);
    void AssignVisualEffectLists(CExoArrayList<CLoopingVisualEffect*>* pDestination,
                                 CExoArrayList<CLoopingVisualEffect*>* pSource);
    int ComputeVisibilityLists(CSWSCreature* pCreature,
                               CSWSPlayerLastUpdateObject* pLastUpdateObject);
    int HandlePlayerToServerChatMessage(CSWSPlayer* pPlayer,
                                        unsigned char nMinor);
    int SendServerToPlayerChatMessage(unsigned char nType,
                                      unsigned long oidSpeaker,
                                      CExoString sMessage,
                                      unsigned long nPlayerId,
                                      const CExoString& sTargetName);

    int SendServerToPlayerChatStrRefMessage(unsigned char nType,
                                            unsigned long oidSpeaker,
                                            unsigned long nStrRef);
    int SendServerToPlayerAIActionPlaySound(unsigned long nPlayerId,
                                            unsigned long oidObject,
                                            CExoString* pSound);
    int SendServerToPlayerQuickChatMessage(unsigned long nPlayerId,
                                           unsigned short nQuickChat);

    unsigned long ReadOBJECTIDServer();
    void WriteOBJECTIDServer(unsigned long oidObject);
    void WriteCExoLocStringServer(CExoLocString& lsString,
                                  unsigned char nGender);

    static int GetLocStringServer(unsigned long nPlayerId,
                                  CExoLocString& lsFirst,
                                  CExoLocString& lsSecond, CExoString& sResult,
                                  unsigned char nGender);
    static int GetLocStringServerTime(unsigned long nPlayerId,
                                      CExoLocString& lsString,
                                      CExoString& sResult, float& fTime,
                                      unsigned char nGender);

    int SendServerToServerAdminMessage(unsigned long nPlayerId,
                                       const CExoString& sMessage);
    void SendServerToServerAdminServerStatus(unsigned long nPlayerId);
    void SendServerToServerAdminPlayerList(unsigned long nPlayerId);
    void SendServerToServerAdminModuleList(unsigned long nPlayerId);

    int SendServerToPlayerAmbientMusicPlay(unsigned long nPlayerId, int bPlay);
    int SendServerToPlayerAmbientMusicSetDelay(unsigned long nPlayerId,
                                               int nDelay);
    int SendServerToPlayerAmbientMusicChangeTrack(unsigned long nPlayerId,
                                                  int bDay, int nTrack);
    int SendServerToPlayerAmbientBattleMusicPlay(unsigned long nPlayerId,
                                                 int bPlay);
    int SendServerToPlayerAmbientBattleMusicChange(unsigned long nPlayerId,
                                                   int nTrack);
    int SendServerToPlayerAmbientSoundLoopPlay(unsigned long nPlayerId,
                                               int bPlay);
    int SendServerToPlayerAmbientSoundLoopChange(unsigned long nPlayerId,
                                                 int bDay, int nTrack);
    int SendServerToPlayerAmbientSoundVolumeChange(unsigned long nPlayerId,
                                                   int bDay, int nVolume);
    int SendServerToPlayerDestroyDeathGUI(unsigned long nPlayerId);
    int SendServerToPlayerMessage(unsigned long nPlayerId, unsigned char nMajor,
                                  unsigned char nMinor, unsigned char* pMessage,
                                  unsigned long nSize);
    int SendServerToPlayerModule_Info(unsigned long nPlayerId);
    int SendServerToPlayerExamineGui_ItemData(CSWSPlayer* pPlayer,
                                              unsigned long oidItem);
    int SendServerToPlayerExamineGui_CreatureData(CSWSPlayer* pPlayer,
                                                  unsigned long oidCreature);
    int SendServerToPlayerExamineGui_PlaceableData(CSWSPlayer* pPlayer,
                                                   unsigned long oidObject);
    int SendServerToPlayerExamineGui_DoorData(CSWSPlayer* pPlayer,
                                              unsigned long oidObject);
    int SendServerToPlayerExamineGui_MineData(CSWSPlayer* pPlayer,
                                              unsigned long oidObject,
                                              CSWSCreature* pCreature,
                                              int bRecoverable);
    int SendServerToPlayerGameObjUpdate_ObjControl(unsigned long nPlayerId,
                                                   unsigned long oidObject);
    int SendServerToPlayerInventory_Drop(unsigned long nPlayerId,
                                         unsigned long oidObject);
    int SendServerToPlayerInventory_DropCancel(unsigned long nPlayerId,
                                               unsigned long oidObject);
    int SendServerToPlayerInventory_EquipCancel(unsigned long nPlayerId,
                                                unsigned long oidObject,
                                                unsigned long nInventorySlot);
    int SendServerToPlayerInventory_Pickup(unsigned long nPlayerId,
                                           unsigned long oidObject);
    int SendServerToPlayerInventory_PickupCancel(unsigned long nPlayerId,
                                                 unsigned long oidObject);
    int SendServerToPlayerInventory_Unequip(unsigned long nPlayerId,
                                            unsigned long oidObject, int bFlag);
    int SendServerToPlayerInventory_UnequipCancel(unsigned long nPlayerId,
                                                  unsigned long oidObject);
    int ComputeGameObjectUpdateForCategory(unsigned long nCategory,
        unsigned long nMessageSizeLimit, CSWSPlayer* pPlayer,
        CSWSObject* pPlayerObject, CGameObjectArray* pObjectArray,
        CSWSPlayerLUOSortedObjectList* pObjects, int nObjectCount);
    CSWSPlayerLUOSortedObjectList* SortObjectsForGameObjectUpdate(
        CSWSPlayer* pPlayer, CSWSObject* pPlayerObject,
        CGameObjectArray* pObjectArray, int* pObjectCount);

    enum { DEFAULT_OBJECT_VISIBLE_DISTANCE = 250 };
    void WriteGameObjUpdate_MajorGUIPanels(CSWSPlayer* pPlayer);

    enum
    {
        GAME_OBJECT_UPDATE_BUFFER_SIZE = 1024,
        GAME_OBJECT_UPDATE_SIZE_LIMIT = 2000
    };
    enum GameObjectUpdateStage
    {
        GAME_OBJECT_STAGE_DELETE_OTHER_AREAS = 0,
        GAME_OBJECT_STAGE_PLAYER = 1,
        GAME_OBJECT_STAGE_MAJOR_GUI = 2,
        GAME_OBJECT_STAGE_COMBAT = 3,
        GAME_OBJECT_STAGE_MINOR_GUI = 4,
        GAME_OBJECT_STAGE_RESERVED_CATEGORY = 6,
        GAME_OBJECT_STAGE_CREATURES = 7,
        GAME_OBJECT_STAGE_PARTY_AI = 8,
        GAME_OBJECT_STAGE_TRIGGERS = 9,
        GAME_OBJECT_STAGE_DOORS = 10,
        GAME_OBJECT_STAGE_ITEMS = 11,
        GAME_OBJECT_STAGE_PLACEABLES = 12,
        GAME_OBJECT_STAGE_AREA_EFFECTS = 13,
        GAME_OBJECT_STAGE_COMPLETE = 14
    };
    void ComputeGameObjectUpdateForObject(CSWSPlayer* pPlayer,
                                          CSWSObject* pPlayerObject,
                                          CGameObjectArray* pObjectArray,
                                          unsigned long oidObject);
    int TestObjectVisible(CSWSObject* pObject, CSWSObject* pPlayerObject);
    unsigned long ComputeUpdateRequired(CSWSPlayer* pPlayer,
        CSWSObject* pObject, CLastUpdateObject* pLastUpdateObject,
        int bPlayerObject);

    enum AppearanceUpdateMask
    {

        APPEARANCE_UPDATE_ALL = 0xFFFFFFFF,
        APPEARANCE_UPDATE_TYPE = 0x0001,
        APPEARANCE_UPDATE_PART0 = 0x0002,
        APPEARANCE_UPDATE_PART1 = 0x0004,
        APPEARANCE_UPDATE_PART2 = 0x0008,
        APPEARANCE_UPDATE_PART3 = 0x0010,
        APPEARANCE_UPDATE_PART4 = 0x0020,
        APPEARANCE_UPDATE_PART5 = 0x0040,
        APPEARANCE_UPDATE_PART6 = 0x0080,
        APPEARANCE_UPDATE_PART7 = 0x0800,
        APPEARANCE_UPDATE_PART8 = 0x1000,
        APPEARANCE_UPDATE_EQUIPMENT = 0x0200,

        APPEARANCE_UPDATE_NAME = 0x0400,
        APPEARANCE_UPDATE_FLAGS = 0x2000,
        APPEARANCE_UPDATE_STATE = 0x4000,
        APPEARANCE_UPDATE_VARIANT = 0x8000
    };
    unsigned long ComputeAppearanceUpdateRequired(CSWSObject* pObject,
        CLastUpdateObject* pLastUpdateObject);
    void WriteGameObjUpdate_UpdateAppearance(CSWSObject* pObject,
        CLastUpdateObject* pLastUpdateObject, unsigned long nAppearanceMask);
    void WriteGameObjUpdate_WriteInventorySlotDelete(CSWSCreature* pCreature,
                                                    unsigned long nSlot);

    enum NewObjectUpdateMask
    {
        NEW_OBJECT_UPDATE_DOOR = 0xFFFFFFF7,
        NEW_OBJECT_UPDATE_ITEM = 0xFFFFFFF3,
        NEW_OBJECT_UPDATE_AREA_EFFECT = 0xFFFFFFFF,
        NEW_OBJECT_UPDATE_TRIGGER = 0xFFFFFFF3,
        NEW_OBJECT_UPDATE_PLACEABLE = 0xFFFFFFF7,
        NEW_OBJECT_UPDATE_CREATURE = 0x00103967
    };
    CLastUpdateObject* CreateNewLastUpdateObject(CSWSPlayer* pPlayer,
        CSWSObject* pObject, unsigned long* pnUpdateMask,
        unsigned long* pnAppearanceMask);
    void TestObjectUpdateDifferences(CSWSPlayer* pPlayer, CSWSObject* pObject,
                                    CLastUpdateObject** ppLastUpdateObject,
                                    unsigned long* pnUpdateMask,
                                    unsigned long* pnAppearanceMask);
    void WriteGameObjUpdate_UpdateObject(CSWSPlayer* pPlayer,
                                        CSWSObject* pObject,
                                        CLastUpdateObject* pLastUpdateObject,
                                        unsigned long nUpdateMask,
                                        unsigned long nAppearanceMask);

    enum ObjectUpdateMask
    {
        OBJECT_UPDATE_LOCATION = 0x00000001,
        OBJECT_UPDATE_ORIENTATION = 0x00000002,
        OBJECT_UPDATE_ANIMATION = 0x00000004,
        OBJECT_UPDATE_VISUAL_EFFECTS = 0x00000008,
        OBJECT_UPDATE_TRAP_STATE = 0x00000010,
        OBJECT_UPDATE_PORTRAIT = 0x00000020,
        OBJECT_UPDATE_AI_STATE = 0x00000040,
        OBJECT_UPDATE_MOVEMENT_RATES = 0x00000100,
        OBJECT_UPDATE_POSITION = 0x00000200,
        OBJECT_UPDATE_HIT_POINTS = 0x00000400,
        OBJECT_UPDATE_DAMAGE_LEVEL = 0x00000800,
        OBJECT_UPDATE_CHARACTER_DETAILS = 0x00001000,
        OBJECT_UPDATE_PARTY_STATE = 0x00004000,
        OBJECT_UPDATE_PERCEPTION = 0x00008000,
        OBJECT_UPDATE_NAME = 0x00010000,
        OBJECT_UPDATE_ASSOCIATE_STATE = 0x00020000,
        OBJECT_UPDATE_AREA = 0x00040000,
        OBJECT_UPDATE_FORCE_POINTS = 0x00080000,
        OBJECT_UPDATE_CORPSE = 0x00100000,
        OBJECT_UPDATE_NEW_PARTY_OBJECT = 0x00056660
    };
    void TestPartyObjectUpdateDifferences(CSWSPlayer* pPlayer,
        CSWSCreature* pCreature, CLastUpdatePartyObject** ppLastUpdateObject,
        unsigned long* pnUpdateMask);
    void StoreValuesInLastUpdatePartyObject(CSWSCreature* pPlayerCreature,
        CLastUpdatePartyObject* pLastUpdateObject, CSWSCreature* pCreature,
        unsigned long nUpdateMask);
    void DeleteLastUpdateObjectsForObject(CSWSPlayer* pPlayer,
                                         unsigned long oidObject);
    void StoreValuesInLastUpdateObject(CSWSPlayer* pPlayer,
                                      CLastUpdateObject* pLastUpdateObject,
                                      CSWSObject* pObject,
                                      unsigned long nUpdateMask,
                                      unsigned long nAppearanceMask);
    int SendServerToPlayerGameObjUpdate(CSWSPlayer* pPlayer,
                                        unsigned long oidObject);
    int SendServerToPlayerGameObjUpdate(CSWSPlayer* pPlayer);
    void DeleteLastUpdateObjectsInOtherAreas(CSWSPlayer* pPlayer);
    int SendServerToPlayerCheatNasty(unsigned long nPlayerId,
                                     unsigned long oidObject);
    int SendServerPlayerItemUpdate_DestroyItem(CSWSPlayer* pPlayer,
                                               unsigned long oidItem);
    int SendServerToPlayerPlayerList_Delete(unsigned long nPlayerId,
                                            CSWSPlayer* pPlayer);
    void UpdateLastUpdateInventory(CSWSPlayer* pPlayer, unsigned long nSlot);
    int SendServerToPlayerInventory_LearnScroll(unsigned long nPlayerId,
                                                unsigned long oidItem,
                                                unsigned char nMinor);
    int SendServerToPlayerShutDownServer(unsigned long nPlayerId,
                                         unsigned long nReason);
    int SendServerToPlayerModule_DumpPlayer(CSWSPlayer* pPlayer);
    int SendServerToPlayerChat_StrRef(unsigned long nPlayerId,
                                      unsigned long oidSpeaker,
                                      unsigned char nMinor,
                                      unsigned long nStrRef);
    int SendServerToPlayerQuickChat(unsigned long nPlayerId,
                                    unsigned long oidSpeaker,
                                    unsigned short nQuickChat);

    void UpdateLastUpdateObject(CSWSPlayer* pPlayer, CSWSObject* pObject,
                               CLastUpdateObject* pLastUpdateObject,
                               unsigned long nUpdateMask);
    void WriteGameObjUpdate_PlayerUpdate(CSWSPlayer* pPlayer,
        CSWSPlayerLastUpdateObject* pPlayerUpdate,
        CLastUpdateObject* pObjectUpdate, unsigned long nUpdateMask);
    void UpdateLastUpdateObjectAppearance(CSWSObject* pObject,
                                         CLastUpdateObject* pLastUpdateObject,
                                         unsigned long nAppearanceMask);
    void AddDoorAppearanceToMessage(CSWSDoor* pDoor);
    void AddPlaceableAppearanceToMessage(CSWSPlaceable* pPlaceable);
    void AddAreaOfEffectObjectToMessage(
        CSWSAreaOfEffectObject* pAreaOfEffectObject);
    void AddActiveItemPropertiesToMessage(CSWSItem* pItem,
                                          CSWSCreature* pCreature);
    void AddItemAppearanceToMessage(CSWSItem* pItem);
    int ComputeInventoryUpdateRequired(CSWSPlayer* pPlayer,
                                       unsigned long nSlot);
    unsigned short ComputeLastUpdate_GuiSkills(CSWSCreature* pCreature);
    int ComputeLastUpdate_GuiKnownSpells(CSWSCreature* pCreature,
        CSWSPlayerLastUpdateObject* pLastUpdateObject);
    void TestPlayerUpdateDifferences(CSWSPlayer* pPlayer,
        CSWSPlayerLastUpdateObject* pPlayerLastUpdate,
        CLastUpdateObject* pLastUpdateObject, unsigned long& nUpdateMask);
    void StoreValuesInLastPlayerUpdateObject(CSWSPlayer* pPlayer,
        CSWSPlayerLastUpdateObject* pPlayerLastUpdate,
        CLastUpdateObject* pLastUpdateObject, unsigned long nUpdateMask);
    void UpdateLastUpdateVisibilityList(CSWSCreature* pCreature,
        CSWSPlayerLastUpdateObject* pLastUpdateObject);

    enum PlayerUpdateMask
    {
        PLAYER_UPDATE_ALL = 0xFFFFFFFF,
        PLAYER_UPDATE_CHARACTER = 0x00000001,
        PLAYER_UPDATE_GUI_SKILLS = 0x00000002,
        PLAYER_UPDATE_GUI_FEATS = 0x00000004,
        PLAYER_UPDATE_STATE = 0x00000008,
        PLAYER_UPDATE_KNOWN_SPELLS = 0x00000010,
        PLAYER_UPDATE_RESERVED_SPELL_LISTS = 0x00000020,
        PLAYER_UPDATE_KNOWN_SPELL_USES = 0x00000040,
        PLAYER_UPDATE_RESERVED_CLASS_SPELL_MASKS = 0x00000080,
        PLAYER_UPDATE_ACTION_QUEUE = 0x00000100,
        PLAYER_UPDATE_HIT_POINTS = 0x00000200,
        PLAYER_UPDATE_ABILITIES = 0x00000800,
        PLAYER_UPDATE_VISIBILITY = 0x00002000,
        PLAYER_UPDATE_SPELL_LIKE_ABILITIES = 0x00004000,
        PLAYER_UPDATE_SKILLS = 0x00008000,
        PLAYER_UPDATE_PORTRAIT = 0x00010000,
        PLAYER_UPDATE_FORCE_POINTS = 0x00020000
    };
    int ComputeLastUpdate_GuiFeats(CSWSCreature* pCreature,
                                  CSWSPlayerLastUpdateObject* pLastUpdate);

    int ComputeLastUpdate_CompareSpellLikeAbility(
        CSWSCreature* pCreature,
        CSWSPlayerLastUpdateObject* pLastUpdateObject);
    void ComputeLastUpdate_WriteSpellLikeAbility(CSWSCreature* pCreature,
        CSWSPlayerLastUpdateObject* pLastUpdateObject);

    enum PlayerFeatUpdate
    {
        PLAYER_FEAT_ADD = 'A', PLAYER_FEAT_DELETE = 'D',
        PLAYER_BONUS_FEAT_ADD = 'B', PLAYER_BONUS_FEAT_DELETE = 'X'
    };
    enum PlayerActionUpdate
    {
        PLAYER_ACTION_ADD = 'A', PLAYER_ACTION_DELETE = 'D',
        PLAYER_ACTION_SPELL = 'S'
    };
    enum SpellLikeAbilityUpdate
    {
        SPELL_LIKE_ABILITY_SIZE = 'S', SPELL_LIKE_ABILITY_UPDATE = 'U'
    };
    enum { PLAYER_SPELL_USES_LEVEL_ZERO = 1 };
    void ComputeLastUpdate_StoreUpdateSpellLikeAbility(
        CSWSCreature* pCreature,
        CSWSPlayerLastUpdateObject* pLastUpdateObject);
    int ComputeLastUpdate_GuiKnownSpellUses(
        CSWSCreature* pCreature,
        CSWSPlayerLastUpdateObject* pLastUpdateObject);

    enum PlayerStateMask
    {
        PLAYER_STATE_DETECT = 0x0001,
        PLAYER_STATE_STEALTH = 0x0002,
        PLAYER_STATE_ASSOCIATE_1 = 0x0004,
        PLAYER_STATE_ASSOCIATE_2 = 0x0008,
        PLAYER_STATE_ASSOCIATE_3 = 0x0010,
        PLAYER_STATE_ASSOCIATE_5 = 0x0020,
        PLAYER_STATE_ASSOCIATE_6 = 0x0040,
        PLAYER_STATE_CREATURE_8E8 = 0x0080,
        PLAYER_STATE_SPECIAL_WALK = 0x0100,
        PLAYER_STATE_ASSOCIATE_4 = 0x0200,
        PLAYER_STATE_DISGUISED = 0x0400,
        PLAYER_STATE_CREATURE_E8 = 0x0800,
        PLAYER_STATE_CREATURE_384 = 0x1000
    };
    unsigned short ComputeLastUpdate_PlayerState(CSWSCreature* pCreature);

    enum GameObjectUpdateCategory
    {
        GAME_OBJECT_CATEGORY_PLAYER = 0,
        GAME_OBJECT_CATEGORY_CREATURE_COMBAT = 1,

        GAME_OBJECT_CATEGORY_RESERVED_3 = 3,
        GAME_OBJECT_CATEGORY_CREATURE = 4,
        GAME_OBJECT_CATEGORY_TRIGGER = 5,
        GAME_OBJECT_CATEGORY_DOOR = 6,
        GAME_OBJECT_CATEGORY_ITEM = 7,
        GAME_OBJECT_CATEGORY_PLACEABLE = 8,
        GAME_OBJECT_CATEGORY_AREA_OF_EFFECT = 9,
        GAME_OBJECT_CATEGORY_NONE = 0xFF
    };
    int SelectCategoryForGameObject(CGameObject* pGameObject,
        CSWSObject* pPlayerObject);
    unsigned char ComputeLastUpdate_AssociateState(CSWSCreature* pCreature);
    int ComputeLastUpdate_ActionQueue(
        CSWSCreature* pCreature,
        CSWSPlayerLastUpdateObject* pLastUpdateObject);
    void UpdateLastUpdateActionQueue(
        CSWSCreature* pCreature,
        CSWSPlayerLastUpdateObject* pLastUpdateObject);
    void WriteGameObjUpdate_WriteInventorySlotAdd(CSWSCreature* pCreature,
                                                  CSWSItem* pItem,
                                                  unsigned long nSlot);
    int SendServerToPlayerUpdateActiveItemProperties(CSWSPlayer* pPlayer,
                                                     unsigned long oidItem,
                                                     unsigned char nProperty,
                                                     unsigned char nMask,
                                                     unsigned char* pValues);
    int SendServerToPlayerVoiceChat_Play(CSWSPlayer* pPlayer,
                                         unsigned long oidObject,
                                         unsigned char nVolume);
    int SendServerToPlayerGuiContainerObject_Open(CSWSPlayer* pPlayer,
                                                  unsigned long oidObject,
                                                  int bOpen);
    int SendServerToPlayerJournalAddWorld(CSWSPlayer* pPlayer, int nIndex,
                                          const CExoString& sText,
                                          const CExoString& sTag,
                                          unsigned long nCalendarDay,
                                          unsigned long nTimeOfDay);
    int SendServerToPlayerJournalAddWorldStrref(CSWSPlayer* pPlayer,
                                                unsigned long nIndex,
                                                unsigned long nTitleStrRef,
                                                unsigned long nTextStrRef,
                                                unsigned long nTagStrRef);
    int SendServerToPlayerJournalDeleteWorld(CSWSPlayer* pPlayer, int nIndex);
    int SendServerToPlayerJournalDeleteWorldStrref(CSWSPlayer* pPlayer,
                                                   unsigned long nStrRef);
    int SendServerToPlayerJournalDeleteWorldAll(CSWSPlayer* pPlayer);
    int SendServerToPlayerJournalFullUpdateNotNeeded(CSWSPlayer* pPlayer,
                                                     int bFullUpdate);
    int SendServerToPlayerJournalUpdated(CSWSPlayer* pPlayer, int bAdded,
                                         int bCompleted,
                                         CExoLocString& lsJournalEntry);
    int SendServerToPlayerModuleUpdate_Time(
        CSWSPlayer* pPlayer, unsigned char nUpdateMask,
        unsigned long nTimeOfDay, unsigned long nDayOfMonth,
        unsigned long nMonth, unsigned long nYear, unsigned char nCalendar,
        unsigned long nCalendarDay);
    int SendServerToPlayerMapPinEnabled(CSWSPlayer* pPlayer,
                                        unsigned long oidMapPin, int bEnabled);
    int SendServerToPlayerGuiTimingEvent(CSWSPlayer* pPlayer, int bStarted,
                                         unsigned char nEventType,
                                         unsigned long nEventId);
    int SendServerToPlayerCamera_ChangeLocation(CSWSPlayer* pPlayer,
                                                int nUpdateMask, float fX,
                                                float fY, float fZ, int nMode);
    int SendServerToPlayerCamera_SetMode(CSWSPlayer* pPlayer,
                                         unsigned char nMode);
    int SendServerToPlayerLogin_CharacterQuery(CSWSPlayer* pPlayer,
                                               unsigned char& nClassCount,
                                               int* pnClasses,
                                               unsigned char* pnClassLevels,
                                               unsigned long& nExperience);
    int SendServerToPlayerModule_ExportReply(CSWSPlayer* pPlayer);
    int SendServerToPlayerModule_EndStartNewModule(int bSuccess);
    int SendServerToPlayerArea_Weather(CSWSPlayer* pPlayer,
                                       unsigned char nWeather, int bEnabled);
    int SendServerToPlayerPopUpGUIPanel(unsigned long oidPC, int nGUIPanel,
                                        int bRespawnButtonEnabled,
                                        int bWaitForHelpButtonEnabled,
                                        int nHelpStringReference,
                                        const CExoString& sHelpString);
    int HasValidString(CExoLocString& lsString, unsigned char nGender);

    void WriteGameObjUpdate_WorkRemaining(CSWSObject* pObject, CSWSArea* pArea,
                                          int nValue, int nValue2);
    void WriteRepositoryUpdate(CSWSPlayer* pPlayer, CSWSObject* pObject,
                                CItemRepository* pRepository,
                                CSWSPlayerLUOInventory* pLastUpdateInventory,
                                unsigned char nList, char nGUI);
};

class CResRef;
class CSWCCreature;
class CSWCItem;
class CSWCObject;
class Vector;
#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"

extern int g_bLoggedIn;

class CSWCLevelUpStats;

class CSWCMessage : public CSWMessage
{
public:

    enum CreatureUpdateMask
    {
        CREATURE_UPDATE_POSITION = 0x00001,
        CREATURE_UPDATE_ORIENTATION = 0x00002,
        CREATURE_UPDATE_ANIMATION = 0x00004,
        CREATURE_UPDATE_VISUAL_EFFECTS = 0x00008,
        CREATURE_UPDATE_PORTRAIT = 0x00020,
        CREATURE_UPDATE_COMBAT_STATE = 0x00040,
        CREATURE_UPDATE_SPEED = 0x00100,
        CREATURE_UPDATE_UNUSED_200 = 0x00200,
        CREATURE_UPDATE_HIT_POINTS = 0x00400,
        CREATURE_UPDATE_STATS_BYTE = 0x00800,
        CREATURE_UPDATE_CLASSES = 0x01000,
        CREATURE_UPDATE_SCENE = 0x02000,
        CREATURE_UPDATE_PARTY_STATUS = 0x04000,
        CREATURE_UPDATE_PERCEPTION = 0x08000,
        CREATURE_UPDATE_NAMES = 0x10000,
        CREATURE_UPDATE_STATES = 0x20000,
        CREATURE_UPDATE_UNUSED_OBJECT = 0x40000,
        CREATURE_UPDATE_FORCE_POINTS = 0x80000
    };

    enum QuickbarActionSelector
    {
        QUICKBAR_ACTION_COMPOSITE = 2,
        QUICKBAR_ACTION_INTEGER_3 = 3,
        QUICKBAR_ACTION_INTEGER_4 = 4,
        QUICKBAR_ACTION_INTEGER_8 = 8,
        QUICKBAR_ACTION_INTEGER_10 = 10,
        QUICKBAR_ACTION_STRINGS = 18,
        QUICKBAR_ACTION_INTEGER_37 = 37,
        QUICKBAR_ACTION_INTEGER_42 = 42,
        QUICKBAR_ACTION_DWORD_BYTE = 44
    };

    enum { QUICKBAR_BUTTON_COUNT = 36 };

    enum DoorUpdateMask { DOOR_UPDATE_FLAGS = 0x10 };

    enum PlaceableUpdateMask { PLACEABLE_UPDATE_FLAGS = 0x10 };

    enum GenericObjectUpdateMask
    {
        GENERIC_OBJECT_UPDATE_POSITION = 0x01,
        GENERIC_OBJECT_UPDATE_ORIENTATION = 0x02,
        GENERIC_OBJECT_UPDATE_ANIMATION = 0x04,
        GENERIC_OBJECT_UPDATE_VISUAL_EFFECTS = 0x08,
        GENERIC_OBJECT_UPDATE_PORTRAIT = 0x20
    };

    enum { MODULE_COMMAND_PREFIX = 's' };

    enum SafeProjectileWeaponHand
    {
        SAFE_PROJECTILE_HAND_RIGHT = 0,
        SAFE_PROJECTILE_HAND_LEFT = 1,
        SAFE_PROJECTILE_HAND_CREATURE = 2
    };

    enum ControlObjectMessage
    {
        CONTROL_OBJECT_ACK_MAJOR = 14,
        CONTROL_OBJECT_ACK_MINOR = 2
    };

    enum GameObjectUpdateSelector
    {
        GAME_OBJECT_UPDATE_RECORDS = 1,
        GAME_OBJECT_UPDATE_CONTROL_OBJECT = 2,
        GAME_OBJECT_UPDATE_VISUAL_EFFECT = 3,
        GAME_OBJECT_UPDATE_STRIP_EQUIPPED_ITEMS = 6,
        GAME_OBJECT_UPDATE_DESTROY_ITEM = 7,
        GAME_OBJECT_UPDATE_ADD_ITEM_SCRIPT = 8
    };

    CSWCMessage();

    enum LoginSelector
    {
        LOGIN_ACCEPTED = 5,
        LOGIN_ACKNOWLEDGED = 6,
        LOGIN_PLAYER_RECORDS = 10,
        LOGIN_END_LOAD_BAR = 16,
        LOGIN_CHARACTER_REPLY = 18
    };

    enum { LOGIN_PLAYER_CAPACITY = 2 };
    ~CSWCMessage();
    int HandleServerToPlayerGuiInventory(unsigned char nMinor);
    int HandleServerToPlayerGuiContainerObjectMessage(unsigned char nMinor);
    int HandleServerToPlayerJournalMessage_AddWorldStrref();
    int HandleServerToPlayerJournalMessage_DeleteWorldAll();
    int HandleServerToPlayerModule_EndStartNewModule();
    int HandleServerToPlayerSafeProjectile(unsigned char nMinor);
    int HandleServerToPlayerSafeProjectileSpell();
    int HandleServerToPlayerSafeProjectileProjectile();
    unsigned long ReadOBJECTIDClient();

    void WriteOBJECTIDClient(unsigned long oidObject);
    int HandleServerToPlayerAreaOfEffectUpdate_Add();
    int HandleServerToPlayerAreaOfEffectUpdate_Delete();
    int HandleServerToPlayerAreaOfEffectUpdate_Update(
        unsigned long oidObject, unsigned long nUpdateMask);
    int HandleServerToPlayerCreatureUpdate_Add();
    int HandleServerToPlayerCreatureUpdate_Appearance();
    void SetGuiSkillRank(CSWCCreature* pCreature, unsigned short nSkillFlags,
                         unsigned short nSkillMask, unsigned short nSkill);
    int HandleServerToPlayerCreatureUpdate_Delete();
    int HandleServerToPlayerCreatureUpdate_Update(unsigned long oidObject,
                                                  unsigned long nUpdateMask);
    int HandleServerToPlayerCreatureUpdate_StripEquippedItems();
    int HandleServerToPlayerDoorUpdate_Add();
    int HandleServerToPlayerDoorUpdate_Delete();
    int HandleServerToPlayerDoorUpdate_Update(unsigned long oidObject,
                                              unsigned long nUpdateMask);
    int HandleServerToPlayerGenericObjectUpdate_Update(
        unsigned long oidObject, unsigned long nUpdateMask);
    int HandleServerToPlayerItemUpdate_Add(int nMode, CSWCItem** ppItem);
    CSWCItem* CreateItem(unsigned long nBaseItem, unsigned char nVariation,
                         char cPrefix);
    int HandleServerToPlayerItemUpdate_Delete();
    int HandleServerToPlayerItemUpdate_Update(unsigned long oidObject,
                                              unsigned long nUpdateMask);
    int HandleServerToPlayerModule_StartStartNewModule();
    int HandleServerToPlayerPlaceableUpdate_Add();
    int HandleServerToPlayerPlaceableUpdate_Delete();
    int HandleServerToPlayerPlaceableUpdate_Update(unsigned long oidObject,
                                                   unsigned long nUpdateMask);
    int HandleServerToPlayerSoundObject_Play();
    int HandleServerToPlayerSoundObject_Stop();
    int HandleServerToPlayerSoundObject_ChangePosition();
    int HandleServerToPlayerSound(unsigned char nMinor);
    int HandleServerToPlayerTriggerUpdate_Add();
    int HandleServerToPlayerTriggerUpdate_Delete();
    int HandleServerToPlayerTriggerUpdate_Update(unsigned long oidObject,
                                                 unsigned long nUpdateMask);
    int HandleServerToPlayerUpdate_Add();
    int HandleServerToPlayerUpdate_Appearance();
    int HandleServerToPlayerUpdate_Delete();
    int HandleServerToPlayerUpdate_Update();

    int HandleServerToPlayerSaveLoad(unsigned char nMinor);
    int HandleServerToPlayerLevelUpMessage(unsigned char nMinor);
    int HandleServerToPlayerGuiInfoPopupMessage(unsigned char nMinor);
    int HandleServerToPlayerGUICharacterSheet(unsigned char nMinor);

    enum CheatSelector
    {
        CHEAT_DO_NASTY = 11,
        CHEAT_PONY_RIDE = 12,
        CHEAT_DEBUG_MODE = 21
    };
    int HandleServerToPlayerCheatMessage(unsigned char nMinor);
    int HandleServerToPlayerCheat_DoNasty();
    int HandleServerToPlayerCheat_PonyRide();
    int HandleServerToPlayerCheat_DebugMode();
    int HandleServerToPlayerGuiQuickbar(unsigned char nMinor);
    int HandleServerToPlayerGuiQuickbar_SetButton(unsigned char nButton,
                                                  int bClear);
    int HandleServerToPlayerGuiTimingEvent(unsigned char nMinor);
    int HandleServerToPlayerGuiTimingEvent_Info();
    int HandleServerToPlayerEndGame();
    int HandleServerToPlayerVoiceChatMessage(unsigned char nMinor);
    int HandleServerToPlayerCombatRound(unsigned char nMinor);
    int HandleServerToPlayerMapPin(unsigned char nMinor);
    int HandleServerToPlayerMapPinEnabled();
    int HandleServerToPlayerMapPinReferenceNumber();
    int HandleServerToPlayerJournalMessage_DeleteWorld();
    int HandleServerToPlayerJournalMessage_DeleteWorldStrref();
    int HandleServerToPlayerJournalMessage_RemoveQuest();
    int HandleServerToPlayerJournalMessage_SetQuestPicture();
    int HandleServerToPlayerJournalMessage_AddWorld();
    int HandleServerToPlayerItemUpdate_DestroyItem();
    int HandleServerToPlayerJournalMessage(unsigned char nMinor);
    int HandleServerToPlayerJournalMessage_AddQuest();
    int HandleServerToPlayerJournalMessage_FullQuestUpdate();
    int HandleServerToPlayerJournalMessage_Updated();
    CExoString ReadCExoLocStringClient(CResRef& cResRef, float* pfLength);
    CExoString ReadCExoLocStringClient();
    int HandleServerToPlayerAmbientMusicPlay();
    int HandleServerToPlayerAmbientMusicSetDelay();
    int HandleServerToPlayerAmbientMusicChangeTrack();
    int HandleServerToPlayerAmbientBattleMusicPlay();
    int HandleServerToPlayerAmbientBattleMusicChange();
    int HandleServerToPlayerAmbientSoundLoopPlay();
    int HandleServerToPlayerAmbientSoundLoopChange();
    int HandleServerToPlayerAmbientSoundVolume();
    int HandleServerToPlayerAmbient(unsigned char nMinor);
    int HandlePlayerToServerShutDownServer(unsigned char nMinor);
    int HandleServerToPlayerUpdate_GuiElements();
    int HandleServerToPlayerUpdate_GuiRepository(int bBank);
    int HandleServerToPlayerUpdate_GuiContainer();
    int HandleServerToPlayerUpdate_GuiInventory();
    int HandleServerToPlayerUpdate_GuiQuickbarItemUseCount();
    int HandleServerToPlayerUpdate_CharacterSheet();
    int HandleServerToPlayerGameObjectUpdate(unsigned char nMinor);
    int HandleServerToPlayerCreatureUpdate_ControlObject(
        unsigned long oidObject);
    int HandleServerToPlayerCreatureUpdate_VisEffect();
    int HandleServerToPlayerItemScript_Add();
    int HandleServerToPlayerUpdate_PlayerInfo();
    int HandleServerToPlayerUpdate_WorkRemaining();
    int HandleServerToPlayerSetCustomToken(unsigned char nMinor);
    int GetSingleCustomTokenStringPart(int nToken, unsigned long nStrRef,
                                       CExoString& sResult);
    int HandleServerToPlayerCamera(unsigned char);
    int HandleServerToPlayerArea(unsigned char);
    int HandleServerToPlayerUpdateVisualEffects(CSWCObject*);
    int HandleServerToPlayerInventory(unsigned char);
    int HandleServerToPlayerActiveItemProperties(unsigned long);
    int HandleServerToPlayerItemAppearance(unsigned long*, unsigned char*,
                                           unsigned char*, unsigned char*);
    int HandleServerToPlayerLogin(unsigned char);
    int HandleServerToPlayerLoadBar(unsigned char);
    int HandleServerToPlayerParty(unsigned char);
    int HandleServerToPlayerServerStatus(unsigned char);
    int HandleServerToPlayerActiveItemPropertyUpdate(unsigned char);
    int HandleServerToPlayerModuleUpdate_Time();
    int HandleServerToPlayerSaveGameStatus();
    int HandleServerToPlayerDebugInfo_Item(unsigned long);
    int HandleServerToPlayerDebugInfo_Placeable(unsigned long);
    int HandleServerToPlayerPopUpGUIPanel(unsigned char);
    int HandleServerToPlayerCharacterDownload(unsigned char);
    int HandleServerToPlayerChat(unsigned char);
    int HandleServerToPlayerCCMessage(unsigned char);
    int HandleServerToPlayerExportReply();
    int HandleServerToPlayerDebugInfo_Creature(unsigned long);
    int HandleServerToPlayerPlayerList(unsigned char);
    int HandleServerToPlayerDebugInfo(unsigned char);
    int HandleServerToPlayerModule(unsigned char);
    int HandleServerToPlayerMessage(unsigned char*, unsigned long);
    int SendPlayerToServerMessage(unsigned char nMajor, unsigned char nMinor,
                                  unsigned char* pData, unsigned long nSize);
    int SendPlayerToServerJournalMessage_QuestScreenOpen();
    int SendPlayerToServerInput_AbortDriveControl();
    int SendPlayerToServerModule_ModuleLoaded();
#ifdef _XBOX
    int SendPlayerToServerModule_SaveGame(unsigned long nSlot,
                                          const CExoString& sName);

    int SendPlayerToServerModule_SaveGame(unsigned long nSlot,
                                          const CExoString& sName,
                                          const CExoString& sPath);
#else
    int SendPlayerToServerModule_SaveGame(unsigned long nSlot,
                                          const CExoString& sName,
                                          const CExoString& sPath);
#endif
    int SendPlayerToServerModule_LoadGame(unsigned long nSlot,
                                          const CExoString& sName,
                                          const CExoString& sPath);
    int SendPlayerToServerParty_Action(unsigned long oidTarget, int nAction);
    int SendPlayerToServerStatus_RequestStatus();
    int SendPlayerToServerGuiContainer_Close(unsigned long oidContainer,
                                             int bTakeAll);
    int SendPlayerToServerInput_CastSpell(unsigned long nSpell,
                                          unsigned char nMultiClass,
                                          unsigned char nDomainLevel,
                                          unsigned char nMetaMagic, int bCheat,
                                          const Vector& vTarget,
                                          unsigned long oidTarget);
    int SendPlayerToServerInput_TogglePauseRequest();
    int SendPlayerToServerInput_GiveItem(unsigned long oidItem,
                                         unsigned long oidTarget,
                                         unsigned long oidOwner, int nStack);
    int SendPlayerToServerArea_AreaLoaded();
    int SendPlayerToServerCheat_Resurrect();
    int SendPlayerToServerCheat_Invulnerability();
    int SendPlayerToServerGuiInventory_Status(int nStatus);
    int SendPlayerToServerInput_Attack(unsigned long oidTarget,
                                       unsigned short nAction);
    int SendPlayerToServerInput_UseSkill(unsigned char nSkill,
                                         unsigned char nUseType,
                                         unsigned long oidTarget,
                                         const Vector& vTarget);
    int SendPlayerToServerInput_UseItem(unsigned long oidItem,
                                        unsigned char nPropertyIndex,
                                        unsigned char nSubIndex,
                                        unsigned long oidTarget,
                                        const Vector& vTarget);
    int SendPlayerToServerInput_UseObject(unsigned long oidObject);
    int SendPlayerToServerInput_ChangeDoorState(unsigned long oidDoor,
                                                unsigned short nState);
    int SendPlayerToServerInput_UnlockObject(unsigned long oidObject,
                                             unsigned long oidKey);
    int SendPlayerToServerInput_Dialog(unsigned long oidObject);
    int SendPlayerToServerInventory_Equip(unsigned long oidItem,
                                          unsigned long nSlot);
    int SendPlayerToServerLogin_LoginLocalCharacter(unsigned char* pData,
                                                    unsigned long nSize,
                                                    int bNewCharacter);
    int SendPlayerToServerLogin_LoginIFOCharacter();
    int SendPlayerToServerLogin_LoginLocalCharacter(const CResRef& cResRef,
                                                    int bNewCharacter);
    int SendPlayerToServer_LevelUp(CSWCLevelUpStats* pOldStats,
                                   CSWCLevelUpStats* pNewStats);
};

#endif
