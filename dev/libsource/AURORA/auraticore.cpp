#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "aurrendergl.h"
#include "aurglext.h"
#include "aurtexman.h"
#include "../math3d/math3d.h"

unsigned int lightmapFragmentShaderID = 0;
unsigned int envmapFragmentShaderID = 0;
unsigned int envmapFogFragmentShaderID = 0;
unsigned int bumpmapFragmentShaderID = 0;
unsigned int normalmapFragmentShaderID = 0;
unsigned int envmapLightmapFragmentShaderID = 0;
unsigned int bumpyshinyFragmentShaderID = 0;
unsigned int fogFragmentShaderID = 0;
unsigned int standardFogFragmentShaderID = 0;

unsigned int envmapCubeFragmentShaderID = 0;
unsigned int envmapFogCubeFragmentShaderID = 0;
unsigned int envmapLightmapCubeFragmentShaderID = 0;

unsigned int bumpmapVPARB_ID = 0;
unsigned int dummyVPARB_ID = 0;
int I_vLightPosPartSpace = 0;
int I_vCameraPosPartSpace = 0;
int I_vCameraPosWorldSpace = 0;
int I_vLightRadius = 0;

char* szBumpyShinyVPARB = 0;
char* szBumpyShinySkinVPARB = 0;
unsigned long* szNormalmapVPARBDeclaration = 0;

enum
{
    GL_PROGRAM_ERROR_POSITION_ARB = 0x864B,
    GL_PROGRAM_ERROR_STRING_ARB = 0x8874
};

#ifndef _XBOX

void GetGLError()
{
    int nError = glGetError();

    if (nError != 0)
    {
        gluErrorString(nError);
    }
}

void GLRender::InitializeATIFragmentShaders()
{
    GLenum nError;

    glEnable(GL_FRAGMENT_SHADER_ATI);
    lightmapFragmentShaderID = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(lightmapFragmentShaderID);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_2_ATI, 0, 0, GL_PRIMARY_COLOR_ARB, 0, 0, GL_REG_1_ATI, 0, 0);
    glAlphaFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 0, GL_PRIMARY_COLOR_ARB, 0, 0, GL_REG_0_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_REG_2_ATI, 0, 0);
    glEndFragmentShaderATI();

    envmapFogFragmentShaderID = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(envmapFogFragmentShaderID);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 7, 0, GL_REG_0_ATI, 0, 0, GL_PRIMARY_COLOR_ARB, 0, 0);
    glColorFragmentOp3ATI(35177, GL_REG_0_ATI, 7, 0, GL_REG_0_ATI, GL_ALPHA, 0, GL_REG_0_ATI, 0, 0,
                          GL_REG_1_ATI, 0, 0);
    glColorFragmentOp3ATI(35177, GL_REG_0_ATI, 7, 0, GL_SECONDARY_INTERPOLATOR_ATI, GL_RED, 0, GL_CON_1_ATI, 0, 0,
                          GL_REG_0_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_CON_0_ATI, 0, 0);
    glEndFragmentShaderATI();

    envmapFragmentShaderID = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(envmapFragmentShaderID);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 7, 0, GL_REG_0_ATI, 0, 0, GL_PRIMARY_COLOR_ARB, 0, 0);
    glColorFragmentOp3ATI(35177, GL_REG_0_ATI, 7, 0, GL_REG_0_ATI, GL_ALPHA, 0, GL_REG_0_ATI, 0, 0,
                          GL_REG_1_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_CON_0_ATI, 0, 0);
    glEndFragmentShaderATI();

    bumpmapFragmentShaderID = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(bumpmapFragmentShaderID);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_3_ATI, GL_TEXTURE3_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_5_ATI, 0, 0, GL_REG_0_ATI, 0, 9, GL_REG_2_ATI, 0, 9);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_4_ATI, 0, 0, GL_REG_0_ATI, 0, 9, GL_REG_1_ATI, 0, 9);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 0, 0, GL_REG_5_ATI, 0, 0, GL_REG_5_ATI, 0, 0);
    glColorFragmentOp1ATI(GL_MOV_ATI, GL_REG_5_ATI, 0, 0, GL_REG_5_ATI, GL_BLUE, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 0, 0, GL_REG_4_ATI, 0, 0, GL_CON_1_ATI, 0, 0);
    glAlphaFragmentOp2ATI(GL_MUL_ATI, GL_REG_5_ATI, 0, GL_REG_5_ATI, GL_RED, 0, GL_REG_5_ATI, GL_RED, 0);
    glPassTexCoordATI(GL_REG_0_ATI, GL_REG_0_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_1_ATI, GL_REG_1_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_2_ATI, GL_REG_2_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_3_ATI, GL_REG_3_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_4_ATI, GL_REG_4_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_5_ATI, GL_REG_5_ATI, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 0, 0, GL_REG_4_ATI, 0, 0, GL_SECONDARY_INTERPOLATOR_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_1_ATI, 0, 0, GL_SECONDARY_INTERPOLATOR_ATI, 0, 0, GL_CON_0_ATI, 0, 0);
    glAlphaFragmentOp2ATI(GL_MUL_ATI, GL_REG_1_ATI, 0, GL_REG_5_ATI, GL_ALPHA, 0, GL_REG_5_ATI, GL_ALPHA, 0);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, 0, 64, GL_PRIMARY_COLOR_ARB, 0, 0, GL_REG_0_ATI, 0, 0);
    glAlphaFragmentOp2ATI(GL_MUL_ATI, GL_REG_1_ATI, 0, GL_REG_1_ATI, GL_ALPHA, 0, GL_REG_1_ATI, GL_ALPHA, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_REG_3_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_1_ATI, 0, 0, GL_REG_1_ATI, GL_ALPHA, 0, GL_REG_1_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_1_ATI, 0, 0, GL_REG_1_ATI, 0, 0, GL_REG_3_ATI, GL_ALPHA, 2);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, 0, 64, GL_REG_1_ATI, 0, 0, GL_REG_0_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_PRIMARY_COLOR_ARB, GL_ALPHA, 0);
    glEndFragmentShaderATI();

    nError = glGetError();
    if (nError != 0)
    {
        gluErrorString(nError);
    }

    normalmapFragmentShaderID = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(normalmapFragmentShaderID);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_3_ATI, GL_TEXTURE3_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_5_ATI, 0, 0, GL_REG_1_ATI, 0, 9, GL_REG_3_ATI, 0, 9);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_4_ATI, 0, 0, GL_REG_1_ATI, 0, 9, GL_REG_2_ATI, 0, 9);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_1_ATI, 0, 0, GL_REG_5_ATI, 0, 0, GL_REG_5_ATI, 0, 0);
    glColorFragmentOp1ATI(GL_MOV_ATI, GL_REG_5_ATI, 0, 0, GL_REG_5_ATI, GL_BLUE, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_1_ATI, 0, 0, GL_REG_4_ATI, 0, 0, GL_CON_1_ATI, 0, 0);
    glAlphaFragmentOp2ATI(GL_MUL_ATI, GL_REG_5_ATI, 0, GL_REG_5_ATI, GL_RED, 0, GL_REG_5_ATI, GL_RED, 0);
    glPassTexCoordATI(GL_REG_0_ATI, GL_REG_0_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_1_ATI, GL_REG_1_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_2_ATI, GL_REG_2_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_3_ATI, GL_REG_3_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_4_ATI, GL_REG_4_ATI, GL_SWIZZLE_STR_ATI);
    glPassTexCoordATI(GL_REG_5_ATI, GL_REG_5_ATI, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_1_ATI, 0, 0, GL_REG_4_ATI, 0, 0, GL_SECONDARY_INTERPOLATOR_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_2_ATI, 0, 0, GL_SECONDARY_INTERPOLATOR_ATI, 0, 0, GL_CON_0_ATI, 0, 0);
    glAlphaFragmentOp2ATI(GL_MUL_ATI, GL_REG_2_ATI, 0, GL_REG_5_ATI, GL_ALPHA, 0, GL_REG_5_ATI, GL_ALPHA, 0);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_1_ATI, 0, 64, GL_PRIMARY_COLOR_ARB, 0, 0, GL_REG_1_ATI, 0, 0);
    glAlphaFragmentOp2ATI(GL_MUL_ATI, GL_REG_2_ATI, 0, GL_REG_2_ATI, GL_ALPHA, 0, GL_REG_2_ATI, GL_ALPHA, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_1_ATI, 0, 0, GL_REG_1_ATI, 0, 0, GL_REG_0_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_2_ATI, 0, 0, GL_REG_2_ATI, GL_ALPHA, 0, GL_REG_2_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_2_ATI, 0, 0, GL_REG_2_ATI, 0, 0, GL_REG_0_ATI, GL_ALPHA, 2);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, 0, 64, GL_REG_2_ATI, 0, 0, GL_REG_1_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_PRIMARY_COLOR_ARB, GL_ALPHA, 0);
    glEndFragmentShaderATI();

    nError = glGetError();
    if (nError != 0)
    {
        gluErrorString(nError);
    }

    envmapLightmapFragmentShaderID = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(envmapLightmapFragmentShaderID);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_3_ATI, 7, 0, GL_PRIMARY_COLOR_ARB, 0, 0, GL_REG_0_ATI, 0, 0);
    glColorFragmentOp3ATI(35176, GL_REG_3_ATI, 7, 0, GL_REG_1_ATI, 0, 0, GL_REG_0_ATI, 0, 0, GL_REG_3_ATI,
                          0, 0);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_4_ATI, 7, 0, GL_REG_2_ATI, 0, 0, GL_REG_0_ATI, GL_ALPHA, 2);
    glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, 7, 0, GL_REG_3_ATI, 0, 0, GL_REG_4_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_CON_0_ATI, GL_ALPHA, 0);
    glEndFragmentShaderATI();

    bumpyshinyFragmentShaderID = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(bumpyshinyFragmentShaderID);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_1_ATI, GL_TEXTURE1_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_2_ATI, GL_TEXTURE2_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_3_ATI, GL_TEXTURE3_ARB, GL_SWIZZLE_STR_ATI);
    glSampleMapATI(GL_REG_4_ATI, GL_TEXTURE4_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_5_ATI, GL_RED_BIT_ATI, 0, GL_REG_1_ATI, 0, 9, GL_REG_0_ATI, 0, 9);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_5_ATI, GL_GREEN_BIT_ATI, 0, GL_REG_2_ATI, 0, 9, GL_REG_0_ATI, 0, 9);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_5_ATI, 4, 0, GL_REG_3_ATI, 0, 9, GL_REG_0_ATI, 0, 9);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_3_ATI, 0, 1, GL_REG_5_ATI, 0, 0, GL_REG_4_ATI, 0, 9);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_3_ATI, 0, 0, GL_REG_5_ATI, 0, 0, GL_REG_3_ATI, 0, 0);
    glColorFragmentOp2ATI(GL_DOT3_ATI, GL_REG_2_ATI, 0, 0, GL_REG_5_ATI, 0, 0, GL_REG_5_ATI, 0, 0);
    glColorFragmentOp3ATI(35176, GL_REG_3_ATI, 0, 0, GL_REG_4_ATI, 0, 13, GL_REG_2_ATI, 0, 0, GL_REG_3_ATI,
                          0, 0);
    glSampleMapATI(GL_REG_5_ATI, GL_REG_3_ATI, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, 0, GL_REG_5_ATI, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_REG_5_ATI, 0, 0);
    glAlphaFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 0, GL_REG_0_ATI, 0, 0, GL_CON_0_ATI, 0, 0);
    glEndFragmentShaderATI();

    nError = glGetError();
    if (nError != 0)
    {
        gluErrorString(nError);
    }

    fogFragmentShaderID = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(fogFragmentShaderID);
    glBeginFragmentShaderATI();
    glColorFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, 0, GL_PRIMARY_COLOR_ARB, 0, 0);
    glAlphaFragmentOp1ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_PRIMARY_COLOR_ARB, GL_ALPHA, 0);
    glEndFragmentShaderATI();
    glDisable(GL_FRAGMENT_SHADER_ATI);
    AurGetGLError();

    standardFogFragmentShaderID = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(standardFogFragmentShaderID);
    glBeginFragmentShaderATI();
    glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
    glColorFragmentOp2ATI(GL_MUL_ATI, GL_REG_0_ATI, 0, 0, GL_PRIMARY_COLOR_ARB, 0, 0, GL_REG_0_ATI, 0, 0);
    glAlphaFragmentOp2ATI(GL_MOV_ATI, GL_REG_0_ATI, 0, GL_PRIMARY_COLOR_ARB, 0, 0, GL_REG_0_ATI, 0, 0);
    glColorFragmentOp3ATI(35177, GL_REG_0_ATI, 7, 0, GL_SECONDARY_INTERPOLATOR_ATI, GL_RED, 0, GL_CON_1_ATI, 0, 0,
                          GL_REG_0_ATI, 0, 0);
    glEndFragmentShaderATI();
    glDisable(GL_FRAGMENT_SHADER_ATI);
}
// PC 0x0049B3A0
void GLRender::SetFragmentShaderConstant0(Vector& vColor, float fAlpha)
{
    glSetFragmentShaderConstantATI(GL_CON_0_ATI, Homogenize(vColor, fAlpha));
}
// PC 0x0049B3C0
void GLRender::SetFragmentShaderConstant1(Vector& vColor, float fAlpha)
{
    glSetFragmentShaderConstantATI(GL_CON_1_ATI, Homogenize(vColor, fAlpha));
}
// PC 0x0049B3E0
void AurGetVertexProgramARBError(char* sError)
{
    GLint anParameters[10];
    const GLubyte* pError = glGetString(GL_PROGRAM_ERROR_STRING_ARB);

    if (*pError != 0)
    {
        glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, anParameters);
    }
}

void GLRender::EnableFragmentShaders(int nShader, CAurTexture* pTexture0,
                                     CAurTexture* pTexture1,
                                     CAurTexture* pTexture2,
                                     CAurTexture* pTexture3)
{
    switch (nShader)
    {
        case MODEL_SHADER_DISABLE:
            glBindFragmentShaderATI(0);
            glDisable(GL_FRAGMENT_SHADER_ATI);
            glEnable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLRender::SetActiveTexture(1);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLRender::SetActiveTexture(2);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLRender::SetActiveTexture(3);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLRender::SetActiveTexture(0);
            GLRender::SetCurrentStageTID(0, 0);
            GLRender::SetCurrentStageTID(1, 0);
            GLRender::SetCurrentStageTID(2, 0);
            GLRender::SetCurrentStageTID(3, 0);
            break;

        case MODEL_SHADER_STANDARD_FOG:
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            glEnable(GL_FRAGMENT_SHADER_ATI);
            glBindFragmentShaderATI(standardFogFragmentShaderID);
            break;

        case MODEL_SHADER_BUMP:
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_2D);
            pTexture3->Bind();
            glEnable(GL_FRAGMENT_SHADER_ATI);
            glBindFragmentShaderATI(bumpmapFragmentShaderID);
            break;

        case MODEL_SHADER_BUMP_RESET:
            glDisable(GL_FRAGMENT_SHADER_ATI);
            GLRender::SetActiveTexture(1);
            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            glDisable(GL_TEXTURE_2D);
            GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = 0;
            GLRender::SetActiveTexture(2);
            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            glDisable(GL_TEXTURE_2D);
            GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = 0;
            GLRender::SetActiveTexture(3);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = 0;
            glEnable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            break;

        case MODEL_SHADER_NORMAL_MAP:
            tex1on = 1;
            glDisable(GL_LIGHTING);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture2->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture3->Bind();
            glEnable(GL_FRAGMENT_SHADER_ATI);
            glBindFragmentShaderATI(normalmapFragmentShaderID);
            break;

        case MODEL_SHADER_ENVIRONMENT:
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            pTexture1->Bind();
            if (pTexture1->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            glEnable(GL_FRAGMENT_SHADER_ATI);
            glBindFragmentShaderATI(envmapFragmentShaderID);
            break;

        case MODEL_SHADER_ATI_ENVIRONMENT_FOG:
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            pTexture1->Bind();
            if (pTexture1->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            glEnable(GL_FRAGMENT_SHADER_ATI);
            glBindFragmentShaderATI(envmapFogFragmentShaderID);
            break;

        case MODEL_SHADER_NV1X_DISABLE:
            if (tex1on != 0)
            {
                GLRender::SetActiveTexture(1);
                if (envmapon != 0)
                {
                    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                    glDisable(GL_TEXTURE_GEN_S);
                    glDisable(GL_TEXTURE_GEN_T);
                    glDisable(GL_TEXTURE_GEN_R);
                    envmapon = 0;
                    GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = 0;
                }
                glDisable(GL_TEXTURE_2D);
                tex1on = 0;
            }
            GLRender::SetActiveTexture(0);
            glDisable(GL_FRAGMENT_SHADER_ATI);
            break;

        case MODEL_SHADER_NV1X_LIGHTMAP:
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture1->Bind();
            tex1on = 1;
            glEnable(GL_FRAGMENT_SHADER_ATI);
            glBindFragmentShaderATI(lightmapFragmentShaderID);
            break;

        case MODEL_SHADER_EMLM:
            envmapon = 1;
            tex1on = 1;
            GLRender::SetActiveTexture(0);
            pTexture0->Bind();
            GLRender::SetActiveTexture(2);
            pTexture1->Bind();
            if (pTexture1->IsCubeMap())
            {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
                glEnable(GL_TEXTURE_GEN_R);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);
            }
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_2D);
            pTexture2->Bind();
            glEnable(GL_FRAGMENT_SHADER_ATI);
            glBindFragmentShaderATI(envmapLightmapFragmentShaderID);
            break;

        case MODEL_SHADER_EMLM_DISABLE:
            GLRender::SetActiveTexture(1);
            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            glDisable(GL_TEXTURE_2D);
            GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = 0;
            GLRender::SetActiveTexture(2);
            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            glDisable(GL_TEXTURE_2D);
            GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = 0;
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_TEXTURE_GEN_R);
            glDisable(GL_FRAGMENT_SHADER_ATI);
            GLRender::SetActiveTexture(0);
            break;

        case MODEL_SHADER_BUMPY_SHINY_ENABLE:
            glEnable(GL_FRAGMENT_SHADER_ATI);
            glBindFragmentShaderATI(bumpyshinyFragmentShaderID);
            break;

        case MODEL_SHADER_TEXTURE_SHADER_BUMP:
            GLRender::MakeLookUpTexture(5.0);
            GLRender::SetActiveTexture(0);
            glEnable(GL_TEXTURE_2D);
            pTexture0->Bind();
            GLRender::SetActiveTexture(1);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            AurTextureGetNormalizationCubeMap()->Bind();
            GLRender::SetActiveTexture(2);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            AurTextureGetNormalizationCubeMap()->Bind();
            GLRender::SetActiveTexture(3);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            AurTextureGetNormalizationCubeMap()->Bind();
            GLRender::SetActiveTexture(4);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            AurTextureGetNormalizationCubeMap()->Bind();
            GLRender::SetActiveTexture(5);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            pTexture3->Bind();
            GLRender::SetActiveTexture(0);
            break;

        case MODEL_SHADER_FOG:
            glEnable(GL_FRAGMENT_SHADER_ATI);
            glBindFragmentShaderATI(fogFragmentShaderID);
            break;

    }
}
#endif

char szBumpmapVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "TEMP vReg10, vLength, vTemp;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "ALIAS vNormalWorld = vLength;\n"
    "ALIAS vFinalColour = vTemp;\n"
    "PARAM fogParams = state.fog.params;\n"
    "DP4   result.position.x, mvpMatrix[0], vertex.position;\n"
    "DP4   result.position.y, mvpMatrix[1], vertex.position;\n"
    "DP4   result.position.z, mvpMatrix[2], vertex.position;\n"
    "DP4   result.position.w, mvpMatrix[3], vertex.position;\n"
    "SUB  vReg10, program.env[12], vertex.position;\n"
    "DP3   vReg10.w, vReg10, vReg10;\n"
    "RSQ   vLength.w, vReg10.w;\n"
    "DST vTemp, vReg10.wwww, vLength.wwww;\n"
    "DP3 vTemp.w, vTemp, program.env[14];\n"
    "RCP vTemp.w, vTemp.w;\n"
    "MOV   result.color.secondary, vTemp.wwww;\n"
    "MUL   vReg10, vReg10, vLength.w;\n"
    "DP3   result.texcoord[1].x, vertex.attrib[12], vReg10;\n"
    "DP3   result.texcoord[1].y, vertex.attrib[13], vReg10;\n"
    "DP3   result.texcoord[1].z, vertex.attrib[14], vReg10;\n"
    "MOV   result.texcoord[1].w, 1;\n"
    "SUB vTemp, program.env[13], vertex.position;\n"
    "DP3   vTemp.w, vTemp, vTemp;\n"
    "RSQ   vTemp.w, vTemp.w;\n"
    "MUL   vTemp, vTemp, vTemp.w;\n"
    "ADD vTemp, vTemp,vReg10;\n"
    "DP3   result.texcoord[2].x, vertex.attrib[12], vTemp;\n"
    "DP3   result.texcoord[2].y, vertex.attrib[13], vTemp;\n"
    "DP3   result.texcoord[2].z, vertex.attrib[14], vTemp;\n"
    "MOV   result.texcoord[2].w, 1;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vertex.normal;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vertex.normal;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vertex.normal;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.texcoord[3], vertex.texcoord[0];\n"
    "DP4   vReg10.x, mvMatrix[0], vertex.position;\n"
    "DP4   vReg10.y, mvMatrix[1], vertex.position;\n"
    "DP4   vReg10.z, mvMatrix[2], vertex.position;\n"
    "DP4   vReg10.w, mvMatrix[3], vertex.position;\n"
    "DP3\tvReg0.x, vReg10, vReg10;\n"
    "RSQ\tvReg0.x, vReg0.x;\n"
    "RCP\tvReg0.x, vReg0.x;\n"
    "MOV result.fogcoord.x, vReg0.x;\n";

char szNormalmapVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "TEMP vReg10, vLength, vTemp;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "ALIAS vNormalWorld = vLength;\n"
    "ALIAS vFinalColour = vTemp;\n"
    "DP4   result.position.x, mvpMatrix[0], vertex.position;\n"
    "DP4   result.position.y, mvpMatrix[1], vertex.position;\n"
    "DP4   result.position.z, mvpMatrix[2], vertex.position;\n"
    "DP4   result.position.w, mvpMatrix[3], vertex.position;\n"
    "SUB  vReg10, program.env[12], vertex.position;\n"
    "DP3   vReg10.w, vReg10, vReg10;\n"
    "RSQ   vLength.w, vReg10.w;\n"
    "DST vTemp, vReg10.wwww, vLength.wwww;\n"
    "DP3 vTemp.w, vTemp, program.env[14];\n"
    "RCP vTemp.w, vTemp.w;\n"
    "MOV   result.color.secondary, vTemp.wwww;\n"
    "MUL   vReg10, vReg10, vLength.w;\n"
    "MOV   result.texcoord[2], vReg10;\n"
    "MOV   result.texcoord[2].w, 1;\n"
    "SUB vTemp, program.env[13], vertex.position;\n"
    "DP3   vTemp.w, vTemp, vTemp;\n"
    "RSQ   vTemp.w, vTemp.w;\n"
    "MUL   vTemp, vTemp, vTemp.w;\n"
    "ADD vTemp, vTemp,vReg10;\n"
    "MOV   result.texcoord[3], vTemp;\n"
    "MOV   result.texcoord[3].w, 1;\n"
    "DP4   vReg10.x, mvMatrix[0], vertex.position;\n"
    "DP4   vReg10.y, mvMatrix[1], vertex.position;\n"
    "DP4   vReg10.z, mvMatrix[2], vertex.position;\n"
    "DP4   vReg10.w, mvMatrix[3], vertex.position;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vertex.normal;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vertex.normal;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vertex.normal;\n"
    "DP3\tvTemp.x, vReg10, vReg10;\n"
    "RSQ\tvTemp.x, vTemp.x;\n"
    "RCP\tvTemp.x, vTemp.x;\n"
    "MOV result.fogcoord, vTemp.x;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.texcoord[1], vertex.texcoord[1];\n";

char szBumpyShinySetupVPARB[] =
    "!!ARBvp1.0ATTRIB vTex0 = vertex.texcoord[0];\n"
    "ATTRIB vTex4 = vertex.attrib[12];\n"
    "ATTRIB vTex5 = vertex.attrib[13];\n"
    "ATTRIB vTex6 = vertex.attrib[14];\n"
    "PARAM mvMatrix[4] = { state.matrix.modelview };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "TEMP vReg1, vReg3, vReg10;\n"
    "TEMP vReg0;\n"
    "TEMP vFinalColour;\n"
    "DP4   result.position.x, mvpMatrix[0], vertex.position;\n"
    "DP4   result.position.y, mvpMatrix[1], vertex.position;\n"
    "DP4   result.position.z, mvpMatrix[2], vertex.position;\n"
    "DP4   result.position.w, mvpMatrix[3], vertex.position;\n"
    "DP3   vReg1.x, mvMatrix[0], vTex4; DP3   vReg10.x, mvMatrix[0], vTex5; DP3   vReg3.x, mvMatrix[0], v"
    "Tex6; DP3   vReg1.y, mvMatrix[1], vTex4; DP3   vReg10.y, mvMatrix[1], vTex5; DP3   vReg3.y, mvMatrix"
    "[1], vTex6; DP3   vReg1.z, mvMatrix[2], vTex4; DP3   vReg10.z, mvMatrix[2], vTex5; DP3   vReg3.z, mv"
    "Matrix[2], vTex6; DP3   result.texcoord[1].x, program.env[92], vReg1; DP3   result.texcoord[1].y, pr"
    "ogram.env[92], vReg10; DP3   result.texcoord[1].z, program.env[92], vReg3; DP3   result.texcoord[2]."
    "x, program.env[91], vReg1; DP3   result.texcoord[2].y, program.env[91], vReg10; DP3   result.texcoor"
    "d[2].z, program.env[91], vReg3; DP3   result.texcoord[3].x, program.env[90], vReg1; DP3   result.tex"
    "coord[3].y, program.env[90], vReg10; DP3   result.texcoord[3].z, program.env[90], vReg3; DP4   vReg1"
    "0.x, vertex.position, mvMatrix[0] ;\n"
    "DP4   vReg10.y, vertex.position, mvMatrix[1] ;\n"
    "DP4   vReg10.z, vertex.position, mvMatrix[2] ;\n"
    "DP4   vReg10.w, vertex.position, mvMatrix[3] ;\n"
    "DP3\tvReg1.x, vReg10, vReg10;\n"
    "RSQ\tvReg1.x, vReg1.x;\n"
    "RCP\tvReg1.x, vReg1.x;\n"
    "MOV result.fogcoord, vReg1.x;\n"
    "RCP   vReg10.w, vReg10.w           ;\n"
    "MUL   vReg10.xyz, vReg10, -vReg10.w    ;\n"
    "MOV   result.texcoord[0], vTex0;\n"
    "MOV   result.color, program.env[15].x;\n"
    "\0"
    "DP3   result.texcoord[1].w, vReg10, program.env[92];\n"
    "DP3   result.texcoord[2].w, vReg10, program.env[91];\n"
    "DP3   result.texcoord[3].w, vReg10, program.env[90];\n";

char szBumpyShinyVPARB_NV[] =
    "DP3   result.texcoord[1].w, vReg10, program.env[92];\n"
    "DP3   result.texcoord[2].w, vReg10, program.env[91];\n"
    "DP3   result.texcoord[3].w, vReg10, program.env[90];\n";

char szBumpyShinyVPARB_ATI[] =
    "DP3   result.texcoord[4].x, vReg10, program.env[92];\n"
    "DP3   result.texcoord[4].y, vReg10, program.env[91];\n"
    "DP3   result.texcoord[4].z, vReg10, program.env[90];\n";

char szStandardVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "TEMP vTemp;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "TEMP vReg10;\n"
    "TEMP vNormalWorld;\n"
    "ALIAS vFinalColour = vTemp;\n"
    "DP4   result.position.x, mvpMatrix[0], vertex.position;\n"
    "DP4   result.position.y, mvpMatrix[1], vertex.position;\n"
    "DP4   result.position.z, mvpMatrix[2], vertex.position;\n"
    "DP4   result.position.w, mvpMatrix[3], vertex.position;\n"
    "DP4   vReg10.x, mvMatrix[0], vertex.position;\n"
    "DP4   vReg10.y, mvMatrix[1], vertex.position;\n"
    "DP4   vReg10.z, mvMatrix[2], vertex.position;\n"
    "DP4   vReg10.w, mvMatrix[3], vertex.position;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vertex.normal;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vertex.normal;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vertex.normal;\n"
    "DP3\tvReg0.x, vReg10, vReg10;\n"
    "RSQ\tvReg0.x, vReg0.x;\n"
    "RCP\tvReg0.x, vReg0.x;\n"
    "MOV result.fogcoord, vReg0.x;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.texcoord[1], vertex.texcoord[1];\n"
    "MOV   result.texcoord[2], vertex.texcoord[2];\n"
    "MOV   result.texcoord[3], vertex.texcoord[3];\n"
    "MOV result.color, program.env[15].wwwz;\n";

char szStandardCubeVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "TEMP vTemp;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3, vReg4;\n"
    "TEMP vReg10;\n"
    "TEMP vNormalWorld;\n"
    "ALIAS vFinalColour = vTemp;\n"
    "DP4   result.position.x, mvpMatrix[0], vertex.position;\n"
    "DP4   result.position.y, mvpMatrix[1], vertex.position;\n"
    "DP4   result.position.z, mvpMatrix[2], vertex.position;\n"
    "DP4   result.position.w, mvpMatrix[3], vertex.position;\n"
    "DP4   vReg10.x, mvMatrix[0], vertex.position;\n"
    "DP4   vReg10.y, mvMatrix[1], vertex.position;\n"
    "DP4   vReg10.z, mvMatrix[2], vertex.position;\n"
    "DP4   vReg10.w, mvMatrix[3], vertex.position;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vertex.normal;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vertex.normal;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vertex.normal;\n"
    "DP3\tvReg0.x, vReg10, vReg10;\n"
    "RSQ\tvReg0.x, vReg0.x;\n"
    "RCP\tvReg0.x, vReg0.x;\n"
    "MOV result.fogcoord, vReg0.x;\n"
    "DP3   vReg0.x, vNormalWorld, -vReg10;\n"
    "MUL   vReg1, vReg0.x, vNormalWorld;\n"
    "MUL   vReg2, vReg1, program.env[16].y;\n"
    "ADD   vReg3, vReg10, vReg2;\n"
    "DP3   vReg3.w, vReg3, vReg3;\n"
    "RSQ   vReg3.w, vReg3.w;\n"
    "MUL   vReg3, vReg3, vReg3.w;\n"
    "DP3   vReg4.x, program.env[92], vReg3;\n"
    "DP3   vReg4.y, program.env[91], vReg3;\n"
    "DP3   vReg4.z, program.env[90], vReg3;\n"
    "MOV   vReg4.w, program.env[16].w;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.texcoord[1], vReg4;\n"
    "MOV   result.texcoord[2], vertex.texcoord[2];\n"
    "MOV   result.texcoord[3], vertex.texcoord[3];\n"
    "MOV result.color, program.env[15].wwwz;\n";

char szStandardLightmapCubeVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "TEMP vTemp;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3, vReg4;\n"
    "TEMP vReg10;\n"
    "TEMP vNormalWorld;\n"
    "ALIAS vFinalColour = vTemp;\n"
    "DP4   result.position.x, mvpMatrix[0], vertex.position;\n"
    "DP4   result.position.y, mvpMatrix[1], vertex.position;\n"
    "DP4   result.position.z, mvpMatrix[2], vertex.position;\n"
    "DP4   result.position.w, mvpMatrix[3], vertex.position;\n"
    "DP4   vReg10.x, mvMatrix[0], vertex.position;\n"
    "DP4   vReg10.y, mvMatrix[1], vertex.position;\n"
    "DP4   vReg10.z, mvMatrix[2], vertex.position;\n"
    "DP4   vReg10.w, mvMatrix[3], vertex.position;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vertex.normal;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vertex.normal;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vertex.normal;\n"
    "DP3\tvReg0.x, vReg10, vReg10;\n"
    "RSQ\tvReg0.x, vReg0.x;\n"
    "RCP\tvReg0.x, vReg0.x;\n"
    "MOV result.fogcoord, vReg0.x;\n"
    "DP3   vReg0.x, vNormalWorld, -vReg10;\n"
    "MUL   vReg1, vReg0.x, vNormalWorld;\n"
    "MUL   vReg2, vReg1, program.env[16].y;\n"
    "ADD   vReg3, vReg10, vReg2;\n"
    "DP3   vReg3.w, vReg3, vReg3;\n"
    "RSQ   vReg3.w, vReg3.w;\n"
    "MUL   vReg3, vReg3, vReg3.w;\n"
    "DP3   vReg4.x, program.env[92], vReg3;\n"
    "DP3   vReg4.y, program.env[91], vReg3;\n"
    "DP3   vReg4.z, program.env[90], vReg3;\n"
    "MOV   vReg4.w, program.env[16].w;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.texcoord[1], vertex.texcoord[1];\n"
    "MOV   result.texcoord[2], vReg4;\n"
    "MOV   result.texcoord[3], vertex.texcoord[3];\n"
    "MOV result.color, program.env[15].wwwz;\n";

char szStandardFogVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "TEMP vTemp;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "TEMP vReg10;\n"
    "TEMP vNormalWorld;\n"
    "ALIAS vFinalColour = vTemp;\n"
    "DP4   result.position.x, mvpMatrix[0], vertex.position;\n"
    "DP4   result.position.y, mvpMatrix[1], vertex.position;\n"
    "DP4   result.position.z, mvpMatrix[2], vertex.position;\n"
    "DP4   result.position.w, mvpMatrix[3], vertex.position;\n"
    "DP4   vReg10.x, mvMatrix[0], vertex.position;\n"
    "DP4   vReg10.y, mvMatrix[1], vertex.position;\n"
    "DP4   vReg10.z, mvMatrix[2], vertex.position;\n"
    "DP4   vReg10.w, mvMatrix[3], vertex.position;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vertex.normal;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vertex.normal;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vertex.normal;\n"
    "DP3\tvReg0.x, vReg10, vReg10;\n"
    "RSQ\tvReg0.x, vReg0.x;\n"
    "RCP\tvReg0.x, vReg0.x;\n"
    "ADD\tvReg0.x, -state.fog.params.y, vReg0;\n"
    "MOV    result.color.xyz, state.fog.color;\n"
    "MUL\tresult.color.w, vReg0.x, state.fog.params.w;\n";

char szBumpyShinySkinSetupVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "ATTRIB vWght = vertex.attrib[1];\n"
    "ATTRIB vTex0 = vertex.texcoord[0];\n"
    "ATTRIB vTex4 = vertex.attrib[12];\n"
    "ATTRIB vTex5 = vertex.attrib[13];\n"
    "ATTRIB vTex6 = vertex.attrib[14];\n"
    "PARAM boneArray[51] = {program.env[18..68] };\n"
    "ADDRESS A0;\n"
    "TEMP vReg0, vReg1, vReg2,  vReg3, vReg4, vReg5, vReg6, vReg9, vReg10;\n"
    "ALIAS vFinalColour = vReg9;\n"
    "MUL   vReg0, vertex.attrib[4], program.env[16].zzzz;\n"
    "ARL   A0.x, vReg0.x;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MUL   vReg9, vWght.x, vReg1;\n"
    "DP3   vReg1.x, vTex4, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex4, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex4, boneArray[A0.x + 2];\n"
    "MUL   vReg4, vWght.x, vReg1;\n"
    "DP3   vReg1.x, vTex5, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex5, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex5, boneArray[A0.x + 2];\n"
    "MUL   vReg5, vWght.x, vReg1;\n"
    "DP3   vReg1.x, vTex6, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex6, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex6, boneArray[A0.x + 2];\n"
    "MUL   vReg6, vWght.x, vReg1;ARL   A0.x, vReg0.y;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MAD   vReg9, vReg1, vWght.y, vReg9;\n"
    "DP3   vReg1.x, vTex4, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex4, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex4, boneArray[A0.x + 2];\n"
    "MAD   vReg4, vWght.y, vReg1, vReg4;\n"
    "DP3   vReg1.x, vTex5, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex5, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex5, boneArray[A0.x + 2];\n"
    "MAD   vReg5, vWght.y, vReg1, vReg5;\n"
    "DP3   vReg1.x, vTex6, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex6,  boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex6, boneArray[A0.x + 2];\n"
    "MAD   vReg6, vWght.y, vReg1, vReg6;ARL   A0.x, vReg0.z;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MAD   vReg9, vReg1, vWght.z, vReg9;\n"
    "DP3   vReg1.x, vTex4, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex4, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex4, boneArray[A0.x + 2];\n"
    "MAD   vReg4, vWght.z, vReg1, vReg4;\n"
    "DP3   vReg1.x, vTex5, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex5, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex5, boneArray[A0.x + 2];\n"
    "MAD   vReg5, vWght.z, vReg1, vReg5;\n"
    "DP3   vReg1.x, vTex6, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex6, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex6, boneArray[A0.x + 2];\n"
    "MAD   vReg6, vWght.z, vReg1, vReg6;ARL   A0.x, vReg0.w;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MAD   vReg9, vReg1, vWght.w, vReg9;\n"
    "DP3   vReg1.x, vTex4, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex4, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex4, boneArray[A0.x + 2];\n"
    "MAD   vReg4, vWght.w, vReg1, vReg4;DP3   vReg1.x, vTex5, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex5, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex5, boneArray[A0.x + 2];\n"
    "MAD   vReg5, vWght.w, vReg1, vReg5;DP3   vReg1.x, vTex6, boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vTex6, boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vTex6, boneArray[A0.x + 2];\n"
    "MAD   vReg6, vWght.w, vReg1, vReg6;MOV   vReg9.w, program.env[16].w;\n"
    "DP4   result.position.x, mvpMatrix[0], vReg9;\n"
    "DP4   result.position.y, mvpMatrix[1], vReg9;\n"
    "DP4   result.position.z, mvpMatrix[2], vReg9;\n"
    "DP4   result.position.w, mvpMatrix[3], vReg9;\n"
    "DP3   vReg1.x, mvMatrix[0], vReg4; DP3   vReg2.x, mvMatrix[0], vReg5; DP3   vReg3.x, mvMatrix[0], vR"
    "eg6; DP3   vReg1.y, mvMatrix[1], vReg4; DP3   vReg2.y, mvMatrix[1], vReg5; DP3   vReg3.y, mvMatrix[1"
    "], vReg6; DP3   vReg1.z, mvMatrix[2], vReg4; DP3   vReg2.z, mvMatrix[2], vReg5; DP3   vReg3.z, mvMat"
    "rix[2], vReg6; DP3 vReg1.w, vReg1, vReg1;\n"
    "RSQ vReg1.w, vReg1.w;\n"
    "MUL vReg1, vReg1, vReg1.w;\n"
    "DP3 vReg2.w, vReg2, vReg2;\n"
    "RSQ vReg2.w, vReg2.w;\n"
    "MUL vReg2, vReg2, vReg2.w;\n"
    "DP3 vReg3.w, vReg3, vReg3;\n"
    "RSQ vReg3.w, vReg3.w;\n"
    "MUL vReg3, vReg3, vReg3.w;\n"
    "DP3   result.texcoord[1].x, program.env[92], vReg1; DP3   result.texcoord[1].y, program.env[92], vRe"
    "g2; DP3   result.texcoord[1].z, program.env[92], vReg3; DP3   result.texcoord[2].x, program.env[91],"
    " vReg1; DP3   result.texcoord[2].y, program.env[91], vReg2; DP3   result.texcoord[2].z, program.env["
    "91], vReg3; DP3   result.texcoord[3].x, program.env[90], vReg1; DP3   result.texcoord[3].y, program."
    "env[90], vReg2; DP3   result.texcoord[3].z, program.env[90], vReg3; DP4   vReg10.x, vertex.position,"
    " mvMatrix[0] ;\n"
    "DP4   vReg10.y, vertex.position, mvMatrix[1] ;\n"
    "DP4   vReg10.z, vertex.position, mvMatrix[2] ;\n"
    "DP4   vReg10.w, vertex.position, mvMatrix[3] ;\n"
    "DP3\tvReg1.x, vReg4, vReg4;\n"
    "RSQ\tvReg1.x, vReg1.x;\n"
    "RCP\tvReg1.x, vReg1.x;\n"
    "MOV result.fogcoord.x, vReg1.x;\n"
    "RCP   vReg10.w, vReg10.w           ;\n"
    "MUL   vReg10.xyz, vReg10, -vReg10.w    ;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.color, program.env[15].x;\n"
    "\0"
    "DP3   result.texcoord[1].w, vReg10, program.env[92];\n"
    "DP3   result.texcoord[2].w, vReg10, program.env[91];\n"
    "DP3   result.texcoord[3].w, vReg10, program.env[90];\n";

char szSkinVPARB[] =
    "!!ARBvp1.0\n"
    "PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "ATTRIB vTex0 = vertex.texcoord[0];\n"
    "ATTRIB vWght = vertex.attrib[1];\n"
    "PARAM boneArray[51] = {program.env[18..68] };\n"
    "PARAM fogParams = state.fog.params;\n"
    "ADDRESS A0;\n"
    "TEMP vReg4, vReg5, vReg6, vReg7, vReg8, vReg9, vReg10;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "TEMP vNormalWorld;\n"
    "ALIAS vFinalColour = vReg9;\n"
    "MUL   vReg0, vertex.attrib[4], program.env[16].zzzz;\n"
    "ARL   A0.x, vReg0.x;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg5.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg5.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg5.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.y;\n"
    "DP4   vReg2.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg2.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg2.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg6.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg6.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg6.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.z;\n"
    "DP4   vReg3.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg3.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg3.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg7.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg7.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg7.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.w;\n"
    "DP4   vReg4.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg4.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg4.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg8.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg8.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg8.z, vertex.normal, boneArray[A0.x + 2];\n"
    "MUL   vReg9, vWght.x, vReg1;\n"
    "MAD   vReg9, vReg2, vWght.y, vReg9;\n"
    "MAD   vReg9, vReg3, vWght.z, vReg9;\n"
    "MAD   vReg9, vReg4, vWght.w, vReg9;\n"
    "MOV   vReg9.w, program.env[16].w;\n"
    "MUL   vReg10, vWght.x, vReg5;\n"
    "MAD   vReg10, vReg6, vWght.y, vReg10;\n"
    "MAD   vReg10, vReg7, vWght.z, vReg10;\n"
    "MAD   vReg10, vReg8, vWght.w, vReg10;\n"
    "DP4   result.position.x, mvpMatrix[0], vReg9;\n"
    "DP4   result.position.y, mvpMatrix[1], vReg9;\n"
    "DP4   result.position.z, mvpMatrix[2], vReg9;\n"
    "DP4   result.position.w, mvpMatrix[3], vReg9;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vReg10;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vReg10;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vReg10;\n"
    "DP3   vNormalWorld.w, vNormalWorld, vNormalWorld;\n"
    "RSQ   vNormalWorld.w, vNormalWorld.w;\n"
    "MUL   vNormalWorld, vNormalWorld, vNormalWorld.w;\n"
    "DP4   vReg10.x, mvMatrix[0], vReg9;\n"
    "DP4   vReg10.y, mvMatrix[1], vReg9;\n"
    "DP4   vReg10.z, mvMatrix[2], vReg9;\n"
    "MOV   vReg10.w, mvMatrix[3].w;\n"
    "DP3\tvReg0.x, vReg10, vReg10;\n"
    "RSQ\tvReg0.x, vReg0.x;\n"
    "RCP\tvReg0.x, vReg0.x;\n"
    "ADD\tvReg1.x, -fogParams.y, vReg0.x;\n"
    "MUL\tresult.color.secondary.x, vReg1.x, state.fog.params.w;\n"
    "MOV\tresult.fogcoord.x, vReg0.x;\n"
    "MOV   result.texcoord[0], vTex0;\n";

char szSkinFogVPARB[] =
    "!!ARBvp1.0\n"
    "PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "ATTRIB vWght = vertex.attrib[1];\n"
    "PARAM boneArray[51] = {program.env[18..68] };\n"
    "PARAM fogParams = state.fog.params;\n"
    "ADDRESS A0;\n"
    "TEMP vReg4, vReg5, vReg6, vReg7, vReg8, vReg9, vReg10;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "MUL   vReg0, vertex.attrib[4], program.env[16].zzzz;\n"
    "ARL   A0.x, vReg0.x;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.y;\n"
    "DP4   vReg2.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg2.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg2.z, vertex.position, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.z;\n"
    "DP4   vReg3.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg3.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg3.z, vertex.position, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.w;\n"
    "DP4   vReg4.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg4.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg4.z, vertex.position, boneArray[A0.x + 2];\n"
    "MUL   vReg9, vWght.x, vReg1;\n"
    "MAD   vReg9, vReg2, vWght.y, vReg9;\n"
    "MAD   vReg9, vReg3, vWght.z, vReg9;\n"
    "MAD   vReg9, vReg4, vWght.w, vReg9;\n"
    "MOV   vReg9.w, program.env[16].w;\n"
    "DP4   result.position.x, mvpMatrix[0], vReg9;\n"
    "DP4   result.position.y, mvpMatrix[1], vReg9;\n"
    "DP4   result.position.z, mvpMatrix[2], vReg9;\n"
    "DP4   result.position.w, mvpMatrix[3], vReg9;\n"
    "DP4   vReg10.x, mvMatrix[0], vReg9;\n"
    "DP4   vReg10.y, mvMatrix[1], vReg9;\n"
    "DP4   vReg10.z, mvMatrix[2], vReg9;\n"
    "MOV   vReg10.w, mvMatrix[3].w;\n"
    "DP3\tvReg0.x, vReg10, vReg10;\n"
    "RSQ\tvReg0.x, vReg0.x;\n"
    "RCP\tvReg0.x, vReg0.x;\n"
    "ADD\tvReg0.x, -fogParams.y, vReg0.x;\n"
    "MOV    result.color.xyz, state.fog.color;\n"
    "MUL\tresult.color.w, vReg0.x, state.fog.params.w;\n";

char szSkinCubeMapVPARB[] =
    "!!ARBvp1.0\n"
    "PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "PARAM boneArray[51] = {program.env[18..68] };\n"
    "PARAM fogParams = state.fog.params;\n"
    "ATTRIB vTex0 = vertex.texcoord[0];\n"
    "ATTRIB vWeight = vertex.attrib[1];\n"
    "ADDRESS A0;\n"
    "TEMP vReg4, vReg5, vReg6, vReg7, vReg8, vReg9, vReg10;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "TEMP vNormalWorld;\n"
    "ALIAS vFinalColour = vReg9;\n"
    "MUL   vReg0, vertex.attrib[4], program.env[16].zzzz;\n"
    "ARL   A0.x, vReg0.x;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg5.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg5.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg5.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.y;\n"
    "DP4   vReg2.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg2.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg2.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg6.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg6.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg6.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.z;\n"
    "DP4   vReg3.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg3.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg3.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg7.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg7.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg7.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.w;\n"
    "DP4   vReg4.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg4.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg4.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg8.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg8.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg8.z, vertex.normal, boneArray[A0.x + 2];\n"
    "MUL   vReg9, vWeight.x, vReg1;\n"
    "MAD   vReg9, vReg2, vWeight.y, vReg9;\n"
    "MAD   vReg9, vReg3, vWeight.z, vReg9;\n"
    "MAD   vReg9, vReg4, vWeight.w, vReg9;\n"
    "MOV   vReg9.w, program.env[16].w;\n"
    "MUL   vReg10, vWeight.x, vReg5;\n"
    "MAD   vReg10, vReg6, vWeight.y, vReg10;\n"
    "MAD   vReg10, vReg7, vWeight.z, vReg10;\n"
    "MAD   vReg10, vReg8, vWeight.w, vReg10;\n"
    "DP4   result.position.x, mvpMatrix[0], vReg9;\n"
    "DP4   result.position.y, mvpMatrix[1], vReg9;\n"
    "DP4   result.position.z, mvpMatrix[2], vReg9;\n"
    "DP4   result.position.w, mvpMatrix[3], vReg9;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vReg10;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vReg10;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vReg10;\n"
    "MOV   vNormalWorld.w, mvMatrix[3].w;\n"
    "DP3   vNormalWorld.w, vNormalWorld, vNormalWorld;\n"
    "RSQ   vNormalWorld.w, vNormalWorld.w;\n"
    "MUL   vNormalWorld, vNormalWorld, vNormalWorld.w;\n"
    "DP4   vReg10.x, mvMatrix[0], vReg9;\n"
    "DP4   vReg10.y, mvMatrix[1], vReg9;\n"
    "DP4   vReg10.z, mvMatrix[2], vReg9;\n"
    "MOV   vReg10.w, mvMatrix[3].w;\n"
    "DP3\tvReg0.x, vReg10, vReg10;\n"
    "RSQ\tvReg0.x, vReg0.x;\n"
    "RCP\tvReg0.x, vReg0.x;\n"
    "MOV\tresult.fogcoord, vReg0.x;\n"
    "ADD\tvReg1.x, -fogParams.y, vReg0.x;\n"
    "MUL\tresult.color.secondary.x, vReg1.x, fogParams.w;\n"
    "MOV result.texcoord[0], vTex0;\n"
    "DP3   vReg0.x, vNormalWorld, -vReg10;\n"
    "MUL   vReg1, vReg0.x, vNormalWorld;\n"
    "MUL   vReg2, vReg1, program.env[16].y;\n"
    "ADD   vReg3, vReg10, vReg2;\n"
    "DP3   vReg3.w, vReg3, vReg3;\n"
    "RSQ   vReg3.w, vReg3.w;\n"
    "MUL   vReg3, vReg3, vReg3.w;\n"
    "DP3   vReg4.x, program.env[92], vReg3;\n"
    "DP3   vReg4.y, program.env[91], vReg3;\n"
    "DP3   vReg4.z, program.env[90], vReg3;\n"
    "MOV   vReg4.w, program.env[16].w;\n"
    "MOV   result.texcoord[1], vReg4;\n";

char szSkinBumpedOutVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "PARAM boneArray[51] = {program.env[18..68] };\n"
    "ADDRESS A0;\n"
    "TEMP vReg4, vReg5, vReg6, vReg7, vReg8, vReg9, vReg10;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "TEMP vNormalWorld;\n"
    "ALIAS vFinalColour = vReg9;\n"
    "ATTRIB vWght = vertex.attrib[1];\n"
    "MUL   vReg0, vertex.attrib[4], program.env[16].z;\n"
    "ARL   A0.x, vReg0.x;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg5.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg5.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg5.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.y;\n"
    "DP4   vReg2.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg2.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg2.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg6.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg6.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg6.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.z;\n"
    "DP4   vReg3.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg3.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg3.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg7.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg7.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg7.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.w;\n"
    "DP4   vReg4.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg4.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg4.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg8.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg8.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg8.z, vertex.normal, boneArray[A0.x + 2];\n"
    "MUL   vReg9, vWght.x, vReg1;\n"
    "MAD   vReg9, vReg2, vWght.y, vReg9;\n"
    "MAD   vReg9, vReg3, vWght.z, vReg9;\n"
    "MAD   vReg9, vReg4, vWght.w, vReg9;\n"
    "MUL   vReg10, vWght.x, vReg5;\n"
    "MAD   vReg10, vReg6, vWght.y, vReg10;\n"
    "MAD   vReg10, vReg7, vWght.z, vReg10;\n"
    "MAD   vReg10, vReg8, vWght.w, vReg10;\n"
    "MOV   vReg10.w, program.env[12].w;\n"
    "MAD vReg9, vReg10, program.env[12].x, vReg9;\n"
    "MOV   vReg9.w, program.env[16].w;\n"
    "DP4   result.position.x, mvpMatrix[0], vReg9;\n"
    "DP4   result.position.y, mvpMatrix[1], vReg9;\n"
    "DP4   result.position.z, mvpMatrix[2], vReg9;\n"
    "DP4   result.position.w, mvpMatrix[3], vReg9;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vReg10;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vReg10;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vReg10;\n"
    "MOV   vNormalWorld.w, mvMatrix[3].w;\n"
    "DP3   vNormalWorld.w, vNormalWorld, vNormalWorld;\n"
    "RSQ   vNormalWorld.w, vNormalWorld.w;\n"
    "MUL   vNormalWorld, vNormalWorld, vNormalWorld.w;\n"
    "DP4   vReg10.x, mvMatrix[0], vReg9;\n"
    "DP4   vReg10.y, mvMatrix[1], vReg9;\n"
    "DP4   vReg10.z, mvMatrix[2], vReg9;\n"
    "MOV   vReg10.w, mvMatrix[3].w;\n"
    "DP3\tvReg0.x, vReg10, vReg10;\n"
    "RSQ\tvReg0.x, vReg0.x;\n"
    "RCP\tvReg0.x, vReg0.x;\n"
    "MOV    result.fogcoord.x, vReg0.x;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.color, program.env[12].w;\n";

char szSkinBumpyVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "PARAM boneArray[51] = {program.env[18..68] };\n"
    "ATTRIB vWght = vertex.attrib[1];\n"
    "ATTRIB vTex4 = vertex.attrib[12];\n"
    "ATTRIB vTex5 = vertex.attrib[13];\n"
    "ATTRIB vTex6 = vertex.attrib[14];\n"
    "ADDRESS A0;\n"
    "TEMP vReg4, vReg5, vReg6, vReg7, vReg8, vReg9, vReg10;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "TEMP vNormalWorld;\n"
    "ALIAS vFinalColour = vReg9;\n"
    "MUL   vReg0, vertex.attrib[4], program.env[16].zzzz;\n"
    "ARL   A0.x, vReg0.x;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg5.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg5.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg5.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.y;\n"
    "DP4   vReg2.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg2.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg2.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg6.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg6.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg6.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.z;\n"
    "DP4   vReg3.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg3.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg3.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg7.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg7.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg7.z, vertex.normal, boneArray[A0.x + 2];\n"
    "ARL   A0.x, vReg0.w;\n"
    "DP4   vReg4.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg4.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg4.z, vertex.position, boneArray[A0.x + 2];\n"
    "DP3   vReg8.x, vertex.normal, boneArray[A0.x + 0];\n"
    "DP3   vReg8.y, vertex.normal, boneArray[A0.x + 1];\n"
    "DP3   vReg8.z, vertex.normal, boneArray[A0.x + 2];\n"
    "MUL   vReg9, vWght.x, vReg1;\n"
    "MAD   vReg9, vReg2, vWght.y, vReg9;\n"
    "MAD   vReg9, vReg3, vWght.z, vReg9;\n"
    "MAD   vReg9, vReg4, vWght.w, vReg9;\n"
    "MOV   vReg9.w, program.env[16].w;\n"
    "DP4   result.position.x, mvpMatrix[0], vReg9;\n"
    "DP4   result.position.y, mvpMatrix[1], vReg9;\n"
    "DP4   result.position.z, mvpMatrix[2], vReg9;\n"
    "DP4   result.position.w, mvpMatrix[3], vReg9;\n"
    "MUL   vReg10, vWght.x, vReg5;\n"
    "MAD   vReg10, vReg6, vWght.y, vReg10;\n"
    "MAD   vReg10, vReg7, vWght.z, vReg10;\n"
    "MAD   vReg10, vReg8, vWght.w, vReg10;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vReg10;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vReg10;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vReg10;\n"
    "MOV   vNormalWorld.w, program.env[16].w;\n"
    "DP3   vNormalWorld.w, vNormalWorld, vNormalWorld;\n"
    "RSQ   vNormalWorld.w, vNormalWorld.w;\n"
    "MUL   vNormalWorld, vNormalWorld, vNormalWorld.w;\n"
    "DP4   vReg10.x, mvMatrix[0], vReg9;\n"
    "DP4   vReg10.y, mvMatrix[1], vReg9;\n"
    "DP4   vReg10.z, mvMatrix[2], vReg9;\n"
    "MOV   vReg10.w, mvMatrix[3].w;\n"
    "DP3\tvReg0.x, vReg10, vReg10;\n"
    "RSQ\tvReg0.x, vReg0.x;\n"
    "RCP\tvReg0.x, vReg0.x;\n"
    "MOV result.fogcoord.x, vReg0.x;\n"
    "ADD  vReg0, program.env[12], -vReg9;\n"
    "DP3   vReg0.w, vReg0, vReg0;\n"
    "RSQ   vReg1.w, vReg0.w;\n"
    "DST vReg2, vReg0.w, vReg1.w;\n"
    "DP3 vReg2.w, vReg2, program.env[14];\n"
    "RCP vReg2.w, vReg2.w;\n"
    "MOV   result.color.secondary, vReg2.w;\n"
    "MUL   vReg0, vReg0, vReg1.w;\n"
    "MOV   vReg0.w, program.env[16].w;\n"
    "ADD vReg2, program.env[13],-vReg9;\n"
    "DP3   vReg2.w, vReg2, vReg2;\n"
    "RSQ   vReg2.w, vReg2.w;\n"
    "MUL   vReg2, vReg2, vReg2.w;\n"
    "MOV   vReg2.w, program.env[16].w;\n"
    "ADD vReg2, vReg2,vReg0;\n"
    "MOV   vReg2.w, program.env[16].w;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.texcoord[3], vertex.texcoord[0];\n"
    "DP3   vReg1.x, vTex4, vReg0;\n"
    "DP3   vReg1.y, vTex5, vReg0;\n"
    "DP3   vReg1.z, vTex6, vReg0;\n"
    "MOV   vReg1.w, program.env[16].w;\n"
    "MOV   result.texcoord[1], vReg1;\n"
    "DP3   vReg1.x, vTex4, vReg2;\n"
    "DP3   vReg1.y, vTex5, vReg2;\n"
    "DP3   vReg1.z, vTex6, vReg2;\n"
    "MOV   vReg1.w, program.env[16].w;\n"
    "MOV   result.texcoord[2], vReg1;\n";

char szNewSkinBumpyVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "PARAM boneArray[51] = {program.env[18..68] };\n"
    "ADDRESS A0;\n"
    "TEMP vReg4, vReg5, vReg6, vReg7, vReg8, vReg9, vReg10;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "TEMP vNormalWorld;\n"
    "ALIAS vFinalColour = vReg9;\n"
    "ATTRIB vWght = vertex.attrib[1];\n"
    "MUL   vReg0, vertex.attrib[4], program.env[16].z;\n"
    "ARL   A0.x, vReg0.x;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MUL   vReg9, vWght.x, vReg1;\n"
    "DP3   vReg1.x, vertex.attrib[12], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[12], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[12], boneArray[A0.x + 2];\n"
    "MUL   vReg4, vWght.x, vReg1;\n"
    "DP3   vReg1.x, vertex.attrib[13], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[13], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[13], boneArray[A0.x + 2];\n"
    "MUL   vReg5, vWght.x, vReg1;\n"
    "DP3   vReg1.x, vertex.attrib[14], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[14], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[14], boneArray[A0.x + 2];\n"
    "MUL   vReg6, vWght.x, vReg1;\n"
    "ARL   A0.x, vReg0.y;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MAD   vReg9, vReg1, vWght.y, vReg9;\n"
    "DP3   vReg1.x, vertex.attrib[12], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[12], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[12], boneArray[A0.x + 2];\n"
    "MAD   vReg4, vWght.y, vReg1, vReg4;\n"
    "DP3   vReg1.x, vertex.attrib[13], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[13], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[13], boneArray[A0.x + 2];\n"
    "MAD   vReg5, vWght.y, vReg1, vReg5;\n"
    "DP3   vReg1.x, vertex.attrib[14], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[14], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[14], boneArray[A0.x + 2];\n"
    "MAD   vReg6, vWght.y, vReg1, vReg6;\n"
    "ARL   A0.x, vReg0.z;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MAD   vReg9, vReg1, vWght.z, vReg9;\n"
    "DP3   vReg1.x, vertex.attrib[12], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[12], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[12], boneArray[A0.x + 2];\n"
    "MAD   vReg4, vWght.z, vReg1, vReg4;\n"
    "DP3   vReg1.x, vertex.attrib[13], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[13], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[13], boneArray[A0.x + 2];\n"
    "MAD   vReg5, vWght.z, vReg1, vReg5;\n"
    "DP3   vReg1.x, vertex.attrib[14], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[14], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[14], boneArray[A0.x + 2];\n"
    "MAD   vReg6, vWght.z, vReg1, vReg6;\n"
    "ARL   A0.x, vReg0.w;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MAD   vReg9, vReg1,vWght.w, vReg9;\n"
    "DP3   vReg1.x, vertex.attrib[12], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[12], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[12], boneArray[A0.x + 2];\n"
    "MAD   vReg4, vWght.w, vReg1, vReg4\n"
    ";DP3   vReg1.x, vertex.attrib[13], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[13], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[13], boneArray[A0.x + 2];\n"
    "MAD   vReg5, vWght.w, vReg1, vReg5\n"
    ";DP3   vReg1.x, vertex.attrib[14], boneArray[A0.x + 0];\n"
    "DP3   vReg1.y, vertex.attrib[14], boneArray[A0.x + 1];\n"
    "DP3   vReg1.z, vertex.attrib[14], boneArray[A0.x + 2];\n"
    "MAD   vReg10,vWght.w, vReg1, vReg6\n"
    ";MOV   vReg9.w, program.env[16].w;\n"
    "MOV   vReg10.w, program.env[16].w;\n"
    "DP4   result.position.x, mvpMatrix[0], vReg9;\n"
    "DP4   result.position.y, mvpMatrix[1], vReg9;\n"
    "DP4   result.position.z, mvpMatrix[2], vReg9;\n"
    "DP4   result.position.w, mvpMatrix[3], vReg9;\n"
    "DP3   vNormalWorld.x, mvMatrix[0], vReg10;\n"
    "DP3   vNormalWorld.y, mvMatrix[1], vReg10;\n"
    "DP3   vNormalWorld.z, mvMatrix[2], vReg10;\n"
    "MOV   vNormalWorld.w, program.env[16].w;\n"
    "DP3   vNormalWorld.w, vNormalWorld, vNormalWorld;\n"
    "RSQ   vNormalWorld.w, vNormalWorld.w;\n"
    "MUL   vNormalWorld, vNormalWorld, vNormalWorld.w;\n"
    "DP4   vReg10.x, mvMatrix[0], vReg9;\n"
    "DP4   vReg10.y, mvMatrix[1], vReg9;\n"
    "DP4   vReg10.z, mvMatrix[2], vReg9;\n"
    "MOV   vReg10.w, mvMatrix[3].w;\n"
    "ADD  vReg0, program.env[12], -vReg9;\n"
    "DP3   vReg0.w, vReg0, vReg0;\n"
    "RSQ   vReg1.w, vReg0.w;\n"
    "DST vReg2, vReg0.w, vReg1.w;\n"
    "DP3 vReg2.w, vReg2, program.env[14];\n"
    "RCP vReg2.w, vReg2.w;\n"
    "MOV   result.color.secondary, vReg2.wwww;\n"
    "MUL   vReg0, vReg0, vReg1.w;\n"
    "MOV   vReg0.w, program.env[16].w;\n"
    "ADD vReg2, program.env[13],-vReg9;\n"
    "DP3   vReg2.w, vReg2, vReg2;\n"
    "RSQ   vReg2.w, vReg2.w;\n"
    "MUL   vReg2, vReg2, vReg2.w;\n"
    "MOV   vReg2.w, program.env[16].w;\n"
    "ADD vReg2, vReg2,vReg0;\n"
    "MOV   vReg2.w, program.env[16].w;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.texcoord[3], vertex.texcoord[0];\n"
    "DP3   vReg1.x, vReg4, vReg0;\n"
    "DP3   vReg1.y, vReg5, vReg0;\n"
    "DP3   vReg1.z, vReg6, vReg0;\n"
    "MOV   vReg1.w, program.env[16].w;\n"
    "MOV   result.texcoord[1], vReg1;\n"
    "DP3   vReg1.x, vReg4, vReg2;\n"
    "DP3   vReg1.y, vReg5, vReg2;\n"
    "DP3   vReg1.z, vReg6, vReg2;\n"
    "MOV   vReg1.w, program.env[16].w;\n"
    "MOV   result.texcoord[2], vReg1;\n";

char szBumpedOutVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "TEMP vReg1, vReg2, vReg9, vReg10, vReg11;\n"
    "MOV\tvReg1, vertex.position;\n"
    "MOV\tvReg2, vertex.normal;\n"
    "MAD\tvReg1, vReg2, program.env[12].x, vReg1;\n"
    "DP4   result.position.x, mvpMatrix[0], vReg1;\n"
    "DP4   result.position.y, mvpMatrix[1], vReg1;\n"
    "DP4   result.position.z, mvpMatrix[2], vReg1;\n"
    "DP4   result.position.w, mvpMatrix[3], vReg1;\n"
    "DP4   vReg10.x, mvMatrix[0], vReg1;\n"
    "DP4   vReg10.y, mvMatrix[1], vReg1;\n"
    "DP4   vReg10.z, mvMatrix[2], vReg1;\n"
    "MOV   vReg10.w, program.env[12].w;\n"
    "DP3   vReg1.w, vReg10, vReg10;\n"
    "RSQ   vReg1.w, vReg1.w;\n"
    "RCP   vReg1.w, vReg1.w;\n"
    "MOV   result.fogcoord, vReg1.w;\n"
    "DP3   vReg11.x, mvMatrix[0], vertex.normal;\n"
    "DP3   vReg11.y, mvMatrix[1], vertex.normal;\n"
    "DP3   vReg11.z, mvMatrix[2], vertex.normal;\n"
    "DP3   vReg11.w, vReg11, vReg11;\n"
    "RSQ   vReg11.w, vReg11.w;\n"
    "MUL   vReg11, vReg11, vReg11.w;\n"
    "MOV   vReg9, vertex.color;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.texcoord[1], vertex.texcoord[1];\n"
    "MOV result.color, program.env[12].w;\n";

char szSkinBumpyWSVPARB[] =
    "!!ARBvp1.0PARAM mvMatrix[4] = { state.matrix.modelview[0] };\n"
    "PARAM mvpMatrix[4] = { state.matrix.mvp };\n"
    "PARAM boneArray[51] = {program.env[18..68] };\n"
    "ADDRESS A0;\n"
    "TEMP vReg4, vReg5, vReg6, vReg7, vReg8, vReg9, vReg10;\n"
    "TEMP vReg0, vReg1, vReg2, vReg3;\n"
    "TEMP vNormalWorld;\n"
    "ALIAS vLightVectorWS = vReg4;\n"
    "ALIAS vFinalColour = vReg9;\n"
    "ALIAS vBoneIndices = vReg8;\n"
    "ATTRIB vWght = vertex.attrib[1];\n"
    "MUL   vBoneIndices, vertex.attrib[4], program.env[16].zzzz;\n"
    "ARL   A0.x, vBoneIndices.x;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MUL   vReg9, vWght.x, vReg1;\n"
    "ARL   A0.x, vBoneIndices.y;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MAD   vReg9, vReg1, vWght.y, vReg9;\n"
    "ARL   A0.x, vBoneIndices.z;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MAD   vReg9, vReg1, vWght.z, vReg9;\n"
    "ARL   A0.x, vBoneIndices.w;\n"
    "DP4   vReg1.x, vertex.position, boneArray[A0.x + 0];\n"
    "DP4   vReg1.y, vertex.position, boneArray[A0.x + 1];\n"
    "DP4   vReg1.z, vertex.position, boneArray[A0.x + 2];\n"
    "MAD   vReg9, vReg1,vWght.w, vReg9;\n"
    "MOV   vReg9.w, program.env[16].w;\n"
    "DP4   result.position.x, mvpMatrix[0], vReg9;\n"
    "DP4   result.position.y, mvpMatrix[1], vReg9;\n"
    "DP4   result.position.z, mvpMatrix[2], vReg9;\n"
    "DP4   result.position.w, mvpMatrix[3], vReg9;\n"
    "DP4   vReg10.x, program.env[8], vReg9;\n"
    "DP4   vReg10.y, program.env[9], vReg9;\n"
    "DP4   vReg10.z, program.env[10], vReg9;\n"
    "MOV   vReg10.w, program.env[16].w;\n"
    "DP3\tvReg1.x, vReg10, vReg10;\n"
    "RSQ\tvReg1.x, vReg1.x;\n"
    "RCP\tvReg1.x, vReg1.x;\n"
    "MOV result.fogcoord, vReg1.x;\n"
    "ADD  vReg0, program.env[12], -vReg9;\n"
    "DP3   vReg0.w, vReg0, vReg0;\n"
    "RSQ   vReg1.w, vReg0.w;\n"
    "DST vReg2, vReg0.w, vReg1.w;\n"
    "DP3 vReg2.w, vReg2, program.env[14];\n"
    "RCP vReg2.w, vReg2.w;\n"
    "MOV   result.color.secondary, vReg2.wwww;\n"
    "MUL   vReg0, vReg0, vReg1.w;\n"
    "MOV   vReg0.w, program.env[16].w;\n"
    "ADD vReg2, program.env[13],-vReg9;\n"
    "DP3   vReg2.w, vReg2, vReg2;\n"
    "RSQ   vReg2.w, vReg2.w;\n"
    "MUL   vReg2, vReg2, vReg2.w;\n"
    "MOV   vReg2.w, program.env[16].w;\n"
    "ADD vReg2, vReg2,vReg0;\n"
    "MOV   vReg2.w, program.env[16].w;\n"
    "MOV   result.texcoord[0], vertex.texcoord[0];\n"
    "MOV   result.texcoord[3], vertex.texcoord[0];\n"
    "MUL\tvReg9, boneArray[A0.x + 0], vReg0.x;\n"
    "MAD\tvReg9, boneArray[A0.x + 1], vReg0.y, vReg9;\n"
    "MAD\tvReg9, boneArray[A0.x + 2], vReg0.z, vReg9;\n"
    "MOV\tvReg5.x, boneArray[A0.x+0].w;\n"
    "MOV\tvReg5.y, boneArray[A0.x+1].w;\n"
    "MOV\tvReg5.z, boneArray[A0.x+2].w;\n"
    "SUB\tvReg9, vReg9, vReg5;\n"
    "MUL\tvReg4, vReg9, vWght.w;\n"
    "MUL\tvReg9, vReg2.x, boneArray[A0.x + 0];\n"
    "MAD\tvReg9, vReg2.y, boneArray[A0.x + 1], vReg9;\n"
    "MAD\tvReg9, vReg2.z, boneArray[A0.x + 2], vReg9;\n"
    "SUB\tvReg9, vReg9, vReg5;\n"
    "MUL\tvReg6, vReg9, vWght.w;\n"
    "ARL   A0.x, vBoneIndices.z;\n"
    "MUL\tvReg9, vReg0.x, boneArray[A0.x + 0];\n"
    "MAD\tvReg9, vReg0.y, boneArray[A0.x + 1], vReg9;\n"
    "MAD\tvReg9, vReg0.z, boneArray[A0.x + 2], vReg9;\n"
    "MOV\tvReg5.x, boneArray[A0.x+0].w;\n"
    "MOV\tvReg5.y, boneArray[A0.x+1].w;\n"
    "MOV\tvReg5.z, boneArray[A0.x+2].w;\n"
    "SUB\tvReg9, vReg9, vReg5;\n"
    "MAD\tvReg4, vReg9, vWght.z, vReg4;\n"
    "MUL\tvReg9, vReg2.x, boneArray[A0.x + 0];\n"
    "MAD\tvReg9, vReg2.y, boneArray[A0.x + 1], vReg9;\n"
    "MAD\tvReg9, vReg2.z, boneArray[A0.x + 2], vReg9;\n"
    "SUB\tvReg9, vReg9, vReg5;\n"
    "MAD\tvReg6, vReg9, vWght.z, vReg6;\n"
    "ARL   A0.x, vBoneIndices.y;\n"
    "MUL\tvReg9, vReg0.x, boneArray[A0.x + 0];\n"
    "MAD\tvReg9, vReg0.y, boneArray[A0.x + 1], vReg9;\n"
    "MAD\tvReg9, vReg0.z, boneArray[A0.x + 2], vReg9;\n"
    "MOV\tvReg5.x, boneArray[A0.x+0].w;\n"
    "MOV\tvReg5.y, boneArray[A0.x+1].w;\n"
    "MOV\tvReg5.z, boneArray[A0.x+2].w;\n"
    "SUB\tvReg9, vReg9, vReg5;\n"
    "MAD\tvReg4, vReg9, vWght.y, vReg4;\n"
    "MUL\tvReg9, vReg2.x, boneArray[A0.x + 0];\n"
    "MAD\tvReg9, vReg2.y, boneArray[A0.x + 1], vReg9;\n"
    "MAD\tvReg9, vReg2.z, boneArray[A0.x + 2], vReg9;\n"
    "SUB\tvReg9, vReg9, vReg5;\n"
    "MAD\tvReg6, vReg9, vWght.y, vReg6;\n"
    "ARL   A0.x, vBoneIndices.x;\n"
    "MUL\tvReg9, vReg0.x, boneArray[A0.x + 0];\n"
    "MAD\tvReg9, vReg0.y, boneArray[A0.x + 1], vReg9;\n"
    "MAD\tvReg9, vReg0.z, boneArray[A0.x + 2], vReg9;\n"
    "MOV\tvReg5.x, boneArray[A0.x+0].w;\n"
    "MOV\tvReg5.y, boneArray[A0.x+1].w;\n"
    "MOV\tvReg5.z, boneArray[A0.x+2].w;\n"
    "SUB\tvReg9, vReg9, vReg5;\n"
    "MAD\tvReg4, vReg9, vWght.x, vReg4;\n"
    "MOV   vReg4.w, program.env[16].w;\n"
    "MOV   result.texcoord[2], vReg4;\n"
    "MUL\tvReg9, vReg2.x, boneArray[A0.x + 0];\n"
    "MAD\tvReg9, vReg2.y, boneArray[A0.x + 1], vReg9;\n"
    "MAD\tvReg9, vReg2.z, boneArray[A0.x + 2], vReg9;\n"
    "SUB\tvReg9, vReg9, vReg5;\n"
    "MAD\tvReg6, vReg9, vWght.z, vReg6;\n"
    "MOV   vReg6.w, program.env[16].w;\n"
    "MOV   result.texcoord[3], vReg6;\n";
