#pragma once

#include "math3d.h"
#include "exobase.h"
#include "exoresman.h"

#include "nwfeat.h"
#include "aurbehavior.h"
#include "nwmessage.h"

class CAurObject;
class CFeatUseListEntry;
class CSWCMapPin;
class CSWCVisualEffectOnObject;
class CSWGuiControl;
class CSWGuiFloatyText;
class CSWGuiKeyMapButton;
struct CSWCPlaceableObjectLight;

typedef CExoArrayList<unsigned long> CCPchArrayULong;
typedef CExoArrayList<CSWCPlaceableObjectLight*> CCPchArraySWCPlaceableObjectLightPtr;
typedef CExoArrayList<CSWCMapPin*> CCPchArraySWCMapPinPtr;
typedef CExoArrayList<CSWGuiControl*> CCPchArraySWGuiControlPtr;
typedef CExoArrayList<CResRef> CCPchArrayResRef;
typedef CExoLinkedList<CAurObject> CCPchLinkedAurObject;
typedef CExoArrayList<unsigned short> CCPchArrayUShort;
typedef CExoArrayList<CSWGuiKeyMapButton*> CCPchArraySWGuiKeyMapButtonPtr;
typedef CExoArrayList<CFeatUseListEntry*> CCPchArrayFeatUseListEntryPtr;
typedef CExoArrayList<CSWGuiFloatyText*> CCPchArraySWGuiFloatyTextPtr;

inline CCPchArraySWGuiControlPtr* CCPchArraySWGuiControlPtr_Construct(int a0) { return new CCPchArraySWGuiControlPtr(a0); }
inline CCPchArrayResRef* CCPchArrayResRef_Construct(int a0) { return new CCPchArrayResRef(a0); }
inline void CCPchArraySWGuiControlPtr_Destroy(CCPchArraySWGuiControlPtr* p) { p->~CCPchArraySWGuiControlPtr(); }
inline void CCPchArraySWGuiKeyMapButtonPtr_Destroy(CCPchArraySWGuiKeyMapButtonPtr* p) { p->~CCPchArraySWGuiKeyMapButtonPtr(); }
inline void CCPchLinkedAurObject_Destroy(CCPchLinkedAurObject* p) { p->~CCPchLinkedAurObject(); }
inline void CCPchArrayULong_Index() { unsigned long & (CCPchArrayULong::*p)(int) = &CCPchArrayULong::operator[]; (void)p; }
inline void CCPchArraySWCPlaceableObjectLightPtr_Index() { struct CSWCPlaceableObjectLight * & (CCPchArraySWCPlaceableObjectLightPtr::*p)(int) = &CCPchArraySWCPlaceableObjectLightPtr::operator[]; (void)p; }
inline void CCPchArraySWCMapPinPtr_Index() { class CSWCMapPin * & (CCPchArraySWCMapPinPtr::*p)(int) = &CCPchArraySWCMapPinPtr::operator[]; (void)p; }
inline void CCPchArrayUShort_AddUnique() { void (CCPchArrayUShort::*p)(unsigned short) = &CCPchArrayUShort::AddUnique; (void)p; }
inline void CCPchArrayUShort_DelIndex() { void (CCPchArrayUShort::*p)(int) = &CCPchArrayUShort::DelIndex; (void)p; }
inline void CCPchArrayULong_DelIndex() { void (CCPchArrayULong::*p)(int) = &CCPchArrayULong::DelIndex; (void)p; }
inline void CCPchArrayFeatUseListEntryPtr_DelIndex() { void (CCPchArrayFeatUseListEntryPtr::*p)(int) = &CCPchArrayFeatUseListEntryPtr::DelIndex; (void)p; }
inline void CCPchArraySWGuiKeyMapButtonPtr_DelIndex() { void (CCPchArraySWGuiKeyMapButtonPtr::*p)(int) = &CCPchArraySWGuiKeyMapButtonPtr::DelIndex; (void)p; }

typedef CExoArrayList<long> CCPchArrayLong;
inline void CCPchArrayLong_Refs() { void (CCPchArrayLong::*p)(int) = &CCPchArrayLong::Allocate; long & (CCPchArrayLong::*q)(int) = &CCPchArrayLong::operator[]; const long & (CCPchArrayLong::*r)(int) const = &CCPchArrayLong::operator[]; (void)p; (void)q; (void)r; }

inline void CCPchSWMessage_Copy(CSWMessage* p) { CSWMessage c(*p); }
inline void CCPchArrayULong_IndexOf() { int (CCPchArrayULong::*p)(unsigned long) const = &CCPchArrayULong::IndexOf; (void)p; }
inline void CCPchArraySWGuiFloatyTextPtr_IndexOf() { int (CCPchArraySWGuiFloatyTextPtr::*p)(class CSWGuiFloatyText *) const = &CCPchArraySWGuiFloatyTextPtr::IndexOf; (void)p; }
inline void CCPchArraySWCMapPinPtr_Allocate() { void (CCPchArraySWCMapPinPtr::*p)(int) = &CCPchArraySWCMapPinPtr::Allocate; (void)p; }
inline void CCPchArraySWGuiControlPtr_Allocate() { void (CCPchArraySWGuiControlPtr::*p)(int) = &CCPchArraySWGuiControlPtr::Allocate; (void)p; }
inline void CCPchArraySWCPlaceableObjectLightPtr_Allocate() { void (CCPchArraySWCPlaceableObjectLightPtr::*p)(int) = &CCPchArraySWCPlaceableObjectLightPtr::Allocate; (void)p; }
inline void CCPchArrayUShort_Allocate() { void (CCPchArrayUShort::*p)(int) = &CCPchArrayUShort::Allocate; (void)p; }

inline CCPchArrayULong* CCPchArrayULong_ConstructArray(int n) { return new CCPchArrayULong[n]; }

inline void CCPchArrayFeatUseListEntryPtr_Add(CCPchArrayFeatUseListEntryPtr* p, CFeatUseListEntry* a) { p->Add(a); }

class CSWGuiInGameItemEntry;
typedef CExoLinkedList<CSWGuiInGameItemEntry> CCPchLinkedSWGuiInGameItemEntry;
inline CCPchLinkedSWGuiInGameItemEntry* CCPchLinkedSWGuiInGameItemEntry_Construct() { return new CCPchLinkedSWGuiInGameItemEntry(); }

struct CSWGuiControlEvent;
struct CSWGuiSkillStruct;
typedef CExoArrayList<CSWGuiControlEvent> CCPchArraySWGuiControlEvent;
typedef CExoArrayList<CSWGuiSkillStruct> CCPchArraySWGuiSkillStruct;
inline void CCPchArraySWGuiControlEvent_Destroy(CCPchArraySWGuiControlEvent* p) { p->~CCPchArraySWGuiControlEvent(); }
inline void CCPchArraySWGuiSkillStruct_Destroy(CCPchArraySWGuiSkillStruct* p) { p->~CCPchArraySWGuiSkillStruct(); }

struct CCPchPanelSlots
{
    virtual void Slot00();
    virtual void Slot04();
    virtual void Slot08();
    virtual void Slot0C();
    virtual void Slot10();
    virtual void Slot14();
    virtual void Slot18();
    virtual void Slot1C();
    virtual void Slot20();
    virtual void Slot24();
    virtual void Slot28();
    virtual void Slot2C();
    virtual void Slot30();
    virtual void Slot34();
    virtual void Slot38();
    virtual void Slot3C();
    virtual void Slot40();
    virtual void Slot44();
    virtual void Slot48();
    virtual void Slot4C();
    virtual void Slot50();
    virtual void Slot54();
    virtual void Slot58();
    virtual void Slot5C();
    virtual void Slot60();
};
inline void CCPchPanelSlots_Thunks()
{
    void (CCPchPanelSlots::*pHandler)() = &CCPchPanelSlots::Slot5C;
    pHandler = &CCPchPanelSlots::Slot60;
    (void)pHandler;
}

class CSWGuiButton;
typedef CExoArrayList<CSWGuiButton*> CCPchArraySWGuiButtonPtr;
inline CCPchArraySWGuiButtonPtr* CCPchArraySWGuiButtonPtr_Construct(int a0) { return new CCPchArraySWGuiButtonPtr(a0); }
