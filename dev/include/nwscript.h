#ifndef __NWSCRIPT_H__
#define __NWSCRIPT_H__

enum { VFX_IMP_HEALING_SMALL = 1001 };
enum { ALIGNMENT_LIGHT_SIDE = 2 };
enum
{
    ANIMATION_FIREFORGET_ACTIVATE = 115,
    ANIMATION_FIREFORGET_BOW = 105,
    ANIMATION_FIREFORGET_CHOKE = 116,
    ANIMATION_FIREFORGET_CUSTOM01 = 119,
    ANIMATION_FIREFORGET_GREETING = 106,
    ANIMATION_FIREFORGET_HEAD_TURN_LEFT = 100,
    ANIMATION_FIREFORGET_HEAD_TURN_RIGHT = 101,
    ANIMATION_FIREFORGET_INJECT = 112,
    ANIMATION_FIREFORGET_PAUSE_BORED = 103,
    ANIMATION_FIREFORGET_PAUSE_SCRATCH_HEAD = 102,
    ANIMATION_FIREFORGET_PERSUADE = 114,
    ANIMATION_FIREFORGET_SALUTE = 104,
    ANIMATION_FIREFORGET_TAUNT = 107,
    ANIMATION_FIREFORGET_THROW_HIGH = 117,
    ANIMATION_FIREFORGET_THROW_LOW = 118,
    ANIMATION_FIREFORGET_TREAT_INJURED = 120,
    ANIMATION_FIREFORGET_USE_COMPUTER = 113,
    ANIMATION_FIREFORGET_VICTORY1 = 108,
    ANIMATION_FIREFORGET_VICTORY2 = 109,
    ANIMATION_FIREFORGET_VICTORY3 = 110,
    ANIMATION_LOOPING_DANCE = 16,
    ANIMATION_LOOPING_DANCE1 = 17,
    ANIMATION_LOOPING_DEACTIVATE = 20,
    ANIMATION_LOOPING_DEAD = 26,
    ANIMATION_LOOPING_DEAD_PRONE = 30,
    ANIMATION_LOOPING_FLIRT = 14,
    ANIMATION_LOOPING_GET_LOW = 10,
    ANIMATION_LOOPING_GET_MID = 11,
    ANIMATION_LOOPING_HORROR = 18,
    ANIMATION_LOOPING_KNEEL_TALK_ANGRY = 31,
    ANIMATION_LOOPING_KNEEL_TALK_SAD = 32,
    ANIMATION_LOOPING_LISTEN = 2,
    ANIMATION_LOOPING_LISTEN_INJURED = 28,
    ANIMATION_LOOPING_MEDITATE = 3,
    ANIMATION_LOOPING_PAUSE = 0,
    ANIMATION_LOOPING_PAUSE2 = 1,
    ANIMATION_LOOPING_PAUSE3 = 24,
    ANIMATION_LOOPING_PAUSE_DRUNK = 13,
    ANIMATION_LOOPING_PAUSE_TIRED = 12,
    ANIMATION_LOOPING_PRONE = 23,
    ANIMATION_LOOPING_READY = 19,
    ANIMATION_LOOPING_SLEEP = 22,
    ANIMATION_LOOPING_SPASM = 21,
    ANIMATION_LOOPING_TALK_FORCEFUL = 7,
    ANIMATION_LOOPING_TALK_INJURED = 27,
    ANIMATION_LOOPING_TALK_LAUGHING = 8,
    ANIMATION_LOOPING_TALK_NORMAL = 5,
    ANIMATION_LOOPING_TALK_PLEADING = 6,
    ANIMATION_LOOPING_TALK_SAD = 9,
    ANIMATION_LOOPING_TREAT_INJURED = 29,
    ANIMATION_LOOPING_USE_COMPUTER = 15,
    ANIMATION_LOOPING_WELD = 25,
    ANIMATION_LOOPING_WORSHIP = 4,
    ANIMATION_PLACEABLE_ACTIVATE = 200,
    ANIMATION_PLACEABLE_ANIMLOOP01 = 204,
    ANIMATION_PLACEABLE_ANIMLOOP02 = 205,
    ANIMATION_PLACEABLE_ANIMLOOP03 = 206,
    ANIMATION_PLACEABLE_ANIMLOOP04 = 207,
    ANIMATION_PLACEABLE_ANIMLOOP05 = 208,
    ANIMATION_PLACEABLE_ANIMLOOP06 = 209,
    ANIMATION_PLACEABLE_ANIMLOOP07 = 210,
    ANIMATION_PLACEABLE_ANIMLOOP08 = 211,
    ANIMATION_PLACEABLE_ANIMLOOP09 = 212,
    ANIMATION_PLACEABLE_ANIMLOOP10 = 213,
    ANIMATION_PLACEABLE_CLOSE = 203,
    ANIMATION_PLACEABLE_DEACTIVATE = 201,
    ANIMATION_PLACEABLE_OPEN = 202,
};

enum
{
    CREATURE_TYPE_CLASS = 2,
    CREATURE_TYPE_DOES_NOT_HAVE_SPELL_EFFECT = 6,
    CREATURE_TYPE_HAS_SPELL_EFFECT = 5,
    CREATURE_TYPE_IS_ALIVE = 4,
    CREATURE_TYPE_PERCEPTION = 7,
    CREATURE_TYPE_PLAYER_CHAR = 1,
    CREATURE_TYPE_RACIAL_TYPE = 0,
    CREATURE_TYPE_REPUTATION = 3,
};

enum
{
    PERCEPTION_HEARD = 5,
    PERCEPTION_HEARD_AND_NOT_SEEN = 2,
    PERCEPTION_NOT_HEARD = 4,
    PERCEPTION_NOT_SEEN = 6,
    PERCEPTION_NOT_SEEN_AND_NOT_HEARD = 1,
    PERCEPTION_SEEN = 7,
    PERCEPTION_SEEN_AND_HEARD = 0,
    PERCEPTION_SEEN_AND_NOT_HEARD = 3,
};

enum
{
    REPUTATION_TYPE_ENEMY = 1,
    REPUTATION_TYPE_FRIEND = 0,
    REPUTATION_TYPE_NEUTRAL = 2,
};

enum { NPC_PLAYER = -1 };

enum
{
    ABILITY_STRENGTH = 0,
    ABILITY_DEXTERITY = 1,
    ABILITY_CONSTITUTION = 2,
    ABILITY_INTELLIGENCE = 3,
    ABILITY_WISDOM = 4,
    ABILITY_CHARISMA = 5
};

enum
{
    ATTACK_RESULT_HIT_SUCCESSFUL = 1,
    ATTACK_RESULT_CRITICAL_HIT = 2,
    ATTACK_RESULT_AUTOMATIC_HIT = 3,
    ATTACK_RESULT_MISS = 4,
    ATTACK_RESULT_ATTACK_RESISTED = 5,
    ATTACK_RESULT_PARRIED = 8,
    ATTACK_RESULT_DEFLECTED = 9,

    ATTACK_RESULT_WIDE_MISS = 10
};

enum { PERSISTENT_ZONE_ACTIVE = 0, PERSISTENT_ZONE_FOLLOW = 1 };

enum
{
    SCRIPT_PROJECTILE_PATH_DEFAULT = 0,
    SCRIPT_PROJECTILE_PATH_HOMING = 1,
    SCRIPT_PROJECTILE_PATH_BALLISTIC = 2,
    SCRIPT_PROJECTILE_PATH_HIGH_BALLISTIC = 3,
    SCRIPT_PROJECTILE_PATH_ACCELERATING = 4
};

enum
{
    DOOR_ACTION_OPEN = 0,
    DOOR_ACTION_UNLOCK = 1,
    DOOR_ACTION_BASH = 2,
    DOOR_ACTION_IGNORE = 3,
    DOOR_ACTION_KNOCK = 4
};

const int SCRIPT_EFFECT_DAMAGE_DEFAULT_TYPE = 3;
const int SCRIPT_MAX_DAMAGE_AMOUNT = 10000;

enum
{
    SHAPE_SPELLCYLINDER = 0,
    SHAPE_CONE = 1,
    SHAPE_CUBE = 2,
    SHAPE_SPELLCONE = 3,
    SHAPE_SPHERE = 4
};

enum
{
    OBJECT_TYPE_CREATURE = 1,
    OBJECT_TYPE_ITEM = 2,
    OBJECT_TYPE_TRIGGER = 4,
    OBJECT_TYPE_DOOR = 8,
    OBJECT_TYPE_AREA_OF_EFFECT = 16,
    OBJECT_TYPE_WAYPOINT = 32,
    OBJECT_TYPE_PLACEABLE = 64,
    OBJECT_TYPE_STORE = 128,
    OBJECT_TYPE_ENCOUNTER = 256,
    OBJECT_TYPE_SOUND = 512,
    OBJECT_TYPE_ALL = 32767,
    OBJECT_TYPE_INVALID = 32767
};

enum
{
    BASE_ITEM_STUN_BATON = 1,
    BASE_ITEM_LIGHTSABER = 8,
    BASE_ITEM_DOUBLE_BLADED_LIGHTSABER = 9,
    BASE_ITEM_SHORT_LIGHTSABER = 10,
    BASE_ITEM_BLASTER_CARBINE = 20
};

enum { DISGUISE_TYPE_P_MAL_A_MED_01 = 137 };

enum
{
    CLASS_TYPE_SOLDIER = 0,
    CLASS_TYPE_SCOUT = 1,
    CLASS_TYPE_SCOUNDREL = 2,
    CLASS_TYPE_JEDIGUARDIAN = 3,
    CLASS_TYPE_JEDICONSULAR = 4,
    CLASS_TYPE_JEDISENTINEL = 5,
    CLASS_TYPE_INVALID = 255
};

enum
{
    ACTION_MOVETOPOINT = 0,
    ACTION_PICKUPITEM = 1,
    ACTION_DROPITEM = 2,
    ACTION_ATTACKOBJECT = 3,
    ACTION_CASTSPELL = 4,
    ACTION_OPENDOOR = 5,
    ACTION_CLOSEDOOR = 6,
    ACTION_DIALOGOBJECT = 7,
    ACTION_DISABLETRAP = 8,
    ACTION_RECOVERTRAP = 9,
    ACTION_FLAGTRAP = 10,
    ACTION_EXAMINETRAP = 11,
    ACTION_SETTRAP = 12,
    ACTION_OPENLOCK = 13,
    ACTION_LOCK = 14,
    ACTION_USEOBJECT = 15,
    ACTION_ANIMALEMPATHY = 16,
    ACTION_REST = 17,
    ACTION_TAUNT = 18,
    ACTION_ITEMCASTSPELL = 19,
    ACTION_COUNTERSPELL = 31,
    ACTION_HEAL = 33,
    ACTION_PICKPOCKET = 34,
    ACTION_FOLLOW = 35,
    ACTION_WAIT = 36,
    ACTION_FOLLOWLEADER = 38,
    ACTION_QUEUEEMPTY = 65534,
    ACTION_INVALID = 65535
};

enum
{
    LIVE_CONTENT_PKG1 = 1,
    LIVE_CONTENT_PKG2 = 2,
    LIVE_CONTENT_PKG3 = 3,
    LIVE_CONTENT_PKG4 = 4,
    LIVE_CONTENT_PKG5 = 5,
    LIVE_CONTENT_PKG6 = 6
};

const int DURATION_TYPE_INSTANT = 0;
const int DURATION_TYPE_TEMPORARY = 1;
const int DURATION_TYPE_PERMANENT = 2;

const int SUBTYPE_MAGICAL = 8;
const int SUBTYPE_SUPERNATURAL = 16;
const int SUBTYPE_EXTRAORDINARY = 24;

enum { CONVERSATION_TYPE_CINEMATIC = 0, CONVERSATION_TYPE_COMPUTER = 1 };

enum
{
    VFX_COM_SPARKS_LARGE = 4003,
    VFX_COM_SPARKS_LIGHTSABER = 4004,
    VFX_COM_POWER_ATTACK_IMPROVED_STAFF = 4012,
    VFX_COM_POWER_BLAST_IMPROVED = 4013,
    VFX_COM_CRITICAL_STRIKE_IMPROVED_STAFF = 4014,
    VFX_COM_SNIPER_SHOT_IMPROVED = 4015,
    VFX_COM_MULTI_SHOT = 4016,
    VFX_COM_WHIRLWIND_STRIKE_STAFF = 4017,
    VFX_COM_CRITICAL_STRIKE_MASTERY_STAFF = 4018,
    VFX_COM_POWER_ATTACK_MASTERY_STAFF = 4019,
    VFX_COM_SNIPER_SHOT_MASTERY = 4020,
    VFX_COM_FLURRY_IMPROVED_STAFF = 4021,
    VFX_COM_RAPID_SHOT_IMPROVED = 4022,
    VFX_COM_BLASTER_IMPACT = 4024,
    VFX_COM_CRITICAL_STRIKE_IMPROVED_SABER = 4025,
    VFX_COM_CRITICAL_STRIKE_MASTERY_SABER = 4026,
    VFX_COM_POWER_ATTACK_IMPROVED_SABER = 4027,
    VFX_COM_POWER_ATTACK_MASTERY_SABER = 4028,
    VFX_COM_POWER_BLAST_MASTERY = 4029,
    VFX_COM_FLURRY_IMPROVED_SABER = 4030,
    VFX_COM_WHIRLWIND_STRIKE_SABER = 4031,

    VFX_COM_SNIPER_SHOT = 4038,
    VFX_ARD_LIGHT_YELLOW_10 = 5000,
    VFX_ARD_LIGHT_YELLOW_20 = 5001
};

enum { EFFECT_TYPE_BEAM = 21 };

class CRes;
class CResGFF;
class CResStruct;
class Vector;
#include "exobase.h"
#include "resgff.h"
#include "exoresman.h"
#include "math3d.h"

class CVirtualMachineInternal;
class CScriptCompilerInternal;
class CVirtualMachineScript;

class CVirtualMachineCmdImplementer
{
public:

    int m_nCommandContext;
    unsigned long m_oidObject;

    enum GameDefinedStructureType
    {
        STRUCTURE_EFFECT = 0,
        STRUCTURE_EVENT = 1,
        STRUCTURE_LOCATION = 2,
        STRUCTURE_TALENT = 3
    };

    CVirtualMachineCmdImplementer();
    virtual ~CVirtualMachineCmdImplementer() {};  // PC 0x004DF820
    virtual void InitializeCommands() = 0;
    virtual int ExecuteCommand(int nCommand, int nArgs) = 0;
    virtual void RunScriptCallback(CExoString& sScript) = 0;
    virtual void ReportError(CExoString& sError, int nError) = 0;
    virtual void* CreateGameDefinedStructure(int nType) = 0;
    virtual void DestroyGameDefinedStructure(int nType, void* pStructure) = 0;
    virtual int GetEqualGameDefinedStructure(int nType, void* pFirst,
                                        void* pSecond) = 0;
    virtual void* CopyGameDefinedStructure(int nType, void* pStructure) = 0;
    virtual int SaveGameDefinedStructure(int nType, void* pStructure,
                                    CResGFF* pGFF, CResStruct* pStruct) = 0;
    virtual int LoadGameDefinedStructure(int nType, void** ppStructure,
                                    CResGFF* pGFF, CResStruct* pStruct) = 0;
};

class CVirtualMachine
{
public:
    CVirtualMachine();
    ~CVirtualMachine();

    int RunScript(CExoString* psScript, unsigned long nObjectId,
                  int nInstructionLimit);
    int RunScriptSituation(void* pScript, unsigned long nObjectId,
                           int bOneInstruction);
    int GetRunScriptReturnValue(int* pnType, void** ppValue);
    void SetCommandImplementer(CVirtualMachineCmdImplementer* pImplementer);
    int StackPopInteger(int* pnValue);
    int StackPushInteger(int nValue);
    int StackPopFloat(float* pfValue);
    int StackPushFloat(float fValue);
    int StackPopVector(Vector* pvValue);
    int StackPushVector(Vector vValue);
    int StackPopString(CExoString* psValue);
    int StackPushString(const CExoString& sValue);
    int StackPopEngineStructure(int nType, void** ppValue);
    int StackPushEngineStructure(int nType, void* pValue);
    int StackPopObject(unsigned long* pnValue);
    int StackPushObject(unsigned long nValue);
    int StackPopCommand(void** ppCommand);
    int SaveScriptSituation(void* pScript, CResGFF* pGFF, CResStruct* pStruct);
    int LoadScriptSituation(void** ppScript, CResGFF* pGFF,
                            CResStruct* pStruct);
    void DeleteScriptSituation(void* pScript);

    CVirtualMachineInternal* m_pVirtualMachineInternal;
};

class CScriptCompiler
{
public:
    CScriptCompiler();
    ~CScriptCompiler();

    void SetCompileSymbolicOutput(int nValue);
    void SetCompileDebugLevel(int nValue);
    void SetIdentifierSpecification(const CExoString& sLanguageSource);
    void SetOutputAlias(const CExoString& sAlias);
    void SetOptimizeBinaryCodeLength(int bValue);
    void SetCompileConditionalFile(int bValue);
    int CompileFile(const CExoString& sFileName);
    int CompileScriptChunk(const CExoString& sScriptChunk);
    int CompileScriptConditional(const CExoString& sScriptConditional);
    int GetCompiledScriptCode(char** ppnCode, int* pnCodeSize);
    void ClearCompiledScriptCode();

    CScriptCompilerInternal* m_pScriptCompilerInternal;
};

extern CVirtualMachine* g_pVirtualMachine;
extern CScriptCompiler* g_pScriptCompiler;
extern int g_nRunScriptVar;

#endif
