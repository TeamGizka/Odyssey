#ifndef __NWSFORCEDACTION_H__
#define __NWSFORCEDACTION_H__

#include "exobase.h"
#include "math3d.h"

class CResGFF;
class CResStruct;

class CSWSForcedAction
{
public:
    CSWSForcedAction();
    ~CSWSForcedAction();

    BOOL SaveForcedAction(CResGFF* pRes, CResStruct* pStruct);

    Vector GetTargetLocation() { return m_vTargetLocation; }

    unsigned long m_nActionId;
    unsigned short m_nGroupId;
    unsigned long m_oidTargetArea;

    Vector m_vTargetLocation;
    unsigned long m_oidTargetObject;
};

#endif
