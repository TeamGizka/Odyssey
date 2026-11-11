#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "exobase.h"
#include <aurlist.h>
#include "aurrendergl.h"
#include "aurtexman.h"
#include "aurtexcontrol.h"
#include "aurtexman.h"
#include "aurora.h"
#include "aurparse.h"
#include "aurString.h"

static unsigned int g_nFontNum = -1;
static bool g_bUseTexturedFont = true;
int initialized = 0;
int g_bEnableAurPostString = 1;
bool g_bPrintfUseRightSide = true;
List<PostedString*> posts;
extern int g_bEnableAurPrintf;
int g_nPrintfIndentChars = 0;
int g_nCurrentPrintLine;

char* FuncInterp(char* sLine);

#ifdef _XBOX

class TraceString
{
public:
    char m_szCommand[256];

    TraceString(char* szCommand);
    ~TraceString();
};

List<TraceString*> TraceStrings;
#else
List<char*> TraceStrings;
#endif
CStringColor g_cDefCol(1.0f, 1.0f, 1.0f, 1.0f);
CStringColor g_cDefCol2(1.0f, 1.0f, 1.0f, 1.0f);

int countreset();

#ifdef _XBOX
static StringGob* g_pStringObj;
#else
static CAurString* g_pStringObj;
#endif
#ifdef _XBOX

void AurOrtho(double dLeft, double dRight, double dBottom, double dTop,
              double dNear, double dFar)
{
    GLRender::SetOrthoTransform(dLeft, dRight, dBottom, dTop, dNear, dFar);
}

void AurInitFont()
{
}

void AurPrintString(char* szString, int nColumn, int nRow)
{
}
#else
// PC 0x00461070
inline void GLRender::SetUpTextFormat()
{
    int nChar;
    int nCount;
    GLubyte* pBitmap;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    fontOffset = glGenLists(128);
    nChar = 32;
    pBitmap = rasters[0];
    for (nCount = 95; nCount != 0; nCount--)
    {
        glNewList(fontOffset + nChar, GL_COMPILE);
        glBitmap(8, 13, 0.0f, 2.0f, 10.0f, 0.0f, pBitmap);
        glEndList();
        nChar++;
        pBitmap += 13;
    }
}

inline void FreeTextFormat()
{
    glDeleteLists(fontOffset, 128);
}

inline void GLRender::PrintStringAtScreenPos(char* szString, float fX, float fY)
{
    glPushAttrib(GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    glRasterPos2f(fX, fY);
    SpitLetters(szString);
    glRasterPos2f(0.0f, 0.0f);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}
// PC 0x004610F0
void AurInitFont()
{
    g_bUseTexturedFont = false;
    GLRender::SetUpTextFormat();
    initialized = 1;
}
#endif
// PC 0x00461110
void AurFreeFont()
{
    if (initialized != 0)
    {
        initialized = 0;
#ifndef _XBOX
        FreeTextFormat();
#endif
        if (g_pStringObj != NULL)
        {
            delete g_pStringObj;
        }
        g_pStringObj = NULL;
    }
}

#ifndef _XBOX

void PrintStringScreenPos_UseTextureFont(char* szString, float fX, float fY)
{
    GLint aViewport[4];

    glEnable(enableTranslationTable[ENABLE_BLEND]);
    glBlendFunc(blendTranslationTable[0], blendTranslationTable[1]);
    if (szString != NULL && *szString != 0)
    {
        glGetIntegerv(GL_VIEWPORT, aViewport);
        glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
        glDisable(enableTranslationTable[ENABLE_DEPTH_TEST]);
        glEnable(GL_COLOR_MATERIAL);
        glMatrixMode(transformTranslationTable[0]);
        glPushMatrix();
        glLoadIdentity();
        AurOrtho((aViewport[2] / -2) * 0.01f, (aViewport[2] / 2) * 0.01f,
                 (aViewport[3] / -2) * 0.01f, (aViewport[3] / 2) * 0.01f, 0.0,
                 10.0);
        glTranslatef(fX * 0.01f, fY * 0.01f, 0.0f);
        CAurFont::s_LoadedFontList.m_pList[g_nFontNum]->m_pTexture->Bind();
        CAurFont::s_LoadedFontList.m_pList[g_nFontNum]->TextOutA(
            szString, strlen(szString));
        glPopMatrix();
        glMatrixMode(transformTranslationTable[2]);
        glEnable(enableTranslationTable[ENABLE_DEPTH_TEST]);
        glDisable(GL_COLOR_MATERIAL);
        glDisable(enableTranslationTable[ENABLE_BLEND]);
        glPopAttrib();
    }
}

void AurPrintString(char* szString, int nColumn, int nRow)
{
    GLint aViewport[4];
    int nRows;
    int nColumns;
    float fX;
    float fY;

    if (g_bEnableAurPostString != 0)
    {
        glGetIntegerv(GL_VIEWPORT, aViewport);
        if (aViewport[2] >= 300 && aViewport[3] >= 100)
        {
            nRows = aViewport[3] / 14;
            nColumns = aViewport[2] / 10;
            if (nRow >= nRows)
            {
                nRow = nRows - 1;
            }
            if (nRow < 0)
            {
                nRow += nRows;
                if (nRow < 0)
                {
                    nRow = 0;
                }
            }
            if (nColumn < 0)
            {
                nColumn = nColumn + nColumns - strlen(szString) + 1;
            }
            if ((int)(nColumn + strlen(szString)) > nColumns)
            {
                nColumn = nColumns - strlen(szString);
            }
            if (nColumn < 0)
            {
                nColumn = 0;
            }
            nRow = nRows - nRow - 1;
            glDisable(enableTranslationTable[ENABLE_LIGHTING]);
            if (initialized == 0)
            {
                g_bUseTexturedFont = false;
                GLRender::SetUpTextFormat();
                initialized = 1;
            }
            if (g_bUseTexturedFont)
            {
                fY = (float)nRow / nRows * aViewport[3] - aViewport[3] / 2;
                fX =
                    (float)nColumn / nColumns * aViewport[2] - aViewport[2] / 2;
                PrintStringScreenPos_UseTextureFont(szString, fX, fY);
            }
            else if (szString != NULL && *szString != 0)
            {
                fY = (float)nRow / nRows;
                fX = (float)nColumn / nColumns;
                GLRender::PrintStringAtScreenPos(szString, fX, fY);
            }
            glEnable(enableTranslationTable[ENABLE_LIGHTING]);
        }
    }
}
#endif
#ifndef _XBOX

int SetConsoleFont(char* szFontName)
{
    if (initialized == 0)
    {
        AurInitFont();
    }
    g_bUseTexturedFont = true;
    if (g_pStringObj == NULL)
    {
        g_pStringObj = NewCAurString(1);
    }
    g_nFontNum = g_pStringObj->LoadFont(szFontName);
    if ((int)g_nFontNum >= CAurFont::s_LoadedFontList.m_nCount)
    {
        g_bUseTexturedFont = false;
    }
    return g_nFontNum;
}
#endif
void PostedString::Post()  // PC 0x00461150
{
    float fElapsedRatio;
    float fRed;
    float fGreen;
    float fBlue;
    float fAlpha;
    CStringColor cOldColor;

    if (m_fDuration > 0.0f)
    {
        fElapsedRatio = (m_fDuration - m_fTimeRemaining) / m_fDuration;
    }
    else
    {
        fElapsedRatio = 0.0f;
    }
    fAlpha =
        fElapsedRatio * m_fEndAlpha + (1.0f - fElapsedRatio) * m_fStartAlpha;
    fBlue = fElapsedRatio * m_fEndBlue + (1.0f - fElapsedRatio) * m_fStartBlue;
    fGreen =
        (1.0f - fElapsedRatio) * m_fStartGreen + fElapsedRatio * m_fEndGreen;
    fRed = fElapsedRatio * m_fEndRed + (1.0f - fElapsedRatio) * m_fStartRed;
    GLRender_SetColor4f(fRed, fGreen, fBlue, fAlpha);
#ifndef _XBOX
    cOldColor = g_cSavedCol;
    g_cSavedCol.m_fRed =
        fElapsedRatio * m_fEndRed + (1.0f - fElapsedRatio) * m_fStartRed;
    g_cSavedCol.m_fGreen =
        (1.0f - fElapsedRatio) * m_fStartGreen + fElapsedRatio * m_fEndGreen;
    g_cSavedCol.m_fBlue =
        fElapsedRatio * m_fEndBlue + (1.0f - fElapsedRatio) * m_fStartBlue;
    g_cSavedCol.m_fAlpha =
        fElapsedRatio * m_fEndAlpha + (1.0f - fElapsedRatio) * m_fStartAlpha;
    AurPrintString(m_szString, m_nColumn, m_nRow);
    g_cSavedCol = cOldColor;
#endif
    m_fTimeRemaining -= DeltaT;
    if (m_fDuration > 0.0f)
    {
        if (m_fTimeRemaining < 0.0f)
        {
#ifndef _XBOX
            posts.Remove(this);
#endif
            delete this;
        }
    }
}
// PC 0x00461240
void AurStringsFree()
{
    int i;
    PostedString* pPostedString;

    i = posts.m_nCount;
    while (i != 0)
    {
        pPostedString = posts.m_pList[--i];
        if (pPostedString != NULL)
        {
#ifndef _XBOX
            posts.Remove(pPostedString);
#endif
            delete pPostedString;
        }
    }
}
// PC 0x004612B0
char* untrace(char* szPrefix)
{
    static char buf[256];
    int i;
    int nDeleted;
#ifdef _XBOX
    TraceString* pTrace;

    i = TraceStrings.m_nCount;
    nDeleted = 0;
    while (i != 0)
    {
        pTrace = TraceStrings.m_pList[--i];
        if (szPrefix != NULL && *szPrefix != 0)
        {
            if (strncmp(pTrace->m_szCommand, szPrefix, strlen(szPrefix)) != 0)
            {
                continue;
            }
        }
        delete pTrace;
        nDeleted++;
    }
#else
    char* szTrace;

    i = TraceStrings.m_nCount;
    nDeleted = 0;
    while (i != 0)
    {
        szTrace = TraceStrings.m_pList[--i];
        if (szPrefix != NULL && *szPrefix != 0)
        {
            if (strncmp(szTrace, szPrefix, strlen(szPrefix)) != 0)
            {
                continue;
            }
        }
        if (szTrace != NULL)
        {
            TraceStrings.Remove(szTrace);
            delete szTrace;
        }
        nDeleted++;
    }
#endif
    sprintf(buf, "deleted %d trace commands", nDeleted);
    return buf;
}

PostedString::PostedString(char* szString, int nColumn, int nRow,
                           float fDuration)
{
    char* szSource;
    int nOffset;
    char cCharacter;
    int i;
    PostedString* pPostedString;

    szSource = szString;
    nOffset = m_szString - szString;
    do
    {
        cCharacter = *szSource;
        szSource[nOffset] = *szSource;
        szSource++;
    } while (cCharacter != 0);
    m_fDuration = fDuration;
    m_fTimeRemaining = fDuration;
    m_nColumn = nColumn;
    m_nRow = nRow;
    i = posts.m_nCount;
    m_nLength = strlen(szString);
    while (i != 0)
    {
        pPostedString = posts.m_pList[--i];
#ifdef _XBOX

        PostedString* pOther = posts.m_pList[i];
        if ((pOther->m_nColumn <= m_nColumn &&
             posts.m_pList[i]->m_nColumn + posts.m_pList[i]->m_nLength >=
                 m_nColumn) ||
            (m_nColumn <= pOther->m_nColumn &&
             m_nColumn + m_nLength >= pOther->m_nColumn))
        {
            if (pOther->m_nRow == m_nRow)
#else
        if ((posts.m_pList[i]->m_nColumn <= m_nColumn &&
             posts.m_pList[i]->m_nColumn + posts.m_pList[i]->m_nLength >=
                 m_nColumn) ||
            (m_nColumn <= posts.m_pList[i]->m_nColumn &&
             m_nColumn + m_nLength >= posts.m_pList[i]->m_nColumn))
        {
            if (posts.m_pList[i]->m_nRow == m_nRow)
#endif
            {
                if (pPostedString != NULL)
                {
#ifndef _XBOX
                    posts.Remove(pPostedString);
#endif
                    delete pPostedString;
                }
            }
        }
    }
    posts.Add(this);
    m_fStartRed = g_cDefCol.m_fRed;
    m_fStartGreen = g_cDefCol.m_fGreen;
    m_fStartBlue = g_cDefCol.m_fBlue;
    m_fStartAlpha = g_cDefCol.m_fAlpha;
    m_fEndRed = g_cDefCol2.m_fRed;
    m_fEndGreen = g_cDefCol2.m_fGreen;
    m_fEndBlue = g_cDefCol2.m_fBlue;
    m_fEndAlpha = g_cDefCol2.m_fAlpha;
}
// PC 0x004614F0
void AurPostString(char* szString, int nColumn, int nRow, float fDuration)
{
    new PostedString(szString, nColumn, nRow, fDuration);
}
// PC 0x00461550
void AurPrintf(const char* szFormat, ...)
{
    char szBuffer[512];
    PostedString* pPostedString;
    va_list vaArgs;

    if (g_bEnableAurPrintf != 0)
    {
        va_start(vaArgs, szFormat);
        vsprintf(szBuffer, szFormat, vaArgs);
        pPostedString = new PostedString(
            szBuffer,
            g_bPrintfUseRightSide ? 80 - strlen(szBuffer) - g_nPrintfIndentChars
                                  : g_nPrintfIndentChars,
            g_nCurrentPrintLine = (g_nCurrentPrintLine + 1) % 40, 10.0f);
        pPostedString->m_fStartRed = 0.92f;
        pPostedString->m_fStartGreen = 0.91f;
        pPostedString->m_fStartBlue = 0.9f;
        pPostedString->m_fStartAlpha = 0.75f;
        pPostedString->m_fEndRed = 0.35f;
        pPostedString->m_fEndGreen = 0.3f;
        pPostedString->m_fEndBlue = 0.4f;
        pPostedString->m_fEndAlpha = 0.3f;
        va_end(vaArgs);
    }
}
// PC 0x00461680
void PostTraceStrings()
{
    char szBuffer[512];
    bool bKeepHistory;
#ifdef _XBOX
    TraceString** ppTrace;
    int i;

    bKeepHistory = g_bKeepHistory;
    i = 0;
    g_bKeepHistory = false;
    for (; i < TraceStrings.m_nCount; i++)
    {
        ppTrace = &TraceStrings.m_pList[i];
        sprintf(szBuffer, "%s: %s", (*ppTrace)->m_szCommand, FuncInterp((*ppTrace)->m_szCommand));
        AurPostString(szBuffer, 4, -4 - i, 0.0f);
    }
#else
    char** ppTrace;
    int i;

    bKeepHistory = g_bKeepHistory;
    i = 0;
    g_bKeepHistory = false;
    for (; i < TraceStrings.m_nCount; i++)
    {
        ppTrace = &TraceStrings.m_pList[i];
        sprintf(szBuffer, "%s: %s", *ppTrace, FuncInterp(*ppTrace));
        AurPostString(szBuffer, 0, -1 - i, 0.0f);
    }
#endif
    g_bKeepHistory = bKeepHistory;
}
// PC 0x00461700
void AurRenderStrings()
{
    int i;

    PostTraceStrings();
    GLRender_LoadMatrixIdentity();
    i = posts.m_nCount;
    while (i != 0)
    {
        posts.m_pList[--i]->Post();
    }
    countreset();
}

int g_bEnableAurPrintf = 0;

#ifdef _XBOX

TraceString::TraceString(char* szCommand)
{
    strcpy(m_szCommand, szCommand);
    TraceStrings.Add(this);
}

TraceString::~TraceString()
{
    TraceStrings.Remove(this);
}

char* trace(char* szCommand)
{
    for (int i = 0; i < TraceStrings.m_nCount; i++)
    {
        if (strcmp(TraceStrings.m_pList[i]->m_szCommand, szCommand) == 0)
        {
            return "already being traced";
        }
    }
    return (char*)new TraceString(szCommand);
}
#endif

#ifdef _XBOX
void AurSetStringColor(char* szColors)
{
    g_cDefCol = CStringColor(1.0f, 1.0f, 1.0f, 1.0f);
    g_cDefCol2 = CStringColor(1.0f, 1.0f, 1.0f, 1.0f);
    if (sscanf(szColors, "%f %f %f %f %f %f %f %f",
               &g_cDefCol.m_fRed, &g_cDefCol.m_fGreen, &g_cDefCol.m_fBlue, &g_cDefCol.m_fAlpha,
               &g_cDefCol2.m_fRed, &g_cDefCol2.m_fGreen, &g_cDefCol2.m_fBlue, &g_cDefCol2.m_fAlpha) < 5)
    {
        g_cDefCol2 = g_cDefCol;
    }
}
#endif

#ifdef _XBOX
PostedString::~PostedString()
{
    posts.Remove(this);
}
#endif

#ifdef _XBOX
int SetConsoleFont(char* szFontName)
{
    g_bUseTexturedFont = true;
    if (g_pStringObj == NULL)
    {
        g_pStringObj = new StringGob(1);
    }
    g_nFontNum = g_pStringObj->SetFont(szFontName);
    if ((int)g_nFontNum >= CAurFont::s_LoadedFontList.m_nCount)
    {
        g_bUseTexturedFont = false;
    }
    return g_nFontNum;
}
#endif
