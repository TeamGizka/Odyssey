#include "exobase.h"
#include "exoresman.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "clientexoappinternal.h"
#include "exotlktable.h"
#include "gameobjectarray.h"
#include "nwmessage.h"
// PC 0x005A0AF0 GOG 0x005A0B60
unsigned long CSWSMessage::ReadOBJECTIDServer()
{
    unsigned long nOldIndex = m_nReadIndex;
    unsigned long nNewIndex = nOldIndex + 4;
    unsigned long oidObject;

    if (nNewIndex > m_nReadBufferSize)
    {
        oidObject = 0;
    }
    else
    {
        unsigned long* pValue = (unsigned long*)(m_pReadBuffer + nOldIndex);
        oidObject = *pValue & 0x7FFFFFFF;
    }
    m_nReadIndex = nNewIndex;
    return oidObject;
}
// PC 0x005A0B20 GOG 0x005A0B90
void CSWSMessage::WriteOBJECTIDServer(unsigned long oidObject)
{
    if (m_nWriteIndex + 4 > m_nWriteBufferSize)
    {
        ExtendWriteBuffer(4);
    }
    if (oidObject != CGameObjectArray::INVALIDOBJID)
    {
        oidObject |= CGameObjectArray::LISTTYPE_MASK;
    }
    unsigned long* pValue = (unsigned long*)(m_pWriteBuffer + m_nWriteIndex);
    *pValue = oidObject;
    m_nWriteIndex += 4;
    m_nTotalBytesWritten += 4;
}
// PC 0x005A0B70 GOG 0x005A0BE0
void CSWSMessage::WriteCExoLocStringServer(CExoLocString& lsString,
                                           unsigned char nGender)
{
    CExoString sString;
    int nPlayerLanguage;
    int nModuleLanguage;
    unsigned char nStringGender;

    if (g_pAppManager->m_pServerExoApp == NULL)
    {
        return;
    }
    nPlayerLanguage =
        g_pAppManager->m_pServerExoApp->GetPlayerLanguage(m_nRecipient);
    nModuleLanguage = g_pAppManager->m_pServerExoApp->GetModuleLanguage();

    if (lsString.GetStringLoc(nPlayerLanguage, &sString, nGender))
    {
        if (m_nWriteIndex + sString.GetLength() + 4 > m_nWriteBufferSize)
        {
            ExtendWriteBuffer(sString.GetLength() + 4);
        }
        WriteBOOL(0);
        WriteCExoString(sString, 32);
        return;
    }
    if (nPlayerLanguage != 0 && lsString.GetStringLoc(0, &sString, nGender))
    {
        if (m_nWriteIndex + sString.GetLength() + 4 > m_nWriteBufferSize)
        {
            ExtendWriteBuffer(sString.GetLength() + 4);
        }
        WriteBOOL(0);
        WriteCExoString(sString, 32);
        return;
    }
    if (nModuleLanguage != nPlayerLanguage && nModuleLanguage != 0 &&
        lsString.GetStringLoc(nModuleLanguage, &sString, nGender))
    {
        if (m_nWriteIndex + sString.GetLength() + 4 > m_nWriteBufferSize)
        {
            ExtendWriteBuffer(sString.GetLength() + 4);
        }
        WriteBOOL(0);
        WriteCExoString(sString, 32);
        return;
    }
    if (lsString.GetString(0, &nPlayerLanguage, &sString, &nStringGender))
    {
        if (m_nWriteIndex + sString.GetLength() + 4 > m_nWriteBufferSize)
        {
            ExtendWriteBuffer(sString.GetLength() + 4);
        }
        WriteBOOL(0);
        WriteCExoString(sString, 32);
        return;
    }
    if ((int)lsString.m_dwStringRef != -1)
    {
        if (m_nWriteIndex + 9 > m_nWriteBufferSize)
        {
            ExtendWriteBuffer(9);
        }
        WriteBOOL(1);
        WriteBYTE(nGender, 8);
        unsigned long nStrRef = lsString.m_dwStringRef;
        WriteDWORD(nStrRef, 32);
    }
    else
    {
        if (m_nWriteIndex + sString.GetLength() > m_nWriteBufferSize)
        {
            ExtendWriteBuffer(sString.GetLength());
        }
        WriteBOOL(0);
        WriteCExoString(sString, 32);
    }
}
// PC 0x005A0E70 GOG 0x005A0EE0
int CSWSMessage::GetLocStringServer(unsigned long nPlayerId,
                                    CExoLocString& lsFirst,
                                    CExoLocString& lsSecond,
                                    CExoString& sResult, unsigned char nGender)
{
    CExoString sFirst;
    CExoString sSecond;
    int nPlayerLanguage;

    sResult = "";
    if (g_pAppManager->m_pServerExoApp == NULL)
    {
        return 0;
    }
    nPlayerLanguage =
        g_pAppManager->m_pServerExoApp->GetPlayerLanguage(nPlayerId);
    lsFirst.GetString(nPlayerLanguage, &sFirst, nGender);
    lsSecond.GetString(nPlayerLanguage, &sSecond, nGender);
    if (sSecond.IsEmpty() || sFirst.IsEmpty())
    {
        sResult = sFirst + sSecond;
    }
    else
    {
        CExoString sSpace(" ");
        sResult = sFirst + sSpace + sSecond;
    }
    return 1;
}
// PC 0x005A1020 GOG 0x005A1090
int CSWSMessage::GetLocStringServerTime(unsigned long nPlayerId,
                                        CExoLocString& lsString,
                                        CExoString& sResult, float& fTime,
                                        unsigned char nGender)
{
    CExoString sFirst;
    CExoString sSecond;
    STR_RES strRes;
    int nPlayerLanguage;
    int nModuleLanguage;
    unsigned char nStringGender;

    sResult = "";
    fTime = 0.0f;
    if (g_pAppManager->m_pServerExoApp == NULL)
    {
        return 0;
    }
    nPlayerLanguage =
        g_pAppManager->m_pServerExoApp->GetPlayerLanguage(nPlayerId);
    nModuleLanguage = g_pAppManager->m_pServerExoApp->GetModuleLanguage();
    if (!lsString.GetStringLoc(nPlayerLanguage, &sResult, nGender) &&
        (nPlayerLanguage == 0 ||
         !lsString.GetStringLoc(0, &sResult, nGender)) &&
        (nModuleLanguage == nPlayerLanguage || nModuleLanguage == 0 ||
         !lsString.GetStringLoc(nModuleLanguage, &sResult, nGender)) &&
        !lsString.GetString(0, &nPlayerLanguage, &sResult, &nStringGender))
    {
        if (lsString.m_dwStringRef != 0xFFFFFFFF)
        {
            g_pTlkTable->Fetch(lsString.m_dwStringRef, strRes, nStringGender);
            sResult = strRes.m_sString;
            fTime = strRes.m_fSoundLength + fTime;
        }
    }
    return 1;
}
