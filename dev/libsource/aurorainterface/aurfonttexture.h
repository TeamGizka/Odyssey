#ifndef __AURFONTTEXTURE_H__
#define __AURFONTTEXTURE_H__

#include "aurora.h"
#include "resfnt.h"

#define FNT_GLYPH_SIZE 16

class CAuroraFontTexture;

#pragma pack(push, 1)

struct SFontTextureDataTrailer
{
    unsigned short nType;
    CAuroraFontTexture* pOwner;
};

#pragma pack(pop)

class CAuroraFontTexture : public CResHelper<CResFNT, RESTYPE_FNT>
{
public:
    CAuroraFontTexture(CResRef cResRef, int bRequest);
    virtual ~CAuroraFontTexture();

    unsigned long* GetFontDataPtr();
    int GetFontTextureAttrib(unsigned long* pnWidth, unsigned long* pnHeight,
                         unsigned char* pnDepth);
    unsigned char* GetFontTextureDataPtr();
    unsigned char* GetPtrToResHelperInfo();
    int Load();
    void Unload();
};

#endif
