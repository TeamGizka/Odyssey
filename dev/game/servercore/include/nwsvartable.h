#ifndef __NWSVARTABLE_H__
#define __NWSVARTABLE_H__

class CScriptLocation;
#include "exobase.h"

class CResGFF;
class CResStruct;

#define SWVARTABLE_LOCAL_NUMBER_MIN -128
#define SWVARTABLE_LOCAL_NUMBER_MAX 127

class CSWVarTable
{
public:
    CSWVarTable();

    int GetLocalBoolean(int nIndex);
    void SetLocalBoolean(int nIndex, int bValue);
    unsigned char GetLocalNumber(int nIndex);
    void SetLocalNumber(int nIndex, unsigned char nValue);
    void SaveVarTable(CResGFF* pRes, CResStruct* pStruct);
    void LoadVarTable(CResGFF* pRes, CResStruct* pStruct);

    unsigned long m_nLocalBoolean[3];

    union
    {
        unsigned char m_aLocalNumber[8];
        unsigned long m_anLocalNumber[2];
    };
};

class CResGFF;
class CResStruct;
class CSWSCreature;

enum
{
    SCRIPTVAR_TYPE_INT = 1,
    SCRIPTVAR_TYPE_FLOAT = 2,
    SCRIPTVAR_TYPE_STRING = 3,
    SCRIPTVAR_TYPE_OBJECT = 4,
    SCRIPTVAR_TYPE_LOCATION = 5
};

class CSWSScriptVar
{
public:
    CSWSScriptVar()  // PC 0x0059EA90
    {
    }

    CSWSScriptVar(const CSWSScriptVar& var)
        : m_sName(var.m_sName), m_nType(var.m_nType), m_nValue(var.m_nValue)
    {
    }

    CSWSScriptVar& operator=(const CSWSScriptVar& var)
    {
        m_sName = var.m_sName;
        m_nType = var.m_nType;
        m_nValue = var.m_nValue;
        return *this;
    }

    CExoString m_sName;
    int m_nType;
    union
    {
        int m_nValue;
        float m_fValue;
        unsigned long m_oidValue;
        CExoString* m_psValue;
        CScriptLocation* m_plValue;
    };
};

class CSWSScriptVarTable : public CExoArrayList<CSWSScriptVar>
{
public:
    static const int VARTYPE_INTEGER;
    static const int VARTYPE_FLOAT;
    static const int VARTYPE_STRING;
    static const int VARTYPE_OBJECT;
    static const int VARTYPE_LOCATION;

    CSWSScriptVarTable() : m_pCreature(NULL) {}
    ~CSWSScriptVarTable();

    CSWSScriptVar* MatchIndex(const CExoString& sName, unsigned long nType,
                              int bCreate);
    void DeleteIndex(const CExoString& sName, unsigned long nType);

    int GetInt(const CExoString& sName);
    float GetFloat(const CExoString& sName);
    CExoString GetString(const CExoString& sName);
    unsigned long GetObject(const CExoString& sName);
    CScriptLocation GetLocation(const CExoString& sName);

    void SetInt(const CExoString& sName, int nValue, int nFlag);
    void SetFloat(const CExoString& sName, float fValue);
    void SetString(const CExoString& sName, CExoString& sValue);
    void SetObject(const CExoString& sName, unsigned long oidValue);
    void SetLocation(const CExoString& sName, CScriptLocation lLocation);

    void DestroyFloat(const CExoString& sName);
    void DestroyString(const CExoString& sName);
    void DestroyObject(const CExoString& sName);

    void LoadVarTable(CResGFF* pRes, CResStruct* pStruct);
    void SaveVarTable(CResGFF* pRes, CResStruct* pStruct);

    CSWSCreature* m_pCreature;
};

#include "nwsobject.h"

class CResGFF;
class CResRef;
class CResStruct;

class CSWGlobalVariable
{
public:

    enum Type
    {
        TYPE_BOOLEAN = 0,
        TYPE_NUMBER = 1,
        TYPE_LOCATION = 2,
        TYPE_STRING = 3
    };

    char m_szIdentifier[22];
    unsigned short m_nIndex : 14;
    unsigned short m_nType : 2;
};

class CSWGlobalVariableTable
{
public:
    CSWGlobalVariableTable();
    ~CSWGlobalVariableTable();

    int Hash(const CExoString& sIdentifier);
    int AddIdentifier(const CExoString& sIdentifier, unsigned long nType,
                      unsigned long nIndex);
    int AddIdentifierBoolean(const CExoString& sIdentifier);
    int AddIdentifierNumber(const CExoString& sIdentifier);
    int AddIdentifierLocation(const CExoString& sIdentifier);
    int AddIdentifierString(const CExoString& sIdentifier);
    int FindIdentifierIndex(const CExoString& sIdentifier);
    void SetBit(unsigned char* pValues, int nIndex, int bValue);
    void GetValueBoolean(const CExoString& sIdentifier, int& bValue);
    void GetValueNumber(const CExoString& sIdentifier, char& cValue);
    void GetValueLocation(const CExoString& sIdentifier,
                          CScriptLocation& lValue);
    void GetValueString(const CExoString& sIdentifier, CExoString& sValue);
    void SetValueBoolean(const CExoString& sIdentifier, int bValue);
    void SetValueNumber(const CExoString& sIdentifier, char cValue);
    void SetValueLocation(const CExoString& sIdentifier,
                          const CScriptLocation& lValue);
    void SetValueString(const CExoString& sIdentifier,
                        const CExoString& sValue);
    void WriteTable(const CExoString& sFileName, int bWriteCatalogue);
    void ReadTableWithoutCatalogue(CResGFF* pRes, CResStruct& sStruct);
    void ClearIdentifiers();
    void ClearValues();
    void ReadTableWithCatalogue(CResGFF* pRes, CResStruct& sStruct);
    int ReadTable(const CExoString& sFileName);
    void Save(const CExoString& sDirectory);
    int Load();
    void ReadCatalogue(const CResRef& cResRef);

    CSWGlobalVariable m_aIdentifiers[1775];
    unsigned char m_aBooleanValues[113];
    char m_acNumberValues[500];
    char m_aUnmappedA8CD[3];
    CScriptLocation m_aLocations[100];
    CExoString m_asStrings[5];
    int m_nBooleanCount;
    int m_nNumberCount;
    int m_nLocationCount;
    int m_nStringCount;
    unsigned long m_anCRCTable[256];
    int m_nCollisions;
    int m_nCollisionSteps;
    int m_nMaximumCollisionSteps;
};

#endif
