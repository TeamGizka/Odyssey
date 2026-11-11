#ifndef __NWSCRIPTINTERNAL_H__
#define __NWSCRIPTINTERNAL_H__

#include "exobase.h"
#include "exoresman.h"

#define CSCRIPTCOMPILER_MAX_TOKEN_LENGTH                    512
#define CSCRIPTCOMPILER_MAX_STACK_ENTRIES                   1024
#define CSCRIPTCOMPILER_MAX_IDENTIFIERS                     4096
#define CSCRIPTCOMPILER_MAX_VARIABLES                       1024
#define CSCRIPTCOMPILER_MAX_STRUCTURES                      256
#define CSCRIPTCOMPILER_MAX_STRUCTURE_FIELDS                4096
#define CSCRIPTCOMPILER_INCLUDE_LEVELS                      16
#define CSCRIPTCOMPILER_MAX_ENGINE_STRUCTURES               10
#define CSCRIPTCOMPILER_SWITCH_LABEL_ARRAY_SIZE             16
#define CSCRIPTCOMPILERIDLISTENTRY_MAX_PARAMETERS           32

#define CSCRIPTCOMPILER_OBJECT_INVALID                      0x7f000000

#define CSCRIPTCOMPILER_TOKEN_UNKNOWN                                  0
#define CSCRIPTCOMPILER_TOKEN_DIVIDE                                   1
#define CSCRIPTCOMPILER_TOKEN_CPLUSCOMMENT                             2
#define CSCRIPTCOMPILER_TOKEN_CCOMMENT                                 3
#define CSCRIPTCOMPILER_TOKEN_INTEGER                                  4
#define CSCRIPTCOMPILER_TOKEN_FLOAT                                    5
#define CSCRIPTCOMPILER_TOKEN_IDENTIFIER                               6
#define CSCRIPTCOMPILER_TOKEN_STRING                                   7
#define CSCRIPTCOMPILER_TOKEN_LOGICAL_AND                              8
#define CSCRIPTCOMPILER_TOKEN_LOGICAL_OR                               9
#define CSCRIPTCOMPILER_TOKEN_MINUS                                    10
#define CSCRIPTCOMPILER_TOKEN_LEFT_BRACE                               11
#define CSCRIPTCOMPILER_TOKEN_RIGHT_BRACE                              12
#define CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET                             13
#define CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET                            14
#define CSCRIPTCOMPILER_TOKEN_SEMICOLON                                15
#define CSCRIPTCOMPILER_TOKEN_COMMA                                    16
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_IF                               17
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ELSE                             18
#define CSCRIPTCOMPILER_TOKEN_EOF                                      19
#define CSCRIPTCOMPILER_TOKEN_COND_GREATER_EQUAL                       20
#define CSCRIPTCOMPILER_TOKEN_COND_LESS_EQUAL                          21
#define CSCRIPTCOMPILER_TOKEN_COND_GREATER_THAN                        22
#define CSCRIPTCOMPILER_TOKEN_COND_LESS_THAN                           23
#define CSCRIPTCOMPILER_TOKEN_COND_NOT_EQUAL                           24
#define CSCRIPTCOMPILER_TOKEN_COND_EQUAL                               25
#define CSCRIPTCOMPILER_TOKEN_PLUS                                     26
#define CSCRIPTCOMPILER_TOKEN_MODULUS                                  27
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_EQUAL                         28
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_INT                              29
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT                            30
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING                           31
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT                           32
#define CSCRIPTCOMPILER_TOKEN_VARIABLE                                 33
#define CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER                       34
#define CSCRIPTCOMPILER_TOKEN_FLOAT_IDENTIFIER                         35
#define CSCRIPTCOMPILER_TOKEN_STRING_IDENTIFIER                        36
#define CSCRIPTCOMPILER_TOKEN_OBJECT_IDENTIFIER                        37
#define CSCRIPTCOMPILER_TOKEN_VOID_IDENTIFIER                          38
#define CSCRIPTCOMPILER_TOKEN_INCLUSIVE_OR                             39
#define CSCRIPTCOMPILER_TOKEN_EXCLUSIVE_OR                             40
#define CSCRIPTCOMPILER_TOKEN_BOOLEAN_AND                              41
#define CSCRIPTCOMPILER_TOKEN_SHIFT_LEFT                               42
#define CSCRIPTCOMPILER_TOKEN_SHIFT_RIGHT                              43
#define CSCRIPTCOMPILER_TOKEN_MULTIPLY                                 44
#define CSCRIPTCOMPILER_TOKEN_HEX_INTEGER                              45
#define CSCRIPTCOMPILER_TOKEN_UNSIGNED_SHIFT_RIGHT                     46
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ACTION                           47
#define CSCRIPTCOMPILER_TOKEN_TILDE                                    48
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_RETURN                           49
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_WHILE                            50
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_FOR                              51
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_DO                               52
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID                             53
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT                           54
#define CSCRIPTCOMPILER_TOKEN_STRUCTURE_PART_SPECIFY                   55
#define CSCRIPTCOMPILER_TOKEN_STRUCTURE_IDENTIFIER                     56
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_INCLUDE                          57
#define CSCRIPTCOMPILER_TOKEN_BOOLEAN_NOT                              58
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_VECTOR                           59
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_DEFINE                           60
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_NUM_STRUCTURES_DEFINITION 61
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE_DEFINITION      62
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0                63
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE1                64
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE2                65
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE3                66
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE4                67
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE5                68
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE6                69
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE7                70
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE8                71
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9                72
#define CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE0_IDENTIFIER             73
#define CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE1_IDENTIFIER             74
#define CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE2_IDENTIFIER             75
#define CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE3_IDENTIFIER             76
#define CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE4_IDENTIFIER             77
#define CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE5_IDENTIFIER             78
#define CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE6_IDENTIFIER             79
#define CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE7_IDENTIFIER             80
#define CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE8_IDENTIFIER             81
#define CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE9_IDENTIFIER             82
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT_SELF                      83
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT_INVALID                   84
#define CSCRIPTCOMPILER_TOKEN_VECTOR_IDENTIFIER                        85
#define CSCRIPTCOMPILER_TOKEN_LEFT_SQUARE_BRACKET                      86
#define CSCRIPTCOMPILER_TOKEN_RIGHT_SQUARE_BRACKET                     87
#define CSCRIPTCOMPILER_TOKEN_INCREMENT                                88
#define CSCRIPTCOMPILER_TOKEN_DECREMENT                                89
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MINUS                         90
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_PLUS                          91
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MULTIPLY                      92
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_DIVIDE                        93
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MODULUS                       94
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_AND                           95
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_XOR                           96
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_OR                            97
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_SHIFT_LEFT                    98
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_SHIFT_RIGHT                   99
#define CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_USHIFT_RIGHT                  100
#define CSCRIPTCOMPILER_TOKEN_QUESTION_MARK                            101
#define CSCRIPTCOMPILER_TOKEN_COLON                                    102
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_CASE                             103
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_BREAK                            104
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_SWITCH                           105
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_DEFAULT                          106
#define CSCRIPTCOMPILER_TOKEN_KEYWORD_CONTINUE                         107

#define CSCRIPTCOMPILER_GRAMMAR_PROGRAM                                0
#define CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT                        1
#define CSCRIPTCOMPILER_GRAMMAR_AFTER_PROGRAM                          2
#define CSCRIPTCOMPILER_GRAMMAR_FUNCTION_PARAM_LIST                    3
#define CSCRIPTCOMPILER_GRAMMAR_AFTER_FUNCTION_PARAM                   4
#define CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_LIST                     5
#define CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_VARLIST                  6
#define CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_VARLIST_SEPARATOR        7
#define CSCRIPTCOMPILER_GRAMMAR_WITHIN_COMPOUND_STATEMENT              8
#define CSCRIPTCOMPILER_GRAMMAR_WITHIN_STATEMENT_LIST                  9
#define CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT                     10
#define CSCRIPTCOMPILER_GRAMMAR_ANY_TYPE_SPECIFIER                     11
#define CSCRIPTCOMPILER_GRAMMAR_NON_VOID_TYPE_SPECIFIER                12
#define CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST                           13
#define CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST_SEPARATOR                 14
#define CSCRIPTCOMPILER_GRAMMAR_ARGUMENT_EXPRESSION_LIST               15
#define CSCRIPTCOMPILER_GRAMMAR_AFTER_ARGUMENT_EXPRESSION              16
#define CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_EXPRESSION                     17
#define CSCRIPTCOMPILER_GRAMMAR_NON_VOID_EXPRESSION                    18
#define CSCRIPTCOMPILER_GRAMMAR_EXPRESSION                             19
#define CSCRIPTCOMPILER_GRAMMAR_ASSIGNMENT_EXPRESSION                  20
#define CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION                 21
#define CSCRIPTCOMPILER_GRAMMAR_LOGICAL_OR_EXPRESSION                  22
#define CSCRIPTCOMPILER_GRAMMAR_LOGICAL_AND_EXPRESSION                 23
#define CSCRIPTCOMPILER_GRAMMAR_INCLUSIVE_OR_EXPRESSION                24
#define CSCRIPTCOMPILER_GRAMMAR_EXCLUSIVE_OR_EXPRESSION                25
#define CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_AND_EXPRESSION                 26
#define CSCRIPTCOMPILER_GRAMMAR_EQUALITY_EXPRESSION                    27
#define CSCRIPTCOMPILER_GRAMMAR_RELATIONAL_EXPRESSION                  28
#define CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION                       29
#define CSCRIPTCOMPILER_GRAMMAR_ADDITIVE_EXPRESSION                    30
#define CSCRIPTCOMPILER_GRAMMAR_MULTIPLICATIVE_EXPRESSION              31
#define CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION                       32
#define CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION                        33
#define CSCRIPTCOMPILER_GRAMMAR_PRIMARY_EXPRESSION                     34
#define CSCRIPTCOMPILER_GRAMMAR_CONSTANT                               35

#define CSCRIPTCOMPILER_OPERATION_COMPOUND_STATEMENT                   0
#define CSCRIPTCOMPILER_OPERATION_STATEMENT                            1
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION                  2
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_INT                          3
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_FLOAT                        4
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_STRING                       5
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_OBJECT                       6
#define CSCRIPTCOMPILER_OPERATION_VARIABLE_LIST                        7
#define CSCRIPTCOMPILER_OPERATION_VARIABLE                             8
#define CSCRIPTCOMPILER_OPERATION_STATEMENT_LIST                       9
#define CSCRIPTCOMPILER_OPERATION_IF_BLOCK                             10
#define CSCRIPTCOMPILER_OPERATION_IF_CHOICE                            11
#define CSCRIPTCOMPILER_OPERATION_IF_CONDITION                         12
#define CSCRIPTCOMPILER_OPERATION_ACTION                               13
#define CSCRIPTCOMPILER_OPERATION_ACTION_ID                            14
#define CSCRIPTCOMPILER_OPERATION_ASSIGNMENT                           15
#define CSCRIPTCOMPILER_OPERATION_ACTION_ARG_LIST                      16
#define CSCRIPTCOMPILER_OPERATION_CONSTANT_INTEGER                     17
#define CSCRIPTCOMPILER_OPERATION_CONSTANT_FLOAT                       18
#define CSCRIPTCOMPILER_OPERATION_CONSTANT_STRING                      19
#define CSCRIPTCOMPILER_OPERATION_INTEGER_EXPRESSION                   20
#define CSCRIPTCOMPILER_OPERATION_NON_VOID_EXPRESSION                  21
#define CSCRIPTCOMPILER_OPERATION_LOGICAL_OR                           22
#define CSCRIPTCOMPILER_OPERATION_LOGICAL_AND                          23
#define CSCRIPTCOMPILER_OPERATION_INCLUSIVE_OR                         24
#define CSCRIPTCOMPILER_OPERATION_EXCLUSIVE_OR                         25
#define CSCRIPTCOMPILER_OPERATION_BOOLEAN_AND                          26
#define CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL                      27
#define CSCRIPTCOMPILER_OPERATION_CONDITION_NOT_EQUAL                  28
#define CSCRIPTCOMPILER_OPERATION_CONDITION_GEQ                        29
#define CSCRIPTCOMPILER_OPERATION_CONDITION_GT                         30
#define CSCRIPTCOMPILER_OPERATION_CONDITION_LT                         31
#define CSCRIPTCOMPILER_OPERATION_CONDITION_LEQ                        32
#define CSCRIPTCOMPILER_OPERATION_SHIFT_LEFT                           33
#define CSCRIPTCOMPILER_OPERATION_SHIFT_RIGHT                          34
#define CSCRIPTCOMPILER_OPERATION_ADD                                  35
#define CSCRIPTCOMPILER_OPERATION_SUBTRACT                             36
#define CSCRIPTCOMPILER_OPERATION_MULTIPLY                             37
#define CSCRIPTCOMPILER_OPERATION_DIVIDE                               38
#define CSCRIPTCOMPILER_OPERATION_MODULUS                              39
#define CSCRIPTCOMPILER_OPERATION_NEGATION                             40
#define CSCRIPTCOMPILER_OPERATION_ACTION_PARAMETER                     41
#define CSCRIPTCOMPILER_OPERATION_UNSIGNED_SHIFT_RIGHT                 42
#define CSCRIPTCOMPILER_OPERATION_STRUCTURE_PART                       43
#define CSCRIPTCOMPILER_OPERATION_ONES_COMPLEMENT                      44
#define CSCRIPTCOMPILER_OPERATION_WHILE_BLOCK                          45
#define CSCRIPTCOMPILER_OPERATION_WHILE_CHOICE                         46
#define CSCRIPTCOMPILER_OPERATION_WHILE_CONDITION                      47
#define CSCRIPTCOMPILER_OPERATION_DOWHILE_BLOCK                        48
#define CSCRIPTCOMPILER_OPERATION_DOWHILE_CONDITION                    49
#define CSCRIPTCOMPILER_OPERATION_FUNCTIONAL_UNIT                      50
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT                       51
#define CSCRIPTCOMPILER_OPERATION_STRUCTURE_DEFINITION                 52
#define CSCRIPTCOMPILER_OPERATION_FUNCTION_IDENTIFIER                  53
#define CSCRIPTCOMPILER_OPERATION_FUNCTION_DECLARATION                 54
#define CSCRIPTCOMPILER_OPERATION_FUNCTION                             55
#define CSCRIPTCOMPILER_OPERATION_FUNCTION_PARAM_NAME                  56
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_VOID                         57
#define CSCRIPTCOMPILER_OPERATION_RETURN                               58
#define CSCRIPTCOMPILER_OPERATION_BOOLEAN_NOT                          59
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0            60
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE1            61
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE2            62
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE3            63
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE4            64
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE5            65
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE6            66
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE7            67
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE8            68
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE9            69
#define CSCRIPTCOMPILER_OPERATION_CONSTANT_OBJECT                      70
#define CSCRIPTCOMPILER_OPERATION_KEYWORD_VECTOR                       71
#define CSCRIPTCOMPILER_OPERATION_CONSTANT_VECTOR                      72
#define CSCRIPTCOMPILER_OPERATION_GLOBAL_VARIABLES                     73
#define CSCRIPTCOMPILER_OPERATION_POST_INCREMENT                       74
#define CSCRIPTCOMPILER_OPERATION_POST_DECREMENT                       75
#define CSCRIPTCOMPILER_OPERATION_PRE_INCREMENT                        76
#define CSCRIPTCOMPILER_OPERATION_PRE_DECREMENT                        77
#define CSCRIPTCOMPILER_OPERATION_COND_BLOCK                           78
#define CSCRIPTCOMPILER_OPERATION_COND_CHOICE                          79
#define CSCRIPTCOMPILER_OPERATION_COND_CONDITION                       80
#define CSCRIPTCOMPILER_OPERATION_SWITCH_BLOCK                         81
#define CSCRIPTCOMPILER_OPERATION_SWITCH_CONDITION                     82
#define CSCRIPTCOMPILER_OPERATION_DEFAULT                              83
#define CSCRIPTCOMPILER_OPERATION_CASE                                 84
#define CSCRIPTCOMPILER_OPERATION_BREAK                                85
#define CSCRIPTCOMPILER_OPERATION_CONTINUE                             86
#define CSCRIPTCOMPILER_OPERATION_WHILE_CONTINUE                       87

#define CSCRIPTCOMPILER_IDENT_STATE_START_OF_LINE                      0
#define CSCRIPTCOMPILER_IDENT_STATE_AFTER_TYPE_DECLARATION             1
#define CSCRIPTCOMPILER_IDENT_STATE_AFTER_FUNCTION_IDENTIFIER          2
#define CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST                  3
#define CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER         4
#define CSCRIPTCOMPILER_IDENT_STATE_AFTER_CONSTANT_IDENTIFIER          5
#define CSCRIPTCOMPILER_IDENT_STATE_DEFINE_STATEMENT                   6
#define CSCRIPTCOMPILER_IDENT_STATE_DEFINE_NUM_ENGINE_STRUCTURE        7
#define CSCRIPTCOMPILER_IDENT_STATE_DEFINE_SINGLE_ENGINE_STRUCTURE     8
#define CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_PARAMETER                9
#define CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_CONSTANT                 10
#define CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT         11

#define STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER                                  -1
#define STRREF_CSCRIPTCOMPILER_ERROR_FATAL_COMPILER_ERROR                                  -2
#define STRREF_CSCRIPTCOMPILER_ERROR_PROGRAM_COMPOUND_STATEMENT_AT_START                   -3
#define STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_END_COMPOUND_STATEMENT                     -4
#define STRREF_CSCRIPTCOMPILER_ERROR_AFTER_COMPOUND_STATEMENT_AT_END                       -5
#define STRREF_CSCRIPTCOMPILER_ERROR_PARSING_VARIABLE_LIST                                 -6
#define STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER                             -7
#define STRREF_CSCRIPTCOMPILER_ERROR_INVALID_DECLARATION_TYPE                              -8
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_LEFT_BRACKET_ON_EXPRESSION                         -9
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_RIGHT_BRACKET_ON_EXPRESSION                        -10
#define STRREF_CSCRIPTCOMPILER_ERROR_BAD_START_OF_STATEMENT                                -11
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_LEFT_BRACKET_ON_ARG_LIST                           -12
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_RIGHT_BRACKET_ON_ARG_LIST                          -13
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_EXPRESSION                         -14
#define STRREF_CSCRIPTCOMPILER_ERROR_PARSING_ASSIGNMENT_STATEMENT                          -15
#define STRREF_CSCRIPTCOMPILER_ERROR_BAD_LVALUE                                            -16
#define STRREF_CSCRIPTCOMPILER_ERROR_BAD_CONSTANT_TYPE                                     -17
#define STRREF_CSCRIPTCOMPILER_ERROR_IDENTIFIER_LIST_FULL                                  -18
#define STRREF_CSCRIPTCOMPILER_ERROR_NON_INTEGER_ID_FOR_INTEGER_CONSTANT                   -19
#define STRREF_CSCRIPTCOMPILER_ERROR_NON_FLOAT_ID_FOR_FLOAT_CONSTANT                       -20
#define STRREF_CSCRIPTCOMPILER_ERROR_NON_STRING_ID_FOR_STRING_CONSTANT                     -21
#define STRREF_CSCRIPTCOMPILER_ERROR_VARIABLE_ALREADY_USED_WITHIN_SCOPE                    -22
#define STRREF_CSCRIPTCOMPILER_ERROR_VARIABLE_DEFINED_WITHOUT_TYPE                         -23
#define STRREF_CSCRIPTCOMPILER_ERROR_INCORRECT_VARIABLE_STATE_LEFT_ON_STACK                -24
#define STRREF_CSCRIPTCOMPILER_ERROR_NON_INTEGER_EXPRESSION_WHERE_INTEGER_REQUIRED         -25
#define STRREF_CSCRIPTCOMPILER_ERROR_VOID_EXPRESSION_WHERE_NON_VOID_REQUIRED               -26
#define STRREF_CSCRIPTCOMPILER_ERROR_INVALID_PARAMETERS_FOR_ASSIGNMENT                     -27
#define STRREF_CSCRIPTCOMPILER_ERROR_DECLARATION_DOES_NOT_MATCH_PARAMETERS                 -28
#define STRREF_CSCRIPTCOMPILER_ERROR_LOGICAL_OPERATION_HAS_INVALID_OPERANDS                -29
#define STRREF_CSCRIPTCOMPILER_ERROR_EQUALITY_TEST_HAS_INVALID_OPERANDS                    -30
#define STRREF_CSCRIPTCOMPILER_ERROR_COMPARISON_TEST_HAS_INVALID_OPERANDS                  -31
#define STRREF_CSCRIPTCOMPILER_ERROR_SHIFT_OPERATION_HAS_INVALID_OPERANDS                  -32
#define STRREF_CSCRIPTCOMPILER_ERROR_ARITHMETIC_OPERATION_HAS_INVALID_OPERANDS             -33
#define STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_OPERATION_IN_SEMANTIC_CHECK                   -34
#define STRREF_CSCRIPTCOMPILER_ERROR_SCRIPT_TOO_LARGE                                      -35
#define STRREF_CSCRIPTCOMPILER_ERROR_RETURN_STATEMENT_HAS_NO_PARAMETERS                    -36
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_WHILE_AFTER_DO_KEYWORD                             -37
#define STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_DEFINITION_MISSING_NAME                      -38
#define STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_DEFINITION_MISSING_PARAMETER_LIST            -39
#define STRREF_CSCRIPTCOMPILER_ERROR_MALFORMED_PARAMETER_LIST                              -40
#define STRREF_CSCRIPTCOMPILER_ERROR_BAD_TYPE_SPECIFIER                                    -41
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_STRUCTURE                          -42
#define STRREF_CSCRIPTCOMPILER_ERROR_ELLIPSIS_IN_IDENTIFIER                                -43
#define STRREF_CSCRIPTCOMPILER_ERROR_FILE_NOT_FOUND                                        -44
#define STRREF_CSCRIPTCOMPILER_ERROR_INCLUDE_RECURSIVE                                     -45
#define STRREF_CSCRIPTCOMPILER_ERROR_INCLUDE_TOO_MANY_LEVELS                               -46
#define STRREF_CSCRIPTCOMPILER_ERROR_PARSING_RETURN_STATEMENT                              -47
#define STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST                               -48
#define STRREF_CSCRIPTCOMPILER_ERROR_PARSING_FUNCTION_DECLARATION                          -49
#define STRREF_CSCRIPTCOMPILER_ERROR_DUPLICATE_FUNCTION_IMPLEMENTATION                     -50
#define STRREF_CSCRIPTCOMPILER_ERROR_TOKEN_TOO_LONG                                        -51
#define STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_STRUCTURE                                   -52
#define STRREF_CSCRIPTCOMPILER_ERROR_LEFT_OF_STRUCTURE_PART_NOT_STRUCTURE                  -53
#define STRREF_CSCRIPTCOMPILER_ERROR_RIGHT_OF_STRUCTURE_PART_NOT_FIELD_IN_STRUCTURE        -54
#define STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_FIELD_IN_STRUCTURE                          -55
#define STRREF_CSCRIPTCOMPILER_ERROR_STRUCTURE_REDEFINED                                   -56
#define STRREF_CSCRIPTCOMPILER_ERROR_VARIABLE_USED_TWICE_IN_SAME_STRUCTURE                 -57
#define STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_IMPLEMENTATION_AND_DEFINITION_DIFFER         -58
#define STRREF_CSCRIPTCOMPILER_ERROR_MISMATCHED_TYPES                                      -59
#define STRREF_CSCRIPTCOMPILER_ERROR_INTEGER_NOT_AT_TOP_OF_STACK                           -60
#define STRREF_CSCRIPTCOMPILER_ERROR_RETURN_TYPE_AND_FUNCTION_TYPE_MISMATCHED              -61
#define STRREF_CSCRIPTCOMPILER_ERROR_NOT_ALL_CONTROL_PATHS_RETURN_A_VALUE                  -62
#define STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_IDENTIFIER                                  -63
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_FUNCTION_MAIN_IN_SCRIPT                            -64
#define STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_MAIN_MUST_HAVE_VOID_RETURN_VALUE             -65
#define STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_MAIN_MUST_HAVE_NO_PARAMETERS                 -66
#define STRREF_CSCRIPTCOMPILER_ERROR_NON_VOID_FUNCTION_CANNOT_BE_A_STATEMENT               -67
#define STRREF_CSCRIPTCOMPILER_ERROR_BAD_VARIABLE_NAME                                     -68
#define STRREF_CSCRIPTCOMPILER_ERROR_NON_OPTIONAL_PARAMETER_CANNOT_FOLLOW_OPTIONAL_PARAMETER -69
#define STRREF_CSCRIPTCOMPILER_ERROR_TYPE_DOES_NOT_HAVE_AN_OPTIONAL_PARAMETER              -70
#define STRREF_CSCRIPTCOMPILER_ERROR_NON_CONSTANT_IN_FUNCTION_DECLARATION                  -71
#define STRREF_CSCRIPTCOMPILER_ERROR_PARSING_CONSTANT_VECTOR                               -72
#define STRREF_CSCRIPTCOMPILER_ERROR_OPERAND_MUST_BE_AN_INTEGER_LVALUE                     -73
#define STRREF_CSCRIPTCOMPILER_ERROR_CONDITIONAL_REQUIRES_SECOND_EXPRESSION                -74
#define STRREF_CSCRIPTCOMPILER_ERROR_CONDITIONAL_MUST_HAVE_MATCHING_RETURN_TYPES           -75
#define STRREF_CSCRIPTCOMPILER_ERROR_MULTIPLE_DEFAULT_STATEMENTS_WITHIN_SWITCH             -76
#define STRREF_CSCRIPTCOMPILER_ERROR_MULTIPLE_CASE_CONSTANT_STATEMENTS_WITHIN_SWITCH       -77
#define STRREF_CSCRIPTCOMPILER_ERROR_CASE_PARAMETER_NOT_A_CONSTANT_INTEGER                 -78
#define STRREF_CSCRIPTCOMPILER_ERROR_SWITCH_MUST_EVALUATE_TO_AN_INTEGER                    -79
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_COLON_AFTER_DEFAULT_LABEL                          -80
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_COLON_AFTER_CASE_LABEL                             -81
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_STATEMENT                          -82
#define STRREF_CSCRIPTCOMPILER_ERROR_BREAK_OUTSIDE_OF_LOOP_OR_CASE_STATEMENT               -83
#define STRREF_CSCRIPTCOMPILER_ERROR_TOO_MANY_PARAMETERS_ON_FUNCTION                       -84
#define STRREF_CSCRIPTCOMPILER_ERROR_UNABLE_TO_OPEN_FILE_FOR_WRITING                       -85
#define STRREF_CSCRIPTCOMPILER_ERROR_UNTERMINATED_STRING_CONSTANT                          -86
#define STRREF_CSCRIPTCOMPILER_ERROR_NO_FUNCTION_INTSC_IN_SCRIPT                           -87
#define STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_INTSC_MUST_HAVE_VOID_RETURN_VALUE            -88
#define STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_INTSC_MUST_HAVE_NO_PARAMETERS                -89
#define STRREF_CSCRIPTCOMPILER_ERROR_JUMPING_OVER_DECLARATION_STATEMENTS_CASE_DISALLOWED   -90

#define STRREF_CSCRIPTCOMPILER_ERROR_JUMPING_OVER_DECLARATION_STATEMENTS_DEFAULT_DISALLOWED -91

#define CSCRIPTCOMPILER_OUTPUT_CODE_BLOCK_SIZE              0x40000
#define CSCRIPTCOMPILER_OUTPUT_CODE_SLACK                   200
#define CSCRIPTCOMPILER_BINARY_SCRIPT_HEADER                13
#define CSCRIPTCOMPILER_SYMBOLIC_SCRIPT_HEADER              20
#define CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH             13
#define CSCRIPTCOMPILER_SYMBOL_LIST_GROW_SIZE               32
#define CSCRIPTCOMPILER_AUXCODE_TYPE_VOID                   1
#define CSCRIPTCOMPILER_STORE_STATE_RESUME_OFFSET           16

class CResNSS : public CRes
{
public:
    CResNSS();
    virtual ~CResNSS();
    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    char* GetNSSDataPtr();
    unsigned long GetNSSSize();

    int m_bServiced;
    unsigned long m_nNSSSize;
    unsigned char* m_pNSSData;
};

#define STRREF_CSCRIPTCOMPILER_ERROR_ELSE_WITHOUT_CORRESPONDING_IF                         -92

class CScriptParseTreeNode
{
public:
    int nOperation;
    CExoString m_psStringData;
    int nIntegerData;
    int nIntegerData2;
    int nIntegerData3;
    float fFloatData;
    float fVectorData[3];
    int nLine;
    int nChar;
    CScriptParseTreeNode* pLeft;
    CScriptParseTreeNode* pRight;
    int nType;
    CExoString m_psTypeName;
    int m_nStackPointer;

    CScriptParseTreeNode()
    {
        nOperation = 0;
        m_psStringData = "";
        nIntegerData = 0;
        nIntegerData2 = 0;
        nIntegerData3 = 0;
        fFloatData = 0.0f;
        pLeft = NULL;
        pRight = NULL;
        fVectorData[0] = 0.0f;
        fVectorData[1] = 0.0f;
        fVectorData[2] = 0.0f;
        nLine = 0;
        nChar = 0;
        nType = 0;
        m_nStackPointer = 0;
    }
};

class CScriptCompilerStackEntry
{
public:
    int nState;
    int nRule;
    int nTerm;
    CScriptParseTreeNode* pCurrentTree;
    CScriptParseTreeNode* pReturnTree;
};

class CScriptCompilerVarStackEntry
{
public:
    CExoString m_psVarName;
    int m_nVarType;
    int m_nVarLevel;
    int m_nVarRunTimeLocation;
    CExoString m_sVarStructureName;

    CScriptCompilerVarStackEntry()
    {
        m_nVarType = 0;
        m_nVarLevel = 0;
        m_nVarRunTimeLocation = 0;
    }
    // PC 0x0061CD80
    ~CScriptCompilerVarStackEntry()
    {
    }
};

class CScriptCompilerStructureFieldEntry
{
public:
    char m_pchType;
    CExoString m_psStructureName;
    CExoString m_psVarName;
    int m_nLocation;

    CScriptCompilerStructureFieldEntry()
    {
        m_pchType = 0;
        m_nLocation = 0;
    }
    // PC 0x0061CDE0
    ~CScriptCompilerStructureFieldEntry()
    {
    }
};

class CScriptCompilerIdListEntry
{
public:
    CExoString m_psIdentifier;
    int m_nIdentifierLength;
    int m_nIdentifierType;
    int m_nReturnType;
    BOOL m_bImplementationInPlace;
    CExoString m_psStructureReturnName;

    CExoString m_psStringData;
    int m_nIntegerData;
    float m_fFloatData;
    float m_fVectorData[3];

    int m_nIdIdentifier;
    int m_nParameters;
    int m_nNonOptionalParameters;
    int m_nParameterSpace;
    char* m_pchParameters;
    CExoString* m_psStructureParameterNames;
    BOOL* m_pbOptionalParameters;
    int* m_pnOptionalParameterIntegerData;
    float* m_pfOptionalParameterFloatData;
    CExoString* m_psOptionalParameterStringData;
    unsigned long* m_poidOptionalParameterObjectData;
    float* m_pfOptionalParameterVectorData;

    int m_nBinarySourceStart;
    int m_nBinarySourceFinish;
    int m_nBinaryDestinationStart;
    int m_nBinaryDestinationFinish;

    CScriptCompilerIdListEntry();
    ~CScriptCompilerIdListEntry();
    int ExpandParameterSpace();
};

class CSourceScript : public CResHelper<CResNSS, RESTYPE_NSS>
{
public:
    int LoadScript(const CExoString& sFileName, char** ppScript, unsigned long* pnScriptLength);
    void UnloadScript();
};

class CScriptCompilerIncludeFileStackEntry
{
public:
    CExoString m_sFileName;
    CSourceScript m_cSourceScript;

    int m_nLine;
    int m_nCharacterOnLine;
    int m_nTokenStatus;
    int m_nTokenCharacters;
    // PC 0x0061CFC0
    ~CScriptCompilerIncludeFileStackEntry()
    {
    }
};

class CScriptCompilerStructureEntry
{
public:
    CExoString m_psName;
    int m_nFieldStart;
    int m_nFieldEnd;
    int m_nByteSize;

    CScriptCompilerStructureEntry()
    {
        m_nFieldStart = 0;
        m_nFieldEnd = 0;
        m_nByteSize = 0;
    }
};

class CScriptCompilerSymbolTableEntry
{
public:
    CExoString m_sSymbolName;
    int m_nLocationPointer;

    CScriptCompilerSymbolTableEntry()
    {
        m_sSymbolName = "";
        m_nLocationPointer = 0;
    }
};

class CScriptCompilerInternal
{
public:
    int m_nCompileDebugLevel;
    BOOL m_bCompileConditionalFile;
    CExoString m_sLanguageSource;
    CExoString m_sOutputAlias;

    int m_nLines;
    int m_nCharacterOnLine;
    int m_nTokenStatus;
    int m_nTokenCharacters;
    char m_pchToken[CSCRIPTCOMPILER_MAX_TOKEN_LENGTH];

    CScriptCompilerStackEntry* m_pSRStack;
    int m_nSRStackEntries;
    int m_nSRStackStates;

    BOOL m_bCompileIdentifierList;
    BOOL m_bCompileIdentifierConstants;
    int m_nIdentifierListState;
    int m_nIdentifierListVector;
    int m_nIdentifierListEngineStructure;
    int m_nIdentifierListReturnType;
    CScriptCompilerIdListEntry* m_pcIdentifierList;
    int m_nOccupiedIdentifiers;
    int m_nMaxPredefinedIdentifierId;
    int m_nPredefinedIdentifierOrder;

    int m_nCompileFileLevel;
    CScriptCompilerIncludeFileStackEntry m_pcIncludeFileStack[CSCRIPTCOMPILER_INCLUDE_LEVELS];

    int m_nVarStackRecursionLevel;
    CScriptCompilerVarStackEntry* m_pcVarStackList;
    int m_nOccupiedVariables;
    int m_nVarStackVariableType;
    CExoString m_sVarStackVariableTypeName;

    CScriptCompilerStructureEntry* m_pcStructList;
    CScriptCompilerStructureFieldEntry* m_pcStructFieldList;
    int m_nMaxStructures;
    int m_nMaxStructureFields;
    int m_nStructureDefinition;
    int m_nStructureDefinitionFieldStart;

    BOOL m_bGlobalVariableDefinition;
    int m_nGlobalVariables;
    int m_nGlobalVariableSize;
    CScriptParseTreeNode* m_pGlobalVariableParseTree;

    int m_nLoopIdentifier;
    int m_nSwitchLevel;
    int m_nSwitchIdentifier;
    int m_nSwitchStackDepth;
    CExoString m_sUndefinedIdentifier;
    BOOL m_bSwitchLabelDefault;
    int m_nSwitchLabelNumber;
    int m_nSwitchLabelArraySize;
    int* m_pnSwitchLabelStatements;

    int m_nNumEngineDefinedStructures;
    BOOL* m_pbEngineDefinedStructureValid;
    CExoString* m_psEngineDefinedStructureName;

    BOOL m_bAssignmentToVariable;
    BOOL m_bInStructurePart;
    BOOL m_bFunctionImp;
    CExoString m_sFunctionImpName;
    int m_nFunctionImpReturnType;
    CExoString m_sFunctionImpReturnStructureName;
    int m_nFunctionImpAbortStackPointer;
    int m_nStackCurrentDepth;
    char m_pchStackTypes[CSCRIPTCOMPILER_MAX_TOKEN_LENGTH];
    int m_nRunTimeIntegers;
    int m_nRunTimeFloats;
    int m_nRunTimeStrings;
    int m_nRunTimeObjects;
    int m_nRunTimeActions;

    int m_nSymbolQueryListSize;
    int m_nSymbolQueryList;
    CScriptCompilerSymbolTableEntry* m_pSymbolQueryList;
    int m_nSymbolLabelListSize;
    int m_nSymbolLabelList;
    CScriptCompilerSymbolTableEntry* m_pSymbolLabelList;

    int m_nCompileSymbolicOutput;
    BOOL m_bOptimizeBinaryCodeLength;
    int m_nTotalCompileNodes;
    BOOL m_bCompilingConditional;
    char* m_pchOutputCode;
    int m_nOutputCodeSize;
    int m_nOutputCodeLength;
    int m_nBinaryCodeLength;
    char m_pchActionParameters[CSCRIPTCOMPILERIDLISTENTRY_MAX_PARAMETERS];
    CExoString m_pchActionParameterStructureNames[CSCRIPTCOMPILERIDLISTENTRY_MAX_PARAMETERS];
    int m_nFinalBinarySize;

    int WriteResolvedOutput();
    void InitializeFinalCode();
    void FinalizeFinalCode();
    int WriteFinalCodeToFile(const CExoString& sFileName);
    void ClearUserDefinedIdentifiers();
    int OutputIdentifierError(const CExoString& sFunctionName, int nError);
    int GetIdentifierByName(const CExoString& sIdentifierName);
    int ValidateLocationOfIdentifier(const CExoString& sFunctionName);
    int DetermineLocationOfCode();
    int ResolveLabels();
    int GetStructureField(const CExoString& sStructureName, const CExoString& sFieldName);
    int FoundReturnStatementOnAllBranches(CScriptParseTreeNode* pNode);
    void ClearAllSymbolLists();
    int CleanUpAfterCompile(int nReturnValue, CScriptParseTreeNode* pReturnTree);
    int AddSymbolToLabelList(const CExoString& sSymbolName, int nLocationPointer);
    int AddSymbolToQueryList(const CExoString& sSymbolName, int nLocationPointer);
    int OutputWalkTreeError(int nError, CScriptParseTreeNode* pNode);
    int TraverseTreeForSwitchLabels(CScriptParseTreeNode* pNode);
    void ClearSwitchLabelList();
    int GenerateCodeForSwitchLabels(CScriptParseTreeNode* pNode);
    void AddStructureToStack(const CExoString& sStructureName, BOOL bGenerateCode);
    void AddVariableToStack(int nVariableType, const CExoString& sVariableTypeName, BOOL bGenerateCode);
    int PostVisitGenerateCode(CScriptParseTreeNode* pNode);
    int InVisitGenerateCode(CScriptParseTreeNode* pNode);
    int PreVisitGenerateCode(CScriptParseTreeNode* pNode);
    int WalkParseTree(CScriptParseTreeNode* pNode);
    int InstallLoader();
    int GenerateFinalCodeFromParseTree();
    CScriptParseTreeNode* InsertGlobalVariablesInParseTree(CScriptParseTreeNode* pOldTree);
    void PrintBinaryAddress();
    void InitializeSwitchLabelList();
    int GetStructureSize(const CExoString& sStructureName);

    int GenerateIdentifierList();
    int PrintParseIdentifierFileError(int nChar, int nError);
    int ParseIdentifierFile();

    float ParseFloatFromTokenString();
    int ParseCharacterNumeric(int nChar);
    int ParseCharacterAlphabet(int nChar);
    int ParseCommentedOutCharacter(int nChar);
    int HandleToken();
    int ParseCharacterColon();
    int ParseCharacterQuestionMark();
    int ParseCharacterEllipsis();
    int ParseCharacterComma();
    int ParseCharacterTilde();
    int ParseCharacterCarat(int nCharNext);
    int ParseCharacterSemicolon();
    int ParseCharacterPercentSign(int nCharNext);
    int ParseCharacterPlusSign(int nCharNext);
    int ParseCharacterEqualSign(int nCharNext);
    int ParseCharacterExclamationPoint(int nCharNext);
    int ParseCharacterLeftAngle(int nCharNext);
    int ParseCharacterRightAngle(int nCharNext);
    int ParseCharacterRightSquareBracket();
    int ParseCharacterLeftSquareBracket();
    int ParseCharacterRightBracket();
    int ParseCharacterLeftBracket();
    int ParseCharacterRightBrace();
    int ParseCharacterLeftBrace();
    int ParseCharacterHyphen(int nCharNext);
    int ParseCharacterQuotationMark();
    int ParseCharacterAsterisk(int nCharNext);
    int ParseCharacterVerticalBar(int nCharNext);
    int ParseCharacterAmpersand(int nCharNext);
    int ParseCharacterSlash(int nCharNext);
    int TestIdentifierToken();
    int HandleIdentifierToken();
    int ParseCharacterPeriod();
    int ParseStringCharacter(int nChar, int nCharNext);
    int ParseNextCharacter(int nChar, int nCharNext);

    void InitializePreDefinedStructures();
    CScriptCompilerInternal();
    void Initialize();
    void InitializeIncludeFile(int nCompileFileLevel);
    void ShutdownIncludeFile(int nCompileFileLevel);
    void DeleteParseTree(BOOL bStack, CScriptParseTreeNode* pNode);
    int OutputError(int nError, const CExoString& sFileName, int nLineNumber,
                    const CExoString& sErrorText);
    void ShutDown();
    ~CScriptCompilerInternal();
    void SetCompileDebugLevel(int nValue);
    void SetCompileSymbolicOutput(int nValue);
    void SetIdentifierSpecification(const CExoString& sLanguageSource);
    void SetOutputAlias(const CExoString& sAlias);
    void SetOptimizeBinaryCodeLength(BOOL bValue);
    void SetCompileConditionalFile(BOOL bValue);
    int CompileFile(const CExoString& sFileName);
    int CompileScriptChunk(const CExoString& sScriptChunk);
    int CompileScriptConditional(const CExoString& sScriptConditional);
    int GetCompiledScriptCode(char** ppnCode, int* pnCodeSize);
    void ClearCompiledScriptCode();

    void TokenInitialize();
    void PushSRStack(int nState, int nRule, int nTerm, CScriptParseTreeNode* pCurrentTree);
    int PopSRStack(int* pnState, int* pnRule, int* pnTerm, CScriptParseTreeNode** ppCurrentTree,
                   CScriptParseTreeNode** ppReturnTree);
    void ModifySRStackReturnTree(CScriptParseTreeNode* pReturnTree);
    CScriptParseTreeNode* DuplicateScriptParseTree(CScriptParseTreeNode* pNode);
    void DeleteCompileStack();
    int CleanUpDuringCompile(int nReturnValue);
    int PrintParseSourceError(int nChar, int nError);
    int ParseSource(char* pScript, int nScriptLength);
    int AddUserDefinedIdentifier(CScriptParseTreeNode* pFunctionDeclaration, BOOL bFunctionImplementation);
    CScriptParseTreeNode* CreateScriptParseTreeNode(int nNodeOperation, CScriptParseTreeNode* pNodeLeft,
                                                    CScriptParseTreeNode* pNodeRight);
    int AddToGlobalVariableList(CScriptParseTreeNode* pGlobalVariableNode);
    BOOL CheckForBadLValue(CScriptParseTreeNode* pNode);
    int GenerateParseTree();
};

#endif
