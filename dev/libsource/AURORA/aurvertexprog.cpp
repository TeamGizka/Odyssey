#include <string.h>
#include <windows.h>
#include <GL/gl.h>

#include "aurrendergl.h"
#include "aurglext.h"
#include "auremitter.h"
#include "aurinternal.h"
#include "aurlightmanager.h"
#include "aurscene.h"
#include "aurlist.h"

int currentframe;

int nVertexProgUnused1;
int nVertexProgUnused2;
List<VertexProgram*> VertexProgramManager::cm_listExistentVertexPrograms;
#ifndef _XBOX

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
// PC 0x004CEB30
void AurVertexProgramMakeCurrent()
{
    currentframe++;
}

#ifndef _XBOX
void GLRender::DrawElementsInShadow(unsigned int nProgram,  // PC 0x004CEB40
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
// PC 0x004CEB80
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
int VertexProgramManager::ResetVertexPrograms()  // PC 0x004CEBF0
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
#endif
// PC 0x004CEC50
void VertexProgram::Enable(Part* pPart, bool bEnableLights)
{
    static char* sLastProgram;
    static bool bLastEnableLights;

#ifdef _XBOX
    if (sLastProgram == m_sProgram &&
        !LightManager::cm_bLightManagerUpdated &&
        bLastEnableLights == bEnableLights)
    {
        GLRender::BindVertexProgram(m_nGLProgram);
        return;
    }
#else
    unsigned int nProgram;

    if (m_nGLProgram != 0 && sLastProgram == m_sProgram &&
        !LightManager::cm_bLightManagerUpdated &&
        bLastEnableLights == bEnableLights && AurNonRadeon8500Validate())
    {
        BindVertexProgram(m_nGLProgram);
        return;
    }
#endif

    sLastProgram = m_sProgram;
    bLastEnableLights = bEnableLights;
    LightManager::cm_bLightManagerUpdated = 0;

    if (pPart && bEnableLights)
    {
        currentscene->m_pLightManager->SetUpVertexProgramLightShaderConstants(
            m_nAllowedLights, pPart->m_pAurObject == NULL);
    }

    if (m_nLastUpdateFrame != currentframe)
    {
#ifdef _XBOX
        unsigned int nProgram = m_nGLProgram;
        if (!GLRender::IsProgramNV(nProgram) || bEnableLights)

        {
            if (m_bLoaded && !CAurInternal::m_nHeadLightEnabled)
            {
                GLRender::BindVertexProgram(m_nGLProgram);
                return;
            }
#else
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
#endif
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
#ifdef _XBOX
            if (!m_nGLProgram)
            {
                GLRender::GenPrograms(1, &m_nGLProgram);
            }
            unsigned long nLength = strlen(m_sVertexProgram);
            GLRender::DrawElementsInShadow(m_nGLProgram, nLength,
                                           m_sVertexProgram, m_pProgram);
            m_bLoaded = 1;
            return;

        }
    }
    GLRender::BindVertexProgram(m_nGLProgram);
#else
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
// PC 0x004CEF00
void VertexProgramManager::AddVertexProgram(VertexProgram* pProgram)
{
    cm_listExistentVertexPrograms.Add(pProgram);
}
#endif
// PC 0x004CEF60
VertexProgram::VertexProgram(char* sProgram, unsigned long* pnProgram)
{
    m_sProgram = sProgram;
    m_nGLProgram = 0;
    m_nLastUpdateFrame = -1;
    m_pProgram = pnProgram;
#ifdef _XBOX
    m_bLoaded = 0;
#else
    m_nAllowedLights = 3;
    m_bLoaded = 0;
    VertexProgramManager::AddVertexProgram(this);
#endif
}

#ifndef _XBOX
VertexProgram::~VertexProgram()  // PC 0x004CEFB0
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

char cBumpyShinyVP[] =
    "vs.1.1 \n"
    "dp4   oPos.x, c[0], v0;\n"
    "dp4   oPos.y, c[1], v0;\n"
    "dp4   oPos.z, c[2], v0;\n"
    "dp4   oPos.w, c[3], v0;\n"
    "dp3   r1.x, c[8], v11;\n"
    "dp3   r2.x, c[8], v12;\n"
    "dp3   r3.x, c[8], v13;\n"
    "dp3   r1.y, c[9], v11;\n"
    "dp3   r2.y, c[9], v12;\n"
    "dp3   r3.y, c[9], v13;\n"
    "dp3   r1.z, c[10], v11;\n"
    "dp3   r2.z, c[10], v12;\n"
    "dp3   r3.z, c[10], v13;\n"
    "dp3   oT1.x, c[92], r1;\n"
    "dp3   oT1.y, c[92], r2;\n"
    "dp3   oT1.z, c[92], r3;\n"
    "dp3   oT2.x, c[91], r1;\n"
    "dp3   oT2.y, c[91], r2;\n"
    "dp3   oT2.z, c[91], r3;\n"
    "dp3   oT3.x, c[90], r1;\n"
    "dp3   oT3.y, c[90], r2;\n"
    "dp3   oT3.z, c[90], r3;\n"
    "dp4   r4.x, v0, c[8];\n"
    "dp4   r4.y, v0, c[9];\n"
    "dp4   r4.z, v0, c[10];\n"
    "dp4   r4.w, v0, c[11];\n"
    "dp3   r1.w, r4, r4;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "rcp   r4.w, r4.w           ;\n"
    "mul   r4.xyz, r4, -r4.w    ;\n"
    "mov   oT0, v7;\n"
    "dp3   oT1.w, r4, c[92];\n"
    "dp3   oT2.w, r4, c[91];\n"
    "dp3   oT3.w, r4, c[90];\n"
    "mov   oD0, c[15].x;\n";

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

char cBumpMapVP[] =
    "vs.1.1 \n"
    "dp4   oPos.x, c[0], v0;\n"
    "dp4   oPos.y, c[1], v0;\n"
    "dp4   oPos.z, c[2], v0;\n"
    "dp4   oPos.w, c[3], v0;\n"
    "mov   r0, v14;\n"
    "dp3   r1.x, v11, r0;\n"
    "dp3   r1.y, v12, r0;\n"
    "dp3   r1.z, v13, r0;\n"
    "mov   r1.w, c[17].x;\n"
    "mov   oT1, r1;\n"
    "mov   r9, c[16].zzzz; \n"
    "mov   oFog, -v6.x;\n"
    "mov   oT0, v7;\n";

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

char cStandardVP[] =
    "vs.1.1 \n"
    "dp4   oPos.x, c[0], v0;\n"
    "dp4   oPos.y, c[1], v0;\n"
    "dp4   oPos.z, c[2], v0;\n"
    "dp4   oPos.w, c[3], v0;\n"
    "dp4   r10.x, c[8], v0;\n"
    "dp4   r10.y, c[9], v0;\n"
    "dp4   r10.z, c[10], v0;\n"
    "dp4   r10.w, c[11], v0;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "dp3   r11.x, c[8], v3;\n"
    "dp3   r11.y, c[9], v3;\n"
    "dp3   r11.z, c[10], v3;\n"
    "mov   r9, c[15].wwwz;\n"
    "mov   oT0, v7;\n"
    "mov   oT1, v8;\n";

char cEmitterVP[] =
    "xvs.1.1 \n"
    "dp4   r4.x, c[0], v0;\n"
    "dp4   r4.y, c[1], v0;\n"
    "dp4   r4.z, c[2], v0;\n"
    "dp4   r4.w, c[3], v0;\n"
    "mov   oPos, r4;\n"
    "dp4   r10.x, c[8], v0;\n"
    "dp4   r10.y, c[9], v0;\n"
    "dp4   r10.z, c[10], v0;\n"
    "dp4   r10.w, c[11], v0;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "mov   oD0, v5;\n"
    "mov   oT2, c[16].xxxx;\n"
    "mov r5,c[19];\n"
    "dp4 r0.x,r5,c[0];\n"
    "dp4 r0.y,r5,c[1];\n"
    "dp4 r0.z,r5,c[2];\n"
    "dp4 r0.w,r5,c[3];\n"
    "rcp r2.w,r0.w;\n"
    "mul r0,r0,r2.w;\n"
    "mov r7, r0;\n"
    "mov r5,c[20];\n"
    "dp4 r1.x,r5,c[0];\n"
    "dp4 r1.y,r5,c[1];\n"
    "dp4 r1.z,r5,c[2];\n"
    "dp4 r1.w,r5,c[3];\n"
    "rcp r2.w,r1.w;\n"
    "mul r1,r1,r2.w;\n"
    "mov r8, r1;\n"
    "add r0,r7,-r8;\n"
    "dp3 r2.x,r0,r0;\n"
    "rsq r1.x,r2.x;\n"
    "rcp r1.y,r1.x;\n"
    "mul r7.y, -r1.y, c[18].x;\n"
    "mul oT2.y, r7.y, c[18].z;\n"
    "rcp r1,r4.w;\n"
    "mul r1,r1,r4.z;\n"
    "mul oT2.z, r1, c[18].x;\n"
    "mov   oT0, v7;\n"
    "mov   oT1, v7;\n"
    "mov   oT3, c[16].xxwx;\n";

char cMotionBlurredVP[] =
    "vs.1.1 \n"
    "dp4   r10.x, c[8],  v0;\n"
    "dp4   r10.y, c[9],  v0;\n"
    "dp4   r10.z, c[10], v0;\n"
    "dp4   r10.w, c[11], v0;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "dp3   r11.x, c[8], v3;\n"
    "dp3   r11.y, c[9], v3;\n"
    "dp3   r11.z, c[10], v3;\n"
    "mov   r4, c[50];\n"
    "dp3   r6.x, r4, r4;\n"
    "rsq   r6.y, r6.x;\n"
    "mul   r4.xyz, r4.xyz, r6.y;\n"
    "mov   r7.x, c[51].x;\n"
    "min   r2.y, r7.x, c[51].y;\n"
    "mul   r5.xyz, r4.xyz, r2.y;\n"
    "mov   r5.w, c[17].x;\n"
    "dp3   r3.w, r4, v3;\n"
    "slt   r2.w, r3.w, c[17].x;\n"
    "mov   r1, c[53].xyzw;\n"
    "mad   r6, r2.w, -r5, v0.xyzw;\n"
    "mul   r7.y, r2.y, c[13].x;\n"
    "rcp   r8.y, c[51].y;\n"
    "mul   r8.w, r2.y, r8.y;\n"
    "mul   r7.y, r7.y, r8.w;\n"
    "mul   r7.y, r7.y, c[52].w;\n"
    "mov   r3.w, c[17].x;\n"
    "mov   r3, c[52].xyz;\n"
    "dp3   r3.w, r3, r3;\n"
    "rsq   r5.w, r3.w;\n"
    "mul   r3, r3, r5.w;\n"
    "add   r5.xyz, r6, -r1;\n"
    "dp3   r5.w, r5, r3;\n"
    "slt   r5.x, r5.w, c[17].x;\n"
    "mov   r7.x, c[17].w;\n"
    "mad   r5.y, r5.x, -c[17].y, r7.x;\n"
    "mul   r5.z, r5.y, r7.y;\n"
    "mul   r5.z, r5.z, r2.w;\n"
    "mad   r6, r5.z, r3, r6;\n"
    "add   r2.x, -r2.w, c[17].w;\n"
    "mul   r1, v3, -c[51].z;\n"
    "mov   r1.w, c[17].x;\n"
    "mad   r5, r2.x, r1, r6;\n"
    "dp4   oPos.x, r5, c[0];\n"
    "dp4   oPos.y, r5, c[1];\n"
    "dp4   oPos.z, r5, c[2];\n"
    "dp4   oPos.w, r5, c[3];\n"
    "add   r2.w, c[17].w, -r2.w;\n"
    "mov r9.xyz, c[17].xxx;\n"
    "mul r9.w, c[17].z, r2.w;\n"
    "mov oD0, r9;\n"
    "mov   oT0, v7;\n";

unsigned long cBumpedOutVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

char cBumpedOutVP[] =
    "vs.1.1 \n"
    "mov r1, v0;\n"
    "mov r2, v3;\n"
    "mad r1, r2, c[12].x, r1;\n"
    "dp4   oPos.x, c[0], r1;\n"
    "dp4   oPos.y, c[1], r1;\n"
    "dp4   oPos.z, c[2], r1;\n"
    "dp4   oPos.w, c[3], r1;\n"
    "dp4   r10.x, c[8], r1;\n"
    "dp4   r10.y, c[9], r1;\n"
    "dp4   r10.z, c[10], r1;\n"
    "dp4   r10.w, c[11], r1;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "dp3   r11.x, c[8], v3;\n"
    "dp3   r11.y, c[9], v3;\n"
    "dp3   r11.z, c[10], v3;\n"
    "mov   r9, c[15].wwwz;\n"
    "mov   oT0, v7;\n"
    "mov   oT1, v8;\n"
    "mov oD0, c[12].wwww;\n";

unsigned long cFogTestVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_END()
};

char cFogTestVP[] =
    "vs.1.0 \n"
    "dp4   oPos.x, c[0], v0;\n"
    "dp4   oPos.y, c[1], v0;\n"
    "dp4   oPos.z, c[2], v0;\n"
    "dp4   oPos.w, c[3], v0;\n"
    "dp4   r10.x, c[8], v0;\n"
    "dp4   r10.y, c[9], v0;\n"
    "dp4   r10.z, c[10], v0;\n"
    "dp4   r10.w, c[11], v0;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "add   r2.x, c[17].y, -c[17].x;\n"
    "add   r2.y, c[17].y, -r1.w;\n"
    "rcp   r2.z, r2.x;\n"
    "mul   r2.w, r2.y, r2.z;\n"
    "mov   oFog, -r2.w;\n"
    "mov   oD1.xyzw, r2.wwww;\n"
    "mov   r3.xyzw, c[16].yxxy;\n"
    "mov   r3.z, r2.w;\n"
    "add   r3.x, r3.x, -r2.w;\n"
    "mov   oD0, r3;";

unsigned long cLightProjectionVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(7, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

char cLightProjectionVP[] =
    "vs.1.0 \n"
    "mov   r1, c[20];\n"
    "add   r2, v0, -r1;\n"
    "dp3   r0.w, r2, r2;\n"
    "rsq   r0.w, r0.w;\n"
    "mul   r2, r2, r0.w;\n"
    "mov   r11, v6;\n"
    "slt   r10.x, r11.x, r11.y;\n"
    "sge   r10.y, r11.x, r11.y;\n"
    "mad   r4, r2, c[21].x, r1;\n"
    "mov   r4.w, c[21].z;\n"
    "dp3   r3.w, r4, r4;\n"
    "rcp   r0.w, r0.w;\n"
    "sge   r9.y, r3.w, r0.w;\n"
    "slt   r9.z, r3.w, r0.w;\n"
    "mov   r0, v0;\n"
    "mul   r0, r0, r9.z;\n"
    "mad   r4, r4, r9.y, r0;\n"
    "mul   r4, r4, r10.y;\n"
    "mad   r5, r2, c[21].y, v0;\n"
    "mov   r5.w, c[21].z;\n"
    "mul   r5, r5, r10.x;\n"
    "mad   r6, r5, r10.x, r4;\n"
    "dp3   r7.x, r2, c[22];\n"
    "rcp   r7.x, r7.x;\n"
    "dp3   r7.y, r1, c[22];\n"
    "add   r7.z, r7.y, c[22].w;\n"
    "mul   r7.w, r7.z, -r7.x;\n"
    "mad   r8, r2, r7.w, r1;\n"
    "mov   r8.w, c[21].z;\n"
    "dp3   r9.x, r6, c[22];\n"
    "add   r9.x, r9.x, c[22].w;\n"
    "sge   r9.y, r9.x, c[21].w;\n"
    "slt   r9.z, r9.x, c[21].w;\n"
    "mul   r8, r8, r9.y;\n"
    "mad   r6, r6, r9.z, r8;\n"
    "dp4   oPos.x, c[0], r6;\n"
    "dp4   oPos.y, c[1], r6;\n"
    "dp4   oPos.z, c[2], r6;\n"
    "dp4   oPos.w, c[3], r6;\n"
    "mov   oD0, v5;\n";

unsigned long cFogOutputVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_END()
};

char cFogOutputVP[] =
    "vs.1.0 \n"
    "dp4   oPos.x, c[0], v0;\n"
    "dp4   oPos.y, c[1], v0;\n"
    "dp4   oPos.z, c[2], v0;\n"
    "dp4   oPos.w, c[3], v0;\n"
    "dp4   r1.x, c[8], v0;\n"
    "dp4   r1.y, c[9], v0;\n"
    "dp4   r1.z, c[10], v0;\n"
    "dp4   r1.w, c[11], v0;\n"
    "dp3   r1.w, r1, r1;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "add   r2.x, c[20].y, -c[20].x;\n"
    "add   r2.y, c[20].y, -r1.w;\n"
    "rcp   r2.z, r2.x;\n"
    "mul   r2.w, r2.y, r2.z;\n"
    "min   r2.w, r2.w, c[22].x;\n"
    "max   r2.w, r2.w, c[22].y;\n"
    "add   r2.w, c[22].x, -r2.w;\n"
    "mul   r3, c[21], r2.w;\n"
    "mov   r3.w, r2.w;\n"
    "mov   oD0, r3;\n"
    "mov   oFog.x, -c[22].y;";

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

char cBumpyShinySkinVP[] =
    "vs.1.1;\n"
    "mul r0, v2, c[16].zzzz;\n"
    "mov a0.x, r0.x;\n"
    "dp4 r1.x, v0, c[a0.x + 18];\n"
    "dp4 r1.y, v0, c[a0.x + 19];\n"
    "dp4 r1.z, v0, c[a0.x + 20];\n"
    "mul r9, v1.x, r1.xyz;\n"
    "dp3   r1.x, v11, c[a0.x + 18];\n"
    "dp3   r1.y, v11, c[a0.x + 19];\n"
    "dp3   r1.z, v11, c[a0.x + 20];\n"
    "mul   r4, v1.x, r1.xyz;\n"
    "dp3   r1.x, v12, c[a0.x + 18];\n"
    "dp3   r1.y, v12, c[a0.x + 19];\n"
    "dp3   r1.z, v12, c[a0.x + 20];\n"
    "mul   r5, v1.x, r1.xyz;\n"
    "dp3   r1.x, v13, c[a0.x + 18];\n"
    "dp3   r1.y, v13, c[a0.x + 19];\n"
    "dp3   r1.z, v13, c[a0.x + 20];\n"
    "mul   r6, v1.x, r1.xyz;\n"
    "mov a0.x, r0.y;\n"
    "dp4 r1.x, v0, c[a0.x + 18];\n"
    "dp4 r1.y, v0, c[a0.x + 19];\n"
    "dp4 r1.z, v0, c[a0.x + 20];\n"
    "mad r9, r1.xyz, v1.y, r9;\n"
    "dp3   r1.x, v11, c[a0.x + 18];\n"
    "dp3   r1.y, v11, c[a0.x + 19];\n"
    "dp3   r1.z, v11, c[a0.x + 20];\n"
    "mad   r4, v1.y, r1.xyz, r4;\n"
    "dp3   r1.x, v12, c[a0.x + 18];\n"
    "dp3   r1.y, v12, c[a0.x + 19];\n"
    "dp3   r1.z, v12, c[a0.x + 20];\n"
    "mad   r5, v1.y, r1.xyz, r5;\n"
    "dp3   r1.x, v13, c[a0.x + 18];\n"
    "dp3   r1.y, v13, c[a0.x + 19];\n"
    "dp3   r1.z, v13, c[a0.x + 20];\n"
    "mad   r6, v1.y, r1.xyz, r6;\n"
    "mov a0.x, r0.z;\n"
    "dp4 r1.x, v0, c[a0.x + 18];\n"
    "dp4 r1.y, v0, c[a0.x + 19];\n"
    "dp4 r1.z, v0, c[a0.x + 20];\n"
    "mad r9, r1.xyz, v1.z, r9;\n"
    "dp3   r1.x, v11, c[a0.x + 18];\n"
    "dp3   r1.y, v11, c[a0.x + 19];\n"
    "dp3   r1.z, v11, c[a0.x + 20];\n"
    "mad   r4, v1.z, r1.xyz, r4;\n"
    "dp3   r1.x, v12, c[a0.x + 18];\n"
    "dp3   r1.y, v12, c[a0.x + 19];\n"
    "dp3   r1.z, v12, c[a0.x + 20];\n"
    "mad   r5, v1.z, r1.xyz, r5;\n"
    "dp3   r1.x, v13, c[a0.x + 18];\n"
    "dp3   r1.y, v13, c[a0.x + 19];\n"
    "dp3   r1.z, v13, c[a0.x + 20];\n"
    "mad   r6, v1.z, r1.xyz, r6;\n"
    "mov a0.x, r0.w;\n"
    "dp4 r1.x, v0, c[a0.x + 18];\n"
    "dp4 r1.y, v0, c[a0.x + 19];\n"
    "dp4 r1.z, v0, c[a0.x + 20];\n"
    "mad r9, r1.xyz, v1.w, r9;\n"
    "dp3   r1.x, v11, c[a0.x + 18];\n"
    "dp3   r1.y, v11, c[a0.x + 19];\n"
    "dp3   r1.z, v11, c[a0.x + 20];\n"
    "mad   r4, v1.w, r1.xyz, r4;\n"
    "dp3   r1.x, v12, c[a0.x + 18];\n"
    "dp3   r1.y, v12, c[a0.x + 19];\n"
    "dp3   r1.z, v12, c[a0.x + 20];\n"
    "mad   r5, v1.w, r1.xyz, r5;\n"
    "dp3   r1.x, v13, c[a0.x + 18];\n"
    "dp3   r1.y, v13, c[a0.x + 19];\n"
    "dp3   r1.z, v13, c[a0.x + 20];\n"
    "mad   r6, v1.w, r1.xyz, r6;\n"
    "mov r9.w, c[16].w;\n"
    "dp4 oPos.x, c[0], r9;\n"
    "dp4 oPos.y, c[1], r9;\n"
    "dp4 oPos.z, c[2], r9;\n"
    "dp4 oPos.w, c[3], r9;\n"
    "dp4 r10.x, c[8], r9;\n"
    "dp4 r10.y, c[9], r9;\n"
    "dp4 r10.z, c[10], r9;\n"
    "dp4 r10.w, c[11], r9;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "mov r9, c[16].xxxw;\n"
    "dp3 r1.x, c[8], r4;\n"
    "dp3 r2.x, c[8], r5;\n"
    "dp3 r3.x, c[8], r6;\n"
    "dp3 r1.y, c[9], r4;\n"
    "dp3 r2.y, c[9], r5;\n"
    "dp3 r3.y, c[9], r6;\n"
    "dp3 r1.z, c[10], r4;\n"
    "dp3 r2.z, c[10], r5;\n"
    "dp3 r3.z, c[10], r6;\n"
    "dp3 oT1.x, c[92], r1;\n"
    "dp3 oT1.y, c[92], r2;\n"
    "dp3 oT1.z, c[92], r3;\n"
    "dp3 oT2.x, c[91], r1;\n"
    "dp3 oT2.y, c[91], r2;\n"
    "dp3 oT2.z, c[91], r3;\n"
    "dp3 oT3.x, c[90], r1;\n"
    "dp3 oT3.y, c[90], r2;\n"
    "dp3 oT3.z, c[90], r3;\n"
    "dp4 r4.x, v0, c[8];\n"
    "dp4 r4.y, v0, c[9];\n"
    "dp4 r4.z, v0, c[10];\n"
    "dp4 r4.w, v0, c[11];\n"
    "rcp r4.w, r4.w;\n"
    "mul r4.xyz, r4, -r4.w;\n"
    "mov oT0, v7;\n"
    "dp3 oT1.w, r4, c[92];\n"
    "dp3 oT2.w, r4, c[91];\n"
    "dp3 oT3.w, r4, c[90];\n"
    "mov oD0, c[15].x;\n";

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

char cSkinVP[] =
    "vs.1.1 \n"
    "mul   r0, v2, c[16].zzzz;\n"
    "mov   a0.x, r0.x;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "dp3   r5.x, v3, c[a0.x + 18];\n"
    "dp3   r5.y, v3, c[a0.x + 19];\n"
    "dp3   r5.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.y;\n"
    "dp4   r2.x, v0, c[a0.x + 18];\n"
    "dp4   r2.y, v0, c[a0.x + 19];\n"
    "dp4   r2.z, v0, c[a0.x + 20];\n"
    "dp3   r6.x, v3, c[a0.x + 18];\n"
    "dp3   r6.y, v3, c[a0.x + 19];\n"
    "dp3   r6.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.z;\n"
    "dp4   r3.x, v0, c[a0.x + 18];\n"
    "dp4   r3.y, v0, c[a0.x + 19];\n"
    "dp4   r3.z, v0, c[a0.x + 20];\n"
    "dp3   r7.x, v3, c[a0.x + 18];\n"
    "dp3   r7.y, v3, c[a0.x + 19];\n"
    "dp3   r7.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.w;\n"
    "dp4   r4.x, v0, c[a0.x + 18];\n"
    "dp4   r4.y, v0, c[a0.x + 19];\n"
    "dp4   r4.z, v0, c[a0.x + 20];\n"
    "dp3   r8.x, v3, c[a0.x + 18];\n"
    "dp3   r8.y, v3, c[a0.x + 19];\n"
    "dp3   r8.z, v3, c[a0.x + 20];\n"
    "mul   r9, v1.x, r1.xyz;\n"
    "mad   r9, r2.xyz, v1.y, r9;\n"
    "mad   r9, r3.xyz, v1.z, r9;\n"
    "mad   r9, r4.xyz, v1.w, r9;\n"
    "mov   r9.w, c[16].w;\n"
    "mul   r10, v1.xxx, r5.xyz;\n"
    "mad   r10, r6.xyz, v1.y, r10;\n"
    "mad   r10, r7.xyz, v1.z, r10;\n"
    "mad   r10, r8.xyz, v1.w, r10;\n"
    "dp4   oPos.x, c[0], r9;\n"
    "dp4   oPos.y, c[1], r9;\n"
    "dp4   oPos.z, c[2], r9;\n"
    "dp4   oPos.w, c[3], r9;\n"
    "dp3   r11.x, c[8], r10;\n"
    "dp3   r11.y, c[9], r10;\n"
    "dp3   r11.z, c[10], r10;\n"
    "mov   r11.w, c[16].w;\n"
    "dp3   r11.w, r11, r11;\n"
    "rsq   r11.w, r11.w;\n"
    "mul   r11, r11, r11.w;\n"
    "dp4   r10.x, c[8], r9;\n"
    "dp4   r10.y, c[9], r9;\n"
    "dp4   r10.z, c[10], r9;\n"
    "mov   r10.w, c[16].w;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "mov   r9, c[16].xxxw;\n"
    "mov   oT0, v7;";

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

char cSkinBumpedOutVP[] =
    "vs.1.1 \n"
    "mul   r0, v2, c[16].zzzz;\n"
    "mov   a0.x, r0.x;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "dp3   r5.x, v3, c[a0.x + 18];\n"
    "dp3   r5.y, v3, c[a0.x + 19];\n"
    "dp3   r5.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.y;\n"
    "dp4   r2.x, v0, c[a0.x + 18];\n"
    "dp4   r2.y, v0, c[a0.x + 19];\n"
    "dp4   r2.z, v0, c[a0.x + 20];\n"
    "dp3   r6.x, v3, c[a0.x + 18];\n"
    "dp3   r6.y, v3, c[a0.x + 19];\n"
    "dp3   r6.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.z;\n"
    "dp4   r3.x, v0, c[a0.x + 18];\n"
    "dp4   r3.y, v0, c[a0.x + 19];\n"
    "dp4   r3.z, v0, c[a0.x + 20];\n"
    "dp3   r7.x, v3, c[a0.x + 18];\n"
    "dp3   r7.y, v3, c[a0.x + 19];\n"
    "dp3   r7.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.w;\n"
    "dp4   r4.x, v0, c[a0.x + 18];\n"
    "dp4   r4.y, v0, c[a0.x + 19];\n"
    "dp4   r4.z, v0, c[a0.x + 20];\n"
    "dp3   r8.x, v3, c[a0.x + 18];\n"
    "dp3   r8.y, v3, c[a0.x + 19];\n"
    "dp3   r8.z, v3, c[a0.x + 20];\n"
    "mul   r9, v1.x, r1.xyz;\n"
    "mad   r9, r2.xyz, v1.y, r9;\n"
    "mad   r9, r3.xyz, v1.z, r9;\n"
    "mad   r9, r4.xyz, v1.w, r9;\n"
    "mov   r9.w, c[16].w;\n"
    "mul   r10, v1.xxx, r5.xyz;\n"
    "mad   r10, r6.xyz, v1.y, r10;\n"
    "mad   r10, r7.xyz, v1.z, r10;\n"
    "mad   r10, r8.xyz, v1.w, r10;\n"
    "mad   r9.xyz, r10.xyz, c[12].xxx, r9.xyz;\n"
    "dp4   oPos.x, c[0], r9;\n"
    "dp4   oPos.y, c[1], r9;\n"
    "dp4   oPos.z, c[2], r9;\n"
    "dp4   oPos.w, c[3], r9;\n"
    "dp3   r11.x, c[8], r10;\n"
    "dp3   r11.y, c[9], r10;\n"
    "dp3   r11.z, c[10], r10;\n"
    "mov   r11.w, c[16].w;\n"
    "dp3   r11.w, r11, r11;\n"
    "rsq   r11.w, r11.w;\n"
    "mul   r11, r11, r11.w;\n"
    "dp4   r10.x, c[8], r9;\n"
    "dp4   r10.y, c[9], r9;\n"
    "dp4   r10.z, c[10], r9;\n"
    "mov   r10.w, c[16].w;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "mov   r9, c[12].wwww;\n"
    "mov   oD0.xyzw, c[12].wwww;\n"
    "mov   oT0, v7;";

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

char cSkinMotionBlurVP[] =
    "vs.1.1 \n"
    "mul   r0, v2, c[16].zzzz;\n"
    "mov   a0.x, r0.x;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "dp3   r5.x, v3, c[a0.x + 18];\n"
    "dp3   r5.y, v3, c[a0.x + 19];\n"
    "dp3   r5.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.y;\n"
    "dp4   r2.x, v0, c[a0.x + 18];\n"
    "dp4   r2.y, v0, c[a0.x + 19];\n"
    "dp4   r2.z, v0, c[a0.x + 20];\n"
    "dp3   r6.x, v3, c[a0.x + 18];\n"
    "dp3   r6.y, v3, c[a0.x + 19];\n"
    "dp3   r6.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.z;\n"
    "dp4   r3.x, v0, c[a0.x + 18];\n"
    "dp4   r3.y, v0, c[a0.x + 19];\n"
    "dp4   r3.z, v0, c[a0.x + 20];\n"
    "dp3   r7.x, v3, c[a0.x + 18];\n"
    "dp3   r7.y, v3, c[a0.x + 19];\n"
    "dp3   r7.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.w;\n"
    "dp4   r4.x, v0, c[a0.x + 18];\n"
    "dp4   r4.y, v0, c[a0.x + 19];\n"
    "dp4   r4.z, v0, c[a0.x + 20];\n"
    "dp3   r8.x, v3, c[a0.x + 18];\n"
    "dp3   r8.y, v3, c[a0.x + 19];\n"
    "dp3   r8.z, v3, c[a0.x + 20];\n"
    "mul   r9, v1.x, r1.xyz;\n"
    "mad   r9, r2.xyz, v1.y, r9;\n"
    "mad   r9, r3.xyz, v1.z, r9;\n"
    "mad   r9, r4.xyz, v1.w, r9;\n"
    "mov   r9.w, c[16].w;\n"
    "mul   r10, v1.xxx, r5.xyz;\n"
    "mad   r10, r6.xyz, v1.y, r10;\n"
    "mad   r10, r7.xyz, v1.z, r10;\n"
    "mad   r10, r8.xyz, v1.w, r10;\n"
    "dp3   r10.w, r10, r10;\n"
    "rsq   r10.w, r10.w;\n"
    "mul   r10, r10, r10.w;\n"
    "mov   r10.w, c[16].x;\n"
    "mov   r4, c[12];\n"
    "mov   r7.x, c[14].x;\n"
    "min   r2.y, r7.x, c[14].y;\n"
    "mul   r5.xyz, r4.xyz, r2.y;\n"
    "mov   r5.w, c[16].x;\n"
    "dp3   r3.w, r4, r10;\n"
    "slt   r2.w, r3.w, c[16].x;\n"
    "mov   r1, c[17].xyz;\n"
    "mov   r1.w, c[16].x;\n"
    "mad   r6, r2.w, -r5, r9.xyzw;\n"
    "mul r7.y, r2.y, c[13].x;\n"
    "rcp   r8.y, c[14].y;\n"
    "mul   r8.w, r2.y, r8.y;\n"
    "mul   r7.y, r7.y, r8.w;\n"
    "mul   r7.y, r7.y, c[17].w;\n"
    "mov   r3.w, c[16].x;\n"
    "mov   r3, c[69].xyz;\n"
    "dp3   r3.w, r3, r3;\n"
    "rsq   r5.w, r3.w;\n"
    "mul   r3, r3, r5.w;\n"
    "add   r5.xyz, r6, -r1;\n"
    "dp3   r5.w, r5, r3;\n"
    "slt   r5.x, r5.w, c[16].x;\n"
    "mov   r7.x, c[16].w;\n"
    "mad   r5.y, r5.x, -c[16].y, r7.x;\n"
    "mul   r5.z, r5.y, r7.y;\n"
    "mul   r5.z, r5.z, r2.w;\n"
    "mad   r6, r5.z, r3, r6;\n"
    "add   r2.x, -r2.w, c[16].w;\n"
    "mul   r1, r10, -c[14].z;\n"
    "mov   r1.w, c[16].x;\n"
    "mad   r5, r2.x, r1, r6;\n"
    "dp4   r1.x, r5, c[0];\n"
    "dp4   r1.y, r5, c[1];\n"
    "dp4   r1.z, r5, c[2];\n"
    "dp4   r1.w, r5, c[3];\n"
    "mov   oPos.xyzw, r1.xyzw;\n"
    "add   r2.w, c[16].w, -r2.w;\n"
    "mov   r9.xyz, c[16].xxx;\n"
    "mul   r9.w, c[69].w, r2.w;\n"
    "mov   oD0, r9;\n"
    "dp3   r11.x, c[8], r10;\n"
    "dp3   r11.y, c[9], r10;\n"
    "dp3   r11.z, c[10], r10;\n"
    "mov   r11.w, c[16].w;\n"
    "dp4   r10.x, c[8], r5;\n"
    "dp4   r10.y, c[9], r5;\n"
    "dp4   r10.z, c[10], r5;\n"
    "mov   r10.w, c[16].w;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "mov   oT0, v7;";

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

char cSkin3BoneVP[] =
    "vs.1.1 \n"
    "mul   r0, v2, c[16].zzzz;\n"
    "mov   a0.x, r0.x;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "dp3   r5.x, v3, c[a0.x + 18];\n"
    "dp3   r5.y, v3, c[a0.x + 19];\n"
    "dp3   r5.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.y;\n"
    "dp4   r2.x, v0, c[a0.x + 18];\n"
    "dp4   r2.y, v0, c[a0.x + 19];\n"
    "dp4   r2.z, v0, c[a0.x + 20];\n"
    "dp3   r6.x, v3, c[a0.x + 18];\n"
    "dp3   r6.y, v3, c[a0.x + 19];\n"
    "dp3   r6.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.z;\n"
    "dp4   r3.x, v0, c[a0.x + 18];\n"
    "dp4   r3.y, v0, c[a0.x + 19];\n"
    "dp4   r3.z, v0, c[a0.x + 20];\n"
    "dp3   r7.x, v3, c[a0.x + 18];\n"
    "dp3   r7.y, v3, c[a0.x + 19];\n"
    "dp3   r7.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.w;\n"
    "dp4   r4.x, v0, c[a0.x + 18];\n"
    "dp4   r4.y, v0, c[a0.x + 19];\n"
    "dp4   r4.z, v0, c[a0.x + 20];\n"
    "dp3   r8.x, v3, c[a0.x + 18];\n"
    "dp3   r8.y, v3, c[a0.x + 19];\n"
    "dp3   r8.z, v3, c[a0.x + 20];\n"
    "mul   r9, v1.x, r1.xyz;\n"
    "mad   r9, r2.xyz, v1.y, r9;\n"
    "mad   r9, r3.xyz, v1.z, r9;\n"
    "mad   r9, r4.xyz, v1.w, r9;\n"
    "mov   r9.w, c[16].w;\n"
    "mul   r10, v1.xxx, r5.xyz;\n"
    "mad   r10, r6.xyz, v1.y, r10;\n"
    "mad   r10, r7.xyz, v1.z, r10;\n"
    "mad   r10, r8.xyz, v1.w, r10;\n"
    "dp4   oPos.x, c[0], r9;\n"
    "dp4   oPos.y, c[1], r9;\n"
    "dp4   oPos.z, c[2], r9;\n"
    "dp4   oPos.w, c[3], r9;\n"
    "dp3   r11.x, c[8], r10;\n"
    "dp3   r11.y, c[9], r10;\n"
    "dp3   r11.z, c[10], r10;\n"
    "mov   r11.w, c[16].w;\n"
    "dp3   r11.w, r11, r11;\n"
    "rsq   r11.w, r11.w;\n"
    "mul   r11, r11, r11.w;\n"
    "dp4   r10.x, c[8], r9;\n"
    "dp4   r10.y, c[9], r9;\n"
    "dp4   r10.z, c[10], r9;\n"
    "mov   r10.w, c[16].w;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "mov   r9, c[16].xxxw;\n"
    "mov   oT0, v7;";

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

char cSkinBumpyVP[] =
    "vs.1.1 \n"
    "mul   r0, v2, c[16].zzzz;\n"
    "mov   a0.x, r0.x;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "dp3   r5.x, v3, c[a0.x + 18];\n"
    "dp3   r5.y, v3, c[a0.x + 19];\n"
    "dp3   r5.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.y;\n"
    "dp4   r2.x, v0, c[a0.x + 18];\n"
    "dp4   r2.y, v0, c[a0.x + 19];\n"
    "dp4   r2.z, v0, c[a0.x + 20];\n"
    "dp3   r6.x, v3, c[a0.x + 18];\n"
    "dp3   r6.y, v3, c[a0.x + 19];\n"
    "dp3   r6.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.z;\n"
    "dp4   r3.x, v0, c[a0.x + 18];\n"
    "dp4   r3.y, v0, c[a0.x + 19];\n"
    "dp4   r3.z, v0, c[a0.x + 20];\n"
    "dp3   r7.x, v3, c[a0.x + 18];\n"
    "dp3   r7.y, v3, c[a0.x + 19];\n"
    "dp3   r7.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.w;\n"
    "dp4   r4.x, v0, c[a0.x + 18];\n"
    "dp4   r4.y, v0, c[a0.x + 19];\n"
    "dp4   r4.z, v0, c[a0.x + 20];\n"
    "dp3   r8.x, v3, c[a0.x + 18];\n"
    "dp3   r8.y, v3, c[a0.x + 19];\n"
    "dp3   r8.z, v3, c[a0.x + 20];\n"
    "mul   r9, v1.x, r1.xyz\n"
    "mad   r9, r2.xyz, v1.y, r9;\n"
    "mad   r9, r3.xyz, v1.z, r9;\n"
    "mad   r9, r4.xyz, v1.w, r9;\n"
    "mov   r9.w, c[16].w;\n"
    "mul   r10, v1.x, r5.xyz;\n"
    "mad   r10, r6.xyz, v1.y, r10;\n"
    "mad   r10, r7.xyz, v1.z, r10;\n"
    "mad   r10, r8.xyz, v1.w, r10;\n"
    "dp4   oPos.x, c[0], r9;\n"
    "dp4   oPos.y, c[1], r9;\n"
    "dp4   oPos.z, c[2], r9;\n"
    "dp4   oPos.w, c[3], r9;\n"
    "dp3   r11.x, c[8], r10;\n"
    "dp3   r11.y, c[9], r10;\n"
    "dp3   r11.z, c[10], r10;\n"
    "mov   r11.w, c[16].w;\n"
    "dp3   r11.w, r11, r11;\n"
    "rsq   r11.w, r11.w;\n"
    "mul   r11, r11, r11.w;\n"
    "dp4   r10.x, c[8], r9;\n"
    "dp4   r10.y, c[9], r9;\n"
    "dp4   r10.z, c[10], r9;\n"
    "mov   r10.w, c[16].w;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "add   r0, c[12], -r9;\n"
    "dp3   r0.w, r0, r0;\n"
    "rsq   r1.w, r0.w;\n"
    "dst   r2, r0.w, r1.w;\n"
    "dp3   r2.w, r2, c[14];\n"
    "rcp   r2.w, r2.w;\n"
    "mov   r4.xyzw, r2.wwww;\n"
    "mul   r0, r0, r1.w;\n"
    "mov   r0.w, c[16].w;\n"
    "add   r2, c[13],-r9;\n"
    "dp3   r2.w, r2, r2;\n"
    "rsq   r2.w, r2.w;\n"
    "mul   r2, r2, r2.w;\n"
    "mov   r2.w, c[16].w;\n"
    "add   r2, r2, r0;mov   r2.w, c[16].w;\n"
    "mov   r9, c[16].xxxw;\n"
    "mov oD0, r9;\n"
    "mov   oT0, v7;\n"
    "mov   oT3, v7;\n"
    "dp3   r1.x, v11, r0;\n"
    "dp3   r1.y, v12, r0;\n"
    "dp3   r1.z, v13, r0;\n"
    "mov   r1.w, c[16].w;\n"
    "mov   oT1, r1;\n"
    "dp3   r1.x, v11, r2;\n"
    "dp3   r1.y, v12, r2;\n"
    "dp3   r1.z, v13, r2;\n"
    "mov   r1.w, c[16].w;\n"
    "mov   oT2, r1;\n"
    "mov   oD1, r4;\n";

char cNewSkinBumpyVP[] =
    "vs.1.1 \n"
    "mul   r0, v2, c[16].zzzz;\n"
    "mov   a0.x, r0.x;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "mul   r9, v1.x, r1.xyz;\n"
    "dp3   r1.x, v11, c[a0.x + 18];\n"
    "dp3   r1.y, v11, c[a0.x + 19];\n"
    "dp3   r1.z, v11, c[a0.x + 20];\n"
    "mul   r4, v1.x, r1.xyz;\n"
    "dp3   r1.x, v12, c[a0.x + 18];\n"
    "dp3   r1.y, v12, c[a0.x + 19];\n"
    "dp3   r1.z, v12, c[a0.x + 20];\n"
    "mul   r5, v1.x, r1.xyz;\n"
    "dp3   r1.x, v13, c[a0.x + 18];\n"
    "dp3   r1.y, v13, c[a0.x + 19];\n"
    "dp3   r1.z, v13, c[a0.x + 20];\n"
    "mul   r6, v1.x, r1.xyz;\n"
    "mov   a0.x, r0.y;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "mad   r9, r1.xyz, v1.y, r9;\n"
    "dp3   r1.x, v11, c[a0.x + 18];\n"
    "dp3   r1.y, v11, c[a0.x + 19];\n"
    "dp3   r1.z, v11, c[a0.x + 20];\n"
    "mad   r4, r1.xyz, v1.y, r4;\n"
    "dp3   r1.x, v12, c[a0.x + 18];\n"
    "dp3   r1.y, v12, c[a0.x + 19];\n"
    "dp3   r1.z, v12, c[a0.x + 20];\n"
    "mad   r5, r1.xyz, v1.y, r5;\n"
    "dp3   r1.x, v13, c[a0.x + 18];\n"
    "dp3   r1.y, v13, c[a0.x + 19];\n"
    "dp3   r1.z, v13, c[a0.x + 20];\n"
    "mad   r6, r1.xyz, v1.y, r6;\n"
    "mov   a0.x, r0.z;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "mad   r9, r1.xyz, v1.z, r9;\n"
    "dp3   r1.x, v11, c[a0.x + 18];\n"
    "dp3   r1.y, v11, c[a0.x + 19];\n"
    "dp3   r1.z, v11, c[a0.x + 20];\n"
    "mad   r4, r1.xyz, v1.z, r4;\n"
    "dp3   r1.x, v12, c[a0.x + 18];\n"
    "dp3   r1.y, v12, c[a0.x + 19];\n"
    "dp3   r1.z, v12, c[a0.x + 20];\n"
    "mad   r5, r1.xyz, v1.z, r5;\n"
    "dp3   r1.x, v13, c[a0.x + 18];\n"
    "dp3   r1.y, v13, c[a0.x + 19];\n"
    "dp3   r1.z, v13, c[a0.x + 20];\n"
    "mad   r6, r1.xyz, v1.z, r6;\n"
    "mov   a0.x, r0.w;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "mad   r9, r1.xyz, v1.w, r9;\n"
    "dp3   r1.x, v11, c[a0.x + 18];\n"
    "dp3   r1.y, v11, c[a0.x + 19];\n"
    "dp3   r1.z, v11, c[a0.x + 20];\n"
    "mad   r4, r1.xyz, v1.w, r4;\n"
    "dp3   r1.x, v12, c[a0.x + 18];\n"
    "dp3   r1.y, v12, c[a0.x + 19];\n"
    "dp3   r1.z, v12, c[a0.x + 20];\n"
    "mad   r5, r1.xyz, v1.w, r5;\n"
    "dp3   r1.x, v13, c[a0.x + 18];\n"
    "dp3   r1.y, v13, c[a0.x + 19];\n"
    "dp3   r1.z, v13, c[a0.x + 20];\n"
    "mad   r6, r1.xyz, v1.w, r6;\n"
    "mov   r9.w, c[16].w;\n"
    "dp4   oPos.x, c[0], r9;\n"
    "dp4   oPos.y, c[1], r9;\n"
    "dp4   oPos.z, c[2], r9;\n"
    "dp4   oPos.w, c[3], r9;\n"
    "dp4   r10.x, c[8], r9;\n"
    "dp4   r10.y, c[9], r9;\n"
    "dp4   r10.z, c[10], r9;\n"
    "mov   r10.w, c[16].w;\n"
    "mov  oFog.x, c[16].x;\n"
    "add   r0, c[12], -r9;\n"
    "dp3   r0.w, r0, r0;\n"
    "rsq   r1.w, r0.w;\n"
    "dst   r2, r0.w, r1.w;\n"
    "dp3   r2.w, r2, c[14];\n"
    "rcp   r2.w, r2.w;\n"
    "mov   oD1.xyzw, r2.wwww;\n"
    "mul   r0, r0, r1.w;\n"
    "mov   r0.w, c[16].w;\n"
    "add   r2, c[13],-r9;\n"
    "dp3   r2.w, r2, r2;\n"
    "rsq   r2.w, r2.w;\n"
    "mul   r2, r2, r2.w;\n"
    "mov   r2.w, c[16].w;\n"
    "add   r2, r2, r0;mov   r2.w, c[16].w;\n"
    "mov   r9, c[16].xxxw;\n"
    "mov oD0, r9;\n"
    "mov   oT0, v7;\n"
    "mov   oT3, v7;\n"
    "dp3   r1.x, r4.xyz, r0;\n"
    "dp3   r1.y, r5.xyz, r0;\n"
    "dp3   r1.z, r6.xyz, r0;\n"
    "mov   r1.w, c[16].w;\n"
    "mov   oT1, r1;\n"
    "dp3   r1.x, r4.xyz, r2;\n"
    "dp3   r1.y, r5.xyz, r2;\n"
    "dp3   r1.z, r6.xyz, r2;\n"
    "mov   r1.w, c[16].w;\n"
    "mov   oT2, r1;\n";

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

char cSkinSphereMapVP[] =
    "vs.1.1 \n"
    "mul   r0, v2, c[16].zzzz;\n"
    "mov   a0.x, r0.x;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "dp3   r5.x, v3, c[a0.x + 18];\n"
    "dp3   r5.y, v3, c[a0.x + 19];\n"
    "dp3   r5.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.y;\n"
    "dp4   r2.x, v0, c[a0.x + 18];\n"
    "dp4   r2.y, v0, c[a0.x + 19];\n"
    "dp4   r2.z, v0, c[a0.x + 20];\n"
    "dp3   r6.x, v3, c[a0.x + 18];\n"
    "dp3   r6.y, v3, c[a0.x + 19];\n"
    "dp3   r6.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.z;\n"
    "dp4   r3.x, v0, c[a0.x + 18];\n"
    "dp4   r3.y, v0, c[a0.x + 19];\n"
    "dp4   r3.z, v0, c[a0.x + 20];\n"
    "dp3   r7.x, v3, c[a0.x + 18];\n"
    "dp3   r7.y, v3, c[a0.x + 19];\n"
    "dp3   r7.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.w;\n"
    "dp4   r4.x, v0, c[a0.x + 18];\n"
    "dp4   r4.y, v0, c[a0.x + 19];\n"
    "dp4   r4.z, v0, c[a0.x + 20];\n"
    "dp3   r8.x, v3, c[a0.x + 18];\n"
    "dp3   r8.y, v3, c[a0.x + 19];\n"
    "dp3   r8.z, v3, c[a0.x + 20];\n"
    "mul   r9, v1.x, r1.xyz;\n"
    "mad   r9, r2.xyz, v1.y, r9;\n"
    "mad   r9, r3.xyz, v1.z, r9;\n"
    "mad   r9, r4.xyz, v1.w, r9;\n"
    "mov   r9.w, c[16].w;\n"
    "mul   r10, v1.xxx, r5.xyz;\n"
    "mad   r10, r6.xyz, v1.y, r10;\n"
    "mad   r10, r7.xyz, v1.z, r10;\n"
    "mad   r10, r8.xyz, v1.w, r10;\n"
    "dp4   oPos.x, c[0], r9;\n"
    "dp4   oPos.y, c[1], r9;\n"
    "dp4   oPos.z, c[2], r9;\n"
    "dp4   oPos.w, c[3], r9;\n"
    "dp3   r11.x, c[8], r10;\n"
    "dp3   r11.y, c[9], r10;\n"
    "dp3   r11.z, c[10], r10;\n"
    "mov   r11.w, c[16].w;\n"
    "dp3   r11.w, r11, r11;\n"
    "rsq   r11.w, r11.w;\n"
    "mul   r11, r11, r11.w;\n"
    "dp4   r10.x, c[8], r9;\n"
    "dp4   r10.y, c[9], r9;\n"
    "dp4   r10.z, c[10], r9;\n"
    "mov   r10.w, c[16].w;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "mov   r9, c[16].xxxw;\n"
    "mov   oT0, v7;dp3   r0.x, -r10, r11;\n"
    "mul   r1, r0.x, r11;\n"
    "mul   r2, r1, c[16].y;\n"
    "add   r4, r10, r2;\n"
    "add   r4.z, r4.z, c[16].w;\n"
    "dp3   r5.x, r4, r4;\n"
    "rsq   r5.y, r5.x;\n"
    "rcp   r5.z, r5.y;\n"
    "mul   r5.w, r5.z, c[16].y;\n"
    "rcp   r5.x, r5.w;\n"
    "mov   r6.xyzw, c[16].xxxw;\n"
    "mad   r6.x, r3.x, r5.x, c[17].w;\n"
    "mad   r6.y, r3.y, r5.x, c[17].w;\n"
    "mov\toT1, r6;\n";

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

char cSkinCubeMapVP[] =
    "vs.1.1 \n"
    "mul   r0, v2, c[16].zzzz;\n"
    "mov   a0.x, r0.x;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "dp3   r5.x, v3, c[a0.x + 18];\n"
    "dp3   r5.y, v3, c[a0.x + 19];\n"
    "dp3   r5.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.y;\n"
    "dp4   r2.x, v0, c[a0.x + 18];\n"
    "dp4   r2.y, v0, c[a0.x + 19];\n"
    "dp4   r2.z, v0, c[a0.x + 20];\n"
    "dp3   r6.x, v3, c[a0.x + 18];\n"
    "dp3   r6.y, v3, c[a0.x + 19];\n"
    "dp3   r6.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.z;\n"
    "dp4   r3.x, v0, c[a0.x + 18];\n"
    "dp4   r3.y, v0, c[a0.x + 19];\n"
    "dp4   r3.z, v0, c[a0.x + 20];\n"
    "dp3   r7.x, v3, c[a0.x + 18];\n"
    "dp3   r7.y, v3, c[a0.x + 19];\n"
    "dp3   r7.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.w;\n"
    "dp4   r4.x, v0, c[a0.x + 18];\n"
    "dp4   r4.y, v0, c[a0.x + 19];\n"
    "dp4   r4.z, v0, c[a0.x + 20];\n"
    "dp3   r8.x, v3, c[a0.x + 18];\n"
    "dp3   r8.y, v3, c[a0.x + 19];\n"
    "dp3   r8.z, v3, c[a0.x + 20];\n"
    "mul   r9, v1.x, r1.xyz;\n"
    "mad   r9, r2.xyz, v1.y, r9;\n"
    "mad   r9, r3.xyz, v1.z, r9;\n"
    "mad   r9, r4.xyz, v1.w, r9;\n"
    "mov   r9.w, c[16].w;\n"
    "mul   r10, v1.x, r5.xyz;\n"
    "mad   r10, r6.xyz, v1.y, r10;\n"
    "mad   r10, r7.xyz, v1.z, r10;\n"
    "mad   r10, r8.xyz, v1.w, r10;\n"
    "dp4   oPos.x, c[0], r9;\n"
    "dp4   oPos.y, c[1], r9;\n"
    "dp4   oPos.z, c[2], r9;\n"
    "dp4   oPos.w, c[3], r9;\n"
    "dp3   r11.x, c[8], r10;\n"
    "dp3   r11.y, c[9], r10;\n"
    "dp3   r11.z, c[10], r10;\n"
    "mov   r11.w, c[16].w;\n"
    "dp3   r11.w, r11, r11;\n"
    "rsq   r11.w, r11.w;\n"
    "mul   r11, r11, r11.w;\n"
    "dp4   r10.x, c[8], r9;\n"
    "dp4   r10.y, c[9], r9;\n"
    "dp4   r10.z, c[10], r9;\n"
    "mov   r10.w, c[16].w;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "mov   r9, c[16].xxxw;\n"
    "mov   oT0, v7;\n"
    "dp3   r0.x, -r10, r11;\n"
    "mul   r1, r0.x, r11;\n"
    "mul   r2, r1, c[16].y;\n"
    "add   r3, r10, r2;\n"
    "dp3   r3.w, r3, r3;\n"
    "rsq   r3.w, r3.w;\n"
    "mul   r3, r3, r3.w;\n"
    "dp3   r4.x, c[92], r3;\n"
    "dp3   r4.y, c[91], r3;\n"
    "dp3   r4.z, c[90], r3;\n"
    "mov   r4.w, c[16].w;\n"
    "mov   oT1, r4;";

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

char cBumpMapOnePassVP[] =
    "vs.1.1;\n"
    "dp4   oPos.x, c[0], v0;\n"
    "dp4   oPos.y, c[1], v0;\n"
    "dp4   oPos.z, c[2], v0;\n"
    "dp4   oPos.w, c[3], v0;\n"
    "add  r0, c[12], -v0;\n"
    "dp3   r0.w, r0, r0;\n"
    "rsq   r1.w, r0.w;\n"
    "dst r2, r0.w, r1.w;\n"
    "dp3 r2.w, r2, c[14];\n"
    "rcp r2.w, r2.w;\n"
    "mov   r4.xyzw, r2.wwww;\n"
    "mul   r0, r0, r1.w;\n"
    "mov   r0.w, c[17].x;\n"
    "dp3   r1.x, v11, r0;\n"
    "dp3   r1.y, v12, r0;\n"
    "dp3   r1.z, v13, r0;\n"
    "mov   r1.w, c[17].x;\n"
    "mov   oT1, r1;\n"
    "add r2, c[13],-v0;\n"
    "dp3   r2.w, r2, r2;\n"
    "rsq   r2.w, r2.w;\n"
    "mul   r2, r2, r2.w;\n"
    "mov   r2.w, c[17].x;\n"
    "add r2, r2,r0;\n"
    "mov r2.w, c[17].x;\n"
    "dp3   r1.x, v11, r2;\n"
    "dp3   r1.y, v12, r2;\n"
    "dp3   r1.z, v13, r2;\n"
    "mov   r1.w, c[17].x;\n"
    "mov   oT2, r1;\n"
    "dp4   r10.x, c[8], v0;\n"
    "dp4   r10.y, c[9], v0;\n"
    "dp4   r10.z, c[10], v0;\n"
    "dp4   r10.w, c[11], v0;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "dp3   r11.x, c[8], v3;\n"
    "dp3   r11.y, c[9], v3;\n"
    "dp3   r11.z, c[10], v3;\n"
    "mov   r9, v5;\n"
    "mov   oT0, v7;\n"
    "mov   oT3, v7;\n"
    "mov   oD1, r4;";

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

char cBumpMapDiffuseVP[] =
    "vs.1.1;\n"
    "dp4 oPos.x, c[0], v0;\n"
    "dp4 oPos.y, c[1], v0;\n"
    "dp4 oPos.z, c[2], v0;\n"
    "dp4 oPos.w, c[3], v0;\n"
    "add r0, c[12], -v0;\n"
    "dp3 r0.w, r0, r0;\n"
    "rsq r1.w, r0.w;\n"
    "dst r2, r0.w, r1.w;\n"
    "dp3 r2.w, r2, c[14];\n"
    "rcp r2.w, r2.w;\n"
    "mov r4, r2.w;\n"
    "mul r0, r0, r1.w;\n"
    "mov r0.w, c[17].x;\n"
    "dp3 r1.x, v11, r0;\n"
    "dp3 r1.y, v12, r0;\n"
    "dp3 r1.z, v13, r0;\n"
    "mov r1.w, c[17].x;\n"
    "mov oT1, r1;\n"
    "dp4 r10.x, c[8], v0;\n"
    "dp4 r10.y, c[9], v0;\n"
    "dp4 r10.z, c[10], v0;\n"
    "dp4 r10.w, c[11], v0;\n"
    "dp3 r11.x, c[8], v3;\n"
    "dp3 r11.y, c[9], v3;\n"
    "dp3 r11.z, c[10], v3;\n"
    "dp3 r1.w, r10, r10;\n"
    "rsq r1.w, r1.w;\n"
    "rcp r1.w, r1.w;\n"
    "mov oFog.x, -r1.w;\n"
    "mov oD1, r4;\n"
    "mov r9, c[15].wwwx;\n"
    "mov oT0, v7;\n"
    "mov oT2, v8;\n"
    "mov oT3, v7;\n";

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

char cBumpMapSpecularVP[] =
    "xvs.1.1;\n"
    "dp4 oPos.x, c[0], v0;\n"
    "dp4 oPos.y, c[1], v0;\n"
    "dp4 oPos.z, c[2], v0;\n"
    "dp4 oPos.w, c[3], v0;\n"
    "add r0, c[12], -v0;\n"
    "dp3 r0.w, r0, r0;\n"
    "rsq r1.w, r0.w;\n"
    "mul   r0, r0, r1.w;\n"
    "dst r2, r0.w, r1.w;\n"
    "dp3 r2.w, r2, c[14];\n"
    "rcp r2.w, r2.w;\n"
    "mov r4, r2.wwww;\n"
    "add r2, c[13], -v0;\n"
    "dp3 r2.w, r2, r2;\n"
    "rsq r2.w, r2.w;\n"
    "mul r2, r2, r2.w;\n"
    "mov r2.w, c[17].x;\n"
    "add r2, r2, r0;\n"
    "mov r2.w, c[17].x;\n"
    "dp3 r1.x, v11, r2;\n"
    "dp3 r1.y, v12, r2;\n"
    "dp3 r1.z, v13, r2;\n"
    "mov r1.w, c[17].x;\n"
    "mov oT1, r1;\n"
    "dp4 r10.x, c[8], v0;\n"
    "dp4 r10.y, c[9], v0;\n"
    "dp4 r10.z, c[10], v0;\n"
    "dp4 r10.w, c[11], v0;\n"
    "dp3 r11.x, c[8], v3;\n"
    "dp3 r11.y, c[9], v3;\n"
    "dp3 r11.z, c[10], v3;\n"
    "dp3 r1.w, r10, r10;\n"
    "rsq r1.w, r1.w;\n"
    "rcp r1.w, r1.w;\n"
    "mov oFog.x, -r1.w;\n"
    "mov oD1, r4;\n"
    "mov r9, c[15].wwwx;\n"
    "mov oT0, v7;\n"
    "mov oT2, v8;\n"
    "mov oT3, v7;\n";

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

char cBlurVP[] =
    "vs.1.1 \n"
    "dp4   oPos.x, c[0], v0;\n"
    "dp4   oPos.y, c[1], v0;\n"
    "dp4   oPos.z, c[2], v0;\n"
    "dp4   oPos.w, c[3], v0;\n"
    "mov a0.x, c[12];\n"
    "add oT0, v7, c[a0.x];\n"
    "add oT1, v7, c[a0.x+1];\n"
    "add oT2, v7, c[a0.x+2];\n"
    "add oT3, v7, c[a0.x+3];\n";

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

char cPhongVP[] =
    "vs.1.1 \n"
    "dp4   oPos.x, c[0], v0;\n"
    "dp4   oPos.y, c[1], v0;\n"
    "dp4   oPos.z, c[2], v0;\n"
    "dp4   oPos.w, c[3], v0;\n"
    "dp4   r10.x, c[8], v0;\n"
    "dp4   r10.y, c[9], v0;\n"
    "dp4   r10.z, c[10], v0;\n"
    "dp4   r10.w, c[11], v0;\n"
    "dp3   r1.w, r10, r10;\n"
    "rsq   r1.w, r1.w;\n"
    "rcp   r1.w, r1.w;\n"
    "mov  oFog.x, -r1.w;\n"
    "dp3   r11.x, c[8], v3;\n"
    "dp3   r11.y, c[9], v3;\n"
    "dp3   r11.z, c[10], v3;\n"
    "mov oT3, v3\n"
    "add  r0, c[12], -v0;\n"
    "mov oT2, r0.xyz;\n"
    "rcp r2, c[14].y;\n"
    "mul r3, r0, r2.y;\n"
    "mul r3, c[15].x, r3;\n"
    "add r3.xy, c[15].y, r3;\n"
    "dp3 r4.x, r3, r3;\n"
    "mul r5, r3, c[15].wwyy;\n"
    "dp3 r5.x, r5, r5;\n"
    "rcp r5.x, r5.x;\n"
    "mul r4.x, r4.x, r5.x;\n"
    "mul r3, r3, r4.x;\n"
    "mov oT1.xy, r3;\n"
    "mov oT0, v7;\n";

unsigned long cShadowTriMeshVariantVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

char cShadowTriMeshVP[] =
    "xvs.1.1;\n"
    "sge  r9.w, v0.z, c[12].w;\n"
    "mov r11, c[16];\n"
    "dp4   r10.x, c[4], r11;\n"
    "dp4   r10.y, c[5], r11;\n"
    "dp4   r10.z, c[6], r11;\n"
    "dp3 r6.w, v3, -r10;\n"
    "sge r9.y, r6.w, c[12].y;\n"
    "add r9.w, r9.y, r9.w;\n"
    "sge r9.w, r9.w, c[12].x;\n"
    "add r9.z, c[12].x, -r9.w;\n"
    "mov r7, c[14];\n"
    "dp4   r3.x, c[0], v0;\n"
    "dp4   r3.y, c[1], v0;\n"
    "dp4   r3.z, c[2], v0;\n"
    "dp4   r3.w, c[3], v0;\n"
    "mul r7, r7, r9.w;\n"
    "mul r3, r3, r9.z;\n"
    "add r4, r3, r3;\n"
    "sub r7, r4, r7;\n"
    "add oPos, r3, r7;\n";

char cShadowTriMeshVariantVP[] =
    "xvs.1.1;\n"
    "sge  r9.w, v0.z, c[15].w;\n"
    "slt  r9.z, v0.z, c[15].w;\n"
    "dp3 r6.w, c[14], -v3;\n"
    "sge r9.y, r6.w, c[12].y;\n"
    "add r9.w, r9.y, r9.w;\n"
    "sge r9.w, r9.w, c[12].x;\n"
    "slt  r9.z, r9.w, c[12].x;\n"
    "mov r8, -c[14];\n"
    "mov r10, v0;\n"
    "add r10, r10, c[17];\n"
    "dp4   r3.x, c[0], r10;\n"
    "dp4   r3.y, c[1], r10;\n"
    "dp4   r3.z, c[2], r10;\n"
    "dp4   r3.w, c[3], r10;\n"
    "dp4   r7.x, c[0], r8;\n"
    "dp4   r7.y, c[1], r8;\n"
    "dp4   r7.z, c[2], r8;\n"
    "dp4   r7 .w, c[3], r8;\n"
    "mul r7, r7, r9.w;\n"
    "mul r3, r3, r9.z;\n"
    "add oPos, r3, r7;\n"
    "mov oD0, c[12].zzzz;\n"
    "mov oB0, c[12].xxxx;\n";

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

char cSkinShadowVariantVP[] =
    "xvs.1.1 \n"
    "mul   r0, v2, c[16].zzzz;\n"
    "mov   a0.x, r0.x;\n"
    "dp4   r1.x, v0, c[a0.x + 18];\n"
    "dp4   r1.y, v0, c[a0.x + 19];\n"
    "dp4   r1.z, v0, c[a0.x + 20];\n"
    "dp3   r5.x, v3, c[a0.x + 18];\n"
    "dp3   r5.y, v3, c[a0.x + 19];\n"
    "dp3   r5.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.y;\n"
    "dp4   r2.x, v0, c[a0.x + 18];\n"
    "dp4   r2.y, v0, c[a0.x + 19];\n"
    "dp4   r2.z, v0, c[a0.x + 20];\n"
    "dp3   r6.x, v3, c[a0.x + 18];\n"
    "dp3   r6.y, v3, c[a0.x + 19];\n"
    "dp3   r6.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.z;\n"
    "dp4   r3.x, v0, c[a0.x + 18];\n"
    "dp4   r3.y, v0, c[a0.x + 19];\n"
    "dp4   r3.z, v0, c[a0.x + 20];\n"
    "dp3   r7.x, v3, c[a0.x + 18];\n"
    "dp3   r7.y, v3, c[a0.x + 19];\n"
    "dp3   r7.z, v3, c[a0.x + 20];\n"
    "mov   a0.x, r0.w;\n"
    "dp4   r4.x, v0, c[a0.x + 18];\n"
    "dp4   r4.y, v0, c[a0.x + 19];\n"
    "dp4   r4.z, v0, c[a0.x + 20];\n"
    "dp3   r8.x, v3, c[a0.x + 18];\n"
    "dp3   r8.y, v3, c[a0.x + 19];\n"
    "dp3   r8.z, v3, c[a0.x + 20];\n"
    "mul   r9, v1.x, r1.xyz;\n"
    "mad   r9, r2.xyz, v1.y, r9;\n"
    "mad   r9, r3.xyz, v1.z, r9;\n"
    "mad   r9, r4.xyz, v1.w, r9;\n"
    "mov   r9.w, c[16].w;\n"
    "mul   r10, v1.xxx, r5.xyz;\n"
    "mad   r10, r6.xyz, v1.y, r10;\n"
    "mad   r10, r7.xyz, v1.z, r10;\n"
    "mad   r10, r8.xyz, v1.w, r10;\n"
    "add r9, r9, c[17];\n"
    "dp4   r3.x, c[0], r9;\n"
    "dp4   r3.y, c[1], r9;\n"
    "dp4   r3.z, c[2], r9;\n"
    "dp4   r3.w, c[3], r9;\n"
    "sge  r8.w, v0.z, c[15].w;\n"
    "slt  r8.z, v0.z, c[15].w;\n"
    "dp3 r6.w, c[14], -r10;\n"
    "sge r8.y, r6.w, c[12].y;\n"
    "add r8.w, r8.y, r8.w;\n"
    "sge r8.w, r8.w, c[12].x;\n"
    "slt  r8.z, r8.w, c[12].x;\n"
    "mov r6, -c[14];\n"
    "dp4   r7.x, c[0], r6;\n"
    "dp4   r7.y, c[1], r6;\n"
    "dp4   r7.z, c[2], r6;\n"
    "dp4   r7.w, c[3], r6;\n"
    "mul r7, r7, r8.w;\n"
    "mul r3, r3, r8.z;\n"
    "add oPos, r7, r3;\n"
    "mov oD0, c[12].xyyx;\n";
#endif
