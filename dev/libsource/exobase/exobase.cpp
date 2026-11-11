#include "exobase.h"
#include "swgui.h"
// PC 0x005E6500  Xbox 0x00134750
CExoBase::CExoBase()
{
    m_pExoDebug = new CExoDebug;
    m_pExoTimers = new CExoTimers;
    m_pExoAliasList = new CExoAliasList;
    m_pExoIni = new CExoIni;
    m_pExoRand = new CExoRand;
    m_pInternal = new CExoBaseInternal;
    EXOXBOXASSERT(m_pInternal);
}
// PC 0x005E6640  Xbox 0x00134890
void CExoBase::GetDirectoryList(CExoArrayList<CExoString>* pList,
                                const CExoString& sPath,
                                unsigned short nResType, int nArg4, int nArg5)
{
    m_pInternal->GetDirectoryList(pList, sPath, nResType, nArg4, nArg5);
}
// PC 0x005E6650
int CExoBase::GetDirectorySize(const CExoString& sPath)
{
    return m_pInternal->GetDirectorySize(sPath);
}
// PC 0x005E6660  Xbox 0x001348A0
const CExoString& CExoBase::GetResourceExtension(unsigned short resourceType)
{
    return m_pInternal->GetResourceExtension(resourceType);
}
// PC 0x005E6670  Xbox 0x001348B0
unsigned short CExoBase::GetResTypeFromExtension(const CExoString& extension)
{
    return m_pInternal->GetResTypeFromExtension(extension);
}
// PC 0x005E6680  Xbox 0x001348C0
void CExoBase::LoadAliases(const CExoString& sFile)
{
    m_pInternal->LoadAliases(sFile);
}

#ifndef _XBOX
void CExoBase::SpawnExternalApplication(const CExoString& sApplication,
                                        const CExoString& sCommandLine,
                                        int bQuit)
{
    m_pInternal->SpawnExternalApplication(sApplication, sCommandLine, bQuit);
}
#endif
// PC 0x005E6690  Xbox 0x001348D0
int CExoBase::WcsToMbs(char* sDest, const wchar_t* sSource,
                       unsigned long nLength)
{
    return m_pInternal->WcsToMbs(sDest, sSource, nLength);
}
// PC 0x005E66A0  Xbox 0x001348E0
CExoBase::~CExoBase()
{
    if (m_pInternal != NULL)
    {
        delete m_pInternal;
        m_pInternal = NULL;
    }
    if (m_pExoIni != NULL)
    {
        delete m_pExoIni;
        m_pExoIni = NULL;
    }
    if (m_pExoAliasList != NULL)
    {
        delete m_pExoAliasList;
        m_pExoAliasList = NULL;
    }
    if (m_pExoTimers != NULL)
    {
        delete m_pExoTimers;
        m_pExoTimers = NULL;
    }
    if (m_pExoDebug != NULL)
    {
        delete m_pExoDebug;
        m_pExoDebug = NULL;
    }
    if (m_pExoRand != NULL)
    {
        delete m_pExoRand;
        m_pExoRand = NULL;
    }
}
