#include "appmanager.h"
#include "clientexoapp.h"
#include "nwscript.h"
#include "nwvmachinecommands.h"
#include "swminigame.h"

#include "swminigame.h"
#include "nwcanimbase.h"
#include "serverexoapp.h"
#include "nwsobject.h"
#include "nwscreaturestats.h"
// PC 0x006176F0
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
    return 0;
}
// PC 0x006176B0
int CSWTrackFollower::GetIsInvulnerable()
{
    return m_fInvincibilityRemaining > 0.0f;
}
// PC 0x006176D0
void CSWMGGunBank::SetBankModel(const CResRef& sBank)
{
    m_resBank = sBank;
}
// PC 0x006177C0
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
// PC 0x00619380
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetSwoopUpgrade(int nCommand, int nArgs)
{
    int nSlot;
    int nUpgrade = 0;

    if (g_pVirtualMachine->StackPopInteger(&nSlot) != 0)
    {
        int nIndex = nSlot;
        nUpgrade = g_pAppManager->GetClientExoApp()->GetMiniGame()->m_anSwoopUpgrades[nIndex];
    }
    return g_pVirtualMachine->StackPushInteger(nUpgrade) != 0 ? 0 : -2000;
}
// PC 0x006193D0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetTrackPosition(int nCommand, int nArgs)
{
    unsigned long oidFollower;
    Vector vPosition(0.0f, 0.0f, 0.0f);

    if (g_pVirtualMachine->StackPopObject(&oidFollower) != 0)
    {
        CSWMiniGameObject* pObject = (CSWMiniGameObject*)g_pAppManager->GetClientExoApp()->GetMGOArray()->GetMiniGameObject(oidFollower, SWMG_OBJECT_ANY);
        CSWTrackFollower* pFollower = pObject->AsFollower();
        vPosition = pFollower->m_pTrack->GetPosition();
    }
    return g_pVirtualMachine->StackPushVector(vPosition) != 0 ? 0 : -2000;
}
// PC 0x00619480
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetFollowerPosition(int nCommand, int nArgs)
{
    Vector vPosition(0.0f, 0.0f, 0.0f);

    if (g_pVirtualMachine->StackPopVector(&vPosition) != 0)
    {
        g_pAppManager->GetClientExoApp()->GetMiniGame()->SetCurPos(vPosition);
    }
    return 0;
}
// PC 0x006194F0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_DestroyMiniGameObject(int nCommand, int nArgs)
{
    unsigned long oidObject;

    if (g_pVirtualMachine->StackPopObject(&oidObject) != 0)
    {
        CSWMiniGameObject* pObject = (CSWMiniGameObject*)g_pAppManager->GetClientExoApp()->GetMGOArray()->GetMiniGameObject(oidObject, SWMG_OBJECT_ANY);
        CSWMGObstacle* pObstacle = pObject->AsObstacle();
        if (pObstacle != NULL)
        {
            g_pAppManager->GetClientExoApp()->GetMiniGame()->RemoveObstacle(pObstacle);
        }
    }
    return 0;
}
// PC 0x00619550
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetJumpSpeed(int nCommand, int nArgs)
{
    float fSpeed;

    if (g_pVirtualMachine->StackPopFloat(&fSpeed) != 0)
    {
        g_pAppManager->GetClientExoApp()->GetMiniGame()->m_fJumpSpeed = fSpeed;
    }
    return 0;
}
// PC 0x00619590
int CSWVirtualMachineCommands::ExecuteCommandSWMG_PlayerApplyForce(int nCommand, int nArgs)
{
    Vector vForce(0.0f, 0.0f, 0.0f);

    if (g_pVirtualMachine->StackPopVector(&vForce) != 0)
    {
        CSWMiniGame* pMiniGame = g_pAppManager->GetClientExoApp()->GetMiniGame();
        pMiniGame->m_fJumpSpeed = vForce.m_fZ;
        CSWMGPhysicsState cCurrent = pMiniGame->m_CurrentState;
        CSWMGPhysicsState cNext = pMiniGame->m_NextState;
        cCurrent.m_vValue.m_fX = vForce.m_fX;
        cNext.m_vValue.m_fX = vForce.m_fX;
        pMiniGame->m_NextState = cNext;
        pMiniGame->m_CurrentState = cCurrent;
        pMiniGame->m_fLateralAccel = vForce.m_fX;
    }
    return 0;
}
// PC 0x00619640
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
// PC 0x006178A0
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
// PC 0x00617980
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
        if (sAnimation == "")
        {
            pObject->PlayAnimation(NULL, 0.0f, 2);
        }
        else
        {
            pObject->PlayAnimation((char*)sAnimation.CStr(), 1.0f, nFlags);
        }
    }
    return 0;
}
// PC 0x00617AE0
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
// PC 0x00617BA0
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
// PC 0x00617C00
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
// PC 0x00617C90
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
// PC 0x00617D70
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
// PC 0x00617DF0
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
// PC 0x00617EF0
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
// PC 0x00617FD0
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
// PC 0x00618020
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
// PC 0x00618100
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
// PC 0x00618170
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
// PC 0x006181E0
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
// PC 0x00619730
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
// PC 0x00618220
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
// PC 0x00619780
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
// PC 0x006182C0
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
// PC 0x00618310
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
// PC 0x00619870
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetFollowerPosition(
    int nCommand, int nArgs)
{
    unsigned long nObjectId;
    Vector vPosition(0.0f, 0.0f, 0.0f);

    if (g_pVirtualMachine->StackPopObject(&nObjectId) == 0)
    {
        return -2001;
    }
    CSWTrackFollower* pFollower =
        (CSWTrackFollower*)g_pAppManager->GetClientExoApp()->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_FOLLOWER);
    if (pFollower != NULL)
    {
        vPosition = pFollower->GetPosition();
    }
    else
    {
        CSWMGObstacle* pObstacle =
            (CSWMGObstacle*)g_pAppManager->GetClientExoApp()->GetMGOArray()->GetMiniGameObject(nObjectId, SWMG_OBJECT_OBSTACLE);
        if (pObstacle != NULL)
        {
            vPosition = ((Gob*)(CAurObject*)pObstacle->m_pModel)->GetPosition();
        }
    }
    return g_pVirtualMachine->StackPushVector(vPosition) != 0 ? 0 : -2000;
}
// PC 0x006183B0
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
// PC 0x00618440
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
// PC 0x006184D0
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
// PC 0x00618600
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
// PC 0x006186C0
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
// PC 0x00618780
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
// PC 0x00618870
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
// PC 0x006189A0
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
// PC 0x00618A50
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
// PC 0x00618B20
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
// PC 0x00618C30
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
// PC 0x00618CE0
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
// PC 0x00618DA0
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
// PC 0x00618EF0
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
// PC 0x00618FB0
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
// PC 0x00619120
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
// PC 0x006191E0
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
// PC 0x006192B0
int CSWVirtualMachineCommands::ExecuteCommandSWMG_SetLateralAccelPerSec(
    int nCommand, int nArgs)
{
    float fValue;
    float fAccel;
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServerExoApp->GetCreatureByGameObjectID(pServerExoApp->GetPlayerCreatureId());
    int nDexMod = pCreature->GetStats()->GetDEXMod(0);

    if (g_pVirtualMachine->StackPopFloat(&fValue) == 0)
    {
        return -2001;
    }
    fValue = (nDexMod * 0.3f + 1.0f) * fValue;
    fAccel = fValue;
    g_pAppManager->m_pClientExoApp->GetMiniGame()->m_fLateralAccel = fAccel;
    return 0;
}
// PC 0x00619340
int CSWVirtualMachineCommands::ExecuteCommandSWMG_GetLateralAccelPerSec(
    int nCommand, int nArgs)
{
    CClientExoApp* pApp;
    float fAccel;

    pApp = g_pAppManager->m_pClientExoApp;
    fAccel = pApp->GetMiniGame()->m_fLateralAccel;
    return g_pVirtualMachine->StackPushFloat(fAccel) != 0 ? 0 : -2000;
}
// PC 0x00619960
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
    m_pCommands[COMMAND_SWMG_GET_SWOOP_UPGRADE] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetSwoopUpgrade;
    m_pCommands[COMMAND_SWMG_GET_TRACK_POSITION] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_GetTrackPosition;
    m_pCommands[COMMAND_SWMG_SET_FOLLOWER_POSITION] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetFollowerPosition;
    m_pCommands[COMMAND_SWMG_DESTROY_MINI_GAME_OBJECT] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_DestroyMiniGameObject;
    m_pCommands[COMMAND_SWMG_SET_JUMP_SPEED] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_SetJumpSpeed;
    m_pCommands[COMMAND_SWMG_PLAYER_APPLY_FORCE] =
        &CSWVirtualMachineCommands::ExecuteCommandSWMG_PlayerApplyForce;
}
