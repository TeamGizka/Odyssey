#include "exobase.h"
#include "resgff.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "nwsarea.h"
#include "nwsmodule.h"
#include "nwmessage.h"
#include "nwsplayer.h"
#include "nwcsoundobject.h"
#include "nwssoundobject.h"

static inline void NwssoundobjectTrace2();
static inline void NwssoundobjectTrace1();
#define NWSSOUNDOBJECT_TRACE() NwssoundobjectTrace1()
// PC 0x00570470
CSWSSoundObject::CSWSSoundObject(unsigned long oidObject)
    : CSWSObject(CGameObject::OBJECT_TYPE_SOUND, oidObject, 0), m_lstSounds(0)
{
    NwssoundobjectTrace2();
    m_bActive = 1;
    m_bPositional = 1;
    m_bLooping = 0;
    m_nVolume = 127;
    m_nVolumeVariation = 0;
    m_nTimes = 3;
    m_fPitchVariation = 0.0f;
    m_fFixedVariance = 1.0f;
    m_nHours = 0;
    m_nGeneratedType = 0;
    m_nInterval = 0;
    m_nIntervalVariation = 0;
    m_fMinDistance = 10.0f;
    m_fMaxDistance = 20.0f;
    m_bContinuous = 0;
    m_bRandomOrder = 0;
    m_bRandomPosition = 0;
    m_fRandomRangeX = 0.0f;
    m_fRandomRangeY = 0.0f;
}
// PC 0x0056FBA0
CSWSSoundObject::~CSWSSoundObject()
{
}
// PC 0x0056FC10
void CSWSSoundObject::Save(CResGFF* pRes, CResStruct* pStruct)
{
    NWSSOUNDOBJECT_TRACE();
    CResList lstSounds;
    CResStruct stSound;
    Vector vPosition = m_vPosition;
    float fValue;
    int nSound;

    pRes->WriteFieldCExoString(pStruct, m_sTag, "Tag");
    pRes->WriteFieldBYTE(pStruct, m_bActive, "Active");
    pRes->WriteFieldBYTE(pStruct, m_bPositional, "Positional");
    pRes->WriteFieldBYTE(pStruct, m_bLooping, "Looping");
    pRes->WriteFieldBYTE(pStruct, m_nVolume, "Volume");
    pRes->WriteFieldBYTE(pStruct, m_nVolumeVariation, "VolumeVrtn");
    pRes->WriteFieldBYTE(pStruct, m_nTimes, "Times");
    fValue = m_fPitchVariation;
    pRes->WriteFieldFLOAT(pStruct, fValue, "PitchVariation");
    pRes->WriteFieldDWORD(pStruct, m_nHours, "Hours");
    pRes->WriteFieldDWORD(pStruct, m_nGeneratedType, "GeneratedType");
    pRes->WriteFieldDWORD(pStruct, m_nInterval, "Interval");
    pRes->WriteFieldDWORD(pStruct, m_nIntervalVariation, "IntervalVrtn");
    fValue = m_fMinDistance;
    pRes->WriteFieldFLOAT(pStruct, fValue, "MinDistance");
    fValue = m_fMaxDistance;
    pRes->WriteFieldFLOAT(pStruct, fValue, "MaxDistance");
    pRes->WriteFieldBYTE(pStruct, m_bContinuous, "Continuous");
    pRes->WriteFieldBYTE(pStruct, m_bRandomOrder, "Random");
    fValue = m_fFixedVariance;
    pRes->WriteFieldFLOAT(pStruct, fValue, "FixedVariance");
    pRes->WriteFieldBYTE(pStruct, m_bRandomPosition, "RandomPosition");
    fValue = m_fRandomRangeX;
    pRes->WriteFieldFLOAT(pStruct, fValue, "RandomRangeX");
    fValue = m_fRandomRangeY;
    pRes->WriteFieldFLOAT(pStruct, fValue, "RandomRangeY");
    fValue = vPosition.m_fX;
    pRes->WriteFieldFLOAT(pStruct, fValue, "XPosition");
    fValue = vPosition.m_fY;
    pRes->WriteFieldFLOAT(pStruct, fValue, "YPosition");
    fValue = vPosition.m_fZ;
    pRes->WriteFieldFLOAT(pStruct, fValue, "ZPosition");
    pRes->AddList(&lstSounds, pStruct, "Sounds");
    for (nSound = 0; nSound < m_lstSounds.num; nSound++)
    {
        pRes->AddListElement(&stSound, &lstSounds, 0);
        pRes->WriteFieldCResRef(&stSound, m_lstSounds.element[nSound], "Sound");
    }
}
// PC 0x0056FE90
void CSWSSoundObject::AddToArea(CSWSArea* pArea, int bLoadingGame)
{
    RemoveFromArea();
    if (pArea != NULL)
    {
        SetArea(pArea);
        pArea->AddObjectToArea(m_nId, bLoadingGame);
        g_pAppManager->m_pServerExoApp->GetModule()->AddObjectToLookupTable(
            m_sTag, m_nId);
    }
}
// PC 0x0056FF00
void CSWSSoundObject::PackIntoMessage(CSWSMessage* pMessage)
{
    NWSSOUNDOBJECT_TRACE();
    Vector vPosition = m_vPosition;
    float fValue;
    unsigned short nSounds;
    unsigned short nSound;

    pMessage->WriteOBJECTIDServer(m_nId);
    pMessage->WriteBOOL(m_bActive);
    pMessage->WriteBOOL(m_bPositional);
    pMessage->WriteBOOL(m_bLooping);
    pMessage->WriteBYTE(m_nVolume, 8);
    pMessage->WriteBYTE(m_nVolumeVariation, 8);
    pMessage->WriteBYTE(m_nTimes, 8);
    fValue = m_fPitchVariation;
    pMessage->WriteFLOAT(fValue, 1.0f, 32);
    fValue = m_fFixedVariance;
    pMessage->WriteFLOAT(fValue, 1.0f, 32);
    pMessage->WriteDWORD(m_nHours, 32);
    pMessage->WriteBYTE(m_nGeneratedType, 8);
    pMessage->WriteDWORD(m_nInterval, 32);
    pMessage->WriteDWORD(m_nIntervalVariation, 32);
    fValue = m_fMinDistance;
    pMessage->WriteFLOAT(fValue, 1.0f, 32);
    fValue = m_fMaxDistance;
    pMessage->WriteFLOAT(fValue, 1.0f, 32);
    pMessage->WriteBOOL(m_bContinuous);
    pMessage->WriteBOOL(m_bRandomOrder);
    pMessage->WriteBOOL(m_bRandomPosition);
    fValue = m_fRandomRangeX;
    pMessage->WriteFLOAT(fValue, 1.0f, 32);
    fValue = m_fRandomRangeY;
    pMessage->WriteFLOAT(fValue, 1.0f, 32);
    fValue = vPosition.m_fX;
    pMessage->WriteFLOAT(fValue, 1.0f, 32);
    fValue = vPosition.m_fY;
    pMessage->WriteFLOAT(fValue, 1.0f, 32);
    fValue = vPosition.m_fZ;
    pMessage->WriteFLOAT(fValue, 1.0f, 32);
    nSounds = m_lstSounds.num;
    pMessage->WriteWORD(nSounds, 16);
    for (nSound = 0; nSound < nSounds; nSound++)
    {
        pMessage->WriteCResRef(m_lstSounds.element[nSound], 16);
    }
}
// PC 0x004015A0
void CSWSSoundObject::AIUpdate()
{
}
// PC 0x0056FB90
void CSWSSoundObject::EventHandler(unsigned long nEventId,
                                   unsigned long oidCaller, void* pEventData,
                                   unsigned long nCalendarDay,
                                   unsigned long nTimeOfDay)
{
}
// PC 0x00570110
void CSWSSoundObject::Play()
{
    CSWSMessage* pMessage;
    CExoLinkedList<CSWSClient>* pPlayerList;
    CExoLinkedListNode* pNode;
    CExoLinkedListNode* pNext;
    CSWSPlayer* pPlayer;

    m_bActive = 1;
    pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    pPlayerList = g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pMessage != NULL && pPlayerList != NULL)
    {
        pNode = pPlayerList->GetHeadPos();
        pNext = pNode;
        if (pNode != NULL)
        {
            do
            {
                pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pNode);
                if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
                {
                    if (pPlayer->GetGameObject()->AsSWSCreature() != NULL)
                    {
                        pMessage->SendServerToPlayerSoundObject_Play(pPlayer,
                                                                     m_nId);
                    }
                }
                pPlayerList->GetNext(pNext);
                pNode = pNext;
            } while (pNext != NULL);
        }
    }
}
// PC 0x005701B0
void CSWSSoundObject::Stop()
{
    CSWSMessage* pMessage;
    CExoLinkedList<CSWSClient>* pPlayerList;
    CExoLinkedListNode* pNode;
    CExoLinkedListNode* pNext;
    CSWSPlayer* pPlayer;

    m_bActive = 0;
    pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    pPlayerList = g_pAppManager->m_pServerExoApp->GetPlayerList();
    if (pMessage != NULL && pPlayerList != NULL)
    {
        pNode = pPlayerList->GetHeadPos();
        pNext = pNode;
        if (pNode != NULL)
        {
            do
            {
                pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pNode);
                if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
                {
                    if (pPlayer->GetGameObject()->AsSWSCreature() != NULL)
                    {
                        pMessage->SendServerToPlayerSoundObject_Stop(pPlayer,
                                                                     m_nId);
                    }
                }
                pPlayerList->GetNext(pNext);
                pNode = pNext;
            } while (pNext != NULL);
        }
    }
}
// PC 0x00570250
void CSWSSoundObject::FadeAndStop(unsigned long nFadeTime)
{
    CSWCSoundObject* pSoundObject;

    m_bActive = 0;
    pSoundObject = GetClientSoundObject();
    if (pSoundObject != NULL)
    {
        pSoundObject->FadeAndStop(nFadeTime);
    }
}
// PC 0x00570290
void CSWSSoundObject::ChangePitchVariation(float fVariation)
{
    CSWCSoundObject* pSoundObject;

    if (fVariation != m_fPitchVariation)
    {
        m_fPitchVariation = fVariation;
        pSoundObject = GetClientSoundObject();
        if (pSoundObject != NULL)
        {
            pSoundObject->ChangePitchVariation(fVariation);
        }
    }
}
// PC 0x005702E0
void CSWSSoundObject::ChangeFixedVariance(float fVariance)
{
    CSWCSoundObject* pSoundObject;

    if (fVariance != m_fFixedVariance)
    {
        m_fFixedVariance = fVariance;
        pSoundObject = GetClientSoundObject();
        if (pSoundObject != NULL)
        {
            pSoundObject->ChangeFixedVariance(fVariance);
        }
    }
}
// PC 0x00570330
void CSWSSoundObject::ChangeVolume(int nVolume)
{
    CSWCSoundObject* pSoundObject;
    int nNewVolume;

    nNewVolume = nVolume;
    if (nNewVolume > 127)
    {
        nNewVolume = 127;
    }
    nNewVolume = nNewVolume < 0 ? 0 : nNewVolume;
    if (nNewVolume != m_nVolume)
    {
        m_nVolume = nNewVolume;
        pSoundObject = GetClientSoundObject();
        if (pSoundObject != NULL)
        {
            pSoundObject->ChangeVolume(nNewVolume);
        }
    }
}
// PC 0x00570390
void CSWSSoundObject::ChangePosition(Vector vPosition)
{
    CSWSMessage* pMessage;
    CExoLinkedList<CSWSClient>* pPlayerList;
    CExoLinkedListNode* pNode;
    CExoLinkedListNode* pNext;
    CSWSPlayer* pPlayer;

    if (equal(vPosition, m_vPosition) == 0)
    {
        SetPosition(vPosition, 1, 1, 0);
        pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
        pPlayerList = g_pAppManager->m_pServerExoApp->GetPlayerList();
        if (pMessage != NULL && pPlayerList != NULL)
        {
            pNode = pPlayerList->GetHeadPos();
            pNext = pNode;
            if (pNode != NULL)
            {
                do
                {
                    pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pNode);
                    if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
                    {
                        if (pPlayer->GetGameObject()->AsSWSCreature() !=
                            NULL)
                        {
                            pMessage
                                ->SendServerToPlayerSoundObject_ChangePosition(
                                    pPlayer, m_nId, vPosition);
                        }
                    }
                    pPlayerList->GetNext(pNext);
                    pNode = pNext;
                } while (pNext != NULL);
            }
        }
    }
}
// PC 0x00570580
int CSWSSoundObject::Load(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;
    Vector vPosition;
    CResList lstSounds;
    CResStruct stSound;
    CResRef resSound;
    int nSounds;
    int nSound;
    int bActiveOld;
    int bPositionalOld;
    int bLoopingOld;
    int nVolumeOld;
    int nVolumeVariationOld;
    int nTimesOld;
    float fPitchVariationOld;
    unsigned long nHoursOld;
    unsigned long nIntervalOld;
    unsigned long nIntervalVariationOld;
    float fMinDistanceOld;
    float fMaxDistanceOld;
    int bContinuousOld;
    int bRandomOrderOld;
    float fFixedVarianceOld;
    int bRandomPositionOld;
    float fRandomRangeXOld;
    float fRandomRangeYOld;

    SetTag(pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess, m_sTag));
    bActiveOld = m_bActive;
    m_bActive = pRes->ReadFieldBYTE(pStruct, "Active", bSuccess, bActiveOld);
    bPositionalOld = m_bPositional;
    m_bPositional =
        pRes->ReadFieldBYTE(pStruct, "Positional", bSuccess, bPositionalOld);
    bLoopingOld = m_bLooping;
    m_bLooping = pRes->ReadFieldBYTE(pStruct, "Looping", bSuccess, bLoopingOld);
    nVolumeOld = m_nVolume;
    m_nVolume = pRes->ReadFieldBYTE(pStruct, "Volume", bSuccess, nVolumeOld);
    nVolumeVariationOld = m_nVolumeVariation;
    m_nVolumeVariation = pRes->ReadFieldBYTE(pStruct, "VolumeVrtn", bSuccess,
                                             nVolumeVariationOld);
    nTimesOld = m_nTimes;
    m_nTimes = pRes->ReadFieldBYTE(pStruct, "Times", bSuccess, nTimesOld);
    fPitchVariationOld = m_fPitchVariation;
    fPitchVariationOld = pRes->ReadFieldFLOAT(pStruct, "PitchVariation",
                                              bSuccess, fPitchVariationOld);
    m_fPitchVariation = fPitchVariationOld;
    nHoursOld = m_nHours;
    m_nHours = pRes->ReadFieldDWORD(pStruct, "Hours", bSuccess, nHoursOld);
    m_nGeneratedType = pRes->ReadFieldDWORD(pStruct, "GeneratedType", bSuccess,
                                            m_nGeneratedType);
    nIntervalOld = m_nInterval;
    m_nInterval =
        pRes->ReadFieldDWORD(pStruct, "Interval", bSuccess, nIntervalOld);
    nIntervalVariationOld = m_nIntervalVariation;
    m_nIntervalVariation = pRes->ReadFieldDWORD(
        pStruct, "IntervalVrtn", bSuccess, nIntervalVariationOld);
    fMinDistanceOld = m_fMinDistance;
    fMinDistanceOld =
        pRes->ReadFieldFLOAT(pStruct, "MinDistance", bSuccess, fMinDistanceOld);
    m_fMinDistance = fMinDistanceOld;
    fMaxDistanceOld = m_fMaxDistance;
    fMaxDistanceOld =
        pRes->ReadFieldFLOAT(pStruct, "MaxDistance", bSuccess, fMaxDistanceOld);
    m_fMaxDistance = fMaxDistanceOld;
    bContinuousOld = m_bContinuous;
    m_bContinuous =
        pRes->ReadFieldBYTE(pStruct, "Continuous", bSuccess, bContinuousOld);
    bRandomOrderOld = m_bRandomOrder;
    m_bRandomOrder =
        pRes->ReadFieldBYTE(pStruct, "Random", bSuccess, bRandomOrderOld);
    fFixedVarianceOld = m_fFixedVariance;
    fFixedVarianceOld = pRes->ReadFieldFLOAT(pStruct, "FixedVariance", bSuccess,
                                             fFixedVarianceOld);
    m_fFixedVariance = fFixedVarianceOld;
    bRandomPositionOld = m_bRandomPosition;
    m_bRandomPosition = pRes->ReadFieldBYTE(pStruct, "RandomPosition", bSuccess,
                                            bRandomPositionOld);
    fRandomRangeXOld = m_fRandomRangeX;
    fRandomRangeXOld = pRes->ReadFieldFLOAT(pStruct, "RandomRangeX", bSuccess,
                                            fRandomRangeXOld);
    m_fRandomRangeX = fRandomRangeXOld;
    fRandomRangeYOld = m_fRandomRangeY;
    fRandomRangeYOld = pRes->ReadFieldFLOAT(pStruct, "RandomRangeY", bSuccess,
                                            fRandomRangeYOld);
    m_fRandomRangeY = fRandomRangeYOld;
    vPosition.m_fX = pRes->ReadFieldFLOAT(pStruct, "XPosition", bSuccess, 0.0f);
    vPosition.m_fY = pRes->ReadFieldFLOAT(pStruct, "YPosition", bSuccess, 0.0f);
    vPosition.m_fZ = pRes->ReadFieldFLOAT(pStruct, "ZPosition", bSuccess, 0.0f);
    SetPosition(vPosition, 1, 1, 0);
    if (pRes->GetList(&lstSounds, pStruct, "Sounds") != 0)
    {
        nSounds = pRes->GetListCount(&lstSounds);
        for (nSound = 0; nSound < nSounds; nSound++)
        {
            if (pRes->GetListElement(&stSound, &lstSounds, nSound) != 0)
            {
                CResRef cDefault("");

                resSound = pRes->ReadFieldCResRef(&stSound, "Sound", bSuccess,
                                                  cDefault);
                if (bSuccess != 0)
                {
                    m_lstSounds.Add(resSound);
                }
            }
        }
    }
    return bSuccess;
}
// PC 0x00570A20
int CSWSSoundObject::LoadFromTemplate(const CResRef& resTemplate)
{
    CResGFF* pTemplate;
    CResStruct stTop;
    int bSuccess;

    pTemplate = new CResGFF(2035, "UTS ", (CResRef&)resTemplate);
    if (pTemplate->m_bLoaded == 0)
    {
        delete pTemplate;
        return 0;
    }
    pTemplate->GetTopLevelStruct(&stTop);
    bSuccess = Load(pTemplate, &stTop);
    delete pTemplate;
    return bSuccess;
}

void CSWSSoundObject::RemoveFromArea()
{
    if (GetArea() != NULL)
    {
        GetArea()->RemoveObjectFromArea(m_nId);
    }
    SetArea(NULL);
}

CSWCSoundObject* CSWSSoundObject::GetClientSoundObject()
{
    unsigned long oidSelf = m_nId;
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    unsigned long oidClient = pApp->ServerToClientObjectId(oidSelf);
    return g_pAppManager->m_pClientExoApp->GetSoundObjectByGameObjectID(oidClient);
}

static inline void NwssoundobjectTrace2()
{
    NwssoundobjectTrace1();
}

static inline void NwssoundobjectTrace1()
{
}
