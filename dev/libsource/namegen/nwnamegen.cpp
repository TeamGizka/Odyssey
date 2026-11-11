#include "nwrules.h"

#include "twodimarray.h"

#include <ctype.h>
#include <stdlib.h>
// PC 0x00711090  Xbox 0x0027BB40
void CSWNameGen::UnloadNameTable()
{
    if (m_bNameTableLoaded != 0)
    {
        m_bNameTableLoaded = 0;
        m_pRes->Release();
    }
}
// PC 0x00711290  Xbox 0x0027BD40
CSWNameGen::CSWNameGen()
    : CResHelper<CResLTR, RESTYPE_LTR>()
{
    m_bNameTableLoaded = 0;
    m_nRace = 0;
    m_nGender = 1;
}
// PC 0x007112E0  Xbox 0x0027BD90
CSWNameGen::~CSWNameGen()
{
    UnloadNameTable();
}
// PC 0x00711340  Xbox 0x0027BDF0
int CSWNameGen::LoadNameTable(const CExoString& sNameTable)
{
    CResRef cResRef(sNameTable);

    SetResRef(cResRef, 0);
    if (m_pRes->Demand() == 0)
    {
        return 0;
    }

    m_bNameTableLoaded = 1;
    return 1;
}
// PC 0x00711390  Xbox 0x0027BE40
CExoString CSWNameGen::GetRandomName(unsigned char nLength)
{
    CExoString sFilterName;
    char szName[64];
    int nFirst;
    int nSecond;
    int nThird;
    int nCount;
    int bNewName;
    int nLetterCount;
    int bDone;
    unsigned char nRetries;
    float fRandom;

    bNewName = 1;
    bDone = 0;
    nRetries = 0;
    nLetterCount = m_pRes->GetLTRHeaderPtr()->m_nLetterCount;

    do
    {
        if (bNewName)
        {
            do
            {
                fRandom = rand() / 32767.0f;
                nFirst = 0;
                if (0 < nLetterCount)
                {
                    do
                    {
                        if (fRandom < m_pRes->GetStartSingleLetterProb()[nFirst])
                        {
                            break;
                        }
                        ++nFirst;
                    } while (nFirst < nLetterCount);
                }
                if (nFirst < nLetterCount)
                {
                    fRandom = rand() / 32767.0f;
                    nSecond = 0;
                    do
                    {
                        if (fRandom < m_pRes->GetStartDoubleLetterProb()[nFirst][nSecond])
                        {
                            break;
                        }
                        ++nSecond;
                    } while (nSecond < nLetterCount);
                    if (nSecond < nLetterCount)
                    {
                        fRandom = rand() / 32767.0f;
                        nThird = 0;
                        do
                        {
                            if (fRandom < m_pRes->GetStartTripleLetterProb()[nFirst][nSecond][nThird])
                            {
                                break;
                            }
                            ++nThird;
                        } while (nThird < nLetterCount);
                    }
                }
            } while (nFirst >= nLetterCount || nSecond >= nLetterCount ||
                     nThird >= nLetterCount);

            nCount = 0;
            szName[nCount++] = nFirst == 26   ? '\''
                               : nFirst == 27 ? '-'
                                              : nFirst + 'a';
            szName[nCount++] = nSecond == 26   ? '\''
                               : nSecond == 27 ? '-'
                                               : nSecond + 'a';
            szName[nCount++] = nThird == 26   ? '\''
                               : nThird == 27 ? '-'
                                              : nThird + 'a';
            bNewName = 0;
        }

        if (nThird < nLetterCount)
        {
            nFirst = nSecond;
            nSecond = nThird;
        }

        fRandom = rand() / 32767.0f;
        if (rand() % 12 <= nCount || (nLength > 0 && nCount == nLength - 1))
        {
            nThird = 0;
            if (nLetterCount > 0)
            {
                do
                {
                    if (fRandom < m_pRes->GetEndTripleLetterProb()[nFirst][nSecond][nThird])
                    {
                        bDone = 1;
                        break;
                    }
                    ++nThird;
                } while (nThird < nLetterCount);
            }
            if (!bDone)
            {
                bNewName = 1;
                ++nRetries;
            }
        }

        if (!bNewName)
        {
            if (!bDone)
            {
                nThird = 0;
                if (nLetterCount > 0)
                {
                    do
                    {
                        if (fRandom < m_pRes->GetMiddleTripleLetterProb()[nFirst][nSecond][nThird])
                        {
                            break;
                        }
                        ++nThird;
                    } while (nThird < nLetterCount);
                }
            }

            if (nThird < nLetterCount)
            {
                szName[nCount++] = nThird == 26   ? '\''
                                   : nThird == 27 ? '-'
                                                  : nThird + 'a';
            }
            else if (nCount > 3)
            {
                nSecond = szName[nCount - 2] == '\''  ? 26
                          : szName[nCount - 2] == '-' ? 27
                                                      : szName[nCount - 2] - 'a';
                nFirst = szName[nCount - 3] == '\''  ? 26
                         : szName[nCount - 3] == '-' ? 27
                                                     : szName[nCount - 3] - 'a';
                --nCount;
            }
            else
            {
                bNewName = 1;
                ++nRetries;
            }
        }

    } while (nRetries < 5 && !bDone);

    if (nRetries >= 5)
    {
        return CExoString("");
    }

    szName[nCount] = 0;
    szName[0] = toupper(szName[0]);
    sFilterName = szName;

    CExoString sName;
    C2DA* pFilter = new C2DA("NameFilter", 0);
    if (pFilter == NULL)
    {
        return sFilterName;
    }

    if (!pFilter->Load2DArray())
    {
        delete pFilter;
        return sFilterName;
    }

    int i;
    int bAllowed = 1;
    for (i = 0; i < pFilter->GetNumRows(); ++i)
    {
        if (pFilter->GetCExoStringEntry(i, "Name", &sName))
        {
            if (sName.CompareNoCase(sFilterName))
            {
                bAllowed = 0;
                break;
            }
        }
    }
    pFilter->Unload2DArray();
    delete pFilter;

    if (!bAllowed)
    {
        return GetRandomName(0);
    }
    return sFilterName;
}
// PC 0x007118C0  Xbox 0x0027C330
CExoString CSWNameGen::GetRandomName(unsigned short nRace,
                                     unsigned char nNameType,
                                     unsigned char nLength)
{
    CExoString sRace;
    CExoString sMaleTable;
    CExoString sFemaleTable;

    switch (nRace)
    {
    case 6:
        sRace = "human";
        break;
    case 0:
        sRace = "dwarf";
        break;
    case 1:
        sRace = "elf";
        break;
    case 2:
        sRace = "gnome";
        break;
    case 3:
        sRace = "halfling";
        break;
    case 4:
        if (rand() % 2)
        {
            sRace = "elf";
        }
        else
        {
            sRace = "human";
        }
        break;
    case 5:
        sRace = "halforc";
        break;
    case 0xFFFF:
        sRace = "animal";
        break;
    default:
        return CExoString("");
    }

    int bFullName = 0;
    if (nRace != 0xFFFF)
    {
        switch (nNameType)
        {
        case 1:
            sMaleTable = sRace + "m";
            break;
        case 2:
            sMaleTable = sRace + "f";
            break;
        case 4:
            sFemaleTable = sRace + "l";
            break;
        default:
            if ((nNameType & 1) && (nNameType & 2))
            {
                return CExoString("");
            }
            if (nNameType & 1)
            {
                sMaleTable = sRace + "m";
            }
            else if (nNameType & 2)
            {
                sMaleTable = sRace + "f";
            }
            if (nNameType & 4)
            {
                sFemaleTable = sRace + "l";
            }
            if (sMaleTable.GetLength() == 0 || sFemaleTable.GetLength() == 0)
            {
                return CExoString("");
            }
            bFullName = 1;
            break;
        }
        if (bFullName)
        {
            CExoString sFirstName;
            CExoString sLastName;
            UnloadNameTable();

            if (nLength > 0)
            {
                int nFirstLength = nLength / 3 + rand() % (nLength / 3);
                while (nFirstLength > 0)
                {
                    int nSplit = nFirstLength;
                    while (nSplit < 2 * nLength / 3)
                    {
                        if (!LoadNameTable(sMaleTable))
                        {
                            return CExoString("");
                        }
                        sFirstName = GetRandomName(nSplit);
                        UnloadNameTable();

                        if (!LoadNameTable(sFemaleTable))
                        {
                            return CExoString("");
                        }
                        sLastName = GetRandomName(nLength - nSplit);
                        UnloadNameTable();

                        if (sFirstName.GetLength() > 0 && sLastName.GetLength() > 0)
                        {
                            break;
                        }
                        ++nSplit;
                    }
                    if (sFirstName.GetLength() > 0 && sLastName.GetLength() > 0)
                    {
                        return sFirstName + " " + sLastName;
                    }
                    --nFirstLength;
                }
                return CExoString("");
            }
            else
            {
                if (!LoadNameTable(sMaleTable))
                {
                    return CExoString("");
                }
                sFirstName = GetRandomName(0);
                UnloadNameTable();
                if (!LoadNameTable(sFemaleTable))
                {
                    return CExoString("");
                }
                sLastName = GetRandomName(0);
                UnloadNameTable();
                if (sFirstName.GetLength() > 0 && sLastName.GetLength() > 0)
                {
                    return sFirstName + " " + sLastName;
                }
            }

            return CExoString("");
        }
    }

    if (m_bNameTableLoaded && m_nRace == nRace &&
        m_nGender == (nNameType & 0xFB))
    {
        return CExoString("");
    }
    UnloadNameTable();

    if (sMaleTable.GetLength() > 0)
    {
        if (!LoadNameTable(sMaleTable))
        {
            return CExoString("");
        }
    }
    else if (sFemaleTable.GetLength() <= 0 || !LoadNameTable(sFemaleTable))
    {
        return CExoString("");
    }

    m_nRace = nRace;
    m_nGender = nNameType & 0xFB;
    return GetRandomName(nLength);
}
