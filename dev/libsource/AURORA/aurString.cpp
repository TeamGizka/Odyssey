#include <stdio.h>
#include <string.h>

#include "aurmodel.h"
#include "aurtexman.h"
#include "aurString.h"
#include "aurbehavior.h"

char g_szNextStringName[256] = "string0";
#ifndef _XBOX

class StringGob : public Gob, public CAurString
{
public:
    // Xbox 0x002F1260
    StringGob(int nMaxLength) : Gob(g_szNextStringName), CAurString(NULL)
    {

        m_szText = "[";
        static int nStringCount = 0;
        sprintf(g_szNextStringName, "string%d", ++nStringCount);
        m_bWrap = nMaxLength != 1;
        m_nMaxLength = nMaxLength;
    }

    int m_nMaxLength;
    int m_bWrap;
    char* m_szText;
};
// Dead code: not in the PC executable
CAurString::CAurString(CAurTexture* pTexture) : TextureController(pTexture)
{
    m_nFont = -1;
}
// Dead code: not in the PC executable
void CAurString::Control()
{
}
// Dead code: not in the PC executable
void CAurString::Init()
{
}
// Dead code: not in the PC executable
int CAurString::LoadFont(char* szFontName)
{
    int i;

    for (i = 0; i < CAurFont::s_LoadedFontList.m_nCount; i++)
    {
        CAurFont* pFont = CAurFont::s_LoadedFontList.m_pList[i];
        if (pFont->m_pTexture != NULL &&
            strcmp(pFont->m_pTexture->GetName(), szFontName) == 0)
        {
            break;
        }
    }
    m_nFont = i;
    return i;
}
// Dead code: not in the PC executable
CAurString* NewCAurString(int nMaxLength)
{
    StringGob* pGob = new StringGob(nMaxLength);
    return pGob;
}
// Dead code: not in the PC executable
void CAurString::DieWithoutParent()
{
    StringGob* pGob = (StringGob*)this;

    if (pGob->m_pBehavior != NULL)
    {
        pGob->m_pBehavior->Message("diewithoutparent 1");
    }
}
#endif

#ifdef _XBOX
// Xbox 0x002F1260
StringGob::StringGob(int nMaxLength)
    : Gob(g_szNextStringName), m_sCaret("["), m_nCaretLinePos(0), m_nCaretColumnPos(0), m_fCaretTime(0.0f)
{
}
// Xbox 0x002F0F20
StringGob::~StringGob()
{
}
#endif
