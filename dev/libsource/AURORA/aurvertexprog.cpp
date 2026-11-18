#include <string.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#include <GL/gl.h>
#else
#include "platform.h"
#endif

#include "aurrendergl.h"
#ifndef _XBOX
#include "aurglext.h"
#else
#include <xgraphics.h>
#endif
#include "auremitter.h"
#include "aurinternal.h"
#include "aurlightmanager.h"
#include "aurscene.h"
#include "aurlist.h"

int currentframe;

int nLastVertexProgram;
int nLastVertexProgramFrame;
#ifndef _XBOX
List<VertexProgram*> VertexProgramManager::cm_listExistentVertexPrograms;

unsigned long* cBumpyShinyRegularVPDeclaration = 0;
unsigned long* cBumpyShinyLightMappedVPDeclaration = 0;
unsigned long* cBumpyShinySkinVPDeclaration = 0;
unsigned long* cBumpMapOnePassVPDeclaration = 0;
unsigned long* cBumpMapSpecularVPDeclaration = 0;
unsigned long* cBumpMapDiffuseVPDeclaration = 0;
unsigned long* cBumpMapVPDeclaration = 0;
unsigned long* cStandardVPDeclaration = 0;
unsigned long* cStandardVP2Declaration = 0;
unsigned long* cStandardVP1To2Declaration = 0;
unsigned long* cFogTestVPDeclaration = 0;
unsigned long* cLightProjectionVPDeclaration = 0;
unsigned long* cFogOutputVPDeclaration = 0;
unsigned long* cSkinVPDeclaration = 0;
unsigned long* cSkinBumpedOutVPDeclaration = 0;
unsigned long* cSkin3BoneVPDeclaration = 0;
unsigned long* cSkinFogOutputVPDeclaration = 0;
unsigned long* cSkinBumpyVPDeclaration = 0;
unsigned long* cSkinSphereMapVPDeclaration = 0;
unsigned long* cSkinCubeMapVPDeclaration = 0;
unsigned long* cMotionBlurredVPDeclaration = 0;
unsigned long* cSkinMotionBlurVPDeclaration = 0;
unsigned long* cBlurVPDeclaration = 0;
unsigned long* cBumpedOutVPDeclaration = 0;
unsigned long* cPhongEMBMVPDeclaration = 0;
unsigned long* cPhongEMBMLightVPDeclaration = 0;
unsigned long* cPhongBumpSpecVPDeclaration = 0;
unsigned long* cPhongBumpDiffuseVPDeclaration = 0;
unsigned long* cPhongBumpOnePassVPDeclaration = 0;
unsigned long* cPhongEnvVPDeclaration = 0;
unsigned long* cPhongLightVPDeclaration = 0;
unsigned long* cPhongFlatVPDeclaration = 0;
unsigned long* cShadowTriMeshVariantVPDeclaration = 0;
unsigned long* cSkinShadowVPDeclaration = 0;
#endif

#ifdef _XBOX
// Xbox 0x0032C760
VertexProgram::VertexProgram(unsigned long* pFunction, unsigned long* pLitFunction,
                             unsigned long* pDeclaration)
{
    m_pFunction[0] = pFunction;
    if (pLitFunction)
    {
        m_pFunction[1] = pLitFunction;
    }
    else
    {
        m_pFunction[1] = pFunction;
    }
    m_nGLProgram[0] = 0;
    m_nGLProgram[1] = 0;
    m_pDeclaration = pDeclaration;
    m_nAllowedLights = 3;
    m_bLoaded = 0;
}
#else
// Dead code: not in the PC executable
void VertexProgram::FindNumberOfAllowedLights()
{
    int nInstructions;
    int nAllowedLights;
    int i;

    nInstructions = 0;
    for (i = 0; i < (int)strlen(m_sProgram); i++)
    {
        if (m_sProgram[i] == ';')
        {
            nInstructions++;
        }
    }

    nAllowedLights = (114 - nInstructions) / 14;
    m_nAllowedLights = nAllowedLights;
    if (nAllowedLights > 3)
    {
        nAllowedLights = 3;
    }

    m_nAllowedLights = nAllowedLights;
}
#endif
// PC 0x004CEB30 GOG 0x004CEC70 Xbox 0x0032C7A0
void AurVertexProgramMakeCurrent()
{
    currentframe++;
}

#ifndef _XBOX
void GLRender::DrawElementsInShadow(unsigned int nProgram,  // PC 0x004CEB40 GOG 0x004CEC80
                                    unsigned long nLength,
                                    const char* sProgram,
                                    unsigned long* pnProgram)
{
    if (usearbvertexprograms != 0)
    {
        glProgramStringARB(34336, 34933, nLength, (const void*)sProgram);
    }
    else
    {
        glLoadProgramNV(34336, nProgram, nLength,
                        (const unsigned char*)sProgram);
    }
}
#endif
// PC 0x004CEB80 GOG 0x004CECC0 Xbox 0x0032C7B0
void TrackMatrices()
{
#ifdef _XBOX
    GLRender::BindVertexProgram(0);
#else
    if (usearbvertexprograms == 0)
    {
        glTrackMatrixNV(34336, 0, nvMatrixTranslationTable[0],
                        nvMatrixTransformTranslationTable[0]);
    }

    if (usearbvertexprograms == 0)
    {
        glTrackMatrixNV(34336, 8, nvMatrixTranslationTable[1],
                        nvMatrixTransformTranslationTable[0]);
    }

    if (usearbvertexprograms != 0)
    {
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB, 0);
    }
    else
    {
        glBindProgramNV(GL_VERTEX_PROGRAM_NV, 0);
    }
#endif
}

#ifndef _XBOX
int VertexProgramManager::ResetVertexPrograms()  // PC 0x004CEBF0 GOG 0x004CED30
{
    int i;

    for (i = 0; i < cm_listExistentVertexPrograms.m_nCount; i++)
    {
        glDeleteProgramsARB(
            1, &cm_listExistentVertexPrograms.m_pList[i]->m_nGLProgram);
        cm_listExistentVertexPrograms.m_pList[i]->m_nGLProgram = 0;
        cm_listExistentVertexPrograms.m_pList[i]->m_bLoaded = 0;
    }

    return cm_listExistentVertexPrograms.m_nCount;
}
#else
// Xbox 0x0032C7C0
VertexProgram::~VertexProgram()
{
    if (GLRender::IsProgramNV(m_nGLProgram[0]))
    {
        GLRender::DeletePrograms(1, &m_nGLProgram[0]);
    }
    if (GLRender::IsProgramNV(m_nGLProgram[1]))
    {
        GLRender::DeletePrograms(1, &m_nGLProgram[1]);
    }
}
#endif
// PC 0x004CEC50 GOG 0x004CED90 Xbox 0x0032C810
void VertexProgram::Enable(Part* pPart, bool bEnableLights)
{
#ifdef _XBOX
    static unsigned long* lastProg;
    static bool wasLit;

    if (LightManager::cm_bLightManagerUpdated || nLastVertexProgramFrame != currentframe ||
        lastProg != m_pFunction[bEnableLights] || wasLit != bEnableLights)
    {
        int bNoObject = 1;
        if (pPart && pPart->m_pAurObject)
        {
            bNoObject = 0;
        }
        if (currentscene)
        {
            currentscene->m_pLightManager->SetUpVertexProgramLightShaderConstants(
                m_nAllowedLights, bNoObject);
        }
    }

    wasLit = bEnableLights;
    LightManager::cm_bLightManagerUpdated = 0;
    nLastVertexProgramFrame = currentframe;
    lastProg = m_pFunction[bEnableLights];

    unsigned int* pnProgram = &m_nGLProgram[bEnableLights];
    if (*pnProgram)
    {
        if (GLRender::IsProgramNV(*pnProgram))
        {
            GLRender::BindVertexProgram(*pnProgram);
            return;
        }
    }
    else
    {
        GLRender::GenPrograms(1, pnProgram);
    }
    GLRender::DrawElementsInShadow(*pnProgram, m_pFunction[bEnableLights], m_pDeclaration);
#else
    static char* lastProg;
    static bool wasLit;

    unsigned int nProgram;

    if (m_nGLProgram != 0 && lastProg == m_sProgram &&
        !LightManager::cm_bLightManagerUpdated &&
        wasLit == bEnableLights && AurNonRadeon8500Validate())
    {
        BindVertexProgram(m_nGLProgram);
        return;
    }

    lastProg = m_sProgram;
    wasLit = bEnableLights;
    LightManager::cm_bLightManagerUpdated = 0;

    if (pPart && bEnableLights)
    {
        currentscene->m_pLightManager->SetUpVertexProgramLightShaderConstants(
            m_nAllowedLights, pPart->m_pAurObject == NULL);
    }

    if (m_nLastUpdateFrame != currentframe)
    {
        nProgram = m_nGLProgram;
        unsigned char bExists = usearbvertexprograms
            ? glIsProgramARB(nProgram) : glIsProgramNV(nProgram);
        if (!bExists || bEnableLights)
        {
            if (m_bLoaded && !CAurInternal::m_nHeadLightEnabled)
            {
                BindVertexProgram(m_nGLProgram);
                return;
            }
            m_nLastUpdateFrame = currentframe;
            strcpy(m_sVertexProgram, m_sProgram);
            if (bEnableLights && m_nAllowedLights > 0)
            {
                char* sLights = new char[LIGHT_PROGRAM_BUFFER_SIZE];
                sLights[0] = 0;
                LightManager::SetupVertexProgramLights(
                    sLights, m_nAllowedLights,
                    pPart ? pPart->m_pAurObject == NULL : false);
                strcat(m_sVertexProgram, sLights);
                delete[] sLights;
            }
            strcat(m_sVertexProgram, "END");

            if (!m_nGLProgram)
            {
                if (usearbvertexprograms)
                    glGenProgramsARB(1, &m_nGLProgram);
                else
                    glGenProgramsNV(1, &m_nGLProgram);
            }
            nProgram = m_nGLProgram;
            if (usearbvertexprograms != 0)
            {
                glBindProgramARB(GL_VERTEX_PROGRAM_ARB, nProgram);
            }
            else
            {
                glBindProgramNV(GL_VERTEX_PROGRAM_NV, nProgram);
            }

            GLRender::DrawElementsInShadow(m_nGLProgram,
                                           strlen(m_sVertexProgram),
                                           m_sVertexProgram, m_pProgram);
            if (usearbvertexprograms)
                AurGetVertexProgramARBError(m_sVertexProgram);
            if (pPart && bEnableLights && currentscene)
            {
                currentscene->m_pLightManager->SetUpVertexProgramLightShaderConstants(
                    m_nAllowedLights, pPart->m_pAurObject == NULL);
            }
            m_bLoaded = 1;
            return;
        }
    }
    nProgram = m_nGLProgram;
    if (usearbvertexprograms != 0)
    {
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB, nProgram);
    }
    else
    {
        glBindProgramNV(GL_VERTEX_PROGRAM_NV, nProgram);
    }
#endif
}

#ifndef _XBOX
// PC 0x004CEF00 GOG 0x004CF040
void VertexProgramManager::AddVertexProgram(VertexProgram* pProgram)
{
    cm_listExistentVertexPrograms.Add(pProgram);
}
// Dead code: not in the PC executable
void VertexProgramManager::RemoveVertexProgram(VertexProgram* pProgram)
{
    cm_listExistentVertexPrograms.Remove(pProgram);
}
// PC 0x004CEF60 GOG 0x004CF0A0 Xbox 0x0032C760
VertexProgram::VertexProgram(char* sProgram, unsigned long* pnProgram)
{
    m_sProgram = sProgram;
    m_nGLProgram = 0;
    m_nLastUpdateFrame = -1;
    m_pProgram = pnProgram;
    m_nAllowedLights = 3;
    m_bLoaded = 0;
    VertexProgramManager::AddVertexProgram(this);
}

VertexProgram::~VertexProgram()  // PC 0x004CEFB0 GOG 0x004CF0F0
{
    unsigned char bExists;
    unsigned int nProgram = m_nGLProgram;

    if (usearbvertexprograms != 0)
    {
        bExists = glIsProgramARB(nProgram);
    }
    else
    {
        bExists = glIsProgramNV(nProgram);
    }

    if (bExists != 0)
    {
        if (usearbvertexprograms != 0)
        {
            glDeleteProgramsARB(1, &m_nGLProgram);
        }
        else
        {
            glDeleteProgramsNV(1, &m_nGLProgram);
        }
    }

    VertexProgramManager::cm_listExistentVertexPrograms.Remove(this);
}
#endif
#ifndef _XBOX

char cBumpyShinyVP[] =
    "!!VP1.0 # This is the vertex program for doing the bumpy-shiny stuff.\n"
    "DP4   o[HPOS].x, c[0], v[OPOS];DP4   o[HPOS].y, c[1], v[OPOS];DP4   o[HPOS].z, c[2], v[OPOS];DP4   o"
    "[HPOS].w, c[3], v[OPOS];DP3   R1.x, c[8], v[TEX4]; DP3   R2.x, c[8], v[TEX5]; DP3   R3.x, c[8], v[TE"
    "X6]; DP3   R1.y, c[9], v[TEX4]; DP3   R2.y, c[9], v[TEX5]; DP3   R3.y, c[9], v[TEX6]; DP3   R1.z, c["
    "10], v[TEX4]; DP3   R2.z, c[10], v[TEX5]; DP3   R3.z, c[10], v[TEX6]; DP3   o[TEX1].x, c[92], R1; DP"
    "3   o[TEX1].y, c[92], R2; DP3   o[TEX1].z, c[92], R3; DP3   o[TEX2].x, c[91], R1; DP3   o[TEX2].y, c"
    "[91], R2; DP3   o[TEX2].z, c[91], R3; DP3   o[TEX3].x, c[90], R1; DP3   o[TEX3].y, c[90], R2; DP3   "
    "o[TEX3].z, c[90], R3; DP4   R4.x, v[OPOS], c[8] ;DP4   R4.y, v[OPOS], c[9] ;DP4   R4.z, v[OPOS], c[1"
    "0] ;DP4   R4.w, v[OPOS], c[11] ;DP3   R1.w, R4, R4;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;MOV o[FOGC], R1"
    ".w;RCP   R4.w, R4.w           ;MUL   R4.xyz, R4, -R4.w    ;MOV   o[TEX0], v[TEX0];DP3   o[TEX1].w, R"
    "4, c[92];DP3   o[TEX2].w, R4, c[91];DP3   o[TEX3].w, R4, c[90];MOV   o[COL0], c[15].x;";

char cBumpyShinySkinVP[] =
    "!!VP1.0 # This is the vertex program for skinned animations.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];MUL   R9, v[WGHT].x, R1;DP3   R1.x, v[TEX4], c[A0.x"
    " + 18];DP3   R1.y, v[TEX4], c[A0.x + 19];DP3   R1.z, v[TEX4], c[A0.x + 20];MUL   R4, v[WGHT].x, R1;D"
    "P3   R1.x, v[TEX5], c[A0.x + 18];DP3   R1.y, v[TEX5], c[A0.x + 19];DP3   R1.z, v[TEX5], c[A0.x + 20]"
    ";MUL   R5, v[WGHT].x, R1;DP3   R1.x, v[TEX6], c[A0.x + 18];DP3   R1.y, v[TEX6], c[A0.x + 19];DP3   R"
    "1.z, v[TEX6], c[A0.x + 20];MUL   R6, v[WGHT].x, R1;ARL   A0.x, R0.y;DP4   R1.x, v[OPOS], c[A0.x + 18"
    "];DP4   R1.y, v[OPOS], c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];MAD   R9, R1, v[WGHT].y, R9;DP"
    "3   R1.x, v[TEX4], c[A0.x + 18];DP3   R1.y, v[TEX4], c[A0.x + 19];DP3   R1.z, v[TEX4], c[A0.x + 20];"
    "MAD   R4, v[WGHT].y, R1, R4;DP3   R1.x, v[TEX5], c[A0.x + 18];DP3   R1.y, v[TEX5], c[A0.x + 19];DP3 "
    "  R1.z, v[TEX5], c[A0.x + 20];MAD   R5, v[WGHT].y, R1, R5;DP3   R1.x, v[TEX6], c[A0.x + 18];DP3   R1"
    ".y, v[TEX6], c[A0.x + 19];DP3   R1.z, v[TEX6], c[A0.x + 20];MAD   R6, v[WGHT].y, R1, R6;ARL   A0.x, "
    "R0.z;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS], c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x"
    " + 20];MAD   R9, R1, v[WGHT].z, R9;DP3   R1.x, v[TEX4], c[A0.x + 18];DP3   R1.y, v[TEX4], c[A0.x + 1"
    "9];DP3   R1.z, v[TEX4], c[A0.x + 20];MAD   R4, v[WGHT].z, R1, R4;DP3   R1.x, v[TEX5], c[A0.x + 18];D"
    "P3   R1.y, v[TEX5], c[A0.x + 19];DP3   R1.z, v[TEX5], c[A0.x + 20];MAD   R5, v[WGHT].z, R1, R5;DP3  "
    " R1.x, v[TEX6], c[A0.x + 18];DP3   R1.y, v[TEX6], c[A0.x + 19];DP3   R1.z, v[TEX6], c[A0.x + 20];MAD"
    "   R6, v[WGHT].z, R1, R6;ARL   A0.x, R0.w;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS], c[A"
    "0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];MAD   R9, R1, v[WGHT].w, R9;DP3   R1.x, v[TEX4], c[A0.x "
    "+ 18];\n"
    "DP3   R1.y, v[TEX4], c[A0.x + 19];\n"
    "DP3   R1.z, v[TEX4], c[A0.x + 20];\n"
    "MAD   R4, v[WGHT].w, R1, R4;DP3   R1.x, v[TEX5], c[A0.x + 18];\n"
    "DP3   R1.y, v[TEX5], c[A0.x + 19];\n"
    "DP3   R1.z, v[TEX5], c[A0.x + 20];\n"
    "MAD   R5, v[WGHT].w, R1, R5;DP3   R1.x, v[TEX6], c[A0.x + 18];\n"
    "DP3   R1.y, v[TEX6], c[A0.x + 19];\n"
    "DP3   R1.z, v[TEX6], c[A0.x + 20];\n"
    "MAD   R6, v[WGHT].w, R1, R5;MOV   R9.w, c[16].w;DP4   o[HPOS].x, c[0], R9;DP4   o[HPOS].y, c[1], R9;"
    "DP4   o[HPOS].z, c[2], R9;DP4   o[HPOS].w, c[3], R9;DP4   R10.x, c[8], R9;DP4   R10.y, c[9], R9;DP4 "
    "  R10.z, c[10], R9;MOV   R10.w, c[16].w;MOV   R9, c[16].xxxw;DP3   R1.x, c[8], R4; DP3   R2.x, c[8],"
    " R5; DP3   R3.x, c[8], R6; DP3   R1.y, c[9], R4; DP3   R2.y, c[9], R5; DP3   R3.y, c[9], R6; DP3   R"
    "1.z, c[10], R4; DP3   R2.z, c[10], R5; DP3   R3.z, c[10], R6; DP3   o[TEX1].x, c[92], R1; DP3   o[TE"
    "X1].y, c[92], R2; DP3   o[TEX1].z, c[92], R3; DP3   o[TEX2].x, c[91], R1; DP3   o[TEX2].y, c[91], R2"
    "; DP3   o[TEX2].z, c[91], R3; DP3   o[TEX3].x, c[90], R1; DP3   o[TEX3].y, c[90], R2; DP3   o[TEX3]."
    "z, c[90], R3; DP4   R4.x, v[OPOS], c[8] ;DP4   R4.y, v[OPOS], c[9] ;DP4   R4.z, v[OPOS], c[10] ;DP4 "
    "  R4.w, v[OPOS], c[11] ;RCP   R4.w, R4.w           ;MUL   R4.xyz, R4, -R4.w    ;DP3   R1.w, R4, R4;R"
    "SQ   R1.w, R1.w;RCP   R1.w, R1.w;MOV o[FOGC], R1.w;MOV   o[TEX0], v[TEX0];DP3   o[TEX1].w, R4, c[92]"
    ";DP3   o[TEX2].w, R4, c[91];DP3   o[TEX3].w, R4, c[90];MOV   o[COL0], c[15].x;";

char cBumpMapOnePassVP[] =
    "!!VP1.0 # This is the vertex program for transforming light/halfangle vectors into tangent space.\n"
    "DP4   o[HPOS].x, c[0], v[OPOS];DP4   o[HPOS].y, c[1], v[OPOS];DP4   o[HPOS].z, c[2], v[OPOS];DP4   o"
    "[HPOS].w, c[3], v[OPOS];ADD  R0, c[12], -v[OPOS];DP3   R0.w, R0, R0;RSQ   R1.w, R0.w;DST R2, R0.w, R"
    "1.w;DP3 R2.w, R2, c[14];RCP R2.w, R2.w;MOV   R4.xyzw, R2.wwww;MUL   R0, R0, R1.w;MOV   R0.w, c[17].x"
    ";DP3   R1.x, v[TEX4], R0;DP3   R1.y, v[TEX5], R0;DP3   R1.z, v[TEX6], R0;MOV   R1.w, c[17].x;MOV   o"
    "[TEX1], R1;ADD R2, c[13],-v[OPOS];DP3   R2.w, R2, R2;RSQ   R2.w, R2.w;MUL   R2, R2, R2.w;MOV   R2.w,"
    " c[17].x;ADD R2, R2,R0;MOV   R2.w, c[17].x;DP3   R1.x, v[TEX4], R2;DP3   R1.y, v[TEX5], R2;DP3   R1."
    "z, v[TEX6], R2;MOV   R1.w, c[17].x;MOV   o[TEX2], R1;DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v"
    "[OPOS];DP4   R10.z, c[10], v[OPOS];DP4   R10.w, c[11], v[OPOS];DP3   R11.x, c[8], v[NRML];DP3   R11."
    "y, c[9], v[NRML];DP3   R11.z, c[10], v[NRML];DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;"
    "MOV   o[FOGC], R1.w;MOV   R9, v[COL0];MOV   o[TEX0], v[TEX0];MOV   o[TEX3], v[TEX0];MOV   o[COL1], R"
    "4;";

char cBumpMapSpecularVP[] =
    "!!VP1.0 # This is the vertex program for transforming light/halfangle vectors into tangent space.\n"
    "DP4   o[HPOS].x, c[0], v[OPOS];DP4   o[HPOS].y, c[1], v[OPOS];DP4   o[HPOS].z, c[2], v[OPOS];DP4   o"
    "[HPOS].w, c[3], v[OPOS];ADD  R0, c[12], -v[OPOS];DP3   R0.w, R0, R0;RSQ   R1.w, R0.w;MUL   R0, R0, R"
    "1.w;DST R2, R0.w, R1.w;DP3 R2.w, R2, c[14];RCP R2.w, R2.w;MOV   R4.xyzw, R2.wwww;ADD R2, c[13],-v[OP"
    "OS];DP3   R2.w, R2, R2;RSQ   R2.w, R2.w;MUL   R2, R2, R2.w;MOV   R2.w, c[17].x;ADD R2, R2,R0;MOV   R"
    "2.w, c[17].x;DP3   R1.x, v[TEX4], R2;DP3   R1.y, v[TEX5], R2;DP3   R1.z, v[TEX6], R2;MOV   R1.w, c[1"
    "7].x;MOV   o[TEX1], R1;DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[O"
    "POS];DP4   R10.w, c[11], v[OPOS];DP3   R11.x, c[8], v[NRML];DP3   R11.y, c[9], v[NRML];DP3   R11.z, "
    "c[10], v[NRML];DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;MOV   o[FOGC], R1.w;MOV   o[CO"
    "L1], R4;MOV   R9, v[COL0];MOV   o[TEX0], v[TEX0];MOV   o[TEX2], v[TEX1];MOV   o[TEX3], v[TEX0];";

char cBumpMapDiffuseVP[] =
    "!!VP1.0 # This is the vertex program for transforming light/halfangle vectors into tangent space.\n"
    "DP4   o[HPOS].x, c[0], v[OPOS];DP4   o[HPOS].y, c[1], v[OPOS];DP4   o[HPOS].z, c[2], v[OPOS];DP4   o"
    "[HPOS].w, c[3], v[OPOS];ADD  R0, c[12], -v[OPOS];DP3   R0.w, R0, R0;RSQ   R1.w, R0.w;DST R2, R0.w, R"
    "1.w;DP3 R2.w, R2, c[14];RCP R2.w, R2.w;MOV   R4.xyzw, R2.wwww;MUL   R0, R0, R1.w;MOV   R0.w, c[17].x"
    ";DP3   R1.x, v[TEX4], R0;DP3   R1.y, v[TEX5], R0;DP3   R1.z, v[TEX6], R0;MOV   R1.w, c[17].x;MOV   o"
    "[TEX1], R1;DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[OPOS];DP4   R"
    "10.w, c[11], v[OPOS];DP3   R11.x, c[8], v[NRML];DP3   R11.y, c[9], v[NRML];DP3   R11.z, c[10], v[NRM"
    "L];DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;MOV o[FOGC], R1.w;MOV   o[COL1], R4;MOV   "
    "R9, v[COL0];MOV   o[TEX0], v[TEX0];MOV   o[TEX2], v[TEX1];MOV   o[TEX3], v[TEX0];";

char cBumpMapVP[] =
    "!!VP1.0 # This is the vertex program for transforming light/halfangle vectors into tangent space.\n"
    "DP4   o[HPOS].x, c[0], v[OPOS];DP4   o[HPOS].y, c[1], v[OPOS];DP4   o[HPOS].z, c[2], v[OPOS];DP4   o"
    "[HPOS].w, c[3], v[OPOS];MOV   R0, v[TEX7];DP3   R1.x, v[TEX4], R0;DP3   R1.y, v[TEX5], R0;DP3   R1.z"
    ", v[TEX6], R0;MOV   R1.w, c[17].x;MOV   o[TEX1], R1;MOV   R9, v[COL0];MOV   o[COL0], v[COL0];MOV   o"
    "[FOGC], v[FOGC];MOV   o[TEX0], v[TEX0];";

char cStandardVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4   o[HPOS].x, c[0], v[OPOS];DP4   o[HPOS].y, c[1], v[OPOS];DP4   o[HPOS].z, c[2], v[OPOS];DP4   o"
    "[HPOS].w, c[3], v[OPOS];DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v["
    "OPOS];DP4   R10.w, c[11], v[OPOS];DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;MOV   o[FOG"
    "C], R1.w;DP3   R11.x, c[8], v[NRML];DP3   R11.y, c[9], v[NRML];DP3   R11.z, c[10], v[NRML];MOV R9, c"
    "[15].wwwz;MOV   o[TEX0], v[TEX0];MOV   o[TEX1], v[TEX1];MOV   o[TEX2], v[TEX2];MOV   o[TEX3], v[TEX3"
    "];MOV o[COL0], c[15].wwwz;";

char cStandardDirtyVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4\to[HPOS].x, c[0], v[OPOS];DP4\to[HPOS].y, c[1], v[OPOS];DP4\to[HPOS].z, c[2], v[OPOS];DP4\to[HPOS].w"
    ", c[3], v[OPOS];DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[OPOS];DP"
    "4   R10.w, c[11], v[OPOS];DP4\tR11.x, R10, c[92];DP4\tR11.y, R10, C[91];DP4\tR11.z, R10, C[90];DP4\tR11."
    "w, R10, C[89];MAD\tR11, R11, C[28].x, R11;\n"
    "MAD\tR11, R11, C[28].y, R11.yzxw;\n"
    "MAD\tR11, R11, C[28].z, R11.zxyw;\n"
    "MAD\to[TEX1], R11, C[28].w, R11;\n"
    "DP3\tR1.w, R10, R10;RSQ\tR1.w, R1.w;RCP\tR1.w, R1.w;MOV\to[FOGC], R1.w;DP3\tR11.x, c[8], v[NRML];DP3\tR11."
    "y, c[9], v[NRML];DP3\tR11.z, c[10], v[NRML];MOV\tR9, c[15].wwwz;MOV   o[TEX0], v[TEX0];MOV   o[TEX2], "
    "v[TEX2];MOV   o[TEX3], v[TEX3];MOV o[COL0], c[15].wwwz;";

char cStandardDirtyLMVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4\to[HPOS].x, c[0], v[OPOS];DP4\to[HPOS].y, c[1], v[OPOS];DP4\to[HPOS].z, c[2], v[OPOS];DP4\to[HPOS].w"
    ", c[3], v[OPOS];DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[OPOS];DP"
    "4   R10.w, c[11], v[OPOS];DP4\tR11.x, R10, c[92];DP4\tR11.y, R10, C[91];DP4\tR11.z, R10, C[90];DP4\tR11."
    "w, R10, C[89];MAD\tR11, R11, C[28].x, R11;\n"
    "MAD\tR11, R11, C[28].y, R11.yzxw;\n"
    "MAD\tR11, R11, C[28].z, R11.zxyw;\n"
    "MAD\to[TEX2], R11, C[28].w, R11;\n"
    "DP3\tR1.w, R10, R10;RSQ\tR1.w, R1.w;RCP\tR1.w, R1.w;MOV\to[FOGC], R1.w;DP3\tR11.x, c[8], v[NRML];DP3\tR11."
    "y, c[9], v[NRML];DP3\tR11.z, c[10], v[NRML];MOV\tR9, c[15].wwwz;MOV   o[TEX0], v[TEX0];MOV   o[TEX1], "
    "v[TEX1];MOV   o[TEX3], v[TEX3];MOV o[COL0], c[15].wwwz;";

char cStandardDirtyEMVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4\to[HPOS].x, c[0], v[OPOS];DP4\to[HPOS].y, c[1], v[OPOS];DP4\to[HPOS].z, c[2], v[OPOS];DP4\to[HPOS].w"
    ", c[3], v[OPOS];DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[OPOS];DP"
    "4   R10.w, c[11], v[OPOS];DP4\tR11.x, R10, c[92];DP4\tR11.y, R10, C[91];DP4\tR11.z, R10, C[90];DP4\tR11."
    "w, R10, C[89];MAD\tR11, R11, C[28].x, R11;\n"
    "MAD\tR11, R11, C[28].y, R11.yzxw;\n"
    "MAD\tR11, R11, C[28].z, R11.zxyw;\n"
    "MAD\to[TEX2], R11, C[28].w, R11;\n"
    "DP3\tR1.w, R10, R10;RSQ\tR1.w, R1.w;RCP\tR1.w, R1.w;MOV\to[FOGC], R1.w;DP3\tR11.x, c[8], v[NRML];DP3\tR11."
    "y, c[9], v[NRML];DP3\tR11.z, c[10], v[NRML];MOV\tR9, c[15].wwwz;MOV   o[TEX0], v[TEX0];DP3   R0.x, -R1"
    "0, R11;MUL   R1, R0.x, R11;MUL   R2, R1, c[16].y;ADD   R3, R10, R2;DP3   R3.w, R3, R3;RSQ   R3.w, R3"
    ".w;MUL   R3, R3, R3.w;DP3   R4.x, c[92], R3;DP3   R4.y, c[91], R3;DP3   R4.z, c[90], R3;MOV   R4.w, "
    "c[16].w;MOV   o[TEX1], R4;";

char cStandardDirtyEMLMVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4\to[HPOS].x, c[0], v[OPOS];DP4\to[HPOS].y, c[1], v[OPOS];DP4\to[HPOS].z, c[2], v[OPOS];DP4\to[HPOS].w"
    ", c[3], v[OPOS];DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[OPOS];DP"
    "4   R10.w, c[11], v[OPOS];DP4\tR11.x, R10, c[92];DP4\tR11.y, R10, C[91];DP4\tR11.z, R10, C[90];DP4\tR11."
    "w, R10, C[89];MAD\tR11, R11, C[28].x, R11;\n"
    "MAD\tR11, R11, C[28].y, R11.yzxw;\n"
    "MAD\tR11, R11, C[28].z, R11.zxyw;\n"
    "MAD\to[TEX3], R11, C[28].w, R11;\n"
    "DP3\tR1.w, R10, R10;RSQ\tR1.w, R1.w;RCP\tR1.w, R1.w;MOV\to[FOGC], R1.w;DP3\tR11.x, c[8], v[NRML];DP3\tR11."
    "y, c[9], v[NRML];DP3\tR11.z, c[10], v[NRML];MOV\tR9, c[15].wwwz;MOV\to[TEX0], v[TEX0];MOV\to[TEX1], v[TE"
    "X1];DP3   R0.x, -R10, R11;MUL   R1, R0.x, R11;MUL   R2, R1, c[16].y;ADD   R3, R10, R2;DP3   R3.w, R3"
    ", R3;RSQ   R3.w, R3.w;MUL   R3, R3, R3.w;DP3   R4.x, c[92], R3;DP3   R4.y, c[91], R3;DP3   R4.z, c[9"
    "0], R3;MOV   R4.w, c[16].w;MOV   o[TEX2], R4;";

char cStandardDirtyLSVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4\to[HPOS].x, c[0], v[OPOS];DP4\to[HPOS].y, c[1], v[OPOS];DP4\to[HPOS].z, c[2], v[OPOS];DP4\to[HPOS].w"
    ", c[3], v[OPOS];DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[OPOS];DP"
    "4   R10.w, c[11], v[OPOS];MOV\tR11, v[OPOS];ADD\tR11, R11, v[TEX0];MOV\to[TEX1], R11;DP3\tR1.w, R10, R10"
    ";RSQ\tR1.w, R1.w;RCP\tR1.w, R1.w;MOV\to[FOGC], R1.w;DP3\tR11.x, c[8], v[NRML];DP3\tR11.y, c[9], v[NRML];D"
    "P3\tR11.z, c[10], v[NRML];MOV\tR9, c[15].wwwz;MOV   o[TEX0], v[TEX0];";

char cStandardDirtyLSLMVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4\to[HPOS].x, c[0], v[OPOS];DP4\to[HPOS].y, c[1], v[OPOS];DP4\to[HPOS].z, c[2], v[OPOS];DP4\to[HPOS].w"
    ", c[3], v[OPOS];DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[OPOS];DP"
    "4   R10.w, c[11], v[OPOS];MOV\tR11, v[OPOS];ADD\tR11, R11, v[TEX0];MOV\to[TEX2], R11;DP3\tR1.w, R10, R10"
    ";RSQ\tR1.w, R1.w;RCP\tR1.w, R1.w;MOV\to[FOGC], R1.w;DP3\tR11.x, c[8], v[NRML];DP3\tR11.y, c[9], v[NRML];D"
    "P3\tR11.z, c[10], v[NRML];MOV\tR9, c[15].wwwz;MOV   o[TEX0], v[TEX0];MOV   o[TEX1], v[TEX1];";

char cStandardDirtyLSEMVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4\to[HPOS].x, c[0], v[OPOS];DP4\to[HPOS].y, c[1], v[OPOS];DP4\to[HPOS].z, c[2], v[OPOS];DP4\to[HPOS].w"
    ", c[3], v[OPOS];DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[OPOS];DP"
    "4   R10.w, c[11], v[OPOS];MOV\tR11, v[OPOS];ADD\tR11, R11, v[TEX0];MOV\to[TEX2], R11;DP3\tR1.w, R10, R10"
    ";RSQ\tR1.w, R1.w;RCP\tR1.w, R1.w;MOV\to[FOGC], R1.w;DP3\tR11.x, c[8], v[NRML];DP3\tR11.y, c[9], v[NRML];D"
    "P3\tR11.z, c[10], v[NRML];MOV\tR9, c[15].wwwz;MOV   o[TEX0], v[TEX0];DP3   R0.x, -R10, R11;MUL   R1, R"
    "0.x, R11;MUL   R2, R1, c[16].y;ADD   R3, R10, R2;DP3   R3.w, R3, R3;RSQ   R3.w, R3.w;MUL   R3, R3, R"
    "3.w;DP3   R4.x, c[92], R3;DP3   R4.y, c[91], R3;DP3   R4.z, c[90], R3;MOV   R4.w, c[16].w;MOV   o[TE"
    "X1], R4;";

char cStandardDirtyLSEMLMVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4\to[HPOS].x, c[0], v[OPOS];DP4\to[HPOS].y, c[1], v[OPOS];DP4\to[HPOS].z, c[2], v[OPOS];DP4\to[HPOS].w"
    ", c[3], v[OPOS];DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[OPOS];DP"
    "4   R10.w, c[11], v[OPOS];MOV\tR11, v[OPOS];ADD\tR11, R11, v[TEX0];MOV\to[TEX3], R11;DP3\tR1.w, R10, R10"
    ";RSQ\tR1.w, R1.w;RCP\tR1.w, R1.w;MOV\to[FOGC], R1.w;DP3\tR11.x, c[8], v[NRML];DP3\tR11.y, c[9], v[NRML];D"
    "P3\tR11.z, c[10], v[NRML];MOV\tR9, c[15].wwwz;MOV   o[TEX0], v[TEX0];MOV   o[TEX1], v[TEX1];DP3   R0.x"
    ", -R10, R11;MUL   R1, R0.x, R11;MUL   R2, R1, c[16].y;ADD   R3, R10, R2;DP3   R3.w, R3, R3;RSQ   R3."
    "w, R3.w;MUL   R3, R3, R3.w;DP3   R4.x, c[92], R3;DP3   R4.y, c[91], R3;DP3   R4.z, c[90], R3;MOV   R"
    "4.w, c[16].w;MOV   o[TEX2], R4;";

char cFogTestVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4   o[HPOS].x, c[0], v[OPOS];DP4   o[HPOS].y, c[1], v[OPOS];DP4   o[HPOS].z, c[2], v[OPOS];DP4   o"
    "[HPOS].w, c[3], v[OPOS];DP4   R10.x, c[8], v[OPOS];DP4   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v["
    "OPOS];DP4   R10.w, c[11], v[OPOS];DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;ADD   R2.x,"
    " c[17].y, -c[17].x;ADD   R2.y, c[17].y, -R1.w;RCP   R2.z, R2.x;MUL   R2.w, R2.y, R2.z;MOV   o[FOGC]."
    "x, R2.w;MOV   o[COL1].xyzw, R2.wwww;MOV   R3.xyzw, c[16].yxxy;MOV   R3.z, R2.w;ADD   R3.x, R3.x, -R2"
    ".w;MOV   o[COL0], R3;";

char cLightProjectionVP[] =
    "!!VP1.0 # This is the shadow vertex program.\n"
    "MOV   R1, c[20];ADD   R2, v[OPOS], -R1;DP3   R0.w, R2, R2;RSQ   R0.w, R0.w;MUL   R2, R2, R0.w;MOV   "
    "R11, v[TEX0];SLT   R10.x, R11.x, R11.y;SGE   R10.y, R11.x, R11.y;MAD   R4, R2, c[21].x, R1;MOV   R4."
    "w, c[21].z;DP3   R3.w, R4, R4;RCP   R0.w, R0.w;SGE   R9.y, R3.w, R0.w;SLT   R9.z, R3.w, R0.w;MOV   R"
    "0, v[OPOS];MUL   R0, R0, R9.z;MAD   R4, R4, R9.y, R0;MUL   R4, R4, R10.y;MAD   R5, R2, c[21].y, v[OP"
    "OS];MOV   R5.w, c[21].z;MUL   R5, R5, R10.x;MAD   R6, R5, R10.x, R4;DP3   R7.x, R2, c[22];RCP   R7.x"
    ", R7.x;DP3   R7.y, R1, c[22];ADD   R7.z, R7.y, c[22].w;MUL   R7.w, R7.z, -R7.x;MAD   R8, R2, R7.w, R"
    "1;MOV   R8.w, c[21].z;DP3   R9.x, R6, c[22];ADD   R9.x, R9.x, c[22].w;SGE   R9.y, R9.x, c[21].w;SLT "
    "  R9.z, R9.x, c[21].w;MUL   R8, R8, R9.y;MAD   R6, R6, R9.z, R8;DP4   o[HPOS].x, c[0], R6;DP4   o[HP"
    "OS].y, c[1], R6;DP4   o[HPOS].z, c[2], R6;DP4   o[HPOS].w, c[3], R6;MOV   o[COL0], v[COL0];";

char cFogOutputVP[] =
    "!!VP1.0 # This is the fog output vertex program.\n"
    "DP4   o[HPOS].x, c[0], v[OPOS];DP4   o[HPOS].y, c[1], v[OPOS];DP4   o[HPOS].z, c[2], v[OPOS];DP4   o"
    "[HPOS].w, c[3], v[OPOS];DP4   R1.x, c[8], v[OPOS];DP4   R1.y, c[9], v[OPOS];DP4   R1.z, c[10], v[OPO"
    "S];DP4   R1.w, c[11], v[OPOS];DP3   R1.w, R1, R1;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;ADD   R2.x, c[20]"
    ".y, -c[20].x;ADD   R2.y, c[20].y, -R1.w;RCP   R2.z, R2.x;MUL   R2.w, R2.y, R2.z;MIN   R2.w, R2.w, c["
    "22].x;MAX   R2.w, R2.w, c[22].y;ADD   R2.w, c[22].x, -R2.w;MUL   R3, c[21], R2.w;MOV   R3.w, R2.w;MO"
    "V   o[COL0], R3;MOV   o[FOGC].x, c[22].y;";

char cSkinVP[] =
    "!!VP1.0 # This is the vertex program for skinned animations.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];DP3   R5.x, v[NRML], c[A0.x + 18];DP3   R5.y, v[NRM"
    "L], c[A0.x + 19];DP3   R5.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.y;DP4   R2.x, v[OPOS], c[A0.x + 18"
    "];DP4   R2.y, v[OPOS], c[A0.x + 19];DP4   R2.z, v[OPOS], c[A0.x + 20];DP3   R6.x, v[NRML], c[A0.x + "
    "18];DP3   R6.y, v[NRML], c[A0.x + 19];DP3   R6.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.z;DP4   R3.x,"
    " v[OPOS], c[A0.x + 18];DP4   R3.y, v[OPOS], c[A0.x + 19];DP4   R3.z, v[OPOS], c[A0.x + 20];DP3   R7."
    "x, v[NRML], c[A0.x + 18];DP3   R7.y, v[NRML], c[A0.x + 19];DP3   R7.z, v[NRML], c[A0.x + 20];ARL   A"
    "0.x, R0.w;DP4   R4.x, v[OPOS], c[A0.x + 18];DP4   R4.y, v[OPOS], c[A0.x + 19];DP4   R4.z, v[OPOS], c"
    "[A0.x + 20];DP3   R8.x, v[NRML], c[A0.x + 18];DP3   R8.y, v[NRML], c[A0.x + 19];DP3   R8.z, v[NRML],"
    " c[A0.x + 20];MUL   R9, v[WGHT].x, R1;MAD   R9, R2, v[WGHT].y, R9;MAD   R9, R3, v[WGHT].z, R9;MAD   "
    "R9, R4, v[WGHT].w, R9;MOV   R9.w, c[16].w;MUL   R10, v[WGHT].x, R5;MAD   R10, R6, v[WGHT].y, R10;MAD"
    "   R10, R7, v[WGHT].z, R10;MAD   R10, R8, v[WGHT].w, R10;DP4   o[HPOS].x, c[0], R9;DP4   o[HPOS].y, "
    "c[1], R9;DP4   o[HPOS].z, c[2], R9;DP4   o[HPOS].w, c[3], R9;DP3   R11.x, c[8], R10;DP3   R11.y, c[9"
    "], R10;DP3   R11.z, c[10], R10;MOV   R11.w, c[16].w;DP3   R11.w, R11, R11;RSQ   R11.w, R11.w;MUL   R"
    "11, R11, R11.w;DP4   R10.x, c[8], R9;DP4   R10.y, c[9], R9;DP4   R10.z, c[10], R9;MOV   R10.w, c[16]"
    ".w;DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;MOV o[FOGC], R1.w;MOV   R9, c[16].xxxw;MOV"
    "   o[TEX0], v[TEX0];";

char cSkinBumpedOutVP[] =
    "!!VP1.0 # This is the vertex program for skinned animations.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];DP3   R5.x, v[NRML], c[A0.x + 18];DP3   R5.y, v[NRM"
    "L], c[A0.x + 19];DP3   R5.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.y;DP4   R2.x, v[OPOS], c[A0.x + 18"
    "];DP4   R2.y, v[OPOS], c[A0.x + 19];DP4   R2.z, v[OPOS], c[A0.x + 20];DP3   R6.x, v[NRML], c[A0.x + "
    "18];DP3   R6.y, v[NRML], c[A0.x + 19];DP3   R6.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.z;DP4   R3.x,"
    " v[OPOS], c[A0.x + 18];DP4   R3.y, v[OPOS], c[A0.x + 19];DP4   R3.z, v[OPOS], c[A0.x + 20];DP3   R7."
    "x, v[NRML], c[A0.x + 18];DP3   R7.y, v[NRML], c[A0.x + 19];DP3   R7.z, v[NRML], c[A0.x + 20];ARL   A"
    "0.x, R0.w;DP4   R4.x, v[OPOS], c[A0.x + 18];DP4   R4.y, v[OPOS], c[A0.x + 19];DP4   R4.z, v[OPOS], c"
    "[A0.x + 20];DP3   R8.x, v[NRML], c[A0.x + 18];DP3   R8.y, v[NRML], c[A0.x + 19];DP3   R8.z, v[NRML],"
    " c[A0.x + 20];MUL   R9, v[WGHT].x, R1;MAD   R9, R2, v[WGHT].y, R9;MAD   R9, R3, v[WGHT].z, R9;MAD   "
    "R9, R4, v[WGHT].w, R9;MOV   R9.w, c[16].w;MUL   R10, v[WGHT].x, R5;MAD   R10, R6, v[WGHT].y, R10;MAD"
    "   R10, R7, v[WGHT].z, R10;MAD   R10, R8, v[WGHT].w, R10;MAD R9, R10, c[12].x, R9;DP4   o[HPOS].x, c"
    "[0], R9;DP4   o[HPOS].y, c[1], R9;DP4   o[HPOS].z, c[2], R9;DP4   o[HPOS].w, c[3], R9;DP3   R11.x, c"
    "[8], R10;DP3   R11.y, c[9], R10;DP3   R11.z, c[10], R10;MOV   R11.w, c[16].w;DP3   R11.w, R11, R11;R"
    "SQ   R11.w, R11.w;MUL   R11, R11, R11.w;DP4   R10.x, c[8], R9;DP4   R10.y, c[9], R9;DP4   R10.z, c[1"
    "0], R9;MOV   R10.w, c[16].w;DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;MOV o[FOGC], R1.w"
    ";MOV   R9, c[12].wwww;MOV   o[COL0].xyzw, c[12].wwww;MOV   o[TEX0], v[TEX0];";

char cSkin3BoneVP[] =
    "!!VP1.0 # This is the vertex program for skinned animations.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];DP3   R5.x, v[NRML], c[A0.x + 18];DP3   R5.y, v[NRM"
    "L], c[A0.x + 19];DP3   R5.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.y;DP4   R2.x, v[OPOS], c[A0.x + 18"
    "];DP4   R2.y, v[OPOS], c[A0.x + 19];DP4   R2.z, v[OPOS], c[A0.x + 20];DP3   R6.x, v[NRML], c[A0.x + "
    "18];DP3   R6.y, v[NRML], c[A0.x + 19];DP3   R6.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.z;DP4   R3.x,"
    " v[OPOS], c[A0.x + 18];DP4   R3.y, v[OPOS], c[A0.x + 19];DP4   R3.z, v[OPOS], c[A0.x + 20];DP3   R7."
    "x, v[NRML], c[A0.x + 18];DP3   R7.y, v[NRML], c[A0.x + 19];DP3   R7.z, v[NRML], c[A0.x + 20];ARL   A"
    "0.x, R0.w;DP4   R4.x, v[OPOS], c[A0.x + 18];DP4   R4.y, v[OPOS], c[A0.x + 19];DP4   R4.z, v[OPOS], c"
    "[A0.x + 20];DP3   R8.x, v[NRML], c[A0.x + 18];DP3   R8.y, v[NRML], c[A0.x + 19];DP3   R8.z, v[NRML],"
    " c[A0.x + 20];MUL   R9, v[WGHT].x, R1;MAD   R9, R2, v[WGHT].y, R9;MAD   R9, R3, v[WGHT].z, R9;MAD   "
    "R9, R4, v[WGHT].w, R9;MOV   R9.w, c[16].w;MUL   R10, v[WGHT].x, R5;MAD   R10, R6, v[WGHT].y, R10;MAD"
    "   R10, R7, v[WGHT].z, R10;MAD   R10, R8, v[WGHT].w, R10;DP4   o[HPOS].x, c[0], R9;DP4   o[HPOS].y, "
    "c[1], R9;DP4   o[HPOS].z, c[2], R9;DP4   o[HPOS].w, c[3], R9;DP3   R11.x, c[8], R10;DP3   R11.y, c[9"
    "], R10;DP3   R11.z, c[10], R10;MOV   R11.w, c[16].w;DP3   R11.w, R11, R11;RSQ   R11.w, R11.w;MUL   R"
    "11, R11, R11.w;DP4   R10.x, c[8], R9;DP4   R10.y, c[9], R9;DP4   R10.z, c[10], R9;MOV   R10.w, c[16]"
    ".w;DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;MOV o[FOGC], R1.w;MOV   R9, c[16].xxxw;MOV"
    "   o[TEX0], v[TEX0];";

char cSkinFogOutputVP[] =
    "!!VP1.0 # This is the vertex program for skinned animations.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];ARL   A0.x, R0.y;DP4   R2.x, v[OPOS], c[A0.x + 18];"
    "DP4   R2.y, v[OPOS], c[A0.x + 19];DP4   R2.z, v[OPOS], c[A0.x + 20];ARL   A0.x, R0.z;DP4   R3.x, v[O"
    "POS], c[A0.x + 18];DP4   R3.y, v[OPOS], c[A0.x + 19];DP4   R3.z, v[OPOS], c[A0.x + 20];ARL   A0.x, R"
    "0.w;DP4   R4.x, v[OPOS], c[A0.x + 18];DP4   R4.y, v[OPOS], c[A0.x + 19];DP4   R4.z, v[OPOS], c[A0.x "
    "+ 20];MUL   R9, v[WGHT].x, R1;MAD   R9, R2, v[WGHT].y, R9;MAD   R9, R3, v[WGHT].z, R9;MAD   R9, R4, "
    "v[WGHT].w, R9;MOV   R9.w, c[16].w;DP4   o[HPOS].x, c[0], R9;DP4   o[HPOS].y, c[1], R9;DP4   o[HPOS]."
    "z, c[2], R9;DP4   o[HPOS].w, c[3], R9;DP4   R1.x, c[8], R9;DP4   R1.y, c[9], R9;DP4   R1.z, c[10], R"
    "9;DP4   R1.w, c[11], R9;DP3   R1.w, R1, R1;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;ADD   R2.x, c[12].y, -c"
    "[12].x;ADD   R2.y, c[12].y, -R1.w;RCP   R2.z, R2.x;MUL   R2.w, R2.y, R2.z;MIN   R2.w, R2.w, c[14].x;"
    "MAX   R2.w, R2.w, c[14].y;ADD   R2.w, c[14].x, -R2.w;MUL   R3, c[13], R2.w;MOV   R3.w, R2.w;MOV   o["
    "COL0], R3;MOV   o[FOGC].x, c[14].y;";

char cSkinBumpyVP[] =
    "!!VP1.0 # This is the vertex program for skinned animations.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];DP3   R5.x, v[NRML], c[A0.x + 18];DP3   R5.y, v[NRM"
    "L], c[A0.x + 19];DP3   R5.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.y;DP4   R2.x, v[OPOS], c[A0.x + 18"
    "];DP4   R2.y, v[OPOS], c[A0.x + 19];DP4   R2.z, v[OPOS], c[A0.x + 20];DP3   R6.x, v[NRML], c[A0.x + "
    "18];DP3   R6.y, v[NRML], c[A0.x + 19];DP3   R6.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.z;DP4   R3.x,"
    " v[OPOS], c[A0.x + 18];DP4   R3.y, v[OPOS], c[A0.x + 19];DP4   R3.z, v[OPOS], c[A0.x + 20];DP3   R7."
    "x, v[NRML], c[A0.x + 18];DP3   R7.y, v[NRML], c[A0.x + 19];DP3   R7.z, v[NRML], c[A0.x + 20];ARL   A"
    "0.x, R0.w;DP4   R4.x, v[OPOS], c[A0.x + 18];DP4   R4.y, v[OPOS], c[A0.x + 19];DP4   R4.z, v[OPOS], c"
    "[A0.x + 20];DP3   R8.x, v[NRML], c[A0.x + 18];DP3   R8.y, v[NRML], c[A0.x + 19];DP3   R8.z, v[NRML],"
    " c[A0.x + 20];MUL   R9, v[WGHT].x, R1;MAD   R9, R2, v[WGHT].y, R9;MAD   R9, R3, v[WGHT].z, R9;MAD   "
    "R9, R4, v[WGHT].w, R9;MOV   R9.w, c[16].w;MUL   R10, v[WGHT].x, R5;MAD   R10, R6, v[WGHT].y, R10;MAD"
    "   R10, R7, v[WGHT].z, R10;MAD   R10, R8, v[WGHT].w, R10;DP4   o[HPOS].x, c[0], R9;DP4   o[HPOS].y, "
    "c[1], R9;DP4   o[HPOS].z, c[2], R9;DP4   o[HPOS].w, c[3], R9;DP3   R11.x, c[8], R10;DP3   R11.y, c[9"
    "], R10;DP3   R11.z, c[10], R10;MOV   R11.w, c[16].w;DP3   R11.w, R11, R11;RSQ   R11.w, R11.w;MUL   R"
    "11, R11, R11.w;DP4   R10.x, c[8], R9;DP4   R10.y, c[9], R9;DP4   R10.z, c[10], R9;MOV   R10.w, c[16]"
    ".w;ADD  R0, c[12], -R9;DP3   R0.w, R0, R0;RSQ   R1.w, R0.w;DST R2, R0.w, R1.w;DP3 R2.w, R2, c[14];RC"
    "P R2.w, R2.w;MOV   R4.xyzw, R2.wwww;MUL   R0, R0, R1.w;MOV   R0.w, c[16].w;ADD R2, c[13],-R9;DP3   R"
    "2.w, R2, R2;RSQ   R2.w, R2.w;MUL   R2, R2, R2.w;MOV   R2.w, c[16].w;ADD R2, R2,R0;MOV   R2.w, c[16]."
    "w;MOV   R9, c[16].xxxw;MOV   o[TEX0], v[TEX0];MOV   o[TEX3], v[TEX0];DP3   R1.x, v[TEX4], R0;DP3   R"
    "1.y, v[TEX5], R0;DP3   R1.z, v[TEX6], R0;MOV   R1.w, c[16].w;MOV   o[TEX1], R1;DP3   R1.x, v[TEX4], "
    "R2;DP3   R1.y, v[TEX5], R2;DP3   R1.z, v[TEX6], R2;MOV   R1.w, c[16].w;MOV   o[TEX2], R1;DP3   R1.w,"
    " R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;MOV   o[FOGC], R1.w;MOV   o[COL1], R4;";

char cNewSkinBumpyVP[] =
    "!!VP1.0 # This is the vertex program for skinned animations.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];MUL   R9, v[WGHT].x, R1;DP3   R1.x, v[TEX4], c[A0.x"
    " + 18];DP3   R1.y, v[TEX4], c[A0.x + 19];DP3   R1.z, v[TEX4], c[A0.x + 20];MUL   R4, v[WGHT].x, R1;D"
    "P3   R1.x, v[TEX5], c[A0.x + 18];DP3   R1.y, v[TEX5], c[A0.x + 19];DP3   R1.z, v[TEX5], c[A0.x + 20]"
    ";MUL   R5, v[WGHT].x, R1;DP3   R1.x, v[TEX6], c[A0.x + 18];DP3   R1.y, v[TEX6], c[A0.x + 19];DP3   R"
    "1.z, v[TEX6], c[A0.x + 20];MUL   R6, v[WGHT].x, R1;ARL   A0.x, R0.y;DP4   R1.x, v[OPOS], c[A0.x + 18"
    "];DP4   R1.y, v[OPOS], c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];MAD   R9, R1, v[WGHT].y, R9;DP"
    "3   R1.x, v[TEX4], c[A0.x + 18];DP3   R1.y, v[TEX4], c[A0.x + 19];DP3   R1.z, v[TEX4], c[A0.x + 20];"
    "MAD   R4, R1, v[WGHT].y, R4;DP3   R1.x, v[TEX5], c[A0.x + 18];DP3   R1.y, v[TEX5], c[A0.x + 19];DP3 "
    "  R1.z, v[TEX5], c[A0.x + 20];MAD   R5, R1, v[WGHT].y, R5;DP3   R1.x, v[TEX6], c[A0.x + 18];DP3   R1"
    ".y, v[TEX6], c[A0.x + 19];DP3   R1.z, v[TEX6], c[A0.x + 20];MAD   R6, R1, v[WGHT].y, R6;ARL   A0.x, "
    "R0.z;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS], c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x"
    " + 20];MAD   R9, R1, v[WGHT].z, R9;DP3   R1.x, v[TEX4], c[A0.x + 18];DP3   R1.y, v[TEX4], c[A0.x + 1"
    "9];DP3   R1.z, v[TEX4], c[A0.x + 20];MAD   R4, R1, v[WGHT].z, R4;DP3   R1.x, v[TEX5], c[A0.x + 18];D"
    "P3   R1.y, v[TEX5], c[A0.x + 19];DP3   R1.z, v[TEX5], c[A0.x + 20];MAD   R5, R1, v[WGHT].z, R5;DP3  "
    " R1.x, v[TEX6], c[A0.x + 18];DP3   R1.y, v[TEX6], c[A0.x + 19];DP3   R1.z, v[TEX6], c[A0.x + 20];MAD"
    "   R6, R1, v[WGHT].z, R6;ARL   A0.x, R0.w;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS], c[A"
    "0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];MAD   R9, R1, v[WGHT].w, R9;DP3   R1.x, v[TEX4], c[A0.x "
    "+ 18];DP3   R1.y, v[TEX4], c[A0.x + 19];DP3   R1.z, v[TEX4], c[A0.x + 20];MAD   R4, R1, v[WGHT].w, R"
    "4;DP3   R1.x, v[TEX5], c[A0.x + 18];DP3   R1.y, v[TEX5], c[A0.x + 19];DP3   R1.z, v[TEX5], c[A0.x + "
    "20];MAD   R5, R1, v[WGHT].w, R5;DP3   R1.x, v[TEX6], c[A0.x + 18];DP3   R1.y, v[TEX6], c[A0.x + 19];"
    "DP3   R1.z, v[TEX6], c[A0.x + 20];MAD   R6, R1, v[WGHT].w, R6;MOV   R9.w, c[16].w;DP4   o[HPOS].x, c"
    "[0], R9;DP4   o[HPOS].y, c[1], R9;DP4   o[HPOS].z, c[2], R9;DP4   o[HPOS].w, c[3], R9;DP4   R10.x, c"
    "[8], R9;DP4   R10.y, c[9], R9;DP4   R10.z, c[10], R9;MOV   R10.w, c[16].w;DP3   R1.w, R10, R10;RSQ  "
    " R1.w, R1.w;RCP   R1.w, R1.w;MOV   o[FOGC], R1.w;ADD  R0, c[12], -R9;DP3   R0.w, R0, R0;RSQ   R1.w, "
    "R0.w;DST R2, R0.w, R1.w;DP3 R2.w, R2, c[14];RCP R2.w, R2.w;MOV   o[COL1].xyzw, R2.wwww;MUL   R0, R0,"
    " R1.w;MOV   R0.w, c[16].w;ADD R2, c[13],-R9;DP3   R2.w, R2, R2;RSQ   R2.w, R2.w;MUL   R2, R2, R2.w;M"
    "OV   R2.w, c[16].w;ADD R2, R2,R0;MOV   R2.w, c[16].w;MOV   R9, c[16].xxxw;MOV   o[TEX0], v[TEX0];MOV"
    "   o[TEX3], v[TEX0];DP3   R1.x, R4, R0;DP3   R1.y, R5, R0;DP3   R1.z, R6, R0;MOV   R1.w, c[16].w;MOV"
    "   o[TEX1], R1;DP3   R1.x, R4, R2;DP3   R1.y, R5, R2;DP3   R1.z, R6, R2;MOV   R1.w, c[16].w;MOV   o["
    "TEX2], R1;";

char cSkinSphereMapNoTextureVP[] =
    "!!VP1.0 # This is the vertex program for skinned animations.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];DP3   R5.x, v[NRML], c[A0.x + 18];DP3   R5.y, v[NRM"
    "L], c[A0.x + 19];DP3   R5.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.y;DP4   R2.x, v[OPOS], c[A0.x + 18"
    "];DP4   R2.y, v[OPOS], c[A0.x + 19];DP4   R2.z, v[OPOS], c[A0.x + 20];DP3   R6.x, v[NRML], c[A0.x + "
    "18];DP3   R6.y, v[NRML], c[A0.x + 19];DP3   R6.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.z;DP4   R3.x,"
    " v[OPOS], c[A0.x + 18];DP4   R3.y, v[OPOS], c[A0.x + 19];DP4   R3.z, v[OPOS], c[A0.x + 20];DP3   R7."
    "x, v[NRML], c[A0.x + 18];DP3   R7.y, v[NRML], c[A0.x + 19];DP3   R7.z, v[NRML], c[A0.x + 20];ARL   A"
    "0.x, R0.w;DP4   R4.x, v[OPOS], c[A0.x + 18];DP4   R4.y, v[OPOS], c[A0.x + 19];DP4   R4.z, v[OPOS], c"
    "[A0.x + 20];DP3   R8.x, v[NRML], c[A0.x + 18];DP3   R8.y, v[NRML], c[A0.x + 19];DP3   R8.z, v[NRML],"
    " c[A0.x + 20];MUL   R9, v[WGHT].x, R1;MAD   R9, R2, v[WGHT].y, R9;MAD   R9, R3, v[WGHT].z, R9;MAD   "
    "R9, R4, v[WGHT].w, R9;MOV   R9.w, c[16].w;MUL   R10, v[WGHT].x, R5;MAD   R10, R6, v[WGHT].y, R10;MAD"
    "   R10, R7, v[WGHT].z, R10;MAD   R10, R8, v[WGHT].w, R10;DP4   o[HPOS].x, c[0], R9;DP4   o[HPOS].y, "
    "c[1], R9;DP4   o[HPOS].z, c[2], R9;DP4   o[HPOS].w, c[3], R9;DP3   R11.x, c[8], R10;DP3   R11.y, c[9"
    "], R10;DP3   R11.z, c[10], R10;MOV   R11.w, c[16].w;DP3   R11.w, R11, R11;RSQ   R11.w, R11.w;MUL   R"
    "11, R11, R11.w;DP4   R10.x, c[8], R9;DP4   R10.y, c[9], R9;DP4   R10.z, c[10], R9;MOV   R10.w, c[16]"
    ".w;MOV   R9, c[16].xxxw;";

char cSkinSphereMapVP[] =
    "!!VP1.0 # This is the vertex program for skinned animations.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];DP3   R5.x, v[NRML], c[A0.x + 18];DP3   R5.y, v[NRM"
    "L], c[A0.x + 19];DP3   R5.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.y;DP4   R2.x, v[OPOS], c[A0.x + 18"
    "];DP4   R2.y, v[OPOS], c[A0.x + 19];DP4   R2.z, v[OPOS], c[A0.x + 20];DP3   R6.x, v[NRML], c[A0.x + "
    "18];DP3   R6.y, v[NRML], c[A0.x + 19];DP3   R6.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.z;DP4   R3.x,"
    " v[OPOS], c[A0.x + 18];DP4   R3.y, v[OPOS], c[A0.x + 19];DP4   R3.z, v[OPOS], c[A0.x + 20];DP3   R7."
    "x, v[NRML], c[A0.x + 18];DP3   R7.y, v[NRML], c[A0.x + 19];DP3   R7.z, v[NRML], c[A0.x + 20];ARL   A"
    "0.x, R0.w;DP4   R4.x, v[OPOS], c[A0.x + 18];DP4   R4.y, v[OPOS], c[A0.x + 19];DP4   R4.z, v[OPOS], c"
    "[A0.x + 20];DP3   R8.x, v[NRML], c[A0.x + 18];DP3   R8.y, v[NRML], c[A0.x + 19];DP3   R8.z, v[NRML],"
    " c[A0.x + 20];MUL   R9, v[WGHT].x, R1;MAD   R9, R2, v[WGHT].y, R9;MAD   R9, R3, v[WGHT].z, R9;MAD   "
    "R9, R4, v[WGHT].w, R9;MOV   R9.w, c[16].w;MUL   R10, v[WGHT].x, R5;MAD   R10, R6, v[WGHT].y, R10;MAD"
    "   R10, R7, v[WGHT].z, R10;MAD   R10, R8, v[WGHT].w, R10;DP4   o[HPOS].x, c[0], R9;DP4   o[HPOS].y, "
    "c[1], R9;DP4   o[HPOS].z, c[2], R9;DP4   o[HPOS].w, c[3], R9;DP3   R11.x, c[8], R10;DP3   R11.y, c[9"
    "], R10;DP3   R11.z, c[10], R10;MOV   R11.w, c[16].w;DP3   R11.w, R11, R11;RSQ   R11.w, R11.w;MUL   R"
    "11, R11, R11.w;DP4   R10.x, c[8], R9;DP4   R10.y, c[9], R9;DP4   R10.z, c[10], R9;MOV   R10.w, c[16]"
    ".w;MOV   R9, c[16].xxxw;MOV   o[TEX0], v[TEX0];DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1."
    "w;MOV o[FOGC].x, R1.w;DP3   R0.x, -R10, R11;MUL   R1, R0.x, R11;MUL   R2, R1, c[16].y;ADD   R4, R10,"
    " R2;ADD   R4.z, R4.z, c[16].w;DP3   R5.x, R4, R4;RSQ   R5.y, R5.x;RCP   R5.z, R5.y;MUL   R5.w, R5.z,"
    " c[16].y;RCP   R5.x, R5.w;MOV   R6.xyzw, c[16].xxxw;MAD   R6.x, R3.x, R5.x, c[17].w;MAD   R6.y, R3.y"
    ", R5.x, c[17].w;MOV\to[TEX1], R6;";

char cSkinCubeMapVP[] =
    "!!VP1.0 # This is the vertex program for skinned animations.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];DP3   R5.x, v[NRML], c[A0.x + 18];DP3   R5.y, v[NRM"
    "L], c[A0.x + 19];DP3   R5.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.y;DP4   R2.x, v[OPOS], c[A0.x + 18"
    "];DP4   R2.y, v[OPOS], c[A0.x + 19];DP4   R2.z, v[OPOS], c[A0.x + 20];DP3   R6.x, v[NRML], c[A0.x + "
    "18];DP3   R6.y, v[NRML], c[A0.x + 19];DP3   R6.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.z;DP4   R3.x,"
    " v[OPOS], c[A0.x + 18];DP4   R3.y, v[OPOS], c[A0.x + 19];DP4   R3.z, v[OPOS], c[A0.x + 20];DP3   R7."
    "x, v[NRML], c[A0.x + 18];DP3   R7.y, v[NRML], c[A0.x + 19];DP3   R7.z, v[NRML], c[A0.x + 20];ARL   A"
    "0.x, R0.w;DP4   R4.x, v[OPOS], c[A0.x + 18];DP4   R4.y, v[OPOS], c[A0.x + 19];DP4   R4.z, v[OPOS], c"
    "[A0.x + 20];DP3   R8.x, v[NRML], c[A0.x + 18];DP3   R8.y, v[NRML], c[A0.x + 19];DP3   R8.z, v[NRML],"
    " c[A0.x + 20];MUL   R9, v[WGHT].x, R1;MAD   R9, R2, v[WGHT].y, R9;MAD   R9, R3, v[WGHT].z, R9;MAD   "
    "R9, R4, v[WGHT].w, R9;MOV   R9.w, c[16].w;MUL   R10, v[WGHT].x, R5;MAD   R10, R6, v[WGHT].y, R10;MAD"
    "   R10, R7, v[WGHT].z, R10;MAD   R10, R8, v[WGHT].w, R10;DP4   o[HPOS].x, c[0], R9;DP4   o[HPOS].y, "
    "c[1], R9;DP4   o[HPOS].z, c[2], R9;DP4   o[HPOS].w, c[3], R9;DP3   R11.x, c[8], R10;DP3   R11.y, c[9"
    "], R10;DP3   R11.z, c[10], R10;MOV   R11.w, c[16].w;DP3   R11.w, R11, R11;RSQ   R11.w, R11.w;MUL   R"
    "11, R11, R11.w;DP4   R10.x, c[8], R9;DP4   R10.y, c[9], R9;DP4   R10.z, c[10], R9;MOV   R10.w, c[16]"
    ".w;DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1.w, R1.w;MOV o[FOGC], R1.w;MOV   R9, c[16].xxxw;MOV"
    "   o[TEX0], v[TEX0];DP3   R0.x, -R10, R11;MUL   R1, R0.x, R11;MUL   R2, R1, c[16].y;ADD   R3, R10, R"
    "2;DP3   R3.w, R3, R3;RSQ   R3.w, R3.w;MUL   R3, R3, R3.w;DP3   R4.x, c[92], R3;DP3   R4.y, c[91], R3"
    ";DP3   R4.z, c[90], R3;MOV   R4.w, c[16].w;MOV   o[TEX1], R4;";

char cMotionBlurredVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4   R9.x,\tc[0],\tv[OPOS];DP4   R9.y,\tc[1],\tv[OPOS];DP4   R9.z,\tc[2],\tv[OPOS];DP4   R9.w,\tc[3],\tv[OP"
    "OS];DP4   R10.x, c[8],\tv[OPOS];DP4   R10.y, c[9],\tv[OPOS];DP4   R10.z, c[10],\tv[OPOS];DP4   R10.w, c"
    "[11],\tv[OPOS];DP3   R1.w,\tR10,\tR10;RSQ   R1.w,\tR1.w;RCP   R1.w,\tR1.w;MOV   o[FOGC].x, -R1.w;DP3   R1"
    "1.x,\tc[8],  v[NRML];DP3   R11.y, c[9],  v[NRML];DP3   R11.z, c[10], v[NRML];MOV   R4, c[50];DP3   R6"
    ".x,\t R4, R4;RSQ   R6.y,\t R6.x;MUL   R4.xyz, R4, R6.y;MOV   R7.x, c[51].x;MIN   R2.y, R7.x, c[51].y;M"
    "UL   R5.xyz, R4, R2.y;MOV   R5.w, c[17].x;DP3   R3.w, R4, v[NRML];SLT   R2.w, R3.w, c[17].x;MOV   R1"
    ", c[53].xyzw;MOV   R1.z, c[17].x;MAD   R6, R2.w, -R5, v[OPOS].xyzw;MUL   R7.y, R2.y, c[13].x;RCP   R"
    "8.y, c[51].y;MUL   R8.w, R2.y, R8.y;MUL   R7.y, R7.y, R8.w;MUL   R7.y, R7.y, c[52].w;MOV   R3.w, c[1"
    "7].x;MOV   R3.xyz, c[52];DP3   R3.w, R3, R3;RSQ   R5.w, R3.w;MUL   R3,   R3, R5.w;ADD   R5.xyz, R6, "
    "-R1;DP3   R5.w, R5, R3;SLT   R5.x, R5.w, c[17].x;MOV   R7.x, c[17].w;MAD   R5.y, R5.x, -c[17].y, R7."
    "x;MUL   R5.z, R5.y, R7.y;MUL   R5.z, R5.z, R2.w;MAD   R6, R5.z, R3, R6;ADD   R2.x, -R2.w, c[17].w;MU"
    "L   R1, v[NRML], -c[51].z;MOV   R1.w, c[17].x;MAD   R5, R2.x, R1, R6;DP4   o[HPOS].x, R5, c[0];DP4  "
    " o[HPOS].y, R5, c[1];DP4   o[HPOS].z, R5, c[2];DP4   o[HPOS].w, R5, c[3];ADD   R2.w, c[17].w, -R2.w;"
    "MOV R9.xyz, c[17].xxxx;MUL R9.w, c[17].z, R2.w;MOV o[COL0], R9;MOV   o[TEX0], v[TEX0];";

char cSkinMotionBlurVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "MUL   R0, v[COL1], c[16].zzzz;ARL   A0.x, R0.x;DP4   R1.x, v[OPOS], c[A0.x + 18];DP4   R1.y, v[OPOS]"
    ", c[A0.x + 19];DP4   R1.z, v[OPOS], c[A0.x + 20];DP3   R5.x, v[NRML], c[A0.x + 18];DP3   R5.y, v[NRM"
    "L], c[A0.x + 19];DP3   R5.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.y;DP4   R2.x, v[OPOS], c[A0.x + 18"
    "];DP4   R2.y, v[OPOS], c[A0.x + 19];DP4   R2.z, v[OPOS], c[A0.x + 20];DP3   R6.x, v[NRML], c[A0.x + "
    "18];DP3   R6.y, v[NRML], c[A0.x + 19];DP3   R6.z, v[NRML], c[A0.x + 20];ARL   A0.x, R0.z;DP4   R3.x,"
    " v[OPOS], c[A0.x + 18];DP4   R3.y, v[OPOS], c[A0.x + 19];DP4   R3.z, v[OPOS], c[A0.x + 20];DP3   R7."
    "x, v[NRML], c[A0.x + 18];DP3   R7.y, v[NRML], c[A0.x + 19];DP3   R7.z, v[NRML], c[A0.x + 20];ARL   A"
    "0.x, R0.w;DP4   R4.x, v[OPOS], c[A0.x + 18];DP4   R4.y, v[OPOS], c[A0.x + 19];DP4   R4.z, v[OPOS], c"
    "[A0.x + 20];DP3   R8.x, v[NRML], c[A0.x + 18];DP3   R8.y, v[NRML], c[A0.x + 19];DP3   R8.z, v[NRML],"
    " c[A0.x + 20];MUL   R9, v[WGHT].x, R1;MAD   R9, R2, v[WGHT].y, R9;MAD   R9, R3, v[WGHT].z, R9;MAD   "
    "R9, R4, v[WGHT].w, R9;MOV   R9.w, c[16].w;MUL   R10, v[WGHT].x, R5;MAD   R10, R6, v[WGHT].y, R10;MAD"
    "   R10, R7, v[WGHT].z, R10;MAD   R10, R8, v[WGHT].w, R10;DP3   R10.w, R10, R10;RSQ   R10.w, R10.w;MU"
    "L   R10,   R10, R10.w;MOV   R10.w, c[16].x;MOV  R4, c[12];MOV   R7.x, c[14].x;MIN   R2.y, R7.x, c[14"
    "].y;MUL   R5.xyz, R4, R2.y;MOV   R5.w, c[16].x;DP3   R3.w, R4, R10;SLT   R2.w, R3.w, c[16].x;MOV   R"
    "1, c[17].xyzw;MOV   R1.w, c[16].x;MAD   R6, R2.w, -R5, R9.xyzw;MUL   R7.y, R2.y, c[13].x;RCP   R8.y,"
    " c[14].y;MUL   R8.w, R2.y, R8.y;MUL   R7.y, R7.y, R8.w;MUL   R7.y, R7.y, c[17].w;MOV   R3.w,   c[16]"
    ".x;MOV   R3.xyz, c[69];DP3   R3.w,   R3, R3;RSQ   R5.w,   R3.w;MUL   R3,     R3, R5.w;ADD   R5.xyz, "
    "R6, -R1;DP3   R5.w, R5, R3;SLT   R5.x, R5.w, c[16].x;MOV   R7.x, c[16].w;MAD   R5.y, R5.x, -c[16].y,"
    " R7.x;MUL   R5.z, R5.y, R7.y;MUL   R5.z, R5.z, R2.w;MAD   R6, R5.z, R3, R6;ADD   R2.x, -R2.w, c[16]."
    "w;MUL   R1, R10, -c[14].z;MOV   R1.w, c[16].x;MAD   R5, R2.x, R1, R6;DP4   R1.x, R5, c[0];DP4   R1.y"
    ", R5, c[1];DP4   R1.z, R5, c[2];DP4   R1.w, R5, c[3];MOV   o[HPOS].xyzw, R1.xyzw;ADD   R2.w, c[16].w"
    ", -R2.w;MOV   R9.xyz, c[16].xxxx;MUL   R9.w, c[69].w, R2.w;MOV   o[COL0], R9;DP3   R11.x, c[8],  R10"
    ";DP3   R11.y, c[9],  R10;DP3   R11.z, c[10], R10;MOV   R11.w, c[16].w;DP4   R10.x, c[8],  R5;DP4   R"
    "10.y, c[9],  R5;DP4   R10.z, c[10], R5;MOV   R10.w, c[16].w;DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RC"
    "P   R1.w, R1.w;MOV   o[FOGC].x, -R1.w;MOV   o[TEX0], v[TEX0];";

char cBlurVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4   R0.x, c[16], v[OPOS];DP4   R0.y, c[17], v[OPOS];DP4   R0.z, c[18], v[OPOS];DP4   R0.w, c[19], "
    "v[OPOS];DP4   R1.x, c[8], R0.x;DP4   R1.y, c[9], R0.y;DP4   R1.z, c[10], R0.z;DP4   R2.x, c[0], R0.x"
    ";DP4   R2.y, c[1], R0.y;DP4   R2.z, c[2], R0.z;DP4   R2.w, c[3], R0.w;DP4   R10.x, c[8], v[OPOS];DP4"
    "   R10.y, c[9], v[OPOS];DP4   R10.z, c[10], v[OPOS];DP4   R3.x, c[0], v[OPOS];DP4   R3.y, c[1], v[OP"
    "OS];DP4   R3.z, c[2], v[OPOS];DP4   R3.w, c[3], v[OPOS];ADD   R4.xyz, R10, -R1;DP3   R11.x, c[8], v["
    "NRML];DP3   R11.y, c[9], v[NRML];DP3   R11.z, c[10], v[NRML];DP3   R5.x, R4, R11;SLT   R6.x, R5.x, c"
    "[19].x;SGE   R6.y, R5.x, c[19].x;MOV   o[HPOS], R3;MOV   R4.w, c[19].w;MOV   o[COL0].xyzw, R4.xyzw;";

char cBumpedOutVP[] =
    "!!VP1.0 # This is the bumped-out vertex program.\n"
    "MOV\tR1, v[OPOS];MOV\tR2, v[NRML];MAD R1, R2, c[12].xxxx, R1;DP4   o[HPOS].x, c[0], R1;DP4   o[HPOS].y"
    ", c[1], R1;DP4   o[HPOS].z, c[2], R1;DP4   o[HPOS].w, c[3], R1;DP4   R10.x, c[8], R1;DP4   R10.y, c["
    "9], R1;DP4   R10.z, c[10], R1;DP4   R10.w, c[11], R1;DP3   R1.w, R10, R10;RSQ   R1.w, R1.w;RCP   R1."
    "w, R1.w;MOV   o[FOGC], R1.w;DP3   R11.x, c[8], v[NRML];DP3   R11.y, c[9], v[NRML];DP3   R11.z, c[10]"
    ", v[NRML];MOV   R9, v[COL0];MOV   o[TEX0], v[TEX0];MOV   o[TEX1], v[TEX1];MOV o[COL0], c[12].wwww;";

char cPhongVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "DP4   o[HPOS].x, c[0], v[OPOS];\n"
    "DP4   o[HPOS].y, c[1], v[OPOS];\n"
    "DP4   o[HPOS].z, c[2], v[OPOS];\n"
    "DP4   o[HPOS].w, c[3], v[OPOS];\n"
    "DP4   R10.x, c[8], v[OPOS];\n"
    "DP4   R10.y, c[9], v[OPOS];\n"
    "DP4   R10.z, c[10], v[OPOS];\n"
    "DP4   R10.w, c[11], v[OPOS];\n"
    "DP3   R1.w, R10, R10;\n"
    "RSQ   R1.w, R1.w;\n"
    "RCP   R1.w, R1.w;\n"
    "MOV  o[FOGC].x, -R1.w;\n"
    "DP3   R11.x, c[8], v[NRML];\n"
    "DP3   R11.y, c[9], v[NRML];\n"
    "DP3   R11.z, c[10], v[NRML];\n"
    "MOV  o[TEX3], v[NRML];\n"
    "ADD  R0, c[12], -v[OPOS];\n"
    "MOV o[TEX2], R0;\n"
    "RCP R2, c[14].y;\n"
    "MUL R3, R0, R2.y;\n"
    "MUL R3, c[15].x, R3;\n"
    "ADD R3.xy, c[15].y, R3;\n"
    "DP3 R4.x, R3, R3;\n"
    "MUL R5, R3, c[15].wwyy;\n"
    "DP3 R5.x, R5, R5;\n"
    "RCP R5.x, R5.x;\n"
    "MUL R4.x, R4.x, R5.x;\n"
    "MUL R3, R3, R4.x;\n"
    "MOV o[TEX1], R3;\n"
    "MOV o[TEX0], v[TEX0];\n";

char cShadowTriMeshVariantVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "SGE  R9.w, v[OPOS].z, c[15].w;\n"
    "SLT  R9.z, v[OPOS].z, c[15].w;\n"
    "DP3 R6.w, c[14], -v[NRML];\n"
    "SGE R9.y, R6.w, c[12].y;\n"
    "ADD R9.w, R9.y, R9.w;\n"
    "SGE R9.w, R9.w, c[12].x;\n"
    "SLT  R9.z, R9.w, c[12].x;\n"
    "MOV R8, -c[14];\n"
    "MOV R10, v[OPOS];\n"
    "ADD R10, R10, c[17];\n"
    "DP4   R3.x, c[0], R10;\n"
    "DP4   R3.y, c[1], R10;\n"
    "DP4   R3.z, c[2], R10;\n"
    "DP4   R3.w, c[3], R10;\n"
    "DP4   R7.x, c[0], R8;\n"
    "DP4   R7.y, c[1], R8;\n"
    "DP4   R7.z, c[2], R8;\n"
    "DP4   R7 .w, c[3], R8;\n"
    "MUL R7, R7, R9.w;\n"
    "MUL R3, R3, R9.z;\n"
    "ADD O[HPOS], R3, R7;\n"
    "MOV o[COL0], c[12].yxyx;\n";

char cSkinShadowVariantVP[] =
    "!!VP1.0 # This is the standard vertex program.\n"
    "MUL   R0, o[COL1], c[16].zzzz;\n"
    "MOV   A0.x, R0.x;\n"
    "DP4   R1.x, v[OPOS], c[A0.x + 18];\n"
    "DP4   R1.y, v[OPOS], c[A0.x + 19];\n"
    "DP4   R1.z, v[OPOS], c[A0.x + 20];\n"
    "DP3   R5.x, v[NRML], c[A0.x + 18];\n"
    "DP3   R5.y, v[NRML], c[A0.x + 19];\n"
    "DP3   R5.z, v[NRML], c[A0.x + 20];\n"
    "MOV   A0.x, R0.y;\n"
    "DP4   R2.x, v[OPOS], c[a0.x + 18];\n"
    "DP4   R2.y, v[OPOS], c[a0.x + 19];\n"
    "DP4   R2.z, v[OPOS], c[a0.x + 20];\n"
    "DP3   R6.x, v[NRML], c[A0.x + 18];\n"
    "DP3   R6.y, v[NRML], c[A0.x + 19];\n"
    "DP3   R6.z, v[NRML], c[A0.x + 20];\n"
    "MOV   A0.x, R0.z;\n"
    "DP4   R3.x, v[OPOS], c[A0.x + 18];\n"
    "DP4   R3.y, v[OPOS], c[A0.x + 19];\n"
    "DP4   R3.z, v[OPOS], c[A0.x + 20];\n"
    "DP3   R7.x, v[NRML], c[A0.x + 18];\n"
    "DP3   R7.y, v[NRML], c[A0.x + 19];\n"
    "DP3   R7.z, v[NRML], c[A0.x + 20];\n"
    "MOV   A0.x, R0.w;\n"
    "DP4   R4.x, v[OPOS], c[A0.x + 18];\n"
    "DP4   R4.y, v[OPOS], c[A0.x + 19];\n"
    "DP4   R4.z, v[OPOS], c[A0.x + 20];\n"
    "DP3   R8.x, v[NRML], c[A0.x + 18];\n"
    "DP3   R8.y, v[NRML], c[A0.x + 19];\n"
    "DP3   R8.z, v[NRML], c[A0.x + 20];\n"
    "MUL   R9, v[WGHT].x, R1.xyz;\n"
    "MAD   R9, R2.xyz, v[WGHT].y, R9;\n"
    "MAD   R9, R3.xyz, v[WGHT].z, R9;\n"
    "MAD   R9, R4.xyz, v[WGHT].w, R9;\n"
    "MOV   R9.w, c[16].w;\n"
    "MUL  R10, v[WGHT].xxx, R5.xyz;\n"
    "MAD  R10, R6.xyz, v[WGHT].y, R10;\n"
    "MAD  R10, R7.xyz, v[WGHT].z, R10;\n"
    "MAD  R10, R8.xyz, v[WGHT].w, R10;\n"
    "ADD R9, R9, c[17];\n"
    "DP4   R3.x, c[0], R9;\n"
    "DP4   R3.y, c[1], R9;\n"
    "DP4   R3.z, c[2], R9;\n"
    "DP4   R3.w, c[3], R9;\n"
    "SGE  R8.w, v[OPOS].z, c[15].w;\n"
    "SLT  R8.z, v[OPOS].z, c[15].w;\n"
    "DP3 R6.w, c[14], -R10;\n"
    "SGE R8.y, R6.w, c[12].y;\n"
    "ADD R8.w, R8.y, R8.w;\n"
    "SGE R8.w, R8.w, c[12].x;\n"
    "SLT R8.z, R8.w, c[12].x;\n"
    "MOV R6, -c[14];\n"
    "DP4   R7.x, c[0], R6;\n"
    "DP4   R7.y, c[1], R6;\n"
    "DP4   R7.z, c[2], R6;\n"
    "DP4   R7.w, c[3], R6;\n"
    "MUL R7, R7, R8.w;\n"
    "MUL R3, R3, R8.z;\n"
    "ADD v[HPOS], R7, R3;\n"
    "MOV o[COL0], c[12].xyyx;\n";
#else

unsigned long cBumpyShinyLightMappedVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(8, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

unsigned long cBumpyShinyRegularVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

#include "vsh/BumpyShinyVP.h"

unsigned long cBumpMapVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(8, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

#include "vsh/BumpMapVP.h"

unsigned long cStandardVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

unsigned long cStandardVP2Declaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(8, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

unsigned long cEmitterVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(5, D3DVSDT_D3DCOLOR),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

unsigned long cMotionBlurredVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

#include "vsh/StandardVP.h"
#include "vsh/StandardLitVP.h"

#include "vsh/StandardDirtyVP.h"
#include "vsh/StandardDirtyEMVP.h"
#include "vsh/StandardDirtyEMLitVP.h"
#include "vsh/StandardDirtyEMLMVP.h"
#include "vsh/StandardDirtyEMLMLitVP.h"
#include "vsh/StandardDirtyLitVP.h"
#include "vsh/StandardDirtyLMVP.h"
#include "vsh/StandardDirtyLMLitVP.h"
#include "vsh/StandardDirtyLSVP.h"
#include "vsh/StandardDirtyLSEMVP.h"
#include "vsh/StandardDirtyLSEMLitVP.h"
#include "vsh/StandardDirtyLSEMLMVP.h"
#include "vsh/StandardDirtyLSEMLMLitVP.h"
#include "vsh/StandardDirtyLSLitVP.h"
#include "vsh/StandardDirtyLSLMVP.h"
#include "vsh/StandardDirtyLSLMLitVP.h"

#include "vsh/EmitterVP.h"

#include "vsh/MotionBlurredVP.h"
#include "vsh/MotionBlurredLitVP.h"

unsigned long cBumpedOutVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

#include "vsh/BumpedOutVP.h"
#include "vsh/BumpedOutLitVP.h"

unsigned long cFogTestVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_END()
};

#include "vsh/FogTestVP.h"
#include "vsh/HeatDistortionVP.h"
#include "vsh/DreamDistortionVP.h"

unsigned long cLightProjectionVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

#include "vsh/LightProjectionVP.h"

unsigned long cFogOutputVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_END()
};

#include "vsh/FogOutputVP.h"

unsigned long cBumpyShinySkinVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(1, D3DVSDT_FLOAT4),
    D3DVSD_REG(2, D3DVSDT_SHORT4),
    D3DVSD_END()
};

#include "vsh/BumpyShinySkinVP.h"

unsigned long cSkinVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(1, D3DVSDT_FLOAT4),
    D3DVSD_REG(2, D3DVSDT_SHORT4),
    D3DVSD_END()
};

#include "vsh/SkinVP.h"
#include "vsh/SkinLitVP.h"
#include "vsh/HoloVP.h"
#include "vsh/HoloLitVP.h"
#include "vsh/HoloSkinVP.h"
#include "vsh/HoloSkinLitVP.h"

unsigned long cSkinBumpedOutVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(1, D3DVSDT_FLOAT4),
    D3DVSD_REG(2, D3DVSDT_SHORT4),
    D3DVSD_END()
};

#include "vsh/SkinBumpedOutVP.h"
#include "vsh/SkinBumpedOutLitVP.h"

unsigned long cSkinMotionBlurVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(1, D3DVSDT_FLOAT4),
    D3DVSD_REG(2, D3DVSDT_SHORT4),
    D3DVSD_END()
};

#include "vsh/SkinMotionBlurVP.h"
#include "vsh/SkinMotionBlurLitVP.h"

unsigned long cSkin3BoneVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(1, D3DVSDT_FLOAT4),
    D3DVSD_REG(2, D3DVSDT_SHORT4),
    D3DVSD_END()
};

#include "vsh/Skin3BoneVP.h"
#include "vsh/Skin3BoneLitVP.h"

unsigned long cSkinBumpyVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(1, D3DVSDT_FLOAT4),
    D3DVSD_REG(2, D3DVSDT_SHORT4),
    D3DVSD_END()
};

#include "vsh/SkinBumpyVP.h"
#include "vsh/SkinBumpyLitVP.h"

#include "vsh/NewSkinBumpyVP.h"

unsigned long cSkinSphereMapVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(1, D3DVSDT_FLOAT4),
    D3DVSD_REG(2, D3DVSDT_SHORT4),
    D3DVSD_END()
};

#include "vsh/SkinSphereMapVP.h"
#include "vsh/SkinSphereMapLitVP.h"

unsigned long cSkinCubeMapVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(1, D3DVSDT_FLOAT4),
    D3DVSD_REG(2, D3DVSDT_SHORT4),
    D3DVSD_END()
};

#include "vsh/SkinCubeMapVP.h"
#include "vsh/SkinCubeMapLitVP.h"

unsigned long cBumpMapOnePassVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

#include "vsh/BumpMapOnePassVP.h"
#include "vsh/BumpMapOnePassLitVP.h"

unsigned long cBumpMapDiffuseVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(8, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

#include "vsh/BumpMapDiffuseVP.h"
#include "vsh/BumpMapDiffuseLitVP.h"

unsigned long cBumpMapSpecularVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(8, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

#include "vsh/BumpMapSpecularVP.h"
#include "vsh/BumpMapSpecularLitVP.h"

unsigned long cSkinFogOutputVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(1, D3DVSDT_FLOAT4),
    D3DVSD_REG(2, D3DVSDT_SHORT4),
    D3DVSD_END()
};

char cSkinFogOutputVP[] =
    "vs.1.1\n"
    "mul r0, v2, c[16].zzzz;\n"
    "arl a0.x, r0.x;\n"
    "dp4 r1.x, v0, c[a0.x+18];\n"
    "dp4 r1.y, v0, c[a0.x+19];\n"
    "dp4 r1.z, v0, c[a0.x+20];\n"
    "arl a0.x, r0.y;\n"
    "dp4 r2.x, v0, c[a0.x+18];\n"
    "dp4 r2.y, v0, c[a0.x+19];\n"
    "dp4 r2.z, v0, c[a0.x+20];\n"
    "arl a0.x, r0.z;\n"
    "dp4 r3.x, v0, c[a0.x+18];\n"
    "dp4 r3.y, v0, c[a0.x+19];\n"
    "dp4 r3.z, v0, c[a0.x+20];\n"
    "arl a0.x, r0.w;\n"
    "dp4 r4.x, v0, c[a0.x+18];\n"
    "dp4 r4.y, v0, c[a0.x+19];\n"
    "dp4 r4.z, v0, c[a0.x+20];\n"
    "mul r9, v1.x, r1;\n"
    "mad r9, r2, v1.y, r9;\n"
    "mad r9, r3, v1.z, r9;\n"
    "mad r9, r4, v1.w, r9;\n"
    "mov r9.w, c[16].w;\n"
    "dp4 oPos.x, c[0], r9;\n"
    "dp4 oPos.y, c[1], r9;\n"
    "dp4 oPos.z, c[2], r9;\n"
    "dp4 oPos.w, c[3], r9;\n"
    "dp4 r1.x, c[8], r9;\n"
    "dp4 r1.y, c[9], r9;\n"
    "dp4 r1.z, c[10], r9;\n"
    "dp4 r1.w, c[11], r9;\n"
    "dp3 r1.w, r1, r1;\n"
    "rsq r1.w, r1.w;\n"
    "rcp r1.w, r1.w;\n"
    "add r2.x, c[12].y, -c[12].x;\n"
    "add r2.y, c[12].y, -r1.w;\n"
    "rcp r2.z, r2.x;\n"
    "mul r2.w, r2.y, r2.z;\n"
    "min r2.w, r2.w, c[14].x;\n"
    "max r2.w, r2.w, c[14].y;\n"
    "add r2.w, c[14].x, -r2.w;\n"
    "mul r3, c[13], r2.w;\n"
    "mov r3.w, r2.w;\n"
    "mov oD0, r3;\n"
    "mov oFog, -c[14].y;\n";

unsigned long cBlurVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(5, D3DVSDT_D3DCOLOR),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

#include "vsh/BlurVP.h"

unsigned long cPhongEMBMVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

unsigned long cPhongEMBMLightVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(8, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

unsigned long cPhongBumpSpecVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(8, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

unsigned long cPhongBumpDiffuseVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(8, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

unsigned long cPhongBumpOnePassVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(12, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(13, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

unsigned long cPhongEnvVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

unsigned long cPhongLightVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(8, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

unsigned long cPhongFlatVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

#include "vsh/PhongVP.h"

unsigned long cShadowTriMeshVariantVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

#include "vsh/ShadowTriMeshVP.h"

#include "vsh/ShadowTriMeshVariantVP.h"

unsigned long cSkinShadowVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_REG(1, D3DVSDT_FLOAT4),
    D3DVSD_REG(2, D3DVSDT_SHORT4),
    D3DVSD_END()
};

#include "vsh/SkinShadowVariantVP.h"
#endif
