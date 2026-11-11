#ifndef __NWSEFFECT_H__
#define __NWSEFFECT_H__

#include "exobase.h"

class CResGFF;
class CResStruct;

#ifndef _XBOX
#pragma pack(push, 1)
#endif

class CGameEffect
{
public:
    CGameEffect(int bDynamic);
    CGameEffect(CGameEffect* pEffect);
    ~CGameEffect();

    void SetNumIntegers(int nNumIntegers);
    int GetInteger(int nIndex);
    void SetInteger(int nIndex, int nValue);
    float GetFloat(int nIndex);
    void SetFloat(int nIndex, float fValue);
    unsigned long GetObjectID(int nIndex);
    void SetObjectID(int nIndex, unsigned long oidValue);
    CExoString GetString(int nIndex);
    void SetString(int nIndex, const CExoString& sValue);
    void SetExpiryTime(unsigned long nDay, unsigned long nTime);
    void GetExpiryTime(unsigned long* pnDay, unsigned long* pnTime);
    int SaveGameEffect(CResGFF* pRes, CResStruct* pStruct);
    void SetCreator(unsigned long oidCreator);
    int GetEffectTypeScriptConstant();
    void LoadGameEffect(CResGFF* pRes, CResStruct* pStruct);
    void CopyEffect(CGameEffect* pEffect, int bSkipIntegers);
    void UpdateLinked();
    void SetLinked(CGameEffect* pLeft, CGameEffect* pRight);

    unsigned short GetEffectType() { return m_nType; }

    float GetDuration() { return m_fDuration; }

    unsigned long GetCreator() { return m_oidCreator; }

    unsigned long GetSpellId() { return m_nSpellId; }

    enum SpellId { SPELL_ID_INVALID = -1 };

    unsigned __int64 m_nID;
    unsigned short m_nType;
    unsigned short m_nSubType;
    unsigned short GetSubType();
    float m_fDuration;
    unsigned long m_nExpireDay;
    unsigned long m_nExpireTime;
    unsigned long m_oidCreator;
    unsigned long m_nSpellId;
    int m_bExposed;

    int m_nCasterLevel;
    CGameEffect* m_pLinkLeft;
    CGameEffect* m_pLinkRight;
    int m_nNumIntegers;
    int* m_pIntegers;
    float m_fFloat[4];
    CExoString m_sString[6];
    unsigned long m_oidObject[4];
    int m_bSkipOnLoad;

    enum
    {
        DURATION_MASK = 0x07,

        DURATION_EQUIPPED = 3,
        SUBTYPE_MASK = 0x18
    };

    static unsigned __int64 m_nNextID;
    static const unsigned short DURATION_TYPE_INSTANTANEOUS;
    static const unsigned short DURATION_TYPE_TEMPORARY;
    static const unsigned short DURATION_TYPE_EQUIPPED;
    static const unsigned short DURATION_TYPE_PERMANENT;
    static const unsigned short DURATION_TYPE_INNATE;
    static const unsigned short SUBTYPE_MAGICAL;
    static const unsigned short SUBTYPE_SUPERNATURAL;
    static const unsigned short SUBTYPE_EXTRAORDINARY;
};

#ifndef _XBOX
#pragma pack(pop)
#endif

#endif
