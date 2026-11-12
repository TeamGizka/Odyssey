#include <string.h>
#include <stdio.h>
#include <math.h>
#include "aurmodel.h"
#include "aurlightmanager.h"
#include "aurconsole.h"
#include "aurmodel.h"

static inline void AurModelParseTrace3();
static inline void AurModelParseTrace2();
static inline void AurModelParseTrace1();
#define AURMODELPARSE_TRACE() AurModelParseTrace3()

MdlNode* ParseNode(char* sField, MaxTree* pTree);
char* AurResGetNextLine();
void* AurResGetData(int nBytes);
Model* currentmodel;
char* skipspace(char* sText);
char* firstword(char* sLine);
int Parse(char* sText, Vector& vVector);
void Parse(char* sText, char* sValue);
int Parse(char* sText, float& fValue);
int Parse(char* sText, int& nValue);
void Parse(char* sText, short& nValue);
int Parse(char* sText, bool& bValue);
int Parse(char* sText, MaxFace_PostProcess& face);
int Parse(char* sText, SkinVertexWeight& weight);
int Parse(char* sText, unsigned int* pValues);

#ifdef _XBOX

void Parse(char* sText, unsigned int& nValue);
#define AURPARSE_XBOX_NO_ENDLIST_SKIP
#else
inline void Parse(char* sText, unsigned int& nValue)
{
    int nInteger;
    Parse(sText, nInteger);
    nValue = nInteger;
}
#endif
void Parse(char* sText, Quaternion& qOrientation);
template <class T> void Parse(char* sText, AurKey<T>& key);
template <class T> void Parse(char* sText, List<T>& lstValues);
#include "aurparse.h"

Model* BinarySearchModel(char* sName, int nFirst, int nLast);
void BinaryInsertModel(Model* pModel, int nFirst, int nLast);
extern float cullvalueforkeys;
// PC 0x00479BA0
template <> inline void List<AurKey<char[32]> >::allocate(int nAllocated)
{
    AurKey<char[32]>* pOldList = m_pList;
    m_nAllocated = nAllocated;
    m_pList = new AurKey<char[32]>[m_nAllocated];
    for (int i = 0; i < m_nCount; i++)
        m_pList[i] = pOldList[i];
    if (pOldList != 0)
        delete[] pOldList;
}
// PC 0x00479C20
void Parse(char* sText, Quaternion& qOrientation)
{
    float fAngle;
    Vector vAxis;

    sscanf(sText, "%f %f %f %f", &vAxis.m_fX, &vAxis.m_fY, &vAxis.m_fZ,
           &fAngle);
    if (vAxis.m_fX == 0.0f && vAxis.m_fY == 0.0f && vAxis.m_fZ == 0.0f)
    {
        qOrientation = Quaternion();
    }
    else
    {
        qOrientation = Quaternion(vAxis, fAngle);
    }
}
// PC 0x00479D20
void Parse(char* sText, AurGenericKey& key)
{
    key.m_nValues = sscanf(sText, "%f %f %f %f %f %f %f %f %f %f",
                          &key.m_fTime, &key.m_afValues[0], &key.m_afValues[1],
                          &key.m_afValues[2], &key.m_afValues[3], &key.m_afValues[4],
                          &key.m_afValues[5], &key.m_afValues[6], &key.m_afValues[7],
                          &key.m_afValues[8]);
    if (key.m_nValues == AurGenericKey::AXIS_ANGLE_FIELD_COUNT)
    {
        static Vector v;
        v.m_fX = key.m_afValues[0];
        v.m_fY = key.m_afValues[1];
        v.m_fZ = key.m_afValues[2];
        Quaternion q(v, key.m_afValues[3]);
        key.m_afValues[0] = q.m_fX;
        key.m_afValues[1] = q.m_fY;
        key.m_afValues[2] = q.m_fZ;
        key.m_afValues[3] = q.m_fW;
    }
}
// PC 0x004796F0
char* skipspace(char* sText)
{
    if (sText == 0)
    {
        return sText;
    }

    while (*sText == ' ' || *sText == '\t')
    {
        sText++;
    }

    return sText;
}
// PC 0x00479710
char* firstword(char* sLine)
{

    static char buf[1024];
    int nLength;

    for (nLength = 0; sLine != 0; sLine++)
    {
        if (*sLine == 0 || *sLine == ' ' || *sLine == '\t' || *sLine == '\n' ||
            *sLine == '\r')
        {
            break;
        }
        buf[nLength++] = *sLine;
    }
    buf[nLength] = 0;

    return buf;
}
// PC 0x00479750
int Parse(char* sText, Vector& vVector)
{
    return sscanf(sText, "%f %f %f", &vVector.m_fX, &vVector.m_fY,
                  &vVector.m_fZ);
}
// PC 0x00479770
void Parse(char* sText, char* sValue)
{
    if (sscanf(sText, "%s", sValue) == 0)
    {
        strcpy(sValue, (char*)0);
    }
}
// PC 0x004797A0
int Parse(char* sText, float& fValue)
{
    return sscanf(sText, "%f", &fValue);
}
// PC 0x004797C0
int Parse(char* sText, int& nValue)
{
    char sValue[64];
    int nResult;

    nResult = sscanf(sText, "%d", &nValue);
    if (nResult == 0)
    {
        sValue[0] = 0;
        sscanf(sText, "%s", sValue);
        if (stricmp("TRUE", sValue) == 0)
        {
            nValue = 1;
        }
        nResult = stricmp("FALSE", sValue);
        if (nResult == 0)
        {
            nValue = 0;
        }
    }

    return nResult;
}
// PC 0x00479830
void Parse(char* sText, short& nValue)
{
    int nInteger;

    sscanf(sText, "%d", &nInteger);
    nValue = (short)nInteger;
}
// PC 0x00479860
int Parse(char* sText, bool& bValue)
{
    char sValue[8];
    int nResult;

    sValue[0] = 0;
    sscanf(sText, "%s", sValue);
    if (stricmp("true", sValue) == 0)
    {
        bValue = true;
    }
    if (stricmp("false", sValue) == 0)
    {
        bValue = false;
    }
    if (stricmp("1", sValue) == 0)
    {
        bValue = true;
    }
    nResult = stricmp("0", sValue);
    if (nResult == 0)
    {
        bValue = false;
    }

    return nResult;
}
// PC 0x004798F0
int Parse(char* sText, MaxFace_PostProcess& face)
{
    if (face.m_pFaces != 0)
    {
        delete[] face.m_pFaces;
    }
    if (face.m_pFaceRemap != 0)
    {
        delete[] face.m_pFaceRemap;
    }
    if (face.m_pFaceIndices != 0)
    {
        delete[] face.m_pFaceIndices;
    }

    face.m_pFaceIndices = new int[12];
    face.m_pFaces = new int[3];
    face.m_pFaceRemap = new int[3];
    return sscanf(sText, "%d %d %d %d %d %d %d %d", &face.m_pFaces[0],
                  &face.m_pFaces[1], &face.m_pFaces[2], &face.m_nFaces,
                  &face.m_pFaceIndices[0], &face.m_pFaceIndices[1],
                  &face.m_pFaceIndices[2], &face.m_nFaceIndices);
}
// PC 0x00479980
int Parse(char* sText, SkinVertexWeight& weight)
{
    int nWeights;

    weight.m_apBones[0] = new char[32];
    weight.m_apBones[1] = new char[32];
    weight.m_apBones[2] = new char[32];
    weight.m_apBones[3] = new char[32];
    sprintf(weight.m_apBones[1], "ROOT");
    weight.m_afWeights[1] = 0.0f;
    sprintf(weight.m_apBones[2], "ROOT");
    weight.m_afWeights[2] = 0.0f;
    sprintf(weight.m_apBones[3], "ROOT");
    weight.m_afWeights[3] = 0.0f;
    nWeights = sscanf(sText, "%s %f %s %f %s %f %s %f", weight.m_apBones[0],
                      &weight.m_afWeights[0], weight.m_apBones[1],
                      &weight.m_afWeights[1], weight.m_apBones[2],
                      &weight.m_afWeights[2], weight.m_apBones[3],
                      &weight.m_afWeights[3]) /
               2;
    weight.m_anBoneIndices[1] = -1;
    weight.m_anBoneIndices[0] = -1;
    weight.m_nWeights = nWeights;
    return nWeights;
}

void classification(char* sType)
{
#ifdef _XBOX
    if (stricmp(sType, "forcedistortion") == 0)
    {
        currentmodel->m_Classification.m_bSceneHitDisabled = 1;
    }
    if (stricmp(sType, "heatdistortion") == 0)
    {
        currentmodel->m_Classification.m_bSceneHitExcluded = 1;
    }
    else if (stricmp(sType, "character") == 0)
    {
        currentmodel->m_Classification.m_bCharacter = 1;
    }
    else if (stricmp(sType, "ambient") == 0)
    {
        currentmodel->m_Classification.m_bSkipVisibilityCheck = 1;
    }
    else if (stricmp(sType, "effect") == 0 || stricmp(sType, "effects") == 0 || stricmp(sType, "track") == 0)
    {
        currentmodel->m_Classification.m_bEmitterBounds = 1;
    }
    else if (stricmp(sType, "door") == 0)
    {
        currentmodel->m_Classification.m_bDoor = 1;
    }
    else if (stricmp(sType, "lightsaber") == 0)
    {
        currentmodel->m_Classification.m_bRenderEarly = 1;
    }
    else if (stricmp(sType, "item") == 0)
    {
        currentmodel->m_Classification.m_bAlternateShadowCaster = 1;
    }
    else if (stricmp(sType, "placeable") == 0)
    {
        currentmodel->m_Classification.m_bAlternateShadowCaster = 1;
        currentmodel->m_Classification.m_bHitFilterOverride = 1;
    }
#else
    if (stricmp(sType, "character") == 0)
    {
        currentmodel->m_Classification.m_bCharacter = 1;
    }
    else if (stricmp(sType, "ambient") == 0)
    {
        currentmodel->m_Classification.m_bSkipVisibilityCheck = 1;
    }
    else if (stricmp(sType, "track") == 0)
    {
        currentmodel->m_Classification.m_bEmitterBounds = 1;
    }
    else if (stricmp(sType, "door") == 0)
    {
        currentmodel->m_Classification.m_bDoor = 1;
    }
    else if (stricmp(sType, "lightsaber") == 0)
    {
        currentmodel->m_Classification.m_bRenderEarly = 1;
    }
    else if (stricmp(sType, "placeable") == 0)
    {
        currentmodel->m_Classification.m_bAlternateShadowCaster = 1;
        currentmodel->m_Classification.m_bHitFilterOverride = 1;
    }
#endif
}

const char* setanimationscale(char* sCommand)
{
    float fScale = 1.0f;

    sscanf(sCommand, "%f", &fScale);
    currentmodel->m_fAnimationScale = fScale;
    return "done";
}
// PC 0x00479A30
int Parse(char* sText, unsigned int* pValues)
{
    int nCount;
    int i;
    char* sLine;

    nCount = 0;
    sscanf(sText, "%d", &nCount);
    if (nCount > 0)
    {
        i = 0;
        do
        {
            sLine = AurResGetNextLine();
            sscanf(sLine, "%d", pValues);
            i++;
            pValues++;
        } while (i < nCount);
    }

    return nCount;
}
// PC 0x00479AE0
template <class T> void Parse(char* sText, AurKey<T>& key)
{
    int nChars;

    sscanf(sText, "%f%n", &key.m_fTime, &nChars);
    Parse(skipspace(sText + nChars), key.m_Value);
}

int compressquaternions = 1;
float cullvalueforkeys = 0.0001f;
int cullunnecessary = 1;
// PC 0x00479E10
void Parse(char* sText, AABB*& pAABB)
{
    sText = skipspace(sText);
    Vector vMinimum;
    Vector vMaximum;
    int nFaceIndex;
    int nSplitMask;
    int nFields = sscanf(sText, "%f %f %f %f %f %f %d %d",
                         &vMinimum.m_fX, &vMinimum.m_fY, &vMinimum.m_fZ,
                         &vMaximum.m_fX, &vMaximum.m_fY, &vMaximum.m_fZ,
                         &nFaceIndex, &nSplitMask);
    pAABB = new AABB;
    pAABB->m_nFaceIndex = nFaceIndex;
    pAABB->m_vMinimum = vMinimum;
    pAABB->m_vMaximum = vMaximum;
    if (nFields == 7)
    {
        vMinimum = vMinimum - Vector(AABB_BOUNDING_TOLERANCE,
                                     AABB_BOUNDING_TOLERANCE,
                                     AABB_BOUNDING_TOLERANCE);
        vMaximum = vMaximum + Vector(AABB_BOUNDING_TOLERANCE,
                                     AABB_BOUNDING_TOLERANCE,
                                     AABB_BOUNDING_TOLERANCE);
    }
    if (pAABB->m_nFaceIndex == AABB::INTERNAL_NODE)
    {
        char* sLine = AurResGetNextLine();
        Parse(sLine, pAABB->m_pLeftChild);
        sLine = AurResGetNextLine();
        Parse(sLine, pAABB->m_pRightChild);
        if (nFields == 7)
        {
            Vector vLeft = pAABB->m_pLeftChild->m_vMinimum +
                           pAABB->m_pLeftChild->m_vMaximum;
            Vector vRight = pAABB->m_pRightChild->m_vMinimum +
                            pAABB->m_pRightChild->m_vMaximum;
            Vector vDelta = vRight - vLeft;
            if ((vDelta.m_fX < 0.0f ? -vDelta.m_fX : vDelta.m_fX) >
                (vDelta.m_fY < 0.0f ? -vDelta.m_fY : vDelta.m_fY) &&
                (vDelta.m_fX < 0.0f ? -vDelta.m_fX : vDelta.m_fX) >
                (vDelta.m_fZ < 0.0f ? -vDelta.m_fZ : vDelta.m_fZ))
                pAABB->m_nSplitMask = 1 << (vDelta.m_fX > 0.0f ?
                    AABB::POSITIVE_X : AABB::NEGATIVE_X);
            else if ((vDelta.m_fY < 0.0f ? -vDelta.m_fY : vDelta.m_fY) >
                     (vDelta.m_fZ < 0.0f ? -vDelta.m_fZ : vDelta.m_fZ))
                pAABB->m_nSplitMask = 1 << (vDelta.m_fY > 0.0f ?
                    AABB::POSITIVE_Y : AABB::NEGATIVE_Y);
            else
                pAABB->m_nSplitMask = 1 << (vDelta.m_fZ > 0.0f ?
                    AABB::POSITIVE_Z : AABB::NEGATIVE_Z);
        }
        else
            pAABB->m_nSplitMask = nSplitMask;
    }
}
// PC 0x0047A170
void CullUnnecessaryKeys(List<AurGenericKey>& keys)
{
    for (int i = keys.m_nCount - 2; i > 1; i--)
    {
        if (fabs(keys[i].m_afValues[0] - keys[i - 1].m_afValues[0]) < cullvalueforkeys &&
            fabs(keys[i].m_afValues[1] - keys[i - 1].m_afValues[1]) < cullvalueforkeys &&
            fabs(keys[i].m_afValues[2] - keys[i - 1].m_afValues[2]) < cullvalueforkeys &&
            fabs(keys[i].m_afValues[3] - keys[i - 1].m_afValues[3]) < cullvalueforkeys &&
            fabs(keys[i - 2].m_afValues[0] - keys[i - 1].m_afValues[0]) < cullvalueforkeys &&
            fabs(keys[i - 2].m_afValues[1] - keys[i - 1].m_afValues[1]) < cullvalueforkeys &&
            fabs(keys[i - 2].m_afValues[2] - keys[i - 1].m_afValues[2]) < cullvalueforkeys &&
            fabs(keys[i - 2].m_afValues[3] - keys[i - 1].m_afValues[3]) < cullvalueforkeys)
        {
            keys.DelIndex(i - 1);
        }
    }
}
// PC 0x0047A280
Model* BinarySearchModel(char* sName, int nFirst, int nLast)
{
    int nMiddle;
    int nCompare;

    while (nLast >= nFirst)
    {
        nMiddle = (nFirst + nLast) / 2;
        nCompare = stricmp(sName, Models[nMiddle]->m_sName);
        if (nCompare == 0)
        {
            return Models[nMiddle];
        }
        if (nCompare < 0)
        {
            nLast = nMiddle - 1;
        }
        else
        {
            nFirst = nMiddle + 1;
        }
    }

    return 0;
}

int countnewcontrollers = 0;

int uselastsupermodelname = 0;
char* lastsupermodelname = 0;
// PC 0x0047AA70
template <> void Parse<char*>(char* sText, List<char*>& lstValues)
{
    int nCount = 0;
    if (sscanf(sText, "%d", &nCount) > 0)
    {
        lstValues.SetSize(nCount);
        for (int i = 0; i < lstValues.m_nCount; i++)
        {
            lstValues[i] = new char[128];
            Parse(AurResGetNextLine(), lstValues[i]);
        }
    }
    else
    {
        char* sLine = AurResGetNextLine();
        while (sLine != 0)
        {
            if (strncmp(skipspace(sLine), "endlist", 7) == 0)
                break;
            if (lstValues.m_nCount == 0)
                lstValues.SetSize(1);
            else if (lstValues.m_nCount == nCount)
                lstValues.Add(lstValues[nCount - 1]);
            lstValues[nCount] = new char[128];
            Parse(sLine, lstValues[nCount]);
            nCount++;
            sLine = AurResGetNextLine();
        }
#ifndef _XBOX
        skipspace(skipspace(sLine) + 7);
#endif
    }
}
// PC 0x0047AC30
template <> void Parse<unsigned int*>(char* sText, List<unsigned int*>& lstValues)
{
    int nCount;
    int nValues;
    int i;
    char* sLine;

    nCount = 0;
    sscanf(sText, "%d", &nCount);
    lstValues.SetSize(nCount);
    for (i = 0; i < lstValues.m_nCount; i++)
    {
        sLine = AurResGetNextLine();
        sscanf(sLine, "%d", &nValues);
        lstValues[i] = new unsigned int[nValues];
        Parse(sLine, lstValues[i]);
    }
}
// PC 0x0047A2E0
Model* BinaryFindModel(char* sName)
{
    int nLast;

    nLast = Models.m_nCount - 1;
    return BinarySearchModel(sName, 0, nLast);
}
#ifndef _XBOX
// PC 0x0047A300
Model* FindModel(char* sName)
{
    Model* pModel;
    Model* pPreviousModel;
    MaxTree* pTree;
    char sBuffer[256];

    if (sName == 0 || stricmp(sName, "NULL") == 0) return 0;

    int nCount = Models.m_nCount;
    pModel = BinarySearchModel(sName, 0, nCount - 1);
    if (pModel != 0)
        return pModel;

    pPreviousModel = currentmodel;
    currentmodel = 0;
    IODispatcher* pDispatcher = IODispatcher::GetRef();
    pTree = pDispatcher->ReadSync(sName);
    if (pTree != 0)
        currentmodel = pTree->AsModel();
    pModel = currentmodel;
    currentmodel = pPreviousModel;
    if (pModel != 0)
    {
        if (stricmp(pModel->m_sName, sName) != 0)
        {
            FILE* pErrorFile = fopen("AR_ERROR.LOG", "w");
            if (pErrorFile)
            {
                sprintf(sBuffer, "Names Differ: %s %s", pModel->m_sName, sName);
                fprintf(pErrorFile, "Names Differ: %s %s", pModel->m_sName, sName);
                fclose(pErrorFile);
            }
        }
        return pModel;
    }
    return 0;
}
#endif
Model* LoadModel(FILE* pModelFile, FILE* pVertexFile)  // PC 0x0047A3F0
{
    if (pModelFile == 0)
        return 0;

    Model* pPreviousModel = currentmodel;
    currentmodel = 0;
    IODispatcher* pDispatcher = IODispatcher::GetRef();
    MaxTree* pTree = pDispatcher->ReadSync(pModelFile, pVertexFile);
    if (pTree != 0)
        currentmodel = pTree->AsModel();
    Model* pModel = currentmodel;
    currentmodel = pPreviousModel;
    if (pModel == 0)
        return 0;

    for (int i = 0; i < Models.m_nCount; i++)
    {
        if (Models.m_pList[i] == pModel)
            continue;
        if (stricmp(Models.m_pList[i]->m_sName, pModel->m_sName) == 0)
        {
            delete pModel;
            return Models.m_pList[i];
        }
    }
    return pModel;
}
// PC 0x0047ACE0
MdlNode* FindMdlNode(MdlNode* pNode, char* sName)
{
    int nChildren;
    int i;
    MdlNode* pFound;

    if (pNode == 0)
    {
        return 0;
    }

    if (stricmp(sName, pNode->TranslateNodeName()) == 0)
    {
        return pNode;
    }

    nChildren = pNode->m_lChildren.m_nCount;
    for (i = 0; i < nChildren; i++)
    {
        pFound = FindMdlNode(pNode->m_lChildren.m_pList[i], sName);
        if (pFound != 0)
        {
            return pFound;
        }
    }

    return 0;
}
#ifdef _XBOX

MdlNode* FindMdlNode(MaxTree* pTree, char* sName)
{
    if (pTree == 0)
    {
        return 0;
    }
    return FindMdlNode(pTree->m_pRootNode, sName);
}
#endif
// PC 0x0047AD40
void CompressQuaternionKey(List<AurGenericKey>& keys)
{
    List<AurGenericKey> compressed;
    Quaternion q;
    for (int i = 0; i < keys.m_nCount; i++)
    {
        AurGenericKey key;
        key.m_fTime = keys[i].m_fTime;
        q.m_fX = keys[i].m_afValues[0];
        q.m_fY = keys[i].m_afValues[1];
        q.m_fZ = keys[i].m_afValues[2];
        q.m_fW = keys[i].m_afValues[3];
        key.m_nValues = AurGenericKey::COMPRESSED_QUATERNION_FIELD_COUNT;
        if (q.m_fW < 0.0f)
            q = q * -1.0f;

        float fX = max(0.0f, min((float)AurGenericKey::QUATERNION_XY_MAX,
                               (q.m_fX + 1.0f) * AurGenericKey::QUATERNION_XY_SCALE));
        float fY = max(0.0f, min((float)AurGenericKey::QUATERNION_XY_MAX,
                               (q.m_fY + 1.0f) * AurGenericKey::QUATERNION_XY_SCALE));
        float fZ = max(0.0f, min((float)AurGenericKey::QUATERNION_Z_MAX,
                               (q.m_fZ + 1.0f) * AurGenericKey::QUATERNION_Z_SCALE));
        int nX, nY, nZ;
        roundToInt(nX, fX);
        roundToInt(nY, fY);
        roundToInt(nZ, fZ);
        *(unsigned int*)&key.m_afValues[0] =
            (((nZ << AurGenericKey::QUATERNION_XY_BITS) | nY)
             << AurGenericKey::QUATERNION_XY_BITS) | nX;
        compressed.Add(key);
    }
    keys = compressed;
}
// PC 0x0047B040
void AddGenericControllerSingle(char* sName, char* sText,
                                List<NewController>& controllers,
                                List<float>& values, void* pOffset, void* pType)
{
    NewController controller;
    controller.m_nOffset = (int)pOffset;
    controller.m_nNumKeys = (short)(int)pType;
    controller.m_nRows = 1;

    AurGenericKey key;
    Parse(sText, key);
    if (key.m_nValues == AurGenericKey::SINGLE_AXIS_ANGLE_FIELD_COUNT)
    {
        static Vector v;
        v.m_fX = key.m_fTime;
        v.m_fY = key.m_afValues[0];
        v.m_fZ = key.m_afValues[1];
        Quaternion q(v, key.m_afValues[2]);
        key.m_fTime = q.m_fX;
        key.m_afValues[0] = q.m_fY;
        key.m_afValues[1] = q.m_fZ;
        key.m_afValues[2] = q.m_fW;
    }

    float* pFields = (float*)&key;
#ifdef _XBOX
    for (int i = key.m_nValues - 1; i >= 0; i--)
        ((float*)&key)[i + 1] = ((float*)&key)[i];
#else
    for (int i = key.m_nValues - 1; i >= 0; i--)
        pFields[i + 1] = pFields[i];
#endif
    key.m_nValues++;
    key.m_fTime = 0.0f;

    switch (key.m_nValues - 1)
    {
    case 1:
        controller.m_nValueFormat = NewController::VALUE_TYPE_FLOAT;
        break;
    case 3:
        controller.m_nValueFormat = NewController::VALUE_TYPE_VECTOR;
        break;
    case 4:
        controller.m_nValueFormat = NewController::VALUE_TYPE_QUATERNION;
        break;
    }

    controller.m_nKeyIndex = values.m_nCount;
    values.Add(key.m_fTime);
    controller.m_nColumnIndex = values.m_nCount;
    for (int i = 1; i < key.m_nValues; i++)
        values.Add(pFields[i]);
    controllers.Add(controller);
    countnewcontrollers++;
}
// PC 0x0047B290
void AddGenericController(char* sName, char* sText,
                          List<NewController>& controllers,
                          List<float>& values, void* pOffset, void* pType)
{
    List<AurGenericKey> keys;
    NewController controller;
    controller.m_nOffset = (int)pOffset;
    controller.m_nNumKeys = (short)(int)pType;
    Parse(sText, keys);
    controller.m_nRows = keys.m_nCount;
    if (keys.m_nCount == 0)
        return;

    switch (keys[0].m_nValues - 1)
    {
    case 1:
        controller.m_nValueFormat = NewController::VALUE_TYPE_FLOAT;
        break;
    case 3:
        controller.m_nValueFormat = NewController::VALUE_TYPE_VECTOR;
        break;
    case 4:
        controller.m_nValueFormat = NewController::VALUE_TYPE_QUATERNION;
        if (cullunnecessary)
        {
            CullUnnecessaryKeys(keys);
            controller.m_nRows = keys.m_nCount;
        }
        if (compressquaternions &&
            !currentmodel->IsQuaternionCompressionDisabled())
        {
            CompressQuaternionKey(keys);
            controller.m_nValueFormat = NewController::VALUE_TYPE_COMPRESSED_QUATERNION;
        }
        break;
    }

    controller.m_nKeyIndex = values.m_nCount;
    for (int i = 0; i < keys.m_nCount; i++)
        values.Add(keys[i].m_fTime);
    controller.m_nColumnIndex = values.m_nCount;
    for (int i = 0; i < keys.m_nCount; i++)
    {
        for (int j = 1; j < keys[i].m_nValues; j++)
            values.Add(keys[i].m_afValues[j - 1]);
    }
    countnewcontrollers += keys.m_nCount;
    controllers.Add(controller);
}
// PC 0x0047B520
void AddBezierController(char* sName, char* sText,
                         List<NewController>& controllers,
                         List<float>& values, void* pOffset, void* pType)
{
    List<AurGenericKey> keys;
    NewController controller;
    controller.m_nOffset = (int)pOffset;
    controller.m_nNumKeys = (short)(int)pType;
    Parse(sText, keys);
    controller.m_nRows = keys.m_nCount;
    if (keys.m_nCount == 0)
        return;

    controller.m_nValueFormat = ((keys[0].m_nValues - 1) / 3) |
                              NewController::INTERPOLATION_BEZIER;
    controller.m_nKeyIndex = values.m_nCount;
    for (int i = 0; i < keys.m_nCount; i++)
        values.Add(keys[i].m_fTime);
    controller.m_nColumnIndex = values.m_nCount;
    for (int i = 0; i < keys.m_nCount; i++)
    {
        for (int j = 1; j < keys[i].m_nValues; j++)
            values.Add(keys[i].m_afValues[j - 1]);
    }
    countnewcontrollers += keys.m_nCount;
    controllers.Add(controller);
}
void MdlNode::InternalParseField(char* sField)  // PC 0x0047B760
{
    if (disablepostprocessing == 0)
    {
        if (stricmp("position", firstword(sField)) == 0)
            Parse(sField + 8, m_vPosition);
        if (stricmp("orientation", firstword(sField)) == 0)
            Parse(sField + 11, m_qOrientation);

        if (stricmp("position", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((Part*)0)->m_vPosition,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("positionkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((Part*)0)->m_vPosition, &((MdlNode*)0)->m_vPosition);
        else if (stricmp("positionbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((Part*)0)->m_vPosition, &((MdlNode*)0)->m_vPosition);

        if (stricmp("orientation", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((Part*)0)->m_qOrientation,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("orientationkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((Part*)0)->m_qOrientation, &((MdlNode*)0)->m_qOrientation);
        else if (stricmp("orientationbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 20,
                m_lstControllers, m_lstControllerData,
                &((Part*)0)->m_qOrientation, &((MdlNode*)0)->m_qOrientation);

        Vector vWireColor;
        if (stricmp("wirecolor", firstword(sField)) == 0)
            Parse(sField + 9, vWireColor);

        if (stricmp("scale", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 5,
                m_lstControllers, m_lstControllerData,
                &((Part*)0)->m_fScale,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("scalekey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((Part*)0)->m_fScale,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("scalebezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((Part*)0)->m_fScale,
                (void*)NewController::NODE_OFFSET_NONE);
    }

    if (strncmp("parent", sField, 6) == 0)
    {
        char sParent[64];
        sscanf(sField + 6, "%s", sParent);
        if (stricmp(sParent, "NULL") == 0)
        {
            m_pParentNode = 0;
            m_pModel->m_pRootNode = this;
        }
        else
        {
#ifdef _XBOX
            MdlNode* pParent = FindMdlNode(m_pModel, sParent);
#else
            MdlNode* pParent;
            if (m_pModel == 0)
                pParent = 0;
            else
                pParent = FindMdlNode(m_pModel->m_pRootNode, sParent);
#endif
            m_pParentNode = pParent;
            if (pParent != 0)
                pParent->m_lChildren.Add(this);
        }
    }
}
// PC 0x0047BAB0
void MdlNodeEmitter::InternalParseField(char* sField)
{
    if (disablepostprocessing == 0)
    {
        bool bPointToPoint = false;
        int nPointToPointSel = 0;
        bool bAffectedByWind = false;
        bool bTinted = false;
        bool bBounce = false;
        int nRandom = 0;
        bool bInherit = false;
        bool bInheritVelocity = false;
        bool bInheritLocal = false;
        bool bSplat = false;
        bool bInheritPart = false;
        bool bDepthTexture = false;

        if (stricmp("p2p", firstword(sField)) == 0)
            Parse(sField + 3, bPointToPoint);
        if (stricmp("p2p_sel", firstword(sField)) == 0)
            Parse(sField + 7, nPointToPointSel);
        if (stricmp("affectedByWind", firstword(sField)) == 0)
            Parse(sField + 14, bAffectedByWind);
        if (stricmp("m_isTinted", firstword(sField)) == 0)
            Parse(sField + 10, bTinted);
        if (stricmp("bounce", firstword(sField)) == 0)
            Parse(sField + 6, bBounce);
        if (stricmp("random", firstword(sField)) == 0)
            Parse(sField + 6, nRandom);
        if (stricmp("inherit", firstword(sField)) == 0)
            Parse(sField + 7, bInherit);
        if (stricmp("inheritvel", firstword(sField)) == 0)
            Parse(sField + 10, bInheritVelocity);
        if (stricmp("inherit_local", firstword(sField)) == 0)
            Parse(sField + 13, bInheritLocal);
        if (stricmp("splat", firstword(sField)) == 0)
            Parse(sField + 5, bSplat);
        if (stricmp("inherit_part", firstword(sField)) == 0)
            Parse(sField + 12, bInheritPart);
        if (stricmp("depth_texture", firstword(sField)) == 0)
            Parse(sField + 13, bDepthTexture);
        if (bPointToPoint)
            EnablePointToPoint();
        if (nPointToPointSel == POINT_TO_POINT_SELECTED)
            EnablePointToPointSel();
        if (bAffectedByWind)
            EnableAffectedByWind();
        if (bTinted)
            EnableTinted();
        if (bBounce)
            EnableBouncing();
        if (nRandom & RANDOM_ENABLED)
            EnableRandom();
        if (nRandom & RANDOM_ORDER)
            EnableRandomOrder();
        if (bInherit)
            EnableInheriting();
        if (bInheritVelocity)
            EnableInheritingVelocity();
        if (bInheritLocal)
            EnableInheritingLocal();
        if (bSplat)
            EnableSplat();
        if (bInheritPart)
            EnableInheritingPart();
        if (bDepthTexture)
            EnableDepthTexture();

        if (stricmp("renderorder", firstword(sField)) == 0)
        {
#ifdef _XBOX
            Parse(sField + 11, m_nRenderOrder);
#else
            int nRenderOrder;
            sscanf(sField + 11, "%d", &nRenderOrder);
            m_nRenderOrder = nRenderOrder;
#endif
        }
        if (stricmp("spawntype", firstword(sField)) == 0)
            Parse(sField + 9, m_nSpawnType);
        if (stricmp("update", firstword(sField)) == 0)
            Parse(sField + 6, m_sUpdate);
        if (stricmp("render", firstword(sField)) == 0)
            Parse(sField + 6, m_sRender);
        if (stricmp("blend", firstword(sField)) == 0)
            Parse(sField + 5, m_sBlend);
        if (stricmp("texture", firstword(sField)) == 0)
            Parse(sField + 7, m_sTexture);
        if (stricmp("chunkName", firstword(sField)) == 0)
            Parse(sField + 9, m_sChunkName);
        if (stricmp("xgrid", firstword(sField)) == 0)
            Parse(sField + 5, m_nXGrid);
        if (stricmp("ygrid", firstword(sField)) == 0)
            Parse(sField + 5, m_nYGrid);
        if (stricmp("loop", firstword(sField)) == 0)
            Parse(sField + 4, m_nLoop);
        if (stricmp("deadspace", firstword(sField)) == 0)
#ifdef _XBOX
            Parse(sField + 9, m_fDeadSpace);
#else
            sscanf(sField + 9, "%f", &m_fDeadSpace);
#endif
        if (stricmp("twosidedtex", firstword(sField)) == 0)
            Parse(sField + 11, m_nTwoSidedTex);
        if (stricmp("blastRadius", firstword(sField)) == 0)
#ifdef _XBOX
            Parse(sField + 11, m_fBlastRadius);
#else
            sscanf(sField + 11, "%f", &m_fBlastRadius);
#endif
        if (stricmp("blastLength", firstword(sField)) == 0)
#ifdef _XBOX
            Parse(sField + 11, m_fBlastLength);
#else
            sscanf(sField + 11, "%f", &m_fBlastLength);
#endif
        if (stricmp("m_bFrameBlending", firstword(sField)) == 0)
            Parse(sField + 16, m_bFrameBlending);
        if (stricmp("m_sDepthTextureName", firstword(sField)) == 0)
            Parse(sField + 19, m_sDepthTextureName);
        if (stricmp("numBranches", firstword(sField)) == 0)
            Parse(sField + 11, m_nNumBranches);
        if (stricmp("controlptsmoothing", firstword(sField)) == 0)
            Parse(sField + 18, m_nControlPointSmoothing);

        if (stricmp("blurlength", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBlurLength, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("blurlengthkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBlurLength, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("blurlengthbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 19,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBlurLength, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("detonate", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_bDetonated, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("detonatekey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_bDetonated, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("detonatebezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_bDetonated, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("velocity", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fVelocity, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("velocitykey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fVelocity, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("velocitybezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fVelocity, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("randvel", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 7,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fVelocityRandom, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("randvelkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fVelocityRandom, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("randvelbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 16,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fVelocityRandom, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("birthrate", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 9,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBirthRate, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("birthratekey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 12,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBirthRate, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("birthratebezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBirthRate, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("m_fRandomBirthRate", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBirthRateRandom, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("m_fRandomBirthRatekey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 21,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBirthRateRandom, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("m_fRandomBirthRatebezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 27,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBirthRateRandom, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("colorStart", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_vColorStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("colorStartkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_vColorStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("colorStartbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 19,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_vColorStart, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("colorMid", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_vColorMid, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("colorMidkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_vColorMid, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("colorMidbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_vColorMid, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("colorEnd", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_vColorEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("colorEndkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_vColorEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("colorEndbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_vColorEnd, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("percentStart", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 12,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPercentStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("percentStartkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 15,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPercentStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("percentStartbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 21,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPercentStart, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("percentMid", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPercentMid, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("percentMidkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPercentMid, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("percentMidbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 19,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPercentMid, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("percentEnd", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPercentEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("percentEndkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPercentEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("percentEndbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 19,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPercentEnd, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("alphaStart", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fAlphaStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("alphaStartkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fAlphaStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("alphaStartbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 19,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fAlphaStart, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("alphaMid", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fAlphaMid, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("alphaMidkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fAlphaMid, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("alphaMidbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fAlphaMid, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("alphaEnd", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fAlphaEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("alphaEndkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fAlphaEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("alphaEndbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fAlphaEnd, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("sizeStart", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 9,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSizeStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeStartkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 12,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSizeStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeStartbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSizeStart, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("sizeMid", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 7,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSizeMid, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeMidkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSizeMid, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeMidbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 16,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSizeMid, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("sizeEnd", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 7,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSizeEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeEndkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSizeEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeEndbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 16,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSizeEnd, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("sizeStart_y", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSizeStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeStart_ykey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSizeStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeStart_ybezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 20,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSizeStart, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("sizeMid_y", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 9,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSizeMid, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeMid_ykey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 12,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSizeMid, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeMid_ybezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSizeMid, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("sizeEnd_y", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 9,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSizeEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeEnd_ykey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 12,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSizeEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("sizeEnd_ybezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSizeEnd, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("frameStart", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fFrameStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("frameStartkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fFrameStart, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("frameStartbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 19,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fFrameStart, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("frameEnd", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fFrameEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("frameEndkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fFrameEnd, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("frameEndbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fFrameEnd, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("lifeExp", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 7,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLifeTime, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lifeExpkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLifeTime, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lifeExpbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 16,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLifeTime, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("mass", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 4,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fMass, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("masskey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 7,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fMass, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("massbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fMass, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("spread", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 6,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fSpread, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("spreadkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 9,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fSpread, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("spreadbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 15,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fSpread, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("particleRot", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fParticleRotation, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("particleRotkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fParticleRotation, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("particleRotbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 20,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fParticleRotation, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("fps", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 3,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fFramesPerSecond, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("fpskey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 6,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fFramesPerSecond, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("fpsbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 12,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fFramesPerSecond, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("xsize", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 5,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSize, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("xsizekey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSize, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("xsizebezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fXSize, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("ysize", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 5,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSize, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("ysizekey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSize, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("ysizebezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fYSize, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("bounce_co", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 9,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBounceCoefficient, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("bounce_cokey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 12,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBounceCoefficient, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("bounce_cobezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fBounceCoefficient, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("lightningDelay", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningDelay, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lightningDelaykey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningDelay, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lightningDelaybezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 23,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningDelay, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("lightningRadius", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 15,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningRadius, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lightningRadiuskey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningRadius, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lightningRadiusbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 24,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningRadius, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("lightningScale", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningScale, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lightningScalekey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningScale, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lightningScalebezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 23,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningScale, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("lightningSubDiv", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 15,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningSubDivisions, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lightningSubDivkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningSubDivisions, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lightningSubDivbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 24,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningSubDivisions, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("lightningZigzag", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 15,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningZigZag, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lightningZigzagkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningZigZag, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("lightningZigzagbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 24,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fLightningZigZag, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("p2p_bezier2", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPointToPointBezier2, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("p2p_bezier2key", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPointToPointBezier2, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("p2p_bezier2bezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 20,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPointToPointBezier2, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("p2p_bezier3", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPointToPointBezier3, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("p2p_bezier3key", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPointToPointBezier3, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("p2p_bezier3bezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 20,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fPointToPointBezier3, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("combinetime", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 11,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fCombineTime, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("combinetimekey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fCombineTime, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("combinetimebezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 20,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fCombineTime, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("drag", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 4,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fDrag, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("dragkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 7,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fDrag, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("dragbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fDrag, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("grav", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 4,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fGravity, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("gravkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 7,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fGravity, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("gravbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fGravity, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("threshold", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 9,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fThreshold, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("thresholdkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 12,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fThreshold, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("thresholdbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fThreshold, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("targetsize", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fTargetSize, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("targetsizekey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fTargetSize, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("targetsizebezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 19,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fTargetSize, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("numcontrolpts", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fNumControlPoints, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("numcontrolptskey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 16,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fNumControlPoints, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("numcontrolptsbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 22,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fNumControlPoints, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("controlptradius", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 15,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fControlPointRadius, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("controlptradiuskey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 18,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fControlPointRadius, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("controlptradiusbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 24,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fControlPointRadius, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("controlptdelay", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fControlPointDelay, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("controlptdelaykey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fControlPointDelay, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("controlptdelaybezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 23,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fControlPointDelay, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("tangentspread", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fTangentSpread, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("tangentspreadkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 16,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fTangentSpread, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("tangentspreadbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 22,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fTangentSpread, (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("tangentlength", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fTangentLength, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("tangentlengthkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 16,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fTangentLength, (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("tangentlengthbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 22,
                m_lstControllers, m_lstControllerData,
                &((PartEmitter*)0)->m_fTangentLength, (void*)NewController::NODE_OFFSET_NONE);

    }
    MdlNode::InternalParseField(sField);
}
// PC 0x0047E260
void MdlNodeCamera::InternalParseField(char* sField)
{
    MdlNode::InternalParseField(sField);
}
// PC 0x0047E270
void MdlNodeReference::InternalParseField(char* sField)
{
    if (disablepostprocessing == 0)
    {
        if (stricmp("refModel", firstword(sField)) == 0)
        {
            Parse(sField + 8, m_sRefModel);
        }
        if (stricmp("reattachable", firstword(sField)) == 0)
        {
            Parse(sField + 12, m_bReattachable);
        }
    }

    return MdlNode::InternalParseField(sField);
}
// PC 0x0047E2E0
MdlNode* ParseNode(char* sField, MaxTree* pTree)
{
    char sType[64];
    char sName[64];
    MdlNode* pNode;

    sscanf(sField, "%s %s", sType, sName);
    if (stricmp(sType, "camera") == 0)
        pNode = new MdlNodeCamera(sName);
    else if (stricmp(sType, "emitter") == 0)
        pNode = new MdlNodeEmitter(sName);
    else if (stricmp(sType, "light") == 0)
        pNode = new MdlNodeLight(sName);
    else if (stricmp(sType, "trimesh") == 0)
        pNode = new MdlNodeTriMesh(sName);
    else if (stricmp(sType, "animmesh") == 0)
        pNode = new MdlNodeAnimMesh(sName);
    else if (stricmp(sType, "danglymesh") == 0)
        pNode = new MdlNodeDanglyMesh(sName);
    else if (stricmp(sType, "reference") == 0)
        pNode = new MdlNodeReference(sName);
    else if (stricmp(sType, "aabb") == 0)
        pNode = new MdlNodeAABB(sName);
    else if (stricmp(sType, "skin") == 0)
        pNode = new MdlNodeSkin(sName);
    else if (stricmp(sType, "lightsaber") == 0)
        pNode = new MdlNodeLightsaber(sName);
    else
        pNode = new MdlNode(sName);

    pNode->m_pModel = pTree;
#ifdef _XBOX
    for (char* sLine = skipspace(AurResGetNextLine()); sLine != 0; sLine = skipspace(AurResGetNextLine()))
    {
        if (strncmp(sLine, "endnode", 7) == 0) break; pNode->ParseFieldDispatch(sLine); }
#else
    bool bEndNode = false; while (!bEndNode) {
        char* sLine = AurResGetNextLine(); if (sLine == 0) bEndNode = true; else {
            while (*sLine == ' ' || *sLine == '\t')
                sLine++;
            if (strncmp(sLine, "endnode", 7) == 0)
                bEndNode = true;
            else
                pNode->ParseFieldDispatch(sLine);
        }
    }
#endif

    if (pTree->AsAnimation())
    {
        MdlNode* pAnimationNode = new MdlNode(sName);
        pAnimationNode->m_lstControllerData = pNode->m_lstControllerData;
        pAnimationNode->m_nNodeID = pNode->m_nNodeID;
        pAnimationNode->m_lstControllers = pNode->m_lstControllers;
        pAnimationNode->m_pModel = pTree;
        if (pNode->m_pParentNode)
        {
            pAnimationNode->m_pParentNode = pNode->m_pParentNode;
            pAnimationNode->m_pParentNode->m_lChildren.Remove(pNode);
            pAnimationNode->m_pParentNode->m_lChildren.Add(pAnimationNode);
        }
        else if (pTree->m_pRootNode == pNode)
            pTree->m_pRootNode = pAnimationNode;

        MdlNode* pOldNode = pNode;
        pNode = pAnimationNode;
        if (pOldNode->AsMdlNodeAABB())
            delete pOldNode->AsMdlNodeAABB();
        else if (pOldNode->AsMdlNodeAnimMesh())
            delete pOldNode->AsMdlNodeAnimMesh();
        else if (pOldNode->AsMdlNodeCamera())
            delete pOldNode->AsMdlNodeCamera();
        else if (pOldNode->AsMdlNodeDanglyMesh())
            delete pOldNode->AsMdlNodeDanglyMesh();
        else if (pOldNode->AsMdlNodeEmitter())
            delete pOldNode->AsMdlNodeEmitter();
        else if (pOldNode->AsMdlNodeLight())
            delete pOldNode->AsMdlNodeLight();
        else if (pOldNode->AsMdlNodeLightsaber())
            delete pOldNode->AsMdlNodeLightsaber();
        else if (pOldNode->AsMdlNodeReference())
            delete pOldNode->AsMdlNodeReference();
        else if (pOldNode->AsMdlNodeSkin())
            delete pOldNode->AsMdlNodeSkin();
        else if (pOldNode->AsMdlNodeTriMesh())
            delete pOldNode->AsMdlNodeTriMesh();
        else
            delete pOldNode;
    }

    if (pNode->m_pModel->AsModel())
    {
        char* sNodeName = new char[strlen(sName) + 1];
        strcpy(sNodeName, sName);
        pNode->m_pModel->AsModel()->m_lNames.Add(sNodeName);
        pNode->m_nIndex = pNode->m_pModel->AsModel()->m_lNames.m_nCount - 1;
    }
    else
    {
        int i = ((Model*)pNode->m_pModel->AsAnimation()->m_pBinaryData)->m_lNames.m_nCount;
        while (i--)
        {
            if (stricmp(((Model*)pNode->m_pModel->AsAnimation()->m_pBinaryData)->m_lNames[i],
                         sName) == 0)
            {
                pNode->m_nIndex = i;
                break;
            }
        }
        if (i == -1)
        {
            char* sNodeName = new char[strlen(sName) + 1];
            strcpy(sNodeName, sName);
            ((Model*)pNode->m_pModel->AsAnimation()->m_pBinaryData)->m_lNames.Add(sNodeName);
            pNode->m_nIndex =
                ((Model*)pNode->m_pModel->AsAnimation()->m_pBinaryData)->m_lNames.m_nCount - 1;
        }
    }

    if (pNode->m_pParentNode == 0 && pTree->m_pRootNode != pNode)
        throw 0;
    pNode->m_nNodeID = (short)pTree->m_nNodeCount;
    pTree->m_nNodeCount++;
    if (disablepostprocessing == 0)
        pNode->PostProcessDispatch();
    return pNode;
}
// PC 0x0047EA70
int MaxTree::InternalParseField(char* sField)
{
    int nResult;

    nResult = strncmp(sField, "node", 4);
    if (nResult == 0)
    {
        return (int)ParseNode(sField + 4, this);
    }

    return nResult;
}
// PC 0x0047EAA0
int Model::InternalParseField(char* sField)
{
    if (stricmp("radius", firstword(sField)) == 0)
    {
        Parse(sField + 6, m_fRadius);
    }
    if (stricmp("bmin", firstword(sField)) == 0)
    {
        Parse(sField + 4, m_vBoundingMin);
    }
    if (stricmp("bmax", firstword(sField)) == 0)
    {
        Parse(sField + 4, m_vBoundingMax);
    }

#ifdef _XBOX
    return MaxTree::InternalParseField(sField);
}

#else
    int nResult = strncmp(sField, "node", 4);
    if (nResult == 0)
    {
        return (int)ParseNode(sField + 4, this);
    }
    return nResult;
}

#endif
// PC 0x0047F350
void MdlNodeLight::InternalParseField(char* sField)
{
    if (disablepostprocessing == 0)
    {
        if (stricmp("color", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 5,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_vColor,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("colorkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_vColor,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("colorbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_vColor,
                (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("radius", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 6,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fRadius,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("radiuskey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 9,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fRadius,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("radiusbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 15,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fRadius,
                (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("shadowradius", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 12,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fShadowRadius,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("shadowradiuskey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 15,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fShadowRadius,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("shadowradiusbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 21,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fShadowRadius,
                (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("verticaldisplacement", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 20,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fShadowHeight,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("verticaldisplacementkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 23,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fShadowHeight,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("verticaldisplacementbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 29,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fShadowHeight,
                (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("multiplier", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 10,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fColorMultiplier,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("multiplierkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 13,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fColorMultiplier,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("multiplierbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 19,
                m_lstControllers, m_lstControllerData,
                &((PartLight*)0)->m_fColorMultiplier,
                (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("ambientonly", firstword(sField)) == 0)
        {
            Parse(sField + 11, m_bAmbientOnly);
        }
        if (stricmp("shadow", firstword(sField)) == 0)
        {
            Parse(sField + 6, m_bShadow);
        }
        if (stricmp("isdynamic", firstword(sField)) == 0)
        {
            Parse(sField + 9, m_nDynamicType);
        }
        if (stricmp("nDynamicType", firstword(sField)) == 0)
        {
            Parse(sField + 12, m_nDynamicType);
        }
        if (stricmp("affectdynamic", firstword(sField)) == 0)
        {
            Parse(sField + 13, m_bAffectDynamic);
        }
        if (stricmp("lightpriority", firstword(sField)) == 0)
        {
            Parse(sField + 13, m_nPriority);
        }
        if (stricmp("generateflare", firstword(sField)) == 0)
        {
            Parse(sField + 13, m_bGenerateFlare);
        }
        if (stricmp("texturenames", firstword(sField)) == 0)
        {
            Parse<char*>(sField + 12, m_lstTextureNames);
        }
        if (stricmp("flaresizes", firstword(sField)) == 0)
        {
            Parse<float>(sField + 10, m_lstFlareSizes);
        }
        if (stricmp("flarepositions", firstword(sField)) == 0)
        {
            Parse<float>(sField + 14, m_lstFlarePositions);
        }
        if (stricmp("flarecolorshifts", firstword(sField)) == 0)
        {
            Parse<Vector>(sField + 16, m_lstFlareColorShifts);
        }
        if (stricmp("flareradius", firstword(sField)) == 0)
        {
#ifdef _XBOX
            Parse(sField + 11, m_fFlareRadius);
#else
            sscanf(sField + 11, "%f", &m_fFlareRadius);
#endif
        }
        if (stricmp("fadingLight", firstword(sField)) == 0)
        {
            Parse(sField + 11, m_bFadingLight);
        }
    }

    return MdlNode::InternalParseField(sField);
}
// PC 0x0047F900
void MdlNodeTriMesh::InternalParseField(char* sField)
{
    if (stricmp("texture0", firstword(sField)) == 0)
        Parse(sField + 8, m_sTexture0);
    if (stricmp("texture1", firstword(sField)) == 0)
        Parse(sField + 8, m_sTexture1);
    char sBitmap[64];
    sBitmap[0] = 0;
    if (stricmp("bitmap", firstword(sField)) == 0)
        Parse(sField + 6, sBitmap);
    if (sBitmap[0] != 0)
        strcpy(m_sTexture0, sBitmap);

    if (disablepostprocessing == 0)
    {
        if (m_pPostProcess == 0)
            m_pPostProcess = new MdlNodeTriMesh_PostProcess;
        MdlNodeTriMesh_PostProcess* pPostProcess = m_pPostProcess;
        void* pLightMap = 0;
        int i = 0;
        List<Vector>& lstPositions = pPostProcess->m_lPositions;
        List<Vector>& lstTextureCoordinates0 = pPostProcess->m_lTextureCoordinates0;
        List<Vector>& lstTextureCoordinates1 = pPostProcess->m_lTextureCoordinates1;
        List<Vector>& lstTextureCoordinates2 = pPostProcess->m_lTextureCoordinates2;
        List<Vector>& lstTextureCoordinates3 = pPostProcess->m_lTextureCoordinates3;
        List<Vector>& lstColors = pPostProcess->m_lColors;
        List<MaxFace_PostProcess>& lstFaces = pPostProcess->m_lFaces;

        if (stricmp("verts", firstword(sField)) == 0)
            Parse<Vector>(sField + 5, lstPositions);
        if (stricmp("tverts", firstword(sField)) == 0)
            Parse<Vector>(sField + 6, lstTextureCoordinates0);
        int bAnimateUV = 0;
        if (stricmp("animateuv", firstword(sField)) == 0)
            Parse(sField + 9, bAnimateUV);
        if (bAnimateUV != 0)
            m_bAnimateUV = 1;
        if (m_bAnimateUV != 0)
        {
            float& fUVDirectionX = m_fUVDirectionX;
            float& fUVDirectionY = m_fUVDirectionY;
            float& fUVJitter = m_fUVJitter;
            float& fUVJitterSpeed = m_fUVJitterSpeed;
            if (stricmp("uvdirectionx", firstword(sField)) == 0)
                Parse(sField + 12, fUVDirectionX);
            if (stricmp("uvdirectiony", firstword(sField)) == 0)
                Parse(sField + 12, fUVDirectionY);
            if (stricmp("uvjitter", firstword(sField)) == 0)
                Parse(sField + 8, fUVJitter);
            if (stricmp("uvjitterspeed", firstword(sField)) == 0)
                Parse(sField + 13, fUVJitterSpeed);
        }
        if (stricmp("tverts1", firstword(sField)) == 0)
            Parse<Vector>(sField + 7, lstTextureCoordinates1);
        if (stricmp("tverts2", firstword(sField)) == 0)
            Parse<Vector>(sField + 7, lstTextureCoordinates2);
        if (stricmp("tverts3", firstword(sField)) == 0)
            Parse<Vector>(sField + 7, lstTextureCoordinates3);
        if (stricmp("colors", firstword(sField)) == 0)
            Parse<Vector>(sField + 6, lstColors);
        if (stricmp("texindices1", firstword(sField)) == 0)
            Parse<Vector>(sField + 11, MaxFace_PostProcess::texindices1);
        if (stricmp("texindices2", firstword(sField)) == 0)
            Parse<Vector>(sField + 11, MaxFace_PostProcess::texindices2);
        if (stricmp("texindices3", firstword(sField)) == 0)
            Parse<Vector>(sField + 11, MaxFace_PostProcess::texindices3);

        if (m_sTexture1[0] != 0 && lstTextureCoordinates1.m_nCount > 0)
        {
            char* sPreviousField = sField;
            char sPreviousTexture[64];
            strcpy(sPreviousTexture, m_sTexture1);
            pLightMap = AurResGet(m_sTexture1, ".lmt", 0, true);
            if (pLightMap != 0)
            {
                List<int> lstOffsets(0);
                sField = AurResGetNextLine();
                while (sField != 0)
                {
                    while (*sField == ' ' || *sField == '\t')
                        ++sField;
                    if (stricmp("tverts1offset", firstword(sField)) == 0)
                        Parse<int>(sField + 13, lstOffsets);
                    if (stricmp("texture1", firstword(sField)) == 0)
                        Parse(sField + 8, m_sTexture1);
                    sField = AurResGetNextLine();
                }
                AurResFree(pLightMap, 0);
                if (lstTextureCoordinates1.m_nCount * 4 == lstOffsets.m_nCount)
                {
                    for (i = 0; i < lstTextureCoordinates1.m_nCount; ++i)
                    {
                        lstTextureCoordinates1[i] = (float)lstOffsets[i * 4] * lstTextureCoordinates1[i];
                        lstTextureCoordinates1[i].m_fX += lstOffsets[i * 4 + 1];
                        lstTextureCoordinates1[i].m_fY += lstOffsets[i * 4 + 2];
                        lstTextureCoordinates1[i] /= (float)lstOffsets[i * 4 + 3];
                    }
                }
            }
            sField = sPreviousField;
        }

        if (lstFaces.m_nCount == 0)
        {
            if (stricmp("faces", firstword(sField)) == 0)
                Parse<MaxFace_PostProcess>(sField + 5, lstFaces);
        }
        if (lstFaces.m_nCount > 0)
        {
            m_lFaces.SetSize(lstFaces.m_nCount);
            for (i = 0; i < lstFaces.m_nCount; ++i)
                m_lFaces[i].m_nSurfaceType = lstFaces[i].m_nFaceIndices;
        }

        if (stricmp("diffuse", firstword(sField)) == 0)
            Parse(sField + 7, m_vDiffuse);
        if (stricmp("ambient", firstword(sField)) == 0)
            Parse(sField + 7, m_vAmbient);
        if (stricmp("shadow", firstword(sField)) == 0)
            Parse(sField + 6, m_bShadow);
        if (stricmp("transparencyhint", firstword(sField)) == 0)
            Parse(sField + 16, m_nTransparencyHint);
        if (stricmp("beaming", firstword(sField)) == 0)
            Parse(sField + 7, m_bBeaming);
        if (stricmp("render", firstword(sField)) == 0)
            Parse(sField + 6, m_bRender);
        if (stricmp("rotatetexture", firstword(sField)) == 0)
            Parse(sField + 13, m_bRotateTexture);
        if (stricmp("bmin", firstword(sField)) == 0)
            Parse(sField + 4, m_vMinimum);
        if (stricmp("bmax", firstword(sField)) == 0)
            Parse(sField + 4, m_vMaximum);
        if (stricmp("vertexindicescount", firstword(sField)) == 0)
            Parse<int>(sField + 18, m_lstIndexCounts);
        if (stricmp("leftoverfaces", firstword(sField)) == 0)
            Parse<unsigned int>(sField + 13, m_lstTriangleIndices);
        if (stricmp("vertexindices", firstword(sField)) == 0)
            Parse<unsigned int*>(sField + 13, m_lstIndexArrays);
        if (stricmp("lightmapped", firstword(sField)) == 0)
            Parse(sField + 11, m_bLightMapped);
        if (stricmp("m_bIsBackgroundGeometry", firstword(sField)) == 0)
            Parse(sField + 23, m_bBackground);
        if (stricmp("dirt_enabled", firstword(sField)) == 0)
            Parse(sField + 12, m_bDirtEnabled);
        if (stricmp("dirt_texture", firstword(sField)) == 0)
        {
            int nDirtTexture;
            sscanf(sField + 12, "%d", &nDirtTexture);
            m_nDirtTexture = nDirtTexture;
        }
        if (stricmp("dirt_worldspace", firstword(sField)) == 0)
        {
            int nDirtCoordSpace;
            sscanf(sField + 15, "%d", &nDirtCoordSpace);
            m_nDirtCoordSpace = nDirtCoordSpace;
        }
        if (stricmp("hologram_donotdraw", firstword(sField)) == 0)
            Parse(sField + 18, m_bHideInHolograms);

        if (stricmp("selfillumcolor", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartTriMesh*)0)->m_vSelfIllumColor,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("selfillumcolorkey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 17,
                m_lstControllers, m_lstControllerData,
                &((PartTriMesh*)0)->m_vSelfIllumColor,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("selfillumcolorbezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 23,
                m_lstControllers, m_lstControllerData,
                &((PartTriMesh*)0)->m_vSelfIllumColor,
                (void*)NewController::NODE_OFFSET_NONE);

        if (stricmp("alpha", firstword(sField)) == 0)
            AddGenericControllerSingle(firstword(sField), sField + 5,
                m_lstControllers, m_lstControllerData,
                &((PartTriMesh*)0)->m_fAlpha,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("alphakey", firstword(sField)) == 0)
            AddGenericController(firstword(sField), sField + 8,
                m_lstControllers, m_lstControllerData,
                &((PartTriMesh*)0)->m_fAlpha,
                (void*)NewController::NODE_OFFSET_NONE);
        else if (stricmp("alphabezierkey", firstword(sField)) == 0)
            AddBezierController(firstword(sField), sField + 14,
                m_lstControllers, m_lstControllerData,
                &((PartTriMesh*)0)->m_fAlpha,
                (void*)NewController::NODE_OFFSET_NONE);
    }
    return MdlNode::InternalParseField(sField);
}
// PC 0x00480430
void MdlNodeLightsaber::InternalParseField(char* sField)
{
    return MdlNodeTriMesh::InternalParseField(sField);
}
// PC 0x00480440
void MdlNodeDanglyMesh::InternalParseField(char* sField)
{
    if (disablepostprocessing == 0)
    {
        if (stricmp("displacement", firstword(sField)) == 0)
        {
#ifdef _XBOX
            Parse(sField + 12, m_fDisplacement);
#else
            sscanf(sField + 12, "%f", &m_fDisplacement);
#endif
        }
        if (stricmp("period", firstword(sField)) == 0)
        {
#ifdef _XBOX
            Parse(sField + 6, m_fPeriod);
#else
            sscanf(sField + 6, "%f", &m_fPeriod);
#endif
        }
        if (stricmp("tightness", firstword(sField)) == 0)
        {
#ifdef _XBOX
            Parse(sField + 9, m_fTightness);
#else
            sscanf(sField + 9, "%f", &m_fTightness);
#endif
        }
        if (stricmp("constraints", firstword(sField)) == 0)
        {
            Parse<float>(sField + 11, m_lConstraints);
        }
    }

    return MdlNodeTriMesh::InternalParseField(sField);
}
// PC 0x00480520
void MdlNodeAnimMesh::InternalParseField(char* sField)
{
    if (disablepostprocessing == 0)
    {
        if (stricmp("sampleperiod", firstword(sField)) == 0)
        {
#ifdef _XBOX
            Parse(sField + 12, m_fSamplePeriod);
#else
            sscanf(sField + 12, "%f", &m_fSamplePeriod);
#endif
        }
        if (stricmp("animverts", firstword(sField)) == 0)
        {
            Parse<Vector>(sField + 9, m_lAnimationVectors1);
        }
        if (stricmp("animtverts", firstword(sField)) == 0)
        {
            Parse<Vector>(sField + 10, m_lAnimationVectors2);
        }
    }

    return MdlNodeTriMesh::InternalParseField(sField);
}
// PC 0x004805D0
void MdlNodeSkin::InternalParseField(char* sField)
{
    if (disablepostprocessing == 0)
    {
        if (stricmp("weights", firstword(sField)) == 0)
        {
            Parse<SkinVertexWeight>(sField + 7, m_lstSkinWeights);
        }
        if (stricmp("qbone_ref_inv", firstword(sField)) == 0)
        {
            Parse<Quaternion>(sField + 13, m_lstQBoneRefInv);
        }
        if (stricmp("tbone_ref_inv", firstword(sField)) == 0)
        {
            Parse<Vector>(sField + 13, m_lstTBoneRefInv);
        }
        if (stricmp("boneconstantindices", firstword(sField)) == 0)
        {
            Parse<int>(sField + 19, m_lstBoneConstantIndices);
        }
    }

    return MdlNodeTriMesh::InternalParseField(sField);
}
// PC 0x004806A0
void MdlNodeAABB::InternalParseField(char* sField)
{
    if (disablepostprocessing == 0)
    {
        if (stricmp("aabb", firstword(sField)) == 0)
        {
            Parse(sField + 4, m_pAABBData);
        }
    }

    return MdlNodeTriMesh::InternalParseField(sField);
}
// PC 0x004806F0
int Animation::InternalParseField(char* sField)
{
    char sName[128];
    if (strncmp("animlength", sField, 10) == 0)
    {
        sscanf(sField + 10, "%s %f", sName, &m_fLength);
    }
    if (strncmp("event", sField, 5) == 0)
    {
        AurKey<char[32]> key;
        key.m_Value[0] = 0;
        Parse(sField + 5, key);
        int i;
        for (i = 0; i < m_lEvents.m_nCount; i++)
        {
            if (!(key.m_fTime > m_lEvents[i].m_fTime))
                break;
        }
        m_lEvents.Insert(key, i);
    }
    if (stricmp("length", firstword(sField)) == 0)
    {
        Parse(sField + 6, m_fLength);
    }
    if (stricmp("transtime", firstword(sField)) == 0)
    {
        Parse(sField + 9, m_fTransitionTime);
    }
    if (stricmp("animroot", firstword(sField)) == 0)
    {
        Parse(sField + 8, m_sAnimRoot);
    }
    return MaxTree::InternalParseField(sField);
}
// PC 0x004808D0
void InsertModel(Model* pModel)
{
    return BinaryInsertModel(pModel, 0, Models.m_nCount - 1);
}
// PC 0x00480870
void BinaryInsertModel(Model* pModel, int nFirst, int nLast)
{
    AURMODELPARSE_TRACE();
    int nMiddle;

    while (nLast >= nFirst)
    {
        nMiddle = (nFirst + nLast) / 2;
        if (stricmp(pModel->m_sName, Models.m_pList[nMiddle]->m_sName) < 0)
        {
            nLast = nMiddle - 1;
        }
        else
        {
            nFirst = nMiddle + 1;
        }
    }

    return Models.Insert(pModel, nFirst);
}

static inline void AurModelParseTrace3()
{
    AurModelParseTrace2();
}

static inline void AurModelParseTrace2()
{
    AurModelParseTrace1();
}

static inline void AurModelParseTrace1()
{
}

#ifndef _XBOX
template <> void List<AurKey<char[32]> >::Insert(AurKey<char[32]> item, int nIndex)  // PC 0x0047EB70
{
    int nLast = m_nCount;
    if (m_nCount == m_nAllocated)
        allocate(m_nAllocated ? m_nAllocated * 2 : INITIAL_ALLOCATION);
    m_pList[m_nCount++] = item;
    for (; nLast > nIndex; nLast--)
        m_pList[nLast] = m_pList[nLast - 1];
    m_pList[nIndex] = item;
    AurModelParseTrace3();
}
#endif

#ifdef _XBOX
void Parse(char* sText, unsigned int& nValue)
{
    int nInteger;
    Parse(sText, nInteger);
    nValue = nInteger;
}
#endif

#ifdef _XBOX

int bRecordSuperModel = 0;
char* sSuperModelRecord = 0;

void newmodel(char* sName)
{
    currentmodel = new Model(sName);
}

void ReadTreeFields(char* sCommand, MaxTree* pTree)
{
    char sAnimRoot[64];

    for (char* sLine = skipspace(AurResGetNextLine()); sLine != 0; sLine = skipspace(AurResGetNextLine()))
    {
        if (strncmp(sLine, "done", 4) == 0 || strncmp(sLine, "end", 3) == 0)
        {
            break;
        }
        if (stricmp("sModelAnimationRootName", firstword(sLine)) == 0)
        {
            Parse(sLine + 23, sAnimRoot);
        }
        pTree->m_pfnParseField(pTree, sLine);
    }
    Model* pModel = pTree->AsModel();
    if (pModel != 0)
    {
        pModel->m_pRootNode->SortChildrenByTransparency();

        if ((pModel->m_pBinaryData = FindMdlNode(pTree, sAnimRoot)) == 0)
        {
            pModel->m_pBinaryData = pModel->m_pRootNode;
        }
    }
}

void beginmodelgeom(char* sCommand)
{
    if (currentmodel->m_pParentModel)
    {
        currentmodel->m_nNodeCount = currentmodel->m_pParentModel->m_nNodeCount + 1;
    }
    ReadTreeFields(sCommand, currentmodel);
    currentmodel->SynchronizeTree(currentmodel->m_pParentModel);
}

void secondaryclassification(char* sType)
{
    if (stricmp(sType, "sensitive") == 0)
    {
        currentmodel->m_Classification.m_bNoQuaternionCompression = 1;
    }
}

void ignorefog(char* sCommand)
{
    int nIgnore;

    sscanf(sCommand, "%d", &nIgnore);
    if (nIgnore == 1)
    {
        currentmodel->m_bModelOptionEnabled = 0;
    }
}

const char* setsupermodel(char* sCommand)
{
    char sSuperModel[64];
    char sName[64];

    if (sSuperModelRecord == 0 && bRecordSuperModel)
    {
        sSuperModelRecord = new char[64];
    }
    sscanf(sCommand, "%s%s", sName, sSuperModel);
    if (currentmodel->m_pParentModel)
    {
        currentmodel->m_pParentModel->DelRef(currentmodel);
    }
    strcpy(currentmodel->m_sSuperModel, sSuperModel);
    Model* pSuperModel = FindModel(sSuperModel);
    if (pSuperModel == currentmodel)
    {
        pSuperModel = 0;
    }
    currentmodel->m_pParentModel = pSuperModel;
    if (sSuperModelRecord)
    {
        strcpy(sSuperModelRecord, sSuperModel);
    }
    if (currentmodel->m_pParentModel)
    {
        currentmodel->m_pParentModel->AddRef(currentmodel);
    }
    return "done";
}

void newanim(char* sCommand)
{
    char sName[64];
    char sModel[64];

    sscanf(sCommand, "%s %s", sName, sModel);
    Animation* pAnimation = new Animation(sName);
    currentmodel->m_lAnimations.Add(pAnimation);

    pAnimation->m_pBinaryData = (unsigned char*)currentmodel;
    ReadTreeFields(sCommand, pAnimation);
    pAnimation->PruneToAnimRoot();
    pAnimation->SynchronizeTree(currentmodel);
}
#endif

#ifdef _XBOX

Model* FindModel(char* sName)
{
    Model* pModel;
    Model* pPreviousModel;
    MaxTree* pTree;
    char sBuffer[256];

    if (sName == 0 || stricmp(sName, "NULL") == 0)
        return 0;

    pModel = BinaryFindModel(sName);
    if (pModel != 0)
        return pModel;

    pPreviousModel = currentmodel;
    currentmodel = 0;
    IODispatcher* pDispatcher = IODispatcher::GetRef();
    pTree = pDispatcher->ReadSync(sName);
    if (pTree != 0)
        currentmodel = pTree->AsModel();
    pModel = currentmodel;
    currentmodel = pPreviousModel;
    if (pModel == 0)
        return 0;
    if (stricmp(pModel->m_sName, sName) != 0)
    {
        FILE* pErrorFile = fopen("AR_ERROR.LOG", "w");
        if (pErrorFile)
        {
            sprintf(sBuffer, "Names Differ: %s %s", pModel->m_sName, sName);
            fprintf(pErrorFile, "Names Differ: %s %s", pModel->m_sName, sName);
            fclose(pErrorFile);
        }
    }
    return pModel;
}
#endif
