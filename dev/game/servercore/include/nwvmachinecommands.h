#ifndef __NWVMACHINECOMMANDS_H__
#define __NWVMACHINECOMMANDS_H__

#include "nwscript.h"

class CResGFF;
class CResStruct;
class CExoString;

#define SWVM_DEGREES_TO_RADIANS (3.14159265358979323846 / 180.0)

#define SWVM_FULL_CIRCLE_DEGREES 360.0f

#define SWVM_CREATE_OBJECT_SEARCH_RADIUS 20.0f

#define SWVM_FACE_DIRECTION_EPSILON_SQUARED 0.0001f

#define SWVM_NEAREST_MINE_DISTANCE_SQUARED 100000000.0f

#define SWVM_SHAPE_DIRECTION_EPSILON_SQUARED 0.0001f
#define SWVM_SHAPE_CYLINDER_RADIUS 1.5f
#define SWVM_SHAPE_CYLINDER_RADIUS_SQUARED 2.25f
#define SWVM_SHAPE_SPELLCONE_COSINE 0.866f

class CSWVirtualMachineCommands : public CVirtualMachineCmdImplementer
{
public:

    static const int COMMAND_CONTEXT_SERVER = 1;
    static const int ERROR_STACK_PUSH = -2000;
    static const int ERROR_STACK_POP = -2001;

    static const int INVALID_TRAP_BASE_TYPE = -1;

    static const int EFFECT_MILLISECONDS_PER_SECOND = 1000;

    static const int MAX_SCRIPT_CLASS_TYPE = 32;

    static const int SCRIPT_ALIGNMENT_COUNT = 4;

    enum { PERCEPTION_REQUIREMENT_ANY = 2 };

    enum
    {
        COMMAND_ACTION_CAST_SPELL_AT_OBJECT = 48,
        COMMAND_ACTION_CAST_SPELL_AT_LOCATION = 234,
        COMMAND_ACTION_CAST_FAKE_SPELL_AT_OBJECT = 501,
        COMMAND_ACTION_CAST_FAKE_SPELL_AT_LOCATION = 502
    };

    enum
    {

        COMMAND_GET_IS_DOOR_ACTION_POSSIBLE = 337,
        COMMAND_TOUCH_ATTACK_MELEE = 146,
        COMMAND_TOUCH_ATTACK_RANGED = 147,
        COMMAND_FORTITUDE_SAVE = 108,
        COMMAND_REFLEX_SAVE = 109,
        COMMAND_WILL_SAVE = 110,

        COMMAND_EFFECT_ENTANGLE = 130,
        COMMAND_EFFECT_DEATH = 133,
        COMMAND_EFFECT_KNOCKDOWN = 134,
        COMMAND_EFFECT_PARALYZE = 148,
        COMMAND_EFFECT_SLEEP = 154,
        COMMAND_EFFECT_CONFUSED = 157,
        COMMAND_EFFECT_FRIGHTENED = 158,
        COMMAND_EFFECT_STUNNED = 161,
        COMMAND_EFFECT_REGENERATE = 164,
        COMMAND_EFFECT_ASSURED_HIT = 51,
        COMMAND_EFFECT_ASSURED_DEFLECTION = 252,
        COMMAND_EFFECT_HASTE = 270,
        COMMAND_EFFECT_DISPEL_MAGIC_ALL = 460,
        COMMAND_EFFECT_DISGUISE = 463,
        COMMAND_EFFECT_FORCE_RESISTED = 402,
        COMMAND_EFFECT_FORCE_FIZZLE = 420,
        COMMAND_EFFECT_TIME_STOP = 467,
        COMMAND_EFFECT_DISPEL_MAGIC_BEST = 473,
        COMMAND_EFFECT_CUTSCENE_HORRIFIED = 754,
        COMMAND_EFFECT_CUTSCENE_PARALYZE = 755,
        COMMAND_EFFECT_CUTSCENE_STUNNED = 756,

        COMMAND_D2 = 95,
        COMMAND_D3 = 96,
        COMMAND_D4 = 97,
        COMMAND_D6 = 98,
        COMMAND_D8 = 99,
        COMMAND_D10 = 100,
        COMMAND_D12 = 101,
        COMMAND_D20 = 102,
        COMMAND_D100 = 103,

        COMMAND_DO_DOOR_ACTION = 338,

        COMMAND_GET_FIRST_ITEM_IN_INVENTORY = 339,
        COMMAND_GET_NEXT_ITEM_IN_INVENTORY = 340,

        COMMAND_GET_LAST_PERCEPTION_HEARD = 257,
        COMMAND_GET_LAST_PERCEPTION_INAUDIBLE = 258,
        COMMAND_GET_LAST_PERCEPTION_SEEN = 259,
        COMMAND_GET_LAST_PERCEPTION_VANISHED = 261,

        COMMAND_SET_FACING = 10,
        COMMAND_SET_FACING_POINT = 143
    };

    enum
    {
        COMMAND_POP_UP_GUI_PANEL = 388,

        COMMAND_VERSUS_ALIGNMENT_EFFECT = 355,
        COMMAND_VERSUS_RACIAL_TYPE_EFFECT = 356,

        COMMAND_CHANGE_FACTION = 173,
        COMMAND_CHANGE_TO_STANDARD_FACTION = 412,
        COMMAND_POP_UP_DEATH_GUI_PANEL = 554
    };

    enum
    {

        COMMAND_GET_TYPE_FROM_TALENT = 362
    };

    enum
    {
        COMMAND_GET_ITEM_ACTIVATED = 439,
        COMMAND_GET_ITEM_ACTIVATOR = 440,
        COMMAND_GET_ITEM_ACTIVATED_TARGET_LOCATION = 441,
        COMMAND_GET_ITEM_ACTIVATED_TARGET = 442
    };

    enum
    {
        COMMAND_GET_LAST_PLAYER_DIED = 291,
        COMMAND_GET_LAST_PLAYER_DYING = 410,
        COMMAND_GET_STARTING_LOCATION = 411,
        COMMAND_GET_LAST_RESPAWN_BUTTON_PRESSER = 419,
        COMMAND_GET_LAST_ITEM_EQUIPPED = 52,

        COMMAND_GET_CLASS_BY_POSITION = 341,
        COMMAND_GET_LEVEL_BY_POSITION = 342,

        COMMAND_GET_CURRENT_HIT_POINTS = 49,
        COMMAND_GET_LEVEL_BY_CLASS = 343,

        COMMAND_GET_MODULE_FILE_NAME = 210,
        COMMAND_GET_MODULE_NAME = 561,

        COMMAND_GET_FIRST_OBJECT_IN_AREA = 93,

        COMMAND_GET_FIRST_IN_PERSISTENT_OBJECT = 262,

        COMMAND_GET_FIRST_OBJECT_IN_SHAPE = 128,

        COMMAND_GET_FIRST_EFFECT = 85,

        COMMAND_GET_OBJECT_SEEN = 289,
        COMMAND_GET_OBJECT_HEARD = 290,

        COMMAND_GET_CREATURE_TALENT_RANDOM = 307,
        COMMAND_GET_CREATURE_TALENT_BEST = 308,
        COMMAND_ACTION_USE_TALENT_ON_OBJECT = 309,
        COMMAND_ACTION_USE_TALENT_AT_LOCATION = 310,
        COMMAND_TALENT_SPELL = 301,
        COMMAND_TALENT_FEAT = 302,
        COMMAND_TALENT_SKILL = 303,
        COMMAND_GET_ID_FROM_TALENT = 363,
        COMMAND_GET_CATEGORY_FROM_TALENT = 735,
        COMMAND_FABS = 67,
        COMMAND_COS = 68,
        COMMAND_SIN = 69,
        COMMAND_TAN = 70,
        COMMAND_ACOS = 71,
        COMMAND_ASIN = 72,
        COMMAND_ATAN = 73,
        COMMAND_LOG = 74,
        COMMAND_POW = 75,
        COMMAND_SQRT = 76,
        COMMAND_ABS = 77,

        COMMAND_MAGICAL_EFFECT = 112,
        COMMAND_SUPERNATURAL_EFFECT = 113,
        COMMAND_EXTRAORDINARY_EFFECT = 114,

        COMMAND_ROUNDS_TO_SECONDS = 121,
        COMMAND_HOURS_TO_SECONDS = 122,
        COMMAND_TURNS_TO_SECONDS = 123,

        COMMAND_STRING_TO_INT = 232,

        COMMAND_ANGLE_TO_VECTOR = 144,
        COMMAND_VECTOR_TO_ANGLE = 145
    };

    enum
    {
        COMMAND_GET_GOOD_EVIL_VALUE = 125,
        COMMAND_GET_ALIGNMENT_GOOD_EVIL = 127,

        COMMAND_ACTION_JUMP_TO_OBJECT = 196,

        COMMAND_ACTION_JUMP_TO_POINT = 214
    };

    enum
    {
        COMMAND_ACTION_MOVE_TO_LOCATION = 21,
        COMMAND_ACTION_FORCE_MOVE_TO_LOCATION = 382,

        COMMAND_ACTION_MOVE_TO_OBJECT = 22,
        COMMAND_ACTION_FORCE_MOVE_TO_OBJECT = 383
    };

    enum
    {
        COMMAND_GET_POSITION_FROM_LOCATION = 223,
        COMMAND_GET_FACING_FROM_LOCATION = 225
    };

    enum
    {
        COMMAND_GET_ENCOUNTER_ACTIVE = 276,
        COMMAND_GET_ENCOUNTER_SPAWNS_MAX = 278,
        COMMAND_GET_ENCOUNTER_SPAWNS_CURRENT = 280,
        COMMAND_GET_ENCOUNTER_DIFFICULTY = 297
    };

    enum
    {
        COMMAND_ACTION_SURRENDER_TO_ENEMIES = 379
    };

    enum
    {
        COMMAND_GET_FIRST_FACTION_MEMBER = 380,
        COMMAND_GET_NEXT_FACTION_MEMBER = 381,
        COMMAND_SURRENDER_TO_ENEMIES = 476,
        COMMAND_SURRENDER_RETAIN_BUFFS = 762,

        COMMAND_SET_GLOBAL_BOOLEAN = 579,
        COMMAND_SET_GLOBAL_NUMBER = 581,

        COMMAND_GET_GLOBAL_BOOLEAN = 578,
        COMMAND_GET_GLOBAL_NUMBER = 580,
        COMMAND_GET_LOCAL_BOOLEAN = 679,

        COMMAND_SET_LOCAL_BOOLEAN = 680,
        COMMAND_GET_LOCAL_NUMBER = 681,
        COMMAND_SET_LOCAL_NUMBER = 682,
        COMMAND_IS_AVAILABLE_NPC = 696,
        COMMAND_GET_NPC_SELECTABILITY = 709
    };

    enum
    {

        COMMAND_GET_IS_ENEMY = 235,
        COMMAND_GET_IS_FRIEND = 236,
        COMMAND_GET_IS_NEUTRAL = 237,
        COMMAND_GET_FIRST_ATTACKER = 727,
        COMMAND_GET_NEXT_ATTACKER = 728,

        COMMAND_CHANGE_BY_FACTION_AND_PACIFY = 736,

        COMMAND_GET_LOCK_KEY_REQUIRED = 537,
        COMMAND_GET_LOCK_KEY_TAG = 538,
        COMMAND_GET_LOCK_LOCKABLE = 539,
        COMMAND_GET_LOCK_UNLOCK_DC = 540,
        COMMAND_GET_LOCK_LOCK_DC = 541,

        COMMAND_SPEAK_STRING = 221,

        COMMAND_FLOATING_TEXT_STR_REF_ON_CREATURE = 525,
        COMMAND_FLOATING_TEXT_STRING_ON_CREATURE = 526,

        COMMAND_GET_TRAP_DISARMABLE = 527,
        COMMAND_GET_TRAP_DETECTABLE = 528,
        COMMAND_GET_TRAP_DETECTED_BY = 529,
        COMMAND_GET_TRAP_FLAGGED = 530,
        COMMAND_GET_TRAP_BASE_TYPE = 531,
        COMMAND_GET_TRAP_ONE_SHOT = 532,
        COMMAND_GET_TRAP_CREATOR = 533,
        COMMAND_GET_TRAP_KEY_TAG = 534,
        COMMAND_GET_TRAP_DISARM_DC = 535,
        COMMAND_GET_TRAP_DETECT_DC = 536,
        COMMAND_SET_TRAP_DETECTED_BY = 550,
        COMMAND_SET_TRAP_DISABLED = 555
    };

    enum ScriptCommand
    {
        COMMAND_RANDOM = 0,
        COMMAND_PRINT_FLOAT = 2,
        COMMAND_FLOAT_TO_STRING = 3,
        COMMAND_PRINT_INTEGER = 4,
        COMMAND_PRINT_OBJECT = 5,
        COMMAND_ASSIGN_COMMAND = 6,
        COMMAND_DELAY_COMMAND = 7,
        COMMAND_EXECUTE_SCRIPT = 8,
        COMMAND_CLEAR_ALL_ACTIONS = 9,
        COMMAND_SWITCH_PLAYER_CREATURE = 11,
        COMMAND_SET_TIME = 12,
        COMMAND_SET_PARTY_LEADER = 13,
        COMMAND_SET_AREA_UNESCAPABLE = 14,
        COMMAND_GET_AREA_UNESCAPABLE = 15,
        COMMAND_GET_TIME_HOUR = 16,
        COMMAND_GET_TIME_MINUTE = 17,
        COMMAND_GET_TIME_SECOND = 18,
        COMMAND_GET_TIME_MILLISECOND = 19,
        COMMAND_ACTION_RANDOM_WALK = 20,
        COMMAND_ACTION_MOVE_AWAY_FROM_OBJECT = 23,
        COMMAND_GET_AREA = 24,
        COMMAND_GET_ENTERING_OBJECT = 25,
        COMMAND_GET_EXITING_OBJECT = 26,
        COMMAND_GET_POSITION = 27,
        COMMAND_GET_FACING = 28,
        COMMAND_GET_ITEM_POSSESSOR = 29,
        COMMAND_GET_ITEM_POSSESSED_BY = 30,
        COMMAND_CREATE_ITEM_ON_OBJECT = 31,
        COMMAND_ACTION_EQUIP_ITEM = 32,
        COMMAND_ACTION_UNEQUIP_ITEM = 33,
        COMMAND_ACTION_PICK_UP_ITEM = 34,
        COMMAND_ACTION_PUT_DOWN_ITEM = 35,
        COMMAND_GET_LAST_ATTACKER = 36,
        COMMAND_ACTION_ATTACK = 37,
        COMMAND_GET_NEAREST_CREATURE = 38,
        COMMAND_ACTION_SPEAK_STRING = 39,
        COMMAND_ACTION_PLAY_ANIMATION = 40,
        COMMAND_GET_DISTANCE_TO_OBJECT = 41,
        COMMAND_GET_IS_OBJECT_VALID = 42,
        COMMAND_ACTION_OPEN_DOOR = 43,
        COMMAND_ACTION_CLOSE_DOOR = 44,
        COMMAND_SET_CAMERA_FACING = 45,
        COMMAND_PLAY_SOUND = 46,
        COMMAND_GET_SPELL_TARGET_OBJECT = 47,
        COMMAND_GET_MAX_HIT_POINTS = 50,
        COMMAND_GET_SUB_SCREEN_ID = 53,
        COMMAND_CANCEL_COMBAT = 54,
        COMMAND_GET_CURRENT_FORCE_POINTS = 55,
        COMMAND_GET_MAX_FORCE_POINTS = 56,
        COMMAND_PAUSE_GAME = 57,
        COMMAND_SET_PLAYER_RESTRICT_MODE = 58,
        COMMAND_GET_STRING_LENGTH = 59,
        COMMAND_GET_STRING_UPPER_CASE = 60,
        COMMAND_GET_STRING_LOWER_CASE = 61,
        COMMAND_GET_STRING_RIGHT = 62,
        COMMAND_GET_STRING_LEFT = 63,
        COMMAND_INSERT_STRING = 64,
        COMMAND_GET_SUB_STRING = 65,
        COMMAND_FIND_SUB_STRING = 66,
        COMMAND_EFFECT_HEAL = 78,
        COMMAND_EFFECT_DAMAGE = 79,
        COMMAND_EFFECT_ABILITY_INCREASE = 80,
        COMMAND_EFFECT_DAMAGE_RESISTANCE = 81,
        COMMAND_EFFECT_RESURRECTION = 82,
        COMMAND_GET_PLAYER_RESTRICT_MODE = 83,
        COMMAND_GET_CASTER_LEVEL = 84,
        COMMAND_GET_NEXT_EFFECT = 86,
        COMMAND_REMOVE_EFFECT = 87,
        COMMAND_GET_IS_EFFECT_VALID = 88,
        COMMAND_GET_EFFECT_DURATION_TYPE = 89,
        COMMAND_GET_EFFECT_SUB_TYPE = 90,
        COMMAND_GET_EFFECT_CREATOR = 91,
        COMMAND_INT_TO_STRING = 92,
        COMMAND_GET_NEXT_OBJECT_IN_AREA = 94,
        COMMAND_VECTOR_MAGNITUDE = 104,
        COMMAND_GET_META_MAGIC_FEAT = 105,
        COMMAND_GET_OBJECT_TYPE = 106,
        COMMAND_GET_RACIAL_TYPE = 107,
        COMMAND_GET_SPELL_SAVE_DC = 111,
        COMMAND_EFFECT_AC_INCREASE = 115,
        COMMAND_GET_AC = 116,
        COMMAND_EFFECT_SAVING_THROW_INCREASE = 117,
        COMMAND_EFFECT_ATTACK_INCREASE = 118,
        COMMAND_EFFECT_DAMAGE_REDUCTION = 119,
        COMMAND_EFFECT_DAMAGE_INCREASE = 120,
        COMMAND_SOUND_OBJECT_SET_FIXED_VARIANCE = 124,
        COMMAND_GET_PARTY_MEMBER_COUNT = 126,
        COMMAND_GET_NEXT_OBJECT_IN_SHAPE = 129,
        COMMAND_SIGNAL_EVENT = 131,
        COMMAND_EVENT_USER_DEFINED = 132,
        COMMAND_ACTION_GIVE_ITEM = 135,
        COMMAND_ACTION_TAKE_ITEM = 136,
        COMMAND_VECTOR_NORMALIZE = 137,
        COMMAND_GET_ITEM_STACK_SIZE = 138,
        COMMAND_GET_ABILITY_SCORE = 139,
        COMMAND_GET_IS_DEAD = 140,
        COMMAND_PRINT_VECTOR = 141,
        COMMAND_VECTOR = 142,
        COMMAND_EFFECT_SPELL_IMMUNITY = 149,
        COMMAND_SET_ITEM_STACK_SIZE = 150,
        COMMAND_GET_DISTANCE_BETWEEN = 151,
        COMMAND_SET_RETURN_STRREF = 152,
        COMMAND_EFFECT_FORCE_JUMP = 153,
        COMMAND_GET_ITEM_IN_SLOT = 155,
        COMMAND_EFFECT_TEMPORARY_FORCE_POINTS = 156,
        COMMAND_EFFECT_CHOKE = 159,
        COMMAND_SET_GLOBAL_STRING = 160,
        COMMAND_SET_COMMANDABLE = 162,
        COMMAND_GET_COMMANDABLE = 163,
        COMMAND_EFFECT_MOVEMENT_SPEED_INCREASE = 165,
        COMMAND_GET_HIT_DICE = 166,
        COMMAND_ACTION_FORCE_FOLLOW_OBJECT = 167,
        COMMAND_GET_TAG = 168,
        COMMAND_RESIST_SPELL = 169,
        COMMAND_GET_EFFECT_TYPE = 170,
        COMMAND_EFFECT_AREA_OF_EFFECT = 171,
        COMMAND_GET_FACTION_EQUAL = 172,
        COMMAND_GET_IS_LISTENING = 174,
        COMMAND_SET_LISTENING = 175,
        COMMAND_SET_LISTEN_PATTERN = 176,
        COMMAND_TEST_STRING_AGAINST_PATTERN = 177,
        COMMAND_GET_MATCHED_SUBSTRING = 178,
        COMMAND_GET_MATCHED_SUBSTRINGS_COUNT = 179,
        COMMAND_EFFECT_VISUAL_EFFECT = 180,
        COMMAND_GET_FACTION_WEAKEST_MEMBER = 181,
        COMMAND_GET_FACTION_STRONGEST_MEMBER = 182,
        COMMAND_GET_FACTION_MOST_DAMAGED_MEMBER = 183,
        COMMAND_GET_FACTION_LEAST_DAMAGED_MEMBER = 184,
        COMMAND_GET_FACTION_GOLD = 185,
        COMMAND_GET_FACTION_AVERAGE_REPUTATION = 186,
        COMMAND_GET_FACTION_AVERAGE_GOOD_EVIL_ALIGNMENT = 187,
        COMMAND_SOUND_OBJECT_GET_FIXED_VARIANCE = 188,
        COMMAND_GET_FACTION_AVERAGE_LEVEL = 189,
        COMMAND_GET_FACTION_AVERAGE_XP = 190,
        COMMAND_GET_FACTION_MOST_FREQUENT_CLASS = 191,
        COMMAND_GET_FACTION_WORST_AC = 192,
        COMMAND_GET_FACTION_BEST_AC = 193,
        COMMAND_GET_GLOBAL_STRING = 194,
        COMMAND_GET_LISTEN_PATTERN_NUMBER = 195,
        COMMAND_GET_WAYPOINT_BY_TAG = 197,
        COMMAND_GET_TRANSITION_TARGET = 198,
        COMMAND_EFFECT_LINK_EFFECTS = 199,
        COMMAND_GET_OBJECT_BY_TAG = 200,
        COMMAND_ADJUST_ALIGNMENT = 201,
        COMMAND_ACTION_WAIT = 202,
        COMMAND_SET_AREA_TRANSITION_BMP = 203,
        COMMAND_ACTION_START_CONVERSATION = 204,
        COMMAND_ACTION_PAUSE_CONVERSATION = 205,
        COMMAND_ACTION_RESUME_CONVERSATION = 206,
        COMMAND_EFFECT_BEAM = 207,
        COMMAND_GET_REPUTATION = 208,
        COMMAND_ADJUST_REPUTATION = 209,
        COMMAND_GET_GOING_TO_BE_ATTACKED_BY = 211,
        COMMAND_EFFECT_FORCE_RESISTANCE_INCREASE = 212,
        COMMAND_GET_LOCATION = 213,
        COMMAND_LOCATION = 215,
        COMMAND_APPLY_EFFECT_AT_LOCATION = 216,
        COMMAND_GET_IS_PC = 217,
        COMMAND_FEET_TO_METERS = 218,
        COMMAND_YARDS_TO_METERS = 219,
        COMMAND_APPLY_EFFECT_TO_OBJECT = 220,
        COMMAND_GET_SPELL_TARGET_LOCATION = 222,
        COMMAND_EFFECT_BODY_FUEL = 224,
        COMMAND_GET_NEAREST_CREATURE_TO_LOCATION = 226,
        COMMAND_GET_NEAREST_OBJECT = 227,
        COMMAND_GET_NEAREST_OBJECT_TO_LOCATION = 228,
        COMMAND_GET_NEAREST_OBJECT_BY_TAG = 229,
        COMMAND_INT_TO_FLOAT = 230,
        COMMAND_FLOAT_TO_INT = 231,
        COMMAND_STRING_TO_FLOAT = 233,
        COMMAND_GET_PC_SPEAKER = 238,
        COMMAND_GET_STRING_BY_STR_REF = 239,
        COMMAND_ACTION_SPEAK_STRING_BY_STR_REF = 240,
        COMMAND_DESTROY_OBJECT = 241,
        COMMAND_GET_MODULE = 242,
        COMMAND_CREATE_OBJECT = 243,
        COMMAND_EVENT_SPELL_CAST_AT = 244,
        COMMAND_GET_LAST_SPELL_CASTER = 245,
        COMMAND_GET_LAST_SPELL = 246,
        COMMAND_GET_USER_DEFINED_EVENT_NUMBER = 247,
        COMMAND_GET_SPELL_ID = 248,
        COMMAND_RANDOM_NAME = 249,
        COMMAND_EFFECT_POISON = 250,
        COMMAND_GET_LOAD_FROM_SAVE_GAME = 251,
        COMMAND_GET_NAME = 253,
        COMMAND_GET_LAST_SPEAKER = 254,
        COMMAND_BEGIN_CONVERSATION = 255,
        COMMAND_GET_LAST_PERCEIVED = 256,
        COMMAND_GET_LAST_CLOSED_BY = 260,
        COMMAND_GET_NEXT_IN_PERSISTENT_OBJECT = 263,
        COMMAND_GET_AREA_OF_EFFECT_CREATOR = 264,
        COMMAND_SHOW_LEVEL_UP_GUI = 265,
        COMMAND_SET_ITEM_NON_EQUIPPABLE = 266,
        COMMAND_GET_BUTTON_MASH_CHECK = 267,
        COMMAND_SET_BUTTON_MASH_CHECK = 268,
        COMMAND_EFFECT_FORCE_PUSH_TARGETED = 269,
        COMMAND_GIVE_ITEM = 271,
        COMMAND_OBJECT_TO_STRING = 272,
        COMMAND_EFFECT_IMMUNITY = 273,
        COMMAND_GET_IS_IMMUNE = 274,
        COMMAND_EFFECT_DAMAGE_IMMUNITY_INCREASE = 275,
        COMMAND_SET_ENCOUNTER_ACTIVE = 277,
        COMMAND_SET_ENCOUNTER_SPAWNS_MAX = 279,
        COMMAND_SET_ENCOUNTER_SPAWNS_CURRENT = 281,
        COMMAND_GET_MODULE_ITEM_ACQUIRED = 282,
        COMMAND_GET_MODULE_ITEM_ACQUIRED_FROM = 283,
        COMMAND_SET_CUSTOM_TOKEN = 284,
        COMMAND_GET_HAS_FEAT = 285,
        COMMAND_GET_HAS_SKILL = 286,
        COMMAND_ACTION_USE_FEAT = 287,
        COMMAND_ACTION_USE_SKILL = 288,
        COMMAND_GET_MODULE_ITEM_LOST = 292,
        COMMAND_GET_MODULE_ITEM_LOST_BY = 293,
        COMMAND_ACTION_DO_COMMAND = 294,
        COMMAND_EVENT_CONVERSATION = 295,
        COMMAND_SET_ENCOUNTER_DIFFICULTY = 296,
        COMMAND_GET_DISTANCE_BETWEEN_LOCATIONS = 298,
        COMMAND_GET_REFLEX_ADJUSTED_DAMAGE = 299,
        COMMAND_PLAY_ANIMATION = 300,
        COMMAND_GET_HAS_SPELL_EFFECT = 304,
        COMMAND_GET_EFFECT_SPELL_ID = 305,
        COMMAND_GET_CREATURE_HAS_TALENT = 306,
        COMMAND_GET_GOLD_PIECE_VALUE = 311,
        COMMAND_GET_IS_PLAYABLE_RACIAL_TYPE = 312,
        COMMAND_JUMP_TO_LOCATION = 313,
        COMMAND_EFFECT_TEMPORARY_HITPOINTS = 314,
        COMMAND_GET_SKILL_RANK = 315,
        COMMAND_GET_ATTACK_TARGET = 316,
        COMMAND_GET_LAST_ATTACK_TYPE = 317,
        COMMAND_GET_LAST_ATTACK_MODE = 318,
        COMMAND_GET_DISTANCE_BETWEEN2_D = 319,
        COMMAND_GET_IS_IN_COMBAT = 320,
        COMMAND_GET_LAST_ASSOCIATE_COMMAND = 321,
        COMMAND_GIVE_GOLD_TO_CREATURE = 322,
        COMMAND_SET_IS_DESTROYABLE = 323,
        COMMAND_SET_LOCKED = 324,
        COMMAND_GET_LOCKED = 325,
        COMMAND_GET_CLICKING_OBJECT = 326,
        COMMAND_SET_ASSOCIATE_LISTEN_PATTERNS = 327,
        COMMAND_GET_LAST_WEAPON_USED = 328,
        COMMAND_ACTION_INTERACT_OBJECT = 329,
        COMMAND_GET_LAST_USED_BY = 330,
        COMMAND_GET_ABILITY_MODIFIER = 331,
        COMMAND_GET_IDENTIFIED = 332,
        COMMAND_SET_IDENTIFIED = 333,
        COMMAND_GET_DISTANCE_BETWEEN_LOCATIONS2_D = 334,
        COMMAND_GET_DISTANCE_TO_OBJECT2_D = 335,
        COMMAND_GET_BLOCKING_DOOR = 336,
        COMMAND_GET_DAMAGE_DEALT_BY_TYPE = 344,
        COMMAND_GET_TOTAL_DAMAGE_DEALT = 345,
        COMMAND_GET_LAST_DAMAGER = 346,
        COMMAND_GET_LAST_DISARMED = 347,
        COMMAND_GET_LAST_DISTURBED = 348,
        COMMAND_GET_LAST_LOCKED = 349,
        COMMAND_GET_LAST_UNLOCKED = 350,
        COMMAND_EFFECT_SKILL_INCREASE = 351,
        COMMAND_GET_INVENTORY_DISTURB_TYPE = 352,
        COMMAND_GET_INVENTORY_DISTURB_ITEM = 353,
        COMMAND_SHOW_UPGRADE_SCREEN = 354,
        COMMAND_VERSUS_TRAP_EFFECT = 357,
        COMMAND_GET_GENDER = 358,
        COMMAND_GET_IS_TALENT_VALID = 359,
        COMMAND_ACTION_MOVE_AWAY_FROM_LOCATION = 360,
        COMMAND_GET_ATTEMPTED_ATTACK_TARGET = 361,
        COMMAND_PLAY_PAZAAK = 364,
        COMMAND_GET_LAST_PAZAAK_RESULT = 365,
        COMMAND_DISPLAY_FEED_BACK_TEXT = 366,
        COMMAND_ADD_JOURNAL_QUEST_ENTRY = 367,
        COMMAND_REMOVE_JOURNAL_QUEST_ENTRY = 368,
        COMMAND_GET_JOURNAL_ENTRY = 369,
        COMMAND_PLAY_RUMBLE_PATTERN = 370,
        COMMAND_STOP_RUMBLE_PATTERN = 371,
        COMMAND_EFFECT_DAMAGE_FORCE_POINTS = 372,
        COMMAND_EFFECT_HEAL_FORCE_POINTS = 373,
        COMMAND_SEND_MESSAGE_TO_PC = 374,
        COMMAND_GET_ATTEMPTED_SPELL_TARGET = 375,
        COMMAND_GET_LAST_OPENED_BY = 376,
        COMMAND_GET_HAS_SPELL = 377,
        COMMAND_OPEN_STORE = 378,
        COMMAND_GET_JOURNAL_QUEST_EXPERIENCE = 384,
        COMMAND_JUMP_TO_OBJECT = 385,
        COMMAND_SET_MAP_PIN_ENABLED = 386,
        COMMAND_EFFECT_HIT_POINT_CHANGE_WHEN_DYING = 387,
        COMMAND_ADD_MULTI_CLASS = 389,
        COMMAND_GET_IS_LINK_IMMUNE = 390,
        COMMAND_EFFECT_DROID_STUN = 391,
        COMMAND_EFFECT_FORCE_PUSHED = 392,
        COMMAND_GIVE_XP_TO_CREATURE = 393,
        COMMAND_SET_XP = 394,
        COMMAND_GET_XP = 395,
        COMMAND_INT_TO_HEX_STRING = 396,
        COMMAND_GET_BASE_ITEM_TYPE = 397,
        COMMAND_GET_ITEM_HAS_ITEM_PROPERTY = 398,
        COMMAND_ACTION_EQUIP_MOST_DAMAGING_MELEE = 399,
        COMMAND_ACTION_EQUIP_MOST_DAMAGING_RANGED = 400,
        COMMAND_GET_ITEM_AC_VALUE = 401,
        COMMAND_EXPLORE_AREA_FOR_PLAYER = 403,
        COMMAND_ACTION_EQUIP_MOST_EFFECTIVE_ARMOR = 404,
        COMMAND_GET_IS_DAY = 405,
        COMMAND_GET_IS_NIGHT = 406,
        COMMAND_GET_IS_DAWN = 407,
        COMMAND_GET_IS_DUSK = 408,
        COMMAND_GET_IS_ENCOUNTER_CREATURE = 409,
        COMMAND_SOUND_OBJECT_PLAY = 413,
        COMMAND_SOUND_OBJECT_STOP = 414,
        COMMAND_SOUND_OBJECT_SET_VOLUME = 415,
        COMMAND_SOUND_OBJECT_SET_POSITION = 416,
        COMMAND_SPEAK_ONE_LINER_CONVERSATION = 417,
        COMMAND_GET_GOLD = 418,
        COMMAND_SET_LIGHTSABER_POWERED = 421,
        COMMAND_GET_IS_WEAPON_EFFECTIVE = 422,
        COMMAND_GET_LAST_SPELL_HARMFUL = 423,
        COMMAND_EVENT_ACTIVATE_ITEM = 424,
        COMMAND_MUSIC_BACKGROUND_PLAY = 425,
        COMMAND_MUSIC_BACKGROUND_STOP = 426,
        COMMAND_MUSIC_BACKGROUND_SET_DELAY = 427,
        COMMAND_MUSIC_BACKGROUND_CHANGE_DAY = 428,
        COMMAND_MUSIC_BACKGROUND_CHANGE_NIGHT = 429,
        COMMAND_MUSIC_BATTLE_PLAY = 430,
        COMMAND_MUSIC_BATTLE_STOP = 431,
        COMMAND_MUSIC_BATTLE_CHANGE = 432,
        COMMAND_AMBIENT_SOUND_PLAY = 433,
        COMMAND_AMBIENT_SOUND_STOP = 434,
        COMMAND_AMBIENT_SOUND_CHANGE_DAY = 435,
        COMMAND_AMBIENT_SOUND_CHANGE_NIGHT = 436,
        COMMAND_GET_LAST_KILLER = 437,
        COMMAND_GET_SPELL_CAST_ITEM = 438,
        COMMAND_GET_IS_OPEN = 443,
        COMMAND_TAKE_GOLD_FROM_CREATURE = 444,
        COMMAND_GET_IS_IN_CONVERSATION = 445,
        COMMAND_EFFECT_ABILITY_DECREASE = 446,
        COMMAND_EFFECT_ATTACK_DECREASE = 447,
        COMMAND_EFFECT_DAMAGE_DECREASE = 448,
        COMMAND_EFFECT_DAMAGE_IMMUNITY_DECREASE = 449,
        COMMAND_EFFECT_AC_DECREASE = 450,
        COMMAND_EFFECT_MOVEMENT_SPEED_DECREASE = 451,
        COMMAND_EFFECT_SAVING_THROW_DECREASE = 452,
        COMMAND_EFFECT_SKILL_DECREASE = 453,
        COMMAND_EFFECT_FORCE_RESISTANCE_DECREASE = 454,
        COMMAND_GET_PLOT_FLAG = 455,
        COMMAND_SET_PLOT_FLAG = 456,
        COMMAND_EFFECT_INVISIBILITY = 457,
        COMMAND_EFFECT_CONCEALMENT = 458,
        COMMAND_EFFECT_FORCE_SHIELD = 459,
        COMMAND_SET_DIALOG_PLACEABLE_CAMERA = 461,
        COMMAND_GET_SOLO_MODE = 462,
        COMMAND_GET_MAX_STEALTH_XP = 464,
        COMMAND_EFFECT_TRUE_SEEING = 465,
        COMMAND_EFFECT_SEE_INVISIBLE = 466,
        COMMAND_SET_MAX_STEALTH_XP = 468,
        COMMAND_EFFECT_BLASTER_DEFLECTION_INCREASE = 469,
        COMMAND_EFFECT_BLASTER_DEFLECTION_DECREASE = 470,
        COMMAND_EFFECT_HORRIFIED = 471,
        COMMAND_EFFECT_SPELL_LEVEL_ABSORPTION = 472,
        COMMAND_GET_CURRENT_STEALTH_XP = 474,
        COMMAND_GET_NUM_STACKED_ITEMS = 475,
        COMMAND_EFFECT_MISS_CHANCE = 477,
        COMMAND_SET_CURRENT_STEALTH_XP = 478,
        COMMAND_GET_CREATURE_SIZE = 479,
        COMMAND_AWARD_STEALTH_XP = 480,
        COMMAND_GET_STEALTH_XP_ENABLED = 481,
        COMMAND_SET_STEALTH_XP_ENABLED = 482,
        COMMAND_ACTION_UNLOCK_OBJECT = 483,
        COMMAND_ACTION_LOCK_OBJECT = 484,
        COMMAND_EFFECT_MODIFY_ATTACKS = 485,
        COMMAND_GET_LAST_TRAP_DETECTED = 486,
        COMMAND_EFFECT_DAMAGE_SHIELD = 487,
        COMMAND_GET_NEAREST_TRAP_TO_OBJECT = 488,
        COMMAND_GET_ATTEMPTED_MOVEMENT_TARGET = 489,
        COMMAND_GET_BLOCKING_CREATURE = 490,
        COMMAND_GET_FORTITUDE_SAVING_THROW = 491,
        COMMAND_GET_WILL_SAVING_THROW = 492,
        COMMAND_GET_REFLEX_SAVING_THROW = 493,
        COMMAND_GET_CHALLENGE_RATING = 494,
        COMMAND_GET_FOUND_ENEMY_CREATURE = 495,
        COMMAND_GET_MOVEMENT_RATE = 496,
        COMMAND_GET_SUB_RACE = 497,
        COMMAND_GET_STEALTH_XP_DECREMENT = 498,
        COMMAND_SET_STEALTH_XP_DECREMENT = 499,
        COMMAND_DUPLICATE_HEAD_APPEARANCE = 500,
        COMMAND_CUTSCENE_ATTACK = 503,
        COMMAND_SET_CAMERA_MODE = 504,
        COMMAND_SET_LOCK_ORIENTATION_IN_DIALOG = 505,
        COMMAND_SET_LOCK_HEAD_FOLLOW_IN_DIALOG = 506,
        COMMAND_CUTSCENE_MOVE = 507,
        COMMAND_ENABLE_VIDEO_EFFECT = 508,
        COMMAND_START_NEW_MODULE = 509,
        COMMAND_DISABLE_VIDEO_EFFECT = 510,
        COMMAND_GET_WEAPON_RANGED = 511,
        COMMAND_DO_SINGLE_PLAYER_AUTO_SAVE = 512,
        COMMAND_GET_GAME_DIFFICULTY = 513,
        COMMAND_GET_USER_ACTIONS_PENDING = 514,
        COMMAND_REVEAL_MAP = 515,
        COMMAND_SET_TUTORIAL_WINDOWS_ENABLED = 516,
        COMMAND_SHOW_TUTORIAL_WINDOW = 517,
        COMMAND_START_CREDIT_SEQUENCE = 518,
        COMMAND_IS_CREDIT_SEQUENCE_IN_PROGRESS = 519,
        COMMAND_GET_CURRENT_ACTION = 522,
        COMMAND_GET_DIFFICULTY_MODIFIER = 523,
        COMMAND_GET_APPEARANCE_TYPE = 524,
        COMMAND_GET_PC_LEVELLING_UP = 542,
        COMMAND_GET_HAS_FEAT_EFFECT = 543,
        COMMAND_SET_PLACEABLE_ILLUMINATION = 544,
        COMMAND_GET_PLACEABLE_ILLUMINATION = 545,
        COMMAND_GET_IS_PLACEABLE_OBJECT_ACTION_POSSIBLE = 546,
        COMMAND_DO_PLACEABLE_OBJECT_ACTION = 547,
        COMMAND_GET_FIRST_PC = 548,
        COMMAND_GET_NEXT_PC = 549,
        COMMAND_GET_IS_TRAPPED = 551,
        COMMAND_SET_EFFECT_ICON = 552,
        COMMAND_FACE_OBJECT_AWAY_FROM_OBJECT = 553,
        COMMAND_GET_LAST_HOSTILE_ACTOR = 556,
        COMMAND_EXPORT_ALL_CHARACTERS = 557,
        COMMAND_MUSIC_BACKGROUND_GET_DAY_TRACK = 558,
        COMMAND_MUSIC_BACKGROUND_GET_NIGHT_TRACK = 559,
        COMMAND_PRINT_LOG_ENTRY = 560,
        COMMAND_GET_FACTION_LEADER = 562,
        COMMAND_SWMG_SET_SPEED_BLUR_EFFECT = 563,
        COMMAND_END_GAME = 564,
        COMMAND_GET_RUN_SCRIPT_VAR = 565,
        COMMAND_GET_CREATURE_MOVMENT_TYPE = 566,
        COMMAND_AMBIENT_SOUND_SET_DAY_VOLUME = 567,
        COMMAND_AMBIENT_SOUND_SET_NIGHT_VOLUME = 568,
        COMMAND_MUSIC_BACKGROUND_GET_BATTLE_TRACK = 569,
        COMMAND_GET_HAS_INVENTORY = 570,
        COMMAND_GET_STR_REF_SOUND_DURATION = 571,
        COMMAND_ADD_TO_PARTY = 572,
        COMMAND_REMOVE_FROM_PARTY = 573,
        COMMAND_ADD_PARTY_MEMBER = 574,
        COMMAND_REMOVE_PARTY_MEMBER = 575,
        COMMAND_IS_OBJECT_PARTY_MEMBER = 576,
        COMMAND_GET_PARTY_MEMBER_BY_INDEX = 577,
        COMMAND_ADD_JOURNAL_WORLD_ENTRY = 669,
        COMMAND_ADD_JOURNAL_WORLD_ENTRY_STRREF = 670,
        COMMAND_BARK_STRING = 671,
        COMMAND_DELETE_JOURNAL_WORLD_ALL_ENTRIES = 672,
        COMMAND_DELETE_JOURNAL_WORLD_ENTRY = 673,
        COMMAND_DELETE_JOURNAL_WORLD_ENTRY_STRREF = 674,
        COMMAND_EFFECT_FORCE_DRAIN = 675,
        COMMAND_EFFECT_PSYCHIC_STATIC = 676,
        COMMAND_PLAY_VISUAL_AREA_EFFECT = 677,
        COMMAND_SET_JOURNAL_QUEST_ENTRY_PICTURE = 678,
        COMMAND_SOUND_OBJECT_GET_PITCH_VARIANCE = 689,
        COMMAND_SOUND_OBJECT_SET_PITCH_VARIANCE = 690,
        COMMAND_SOUND_OBJECT_GET_VOLUME = 691,
        COMMAND_GET_GLOBAL_LOCATION = 692,
        COMMAND_SET_GLOBAL_LOCATION = 693,
        COMMAND_ADD_AVAILABLE_NPC_BY_OBJECT = 694,
        COMMAND_REMOVE_AVAILABLE_NPC = 695,
        COMMAND_ADD_AVAILABLE_NPC_BY_TEMPLATE = 697,
        COMMAND_SPAWN_AVAILABLE_NPC = 698,
        COMMAND_IS_NPC_PARTY_MEMBER = 699,
        COMMAND_ACTION_BARK_STRING = 700,
        COMMAND_GET_IS_CONVERSATION_ACTIVE = 701,
        COMMAND_EFFECT_LIGHTSABER_THROW = 702,
        COMMAND_EFFECT_WHIRL_WIND = 703,
        COMMAND_GET_PARTY_AI_STYLE = 704,
        COMMAND_GET_NPCAI_STYLE = 705,
        COMMAND_SET_PARTY_AI_STYLE = 706,
        COMMAND_SET_NPCAI_STYLE = 707,
        COMMAND_SET_NPC_SELECTABILITY = 708,
        COMMAND_CLEAR_ALL_EFFECTS = 710,
        COMMAND_GET_LAST_CONVERSATION = 711,
        COMMAND_SHOW_PARTY_SELECTION_GUI = 712,
        COMMAND_GET_STANDARD_FACTION = 713,
        COMMAND_GIVE_PLOT_XP = 714,
        COMMAND_GET_MIN_ONE_HP = 715,
        COMMAND_SET_MIN_ONE_HP = 716,
        COMMAND_SET_GLOBAL_FADE_IN = 719,
        COMMAND_SET_GLOBAL_FADE_OUT = 720,
        COMMAND_GET_LAST_HOSTILE_TARGET = 721,
        COMMAND_GET_LAST_ATTACK_ACTION = 722,
        COMMAND_GET_LAST_FORCE_POWER_USED = 723,
        COMMAND_GET_LAST_COMBAT_FEAT_USED = 724,
        COMMAND_GET_LAST_ATTACK_RESULT = 725,
        COMMAND_GET_WAS_FORCE_POWER_SUCCESSFUL = 726,
        COMMAND_SET_FORMATION = 729,
        COMMAND_ACTION_FOLLOW_LEADER = 730,
        COMMAND_SET_FORCE_POWER_UNSUCCESSFUL = 731,
        COMMAND_GET_IS_DEBILITATED = 732,
        COMMAND_PLAY_MOVIE = 733,
        COMMAND_SAVE_NPC_STATE = 734,
        COMMAND_CHANGE_FACTION_BY_FACTION = 737,
        COMMAND_PLAY_ROOM_ANIMATION = 738,
        COMMAND_SHOW_GALAXY_MAP_GUI = 739,
        COMMAND_SET_PLANET_SELECTABLE = 740,
        COMMAND_GET_PLANET_SELECTABLE = 741,
        COMMAND_SET_PLANET_AVAILABLE = 742,
        COMMAND_GET_PLANET_AVAILABLE = 743,
        COMMAND_GET_SELECTED_PLANET = 744,
        COMMAND_SOUND_OBJECT_FADE_AND_STOP = 745,
        COMMAND_SET_AREA_FOG_COLOR = 746,
        COMMAND_CHANGE_ITEM_COST = 747,
        COMMAND_GET_IS_LIVE_CONTENT_AVAILABLE = 748,
        COMMAND_RESET_DIALOG_STATE = 749,
        COMMAND_SET_GOOD_EVIL_VALUE = 750,
        COMMAND_GET_IS_POISONED = 751,
        COMMAND_GET_SPELL_TARGET = 752,
        COMMAND_SET_SOLO_MODE = 753,
        COMMAND_CANCEL_POST_DIALOG_CHARACTER_SWITCH = 757,
        COMMAND_SET_MAX_HIT_POINTS = 758,
        COMMAND_NO_CLICKS_FOR = 759,
        COMMAND_HOLD_WORLD_FADE_IN_FOR_DIALOG = 760,
        COMMAND_SHIP_BUILD = 761,
        COMMAND_SUPPRESS_STATUS_SUMMARY_ENTRY = 763,
        COMMAND_GET_CHEAT_CODE = 764,
        COMMAND_SET_MUSIC_VOLUME = 765,
        COMMAND_CREATE_ITEM_ON_FLOOR = 766,
        COMMAND_SET_AVAILABLE_NPC_ID = 767,
        COMMAND_IS_MOVIE_PLAYING = 768,
        COMMAND_QUEUE_MOVIE = 769,
        COMMAND_PLAY_MOVIE_QUEUE = 770,
        COMMAND_YAVIN_HACK_CLOSE_DOOR = 771,
    };

    static const int COMMAND_COUNT = 772;
    typedef int (CSWVirtualMachineCommands::*CommandFunction)(int, int);
    CommandFunction* m_pCommands;
    virtual ~CSWVirtualMachineCommands();
    virtual void InitializeCommands();
    virtual int ExecuteCommand(int, int);
    virtual void RunScriptCallback(CExoString&);
    virtual void ReportError(CExoString&, int);
    virtual void* CreateGameDefinedStructure(int);
    virtual void DestroyGameDefinedStructure(int, void*);
    virtual int GetEqualGameDefinedStructure(int, void*, void*);
    virtual void* CopyGameDefinedStructure(int, void*);
    virtual int SaveGameDefinedStructure(int, void*, CResGFF*, CResStruct*);
    virtual int LoadGameDefinedStructure(int, void**, CResGFF*, CResStruct*);
    int ExecuteCommandActionBarkString(int nCommand, int nArgs);
    int ExecuteCommandActionDoCommand(int nCommand, int nArgs);
    int ExecuteCommandActionEquipMostDamagingMelee(int nCommand, int nArgs);
    int ExecuteCommandActionEquipMostDamagingRanged(int nCommand, int nArgs);
    int ExecuteCommandActionEquipMostEffectiveArmor(int nCommand, int nArgs);
    int ExecuteCommandActionExchangeItem(int nCommand, int nArgs);
    int ExecuteCommandActionFollowObject(int nCommand, int nArgs);
    int ExecuteCommandActionFollowLeader(int nCommand, int nArgs);
    int ExecuteCommandActionInteractObject(int nCommand, int nArgs);
    int ExecuteCommandActionJumpToObject(int nCommand, int nArgs);
    int ExecuteCommandActionJumpToPoint(int nCommand, int nArgs);
    int ExecuteCommandActionLockActions(int nCommand, int nArgs);
    int ExecuteCommandActionMoveAwayFromLocation(int nCommand, int nArgs);
    int ExecuteCommandActionPauseConversation(int nCommand, int nArgs);
    int ExecuteCommandActionRandomWalk(int nCommand, int nArgs);
    int ExecuteCommandActionResumeConversation(int nCommand, int nArgs);
    int ExecuteCommandActionStartConversation(int nCommand, int nArgs);
    int ExecuteCommandActionUseFeat(int nCommand, int nArgs);
    int ExecuteCommandActionUseSkill(int nCommand, int nArgs);
    int ExecuteCommandActionUseTalent(int nCommand, int nArgs);
    int ExecuteCommandAddAvailableNPCByObject(int nCommand, int nArgs);
    int ExecuteCommandAddAvailableNPCByTemplate(int nCommand, int nArgs);
    int ExecuteCommandAddMultiClass(int nCommand, int nArgs);
    int ExecuteCommandAddPartyMember(int nCommand, int nArgs);
    int ExecuteCommandAddToParty(int nCommand, int nArgs);
    int ExecuteCommandAdjustAlignment(int nCommand, int nArgs);
    int ExecuteCommandAdjustReputation(int nCommand, int nArgs);
    int ExecuteCommandAmbientSound(int nCommand, int nArgs);
    int ExecuteCommandApplyEffectAtPoint(int nCommand, int nArgs);
    int ExecuteCommandApplyEffectOnObject(int nCommand, int nArgs);
    int ExecuteCommandAssignCommand(int nCommand, int nArgs);
    int ExecuteCommandAttack(int nCommand, int nArgs);
    int ExecuteCommandCutsceneAttack(int nCommand, int nArgs);
    int ExecuteCommandCutsceneMove(int nCommand, int nArgs);
    int ExecuteCommandBeginConversation(int nCommand, int nArgs);
    int ExecuteCommandCastSpell(int nCommand, int nArgs);
    int ExecuteCommandChangeFaction(int nCommand, int nArgs);
    int ExecuteCommandClearAllActions(int nCommand, int nArgs);
    int ExecuteCommandCloseDoor(int nCommand, int nArgs);
    int ExecuteCommandCreateItemOnObject(int nCommand, int nArgs);
    int ExecuteCommandCreateObject(int nCommand, int nArgs);
    int ExecuteCommandDelayCommand(int nCommand, int nArgs);
    int ExecuteCommandDestroyObject(int nCommand, int nArgs);
    int ExecuteCommandDisplayFeedBackText(int nCommand, int nArgs);
    int ExecuteCommandDistanceConversions(int nCommand, int nArgs);
    int ExecuteCommandDoDoorAction(int nCommand, int nArgs);
    int ExecuteCommandDoSinglePlayerAutoSave(int nCommand, int nArgs);
    int ExecuteCommandEffectAbilityDecrease(int nCommand, int nArgs);
    int ExecuteCommandEffectAbilityIncrease(int nCommand, int nArgs);
    int ExecuteCommandEffectACDecrease(int nCommand, int nArgs);
    int ExecuteCommandEffectACIncrease(int nCommand, int nArgs);
    int ExecuteCommandEffectAreaEffect(int nCommand, int nArgs);
    int ExecuteCommandEffectAttackDecrease(int nCommand, int nArgs);
    int ExecuteCommandEffectAttackIncrease(int nCommand, int nArgs);
    int ExecuteCommandEffectBeam(int nCommand, int nArgs);
    int ExecuteCommandEffectComplex(int nCommand, int nArgs);
    int ExecuteCommandEffectConcealment(int nCommand, int nArgs);
    int ExecuteCommandEffectDamage(int nCommand, int nArgs);
    int ExecuteCommandEffectDamageDecrease(int nCommand, int nArgs);
    int ExecuteCommandEffectDamageImmunityDecrease(int nCommand, int nArgs);
    int ExecuteCommandEffectDamageImmunityIncrease(int nCommand, int nArgs);
    int ExecuteCommandEffectDamageIncrease(int nCommand, int nArgs);
    int ExecuteCommandEffectDamageReduction(int nCommand, int nArgs);
    int ExecuteCommandEffectDamageResistance(int nCommand, int nArgs);
    int ExecuteCommandEffectForceShield(int nCommand, int nArgs);
    int ExecuteCommandEffectDamageShield(int nCommand, int nArgs);
    int ExecuteCommandEffectForceDrain(int nCommand, int nArgs);
    int ExecuteCommandEffectHeal(int nCommand, int nArgs);
    int ExecuteCommandEffectHitPointChangeWhenDying(int nCommand, int nArgs);
    int ExecuteCommandEffectImmunity(int nCommand, int nArgs);
    int ExecuteCommandEffectInvisibility(int nCommand, int nArgs);
    int ExecuteCommandEffectLinkEffects(int nCommand, int nArgs);
    int ExecuteCommandEffectMissChance(int nCommand, int nArgs);
    int ExecuteCommandEffectModifyAttacks(int nCommand, int nArgs);
    int ExecuteCommandEffectMovementSpeedDecrease(int nCommand, int nArgs);
    int ExecuteCommandEffectMovementSpeedIncrease(int nCommand, int nArgs);
    int ExecuteCommandEffectPoison(int nCommand, int nArgs);
    int ExecuteCommandEffectResurrection(int nCommand, int nArgs);
    int ExecuteCommandEffectSavingThrowDecrease(int nCommand, int nArgs);
    int ExecuteCommandEffectSavingThrowIncrease(int nCommand, int nArgs);
    int ExecuteCommandEffectSeeInvisible(int nCommand, int nArgs);
    int ExecuteCommandEffectSkillDecrease(int nCommand, int nArgs);
    int ExecuteCommandEffectSkillIncrease(int nCommand, int nArgs);
    int ExecuteCommandEffectSpellImmunity(int nCommand, int nArgs);
    int ExecuteCommandEffectSpellLevelAbsorption(int nCommand, int nArgs);
    int ExecuteCommandEffectForceResistanceDecrease(int nCommand, int nArgs);
    int ExecuteCommandEffectForceResistanceIncrease(int nCommand, int nArgs);
    int ExecuteCommandEffectTemporaryFP(int nCommand, int nArgs);
    int ExecuteCommandEffectTemporaryHP(int nCommand, int nArgs);
    int ExecuteCommandEffectTrueSeeing(int nCommand, int nArgs);
    int ExecuteCommandEffectVisualEffect(int nCommand, int nArgs);
    int ExecuteCommandEffectDroidStun(int nCommand, int nArgs);
    int ExecuteCommandEffectForceJump(int nCommand, int nArgs);
    int ExecuteCommandEffectForcePushed(int nCommand, int nArgs);
    int ExecuteCommandEffectForcePushTargeted(int nCommand, int nArgs);
    int ExecuteCommandEffectWhirlwind(int nCommand, int nArgs);
    int ExecuteCommandEffectBlasterDeflectionIncrease(int, int);
    int ExecuteCommandEffectBlasterDeflectionDecrease(int, int);
    int ExecuteCommandEffectHorrified(int nCommand, int nArgs);
    int ExecuteCommandEffectDamageForcePoints(int nCommand, int nArgs);
    int ExecuteCommandEffectHealForcePoints(int nCommand, int nArgs);
    int ExecuteCommandEffectTemporaryForcePoints(int nCommand, int nArgs);
    int ExecuteCommandEffectChoke(int nCommand, int nArgs);
    int ExecuteCommandEffectBodyFuel(int nCommand, int nArgs);
    int ExecuteCommandEffectPsychicStatic(int nCommand, int nArgs);
    int ExecuteCommandEffectLightsaberThrow(int nCommand, int nArgs);
    int ExecuteCommandEndGame(int nCommand, int nArgs);
    int ExecuteCommandEquipItem(int nCommand, int nArgs);
    int ExecuteCommandEventActivateItem(int nCommand, int nArgs);
    int ExecuteCommandEventConversation(int nCommand, int nArgs);
    int ExecuteCommandEventSpellCastAt(int nCommand, int nArgs);
    int ExecuteCommandEventUserDefined(int nCommand, int nArgs);
    int ExecuteCommandExecuteScript(int nCommand, int nArgs);
    int ExecuteCommandExportAllCharacters(int nCommand, int nArgs);
    int ExecuteCommandFindSubString(int nCommand, int nArgs);
    int ExecuteCommandFloatingText(int nCommand, int nArgs);
    int ExecuteCommandFloatToInt(int nCommand, int nArgs);
    int ExecuteCommandFloatToString(int nCommand, int nArgs);
    int ExecuteCommandGetAbilityScore(int nCommand, int nArgs);
    int ExecuteCommandGetAbilityModifier(int nCommand, int nArgs);
    int ExecuteCommandGetAC(int nCommand, int nArgs);
    int ExecuteCommandGetAlignment(int nCommand, int nArgs);
    int ExecuteCommandGetAOEObjectCreator(int nCommand, int nArgs);
    int ExecuteCommandGetAppearanceType(int nCommand, int nArgs);
    int ExecuteCommandGetArea(int nCommand, int nArgs);
    int ExecuteCommandGetAreaUnescapable(int nCommand, int nArgs);
    int ExecuteCommandGetAttackTarget(int nCommand, int nArgs);
    int ExecuteCommandGetAttemptedAttackTarget(int nCommand, int nArgs);
    int ExecuteCommandGetAttemptedSpellTarget(int nCommand, int nArgs);
    int ExecuteCommandGetAttemptedMovementTarget(int nCommand, int nArgs);
    int ExecuteCommandGetSpellTarget(int nCommand, int nArgs);
    int ExecuteCommandGetBaseItemType(int nCommand, int nArgs);
    int ExecuteCommandGetBattleTrack(int nCommand, int nArgs);
    int ExecuteCommandGetBlockingDoor(int nCommand, int nArgs);
    int ExecuteCommandGetCasterLevel(int nCommand, int nArgs);
    int ExecuteCommandGetChallengeRating(int nCommand, int nArgs);
    int ExecuteCommandGetClassInformation(int nCommand, int nArgs);
    int ExecuteCommandGetCommandable(int nCommand, int nArgs);
    int ExecuteCommandGetCreatureHasTalent(int nCommand, int nArgs);
    int ExecuteCommandGetCreatureSize(int nCommand, int nArgs);
    int ExecuteCommandGetCreatureTalent(int nCommand, int nArgs);
    int ExecuteCommandGetDamageDealtByType(int nCommand, int nArgs);
    int ExecuteCommandGetDayTrack(int nCommand, int nArgs);
    int ExecuteCommandGetDistanceBetween(int nCommand, int nArgs);
    int ExecuteCommandGetDistanceBetween2D(int nCommand, int nArgs);
    int ExecuteCommandGetDistanceBetweenLocations(int nCommand, int nArgs);
    int ExecuteCommandGetDistanceBetweenLocations2D(int nCommand, int nArgs);
    int ExecuteCommandGetDistanceToObject(int nCommand, int nArgs);
    int ExecuteCommandGetDistanceToObject2D(int nCommand, int nArgs);
    int ExecuteCommandGetEffectCreator(int nCommand, int nArgs);
    int ExecuteCommandGetEffectDurationType(int nCommand, int nArgs);
    int ExecuteCommandGetEffectSpellId(int nCommand, int nArgs);
    int ExecuteCommandGetEffectSubType(int nCommand, int nArgs);
    int ExecuteCommandGetEffectType(int nCommand, int nArgs);
    int ExecuteCommandGetEmotions(int nCommand, int nArgs);
    int ExecuteCommandGetEncounterData(int nCommand, int nArgs);
    int ExecuteCommandGetEnteringObject(int nCommand, int nArgs);
    int ExecuteCommandGetFacing(int nCommand, int nArgs);
    int ExecuteCommandGetFactionAverageGoodEvilAlignment(int, int);
    int ExecuteCommandGetFactionAverageLevel(int nCommand, int nArgs);
    int ExecuteCommandGetFactionAverageReputation(int nCommand, int nArgs);
    int ExecuteCommandGetFactionAverageXP(int nCommand, int nArgs);
    int ExecuteCommandGetFactionBestAC(int nCommand, int nArgs);
    int ExecuteCommandGetFactionEqual(int nCommand, int nArgs);
    int ExecuteCommandGetFactionGold(int nCommand, int nArgs);
    int ExecuteCommandGetFactionLeastDamagedMember(int nCommand, int nArgs);
    int ExecuteCommandGetFactionLeader(int nCommand, int nArgs);
    int ExecuteCommandGetFactionMember(int nCommand, int nArgs);
    int ExecuteCommandGetFactionMostDamagedMember(int nCommand, int nArgs);
    int ExecuteCommandGetFactionMostFrequentClass(int nCommand, int nArgs);
    int ExecuteCommandGetFactionStrongestMember(int nCommand, int nArgs);
    int ExecuteCommandGetFactionWeakestMember(int nCommand, int nArgs);
    int ExecuteCommandGetFactionWorstAC(int nCommand, int nArgs);
    int ExecuteCommandGetFortitudeSavingThrow(int nCommand, int nArgs);
    int ExecuteCommandGetGameDifficulty(int nCommand, int nArgs);
    int ExecuteCommandGetGlobalLocation(int nCommand, int nArgs);
    int ExecuteCommandGetGlobalNumber(int nCommand, int nArgs);
    int ExecuteCommandGetGlobalString(int nCommand, int nArgs);
    int ExecuteCommandGetGoingToBeAttackedBy(int nCommand, int nArgs);
    int ExecuteCommandGetGold(int nCommand, int nArgs);
    int ExecuteCommandGetGoldPieceValue(int nCommand, int nArgs);
    int ExecuteCommandGetHasFeat(int nCommand, int nArgs);
    int ExecuteCommandGetHasFeatEffect(int nCommand, int nArgs);
    int ExecuteCommandGetHasInventory(int nCommand, int nArgs);
    int ExecuteCommandGetHasSkill(int nCommand, int nArgs);
    int ExecuteCommandGetHasSpell(int nCommand, int nArgs);
    int ExecuteCommandGetHasSpellEffect(int nCommand, int nArgs);
    int ExecuteCommandGetHitDice(int nCommand, int nArgs);
    int ExecuteCommandGetHitpoints(int nCommand, int nArgs);
    int ExecuteCommandGetCurrentForcePoints(int nCommand, int nArgs);
    int ExecuteCommandGetMaxForcePoints(int nCommand, int nArgs);
    int ExecuteCommandGetIdentified(int nCommand, int nArgs);
    int ExecuteCommandGetInventoryDisturbItem(int nCommand, int nArgs);
    int ExecuteCommandGetInventoryDisturbType(int nCommand, int nArgs);
    int ExecuteCommandGetIsDawn(int nCommand, int nArgs);
    int ExecuteCommandGetIsDay(int nCommand, int nArgs);
    int ExecuteCommandGetIsDoorActionPossible(int nCommand, int nArgs);
    int ExecuteCommandGetIsDusk(int nCommand, int nArgs);
    int ExecuteCommandGetIsEncounterCreature(int nCommand, int nArgs);
    int ExecuteCommandGetIsImmune(int nCommand, int nArgs);
    int ExecuteCommandGetIsInCombat(int nCommand, int nArgs);
    int ExecuteCommandGetIsNight(int nCommand, int nArgs);
    int ExecuteCommandGetIsOpen(int nCommand, int nArgs);
    int ExecuteCommandGetIsRacialTypePlayable(int nCommand, int nArgs);
    int ExecuteCommandGetIsTalentValid(int nCommand, int nArgs);
    int ExecuteCommandGetIsMined(int nCommand, int nArgs);
    int ExecuteCommandGetIsWeaponEffective(int nCommand, int nArgs);
    int ExecuteCommandGetItemACValue(int nCommand, int nArgs);
    int ExecuteCommandGetItemHasItemProperty(int nCommand, int nArgs);
    int ExecuteCommandGetItemInSlot(int nCommand, int nArgs);
    int ExecuteCommandGetItemPossessedBy(int nCommand, int nArgs);
    int ExecuteCommandGetItemPossessor(int nCommand, int nArgs);
    int ExecuteCommandGetJournalQuestExperience(int nCommand, int nArgs);
    int ExecuteCommandGetLastAttacker(int nCommand, int nArgs);
    int ExecuteCommandGetLastHostileTarget(int nCommand, int nArgs);
    int ExecuteCommandGetLastAttackAction(int nCommand, int nArgs);
    int ExecuteCommandGetLastAttackMode(int nCommand, int nArgs);
    int ExecuteCommandGetLastAttackResult(int nCommand, int nArgs);
    int ExecuteCommandGetWasForcePowerSuccessful(int nCommand, int nArgs);
    int ExecuteCommandGetLastAttackType(int nCommand, int nArgs);
    int ExecuteCommandGetLastClosedBy(int nCommand, int nArgs);
    int ExecuteCommandGetLastCombatFeatUsed(int nCommand, int nArgs);
    int ExecuteCommandGetLastDamager(int nCommand, int nArgs);
    int ExecuteCommandGetLastDisarmed(int nCommand, int nArgs);
    int ExecuteCommandGetLastDisturbed(int nCommand, int nArgs);
    int ExecuteCommandGetLastForcePowerUsed(int nCommand, int nArgs);
    int ExecuteCommandGetLastHostileActor(int nCommand, int nArgs);
    int ExecuteCommandGetLastKiller(int nCommand, int nArgs);
    int ExecuteCommandGetLastLocked(int nCommand, int nArgs);
    int ExecuteCommandGetLastOpenedBy(int nCommand, int nArgs);
    int ExecuteCommandGetLastPazaakResult(int nCommand, int nArgs);
    int ExecuteCommandGetLastPerceived(int nCommand, int nArgs);
    int ExecuteCommandGetLastPerception(int nCommand, int nArgs);
    int ExecuteCommandGetLastSpeaker(int nCommand, int nArgs);
    int ExecuteCommandGetLastMineDetected(int nCommand, int nArgs);
    int ExecuteCommandGetLastUnlocked(int nCommand, int nArgs);
    int ExecuteCommandGetLastUsedBy(int nCommand, int nArgs);
    int ExecuteCommandGetLastWeaponUsed(int nCommand, int nArgs);
    int ExecuteCommandGetLeavingObject(int nCommand, int nArgs);
    int ExecuteCommandGetLoadFromSaveGame(int nCommand, int nArgs);
    int ExecuteCommandGetLocalValue(int nCommand, int nArgs);
    int ExecuteCommandGetLocation(int nCommand, int nArgs);
    int ExecuteCommandGetLocked(int nCommand, int nArgs);
    int ExecuteCommandGetLockInfo(int nCommand, int nArgs);
    int ExecuteCommandGetMatchedStringsCount(int nCommand, int nArgs);
    int ExecuteCommandGetMatchedStringsNum(int nCommand, int nArgs);
    int ExecuteCommandGetMetaMagicFeat(int nCommand, int nArgs);
    int ExecuteCommandGetModule(int nCommand, int nArgs);
    int ExecuteCommandGetModuleItemStuff(int nCommand, int nArgs);
    int ExecuteCommandGetModuleName(int nCommand, int nArgs);
    int ExecuteCommandGetMovementRate(int nCommand, int nArgs);
    int ExecuteCommandGetName(int nCommand, int nArgs);
    int ExecuteCommandGetNightTrack(int nCommand, int nArgs);
    int ExecuteCommandGetNumStackedItems(int nCommand, int nArgs);
    int ExecuteCommandGetObjectByTag(int nCommand, int nArgs);
    int ExecuteCommandGetObjectInArea(int nCommand, int nArgs);
    int ExecuteCommandGetObjectType(int nCommand, int nArgs);
    int ExecuteCommandGetObjectValid(int nCommand, int nArgs);
    int ExecuteCommandGetObjectVisibility(int nCommand, int nArgs);
    int ExecuteCommandGetPartyMemberByIndex(int nCommand, int nArgs);
    int ExecuteCommandGetPartyMemberCount(int nCommand, int nArgs);
    int ExecuteCommandGetPC(int nCommand, int nArgs);
    int ExecuteCommandGetPCLevellingUp(int nCommand, int nArgs);
    int ExecuteCommandGetPCSpeaker(int nCommand, int nArgs);
    int ExecuteCommandGetPlaceableIllumination(int nCommand, int nArgs);
    int ExecuteCommandGetPlotFlag(int nCommand, int nArgs);
    int ExecuteCommandGetPosition(int nCommand, int nArgs);
    int ExecuteCommandGetRacialType(int nCommand, int nArgs);
    int ExecuteCommandGetReflexAdjustedDamage(int nCommand, int nArgs);
    int ExecuteCommandGetReflexSavingThrow(int nCommand, int nArgs);
    int ExecuteCommandGetReputation(int nCommand, int nArgs);
    int ExecuteCommandGetRestrictMode(int nCommand, int nArgs);
    int ExecuteCommandGetRunScriptVar(int nCommand, int nArgs);
    int ExecuteCommandGetSkillRank(int nCommand, int nArgs);
    int ExecuteCommandGetSpellCast(int nCommand, int nArgs);
    int ExecuteCommandGetSpellCasterItem(int nCommand, int nArgs);
    int ExecuteCommandGetSpellId(int nCommand, int nArgs);
    int ExecuteCommandGetSpellSaveDC(int nCommand, int nArgs);
    int ExecuteCommandGetSpellTargetLoc(int nCommand, int nArgs);
    int ExecuteCommandGetSpellTargetObject(int nCommand, int nArgs);
    int ExecuteCommandGetStat(int nCommand, int nArgs);
    int ExecuteCommandGetStrByStrRef(int nCommand, int nArgs);
    int ExecuteCommandGetStringLeft(int nCommand, int nArgs);
    int ExecuteCommandGetStringLength(int nCommand, int nArgs);
    int ExecuteCommandGetStringLowerCase(int nCommand, int nArgs);
    int ExecuteCommandGetStringRight(int nCommand, int nArgs);
    int ExecuteCommandGetStringUpperCase(int nCommand, int nArgs);
    int ExecuteCommandGetStrRefSoundDuration(int nCommand, int nArgs);
    int ExecuteCommandGetSubString(int nCommand, int nArgs);
    int ExecuteCommandGetTag(int nCommand, int nArgs);
    int ExecuteCommandGetTimeHour(int nCommand, int nArgs);
    int ExecuteCommandGetTimeMillisecond(int nCommand, int nArgs);
    int ExecuteCommandGetTimeMinute(int nCommand, int nArgs);
    int ExecuteCommandGetTimeSecond(int nCommand, int nArgs);
    int ExecuteCommandGetTotalDamageDealt(int nCommand, int nArgs);
    int ExecuteCommandGetUserActionsPending(int nCommand, int nArgs);
    int ExecuteCommandGetUserDefinedEventNumber(int nCommand, int nArgs);
    int ExecuteCommandGetWaypointByTag(int nCommand, int nArgs);
    int ExecuteCommandGetWaypointByTrigger(int nCommand, int nArgs);
    int ExecuteCommandGetWeaponRanged(int nCommand, int nArgs);
    int ExecuteCommandGetWillSavingThrow(int nCommand, int nArgs);
    int ExecuteCommandGetXP(int nCommand, int nArgs);
    int ExecuteCommandGiveGoldToCreature(int nCommand, int nArgs);
    int ExecuteCommandGiveXPToCreature(int nCommand, int nArgs);
    int ExecuteCommandGiveItem(int nCommand, int nArgs);
    int ExecuteCommandInsertString(int nCommand, int nArgs);
    int ExecuteCommandIntToFloat(int nCommand, int nArgs);
    int ExecuteCommandIntToHexString(int nCommand, int nArgs);
    int ExecuteCommandIntToString(int nCommand, int nArgs);
    int ExecuteCommandIsAIState(int nCommand, int nArgs);
    int ExecuteCommandIsAvailableNPC(int nCommand, int nArgs);
    int ExecuteCommandIsEffectValid(int nCommand, int nArgs);
    int ExecuteCommandGetIsInConversation(int nCommand, int nArgs);
    int ExecuteCommandGetIsConversationActive(int nCommand, int nArgs);
    int ExecuteCommandGetCreatureMovmentType(int nCommand, int nArgs);
    int ExecuteCommandGetLastConversation(int nCommand, int nArgs);
    int ExecuteCommandIsListening(int nCommand, int nArgs);
    int ExecuteCommandIsNPCPartyMember(int nCommand, int nArgs);
    int ExecuteCommandIsObjectPartyMember(int nCommand, int nArgs);
    int ExecuteCommandIsPC(int nCommand, int nArgs);
    int ExecuteCommandItemActivated(int nCommand, int nArgs);
    int ExecuteCommandLocation(int nCommand, int nArgs);
    int ExecuteCommandLocationAccess(int nCommand, int nArgs);
    int ExecuteCommandMath(int nCommand, int nArgs);
    int ExecuteCommandModuleAccess(int nCommand, int nArgs);
    int ExecuteCommandMoveAwayFromObject(int nCommand, int nArgs);
    int ExecuteCommandMoveToObject(int nCommand, int nArgs);
    int ExecuteCommandMoveToPoint(int nCommand, int nArgs);
    int ExecuteCommandMusicBackground(int nCommand, int nArgs);
    int ExecuteCommandMusicBattle(int nCommand, int nArgs);
    int ExecuteCommandObjectToString(int nCommand, int nArgs);
    int ExecuteCommandOpenDoor(int nCommand, int nArgs);
    int ExecuteCommandOpenStore(int nCommand, int nArgs);
    int ExecuteCommandPickUpItem(int nCommand, int nArgs);
    int ExecuteCommandPlayAnimation(int nCommand, int nArgs);
    int ExecuteCommandPlayRoomAnimation(int nCommand, int nArgs);
    int ExecuteCommandPlayMovie(int nCommand, int nArgs);
    int ExecuteCommandPlayPazaak(int nCommand, int nArgs);
    int ExecuteCommandPlayRumblePattern(int nCommand, int nArgs);
    int ExecuteCommandPlaySound(int nCommand, int nArgs);
    int ExecuteCommandPlayVisualAreaEffect(int nCommand, int nArgs);
    int ExecuteCommandPopUpGUIPanel(int nCommand, int nArgs);
    int ExecuteCommandPrintFloat(int nCommand, int nArgs);
    int ExecuteCommandPrintInteger(int nCommand, int nArgs);
    int ExecuteCommandPrintLogEntry(int nCommand, int nArgs);
    int ExecuteCommandPrintObject(int nCommand, int nArgs);
    int ExecuteCommandPrintString(int nCommand, int nArgs);
    int ExecuteCommandPrintVector(int nCommand, int nArgs);
    int ExecuteCommandPutDownItem(int nCommand, int nArgs);
    int ExecuteCommandRandom(int nCommand, int nArgs);
    int ExecuteCommandRandomName(int nCommand, int nArgs);
    int ExecuteCommandRemoveAvailableNPC(int nCommand, int nArgs);
    int ExecuteCommandRemoveFromParty(int nCommand, int nArgs);
    int ExecuteCommandRemovePartyMember(int nCommand, int nArgs);
    int ExecuteCommandResistSpell(int nCommand, int nArgs);
    int ExecuteCommandRollDice(int nCommand, int nArgs);
    int ExecuteCommandSaveNPCState(int nCommand, int nArgs);
    int ExecuteCommandSavingThrow(int nCommand, int nArgs);
    int ExecuteCommandSendMessageToPC(int nCommand, int nArgs);
    int ExecuteCommandSetAreaTransitionBMP(int nCommand, int nArgs);
    int ExecuteCommandSetAreaUnescapable(int nCommand, int nArgs);
    int ExecuteCommandSetCameraLocation(int nCommand, int nArgs);
    int ExecuteCommandSetCameraMode(int nCommand, int nArgs);
    int ExecuteCommandSetCommandable(int nCommand, int nArgs);
    int ExecuteCommandSetEncounterData(int nCommand, int nArgs);
    int ExecuteCommandSetFacing(int nCommand, int nArgs);
    int ExecuteCommandSetGlobalLocation(int nCommand, int nArgs);
    int ExecuteCommandSetGlobalNumber(int nCommand, int nArgs);
    int ExecuteCommandSetGlobalString(int nCommand, int nArgs);
    int ExecuteCommandSetIdentified(int nCommand, int nArgs);
    int ExecuteCommandSetIsDestroyable(int nCommand, int nArgs);
    int ExecuteCommandSetLightsaberPowered(int nCommand, int nArgs);
    int ExecuteCommandSetListening(int nCommand, int nArgs);
    int ExecuteCommandSetListenString(int nCommand, int nArgs);
    int ExecuteCommandSetLocalValue(int nCommand, int nArgs);
    int ExecuteCommandSetLocked(int nCommand, int nArgs);
    int ExecuteCommandSetNPCSelectability(int nCommand, int nArgs);
    int ExecuteCommandSetPlaceableIllumination(int nCommand, int nArgs);
    int ExecuteCommandSetPlotFlag(int nCommand, int nArgs);
    int ExecuteCommandSetRestrictMode(int nCommand, int nArgs);
    int ExecuteCommandSetReturnStrref(int nCommand, int nArgs);
    int ExecuteCommandSetSpeedBlurEffect(int nCommand, int nArgs);
    int ExecuteCommandSetSubType(int nCommand, int nArgs);
    int ExecuteCommandSetTime(int nCommand, int nArgs);
    int ExecuteCommandSetForcePowerUnsuccessful(int nCommand, int nArgs);
    int ExecuteCommandSetXP(int nCommand, int nArgs);
    int ExecuteCommandShipBuild(int nCommand, int nArgs);
    int ExecuteCommandShowLevelUpGUI(int nCommand, int nArgs);
    int ExecuteCommandShowPartySelection(int nCommand, int nArgs);
    int ExecuteCommandShowUpgradeScreen(int nCommand, int nArgs);
    int ExecuteCommandSignalEvent(int nCommand, int nArgs);
    int ExecuteCommandSoundObjectGetFixedVariance(int nCommand, int nArgs);
    int ExecuteCommandSoundObjectGetPitchVariation(int nCommand, int nArgs);
    int ExecuteCommandSoundObjectGetVolume(int nCommand, int nArgs);
    int ExecuteCommandSoundObjectPlay(int nCommand, int nArgs);
    int ExecuteCommandSoundObjectSetFixedVariance(int nCommand, int nArgs);
    int ExecuteCommandSoundObjectSetPitchVariation(int nCommand, int nArgs);
    int ExecuteCommandSoundObjectSetPosition(int nCommand, int nArgs);
    int ExecuteCommandSoundObjectSetVolume(int nCommand, int nArgs);
    int ExecuteCommandSoundObjectStop(int nCommand, int nArgs);
    int ExecuteCommandSoundObjectFadeAndStop(int nCommand, int nArgs);
    int ExecuteCommandSpawnAvailableNPC(int nCommand, int nArgs);
    int ExecuteCommandSpeakOneLinerConversation(int nCommand, int nArgs);
    int ExecuteCommandSpeakString(int nCommand, int nArgs);
    int ExecuteCommandSpeakStrRef(int nCommand, int nArgs);
    int ExecuteCommandStartNewModule(int nCommand, int nArgs);
    int ExecuteCommandStopRumblePattern(int nCommand, int nArgs);
    int ExecuteCommandStringConversions(int nCommand, int nArgs);
    int ExecuteCommandSurrenderToEnemies(int nCommand, int nArgs);
    int ExecuteCommandSwitchPlayerCreature(int nCommand, int nArgs);
    int ExecuteCommandTakeGoldFromCreature(int nCommand, int nArgs);
    int ExecuteCommandTalent(int nCommand, int nArgs);
    int ExecuteCommandTalentAccess(int nCommand, int nArgs);
    int ExecuteCommandTimeConversions(int nCommand, int nArgs);
    int ExecuteCommandTestString(int nCommand, int nArgs);
    int ExecuteCommandUnequipItem(int nCommand, int nArgs);
    int ExecuteCommandVector(int nCommand, int nArgs);
    int ExecuteCommandVectorConversions(int nCommand, int nArgs);
    int ExecuteCommandVectorMagnitude(int nCommand, int nArgs);
    int ExecuteCommandVectorNormalize(int nCommand, int nArgs);
    int ExecuteCommandVersusEffect(int nCommand, int nArgs);
    int ExecuteCommandWait(int nCommand, int nArgs);
    int ExecuteCommandGetIsLinkImmune(int nCommand, int nArgs);
    int ExecuteCommandGetPartyAIStyle(int nCommand, int nArgs);
    int ExecuteCommandGetNPCAIStyle(int nCommand, int nArgs);
    int ExecuteCommandSetPartyLeader(int nCommand, int nArgs);
    int ExecuteCommandSetPartyAIStyle(int nCommand, int nArgs);
    int ExecuteCommandSetNPCAIStyle(int nCommand, int nArgs);
    int ExecuteCommandClearAllEffects(int nCommand, int nArgs);
    int ExecuteCommandGetStandardFaction(int nCommand, int nArgs);
    int ExecuteCommandGivePlotXP(int nCommand, int nArgs);
    int ExecuteCommandGetMinOneHP(int nCommand, int nArgs);
    int ExecuteCommandSetMinOneHP(int nCommand, int nArgs);
    int ExecuteCommandSetFadeIn(int nCommand, int nArgs);
    int ExecuteCommandSetFadeOut(int nCommand, int nArgs);
    int ExecuteCommandGetAttacker(int nCommand, int nArgs);
    int ExecuteCommandSetFormation(int nCommand, int nArgs);
    int ExecuteCommandGetIsDebilitated(int nCommand, int nArgs);
    int ExecuteCommandChangeByFaction(int nCommand, int nArgs);
    int ExecuteCommandGetSubScreenID(int nCommand, int nArgs);
    int ExecuteCommandCancelCombat(int nCommand, int nArgs);
    int ExecuteCommandPauseGame(int nCommand, int nArgs);
    int ExecuteCommandGetItemStackSize(int nCommand, int nArgs);
    int ExecuteCommandSetItemStackSize(int nCommand, int nArgs);
    int ExecuteCommandShowGalaxyMapGui(int nCommand, int nArgs);
    int ExecuteCommandSetGalaxyMapPlanetAvailable(int nCommand, int nArgs);
    int ExecuteCommandGetGalaxyMapPlanetAvailable(int nCommand, int nArgs);
    int ExecuteCommandSetGalaxyMapPlanetSelectable(int nCommand, int nArgs);
    int ExecuteCommandGetGalaxyMapPlanetSelectable(int nCommand, int nArgs);
    int ExecuteCommandGetGalaxyMapSelectedPlanet(int nCommand, int nArgs);
    int ExecuteCommandSetItemNonEquippable(int nCommand, int nArgs);
    int ExecuteCommandGetButtonMashCheck(int nCommand, int nArgs);
    int ExecuteCommandSetButtonMashCheck(int nCommand, int nArgs);
    int ExecuteCommandSetDialogPlaceableCamera(int nCommand, int nArgs);
    int ExecuteCommandGetSoloMode(int nCommand, int nArgs);
    int ExecuteCommandGetMaxStealthXP(int nCommand, int nArgs);
    int ExecuteCommandSetMaxStealthXP(int nCommand, int nArgs);
    int ExecuteCommandGetCurrentStealthXP(int nCommand, int nArgs);
    int ExecuteCommandSetCurrentStealthXP(int nCommand, int nArgs);
    int ExecuteCommandAwardStealthXP(int nCommand, int nArgs);
    int ExecuteCommandGetStealthXPEnabled(int nCommand, int nArgs);
    int ExecuteCommandSetStealthXPEnabled(int nCommand, int nArgs);
    int ExecuteCommandGetSubRace(int nCommand, int nArgs);
    int ExecuteCommandGetBlockingCreature(int nCommand, int nArgs);
    int ExecuteCommandGetFoundEnemyCreature(int nCommand, int nArgs);
    int ExecuteCommandGetStealthXPDecrement(int nCommand, int nArgs);
    int ExecuteCommandSetStealthXPDecrement(int nCommand, int nArgs);
    int ExecuteCommandDuplicateHeadAppearance(int nCommand, int nArgs);
    int ExecuteCommandSetLockOrientationInDialog(int nCommand, int nArgs);
    int ExecuteCommandSetLockHeadFollowInDialog(int nCommand, int nArgs);
    int ExecuteCommandEnableVideoEffect(int nCommand, int nArgs);
    int ExecuteCommandDisableVideoEffect(int nCommand, int nArgs);
    int ExecuteCommandRevealMap(int nCommand, int nArgs);
    int ExecuteCommandSetTutorialWindowsEnabled(int nCommand, int nArgs);
    int ExecuteCommandShowTutorialWindow(int nCommand, int nArgs);
    int ExecuteCommandIsCreditSequenceInProgress(int nCommand, int nArgs);
    int ExecuteCommandStartCreditSequence(int nCommand, int nArgs);
    int ExecuteCommandGetDifficultyMultiplier(int nCommand, int nArgs);
    int ExecuteCommandSetEffectIcon(int nCommand, int nArgs);
    int ExecuteCommandFaceObjectAwayFromObject(int nCommand, int nArgs);
    int ExecuteCommandSetAreaFogColor(int nCommand, int nArgs);
    int ExecuteCommandChangeItemCost(int nCommand, int nArgs);
    int ExecuteCommandGetIsLiveContentAvailable(int nCommand, int nArgs);
    int ExecuteCommandResetDialogState(int nCommand, int nArgs);
    int ExecuteCommandSetGoodEvilValue(int nCommand, int nArgs);
    int ExecuteCommandGetIsPoisoned(int nCommand, int nArgs);
    int ExecuteCommandSetSoloMode(int nCommand, int nArgs);
    int ExecuteCommandCancelPostDialogCharacterSwitch(int, int);
    int ExecuteCommandSetMaxHitPoints(int nCommand, int nArgs);
    int ExecuteCommandNoClicksFor(int nCommand, int nArgs);
    int ExecuteCommandHoldWorldFadeInForDialog(int nCommand, int nArgs);
    int ExecuteCommandSuppressStatusSummaryEntry(int nCommand, int nArgs);
    int ExecuteCommandGetCheatCode(int nCommand, int nArgs);
    int ExecuteCommandSetMusicVolume(int nCommand, int nArgs);
    int ExecuteCommandCreateItemOnFloor(int nCommand, int nArgs);
    int ExecuteCommandSetAvailableNPCID(int nCommand, int nArgs);
    int ExecuteCommandIsMoviePlaying(int nCommand, int nArgs);
    int ExecuteCommandQueueMovie(int nCommand, int nArgs);
    int ExecuteCommandPlayMovieQueue(int nCommand, int nArgs);
    int ExecuteCommandYavinHackCloseDoor(int nCommand, int nArgs);
    int ExecuteCommandAddJournalQuestEntry(int nCommand, int nArgs);
    int ExecuteCommandAddJournalWorldEntry(int nCommand, int nArgs);
    int ExecuteCommandAddJournalWorldEntryStrref(int nCommand, int nArgs);
    int ExecuteCommandBarkString(int nCommand, int nArgs);
    int ExecuteCommandDeleteJournalWorldAllEntries(int nCommand, int nArgs);
    int ExecuteCommandDeleteJournalWorldEntry(int nCommand, int nArgs);
    int ExecuteCommandDeleteJournalWorldEntryStrref(int nCommand, int nArgs);
    int ExecuteCommandDoTouchAttack(int nCommand, int nArgs);
    int ExecuteCommandGetCurrentAction(int nCommand, int nArgs);
    int ExecuteCommandGetEffect(int nCommand, int nArgs);
    int ExecuteCommandGetInPersistentObject(int nCommand, int nArgs);
    int ExecuteCommandGetItemInInventory(int nCommand, int nArgs);
    int ExecuteCommandGetJournalEntry(int nCommand, int nArgs);
    int ExecuteCommandGetNearestMine(int nCommand, int nArgs);
    int ExecuteCommandGetObjectInShape(int nCommand, int nArgs);
    int ExecuteCommandRemoveEffect(int nCommand, int nArgs);
    int ExecuteCommandRemoveJournalQuestEntry(int nCommand, int nArgs);
    int ExecuteCommandSetCustomToken(int nCommand, int nArgs);
    int ExecuteCommandSetJournalQuestEntryPicture(int nCommand, int nArgs);
    int ExecuteCommandSetMapPinEnabled(int nCommand, int nArgs);
    int ExecuteCommandGetNearestObject(int nCommand, int nArgs);
    int ExecuteCommandGetMineInfo(int nCommand, int nArgs);
    int ExecuteCommandGetMatchedString(int nCommand, int nArgs);
    int ExecuteCommandAurPostString(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetLastEvent(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetLastEventModelName(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetObjectByName(int nCommand, int nArgs);
    int ExecuteCommandSWMG_PlayAnimation(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetLastBulletInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetLastBulletShooter(int nCommand, int nArgs);
    int ExecuteCommandSWMG_AdjustFollowerHitPoints(int nCommand, int nArgs);
    int ExecuteCommandSWMG_DefaultImplementations(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetLastObjectHit(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetObjectName(int nCommand, int nArgs);
    int ExecuteCommandSWMG_IsSomething(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetLastHPChange(int nCommand, int nArgs);
    int ExecuteCommandSWMG_RemoveAnimation(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetCameraClipping(int nCommand, int nArgs);
    int ExecuteCommandSWMG_SetCameraClipping(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetPlayer(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetMiniGameCounts(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetMiniGameObjects(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetFollowerIntInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_StartInvulnerability(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetFollowerFloatInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetFollowerPosition(int nCommand, int nArgs);
    int ExecuteCommandSWMG_SetFollowerIntInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_SetFollowerFloatInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetGunBankStringInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetGunBankIntInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetGunBankFloatInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetTargettingGunBankFloatInfo(int nCommand,
                                                         int nArgs);
    int ExecuteCommandSWMG_SetGunBankStringInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_SetGunBankIntInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_SetGunBankFloatInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_SetTargettingGunBankFloatInfo(int nCommand,
                                                         int nArgs);
    int ExecuteCommandSWMG_GetLastBulletHitPart(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetPlayerFloatInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetPlayerVectorInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_SetPlayerFloatInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_SetPlayerVectorInfo(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetSoundProperty(int nCommand, int nArgs);
    int ExecuteCommandSWMG_SetSoundProperty(int nCommand, int nArgs);
    int ExecuteCommandSWMG_SetLateralAccelPerSec(int nCommand, int nArgs);
    int ExecuteCommandSWMG_GetLateralAccelPerSec(int nCommand, int nArgs);
    void InitializeSWMGCommands();
    int ExecuteCommandExploreAreaForPlayer(int nCommand, int nArgs);
    int ExecuteCommandGetLastAssociateCommand(int nCommand, int nArgs);
    int ExecuteCommandSetAssociateListenPatterns(int nCommand, int nArgs);
};

#endif
