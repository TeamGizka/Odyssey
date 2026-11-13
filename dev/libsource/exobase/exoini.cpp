#include "exobase.h"
#include "exoiniinternal.h"
#ifndef _XBOX
CExoIni::CExoIni()  // PC 0x00632080 GOG 0x00631F70
{
    m_pInternal = new CExoIniInternal();
}
// PC 0x006320F0 GOG 0x00631FE0
int CExoIni::ReadIniEntry(CExoString& sValue, const CExoString& sSection,
                          const CExoString& sKey, const CExoString& sDefault)
{
    return m_pInternal->ReadIniEntry(sValue, sSection, sKey, sDefault);
}
// PC 0x00632100 GOG 0x00631FF0
int CExoIni::WriteIniEntry(const CExoString& sSection, const CExoString& sKey,
                           const CExoString& sValue, const CExoString& sDefault)
{
    return m_pInternal->WriteIniEntry(sSection, sKey, sValue, sDefault);
}
// PC 0x00632110 GOG 0x00632000
CExoIni::~CExoIni()
{
    delete m_pInternal;
    m_pInternal = 0;
}
#else

CExoIni::CExoIni()
{
}

int CExoIni::ReadIniEntry(CExoString& sValue, const CExoString& sSection,
                          const CExoString& sKey, const CExoString& sDefault)
{
    return 0;
}

int CExoIni::WriteIniEntry(const CExoString& sSection, const CExoString& sKey,
                           const CExoString& sValue, const CExoString& sDefault)
{
    return 0;
}

CExoIni::~CExoIni()
{
}
#endif
