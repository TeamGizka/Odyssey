#include "exobase.h"

#include <stdlib.h>
#include <string.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif
// PC 0x00634D50 GOG 0x00634C40 Xbox 0x0014A120
void CExoStringList::Introduce(CExoString* pString, int nPosition)
{
    int nNewSize = m_nAllocated;
    int i;

    if (m_nCount >= nNewSize)
    {
        nNewSize += 5;
    }
    if (nNewSize > m_nAllocated)
    {
        CExoString** pNewStrings =
            (CExoString**)calloc(nNewSize, sizeof(CExoString*));
        for (i = 0; i < m_nAllocated; i++)
        {
            pNewStrings[i] = m_pStrings[i];
        }
        free(m_pStrings);
        m_pStrings = pNewStrings;
    }
    for (i = m_nCount; i > nPosition; i--)
    {
        m_pStrings[i] = m_pStrings[i - 1];
    }
    m_pStrings[nPosition] = pString;
    m_nAllocated = nNewSize;
    m_nCount++;
}
// PC 0x00634DE0 GOG 0x00634CD0 Xbox 0x0014A1B0
CExoStringList::CExoStringList()
{
    m_pStrings = 0;
    m_nCount = 0;
    m_nDuplicateMode = 0;
    m_nSortMode = 0;
    m_nAllocated = 0;
}
// PC 0x00634E00 GOG 0x00634CF0 Xbox 0x0014A1D0
CExoStringList::CExoStringList(int nSortMode, int nDuplicateMode, int nSize)
{
    m_pStrings = (CExoString**)calloc(nSize, sizeof(CExoString*));
    m_nCount = 0;
    m_nDuplicateMode = nDuplicateMode;
    m_nSortMode = nSortMode;
    m_nAllocated = nSize;
}
// PC 0x00635110 GOG 0x00635000 Xbox 0x0014A4A0
CExoStringList::~CExoStringList()
{
    if (m_pStrings)
    {
        for (int i = 0; i < m_nCount; i++)
        {
            if (m_pStrings[i])
            {
                delete m_pStrings[i];
            }
        }
        free(m_pStrings);
    }
}
// PC 0x00634E40 GOG 0x00634D30 Xbox 0x0014A210
void CExoStringList::Clear()
{
    for (int i = 0; i < m_nAllocated; i++)
    {
        if (m_pStrings[i])
        {
            delete m_pStrings[i];
            m_pStrings[i] = 0;
        }
    }
    m_nCount = 0;
}
// PC 0x00634E90 GOG 0x00634D80 Xbox 0x0014A270
CExoString* CExoStringList::GetString(int nIndex)
{
    return m_pStrings[nIndex];
}
// PC 0x00634E90 GOG 0x00634D80 Xbox 0x0014A270
CExoString* CExoStringList::operator[](int nIndex)
{
    return m_pStrings[nIndex];
}

// Dead code: not in the PC executable
void CExoStringList::Insert(CExoString* pString, int nPosition)
{
    if (m_nSortMode == 0)
    {
#ifndef _XBOX
        MessageBox(NULL,
                   "Cannot call Insert directly on a sorted list.  Use Add instead.",
                   "Error: ExoStringList", MB_OKCANCEL);
#endif
        return;
    }
    Introduce(pString, nPosition);
}
// PC 0x00635160 GOG 0x00635050 Xbox 0x0014A4F0
void CExoStringList::Add(CExoString* pString)
{
    int nCount = m_nCount;
    BOOL bAdd = TRUE;

    if (m_nSortMode == 0)
    {
        AddSorted(pString);
        return;
    }

    if (m_nDuplicateMode == 1)
    {
        for (int i = 0; i < m_nCount; i++)
        {
            if (strcmp(pString->CStr(), m_pStrings[i]->CStr()) == 0)
            {
                i = m_nCount;
                bAdd = FALSE;
            }
        }
    }
    else if (m_nDuplicateMode == 2)
    {
#ifndef _XBOX
        MessageBox(NULL, "Duplicate string found.", "Error: ExoStringList",
                   MB_OKCANCEL);
#endif
        bAdd = FALSE;
    }

    if (bAdd)
    {
        Introduce(pString, nCount);
    }
}

// Dead code: not in the PC executable
void CExoStringList::Delete(CExoString* pString)
{
    int nCount = m_nCount;
    CExoString** pNewStrings =
        (CExoString**)calloc(m_nAllocated, sizeof(CExoString*));
    int i;

    if (pNewStrings)
    {
        for (i = 0; i < m_nCount; i++)
        {
            if (m_pStrings[i])
            {
                if (strcmp(pString->CStr(), m_pStrings[i]->CStr()) == 0)
                {
                    delete m_pStrings[i];
                    m_pStrings[i] = 0;
                    nCount--;
                }
            }
        }

        int j = 0;
        for (i = 0; i < m_nAllocated; i++)
        {
            if (m_pStrings[i])
            {
                pNewStrings[j] = m_pStrings[i];
                j++;
            }
        }
    }

    m_nCount = nCount;
    free(m_pStrings);
    m_pStrings = pNewStrings;
}
// PC 0x00634EA0 GOG 0x00634D90 Xbox 0x0014A280
void CExoStringList::AddSorted(CExoString* pString)
{
    int bDone = 0;
    int nHigh = 0;
    int nLow = 0;
    int nCompare = 0;
    int nMid;
    int nDirection;

    if (m_nCount > 0)
    {
        nCompare = strcmp(pString->CStr(), m_pStrings[0]->CStr());
        if (nCompare < 0)
        {
            Introduce(pString, 0);
            bDone = 1;
        }
        else if (nCompare == 0)
        {
            if (m_nDuplicateMode == 0)
            {
                Introduce(pString, 0);
            }
            else if (m_nDuplicateMode == 2)
            {
#ifndef _XBOX
                MessageBox(NULL, "Duplicate string detected.",
                           "Error: ExoStringList", MB_OKCANCEL);
#endif
            }
            bDone = 1;
        }
    }
    if (!bDone && m_nCount > 0)
    {
        nHigh = m_nCount - 1;
        nCompare = strcmp(pString->CStr(), m_pStrings[m_nCount - 1]->CStr());
        if (nCompare > 0)
        {
            Introduce(pString, m_nCount);
            bDone = 1;
        }
        else if (nCompare == 0)
        {
            if (m_nDuplicateMode == 0)
            {
                Introduce(pString, m_nCount);
            }
            else if (m_nDuplicateMode == 2)
            {
#ifndef _XBOX
                MessageBox(NULL, "Duplicate string detected.",
                           "Error: ExoStringList", MB_OKCANCEL);
#endif
            }
            bDone = 1;
        }
    }

    nDirection = m_nCount / 2;
    nMid = nDirection;
    while (!bDone && nHigh - nLow > 1)
    {
        nCompare = strcmp(pString->CStr(), m_pStrings[nMid]->CStr());
        if (nCompare < 0)
        {
            nHigh = nMid;
            nDirection = -1;
        }
        else if (nCompare > 0)
        {
            nLow = nMid;
            nDirection = 1;
        }
        else
        {
            if (m_nDuplicateMode == 0)
            {
                Introduce(pString, nMid);
            }
            else if (m_nDuplicateMode == 2)
            {
#ifndef _XBOX
                MessageBox(NULL, "Duplicate string detected.",
                           "Error: ExoStringList", MB_OKCANCEL);
#endif
            }
            bDone = 1;
        }
        nDirection *= (nHigh - nLow) / 2;
        nMid += nDirection;
    }
    if (!bDone)
    {
        if (nCompare <= 0)
        {
            Introduce(pString, nMid);
        }
        else
        {
            Introduce(pString, nMid + 1);
        }
    }
}
// PC 0x00635230 GOG 0x00635120 Xbox 0x0014A5C0
CExoStringList* CExoStringList::Merge(CExoStringList* pList, int nSortMode,
                                      int nDuplicateMode)
{
    CExoStringList* pNewList =
        new CExoStringList(nSortMode, nDuplicateMode, m_nAllocated);
    int i;

    for (i = 0; i < m_nCount; i++)
    {
        pNewList->Add(new CExoString(m_pStrings[i]->CStr()));
    }

    if (pList != NULL)
    {
        for (i = 0; i < pList->m_nCount; i++)
        {
            pNewList->Add(new CExoString(pList->m_pStrings[i]->CStr()));
        }
    }

    return pNewList;
}

// Dead code: not in the PC executable
void CExoStringList::Remove(int nIndex)
{
    if (m_pStrings[nIndex])
    {
        delete m_pStrings[nIndex];
    }

    for (int i = nIndex; i < m_nCount - 1; i++)
    {
        m_pStrings[i] = m_pStrings[i + 1];
    }

    m_nCount--;
}

// Dead code: not in the PC executable
CExoStringList& CExoStringList::operator=(CExoStringList& list)
{
    int i;

    if (this != NULL)
    {
        for (i = 0; i < m_nCount; i++)
        {
            if (m_pStrings[i])
            {
                delete m_pStrings[i];
            }
        }
        free(m_pStrings);

        m_nCount = list.m_nCount;
        m_nAllocated = list.m_nAllocated;
        m_nSortMode = list.m_nSortMode;
        m_nDuplicateMode = list.m_nDuplicateMode;

        m_pStrings = (CExoString**)calloc(m_nAllocated, sizeof(CExoString*));
        for (i = 0; i < m_nCount; i++)
        {
            m_pStrings[i] = new CExoString(list.m_pStrings[i]->CStr());
        }
    }

    return *this;
}

// PC 0x006CB4A0 GOG 0x006CB1A0 Xbox 0x0014A260
int CExoStringList::GetCount()
{
    return m_nCount;
}

// Dead code: not in the PC executable
int CExoStringList::GetDuplicate()
{
    return m_nDuplicateMode;
}

// Dead code: not in the PC executable
int CExoStringList::GetSorted()
{
    return m_nSortMode;
}

// Dead code: not in the PC executable
int CExoStringList::GetSize()
{
    return m_nAllocated;
}
