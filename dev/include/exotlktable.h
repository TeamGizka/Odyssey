#ifndef __EXOTLKTABLE_H__
#define __EXOTLKTABLE_H__

class CExoFile;
#include "exobase.h"
#include "exoresman.h"

struct STR_RES
{
    STR_RES()  // PC 0x004145E0 GOG 0x00414590
    {
    }
    // PC 0x0061CDD0 GOG 0x00635DB0
    ~STR_RES()
    {
    }

    CExoString m_sString;
    CResRef m_resSound;
    float m_fSoundLength;
};

class CTlkFile : public CExoFile
{
public:
    CTlkFile(const CExoString& sResRef);
    int ReadHeader();

    static const unsigned long TLK_FILE_TYPE;

    static const char* TLK_FILE_VERSION;

    struct StringEntry
    {
        unsigned long m_nFlags;
        unsigned char m_resSound[16];
        unsigned long m_nVolumeVariance;
        unsigned long m_nPitchVariance;
        unsigned long m_nOffset;
        unsigned long m_nLength;
        float m_fSoundLength;
    };

    unsigned long m_nFileType;
    unsigned long m_nFileVersion;
    unsigned long m_nLanguageID;
    unsigned long m_nStringCount;
    unsigned long m_nStringEntriesOffset;
};

class CTlkTableToken
{
public:

    enum Type
    {
        GUI_STRING = 0, GENDER_STRING = 1, FULL_NAME = 2,
        FIRST_NAME = 3, LAST_NAME = 4, RACE_NAME = 5,
        RACE_NAME_LOWER = 6, SUBRACE_NAME = 7, CLASS_NAME = 8,
        CLASS_NAME_LOWER = 9, CLASS_LEVEL = 10, ALIGNMENT_NAME = 11,
        ALIGNMENT_NAME_LOWER = 12, ALIGNMENT_STRING = 14,
        YEAR = 15, HOUR = 16, MONTH = 17, TIME_OF_DAY = 18,
        PARSE_BEGIN = 19, PARSE_SKIP = 20, PARSE_EMPTY = 21,
        PARSE_END = 22, PLAYER_NAME = 23, CHARACTER = 24,
        INPUT_CONTROL = 25, MOUSE_BUTTON = 26, BOUNTY = 27
    };
    unsigned long m_nHash;
    CExoString m_sToken;
    int m_nType;
    unsigned long m_nStringRefs[4];
    unsigned long m_nTokenValue;

};

class CTlkTable
{
public:

    enum { LANGUAGE_POLISH = 5, LANGUAGE_ASIAN_START = 1000 };
    static const unsigned long STRING_ID_MASK;
    static const unsigned char INIT_FILESARRAY_SLOTS;
    static const unsigned char FILESARRAY_GROW_SIZE;
    static const unsigned short TEXT_PRESENT;
    static const unsigned short SND_PRESENT;
    static const unsigned short SNDLENGTH_PRESENT;
    static const unsigned short STRING_NOT_IN_PROJECT;
    static const unsigned char GENDER_MALE;
    static const unsigned char NUM_CONTENT_PKGS;

    static const int CUSTOMTOKEN_RESERVED_END;
    static const unsigned char GENDER_FEMALE;

    enum
    {
        CUSTOMTOKEN_PREFIX_LENGTH = 6,
        CUSTOMTOKEN_MIN_LENGTH = CUSTOMTOKEN_PREFIX_LENGTH + 1,
        CUSTOMTOKEN_INVALID = -1
    };
    static const unsigned long STRREF_INVALID = 0xffffffff;
    static const unsigned long STRREF_INDEX_MASK = 0x00ffffff;

    CTlkTable();

    ~CTlkTable();
    int OpenFile(const CExoString& sResRef);
    int AddFile(const CExoString& sResRef, int nContentPackage);
    int Fetch(unsigned long nStrRef, STR_RES& pResult, unsigned char nGender);
    CExoString GetSimpleString(unsigned long nStrRef);
    void ClearCustomTokens();
    void SetCustomToken(int nToken, const CExoString& sValue);
    void ParseStr(CExoString& sText);
    int GetLanguageVersion();
    int ExoIsDBCSLeadByte(unsigned char c);
    // PC 0x006049D0 GOG 0x006049B0
    virtual void GetTokenValue(const CTlkTableToken&, CExoString&)
    {
    }

    struct SCustomToken
    {
        unsigned long m_nToken;
        CExoString m_sValue;
    };

    CTlkFile* m_aFiles[14];
    unsigned char m_nCurGender;
    CTlkTableToken* m_pLanguageTokens;
    int m_nLanguageTokenCount;
    SCustomToken* m_pCustomTokens;
    int m_nCustomTokenCount;
    int m_bParsing;
    int m_bDebugMode;

protected:
    int FetchInternal(unsigned long nStrRef, STR_RES& strRes, int bParseTokens);
};

extern CTlkTable* g_pTlkTable;

#endif
