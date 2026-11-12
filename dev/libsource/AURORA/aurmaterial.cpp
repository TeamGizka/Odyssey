#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>
#include <string.h>
#include "aurmaterial.h"
#include "aurrendergl.h"
#include "aurconsole.h"

List<Material*> materials;

static inline CAurTexture* AsTexture(SafePointer<CAurTexture>& pPointer)
{
    return (CAurTexture*)pPointer.m_pObject;
}

int usenewalpha = 1;
float materialalpha = 0.35f;
float* Homogenize(const Vector& vVector, const float& fValue);

char* firstword(char* sLine);
void Parse(char* sValue, char* sDest);
int Parse(char* sValue, bool& bValue);
int Parse(char* sValue, int& nValue);
int Parse(char* sValue, float& fValue);

static inline void CopyString(char* sDest, const char* sSource)
{
    char c;

    do
    {
        c = *sSource++;
        *sDest++ = c;
    } while (c != 0);
}

CAurTexture* LoadTexture(char* sName)
{
    if (sName == 0 || *sName == 0 || strcmp("NULL", sName) == 0)
    {
        return AurTextureGetNULL();
    }
    return AurTextureGetReference(sName, 0);
}
// PC 0x00479A90
template <> List<Aurora::AuroraBlendModes>::~List()
{
    if (m_pList != 0)
    {
        delete[] m_pList;
    }
}
// PC 0x004AB240
void Material::ParseField(char* sField)
{
    char sBlending[32];
    char sBumpMap[32];
    char sBumpyShiny[32];
    char sEnvMap[32];

    sBumpMap[0] = 0;
    sBumpyShiny[0] = 0;
    sEnvMap[0] = 0;
    sBlending[0] = 0;

    if (stricmp("bumpmaptexture", firstword(sField)) == 0)
    {
        Parse(sField + 14, sBumpMap);
    }
    if (stricmp("bumpyshinytexture", firstword(sField)) == 0)
    {
        Parse(sField + 17, sBumpyShiny);
    }
    if (stricmp("envmaptexture", firstword(sField)) == 0)
    {
        Parse(sField + 13, sEnvMap);
    }
    if (stricmp("blending", firstword(sField)) == 0)
    {
        Parse(sField + 8, sBlending);
    }
    if (stricmp("decal", firstword(sField)) == 0)
    {
        Parse(sField + 5, m_bDecal);
    }
    if (stricmp("renderbmlmtype", firstword(sField)) == 0)
    {
        Parse(sField + 14, m_nRenderType);
    }
    if (stricmp("wateralpha", firstword(sField)) == 0)
    {
        Parse(sField + 10, m_fWaterAlpha);
    }

    if (sBumpMap[0] != 0)
    {
        m_sBumpMapName = new char[32];
        CopyString(m_sBumpMapName, sBumpMap);
    }
    if (sBumpyShiny[0] != 0)
    {
        m_sBumpyShinyName = new char[32];
        CopyString(m_sBumpyShinyName, sBumpyShiny);
    }
    if (sEnvMap[0] != 0)
    {
        m_sEnvMapName = new char[32];
        CopyString(m_sEnvMapName, sEnvMap);
    }
    if (sBlending[0] != 0)
    {
        if (strcmp(sBlending, "additive") == 0)
        {
            m_lstBlendSrc.m_pList[0] = BLEND_SRC_ALPHA;
            m_lstBlendDst.m_pList[0] = BLEND_ONE;
        }
        else if (strcmp(sBlending, "punchthrough") == 0)
        {
            m_lstBlendSrc.m_pList[0] = BLEND_ONE;
            m_lstBlendDst.m_pList[0] = BLEND_ZERO;
        }
    }
}

#ifndef _XBOX
inline
#endif
void Material::PurgeBadTexture(SafePointer<CAurTexture>& pTexture)
{
    CAurTexture* pResolved = AsTexture(pTexture)->GetReferencedTexture();

    if (pResolved == 0)
    {
        AurTextureGetNULL();
    }
    else
    {
        if (pResolved->IsInvalid())
        {
            AurTextureGetNULL();
            AurTextureRelease(pResolved);
        }
    }
}
// PC 0x004AB460
int Material::BindTexture0()
{
    PurgeBadTexture(m_pTexture0);

    if (enablebumpmap != 0 && AsTexture(m_pAltTexture) != 0)
    {
        AsTexture(m_pAltTexture)->Bind();
        return AurTextureGetTID(AsTexture(m_pTexture0));
    }
    else
    {
        AsTexture(m_pTexture0)->Bind();
        return AurTextureGetTID(AsTexture(m_pTexture0));
    }
}
// PC 0x004AB4D0
int Material::BindTexture1()
{
    PurgeBadTexture(m_pTexture1);

    AsTexture(m_pTexture1)->Bind();
    return AurTextureGetTID(AsTexture(m_pTexture1));
}
// PC 0x004AB520
int Material::BindEnvMap()
{
    PurgeBadTexture(m_pEnvMap);

    AsTexture(m_pEnvMap)->Bind();
    return AurTextureGetTID(AsTexture(m_pEnvMap));
}
// PC 0x004AB570
int Material::GetTextureTID()
{
    return AurTextureGetTID(AsTexture(m_pTexture0));
}
// PC 0x004AB580
int Material::SetDownSampling(int nMax)
{
    char sCommand[20];

    sprintf(sCommand, "downsamplemax %d", nMax);
    return AurTextureMessage((CAurTexture*)m_pTexture0.m_pObject, sCommand);
}
// PC 0x004AB5C0
int Material::BumpMapIsSpecular()
{
    if (AsTexture(m_pBumpMap) != 0)
    {
        return AsTexture(m_pBumpMap)->IsSpecularBumpMap();
    }
    else
    {
        return 0;
    }
}
// PC 0x004AB5E0
void Material::SetBlendingMode(int nIndex)
{
    GLRender_SetBlendFunc(m_lstBlendSrc.m_pList[nIndex], m_lstBlendDst.m_pList[nIndex]);

    if (m_lstBlendSrc.m_pList[nIndex] != 0 ||
        m_lstBlendDst.m_pList[nIndex] != 2)
    {
        GLRender_SetDepthMask(1);
    }
    else
    {
        GLRender_SetDepthMask(0);
    }

    if (usenewalpha != 0)
    {
        if (m_lstBlendSrc.m_pList[nIndex] == 2 &&
            m_lstBlendDst.m_pList[nIndex] == 3)
        {
            GLRender_SetAlphaFunc(CMP_GREATER, materialalpha);
        }
        else
        {
            GLRender_SetAlphaFunc(CMP_GREATER, 0.0f);
        }
    }
}
// PC 0x004AB670
void Material::SetGUIBlendingMode()
{
    GLRender_SetBlendFunc(*m_lstBlendSrc.m_pList, *m_lstBlendDst.m_pList);

    if (usenewalpha != 0)
    {
        if (*m_lstBlendSrc.m_pList == 2 && *m_lstBlendDst.m_pList == 3)
        {
            GLRender_SetAlphaFunc(CMP_GREATER, materialalpha);
        }
        else
        {
            GLRender_SetAlphaFunc(CMP_GREATER, 0.0f);
        }
    }
}
// PC 0x004AB6E0
void Material::SetMaterialColor(Vector vColor, float fAlpha)
{
    if (AsTexture(m_pTexture0)->IsUsingGlobalAlpha())
    {
        fAlpha = g_fGlobalAlpha * fAlpha;
        GLRender_SetMaterialv(0, MATERIAL_DIFFUSE, Homogenize(vColor, fAlpha));
    }
}
// PC 0x004AB730
unsigned char Material::HasBumpyShinyMap()
{
    return AsTexture(m_pEnvMap) != 0 && AsTexture(m_pEnvMap)->IsCubeMap() != 0;
}
// PC 0x004AB750
unsigned char Material::HasEnvMap()
{
    return AsTexture(m_pEnvMap) != 0 && AsTexture(m_pEnvMap)->IsNull() == 0 &&
           AsTexture(m_pEnvMap)->IsInvalid() == 0;
}
// PC 0x004AB780
void Material::Init(char* sName)
{
    char sLine[8192];
    char* pLine;
    char* pData;
    int nSize = 0;
    int nLeft;
    int nLength;

    if (AsTexture(m_pTexture0) != 0 && AsTexture(m_pTexture0)->IsTxiLoaded())
    {
        pData = AsTexture(m_pTexture0)->GetTxiData();
        nLeft = AsTexture(m_pTexture0)->GetTxiSize();
        if (pData != 0 && nLeft > 0)
        {
            do
            {
                nLength = 0;
                do
                {
                    sLine[nLength++] = *pData++;
                } while (nLength < nLeft && sLine[nLength - 1] != 10);

                nLeft -= nLength;
                if (nLeft < 0)
                {
                    pLine = 0;
                }
                else
                {
                    sLine[nLength] = 0;
                    for (pLine = sLine; *pLine == 32 || *pLine == 9; pLine++)
                    {
                    }
                    ParseField(pLine);
                }
            } while (nLeft != 0 && pLine != 0 && nLeft > 0);
        }
    }
    else
    {
        void* pFile = AurResGet(sName, ".txi", &nSize, true);
        if (pFile == 0)
        {
            AsTexture(m_pTexture0)->SetTxiLoaded(1);
            return;
        }

        int nBytes = nSize < 0 ? -1 - nSize : nSize;
        if (nBytes > 0)
        {
            AsTexture(m_pTexture0)
                ->SetTxiData(AurResGetDataBytes(0, pFile), nBytes);
            for (pLine = AurResGetNextLine(); pLine != 0;
                 pLine = AurResGetNextLine())
            {
                while (*pLine == 32 || *pLine == 9)
                {
                    pLine++;
                }
                ParseField(pLine);
            }
        }

        AurResFree(pFile, nSize);
        if (AsTexture(m_pTexture0) != 0)
        {
            AsTexture(m_pTexture0)->SetTxiLoaded(1);
        }
    }
}
// PC 0x004AB900
Material::Material()
{
    m_sBumpMapName = 0;
    m_sEnvMapName = 0;
    m_sBumpyShinyName = 0;
    m_pTexture0 = 0;
    m_pTexture1 = 0;
    m_pTexture2 = 0;
    m_pTexture3 = 0;
    m_pBumpMap = 0;
    m_pAltTexture = 0;
    m_lstBlendSrc.allocate(2);
    m_lstBlendDst.allocate(2);
    m_lstBlendSrc.Add(BLEND_SRC_ALPHA);
    m_lstBlendDst.Add(BLEND_ONE_MINUS_SRC_ALPHA);
    m_bDecal = 0;
    m_nRenderType = 1;
    materials.Add(this);
    m_fWaterAlpha = 1.0f;
}
// PC 0x004ABAB0
void Material::ClearTextures()
{
    CAurTexture* pTexture;

    pTexture = AsTexture(m_pTexture0);
    if (pTexture != 0)
    {
        m_pTexture0 = 0;
        AurTextureRelease(pTexture);
    }
    pTexture = AsTexture(m_pTexture1);
    if (pTexture != 0)
    {
        m_pTexture1 = 0;
        AurTextureRelease(pTexture);
    }
    pTexture = AsTexture(m_pTexture2);
    if (pTexture != 0)
    {
        m_pTexture2 = 0;
        AurTextureRelease(pTexture);
    }
    pTexture = AsTexture(m_pTexture3);
    if (pTexture != 0)
    {
        m_pTexture3 = 0;
        AurTextureRelease(pTexture);
    }
    pTexture = AsTexture(m_pBumpMap);
    if (pTexture != 0)
    {
        m_pBumpMap = 0;
        AurTextureRelease(pTexture);
    }
    pTexture = AsTexture(m_pEnvMap);
    if (pTexture != 0)
    {
        m_pEnvMap = 0;
        AurTextureRelease(pTexture);
    }
    pTexture = AsTexture(m_pAltTexture);
    if (pTexture != 0)
    {
        m_pAltTexture = 0;
        AurTextureRelease(pTexture);
    }
}

static inline bool IsTextureName(char* sName)
{
    return sName != 0 && *sName != 0 && strcmp("NULL", sName) != 0;
}
// PC 0x004ABBD0
void Material::InitializeTextures(char* sTexture0, char* sTexture1,
                                  char* sTexture2, char* sTexture3)
{
#ifdef _XBOX
    m_pTexture0 = LoadTexture(sTexture0);
    if (IsTextureName(sTexture0))
    {
        Init(sTexture0);
    }
    m_pTexture1 = LoadTexture(sTexture1);
    m_pTexture2 = LoadTexture(sTexture2);
    m_pTexture3 = LoadTexture(sTexture3);
#else
    CAurTexture* pTexture;

    if (IsTextureName(sTexture0))
    {
        pTexture = AurTextureGetReference(sTexture0, 0);
    }
    else
    {
        pTexture = AurTextureGetNULL();
    }
    m_pTexture0 = pTexture;
    if (IsTextureName(sTexture0))
    {
        Init(sTexture0);
    }

    if (IsTextureName(sTexture1))
    {
        pTexture = AurTextureGetReference(sTexture1, 0);
    }
    else
    {
        pTexture = AurTextureGetNULL();
    }
    m_pTexture1 = pTexture;

    if (IsTextureName(sTexture2))
    {
        pTexture = AurTextureGetReference(sTexture2, 0);
    }
    else
    {
        pTexture = AurTextureGetNULL();
    }
    m_pTexture2 = pTexture;

    if (IsTextureName(sTexture3))
    {
        pTexture = AurTextureGetReference(sTexture3, 0);
    }
    else
    {
        pTexture = AurTextureGetNULL();
    }
    m_pTexture3 = pTexture;
#endif

    if (m_sBumpMapName != 0)
    {
        if (IsTextureName(m_sBumpMapName))
        {
            m_pBumpMap = AurTextureGetReference(m_sBumpMapName, 0);
        }
        delete[] m_sBumpMapName;
        m_sBumpMapName = 0;
    }

    if (m_sBumpyShinyName != 0)
    {
        if (IsTextureName(m_sBumpyShinyName))
        {
            m_pEnvMap = AurTextureGetReference(m_sBumpyShinyName, 0);
            AsTexture(m_pEnvMap)->GetReferencedTexture()->m_bEnvironmentMapped =
                1;
        }
        delete[] m_sBumpyShinyName;
        m_sBumpyShinyName = 0;
    }

    if (m_sEnvMapName != 0)
    {
        if (IsTextureName(m_sEnvMapName))
        {
            m_pEnvMap = AurTextureGetReference(m_sEnvMapName, 0);
            AsTexture(m_pEnvMap)->GetReferencedTexture()->m_bEnvironmentMapped =
                1;
        }
        AsTexture(m_pTexture0)->SetIsEnvironmentMapped(true);
        delete[] m_sEnvMapName;
        m_sEnvMapName = 0;
    }
}
// PC 0x004ABDE0
void Material::LoadTextures(Part* pPart)
{
    char* sTexture0;
    char* sTexture1 = 0;
    PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
    PartEmitter* pEmitter = pPart->AsPartEmitter();

    if (pTriMesh != 0)
    {
        PartTriMeshData* pData = (PartTriMeshData*)pTriMesh->m_pNode;
        sTexture0 = pData->m_sTexture0;
        sTexture1 = pData->m_sTexture1;
    }
    else if (pEmitter != 0)
    {
        sTexture0 = ((MdlNodeEmitter*)pEmitter->m_pNode)->m_sTexture;
    }
    else
    {
        return;
    }

    InitializeTextures(sTexture0, sTexture1, 0, 0);
}
// PC 0x004ABE50
void Material::ReplaceTexture(char* sOld, char* sNew, char* sLayer, int* pTXI,
                              int nFlags)
{
    if (AsTexture(m_pTexture0)->IsNull() != 0)
    {
        if (sLayer == 0)
        {
            return;
        }
        CAurTexture* pOld = AsTexture(m_pTexture0);
        m_pTexture0 = AurTextureGetReference(sLayer, 0);
        AurTextureRelease(pOld);
    }

    if (sNew != 0 && strlen(sNew) != 0)
    {
        if (AurTextureReplaceSingleReference(AsTexture(m_pTexture0), sOld, sNew,
                                             sNew, pTXI, nFlags) != 0)
        {
            AurTextureSetLayered(AsTexture(m_pTexture0), sNew, pTXI, nFlags);
        }
    }
    else
    {
        AurTextureSetLayered(AsTexture(m_pTexture0), sNew, pTXI, nFlags);
    }
}
// PC 0x004ABF00
void Material::ReplaceTexture(char* sOld, char* sNew)
{
    if (AsTexture(m_pTexture0) != 0 && AsTexture(m_pTexture0)->IsNull() == 0)
    {
        if (_stricmp(AsTexture(m_pTexture0)->GetName(), "P_BaoDur_Arm") != 0)
        {
            AurTextureReplaceSingleReference(AsTexture(m_pTexture0), sOld,
                                             sNew, 0, 0, 0);
        }
        return;
    }

    if (sNew == 0)
    {
        return;
    }
    CAurTexture* pOld = AsTexture(m_pTexture0);
    m_pTexture0 = AurTextureGetReference(sOld, sNew);
    AurTextureRelease(pOld);
}
// PC 0x00420990
unsigned char Material::HasNormalMap()
{
    return 0;
}

#ifdef _XBOX

void AurReportError(int nLine, const char* sFile, const char* sMessage);
#endif
// PC 0x004ABF80
unsigned char Material::HasBumpMap()
{
    char sMessage[256];

    if (AsTexture(m_pBumpMap) != 0)
    {
        if (AsTexture(m_pBumpMap)->IsFullyLoaded() &&
            !AsTexture(m_pBumpMap)->IsBumpMap())
        {
            sprintf(sMessage, "Invalid bumpmap: %s.tga",
                    AsTexture(m_pBumpMap)->GetName());
#ifdef _XBOX
            AurReportError(697, "aurmaterial.cpp", sMessage);
#else
            MessageBoxA(0, sMessage, "Invalid Bumpmap!",
                        MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
#endif
            CAurTexture* pBumpMap = AsTexture(m_pBumpMap);
            m_pBumpMap = 0;
            AurTextureRelease(pBumpMap);
            return 0;
        }
        return 1;
    }
    return 0;
}
// PC 0x004AC010
Material::~Material()
{
    ClearTextures();
    materials.Remove(this);
}
