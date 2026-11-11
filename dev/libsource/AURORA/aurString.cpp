#include <stdio.h>
#include <string.h>

#include "aurmodel.h"
#include "aurtexman.h"
#include "aurString.h"
#include "aurbehavior.h"

char g_szNextStringName[256] = "string0";

class StringGob : public Gob, public CAurString
{
public:

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

CAurString::CAurString(CAurTexture* pTexture) : TextureController(pTexture)
{
    m_nFont = -1;
}

void CAurString::Control()
{
}

void CAurString::Init()
{
}

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

CAurString* NewCAurString(int nMaxLength)
{
    StringGob* pGob = new StringGob(nMaxLength);
    return pGob;
}

void CAurString::DieWithoutParent()
{
    StringGob* pGob = (StringGob*)this;

    if (pGob->m_pBehavior != NULL)
    {
        pGob->m_pBehavior->Message("diewithoutparent 1");
    }
}
