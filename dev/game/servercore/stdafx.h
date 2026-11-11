#pragma once

#include "math3d.h"
#include "exobase.h"
#include "exoresman.h"

typedef CExoArrayList<unsigned long> SCPchArrayULong;
typedef CExoArrayList<CResRef> SCPchArrayResRef;
typedef CExoArrayList<unsigned short> SCPchArrayUShort;

inline SCPchArrayResRef* SCPchArrayResRef_Construct(int a0) { return new SCPchArrayResRef(a0); }
inline void SCPchArrayULong_Destroy(SCPchArrayULong* p) { p->~SCPchArrayULong(); }
inline void SCPchArrayULong_Index() { unsigned long & (SCPchArrayULong::*p)(int) = &SCPchArrayULong::operator[]; (void)p; }
inline void SCPchArrayULong_Contains() { int (SCPchArrayULong::*p)(unsigned long) const = &SCPchArrayULong::Contains; (void)p; }
inline void SCPchArrayULong_Allocate() { void (SCPchArrayULong::*p)(int) = &SCPchArrayULong::Allocate; (void)p; }

typedef CExoArrayList<CExoString> SCPchArrayString;
inline void SCPchArrayString_Index() { const CExoString & (SCPchArrayString::*p)(int) const = &SCPchArrayString::operator[]; (void)p; }

