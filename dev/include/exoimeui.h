#ifndef __EXOIMEUI_H__
#define __EXOIMEUI_H__

#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif
#include "exobase.h"

class CExoImeUI
{
public:
    enum { COMPOSITION_IN_PROGRESS = 1, CANDIDATE_LIST_OPEN = 2 };
    unsigned long m_nFlags;
    char m_aUnmapped0[256];
    int m_nUnknown104;
    int m_nCompositionActive;

    int m_nUnknown10C;

    CExoString m_sResult;
    CExoString m_sComposition;
    CExoString m_sCandidate;

    CExoImeUI();

    void HandleImeKeyDown(HWND hWnd, unsigned int nMsg, long nParam);
    int HandleImeNotify(HWND hWnd, unsigned int nMsg, long nParam);

    void TurnImeOff(HWND hWnd);
    void TurnImeOn(HWND hWnd);
    CExoString GetImeModeDescription(HWND hWnd);

#if defined(_WIN32) && !defined(_XBOX)
private:

    enum { MAX_CANDIDATE_LISTS = 32, DEFAULT_CANDIDATE_PAGE_SIZE = 5,
           CANDIDATE_TEXT_BUFFER_SIZE = 50 };
    void GetCompositionString(HWND hWnd, long nFlags);
    void GetResultString(HWND hWnd, int bReadClauses);
    void UpdateOpenStatus(HWND hWnd);
    CExoString FormatCandidateList(struct tagCANDIDATELIST* pList);
    void UpdateCandidateList(HWND hWnd, unsigned long nLists);
#endif
};

#endif
