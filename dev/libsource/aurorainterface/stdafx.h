#pragma once

#include "exobase.h"

class CAuroraModel;
typedef CExoLinkedList<CAuroraModel> AurIfPchListAuroraModel;

inline void AurIfPchListAuroraModel_GetHeadPos() { CExoLinkedListNode* (AurIfPchListAuroraModel::*p)() = &AurIfPchListAuroraModel::GetHeadPos; (void)p; }
inline void AurIfPchListAuroraModel_GetHead() { CAuroraModel* (AurIfPchListAuroraModel::*p)() = &AurIfPchListAuroraModel::GetHead; (void)p; }
inline void AurIfPchListAuroraModel_GetNext() { CAuroraModel* (AurIfPchListAuroraModel::*p)(CExoLinkedListNode*&) = &AurIfPchListAuroraModel::GetNext; (void)p; }
inline void AurIfPchListAuroraModel_Remove() { CAuroraModel* (AurIfPchListAuroraModel::*p)(CExoLinkedListNode*) = &AurIfPchListAuroraModel::Remove; (void)p; }
