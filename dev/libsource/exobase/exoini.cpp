#include "exobase.h"
#include "exoiniinternal.h"
#ifndef _XBOX
CExoIni::CExoIni()  // PC 0x005E6750
{
    m_pInternal = new CExoIniInternal();
}
// PC 0x005E67C0  Xbox 0x000C7740
int CExoIni::ReadIniEntry(CExoString& sValue, const CExoString& sSection,
                          const CExoString& sKey, const CExoString& sDefault)
{
    return m_pInternal->ReadIniEntry(sValue, sSection, sKey, sDefault);
}
// PC 0x005E67D0
int CExoIni::WriteIniEntry(const CExoString& sSection, const CExoString& sKey,
                           const CExoString& sValue, const CExoString& sDefault)
{
    return m_pInternal->WriteIniEntry(sSection, sKey, sValue, sDefault);
}
// PC 0x005E67E0
CExoIni::~CExoIni()
{
    delete m_pInternal;
    m_pInternal = 0;
}
#else
// PC 0x005E6750
CExoIni::CExoIni()
{
}
// PC 0x005E67C0  Xbox 0x000C7740
int CExoIni::ReadIniEntry(CExoString& sValue, const CExoString& sSection,
                          const CExoString& sKey, const CExoString& sDefault)
{
    return 0;
}
// PC 0x005E67D0
int CExoIni::WriteIniEntry(const CExoString& sSection, const CExoString& sKey,
                           const CExoString& sValue, const CExoString& sDefault)
{
    return 0;
}
// PC 0x005E67E0
CExoIni::~CExoIni()
{
}
#endif
