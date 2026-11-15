#pragma once

#include "math3d.h"
#include "exobase.h"
#include "exoresman.h"

template CExoArrayList<CResRef>::CExoArrayList(int);
template CExoArrayList<unsigned long>::~CExoArrayList();
template unsigned long& CExoArrayList<unsigned long>::operator[](int);
template int CExoArrayList<unsigned long>::Contains(unsigned long) const;
template void CExoArrayList<unsigned long>::Allocate(int);
template const CExoString& CExoArrayList<CExoString>::operator[](int) const;

