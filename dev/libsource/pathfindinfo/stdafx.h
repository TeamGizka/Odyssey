#pragma once

#include "math3d.h"
#include "exobase.h"
#include "exoresman.h"
#include "resare.h"

inline const CResRef& PathfindPchAreResRef(const CResHelper<CResARE, RESTYPE_ARE>* pHelper)
{
    return pHelper->GetResRef();
}
