#ifndef AURCONSOLE_H
#define AURCONSOLE_H

#include <stdio.h>

#include "exobase.h"
#include <aurlist.h>
#include "aurora.h"

class OpenResource
{
public:
#ifdef _XBOX
    OpenResource(int nBufferSize);
#else
    OpenResource(int nBufferSize)  // Xbox 0x002A0A80
    {
        m_pFile = 0;
        m_nLines = -1;
        m_nBufferSize = nBufferSize;
        m_pBuffer = 0;
        m_pData = 0;
        m_nDataLeft = 0;
        m_pResource = 0;
    }
#endif

#ifdef _XBOX
    ~OpenResource();

#endif
    FILE* m_pFile;
    int m_nLines;
    char* m_pBuffer;
    char* m_pData;
    int m_nDataLeft;
    int m_nBufferSize;
    void* m_pResource;
};

extern char returnvalue[];
char* GetToken(char** psLine);

class funcholderbase
{
public:
    virtual ~funcholderbase();
    // PC 0x0060E760
    virtual void Engage(char* sArguments)
    {
    }
    // PC 0x0060E760
    virtual void PreEngage(char* sArguments)
    {
    }
};

template <class T> class funcholder : public funcholderbase
{
public:
#ifdef _XBOX
    funcholder(T pFunction);
#else
    funcholder(T pFunction)
    {
        m_pFunction = pFunction;
    }
#endif

    virtual void Engage(char* sArguments);
    // PC 0x0044C000  Xbox 0x002A1060
    virtual void PreEngage(char* sArguments)
    {
        returnvalue[0] = 0;
    }

    T m_pFunction;
};
#ifdef _XBOX

template <class T> funcholder<T>::funcholder(T pFunction)
{
    m_pFunction = pFunction;
}
#endif

#ifdef _XBOX
class Vector;
class Quaternion;
#endif

class ConsoleFunc
{
public:
    ConsoleFunc(const char* sName, void (*pFunction)());
    ConsoleFunc(const char* sName, void (*pFunction)(char*));
    ConsoleFunc(const char* sName, void (*pFunction)(int));
#ifdef _XBOX

    ConsoleFunc(const char* sName, int (*pFunction)(char*));
    ConsoleFunc(const char* sName, float (*pFunction)(char*));
    ConsoleFunc(const char* sName, char* (*pFunction)(char*));
    ConsoleFunc(const char* sName, int* pVariable);
    ConsoleFunc(const char* sName, float* pVariable);
    ConsoleFunc(const char* sName, Vector* pVariable);
    ConsoleFunc(const char* sName, Quaternion* pVariable);
#endif
    ~ConsoleFunc();

    char m_sName[80];
    funcholderbase* m_pHolder;
};

void ConsoleFuncs_Add(ConsoleFunc* pFunc);
void ConsoleFuncs_Remove(const char* sName);
funcholderbase* FindConCallBack(char* sName);
char* FuncPreInterp(char* sLine);
char* FuncInterp(char* sLine);
char* dofile(char* sFileName);
void* AurResGet(char* sName, char* sExtension, int* pnSize, bool bRaw);
void* AurResGet(FILE* pFile);
void AurResFree(void* pResource, int nSize);
char* GetDataFromFile(int nBytes);
char* AurResGetNextLine(void* pResource);
char* AurResGetNextLine();
void* AurResGetData(int nBytes);
char* CommandCompletion(char* sLine, List<char*>& lstCompletions);
char* AurResGetDataBytes(unsigned long nBytes, void* pResource);
void AurResFreeDataBytes(void* pResource, unsigned char* pData);
OpenResource* GetResource(void* pResource);
#ifdef _XBOX

int DeleteFuncWithPrefix(char* sPrefix);
extern char g_sObjectFuncName[256];
#define AUR_OBJECT_CONSOLE_VAR(sObject, sField, pVariable)              {                                                                       g_sObjectFuncName[0] = 0;                                           if ((sObject) && *(sObject))                                            strcpy(g_sObjectFuncName, (sObject));                           strcat(g_sObjectFuncName, ".");                                     strcat(g_sObjectFuncName, sField);                                  if ((sObject) && *(sObject))                                            new ConsoleFunc(g_sObjectFuncName, pVariable);              }
#endif

#ifdef _XBOX
void AurResRemove(void* pResource);
#else
int AurResRemove(void* pResource);
#endif

#endif
