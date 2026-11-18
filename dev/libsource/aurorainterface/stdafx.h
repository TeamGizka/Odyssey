#pragma once

#include "exobase.h"

class CAuroraModel;

template CExoLinkedListNode* CExoLinkedList<CAuroraModel>::GetHeadPos();
template CAuroraModel* CExoLinkedList<CAuroraModel>::GetHead();
template CAuroraModel* CExoLinkedList<CAuroraModel>::GetNext(CExoLinkedListNode*&);
template CAuroraModel* CExoLinkedList<CAuroraModel>::Remove(CExoLinkedListNode*);
