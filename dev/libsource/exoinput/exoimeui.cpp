#include "exoimeui.h"

#if defined(_WIN32) && !defined(_XBOX)
#include <imm.h>
#include <stdio.h>
// PC 0x005DF740
void CExoImeUI::GetCompositionString(HWND hWnd, long nFlags)
{
    HIMC hContext = ImmGetContext(hWnd);
    if (hContext != 0)
    {
        DWORD nLength = ImmGetCompositionStringA(hContext, GCS_COMPSTR, 0, 0);
        if (nLength <= 0)
        {
            ImmReleaseContext(hWnd, hContext);
            return;
        }
        char* pText = new char[nLength + 1];
        ImmGetCompositionStringA(hContext, GCS_COMPSTR, pText, nLength);
        pText[nLength] = 0;
        m_sComposition = pText;
        delete[] pText;
        if (nFlags & GCS_COMPATTR)
        {
            DWORD nAttributes = ImmGetCompositionStringA(hContext, GCS_COMPATTR, 0, 0);
            if (nAttributes > 0)
            {
                char* pAttributes = new char[nAttributes + 1];
                ImmGetCompositionStringA(hContext, GCS_COMPATTR,
                                         pAttributes, nAttributes);
                pAttributes[nAttributes] = 0;
                delete[] pAttributes;
            }
        }
        ImmReleaseContext(hWnd, hContext);
    }
}

CExoString CExoImeUI::GetImeModeDescription(HWND hWnd)
{
    CExoString sMode;
    HIMC hContext = ImmGetContext(hWnd);
    if (hContext != 0)
    {
        int nLanguage = PRIMARYLANGID(GetSystemDefaultLangID());
        if (nLanguage == LANG_JAPANESE)
        {
            DWORD nConversion;
            DWORD nSentence;
            ImmGetConversionStatus(hContext, &nConversion, &nSentence);
            if ((nConversion & (IME_CMODE_NATIVE | IME_CMODE_KATAKANA)) ==
                (IME_CMODE_NATIVE | IME_CMODE_KATAKANA))
                return CExoString("\x91\x53\x8a\x70\x83\x4a\x83\x5e\x83\x4a\x83\x69");
            return CExoString("\x91\x53\x8a\x70\x82\xd0\x82\xe7\x82\xaa\x82\xc8");
        }
        if (nLanguage != LANG_CHINESE)
            return CExoString("");
        char sDescription[5] = "";
        ImmGetDescriptionA(GetKeyboardLayout(0), sDescription, 5);
        sDescription[4] = 0;
        sMode = sDescription[0] != 0 ? sDescription : "En";
        ImmReleaseContext(hWnd, hContext);
    }
    return sMode;
}
// PC 0x005DF800
void CExoImeUI::GetResultString(HWND hWnd, int bReadClauses)
{
    HIMC hContext = ImmGetContext(hWnd);
    if (hContext != 0)
    {
        DWORD nLength = ImmGetCompositionStringA(hContext, GCS_RESULTSTR, 0, 0);
        if (nLength <= 0)
        {
            m_sResult = "";
            ImmReleaseContext(hWnd, hContext);
            return;
        }
        char* pText = new char[nLength + 1];
        ImmGetCompositionStringA(hContext, GCS_RESULTSTR, pText, nLength);
        pText[nLength] = 0;
        m_sResult = pText;
        delete[] pText;
        if (bReadClauses)
        {
            DWORD nClauses = ImmGetCompositionStringA(hContext, GCS_RESULTCLAUSE, 0, 0);
            if (nClauses > 0)
            {
                char* pClauses = new char[nClauses + 1];
                ImmGetCompositionStringA(hContext, GCS_RESULTCLAUSE, pClauses, nClauses);
                pClauses[nClauses] = 0;
                {
                    CExoString sClauses(pClauses);
                    m_sResult = m_sResult + sClauses;
                }
                delete[] pClauses;
                m_sComposition = "";
                m_sCandidate = "";
            }
        }
        ImmReleaseContext(hWnd, hContext);
    }
}
// PC 0x005DF980
void CExoImeUI::UpdateOpenStatus(HWND hWnd)
{
    HIMC hContext = ImmGetContext(hWnd);
    if (hContext != 0)
    {
        if (!ImmGetOpenStatus(hContext))
        {
            m_sCandidate = "";
            m_nFlags = 0;
        }
        ImmReleaseContext(hWnd, hContext);
    }
}
// PC 0x005DF9D0
CExoString CExoImeUI::FormatCandidateList(CANDIDATELIST* pList)
{
    int nNumber = 1;
    CExoString sCandidates;
    unsigned long nPageSize = pList->dwPageSize;
    if (nPageSize == 0)
        nPageSize = DEFAULT_CANDIDATE_PAGE_SIZE;
    unsigned long nEnd = pList->dwPageStart + nPageSize;
    if (nEnd >= pList->dwCount)
        nEnd = pList->dwCount;
    char sItem[CANDIDATE_TEXT_BUFFER_SIZE];
    for (unsigned long i = pList->dwPageStart; i < nEnd; ++i)
    {
        char* pCandidate = (char*)pList + pList->dwOffset[i];
        sItem[lstrlenA(pCandidate)] = 0;
        sprintf(sItem, "%d:%s  ", nNumber++, pCandidate);
        CExoString sCandidate(sItem);
        sCandidates = sCandidates + sCandidate;
    }
    return sCandidates;
}
// PC 0x005DFB00
void CExoImeUI::HandleImeKeyDown(HWND hWnd, unsigned int, long nParam)
{
    if (nParam & GCS_RESULTSTR)
    {
        if (nParam & GCS_RESULTCLAUSE)
            GetResultString(hWnd, 1);
        else
            GetResultString(hWnd, 0);
    }
    else if (nParam & GCS_COMPSTR)
        GetCompositionString(hWnd, nParam);
}
// PC 0x005DFB40
void CExoImeUI::UpdateCandidateList(HWND hWnd, unsigned long nLists)
{
    m_nFlags &= ~CANDIDATE_LIST_OPEN;
    m_sCandidate = "";
    HIMC hContext = ImmGetContext(hWnd);
    if (hContext == 0)
        return;
    unsigned long nList;
    for (nList = 0; nList < MAX_CANDIDATE_LISTS; ++nList)
        if ((1UL << nList) & nLists)
            break;
    if (nList == MAX_CANDIDATE_LISTS)
    {
        ImmReleaseContext(hWnd, hContext);
        return;
    }
    DWORD nSize = ImmGetCandidateListA(hContext, nList, 0, 0);
    if (nSize == 0)
    {
        ImmReleaseContext(hWnd, hContext);
        return;
    }
    CANDIDATELIST* pList = (CANDIDATELIST*)new unsigned char[nSize];
    ImmGetCandidateListA(hContext, nList, pList, nSize);
    m_sCandidate = FormatCandidateList(pList);
    delete[] (unsigned char*)pList;
    m_nFlags |= CANDIDATE_LIST_OPEN;
    ImmReleaseContext(hWnd, hContext);
}
// PC 0x005DFC40
int CExoImeUI::HandleImeNotify(HWND hWnd, unsigned int nMsg, long nParam)
{
    switch (nMsg)
    {
    case IMN_CHANGECANDIDATE:
    case IMN_OPENCANDIDATE:
        UpdateCandidateList(hWnd, nParam);
        return 1;
    case IMN_CLOSECANDIDATE:
        m_sCandidate = "";
        m_nFlags &= ~CANDIDATE_LIST_OPEN;
        return 1;
    case IMN_SETOPENSTATUS:
        UpdateOpenStatus(hWnd);
        return 1;
    default:
        return 0;
    }
}

void CExoImeUI::TurnImeOff(HWND hWnd)
{
    HIMC hContext = ImmGetContext(hWnd);
    if (hContext != 0)
    {
        DWORD nConversion;
        DWORD nSentence;
        ImmGetConversionStatus(hContext, &nConversion, &nSentence);
        nConversion &= ~IME_CMODE_NATIVE;
        ImmSetConversionStatus(hContext, nConversion, nSentence);
        int nLanguage = PRIMARYLANGID(GetSystemDefaultLangID());
        if (nLanguage == LANG_JAPANESE || nLanguage == LANG_CHINESE)
        {
            if (nLanguage == LANG_JAPANESE)
                ImmSetOpenStatus(hContext, FALSE);
            if (m_nUnknown104 != 0)
                ActivateKeyboardLayout((HKL)m_nUnknown104, KLF_REORDER);
        }
        ImmReleaseContext(hWnd, hContext);
        m_nCompositionActive = 0;
        m_sCandidate = "";
        m_sComposition = "";
        m_sResult = "";
    }
}

void CExoImeUI::TurnImeOn(HWND hWnd)
{
    m_nUnknown10C = 0;
    HIMC hContext = ImmGetContext(hWnd);
    if (hContext != 0)
    {
        DWORD nConversion;
        DWORD nSentence;
        ImmGetConversionStatus(hContext, &nConversion, &nSentence);
        int nLanguage = PRIMARYLANGID(GetSystemDefaultLangID());
        nConversion |= IME_CMODE_NATIVE;
        if (nLanguage == LANG_CHINESE)
        {
            if (m_nUnknown104 == 0)
                m_nUnknown104 = (int)ActivateKeyboardLayout((HKL)HKL_NEXT, 0);
            else
                ActivateKeyboardLayout((HKL)HKL_NEXT, 0);
            if ((int)GetKeyboardLayout(0) == m_nUnknown104)
                ActivateKeyboardLayout((HKL)HKL_NEXT, 0);
        }
        else if (nLanguage == LANG_JAPANESE)
        {
            if (m_nUnknown104 == 0)
                m_nUnknown104 = (int)ActivateKeyboardLayout((HKL)HKL_NEXT, KLF_REORDER);
            else
                ActivateKeyboardLayout((HKL)HKL_NEXT, KLF_REORDER);
            ImmSetOpenStatus(hContext, TRUE);
            if (m_nUnknown10C == 0)
                nConversion |= IME_CMODE_KATAKANA | IME_CMODE_FULLSHAPE;
            else
                nConversion = (nConversion & ~IME_CMODE_KATAKANA) | IME_CMODE_FULLSHAPE;
            m_nUnknown10C = m_nUnknown10C == 0;
        }
        ImmSetConversionStatus(hContext, nConversion, nSentence);
        m_nCompositionActive = 1;
        ImmReleaseContext(hWnd, hContext);
        m_sCandidate = "";
        m_sComposition = "";
        m_sResult = "";
    }
}
#endif
