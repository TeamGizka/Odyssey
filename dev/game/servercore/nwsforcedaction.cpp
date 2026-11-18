#include "exobase.h"
#include "resgff.h"
#include "nwsforcedaction.h"
#include "gameobjectarray.h"
// PC 0x00540B30 GOG 0x00540C50 Xbox 0x00263970
CSWSForcedAction::CSWSForcedAction()
{
    m_nActionId = 0;
    m_nGroupId = 0;
    m_oidTargetArea = OBJECT_INVALID;
    m_oidTargetObject = OBJECT_INVALID;
}
// PC 0x004015A0 GOG 0x004015A0 Xbox 0x001A2B40
CSWSForcedAction::~CSWSForcedAction()
{
}
// PC 0x00540B50 GOG 0x00540C70 Xbox 0x002639A0
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

// Dead code: not in the PC executable
BOOL CSWSForcedAction::LoadForcedAction(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;
    m_nActionId = pRes->ReadFieldDWORD(pStruct, "ActionId", bSuccess, 0);
    m_nGroupId = pRes->ReadFieldWORD(pStruct, "GroupId", bSuccess, 0);
    m_oidTargetArea = pRes->ReadFieldDWORD(pStruct, "Target_Area", bSuccess, 0);
    m_oidTargetObject = pRes->ReadFieldDWORD(pStruct, "Target_Object", bSuccess, 0);
    m_vTargetLocation.m_fX = pRes->ReadFieldFLOAT(pStruct, "Target_LocX", bSuccess, 0.0f);
    m_vTargetLocation.m_fY = pRes->ReadFieldFLOAT(pStruct, "Target_LocY", bSuccess, 0.0f);
    m_vTargetLocation.m_fZ = pRes->ReadFieldFLOAT(pStruct, "Target_LocZ", bSuccess, 0.0f);
    return TRUE;
}
