#ifndef __EXORESMAN_H__
#define __EXORESMAN_H__



#define RESTYPE_BIK 2063


#define RESTYPE_JRL 2056



#define RESTYPE_UTC 2027


enum ExoResourceType
{
    RESTYPE_TGA = 3,
    RESTYPE_WAV = 4,
    RESTYPE_PLT = 6,

    RESTYPE_MP3 = 8,


    RESTYPE_BMU = 8,


    RESTYPE_TXT = 10,


    RESTYPE_MDL = 2002,
    RESTYPE_NSS = 2009,
    RESTYPE_NCS = 2010,


    RESTYPE_MOD = 2011,
    RESTYPE_IFO = 2014,

    RESTYPE_WOK = 2016,
    RESTYPE_2DA = 2017,

    RESTYPE_TLK = 2018,
    RESTYPE_TXI = 2022,

    RESTYPE_GIT = 2023,

    RESTYPE_UTI = 2025,

    RESTYPE_DLG = 2029,
    RESTYPE_DDS = 2033,
    RESTYPE_LTR = 2036,

    RESTYPE_FAC = 2038,

    RESTYPE_UTP = 2044,
    RESTYPE_GUI = 2047,

    RESTYPE_UTM = 2051,


    RESTYPE_SAV = 2057,
    RESTYPE_4PC = 2059,
    RESTYPE_SSF = 2060,


    RESTYPE_NWM = 2062,
    RESTYPE_LYT = 3000,
    RESTYPE_VIS = 3001,

    RESTYPE_RIM = 3002,

    RESTYPE_PTH = 3003,
    RESTYPE_LIP = 3004,
    RESTYPE_TPC = 3007,
    RESTYPE_MDX = 3008,

    RESTYPE_ERF = 9997,
    RESTYPE_BIF = 9998
};


#define RESTYPE_RSV 3009



#define RESTYPE_INVALID 0xFFFF


#define RESTYPE_BIC 2015

class CExoLinkedListInternal;
class CResGFF;

#include "exobase.h"


const unsigned short RESTYPE_ARE = 2012;





class CExoString;

class CResRef
{
public:
    CResRef();
    CResRef(unsigned char* pResRef, unsigned char nLength);
    CResRef(unsigned char* pResRef);
    CResRef(char* pString);
    CResRef(const CExoString& sString);
    unsigned char* GetResRef() const;
    char* GetResRefStr() const;
    void CopyToString(char* pString) const;


    void CopyToString(CExoString& sString) const;
    int IsValid() const;
    unsigned char GetLength() const;
    int operator==(const CResRef& other) const;
    int operator==(char* pString) const;
    int operator!=(char* pString) const;
    CResRef operator=(const CResRef& other);
    CResRef operator=(unsigned char* pResRef);
    CResRef operator=(char* pString);
    CResRef operator=(const CExoString& sString);
    CResRef operator+=(const CExoString& sString);
    int operator!=(const CResRef& other) const;
    int operator!=(const CExoString& sString) const;

    unsigned char m_resRef[16];


    static char RETURN_BUFFERS[4][17];
    static int s_nCurBufferIndex;
};












class CKeyTableEntry;






















class CRes
{
public:



    enum
    {
        FLAG_SERVICED = 0x4,
        RESID_TABLE_SHIFT = 14,
        RESID_TABLE_MASK = 0x3F
    };

    CRes();
    virtual ~CRes();






    // PC 0x00406590 GOG 0x00406560
    virtual int GetFixedResourceSize()
    {
        return -1;
    }
    virtual int GetFixedResourceDataOffset()  // PC 0x004065A0 GOG 0x00406570
    {
        return 0;
    }
    virtual int OnResourceFreed()  // PC 0x0065D0F0 GOG 0x0065CF00
    {
        return 1;
    }
    virtual int OnResourceServiced()  // PC 0x0065D0F0 GOG 0x0065CF00
    {
        return 1;
    }

    void CancelRequest();
    void* Demand();
    void Dump(int bFlag);
    int GetDemands();
    void GetResRef(CResRef& cResRef, unsigned short& nType);
    int ReadRaw(int nSize, char* pBuffer);
    int Release();
    unsigned long GetSize();
    void* GetData();
    int Request();

    unsigned short m_nDemandCount;
    unsigned short m_nRequestCount;
    unsigned long m_nResId;
    unsigned long m_nFlags;
    void* m_pResource;
    CKeyTableEntry* m_pKeyEntry;
    unsigned long m_nSize;
    unsigned long m_nRefCount;



    int m_bAllocPrefix;
    int m_bAllocSuffix;
};



class CRes2DA : public CRes
{
public:
    CRes2DA();
    virtual ~CRes2DA();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    void* Get2DADataPtr();
    unsigned long Get2DASize();

    int m_bServiced;
    unsigned long m_n2DASize;
    void* m_p2DAData;
    unsigned long m_nUnknown34;
    int m_bPacked;
};








class CRes4PC : public CRes
{
public:
    CRes4PC();
    virtual ~CRes4PC();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    int Get4PCAttrib(unsigned long* pnA, unsigned long* pnB, float* pfC,
                     int* pnD);
    unsigned char* Get4PCDataPtr();

    int m_bServiced;
    int m_nReserved;
    unsigned char* m_pData;
    unsigned long* m_pHeader;
};





class CResDDS : public CRes
{
public:
    CResDDS();
    virtual ~CResDDS();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    int GetDDSAttrib(unsigned long* pnWidth, unsigned long* pnHeight,
                     unsigned char* pnBpp, unsigned long* pnSize,
                     float* pfAlpha);
    unsigned char* GetDDSDataPtr();

    int m_bServiced;
    int m_nReserved;
    unsigned char* m_pData;
    unsigned long* m_pHeader;
};

class CResLIP : public CRes
{
public:
    CResLIP();
    ~CResLIP();
    virtual int OnResourceFreed();
    char* GetLIPDataPtr();
    virtual int OnResourceServiced();

    int m_bServiced;
    unsigned long m_nLIPSize;
    char* m_pLIPData;
};

#pragma pack(push, 1)

struct LTRHeader
{
    unsigned long m_nFileType;
    unsigned long m_nVersion;
    unsigned char m_nLetterCount;
};

#pragma pack(pop)

class CResLTR : public CRes
{
public:
    CResLTR();
    virtual ~CResLTR();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    LTRHeader* GetLTRHeaderPtr() { return m_pLTRData; }
    float*** GetStartTripleLetterProb() { return m_pppnLetterTable; }
    float*** GetMiddleTripleLetterProb() { return m_pppnLetterTable2; }
    float*** GetEndTripleLetterProb() { return m_pppnLetterTable3; }
    float** GetStartDoubleLetterProb() { return m_ppnLetterTable; }
    float* GetStartSingleLetterProb() { return m_pnLetterTable; }

    float*** m_pppnLetterTable;
    float*** m_pppnLetterTable2;
    float*** m_pppnLetterTable3;
    float** m_ppnLetterTable;
    float** m_ppnLetterTable2;
    float** m_ppnLetterTable3;
    float* m_pnLetterTable;
    float* m_pnLetterTable2;
    float* m_pnLetterTable3;
    LTRHeader* m_pLTRData;
    unsigned char* m_pData;
    unsigned long m_nDataSize;
    int m_bServiced;
};


#define RES_TYPE_LYT 3000

class CResLYT : public CRes
{
public:
    int m_bServiced;
    unsigned long m_nLYTSize;
    unsigned char* m_pLYTData;
    virtual int OnResourceFreed();
    virtual int OnResourceServiced();
    unsigned char* GetLYTDataPtr();
    int GetLYTSize();
    CResLYT();
    ~CResLYT();
};

class CResMDX : public CRes
{
public:
    CResMDX();
    ~CResMDX();
    virtual int OnResourceFreed();
    unsigned char* GetMDXDataPtr();
    unsigned long GetMDXSize();
    virtual int OnResourceServiced();

    int m_bServiced;
    unsigned long m_nMDXSize;
    unsigned char* m_pMDXData;
};


#define PLT_FILE_SIGNATURE 0x20544C50





class CResPLT : public CRes
{
public:
    CResPLT();
    virtual ~CResPLT();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    int GetPLTAttrib(unsigned long* pnA, unsigned long* pnB, unsigned long* pnC);
    unsigned char* GetPLTDataPtr();

private:
    int ReadRLEPLT();

    int m_bDecoded;
    int m_bCompressed;
    int m_bServiced;
    unsigned long m_nPLTSize;
    unsigned long m_nDataSize;
    unsigned char* m_pPLTData;
    unsigned long* m_pPLTHeader;
};



enum TGARLEPacketMask
{
    TGA_RLE_COUNT_MASK = 0x7F,
    TGA_RLE_REPEAT_MASK = 0x80
};


enum TGAImageType
{
    TGA_IMAGE_COLOR_MAPPED = 1,
    TGA_IMAGE_TRUE_COLOR = 2,
    TGA_IMAGE_GRAYSCALE = 3,
    TGA_IMAGE_RLE_COLOR_MAPPED = 9,
    TGA_IMAGE_RLE_TRUE_COLOR = 10
};

#pragma pack(push, 1)
struct TGAHeader
{
    unsigned char nIDLength;
    unsigned char nColorMapType;
    unsigned char nImageType;
    unsigned short nColorMapStart;
    unsigned short nColorMapLength;
    unsigned char nColorMapDepth;
    unsigned short nXOrigin;
    unsigned short nYOrigin;
    unsigned short nWidth;
    unsigned short nHeight;
    unsigned char nBitsPerPixel;
    unsigned char nDescriptor;
};
#pragma pack(pop)

class CResTGA : public CRes
{
public:
    CResTGA();
    virtual ~CResTGA();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    int GetTGAAttrib(unsigned long* pnWidth, unsigned long* pnHeight, unsigned char* pnDepth);
    unsigned char* GetTGADataPtr();
    unsigned long GetTGASize();

    int ReadColorMappedRLETGA();
    int ReadUnmappedRLETGA();

    int m_bDataOwned;
    int m_bColorMapped;
    int m_bRLE;
    int m_bServiced;
    unsigned long m_nDataSize;
    unsigned long m_nRLESize;
    unsigned char* m_pColorMap;
    unsigned char* m_pTGAData;
    TGAHeader* m_pHeader;
};

#pragma pack(push, 1)
struct TPCHeader
{
    unsigned long m_nDataSize;
    float m_fAlphaBlending;
    unsigned short m_nWidth;
    unsigned short m_nHeight;
    unsigned char m_bFormat1 : 1;
    unsigned char m_bFormat3 : 1;
    unsigned char m_bFormat4 : 1;
    unsigned char m_nFormatUnused : 5;
    unsigned char m_nMipMapCount;
    unsigned char m_nUnknown0E[114];
};
#pragma pack(pop)

class CResTPC : public CRes
{
public:

    enum TextureEncoding
    {
        TPC_ENCODING_SINGLE_CHANNEL = 1,
        TPC_ENCODING_RGB = 3,
        TPC_ENCODING_RGBA = 4
    };
    enum
    {
        TPC_COMPRESSED_BLOCK_DIMENSION = 4,
        TPC_COMPRESSED_RGB_BLOCK_SIZE = 8,
        TPC_COMPRESSED_RGBA_BLOCK_SIZE = 16,
        TPC_CUBE_FACE_COUNT = 6
    };

    CResTPC();
    virtual ~CResTPC();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    int GetTPCAttrib(int* pnWidth, int* pnHeight, char* pnFormat,
                     int* pnCompressedSize, int* pnUncompressedSize,
                     int* pnRemainingSize, float* pfUnknown,
                     char* pnMipMapCount);
    unsigned char* GetTPCDataPtr();
    int GetTPCSize();
    unsigned char* GetTPCTXIPtr();
    int GetTPCTXISize();

    int m_bCompressed;
    int m_bServiced;
    int m_nUncompressedSize;
    int m_nCompressedSize;
    int m_nRemainingSize;
    unsigned char* m_pData;
    TPCHeader* m_pHeader;
    unsigned char* m_pEnd;
};

class CResTXI : public CRes
{
public:
    CResTXI();
    virtual ~CResTXI();
    virtual int OnResourceFreed();
    virtual int OnResourceServiced();
    void* GetTXIDataPtr();
    unsigned long GetTXISize();



    int m_bServiced;
    unsigned long m_nTXISize;
    unsigned char* m_pTXIData;
};

class CResVIS : public CRes
{
public:


    int m_bServiced;
    unsigned long m_nVISSize;
    unsigned char* m_pVISData;
    CResVIS();
    ~CResVIS();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();
    unsigned char* GetVISDataPtr();
    unsigned long GetVISSize();
};



const unsigned long BMU_FILE_TYPE = 0x20554D42;
const unsigned long BMU_FILE_VERSION = 0x302E3156;
const unsigned long WAVE_RIFF_FILE_TYPE = 0x46464952;

#pragma pack(push, 1)
struct CBMUFileHeader
{
    unsigned long m_nFileType;
    unsigned long m_nVersion;
};

struct CLegacyWaveHeader
{
    unsigned char m_auUnknown[470];
};
#pragma pack(pop)

class CResWave : public CRes
{
public:
    CResWave();
    virtual ~CResWave();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

#ifdef _XBOX

    void* GetData();
#else

    virtual void* GetData();
#endif

#ifdef _XBOX

    int m_bDataOwned;
    int m_bServiced;
    int m_bLegacyHeader;
    unsigned long m_nDataSize;
#else
    int m_bDataOwned;
    int m_bLegacyHeader;
    int m_bServiced;
    unsigned long m_nDataSize;
    int m_nUnknown38;
#endif
};

class CResMDL : public CRes
{
public:


    int m_bServiced;
    unsigned long m_nMDLSize;
    unsigned char* m_pMDLData;
    CResMDL();
    virtual ~CResMDL();

    virtual int OnResourceFreed();
    unsigned char* GetMDLDataPtr();
    unsigned long GetMDLSize();
    virtual int OnResourceServiced();
};

class CResNCS : public CRes
{
public:
    CResNCS();
    virtual ~CResNCS();
    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    char* GetNCSDataPtr();
    unsigned long GetNCSSize();

    int m_bServiced;
    unsigned long m_nNCSSize;
    unsigned char* m_pNCSData;
};





class CResDWK : public CRes
{
public:
    CResDWK();
    virtual ~CResDWK();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    unsigned char* GetDWKDataPtr();
    unsigned long GetDWKSize();

    int m_bServiced;
    unsigned long m_nDWKSize;
    unsigned char* m_pDWKData;
};





class CResSSF : public CRes
{
public:
    CResSSF();
    virtual ~CResSSF();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    int m_bServiced;
};






















template <class T, unsigned short nResType> class CResHelper
{
public:
    CResHelper();

    virtual ~CResHelper();

    void SetResRef(const CResRef& cResRef, int bRequest);



    const CResRef& GetResRef() const { return m_cResRef; }

    int m_bRequested;
    T* m_pRes;
    CResRef m_cResRef;
};

template <class T, unsigned short nResType> inline CResHelper<T, nResType>::CResHelper()
{
    m_pRes = NULL;
    m_cResRef = "";
    m_bRequested = 0;
}

template <class T, unsigned short nResType> inline CResHelper<T, nResType>::~CResHelper()
{
    if (m_pRes != NULL)
    {
        if (m_cResRef != "")
        {
            if (m_bRequested)
            {
                m_pRes->CancelRequest();
                m_bRequested = 0;
            }

            if (g_pExoResMan->ReleaseResObject(m_pRes) == 0)
            {
                if (m_pRes != NULL)
                {
                    delete m_pRes;
                }

                m_pRes = NULL;
            }
        }
    }
}

template <class T, unsigned short nResType> inline void CResHelper<T, nResType>::SetResRef(const CResRef& cResRef, int bRequest)
{
    if (!(cResRef == m_cResRef))
    {
        if (m_pRes != NULL && m_cResRef != "")
        {
            if (m_bRequested)
            {
                m_pRes->CancelRequest();
            }

            if (g_pExoResMan->ReleaseResObject(m_pRes) == 0)
            {
                if (m_pRes != NULL)
                {
                    delete m_pRes;
                }

                m_pRes = NULL;
            }
        }

        if (cResRef == "")
        {
            m_pRes = NULL;
            m_cResRef = "";
        }
        else
        {
            T* pRes = (T*)g_pExoResMan->GetResObject(cResRef, nResType);

            if (pRes == NULL)
            {
                pRes = new T;
                g_pExoResMan->SetResObject(cResRef, nResType, pRes);
            }

            if (pRes != NULL)
            {
                m_pRes = pRes;

                if (bRequest)
                {
                    m_bRequested = 1;
                    pRes->Request();
                }

                m_cResRef = cResRef;
            }
        }
    }
}



struct LIPHeader
{
    char m_szVersion[8];
    float m_fAnimLength;
    unsigned long m_nEntryCount;
};

class CLIP : public CResHelper<CResLIP, RESTYPE_LIP>
{
public:
    unsigned char m_aUnmapped[4];
    int m_bLipLoaded;
    int m_nEntryCount;
    float m_AnimLength;
    char* m_pLipData;

    virtual ~CLIP();
    float GetAnimLength();
    int GetVisemeIndex(unsigned long);
    float GetTimeIndex(unsigned long);
    CLIP();
    int LoadLip(CExoString);
    int GetNumVisemes();
    int UnloadLip();
};

class CExoString;
class Vector;
class Quaternion;

class CLYT : public CResHelper<CResLYT, RESTYPE_LYT>
{
public:
    CLYT();
    ~CLYT();

    int GetRoomCount();
    int GetTrackCount();
    int GetObstacleCount();
    int UnloadLayout();
    CResRef GetRoomName(int nRoom);
    Vector GetRoomPosition(int nRoom);
    CResRef GetTrackName(int nTrack);
    Vector GetTrackPosition(int nTrack);
    CResRef GetObstacleName(int nObstacle);
    Vector GetObstaclePosition(int nObstacle);
    int LoadLayout(const CExoString& sLayout);
    int LoadLayout(const CResRef& cResRef);

    unsigned char m_aUnmapped[4];
    int m_bLayoutLoaded;
    int m_nRoomCount;
    int m_nTrackCount;
    int m_nObstacleCount;
    int m_nDoorHookCount;
    CResRef* m_pRoomNames;
    CResRef* m_pTrackNames;
    CResRef* m_pObstacleNames;
    Vector* m_pRoomPositions;
    Vector* m_pTrackPositions;
    Vector* m_pObstaclePositions;
    CExoString* m_pDoorHookNames;
    CExoString* m_pDoorHookTargets;
    Vector* m_pDoorHookPositions;
    Quaternion* m_pDoorHookOrientations;
    int* m_pDoorHookIds;
};





class CExoStringList;
class CRes;





class CKeyTableEntry
{
public:
    CKeyTableEntry()
    {
        m_pRes = NULL;
        m_nResId = 0;
        m_sDirectory = "";
    }

    CResRef m_resRef;
    CRes* m_pRes;



    unsigned long m_nResId;
    unsigned short m_nReserved;
    unsigned short m_nType;
    CExoString m_sDirectory;
};

class CExoPackedFile;



struct CExoKeyFileHeader
{
    unsigned long nFileType;
    unsigned long nVersion;
    unsigned long nBifCount;
    unsigned long nKeyCount;
    unsigned long nBifTableOffset;
    unsigned long nKeyTableOffset;
};



struct CExoKeyFileKeyEntry
{
    unsigned char szResRef[16];
    unsigned short nType;
    unsigned long nId;
};




struct CExoKeyFileBifEntry
{
    unsigned long nFileSize;
    unsigned long nNameOffset;
    unsigned short nNameSize;
    unsigned short nDrives;
};




struct CExoEncapsulatedFileHeader
{
    unsigned long nFileType;
    unsigned long nVersion;
    unsigned long nLanguageCount;
    unsigned long nLocalizedStringSize;
    unsigned long nEntryCount;
    unsigned long nLocalizedStringOffset;
    unsigned long nKeyListOffset;
    unsigned long nResourceListOffset;
    unsigned long nBuildYear;
    unsigned long nBuildDay;
    unsigned long nDescriptionStrRef;
    unsigned char aReserved[116];
};


struct CExoEncapsulatedFileKey
{
    unsigned char szResRef[16];
    unsigned long nResId;
    unsigned short nResType;
    unsigned short nUnused;
};


struct CExoEncapsulatedFileResource
{
    unsigned long nOffset;
    unsigned long nSize;
};

class CExoKeyTable
{
public:


    enum { TABLE_ID_MASK = 0x0FFFFFFF };
    CExoKeyTable();
    ~CExoKeyTable();
    int GetEntryCount(int bCountAll);
    unsigned long GetTableIndex(unsigned long& nIndex, const CResRef& resRef,
                                unsigned short nType);
    unsigned long Hash(const CResRef& resRef, unsigned short nType);
    CKeyTableEntry* AllocateTable(unsigned long nSize);
    CKeyTableEntry* AddKey(const CResRef& resRef, unsigned short nType,
                           unsigned long nResId, int, CExoString sDirectory);
    CKeyTableEntry* FindKey(const CResRef& resRef, unsigned short nType);
    CExoStringList* GetKeysOfType(unsigned short nType);
    void DestroyTable();
    int LocateBifFile(const CExoString& sFileName);
    int AddDirectoryContents(int bFlag);
    int AddEncapsulatedContents(int bFlag);
    int AddResourceImageContents(int bFlag);
    int AddKeyTableContents(int bFlag);
    int BuildNewTable(unsigned long nTableType, const CExoString& sName,
                      unsigned long nId, int bFlag);
    void RebuildTable();

    unsigned long m_nUnknown00;
    int m_bDestroying;



    unsigned long m_nPriority;
    unsigned long m_nTableSize;
    CKeyTableEntry* m_pKeyTable;
    unsigned long m_nCollisions;
    unsigned long m_nUnknown18;
    unsigned long m_nTableType;
    CExoString m_sResourceName;
    unsigned long m_nId;
    unsigned long m_nPackedFileCount;
    CExoPackedFile** m_pPackedFiles;

    static unsigned long m_nCRC32[256];
};

#include "exoresfile.h"






















class CExoEncapsulatedFile : public CExoPackedFile
{
public:
    CExoEncapsulatedFile();

    virtual ~CExoEncapsulatedFile();

    virtual void AddRefCount();
    virtual void AddAsyncRefCount();
    virtual int CloseFile();
    virtual int CloseAsyncFile();
    virtual void DeleteRefCount();
    virtual void DeleteAsyncRefCount();




    virtual unsigned long GetResourceSize(unsigned long nId);
    virtual int Initialize();
    virtual int OpenFile();
    virtual int OpenAsyncFile();

    virtual unsigned long ReadResource(unsigned long nId, void* pBuffer,
                                       unsigned long nSize,
                                       unsigned long nOffset);
    virtual void ReadResourceAsync(unsigned long nId, void* pBuffer,
                                   unsigned long nSize, unsigned long nOffset);

    virtual int LoadHeader(unsigned char nType);
    virtual int UnloadHeader();



    CExoLocString m_lsDescription;
    CExoEncapsulatedFileHeader* m_pHeader;
    CExoEncapsulatedFileResource* m_pResourceList;




    unsigned char m_bFlag;
};













struct CExoResourceImageFileHeader
{
    unsigned char m_nFileHeader[12];
    unsigned long m_nEntryCount;
    unsigned long m_nEntryOffset;
};

struct CExoResourceImageFileEntry
{
    char m_szResRef[16];
    unsigned short m_nType;
    unsigned short m_nReserved;
    unsigned long m_nId;
    unsigned long m_nOffset;
    unsigned long m_nSize;
};

class CExoResourceImageFile : public CExoPackedFile
{
public:
    CExoResourceImageFile();

    virtual ~CExoResourceImageFile();

    virtual void AddRefCount();
    virtual void AddAsyncRefCount();
    virtual int CloseFile();
    virtual int CloseAsyncFile();
    virtual void DeleteRefCount();
    virtual void DeleteAsyncRefCount();

    virtual unsigned long GetResourceSize(unsigned long nId);
    virtual int Initialize();
    virtual int OpenFile();
    virtual int OpenAsyncFile();

    virtual unsigned long ReadResource(unsigned long nId, void* pBuffer,
                                       unsigned long nSize,
                                       unsigned long nOffset);
    virtual void ReadResourceAsync(unsigned long nId, void* pBuffer,
                                   unsigned long nSize, unsigned long nOffset);

    virtual int LoadHeader(unsigned char);
    virtual int UnloadHeader();


    virtual void* GetResource(unsigned long nId);


    CExoResourceImageFileEntry GetKeyListEntry(unsigned long nIndex)
    {
        return ((CExoResourceImageFileEntry*)(m_pImageFile +
                                              ((CExoResourceImageFileHeader*)
                                                   m_pImageFile)
                                                  ->m_nEntryOffset))[nIndex];
    }

    unsigned char* m_pImageFile;
};












class CExoKeyTable;
class CExoString;
class CKeyTableEntry;
class CResRef;

class CExoFile;
class CExoStringList;




class CExoResMan
{
public:


    enum ResourceTableType
    {
        TABLE_NONE = 0,
        TABLE_KEY = 1,
        TABLE_DIRECTORY = 2,
        TABLE_ENCAPSULATED = 3,
        TABLE_RESOURCE_IMAGE = 4
    };



    enum ResourcePriority
    {
        PRIORITY_MODULE = 2
    };


    enum AsyncLoadState
    {
        ASYNC_IDLE = 0,
        ASYNC_LOAD_REQUESTED = 1,
        ASYNC_MODULE_LOADED = 2,
        ASYNC_MODULE_IMAGE_LOADED = 3,
        ASYNC_LOAD_FAILED = 4
    };



    enum ModuleResourceLoadedFlags
    {
        LOADED_CURRENT_GAME_RIM = 0x01,
        LOADED_MODULE_RIM = 0x02,
        LOADED_CURRENT_GAME_KEY = 0x04,
        LOADED_SAVE_GAME_RIM = 0x08,
        LOADED_MODULE_AREA_RIM = 0x10,
        LOADED_MODULE_ADX_RIM = 0x20
    };

    CExoResMan();
    ~CExoResMan();

    int AddEncapsulatedResourceFile(const CExoString& sFileName,
                                    unsigned long nId);
    void AddFixedKeyTableFile(const CExoString& sFileName);
    int AddKeyTable(const CExoString& sFileName, unsigned long nType,
                    unsigned long nId);
    void AddResourceDirectory(const CExoString& sFileName);
    void AddResourceImageFile(const CExoString& sFileName);
    void AddResourceImageFilesAsync(const CExoString& sModuleName,
                                    int bIsModule, int bFromCurrentGame);
    void AsyncLoad();
    CExoString CacheResource(const CExoString& sFileName, unsigned short nType);
    CExoString CacheResource(const CExoString& sFileName, unsigned short nType,
                             int& nResult);
    int CancelRequest(CRes* pRes);
    int CleanDirectory(const CExoString& sPath, int bRecurse,
                       int bRecurseClean);
    int CreateDirectory2(const CExoString& sPath);
#ifdef _XBOX


    int CreateSignatureFile(const CExoString& sFileName, unsigned short nType,
                            unsigned long nSize, unsigned char* pSignature,
                            unsigned long nFlags);
    int VerifySignatureFile(const CExoString& sFileName,
                            const unsigned char* pSignature);
    void WriteSignatureFile(const CExoString& sFileName,
                            const unsigned char* pSignature);


    int VerifySignatureFile(const CExoString& sFileName, unsigned short nType,
                            unsigned long nSize, unsigned long nFlags);




    void ReportMissingFile(const CExoString& sFileName, unsigned short nType,
                           int bFatal);
#endif


    int CreateSignatureFile(const CExoString& sFileName, unsigned short nType,
                            unsigned long nArg3, unsigned long nArg4);
    void* Demand(CRes* pRes);
    void Dump(CRes* pRes, int bFlag);
    int Exists(const CResRef& cResRef, unsigned short nType,
               unsigned long* pnTableType);
    int Exists(const CResRef& cResRef, unsigned short nType,
               unsigned long nTableType);
    int FileCopy(const CExoString& sSource, const CExoString& sDest,
                 unsigned short nSourceType, unsigned short nDestType);


    int FindModuleDownloadedPkg(const CExoString& sModuleName);
    int Free(CRes* pRes);
    int FreeChunk();
    void FreeResourceData(CRes* pRes);
    int GetFreeDiskSpace(const CExoString& sPath,
                         unsigned __int64* pnFreeBytes);
    int GetIsStaticType(unsigned short nType);
    int GetKeyEntry(const CResRef& cResRef, unsigned short nType,
                    CExoKeyTable** ppTable, CKeyTableEntry** ppEntry);
    int GetKeyEntryFromTable(CExoLinkedList<CExoKeyTable>& lstTables,
                             const CResRef& cResRef, unsigned short nType,
                             CExoKeyTable** ppTable, CKeyTableEntry** ppEntry,
                             unsigned long nId);
    int GetNewResRef(const CResRef& cResRef, unsigned short nType,
                     CResRef& cNewResRef);
    CExoStringList* GetResOfType(unsigned short nType, int bEncapsulatedOnly);
    CExoStringList* GetResOfType(unsigned short nType, CRes* pRes);
    CExoStringList* GetResOfTypeFromTable(
        CExoLinkedList<CExoKeyTable>& lstTables, unsigned short nType);
    CRes* GetResObject(const CResRef& cResRef, unsigned short nType);
    int GetResourcesInMemory(int* pnResources, int* pnKeys,
                             int* pnServicedSize, int* pnImageSize,
                             int* pnModelSize, int* pnModelDataSize,
                             int* pnTextureSize, int* pnWaveSize);
    void GetResRefFromFile(CResRef& cResRef, const CExoString& sFileName);
    unsigned short GetResTypeFromFile(const CExoString& sFileName);
    CExoKeyTable* GetTable(CRes* pRes);
    int GetTableCount(CRes* pRes, int bCountAll);
    int Malloc(CRes* pRes);
    int NukeDirectory(const CExoString& sPath, int bDeleteFiles, int bRecurse);
    int ReadRaw(CRes* pRes, int nSize, char* pBuffer);
    int Release(CRes* pRes);
    int ReleaseResObject(CRes* pRes);
    int RemoveEncapsulatedResourceFile(const CExoString& sFileName);
    int RemoveFile(const CExoString& sFileName, unsigned short nType);
    int RemoveFixedKeyTableFile(const CExoString& sFileName);
    int RemoveKeyTable(const CExoString& sFileName, unsigned long nType);
    int RemoveResourceDirectory(const CExoString& sFileName);
    int RemoveResourceImageFile(const CExoString& sFileName);

    int Request(CRes* pRes)
    {
        if (pRes == NULL)
        {
            return 0;
        }

        if (pRes->m_nFlags & 0x4)
        {
            return 0;
        }

        if (pRes->m_nRequestCount > 0)
        {
            pRes->m_nRequestCount++;
        }
        else
        {
            pRes->m_nRequestCount = 0;
            m_lstRequests.AddTail(pRes);
            pRes->m_nRequestCount++;
        }

        return 1;
    }

    int ServiceCurrentAsyncRes();
    int ServiceFromDirectory(CRes* pRes, int bAsync);
    int ServiceFromDirectoryRaw(CRes* pRes, int nSize, char* pBuffer);
    int ServiceFromEncapsulated(CRes* pRes, int bAsync);
    int ServiceFromEncapsulatedRaw(CRes* pRes, int nSize, char* pBuffer);
    int ServiceFromImage(CRes* pRes, int bAsync);
    int ServiceFromImageRaw(CRes* pRes, int nSize, char* pBuffer);
    int ServiceFromResFile(CRes* pRes, int bAsync);
    int ServiceFromResFileRaw(CRes* pRes, int nSize, char* pBuffer);
    void SetResObject(const CResRef& cResRef, unsigned short nType, CRes* pRes);
    void Update(unsigned long nTime);
    int UpdateKeyTable(const CExoString& sFileName, unsigned long nType);
    int UpdateResourceDirectory(const CExoString& sFileName);
    int WipeDirectory(const CExoString& sPath, int bDeleteFiles,
                      int bRemoveDirectory, int bRecurseClean, int bRecurse);

    unsigned long m_nTotalPhysicalMemory;
    int m_nMemoryLimit;
    int m_nAvailableMemory;
    unsigned long m_nUnknown0C;
    CExoLinkedList<CExoKeyTable> m_lstKeyTables;
    CExoLinkedList<CExoKeyTable> m_lstDirectories;
    CExoLinkedList<CExoKeyTable> m_lstEncapsulated;
    CExoLinkedList<CExoKeyTable> m_lstResourceImages;


    CExoLinkedList<CRes> m_lstReleased;
    CExoLinkedList<CRes> m_lstRequests;
    CRes* m_pCurrentAsyncRes;
    CExoFile* m_pCurrentAsyncFile;
    int m_bUpdateDisabled;
    unsigned char m_bCharGenImageLoaded : 1;
    unsigned char m_bMainMenuImageLoaded : 1;
    unsigned char m_nImageLoadedFlagsUnused : 6;
    void* m_hAsyncThread;
    unsigned long m_nAsyncThreadId;
    CExoCriticalSection m_csAsync;


    unsigned char m_nAsyncState;
    int m_bExitAsyncThread;
    CExoString m_sAsyncModuleName;
    int m_bAsyncIsModule;
    int m_bAsyncFromCurrentGame;


    unsigned char m_nAsyncLoadedFlags;
};


void AsyncRimLoadThreadProc(void* pParameter);



extern CExoResMan* g_pExoResMan;

extern int g_bModifiedSound;

#ifdef _XBOX



#define RESTYPE_TPC 3006
#endif

#endif
