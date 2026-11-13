#include "exobase.h"
#include "exoresman.h"
#include "exotlktable.h"
#include "clientexoappinternal.h"
#include "twodimarray.h"
#include "soundset.h"
// PC 0x006CDFD0 GOG 0x006CDCD0
int CSoundSet::GetStrres(unsigned short nSound, STR_RES& strRes)
{
    SSFFileHeader* pHeader;
    unsigned long nStrRef;

    pHeader = (SSFFileHeader*)m_pRes->GetData();
    if (nSound <= 0)
    {
        return 0;
    }
    if (nSound >= 29)
    {
        return 0;
    }
    nStrRef = *(unsigned long*)((char*)pHeader + pHeader->m_nStrRefTableOffset +
                                4 * nSound - 4);
    if (nStrRef != 0xFFFFFFFF)
    {
        g_pTlkTable->Fetch(nStrRef, strRes, CTlkTable::GENDER_MALE);
        return 1;
    }
    return 0;
}
// PC 0x006CE030 GOG 0x006CDD30
CSoundSetList::~CSoundSetList()
{
    if (m_pResRefs != NULL)
    {
        delete[] m_pResRefs;
        m_pResRefs = NULL;
    }
    if (m_pGenders != NULL)
    {
        delete[] m_pGenders;
        m_pGenders = NULL;
    }
    if (m_pStrRefs != NULL)
    {
        delete[] m_pStrRefs;
        m_pStrRefs = NULL;
    }
}
// PC 0x006CE080 GOG 0x006CDD80
CResRef CSoundSetList::GetSoundSet(unsigned short nSoundSet)
{
    if (nSoundSet >= m_nCount)
    {
        CResRef cResRef;
        return cResRef;
    }

    return m_pResRefs[nSoundSet];
}
// PC 0x006CE2D0 GOG 0x006CDFD0
CSoundSet::CSoundSet(const CResRef& cResRef)
{
    SetResRef(cResRef, 0);
}
// PC 0x006CE350 GOG 0x006CE050
CSoundSet::~CSoundSet()
{
    if (m_pRes != NULL && m_pRes->GetDemands() != 0)
    {
        m_pRes->Release();
    }
}
// PC 0x006CE3C0 GOG 0x006CE0C0
CSoundSetList::CSoundSetList()
{
    C2DA* pSoundSet;
    C2DA* pSoundSetType;
    unsigned short i;
    int nCount;

    m_pResRefs = NULL;
    m_pGenders = NULL;
    m_pStrRefs = NULL;
    m_nCount = 0;

    pSoundSet = new C2DA("SoundSet", 0);
    if (pSoundSet != NULL && pSoundSet->Load2DArray())
    {
        pSoundSetType = new C2DA("SoundSetType", 0);
        if (pSoundSetType == NULL)
        {
            delete pSoundSet;
        }
        else if (!pSoundSetType->Load2DArray())
        {
            delete pSoundSet;
        }
        else
        {
            nCount = pSoundSet->m_nNumRows;
            m_nCount = nCount;
            m_pResRefs = new CResRef[m_nCount];
            m_pGenders = new unsigned char[m_nCount];
            m_pStrRefs = new unsigned long[m_nCount];

            for (i = 0; i < m_nCount; i++)
            {
                CExoString sResRef;
                int nValue;

                pSoundSet->GetCExoStringEntry(i, "RESREF", &sResRef);
                m_pResRefs[i] = sResRef;

                pSoundSet->GetINTEntry(i, "STRREF", &nValue);
                m_pStrRefs[i] = nValue;

                pSoundSet->GetINTEntry(i, "GENDER", &nValue);
                m_pGenders[i] = (unsigned char)nValue;
            }

            delete pSoundSet;
            delete pSoundSetType;
        }
    }
}
