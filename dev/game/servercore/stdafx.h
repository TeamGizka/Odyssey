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
template int CExoLinkedList<CSWSObjectActionNode>::IsEmpty();
template CSWSObjectActionNode* CExoLinkedList<CSWSObjectActionNode>::RemoveHead();
template CExoLinkedListNode* CExoLinkedList<CSWSObjectActionNode>::AddHead(CSWSObjectActionNode*);
class CSWSClient;
template CExoLinkedListNode* CExoLinkedList<CSWSClient>::GetHeadPos();
template CSWSClient* CExoLinkedList<CSWSClient>::GetAtPos(CExoLinkedListNode*);
template CSWSClient* CExoLinkedList<CSWSClient>::GetNext(CExoLinkedListNode*&);

