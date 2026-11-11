#ifndef __GAMEOBJECTARRAY_H__
#define __GAMEOBJECTARRAY_H__

#include "exobase.h"



const unsigned long OBJECT_INVALID = 0x7F000000;



const unsigned long OBJECT_LISTTYPE_MASK = 0x40000000;


const unsigned long OBJECT_CLIENT_ID_MASK = 0x80000000;


enum CreatureSize
{
    CREATURE_SIZE_INVALID = 0,
    CREATURE_SIZE_TINY = 1,
    CREATURE_SIZE_SMALL = 2,
    CREATURE_SIZE_MEDIUM = 3,
    CREATURE_SIZE_LARGE = 4,
    CREATURE_SIZE_HUGE = 5
};


const unsigned short VFX_DUR_INVISIBILITY = 2001;
const unsigned short VFX_COM_SPARKS_PARRY_METAL = 4011;
const unsigned short VFX_ARD_HEAT_SHIMMER = 5003;


class CSWCObject;
class CSWSObject;
class CSWCDoor;
class CSWSDoor;
class CSWCModule;
class CSWSModule;
class CSWCArea;
class CSWSArea;
class CSWCCreature;
class CSWSCreature;
class CSWCItem;
class CSWSItem;
class CSWCTrigger;
class CSWSTrigger;
class CSWCProjectile;
class CSWSPlaceable;
class CSWCPlaceable;
class CSWSAreaOfEffectObject;
class CSWCAreaOfEffectObject;
class CSWSWaypoint;
class CSWSEncounter;
class CSWSStore;
class CSWCStore;
class CSWSSoundObject;
class CSWCSoundObject;




class CGameObject
{
public:
    CGameObject(unsigned char nObjectType, unsigned long oidSelf);


    unsigned long GetId() { return m_nId; }

    virtual ~CGameObject()
    {
    }
    // PC 0x005EE1C0
    virtual void SetId(unsigned long nId)
    {
    }
    // PC 0x006049D0
    virtual void ResetUpdateTimes(unsigned long nDay, unsigned long nTime)
    {
    }
    // PC 0x004065A0
    virtual CSWCObject* AsSWCObject()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSObject* AsSWSObject()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCDoor* AsSWCDoor()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSDoor* AsSWSDoor()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCModule* AsSWCModule()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSModule* AsSWSModule()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCArea* AsSWCArea()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSArea* AsSWSArea()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCCreature* AsSWCCreature()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSCreature* AsSWSCreature()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCItem* AsSWCItem()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSItem* AsSWSItem()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCTrigger* AsSWCTrigger()
    {
        return NULL;
    }


    // PC 0x004065A0
    virtual CSWSTrigger* AsSWSTrigger()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCProjectile* AsSWCProjectile()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSPlaceable* AsSWSPlaceable()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCPlaceable* AsSWCPlaceable()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSAreaOfEffectObject* AsSWSAreaOfEffectObject()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCAreaOfEffectObject* AsSWCAreaOfEffectObject()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSWaypoint* AsSWSWaypoint()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSEncounter* AsSWSEncounter()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCStore* AsSWCStore()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSStore* AsSWSStore()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWSSoundObject* AsSWSSoundObject()
    {
        return NULL;
    }
    // PC 0x004065A0
    virtual CSWCSoundObject* AsSWCSoundObject()
    {
        return NULL;
    }

    static const unsigned char OBJECT_TYPE_GUI;
    static const unsigned char OBJECT_TYPE_TILE;
    static const unsigned char OBJECT_TYPE_MODULE;
    static const unsigned char OBJECT_TYPE_AREA;
    static const unsigned char OBJECT_TYPE_CREATURE;
    static const unsigned char OBJECT_TYPE_AREAOFEFFECTOBJECT;
    static const unsigned char OBJECT_TYPE_ITEM;
    static const unsigned char OBJECT_TYPE_PROJECTILE;
    static const unsigned char OBJECT_TYPE_PLACEABLE;
    static const unsigned char OBJECT_TYPE_DOOR;
    static const unsigned char OBJECT_TYPE_TRIGGER;
    static const unsigned char OBJECT_TYPE_AREA_OF_EFFECT;
    static const unsigned char OBJECT_TYPE_WAYPOINT;
    static const unsigned char OBJECT_TYPE_ENCOUNTER;
    static const unsigned char OBJECT_TYPE_STORE;
    static const unsigned char OBJECT_TYPE_SOUND;

    unsigned long m_nId;
    unsigned char m_nObjectType;
};


class CGameObject;

class CGameObjectArrayNode
{
public:
    CGameObjectArrayNode()
    {
        m_oidObject = 0;
        m_pObject = NULL;
        m_pNext = NULL;
    }

    unsigned long m_oidObject;
    CGameObject* m_pObject;
    CGameObjectArrayNode* m_pNext;
};

class CGameObjectArray
{
public:
    CGameObjectArray(int bExternal);
    ~CGameObjectArray();
    unsigned char AddObjectAtPos(unsigned long oidObject, CGameObject* pObject);
    unsigned char AddExternalObject(unsigned long& oidObject,
                                    CGameObject* pObject, int bAscending);
    unsigned char AddInternalObject(unsigned long& oidObject,
                                    CGameObject* pObject, int bDescending);
    unsigned char Delete(unsigned long oidObject, CGameObject** ppObject);
    unsigned char Delete(unsigned long oidObject);
    unsigned char GetGameObject(unsigned long oidObject,
                                CGameObject** ppObject);

    static const unsigned long ARRAY_SIZE;
    static const unsigned char SUCCESS;
    static const unsigned long INVALIDOBJID;
    static const unsigned long MINCHAROBJID;
    static const unsigned long LISTTYPE_MASK;
    static const unsigned long MAXCHAROBJID;
    static const unsigned long MINOBJECTID;
    static const unsigned long MAXOBJECTID;
    static const unsigned long CHAROBJECT_MASK;
    static const unsigned long LISTINDEX_MASK;
    static const unsigned long ARRAYINDEX_MASK;
    static const unsigned long ARRAYINDEX_SHIFT;
    static const unsigned char BAD_ID;
    static const unsigned char OUT_OF_MEMORY;
    static const unsigned char INVALIDLIST;
    static const unsigned char NULL_GAMEOBJECT;
    static const unsigned char INTERNALOBJECT;
    static const unsigned char EXTERNALOBJECT;

    CGameObjectArrayNode** m_pArray;
    unsigned long m_nNextObjectId[2];
    unsigned long m_nPrevObjectId[2];
    int m_bExternal;
};

#endif
