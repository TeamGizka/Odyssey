#ifndef AURPARSE_H
#define AURPARSE_H

#include <stdio.h>
#include <string.h>
#include <aurlist.h>

class Vector;
int Parse(char* sText, Vector& vValue);
void Parse(char* sText, char* sValue);
char* skipspace(char* sText);
char* AurResGetNextLine();
void* AurResGetData(int nBytes);
int Parse(char* sText, float& fValue);

template <class T> void Parse(char* sText, List<T>& lstValues)
{
    int nCount = 0;
    char cFormat;
    int nFields = sscanf(sText, "%d %c", &nCount, &cFormat);
    if (nFields > 0)
    {
        lstValues.SetSize(nCount);
        if (nFields == 1)
        {
            for (int i = 0; i < lstValues.m_nCount; i++)
            {
                char* sLine = AurResGetNextLine();
                if (sLine == 0)
                    break;
                Parse(sLine, lstValues[i]);
            }
        }
        else
        {
            int nBytes = nCount * sizeof(T);
            memcpy(lstValues.m_pList, AurResGetData(nBytes), nBytes);
            *sText = 0;
        }
    }
    else
    {
        char* sLine = AurResGetNextLine();
        while (sLine != 0)
        {
            if (strncmp(skipspace(sLine), "endlist", 7) == 0)
                break;
            if (lstValues.m_nCount == 0)
                lstValues.SetSize(1);
            else if (lstValues.m_nCount == nCount)
                lstValues.Add(lstValues[nCount - 1]);
            Parse(sLine, lstValues[nCount]);
            nCount++;
            sLine = AurResGetNextLine();
        }
        skipspace(skipspace(sLine) + 7);
    }
}

#endif
