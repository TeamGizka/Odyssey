#ifndef __EXOENCRYPT_H__
#define __EXOENCRYPT_H__

#include "exobase.h"

class CExoEncrypt
{
public:
    CExoString EncryptString(const CExoString& sString);

    void MD5Init();
    void MD5Update(unsigned char* pInput, unsigned long nInputLength);
    void MD5Final();
    CExoString GenerateChallenge(unsigned short nLength);
    void Transform(unsigned long* pState, unsigned long* pInput);

    unsigned long m_anCount[2];
    unsigned long m_anState[4];
    unsigned char m_anInput[64];
    unsigned char m_anUnknown58[64];
    unsigned char m_anDigest[16];
};

#endif
