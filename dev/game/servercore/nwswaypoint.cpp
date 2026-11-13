#include "exobase.h"
#include "exoresman.h"
#include "resgff.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "nwsarea.h"
#include "nwsmodule.h"
#include "serveraimaster.h"
#include "nwswaypoint.h"
// PC 0x0056F3B0 GOG 0x0056F4E0
CSWSWaypoint::CSWSWaypoint(unsigned long oidWaypoint)
    : CSWSObject(CGameObject::OBJECT_TYPE_WAYPOINT, oidWaypoint, 0)
{
    m_nAnimation = ANIMATION_COMBAT_READY;
    m_bHasMapNote = 0;
    m_bMapNoteEnabled = 0;
}
// PC 0x004015A0 GOG 0x004015A0
void CSWSWaypoint::AIUpdate()
{
}
// PC 0x0056F450 GOG 0x0056F580
void CSWSWaypoint::EventHandler(unsigned long nEventId, unsigned long oidCaller,
                                void* pEventData, unsigned long nCalendarDay,
                                unsigned long nTimeOfDay)
{
    if (nEventId == CServerAIMaster::EVENT_DESTROY_OBJECT)
    {
        if (m_nEC != 0)
        {
            delete this;
        }
    }
}
// PC 0x0056F470 GOG 0x0056F5A0
int CSWSWaypoint::LoadWaypoint(CResGFF* pRes, CResStruct* pStruct)
{
    Vector vPosition;
    Vector vOrientation;
    CExoLocString lsMapNote;
    int bSuccess;
    int bLocalizedNameSuccess;
    int bHasMapNote;
    int bMapNoteEnabled;

    {
        CExoString sDefault("");
        SetTag(pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess, sDefault));
    }
    m_lsLocalizedName = pRes->ReadFieldCExoLocString(
        pStruct, "LocalizedName", bLocalizedNameSuccess, CExoLocString());

    vPosition.m_fX = pRes->ReadFieldFLOAT(pStruct, "XPosition", bSuccess, 0.0f);
    vPosition.m_fY = pRes->ReadFieldFLOAT(pStruct, "YPosition", bSuccess, 0.0f);
    vPosition.m_fZ = pRes->ReadFieldFLOAT(pStruct, "ZPosition", bSuccess, 0.0f);
    vOrientation.m_fX =
        pRes->ReadFieldFLOAT(pStruct, "XOrientation", bSuccess, 0.0f);
    vOrientation.m_fY =
        pRes->ReadFieldFLOAT(pStruct, "YOrientation", bSuccess, 0.0f);
    vOrientation.m_fZ =
        pRes->ReadFieldFLOAT(pStruct, "ZOrientation", bSuccess, 0.0f);
    if (vOrientation.m_fX * vOrientation.m_fX +
            vOrientation.m_fY * vOrientation.m_fY +
            vOrientation.m_fZ * vOrientation.m_fZ !=
        1.0f)
    {
        vOrientation.Normalize();
    }

    SetPosition(vPosition, 1, 1, 0);
    SetOrientation(vOrientation);

    bHasMapNote = pRes->ReadFieldBYTE(pStruct, "HasMapNote", bSuccess, 0);
    if (bSuccess != 0 && bHasMapNote != 0)
    {
        bMapNoteEnabled =
            pRes->ReadFieldBYTE(pStruct, "MapNoteEnabled", bSuccess, 0);
        lsMapNote = pRes->ReadFieldCExoLocString(pStruct, "MapNote", bSuccess,
                                                 CExoLocString());
        if (bSuccess != 0)
        {
            m_bHasMapNote = bHasMapNote;
            m_bMapNoteEnabled = bMapNoteEnabled;
            m_lsMapNote = lsMapNote;
        }
    }

    g_pAppManager->m_pServerExoApp->GetModule()->AddObjectToLookupTable(
        m_sTag, m_nId);
    return 1;
}
// PC 0x0056F770 GOG 0x0056F8A0
int CSWSWaypoint::SaveWaypoint(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldCExoString(pStruct, GetTag(), "Tag");
    pRes->WriteFieldCExoLocString(pStruct, m_lsLocalizedName, "LocalizedName");
    Vector vPosition = m_vPosition;
    pRes->WriteFieldFLOAT(pStruct, vPosition.m_fX, "XPosition");
    pRes->WriteFieldFLOAT(pStruct, vPosition.m_fY, "YPosition");
    pRes->WriteFieldFLOAT(pStruct, vPosition.m_fZ, "ZPosition");
    Vector vOrientation = m_vOrientation;
    pRes->WriteFieldFLOAT(pStruct, vOrientation.m_fX, "XOrientation");
    pRes->WriteFieldFLOAT(pStruct, vOrientation.m_fY, "YOrientation");
    pRes->WriteFieldFLOAT(pStruct, vOrientation.m_fZ, "ZOrientation");
    pRes->WriteFieldBYTE(pStruct, (unsigned char)m_bHasMapNote, "HasMapNote");
    if (m_bHasMapNote != 0)
    {
        pRes->WriteFieldBYTE(pStruct, (unsigned char)m_bMapNoteEnabled,
                             "MapNoteEnabled");
        pRes->WriteFieldCExoLocString(pStruct, m_lsMapNote, "MapNote");
    }
    return 1;
}
// PC 0x0056F890 GOG 0x0056F9C0
void CSWSWaypoint::RemoveFromArea()
{
    if (GetArea() != NULL)
    {
        GetArea()->RemoveObjectFromArea(m_nId);
    }
    SetArea(NULL);
    Vector vPosition;
    SetPosition(vPosition, 1, 1, 0);
}
// PC 0x0056F8F0 GOG 0x0056FA20
int CSWSWaypoint::LoadFromTemplate(const CResRef& cResRef)
{
    CResGFF* pRes = new CResGFF(2058, "UTW ", cResRef);

    if (pRes->m_bLoaded == 0)
    {
        delete pRes;
        CExoString sMessage;
        CExoString sResRef;
        cResRef.CopyToString(sResRef);
        sMessage.Format("Waypoint template %s doesn't exist.\n",
                        sResRef.CStr());
        return 0;
    }

    CResStruct stTop;
    pRes->GetTopLevelStruct(&stTop);
    LoadWaypoint(pRes, &stTop);
    delete pRes;
    return 1;
}
// PC 0x0056FA20 GOG 0x0056FB50
CSWSWaypoint::~CSWSWaypoint()
{
    if (GetArea() != NULL)
    {
        RemoveFromArea();
    }
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        unsigned long oid = m_nId;
        pModule->RemoveObjectFromLookupTable(m_sTag, oid);
    }
}
// PC 0x0056FAC0 GOG 0x0056FBF0
void CSWSWaypoint::AddToArea(CSWSArea* pArea, float fX, float fY, float fZ,
                             int bLoadingGame)
{
    RemoveFromArea();

    if (pArea != NULL)
    {
        CExoString sTag;
        SetArea(pArea);
        Vector vPosition(fX, fY, fZ);
        SetPosition(vPosition, 0, 1, 0);
        GetArea()->AddObjectToArea(m_nId, bLoadingGame);
    }
}
