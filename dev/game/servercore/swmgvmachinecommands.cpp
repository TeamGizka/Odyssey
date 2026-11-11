#include "appmanager.h"
#include "clientexoapp.h"
#include "nwscript.h"
#include "nwvmachinecommands.h"
#include "swminigame.h"

#include "swminigame.h"
#include "nwcanimbase.h"
// PC 0x005CB350  Xbox 0x00274770
int CSWVirtualMachineCommands::ExecuteCommandAurPostString(int nCommand,
                                                           int nArgs)
{
    int nX;
    int nY;
    float fLife;
    CExoString sString;

    if (g_pVirtualMachine->StackPopString(&sString) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nX) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nY) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&fLife) == 0)
    {
        return -2001;
    }
    sString.CStr();
    return 0;
}
// PC 0x005CB440
int CSWTrackFollower::GetIsInvulnerable()
{
    return m_fInvincibilityRemaining > 0.0f;
}
// PC 0x005CB460
void CSWMGGunBank::SetBankModel(const CResRef& sBank)
{
    m_resBank = sBank;
}
// PC 0x005CB480  Xbox 0x00274860
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastEvent(int nCommand,
                                                               int nArgs)
{
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;

    pApp = g_pAppManager->m_pClientExoApp;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        m_oidObject, 0);

    CExoString sEvent;
    if (pObject != NULL)
    {
        sEvent = pObject->m_sName;
    }
    if (g_pVirtualMachine->StackPushString(sEvent.CStr()) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005CB560  Xbox 0x00274940
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastEventModelName(
    int nCommand, int nArgs)
{
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;

    pApp = g_pAppManager->m_pClientExoApp;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        m_oidObject, 0);

    CExoString sModelName;
    if (pObject != NULL && pObject->m_pModel.m_pObject != NULL)
    {

        sModelName = static_cast<Gob*>(pObject->m_pModel.m_pObject)->GetModelName();
    }
    if (g_pVirtualMachine->StackPushString(sModelName.CStr()) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005CB650  Xbox 0x00274A30
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetObjectByName(int nCommand,
                                                                  int nArgs)
{
    CExoString sName;
    unsigned long nObjectId;
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;

    nObjectId = 0xFF;
    if (g_pVirtualMachine->StackPopString(&sName) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pObject =
        (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObjectByName(sName);
    if (pObject != NULL)
    {
        nObjectId = pObject->m_nObjectId;
    }
    if (g_pVirtualMachine->StackPushObject(nObjectId) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005CB730  Xbox 0x00274B10
int CSWVirtualMachineCommands::ExecuteCommandSWMG_PlayAnimation(int nCommand,
                                                                int nArgs)
{
    int nFlags = 0;
    unsigned long nObjectId;
    int bLooping;
    int bOverlay;
    int bQueue;
    CExoString sAnimation;
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopString(&sAnimation) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&bLooping) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&bQueue) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&bOverlay) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        nObjectId, 0);
    if (pObject != NULL)
    {
        if (bLooping == 0)
        {
            nFlags |= 1;
        }
        if (bOverlay != 0)
        {
            nFlags |= 8;
        }
        if (bQueue != 0)
        {
            nFlags |= 0x20;
        }
        pObject->PlayAnimation((char*)sAnimation.CStr(), 1.0f, nFlags);
    }
    return 0;
}
// PC 0x005CB870  Xbox 0x00274C50
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastBulletInfo(
    int nCommand, int nArgs)
{
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;
    CSWTrackFollower* pFollower;
    int nValue;

    pApp = g_pAppManager->m_pClientExoApp;
    nValue = -1;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        m_oidObject, 0);
    if (pObject != NULL)
    {
        switch (nCommand)
        {
        case 587:
            nValue = pObject->m_bullet.m_nDamage;
            break;
        case 588:
            nValue = pObject->m_bullet.m_nTargetType;
            break;
        case 595:
            if (pObject->AsFollower() != 0)
            {
                pFollower = (CSWTrackFollower*)pObject->AsFollower();
                nValue = pFollower->m_lastBulletFired.m_nDamage;
            }
            break;
        case 596:
            if (pObject->AsFollower() != 0)
            {
                pFollower = (CSWTrackFollower*)pObject->AsFollower();
                nValue = pFollower->m_lastBulletFired.m_nTargetType;
            }
            break;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nValue) != 0 ? 0 : -2000;
}
// PC 0x005CB930  Xbox 0x00274D10
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastBulletShooter(
    int nCommand, int nArgs)
{
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;
    CSWMiniGameObject* pShooter;
    unsigned long nObjectId;

    pApp = g_pAppManager->m_pClientExoApp;
    nObjectId = 0xFF;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        m_oidObject, 0);
    if (pObject != NULL)
    {
        pShooter = (CSWMiniGameObject*)pObject->GetLastBulletHitShooter();
        if (pShooter != NULL)
        {
            nObjectId = pShooter->m_nObjectId;
        }
    }
    return g_pVirtualMachine->StackPushObject(nObjectId) != 0 ? 0 : -2000;
}
// PC 0x005CB990  Xbox 0x00274D70
int CSWVirtualMachineCommands::ExecuteCommandSWMG_AdjustFollowerHitPoints(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    int nHitPoints;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nHitPoints) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        switch (nCommand)
        {
        case 590:
            pFollower->AdjustHitPoints(nHitPoints);
            break;
        case 604:
            pFollower->SetHitPoints(nHitPoints);
            break;
        }
    }
    return 0;
}
// PC 0x005CBA20  Xbox 0x00274E00
int CSWVirtualMachineCommands::ExecuteCommandSWMG_DefaultImplementations(
    int nCommand, int nArgs)
{
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;

    pApp = g_pAppManager->m_pClientExoApp;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        m_oidObject, 0);
    if (pObject == NULL)
    {
        return 0;
    }
    switch (nCommand)
    {
    case 591:
        pObject->OnHitBullet(1);
        break;
    case 592:
        if (pObject->AsFollower() != 0)
        {
            ((CSWTrackFollower*)pObject->AsFollower())->OnHitObstacle(1);
        }
        break;
    case 598:
        if (pObject->AsFollower() != 0)
        {
            ((CSWTrackFollower*)pObject->AsFollower())->OnDeath(1);
        }
        break;
    case 605:
        if (pObject->AsFollower() != 0)
        {
            ((CSWTrackFollower*)pObject->AsFollower())->OnDamage(1);
        }
        break;
    }
    return 0;
}
// PC 0x005CBB00  Xbox 0x00274EE0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastObjectHit(int nCommand,
                                                                   int nArgs)
{
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;
    CSWMiniGameObject* pHit;
    CSWTrackFollower* pFollower;
    unsigned long nObjectId;

    pApp = g_pAppManager->m_pClientExoApp;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        m_oidObject, 0);
    nObjectId = 0xFF;
    if (pObject != NULL)
    {
        switch (nCommand)
        {
        case 593:
            pHit = (CSWMiniGameObject*)pObject->GetLastFollowerHit();
            if (pHit != NULL)
            {
                nObjectId = pHit->m_nObjectId;
            }
            break;
        case 594:
            pFollower = (CSWTrackFollower*)pObject->AsFollower();
            if (pFollower != NULL)
            {
                pHit = (CSWMiniGameObject*)pFollower->GetLastObstacleHit();
                if (pHit != NULL)
                {
                    nObjectId = pHit->m_nObjectId;
                }
            }
            break;
        }
    }
    return g_pVirtualMachine->StackPushObject(nObjectId) != 0 ? 0 : -2000;
}
// PC 0x005CBB80  Xbox 0x00274F60
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetObjectName(int nCommand,
                                                                int nArgs)
{
    CExoString sName;
    unsigned long nObjectId;
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        nObjectId, 0);
    if (pObject != NULL)
    {
        sName = pObject->GetName();
    }
    if (g_pVirtualMachine->StackPushString(sName) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005CBC80  Xbox 0x00275060
int CSWVirtualMachineCommands::ExecuteCommandSWMG_IsSomething(int nCommand,
                                                              int nArgs)
{
    unsigned long nObjectId;
    int bResult;
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;
    CSWTrackFollower* pFollower;

    bResult = 0;
    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        nObjectId, 0);
    if (pObject != NULL)
    {
        switch (nCommand)
        {
        case 599:
            if (pObject->AsFollower() != 0)
            {
                bResult = 1;
            }
            break;
        case 600:
            if (pObject->AsPlayer() != 0)
            {
                bResult = 1;
            }
            break;
        case 601:
            if (pObject->AsEnemy() != 0)
            {
                bResult = 1;
            }
            break;
        case 603:
            if (pObject->AsObstacle() != 0)
            {
                bResult = 1;
            }
        case 602:
            if (pObject->AsEnemy() != 0)
            {
                CSWMiniEnemy* pEnemy = (CSWMiniEnemy*)pObject->AsEnemy();
                if (pEnemy->m_bTrigger != 0)
                {
                    bResult = 1;
                }
            }
            break;
        }
    }
    return g_pVirtualMachine->StackPushInteger(bResult) != 0 ? 0 : -2000;
}
// PC 0x005CBD60
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastHPChange(int nCommand,
                                                                  int nArgs)
{
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;
    int nChange;

    pApp = g_pAppManager->m_pClientExoApp;
    nChange = 0;
    pFollower = (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(
        m_oidObject, 4);
    if (pFollower != NULL)
    {
        nChange = pFollower->m_nDamage;
    }
    return g_pVirtualMachine->StackPushInteger(nChange) != 0 ? 0 : -2000;
}
// PC 0x005CBDB0  Xbox 0x00275190
int CSWVirtualMachineCommands::ExecuteCommandSWMG_RemoveAnimation(int nCommand,
                                                                  int nArgs)
{
    CExoString sAnimation;
    unsigned long nObjectId;
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopString(&sAnimation) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        nObjectId, 0);
    if (pObject != NULL)
    {
        pObject->PlayAnimation((char*)sAnimation.CStr(), 1.0f, 128);
    }
    return 0;
}
// PC 0x005CBE90  Xbox 0x00275270
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetCameraClipping(
    int nCommand, int nArgs)
{
    CClientExoApp* pApp;
    CSWMiniGame* pMiniGame;
    float fClip;

    fClip = 0.0f;
    pApp = g_pAppManager->m_pClientExoApp;
    pMiniGame = pApp->GetMiniGame();
    if (pMiniGame != NULL)
    {
        switch (nCommand)
        {
        case 608:
            pMiniGame->GetClipDist(&fClip, NULL);
            break;
        case 609:
            pMiniGame->GetClipDist(NULL, &fClip);
            break;
        }
    }
    return g_pVirtualMachine->StackPushFloat(fClip) != 0 ? 0 : -2000;
}
// PC 0x005CBF00  Xbox 0x002752D0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetCameraClipping(
    int nCommand, int nArgs)
{
    CClientExoApp* pApp;
    CSWMiniGame* pMiniGame;
    float fNear;
    float fFar;

    pApp = g_pAppManager->m_pClientExoApp;
    pMiniGame = pApp->GetMiniGame();
    if (g_pVirtualMachine->StackPopFloat(&fNear) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&fFar) == 0)
    {
        return -2001;
    }
    if (pMiniGame != NULL)
    {
        pMiniGame->SetClipDist(fNear, fFar);
    }
    return 0;
}
// PC 0x005CBF70  Xbox 0x00275340
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayer(int nCommand,
                                                            int nArgs)
{
    CSWMiniGame* pMiniGame;
    unsigned long oidPlayer;
    CSWMiniPlayer* pPlayer;

    pMiniGame = g_pAppManager->m_pClientExoApp->GetMiniGame();
    oidPlayer = 0xFF;
    if (pMiniGame != NULL)
    {
        pPlayer = pMiniGame->m_pPlayer;
        if (pPlayer != NULL)
        {
            oidPlayer = pPlayer->m_nObjectId;
        }
    }
    return g_pVirtualMachine->StackPushObject(oidPlayer) != 0 ? 0 : -2000;
}
// PC 0x005CBFB0  Xbox 0x00275380
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetMiniGameCounts(
    int nCommand, int nArgs)
{
    CClientExoApp* pApp;
    CSWMiniGame* pMiniGame;
    int nCount;

    pApp = g_pAppManager->m_pClientExoApp;
    pMiniGame = pApp->GetMiniGame();
    nCount = 0;
    if (pMiniGame != NULL)
    {
        switch (nCommand)
        {
        case 612:
            nCount = pMiniGame->m_lstEnemyIds.num;
            break;
        case 614:
            nCount = pMiniGame->m_lstObstacleIds.num;
            break;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nCount) != 0 ? 0 : -2000;
}
// PC 0x005CC000  Xbox 0x002753D0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetMiniGameObjects(
    int nCommand, int nArgs)
{
    CClientExoApp* pApp;
    CSWMiniGame* pMiniGame;
    CSWMiniGameObject* pObject;
    unsigned long nObjectId;
    int nEntry;

    pApp = g_pAppManager->m_pClientExoApp;
    pMiniGame = pApp->GetMiniGame();
    nObjectId = 0xFF;
    if (g_pVirtualMachine->StackPopInteger(&nEntry) == 0)
    {
        return -2001;
    }
    if (pMiniGame != NULL)
    {
        switch (nCommand)
        {
        case 613:
            pObject = (CSWMiniGameObject*)pMiniGame->GetEnemy(nEntry);
            break;
        case 615:
            pObject = (CSWMiniGameObject*)pMiniGame->GetObstacle(nEntry);
            break;
        }
        if (pObject != NULL)
        {
            nObjectId = pObject->m_nObjectId;
        }
    }
    return g_pVirtualMachine->StackPushObject(nObjectId) != 0 ? 0 : -2000;
}
// PC 0x005CC0A0  Xbox 0x00275470
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerIntInfo(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    int nValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;

    nValue = 0;
    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        switch (nCommand)
        {
        case 616:
            nValue = pFollower->m_nHitPoints;
            break;
        case 617:
            nValue = pFollower->m_nMaxHPs;
            break;
        case 621:
            nValue = pFollower->m_nNumLoops;
            break;
        case 624:
            nValue = pFollower->m_lstGunBanks.num;
            break;
        case 665:
            nValue = pFollower->GetIsInvulnerable();
            break;
        }
    }
    return g_pVirtualMachine->StackPushInteger(nValue) != 0 ? 0 : -2000;
}
// PC 0x005CC190  Xbox 0x00275560
int CSWVirtualMachineCommands::ExecuteCommandSWMG_StartInvulnerability(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        pFollower->m_fInvincibilityRemaining = pFollower->m_fInvincePeriod;
    }
    return 0;
}
// PC 0x005CC1E0  Xbox 0x002755B0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerFloatInfo(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    float fValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;

    fValue = 0.0f;
    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        switch (nCommand)
        {
        case 619:
            fValue = pFollower->m_fSphereRadius;
            break;
        case 642:
            fValue = pFollower->m_fInvincePeriod;
            break;
        }
    }
    return g_pVirtualMachine->StackPushFloat(fValue) != 0 ? 0 : -2000;
}
// PC 0x005CC280  Xbox 0x00275650
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerPosition(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    Vector vPosition;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        vPosition = pFollower->GetPosition();
    }
    return g_pVirtualMachine->StackPushVector(vPosition) != 0 ? 0 : -2000;
}
// PC 0x005CC330  Xbox 0x00275700
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetFollowerIntInfo(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    int nValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        switch (nCommand)
        {
        case 618:
            pFollower->SetMaxHitPoints(nValue);
            break;
        case 622:
            pFollower->SetLooping(nValue);
            break;
        }
    }
    return 0;
}
// PC 0x005CC3C0  Xbox 0x00275790
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetFollowerFloatInfo(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    float fValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&fValue) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        switch (nCommand)
        {
        case 620:
            pFollower->SetSphereRadius(fValue);
            break;
        case 648:
            pFollower->SetInvincibility(fValue);
            break;
        }
    }
    return 0;
}
// PC 0x005CC450  Xbox 0x00275820
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankStringInfo(
    int nCommand, int nArgs)
{
    CExoString sModel;
    unsigned long nObjectId;
    int nGunBank;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;
    CSWMGTargettingGunBank* pGunBank;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nGunBank) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        pGunBank = pFollower->GetGunBank(nGunBank);
        if (pGunBank != NULL)
        {
            switch (nCommand)
            {
            case 625:
                pGunBank->m_resBank.CopyToString(sModel);
                break;
            case 626:
                pGunBank->m_resGun.CopyToString(sModel);
                break;
            }
        }
    }
    if (g_pVirtualMachine->StackPushString(sModel) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005CC580  Xbox 0x00275940
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankIntInfo(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    int nGunBank;
    int nValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;
    CSWMGTargettingGunBank* pGunBank;

    nValue = 0;
    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nGunBank) == 0)
        {
            return -2001;
        }
        pApp = g_pAppManager->m_pClientExoApp;
        pFollower = (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(
            nObjectId, 4);
        if (pFollower != NULL)
        {
            pGunBank = pFollower->GetGunBank(nGunBank);
            if (pGunBank != NULL)
            {
                switch (nCommand)
                {
                case 627:
                    nValue = pGunBank->m_bullet.m_nDamage;
                    break;
                case 631:
                    nValue = pGunBank->m_bullet.m_nTargetType;
                    break;
                case 640:
                    nValue = pGunBank->IsTargetting();
                    break;
                }
            }
        }
        return g_pVirtualMachine->StackPushInteger(nValue) != 0 ? 0 : -2000;
    }
}
// PC 0x005CC640  Xbox 0x00275A00
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankFloatInfo(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    int nGunBank;
    float fValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;
    CSWMGTargettingGunBank* pGunBank;

    fValue = 0.0f;
    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nGunBank) == 0)
        {
            return -2001;
        }
        pApp = g_pAppManager->m_pClientExoApp;
        pFollower = (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(
            nObjectId, 4);
        if (pFollower != NULL)
        {
            pGunBank = pFollower->GetGunBank(nGunBank);
            if (pGunBank != NULL)
            {
                switch (nCommand)
                {
                case 628:
                    fValue = pGunBank->m_bullet.m_fRateOfFire;
                    break;
                case 629:
                    fValue = pGunBank->m_bullet.m_fLifeSpan;
                    break;
                case 630:
                    fValue = pGunBank->m_bullet.m_fSpeed;
                    break;
                }
            }
        }
        return g_pVirtualMachine->StackPushFloat(fValue) != 0 ? 0 : -2000;
    }
}
// PC 0x005CC700  Xbox 0x00275AC0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetTargettingGunBankFloatInfo(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    int nGunBank;
    float fValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;
    CSWMGTargettingGunBank* pGunBank;

    fValue = 0.0f;
    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nGunBank) == 0)
        {
            return -2001;
        }
        pApp = g_pAppManager->m_pClientExoApp;
        pFollower = (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(
            nObjectId, 4);
        if (pFollower != NULL)
        {
            pGunBank = pFollower->GetGunBank(nGunBank);
            if (pGunBank != NULL && pGunBank->IsTargetting() != 0)
            {
                switch (nCommand)
                {
                case 661:
                    fValue = pGunBank->m_params.m_fHorizontalSpread;
                    break;
                case 662:
                    fValue = pGunBank->m_params.m_fVerticalSpread;
                    break;
                case 663:
                    fValue = pGunBank->m_params.m_fSensingRadius;
                    break;
                case 664:
                    fValue = pGunBank->m_params.m_fInaccuracy;
                    break;
                }
            }
        }
        return g_pVirtualMachine->StackPushFloat(fValue) != 0 ? 0 : -2000;
    }
}
// PC 0x005CC7F0  Xbox 0x00275BB0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetGunBankStringInfo(
    int nCommand, int nArgs)
{
    CExoString sModel;
    unsigned long nObjectId;
    int nGunBank;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;
    CSWMGTargettingGunBank* pGunBank;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nGunBank) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopString(&sModel) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower = (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(
        nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        pGunBank = pFollower->GetGunBank(nGunBank);
        if (pGunBank != NULL)
        {
            switch (nCommand)
            {
            case 632:
            {
                CResRef resBank(sModel);
                pGunBank->SetBankModel(resBank);
                break;
            }
            case 633:
                CResRef resGun(sModel);
                pGunBank->SetGunModel(resGun);
                break;
            }
        }
    }
    return 0;
}
// PC 0x005CC920  Xbox 0x00275CE0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetGunBankIntInfo(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    int nGunBank;
    int nValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;
    CSWMGTargettingGunBank* pGunBank;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nGunBank) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        pGunBank = pFollower->GetGunBank(nGunBank);
        if (pGunBank != NULL)
        {
            switch (nCommand)
            {
            case 634:
                pGunBank->m_bullet.m_nDamage = nValue;
                break;
            case 638:
                pGunBank->SetBulletTargetType(nValue);
                break;
            }
        }
    }
    return 0;
}
// PC 0x005CC9D0  Xbox 0x00275D90
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetGunBankFloatInfo(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    int nGunBank;
    float fValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;
    CSWMGTargettingGunBank* pGunBank;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nGunBank) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&fValue) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        pGunBank = pFollower->GetGunBank(nGunBank);
        if (pGunBank != NULL)
        {
            switch (nCommand)
            {
            case 635:
                pGunBank->SetRateOfFire(fValue);
                break;
            case 636:
                pGunBank->SetBulletLifeSpan(fValue);
                break;
            case 637:
                pGunBank->SetBulletSpeed(fValue);
                break;
            }
        }
    }
    return 0;
}
// PC 0x005CCAA0  Xbox 0x00275E60
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetTargettingGunBankFloatInfo(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    int nGunBank;
    float fValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;
    CSWMGTargettingGunBank* pGunBank;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nGunBank) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopFloat(&fValue) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower == NULL)
    {
        return 0;
    }
    pGunBank = pFollower->GetGunBank(nGunBank);
    if (pGunBank == NULL || pGunBank->IsTargetting() == 0)
    {
        return 0;
    }
    switch (nCommand)
    {
    case 661:
        pGunBank->SetHorizontalSpread(fValue);
        break;
    case 662:
        pGunBank->SetVerticalSpread(fValue);
        break;
    case 663:
        pGunBank->SetSensingRadius(fValue);
        break;
    case 664:
        pGunBank->SetInaccuracy(fValue);
        break;
    }
    return 0;
}
// PC 0x005CCBB0  Xbox 0x00275F70
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastBulletHitPart(
    int nCommand, int nArgs)
{
    CExoString sPart;
    CClientExoApp* pApp;
    CSWMiniGameObject* pObject;

    pApp = g_pAppManager->m_pClientExoApp;
    pObject = (CSWMiniGameObject*)pApp->GetMGOArray()->GetMiniGameObject(
        m_oidObject, 0);
    if (pObject != NULL)
    {
        sPart = pObject->m_sLastBulletHitPart;
    }
    if (g_pVirtualMachine->StackPushString(sPart) == 0)
    {
        return -2000;
    }
    return 0;
}
// PC 0x005CCC60  Xbox 0x00276020
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerFloatInfo(
    int nCommand, int nArgs)
{
    float fValue;
    CSWMiniPlayer* pPlayer;

    fValue = 0.0f;
    pPlayer = g_pAppManager->m_pClientExoApp->GetMiniGame()->m_pPlayer;
    if (pPlayer != NULL)
    {
        switch (nCommand)
        {
        case 643:
            fValue = pPlayer->m_fTrackSpeed;
            break;
        case 644:
            fValue = pPlayer->m_fMinimumSpeed;
            break;
        case 645:
            fValue = pPlayer->m_fAPS;
            break;
        case 667:
            fValue = pPlayer->m_fMaximumSpeed;
            break;
        }
    }
    return g_pVirtualMachine->StackPushFloat(fValue) != 0 ? 0 : -2000;
}
// PC 0x005CCD20  Xbox 0x002760E0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerVectorInfo(
    int nCommand, int nArgs)
{
    Vector vValue;
    CSWMiniPlayer* pPlayer;

    pPlayer = g_pAppManager->m_pClientExoApp->GetMiniGame()->m_pPlayer;
    if (pPlayer != NULL)
    {
        switch (nCommand)
        {
        case 641:
            vValue = pPlayer->m_vOffset;
            break;
        case 646:
            vValue = pPlayer->m_vTunnelMinimum;
            break;
        case 653:
            vValue = pPlayer->m_vTunnelMaximum;
            break;
        case 655:
            vValue = pPlayer->m_vOrigin;
            break;
        case 717:
            vValue = pPlayer->m_vTunnelWrap;
            break;
        }
    }
    return g_pVirtualMachine->StackPushVector(vValue) != 0 ? 0 : -2000;
}
// PC 0x005CCE70  Xbox 0x00276220
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerFloatInfo(
    int nCommand, int nArgs)
{
    float fValue;
    CClientExoApp* pApp;
    CSWMiniPlayer* pPlayer;

    if (g_pVirtualMachine->StackPopFloat(&fValue) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pPlayer = pApp->GetMiniGame()->m_pPlayer;
    if (pPlayer != NULL)
    {
        switch (nCommand)
        {
        case 649:
            pPlayer->SetSpeed(fValue);
            break;
        case 650:
            pPlayer->SetMinSpeed(fValue);
            break;
        case 651:
            pPlayer->SetAPS(fValue);
            break;
        case 668:
            pPlayer->SetMaxSpeed(fValue);
            break;
        }
    }
    return 0;
}
// PC 0x005CCF30  Xbox 0x002762E0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerVectorInfo(
    int nCommand, int nArgs)
{
    Vector vValue;
    CSWMiniPlayer* pPlayer;

    if (g_pVirtualMachine->StackPopVector(&vValue) == 0)
    {
        return -2001;
    }
    pPlayer = g_pAppManager->m_pClientExoApp->GetMiniGame()->m_pPlayer;
    if (pPlayer != NULL)
    {
        switch (nCommand)
        {
        case 647:
            pPlayer->SetOffset(vValue);
            break;
        case 652:
            pPlayer->m_vTunnelMinimum = vValue;
            break;
        case 654:
            pPlayer->m_vTunnelMaximum = vValue;
            break;
        case 656:
            pPlayer->SetOrigin(vValue);
        case 718:
            pPlayer->m_vTunnelWrap = vValue;
            break;
        }
    }
    return 0;
}
// PC 0x005CD0A0  Xbox 0x00276430
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetSoundProperty(int nCommand,
                                                                   int nArgs)
{
    unsigned long nObjectId;
    int nSound;
    int nValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;

    nValue = 0;
    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    else
    {
        if (g_pVirtualMachine->StackPopInteger(&nSound) == 0)
        {
            return -2001;
        }
        pApp = g_pAppManager->m_pClientExoApp;
        pFollower = (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(
            nObjectId, 4);
        if (pFollower != NULL)
        {
            switch (nCommand)
            {
            case 683:
                nValue = pFollower->GetSoundVolume(nSound);
                break;
            case 685:
                nValue = pFollower->GetSoundFrequencyIsRandom(nSound);
                break;
            case 687:
                nValue = pFollower->GetSoundFrequency(nSound);
                break;
            }
        }
        return g_pVirtualMachine->StackPushInteger(nValue) != 0 ? 0 : -2000;
    }
}
// PC 0x005CD160  Xbox 0x002764F0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetSoundProperty(int nCommand,
                                                                   int nArgs)
{
    unsigned long nObjectId;
    int nSound;
    int nValue;
    CClientExoApp* pApp;
    CSWTrackFollower* pFollower;

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nSound) == 0)
    {
        return -2001;
    }
    if (g_pVirtualMachine->StackPopInteger(&nValue) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pFollower =
        (CSWTrackFollower*)pApp->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        switch (nCommand)
        {
        case 684:
            pFollower->SetSoundFrequency(nSound, nValue);
            break;
        case 686:
            pFollower->SetSoundFrequencyIsRandom(nSound, nValue);
            break;
        case 688:
            pFollower->SetSoundVolume(nSound, nValue);
            break;
        }
    }
    return 0;
}
// PC 0x005CD230  Xbox 0x002765C0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetLateralAccelPerSec(
    int nCommand, int nArgs)
{
    float fValue;
    float fAccel;
    CClientExoApp* pApp;

    if (g_pVirtualMachine->StackPopFloat(&fValue) == 0)
    {
        return -2001;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    fAccel = fValue;
    pApp->GetMiniGame()->m_fLateralAccel = fAccel;
    return 0;
}
// PC 0x005CD280  Xbox 0x00276610
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLateralAccelPerSec(
    int nCommand, int nArgs)
{
    CClientExoApp* pApp;
    float fAccel;

    pApp = g_pAppManager->m_pClientExoApp;
    fAccel = pApp->GetMiniGame()->m_fLateralAccel;
    return g_pVirtualMachine->StackPushFloat(fAccel) != 0 ? 0 : -2000;
}
// PC 0x005CD2C0
void CSWVirtualMachineCommands::InitializeSWMGCommands()
{
    m_pCommands[582] = &CSWVirtualMachineCommands::ExecuteCommandAurPostString;
    m_pCommands[583] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastEvent;
    m_pCommands[584] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastEventModelName;
    m_pCommands[585] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetObjectByName;
    m_pCommands[586] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_PlayAnimation;
    m_pCommands[587] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastBulletInfo;
    m_pCommands[588] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastBulletInfo;
    m_pCommands[589] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastBulletShooter;
    m_pCommands[590] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_AdjustFollowerHitPoints;
    m_pCommands[591] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_DefaultImplementations;
    m_pCommands[592] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_DefaultImplementations;
    m_pCommands[593] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastObjectHit;
    m_pCommands[594] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastObjectHit;
    m_pCommands[595] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastBulletInfo;
    m_pCommands[596] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastBulletInfo;
    m_pCommands[597] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetObjectName;
    m_pCommands[598] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_DefaultImplementations;
    m_pCommands[599] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_IsSomething;
    m_pCommands[600] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_IsSomething;
    m_pCommands[601] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_IsSomething;
    m_pCommands[602] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_IsSomething;
    m_pCommands[603] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_IsSomething;
    m_pCommands[604] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_AdjustFollowerHitPoints;
    m_pCommands[605] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_DefaultImplementations;
    m_pCommands[606] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastHPChange;
    m_pCommands[607] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_RemoveAnimation;
    m_pCommands[608] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetCameraClipping;
    m_pCommands[609] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetCameraClipping;
    m_pCommands[610] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetCameraClipping;
    m_pCommands[611] = &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayer;
    m_pCommands[612] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetMiniGameCounts;
    m_pCommands[613] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetMiniGameObjects;
    m_pCommands[614] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetMiniGameCounts;
    m_pCommands[615] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetMiniGameObjects;
    m_pCommands[616] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerIntInfo;
    m_pCommands[617] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerIntInfo;
    m_pCommands[618] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetFollowerIntInfo;
    m_pCommands[619] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerFloatInfo;
    m_pCommands[620] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetFollowerFloatInfo;
    m_pCommands[621] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerIntInfo;
    m_pCommands[622] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetFollowerIntInfo;
    m_pCommands[623] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerPosition;
    m_pCommands[624] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerIntInfo;
    m_pCommands[625] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankStringInfo;
    m_pCommands[626] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankStringInfo;
    m_pCommands[627] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankIntInfo;
    m_pCommands[628] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankFloatInfo;
    m_pCommands[629] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankFloatInfo;
    m_pCommands[630] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankFloatInfo;
    m_pCommands[631] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankIntInfo;
    m_pCommands[632] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetGunBankStringInfo;
    m_pCommands[633] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetGunBankStringInfo;
    m_pCommands[634] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetGunBankIntInfo;
    m_pCommands[635] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetGunBankFloatInfo;
    m_pCommands[636] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetGunBankFloatInfo;
    m_pCommands[637] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetGunBankFloatInfo;
    m_pCommands[638] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetGunBankIntInfo;
    m_pCommands[639] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLastBulletHitPart;
    m_pCommands[640] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetGunBankIntInfo;
    m_pCommands[641] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerVectorInfo;
    m_pCommands[642] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerFloatInfo;
    m_pCommands[643] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerFloatInfo;
    m_pCommands[644] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerFloatInfo;
    m_pCommands[667] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerFloatInfo;
    m_pCommands[645] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerFloatInfo;
    m_pCommands[646] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerVectorInfo;
    m_pCommands[647] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerVectorInfo;
    m_pCommands[648] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetFollowerFloatInfo;
    m_pCommands[649] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerFloatInfo;
    m_pCommands[650] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerFloatInfo;
    m_pCommands[668] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerFloatInfo;
    m_pCommands[651] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerFloatInfo;
    m_pCommands[652] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerVectorInfo;
    m_pCommands[653] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerVectorInfo;
    m_pCommands[654] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerVectorInfo;
    m_pCommands[655] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerVectorInfo;
    m_pCommands[656] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerVectorInfo;
    m_pCommands[657] = &CSWVirtualMachineCommands::
                           ExecuteCommandSWMG_GetTargettingGunBankFloatInfo;
    m_pCommands[658] = &CSWVirtualMachineCommands::
                           ExecuteCommandSWMG_GetTargettingGunBankFloatInfo;
    m_pCommands[659] = &CSWVirtualMachineCommands::
                           ExecuteCommandSWMG_GetTargettingGunBankFloatInfo;
    m_pCommands[660] = &CSWVirtualMachineCommands::
                           ExecuteCommandSWMG_GetTargettingGunBankFloatInfo;
    m_pCommands[661] = &CSWVirtualMachineCommands::
                           ExecuteCommandSWMG_SetTargettingGunBankFloatInfo;
    m_pCommands[662] = &CSWVirtualMachineCommands::
                           ExecuteCommandSWMG_SetTargettingGunBankFloatInfo;
    m_pCommands[663] = &CSWVirtualMachineCommands::
                           ExecuteCommandSWMG_SetTargettingGunBankFloatInfo;
    m_pCommands[664] = &CSWVirtualMachineCommands::
                           ExecuteCommandSWMG_SetTargettingGunBankFloatInfo;
    m_pCommands[665] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerIntInfo;
    m_pCommands[666] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_StartInvulnerability;
    m_pCommands[683] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetSoundProperty;
    m_pCommands[684] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetSoundProperty;
    m_pCommands[685] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetSoundProperty;
    m_pCommands[686] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetSoundProperty;
    m_pCommands[687] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetSoundProperty;
    m_pCommands[688] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetSoundProperty;
    m_pCommands[718] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetPlayerVectorInfo;
    m_pCommands[717] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetPlayerVectorInfo;
    m_pCommands[521] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLateralAccelPerSec;
    m_pCommands[520] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetLateralAccelPerSec;
}
