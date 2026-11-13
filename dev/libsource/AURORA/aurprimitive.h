#ifndef __AURPRIMITIVE_H__
#define __AURPRIMITIVE_H__

class MdlNodeTriMesh;
class Part;
class PartSkin;
class PartTriMesh;
class Vector;
class NodeVertexDangly;
class NodeVertexAnim;
class VertexPrimitiveAnim;
class VertexPrimitiveDangly;
class VertexPrimitiveFlat;
class VertexPrimitiveSkinned;

#include <aurlist.h>
#include "../math3d/math3d.h"

class NodeVertex
{
public:

    static const unsigned char NODEVERTEX_ANIM_ID;
    static const unsigned char NODEVERTEX_DANGLY_ID;
    static const unsigned char NODEVERTEX_SKINNED_ID;

    enum VertexFlags
    {
        VERTEX_HAS_TEXCOORD0 = 1,
        VERTEX_HAS_TEXCOORD1 = 2,
        VERTEX_HAS_TEXCOORD2 = 4,
        VERTEX_HAS_TEXCOORD3 = 8
    };

    Vector m_vPosition;
    Vector m_vTexCoord0;
    Vector m_vTexCoord1;
    Vector m_vTexCoord2;
    Vector m_vTexCoord3;
    Vector m_vNormal;
    Vector m_vColor;
    unsigned char m_nFlags;
    char m_nType;
    NodeVertexAnim* AsNodeVertexAnim();
    NodeVertexDangly* AsNodeVertexDangly();
    NodeVertex(Vector*, Vector*, Vector*, Vector*, Vector*, Vector*, Vector*);
    unsigned char operator==(NodeVertex const&);
};

class NodeVertexAnim : public NodeVertex
{
public:

    List<Vector> m_lstPositions;
    List<Vector> m_lstNormals;
    List<Vector> m_lstTexCoords;
    ~NodeVertexAnim();
    unsigned char operator==(NodeVertexAnim const&);
    NodeVertexAnim(Vector*, Vector*, Vector*, Vector*, Vector*, Vector*,
                   Vector*, List<Vector>&, List<Vector>&, List<Vector>&);
};

class NodeVertexDangly : public NodeVertex
{
public:
    float m_fConstraint;
    NodeVertexDangly(Vector*, Vector*, Vector*, Vector*, Vector*, Vector*,
                     Vector*, float);
    unsigned char operator==(NodeVertexDangly const&);
};

class VertexPrimitive
{
public:
#ifdef _XBOX

    unsigned long m_nArrayPool;
#endif

    MdlNodeTriMesh* m_pNode;
    MdlNodeTriMesh* GetNode();
    Part* m_pPart;

    int m_nArrayCount;

    int m_bStatic;

    char m_bRebuild;
    VertexPrimitive(Part* pPart);

    virtual ~VertexPrimitive();

    PartTriMesh* GetPartTriMesh();
    virtual VertexPrimitiveFlat* AsVertexPrimitiveFlat() { return 0; }  // PC 0x004065A0 GOG 0x00406570
    virtual VertexPrimitiveSkinned* AsVertexPrimitiveSkinned() { return 0; }  // PC 0x004065A0 GOG 0x00406570
    virtual VertexPrimitiveAnim* AsVertexPrimitiveAnim() { return 0; }  // PC 0x004065A0 GOG 0x00406570
    virtual VertexPrimitiveDangly* AsVertexPrimitiveDangly() { return 0; }  // PC 0x004065A0 GOG 0x00406570
    virtual void BuildVertexArrays() = 0;
    virtual void SetupVertexArraysStandard();

    virtual void SetupArrays() = 0;
    virtual int SetupArraysVAR(unsigned char*) = 0;
    virtual unsigned char* ReconnectArraysVAR(unsigned char*) = 0;
#ifdef _XBOX

    virtual void DeleteArrayPool();
#endif
    virtual void Rebuild() { m_bRebuild = 1; }  // PC 0x00450BE0 GOG 0x00450D20
};

class VertexPrimitiveFlat : public VertexPrimitive
{
public:
    int m_nPool;
    virtual ~VertexPrimitiveFlat();
    virtual VertexPrimitiveFlat* AsVertexPrimitiveFlat() { return this; }  // PC 0x00698BD0 GOG 0x00698980
    virtual void BuildVertexArrays();
    virtual void SetupArrays();
    virtual int SetupArraysVAR(unsigned char*) { return 0; }  // PC 0x006955A0 GOG 0x00695350
    virtual unsigned char* ReconnectArraysVAR(unsigned char*);

    virtual void UpdateLightVectors(const Vector&);
    virtual void UpdateHalfAngleVectors(const Vector&, const Vector&);
    virtual void UpdateFogValues(const Vector&, float);
    VertexPrimitiveFlat(PartTriMesh*);
};

class VertexPrimitiveAnim : public VertexPrimitiveFlat
{
public:

    unsigned long m_nVertexPool;
    unsigned long m_nTexCoordPool;
    ~VertexPrimitiveAnim();
    virtual VertexPrimitiveAnim* AsVertexPrimitiveAnim() { return this; }  // PC 0x00698BD0 GOG 0x00698980
    virtual void BuildVertexArrays();
    VertexPrimitiveAnim(PartTriMesh*);
};

class VertexPrimitiveDangly : public VertexPrimitiveFlat
{
public:

    Vector* m_pOffsets;
    ~VertexPrimitiveDangly();
    virtual VertexPrimitiveDangly* AsVertexPrimitiveDangly() { return this; }  // PC 0x00698BD0 GOG 0x00698980
    virtual void BuildVertexArrays();
    VertexPrimitiveDangly(PartTriMesh*);
};

class VertexPrimitiveSkinned : public VertexPrimitiveFlat
{
public:
    ~VertexPrimitiveSkinned();
    virtual VertexPrimitiveSkinned* AsVertexPrimitiveSkinned() { return this; }  // PC 0x00698BD0 GOG 0x00698980
    virtual void BuildVertexArrays();
    virtual unsigned char* ReconnectArraysVAR(unsigned char*);
    VertexPrimitiveSkinned(PartSkin*);
};

void RenderFlat(VertexPrimitiveFlat* pPrimitive);
#ifdef _XBOX
void RenderEnvironmentMapped(VertexPrimitiveFlat* pPrimitive, bool bFog);
#else
void RenderEnvironmentMapped(VertexPrimitiveFlat* pPrimitive, bool bVertexProgram,
                              bool bATI);
#endif

#endif
