#ifndef __RESGFF_H__
#define __RESGFF_H__

class CExoFile;
class CExoLocString;
class CResRef;
#include "exobase.h"
#include "exoresman.h"

class Vector;
class Quaternion;

class CResStruct
{
public:
    unsigned long m_nStructIndex;
};

class CResList
{
public:
    unsigned long m_nStructIndex;
    char m_szLabel[16];
};

class CResGFFFieldIDHash
{
public:
    static void Initialize();
    static unsigned long Hash(const char *szLabel, unsigned long nLength);

    static unsigned long m_nCRC32[256];
};

class CResGFFHeader
{
public:
    unsigned long m_nFileType;
    unsigned long m_nVersion;
    unsigned long m_nStructOffset;
    unsigned long m_nStructCount;
    unsigned long m_nFieldOffset;
    unsigned long m_nFieldCount;
    unsigned long m_nLabelOffset;
    unsigned long m_nLabelCount;
    unsigned long m_nFieldDataOffset;
    unsigned long m_nFieldDataCount;
    unsigned long m_nFieldIndicesOffset;
    unsigned long m_nFieldIndicesCount;
    unsigned long m_nListIndicesOffset;
    unsigned long m_nListIndicesCount;
};

class CResGFFStruct
{
public:
    unsigned long m_nType;
    unsigned long m_nDataOrDataOffset;
    unsigned long m_nFieldCount;
};

class CResGFFField
{
public:
    unsigned long m_nType;
    unsigned long m_nLabelIndex;
    union
    {
        unsigned long m_nDataOrDataOffset;
        unsigned char m_nByte;
        char m_cChar;
        unsigned short m_nWord;
        short m_nShort;
        float m_fFloat;
    };
};

class CResGFFLabel
{
public:
    unsigned long m_anName[4];
};

class CResGFF : public CRes
{
public:
    static const int STRUCT_STARTSIZE;
    static const int FIELD_STARTSIZE;
    static const int LABEL_STARTSIZE;
    static const int DATAFIELD_STARTSIZE;
    static const int DATALAYOUTFIELD_STARTSIZE;
    static const int DATALAYOUTLIST_STARTSIZE;

    enum { FIELD_FLOAT = 8, FIELD_INDEX_INVALID = 0xFFFFFFFF };

    CResGFF();
    CResGFF(unsigned short nResType, char* szFileType, const CResRef& cResRef);
    virtual ~CResGFF();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    void ReleaseResource();
    CResGFFStruct* GetStruct(CResStruct* pStructure);

    CResGFFField* GetField(CResGFFStruct* pStruct, unsigned long nIndex);
    CResGFFField* GetField(CResStruct* pStructure, unsigned long nIndex);
    unsigned char* GetDataField(CResGFFField* pField, unsigned long& nSize);
    unsigned char* GetDataLayoutList(CResGFFField* pField,
                                     unsigned long& nSize);

    int InitializeForWriting();
    int AddStruct(unsigned long nStructType);
    int AddLabel(char* szLabel);
    unsigned char* AddDataField(unsigned long nOldOffset,
                                unsigned long nOldSize, unsigned long nSize);
    unsigned char* AddDataLayoutField(unsigned long nOldOffset,
                                      unsigned long nOldSize,
                                      unsigned long nSize);
    unsigned char* AddDataLayoutList(unsigned long nOldOffset,
                                     unsigned long nOldSize,
                                     unsigned long nSize);
    int GetFileData(char* szFileName);
    int GetElementType(CResStruct* pStructure);
    int GetFieldCount(CResStruct* pStructure);
    int GetTopLevelStruct(CResStruct* pStructure);
    int CreateGFFFile(CResStruct* pStructure, const CExoString& sFileType,
                      const CExoString& sVersion);
    int WriteGFFData(CExoFile& file, unsigned long& nSize);
    int WriteGFFFile(char* szFileName);
    int GetTotalSize();
    unsigned long GetFieldByLabel(CResStruct* pStructure, char* szLabel);
    CResGFFField* AddField(CResStruct* pStructure, char* szLabel,
                           unsigned long nFieldType);
    int GetFieldType(CResStruct* pStructure, char* szLabel,
                     unsigned long nIndex);
    unsigned long GetFieldSize(CResStruct* pStructure, char* szLabel,
                               unsigned long nIndex);

    int GetList(CResList* pList, CResStruct* pStructure, char* szLabel);
    unsigned long GetListCount(CResList* pList);
    int GetListElement(CResStruct* pStructure, CResList* pList,
                       unsigned long nIndex);
    int GetStructFromStruct(CResStruct* pNewStruct, CResStruct* pParent,
                            char* szLabel);
    int AddStructToStruct(CResStruct* pNewStruct, CResStruct* pParent,
                          char* szLabel, unsigned long nStructType);
    int AddList(CResList* pList, CResStruct* pStructure, char* szLabel);
    int AddListElement(CResStruct* pStructure, CResList* pList,
                       unsigned long nStructType);

    unsigned char ReadFieldBYTE(CResStruct* pStructure, char* szLabel,
                                int& bSuccess, unsigned char nDefault);
    char ReadFieldCHAR(CResStruct* pStructure, char* szLabel, int& bSuccess,
                       char nDefault);
    unsigned short ReadFieldWORD(CResStruct* pStructure, char* szLabel,
                                 int& bSuccess, unsigned short nDefault);
    short ReadFieldSHORT(CResStruct* pStructure, char* szLabel, int& bSuccess,
                         short nDefault);
    unsigned long ReadFieldDWORD(CResStruct* pStructure, char* szLabel,
                                 int& bSuccess, unsigned long nDefault);
    int ReadFieldINT(CResStruct* pStructure, char* szLabel, int& bSuccess,
                     int nDefault);
    float ReadFieldFLOAT(CResStruct* pStructure, char* szLabel, int& bSuccess,
                         float fDefault);
    unsigned __int64 ReadFieldDWORD64(CResStruct* pStructure, char* szLabel,
                                      int& bSuccess, unsigned __int64 nDefault);
    CResRef ReadFieldCResRef(CResStruct* pStructure, char* szLabel,
                             int& bSuccess, const CResRef& cDefault);
    CExoString ReadFieldCExoString(CResStruct* pStructure, char* szLabel,
                                   int& bSuccess, const CExoString& sDefault);
    CExoLocString ReadFieldCExoLocString(CResStruct* pStructure, char* szLabel,
                                         int& bSuccess,
                                         const CExoLocString& lsDefault);
    Quaternion ReadFieldQuaternion(CResStruct* pStructure, char* szLabel,
                                   int& bSuccess, const Quaternion& qDefault);
    Vector ReadFieldVector(CResStruct* pStructure, char* szLabel,
                           int& bSuccess, const Vector& vDefault);
    void* ReadFieldVOID(CResStruct* pStructure, void* pBuffer,
                        unsigned long nSize, char* szLabel, int& bSuccess,
                        void* pDefault);

    BOOL WriteFieldBYTE(CResStruct* pStructure, unsigned char nValue,
                        char* szLabel);
    BOOL WriteFieldCHAR(CResStruct* pStructure, char cValue, char* szLabel);
    BOOL WriteFieldWORD(CResStruct* pStructure, unsigned short wValue,
                        char* szLabel);
    BOOL WriteFieldSHORT(CResStruct* pStructure, short nValue, char* szLabel);
    BOOL WriteFieldDWORD(CResStruct* pStructure, unsigned long lValue,
                         char* szLabel);
    BOOL WriteFieldINT(CResStruct* pStructure, int nValue, char* szLabel);
    BOOL WriteFieldDWORD64(CResStruct* pStructure, unsigned __int64 nValue,
                           char* szLabel);
    BOOL WriteFieldFLOAT(CResStruct* pStructure, float fValue, char* szLabel);
    BOOL WriteFieldCResRef(CResStruct* pStructure, const CResRef& cValue,
                           char* szLabel);
    BOOL WriteFieldCExoString(CResStruct* pStructure, const CExoString& sValue,
                              char* szLabel);
    BOOL WriteFieldCExoLocString(CResStruct* pStructure,
                                 const CExoLocString& lsValue, char* szLabel);
    BOOL WriteFieldVOID(CResStruct* pStructure, const void* pData,
                        unsigned long nSize, char* szLabel);
    BOOL WriteFieldQuaternion(CResStruct* pStructure, const Quaternion& qValue,
                              char* szLabel);
    BOOL WriteFieldVector(CResStruct* pStructure, const Vector& vValue,
                          char* szLabel);

    void Pack(unsigned char nFlag, unsigned long nSize);
    int WriteGFFFile(const CExoString& sFileName, unsigned short nResType);

    static char* VERSION;

    unsigned long m_nStructAllocInc;
    unsigned long m_nFieldAllocInc;
    unsigned long m_nLabelAllocInc;
    unsigned long m_nFieldDataAllocInc;
    unsigned long m_nFieldIndicesAllocInc;
    unsigned long m_nListIndicesAllocInc;
    CResGFFHeader* m_pHeader;
    CResGFFStruct* m_pStructs;
    unsigned long m_nStructsAlloc;
    CResGFFField* m_pFields;
    unsigned long m_nFieldsAlloc;
    CResGFFLabel* m_pLabels;
    unsigned long m_nLabelsAlloc;
    unsigned char* m_pFieldData;
    unsigned long m_nFieldDataAlloc;
    unsigned char* m_pFieldIndices;
    unsigned long m_nFieldIndicesAlloc;
    unsigned long m_nFieldIndicesLost;
    unsigned char* m_pListIndices;
    unsigned long m_nListIndicesAlloc;
    unsigned long m_nListIndicesLost;
    unsigned char m_aUnmapped7C[16];
    char m_cFlags;
    char m_szFileType[4];
    unsigned char m_aPad91[3];
    int m_bLoaded;
    int m_bInResource;
    int m_bDemanded;
};

#endif
