#include "nwcspellvisual.h"
#include "exobase.h"
#include "nwcanimbase.h"
#include "nwcobject.h"
#include <string.h>
// PC 0x006EFCB0 GOG 0x006EF8A0
CSWCSpellCastVisual::CSWCSpellCastVisual()
{
    m_pHeadConjureVisual = 0;
    m_pHandConjureVisual = 0;
    m_pGroundConjureVisual = 0;
    m_pHeadCastVisual = 0;
    m_pHandCastVisual = 0;
    m_pGroundCastVisual = 0;
    m_nConjureTime = 0;
    m_nCastTime = 0;
    m_bTimerRunning = 0;
    m_bHeadConjureFaded = 0;
    m_bHandConjureFaded = 0;
    m_bGroundConjureFaded = 0;
}
// PC 0x006EFCE0 GOG 0x006EF8D0
void CSWCSpellCastVisual::UnloadConjureVisual()
{
    if (m_pHeadConjureVisual != 0)
    {
        m_pHeadConjureVisual->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        delete m_pHeadConjureVisual;
        m_pHeadConjureVisual = 0;
    }
    if (m_pHandConjureVisual != 0)
    {
        m_pHandConjureVisual->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        delete m_pHandConjureVisual;
        m_pHandConjureVisual = 0;
    }
    if (m_pGroundConjureVisual != 0)
    {
        m_pGroundConjureVisual->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        delete m_pGroundConjureVisual;
        m_pGroundConjureVisual = 0;
    }
}
// PC 0x006EFD60 GOG 0x006EF950
void CSWCSpellCastVisual::UnloadCastVisual()
{
    if (m_pHeadCastVisual != 0)
    {
        m_pHeadCastVisual->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        delete m_pHeadCastVisual;
        m_pHeadCastVisual = 0;
    }
    if (m_pHandCastVisual != 0)
    {
        m_pHandCastVisual->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        delete m_pHandCastVisual;
        m_pHandCastVisual = 0;
    }
    if (m_pGroundCastVisual != 0)
    {
        m_pGroundCastVisual->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        delete m_pGroundCastVisual;
        m_pGroundCastVisual = 0;
    }
}
// PC 0x006EFDE0 GOG 0x006EF9D0
void CSWCSpellCastVisual::UpdateTimer(unsigned long nTime)
{
    if (m_bHeadConjureFaded)
    {
        if (m_pHeadConjureVisual != 0)
        {
            m_pHeadConjureVisual->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            delete m_pHeadConjureVisual;
            m_pHeadConjureVisual = 0;
        }
    }
    if (m_bHandConjureFaded)
    {
        if (m_pHandConjureVisual != 0)
        {
            m_pHandConjureVisual->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            delete m_pHandConjureVisual;
            m_pHandConjureVisual = 0;
        }
    }
    if (m_bGroundConjureFaded)
    {
        if (m_pGroundConjureVisual != 0)
        {
            m_pGroundConjureVisual->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            delete m_pGroundConjureVisual;
            m_pGroundConjureVisual = 0;
        }
    }
    if (m_bTimerRunning)
    {
        if (m_nConjureTime == 0 && m_nCastTime == 0)
        {
            m_bTimerRunning = 0;
        }
        else if (m_nConjureTime > 0)
        {
            if (m_nConjureTime <= nTime)
            {
                UnloadConjureVisual();
                m_nConjureTime = 0;
            }
            else
            {
                m_nConjureTime -= nTime;
            }
        }
        else if (m_nCastTime > 0)
        {
            if (m_nCastTime <= nTime)
            {
                UnloadCastVisual();
                m_nCastTime = 0;
                m_bTimerRunning = 0;
            }
            else
            {
                m_nCastTime -= nTime;
            }
        }
    }
}
// PC 0x006EFEC0 GOG 0x006EFAB0
void FadeAnimDoneEvent(CAurObject* pObject, char*, void* pData)
{
    CSWCSpellCastVisual* pVisual = static_cast<CSWCSpellCastVisual*>(pData);
    if (pVisual != 0)
    {
        if (pObject == pVisual->m_pHeadConjureVisual)
        {
            pVisual->m_bHeadConjureFaded = 1;
        }
        if (pObject == pVisual->m_pHandConjureVisual)
        {
            pVisual->m_bHandConjureFaded = 1;
        }
        if (pObject == pVisual->m_pGroundConjureVisual)
        {
            pVisual->m_bGroundConjureFaded = 1;
        }
    }
}
// PC 0x006EFEF0 GOG 0x006EFAE0
CSWCSpellCastVisual::~CSWCSpellCastVisual()
{
    UnloadConjureVisual();
    UnloadCastVisual();
}
// PC 0x006EFF00 GOG 0x006EFAF0
int CSWCSpellCastVisual::LoadConjureVisual(
    CResRef cHeadVisual, CResRef cHandVisual, CResRef cGroundVisual,
    CResRef cSound, CResRef cVocalization, CSWCCreature* pCreature)
{
    CExoString sVisual;
    CSWCObject* pObject = reinterpret_cast<CSWCObject*>(pCreature);

    UnloadConjureVisual();
    cHeadVisual.CopyToString(sVisual);
    if (sVisual.GetLength() > 0)
    {
        m_pHeadConjureVisual = static_cast<Gob*>(
            NewCAurObject((char*)sVisual.CStr(), "conjurevisual", 0, 0));
        if (m_pHeadConjureVisual != 0)
        {
            m_bHeadConjureFaded = 0;
            m_pHeadConjureVisual->RegisterCallBack(
                FadeAnimDoneEvent, "donefade", this, 9999.0f, 0);
            m_pHeadConjureVisual->AttachToObject(pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1),
                                                 "headconjure", 0);
            m_pHeadConjureVisual->PlayAnimation("conjure01", 1.0f, 0, 0.0f);
        }
    }
    cHandVisual.CopyToString(sVisual);
    if (sVisual.GetLength() > 0)
    {
        m_pHandConjureVisual = static_cast<Gob*>(
            NewCAurObject((char*)sVisual.CStr(), "conjurevisual", 0, 0));
        if (m_pHandConjureVisual != 0)
        {
            m_bHandConjureFaded = 0;
            m_pHandConjureVisual->RegisterCallBack(
                FadeAnimDoneEvent, "donefade", this, 9999.0f, 0);
            m_pHandConjureVisual->AttachToObject(pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1),
                                                 "handconjure", 0);
            m_pHandConjureVisual->PlayAnimation("conjure01", 1.0f, 0, 0.0f);
        }
    }
    cGroundVisual.CopyToString(sVisual);
    if (sVisual.GetLength() > 0)
    {
        m_pGroundConjureVisual = static_cast<Gob*>(
            NewCAurObject((char*)sVisual.CStr(), "conjuregroundvisual", 0, 0));
        if (m_pGroundConjureVisual != 0)
        {
            m_bGroundConjureFaded = 0;
            m_pGroundConjureVisual->RegisterCallBack(
                FadeAnimDoneEvent, "donefade", this, 9999.0f, 0);
            m_pGroundConjureVisual->AttachToObject(pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1),
                                                   "root", 0);
            m_pGroundConjureVisual->PlayAnimation("conjure01", 1.0f, 0, 0.0f);
        }
    }

    Quaternion qOrientation;
    qOrientation.m_fZ = 0.0f;
    qOrientation.m_fY = 0.0f;
    qOrientation.m_fX = 0.0f;
    qOrientation.m_fW = 1.0f;
    Vector vPosition;
    Gob* pModel = pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
    if (pModel == 0 ||
        pModel->GetPartPosition("handconjure", &vPosition, &qOrientation) == 0)
    {
        vPosition = pObject->m_vPosition;
    }
    if (*cSound.GetResRef() != 0)
    {
        g_pExoSound->Play3DOneShotSound(cSound, vPosition, 1.5f, 13, 0, 0, 0.0f,
                                        0.0f);
    }
    if (*cVocalization.GetResRef() != 0)
    {
        g_pExoSound->Play3DOneShotSound(cVocalization, vPosition, 1.5f, 13, 0,
                                        0, 0.0f, 0.0f);
    }
    return 1;
}
// PC 0x006F0250 GOG 0x006EFE40
int CSWCSpellCastVisual::LoadCastVisual(CResRef cHeadVisual,
                                        CResRef cHandVisual,
                                        CResRef cGroundVisual,
                                        CSWCCreature* pCreature)
{
    char sVisual[20];
    CSWCObject* pObject = reinterpret_cast<CSWCObject*>(pCreature);

    UnloadCastVisual();
    cHeadVisual.CopyToString(sVisual);
    if (strlen(sVisual) != 0)
    {
        m_pHeadCastVisual =
            static_cast<Gob*>(NewCAurObject(sVisual, "castvisual", 0, 0));
        if (m_pHeadCastVisual != 0)
        {
            m_pHeadCastVisual->AttachToObject(pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1),
                                              "headconjure", 0);
            m_pHeadCastVisual->PlayAnimation("cast01", 1.0f, 1, 0.0f);
        }
    }
    cHandVisual.CopyToString(sVisual);
    if (strlen(sVisual) != 0)
    {
        m_pHandCastVisual =
            static_cast<Gob*>(NewCAurObject(sVisual, "castvisual", 0, 0));
        if (m_pHandCastVisual != 0)
        {
            m_pHandCastVisual->AttachToObject(pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1),
                                              "handconjure", 0);
            m_pHandCastVisual->PlayAnimation("cast01", 1.0f, 1, 0.0f);
        }
    }
    cGroundVisual.CopyToString(sVisual);
    if (strlen(sVisual) != 0)
    {
        m_pGroundCastVisual =
            static_cast<Gob*>(NewCAurObject(sVisual, "castgroundvisual", 0, 0));
        if (m_pGroundCastVisual != 0)
        {
            m_pGroundCastVisual->AttachToObject(pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1),
                                                "root", 0);
            m_pGroundCastVisual->PlayAnimation("cast01", 1.0f, 1, 0.0f);
        }
    }
    if (m_pHandConjureVisual != 0)
    {
        m_pHandConjureVisual->PlayAnimation("fade", 1.0f, 1, 0.0f);
    }
    if (m_pHeadConjureVisual != 0)
    {
        m_pHeadConjureVisual->PlayAnimation("fade", 1.0f, 1, 0.0f);
    }
    if (m_pGroundConjureVisual != 0)
    {
        m_pGroundConjureVisual->PlayAnimation("fade", 1.0f, 1, 0.0f);
    }
    return 1;
}
