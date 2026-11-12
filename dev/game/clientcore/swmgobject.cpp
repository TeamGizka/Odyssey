#include "swminigame.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "exobase.h"
#include "nwscript.h"
#include "resgff.h"
#include "aurmodel.h"
#include "swminigame.h"

#include <string.h>
// PC 0x006C1080
CSWMiniGameObjectArray::CSWMiniGameObjectArray()
{
    for (int i = 0; i < 255; i++)
    {
        m_apObjects[i] = 0;
    }
    m_nCurrentObject = 0;
}
// PC 0x006C10A0
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
// PC 0x006C1120
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
// PC 0x006C11E0
inline unsigned long CSWMiniGameObjectArray::FindNextEmptyIndex()
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
// PC 0x006C1220
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
// PC 0x006C12B0
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
// PC 0x006C12E0
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
// PC 0x006C1310
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
// PC 0x006C1450
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
// PC 0x006C14F0
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
// PC 0x006C15A0
void CSWMiniGameObject::LoadScripts(CResGFF* pGFF, CResStruct* pStruct,
                                    CResStruct& structScripts)
{
    int bSuccess;

    {
        CResRef resDefault("");
        SetScriptName(0, pGFF->ReadFieldCResRef(&structScripts, "OnCreate",
                                                bSuccess, resDefault));
    }
    {
        CResRef resDefault("");
        SetScriptName(1, pGFF->ReadFieldCResRef(&structScripts, "OnHitBullet",
                                                bSuccess, resDefault));
    }
    {
        CResRef resDefault("");
        SetScriptName(2, pGFF->ReadFieldCResRef(&structScripts, "OnHitFollower",
                                                bSuccess, resDefault));
    }
    {
        CResRef resDefault("");
        SetScriptName(3, pGFF->ReadFieldCResRef(&structScripts, "OnAnimEvent",
                                                bSuccess, resDefault));
    }
    {
        CResRef resDefault("");
        SetScriptName(4, pGFF->ReadFieldCResRef(&structScripts, "OnHeartbeat",
                                                bSuccess, resDefault));
    }
}
// PC 0x006C16C0
CSWMiniGameObject::CSWMiniGameObject(unsigned long nType)
{
    CSWMiniGameObjectArray* pArray =
        g_pAppManager->m_pClientExoApp->GetMGOArray();
    m_nObjectId = pArray->FindNextEmptyIndex();
    g_pAppManager->m_pClientExoApp->GetMGOArray()->m_apObjects[m_nObjectId] = this;
    m_nType = nType;
    m_bCreated = 0;
    m_pModel = 0;
    m_nLastBulletHitShooter = 255;
    m_nLastFollowerHit = 255;
}
// PC 0x006C1790
CSWMiniGameObject::~CSWMiniGameObject()
{
    g_pAppManager->m_pClientExoApp->GetMGOArray()->m_apObjects[m_nObjectId] = 0;
}
