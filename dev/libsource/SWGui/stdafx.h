#pragma once

#include "swgui.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"

class CSWGuiControl;

typedef CExoArrayList<CSWGuiControl*> SWGuiPchArraySWGuiControlPtr;

inline CSWGuiObject* SWGuiPchObject_Construct() { return new CSWGuiObject; }
inline void SWGuiPchTextParams_Destroy(CSWGuiTextParams* p) { p->~CSWGuiTextParams(); }
inline SWGuiPchArraySWGuiControlPtr* SWGuiPchArraySWGuiControlPtr_Construct(int a0) { return new SWGuiPchArraySWGuiControlPtr(a0); }
inline void SWGuiPchArraySWGuiControlPtr_Add() { void (SWGuiPchArraySWGuiControlPtr::*p)(class CSWGuiControl *) = &SWGuiPchArraySWGuiControlPtr::Add; (void)p; }
inline void SWGuiPchArraySWGuiControlPtr_IndexOf() { int (SWGuiPchArraySWGuiControlPtr::*p)(class CSWGuiControl *) const = &SWGuiPchArraySWGuiControlPtr::IndexOf; (void)p; }
inline void SWGuiPchArraySWGuiControlPtr_Allocate() { void (SWGuiPchArraySWGuiControlPtr::*p)(int) = &SWGuiPchArraySWGuiControlPtr::Allocate; (void)p; }

class CAurObject;
typedef CExoArrayList<CAurObject*> SWGuiPchArrayAurObjectPtr;
inline SWGuiPchArrayAurObjectPtr* SWGuiPchArrayAurObjectPtr_Construct(int a0) { return new SWGuiPchArrayAurObjectPtr(a0); }
inline void SWGuiPchArrayAurObjectPtr_Insert() { void (SWGuiPchArrayAurObjectPtr::*p)(CAurObject*, int) = &SWGuiPchArrayAurObjectPtr::Insert; (void)p; }

typedef CExoLinkedList<CRes> SWGuiPchListRes;
inline void SWGuiPchListRes_Walk() { CRes* (SWGuiPchListRes::*p)(CExoLinkedListNode*) = &SWGuiPchListRes::GetAtPos; CRes* (SWGuiPchListRes::*q)(CExoLinkedListNode*&) = &SWGuiPchListRes::GetNext; (void)p; (void)q; }
