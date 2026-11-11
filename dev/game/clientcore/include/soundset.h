#ifndef __SOUNDSET_H__
#define __SOUNDSET_H__

#include "exoresman.h"

const unsigned short SOUNDSET_INVALID = 0xFFFF;

class CResRef;
struct STR_RES;

enum SoundSetSound
{

    SOUNDSET_NASTY = 10,

    SOUNDSET_ATTACK_VOICE_2 = 11,
    SOUNDSET_ATTACK_VOICE_3 = 12,

    SOUNDSET_PAIN_1 = 13,
    SOUNDSET_PAIN_2 = 14,

    SOUNDSET_DEATH = 16,
    SOUNDSET_ENTER_STEALTH = 21
};

struct SSFFileHeader
{
    char m_szFileType[4];
    char m_szFileVersion[4];
    unsigned long m_nStrRefTableOffset;
};

class CSoundSet : public CResHelper<CResSSF, RESTYPE_SSF>
{
public:
    CSoundSet(const CResRef& cResRef);
    virtual ~CSoundSet();

    int GetStrres(unsigned short nSound, STR_RES& strRes);
};

class CSoundSetList
{
public:
    CSoundSetList();
    ~CSoundSetList();

    CResRef GetSoundSet(unsigned short nSoundSet);

    unsigned short m_nCount;
    CResRef* m_pResRefs;
    unsigned char* m_pGenders;
    unsigned long* m_pStrRefs;
};

#endif
