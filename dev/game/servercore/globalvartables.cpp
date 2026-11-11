#include <ctype.h>
#include <string.h>

#include "exobase.h"
#include "exoresman.h"
#include "resgff.h"
#include "twodimarray.h"
#include "nwsobject.h"
#include "nwsvartable.h"
#include "swmain.h"
// PC 0x00528DE0  Xbox 0x00162360
CSWGlobalVariableTable::~CSWGlobalVariableTable()
{
}
// PC 0x00528E50  Xbox 0x001623D0
int CSWGlobalVariableTable::Hash(const CExoString& sIdentifier)
{
    const CExoString* pIdentifier;
    unsigned int nHash;
    int i;
    char cCharacter;

    pIdentifier = &sIdentifier;
    nHash = 0;
    for (i = 0; i < pIdentifier->GetLength(); ++i)
    {
        cCharacter = (*pIdentifier)[i];
        nHash =
            m_anCRCTable[(nHash ^ toupper(cCharacter)) & 0xFF] ^ (nHash >> 8);
    }

    return nHash % 1775;
}
// PC 0x00528EC0  Xbox 0x00162440
int CSWGlobalVariableTable::AddIdentifier(const CExoString& sIdentifier,
                                          unsigned long nType,
                                          unsigned long nIndex)
{
    int nIdentifier;
    int nNextIdentifier;
    int nSteps;
    int nMaximumCollisionSteps;
    CSWGlobalVariable* pIdentifier;

    nIdentifier = Hash(sIdentifier);

    if (m_aIdentifiers[nIdentifier].m_szIdentifier[0] != 0)
    {
        nNextIdentifier = (nIdentifier + 1) % 1775;
        ++m_nCollisions;
        nSteps = 1;

        if (nIdentifier != nNextIdentifier)
        {
            do
            {
                if (m_aIdentifiers[nNextIdentifier].m_szIdentifier[0] == 0)
                {
                    break;
                }

                nNextIdentifier = (nNextIdentifier + 1) % 1775;
                ++nSteps;
            } while (nIdentifier != nNextIdentifier);

            if (nIdentifier != nNextIdentifier)
            {
                nIdentifier = nNextIdentifier;
            }
#ifdef _XBOX
            else
            {
                EXOXBOXASSERT(0);
            }
#endif
        }

        nMaximumCollisionSteps = m_nMaximumCollisionSteps;
        m_nCollisionSteps += nSteps;
        if (nSteps > nMaximumCollisionSteps)
        {
            m_nMaximumCollisionSteps = nSteps;
        }
    }

    strcpy(m_aIdentifiers[nIdentifier].m_szIdentifier, sIdentifier.CStr());
    pIdentifier = &m_aIdentifiers[nIdentifier];
    pIdentifier->m_nType = nType;
    pIdentifier->m_nIndex = nIndex;

    return nIdentifier;
}
// PC 0x00528F70  Xbox 0x00162500
int CSWGlobalVariableTable::AddIdentifierBoolean(const CExoString& sIdentifier)
{
    int nIndex = m_nBooleanCount;
    int nIdentifier = -1;
    if (nIndex < 900)
    {
        nIdentifier = AddIdentifier(sIdentifier, 0, nIndex);
        m_nBooleanCount++;
    }
    return nIdentifier;
}
// PC 0x00528FA0  Xbox 0x00162530
int CSWGlobalVariableTable::AddIdentifierNumber(const CExoString& sIdentifier)
{
    int nIndex = m_nNumberCount;
    int nIdentifier = -1;
    if (nIndex < 500)
    {
        nIdentifier = AddIdentifier(sIdentifier, 1, nIndex);
        m_nNumberCount++;
    }
    return nIdentifier;
}
// PC 0x00528FD0  Xbox 0x00162560
int CSWGlobalVariableTable::AddIdentifierLocation(const CExoString& sIdentifier)
{
    int nCount = m_nLocationCount;
    int nResult = -1;

    if (nCount < 100)
    {
        nResult = AddIdentifier(sIdentifier, 2, nCount);
        ++m_nLocationCount;
    }

    return nResult;
}
// PC 0x00529000  Xbox 0x00162590
int CSWGlobalVariableTable::AddIdentifierString(const CExoString& sIdentifier)
{
    int nCount = m_nStringCount;
    int nResult = -1;

    if (nCount < 5)
    {
        nResult = AddIdentifier(sIdentifier, 3, nCount);
        ++m_nStringCount;
    }

    return nResult;
}
// PC 0x00529030  Xbox 0x001625C0
int CSWGlobalVariableTable::FindIdentifierIndex(const CExoString& sIdentifier)
{
    int nIdentifier = Hash(sIdentifier);

    if (m_aIdentifiers[nIdentifier].m_szIdentifier[0] != 0 &&
        sIdentifier.CompareNoCase(m_aIdentifiers[nIdentifier].m_szIdentifier) ==
            0)
    {
        for (int i = (nIdentifier + 1) % 1775; nIdentifier != i;
             i = (i + 1) % 1775)
        {
            if (m_aIdentifiers[i].m_szIdentifier[0] == 0)
            {
                break;
            }

            if (sIdentifier.CompareNoCase(m_aIdentifiers[i].m_szIdentifier) !=
                0)
            {
                break;
            }
        }
        nIdentifier = i;
    }

    if (sIdentifier.CompareNoCase(m_aIdentifiers[nIdentifier].m_szIdentifier) ==
        0)
    {
        return -1;
    }

    return nIdentifier;
}
// PC 0x005290C0  Xbox 0x00162650
void CSWGlobalVariableTable::SetBit(unsigned char* pValues, int nIndex,
                                    int bValue)
{
    int nByte = nIndex / 8;
    int nBit = nIndex - 8 * nByte;
    char cMask = 1 << (7 - nBit);

    if (bValue != 0)
    {
        pValues[nByte] |= cMask;
    }
    else
    {
        pValues[nByte] &= ~cMask;
    }
}
// PC 0x00529110  Xbox 0x001626A0
void CSWGlobalVariableTable::GetValueBoolean(const CExoString& sIdentifier,
                                             int& bValue)
{
    int nIdentifier = FindIdentifierIndex(sIdentifier);
    int nValueIndex;

    bValue = 0;
    if (nIdentifier < 0)
    {
        CExoString sError;
        sError.Format("Script var BOOLEAN '%s' not in catalogue!",
                      sIdentifier.CStr());
    }
    else
    {
        if (m_aIdentifiers[nIdentifier].m_nType != 0)
        {
            CExoString sError;
            sError.Format("Script var '%s' is not BOOLEAN!",
                          sIdentifier.CStr());
        }
        else
        {
            nValueIndex = m_aIdentifiers[nIdentifier].m_nIndex;
            bValue = (m_aBooleanValues[nValueIndex / 8] >>
                      (8 * (nValueIndex / 8) - nValueIndex + 7)) &
                     1;
        }
    }
}
// PC 0x00529240  Xbox 0x001627D0
void CSWGlobalVariableTable::GetValueNumber(const CExoString& sIdentifier,
                                            char& cValue)
{
    int nIdentifier = FindIdentifierIndex(sIdentifier);

    cValue = 0;
    if (nIdentifier < 0)
    {
        CExoString sError;
        sError.Format("Script var NUMBER '%s' not in catalogue!",
                      sIdentifier.CStr());
    }
    else
    {
        if (m_aIdentifiers[nIdentifier].m_nType != 1)
        {
            CExoString sError;
            sError.Format("Script var '%s' not a NUMBER!", sIdentifier.CStr());
        }
        else
        {
            cValue = m_acNumberValues[m_aIdentifiers[nIdentifier].m_nIndex];
        }
    }
}
// PC 0x00529350  Xbox 0x001628E0
void CSWGlobalVariableTable::GetValueLocation(const CExoString& sIdentifier,
                                              CScriptLocation& lValue)
{
    int nIdentifier = FindIdentifierIndex(sIdentifier);

    if (nIdentifier < 0)
    {
        CExoString sError;
        sError.Format("Script var LOCATION '%s' not in catalogue!",
                      sIdentifier.CStr());
    }
    else
    {
        if (m_aIdentifiers[nIdentifier].m_nType != 2)
        {
            CExoString sError;
            sError.Format("Script var '%s' not a LOCATION!",
                          sIdentifier.CStr());
        }
        else
        {
            lValue = m_aLocations[m_aIdentifiers[nIdentifier].m_nIndex];
        }
    }
}
// PC 0x00529460  Xbox 0x00162A10
void CSWGlobalVariableTable::GetValueString(const CExoString& sIdentifier,
                                            CExoString& sValue)
{
    int nIdentifier = FindIdentifierIndex(sIdentifier);

    if (nIdentifier < 0)
    {
        CExoString sError;
        sError.Format("Script var STRING '%s' not in catalogue!",
                      sIdentifier.CStr());
    }
    else
    {
        if (m_aIdentifiers[nIdentifier].m_nType != 3)
        {
            CExoString sError;
            sError.Format("Script var '%s' not a STRING!", sIdentifier.CStr());
        }
        else
        {
            sValue = m_asStrings[m_aIdentifiers[nIdentifier].m_nIndex];
        }
    }
}
// PC 0x00529570  Xbox 0x00162B20
void CSWGlobalVariableTable::SetValueBoolean(const CExoString& sIdentifier,
                                             int bValue)
{
    int nIdentifier = FindIdentifierIndex(sIdentifier);

    if (nIdentifier < 0)
    {
        CExoString sError;
        sError.Format("Script var BOOLEAN '%s' not in catalogue!",
                      sIdentifier.CStr());
    }
    else
    {
        if (m_aIdentifiers[nIdentifier].m_nType != 0)
        {
            CExoString sError;
            sError.Format("Script var '%s' not a BOOLEAN!", sIdentifier.CStr());
        }
        else
        {
            SetBit(m_aBooleanValues, m_aIdentifiers[nIdentifier].m_nIndex,
                   bValue);
        }
    }
}
// PC 0x00529680  Xbox 0x00162C30
void CSWGlobalVariableTable::SetValueNumber(const CExoString& sIdentifier,
                                            char cValue)
{
    int nIdentifier = FindIdentifierIndex(sIdentifier);

    if (nIdentifier < 0)
    {
        CExoString sError;
        sError.Format("Script var NUMBER '%s' not in catalogue!",
                      sIdentifier.CStr());
    }
    else
    {
        if (m_aIdentifiers[nIdentifier].m_nType != 1)
        {
            CExoString sError;
            sError.Format("Script var '%s' not a NUMBER!", sIdentifier.CStr());
        }
        else
        {
            m_acNumberValues[m_aIdentifiers[nIdentifier].m_nIndex] = cValue;
        }
    }
}
// PC 0x00529790  Xbox 0x00162D40
void CSWGlobalVariableTable::SetValueLocation(const CExoString& sIdentifier,
                                              const CScriptLocation& lValue)
{
    int nIdentifier = FindIdentifierIndex(sIdentifier);

    if (nIdentifier < 0)
    {
        CExoString sError;
        sError.Format("Script var LOCATION '%s' not in catalogue!",
                      sIdentifier.CStr());
    }
    else
    {
        if (m_aIdentifiers[nIdentifier].m_nType != 2)
        {
            CExoString sError;
            sError.Format("Script var '%s' not a LOCATION!",
                          sIdentifier.CStr());
        }
        else
        {
            m_aLocations[m_aIdentifiers[nIdentifier].m_nIndex] = lValue;
        }
    }
}
// PC 0x005298A0  Xbox 0x00162E70
void CSWGlobalVariableTable::SetValueString(const CExoString& sIdentifier,
                                            const CExoString& sValue)
{
    int nIdentifier = FindIdentifierIndex(sIdentifier);

    if (nIdentifier < 0)
    {
        CExoString sError;
        sError.Format("Script var STRING '%s' not in catalogue!",
                      sIdentifier.CStr());
    }
    else
    {
        if (m_aIdentifiers[nIdentifier].m_nType != 3)
        {
            CExoString sError;
            sError.Format("Script var '%s' not a STRING!", sIdentifier.CStr());
        }
        else
        {
            m_asStrings[m_aIdentifiers[nIdentifier].m_nIndex] = sValue;
        }
    }
}
// PC 0x005299B0  Xbox 0x00162F80
void CSWGlobalVariableTable::WriteTable(const CExoString& sFileName,
                                       int bWriteCatalogue)
{
    unsigned char aBooleanValues[113];
    char acNumberValues[500];
    CScriptLocation aLocations[100];
    CExoString asStrings[5];
    CExoString sAlias;
    CResStruct sTop;
    CResStruct sEntry;
    CResStruct sString;
    CResList lBooleans;
    CResList lNumbers;
    CResList lLocations;
    CResList lStrings;
    CResList lValues;
    CResList* pCatalogue;

    int nAliasEnd = sFileName.Find(":", 0);
    if (nAliasEnd != -1)
    {
        sAlias = sFileName.Left(nAliasEnd + 1);
    }

    CResGFF* pRes = new CResGFF;
    pRes->CreateGFFFile(&sTop, "GVT ", "V2.0");

    if (bWriteCatalogue)
    {
        pRes->AddList(&lBooleans, &sTop, "CatBoolean");
        pRes->AddList(&lNumbers, &sTop, "CatNumber");
        pRes->AddList(&lLocations, &sTop, "CatLocation");
        pRes->AddList(&lStrings, &sTop, "CatString");
    }

    memset(aBooleanValues, 0, sizeof(aBooleanValues));
    memset(acNumberValues, 0, sizeof(acNumberValues));
    int nBoolean = 0;
    int nNumber = 0;
    int nLocation = 0;
    int nString = 0;

    for (int i = 0; i < 1775; ++i)
    {
        if (m_aIdentifiers[i].m_szIdentifier[0] != 0)
        {
            switch (m_aIdentifiers[i].m_nType)
            {
            case CSWGlobalVariable::TYPE_BOOLEAN:
                {
                    int nIndex = m_aIdentifiers[i].m_nIndex;
                    int nByte = nIndex / 8;
                    int nBit = nIndex - 8 * nByte;
                    int bValue = (m_aBooleanValues[nByte] >> (7 - nBit)) & 1;
                    SetBit(aBooleanValues, nBoolean, bValue);
                    ++nBoolean;
                    pCatalogue = &lBooleans;
                }
                break;
            case CSWGlobalVariable::TYPE_NUMBER:
                acNumberValues[nNumber] =
                    m_acNumberValues[m_aIdentifiers[i].m_nIndex];
                pCatalogue = &lNumbers;
                ++nNumber;
                break;
            case CSWGlobalVariable::TYPE_LOCATION:
                aLocations[nLocation] =
                    m_aLocations[m_aIdentifiers[i].m_nIndex];
                pCatalogue = &lLocations;
                ++nLocation;
                break;
            case CSWGlobalVariable::TYPE_STRING:
                asStrings[nString] =
                    m_asStrings[m_aIdentifiers[i].m_nIndex];
                pCatalogue = &lStrings;
                ++nString;
                break;
#ifdef _XBOX
            default:
                EXOXBOXASSERT(0);
                break;
#endif
            }

            if (bWriteCatalogue)
            {
                pRes->AddListElement(&sEntry, pCatalogue, 0);
                pRes->WriteFieldCExoString(&sEntry,
                                          m_aIdentifiers[i].m_szIdentifier,
                                          "Name");
            }
        }
    }

    pRes->WriteFieldVOID(&sTop, aBooleanValues, m_nBooleanCount / 8 + 1,
                         "ValBoolean");
    pRes->WriteFieldVOID(&sTop, acNumberValues, m_nNumberCount, "ValNumber");
    pRes->WriteFieldVOID(&sTop, aLocations, sizeof(aLocations), "ValLocation");
    pRes->AddList(&lValues, &sTop, "ValString");
    for (int i = 0; i < nString; ++i)
    {
        pRes->AddListElement(&sString, &lValues, 0);
        pRes->WriteFieldCExoString(&sString, asStrings[i], "String");
    }

    pRes->WriteGFFFile(sFileName, 0);
    delete pRes;
    if (sAlias == "REBOOTDATA:")
    {
        g_pExoResMan->CreateSignatureFile(sFileName, 0, 0, 0);
    }
}
// PC 0x00529EF0  Xbox 0x001634C0
void CSWGlobalVariableTable::ReadTableWithoutCatalogue(CResGFF* pRes,
                                                      CResStruct& sStruct)
{
    unsigned char aBooleanValues[113];
    char acNumberValues[500];
    CScriptLocation aLocations[100];
    CExoString asStrings[5];
    CResList lStrings;
    CResStruct sString;
    int bSuccess;
    char cDefault = 0;
    int nString = 0;

    memset(aBooleanValues, 0, sizeof(aBooleanValues));
    memset(acNumberValues, 0, sizeof(acNumberValues));
    pRes->ReadFieldVOID(&sStruct, aBooleanValues, sizeof(aBooleanValues),
                        "ValBoolean", bSuccess, &cDefault);
    pRes->ReadFieldVOID(&sStruct, acNumberValues, sizeof(acNumberValues),
                        "ValNumber", bSuccess, &cDefault);
    pRes->ReadFieldVOID(&sStruct, aLocations, sizeof(aLocations),
                        "ValLocation", bSuccess, &cDefault);

    if (pRes->GetList(&lStrings, &sStruct, "ValString"))
    {
        nString = pRes->GetListCount(&lStrings) > 5 ?
                      5 : pRes->GetListCount(&lStrings);
        for (int i = 0; i < nString; ++i)
        {
            pRes->GetListElement(&sString, &lStrings, i);
            asStrings[i] = pRes->ReadFieldCExoString(&sString, "String",
                                                     bSuccess, "");
        }
    }

    int nBoolean = 0;
    int nNumber = 0;
    int nLocation = 0;

    for (int i = 0; i < 1775; ++i)
    {
        if (m_aIdentifiers[i].m_szIdentifier[0] != 0)
        {
            switch (m_aIdentifiers[i].m_nType)
            {
            case CSWGlobalVariable::TYPE_BOOLEAN:
                {
                    int nByte = nBoolean / 8;
                    int nBit = nBoolean - 8 * nByte;
                    int bValue = (aBooleanValues[nByte] >> (7 - nBit)) & 1;
                    SetBit(m_aBooleanValues, m_aIdentifiers[i].m_nIndex,
                           bValue);
                }
                ++nBoolean;
                break;
            case CSWGlobalVariable::TYPE_NUMBER:
                m_acNumberValues[m_aIdentifiers[i].m_nIndex] =
                    acNumberValues[nNumber++];
                break;
            case CSWGlobalVariable::TYPE_LOCATION:
                m_aLocations[m_aIdentifiers[i].m_nIndex] =
                    aLocations[nLocation++];
                break;
            case CSWGlobalVariable::TYPE_STRING:
                m_asStrings[m_aIdentifiers[i].m_nIndex] =
                    asStrings[nString++];
                break;
#ifdef _XBOX
            default:
                EXOXBOXASSERT(0);
                break;
#endif
            }
        }
    }
}

void CSWGlobalVariableTable::ClearValues()
{
    memset(m_aBooleanValues, 0, sizeof(m_aBooleanValues));
    memset(m_acNumberValues, 0, sizeof(m_acNumberValues));
    memset(m_aLocations, 0, sizeof(m_aLocations));
    memset(m_asStrings, 0, sizeof(m_asStrings));
}
// PC 0x0052A280  Xbox 0x00163880
void CSWGlobalVariableTable::ReadTableWithCatalogue(CResGFF* pRes,
                                                   CResStruct& sStruct)
{
    unsigned char aBooleanValues[113];
    char acNumberValues[500];
    CScriptLocation aLocations[100];
    CExoString asStrings[5];
    CExoString sName;
    CResList lCatalogue;
    CResStruct sEntry;
    CResList lStrings;
    CResStruct sString;
    int bSuccess;
    int nIdentifier;
    int i;
    char cDefault = 0;

    ClearValues();
    memset(aBooleanValues, 0, sizeof(aBooleanValues));
    memset(acNumberValues, 0, sizeof(acNumberValues));
    pRes->ReadFieldVOID(&sStruct, aBooleanValues, sizeof(aBooleanValues),
                        "ValBoolean", bSuccess, &cDefault);
    if (bSuccess)
    {
        pRes->GetList(&lCatalogue, &sStruct, "CatBoolean");
        for (i = 0; i < (int)pRes->GetListCount(&lCatalogue); ++i)
        {
            pRes->GetListElement(&sEntry, &lCatalogue, i);
            sName = pRes->ReadFieldCExoString(&sEntry, "Name", bSuccess, "");
            if (bSuccess && sName != "")
            {
                nIdentifier = FindIdentifierIndex(sName);
                if (nIdentifier < 0)
                {
                    nIdentifier = AddIdentifierBoolean(sName);
                    if (nIdentifier < 0)
                    {
                        CExoString sError;
                        sError.Format("ReadTableWithCat(): BOOLEAN '%s' won't fit!",
                                      sName.CStr());
                    }
                }
                int nByte = i / 8;
                int nBit = i - 8 * nByte;
                int bValue = (aBooleanValues[nByte] >> (7 - nBit)) & 1;
                SetBit(m_aBooleanValues, m_aIdentifiers[nIdentifier].m_nIndex,
                       bValue);
            }
        }
    }

    pRes->ReadFieldVOID(&sStruct, acNumberValues, sizeof(acNumberValues),
                        "ValNumber", bSuccess, &cDefault);
    if (bSuccess)
    {
        pRes->GetList(&lCatalogue, &sStruct, "CatNumber");
        for (i = 0; i < (int)pRes->GetListCount(&lCatalogue); ++i)
        {
            pRes->GetListElement(&sEntry, &lCatalogue, i);
            sName = pRes->ReadFieldCExoString(&sEntry, "Name", bSuccess, "");
            if (bSuccess && sName != "")
            {
                nIdentifier = FindIdentifierIndex(sName);
                if (nIdentifier < 0)
                {
                    nIdentifier = AddIdentifierNumber(sName);
                    if (nIdentifier < 0)
                    {
                        CExoString sError;
                        sError.Format("ReadTableWithCat(): NUMBER '%s' won't fit!",
                                      sName.CStr());
                    }
                }
                m_acNumberValues[m_aIdentifiers[nIdentifier].m_nIndex] =
                    acNumberValues[i];
            }
        }
    }

    pRes->ReadFieldVOID(&sStruct, aLocations, sizeof(aLocations),
                        "ValLocation", bSuccess, &cDefault);
    if (bSuccess)
    {
        pRes->GetList(&lCatalogue, &sStruct, "CatLocation");
        for (i = 0; i < (int)pRes->GetListCount(&lCatalogue); ++i)
        {
            pRes->GetListElement(&sEntry, &lCatalogue, i);
            sName = pRes->ReadFieldCExoString(&sEntry, "Name", bSuccess, "");
            if (bSuccess && sName != "")
            {
                nIdentifier = FindIdentifierIndex(sName);
                if (nIdentifier < 0)
                {
                    nIdentifier = AddIdentifierLocation(sName);
                    if (nIdentifier < 0)
                    {
                        CExoString sError;
                        sError.Format("ReadTableWithCat(): LOCATION '%s' won't fit!",
                                      sName.CStr());
                    }
                }
                m_aLocations[m_aIdentifiers[nIdentifier].m_nIndex] =
                    aLocations[i];
            }
        }
    }

    if (pRes->GetList(&lStrings, &sStruct, "ValString"))
    {
        int nString = pRes->GetListCount(&lStrings) > 5 ?
                          5 : pRes->GetListCount(&lStrings);
        for (i = 0; i < nString; ++i)
        {
            pRes->GetListElement(&sString, &lStrings, i);
            asStrings[i] = pRes->ReadFieldCExoString(&sString, "String",
                                                     bSuccess, "");
        }

        pRes->GetList(&lCatalogue, &sStruct, "CatString");
        for (i = 0; i < (int)pRes->GetListCount(&lCatalogue); ++i)
        {
            pRes->GetListElement(&sEntry, &lCatalogue, i);
            sName = pRes->ReadFieldCExoString(&sEntry, "Name", bSuccess, "");
            if (bSuccess && sName != "")
            {
                nIdentifier = FindIdentifierIndex(sName);
                if (nIdentifier < 0)
                {
                    nIdentifier = AddIdentifierString(sName);
                    if (nIdentifier < 0)
                    {
                        CExoString sError;
                        sError.Format("ReadTableWithCat(): STRING '%s' won't fit!",
                                      sName.CStr());
                    }
                }
                m_asStrings[m_aIdentifiers[nIdentifier].m_nIndex] =
                    asStrings[i];
            }
        }
    }
}
// PC 0x0052ABE0  Xbox 0x00164220
int CSWGlobalVariableTable::ReadTable(const CExoString& sFileName)
{
    CResStruct sTop;
    CResList lCatalogue;

    CResGFF* pRes = new CResGFF(0, "GVT ", sFileName);
    if (pRes == NULL || pRes->m_bLoaded == 0)
    {
#ifdef _XBOX
        HandleLoadError(0xA3A8);
#else
        HandleLoadError(0xBC4E);
#endif
        delete pRes;
        return 0;
    }

    pRes->GetTopLevelStruct(&sTop);
    if (pRes->GetList(&lCatalogue, &sTop, "CatBoolean"))
    {
        ReadTableWithCatalogue(pRes, sTop);
    }
    else
    {
        ReadTableWithoutCatalogue(pRes, sTop);
    }
    delete pRes;
    return 1;
}
// PC 0x0052AD10  Xbox 0x00164350
void CSWGlobalVariableTable::Save(const CExoString& sDirectory)
{
    CExoString sFileName = sDirectory + "\\" + "GLOBALVARS";

    WriteTable(sFileName, 1);
}
// PC 0x0052ADE0  Xbox 0x00164420
int CSWGlobalVariableTable::Load()
{
    CExoString sFileName("GLOBALVARS");

    return ReadTable(sFileName);
}
// PC 0x0052AE50  Xbox 0x00164520
void CSWGlobalVariableTable::ReadCatalogue(const CResRef& cResRef)
{
    C2DA cCatalogue(cResRef, 1);
    CExoString sName;
    CExoString sType;

    if (cCatalogue.Load2DArray())
    {
        for (int i = 0; i < cCatalogue.m_nNumRows; ++i)
        {
            if (cCatalogue.GetCExoStringEntry(i, "Name", &sName))
            {
                if (sName.GetLength() < 22)
                {
                    if (cCatalogue.GetCExoStringEntry(i, "Type", &sType))
                    {
                        if (FindIdentifierIndex(sName) < 0)
                        {
                            int nIdentifier = 0;

                            if (sType.CompareNoCase("BOOLEAN"))
                            {
                                nIdentifier = AddIdentifierBoolean(sName);
                            }
                            else if (sType.CompareNoCase("NUMBER"))
                            {
                                nIdentifier = AddIdentifierNumber(sName);
                            }
                            else if (sType.CompareNoCase("LOCATION"))
                            {
                                nIdentifier = AddIdentifierLocation(sName);
                            }
                            else if (sType.CompareNoCase("STRING"))
                            {
                                nIdentifier = AddIdentifierString(sName);
                            }
                            else
                            {
                                CExoString sError;
                                sError.Format("Script var '%s' has bad type in catalogue!",
                                              sName.CStr());
                            }

                            if (nIdentifier < 0)
                            {
                                CExoString sError;
                                sError.Format("Script var '%s' '%s' won't fit in table!",
                                              sType.CStr(), sName.CStr());
                            }
                        }
                        else
                        {
                            CExoString sError;
                            sError.Format("Script var '%s' duplicated in catalogue!",
                                          sName.CStr());
                        }
                    }
                }
                else
                {
                    CExoString sError;
                    sError.Format("Script var '%s' longer than %d letters!",
                                  sName.CStr(), 21);
                }
            }
        }
    }
}

void CSWGlobalVariableTable::ClearIdentifiers()
{
    memset(m_aIdentifiers, 0, sizeof(m_aIdentifiers));
    m_nBooleanCount = 0;
    m_nNumberCount = 0;
    m_nLocationCount = 0;
    m_nStringCount = 0;
    m_nCollisions = 0;
    m_nCollisionSteps = 0;
    m_nMaximumCollisionSteps = 0;
    ClearValues();
}
// PC 0x0052B170  Xbox 0x00164840
CSWGlobalVariableTable::CSWGlobalVariableTable()
{
    unsigned int nCRC;
    int i;

    for (unsigned int nValue = 0; nValue <= 0xFF; ++nValue)
    {
        nCRC = nValue;
        for (i = 8; i != 0; --i)
        {
            if (nCRC & 1)
            {
                nCRC = (nCRC >> 1) ^ 0xEDB88320;
            }
            else
            {
                nCRC >>= 1;
            }
        }
        m_anCRCTable[nValue] = nCRC;
    }

    ClearIdentifiers();
}
