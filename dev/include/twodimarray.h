





#ifndef _2DA_H
#define _2DA_H

class CRes;
#include "exobase.h"
#include "exoresman.h"



class CRes2DA;




























class C2DA : public CResHelper<CRes2DA, RESTYPE_2DA>
{
public:
    C2DA(CResRef cResRef, int bRequest);
    virtual ~C2DA();

    int GetColumnIndex(const CExoString& sColumnLabel);
    int GetRowIndex(const CExoString& sRowLabel);
    int GetCExoStringEntry(int nRow, int nColumn, CExoString* psEntry);
    int GetCExoStringEntry(int nRow, const CExoString& sColumn,
                           CExoString* psEntry);
    int GetCExoStringEntry(const CExoString& sRow, const CExoString& sColumn,
                           CExoString* psEntry);
    int GetCExoStringEntry(const CExoString& sRow, int nColumn,
                           CExoString* psEntry);
    int GetFLOATEntry(int nRow, int nColumn, float* pfEntry);
    int GetFLOATEntry(int nRow, const CExoString& sColumn, float* pfEntry);
    int GetFLOATEntry(const CExoString& sRow, const CExoString& sColumn,
                      float* pfEntry);
    int GetFLOATEntry(const CExoString& sRow, int nColumn, float* pfEntry);
    int GetINTEntry(int nRow, int nColumn, int* pnEntry);
    int GetINTEntry(int nRow, const CExoString& sColumn, int* pnEntry);
    int GetINTEntry(const CExoString& sRow, const CExoString& sColumn,
                    int* pnEntry);
    int GetINTEntry(const CExoString& sRow, int nColumn, int* pnEntry);

    unsigned long GetNextLineLength(char* pData, unsigned long nSize);
    int GetNextToken(char** ppData, unsigned long* pnSize, char* szToken,
                     unsigned long* pnSkipped);
    int SkipNewLines(char** ppData, unsigned long* pnSize);
    int Load2DArray();
    void Unload2DArray();

    int SaveBinary2da(char* sFileName);

    int SetINTEntry(int nRow, int nColumn, int nValue);
    int SetINTEntry(int nRow, const CExoString& sColumn, int nValue);
    int SetINTEntry(const CExoString& sRow, const CExoString& sColumn, int nValue);
    int SetINTEntry(const CExoString& sRow, int nColumn, int nValue);
    int SetFLOATEntry(int nRow, int nColumn, float fValue);
    int SetFLOATEntry(int nRow, const CExoString& sColumn, float fValue);
    int SetFLOATEntry(const CExoString& sRow, const CExoString& sColumn, float fValue);
    int SetFLOATEntry(const CExoString& sRow, int nColumn, float fValue);


    int GetNumRows() { return m_nNumRows; }

    CExoString m_sDefault;
    int m_nNumRows;
    int m_nNumColumns;
    int m_bLoaded;
    CExoString* m_pRowLabels;
    CExoString* m_pColumnLabels;
    CExoString** m_ppEntries;
    int m_bPacked;
    char* m_pEntryData;
    unsigned short* m_pEntryOffsets;
    char* m_pLabelData;
    unsigned short* m_pRowLabelOffsets;
    unsigned short* m_pColumnLabelOffsets;


    int GetFLOATEntry(int nRow, CExoString* psColumn, float* pfValue);
};

#endif
