#include "nwmessage.h"
#include "exotlktable.h"

inline void ByteSwap(unsigned long* pValue)
{
}

class CSWCMessageObjectID
{
public:
    CSWCMessageObjectID(unsigned char* pData)
    {
        m_pData = pData;
    }
    ~CSWCMessageObjectID()
    {
    }

    unsigned long GetObjectID() const
    {
        return *(unsigned long*)m_pData;
    }

private:
    unsigned char* m_pData;
};
// PC 0x00692630  Xbox 0x000C1300
unsigned long CSWCMessage::ReadOBJECTIDClient()
{
    unsigned long oidObject = (m_nReadIndex + sizeof(unsigned long) > m_nReadBufferSize)
        ? 0
        : CSWCMessageObjectID(m_pReadBuffer + m_nReadIndex).GetObjectID();
    ByteSwap(&oidObject);
    m_nReadIndex += sizeof(unsigned long);
    return oidObject;
}
// PC 0x004D68E0
void CSWCMessage::WriteOBJECTIDClient(unsigned long oidObject)
{
    if (m_nWriteIndex + sizeof(oidObject) > m_nWriteBufferSize)
        ExtendWriteBuffer(sizeof(oidObject));
    *(unsigned long*)(m_pWriteBuffer + m_nWriteIndex) = oidObject;
    m_nWriteIndex += sizeof(oidObject);
    m_nTotalBytesWritten += sizeof(oidObject);
}
// PC 0x00692660  Xbox 0x000C1330
CExoString CSWCMessage::ReadCExoLocStringClient(CResRef& cResRef,
                                             float* pfLength)
{
    CExoString sResult;

    cResRef = "";
    if (pfLength != NULL)
    {
        *pfLength = 0.0f;
    }

    int bStringReference = ReadBOOL();

    if (bStringReference != 0)
    {
        STR_RES strRes;
        unsigned char nGender = ReadBYTE(8);
        unsigned long nStrRef = ReadDWORD(32);

        if (!g_pTlkTable->Fetch(nStrRef, strRes, nGender))
        {
            return CExoString("");
        }

        cResRef = strRes.m_resSound;
        if (pfLength != NULL)
        {
            *pfLength = strRes.m_fSoundLength;
        }
        return strRes.m_sString;
    }

    return ReadCExoString(32);
}

CExoString CSWCMessage::ReadCExoLocStringClient()
{
    CResRef cResRef;
    return ReadCExoLocStringClient(cResRef, NULL);
}
