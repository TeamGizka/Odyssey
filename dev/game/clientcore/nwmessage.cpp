#include <string.h>
#include "exobase.h"

#include "exoresman.h"
#include "resgff.h"
#include "nwmessage.h"
#include "gameobjectarray.h"

static inline void TraceReadVector();
static inline void TraceMessageRead();
#define NWMESSAGE_TRACE() TraceReadVector()

const int CSWMessage::DEFAULT_WRITE_BUFFER_SIZE = 128;
// PC 0x004D7360  Xbox 0x00095100
CExoString CSWCCMessageData::GetString(int nIndex)
{
    if (nIndex < m_lstStrings.num)
    {
        return m_lstStrings.element[nIndex];
    }

    return "";
}
// PC 0x00405040
int CSWMessage::PeekAtWriteMessageSize()
{
    return (int)this->m_nWriteIndex;
}
// PC 0x004D76B0
void CSWCCMessageData::SetInteger(int nIndex, int nValue)
{
    if (nIndex >= m_lstIntegers.num)
    {
        m_lstIntegers.SetSize(nIndex + 1);
    }

    m_lstIntegers.element[nIndex] = nValue;
}
// PC 0x004D7720
void CSWCCMessageData::SetFloat(int nIndex, float fValue)
{
    if (nIndex >= m_lstFloats.num)
    {
        m_lstFloats.SetSize(nIndex + 1);
    }

    m_lstFloats.element[nIndex] = fValue;
}
// PC 0x004D7780
void CSWCCMessageData::SetObjectID(int nIndex, unsigned long oidValue)
{
    if (nIndex >= m_lstObjectIDs.num)
    {
        m_lstObjectIDs.SetSize(nIndex + 1);
    }

    m_lstObjectIDs.element[nIndex] = oidValue;
}
// Dead code: not in the PC executable
unsigned char CSWMessage::ReadBit(unsigned char nBit)
{
    unsigned char* pFragments = (unsigned char*)m_pReadFragmentsBuffer;

    return (pFragments[m_nReadFragmentsIndex] & (1 << (7 - nBit))) >> (7 - nBit);
}
// Dead code: not in the PC executable
unsigned __int64 CSWMessage::ReadBits(int nBits)
{
    unsigned char* pFragments = (unsigned char*)m_pReadFragmentsBuffer;
    unsigned __int64 nValue = 0;
    unsigned char cBit;
    int i;

    for (i = 0; i < nBits; i++)
    {
        cBit = (unsigned char)((pFragments[m_nReadFragmentsIndex] &
                                (1 << (7 - m_nReadFragmentsBitPos))) >>
                               (7 - m_nReadFragmentsBitPos));
        nValue = (nValue << 1) | (cBit & 1);

        m_nReadFragmentsBitPos++;

        if (m_nReadFragmentsBitPos == 8)
        {
            m_nReadFragmentsIndex++;
            m_nReadFragmentsBitPos = 0;
        }
    }

    return nValue;
}
// Dead code: not in the PC executable
void CSWMessage::WriteBit(unsigned char bValue)
{
    unsigned char* pFragment = m_pWriteFragmentsBuffer + m_nWriteFragmentsIndex;
    unsigned char nBitPos = *(unsigned char*)&m_nWriteFragmentsBitPos;

    *pFragment &= ~(1 << (7 - nBitPos));
    *pFragment |= bValue << (7 - nBitPos);
}
// Dead code: not in the PC executable
void CSWMessage::WriteBits(unsigned __int64 nValue, int nBits)
{
    unsigned char& nBitPos = *(unsigned char*)&m_nWriteFragmentsBitPos;
    unsigned char* pFragment;

    while (nBits > 0)
    {
        nBits--;

        pFragment = m_pWriteFragmentsBuffer + m_nWriteFragmentsIndex;
        *pFragment &= ~(1 << (7 - nBitPos));
        *pFragment |= ((unsigned char)(nValue >> nBits) & 1) << (7 - nBitPos);

        nBitPos++;

        if (nBitPos == 8)
        {
            m_nWriteFragmentsIndex++;
            nBitPos = 0;
        }
    }
}
// Dead code: not in the PC executable
unsigned __int64 CSWMessage::ReadUnsigned(int nBits)
{
    unsigned __int64 nValue = 0;
    unsigned long nFragmentIndex;

    if (nBits == 64)
    {
        if (m_nReadIndex + 8 > m_nReadBufferSize)
        {
            m_nReadIndex += 8;
            return 0;
        }

        nValue = *(unsigned __int64*)(m_pReadBuffer + m_nReadIndex);
        m_nReadIndex += 8;
        return nValue;
    }

    while (nBits >= 32)
    {
        if (m_nReadIndex + 4 > m_nReadBufferSize)
        {
            m_nReadIndex += 4;
            return 0;
        }

        nValue = (nValue << 32) |
                 *(unsigned long*)(m_pReadBuffer + m_nReadIndex);
        m_nReadIndex += 4;
        nBits -= 32;
    }

    while (nBits >= 16)
    {
        if (m_nReadIndex + 2 > m_nReadBufferSize)
        {
            m_nReadIndex += 2;
            return 0;
        }

        nValue = (nValue << 16) |
                 *(unsigned short*)(m_pReadBuffer + m_nReadIndex);
        m_nReadIndex += 2;
        nBits -= 16;
    }

    while (nBits >= 8)
    {
        if (m_nReadIndex + 1 > m_nReadBufferSize)
        {
            m_nReadIndex += 1;
            return 0;
        }

        nValue = (nValue << 8) | m_pReadBuffer[m_nReadIndex];
        m_nReadIndex += 1;
        nBits -= 8;
    }

    if (nBits > 0)
    {
        nFragmentIndex = m_nReadFragmentsIndex +
                         (unsigned char)((m_nReadFragmentsBitPos - 1 + nBits) >> 3);

        if (nFragmentIndex > m_nReadFragmentsBufferSize)
        {
            m_nReadFragmentsIndex = nFragmentIndex;
            return 0;
        }

        nValue = (nValue << nBits) | ReadBits(nBits);
    }

    return nValue;
}
// Dead code: not in the PC executable
__int64 CSWMessage::ReadSigned(int nBits)
{
    unsigned char bNegative;
    __int64 nValue;

    if (m_nReadFragmentsIndex > m_nReadFragmentsBufferSize)
    {
        return 0;
    }

    bNegative = (unsigned char)ReadBits(1);
    nValue = (__int64)ReadUnsigned(nBits - 1);

    if (bNegative)
    {
        nValue = -nValue;

        if (nValue == 0)
        {
            nValue = 0x8000000000000000;
        }
    }

    return nValue;
}
// PC 0x004D60F0  Xbox 0x00094D40
CSWMessage::CSWMessage()
{
    m_pWriteBuffer = new unsigned char[DEFAULT_WRITE_BUFFER_SIZE];
    m_nWriteIndex = 0;
    m_bWritingMessage = 0;
    m_pReadBuffer = NULL;
    m_nReadBufferSize = 0;
    m_nReadIndex = 0;
    m_pWriteFragmentsBuffer = NULL;
    m_nWriteBufferSize = DEFAULT_WRITE_BUFFER_SIZE;
}
// PC 0x004D6130  Xbox 0x00094D80
CSWMessage::~CSWMessage()
{
    if (m_pWriteBuffer != NULL)
    {
        delete[] m_pWriteBuffer;
        m_pWriteBuffer = NULL;
    }

    if (m_pWriteFragmentsBuffer != NULL)
    {
        delete[] m_pWriteFragmentsBuffer;
        m_pWriteFragmentsBuffer = NULL;
    }
}
// PC 0x004D6170  Xbox 0x00094DC0
void CSWMessage::ClearReadMessage()
{
    m_nReadIndex = m_nReadBufferSize;
}
// PC 0x004D6180  Xbox 0x00094E00
CResRef CSWMessage::ReadCResRef(int nLength)
{
    unsigned char szResRef[16];
    int nCount = nLength;

    if (nCount > 16)
    {
        nCount = 16;
    }

    if (m_nReadIndex + nCount > m_nReadBufferSize)
    {
        m_nReadIndex += nCount;
        return CResRef("");
    }

    memcpy(szResRef, m_pReadBuffer + m_nReadIndex, nCount);
    m_nReadIndex += nCount;
    return CResRef(szResRef);
}
// PC 0x004D6200  Xbox 0x00094E90
void* CSWMessage::ReadVOIDPtr(int nBytes)
{
    void* pValue;

    if (m_nReadIndex + nBytes > m_nReadBufferSize)
    {
        pValue = NULL;
    }
    else
    {
        pValue = m_pReadBuffer + m_nReadIndex;
    }

    m_nReadIndex += nBytes;

    return pValue;
}
// PC 0x004D6230  Xbox 0x00094EC0
int CSWMessage::MessageReadOverflow(int nBytes)
{
    return m_nReadIndex > m_nReadBufferSize;
}
// PC 0x004D6240  Xbox 0x00094ED0
int CSWMessage::MessageReadUnderflow(int nBytes)
{
    return m_nReadIndex < m_nReadBufferSize;
}
// PC 0x004D6250  Xbox 0x00094EE0
int CSWMessage::MessageMoreDataToRead()
{
    return m_nReadIndex < m_nReadBufferSize;
}
// PC 0x004D69F0  Xbox 0x000956E0
CSWCCMessageData::CSWCCMessageData() :
    m_lstIntegers(0), m_lstFloats(0), m_lstStrings(0), m_lstObjectIDs(0)
{
    m_nType = 0;
}
// PC 0x004D6A40  Xbox 0x00095730
CSWCCMessageData::~CSWCCMessageData()
{
}
// PC 0x004D6AE0  Xbox 0x000957D0
void CSWCCMessageData::SetString(int nIndex, const CExoString& sValue)
{
    if (nIndex >= m_lstStrings.num)
    {
        m_lstStrings.SetSize(nIndex + 1);
    }

    m_lstStrings.element[nIndex] = sValue;
}
// PC 0x004D6B10  Xbox 0x00095800
int CSWCCMessageData::LoadData(CResGFF* pRes, CResStruct* pStruct)
{
    CResList lstList;
    CResStruct sElement;
    int bSuccess;
    unsigned long i;

    m_nType = pRes->ReadFieldBYTE(pStruct, "Type", bSuccess, 0);

    if (pRes->GetList(&lstList, pStruct, "IntList") == 1)
    {
        for (i = 0; i < pRes->GetListCount(&lstList); i++)
        {
            pRes->GetListElement(&sElement, &lstList, i);
            SetInteger(
                i, pRes->ReadFieldINT(&sElement, "IntegerValue", bSuccess, 0));
        }
    }

    if (pRes->GetList(&lstList, pStruct, "FloatList") == 1)
    {
        for (i = 0; i < pRes->GetListCount(&lstList); i++)
        {
            pRes->GetListElement(&sElement, &lstList, i);
            SetFloat(i, pRes->ReadFieldFLOAT(&sElement, "FloatValue", bSuccess,
                                             0.0f));
        }
    }

    if (pRes->GetList(&lstList, pStruct, "ObjectIDList") == 1)
    {
        for (i = 0; i < pRes->GetListCount(&lstList); i++)
        {
            pRes->GetListElement(&sElement, &lstList, i);
            SetObjectID(
                i, pRes->ReadFieldDWORD(&sElement, "ObjectValue", bSuccess, 0));
        }
    }

    if (pRes->GetList(&lstList, pStruct, "StringList") == 1)
    {
        for (i = 0; i < pRes->GetListCount(&lstList); i++)
        {
            pRes->GetListElement(&sElement, &lstList, i);
            SetString(i, pRes->ReadFieldCExoString(&sElement, "StringValue",
                                                   bSuccess, ""));
        }
    }

    return 1;
}
// PC 0x004D6EE0  Xbox 0x00095BD0
void CSWCCMessageData::CopyTo(CSWCCMessageData* pData)
{
    int i;

    if (pData != NULL)
    {
        pData->m_lstIntegers.num = 0;
        pData->m_lstFloats.num = 0;
        pData->m_lstObjectIDs.num = 0;
        pData->m_lstStrings.num = 0;
        pData->m_nType = 0;

        for (i = 0; i < m_lstIntegers.num; i++)
        {
            pData->SetInteger(i, m_lstIntegers.element[i]);
        }

        for (i = 0; i < m_lstFloats.num; i++)
        {
            pData->SetFloat(i, m_lstFloats.element[i]);
        }

        for (i = 0; i < m_lstObjectIDs.num; i++)
        {
            pData->SetObjectID(i, m_lstObjectIDs.element[i]);
        }

        for (i = 0; i < m_lstStrings.num; i++)
        {
            pData->SetString(i, m_lstStrings.element[i]);
        }

        pData->m_nType = m_nType;
    }
}
// PC 0x004D7120  Xbox 0x00095E20
Vector CSWMessage::ReadVector()
{
    NWMESSAGE_TRACE();
    Vector vValue;

    vValue.m_fX = ReadFLOAT(0.0f, 32);
    vValue.m_fY = ReadFLOAT(0.0f, 32);
    vValue.m_fZ = ReadFLOAT(0.0f, 32);

    return vValue;
}
// PC 0x004D71B0  Xbox 0x00095EB0
void CSWMessage::WriteVector(const Vector& vValue)
{
    WriteFLOAT(vValue.m_fX, 0.0f, 32);
    WriteFLOAT(vValue.m_fY, 0.0f, 32);
    WriteFLOAT(vValue.m_fZ, 0.0f, 32);
}
// PC 0x004D6920  Xbox 0x00095610
void CSWMessage::WriteFLOAT(float fValue, float fDefault, int nBits)
{
    if (m_nWriteIndex + 4 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(4);
    }

    *(float*)(m_pWriteBuffer + m_nWriteIndex) = fValue;
    m_nWriteIndex += 4;
    m_nTotalBytesWritten += 4;
}
// PC 0x004D6960  Xbox 0x00095650
void CSWMessage::WriteCExoString(const CExoString& sValue, int nBits)
{
    unsigned long nLength = sValue.GetLength();

    WriteINT(nLength, 32);

    if (m_nWriteIndex + nLength > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(nLength);
    }

    memcpy(m_pWriteBuffer + m_nWriteIndex, sValue.CStr(), nLength);
    m_nWriteIndex += nLength;
    m_nTotalBytesWritten += nLength;
}
// PC 0x004D6260  Xbox 0x00094EF0
void CSWMessage::ExtendWriteBuffer(unsigned long nExtraBytes)
{
    unsigned long nNewSize = m_nWriteBufferSize + nExtraBytes + 128;
    unsigned char* pNewBuffer = new unsigned char[nNewSize];

    EXOXBOXASSERT(pNewBuffer);
    memcpy(pNewBuffer, m_pWriteBuffer, m_nWriteIndex);

    if (m_pWriteBuffer != NULL)
    {
        delete[] m_pWriteBuffer;
    }

    m_nWriteBufferSize = nNewSize;
    m_pWriteBuffer = pNewBuffer;
}
// PC 0x004D62C0  Xbox 0x00095590
void CSWMessage::WriteCHAR(char cValue, int nBits)
{
    if (m_nWriteIndex + 1 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(1);
    }

    m_pWriteBuffer[m_nWriteIndex] = cValue;
    m_nWriteIndex += 1;
    m_nTotalBytesWritten += 1;
}
// PC 0x004D6300  Xbox 0x000955D0
void CSWMessage::WriteSHORT(short nValue, int nBits)
{
    if (m_nWriteIndex + 2 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(2);
    }

    *(short*)(m_pWriteBuffer + m_nWriteIndex) = nValue;
    m_nWriteIndex += 2;
    m_nTotalBytesWritten += 2;
}
// PC 0x004D6340
void CSWMessage::WriteDWORD(unsigned long nValue, int nBits)
{
    if (m_nWriteIndex + 4 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(4);
    }

    *(unsigned long*)(m_pWriteBuffer + m_nWriteIndex) = nValue;
    m_nWriteIndex += 4;
    m_nTotalBytesWritten += 4;
}
// PC 0x004D62C0
void CSWMessage::WriteBYTE(unsigned char cValue, int nBits)
{
    if (m_nWriteIndex + 1 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(1);
    }

    m_pWriteBuffer[m_nWriteIndex] = cValue;
    m_nWriteIndex += 1;
    m_nTotalBytesWritten += 1;
}
// PC 0x004D6300
void CSWMessage::WriteWORD(unsigned short nValue, int nBits)
{
    if (m_nWriteIndex + 2 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(2);
    }

    *(unsigned short*)(m_pWriteBuffer + m_nWriteIndex) = nValue;
    m_nWriteIndex += 2;
    m_nTotalBytesWritten += 2;
}
// Dead code: not in the PC executable
void CSWMessage::WriteUnsigned(unsigned __int64 nValue, int nBits)
{
    unsigned short nBytes = (unsigned short)(nBits >> 3);

    if (m_nWriteIndex + nBytes > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(nBytes);
    }

    if (nBits == 64)
    {
        if (m_nWriteIndex + 8 > m_nWriteBufferSize)
        {
            ExtendWriteBuffer(8);
        }

        *(unsigned __int64*)(m_pWriteBuffer + m_nWriteIndex) = nValue;
        m_nWriteIndex += 8;
        m_nTotalBytesWritten += 8;
        return;
    }

    while (nBits >= 32)
    {
        nBits -= 32;
        WriteDWORD((unsigned long)(nValue >> nBits), 32);
    }

    while (nBits >= 16)
    {
        nBits -= 16;
        WriteWORD((unsigned short)(nValue >> nBits), 16);
    }

    while (nBits >= 8)
    {
        nBits -= 8;
        WriteBYTE((unsigned char)(nValue >> nBits), 8);
    }

    if (nBits > 0)
    {
        WriteBits(nValue & (0xFF >> (8 - nBits)), nBits);
    }
}
// Dead code: not in the PC executable
void CSWMessage::WriteSigned(__int64 nValue, int nBits)
{
    if (nValue < 0)
    {
        WriteBits(1, 1);
        WriteUnsigned(-nValue, nBits - 1);
    }
    else
    {
        WriteBits(0, 1);
        WriteUnsigned(nValue, nBits - 1);
    }
}
// PC 0x004D6340  Xbox 0x00094F70
void CSWMessage::WriteINT(int nValue, int nBits)
{
    if (m_nWriteIndex + 4 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(4);
    }

    *(int*)(m_pWriteBuffer + m_nWriteIndex) = nValue;
    m_nWriteIndex += 4;
    m_nTotalBytesWritten += 4;
}
// PC 0x004D6380  Xbox 0x00094FB0
void CSWMessage::WriteCResRef(CResRef cResRef, int nLength)
{
    if (m_nWriteIndex + nLength > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(nLength);
    }

    memcpy(m_pWriteBuffer + m_nWriteIndex, cResRef.GetResRef(), nLength);
    m_nWriteIndex += nLength;
    m_nTotalBytesWritten += nLength;
}
// PC 0x004D63E0  Xbox 0x00095010
void CSWMessage::WriteVOIDPtr(void* pValue, int nBytes)
{
    if (m_nWriteIndex + nBytes > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(nBytes);
    }

    memcpy(m_pWriteBuffer + m_nWriteIndex, pValue, nBytes);
    m_nWriteIndex += nBytes;
    m_nTotalBytesWritten += nBytes;
}
// PC 0x004D6440  Xbox 0x00095090
int CSWMessage::GetWriteMessage(unsigned char** ppBuffer, unsigned long* pnSize)
{
    int bRet = 1;
    EXOXBOXASSERT(m_bWritingMessage);
    m_bWritingMessage = 0;
    *pnSize = m_nWriteIndex;

    if (m_nWriteIndex == 0)
        bRet = 0;
    if (bRet)
        *ppBuffer = m_pWriteBuffer;
    else
        *ppBuffer = NULL;
    return bRet;
}
// PC 0x004D6480
int CSWCCMessageData::GetInteger(int nIndex)
{
    if (nIndex < m_lstIntegers.num)
    {
        return m_lstIntegers.element[nIndex];
    }

    return 0;
}
// PC 0x004D64A0
float CSWCCMessageData::GetFloat(int nIndex)
{
    if (nIndex < m_lstFloats.num)
    {
        return m_lstFloats.element[nIndex];
    }

    return 0.0f;
}
// PC 0x004D64C0  Xbox 0x000950E0
unsigned long CSWCCMessageData::GetObjectID(int nIndex)
{
    if (nIndex < m_lstObjectIDs.num)
    {
        return m_lstObjectIDs.element[nIndex];
    }

    return OBJECT_INVALID;
}
// PC 0x004D64E0  Xbox 0x00095140
int CSWCCMessageData::SaveData(CResGFF* pRes, CResStruct* pStruct)
{
    CResList lstList;
    CResStruct sElement;
    int i;

    pRes->WriteFieldBYTE(pStruct, m_nType, "Type");

    pRes->AddList(&lstList, pStruct, "IntList");

    for (i = 0; i < m_lstIntegers.num; i++)
    {
        pRes->AddListElement(&sElement, &lstList, 0xBAAD);
        pRes->WriteFieldINT(&sElement, m_lstIntegers.element[i],
                            "IntegerValue");
    }

    pRes->AddList(&lstList, pStruct, "FloatList");

    for (i = 0; i < m_lstFloats.num; i++)
    {
        pRes->AddListElement(&sElement, &lstList, 0xBAAD);
        pRes->WriteFieldFLOAT(&sElement, m_lstFloats.element[i], "FloatValue");
    }

    pRes->AddList(&lstList, pStruct, "ObjectIDList");

    for (i = 0; i < m_lstObjectIDs.num; i++)
    {
        pRes->AddListElement(&sElement, &lstList, 0xBAAD);
        pRes->WriteFieldDWORD(&sElement, m_lstObjectIDs.element[i],
                              "ObjectValue");
    }

    pRes->AddList(&lstList, pStruct, "StringList");

    for (i = 0; i < m_lstStrings.num; i++)
    {
        pRes->AddListElement(&sElement, &lstList, 0xBAAD);
        pRes->WriteFieldCExoString(&sElement, m_lstStrings.element[i],
                                   "StringValue");
    }

    return 1;
}
// PC 0x004D66B0  Xbox 0x00095330
void CSWMessage::SetReadMessage(unsigned char* pBuffer, unsigned long nSize,
                                unsigned long nRecipient, int nMsgType)
{
    m_pReadBuffer = pBuffer;
    m_nReadBufferSize = nSize;
    m_nReadIndex = 0;
    m_nRecipient = nRecipient;
    m_nMsgType = nMsgType;
    m_nReadFragmentsBufferSize = 0;
    m_nReadFragmentsIndex = 0;

    if (nSize == 0)
    {
        m_nReadFragmentsBitPos = m_nLastByteBits;
    }
}
// PC 0x004D66F0  Xbox 0x00095370
int CSWMessage::ReadBOOL()
{
    unsigned char bValue;

    if (m_nReadIndex + 4 > m_nReadBufferSize)
    {
        m_nReadIndex += 4;
        return 0;
    }

    bValue = m_pReadBuffer[m_nReadIndex];
    m_nReadIndex += 4;
    return bValue;
}
// PC 0x004D6720
char CSWMessage::ReadCHAR(int nBits)
{
    if (m_nReadIndex + 1 > m_nReadBufferSize)
    {
        m_nReadIndex += 1;
        return 0;
    }

    char cValue = *(char*)(m_pReadBuffer + m_nReadIndex);
    m_nReadIndex += 1;
    return cValue;
}
// PC 0x004D6750  Xbox 0x00094DD0
short CSWMessage::ReadSHORT(int nBits)
{
    if (m_nReadIndex + 2 > m_nReadBufferSize)
    {
        m_nReadIndex += 2;
        return 0;
    }

    short nValue = *(short*)(m_pReadBuffer + m_nReadIndex);
    m_nReadIndex += 2;
    return nValue;
}
// PC 0x004D6780
int CSWMessage::ReadINT(int nBits)
{
    if (m_nReadIndex + 4 > m_nReadBufferSize)
    {
        m_nReadIndex += 4;
        return 0;
    }
    int nValue = *(int*)(m_pReadBuffer + m_nReadIndex);
    m_nReadIndex += 4;
    return nValue;
}
// PC 0x004D6720  Xbox 0x000953A0
unsigned char CSWMessage::ReadBYTE(int nBits)
{
    unsigned char cValue;

    if (m_nReadIndex + 1 > m_nReadBufferSize)
    {
        cValue = 0;
    }
    else
    {
        cValue = m_pReadBuffer[m_nReadIndex];
    }

    m_nReadIndex += 1;

    return cValue;
}
// PC 0x004D6750
unsigned short CSWMessage::ReadWORD(int nBits)
{
    if (m_nReadIndex + 2 > m_nReadBufferSize)
    {
        m_nReadIndex += 2;
        return 0;
    }

    unsigned short nValue = *(unsigned short*)(m_pReadBuffer + m_nReadIndex);

    m_nReadIndex += 2;

    return nValue;
}
// PC 0x004D6780  Xbox 0x000953D0
unsigned long CSWMessage::ReadDWORD(int nBits)
{
    unsigned long nValue;

    if (m_nReadIndex + 4 > m_nReadBufferSize)
    {
        nValue = 0;
    }
    else
    {
        nValue = *(unsigned long*)(m_pReadBuffer + m_nReadIndex);
    }

    m_nReadIndex += 4;

    return nValue;
}
// PC 0x004D67B0  Xbox 0x00095400
float CSWMessage::ReadFLOAT(float fDefault, int nBits)
{
    float fValue;

    if (m_nReadIndex + 4 > m_nReadBufferSize)
    {
        fValue = 0.0f;
    }
    else
    {
        fValue = *(float*)(m_pReadBuffer + m_nReadIndex);
    }

    m_nReadIndex += 4;

    return fValue;
}
// PC 0x004D67E0  Xbox 0x00095430
CExoString CSWMessage::ReadCExoString(int nBits)
{
    int nLength = ReadINT(nBits);

    if (m_nReadIndex + nLength > m_nReadBufferSize)
    {
        m_nReadIndex += nLength;
        return CExoString("");
    }

    CExoString sValue((char*)(m_pReadBuffer + m_nReadIndex), nLength);

    m_nReadIndex += nLength;
    return sValue;
}
// PC 0x004D68A0  Xbox 0x00095500
void CSWMessage::CreateWriteMessage(unsigned long nMsgLen,
                                    unsigned long nRecipient, int nMsgType)
{
    m_nMsgType = nMsgType;
    m_nRecipient = nRecipient;
    EXOXBOXASSERT(m_bWritingMessage != 1);
    nMsgLen += 3;
    m_bWritingMessage = 1;
    m_nWriteIndex = 3;

    if (m_nWriteBufferSize < nMsgLen)
    {
        ExtendWriteBuffer(nMsgLen - m_nWriteBufferSize);
    }
}
// PC 0x004D68E0  Xbox 0x00095550
void CSWMessage::WriteBOOL(int bValue)
{
    if (m_nWriteIndex + 4 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(4);
    }

    *(int*)(m_pWriteBuffer + m_nWriteIndex) = bValue;
    m_nWriteIndex += 4;
    m_nTotalBytesWritten += 4;
}

static inline void TraceReadVector()
{
    TraceMessageRead();
}

static inline void TraceMessageRead()
{
}
// Dead code: not in the PC executable
void CSWCCMessageData::ClearData()
{
    m_lstIntegers.num = 0;
    m_lstFloats.num = 0;
    m_lstObjectIDs.num = 0;
    m_lstStrings.num = 0;
    m_nType = 0;
}
// Dead code: not in the PC executable
void CSWMessage::WriteDOUBLE(double fValue, double fDefault, int nBits)
{
    if (m_nWriteIndex + 8 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(8);
    }

    *(double*)(m_pWriteBuffer + m_nWriteIndex) = fValue;
    m_nWriteIndex += 8;
    m_nTotalBytesWritten += 8;
}
// Dead code: not in the PC executable
void CSWMessage::WriteDOUBLE(double fValue, double fLowerRange,
                             double fUpperRange, int nBits)
{
    if (m_nWriteIndex + 8 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(8);
    }

    *(double*)(m_pWriteBuffer + m_nWriteIndex) = fValue;
    m_nWriteIndex += 8;
    m_nTotalBytesWritten += 8;
}
// Dead code: not in the PC executable
void CSWMessage::WriteDWORD64(unsigned __int64 nValue, int nBits)
{
    if (m_nWriteIndex + 8 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(8);
    }

    *(unsigned __int64*)(m_pWriteBuffer + m_nWriteIndex) = nValue;
    m_nWriteIndex += 8;
    m_nTotalBytesWritten += 8;
}
// Dead code: not in the PC executable
void CSWMessage::ExtendWriteFragmentsBuffer(unsigned long nExtraBytes)
{
}
// Dead code: not in the PC executable
void CSWMessage::WriteINT64(__int64 nValue, int nBits)
{
    if (m_nWriteIndex + 8 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(8);
    }

    *(__int64*)(m_pWriteBuffer + m_nWriteIndex) = nValue;
    m_nWriteIndex += 8;
    m_nTotalBytesWritten += 8;
}
// Dead code: not in the PC executable
__int64 CSWMessage::ReadINT64(int nBits)
{
    if (m_nReadIndex + 8 > m_nReadBufferSize)
    {
        m_nReadIndex += 8;
        return 0;
    }
    __int64 nValue = *(__int64*)(m_pReadBuffer + m_nReadIndex);
    m_nReadIndex += 8;
    return nValue;
}
// Dead code: not in the PC executable
unsigned __int64 CSWMessage::ReadDWORD64(int nBits)
{
    unsigned __int64 nValue;

    if (m_nReadIndex + 8 > m_nReadBufferSize)
    {
        nValue = 0;
    }
    else
    {
        nValue = *(unsigned __int64*)(m_pReadBuffer + m_nReadIndex);
    }

    m_nReadIndex += 8;

    return nValue;
}
// Dead code: not in the PC executable
double CSWMessage::ReadDOUBLE(double fDefault, int nBits)
{
    double fValue;

    if (m_nReadIndex + 8 > m_nReadBufferSize)
    {
        fValue = 0.0;
    }
    else
    {
        fValue = *(double*)(m_pReadBuffer + m_nReadIndex);
    }

    m_nReadIndex += 8;

    return fValue;
}
// Dead code: not in the PC executable
double CSWMessage::ReadDOUBLE(double fLowerRange, double fUpperRange, int nBits)
{
    double fValue;

    if (m_nReadIndex + 8 > m_nReadBufferSize)
    {
        fValue = 0.0;
    }
    else
    {
        fValue = *(double*)(m_pReadBuffer + m_nReadIndex);
    }

    m_nReadIndex += 8;

    return fValue;
}
