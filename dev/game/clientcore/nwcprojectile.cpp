#include "nwcprojectile.h"

#include "nwcanimbase.h"
#include "nwsobject.h"
#include "appmanager.h"
#include "gameobjectarray.h"
#include "clientexoapp.h"
#include "exobase.h"
#include "nwrules.h"
#include "nwspell.h"
#include "worldtimer.h"
#include "clientaimaster.h"
#include "nwcmodule.h"
#include "nwcarea.h"
#include "aurscene.h"
#include "../../libsource/AURORA/aurpointer.h"
#include <math.h>
#include <stdlib.h>
#include "nwccreature.h"
#include "nwscreature.h"
#include "nwcitem.h"
#include "aurcamera.h"
#include "nwitem.h"

#include "nwcplaceable.h"
#include "nwcdoor.h"
#include "twodimarray.h"
#include "twodimarrays.h"

int g_nProjectileUnused = 0;

const int CSWCProjectile::NUM_IMPACT_SOUNDS = 2;
const int CSWCProjectile::NUM_SHOT_SOUNDS = 2;
// PC 0x007409E0
int CSWCProjectile::SetDesiredPositionTimer(int nTime)
{
    m_nDesiredPositionTime = nTime;
    m_nDesiredPositionTimer = nTime;
    return nTime;
}
// PC 0x00740A00
int CSWCProjectile::LoadModel(const CResRef& cModel, unsigned char nModel,
                              unsigned char nFlags)
{
    if (m_pAnimBase == NULL)
    {
        m_pAnimBase = new CSWCAnimBaseProjectile;
        if (m_pAnimBase == NULL)
        {
            return 0;
        }
    }
    if (!m_pAnimBase->LoadModel(cModel, nModel))
    {
        return 0;
    }
    Gob* pModel = (Gob*)m_pAnimBase->GetModel(nModel);
    pModel->SetExternalData(this);
    pModel->SetExternalDataType(CGameObject::OBJECT_TYPE_PROJECTILE);
    m_pAnimBase->TurnOffShadows();
    return 1;
}
// PC 0x00740AE0
void CSWCProjectile::UnloadModel()
{
    if (m_pReferenceObject != NULL)
    {
        m_pReferenceObject->AttachToObject(NULL, NULL, Gob::ATTACH_NORMAL);
        m_pReferenceObject->AttachToScene(NULL);
        delete m_pReferenceObject;
        m_pReferenceObject = NULL;
    }
    if (m_pAnimBase != NULL)
    {
        delete m_pMuzzleFlash;
        m_pMuzzleFlash = NULL;
        m_pAnimBase->UnloadModel();
        delete m_pAnimBase;
        m_pAnimBase = NULL;
    }
}
// PC 0x00740B60
void CSWCProjectile::UnloadModel(unsigned char nModel)
{
    if (m_pReferenceObject != NULL)
    {
        m_pReferenceObject->AttachToObject(NULL, NULL, Gob::ATTACH_NORMAL);
        m_pReferenceObject->AttachToScene(NULL);
        delete m_pReferenceObject;
        m_pReferenceObject = NULL;
    }
    if (m_pAnimBase != NULL)
    {
        m_pAnimBase->UnloadModel(nModel);
    }
}
// PC 0x00740BB0
void CSWCProjectile::MoveProjectileAccelerating()
{
    Vector vDirection;
    Vector vPosition;
    vDirection = m_vDesiredPosition - m_vPosition;
    float fTime = m_nDesiredPositionTime / 1000.0f;
    m_vVelocity = (1.0f / (2.0f * fTime)) *
        (2.0f * vDirection - (fTime * fTime) * m_vAcceleration);
    vPosition = m_vPosition + (m_nTimeDelta / 1000.0f) * m_vVelocity;

    Vector vMovement = m_vPosition - vPosition;
    if (vMovement * vMovement > vDirection * vDirection)
    {
        vPosition = m_vDesiredPosition;
    }

    if (m_nOrientationType == PROJECTILE_ORIENTATION_TARGET)
    {
        vDirection = m_pCurrentBehavior->m_vTargetPosition - vPosition;
    }
    else if (m_nOrientationType == PROJECTILE_ORIENTATION_MOVEMENT)
    {
        vDirection = vPosition - m_vPosition;
    }

    if (m_nOrientationType != PROJECTILE_ORIENTATION_NONE &&
        vDirection * vDirection > PROJECTILE_ORIENTATION_MIN_DIRECTION_SQUARED)
    {
        SetOrientation(normalize(vDirection));
    }
    SetPosition(vPosition);
}
// PC 0x00740E70
void CSWCProjectile::MoveProjectileSpiral()
{
    Vector vDirection;
    Vector vPosition;
    vDirection = m_vDesiredPosition - m_vPosition;
    float fTime = m_nDesiredPositionTime / 1000.0f;
    Vector vSpiral;
    if (m_bSpiralCounterClockwise)
    {
        vSpiral.m_fX = -vDirection.m_fY;
        vSpiral.m_fY = vDirection.m_fX;
    }
    else
    {
        vSpiral.m_fX = vDirection.m_fY;
        vSpiral.m_fY = -vDirection.m_fX;
    }
    m_vVelocity = PROJECTILE_SPIRAL_SPEED * normalize(vSpiral) +
        (1.0f / fTime) * vDirection;
    vPosition = m_vPosition + (m_nTimeDelta / 1000.0f) * m_vVelocity;

    Vector vMovement = m_vPosition - vPosition;
    if (vMovement * vMovement > vDirection * vDirection)
    {
        vPosition = m_vDesiredPosition;
    }

    if (m_nOrientationType == PROJECTILE_ORIENTATION_TARGET)
    {
        vDirection = m_pCurrentBehavior->m_vTargetPosition - vPosition;
    }
    else if (m_nOrientationType == PROJECTILE_ORIENTATION_MOVEMENT)
    {
        vDirection = vPosition - m_vPosition;
    }

    if (m_nOrientationType != PROJECTILE_ORIENTATION_NONE &&
        vDirection * vDirection > PROJECTILE_ORIENTATION_MIN_DIRECTION_SQUARED)
    {
        SetOrientation(normalize(vDirection));
    }
    SetPosition(vPosition);
}
// PC 0x00741170
void CSWCProjectile::SetProjectileVelAndAccel(const Vector& vDirection)
{

    static int bWentLeft = 0;
    float fAngle;
    float fRandomAngle;
    Vector vDelta;
    Vector vRandom;
    Vector vTarget;
    if (m_pCurrentBehavior == NULL)
    {
        return;
    }

    if (m_nDesiredTargetId != OBJECT_INVALID)
    {
        CGameObjectArray* pArray =
            g_pAppManager->GetClientExoApp()->GetGameObjectArray();
        CGameObject* pTarget;
        unsigned long oidTarget = m_nDesiredTargetId;
        if (pArray->GetGameObject(oidTarget, &pTarget) ==
                CGameObjectArray::SUCCESS &&
            pTarget->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
        {
            Vector vPosition;
            Quaternion qOrientation;
            CSWCObject* pObject = (CSWCObject*)pTarget;
            if (pObject->GetModel(255, 1) != NULL)
            {
                if (!pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPartPosition(
                        (char*)m_sImpactHook.CStr(), &vPosition, &qOrientation))
                {
                    vPosition = pObject->m_vPosition;
                    vPosition.m_fZ += 1.25f;
                }
                vTarget = vPosition + m_pCurrentBehavior->m_vStartPosition;
            }
            else
            {
                vTarget = m_vDesiredPosition;
            }
        }
    }
    else
    {
        vTarget = m_pCurrentBehavior->m_vStartPosition;
    }

    vDelta = vTarget - m_vPosition;
    if (vDelta * vDelta <= 0.00001f)
    {
        m_vVelocity = Vector();
        m_vAcceleration = Vector();
        return;
    }

    float fTime = m_nDesiredPositionTime / 1000.0f;
    Vector vOrientation = vDirection;
    CGameObject* pLinkedTarget;
    switch (m_pCurrentBehavior->m_nType)
    {
    case CProjectileBehavior::BEHAVIOR_SPIRAL_APPROACH:
        vOrientation.m_fZ = 0.0f;
        m_vAcceleration = normalize(vOrientation) * 7.5f;
        m_vVelocity = (2.0f / fTime) * vDelta - m_vAcceleration;
        m_vAcceleration.m_fX =
            (m_vAcceleration.m_fX * m_vAcceleration.m_fX -
             m_vVelocity.m_fX * m_vVelocity.m_fX) / (2.0f * vDelta.m_fX);
        m_vAcceleration.m_fY =
            (m_vAcceleration.m_fY * m_vAcceleration.m_fY -
             m_vVelocity.m_fY * m_vVelocity.m_fY) / (2.0f * vDelta.m_fY);
        break;
    case CProjectileBehavior::TYPE_BALLISTIC:
        m_vAcceleration = Vector(0.0f, 0.0f, -9.81f);
        break;
    case CProjectileBehavior::TYPE_BURST_UP:
    {
        vOrientation = normalize(vOrientation);
        fAngle = acos(vOrientation.m_fX);
        if (vOrientation.m_fY < 0.0f)
        {
            fAngle = TWO_PI - fAngle;
        }
        fRandomAngle = (rand() % 900) / 10.0f;
        fRandomAngle *= DEG_TO_RAD;
        if (bWentLeft)
        {
            fRandomAngle = fAngle - fRandomAngle;
        }
        else
        {
            fRandomAngle = fRandomAngle + fAngle;
        }
        vRandom.m_fX = cos(fRandomAngle);
        vRandom.m_fY = sin(fRandomAngle);
        vRandom.m_fZ = (rand() % 100) * 0.01f;
        bWentLeft = !bWentLeft;
        int nRange = abs((int)(log(magnitude(vDelta)) * 5.0)) + 1;
        m_vVelocity = normalize(vRandom) * ((rand() % nRange) + 3.0f);
        m_vAcceleration = (1.0f / (fTime * fTime)) *
            (2.0f * (vDelta - fTime * m_vVelocity));
        break;
    }
    case CProjectileBehavior::TYPE_BURST:
    {
        vOrientation = normalize(vOrientation);
        fAngle = acos(vOrientation.m_fX);
        if (vOrientation.m_fY < 0.0f)
        {
            fAngle = TWO_PI - fAngle;
        }
        fRandomAngle = (rand() % 900) / 10.0f;
        fRandomAngle *= DEG_TO_RAD;
        if (bWentLeft)
        {
            fRandomAngle = fAngle - fRandomAngle;
        }
        else
        {
            fRandomAngle = fRandomAngle + fAngle;
        }
        vRandom.m_fX = cos(fRandomAngle);
        vRandom.m_fY = sin(fRandomAngle);
        if (rand() % 2)
        {
            vRandom.m_fZ = (rand() % 100) * 0.01f;
        }
        else
        {
            vRandom.m_fZ = (rand() % 50) * -0.01f;
        }
        bWentLeft = !bWentLeft;
        int nRange = abs((int)(log(magnitude(vDelta)) * 5.0)) + 1;
        m_vVelocity = normalize(vRandom) * ((rand() % nRange) + 3.0f);
        m_vAcceleration = (1.0f / (fTime * fTime)) *
            (2.0f * (vDelta - fTime * m_vVelocity));
        break;
    }
    case CProjectileBehavior::TYPE_ACCELERATING:
        m_vVelocity = Vector();
        m_vAcceleration = (1.0f / (fTime * fTime)) *
            (2.0f * (vDelta - fTime * m_vVelocity));
        break;
    case CProjectileBehavior::TYPE_HOMING:
    case CProjectileBehavior::BEHAVIOR_SPIRAL:
    case CProjectileBehavior::TYPE_UNACCELERATED:
        m_vAcceleration = Vector();
        break;
    case CProjectileBehavior::TYPE_LINKED:
    {
        m_vVelocity = Vector();
        m_vAcceleration = Vector();
        Gob* pModel = GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
        if (pModel != NULL)
        {
            CGameObjectArray* pArray =
                g_pAppManager->GetClientExoApp()->GetGameObjectArray();
            if (pArray != NULL)
            {
                if (pArray->GetGameObject(m_nDesiredTargetId, &pLinkedTarget) ==
                    CGameObjectArray::SUCCESS)
                {
                    Vector vPosition;
                    Quaternion qOrientation;
                    CSWCObject* pObject = (CSWCObject*)pLinkedTarget;
                    if (pObject->GetModel(255, 1) != NULL)
                    {
                        if (pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->GetPartPosition(
                                (char*)m_sImpactHook.CStr(), &vPosition, &qOrientation))
                        {
                            pModel->ReattachReference(pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1),
                                                      (char*)m_sImpactHook.CStr());
                        }
                        else
                        {
                            pModel->ReattachReference(pObject->GetModel(255, 1), "root");
                        }
                    }
                }
                else
                {
                    m_pReferenceObject = (Gob*)NewCAurObject("ref", "dummytarget", NULL, NULL);
                    if (m_pReferenceObject != NULL)
                    {
                        m_pReferenceObject->SetPosition(m_pCurrentBehavior->m_vStartPosition);
                        pModel->ReattachReference(m_pReferenceObject, "root");
                    }
                }
            }
        }
        break;
    }
    case CProjectileBehavior::TYPE_LINKED_BURST_UP:
    {
        vOrientation = normalize(vOrientation);
        fAngle = acos(vOrientation.m_fX);
        if (vOrientation.m_fY < 0.0f)
        {
            fAngle = TWO_PI - fAngle;
        }
        fRandomAngle = (rand() % 900) / 10.0f;
        fRandomAngle *= DEG_TO_RAD;
        if (bWentLeft)
        {
            fRandomAngle = fAngle - fRandomAngle;
        }
        else
        {
            fRandomAngle = fRandomAngle + fAngle;
        }
        vRandom.m_fX = cos(fRandomAngle);
        vRandom.m_fY = sin(fRandomAngle);
        vRandom.m_fZ = (rand() % 100) * 0.01f;
        bWentLeft = !bWentLeft;
        int nRange = abs((int)(log(magnitude(vDelta)) * 5.0)) + 1;
        m_vVelocity = normalize(vRandom) * ((rand() % nRange) + 3.0f);
        m_vAcceleration = (1.0f / (fTime * fTime)) *
            (2.0f * (vDelta - fTime * m_vVelocity));
        m_pReferenceObject = (Gob*)NewCAurObject("ref", "", NULL, NULL);
        if (m_pReferenceObject != NULL)
        {
            Gob* pModel = GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
            Vector vPosition;
            m_pReferenceObject->SetPosition(vPosition = pModel->GetPosition());
            vPosition.m_fZ += 0.01f;
            pModel->SetPosition(vPosition);
            m_pReferenceObject->SetOrientation(pModel->GetOrientation());
            m_pReferenceObject->AttachToScene(pModel->GetScene());
            pModel->ReattachReference(m_pReferenceObject, "root");
        }
        break;
    }
    default:
        m_vVelocity = Vector();
        m_vAcceleration = Vector();
        break;
    }
}
// PC 0x00741F60
void CSWCProjectile::SetProjectileSpellId(unsigned long nSpellId)
{
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
    if (pSpell != NULL || nSpellId == 11000)
    {
        m_nSpellId = nSpellId;
        if (nSpellId == 11000)
        {
            m_cProjectileSound = "c_cow_dead";
        }
        else
        {
            m_cFlightSound = pSpell->m_resProjectileSound;
            m_cProjectileSound = pSpell->m_resProjectileSound;
            m_bUseModuleOrientation = 0;
            m_nOrientationType = pSpell->m_nProjectileOrientation;
        }
    }
}
// PC 0x00742000
int CSWCProjectile::SetAnimation(unsigned short nAnimation)
{
    int nResult = 0;
    if (m_pAnimBase != NULL &&
        m_pAnimBase->SetAnimation(nAnimation, 1.0f, nResult, 0.0f) != 0)
    {
        m_nCurrentAnimation = nAnimation;
        nResult = 1;
    }
    return nResult;
}
// PC 0x00742050
void CSWCProjectile::SetSpawnCombatVisualOnImpact(unsigned char bSpawn,
                                                  int nSpellId, short nEffect)
{
    m_bSpawnCombatVisualOnImpact = bSpawn;
    m_nCombatVisualSpellId = nSpellId;
    m_nCombatVisualEffect = nEffect;
}
// PC 0x00742080
void CSWCProjectile::SetDesiredTargetObject(unsigned long nTargetId,
                                            const CExoString& sImpactHook)
{
    CSWCObject* pObject =
        g_pAppManager->m_pClientExoApp->GetGameObject(nTargetId);
    CSWCPlaceable* pPlaceable = NULL;
    CSWCDoor* pDoor = NULL;

    if (pObject == NULL)
    {
        m_nDesiredTargetId = CGameObjectArray::INVALIDOBJID;
    }
    else
    {
        pPlaceable = pObject->AsSWCPlaceable();
        pDoor = pObject->AsSWCDoor();
    }

    if (pPlaceable != NULL)
    {
        CExoString sSuffix("_impact");
        m_sImpactHook = pPlaceable->m_sModel + sSuffix;
    }
    else if (pDoor != NULL)
    {
        CExoString sSuffix("impc");
        m_sImpactHook = pDoor->m_sModel + sSuffix;
    }
    else if (sImpactHook == "")
    {
        m_sImpactHook = "impact";
    }
    else
    {
        m_sImpactHook = sImpactHook;
    }

    m_nDesiredTargetId = nTargetId;
}
// PC 0x007421F0
void CSWCProjectile::CreateMuzzleFlash(const CExoString& sModel,
                                       CAurObject* pObject,
                                       const CExoString& sHook)
{
    if (!sModel.IsEmpty() && pObject != NULL)
    {
        m_pMuzzleFlash = (Gob*)NewCAurObject((char*)sModel.CStr(),
                                             "muzzleflash", NULL, NULL);
        m_pMuzzleFlash->AttachToObject(pObject, (char*)sHook.CStr(), Gob::ATTACH_NO_ORIENTATION);
        m_pMuzzleFlash->PlayAnimation("impact", 1.0f, 1, 0.0f);
    }
}
// PC 0x007422D0
void CSWCProjectile::ClearBehaviorList()
{
    while (m_lstBehaviors.GetHeadPos() != NULL)
    {
        CProjectileBehavior* pBehavior = m_lstBehaviors.RemoveHead();
        if (pBehavior != NULL)
        {
            delete pBehavior;
        }
    }
}
// PC 0x00742300
void CSWCProjectile::SetBehaviorListHoming()
{
    Vector vPosition;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else if (g_pAppManager->m_pClientExoApp->GetGameObject(
                 m_nDesiredTargetId) != NULL)
    {
        vPosition = Vector();
    }
    else
    {
        vPosition = m_vDesiredPosition;
    }
    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = 1;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x007423F0
void CSWCProjectile::SetBehaviorListBallistic()
{
    Vector vPosition;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else if (g_pAppManager->m_pClientExoApp->GetGameObject(
                 m_nDesiredTargetId) != NULL)
    {
        vPosition = Vector();
    }
    else
    {
        vPosition = m_vDesiredPosition;
    }
    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = 2;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x007424E0
void CSWCProjectile::SetBehaviorListHighBallistic()
{
    Vector vPosition;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        CSWCObject* pTarget = pApp->GetGameObject(m_nDesiredTargetId);
        if (pTarget != NULL)
        {
            vPosition = Vector();
        }
        else
        {
            vPosition = m_vDesiredPosition;
        }
    }
    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_nType = CProjectileBehavior::TYPE_HOMING;
    pBehavior->m_vStartPosition = m_vPosition;
    pBehavior->m_vStartPosition.m_fZ += CSWCPROJECTILE_HIGH_BALLISTIC_HEIGHT;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer / 2u;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = CGameObjectArray::INVALIDOBJID;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);

    pBehavior = new CProjectileBehavior;
    pBehavior->m_nType = CProjectileBehavior::TYPE_HIGH_BALLISTIC_TRANSITION;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_vStartPosition.m_fZ += CSWCPROJECTILE_HIGH_BALLISTIC_HEIGHT;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = 0;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);

    pBehavior = new CProjectileBehavior;
    pBehavior->m_nType = CProjectileBehavior::TYPE_HOMING;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer - m_nDesiredPositionTimer / 2u;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x007426E0
void CSWCProjectile::SetBehaviorListBurstUp()
{
    Vector vPosition;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else if (g_pAppManager->m_pClientExoApp->GetGameObject(
                 m_nDesiredTargetId) != NULL)
    {
        vPosition = Vector();
    }
    else
    {
        vPosition = m_vDesiredPosition;
    }
    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = 4;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x007427D0
void CSWCProjectile::SetBehaviorListBurst()
{
    Vector vPosition;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else if (g_pAppManager->m_pClientExoApp->GetGameObject(
                 m_nDesiredTargetId) != NULL)
    {
        vPosition = Vector();
    }
    else
    {
        vPosition = m_vDesiredPosition;
    }
    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = 9;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x007428C0
void CSWCProjectile::SetBehaviorListAccelerating()
{
    Vector vPosition;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else if (g_pAppManager->m_pClientExoApp->GetGameObject(
                 m_nDesiredTargetId) != NULL)
    {
        vPosition = Vector();
    }
    else
    {
        vPosition = m_vDesiredPosition;
    }
    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = 5;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x007429B0
void CSWCProjectile::SetBehaviorListSpiral()
{
    Vector vSpiral;
    Vector vDirection = m_vDesiredPosition - m_vPosition;
    m_bSpiralCounterClockwise = rand() % 2;
    if (m_bSpiralCounterClockwise)
    {
        vSpiral.m_fX = -vDirection.m_fY;
        vSpiral.m_fY = vDirection.m_fX;
        vSpiral.m_fZ = 0.0f;
    }
    else
    {
        vSpiral.m_fX = vDirection.m_fY;
        vSpiral.m_fY = -vDirection.m_fX;
        vSpiral.m_fZ = 0.0f;
    }
    if (vSpiral * vSpiral <= PROJECTILE_SPIRAL_MIN_DIRECTION_SQUARED)
    {
        vSpiral = Vector(0.0f, 1.0f, 0.0f);
    }
    vSpiral = PROJECTILE_SPIRAL_RADIUS * normalize(vSpiral);
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vSpiral = vSpiral + m_vDesiredPosition;
        vSpiral.m_fZ = m_vPosition.m_fZ;
    }
    if (m_nDesiredPositionTimer - PROJECTILE_SPIRAL_DURATION)
    {
        CProjectileBehavior* pBehavior = new CProjectileBehavior;
        pBehavior->m_nType = CProjectileBehavior::BEHAVIOR_SPIRAL_APPROACH;
        pBehavior->m_vStartPosition = vSpiral;
        if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
        {
            pBehavior->m_vTargetPosition = m_vDesiredPosition;
        }
        else
        {
            pBehavior->m_vTargetPosition = Vector();
        }
        pBehavior->m_nSpawnCount = 0;
        pBehavior->m_nDuration = m_nDesiredPositionTimer - PROJECTILE_SPIRAL_DURATION;
        pBehavior->m_nElapsedTime = 0;
        pBehavior->m_nTargetId = m_nDesiredTargetId;
        pBehavior->m_nProjectileType = 0;
        pBehavior->m_fDistance = PROJECTILE_SPIRAL_RADIUS;
        m_lstBehaviors.AddTail(pBehavior);
    }
    else if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        SetPosition(vSpiral);
    }
    else
    {
        CGameObjectArray* pGameObjectArray =
            g_pAppManager->m_pClientExoApp->GetGameObjectArray();
        CGameObject* pTarget;
        if (pGameObjectArray->GetGameObject(m_nDesiredTargetId, &pTarget) ==
            CGameObjectArray::SUCCESS)
        {
            Vector vPosition;

            vPosition = vSpiral + ((CSWCObject*)pTarget)->m_vPosition;
            vPosition.m_fZ = m_vPosition.m_fZ;
            SetPosition(vPosition);
        }
    }

    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_nType = CProjectileBehavior::BEHAVIOR_SPIRAL;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        pBehavior->m_vStartPosition = m_vDesiredPosition;
        pBehavior->m_vTargetPosition = m_vDesiredPosition;
    }
    else
    {
        pBehavior->m_vStartPosition = Vector();
        pBehavior->m_vTargetPosition = Vector();
    }
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = PROJECTILE_SPIRAL_DURATION;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x00742D90
void CSWCProjectile::SetBehaviorListLinked()
{
    Vector vPosition;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else if (g_pAppManager->m_pClientExoApp->GetGameObject(
                 m_nDesiredTargetId) != NULL)
    {
        vPosition = Vector();
    }
    else
    {
        vPosition = m_vDesiredPosition;
    }
    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = 7;
    pBehavior->m_vTargetPosition = Vector();
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x00742EA0
void CSWCProjectile::SetBehaviorListParried()
{
    Vector vPosition = Vector();
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        if (pApp->GetGameObject(m_nDesiredTargetId) == NULL)
        {
            vPosition = m_vDesiredPosition;
        }
    }

    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_nType = 1;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nProjectileType = 1;
    pBehavior->m_fDistance = 0.0f;
    m_lstBehaviors.AddTail(pBehavior);

    pBehavior = new CProjectileBehavior;
    pBehavior->m_nType = 1;
    pBehavior->m_nTargetId = OBJECT_INVALID;
    pBehavior->m_vStartPosition = m_vSourcePosition;
    pBehavior->m_vTargetPosition = m_vSourcePosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nSourceDuration;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nProjectileType = 1;
    pBehavior->m_fDistance = 0.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x00743000
void CSWCProjectile::SetBehaviorListDeflected()
{
    Vector vPosition = Vector();
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        if (pApp->GetGameObject(m_nDesiredTargetId) == NULL)
        {
            vPosition = m_vDesiredPosition;
        }
    }

    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_nType = 1;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nProjectileType = 1;
    pBehavior->m_fDistance = 0.0f;
    m_lstBehaviors.AddTail(pBehavior);

    pBehavior = new CProjectileBehavior;
    pBehavior->m_nType = 1;
    pBehavior->m_nTargetId = m_nSourceTargetId;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nProjectileType = 1;
    pBehavior->m_fDistance = 0.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x00743150
void CSWCProjectile::SetBehaviorListReflected()
{
    Vector vPosition = Vector();
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        if (pApp->GetGameObject(m_nDesiredTargetId) == NULL)
        {
            vPosition = m_vDesiredPosition;
        }
    }

    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_nType = 1;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nProjectileType = 1;
    pBehavior->m_fDistance = (float)(1.0 * m_fSourceDistance);
    m_lstBehaviors.AddTail(pBehavior);

    pBehavior = new CProjectileBehavior;
    pBehavior->m_nType = 1;
    pBehavior->m_nTargetId = OBJECT_INVALID;
    pBehavior->m_vStartPosition = m_vSourcePosition;
    pBehavior->m_vTargetPosition = m_vSourcePosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nSourceDuration;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nProjectileType = 1;
    pBehavior->m_fDistance = 0.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x007432B0
void CSWCProjectile::SetBehaviorListLinkedBurstUp()
{
    Vector vPosition;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else if (g_pAppManager->m_pClientExoApp->GetGameObject(
                 m_nDesiredTargetId) != NULL)
    {
        vPosition = Vector();
    }
    else
    {
        vPosition = m_vDesiredPosition;
    }
    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = 12;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x007433A0
void CSWCProjectile::SetBehaviorListTest()
{
    Vector vPosition;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else if (g_pAppManager->m_pClientExoApp->GetGameObject(
                 m_nDesiredTargetId) != NULL)
    {
        vPosition = Vector();
    }
    else
    {
        vPosition = m_vDesiredPosition;
    }

    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = 2;
    pBehavior->m_vTargetPosition = m_vDesiredPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 6 * (unsigned int)m_nDesiredPositionTimer / 10;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);

    pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = 4;
    pBehavior->m_vTargetPosition = m_vDesiredPosition;
    pBehavior->m_nSpawnCount = 3;
    pBehavior->m_nDuration = 6 * (unsigned int)m_nDesiredPositionTimer / 10;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 9;
    pBehavior->m_fDistance = -1.0f;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x00743560
CSWCProjectile::CSWCProjectile()
    : CSWCObject(CGameObject::OBJECT_TYPE_PROJECTILE)
{
    unsigned long nDay;
    unsigned long nTime;
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
    m_vPosition = Vector();
    m_vDesiredPosition = Vector();
    m_bHasImpactSound = 1;
    m_bImpactSoundStarted = 0;
    m_bProjectileFinished = 0;
    m_bUseModuleOrientation = 0;
    m_nProjectileType = 0;
    m_vVelocity = Vector();
    m_vAcceleration = Vector();
    m_bApplyImpactToTarget = 1;
    m_nProjectileDuration = 1;
    m_pSound = NULL;
    m_nDesiredPositionTime = 0;
    m_nDesiredPositionTimer = 0;
    g_pAppManager->GetClientExoApp()->GetGameObjectArray()->AddInternalObject(m_nId, this, 0);
    g_pAppManager->GetClientExoApp()->GetClientAIMaster()->AddObject(this);
    m_pCurrentBehavior = NULL;
    m_nSpellId = 0;
    m_pReferenceObject = NULL;
    m_bSpiralCounterClockwise = 0;
    m_nOrientationType = PROJECTILE_ORIENTATION_NONE;
    m_bSpawnCombatVisualOnImpact = 0;
    m_vImpactPosition = Vector();
    m_nCombatVisualEffect = 0;
    m_nCombatVisualSpellId = 0;
    m_nImpactType = 0;
    m_sImpactHook = "impact";
    m_bImpactAnimationStarted = 0;
    m_nUnused = 0;
    m_pMuzzleFlash = NULL;
    m_nSourceDuration = 0;
    m_fSourceDistance = 0.0f;
    m_nDesiredTargetId = OBJECT_INVALID;
    m_nSourceTargetId = OBJECT_INVALID;
}
// PC 0x007437F0
CSWCProjectile::~CSWCProjectile()
{
    g_pAppManager->m_pClientExoApp->GetGameObjectArray()->Delete(m_nId);
    ClearBehaviorList();
    UnloadModel();
}
// PC 0x007438C0
void CSWCProjectile::SetBehaviorListTripleBallistic(int bHit)
{
    CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
    CAurScene* pScene = pArea->m_pScene;
    int nSoundRow1 = -1;
    int nSoundRow2 = -1;
    int nSoundRow3 = -1;
    int nSoundRow4;
    Vector vHit;
    CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
    int bValid = 1;
    Vector vPosition;
    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else
    {
        CClientExoApp* pApp = g_pAppManager->GetClientExoApp();
        CSWCObject* pTarget = pApp->GetGameObject(m_nDesiredTargetId);
        if (pTarget != NULL)
        {
            vPosition = Vector();
        }
        else
        {
            vPosition = m_vDesiredPosition;
        }
    }
    Vector vPosition1 = 0.8f * (vPosition - m_vPosition) + m_vPosition;
    Vector vGround1 = vPosition1;
    int nDuration1 = (int)(0.6f * (unsigned int)m_nDesiredPositionTimer);
    Vector vPosition2 = 0.9f * (vPosition - m_vPosition) + m_vPosition;
    Vector vGround2 = vPosition2;
    int nDuration2 = (int)(0.2f * (unsigned int)m_nDesiredPositionTimer);
    Vector vPosition3 = 0.95f * (vPosition - m_vPosition) + m_vPosition;
    Vector vGround3 = vPosition3;
    int nDuration3 = (int)(0.1f * (unsigned int)m_nDesiredPositionTimer);
    Vector vGround4 = vPosition;
    if (pScene != NULL)
    {
        Vector vEnd = vPosition1;
        Vector vStart = vPosition1;
        vStart.m_fZ = 1000.0f;
        vEnd.m_fZ = -1000.0f;
        hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
        if (pScene->HitCheck(vStart, vEnd, &vHit, &hitInfo))
        {
            vGround1.m_fZ = vHit.m_fZ;
            nSoundRow1 = hitInfo.m_nOutputIndex;
        }
        else
        {
            vGround1.m_fZ = 0.0f;
            bValid = 0;
        }
        vEnd = vPosition2;
        vStart = vPosition2;
        vStart.m_fZ = 1000.0f;
        vEnd.m_fZ = -1000.0f;
        if (pScene->HitCheck(vStart, vEnd, &vHit, &hitInfo))
        {
            vGround2.m_fZ = vHit.m_fZ;
            nSoundRow2 = hitInfo.m_nOutputIndex;
        }
        else
        {
            vGround2.m_fZ = 0.0f;
            bValid = 0;
        }
        vEnd = vPosition3;
        vStart = vPosition3;
        vStart.m_fZ = 1000.0f;
        vEnd.m_fZ = -1000.0f;
        if (pScene->HitCheck(vStart, vEnd, &vHit, &hitInfo))
        {
            vGround3.m_fZ = vHit.m_fZ;
            nSoundRow3 = hitInfo.m_nOutputIndex;
        }
        else
        {
            vGround3.m_fZ = 0.0f;
            bValid = 0;
        }
        vEnd = vPosition;
        vStart = vPosition;
        vStart.m_fZ = 1000.0f;
        vEnd.m_fZ = -1000.0f;
        if (pScene->HitCheck(vStart, vEnd, &vHit, &hitInfo))
        {
            vGround4.m_fZ = vHit.m_fZ;
            nSoundRow4 = hitInfo.m_nOutputIndex;
        }
        else
        {
            bValid = 0;
        }
    }
    else
    {
        bValid = 0;
    }
    CProjectileBehavior* pBehavior;
    if (bValid)
    {
        pBehavior = new CProjectileBehavior;
        pBehavior->m_vStartPosition = vGround1;
        pBehavior->m_nType = CProjectileBehavior::TYPE_BALLISTIC;
        pBehavior->m_vTargetPosition = vGround1;
        pBehavior->m_nSpawnCount = 0;
        pBehavior->m_nDuration = nDuration1;
        pBehavior->m_nElapsedTime = 0;
        pBehavior->m_nTargetId = m_nDesiredTargetId;
        pBehavior->m_nProjectileType = 0;
        pBehavior->m_fDistance = PROJECTILE_BEHAVIOR_DISTANCE_UNSET;
        pBehavior->m_nSoundRow = nSoundRow1;
        m_lstBehaviors.AddTail(pBehavior);
        float fSpeed2 = magnitude(m_vPosition - m_vDesiredPosition) / (unsigned int)m_nDesiredPositionTimer;
        int nTime2 = (int)(magnitude(vGround1 - vGround2) / fSpeed2);
        pBehavior = new CProjectileBehavior;
        pBehavior->m_vStartPosition = vGround2;
        pBehavior->m_nType = CProjectileBehavior::TYPE_BALLISTIC;
        pBehavior->m_vTargetPosition = vGround2;
        pBehavior->m_nSpawnCount = 0;
        pBehavior->m_nDuration = nDuration2;
        pBehavior->m_nElapsedTime = 0;
        pBehavior->m_nTargetId = OBJECT_INVALID;
        pBehavior->m_nProjectileType = 0;
        pBehavior->m_fDistance = PROJECTILE_BEHAVIOR_DISTANCE_UNSET;
        pBehavior->m_nSoundRow = nSoundRow2;
        m_lstBehaviors.AddTail(pBehavior);
        float fSpeed3 = magnitude(m_vPosition - m_vDesiredPosition) / (unsigned int)m_nDesiredPositionTimer;
        int nTime3 = (int)(magnitude(vGround2 - vGround3) / fSpeed3);
        pBehavior = new CProjectileBehavior;
        pBehavior->m_vStartPosition = vGround3;
        pBehavior->m_nType = CProjectileBehavior::TYPE_BALLISTIC;
        pBehavior->m_vTargetPosition = vGround3;
        pBehavior->m_nSpawnCount = 0;
        pBehavior->m_nDuration = nDuration3;
        pBehavior->m_nElapsedTime = 0;
        pBehavior->m_nTargetId = OBJECT_INVALID;
        pBehavior->m_nProjectileType = 0;
        pBehavior->m_fDistance = PROJECTILE_BEHAVIOR_DISTANCE_UNSET;
        pBehavior->m_nSoundRow = nSoundRow3;
        m_lstBehaviors.AddTail(pBehavior);
        float fSpeed4 = magnitude(m_vPosition - m_vDesiredPosition) / (unsigned int)m_nDesiredPositionTimer;
        int nTime4 = (int)(magnitude(vGround2 - vGround3) / fSpeed4);
        pBehavior = new CProjectileBehavior;
        pBehavior->m_vStartPosition = vGround4;
        pBehavior->m_nType = CProjectileBehavior::TYPE_BALLISTIC;
        pBehavior->m_vTargetPosition = vGround4;
        pBehavior->m_nSpawnCount = 0;
        pBehavior->m_nDuration = nDuration3;
        pBehavior->m_nElapsedTime = 0;
        pBehavior->m_nTargetId = OBJECT_INVALID;
        pBehavior->m_nProjectileType = 0;
        pBehavior->m_fDistance = PROJECTILE_BEHAVIOR_DISTANCE_UNSET;
        pBehavior->m_nSoundRow = nSoundRow4;
        m_lstBehaviors.AddTail(pBehavior);
    }
    else
    {
        float fSpeed = magnitude(m_vPosition - m_vDesiredPosition) / (unsigned int)m_nDesiredPositionTimer;
        int nTime = (int)(magnitude(m_vPosition - vPosition) / fSpeed);
        pBehavior = new CProjectileBehavior;
        pBehavior->m_vStartPosition = vPosition;
        pBehavior->m_nType = CProjectileBehavior::TYPE_BALLISTIC;
        pBehavior->m_vTargetPosition = vPosition;
        pBehavior->m_nSpawnCount = 0;
        pBehavior->m_nDuration = m_nDesiredPositionTimer;
        pBehavior->m_nElapsedTime = 0;
        pBehavior->m_nTargetId = OBJECT_INVALID;
        pBehavior->m_nProjectileType = 0;
        pBehavior->m_fDistance = PROJECTILE_BEHAVIOR_DISTANCE_UNSET;
        m_lstBehaviors.AddTail(pBehavior);
    }
}
// PC 0x007442A0
void CSWCProjectile::SetBehaviorListBounce()
{
    CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
    Vector vHit;
    Vector vPositions[3];
    Vector vEnd;
    Vector vStart;
    Vector vDirection = m_vDesiredPosition - m_vPosition;
    hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
    CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
    CAurScene* pScene = pArea->m_pScene;
    unsigned char nCount = 3;
    vPositions[2] = m_vDesiredPosition;
    unsigned int nDurations[3];
    float fFirst = 0.25f;
    do
    {
        fFirst -= 0.05f;
        vPositions[1] = m_vDesiredPosition - fFirst * vDirection;
        vStart = vPositions[1];
        vEnd = vPositions[1];
        vStart.m_fZ = 1000.0f;
        vEnd.m_fZ = -1000.0f;
        if (pScene->HitCheck(vStart, vEnd, &vHit, &hitInfo))
        {
            vPositions[1].m_fZ = vHit.m_fZ;
        }
        else
        {
            vPositions[1].m_fZ = (m_vDesiredPosition.m_fZ + m_vPosition.m_fZ) * 0.5f;
        }
        if (!(fFirst > 0.00001f))
        {
            break;
        }
    } while (vPositions[2].m_fZ - vPositions[1].m_fZ >= 1.0f);
    if (fFirst > 0.00001f)
    {
        float fSecond = 0.55f;
        do
        {
            fSecond -= 0.05f;
            vPositions[0] = m_vDesiredPosition - fSecond * vDirection;
            vStart = vPositions[0];
            vEnd = vPositions[0];
            vStart.m_fZ = 1000.0f;
            vEnd.m_fZ = -1000.0f;
            if (pScene->HitCheck(vStart, vEnd, &vHit, &hitInfo))
            {
                vPositions[0].m_fZ = vHit.m_fZ;
            }
            else
            {
                vPositions[0].m_fZ = (m_vDesiredPosition.m_fZ + m_vPosition.m_fZ) * 0.5f;
            }
        } while (fSecond > fFirst && vPositions[1].m_fZ - vPositions[0].m_fZ >= 0.5f);
        Vector vSecondSegment = vPositions[0] - vPositions[1];
        Vector vFirstSegment = vPositions[1] - vPositions[2];
        if (vSecondSegment * vSecondSegment < vFirstSegment * vFirstSegment)
        {
            fSecond = fFirst;
        }
        if (fSecond <= fFirst)
        {
            nCount = 2;
            vPositions[0] = vPositions[1];
            vPositions[1] = m_vDesiredPosition;
        }
    }
    else
    {
        nDurations[1] = 0;
        nCount = 1;
        vPositions[0] = m_vDesiredPosition;
    }
    if (nCount == 1)
    {
        nDurations[0] = m_nDesiredPositionTimer;
    }
    else if (nCount == 3)
    {
        nDurations[2] = 2 * (unsigned int)m_nDesiredPositionTimer / 10;
        nDurations[1] = 3 * (unsigned int)m_nDesiredPositionTimer / 10;
        nDurations[0] = m_nDesiredPositionTimer - nDurations[2] - nDurations[1];
    }
    else
    {
        nDurations[1] = 3 * (unsigned int)m_nDesiredPositionTimer / 10;
        nDurations[0] = m_nDesiredPositionTimer - nDurations[1];
    }
    for (unsigned char n = 0; n < nCount; ++n)
    {
        CProjectileBehavior* pBehavior = new CProjectileBehavior;
        pBehavior->m_vStartPosition = vPositions[n];
        pBehavior->m_nType = CProjectileBehavior::TYPE_BALLISTIC;
        pBehavior->m_vTargetPosition = m_vDesiredPosition;
        pBehavior->m_nSpawnCount = 0;
        pBehavior->m_nDuration = nDurations[n];
        pBehavior->m_nElapsedTime = 0;
        pBehavior->m_nTargetId = CGameObjectArray::INVALIDOBJID;
        pBehavior->m_nProjectileType = 0;
        pBehavior->m_fDistance = PROJECTILE_BEHAVIOR_DISTANCE_UNSET;
        m_lstBehaviors.AddTail(pBehavior);
    }
}
// PC 0x00744840
void CSWCProjectile::SetBehaviorListDoubleBallistic()
{
    Vector vPosition;
    Vector vDirection;
    Vector vHit;
    CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);

    if (m_nDesiredTargetId == CGameObjectArray::INVALIDOBJID)
    {
        vPosition = m_vDesiredPosition;
    }
    else
    {
        CClientExoApp* pApp = g_pAppManager->GetClientExoApp();
        CSWCObject* pTarget = pApp->GetGameObject(m_nDesiredTargetId);
        if (pTarget != NULL)
        {
            vPosition = Vector();
        }
        else
        {
            vPosition = m_vDesiredPosition;
        }
    }

    CSWCArea* pArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
    CAurScene* pScene = pArea->m_pScene;
    CProjectileBehavior* pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = CProjectileBehavior::TYPE_BALLISTIC;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = m_nDesiredPositionTimer;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = m_nDesiredTargetId;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = PROJECTILE_BEHAVIOR_DISTANCE_UNSET;
    m_lstBehaviors.AddTail(pBehavior);

    hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
    Vector vReverse = -m_vOrientation;
    float fAngle = (float)atan(vReverse.m_fY / vReverse.m_fX);
    fAngle += (rand() % PROJECTILE_DOUBLE_BALLISTIC_ANGLE_STEPS) / PROJECTILE_DOUBLE_BALLISTIC_ANGLE_UNITS_PER_RADIAN + PROJECTILE_DOUBLE_BALLISTIC_MIN_ANGLE;
    if (fAngle > PROJECTILE_DOUBLE_BALLISTIC_MAX_ANGLE)
    {
        fAngle = PROJECTILE_DOUBLE_BALLISTIC_MAX_ANGLE;
    }
    vDirection.m_fX = cos(fAngle);
    vDirection.m_fY = sin(fAngle);
    if (vReverse.m_fX < 0.0f)
    {
        vDirection = Vector(-vDirection.m_fX, -vDirection.m_fY, 0.0f);
    }
    vPosition = PROJECTILE_DOUBLE_BALLISTIC_OFFSET * vDirection + m_vDesiredPosition;
    if (pScene != NULL)
    {
        Vector vEnd = vPosition;
        Vector vStart = vPosition;
        vStart.m_fZ = PROJECTILE_DOUBLE_BALLISTIC_GROUND_TRACE_HEIGHT;
        vEnd.m_fZ = -PROJECTILE_DOUBLE_BALLISTIC_GROUND_TRACE_HEIGHT;
        if (pScene->HitCheck(vStart, vEnd, &vHit, &hitInfo))
        {
            vPosition.m_fZ = vHit.m_fZ;
        }
        else
        {
            vPosition.m_fZ = 0.0f;
        }
    }
    else
    {
        vPosition.m_fZ = 0.0f;
    }

    float fSpeed = magnitude(m_vPosition - m_vDesiredPosition) /
                  (unsigned int)m_nDesiredPositionTimer * PROJECTILE_DOUBLE_BALLISTIC_RETURN_SPEED_SCALE;
    int nDuration = (int)(magnitude(m_vDesiredPosition - vPosition) / fSpeed);
    pBehavior = new CProjectileBehavior;
    pBehavior->m_vStartPosition = vPosition;
    pBehavior->m_nType = CProjectileBehavior::TYPE_BALLISTIC;
    pBehavior->m_vTargetPosition = vPosition;
    pBehavior->m_nSpawnCount = 0;
    pBehavior->m_nDuration = nDuration;
    pBehavior->m_nElapsedTime = 0;
    pBehavior->m_nTargetId = OBJECT_INVALID;
    pBehavior->m_nProjectileType = 0;
    pBehavior->m_fDistance = PROJECTILE_BEHAVIOR_DISTANCE_UNSET;
    m_lstBehaviors.AddTail(pBehavior);
}
// PC 0x00744C90
void CSWCProjectile::SetProjectileType(unsigned long nType)
{
    m_nProjectileType = nType;

    if (m_nDesiredTargetId != CGameObjectArray::INVALIDOBJID)
    {
        CGameObjectArray* pGameObjectArray =
            g_pAppManager->m_pClientExoApp->GetGameObjectArray();
        CGameObject* pTarget;
        if (pGameObjectArray->GetGameObject(m_nDesiredTargetId, &pTarget) ==
            CGameObjectArray::SUCCESS)
        {
            m_vDesiredPosition = ((CSWCObject*)pTarget)->m_vPosition;
        }
    }

    ClearBehaviorList();

    switch (nType)
    {
    case PROJECTILE_PATH_TYPE_HOMING:
        SetBehaviorListHoming();
        break;
    case PROJECTILE_PATH_TYPE_BALLISTIC:
        SetBehaviorListBallistic();
        break;
    case PROJECTILE_PATH_TYPE_HIGH_BALLISTIC:
        SetBehaviorListHighBallistic();
        break;
    case PROJECTILE_PATH_TYPE_BURST_UP:
        SetBehaviorListBurstUp();
        break;
    case PROJECTILE_PATH_TYPE_BURST:
        SetBehaviorListBurst();
        break;
    case PROJECTILE_PATH_TYPE_ACCELERATING:
        SetBehaviorListAccelerating();
        break;
    case PROJECTILE_PATH_TYPE_SPIRAL:
        SetBehaviorListSpiral();
        break;
    case PROJECTILE_PATH_TYPE_LINKED:
        SetBehaviorListLinked();
        break;
    case PROJECTILE_PATH_TYPE_BOUNCE:
        SetBehaviorListBounce();
        break;
    case PROJECTILE_PATH_TYPE_LINKED_BURST_UP:
        SetBehaviorListLinkedBurstUp();
        break;
    case PROJECTILE_PATH_TYPE_TEST:
        SetBehaviorListTest();
        break;
    case PROJECTILE_PATH_TYPE_TRIPLE_BALLISTIC_HIT:
        SetBehaviorListTripleBallistic(1);
        break;
    case PROJECTILE_PATH_TYPE_TRIPLE_BALLISTIC_MISS:
        SetBehaviorListTripleBallistic(0);
        break;
    case PROJECTILE_PATH_TYPE_DOUBLE_BALLISTIC:
        SetBehaviorListDoubleBallistic();
        break;
    case PROJECTILE_PATH_TYPE_PARRIED:
        SetBehaviorListParried();
        break;
    case PROJECTILE_PATH_TYPE_DEFLECTED:
        SetBehaviorListDeflected();
        break;
    case PROJECTILE_PATH_TYPE_REFLECTED:
        SetBehaviorListReflected();
        break;
    default:
        return;
    }

    m_pCurrentBehavior = m_lstBehaviors.RemoveHead();
    SetDesiredPositionTimer(m_pCurrentBehavior->m_nDuration);
    m_vDesiredPosition = m_pCurrentBehavior->m_vStartPosition;
    SetProjectileVelAndAccel(m_vOrientation);
}
// PC 0x00744E80
int CSWCProjectile::UpdateCurrentBehavior()
{
    if (m_pCurrentBehavior != NULL)
    {
        delete m_pCurrentBehavior;
        m_pCurrentBehavior = NULL;
    }

    do
    {
        if (m_lstBehaviors.GetHeadPos() == NULL)
        {
            return 0;
        }
        m_pCurrentBehavior = m_lstBehaviors.RemoveHead();
        if (m_pCurrentBehavior == NULL)
        {
            return 0;
        }

        if (m_pCurrentBehavior->m_nType == CProjectileBehavior::TYPE_BALLISTIC &&
            m_pCurrentBehavior->m_nSoundRow != -1)
        {
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pGrenadeSound);
            C2DA* pGrenadeSound = p2DArrays->m_pGrenadeSound;
            CExoString sSOUND;
            if (pGrenadeSound->GetCExoStringEntry(
                    m_pCurrentBehavior->m_nSoundRow, "SOUND", &sSOUND))
            {
                CResRef cSound(sSOUND);
                g_pExoSound->Play3DOneShotSound(cSound, m_vPosition,
                                               0.0f, 15, 0, 255, 0.0f, 0.0f);
            }
        }

        if (m_pCurrentBehavior->m_nType ==
            CProjectileBehavior::TYPE_HIGH_BALLISTIC_TRANSITION)
        {
            SetAnimation(PROJECTILE_ANIMATION_DEFAULT);
            GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->AnimateExternal(1.0f);
            if (m_pCurrentBehavior->m_nTargetId == CGameObjectArray::INVALIDOBJID)
            {
                SetPosition(m_pCurrentBehavior->m_vStartPosition);
            }
            else
            {
                CGameObject* pTarget;
                CGameObjectArray* pObjects =
                    g_pAppManager->m_pClientExoApp->GetGameObjectArray();
                if (pObjects->GetGameObject(m_pCurrentBehavior->m_nTargetId,
                                           &pTarget) == CGameObjectArray::SUCCESS)
                {
                    SetPosition(((CSWCObject*)pTarget)->m_vPosition +
                                m_pCurrentBehavior->m_vStartPosition);
                }
            }
            GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1)->AnimateExternal(1.0f);
            SetAnimation(PROJECTILE_ANIMATION_TRAVEL);
        }

        for (unsigned char nSpawn = 0;
             nSpawn < m_pCurrentBehavior->m_nSpawnCount; ++nSpawn)
        {
            int nSpellId = m_nSpellId;
            CSWSpellArray* pSpellArray = g_pRules->m_pSpellArray;
            CSWSpell* pSpell = pSpellArray->GetSpell(nSpellId);
            if (pSpell == NULL)
            {
                break;
            }
            CSWCProjectile* pProjectile = new CSWCProjectile;
            pProjectile->SetProjectileSpellId(m_nSpellId);
            pProjectile->LoadModel(pSpell->m_resProjectileModel, CSWCObject::MODEL_SLOT_DEFAULT, 0);
            CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
            if (pModule != NULL && pModule->m_pInfo != NULL)
            {
                CAurScene* pScene = pModule->m_pInfo->m_pScene;
                pProjectile->AttachToScene(pScene);
            }
            pProjectile->SetPosition(m_vPosition);
            pProjectile->SetOrientation(m_vOrientation);
            pProjectile->SetDesiredPositionTimer(m_pCurrentBehavior->m_nDuration);
            pProjectile->SetAnimation(GetCurrentAnimation());
            const Vector& vStartPosition = m_pCurrentBehavior->m_vStartPosition;
            pProjectile->m_vDesiredPosition = vStartPosition;
            pProjectile->SetDesiredTargetObject(m_nDesiredTargetId, "");
            pProjectile->SetProjectileType(m_pCurrentBehavior->m_nProjectileType);
        }

        SetDesiredPositionTimer(m_pCurrentBehavior->m_nDuration);
        m_vDesiredPosition = m_pCurrentBehavior->m_vStartPosition;
        SetProjectileVelAndAccel(m_vOrientation);

        if (m_nProjectileType == PROJECTILE_PATH_TYPE_DEFLECTED)
        {
            m_bImpactAnimationStarted = 1;
            SetDesiredTargetObject(m_pCurrentBehavior->m_nTargetId, "impact");
        }
        else if (m_nProjectileType == PROJECTILE_PATH_TYPE_REFLECTED &&
                 m_pArea != NULL)
        {
            m_pArea->ApplyFireAndForgetVisualEffect(VFX_COM_SPARKS_PARRY_METAL,
                                                   m_vPosition);
        }
    } while (m_pCurrentBehavior->m_nType ==
                 CProjectileBehavior::TYPE_HIGH_BALLISTIC_TRANSITION ||
             m_nTimeDelta >= m_nDesiredPositionTime);

    return 1;
}
// PC 0x00745340
void CSWCProjectile::MoveProjectileToPosition()
{
    Vector vTarget;
    if (m_pCurrentBehavior == NULL)
    {
        m_bProjectileFinished = 1;
        return;
    }

    int bCompleted = 0;
    if (m_pCurrentBehavior->m_nTargetId == CGameObjectArray::INVALIDOBJID)
        m_vDesiredPosition = m_pCurrentBehavior->m_vStartPosition;
    else
    {
        Quaternion qTarget;
        CGameObject* pGameObject;
        CGameObjectArray* pObjects = g_pAppManager->GetClientExoApp()->GetGameObjectArray();
        if (pObjects->GetGameObject(
                m_pCurrentBehavior->m_nTargetId, &pGameObject) == CGameObjectArray::SUCCESS)
        {
            CSWCObject* pTarget = (CSWCObject*)pGameObject;
            if (pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) != NULL)
            {
                Gob* pModel = pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
                if (!pModel->GetPartPosition(m_sImpactHook.CStr(), &vTarget, &qTarget))
                {
                    if (m_nImpactType == IMPACT_UNATTACHED_3 || m_nImpactType == IMPACT_SHIELD)
                    {
                        vTarget = pTarget->m_vPosition;
                        vTarget.m_fZ += PROJECTILE_TARGET_HEIGHT;
                        vTarget += -m_vOrientation * PROJECTILE_TARGET_BACK_OFFSET;
                    }
                    else
                    {
                        vTarget = pTarget->m_vPosition;
                        vTarget.m_fZ += PROJECTILE_TARGET_HEIGHT;
                    }
                }
            }
            else
                vTarget = m_vDesiredPosition;
            m_pCurrentBehavior->m_vTargetPosition = vTarget;
            m_vDesiredPosition = vTarget + m_pCurrentBehavior->m_vStartPosition;
        }
        else
            m_vDesiredPosition = m_pCurrentBehavior->m_vStartPosition;
    }

    if (m_nTimeDelta >= m_nDesiredPositionTime)
    {
        m_nTimeDelta -= m_nDesiredPositionTime;
        m_nDesiredPositionTime = 0;
        SetPosition(m_vDesiredPosition);
        bCompleted = 1;
    }
    if (m_nDesiredPositionTime - m_nTimeDelta <=
            (unsigned long)m_pCurrentBehavior->m_nElapsedTime || bCompleted)
    {
        if (!UpdateCurrentBehavior())
            return;
    }
    m_nDesiredPositionTime -= m_nTimeDelta;
    if (m_pCurrentBehavior == NULL)
        return;

    if (m_pCurrentBehavior->m_nTargetId == CGameObjectArray::INVALIDOBJID)
        m_vDesiredPosition = m_pCurrentBehavior->m_vStartPosition;
    else
    {
        Quaternion qTarget;
        CGameObject* pGameObject;
        CGameObjectArray* pObjects = g_pAppManager->GetClientExoApp()->GetGameObjectArray();
        if (pObjects->GetGameObject(
                m_pCurrentBehavior->m_nTargetId, &pGameObject) == CGameObjectArray::SUCCESS)
        {
            CSWCObject* pTarget = (CSWCObject*)pGameObject;
            if (pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) != NULL)
            {
                Gob* pModel = pTarget->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
                if (!pModel->GetPartPosition(m_sImpactHook.CStr(), &vTarget, &qTarget))
                {
                    if (m_nImpactType == IMPACT_UNATTACHED_3 || m_nImpactType == IMPACT_SHIELD)
                    {
                        vTarget = pTarget->m_vPosition;
                        vTarget.m_fZ += PROJECTILE_TARGET_HEIGHT;
                        vTarget += -m_vOrientation * PROJECTILE_TARGET_BACK_OFFSET;
                    }
                    else
                    {
                        vTarget = pTarget->m_vPosition;
                        vTarget.m_fZ += PROJECTILE_TARGET_HEIGHT;
                    }
                }
            }
            else
                vTarget = m_vDesiredPosition;
            m_pCurrentBehavior->m_vTargetPosition = vTarget;
            m_vDesiredPosition = vTarget + m_pCurrentBehavior->m_vStartPosition;
        }
    }

    Vector vDistance = m_vDesiredPosition - m_vPosition;
    if (vDistance * vDistance <= PROJECTILE_POSITION_EPSILON_SQUARED)
    {
        SetPosition(m_vDesiredPosition);
        if (m_pCurrentBehavior->m_fDistance >= 0.0f)
        {
            unsigned long nRemaining = m_nDesiredPositionTime;
            UpdateCurrentBehavior();
            m_nDesiredPositionTime += nRemaining;
        }
        return;
    }
    if (m_pCurrentBehavior->m_fDistance >= 0.0f &&
        m_pCurrentBehavior->m_nTargetId != CGameObjectArray::INVALIDOBJID)
    {
        vDistance = m_vPosition - vTarget;
        if (vDistance * vDistance <=
            m_pCurrentBehavior->m_fDistance * m_pCurrentBehavior->m_fDistance)
        {
            unsigned long nRemaining = m_nDesiredPositionTime;
            UpdateCurrentBehavior();
            m_nDesiredPositionTime += nRemaining;
            return;
        }
    }

    m_vImpactPosition = m_vPosition;
    switch (m_pCurrentBehavior->m_nType)
    {
    case CProjectileBehavior::TYPE_UNACCELERATED:
        MoveProjectileAccelerating();
        SetAnimation(PROJECTILE_ANIMATION_THROWRETURN);
        break;
    case CProjectileBehavior::BEHAVIOR_SPIRAL:
        MoveProjectileSpiral();
        break;
    case CProjectileBehavior::TYPE_HOMING:
    case CProjectileBehavior::TYPE_BALLISTIC:
    case CProjectileBehavior::TYPE_BURST_UP:
    case CProjectileBehavior::TYPE_ACCELERATING:
    case CProjectileBehavior::TYPE_BURST:
    case CProjectileBehavior::BEHAVIOR_SPIRAL_APPROACH:
    case CProjectileBehavior::TYPE_LINKED_BURST_UP:
        MoveProjectileAccelerating();
        break;
    }
}
// PC 0x00745980
void CSWCProjectile::AIUpdate()
{
    CExoString sSound;
    unsigned long nDay, nTime, nElapsedDay, nElapsedTime;
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastUpdateDay, m_nLastUpdateTime,
        &nElapsedDay, &nElapsedTime);
    m_nTimeDelta = nElapsedTime;

    if (m_bProjectileFinished == 1)
    {
        unsigned long nRemaining = m_nDesiredPositionTime;
        m_nDesiredPositionTime = 0;
        MoveProjectileToPosition();
        m_nDesiredPositionTime = nRemaining;
    }
    else
    {
        MoveProjectileToPosition();
    }

    if (m_bHasImpactSound == 1)
    {
        if (m_pSound != NULL)
            m_pSound->SetPosition(m_vPosition, 1.5f);

        if (m_bProjectileFinished == 1)
        {
            Quaternion qCamera = g_pAppManager->GetClientExoApp()->GetModule()->
                GetCamera()->GetOrientation();
            if (m_bUseModuleOrientation)
            {
                Vector vOrientation(cos((Yaw(qCamera) - 90.0f) * DEG_TO_RAD),
                                    sin((Yaw(qCamera) - 90.0f) * DEG_TO_RAD), 0.0f);
                SetOrientation(vOrientation);
            }
            if (m_nTimeDelta >= m_nDesiredPositionTime)
                m_nDesiredPositionTime = 0;
            else
                m_nDesiredPositionTime -= m_nTimeDelta;
            if (m_nDesiredPositionTime == 0)
            {
                AttachToScene(NULL);
                delete m_pSound;
                delete this;
                return;
            }
        }
        else if (GetCurrentAnimation() == PROJECTILE_ANIMATION_TRAVEL ||
                 GetCurrentAnimation() == PROJECTILE_ANIMATION_THROWOUT ||
                 GetCurrentAnimation() == PROJECTILE_ANIMATION_THROWRETURN ||
                 GetCurrentAnimation() == PROJECTILE_ANIMATION_COMBAT)
        {
            if (!m_bImpactSoundStarted)
            {
                if (m_pSound != NULL)
                {
                    delete m_pSound;
                    m_pSound = NULL;
                }
                if (*m_cFlightSound.GetResRef())
                {
                    m_pSound = new CExoSoundSource(m_cFlightSound);
                    if (m_pSound != NULL)
                    {
                        m_pSound->SetPriorityGroup(CExoSound::PRIORITY_GROUP_PROJECTILE);
                        m_pSound->Set3D(1);
                        m_pSound->SetLooping(1);
                        m_pSound->SetPosition(m_vPosition, 1.5f);
                        m_pSound->Play();
                    }
                }
            }
            if (m_nDesiredPositionTime <= PROJECTILE_IMPACT_REACTION_TIME &&
                m_nProjectileType >= PROJECTILE_PATH_TYPE_PARRIED &&
                !m_bImpactAnimationStarted)
            {
                unsigned long oidTarget = m_nDesiredTargetId;
                CSWCCreature* pCreature = g_pAppManager->GetClientExoApp()->
                    GetCreatureByGameObjectID(oidTarget);
                if (pCreature != NULL &&
                    !pCreature->AnimationFireAndForget(pCreature->GetCurrentAnimation()))
                {
                    CSWSCreature* pServerCreature = pCreature->GetServerCreature();
                    if (pServerCreature != NULL && !pServerCreature->GetDead() &&
                        !pServerCreature->GetDeadTemp())
                    {
                        if (m_nProjectileType == PROJECTILE_PATH_TYPE_PARRIED ||
                            m_nProjectileType == PROJECTILE_PATH_TYPE_DEFLECTED)
                        {
                            unsigned short nAnimation = rand() % 2 ?
                                CREATURE_ANIMATION_PROJECTILE_PARRY_1 :
                                CREATURE_ANIMATION_PROJECTILE_PARRY_2;
                            pCreature->GetAnimationBase()->SetOverlayAnimation(nAnimation, 1);
                            m_bImpactAnimationStarted = 1;
                        }
                    }
                }
            }
            m_bImpactSoundStarted = 1;
            if (m_nDesiredPositionTime == 0)
            {
                SetAnimation(PROJECTILE_ANIMATION_IMPACT);
                m_bProjectileFinished = 1;
                if (m_pSound != NULL)
                {
                    m_pSound->Stop();
                    delete m_pSound;
                    m_pSound = NULL;
                }
                if (m_cProjectileSound.IsValid() == 1)
                    g_pExoSound->Play3DOneShotSound(m_cProjectileSound, m_vPosition,
                        0.0f, CExoSound::PRIORITY_GROUP_PROJECTILE, 0, 0, 0.0f, 0.0f);
                if (m_bApplyImpactToTarget)
                {
                    CGameObject* pGameObject;
                    CGameObjectArray* pObjects = g_pAppManager->GetClientExoApp()->
                        GetGameObjectArray();
                    if (pObjects->GetGameObject(m_nDesiredTargetId, &pGameObject) ==
                        CGameObjectArray::SUCCESS)
                    {
                        Quaternion qOrientation;
                        Vector vPosition;
                        Gob* pTargetModel = NULL;
                        CSWCObject* pTarget = (CSWCObject*)pGameObject;
                        switch (m_nImpactType)
                        {
                        case IMPACT_TARGET:
                            if (pTarget->GetAnimationBase() != NULL)
                            {
                                if (pTarget->m_bOtherTexture == 0)
                                    pTargetModel = pTarget->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
                                else
                                    m_nProjectileDuration = 0;
                            }
                            break;
                        case IMPACT_SHIELD:
                            {
                                CSWCItem* pShield = g_pAppManager->GetClientExoApp()->
                                    GetItemByGameObjectID(pTarget->AsSWCCreature()->
                                        GetEquippedItemID(INVENTORY_SLOT_LEFTHAND));
                                if (pShield != NULL && g_pRules->m_pBaseItems->
                                    GetBaseItem(pShield->GetBaseItemType())->m_nWeaponWield == 0)
                                {
                                    if (pTarget->m_bOtherTexture == 0)
                                        pTargetModel = pShield->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
                                    else
                                        m_nProjectileDuration = 0;
                                }
                            }
                            break;
                        case IMPACT_UNATTACHED_4:
                            pTargetModel = NULL;
                            break;
                        }
                        Gob* pModel = GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
                        if (pTargetModel != NULL && pModel != NULL &&
                            m_nImpactType != IMPACT_UNATTACHED_1 &&
                            m_nImpactType != IMPACT_UNATTACHED_3)
                        {
                            int bAttached = FALSE;
                            if (m_nImpactType == IMPACT_TARGET)
                            {
                                pModel->AttachToObject(pTargetModel, m_sImpactHook.CStr(), Gob::ATTACH_NORMAL);
                                bAttached = TRUE;
                            }
                            else if (m_nImpactType == IMPACT_SHIELD)
                            {
                                CExoString sHook;
                                sHook.Format("imp_shield_%d", rand() % 2 + 1);
                                pModel->AttachToObject(pTargetModel, sHook.CStr(), Gob::ATTACH_NORMAL);
                                bAttached = TRUE;
                            }
                            if (bAttached)
                            {
                                pModel->GetPartPosition("root", &vPosition, &qOrientation);
                                qOrientation = YawPitchRoll(Yaw(m_vOrientation),
                                                           Pitch(m_vOrientation), 0.0f);
                                pModel->SetPartPosition("root", vPosition, qOrientation);
                            }
                        }
                        if (pTarget->AsSWCCreature() != NULL && m_bSpawnCombatVisualOnImpact)
                        {
                            Vector vDirection = m_vPosition - m_vImpactPosition;
                            CSWCCombatAttackData* pAttackData = new CSWCCombatAttackData;
                            pAttackData->m_nAttackResult = m_nCombatVisualSpellId ? 2 : 1;
                            pAttackData->m_nDamage = m_nCombatVisualEffect;
                            pAttackData->m_nImpactVisual = 1;
                            pTarget->AsSWCCreature()->SpawnHitVisuals(
                                m_bSpawnCombatVisualOnImpact, pAttackData,
                                vDirection, m_sImpactHook);
                            delete pAttackData;
                            m_bSpawnCombatVisualOnImpact = 0;
                        }
                    }
                    m_nDesiredPositionTime = m_nProjectileDuration;
                }
                else
                {
                    m_nDesiredPositionTime = 0;
                }
            }
        }
    }
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
}
