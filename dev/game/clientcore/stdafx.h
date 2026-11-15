#pragma once

#include "math3d.h"
#include "exobase.h"
#include "exoresman.h"

#include "nwfeat.h"
#include "aurbehavior.h"
#include "nwmessage.h"
#include "nwspell.h"
#include "exoinput.h"
#include "nwambientsound.h"

class CAurObject;
class CFeatUseListEntry;
class CSWCMapPin;
class CSWGuiButton;
class CSWGuiControl;
class CSWGuiFloatyText;
class CSWGuiInGameItemEntry;
class CSWGuiKeyMapButton;
class CSWSItem;
struct CSWCPlaceableObjectLight;
struct CSWGuiControlEvent;
struct CSWGuiSkillStruct;

template CExoArrayList<CSWGuiControl*>::CExoArrayList(int);
template CExoArrayList<CResRef>::CExoArrayList(int);
template CExoArrayList<CSWGuiControl*>::~CExoArrayList();
template CExoArrayList<CSWGuiKeyMapButton*>::~CExoArrayList();
template CExoLinkedList<CAurObject>::~CExoLinkedList();
template unsigned long& CExoArrayList<unsigned long>::operator[](int);
template void CExoArrayList<unsigned short>::AddUnique(unsigned short);
template void CExoArrayList<unsigned short>::DelIndex(int);
template void CExoArrayList<CSWSItem*>::Add(CSWSItem*);
template void CExoArrayList<unsigned long>::DelIndex(int);
template void CExoArrayList<CFeatUseListEntry*>::DelIndex(int);
template void CExoArrayList<CSWGuiKeyMapButton*>::DelIndex(int);
template int CExoArrayList<unsigned long>::IndexOf(unsigned long) const;
template int CExoArrayList<CSWGuiFloatyText*>::IndexOf(CSWGuiFloatyText*) const;
template void CExoArrayList<CSWCMapPin*>::Allocate(int);
template void CExoArrayList<CSWGuiControl*>::Allocate(int);
template void CExoArrayList<CSWCPlaceableObjectLight*>::Allocate(int);
template CExoArrayList<unsigned long>::CExoArrayList(int);
template CExoArrayList<unsigned long>::~CExoArrayList();
template void CExoArrayList<CFeatUseListEntry*>::Add(CFeatUseListEntry*);
template CExoLinkedList<CSWGuiInGameItemEntry>::CExoLinkedList();
template CExoArrayList<CSWGuiControlEvent>::~CExoArrayList();
template CExoArrayList<CSWGuiSkillStruct>::~CExoArrayList();
template CExoArrayList<CSWGuiButton*>::CExoArrayList(int);
template CExoArrayList<CSWSItem*>::~CExoArrayList();
