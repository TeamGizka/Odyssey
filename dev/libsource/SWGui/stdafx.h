#pragma once

#include "swgui.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"

class CAurObject;
class CSWGuiControl;

template CExoArrayList<CSWGuiControl*>::CExoArrayList(int);
template void CExoArrayList<CSWGuiControl*>::Add(CSWGuiControl*);
template int CExoArrayList<CSWGuiControl*>::IndexOf(CSWGuiControl*) const;
template void CExoArrayList<CSWGuiControl*>::Allocate(int);
template CExoArrayList<CAurObject*>::CExoArrayList(int);
template void CExoArrayList<CAurObject*>::Insert(CAurObject*, int);
template CRes* CExoLinkedList<CRes>::GetAtPos(CExoLinkedListNode*);
template CRes* CExoLinkedList<CRes>::GetNext(CExoLinkedListNode*&);
