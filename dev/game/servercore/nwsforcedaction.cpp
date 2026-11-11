#include "exobase.h"
#include "resgff.h"
#include "nwsforcedaction.h"
#include "gameobjectarray.h"
// PC 0x00540B30
CSWSForcedAction::CSWSForcedAction()
{
    m_nActionId = 0;
    m_nGroupId = 0;
    m_oidTargetArea = OBJECT_INVALID;
    m_oidTargetObject = OBJECT_INVALID;
}
// PC 0x004015A0
CSWSForcedAction::~CSWSForcedAction()
{
}
// PC 0x00540B50
BOOL CSWSForcedAction::SaveForcedAction(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldDWORD(pStruct, m_nActionId, "ActionId");
    pRes->WriteFieldWORD(pStruct, m_nGroupId, "GroupId");
    pRes->WriteFieldDWORD(pStruct, m_oidTargetArea, "Target_Area");
    pRes->WriteFieldDWORD(pStruct, m_oidTargetObject, "Target_Object");
    pRes->WriteFieldFLOAT(pStruct, m_vTargetLocation.m_fX, "Target_LocX");
    pRes->WriteFieldFLOAT(pStruct, m_vTargetLocation.m_fY, "Target_LocY");
    pRes->WriteFieldFLOAT(pStruct, m_vTargetLocation.m_fZ, "Target_LocZ");
    return TRUE;
}
