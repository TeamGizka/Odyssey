#include <stdio.h>
#include <string.h>
#include "exobase.h"
#include "exoencrypt.h"

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define FF(a, b, c, d, x, s, ac) \
    { \
        (a) += F((b), (c), (d)) + (x) + (unsigned long)(ac); \
        (a) = ROTATE_LEFT((a), (s)); \
        (a) += (b); \
    }
#define GG(a, b, c, d, x, s, ac) \
    { \
        (a) += G((b), (c), (d)) + (x) + (unsigned long)(ac); \
        (a) = ROTATE_LEFT((a), (s)); \
        (a) += (b); \
    }
#define HH(a, b, c, d, x, s, ac) \
    { \
        (a) += H((b), (c), (d)) + (x) + (unsigned long)(ac); \
        (a) = ROTATE_LEFT((a), (s)); \
        (a) += (b); \
    }
#define II(a, b, c, d, x, s, ac) \
    { \
        (a) += I((b), (c), (d)) + (x) + (unsigned long)(ac); \
        (a) = ROTATE_LEFT((a), (s)); \
        (a) += (b); \
    }

static unsigned char PADDING[64] =
{
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Dead code: not in the PC executable
CExoString CExoEncrypt::EncryptString(const CExoString& sString)
{
    char szHash[33];

    MD5Init();
    MD5Update((unsigned char*)sString.CStr(), sString.GetLength());
    MD5Final();
    for (char i = 0; i < 16; i++)
    {
        sprintf(&szHash[i * 2], "%02x", m_anDigest[i]);
    }
    szHash[32] = 0;
    return CExoString(szHash);
}

// Dead code: not in the PC executable
void CExoEncrypt::MD5Init()
{
    m_anCount[0] = m_anCount[1] = 0;
    m_anState[0] = 0x67452301;
    m_anState[1] = 0xEFCDAB89;
    m_anState[2] = 0x98BADCFE;
    m_anState[3] = 0x10325476;
    memset(m_anInput, 0, sizeof(m_anInput));
    memset(m_anBuffer, 0, sizeof(m_anBuffer));
    memset(m_anDigest, 0, sizeof(m_anDigest));
}

// Dead code: not in the PC executable
void CExoEncrypt::MD5Update(unsigned char* pInput, unsigned long nInputLength)
{
    unsigned long anInput[16];
    int nIndex;
    unsigned int i, ii;

    nIndex = (int)((m_anCount[0] >> 3) & 0x3F);

    if ((m_anCount[0] + ((unsigned long)nInputLength << 3)) < m_anCount[0])
    {
        m_anCount[1]++;
    }
    m_anCount[0] += ((unsigned long)nInputLength << 3);
    m_anCount[1] += ((unsigned long)nInputLength >> 29);

    while (nInputLength--)
    {
        m_anInput[nIndex++] = *pInput++;

        if (nIndex == 0x40)
        {
            for (i = 0, ii = 0; i < 16; i++, ii += 4)
            {
                anInput[i] = (((unsigned long)m_anInput[ii + 3]) << 24) |
                             (((unsigned long)m_anInput[ii + 2]) << 16) |
                             (((unsigned long)m_anInput[ii + 1]) << 8) |
                             ((unsigned long)m_anInput[ii]);
            }
            Transform(m_anState, anInput);
            nIndex = 0;
        }
    }
}

// Dead code: not in the PC executable
void CExoEncrypt::MD5Final()
{
    unsigned long anInput[16];
    int nIndex;
    unsigned int i, ii;
    unsigned int nPadLength;

    anInput[14] = m_anCount[0];
    anInput[15] = m_anCount[1];

    nIndex = (int)((m_anCount[0] >> 3) & 0x3F);

    nPadLength = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
    MD5Update(PADDING, nPadLength);

    for (i = 0, ii = 0; i < 14; i++, ii += 4)
    {
        anInput[i] = (((unsigned long)m_anInput[ii + 3]) << 24) |
                     (((unsigned long)m_anInput[ii + 2]) << 16) |
                     (((unsigned long)m_anInput[ii + 1]) << 8) |
                     ((unsigned long)m_anInput[ii]);
    }
    Transform(m_anState, anInput);

    for (i = 0, ii = 0; i < 4; i++, ii += 4)
    {
        m_anDigest[ii] = (unsigned char)(m_anState[i] & 0xFF);
        m_anDigest[ii + 1] = (unsigned char)((m_anState[i] >> 8) & 0xFF);
        m_anDigest[ii + 2] = (unsigned char)((m_anState[i] >> 16) & 0xFF);
        m_anDigest[ii + 3] = (unsigned char)((m_anState[i] >> 24) & 0xFF);
    }
}

// Dead code: not in the PC executable
CExoString CExoEncrypt::GenerateChallenge(unsigned short nLength)
{
    return g_pExoBase->m_pExoRand->GetString(nLength);
}

// Dead code: not in the PC executable
void CExoEncrypt::Transform(unsigned long* pState, unsigned long* pInput)
{
    unsigned long a = pState[0], b = pState[1], c = pState[2], d = pState[3];

    FF(a, b, c, d, pInput[0], S11, 0xD76AA478);
    FF(d, a, b, c, pInput[1], S12, 0xE8C7B756);
    FF(c, d, a, b, pInput[2], S13, 0x242070DB);
    FF(b, c, d, a, pInput[3], S14, 0xC1BDCEEE);
    FF(a, b, c, d, pInput[4], S11, 0xF57C0FAF);
    FF(d, a, b, c, pInput[5], S12, 0x4787C62A);
    FF(c, d, a, b, pInput[6], S13, 0xA8304613);
    FF(b, c, d, a, pInput[7], S14, 0xFD469501);
    FF(a, b, c, d, pInput[8], S11, 0x698098D8);
    FF(d, a, b, c, pInput[9], S12, 0x8B44F7AF);
    FF(c, d, a, b, pInput[10], S13, 0xFFFF5BB1);
    FF(b, c, d, a, pInput[11], S14, 0x895CD7BE);
    FF(a, b, c, d, pInput[12], S11, 0x6B901122);
    FF(d, a, b, c, pInput[13], S12, 0xFD987193);
    FF(c, d, a, b, pInput[14], S13, 0xA679438E);
    FF(b, c, d, a, pInput[15], S14, 0x49B40821);

    GG(a, b, c, d, pInput[1], S21, 0xF61E2562);
    GG(d, a, b, c, pInput[6], S22, 0xC040B340);
    GG(c, d, a, b, pInput[11], S23, 0x265E5A51);
    GG(b, c, d, a, pInput[0], S24, 0xE9B6C7AA);
    GG(a, b, c, d, pInput[5], S21, 0xD62F105D);
    GG(d, a, b, c, pInput[10], S22, 0x02441453);
    GG(c, d, a, b, pInput[15], S23, 0xD8A1E681);
    GG(b, c, d, a, pInput[4], S24, 0xE7D3FBC8);
    GG(a, b, c, d, pInput[9], S21, 0x21E1CDE6);
    GG(d, a, b, c, pInput[14], S22, 0xC33707D6);
    GG(c, d, a, b, pInput[3], S23, 0xF4D50D87);
    GG(b, c, d, a, pInput[8], S24, 0x455A14ED);
    GG(a, b, c, d, pInput[13], S21, 0xA9E3E905);
    GG(d, a, b, c, pInput[2], S22, 0xFCEFA3F8);
    GG(c, d, a, b, pInput[7], S23, 0x676F02D9);
    GG(b, c, d, a, pInput[12], S24, 0x8D2A4C8A);

    HH(a, b, c, d, pInput[5], S31, 0xFFFA3942);
    HH(d, a, b, c, pInput[8], S32, 0x8771F681);
    HH(c, d, a, b, pInput[11], S33, 0x6D9D6122);
    HH(b, c, d, a, pInput[14], S34, 0xFDE5380C);
    HH(a, b, c, d, pInput[1], S31, 0xA4BEEA44);
    HH(d, a, b, c, pInput[4], S32, 0x4BDECFA9);
    HH(c, d, a, b, pInput[7], S33, 0xF6BB4B60);
    HH(b, c, d, a, pInput[10], S34, 0xBEBFBC70);
    HH(a, b, c, d, pInput[13], S31, 0x289B7EC6);
    HH(d, a, b, c, pInput[0], S32, 0xEAA127FA);
    HH(c, d, a, b, pInput[3], S33, 0xD4EF3085);
    HH(b, c, d, a, pInput[6], S34, 0x04881D05);
    HH(a, b, c, d, pInput[9], S31, 0xD9D4D039);
    HH(d, a, b, c, pInput[12], S32, 0xE6DB99E5);
    HH(c, d, a, b, pInput[15], S33, 0x1FA27CF8);
    HH(b, c, d, a, pInput[2], S34, 0xC4AC5665);

    II(a, b, c, d, pInput[0], S41, 0xF4292244);
    II(d, a, b, c, pInput[7], S42, 0x432AFF97);
    II(c, d, a, b, pInput[14], S43, 0xAB9423A7);
    II(b, c, d, a, pInput[5], S44, 0xFC93A039);
    II(a, b, c, d, pInput[12], S41, 0x655B59C3);
    II(d, a, b, c, pInput[3], S42, 0x8F0CCC92);
    II(c, d, a, b, pInput[10], S43, 0xFFEFF47D);
    II(b, c, d, a, pInput[1], S44, 0x85845DD1);
    II(a, b, c, d, pInput[8], S41, 0x6FA87E4F);
    II(d, a, b, c, pInput[15], S42, 0xFE2CE6E0);
    II(c, d, a, b, pInput[6], S43, 0xA3014314);
    II(b, c, d, a, pInput[13], S44, 0x4E0811A1);
    II(a, b, c, d, pInput[4], S41, 0xF7537E82);
    II(d, a, b, c, pInput[11], S42, 0xBD3AF235);
    II(c, d, a, b, pInput[2], S43, 0x2AD7D2BB);
    II(b, c, d, a, pInput[9], S44, 0xEB86D391);

    pState[0] += a;
    pState[1] += b;
    pState[2] += c;
    pState[3] += d;
}
