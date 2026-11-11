#include "exoiniinternal.h"
#include "exobase.h"
#include <stdio.h>
#include <string.h>
// PC 0x00635F30
CExoIniInternal::CExoIniInternal()
{
    m_sFile = "";
}
// PC 0x00635F90
CFileSection::~CFileSection()
{
}
// PC 0x00635FF0
CFileSection::CFileSection()
{
}
// PC 0x00636040
void CExoIniInternal::Destroy()
{
    while (m_lstSections.GetHeadPos())
    {
        CFileSection* pSection =
            (CFileSection*)m_lstSections.RemoveHead();

        while (pSection->m_lstEntries.GetHeadPos())
        {
            CFileKey* pKey =
                (CFileKey*)pSection->m_lstEntries.RemoveHead();
            delete pKey;
        }

        delete pSection;
    }

    m_sFile = "";
}
// PC 0x006360B0
int CExoIniInternal::ReadIniEntry(CExoString& sValue, const CExoString& sFile,
                                  const CExoString& sSection,
                                  const CExoString& sEntry)
{
    char sLine[512];
    char szFile[96];
    char szSection[96];
    char szEntry[96];

    sprintf(szFile, "%s", sFile.CStr());
    sprintf(szSection, "%s", sSection.CStr());
    sprintf(szEntry, "%s", sEntry.CStr());

    if (!m_sFile.CompareNoCase(szFile))
    {
        Destroy();
    }

    if (m_lstSections.GetHead() == NULL)
    {
        m_sFile = sFile;

        FILE* pFile = fopen(szFile, "r");

        if (pFile == NULL)
        {
            return 0;
        }

        while (fgets(sLine, 512, pFile) != NULL)
        {
            CFileSection* pSection = new CFileSection;

            pSection->m_sName = sLine;
            m_lstSections.AddTail(pSection);

            if (sLine[0] == '[')
            {
                while (fgets(sLine, 512, pFile) != NULL)
                {
                    if (sLine[0] == '[')
                    {
                        fseek(pFile, -1 - (int)strlen(sLine), SEEK_CUR);
                        break;
                    }

                    CFileKey* pKey = new CFileKey;

                    pKey->m_sLine = sLine;
                    pSection->m_lstEntries.AddTail(pKey);
                }
            }
        }

        fclose(pFile);
    }

    CFileSection* pSection = m_lstSections.GetHead();
    CExoLinkedListNode* pSectionPos = m_lstSections.GetHeadPos();

    while (pSection != NULL)
    {
        strcpy(sLine, pSection->m_sName.CStr());

        char* pEnd = strchr(sLine, ']');

        if (pEnd != NULL)
        {
            *pEnd = 0;
        }

        char* pStart = strchr(sLine, '[');

        if (pStart == NULL)
        {
            pStart = sLine;
        }

        ++pStart;

        if (strcmp(pStart, szSection) == 0)
        {
            CFileKey* pKey = pSection->m_lstEntries.GetHead();
            CExoLinkedListNode* pEntryPos = pSection->m_lstEntries.GetHeadPos();

            while (pKey != NULL)
            {
                strcpy(sLine, pKey->m_sLine.CStr());

                char* pEqual = strchr(sLine, '=');

                if (pEqual != NULL)
                {
                    *pEqual = 0;
                }

                if (strcmp(sLine, szEntry) == 0)
                {
                    sValue = strchr(pKey->m_sLine.CStr(), '=') + 1;

                    if (sValue.Right(1) == "\n")
                    {
                        sValue = sValue.Left(sValue.GetLength() - 1);
                    }

                    return 1;
                }

                pKey = pSection->m_lstEntries.GetNext(pEntryPos);
            }

            return 0;
        }

        pSection = m_lstSections.GetNext(pSectionPos);
    }

    return 0;
}
// PC 0x006364F0
int CExoIniInternal::WriteIniEntry(const CExoString& sValue,
                                   const CExoString& sFile,
                                   const CExoString& sSection,
                                   const CExoString& sEntry)
{
    char szFile[96];
    char szEntry[96];
    char sLine[512];
    char szSection[96];
    char szValue[512];

    sprintf(szFile, "%s", sFile.CStr());
    sprintf(szSection, "%s", sSection.CStr());
    sprintf(szEntry, "%s", sEntry.CStr());
    sprintf(szValue, "%s", sValue.CStr());

    if (!m_sFile.CompareNoCase(szFile))
    {
        Destroy();
    }

    if (m_lstSections.GetHead() == NULL)
    {
        m_sFile = sFile;

        FILE* pFile = fopen(szFile, "r");

        if (pFile == NULL)
        {
            pFile = fopen(szFile, "w");

            if (pFile == NULL)
            {
                return 0;
            }
        }

        while (fgets(sLine, 512, pFile) != NULL)
        {
            CFileSection* pSection = new CFileSection;

            pSection->m_sName = sLine;
            m_lstSections.AddTail(pSection);

            if (sLine[0] == '[')
            {
                while (fgets(sLine, 512, pFile) != NULL)
                {
                    if (sLine[0] == '[')
                    {
                        fseek(pFile, -1 - (int)strlen(sLine), SEEK_CUR);
                        break;
                    }

                    CFileKey* pKey = new CFileKey;

                    pKey->m_sLine = sLine;
                    pSection->m_lstEntries.AddTail(pKey);
                }
            }
        }

        fclose(pFile);
    }

    CFileSection* pSection = m_lstSections.GetHead();
    CExoLinkedListNode* pPos = m_lstSections.GetHeadPos();
    CExoLinkedListNode* pEntryPos;

    while (pSection != NULL)
    {
        strcpy(sLine, pSection->m_sName.CStr());

        char* pEnd = strchr(sLine, ']');

        if (pEnd != NULL)
        {
            *pEnd = 0;
        }

        char* pStart = strchr(sLine, '[');

        if (pStart == NULL)
        {
            pStart = sLine;
        }

        ++pStart;

        if (strcmp(pStart, szSection) == 0)
        {
            CFileKey* pKey = pSection->m_lstEntries.GetHead();
            char* pEqual = NULL;
            int nCompare = 1;

            pEntryPos = pSection->m_lstEntries.GetHeadPos();

            if (pKey != NULL)
            {
                for (;;)
                {
                    strcpy(sLine, pKey->m_sLine.CStr());

                    pEqual = strchr(sLine, '=');

                    if (pEqual != NULL)
                    {
                        *pEqual = 0;
                    }

                    nCompare = strcmp(sLine, szEntry);

                    if (nCompare == 0)
                    {
                        break;
                    }

                    pKey = pSection->m_lstEntries.GetNext(pEntryPos);

                    if (pKey == NULL)
                    {
                        break;
                    }
                }
            }

            if (nCompare == 0)
            {
                if (pEqual != NULL)
                {
                    strcpy(pEqual + 1, szValue);
                    *pEqual = '=';
                }
                else
                {
                    strcat(sLine, "=");
                    strcat(sLine, szValue);
                }

                pKey->m_sLine = sLine;
                pKey->m_sLine = pKey->m_sLine + "\n";
            }
            else
            {
                pKey = new CFileKey;

                pKey->m_sLine = pKey->m_sLine + szEntry;
                pKey->m_sLine = pKey->m_sLine + "=";
                pKey->m_sLine = pKey->m_sLine + szValue;
                pKey->m_sLine = pKey->m_sLine + "\n";
                pSection->m_lstEntries.AddHead(pKey);
            }
            break;
        }

        pSection = m_lstSections.GetNext(pPos);

    }

    if (pSection == NULL)
    {
        pSection = new CFileSection;

        pSection->m_sName = "\n[";
        pSection->m_sName = pSection->m_sName + szSection;
        pSection->m_sName = pSection->m_sName + "]\n";

        CFileKey* pKey = new CFileKey;

        pKey->m_sLine = szEntry;
        pKey->m_sLine = pKey->m_sLine + "=";
        pKey->m_sLine = pKey->m_sLine + szValue;
        pKey->m_sLine = pKey->m_sLine + "\n";
        pSection->m_lstEntries.AddHead(pKey);
        m_lstSections.AddTail(pSection);
    }

    FILE* pFile = fopen(szFile, "w");

    if (pFile == NULL)
    {
        CExoString sError;

        sError.Format("ERROR: %s file is marked read only! Are you sure that "
                      "you are running from the install location and not "
                      "from the network?",
                      szFile);
        sError.CStr();
        return 0;
    }

    pSection = m_lstSections.GetHead();
    pPos = m_lstSections.GetHeadPos();

    for (; pSection != NULL; pSection = m_lstSections.GetNext(pPos))
    {
        fprintf(pFile, "%s", pSection->m_sName.CStr());

        CFileKey* pKey = pSection->m_lstEntries.GetHead();
        pEntryPos = pSection->m_lstEntries.GetHeadPos();

        for (; pKey != NULL; pKey = pSection->m_lstEntries.GetNext(pEntryPos))
        {
            fprintf(pFile, "%s", pKey->m_sLine.CStr());
        }
    }

    fclose(pFile);

    return 1;
}
// PC 0x00636EF0
CExoIniInternal::~CExoIniInternal()
{
    Destroy();
}
