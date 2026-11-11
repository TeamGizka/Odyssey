#ifndef AURSTRING_H
#define AURSTRING_H

#include "aurtexcontrol.h"

class CAurString : public TextureController
{
public:
    CAurString(CAurTexture* pTexture);
    virtual void Control();
    virtual void Init();
    int LoadFont(char* szFontName);
    void DieWithoutParent();

    int m_nFont;
};

CAurString* NewCAurString(int nMaxLength);

#endif
