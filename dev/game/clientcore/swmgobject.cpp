#include "swminigame.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "exobase.h"
#include "nwscript.h"
#include "resgff.h"
#include "aurmodel.h"
#include "swminigame.h"

#include <string.h>
// PC 0x006C1080 GOG 0x006C0E60 Xbox 0x000B4400
CSWMiniGameObjectArray::CSWMiniGameObjectArray()
{
    for (int i = 0; i < 255; i++)
    {
        m_apObjects[i] = 0;
    }
    m_nCurrentObject = 0;
}
// PC 0x006C10A0 GOG 0x006C0E80 Xbox 0x000B4420
void* CSWMiniGameObjectArray::GetMiniGameObject(unsigned long nObjectId,
                                                 unsigned char nType)
{
    CSWMiniGameObject* pObject;
    CSWMiniEnemy* pEnemy;

    if (nObjectId >= 255)
    {
        return 0;
    }

    pObject = m_apObjects[nObjectId];
    if (pObject == 0)
    {
        return 0;
    }

    switch (nType)
    {
    case 0:
        return pObject;
    case 4:
        return pObject->AsFollower();
    case 3:
        return pObject->AsObstacle();
    case 1:
        return pObject->AsPlayer();
    case 2:
        return pObject->AsEnemy();
    case 5:
        pEnemy = (CSWMiniEnemy*)pObject->AsEnemy();
        if (pEnemy == 0 || !pEnemy->m_bTrigger)
        {
            return 0;
        }
        return pEnemy;
    }

    return 0;
}
// PC 0x006C1120 GOG 0x006C0F00 Xbox 0x000B44B0
CSWMiniGameObject* CSWMiniGameObjectArray::GetMiniGameObjectByName(const CExoString& sName)
{
    int nObject;

    for (nObject = 0; nObject < 255; nObject++)
    {
        if (m_apObjects[nObject] != 0 &&
            stricmp(sName.CStr(), m_apObjects[nObject]->GetName().CStr()) == 0)
        {
            return m_apObjects[nObject];
        }
    }
    return 0;
}
// PC 0x006C11E0 GOG 0x006C0FC0 Xbox 0x000B4530
inline unsigned long CSWMiniGameObjectArray::GetNextID()
{
    unsigned long nStart = m_nCurrentObject;
    while (m_apObjects[m_nCurrentObject] != 0)
    {
        ++m_nCurrentObject;
        if (m_nCurrentObject == MAX_OBJECTS)
            m_nCurrentObject = 0;
        if (m_nCurrentObject == nStart)
            return INVALID_OBJECT_ID;
    }
    return m_nCurrentObject;
}
// PC 0x006C1220 GOG 0x006C1000 Xbox 0x000B4570
void CSWMiniGameObject::Update(float fTime)
{
    CResRef resScript = GetScriptName(4);

    if (resScript.IsValid())
    {
        CExoString sScript;
        resScript.CopyToString(sScript);
        unsigned long nObjectId = m_nObjectId;
        g_pVirtualMachine->RunScript(&sScript, nObjectId, 1);
    }
}
// PC 0x006C12B0 GOG 0x006C1090 Xbox 0x000B45E0
void* CSWMiniGameObject::GetLastBulletHitShooter()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    void* pResult;

    pResult = pApp->GetMGOArray()->GetMiniGameObject(
        m_nLastBulletHitShooter, 4);
    if (pResult == 0)
    {
        m_nLastBulletHitShooter = 255;
    }
    return pResult;
}
// PC 0x006C12E0 GOG 0x006C10C0 Xbox 0x000B4610
void* CSWMiniGameObject::GetLastFollowerHit()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    void* pResult;

    pResult = pApp->GetMGOArray()->GetMiniGameObject(m_nLastFollowerHit, SWMG_OBJECT_FOLLOWER);
    if (pResult == 0)
    {
        m_nLastFollowerHit = 255;
    }
    return pResult;
}
// PC 0x006C1310 GOG 0x006C10F0 Xbox 0x000B4640
void CSWMiniGameObject::OnHitBullet(int bScript)
{
    if (bScript == 0)
    {
        CResRef resScript = GetScriptName(1);
        if (resScript.IsValid())
        {
            CExoString sScript;
            resScript.CopyToString(sScript);
            unsigned long nObjectId = m_nObjectId;
            g_pVirtualMachine->RunScript(&sScript, nObjectId, 1);
            return;
        }
    }

    CSWTrackFollower* pFollower = (CSWTrackFollower*)AsFollower();
    if (pFollower != 0 &&
        (pFollower->TargetType() & m_bullet.m_nTargetType) != 0)
    {
        pFollower->AdjustHitPoints(-m_bullet.m_nDamage);
    }

    if (m_bullet.m_resCollisionSound.IsValid())
    {
        unsigned char nGroup = 23;
        if (AsPlayer() == 0)
        {
            nGroup = 24;
        }
        CResRef resSound((char*)m_bullet.m_resCollisionSound.GetResRef());
        g_pExoSound->Play3DOneShotSound(resSound, m_vSoundPosition, 0.0f,
                                        nGroup, 0, 0, 0.0f, 0.0f);
    }
}
// PC 0x006C1450 GOG 0x006C1230 Xbox 0x000B4760
void CSWMiniGameObject::OnHitFollower(int bScript)
{
    if (bScript == 0)
    {
        CResRef resScript = GetScriptName(2);
        if (resScript.IsValid())
        {
            CExoString sScript;
            resScript.CopyToString(sScript);
            unsigned long nObjectId = m_nObjectId;
            g_pVirtualMachine->RunScript(&sScript, nObjectId, 1);
        }
    }
}
// PC 0x006C14F0 GOG 0x006C12D0 Xbox 0x000B47E0
void CSWMiniGameObject::OnCreate()
{
    CResRef resScript;

    if (AsFollower() != 0)
    {
        resScript = GetScriptName(0);
        if (resScript.IsValid())
        {
            CExoString sScript;
            resScript.CopyToString(sScript);
            unsigned long nObjectId = m_nObjectId;
            g_pVirtualMachine->RunScript(&sScript, nObjectId, 1);
        }
    }
}
// PC 0x006C15A0 GOG 0x006C1380 Xbox 0x000B4880
void CSWMiniGameObject::LoadScripts(CResGFF* pGFF, CResStruct* pStruct,
                                    CResStruct& structScripts)
{
    int bSuccess;

    SetScriptName(0, pGFF->ReadFieldCResRef(&structScripts, "OnCreate",
                                            bSuccess, ""));
    SetScriptName(1, pGFF->ReadFieldCResRef(&structScripts, "OnHitBullet",
                                            bSuccess, ""));
    SetScriptName(2, pGFF->ReadFieldCResRef(&structScripts, "OnHitFollower",
                                            bSuccess, ""));
    SetScriptName(3, pGFF->ReadFieldCResRef(&structScripts, "OnAnimEvent",
                                            bSuccess, ""));
    SetScriptName(4, pGFF->ReadFieldCResRef(&structScripts, "OnHeartbeat",
                                            bSuccess, ""));
}
// PC 0x006C16C0 GOG 0x006C14A0 Xbox 0x000B49A0
CSWMiniGameObject::CSWMiniGameObject(unsigned long nType)
{
    CSWMiniGameObjectArray* pArray =
        g_pAppManager->m_pClientExoApp->GetMGOArray();
    m_nObjectId = pArray->GetNextID();
    g_pAppManager->m_pClientExoApp->GetMGOArray()->m_apObjects[m_nObjectId] = this;
    m_nType = nType;
    m_bCreated = 0;
    m_pModel = 0;
    m_nLastBulletHitShooter = 255;
    m_nLastFollowerHit = 255;
}
// PC 0x006C1790 GOG 0x006C1570 Xbox 0x000B4A40
CSWMiniGameObject::~CSWMiniGameObject()
{
    g_pAppManager->m_pClientExoApp->GetMGOArray()->m_apObjects[m_nObjectId] = 0;
}
